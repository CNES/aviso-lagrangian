# This file is part of lagrangian library.
#
# lagrangian is free software: you can redistribute it and/or modify it under
# the terms of GNU Lesser General Public License as published by the Free
# Software Foundation, either version 3 of the License, or (at your option) any
# later version.
#
# lagrangian is distributed in the hope that it will be useful, but WITHOUT ANY
# WARRANTY; without even the implied warranty of MERCHANTABILITY or FITNESS FOR
# A PARTICULAR PURPOSE.  See the GNU General Public License for more details.
#
# You should have received a copy of GNU Lesser General Public License along
# with lagrangian. If not, see <http://www.gnu.org/licenses/>.
cmake_minimum_required(VERSION 3.12)

if(POLICY CMP0167)
  cmake_policy(SET CMP0167 NEW)
endif()

include(CheckFunctionExists)
include(CheckCXXSourceRuns)

if("${CMAKE_SOURCE_DIR}" STREQUAL "${CMAKE_CURRENT_BINARY_DIR}")
  message(FATAL_ERROR "The build directory must be different from the \
        root directory of this software.")
endif()

cmake_policy(SET CMP0048 NEW)
project(lagrangian LANGUAGES CXX)

if(POLICY CMP0063)
  cmake_policy(SET CMP0063 NEW)
endif()

if(POLICY CMP0074)
  cmake_policy(SET CMP0074 NEW)
endif()

if(POLICY CMP0077)
  cmake_policy(SET CMP0077 NEW)
endif()

# CMake module search path
set(CMAKE_MODULE_PATH
    "${CMAKE_CURRENT_SOURCE_DIR}/cmake"
    "${CMAKE_CURRENT_SOURCE_DIR}/third_party/pybind11/tools"
    "${CMAKE_MODULE_PATH}")

# By default, build type is set to release, with debugging information.
if(NOT CMAKE_BUILD_TYPE)
  set(CMAKE_BUILD_TYPE RELWITHDEBINFO)
endif()
message("-- Build type: ${CMAKE_BUILD_TYPE}")

# The library must be built using C++17 compiler.
set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_EXTENSIONS OFF)
set(CMAKE_MACOSX_RPATH 1)

include(CheckCXXCompilerFlag)
if(NOT WIN32)
  check_cxx_compiler_flag("-std=c++17" HAS_CPP17_FLAG)
else()
  check_cxx_compiler_flag("/std:c++17" HAS_CPP17_FLAG)
  add_definitions(-D_USE_MATH_DEFINES)
  add_definitions(-DBOOST_ALL_NO_LIB)
endif()
if(NOT HAS_CPP17_FLAG)
  message(FATAL_ERROR "Unsupported compiler -- requires C++17 support!")
endif()

#
# ------------------------------------------------------------------------------
# Macro: CHECK_cxx_flags_RUN
#
# Checks if the specified C++ compiler and linker flags are supported by
# attempting to compile and run a simple test program with those flags.
#
# Arguments: result_        - Variable to store the result (TRUE if the test
# program runs successfully, FALSE otherwise). cxx_flags_     - C++ compiler
# flags to test. linker_flags_  - Linker flags to test.
#
# Usage: check_cxx_flags_run(<result_var> <cxx_flags> <linker_flags>)
#
# The macro temporarily sets the required flags and libraries, runs the test,
# and then restores the previous state.
# ------------------------------------------------------------------------------
macro(CHECK_cxx_flags_RUN result_ cxx_flags_ linker_flags_)
  set(CMAKE_REQUIRED_FLAGS ${cxx_flags_})
  set(CMAKE_REQUIRED_LIBRARIES ${linker_flags_})
  set(CMAKE_REQUIRED_QUIET FALSE)
  check_cxx_source_runs("int main(int argc, char **argv) { return 0; }"
                        ${result_})
  set(CMAKE_REQUIRED_FLAGS "")
  set(CMAKE_REQUIRED_LIBRARIES "")
  unset(result_)
endmacro()

# Always use libc++ on Clang
if(CMAKE_CXX_COMPILER_ID MATCHES "Clang")
  check_cxx_flags_run(HAS_LIBCPP "-stdlib=libc++" "-stdlib=libc++")
  if(HAS_LIBCPP)
    string(APPEND CMAKE_cxx_flags " -stdlib=libc++")
    string(APPEND CMAKE_EXE_linker_flags " -stdlib=libc++")
    string(APPEND CMAKE_SHARED_linker_flags " -stdlib=libc++")
    check_cxx_flags_run(HAS_LIBCPPABI "-stdlib=libc++"
                        "-stdlib=libc++ -lc++abi")
    if(HAS_LIBCPPABI)
      string(APPEND CMAKE_EXE_linker_flags " -lc++abi")
      string(APPEND CMAKE_SHARED_linker_flags " -lc++abi")
    endif()
  endif()
  check_cxx_flags_run(HAS_SIZED_DEALLOCATION "-fsized-deallocation" "")
  if(HAS_SIZED_DEALLOCATION)
    string(APPEND CMAKE_cxx_flags " -fsized-deallocation")
  endif()
endif()

if(NOT WIN32)
  if(NOT CMAKE_cxx_flags MATCHES "-Wall$")
    string(APPEND CMAKE_cxx_flags " -Wall")
  endif()
  if(NOT CMAKE_CXX_COMPILER MATCHES "icpc$" AND NOT CMAKE_cxx_flags MATCHES
                                                "-Wpedantic$")
    string(APPEND CMAKE_cxx_flags " -Wpedantic")
  endif()
endif()

if(MSVC)
  # Disable warnings about using deprecated std::equal_to<>::result_type
  add_definitions(-D_SILENCE_CXX17_ADAPTOR_TYPEDEFS_DEPRECATION_WARNING)
endif()

check_function_exists(pow POW_FUNCTION_EXISTS)
if(NOT POW_FUNCTION_EXISTS)
  unset(POW_FUNCTION_EXISTS CACHE)
  list(APPEND CMAKE_REQUIRED_LIBRARIES m)
  check_function_exists(pow POW_FUNCTION_EXISTS)
  if(POW_FUNCTION_EXISTS)
    set(MATH_LIBRARY
        m
        CACHE STRING "" FORCE)
  else()
    message(FATAL_ERROR "Failed making the pow() function available")
  endif()
endif()

# Python
find_package(
  Python3 3.7
  COMPONENTS Interpreter Development
  REQUIRED)
execute_process(
  COMMAND
    ${Python3_EXECUTABLE} -c [=[import os
import sysconfig
import sys
sys.stdout.write(os.path.dirname(sysconfig.get_config_h_filename()))
]=]
  OUTPUT_VARIABLE PYTHON_INCLUDE_DIR)

# Boost
find_package(Boost 1.63 REQUIRED COMPONENTS date_time)
include_directories(${Boost_INCLUDE_DIRS})

# Udunits2
find_package(UDUNITS2 REQUIRED)
include_directories(${UDUNITS2_INCLUDES})

# NetCDF
find_package(NetCDF 4.1.1 REQUIRED)
include_directories(${NETCDF_INCLUDE_DIR})

# Pybind11
add_subdirectory(${CMAKE_CURRENT_SOURCE_DIR}/third_party/pybind11)

set(CMAKE_VISIBILITY_INLINES_HIDDEN ON)
set(CMAKE_CXX_VISIBILITY_PRESET hidden)
set(CMAKE_POSITION_INDEPENDENT_CODE ON)

# NETCDF-CXX4
include_directories(${CMAKE_CURRENT_SOURCE_DIR}/third_party/netcdf-cxx4/cxx4)
file(GLOB_RECURSE NCXX4_SOURCES
     "${CMAKE_CURRENT_SOURCE_DIR}/third_party/netcdf-cxx4/cxx4/nc*.cpp")
add_library(ncxx4 STATIC ${NCXX4_SOURCES})

# Internal library
include_directories(${CMAKE_CURRENT_SOURCE_DIR}/src/include)
file(GLOB_RECURSE LIB_SOURCES "${CMAKE_CURRENT_SOURCE_DIR}/src/lib/*.cpp")
add_library(lagrangian STATIC ${LIB_SOURCES})

# Python wrapper
file(GLOB_RECURSE WRAPPER_SOURCES "${CMAKE_CURRENT_SOURCE_DIR}/src/core/*.cpp")
pybind11_add_module(core ${WRAPPER_SOURCES})
target_link_libraries(
  core PRIVATE lagrangian ncxx4 Boost::date_time ${NETCDF_LIBRARIES}
               ${LIBXML2_LIBRARIES} ${UDUNITS2_LIBRARIES})
