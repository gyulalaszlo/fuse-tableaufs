# Copyright (c) 2012-2015, Tamas Foldi, Starschema
#
# Redistribution and use in source and binary forms, with or without
# modification, are permitted provided that the following conditions
# are met:
#
# 1. Redistributions of source code must retain the above copyright
#    notice, this list of conditions and the following disclaimer.
# 2. Redistributions in binary form must reproduce the above copyright
#    notice, this list of conditions and the following disclaimer in the
#    documentation and/or other materials provided with the distribution.
#
# THIS SOFTWARE IS PROVIDED BY THE AUTHOR ``AS IS'' AND ANY EXPRESS OR
# IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE IMPLIED WARRANTIES
# OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE ARE DISCLAIMED.
# IN NO EVENT SHALL THE AUTHOR BE LIABLE FOR ANY DIRECT, INDIRECT,
# INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING, BUT
# NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES; LOSS OF USE,
# DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON ANY
# THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT
# (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE OF
# THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.


# FUSE_CFLAGS_OTHER may contain more then one option so we replace
# the bad joining semicolons
#string(REPLACE ";" " " FUSE_CFLAGS_OTHER_SPLIT ${FUSE_CFLAGS_OTHER})

if (APPLE)
  # On OSX:
  # FUSE_CFLAGS_OTHER=-D_FILE_OFFSET_BITS=64-D_DARWIN_USE_64_BIT_INODE
  # so we need to split the flags by the dashes
  string(REPLACE "-" " -" FUSE_CFLAGS_OTHER_SPLIT ${FUSE_CFLAGS_OTHER})
else()
  # TODO: check on Linux
  string(REPLACE ";" " " FUSE_CFLAGS_OTHER_SPLIT ${FUSE_CFLAGS_OTHER})
endif()

message(FUSE_CFLAGS_OTHER=${FUSE_CFLAGS_OTHER})
message(FUSE_CFLAGS=${FUSE_CFLAGS})
message(">> Setting FUSE_CFLAGS_OTHER to ${FUSE_CFLAGS_OTHER_SPLIT}")

set( TFS_COMPILE_FLAGS  "${FUSE_CFLAGS_OTHER_SPLIT} ${POSTGRES_CFLAGS}")

SET (CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} ${TFS_COMPILE_FLAGS} -Wall")

message("TFS_COMPILE_FLAGS=${TFS_COMPILE_FLAGS}")

add_definitions(-DTFS_TEST_SERVER="${TFS_TEST_SERVER_ADDRESS}")
add_definitions(-DTFS_TEST_FILE_DIR="${CMAKE_SOURCE_DIR}/test-data/files/")
add_definitions(-DTFS_COMPILED_WITH="${CMAKE_CXX_FLAGS_INIT} ${CMAKE_CXX_FLAGS}")
if (CXX_HAS_CPP14)
  add_definitions(-DHAS_CPP14=1)
endif()


add_library( tableaufs-pgconn

  utils/logger.cc
  utils/debug_utilities.cc

  connection/pgconn.cc
  connection/pg_helpers.cc
  connection/pg_connection.cc

  connection/parse_path.cc

  connection/tfs_postgres_config.cc
  connection/tfs_postgres.cc

  capnp/tfs.capnp.c++
  connection/tfs_capnp.cc

  )

target_link_libraries( tableaufs-pgconn
  ${POSTGRES_LIBRARY}
  ${ZMQ_LIBRARY}
  ${ZMQPP_LIBRARY}
  ${CAPNPROTO_LIBRARIES}
  stdc++
  )

# Tests
# -----

function(add_tfs_test test_name files)
  add_executable( ${test_name} tests/test_runner.cc ${files})
  target_link_libraries( ${test_name} tableaufs-pgconn gtest)
  add_test( ${test_name} ${test_name} )
endfunction()

add_tfs_test( test_readdir tests/test_tfs_postgres_readdir.cc )
add_tfs_test( test_stat tests//test_tfs_postgres_stat.cc )
add_tfs_test( test_parse_path tests/test_parse_path.cc )
add_tfs_test( test_open_file tests/test_open_file.cc )
add_tfs_test( test_read_file tests/test_read_file.cc )

add_tfs_test( test_capnp tests/test_capnp.cc )

# Add the main executable
add_executable( fuse-tfs
  app/fuse_tfs.cc

  )

# Set the compile flags on a pre-target basis
set_target_properties(fuse-tfs PROPERTIES
  COMPILE_FLAGS ${TFS_COMPILE_FLAGS}
  )

# Link it with all the good stuff
target_link_libraries( fuse-tfs
  ${POSTGRES_LIBRARY}
  ${FUSE_LDFLAGS}
  tableaufs-pgconn
  )


# Add the main executable
add_executable( tableaufs
  tableaufs.c
  workgroup.c
  )

# Set the compile flags on a pre-target basis
set_target_properties(tableaufs PROPERTIES
  COMPILE_FLAGS ${TFS_COMPILE_FLAGS}
  )

# Link it with all the good stuff
target_link_libraries( tableaufs
  ${POSTGRES_LIBRARY}
  ${FUSE_LDFLAGS}
  tableaufs-pgconn
  )

install(
  TARGETS tableaufs
  RUNTIME
  DESTINATION bin )





# Symlink the tableaufs executable as mount_tableaufs for osx
if(APPLE)
  # The installed executable location
  set(TFS_EXEC_INSTALL_PATH "${CMAKE_INSTALL_PREFIX}/bin/tableaufs")
  set(TFS_MOUNT_EXEC_INSTALL_PATH "/sbin/mount_tableaufs")
  install(CODE "
  EXECUTE_PROCESS(
    COMMAND ln -sf ${TFS_EXEC_INSTALL_PATH} ${TFS_MOUNT_EXEC_INSTALL_PATH}
    )
  ")
endif()

