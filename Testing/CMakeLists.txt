if(BUILD_MIDASCPP_TESTING)
  set(midascli_command "${MIDASCPP_BINARY_DIR}/bin/MIDAScli")
  set(sqlite3_command "${MIDASCPP_BINARY_DIR}/bin/sqlite3")
  set(dashboard_midas_server_url http://localhost/midas/api/rest CACHE STRING "The URL of the REST API for MIDAS testing")
  set(dashboard_midas_api_key NO_KEY CACHE STRING "The MIDAS API key for test dashboard")
  set(dashboard_midas_app_name test CACHE STRING "The MIDAS API name for test dashboard")
  set(dashboard_midas_email admin@localhost CACHE STRING "The MIDAS login email for test dashboard")
  site_name(dashboard_top_level_community_name)
  
  set(sqlite_db "${MIDASCPP_BINARY_DIR}/midas.db")
  
    # Setup a mock tree we can add
  configure_file(
    "${MIDASCPP_SOURCE_DIR}/README.txt"
    "${MIDASCPP_BINARY_DIR}/Testing/${dashboard_top_level_community_name}/Subcomm/Collection/Item/bitstream.txt"
    COPYONLY)

  # Add target to copy our database to the testing dir during build
  add_custom_command (
    TARGET sqlite_db
    POST_BUILD
    COMMAND "${CMAKE_CMAKE_COMMAND}" -E copy "${MIDASCPP_BINARY_DIR}/midas.db" "${MIDASCPP_BINARY_DIR}/Testing/midas.db"
    COMMENT "Copying SQLite database into Test dir..."
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
endif(BUILD_MIDASCPP_TESTING)
  

