project(MIDASRest)

cmake_minimum_required(VERSION 2.4)
if(COMMAND cmake_policy)
  cmake_policy(SET CMP0003 NEW)
endif(COMMAND cmake_policy)

# --------------------------------------------------------------------------
# Sources
set(MIDASRest_SRCS

  mdoBitstream.h
  mdoBitstream.cxx
  mdoCollection.h
  mdoCollection.cxx
  mdoCommunity.h
  mdoCommunity.cxx
  mdoDicom.h
  mdoDicom.cxx
  mdoItem.h
  mdoItem.cxx
  mdoProxy.h
  mdoProxy.cxx
  mdoProxyInterface.h
  mdoProxyInterface.cxx

  mwsBitstream.h
  mwsBitstream.cxx
  mwsCollection.h
  mwsCollection.cxx
  mwsCommunity.h
  mwsCommunity.cxx
  mwsDicom.h
  mwsDicom.cxx
  mwsItem.h
  mwsItem.cxx
  mwsRestAPI.h
  mwsRestAPI.cxx
  mwsRestXMLParser.h
  mwsRestXMLParser.cxx
  mwsWebAPI.h
  mwsWebAPI.cxx
  mwsSettings.h
  mwsSettings.cxx

  metaCommand.h
  metaCommand.cxx
 
  mdsSQLiteDatabase.h
  mdsSQLiteDatabase.cxx
  mdsItem.h
  mdsItem.cxx
  mdsBitstream.h
  mdsBitstream.cxx
  mdsObject.h
)

#-----------------------------------------------------------------------------
#
add_library(MIDAS ${MIDASRest_SRCS}) 
target_link_libraries(MIDAS cmcurl ITKEXPAT sqlite kwsys)


