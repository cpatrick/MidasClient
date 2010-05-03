set(midascli_command "${MIDASCPP_BINARY_DIR}/bin/MIDAScli")
set(midas_server_url http://xanadu/MIDASserver/midas/api/rest)
set(midas_api_key CFtrF6RJxrOY6DVTFbiH73w6LGGlt4o7zGIFUthG)
set(midas_app_name zach)
set(midas_email admin@localhost)

configure_file("${MIDASCPP_BINARY_DIR}/midas.db" "${MIDASCPP_BINARY_DIR}/Testing/midas.db" COPYONLY)

add_test(testCliNoArgs ${midascli_command})
set_tests_properties(testCliNoArgs PROPERTIES WILL_FAIL ON)

add_test(testClean ${midascli_command} clean)

add_test(testClone ${midascli_command} clone ${midas_server_url})
set_tests_properties(testClone PROPERTIES DEPENDS testClean)

add_test(testCreateProfile ${midascli_command} create_profile --name test --email ${midas_email} --app-name ${midas_app_name} --api-key ${midas_api_key})
set_tests_properties(testCreateProfile PROPERTIES DEPENDS testClone)

add_test(testPushNoResources ${midascli_command} --profile test push)
set_tests_properties(testPushNoResources PROPERTIES DEPENDS testCreateProfile)
