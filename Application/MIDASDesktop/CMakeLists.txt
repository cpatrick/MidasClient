project(MIDASDesktop)
cmake_minimum_required(VERSION 2.6)

include(${MIDASClient_BINARY_DIR}/../CTK-build/CTK-build/Libs/DataManagement/MIDASClient/Core/CTKDataManagementMIDASClientCoreConfig.cmake)
include(${MIDASClient_BINARY_DIR}/../CTK-build/CTK-build/Libs/DataManagement/MIDASClient/Widgets/CTKDataManagementMIDASClientWidgetsConfig.cmake)

include_directories(
  ${MIDASDesktop_SOURCE_DIR}
  ${MIDASDesktop_SOURCE_DIR}/GUI
  ${MIDASDesktop_BINARY_DIR}
  ${CTKDataManagementMIDASClientCore_INCLUDE_DIRS}
  ${CTKDataManagementMIDASClientWidgets_INCLUDE_DIRS})
link_directories(
  ${CTKDataManagementMIDASClientCore_LIBRARY_DIRS}
  ${CTKDataManagementMIDASClientWidgets_LIBRARY_DIRS}
)

find_package(Qt4 REQUIRED)

set(MIDASDesktopUI_SRCS
  GUI/MidasApplication.cxx
  GUI/MIDASDesktopUI.cxx
  GUI/GUILogger.cxx
)

set(MIDASDesktopUI_SRCS ${MIDASDesktopUI_SRCS} MIDASDesktop.rc)

set(MIDASDesktopUI_UI_SRCS
  GUI/MIDASDesktopUI.ui
)

set(MIDASDesktopUI_RCCS MIDASDesktopIcons.qrc)

set(MIDASDesktopUI_MOC_SRCS
  GUI/MidasApplication.h
  GUI/MIDASDesktopUI.h 
  GUI/GUILogger.h
)

QT4_WRAP_CPP(MIDASDesktopUI_SRCS ${MIDASDesktopUI_MOC_SRCS})
QT4_WRAP_UI(MIDASDesktopUI_UI_CXX ${MIDASDesktopUI_UI_SRCS})
QT4_ADD_RESOURCES(MIDASDesktopUI_SRCS ${MIDASDesktopUI_RCCS})

# MIDASDesktop Executable
if(WIN32)
  add_executable(MIDASDesktop WIN32 ${MIDASDesktopUI_SRCS} ${MIDASDesktopUI_UI_CXX} main.cxx MIDASDesktop.rc)
else(WIN32)
  add_executable(MIDASDesktop ${MIDASDesktopUI_SRCS} ${MIDASDesktopUI_UI_CXX} main.cxx)
endif(WIN32)

target_link_libraries(MIDASDesktop
  ${CTKDataManagementMIDASClientWidgets_LIBRARIES}
  ${CTKDataManagementMIDASClientCore_LIBRARIES}
  ${QT_QTMAIN_LIBRARY}
)

if(WIN32)
  target_link_libraries (MIDASDesktop
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
    target_link_libraries(MIDASDesktop ${X11_LIBRARIES})
    
    if(XFT_LIBRARY)
      target_link_libraries(MIDASDesktop ${XFT_LIBRARY})
    endif(XFT_LIBRARY)
    
    if(GLIB_LIBRARY)
      target_link_libraries(MIDASDesktop ${GLIB_LIBRARY})
    endif(GLIB_LIBRARY)
    
    if(GTHREAD_LIBRARY)
      target_link_libraries(MIDASDesktop ${GTHREAD_LIBRARY})
    endif(GTHREAD_LIBRARY)

    if(XT_LIBRARY)
      target_link_libraries(MIDASDesktop ${XT_LIBRARY})
    endif(XT_LIBRARY)

    if(XI_LIBRARY)
      target_link_libraries(MIDASDesktop ${XI_LIBRARY})
    endif(XI_LIBRARY)
    
    if(XINERAMA_LIBRARY)
      target_link_libraries(MIDASDesktop ${XINERAMA_LIBRARY})
    endif(XINERAMA_LIBRARY)

    if(XRANDR_LIBRARY)
      target_link_libraries(MIDASDesktop ${XRANDR_LIBRARY})
    endif(XRANDR_LIBRARY)
    
    if(XCURSOR_LIBRARY)
      target_link_libraries(MIDASDesktop ${XCURSOR_LIBRARY})
    endif(XCURSOR_LIBRARY)
      
    if(CRYPT_LIBRARY)
      target_link_libraries(MIDASDesktop ${CRYPT_LIBRARY})
    endif(CRYPT_LIBRARY)
  endif(NOT APPLE)
endif(UNIX)

if ( APPLE ) 
    # Link with frameworks required for static Qt
    find_library ( CARBON_LIBRARY Carbon )
    find_library ( APLICATIONSERVICES_LIBRARY ApplicationServices )
    find_library ( APPKIT_LIBRARY AppKit )
    target_link_libraries(MIDASDesktop
      ${APPKIT_LIBRARY}
      ${CARBON_LIBRARY}
      ${APPLICATIONSERVICES_LIBRARY}
    )
    set(QT_QTGUI_MENU_NIB
        "/Library/Frameworks/QtGui.framework/Versions/Current/Resources/qt_menu.nib"
        CACHE PATH "Location of the qt_menu.nib resource to be packaged for install")
    install(DIRECTORY ${QT_QTGUI_MENU_NIB} DESTINATION bin COMPONENT Runtime)
endif( APPLE )

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
        if(IS_DIRECTORY "${QT_${qtlib}_LIBRARY_RELEASE}")
          install(DIRECTORY "${QT_${qtlib}_LIBRARY_RELEASE}" 
            DESTINATION ${QT_INSTALL_LIB_DIR} COMPONENT Runtime)
        else()
          install(FILES "${QT_${qtlib}_LIBRARY_RELEASE}" 
            DESTINATION ${QT_INSTALL_LIB_DIR} COMPONENT Runtime)
        endif()
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
