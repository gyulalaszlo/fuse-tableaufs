# - Try to find ZMQ
# Once done this will define
#
#  ZMQPP_FOUND - system has ZMQ
#  ZMQPP_INCLUDE_DIRS - the ZMQ include directory
#  ZMQPP_LIBRARIES - Link these to use ZMQ
#  ZMQPP_DEFINITIONS - Compiler switches required for using ZMQ
#
#  Copyright (c) 2011 Lee Hambley <lee.hambley@gmail.com>
#
#  Redistribution and use is allowed according to the terms of the New
#  BSD license.
#  For details see the accompanying COPYING-CMAKE-SCRIPTS file.
#

if (ZMQPP_LIBRARIES AND ZMQPP_INCLUDE_DIRS)
  # in cache already
  set(ZMQPP_FOUND TRUE)
else (ZMQPP_LIBRARIES AND ZMQPP_INCLUDE_DIRS)

  find_path(ZMQPP_INCLUDE_DIR
    NAMES
      zmqpp/zmqpp.hpp
    PATHS
      /usr/include
      /usr/local/include
      /opt/local/include
      /sw/include
  )

  find_library(ZMQPP_LIBRARY
    NAMES
      zmqpp
    PATHS
      /usr/lib
      /usr/local/lib
      /opt/local/lib
      /sw/lib
  )

  set(ZMQPP_INCLUDE_DIRS
    ${ZMQPP_INCLUDE_DIR}
  )

  if (ZMQPP_LIBRARY)
    set(ZMQPP_LIBRARIES
        ${ZMQPP_LIBRARIES}
        ${ZMQPP_LIBRARY}
    )
  endif (ZMQPP_LIBRARY)

  include(FindPackageHandleStandardArgs)
  find_package_handle_standard_args(ZMQPP DEFAULT_MSG ZMQPP_LIBRARIES ZMQPP_INCLUDE_DIRS)

  # show the ZMQPP_INCLUDE_DIRS and ZMQPP_LIBRARIES variables only in the advanced view
  mark_as_advanced(ZMQPP_INCLUDE_DIRS ZMQPP_LIBRARIES)

endif (ZMQPP_LIBRARIES AND ZMQPP_INCLUDE_DIRS)

