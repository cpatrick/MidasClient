include_directories(
  "${MIDASClient_SOURCE_DIR}/Libs/External/sqlite"
  ${MIDASClient_SOURCE_DIR}/Libs/Core/Code
  ${MIDASClient_BINARY_DIR}/Libs/Core/Code
  ${CMAKE_CURRENT_SOURCE_DIR}
  ${CMAKE_CURRENT_BINARY_DIR}
  GUI
  icons
)

set(MIDASWidgetsUI_SRCS
  GUI/SignInUI.cxx
  GUI/SignInThread.cxx
  GUI/CreateMidasResourceUI.cxx
  GUI/CreateProfileUI.cxx
  GUI/DeleteResourceUI.cxx
  GUI/AboutUI.cxx
  GUI/PreferencesUI.cxx
  GUI/PullUI.cxx
  GUI/PushUI.cxx
  GUI/GUIAgreement.cxx
  GUI/GUIFileOverwriteHandler.cxx
  GUI/GUIMirrorHandler.cxx
  GUI/GUIUpgradeHandler.cxx
  GUI/GUIProgress.cxx
  GUI/MidasTreeView.cxx
  GUI/MidasTreeViewServer.cxx
  GUI/MidasTreeViewClient.cxx
  GUI/TreeViewUpdateHandler.cxx
  GUI/MidasTreeItem.cxx
  GUI/MidasTreeModel.cxx
  GUI/MidasTreeModelServer.cxx
  GUI/MidasTreeModelClient.cxx
  GUI/MidasCommunityTreeItem.cxx
  GUI/MidasCollectionTreeItem.cxx
  GUI/MidasItemTreeItem.cxx
  GUI/MidasBitstreamTreeItem.cxx
  GUI/MidasResourceDescTable.cxx
  GUI/MidasSearchResultList.cxx
  GUI/MirrorPickerUI.cxx
  GUI/ExpandTreeThread.cxx
  GUI/UpdateTreeViewThread.cxx
  GUI/PollFilesystemThread.cxx
  GUI/AddBitstreamsThread.cxx
  GUI/DeleteThread.cxx
  GUI/SearchThread.cxx
  GUI/UnifyTreeThread.cxx
  GUI/ResourceEdit.cxx
  GUI/ButtonDelegate.cxx
  GUI/ButtonLineEdit.cxx
  GUI/AddAuthorUI.cxx
  GUI/AddKeywordUI.cxx
  GUI/AgreementUI.cxx
  GUI/UpgradeUI.cxx
  GUI/FileOverwriteUI.cxx
  GUI/TextEditDelegate.cxx
  GUI/IncompleteTransferWidget.cxx

  GUI/Midas3TreeModel.cxx
  GUI/Midas3TreeModelServer.cxx
  GUI/Midas3TreeModelClient.cxx
  GUI/Midas3TreeView.cxx
  GUI/Midas3TreeViewServer.cxx
  GUI/Midas3TreeViewClient.cxx
  GUI/Midas3TreeItem.cxx
  GUI/Midas3FolderTreeItem.cxx
  GUI/Midas3ItemTreeItem.cxx
  GUI/Midas3BitstreamTreeItem.cxx
)

set(MIDASWidgetsUI_UI_SRCS
  GUI/CreateMidasResourceUI.ui
  GUI/CreateProfileUI.ui
  GUI/DeleteResourceUI.ui
  GUI/SignInUI.ui
  GUI/AboutUI.ui
  GUI/PullUI.ui
  GUI/PushUI.ui
  GUI/PreferencesUI.ui
  GUI/AddAuthorUI.ui
  GUI/AddKeywordUI.ui
  GUI/AgreementUI.ui
  GUI/UpgradeUI.ui
  GUI/FileOverwriteUI.ui
  GUI/MirrorPickerUI.ui
)

set(MIDASWidgetsUI_RCCS MIDASWidgetsIcons.qrc)

set(MIDASWidgetsUI_MOC_SRCS
  GUI/CreateMidasResourceUI.h
  GUI/CreateProfileUI.h
  GUI/DeleteResourceUI.h
  GUI/PullUI.h
  GUI/PushUI.h
  GUI/SignInUI.h
  GUI/SignInThread.h
  GUI/SearchThread.h
  GUI/UpdateTreeViewThread.h
  GUI/PollFilesystemThread.h
  GUI/AddBitstreamsThread.h
  GUI/AboutUI.h
  GUI/PreferencesUI.h
  GUI/MidasTreeViewBase.h
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
  GUI/MidasResourceDescTable.h
  GUI/MidasSearchResultList.h
  GUI/ExpandTreeThread.h
  GUI/DeleteThread.h
  GUI/SynchronizerThread.h
  GUI/UnifyTreeThread.h
  GUI/GUIAgreement.h
  GUI/GUIFileOverwriteHandler.h
  GUI/GUIMirrorHandler.h
  GUI/GUIProgress.h
  GUI/GUIUpgradeHandler.h
  GUI/ResourceEdit.h
  GUI/TextEditDelegate.h
  GUI/ButtonDelegate.h
  GUI/ButtonLineEdit.h
  GUI/ButtonEditUI.h
  GUI/AddAuthorUI.h
  GUI/AddKeywordUI.h
  GUI/AgreementUI.h
  GUI/UpgradeUI.h
  GUI/FileOverwriteUI.h
  GUI/TreeViewUpdateHandler.h
  GUI/MirrorPickerUI.h
  GUI/IncompleteTransferWidget.h

  GUI/Midas3TreeModel.h
  GUI/Midas3TreeModelServer.h
  GUI/Midas3TreeModelClient.h
  GUI/Midas3TreeView.h
  GUI/Midas3TreeViewServer.h
  GUI/Midas3TreeViewClient.h
  GUI/Midas3TreeItem.h
  GUI/Midas3FolderTreeItem.h
  GUI/Midas3ItemTreeItem.h
  GUI/Midas3BitstreamTreeItem.h
)

QT4_WRAP_CPP(MIDASWidgetsUI_SRCS ${MIDASWidgetsUI_MOC_SRCS})
QT4_WRAP_UI(MIDASWidgetsUI_UI_CXX ${MIDASWidgetsUI_UI_SRCS})
QT4_ADD_RESOURCES(MIDASWidgetsUI_SRCS ${MIDASWidgetsUI_RCCS})

add_library(MIDASClientWidgets ${MIDASWidgetsUI_SRCS} ${MIDASWidgetsUI_UI_CXX})

if(WIN32)
  target_link_libraries (MIDASClientWidgets
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
    target_link_libraries(MIDASClientWidgets ${X11_LIBRARIES})

    if(XFT_LIBRARY)
      target_link_libraries(MIDASClientWidgets ${XFT_LIBRARY})
    endif(XFT_LIBRARY)

    if(GLIB_LIBRARY)
      target_link_libraries(MIDASClientWidgets ${GLIB_LIBRARY})
    endif(GLIB_LIBRARY)

    if(GTHREAD_LIBRARY)
      target_link_libraries(MIDASClientWidgets ${GTHREAD_LIBRARY})
    endif(GTHREAD_LIBRARY)

    if(XT_LIBRARY)
      target_link_libraries(MIDASClientWidgets ${XT_LIBRARY})
    endif(XT_LIBRARY)

    if(XI_LIBRARY)
      target_link_libraries(MIDASClientWidgets ${XI_LIBRARY})
    endif(XI_LIBRARY)

    if(XINERAMA_LIBRARY)
      target_link_libraries(MIDASClientWidgets ${XINERAMA_LIBRARY})
    endif(XINERAMA_LIBRARY)

    if(XRANDR_LIBRARY)
      target_link_libraries(MIDASClientWidgets ${XRANDR_LIBRARY})
    endif(XRANDR_LIBRARY)

    if(XCURSOR_LIBRARY)
      target_link_libraries(MIDASClientWidgets ${XCURSOR_LIBRARY})
    endif(XCURSOR_LIBRARY)

    if(CRYPT_LIBRARY)
      target_link_libraries(MIDASClientWidgets ${CRYPT_LIBRARY})
    endif(CRYPT_LIBRARY)
  endif(NOT APPLE)
endif(UNIX)

target_link_libraries(MIDASClientWidgets
  MIDASClientCore
  ${QT_QTCORE_LIBRARY}
  ${QT_QTGUI_LIBRARY}
  ${QT_QTMAIN_LIBRARY}
)

include(GenerateConfig.cmake)
