if(UDUNITS2_INCLUDES)
  # Already in cache, be silent
  set(UDUNITS2_FIND_QUIETLY TRUE)
endif()

find_path(
  UDUNITS2_INCLUDES udunits2.h
  HINTS "${UDUNITS2_ROOT}/include" "$ENV{UDUNITS2_ROOT}/include"
  PATH_SUFFIXES "udunits2"
  DOC "Path to udunits2.h")

if(${UDUNITS2_INCLUDES} MATCHES "udunits2/?$")
  string(REGEX REPLACE "/include/udunits2/?$" "/lib" UDUNITS2_LIB_HINT
                       ${UDUNITS2_INCLUDES})
else()
  string(REGEX REPLACE "/include/?$" "/lib" UDUNITS2_LIB_HINT
                       ${UDUNITS2_INCLUDES})
endif()

find_library(
  UDUNITS2_LIBRARIES
  NAMES udunits2
  HINTS ${UDUNITS2_LIB_HINT})

# handle the QUIETLY and REQUIRED arguments and set UDUNITS2_FOUND to TRUE if
# all listed variables are TRUE
include(FindPackageHandleStandardArgs)
find_package_handle_standard_args(UDUNITS2 DEFAULT_MSG UDUNITS2_LIBRARIES
                                  UDUNITS2_INCLUDES)

mark_as_advanced(UDUNITS2_LIBRARIES UDUNITS2_INCLUDES)
