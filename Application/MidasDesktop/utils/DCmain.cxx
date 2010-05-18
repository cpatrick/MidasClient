/*=========================================================================

  Program:   Dionysus
  Module:    DCmain.cxx
  Language:  C++
  Date:      $Date$
  Version:   $Revision$

  Copyright (c) Kitware Inc. All rights reserved.
  See Copyright.txt or http://www.Kitware.com/Copyright.htm for details.

     This software is distributed WITHOUT ANY WARRANTY; without even
     the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR
     PURPOSE.  See the above copyright notices for more information.

=========================================================================*/

#include "DSpaceExporter.h"
#include "DSpaceImporter.h"
#include "MidasClientComputerID.h"
#include "MidasClientLicense.h"
#include "kwCrypt.h"
#include "DYSInit.h"
#include "metaCommand.h"
#include "metaOutput.h"
#include <kwsys/Glob.hxx>
#include <kwsys/Directory.hxx>
#include <kwsys/SystemTools.hxx>

int main(int argc,char* argv[])
{
  DSpaceExporter exporter;
  DSpaceImporter importer;
  exporter.SetImporter(&importer);

  DYSInit* init = new DYSInit;

  bool DemoVersion = true;

  MetaCommand command;
  command.SetName("DionysusCommand");
  command.SetVersion("1.0");
  command.SetAuthor("Kitware Inc");
  command.SetDescription("Lanch Dionysus");

  command.SetOption("setting","Setting",false,
                    "Parameters to be connect to the database");

  command.SetOption("config","Config",false,"specify xml path");
  command.AddOptionField("config","configxml", MetaCommand::STRING,false);

  command.SetOption("newEmail","NewEmail", false,
                    "New email to be connected to the Database");
  command.AddOptionField("newEmail","email", MetaCommand::STRING,false);

  command.SetOption("newPassword","NewPassword",false,
                    "New password to be connected to the Database");
  command.AddOptionField("newPassword","password", MetaCommand::STRING,false);

  command.SetOption("newDBName","NewDBName",false,"Name of the Database");
  command.AddOptionField("newDBName","dbname", MetaCommand::STRING,false);

  command.SetOption("newDBHost","NewDBHost",false, "Host of the Database");
  command.AddOptionField("newDBHost","dbhost",MetaCommand::STRING,false);

  command.SetOption("newDBPassword","NewDBPassword",false,"DB Password");
  command.AddOptionField("newDBPassword","dbpassword",
                          MetaCommand::STRING,false);

  command.SetOption("newMidasURL","NewMidasURL",false, "URL of Midas");
  command.AddOptionField("newMidasURL","midasurl",MetaCommand::STRING,false);


  command.SetOption("listall","ListAll",false,"List all the you can access");

  command.SetOption("listcommunity","ListCommunity",false,
                    "List all the community you can access");

  command.SetOption("listcollection","ListCollection",false,
                    "List all the collection you can access");
  command.AddOptionField("listcollection","communityid",
                          MetaCommand::STRING,false);
  
  command.SetOption("listitem","ListItem",false,
                    "List all the items from a collection");
  command.AddOptionField("listitem","collectionid",
                          MetaCommand::STRING,false);

  command.SetOption("listbitstream","ListBitstream",false,
                    "List all the bitstreams from an item");
  command.AddOptionField("listbitstream","itemid",
                          MetaCommand::STRING,false);

  command.SetOption("createcommunity","CreateCommunity",false,
                    "Create a new community on the Database");
  command.AddOptionField("createcommunity","parentid",
                          MetaCommand::INT,false);
  command.AddOptionField("createcommunity","communityname",
                          MetaCommand::STRING,false);

  command.SetOption("deletecommunity","DeleteCommunity",false,
                    "Delete a community on the Database");
  command.AddOptionField("deletecommunity","communityid",
                          MetaCommand::STRING,false);

  command.SetOption("createcollection","CreateCollection",false,
                    "Create a new collection on the Database");
  command.AddOptionField("createcollection","parentcommunityid",
                          MetaCommand::INT,false);
  command.AddOptionField("createcollection","collectionname",
                          MetaCommand::STRING,false);
  command.AddOptionField("createcollection","createdefaultgroup",
                          MetaCommand::BOOL,false);

  command.SetOption("deletecollection","DeleteCollection",false,
                    "Delete a collection on the Database");
  command.AddOptionField("deletecollection","collectionid",
                          MetaCommand::STRING,false);

  command.SetOption("createitem","CreateItem",false,
                    "Create a new item on the Database");
  command.AddOptionField("createitem","parentcollectionid",
                          MetaCommand::INT,false);
  command.AddOptionField("createitem","itemtitle",
                          MetaCommand::STRING,false);
  command.AddOptionField("createitem","lastname",
                          MetaCommand::STRING,false);
  command.AddOptionField("createitem","firstname",
                          MetaCommand::STRING,false);

  command.SetOption("uploadbitstream","UploadBitstream",false,
                    "Add bitstream to an item");
  command.AddOptionField("uploadbitstream","itemid",
                          MetaCommand::STRING,false);
  command.AddOptionField("uploadbitstream","directory_file",
                          MetaCommand::STRING,false);
  
  command.SetOption("downloaditem","DownloadItem",false,
                    "Download bitstreams from an item");
  command.AddOptionField("downloaditem","itemid",
                          MetaCommand::STRING,false);
  command.AddOptionField("downloaditem","directory",
                          MetaCommand::STRING,false);

  command.SetOption("downloadbitstream","DownloadBitstream",false,
                    "Download bitstream");
  command.AddOptionField("downloadbitstream","currentitem",
                          MetaCommand::STRING,false);
  command.AddOptionField("downloadbitstream","bitstreamid",
                          MetaCommand::STRING,false);
  command.AddOptionField("downloadbitstream","directory",
                          MetaCommand::STRING,false);

  command.SetOption("deleteitem","DeleteItem",false,
                    "Delete an item");
  command.AddOptionField("deleteitem","itemid",
                          MetaCommand::STRING,false);

  command.SetOption("deletebitstream","DeleteBitstream",false,
                    "Delete a bitstream");
  command.AddOptionField("deletebitstream","currentitem",
                          MetaCommand::STRING,false);
  command.AddOptionField("deletebitstream","bitstreamid",
                          MetaCommand::STRING,false);

  command.SetOption("searchitem","Search",false,
                    "Search item corresponding to the request");
  command.AddOptionField("searchitem","request",
                          MetaCommand::STRING,false);

  MetaOutput output;
  output.SetMetaCommand(&command);

  if(!command.Parse(argc,argv))
    {
    return 1;
    }

  
  if(command.GetOptionWasSet("config"))
    {
    std::string configxml = command.GetValueAsString("config","configxml");
    std::cout << "Using  " << configxml.c_str() << std::endl;
    init->LoadFile(configxml.c_str());
    }
  else
    {
    init->LoadFile("MidasClient.xml");
    }

  MidasClientLicense License(init);

  std::string Email = command.GetValueAsString("newEmail","email");
  std::string Password = command.GetValueAsString("newPassword","password");
  std::string DBName = command.GetValueAsString("newDBName",
                                                "dbname");
  std::string DBHost = command.GetValueAsString("newDBHost",
                                                "dbhost");
  std::string DBPassword = command.GetValueAsString("newDBPassword",
                                                    "dbpassword");
  std::string MidasURL = command.GetValueAsString("newMidasURL","midasurl");

  std::string Communityid = command.GetValueAsString("listcollection","communityid");
  std::string Collectionid = command.GetValueAsString("listitem","collectionid");
  std::string Itemid = command.GetValueAsString("listbitstream","itemid");

  std::string CreateCommunityName = command.GetValueAsString("createcommunity","communityname");
  unsigned int SetParentid = command.GetValueAsInt("createcommunity","parentid");
  std::string DeleteCommunityId = command.GetValueAsString("deletecommunity","communityid");

  std::string CreateCollectionName = command.GetValueAsString("createcollection","collectionname");
  unsigned int SetCommunityid = command.GetValueAsInt("createcollection","parentcommunityid");
  bool CreateDefaultGroup = command.GetValueAsInt("createcollection","createdefaultgroup");
  std::string DeleteCollectionId = command.GetValueAsString("deletecollection","collectionid");

  std::string ItemTitle = command.GetValueAsString("createitem","itemtitle");
  unsigned int SetCollectionid = command.GetValueAsInt("createitem","parentcollectionid");


  std::string UploadItem = command.GetValueAsString("uploadbitstream","itemid");
  std::string BitstreamDirectory = command.GetValueAsString("uploadbitstream","directory_file");
  std::string DownloadItem = command.GetValueAsString("downloaditem","itemid");
  std::string DownloadItemDirectory = command.GetValueAsString("downloaditem","directory");
  std::string DeleteItem = command.GetValueAsString("deleteitem","itemid");
  std::string DeleteBitstreamItem = command.GetValueAsString("deletebitstream","currentitem");
  std::string DeleteBitstream = command.GetValueAsString("deletebitstream","bitstreamid");
  std::string CurrentItem = command.GetValueAsString("downloadbitstream","currentitem");
  std::string DownloadBitstream = command.GetValueAsString("downloadbitstream","bitstreamid");
  std::string DownloadBitstreamDirectory = command.GetValueAsString("downloadbitstream","directory");

  std::string Request = command.GetValueAsString("searchitem","request");

  /*if(License.CanStart())
    {
    if(License.HasFullLicense()) // not a demo version
      {
      DemoVersion = false;
      std::cout<<"\n****************************************************"<<std::endl;
      std::cout<<"You are running the complete version of MidasClient."<<std::endl;
      std::cout<<"****************************************************\n"<<std::endl;
      }
    else if(License.GetDaysRemaining()<=0 || !(License.Checkdownloads() && License.Checkuploads()))
      {  
      std::cout<<"\n***************************************************************"<<std::endl;
      std::cout<<"You have arrived at the term of your evaluation of MidasClient."<<std::endl;
      std::cout<<"If you wish to buy MidasClient please refer to www.kitware.com."<<std::endl;
      std::cout<<"=> You have "<<License.GetDownloadLeft()<<" downloads, "<<License.GetUploadLeft();
      std::cout<<" uploads left and "<<License.GetDaysRemaining()<<" days remaining."<<std::endl;
      std::cout<<"***************************************************************"<<std::endl;
      return 0;
      }

    if(DemoVersion)
      {
      std::cout<<"\n****************************************************************"<<std::endl;
      std::cout<<"This is a demo version of MidasClient."<<std::endl;
      std::cout<<"Visit www.kitware.com/products/midas.html for more information."<<std::endl;
      std::cout<<"You have "<<License.GetDownloadLeft()<<" downloads, "<<License.GetUploadLeft();
      std::cout<<" uploads left and "<<License.GetDaysRemaining()<<" days remaining."<<std::endl;
      std::cout<<"****************************************************************\n"<<std::endl;
      }
    }

   else
    {
    bool agree_license = false;
    std::string agree="";

    std::string lastname="";
    std::string firstname="";
    std::string company="";
    std::string email1="";
    std::string email2="";

    std::cout<<"Last name : ";
    std::cin>>lastname;
    std::cout<<"First Name : ";
    std::cin>>firstname;
    std::cout<<"Company : ";
    std::cin>>company;
    std::cout<<"Email : ";
    std::cin>>email1;
    std::cout<<"Email : ";
    std::cin>>email2;

    while(!(email1 == email2))
      {
      std::cout<<"The two emails don't match !"<<std::endl;
      std::cout<<"Please retype your email :"<<std::endl;
      std::cout<<"Email :";
      std::cin>>email1;
      std::cout<<"Email :";
      std::cin>>email2;
      std::cout<<std::endl;
      }

    init->SetLastName(lastname.c_str());
    init->SetFirstName(firstname.c_str());
    init->SetAffiliation(company.c_str());
    init->SetUserEmail(email1.c_str());
    init->WriteFile(); // Write the ini file 

    // Display Disclaimer
    std::cout<<"\n\n***********************************************************"<<std::endl;
    std::cout<<"Disclaimer :"<<std::endl;
    std::cout<<"MIDAS Client Agreement"<<std::endl;
    std::cout<<"Version 1.0, June 1, 2007\n"<<std::endl;
 
    std::cout<<"Copyright (c) Kitware Inc. All rights reserved."<<std::endl;
    std::cout<<"http://www.Kitware.com/Copyright.htm for details."<<std::endl;

    std::cout<<"\nThis software is distributed WITHOUT ANY WARRANTY;without "<<std::endl;
    std::cout<<"even the implied warranty of MERCHANTABILITY or FITNESS FOR "<<std::endl;
    std::cout<<"A PARTICULAR PURPOSE.  See the above copyright notices for "<<std::endl;
    std::cout<<"more information."<<std::endl;
    std::cout<<"***********************************************************\n"<<std::endl;

    std::cout<<"Are you agree with the disclaimer ? (yes / no) : ";
    std::cin>>agree;

    if(agree == "yes" || agree == "y" || agree == "YES" || agree == "Y")
      {
      agree_license = true;
      }

    if(agree_license)
      {
      if(License.NewUser())
        {
        if(License.HasFullLicense()) // not a demo version
          {
          DemoVersion = false;
          }
        else
          {
          License.CanStart();
          License.Checkdownloads();
          License.Checkuploads();

          std::cout<<"\n****************************************************************"<<std::endl;
          std::cout<<"This is a demo version of MidasClient."<<std::endl;
          std::cout<<"Visit www.kitware.com/products/midas.html for more information."<<std::endl;
          std::cout<<"You have "<<License.GetDownloadLeft()<<" downloads, "<<License.GetUploadLeft();
          std::cout<<" uploads left and "<<License.GetDaysRemaining()<<" days remaining."<<std::endl;
          std::cout<<"****************************************************************\n"<<std::endl;
          }
        }
      else
        {
        std::cout<<"You are already registered on an other computer"<<std::endl;
        return 0;
        }
      }
    else
      {
      std::cout<<"You must be agree with the disclaimer"<<std::endl;
      return 0;
      }
  }*/

  //Display settings
  if(command.GetOptionWasSet("setting"))
    {
    std::cout<<"Email : "<<init->GetEmail()<<std::endl;
    std::cout<<"Password : ******" <<std::endl;
    std::cout<<"Database Name : " <<init->GetDatabaseName()<<std::endl;
    std::cout<<"Database Password: ******" <<std::endl;
    std::cout<<"Database Host : " <<init->GetDatabaseHost()<<std::endl;
    std::cout<<"Midas URL : " <<init->GetMidasURL()<<std::endl;
    std::cout<<"PublicationDB URL : "<<init->GetPublicationDBURL()<<std::endl;
    }


  //change email
  if(command.GetOptionWasSet("newEmail") && Email.size() != 0)
    {
    init->SetEmail(Email.c_str());
    std::cout<<"Email changed to : "<<Email.c_str()<<std::endl;
    init->WriteFile("MidasClient.xml");
    }

  //change password
  if(command.GetOptionWasSet("newPassword") && Password.size() != 0)
    {
    kwCrypt crypt;
    std::string cryptedpass = crypt.Crypt(Password.c_str());
    init->SetPassword(cryptedpass.c_str());
    std::cout<<"Password changed to : "<<cryptedpass.c_str()<<std::endl;
    init->WriteFile("MidasClient.xml");
    }

  //change database name
  if(command.GetOptionWasSet("newDBName") && DBName.size() != 0)
    {
    init->SetDatabaseName(DBName.c_str());
    std::cout<<"Database name changed to : "<<DBName.c_str()<<std::endl;
    init->WriteFile("MidasClient.xml");
    }
  //change database host
  if(command.GetOptionWasSet("newDBHost") && DBHost.size() != 0)
    {
    init->SetDatabaseHost(DBHost.c_str());
    std::cout<<"Database host changed to : "<<DBHost.c_str()<<std::endl;
    init->WriteFile("MidasClient.xml");
    }

  //change database password
  if(command.GetOptionWasSet("newDBPassword") && DBPassword.size() != 0)
    {
    kwCrypt crypt;
    std::string cryptedpass = crypt.Crypt(DBPassword.c_str());
    init->SetDatabasePassword(cryptedpass.c_str());
    std::cout<<"Database password changed to : ";
    std::cout<<cryptedpass.c_str()<<std::endl;
    init->WriteFile("MidasClient.xml");
    }

  //change database URL
  if(command.GetOptionWasSet("newMidasURL") && MidasURL.size() != 0)
    {
    init->SetMidasURL(MidasURL.c_str());
    std::cout<<"Midas URL changed to : "<<MidasURL.c_str()<<std::endl;
    init->WriteFile("MidasClient.xml");
    }


  //connection to the database
  importer.SetDatabaseName(init->GetDatabaseName().c_str());
  importer.SeUserName(init->GetDatabaseUser().c_str());
  kwCrypt crypt;
  std::string decryptedDB = crypt.Decrypt(init->GetDatabasePassword().c_str());
  importer.SetPassword(decryptedDB.c_str());
  importer.SetPort(init->GetDatabasePort().c_str());
  importer.SetHostname(init->GetDatabaseHost().c_str());
  std::string decrypted = crypt.Decrypt(init->GetPassword().c_str());

  if(importer.Connect(init->GetEmail().c_str(),decrypted.c_str()))
    {
    //listall
    if(command.GetOptionWasSet("listall"))
      {
    importer.PrintTree();
      }
    
  //listcommunity
    if(command.GetOptionWasSet("listcommunity"))
      {
      std::string communityName;
      std::vector <std::string> result = importer.GetCommunity();
      for(unsigned int i= 0;i<result.size();i++)
        {
        if (importer.CommunityPolicy(result[i]))
          {
          communityName = importer.GetCommunityFromCommunity(result[i]);
          std::cout<<importer.GetCommunityName(result[i])<<" [ID :"<<result[i];
          std::cout<<"]  {"<<communityName;
          if(communityName != "ROOT")
            {
            std::cout<<"  [ID :";
            std::cout<<importer.GetCommunityIdFromName(communityName)<<"]";
            }
          std::cout<<"}"<<std::endl;
          }
        }
      }

    //listcollection
    if(command.GetOptionWasSet("listcollection"))
      {
      std::vector <std::string> result = importer.GetCollection(Communityid);
      std::cout<<importer.GetCommunityName(Communityid)<<" [ID :";
      std::cout<<Communityid<<"]"<<std::endl;
      for(unsigned int i= 0;i<result.size();i++)
        {
        if (importer.CollectionPolicy(result[i]))
          {
          std::cout<<"  -> "<<importer.GetCollectionName(result[i])<<" ";
          std::cout<<" [ID :"<<result[i]<<"]"<<std::endl;
          }
        else
          {
          std::cout<<"Access Denied : You are not allowed to access to this collection"<<std::endl;
          }
        }
      }

    //listitem
    if(command.GetOptionWasSet("listitem"))
      {
      std::vector <std::string> result = importer.GetItemFromCollection(Collectionid);
      std::cout<<importer.GetCollectionName(Collectionid)<<" [ID :";
      std::cout<<Collectionid<<"]"<<std::endl;
      for(unsigned int i= 0;i<result.size();i++)
        {
        if(importer.ItemPolicy(result[i]))
          {
          std::cout<<"  -> "<<importer.GetTitle(result[i])<<" ";
          std::cout<<" [ID :"<<result[i]<<"]"<<std::endl;
          }
        else
          {
          std::cout<<"Access Denied : You are not allowed to access to this item"<<std::endl;
          }
        }
      }

    //listbistream
    if(command.GetOptionWasSet("listbitstream"))
      {
      if(importer.ItemPolicy(Itemid))
        {
        std::vector <std::string> bitstreamid=
                                    importer.BitstreamId(Itemid);
        std::vector <std::string> bitstreamname=
                                    importer.BitstreamName(Itemid);
        std::cout<<importer.GetTitle(Itemid)<<" ["<<Itemid<<"]"<<std::endl;
        for(unsigned int i=0;i<bitstreamname.size();i++)
          {
          std::cout<<"   -> "<<bitstreamname[i]<<
            " ["<<bitstreamid[i]<<"]"<<std::endl;
          }
        }
      else
        {
        std::cout<<"Access Denied : You are not allowed to access to these bitstreams"<<std::endl;
        }
      }
    
    //createcommunity
    if(command.GetOptionWasSet("createcommunity") &&
    CreateCommunityName.size() != 0)
      {
      int communityid = importer.CreateCommunity(SetParentid, CreateCommunityName, "", "");
      output.AddIntField("communityid","Id of the created community",communityid); 
      output.Write();
      std::cout << "New Community created: "<<CreateCommunityName<<" [ID :"<<communityid<<"]"<<std::endl;
      }

    //createcollection
    if(command.GetOptionWasSet("createcollection") &&
    CreateCollectionName.size() != 0 && SetCommunityid != 0)
      {
      int collectionid = importer.CreateCollection(SetCommunityid, CreateCollectionName, "", "", true);
      output.AddIntField("collectionid","Id of the created collection",collectionid); 
      output.Write();
      std::cout<<"New Collection created : "<<CreateCollectionName;
      std::cout<<" [ID :"<<collectionid<<"]"<<std::endl;
      }
   
    //deletecommunity
    if(command.GetOptionWasSet("deletecommunity") &&
    DeleteCommunityId.size() != 0)
      {
      if(importer.CommunityPolicy(DeleteCommunityId))
        {
        std::string CommunityName =
        importer.GetCommunityName(DeleteCommunityId);
        
        importer.DeleteCommunity(DeleteCommunityId);
        std::cout<<"Community deleted : " <<CommunityName;
        std::cout<<" [ID :"<<DeleteCommunityId<<"]"<<std::endl;
        }
      }

    //deletecollection
    if(command.GetOptionWasSet("deletecollection") &&
    DeleteCollectionId.size() != 0)
      {
      if(importer.CollectionPolicy(DeleteCollectionId))
        {  
        std::string CollectionName =
        importer.GetCollectionName(DeleteCollectionId);
        
        importer.DeleteCollection(DeleteCollectionId);
        std::cout <<"Collection deleted : "<<CollectionName;
        std::cout<<" [ID :"<<DeleteCollectionId<<"]"<<std::endl;
        }
      }

    //createitem
    if(command.GetOptionWasSet("createitem") && ItemTitle.size() != 0
    && SetCollectionid != 0)
      {   
      std::string LastName = 
        command.GetValueAsString("createitem","lastname");
      std::string FirstName = 
        command.GetValueAsString("createitem","firstname");
      std::string decrypted = crypt.Decrypt(init->
                                             GetPassword().c_str());
      exporter.SetEmail(init->GetEmail().c_str());
      importer.SetTitle(ItemTitle);
      exporter.SetPassword(decrypted.c_str());
      if(LastName.size() != 0 && FirstName.size() != 0)
        {
        importer.SetAuthors(FirstName.c_str(),LastName.c_str(),1,0);
        }
      
      #ifdef USE_PublicationDB
      exporter.SetUploadUrl(init->GetPublicationDBURL().c_str());
      exporter.CreatePBItem(SetCollectionid);
      std::string itemid = importer.GetItemId(ItemTitle);
      #else
      exporter.SetUploadUrl(init->GetMidasURL().c_str());
      int ID=importer.CreateItem(SetCollectionid);
      std::string itemid = importer.GetItemId(ItemTitle);
      importer.UpdateItemPolicy(itemid.c_str());
      #endif

      output.AddIntField("itemid","Id of the created item",ID); 
      output.Write();
      std::cout << "New Item created : " << itemid;
      std::cout<<" " << ItemTitle << std::endl;
      }

    //upload item
      if(command.GetOptionWasSet("uploadbitstream") &&
      UploadItem.size() != 0 && BitstreamDirectory.size() != 0)
        {
        if(importer.ItemPolicy(UploadItem))
          {
          exporter.SetEmail(init->GetEmail().c_str());
          exporter.SetPassword(decrypted.c_str());
          
          #ifdef USE_PublicationDB
          exporter.SetUploadUrl(init->GetPublicationDBURL().c_str());
          #else
          exporter.SetUploadUrl(init->GetMidasURL().c_str());
          #endif

          kwsys::Directory dir;
          if(dir.Load(BitstreamDirectory.c_str()))
            {
            kwsys::Glob glob;
            std::string globoption = BitstreamDirectory;
            globoption += "/*.*";
            glob.FindFiles(globoption.c_str());
            std::vector<std::string> bitstream = glob.GetFiles();

            for(unsigned int i=0;i<bitstream.size();i++)
              {
              std::cout<<"Uploading file "<<i+1<<" on ";
              std::cout<<bitstream.size()<<std::endl;
              exporter.SetFilename(bitstream[i].c_str());
              #ifdef USE_PublicationDB
              exporter.UploadPBBitstream(UploadItem.c_str());
              #else
              exporter.UploadBitstream(UploadItem.c_str());
              #endif
              if(DemoVersion)
                {
                License.NewUpload();
                }
              }
            }
          else if(kwsys::SystemTools::FileExists(BitstreamDirectory.c_str()))
            {
            std::cout<<"Uploading file : ";
            std::cout<<BitstreamDirectory.c_str()<<std::endl;
            exporter.SetFilename(BitstreamDirectory.c_str());
            #ifdef USE_PublicationDB
            exporter.UploadPBBitstream(UploadItem.c_str());
            #else
            exporter.UploadBitstream(UploadItem.c_str());
            #endif
            if(DemoVersion)
              {
              License.NewUpload();
              }
            }
          else
            {
            std::cout<<"wrong filename"<<std::endl;
            }
          }
        else
          {
          std::cout<<"access denied"<<std::endl;
          }
        }

    //download item
      if(command.GetOptionWasSet("downloaditem") && DownloadItem.size() != 0
      && DownloadItemDirectory.size() != 0)
        {
        if(importer.ItemPolicy(DownloadItem))
          {
          std::vector <std::string> bitstreamid=
                                      importer.BitstreamId(DownloadItem);
          std::vector <std::string> bitstreamname=
                                      importer.BitstreamName(DownloadItem);

          exporter.SetDownloadUrl(init->GetMidasURL().c_str());
          exporter.SetEmail(init->GetEmail().c_str());
          exporter.SetPassword(decrypted.c_str());
          for(unsigned int i=0;i<bitstreamname.size();i++)
            {
            std::cout<<"Downloading file "<<i+1<<" on "<<
            bitstreamname.size()<<std::endl;
            std::string URL = "";
            for(unsigned int j=21 ;
            j<importer.GetHandle(DownloadItem).size() ; j++)
              {
              URL += importer.GetHandle(DownloadItem).c_str()[j];
              }
            URL += "/";
            URL += bitstreamid[i];
            
            kwsys::Directory dir;
            if(!dir.Load(DownloadItemDirectory.c_str()))
              {
              kwsys::SystemTools::MakeDirectory(DownloadItemDirectory.c_str());
              }
            std::string filename = DownloadItemDirectory;
            filename += "/";
            filename += bitstreamname[i]; 
            exporter.DownloadBitstream(URL,filename);

            if(DemoVersion)
              {
              License.NewDownload();
              }
            }
          }
        else
          {
          std::cout<<"access denied"<<std::endl;
          }
        }

    //delete item
    if(command.GetOptionWasSet("deleteitem") && DeleteItem.size() != 0)
      {
      if(importer.ItemPolicy(DeleteItem))
        {
        importer.DeleteItem(DeleteItem);
        std::cout << "Item deleted: " << DeleteItem << std::endl;
        }
      else
        {
        std::cout<<"access denied"<<std::endl;
        }
      }

    //delete bitstream
    if(command.GetOptionWasSet("deletebitstream") &&
    DeleteBitstreamItem.size() != 0 && DeleteBitstream.size() != 0)
      {
      if(importer.ItemPolicy(DeleteBitstream))
        {
        std::string bundle =
          importer.GetBundleFromBitstream(DeleteBitstream);
          
        importer.DeleteBitstream(DeleteBitstreamItem,bundle,DeleteBitstream);
        std::cout << "Bitstream deleted: " << DeleteBitstream << std::endl;
        }
      else
        {
        std::cout<<"access denied"<<std::endl;
        }
      }

    //download bitstream
    if(command.GetOptionWasSet("downloadbitstream") && CurrentItem.size() != 0
    && DownloadBitstream.size() != 0 && DownloadBitstreamDirectory.size() != 0)
      {
      if(importer.ItemPolicy(CurrentItem))
        {
        std::string bitstreamname =
        importer.BitstreamNameFromId(DownloadBitstream);
        
        kwsys::Directory dir;
        if(!dir.Load(DownloadBitstreamDirectory.c_str()))
            {
            kwsys::SystemTools::
            MakeDirectory(DownloadBitstreamDirectory.c_str());
            }

        std::cout<<"Downloading file : "<<bitstreamname<<std::endl;
        std::string URL = "";
        
        exporter.SetDownloadUrl(init->GetMidasURL().c_str());
        exporter.SetEmail(init->GetEmail().c_str());
        exporter.SetPassword(decrypted.c_str());

        for(unsigned int j=21;j<importer.GetHandle(CurrentItem).size();j++)
          {
          URL += importer.GetHandle(CurrentItem).c_str()[j];
          }
        URL += "/";
        URL += DownloadBitstream;
        
        std::string filename = DownloadBitstreamDirectory;
        filename += "/";
        filename += bitstreamname; 
        exporter.DownloadBitstream(URL,filename);

        if(DemoVersion)
          {
          License.NewDownload();
          }
        }
      else
        {
        std::cout<<"access denied"<<std::endl;
        }
      }

    //search items
    if(command.GetOptionWasSet("searchitem") && Request.size() != 0)
      {
      int option[8] = {0,1,1,1,1,1,1,1};
      std::vector <std::string> result = importer.Search(Request,option);
      if(result.size()==0)
        {
        std::cout<<"No results for that request..."<<std::endl;
        }

      for(unsigned int i= 0;i<result.size();i++)
        {
        if (importer.ItemPolicy(result[i]))
          {
          bool check = false;
          for(unsigned int j=0;j<i;j++)
            {
            if(result[j] == result[i])
              {
              check = true;
              }
            }
          if(check == false)
            {
            std::cout<<importer.GetTitle(result[i])<<" [ID : ";
            std::cout<<result[i]<<"]"<<std::endl;
            std::vector <std::string> collections = importer.
            GetCollectionFromItem(result[i]);
            for(unsigned int j= 0;j<collections.size();j++)
              {
              if(importer.CollectionPolicy(collections[j]))
                {
                std::cout<<"  in collection : ";
                std::cout<<importer.GetCollectionName(collections[j])<<" [ID :";
                std::cout<<collections[j]<<"]"<<std::endl;
                }
              }
            std::cout<<std::endl;
            }
          }
        }
      }
    }
  else
    {
    std::cout<<"cannot connect to the database"<<std::endl;
    }


  return 0;
}
