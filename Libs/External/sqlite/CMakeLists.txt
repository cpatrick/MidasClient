IF(COMMAND cmake_policy)
  cmake_policy(SET CMP0003 NEW)
ENDIF(COMMAND cmake_policy)

CMAKE_MINIMUM_REQUIRED(VERSION 2.4)

PROJECT(sqlite)

INCLUDE(${CMAKE_ROOT}/Modules/CMakeBackwardCompatibilityC.cmake)

SET(SQLite_SRCS
   sqlite3.c
)

IF (WIN32)
  ADD_LIBRARY( sqlite STATIC ${SQLite_SRCS} )
ELSE (WIN32)
  INCLUDE(CheckIncludeFiles)
  CHECK_INCLUDE_FILES("pthread.h"        HAVE_PTHREAD_H)
  IF(HAVE_PTHREAD_H AND CMAKE_USE_PTHREADS)
    ADD_DEFINITIONS(-DHAVE_PTHREAD_H)
  ENDIF(HAVE_PTHREAD_H AND CMAKE_USE_PTHREADS)
  ADD_LIBRARY( sqlite ${SQLite_SRCS} )
  IF (UNIX AND HAVE_PTHREAD_H AND CMAKE_USE_PTHREADS)
    TARGET_LINK_LIBRARIES(sqlite ${CMAKE_THREAD_LIBS})
  ENDIF (UNIX AND HAVE_PTHREAD_H AND CMAKE_USE_PTHREADS)
ENDIF (WIN32)

ADD_EXECUTABLE(sqlite3 shell.c)
TARGET_LINK_LIBRARIES(sqlite3 sqlite)

IF(UNIX)
  TARGET_LINK_LIBRARIES(sqlite3 dl)
ENDIF(UNIX)
