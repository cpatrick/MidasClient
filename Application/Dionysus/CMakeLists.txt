project(Dionysus)
cmake_minimum_required(VERSION 2.6)

# Find Qt
find_package(Qt4 REQUIRED)

include_directories(
  ${QT_INCLUDES}
  ${CMAKE_CURRENT_SOURCE_DIR}
  ${CMAKE_CURRENT_BINARY_DIR}
  GUI
  icons
  MIDAScpp
  utils
)

if(WIN32)
  link_libraries (
    imm32.lib
    winmm.lib
    Snmpapi.lib
  )
endif(WIN32)


if(UNIX)
  find_package(X11)

  if(NOT APPLE)
    link_libraries(
      #X11
      #Xext
      Xft
      png
      glib-2.0
      gthread-2.0
      Xt
      Xi
      Xinerama
      ${X11_LIBRARIES}
      Xrandr
      Xcursor
      crypt
    )
  endif(NOT APPLE)
endif(UNIX)

subdirs(utils)

set(DionysusUI_SRCS
  main.cxx
  GUI/DionysusUI.cxx
  GUI/DionysusUI.h
  GUI/SettingsUI.cxx
  GUI/SettingsUI.h
  GUI/SignInUI.cxx
  GUI/SignInUI.h
  GUI/UserUI.cxx
  GUI/UserUI.h
  GUI/CreateMidasCommunityUI.cxx
  GUI/CreateMidasCommunityUI.h
  GUI/CreateMidasCollectionUI.cxx
  GUI/CreateMidasCollectionUI.h
  GUI/CreateMidasItemUI.cxx
  GUI/CreateMidasItemUI.h
  GUI/CreateProfileUI.cxx
  GUI/CreateProfileUI.h
  GUI/UploadAgreementUI.cxx
  GUI/UploadAgreementUI.h
  GUI/AboutUI.cxx
  GUI/AboutUI.h
  GUI/AddResourceUI.cxx
  GUI/AddResourceUI.h
  GUI/PullUI.cxx
  GUI/PullUI.h
  GUI/GUILogger.cxx
  GUI/GUILogger.h
  GUI/GUIProgress.cxx
  GUI/GUIProgress.h
  GUI/MidasTreeView.cxx
  GUI/MidasTreeView.h
  GUI/MidasTreeViewClient.cxx
  GUI/MidasTreeViewClient.h
  GUI/MidasTreeItem.cxx
  GUI/MidasTreeItem.h
  GUI/MidasTreeModel.cxx
  GUI/MidasTreeModel.h
  GUI/MidasTreeModelClient.cxx
  GUI/MidasTreeModelClient.h
  GUI/MidasCommunityTreeItem.cxx
  GUI/MidasCommunityTreeItem.h
  GUI/MidasCollectionTreeItem.h
  GUI/MidasCollectionTreeItem.cxx
  GUI/MidasItemTreeItem.h
  GUI/MidasItemTreeItem.cxx
  GUI/MidasBitstreamTreeItem.h
  GUI/MidasBitstreamTreeItem.cxx
  GUI/MidasApplication.h
  GUI/MidasApplication.cxx
  GUI/MidasResourceDescTable.h
  GUI/MidasResourceDescTable.cxx
  GUI/MidasSearchResultList.h
  GUI/MidasSearchResultList.cxx
  GUI/MidasItemDelegate.h
  GUI/MidasItemDelegate.cxx
  GUI/ProcessingStatusUI.h
  GUI/ProcessingStatusUI.cxx
)

#set(DionysusUI_SRCS ${DionysusUI_SRCS} MidasClient.rc)

set(DionysusUI_UI_SRCS
  GUI/DionysusUI.ui
  GUI/CreateMidasCommunityUI.ui
  GUI/CreateMidasCollectionUI.ui
  GUI/CreateMidasItemUI.ui
  GUI/CreateProfileUI.ui
  GUI/UploadAgreementUI.ui
  GUI/SettingsUI.ui
  GUI/UserUI.ui
  GUI/SignInUI.ui
  GUI/AboutUI.ui
  GUI/ProcessingStatusUI.ui
  GUI/PullUI.ui
  GUI/AddResourceUI.ui
)

set(DionysusUI_RCCS DionysusIcons.qrc)

set(DionysusUI_MOC_SRCS
  GUI/DionysusUI.h 
  GUI/CreateMidasCommunityUI.h
  GUI/CreateMidasCollectionUI.h
  GUI/CreateMidasItemUI.h
  GUI/CreateProfileUI.h
  GUI/PullUI.h
  GUI/AddResourceUI.h
  GUI/SettingsUI.h 
  GUI/UploadAgreementUI.h 
  GUI/SignInUI.h 
  GUI/UserUI.h
  GUI/AboutUI.h
  GUI/MidasTreeView.h
  GUI/MidasTreeViewClient.h
  GUI/MidasTreeItem.h
  GUI/MidasTreeModel.h
  GUI/MidasTreeModelClient.h
  GUI/MidasTreeModelClient.h
  GUI/MidasCommunityTreeItem.h
  GUI/MidasCollectionTreeItem.h
  GUI/MidasItemTreeItem.h
  GUI/MidasBitstreamTreeItem.h
  GUI/MidasApplication.h
  GUI/MidasResourceDescTable.h
  GUI/MidasSearchResultList.h
  GUI/MidasItemDelegate.h
  GUI/ProcessingStatusUI.h
)

QT4_WRAP_CPP(DionysusUI_SRCS ${DionysusUI_MOC_SRCS})
QT4_WRAP_UI(DionysusUI_UI_CXX ${DionysusUI_UI_SRCS})
QT4_ADD_RESOURCES(DionysusUI_SRCS ${DionysusUI_RCCS})
 
# MidasDesktop Executable
if(WIN32)
  add_executable(MidasDesktop WIN32 ${DionysusUI_SRCS} ${DionysusUI_UI_CXX})
else(WIN32)
  add_executable(MidasDesktop ${DionysusUI_SRCS} ${DionysusUI_UI_CXX})
endif(WIN32)

target_link_libraries(MidasDesktop
  MIDAS
  DionysusUtils
  kwsys
  ${QT_QTCORE_LIBRARY}
  ${QT_QTMAIN_LIBRARY}
  ${QT_QTGUI_LIBRARY}
)

install(TARGETS MidasDesktop DESTINATION bin)

# -----Install Qt Shared libraries on windows-------------
if ( NOT Q_WS_MAC)
    if (DEFINED QT_QMAKE_EXECUTABLE)
        set (QTLIBLIST QtCore QtGui)

        if(MSVC)
            set(TYPE "d")
            foreach(qtlib ${QTLIBLIST})
              if(WIN32)
                get_filename_component(QT_DLL_PATH_tmp ${QT_QMAKE_EXECUTABLE} PATH)
                file(MAKE_DIRECTORY ${CMAKE_RUNTIME_OUTPUT_DIRECTORY}/Debug)
                file(MAKE_DIRECTORY ${CMAKE_RUNTIME_OUTPUT_DIRECTORY}/Release)
                file(MAKE_DIRECTORY ${CMAKE_RUNTIME_OUTPUT_DIRECTORY}/MinSizeRel)
                file(MAKE_DIRECTORY ${CMAKE_RUNTIME_OUTPUT_DIRECTORY}/RelWithDebInfo)
                install(FILES ${QT_DLL_PATH_tmp}/${qtlib}${type}d4.dll
                  #DESTINATION ./
                  DESTINATION bin
                  CONFIGURATIONS Debug
                  COMPONENT Applications)
                install(FILES ${QT_DLL_PATH_tmp}/${qtlib}4.dll
                  #DESTINATION ./
                  DESTINATION bin
                  CONFIGURATIONS Release
                  COMPONENT Applications)
                add_custom_target(Midas_${qtlib}-Debug-Copy ALL
                  COMMAND ${CMAKE_COMMAND} -E copy_if_different ${QT_DLL_PATH_tmp}/${qtlib}${TYPE}4.dll
                  ${CMAKE_RUNTIME_OUTPUT_DIRECTORY}/Debug/
                  COMMENT "Copying ${qtlib}${TYPE}4.dll to ${CMAKE_RUNTIME_OUTPUT_DIRECTORY}/Debug/")
                add_custom_target(Midas_${qtlib}-Release-Copy ALL
                  COMMAND ${CMAKE_COMMAND} -E copy_if_different ${QT_DLL_PATH_tmp}/${qtlib}4.dll
                  ${CMAKE_RUNTIME_OUTPUT_DIRECTORY}/Release/
                  COMMENT "Copying ${qtlib}4.dll to ${CMAKE_RUNTIME_OUTPUT_DIRECTORY}/Release/")
              endif(WIN32)
            endforeach(qtlib)
        endif()
    endif(DEFINED QT_QMAKE_EXECUTABLE)
endif()
