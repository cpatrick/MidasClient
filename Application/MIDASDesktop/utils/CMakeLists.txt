PROJECT(MIDASDesktopUtils)
cmake_minimum_required(VERSION 2.6)

SET(MIDASDesktopUtils_SRCS
  Utils.h
  Printable.h
  md5.h
  md5.cxx
  Logger.cxx
)

# Create the base library
ADD_LIBRARY(MIDASDesktopUtils ${MIDASDesktopUtils_SRCS})
