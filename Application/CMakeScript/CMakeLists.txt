include(CTest)
include(MIDAS.cmake)

enable_testing()

if(BUILD_TESTING)
  add_test(DeleteDownloadedData "${CMAKE_COMMAND}" -E remove -f "${MIDASClient_BINARY_DIR}/MIDAS_Data/MIDAS_Hashes/879071fb20d959235fac207f36773851")
  add_test(DeleteDownloadedSymlink "${CMAKE_COMMAND}" -E remove_directory "${MIDASClient_BINARY_DIR}/MIDAS_Data/test1")

  set(MIDAS_REST_URL ${dashboard_midas_server_url})
  set(MIDAS_KEY_DIR "${CMAKE_CURRENT_SOURCE_DIR}/TestKeys")

  midas_add_test(TestAddMidasTest "${CMAKE_COMMAND}" -E compare_files MIDAS{test1/input/add2.png.md5} "${MIDASClient_SOURCE_DIR}/Application/MIDASDesktop/icons/add2.png" MIDAS_FETCH_ONLY{test1/input/add2.png.md5})
  set_tests_properties(TestAddMidasTest_fetchData PROPERTIES PASS_REGULAR_EXPRESSION "Data is here:.*/midas.bitstream.by.hash\\?hash=879071fb20d959235fac207f36773851&algorithm=MD5")

  midas_add_test(MIDAS_DIRECTORY "${CMAKE_COMMAND}" -E echo MIDAS_DIRECTORY{test1/input})
  set_tests_properties(MIDAS_DIRECTORY PROPERTIES PASS_REGULAR_EXPRESSION "${MIDASClient_BINARY_DIR}/MIDAS_Data/test1/input")

  # ----------------------- Test MIDAS_TGZ capabilities ----------------------------------------
  if(NOT EXISTS "${MIDASClient_BINARY_DIR}/Testing/testArchive.tar.gz")
    execute_process(COMMAND "${CMAKE_COMMAND}" -E tar czf testArchive.tar.gz Application
      WORKING_DIRECTORY "${MIDASClient_SOURCE_DIR}")
    file(RENAME "${MIDASClient_SOURCE_DIR}/testArchive.tar.gz" "${MIDASClient_BINARY_DIR}/Testing/testArchive.tar.gz")
  endif(NOT EXISTS "${MIDASClient_BINARY_DIR}/Testing/testArchive.tar.gz")

  execute_process(COMMAND "${CMAKE_COMMAND}" -E md5sum "${MIDASClient_BINARY_DIR}/Testing/testArchive.tar.gz" OUTPUT_VARIABLE md5output)
  string(SUBSTRING ${md5output} 0 32 computedChecksum)
  set(MIDAS_KEY_DIR "${MIDASClient_BINARY_DIR}/Testing/keys")
  file(MAKE_DIRECTORY "${MIDAS_KEY_DIR}/testTgz")
  file(WRITE "${MIDAS_KEY_DIR}/testTgz/testArchive.tar.gz.md5" ${computedChecksum})

  add_test(TestUploadTGZ "${MIDASClient_BINARY_DIR}/bin/MIDAScli"
    --profile test
    --database "${MIDASClient_BINARY_DIR}/midas.db"
    upload "${MIDASClient_BINARY_DIR}/Testing/testArchive.tar.gz"
    "${dashboard_top_level_community_name}_changed/Subcomm/Collection_changed/Item_changed"
    ${dashboard_midas_server_url})

  midas_add_test(MIDAS_TGZ "${CMAKE_COMMAND}" -E echo MIDAS_TGZ{testTgz/testArchive.tar.gz.md5})
  add_test(VerifyArchivePresent "${CMAKE_COMMAND}" -E md5sum "${MIDASClient_BINARY_DIR}/MIDAS_Data/testTgz/testArchive/Application/CMakeScript/MIDAS.cmake")

  set_tests_properties(
    TestAddMidasTest
    TestAddMidasTest_fetchData
    DeleteDownloadedData
    DeleteDownloadedSymlink
    MIDAS_DIRECTORY
    MIDAS_DIRECTORY_fetchData
    TestUploadTGZ
    MIDAS_TGZ
    MIDAS_TGZ_fetchData
    PROPERTIES LABELS "CMakeScript")
endif(BUILD_TESTING)
