/*=========================================================================
  Copyright (c) 2002 Insight Consortium. All rights reserved.
  See ITKCopyright.txt or http://www.itk.org/HTML/Copyright.htm for details.

     This software is distributed WITHOUT ANY WARRANTY; without even 
     the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR 
     PURPOSE.  See the above copyright notices for more information.

=========================================================================*/
// MIDAScli is a command line application to interact with MIDAS as a command line

#include "mdoItem.h"
#include "mdoCommunity.h"
#include "mdoProxy.h"
#include "mwsItem.h"
#include "mwsCommunity.h"
#include "mdsSQLiteDatabase.h"
#include "mdsItem.h"

#include <iostream>

void testDownload()
{
  // Setup the WebAPI 
  mdo::Item item;
    
  // Add a local cache
  mds::SQLiteDatabase database;
  database.Open("C:/Julien/MidasDesktop/midasdesktop.db");
  mds::Item itemLocalDatabase;
  itemLocalDatabase.SetIsCache(true);
  itemLocalDatabase.SetDatabase(&database);
  itemLocalDatabase.SetName("MIDAS Desktop");
  item.GetProxy()->AddInterface(&itemLocalDatabase);

  // Then look on MIDAS
  mws::WebAPI api;
  api.SetServerUrl("http://localhost/MIDAS/MidasServer/MidasCake/api/rest/");
  mws::Item itemRest;
  itemRest.SetWebAPI(&api);
  itemRest.SetName("MIDAS Remote");
  item.GetProxy()->AddInterface(&itemRest);


  //item.SetId(72);
  //item.Load();
  //item.SetVerbose(true);
  std::string filename = item.DownloadBitstream("4","C:/Julien/test1234.jpg");
  std::cout << filename.c_str() << std::endl;
  item.Print(std::cout);

  database.Close();
}

/** Test upload project XML */
void testProjectUploadXML()
{
  mws::WebAPI api;
  api.SetVerbose(false);
  api.SetServerUrl("http://localhost/MIDAS/MidasServer/MidasCake/api/rest/");
  api.LoginWithPassword("testapp","admin@localhost","200ceb26807d6bf99fd6f4f0d1ca54d4");
  
  // Lazy for now :)
  api.UploadFile("project.upload.project/?projectid=2",
                 "C:/Julien/test.xml");
  /*mws::Project projectRest;
  projectRest.SetWebAPI(&api);
 
  mdo::Community community;
  community.GetProxy()->AddInterface(&communityRest);
  
  community.SetId(0);
  community.LoadTree();
  community.Print(std::cout);
  */
}

int main(int argc, char **argv)
{    
  testProjectUploadXML();
  
  // Test for the rest api
  /*RestAPI *rest = new RestAPI();
  rest->SetServerUrl("http://localhost/MIDAS/MidasServer/MidasCake/");
  rest->Initialize();
  rest->SetVerbose(false);
  rest->Execute("midas/api/rest/midas.item.get?id=1");
  std::cout << rest->GetOutputBuffer().c_str() << std::endl;
  rest->Finalize();
  delete rest;
  */
  
  /*
  mws::WebAPI midasAPI;
  midasAPI.SetServerUrl("http://localhost/MIDAS/MidasServer/MidasCake/");
  midasAPI.SetVerbose(false);
  bool success = midasAPI.Execute("midas/api/rest/midas.item.get?id=1");
  if(!success)
    {
    std::cout << midasAPI.GetErrorCode() << " : " << midasAPI.GetErrorMessage() << std::endl;
    }*/
  
  

  /*
  mws::Community communityRest;
  communityRest.SetWebAPI(&api);
 
  mdo::Community community;
  community.GetProxy()->AddInterface(&communityRest);
  
  community.SetId(0);
  community.LoadTree();
  community.Print(std::cout);
  */
  
  //item.SetVerbose(true);
  //item.Login("MidasClient","admin@localhost","utMJcaIwlMWTYxgPI93NDQ9LmkTcXO7413bPFeVt");
  //item.DownloadBitstream(9,"C:/Julien/test1234.jpg");
  //unsigned int bitstreamid = item.UploadBitstream(9,"C:/Julien/KWiK.txt");
  //std::cout << "Bitstreamid = " << bitstreamid << std::endl;
  
  return 0;
 }
