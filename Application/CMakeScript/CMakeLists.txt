include(CTest)
include(MIDAS.cmake)

enable_testing()

if(BUILD_MIDASCPP_TESTING)
  add_test(DeleteDownloadedData "${CMAKE_COMMAND}" -E remove -f "${MIDASCPP_BINARY_DIR}/MIDAS_Data/MIDAS_Hashes/879071fb20d959235fac207f36773851")
  add_test(DeleteDownloadedSymlink "${CMAKE_COMMAND}" -E remove_directory "${MIDASCPP_BINARY_DIR}/MIDAS_Data/test1")

  set(MIDAS_REST_URL ${dashboard_midas_server_url})
  set(MIDAS_KEY_DIR "${CMAKE_CURRENT_SOURCE_DIR}/TestKeys")

  midas_add_test(TestAddMidasTest "${CMAKE_COMMAND}" -E compare_files MIDAS{test1/input/add2.png.md5} "${MIDASCPP_SOURCE_DIR}/Application/MIDASDesktop/icons/add2.png" MIDAS_FETCH_ONLY{test1/input/bitstream.txt.md5})
  set_tests_properties(TestAddMidasTest_fetchData PROPERTIES PASS_REGULAR_EXPRESSION "Data is here:.*/midas.bitstream.by.hash\\?hash=879071fb20d959235fac207f36773851&algorithm=MD5")

  set_tests_properties(TestAddMidasTest TestAddMidasTest_fetchData DeleteDownloadedData DeleteDownloadedSymlink PROPERTIES LABELS "CMakeScript")
endif(BUILD_MIDASCPP_TESTING)
