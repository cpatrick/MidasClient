project(MIDASDesktop)
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

set(MIDASDesktopUI_SRCS
  main.cxx
  GUI/MIDASDesktopUI.cxx
  GUI/MIDASDesktopUI.h
  GUI/SignInUI.cxx
  GUI/SignInUI.h
  GUI/SignInThread.cxx
  GUI/SignInThread.h
  GUI/CreateMidasCommunityUI.cxx
  GUI/CreateMidasCommunityUI.h
  GUI/CreateMidasCollectionUI.cxx
  GUI/CreateMidasCollectionUI.h
  GUI/CreateMidasItemUI.cxx
  GUI/CreateMidasItemUI.h
  GUI/CreateProfileUI.cxx
  GUI/CreateProfileUI.h
  GUI/DeleteResourceUI.h
  GUI/DeleteResourceUI.cxx
  GUI/UploadAgreementUI.cxx
  GUI/UploadAgreementUI.h
  GUI/AboutUI.cxx
  GUI/AboutUI.h
  GUI/AutoRefreshUI.h
  GUI/AutoRefreshUI.cxx
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
  GUI/ProcessingStatusUI.h
  GUI/ProcessingStatusUI.cxx
  GUI/RefreshServerTreeThread.h
  GUI/SynchronizerThread.h
  GUI/ExpandTreeThread.h
  GUI/ExpandTreeThread.cxx
  GUI/SearchThread.h
  GUI/SearchThread.cxx
)

set(MIDASDesktopUI_SRCS ${MIDASDesktopUI_SRCS} MidasClient.rc)

set(MIDASDesktopUI_UI_SRCS
  GUI/MIDASDesktopUI.ui
  GUI/CreateMidasCommunityUI.ui
  GUI/CreateMidasCollectionUI.ui
  GUI/CreateMidasItemUI.ui
  GUI/CreateProfileUI.ui
  GUI/DeleteResourceUI.ui
  GUI/UploadAgreementUI.ui
  GUI/SignInUI.ui
  GUI/AboutUI.ui
  GUI/ProcessingStatusUI.ui
  GUI/PullUI.ui
  GUI/AutoRefreshUI.ui
)

set(MIDASDesktopUI_RCCS MIDASDesktopIcons.qrc)

set(MIDASDesktopUI_MOC_SRCS
  GUI/MIDASDesktopUI.h 
  GUI/CreateMidasCommunityUI.h
  GUI/CreateMidasCollectionUI.h
  GUI/CreateMidasItemUI.h
  GUI/CreateProfileUI.h
  GUI/DeleteResourceUI.h
  GUI/PullUI.h
  GUI/UploadAgreementUI.h 
  GUI/SignInUI.h
  GUI/SignInThread.h
  GUI/SearchThread.h
  GUI/AboutUI.h
  GUI/AutoRefreshUI.h
  GUI/MidasTreeView.h
  GUI/MidasTreeViewClient.h
  GUI/MidasTreeItem.h
  GUI/MidasTreeModel.h
  GUI/MidasTreeModelClient.h
  GUI/MidasCommunityTreeItem.h
  GUI/MidasCollectionTreeItem.h
  GUI/MidasItemTreeItem.h
  GUI/MidasBitstreamTreeItem.h
  GUI/MidasApplication.h
  GUI/MidasResourceDescTable.h
  GUI/MidasSearchResultList.h
  GUI/ProcessingStatusUI.h
  GUI/RefreshServerTreeThread.h
  GUI/ExpandTreeThread.h
  GUI/SynchronizerThread.h
  GUI/GUIProgress.h
  GUI/GUILogger.h
)

QT4_WRAP_CPP(MIDASDesktopUI_SRCS ${MIDASDesktopUI_MOC_SRCS})
QT4_WRAP_UI(MIDASDesktopUI_UI_CXX ${MIDASDesktopUI_UI_SRCS})
QT4_ADD_RESOURCES(MIDASDesktopUI_SRCS ${MIDASDesktopUI_RCCS})
 
# MIDASDesktop Executable
if(WIN32)
  add_executable(MIDASDesktop WIN32 ${MIDASDesktopUI_SRCS} ${MIDASDesktopUI_UI_CXX})
else(WIN32)
  add_executable(MIDASDesktop ${MIDASDesktopUI_SRCS} ${MIDASDesktopUI_UI_CXX})
endif(WIN32)

target_link_libraries(MIDASDesktop
  MIDAS
  MIDASDesktopUtils
  kwsys
  ${QT_QTCORE_LIBRARY}
  ${QT_QTMAIN_LIBRARY}
  ${QT_QTGUI_LIBRARY}
)

install(TARGETS MIDASDesktop DESTINATION bin)

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
              endif(WIN32)
            endforeach(qtlib)
        endif()
    endif(DEFINED QT_QMAKE_EXECUTABLE)
endif()
