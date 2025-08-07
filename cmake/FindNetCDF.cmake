# * Find NetCDF Find the native NetCDF includes and library
#
# NETCDF_INCLUDE_DIR  - user modifiable choice of where netcdf headers are
# NETCDF_LIBRARY      - user modifiable choice of where netcdf libraries are
#
# Your package can require certain interfaces to be FOUND by setting these
#
# NETCDF_CXX         - require the C++ interface and link the C++ library
# NETCDF_F77         - require the F77 interface and link the fortran library
# NETCDF_F90         - require the F90 interface and link the fortran library
#
# Or equivalently by calling FindNetCDF with a COMPONENTS argument containing
# one or more of "CXX;F77;F90".
#
# When interfaces are requested the user has access to interface specific hints:
#
# NETCDF_${LANG}_INCLUDE_DIR - where to search for interface header files
# NETCDF_${LANG}_LIBRARY     - where to search for interface libraries
#
# This module returns these variables for the rest of the project to use.
#
# NETCDF_FOUND          - True if NetCDF found including required interfaces
# (see below) NETCDF_VERSION        - NetCDF version NETCDF_LIBRARIES      - All
# netcdf related libraries. NETCDF_INCLUDE_DIRS   - All directories to include.
# NETCDF_HAS_INTERFACES - Whether requested interfaces were found or not.
# NETCDF_${LANG}_INCLUDE_DIRS - Interface-specific include dirs
# NETCDF_${LANG}_LIBRARIES    - Interface-specific libraries
#
# Normal usage would be: set (NETCDF_F90 "YES") find_package (NetCDF REQUIRED)
# target_link_libraries (uses_everthing ${NETCDF_LIBRARIES})
# target_link_libraries (only_uses_f90 ${NETCDF_F90_LIBRARIES})

# search starting from user editable cache var
if(NETCDF_INCLUDE_DIR AND NETCDF_LIBRARY)
  # Already in cache, be silent
  set(NETCDF_FIND_QUIETLY TRUE)
endif()

set(USE_DEFAULT_PATHS "NO_DEFAULT_PATH")
if(NETCDF_USE_DEFAULT_PATHS)
  set(USE_DEFAULT_PATHS "")
endif()

find_path(NETCDF_INCLUDE_DIR netcdf.h PATHS "${NETCDF_DIR}/include")
mark_as_advanced(NETCDF_INCLUDE_DIR)
set(NETCDF_C_INCLUDE_DIRS ${NETCDF_INCLUDE_DIR})

find_library(
  NETCDF_LIBRARY
  NAMES netcdf
  PATHS "${NETCDF_DIR}/lib"
  HINTS "${NETCDF_INCLUDE_DIR}/../lib")
mark_as_advanced(NETCDF_LIBRARY)

set(NETCDF_C_LIBRARIES ${NETCDF_LIBRARY})

# start finding requested language components
set(NETCDF_LIBS "")
set(NETCDF_INCLUDES "${NETCDF_INCLUDE_DIR}")

get_filename_component(NETCDF_LIB_DIRS "${NETCDF_LIBRARY}" PATH)
# Will be set to NO if we're missing any interfaces
set(NETCDF_HAS_INTERFACES "YES")

# ------------------------------------------------------------------------------
# NETCDF_CHECK_INTERFACE
#
# Checks for the presence of a NetCDF interface for a given language.
#
# Arguments: lang    - The language identifier (e.g., C, CXX, Fortran). header -
# The header file name to search for. libs    - The library name(s) to search
# for.
#
# This macro attempts to locate the specified NetCDF header and library files
# for the given language. If found, it sets the corresponding include and
# library variables, and appends them to the global lists NETCDF_LIBS and
# NETCDF_INCLUDES. If not found, it sets NETCDF_HAS_INTERFACES to "NO" and
# prints a status message.
# ------------------------------------------------------------------------------
macro(NETCDF_CHECK_INTERFACE lang header libs)
  if(NETCDF_${lang})
    # search starting from user modifiable cache var
    find_path(
      NETCDF_${lang}_INCLUDE_DIR
      NAMES ${header}
      HINTS "${NETCDF_INCLUDE_DIR}" "${NETCDF_${lang}_ROOT}/include"
            ${USE_DEFAULT_PATHS})

    find_library(
      NETCDF_${lang}_LIBRARY
      NAMES ${libs}
      HINTS "${NETCDF_LIB_DIRS}" "${NETCDF_${lang}_ROOT}/lib"
            ${USE_DEFAULT_PATHS})

    mark_as_advanced(NETCDF_${lang}_INCLUDE_DIR NETCDF_${lang}_LIBRARY)

    # export to internal varS that rest of project can use directly
    set(NETCDF_${lang}_LIBRARIES ${NETCDF_${lang}_LIBRARY})
    set(NETCDF_${lang}_INCLUDE_DIRS ${NETCDF_${lang}_INCLUDE_DIR})

    if(NETCDF_${lang}_INCLUDE_DIR AND NETCDF_${lang}_LIBRARY)
      list(APPEND NETCDF_LIBS ${NETCDF_${lang}_LIBRARY})
      list(APPEND NETCDF_INCLUDES ${NETCDF_${lang}_INCLUDE_DIR})
    else()
      set(NETCDF_HAS_INTERFACES "NO")
      message(STATUS "Failed to find NetCDF interface for ${lang}")
    endif()
  endif()
endmacro()

list(FIND NetCDF_FIND_COMPONENTS "CXX" _nextcomp)
if(_nextcomp GREATER -1)
  set(NETCDF_CXX 1)
endif()
list(FIND NetCDF_FIND_COMPONENTS "F77" _nextcomp)
if(_nextcomp GREATER -1)
  set(NETCDF_F77 1)
endif()
list(FIND NetCDF_FIND_COMPONENTS "F90" _nextcomp)
if(_nextcomp GREATER -1)
  set(NETCDF_F90 1)
endif()
netcdf_check_interface(CXX netcdfcpp.h netcdf_c++)
netcdf_check_interface(F77 netcdf.inc netcdff)
netcdf_check_interface(F90 netcdf.mod netcdff)

# export accumulated results to internal varS that rest of project can depend on
list(APPEND NETCDF_LIBS "${NETCDF_C_LIBRARIES}")
set(NETCDF_LIBRARIES ${NETCDF_LIBS})
set(NETCDF_INCLUDE_DIRS ${NETCDF_INCLUDES})

if(EXISTS "${NETCDF_INCLUDE_DIRS}/netcdf_meta.h")
  file(STRINGS "${NETCDF_INCLUDE_DIRS}/netcdf_meta.h" _version_contents
       REGEX "define NC_VERSION")
  string(REGEX REPLACE ".*([0-9]\\.[0-9]\\.[0-9]).*" "\\1" NETCDF_VERSION
                       ${_version_contents})
else()
  find_program(_nc_config "nc-config")
  if(_nc_config)
    execute_process(COMMAND ${_nc_config} "--version"
                    OUTPUT_VARIABLE _version_contents)
    string(REGEX REPLACE ".*([0-9]\\.[0-9]\\.[0-9]).*" "\\1" NETCDF_VERSION
                         ${_version_contents})
  endif()
  unset(_nc_config)
endif()
unset(_version_contents)

# handle the QUIETLY and REQUIRED arguments and set NETCDF_FOUND to TRUE if all
# listed variables are TRUE
include(FindPackageHandleStandardArgs)
find_package_handle_standard_args(
  NetCDF
  FOUND_VAR NETCDF_FOUND
  REQUIRED_VARS NETCDF_LIBRARIES NETCDF_INCLUDE_DIRS NETCDF_HAS_INTERFACES
  VERSION_VAR NETCDF_VERSION)
