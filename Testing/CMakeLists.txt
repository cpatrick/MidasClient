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

  # "create_profile" command tests
  add_test(testCreateProfile ${midascli_command} --database ${sqlite_db} create_profile --name test --email ${dashboard_midas_email} --app-name ${dashboard_midas_app_name} --api-key ${dashboard_midas_api_key})

  add_test(testPushNoResources ${midascli_command} --database ${sqlite_db} --profile test push)

  # Restore database to a fresh state again
  add_test(cleanDatabase2 ${midascli_command} --database ${sqlite_db} clean)

  # "add" command tests
  add_test(testAddTopLevelCommunity ${midascli_command} --database ${sqlite_db} add -C "${dashboard_top_level_community_name}")
  add_test(testAddSubCommunity ${midascli_command} --database ${sqlite_db} add -C "${dashboard_top_level_community_name}/Subcomm")
  add_test(testAddCollection ${midascli_command} --database ${sqlite_db} add -c "${dashboard_top_level_community_name}/Subcomm/Collection")
  add_test(testAddItem ${midascli_command} --database ${sqlite_db} add -i "${dashboard_top_level_community_name}/Subcomm/Collection/Item")
  add_test(testAddBitstream ${midascli_command} --database ${sqlite_db} add -b "${dashboard_top_level_community_name}/Subcomm/Collection/Item/bitstream.txt")
  add_test(testStatusAdded ${midascli_command} --database ${sqlite_db} status)
  set_tests_properties(testStatusAdded PROPERTIES WILL_FAIL ON)

  # "push" command tests
  add_test(testPushNoAuth ${midascli_command} --database ${sqlite_db} push)
  set_tests_properties(testPushNoAuth PROPERTIES WILL_FAIL ON)
  add_test(testPush ${midascli_command} --database ${sqlite_db} --profile test push)
  
  # "upload" command tests
  add_test(testUpload ${midascli_command} --database ${sqlite_db} --profile test upload "${MIDASCPP_SOURCE_DIR}/Application/MIDASDesktop/icons/add2.png" "${dashboard_top_level_community_name}/Subcomm/Collection/Item")

  # metadata editing tests - community
  add_test(delete.community.dest "${CMAKE_COMMAND}" -E remove_directory "${dashboard_top_level_community_name}_changed")
  add_test(metadata.community.name ${midascli_command} --database ${sqlite_db} set_metadata "${dashboard_top_level_community_name}" name "${dashboard_top_level_community_name}_changed")
  add_test(metadata.community.copyright ${midascli_command} --database ${sqlite_db} set_metadata "${dashboard_top_level_community_name}_changed" copyright "new copyright text")
  add_test(metadata.community.description ${midascli_command} --database ${sqlite_db} set_metadata "${dashboard_top_level_community_name}_changed" description "new description text")
  add_test(metadata.community.introductory ${midascli_command} --database ${sqlite_db} set_metadata "${dashboard_top_level_community_name}_changed" introductory "new introductory text")
  add_test(testStatusModified.community ${midascli_command} --database ${sqlite_db} status)
  set_tests_properties(testStatusModified.community PROPERTIES WILL_FAIL ON)
  
  # metadata editing tests - collection
  add_test(delete.collection.dest "${CMAKE_COMMAND}" -E remove_directory "${dashboard_top_level_community_name}_changed/Subcomm/Collection_changed")
  add_test(metadata.collection.name ${midascli_command} --database ${sqlite_db} set_metadata "${dashboard_top_level_community_name}_changed/Subcomm/Collection" name "Collection_changed")
  add_test(metadata.collection.copyright ${midascli_command} --database ${sqlite_db} set_metadata "${dashboard_top_level_community_name}_changed/Subcomm/Collection_changed" copyright "new copyright text")
  add_test(metadata.collection.description ${midascli_command} --database ${sqlite_db} set_metadata "${dashboard_top_level_community_name}_changed/Subcomm/Collection_changed" description "new description text")
  add_test(metadata.collection.introductory ${midascli_command} --database ${sqlite_db} set_metadata "${dashboard_top_level_community_name}_changed/Subcomm/Collection_changed" introductory "new introductory text")
  add_test(testStatusModified.collection ${midascli_command} --database ${sqlite_db} status)
  set_tests_properties(testStatusModified.collection PROPERTIES WILL_FAIL ON)
  
  # metadata editing tests - collection
  add_test(delete.item.dest "${CMAKE_COMMAND}" -E remove_directory "${dashboard_top_level_community_name}_changed/Subcomm/Collection_changed/Item_changed")
  add_test(metadata.item.title ${midascli_command} --database ${sqlite_db} set_metadata "${dashboard_top_level_community_name}_changed/Subcomm/Collection_changed/Item" title "Item_changed")
  add_test(metadata.item.abstract ${midascli_command} --database ${sqlite_db} set_metadata "${dashboard_top_level_community_name}_changed/Subcomm/Collection_changed/Item_changed" abstract "new abstract text")
  add_test(metadata.item.description ${midascli_command} --database ${sqlite_db} set_metadata "${dashboard_top_level_community_name}_changed/Subcomm/Collection_changed/Item_changed" description "new description text")
  add_test(metadata.item.authors ${midascli_command} --database ${sqlite_db} set_metadata "${dashboard_top_level_community_name}_changed/Subcomm/Collection_changed/Item_changed" authors "Last, First")
  add_test(metadata.item.authors.append ${midascli_command} --database ${sqlite_db} set_metadata -a "${dashboard_top_level_community_name}_changed/Subcomm/Collection_changed/Item_changed" authors "Author, Other")
  add_test(metadata.item.keywords ${midascli_command} --database ${sqlite_db} set_metadata "${dashboard_top_level_community_name}_changed/Subcomm/Collection_changed/Item_changed" keywords "foo")
  add_test(metadata.item.keywords.append ${midascli_command} --database ${sqlite_db} set_metadata -a "${dashboard_top_level_community_name}_changed/Subcomm/Collection_changed/Item_changed" keywords "bar")
  add_test(testStatusModified.item ${midascli_command} --database ${sqlite_db} status)
  set_tests_properties(testStatusModified.item PROPERTIES WILL_FAIL ON)

  # push metadata changes
  add_test(pushModifiedMetadata ${midascli_command} --profile test --database ${sqlite_db} push)
  set_tests_properties(pushModifiedMetadata PROPERTIES
    PASS_REGULAR_EXPRESSION
    "Pushed community ${dashboard_top_level_community_name}_changed.*Pushed collection Collection_changed.*Pushed item Item_changed"
  )

  # Restore database to a fresh state again
  add_test(cleanDatabase3 ${midascli_command} --database ${sqlite_db} clean)

  # Coverage for runtime new database creation
  add_test(deleteNewDatabase "${CMAKE_COMMAND}" -E remove -f ${new_db})
  add_test(testCreateDatabaseRuntime ${midascli_command} --database ${new_db} clean)

endif(BUILD_MIDASCPP_TESTING)
  

