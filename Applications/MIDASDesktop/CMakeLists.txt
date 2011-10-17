project(MIDASDesktop)
cmake_minimum_required(VERSION 2.6)

include(${MIDASClient_BINARY_DIR}/Libs/Core/MIDASClientCoreConfig.cmake)
include(${MIDASClient_BINARY_DIR}/Libs/Widgets/MIDASClientWidgetsConfig.cmake)

include_directories(
  ${MIDASDesktop_SOURCE_DIR}
  ${MIDASDesktop_SOURCE_DIR}/GUI
  ${MIDASDesktop_BINARY_DIR}
  ${MIDASClientCore_INCLUDE_DIRS}
  ${MIDASClientWidgets_INCLUDE_DIRS})
link_directories(
  ${MIDASClientCore_LIBRARY_DIRS}
  ${MIDASClientWidgets_LIBRARY_DIRS}
)

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
elseif(APPLE)
  add_executable(MIDASDesktop MACOSX_BUNDLE ${MIDASDesktopUI_SRCS} ${MIDASDesktopUI_UI_CXX} main.cxx)
  set_target_properties(MIDASDesktop PROPERTIES
    MACOSX_BUNDLE_INFO_STRING "MIDAS Desktop"
    MACOSX_BUNDLE_ICON_FILE "MIDAS_Desktop_LargeIcon.icns"
    )
else()
  add_executable(MIDASDesktop ${MIDASDesktopUI_SRCS} ${MIDASDesktopUI_UI_CXX} main.cxx)
endif()

target_link_libraries(MIDASDesktop
  ${MIDASClientWidgets_LIBRARIES}
  ${MIDASClientCore_LIBRARIES}
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

if(APPLE)
  # Link with frameworks required for static Qt
  find_library(CARBON_LIBRARY Carbon)
  find_library(APLICATIONSERVICES_LIBRARY ApplicationServices)
  find_library(APPKIT_LIBRARY AppKit)
  target_link_libraries(MIDASDesktop
    ${APPKIT_LIBRARY}
    ${CARBON_LIBRARY}
    ${APPLICATIONSERVICES_LIBRARY}
  )
  install(FILES "${MIDASDesktop_SOURCE_DIR}/icons/MIDAS_Desktop_LargeIcon.icns"
          DESTINATION "MIDASDesktop.app/Contents/Resources")
  install(CODE
    "execute_process(COMMAND macdeployqt \"${MIDASDesktop_BINARY_DIR}/MIDASDesktop.app\")
    ")
  install(TARGETS MIDASDesktop BUNDLE DESTINATION ".")
else()
  install(TARGETS MIDASDesktop DESTINATION bin COMPONENT Runtime)
endif()

# -----Install Qt Shared libraries on linux and windows-------------
if (DEFINED QT_QMAKE_EXECUTABLE)
  set(QTLIBLIST QTCORE QTGUI QTNETWORK QTSCRIPT)
  set(QT_INSTALL_LIB_DIR bin)

  foreach(qtlib ${QTLIBLIST})
    if(NOT APPLE AND UNIX)
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
