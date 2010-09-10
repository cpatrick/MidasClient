if(BUILD_MIDASCPP_TESTING)
  set(midascli_command "${MIDASCPP_BINARY_DIR}/bin/MIDAScli")
  set(sqlite3_command "${MIDASCPP_BINARY_DIR}/bin/sqlite3")
  set(dashboard_midas_server_url http://localhost/midas/api/rest CACHE STRING "The URL of the REST API for MIDAS testing")
  set(dashboard_midas_api_key NO_KEY CACHE STRING "The MIDAS API key for test dashboard")
  set(dashboard_midas_app_name test CACHE STRING "The MIDAS API name for test dashboard")
  set(dashboard_midas_email admin@localhost CACHE STRING "The MIDAS login email for test dashboard")
  site_name(dashboard_top_level_community_name)
  
  set(sqlite_db "${MIDASCPP_BINARY_DIR}/midas.db")
  set(new_db "${MIDASCPP_BINARY_DIR}/midas_new.db")
  
    # Setup a mock tree we can add
  configure_file(
    "${MIDASCPP_SOURCE_DIR}/README.txt"
    "${MIDASCPP_BINARY_DIR}/Testing/${dashboard_top_level_community_name}/Subcomm/Collection/Item/bitstream.txt"
    COPYONLY
  )

  add_test(testCliNoArgs ${midascli_command})
  set_tests_properties(testCliNoArgs PROPERTIES WILL_FAIL ON)

  # Restore database to a fresh state
  add_test(cleanDatabase ${midascli_command} --database ${sqlite_db} clean)
  add_test(deleteAuthProfiles ${sqlite3_command} ${sqlite_db} "DELETE FROM auth_profile;")

  # "clone" command tests
  add_test(testClone ${midascli_command} --database ${sqlite_db} clone ${dashboard_midas_server_url})
  set_tests_properties(testClone PROPERTIES DEPENDS "cleanDatabase;deleteAuthProfiles")

  # "create_profile" command tests
  add_test(testCreateProfile ${midascli_command} --database ${sqlite_db} create_profile --name test --email ${dashboard_midas_email} --app-name ${dashboard_midas_app_name} --api-key ${dashboard_midas_api_key})
  set_tests_properties(testCreateProfile PROPERTIES DEPENDS testClone)

  add_test(testPushNoResources ${midascli_command} --database ${sqlite_db} --profile test push)
  set_tests_properties(testPushNoResources PROPERTIES DEPENDS testCreateProfile)

  # Restore database to a fresh state again
  add_test(cleanDatabase2 ${midascli_command} --database ${sqlite_db} clean)
  set_tests_properties(cleanDatabase2 PROPERTIES DEPENDS testPushNoResources)

  # "add" command tests
  add_test(testAddTopLevelCommunity ${midascli_command} --database ${sqlite_db} add -C "${dashboard_top_level_community_name}")
  set_tests_properties(testAddTopLevelCommunity PROPERTIES DEPENDS cleanDatabase2)
  add_test(testAddSubCommunity ${midascli_command} --database ${sqlite_db} add -C "${dashboard_top_level_community_name}/Subcomm")
  set_tests_properties(testAddSubCommunity PROPERTIES DEPENDS testAddTopLevelCommunity)
  add_test(testAddCollection ${midascli_command} --database ${sqlite_db} add -c "${dashboard_top_level_community_name}/Subcomm/Collection")
  set_tests_properties(testAddCollection PROPERTIES DEPENDS testAddSubCommunity)
  add_test(testAddItem ${midascli_command} --database ${sqlite_db} add -i "${dashboard_top_level_community_name}/Subcomm/Collection/Item")
  set_tests_properties(testAddItem PROPERTIES DEPENDS testAddCollection)
  add_test(testAddBitstream ${midascli_command} --database ${sqlite_db} add -b "${dashboard_top_level_community_name}/Subcomm/Collection/Item/bitstream.txt")
  set_tests_properties(testAddBitstream PROPERTIES DEPENDS testAddItem)

  # "push" command tests
  add_test(testPushNoAuth ${midascli_command} --database ${sqlite_db} push)
  set_tests_properties(testPushNoAuth PROPERTIES DEPENDS testAddBitstream WILL_FAIL ON)
  add_test(testPush ${midascli_command} --database ${sqlite_db} --profile test push)
  set_tests_properties(testPush PROPERTIES DEPENDS testPushNoAuth)
  
  # metadata editing tests - community
  add_test(delete.community.dest "${CMAKE_COMMAND}" -E remove_directory "${dashboard_top_level_community_name}_changed")
  add_test(metadata.community.name ${midascli_command} --database ${sqlite_db} set_metadata "${dashboard_top_level_community_name}" name "${dashboard_top_level_community_name}_changed")
  set_tests_properties(metadata.community.name PROPERTIES DEPENDS delete.community.dest)
  add_test(metadata.community.copyright ${midascli_command} --database ${sqlite_db} set_metadata "${dashboard_top_level_community_name}_changed" copyright "new copyright text")
  add_test(metadata.community.description ${midascli_command} --database ${sqlite_db} set_metadata "${dashboard_top_level_community_name}_changed" description "new description text")
  add_test(metadata.community.introductory ${midascli_command} --database ${sqlite_db} set_metadata "${dashboard_top_level_community_name}_changed" introductory "new introductory text")
  set_tests_properties(metadata.community.copyright metadata.community.description metadata.community.introductory PROPERTIES DEPENDS metadata.community.name)
  add_test(pushmodified.community ${midascli_command} --database ${sqlite_db} push)
  set_tests_properties(pushmodified.community PROPERTIES DEPENDS "metadata.community.introductory;metadata.community.copyright;metadata.community.description"
    PASS_REGULAR_EXPRESSION "community ${dashboard_top_level_community_name}_changed")
  
  # Restore database to a fresh state again
  add_test(cleanDatabase3 ${midascli_command} --database ${sqlite_db} clean)
  set_tests_properties(cleanDatabase3 PROPERTIES DEPENDS testPush)

  # Coverage for runtime new database creation
  add_test(deleteNewDatabase "${CMAKE_COMMAND}" -E remove -f ${new_db})
  add_test(testCreateDatabaseRuntime ${midascli_command} --database ${new_db} clean)
  set_tests_properties(testCreateDatabaseRuntime PROPERTIES DEPENDS deleteNewDatabase)
  
  add_test(testUpload ${midascli_command} --database ${sqlite_db} --profile test upload "${MIDASCPP_SOURCE_DIR}/Application/MIDASDesktop/icons/add2.png" "${dashboard_top_level_community_name}/Subcomm/Collection/Item")
  set_tests_properties(testUpload PROPERTIES DEPENDS cleanDatabase3)

endif(BUILD_MIDASCPP_TESTING)
  

