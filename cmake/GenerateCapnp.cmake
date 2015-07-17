
# Generate CAPNP files from a source
function(generate_capnp capnp_source)

  # Build the CAPNP sources
  #capnp compile -oc++ src/capnp/tfs.capnp --verbose
  set(TFS_CAPNP_SOURCE ${CMAKE_CURRENT_SOURCE_DIR}/${capnp_source})
  set(TFS_CAPNP_FILES ${TFS_CAPNP_SOURCE}.c++ ${TFS_CAPNP_SOURCE}.h)
  add_custom_command(OUTPUT  ${TFS_CAPNP_FILES}
    COMMAND capnp compile -oc++ ${TFS_CAPNP_SOURCE}
    WORKING_DIRECTORY ${CMAKE_CURRENT_SOURCE_DIR}
    COMMENT "Generating CAPNP files for ${TFS_CAPNP_SOURCE} -> ${TFS_CAPNP_FILES}"
    VERBOSE
    )

endfunction()

