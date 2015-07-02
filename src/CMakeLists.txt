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
string(REPLACE ";" " " FUSE_CFLAGS_OTHER_SPLIT ${FUSE_CFLAGS_OTHER})
message(">> Setting FUSE_CFLAGS_OTHER to ${FUSE_CFLAGS_OTHER_SPLIT}")
set( TFS_COMPILE_FLAGS  "${FUSE_CFLAGS_OTHER_SPLIT} ${POSTGRES_CFLAGS}")

add_library( tableaufs-pgconn
  connection/pgconn.cc
  connection/pg_helpers.cc
  connection/pg_connection.cc

  connection/parse_path.cc

  connection/tfs_postgres_config.cc
  connection/tfs_postgres.cc

  )

target_link_libraries( tableaufs-pgconn
  ${POSTGRES_LIBRARY}
  ${ZMQ_LIBRARY}
  ${ZMQPP_LIBRARY}
  stdc++
  )

# Tests
# -----

# Postgres connection test
add_executable( pgconn-test tests/test_runner.cc tests/pgconn_interface_tests.cc)
target_link_libraries( pgconn-test tableaufs-pgconn gtest)
add_test( pgconn pgconn-test )

# Path parser test
add_executable( parse-path-test tests/test_runner.cc tests/parse_path_test.cc)
target_link_libraries( parse-path-test tableaufs-pgconn gtest)
add_test( parse-path parse-path-test )


# Add the main executable
add_executable( tableaufs
  tableaufs.c
  workgroup.c
  )

# Set the compile flags on a pre-target basis
set_target_properties(tableaufs PROPERTIES
  COMPILE_FLAGS ${TFS_COMPILE_FLAGS}
  #CXX_STANDARD_REQUIRED On
  #CXX_STANDARD 11
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
