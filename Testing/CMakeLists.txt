site_name(host_name)
set(midascli_command "${MIDASCPP_BINARY_DIR}/bin/MIDAScli")
set(sqlite3_command "${MIDASCPP_BINARY_DIR}/bin/sqlite3")
set(midas_server_url http://${host_name}/midas/api/rest)
set(midas_api_key CFtrF6RJxrOY6DVTFbiH73w6LGGlt4o7zGIFUthG)
set(midas_app_name zach)
set(midas_email admin@localhost)
site_name(top_level_community_name)


# Setup a mock tree we can add
execute_process(COMMAND mkdir "${MIDASCPP_BINARY_DIR}/Testing/${top_level_community_name}")
execute_process(COMMAND mkdir "${MIDASCPP_BINARY_DIR}/Testing/${top_level_community_name}/Subcomm")
execute_process(COMMAND mkdir "${MIDASCPP_BINARY_DIR}/Testing/${top_level_community_name}/Subcomm/Collection")
execute_process(COMMAND mkdir "${MIDASCPP_BINARY_DIR}/Testing/${top_level_community_name}/Subcomm/Collection/Item")
configure_file(
  "${MIDASCPP_SOURCE_DIR}/README.TXT"
  "${MIDASCPP_BINARY_DIR}/Testing/${top_level_community_name}/Subcomm/Collection/Item/bitstream.txt"
  COPYONLY)

# Pull a copy of the database into this dir so we can play with it
configure_file("${MIDASCPP_BINARY_DIR}/midas.db" "${MIDASCPP_BINARY_DIR}/Testing/midas.db" COPYONLY)

add_test(testCliNoArgs ${midascli_command})
set_tests_properties(testCliNoArgs PROPERTIES WILL_FAIL ON)

# Restore database to a fresh state
add_test(cleanDatabase ${midascli_command} clean)
add_test(deleteAuthProfiles ${sqlite3_command} midas.db "DELETE FROM auth_profile;")

# "clone" command tests
add_test(testClone ${midascli_command} clone ${midas_server_url})
set_tests_properties(testClone PROPERTIES DEPENDS "cleanDatabase;deleteAuthProfiles")

# "create_profile" command tests
add_test(testCreateProfile ${midascli_command} create_profile --name test --email ${midas_email} --app-name ${midas_app_name} --api-key ${midas_api_key})
set_tests_properties(testCreateProfile PROPERTIES DEPENDS testClone)

add_test(testPushNoResources ${midascli_command} --profile test push)
set_tests_properties(testPushNoResources PROPERTIES DEPENDS testCreateProfile)

# Restore database to a fresh state again
add_test(cleanDatabase2 ${midascli_command} clean)
set_tests_properties(cleanDatabase2 PROPERTIES DEPENDS testPushNoResources)

# "add" command tests
add_test(testAddTopLevelCommunity ${midascli_command} add -C "${top_level_community_name}")
set_tests_properties(testAddTopLevelCommunity PROPERTIES DEPENDS cleanDatabase2)
add_test(testAddSubCommunity ${midascli_command} add -C "${top_level_community_name}/Subcomm")
set_tests_properties(testAddSubCommunity PROPERTIES DEPENDS testAddTopLevelCommunity)
add_test(testAddCollection ${midascli_command} add -c "${top_level_community_name}/Subcomm/Collection")
set_tests_properties(testAddCollection PROPERTIES DEPENDS testAddSubCommunity)
add_test(testAddItem ${midascli_command} add -i "${top_level_community_name}/Subcomm/Collection/Item")
set_tests_properties(testAddItem PROPERTIES DEPENDS testAddCollection)
add_test(testAddBitstream ${midascli_command} add -b "${top_level_community_name}/Subcomm/Collection/Item/bitstream.txt")
set_tests_properties(testAddBitstream PROPERTIES DEPENDS testAddItem)

# "push" command tests
add_test(testPushNoAuth ${midascli_command} push)
set_tests_properties(testPushNoAuth PROPERTIES DEPENDS testAddBitstream WILL_FAIL ON)
add_test(testPush ${midascli_command} --profile test push)
set_tests_properties(testPush PROPERTIES DEPENDS testPushNoAuth)
