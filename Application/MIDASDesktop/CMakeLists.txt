project(MIDASDesktop)
cmake_minimum_required(VERSION 2.6)

# Find Qt
find_package(Qt4 REQUIRED)

include_directories(
  "${MIDASCPP_BINARY_DIR}/Code"
  ${QT_INCLUDES}
  ${CMAKE_CURRENT_SOURCE_DIR}
  ${CMAKE_CURRENT_BINARY_DIR}
  GUI
  icons
  MIDAScpp
  utils
)

subdirs(utils)

set(MIDASDesktopUI_SRCS
  GUI/MIDASDesktopUI.cxx
  GUI/MIDASDesktopUI.h
  GUI/SignInUI.cxx
  GUI/SignInUI.h
  GUI/SignInThread.cxx
  GUI/SignInThread.h
  GUI/CreateMidasResourceUI.cxx
  GUI/CreateMidasResourceUI.h
  GUI/CreateProfileUI.cxx
  GUI/CreateProfileUI.h
  GUI/DeleteResourceUI.h
  GUI/DeleteResourceUI.cxx
  GUI/AboutUI.cxx
  GUI/AboutUI.h
  GUI/PreferencesUI.h
  GUI/PreferencesUI.cxx
  GUI/PullUI.cxx
  GUI/PullUI.h
  GUI/GUIAgreement.cxx
  GUI/GUIAgreement.h
  GUI/GUIFileOverwriteHandler.h
  GUI/GUIFileOverwriteHandler.cxx
  GUI/GUILogger.cxx
  GUI/GUILogger.h
  GUI/GUIProgress.cxx
  GUI/GUIProgress.h
  GUI/MidasTreeView.cxx
  GUI/MidasTreeView.h
  GUI/MidasTreeViewServer.cxx
  GUI/MidasTreeViewServer.h
  GUI/MidasTreeViewClient.cxx
  GUI/MidasTreeViewClient.h
  GUI/TreeViewUpdateHandler.h
  GUI/TreeViewUpdateHandler.cxx
  GUI/MidasTreeItem.cxx
  GUI/MidasTreeModel.h
  GUI/MidasTreeModel.cxx
  GUI/MidasTreeModelServer.cxx
  GUI/MidasTreeModelServer.h
  GUI/MidasTreeModelClient.cxx
  GUI/MidasTreeModelClient.h
  GUI/MidasTreeItem.h
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
  GUI/ReadDatabaseThread.h
  GUI/ReadDatabaseThread.cxx
  GUI/PollFilesystemThread.h
  GUI/PollFilesystemThread.cxx
  GUI/SearchThread.h
  GUI/SearchThread.cxx
  GUI/UnifyTreeThread.h
  GUI/UnifyTreeThread.cxx
  GUI/ResourceEdit.h
  GUI/ResourceEdit.cxx
  GUI/ButtonDelegate.h
  GUI/ButtonDelegate.cxx
  GUI/ButtonLineEdit.h
  GUI/ButtonLineEdit.cxx
  GUI/ButtonEditUI.h
  GUI/AddAuthorUI.h
  GUI/AddAuthorUI.cxx
  GUI/AddKeywordUI.h
  GUI/AddKeywordUI.cxx
  GUI/AgreementUI.h
  GUI/AgreementUI.cxx
  GUI/FileOverwriteUI.h
  GUI/FileOverwriteUI.cxx
  GUI/TextEditDelegate.h
  GUI/TextEditDelegate.cxx
)

set(MIDASDesktopUI_SRCS ${MIDASDesktopUI_SRCS} MIDASDesktop.rc)

set(MIDASDesktopUI_UI_SRCS
  GUI/MIDASDesktopUI.ui
  GUI/CreateMidasResourceUI.ui
  GUI/CreateProfileUI.ui
  GUI/DeleteResourceUI.ui
  GUI/SignInUI.ui
  GUI/AboutUI.ui
  GUI/ProcessingStatusUI.ui
  GUI/PullUI.ui
  GUI/PreferencesUI.ui
  GUI/AddAuthorUI.ui
  GUI/AddKeywordUI.ui
  GUI/AgreementUI.ui
  GUI/FileOverwriteUI.ui
)

set(MIDASDesktopUI_RCCS MIDASDesktopIcons.qrc)

set(MIDASDesktopUI_MOC_SRCS
  GUI/MIDASDesktopUI.h 
  GUI/CreateMidasResourceUI.h
  GUI/CreateProfileUI.h
  GUI/DeleteResourceUI.h
  GUI/PullUI.h
  GUI/SignInUI.h
  GUI/SignInThread.h
  GUI/SearchThread.h
  GUI/ReadDatabaseThread.h
  GUI/PollFilesystemThread.h
  GUI/AboutUI.h
  GUI/PreferencesUI.h
  GUI/MidasTreeView.h
  GUI/MidasTreeViewServer.h
  GUI/MidasTreeViewClient.h
  GUI/MidasTreeItem.h
  GUI/MidasTreeModel.h
  GUI/MidasTreeModelServer.h
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
  GUI/UnifyTreeThread.h
  GUI/GUIAgreement.h
  GUI/GUIFileOverwriteHandler.h
  GUI/GUIProgress.h
  GUI/GUILogger.h
  GUI/ResourceEdit.h
  GUI/TextEditDelegate.h
  GUI/ButtonDelegate.h
  GUI/ButtonLineEdit.h
  GUI/ButtonEditUI.h
  GUI/AddAuthorUI.h
  GUI/AddKeywordUI.h
  GUI/AgreementUI.h
  GUI/FileOverwriteUI.h
  GUI/TreeViewUpdateHandler.h
)

QT4_WRAP_CPP(MIDASDesktopUI_SRCS ${MIDASDesktopUI_MOC_SRCS})
QT4_WRAP_UI(MIDASDesktopUI_UI_CXX ${MIDASDesktopUI_UI_SRCS})
QT4_ADD_RESOURCES(MIDASDesktopUI_SRCS ${MIDASDesktopUI_RCCS})

add_library(MIDASDesktopLib ${MIDASDesktopUI_SRCS} ${MIDASDesktopUI_UI_CXX})

# MIDASDesktop Executable
if(WIN32)
  add_executable(MIDASDesktop WIN32 main.cxx MIDASDesktop.rc)
else(WIN32)
  add_executable(MIDASDesktop main.cxx)
endif(WIN32)

if(WIN32)
  target_link_libraries (MIDASDesktopLib
    imm32.lib
    winmm.lib
    Snmpapi.lib
  )
endif(WIN32)

if(UNIX)
  find_package(X11)
  find_library(XFT_LIBRARY Xft)
  find_library(GLIB_LIBRARY glib-2.0)
  find_library(GTHREAD_LIBRARY gthread-2.0)
  find_library(XT_LIBRARY Xt)
  find_library(XI_LIBRARY Xi)
  find_library(XINERAMA_LIBRARY Xinerama)
  find_library(XRANDR_LIBRARY Xrandr)
  find_library(XCURSOR_LIBRARY Xcursor)
  find_library(CRYPT_LIBRARY crypt)

  if(NOT APPLE)
    target_link_libraries(MIDASDesktopLib ${X11_LIBRARIES})
    
    if(XFT_LIBRARY)
      target_link_libraries(MIDASDesktopLib ${XFT_LIBRARY})
    endif(XFT_LIBRARY)
    
    if(GLIB_LIBRARY)
      target_link_libraries(MIDASDesktopLib ${GLIB_LIBRARY})
    endif(GLIB_LIBRARY)
    
    if(GTHREAD_LIBRARY)
      target_link_libraries(MIDASDesktopLib ${GTHREAD_LIBRARY})
    endif(GTHREAD_LIBRARY)

    if(XT_LIBRARY)
      target_link_libraries(MIDASDesktopLib ${XT_LIBRARY})
    endif(XT_LIBRARY)

    if(XI_LIBRARY)
      target_link_libraries(MIDASDesktopLib ${XI_LIBRARY})
    endif(XI_LIBRARY)
    
    if(XINERAMA_LIBRARY)
      target_link_libraries(MIDASDesktopLib ${XINERAMA_LIBRARY})
    endif(XINERAMA_LIBRARY)

    if(XRANDR_LIBRARY)
      target_link_libraries(MIDASDesktopLib ${XRANDR_LIBRARY})
    endif(XRANDR_LIBRARY)
    
    if(XCURSOR_LIBRARY)
      target_link_libraries(MIDASDesktopLib ${XCURSOR_LIBRARY})
    endif(XCURSOR_LIBRARY)
      
    if(CRYPT_LIBRARY)
      target_link_libraries(MIDASDesktopLib ${CRYPT_LIBRARY})
    endif(CRYPT_LIBRARY)
  endif(NOT APPLE)
endif(UNIX)

target_link_libraries(MIDASDesktopLib
  MIDAS
  MIDASDesktopUtils
  kwsys
  ${QT_QTCORE_LIBRARY}
  ${QT_QTGUI_LIBRARY}
  ${QT_QTMAIN_LIBRARY}
)

target_link_libraries(MIDASDesktop
  MIDASDesktopLib
)

install(TARGETS MIDASDesktop DESTINATION bin COMPONENT Runtime)

# -----Install Qt Shared libraries-------------
if (DEFINED QT_QMAKE_EXECUTABLE)
    set (QTLIBLIST QTCORE QTGUI)
    if(UNIX OR APPLE)
      list(APPEND QTLIBLIST QTDBUS)
    endif()
    set (QT_INSTALL_LIB_DIR bin)
    foreach(qtlib ${QTLIBLIST})
	  if(APPLE)
        INSTALL(DIRECTORY "${QT_${qtlib}_LIBRARY_RELEASE}" 
          DESTINATION ${QT_INSTALL_LIB_DIR} COMPONENT Runtime)
      elseif(UNIX)
        # Install .so and versioned .so.x.y
        get_filename_component(QT_LIB_DIR_tmp ${QT_${qtlib}_LIBRARY_RELEASE} PATH)
        get_filename_component(QT_LIB_NAME_tmp ${QT_${qtlib}_LIBRARY_RELEASE} NAME)
        file(GLOB libFiles_tmp "${QT_LIB_DIR_tmp}/${QT_LIB_NAME_tmp}*")
        foreach(libFile ${libFiles_tmp})
          if(NOT libFile MATCHES "\\.debug")
            install(FILES ${libFile}
              COMPONENT Runtime
              DESTINATION ${QT_INSTALL_LIB_DIR})
          endif()
        endforeach()
      elseif(WIN32)
        get_filename_component(QT_DLL_PATH_tmp ${QT_QMAKE_EXECUTABLE} PATH)

        install(FILES ${QT_DLL_PATH_tmp}/${qtlib}${type}d4.dll
          DESTINATION bin
          CONFIGURATIONS Debug
          COMPONENT Applications)
        install(FILES ${QT_DLL_PATH_tmp}/${qtlib}4.dll
          DESTINATION bin
          CONFIGURATIONS Release
          COMPONENT Applications)
      endif()
    endforeach(qtlib)
endif(DEFINED QT_QMAKE_EXECUTABLE)
