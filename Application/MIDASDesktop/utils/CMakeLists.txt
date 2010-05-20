PROJECT(MIDASDesktopUtils)
cmake_minimum_required(VERSION 2.6)

SET(MIDASDesktopUtils_SRCS
  Utils.h
#  DetailedException.h
#  DetailedException.cxx
  Printable.h
  metaOutput.h
  metaOutput.cxx
  md5.h
  md5.cxx
  Logger.cxx
)

# Create the base library
ADD_LIBRARY(MIDASDesktopUtils ${MIDASDesktopUtils_SRCS})

