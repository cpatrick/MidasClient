PROJECT(DionysusUtils)
cmake_minimum_required(VERSION 2.6)

SET(DionysusUtils_SRCS
  kwCrypt.h
  kwCrypt.cxx
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
ADD_LIBRARY(DionysusUtils ${DionysusUtils_SRCS})

