/*=========================================================================

  Copyright (c) Kitware Inc. All rights reserved.
  See Copyright.txt or http://www.Kitware.com/Copyright.htm for details.

     This software is distributed WITHOUT ANY WARRANTY; without even
     the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR
     PURPOSE.  See the above copyright notices for more information.

=========================================================================*/

#include "midasCLI.h"
#include "midasSynchronizer.h"
#include "midasDatabaseProxy.h"
#include "midasDotProgressReporter.h"
#include "midasStatus.h"
#include "midasStdOutLog.h"
#include "midasUtils.h"
#include "mdsObject.h"
#include "mdoObject.h"
#include "mdsCommunity.h"
#include "mdsCollection.h"
#include "mdsItem.h"
#include "mdsBitstream.h"


midasCLI::midasCLI()
{
  this->TempProfile = new midasAuthProfile;
  this->UseTempProfile = false;
  this->RootDir = "";
  this->ServerURL = "";
  this->Synchronizer = new midasSynchronizer();
  this->Database = kwsys::SystemTools::GetCurrentWorkingDirectory() + "/midas.db";
  this->Synchronizer->SetProgressReporter(
    reinterpret_cast<midasProgressReporter*>(
    new midasDotProgressReporter(30)));
  this->SetLog(new midasStdOutLog());
  this->Synchronizer->SetLog(this->GetLog());
  int time = static_cast<unsigned int>(kwsys::SystemTools::GetTime() * 1000);
  srand (time); //init random number generator 
}

midasCLI::~midasCLI()
{
  delete this->TempProfile;
  delete this->Log;
  delete this->Synchronizer;
  delete mws::WebAPI::Instance()->GetRestAPI();
}

//-------------------------------------------------------------------
int midasCLI::Perform(std::vector<std::string> args)
{
  if(args.size() == 0)
    {
    this->PrintUsage();
    return -1;
    }

  bool ok = false;

  for(unsigned i = 0; i < args.size(); i++)
    {
    if(args[i] == "add")
      {
      this->Synchronizer->SetDatabase(this->Database);
      std::vector<std::string> postOpArgs(args.begin() + i + 1, args.end());
      ok = this->ParseAdd(postOpArgs);
      break;
      }
    else if(args[i] == "clean")
      {
      this->Synchronizer->SetDatabase(this->Database);
      std::vector<std::string> postOpArgs(args.begin() + i + 1, args.end());
      ok = this->ParseClean(postOpArgs);
      break;
      }
    else if(args[i] == "clone")
      {
      this->Synchronizer->SetDatabase(this->Database);
      std::vector<std::string> postOpArgs(args.begin() + i + 1, args.end());
      ok = this->ParseClone(postOpArgs);
      break;
      }
    else if(args[i] == "create_profile")
      {
      this->Synchronizer->SetDatabase(this->Database);
      std::vector<std::string> postOpArgs(args.begin() + i + 1, args.end());
      return this->PerformCreateProfile(postOpArgs);
      }
    else if(args[i] == "delete")
      {
      this->Synchronizer->SetDatabase(this->Database);
      std::vector<std::string> postOpArgs(args.begin() + i + 1, args.end());
      return this->PerformDelete(postOpArgs);
      }
    else if(args[i] == "pull")
      {
      this->Synchronizer->SetDatabase(this->Database);
      std::vector<std::string> postOpArgs(args.begin() + i + 1, args.end());
      ok = this->ParsePull(postOpArgs);
      break;
      }
    else if(args[i] == "push")
      {
      this->Synchronizer->SetDatabase(this->Database);
      std::vector<std::string> postOpArgs(args.begin() + i + 1, args.end());
      ok = this->ParsePush(postOpArgs);
      break;
      }
    else if(args[i] == "set_metadata")
      {
      this->Synchronizer->SetDatabase(this->Database);
      std::vector<std::string> postOpArgs(args.begin() + i + 1, args.end());
      return this->PerformSetMetadata(postOpArgs);
      }
    else if(args[i] == "set_root_dir")
      {
      this->Synchronizer->SetDatabase(this->Database);
      std::vector<std::string> postOpArgs(args.begin() + i + 1, args.end());
      return this->SetRootDir(postOpArgs);
      }
    else if(args[i] == "status")
      {
      this->Synchronizer->SetDatabase(this->Database);
      std::vector<std::string> postOpArgs(args.begin() + i + 1, args.end());
      ok = this->ParseStatus(postOpArgs);
      break;
      }
    else if(args[i] == "upload")
      {
      this->Synchronizer->SetDatabase(this->Database);
      std::vector<std::string> postOpArgs(args.begin() + i + 1, args.end());
      ok = this->ParseUpload(postOpArgs);
      break;
      }
    else if(args[i] == "--database" && i + 1 < args.size())
      {
      i++;
      this->Database = args[i];
      }
    else if(args[i] == "--profile" && i + 1 < args.size())
      {
      i++;
      this->Synchronizer->GetAuthenticator()->SetProfile(args[i]);
      }
    else if(args[i] == "--url" && i + 1 < args.size())
      {
      i++;
      this->UseTempProfile = true;
      this->TempProfile->Url = args[i];
      this->ServerURL = args[i];
      }
    else if(args[i] == "--email" && i + 1 < args.size())
      {
      i++;
      this->UseTempProfile = true;
      this->TempProfile->User = args[i];
      }
    else if(args[i] == "--app-name" && i + 1 < args.size())
      {
      i++;
      this->UseTempProfile = true;
      this->TempProfile->AppName = args[i];
      }
    else if(args[i] == "--api-key" && i + 1 < args.size())
      {
      i++;
      this->UseTempProfile = true;
      this->TempProfile->ApiKey = args[i];
      }
    else if(args[i] == "--help")
      {
      if(i + 1 < args.size())
        {
        i++;
        this->PrintCommandHelp(args[i]);
        }
      else
        {
        this->PrintUsage();
        }
      return 0;
      }
    else
      {
      this->PrintUsage();
      return -1;
      }
    }

  if(ok)
    {
    int rc = this->RunSynchronizer();

    if(this->UseTempProfile)
      {
      this->Synchronizer->GetDatabase()->DeleteProfile(
        this->Synchronizer->GetAuthenticator()->GetProfile());
      }

    return rc;
    }
  this->PrintUsage();
  return -1;
}

//-------------------------------------------------------------------
int midasCLI::RunSynchronizer()
{
  if(this->ServerURL != "")
    {
    this->Synchronizer->SetServerURL(this->ServerURL);
    }

  // Create and use temporary profile
  if(this->UseTempProfile)
    {
    std::string tempName = midasUtils::GenerateUUID();

    if(!this->Synchronizer->GetAuthenticator()->AddAuthProfile(
      this->TempProfile->User, this->TempProfile->AppName,
      this->TempProfile->ApiKey, this->TempProfile->RootDir, tempName))
      {
      std::cout << "Ad hoc authentication failed." << std::endl;
      return -1;
      }

    this->Synchronizer->GetAuthenticator()->SetProfile(tempName);
    }

  std::string oldRoot = "";
  if(this->RootDir != "")
    {
    if(!kwsys::SystemTools::FileIsDirectory(this->RootDir.c_str()))
      {
      std::cerr << "Failed: \"" << this->RootDir << "\" does not refer to a "
        "valid directory." << std::endl;
      return -1;
      }

    oldRoot = this->Synchronizer->GetDatabase()->GetSetting(
      midasDatabaseProxy::ROOT_DIR);
    this->Synchronizer->GetDatabase()->SetSetting(
      midasDatabaseProxy::ROOT_DIR, this->RootDir);
    }

  int retVal = this->Synchronizer->Perform();

  if(oldRoot != "")
    {
    this->Synchronizer->GetDatabase()->SetSetting(
      midasDatabaseProxy::ROOT_DIR, oldRoot);
    }
  return retVal;
}

//-------------------------------------------------------------------
int midasCLI::PerformCreateProfile(std::vector<std::string> args)
{
  unsigned i;

  std::string name, user, apiKey, appName, rootDir;
  for(i = 0; i < args.size(); i++)
    {
    if(args[i] == "-e" || args[i] == "--email"
      && args.size() > i + 1)
      {
      i++;
      user = args[i];
      }
    else if(args[i] == "-n" || args[i] == "--name"
      && args.size() > i + 1)
      {
      i++;
      name = args[i];
      }
    else if(args[i] == "-k" || args[i] == "--api-key"
      && args.size() > i + 1)
      {
      i++;
      apiKey = args[i];
      }
    else if(args[i] == "-a" || args[i] == "--app-name"
      && args.size() > i + 1)
      {
      i++;
      appName = args[i];
      }
    else if(args[i] == "-r" || args[i] == "--root-dir"
      && args.size() > i + 1)
      {
      i++;
      rootDir = args[i];
      }
    else
      {
      this->Synchronizer->SetServerURL(args[i]);
      }
    }

  //Print the available authentication auth profiles if no args given
  if(name == "" && user == "" && apiKey == "" && appName == "")
    {
    std::vector<std::string> profiles =
      this->Synchronizer->GetDatabase()->GetAuthProfiles();

    std::cout << "Available authentication profiles:" << std::endl;
    for(std::vector<std::string>::iterator i = profiles.begin();
        i != profiles.end(); ++i)
      {
      std::cout << "  " << *i << std::endl;
      }
    return 0;
    }

  if(name == "" || user == "" || apiKey == "" || appName == "")
    {
    this->PrintCommandHelp("create_profile");
    return -1;
    }

  std::cout << "Adding authentication profile '" << name << "'" << std::endl;
  bool ok = this->Synchronizer->GetAuthenticator()->AddAuthProfile(
    user, appName, apiKey, rootDir, name);

  if(ok)
    {
    std::cout << "Profile successfully created." << std::endl;
    return 0;
    }
  else
    {
    std::cout << "Failed to add authentication profile." << std::endl;
    return -1;
    }
}

//-------------------------------------------------------------------
bool midasCLI::ParseAdd(std::vector<std::string> args)
{
  this->Synchronizer->SetOperation(midasSynchronizer::OPERATION_ADD);
  
  unsigned i;
  for(i = 0; i < args.size(); i++)
    {
    if(args[i] == "-c")
      {
      this->Synchronizer->SetResourceType(midasResourceType::COLLECTION);
      }
    else if(args[i] == "-C")
      {
      this->Synchronizer->SetResourceType(midasResourceType::COMMUNITY);
      }
    else if(args[i] == "-i")
      {
      this->Synchronizer->SetResourceType(midasResourceType::ITEM);
      }
    else if(args[i] == "-b")
      {
      this->Synchronizer->SetResourceType(midasResourceType::BITSTREAM);
      }
    else if(args[i] == "-p")
      {
      this->Synchronizer->SetPathMode(true);
      }
    else if(args[i] == "--parent" && i + 1 < args.size())
      {
      i++;
      this->Synchronizer->SetServerHandle(args[i]);
      }
    else
      {
      break;
      }
    }

  if(i < args.size() &&
     this->Synchronizer->GetResourceType() != midasResourceType::NONE)
    {
    this->Synchronizer->SetClientHandle(args[i]);
    }
  else
    {
    this->PrintCommandHelp("add");
    return false;
    }

  i++;
  if(i < args.size())
    {
    this->ServerURL = args[i];
    }
  return true;
}

//-------------------------------------------------------------------
bool midasCLI::ParseClean(std::vector<std::string> args)
{
  this->Synchronizer->SetOperation(midasSynchronizer::OPERATION_CLEAN);
  return true;
}

//-------------------------------------------------------------------
bool midasCLI::ParseClone(std::vector<std::string> args)
{
  this->Synchronizer->SetOperation(midasSynchronizer::OPERATION_CLONE);
  this->Synchronizer->SetRecursive(true);

  unsigned i;
  for(i = 0; i < args.size(); i++)
    {
    if(args[i] == "-d" && i + 1 < args.size())
      {
      i++;
      this->RootDir = args[i];
      }
    else
      {
      break;
      }
    }
  
  if(i < args.size())
    {
    this->ServerURL = args[i];
    }
  else if(this->ServerURL == "" && this->Synchronizer->GetServerURL() == "")
    {
    this->PrintCommandHelp("clone");
    return false;
    }
  return true;
}

//-------------------------------------------------------------------
bool midasCLI::ParsePull(std::vector<std::string> args)
{
  this->Synchronizer->SetOperation(midasSynchronizer::OPERATION_PULL);

  unsigned i;
  for(i = 0; i < args.size(); i++)
    {
    if(args[i] == "-r")
      {
      this->Synchronizer->SetRecursive(true);
      }
    else if(args[i] == "-p")
      {
      this->Synchronizer->SetPathMode(true);
      }
    else if(args[i] == "-c")
      {
      this->Synchronizer->SetResourceType(midasResourceType::COLLECTION);
      }
    else if(args[i] == "-C")
      {
      this->Synchronizer->SetResourceType(midasResourceType::COMMUNITY);
      }
    else if(args[i] == "-i")
      {
      this->Synchronizer->SetResourceType(midasResourceType::ITEM);
      }
    else if(args[i] == "-b")
      {
      this->Synchronizer->SetResourceType(midasResourceType::BITSTREAM);
      }
    else if(args[i] == "-d" && i + 1 < args.size())
      {
      i++;
      this->RootDir = args[i];
      }
    else
      {
      break;
      }
    }

  if(this->Synchronizer->GetResourceType() == midasResourceType::NONE &&
     !this->Synchronizer->IsPathMode())
    {
    this->PrintCommandHelp("pull");
    return false;
    }

  if(i < args.size())
    {
    this->Synchronizer->SetServerHandle(args[i]);
    }
  else
    {
    this->PrintCommandHelp("pull");
    return false;
    }

  i++;
  if(i < args.size())
    {
    this->ServerURL = args[i];
    }
  return true;
}

//-------------------------------------------------------------------
bool midasCLI::ParsePush(std::vector<std::string> args)
{
  this->Synchronizer->SetOperation(midasSynchronizer::OPERATION_PUSH);

  if(!args.size() && this->Synchronizer->GetServerURL() == "")
    {
    this->PrintCommandHelp("push");
    return false;
    }
  else if(args.size())
    {
    this->Synchronizer->GetDatabase()->CheckModifiedBitstreams();
    this->ServerURL = args[0];
    }
  return true;
}

//-------------------------------------------------------------------
int midasCLI::SetRootDir(std::vector<std::string> args)
{
  if(!args.size())
    {
    this->PrintCommandHelp("set_root_dir");
    return -1;
    }

  std::string root_dir = args[0];
  if(!kwsys::SystemTools::FileIsDirectory(root_dir.c_str()))
    {
    std::cerr << "Failed: \"" << root_dir << "\" does not refer to a valid "
      "directory." << std::endl;
    return -1;
    }
  kwsys::SystemTools::ConvertToUnixSlashes(root_dir);
  this->Synchronizer->GetDatabase()->SetSetting(
    midasDatabaseProxy::ROOT_DIR, root_dir);

  std::cout << "Changed root directory to " << root_dir << "." << std::endl;
  return 0;
}

//-------------------------------------------------------------------
bool midasCLI::ParseStatus(std::vector<std::string> args)
{
  this->Synchronizer->GetDatabase()->CheckModifiedBitstreams();
  std::vector<midasStatus> stats = this->Synchronizer->GetStatusEntries();
  for(std::vector<midasStatus>::iterator i = stats.begin(); i != stats.end();
      ++i)
    {
    switch(i->GetDirtyAction())
      {
      case midasDirtyAction::ADDED:
        std::cout << "A";
        break;
      case midasDirtyAction::MODIFIED:
        std::cout << "M";
        break;
      case midasDirtyAction::REMOVED:
        std::cout << "R";
        break;
      }
    std::cout << "-";
    switch(i->GetType())
      {
      case midasResourceType::BITSTREAM:
        std::cout << "b";
        break;
      case midasResourceType::COLLECTION:
        std::cout << "c";
        break;
      case midasResourceType::COMMUNITY:
        std::cout << "C";
        break;
      case midasResourceType::ITEM:
        std::cout << "i";
        break;
      }

    std::cout << " " << i->GetPath() << std::endl;
    }
  return true;
}

//-------------------------------------------------------------------
bool midasCLI::ParseUpload(std::vector<std::string> args)
{
  this->Synchronizer->SetOperation(midasSynchronizer::OPERATION_UPLOAD);

  if(args.size() < 2)
    {
    this->PrintCommandHelp("upload");
    return false;
    }

  this->Synchronizer->SetClientHandle(args[0]);
  this->Synchronizer->SetServerHandle(args[1]);

  return true;
}

int midasCLI::PerformDelete(std::vector<std::string> args)
{
  bool deleteOnDisk = false;
  unsigned i;
  for(i = 0; i < args.size(); i++)
    {
    if(args[i] == "-d")
      {
      deleteOnDisk = true;
      }
    else
      {
      break;
      }
    }

  if(i + 1 > args.size())
    {
    this->PrintCommandHelp("delete");
    return -1;
    }
  std::string path = args[i];
  std::string uuid = this->Synchronizer->GetDatabase()->GetUuidFromPath(path);

  if(uuid == "")
    {
    std::stringstream text;
    text << "Error: No resource in the database for path \"" << path << "\""
      << std::endl;
    this->Log->Error(text.str());
    return -2;
    }

  if(this->Synchronizer->GetDatabase()->
     DeleteResource(uuid, deleteOnDisk))
    {
    std::stringstream text;
    text << "Deleted resource at " << path << std::endl;
    this->Log->Message(text.str());
    return 0;
    }
  else
    {
    std::stringstream text;
    text << "Error: failed to delete resource at " << path << std::endl;
    this->Log->Error(text.str());
    return -3;
    }
}

int midasCLI::PerformSetMetadata(std::vector<std::string> args)
{
  bool append = false;
  unsigned i;
  for(i = 0; i < args.size(); i++)
    {
    if(args[i] == "-a" || args[i] == "--append")
      {
      append = true;
      }
    else
      {
      break;
      }
    }

  if(i + 3 > args.size())
    {
    this->PrintCommandHelp("set_metadata");
    return -1;
    }
  std::string path = args[i];
  std::string key = args[i+1];
  std::string value = args[i+2];

  std::string uuid = this->Synchronizer->GetDatabase()->GetUuidFromPath(path);

  if(uuid == "")
    {
    std::stringstream text;
    text << "Error: No resource in the database for path \"" << path << "\""
      << std::endl;
    this->Log->Error(text.str());
    return -2;
    }

  midasResourceRecord resource =
    this->Synchronizer->GetDatabase()->GetRecordByUuid(uuid);

  mdo::Object* obj;
  mds::Object* mdsObj;
  switch(resource.Type)
    {
    case midasResourceType::COMMUNITY:
      obj = new mdo::Community;
      mdsObj = new mds::Community;
      break;
    case midasResourceType::COLLECTION:
      obj = new mdo::Collection;
      mdsObj = new mds::Collection;
      break;
    case midasResourceType::ITEM:
      obj = new mdo::Item;
      mdsObj = new mds::Item;
      break;
    case midasResourceType::BITSTREAM:
      obj = new mdo::Bitstream;
      mdsObj = new mds::Bitstream;
      break;
    }

  obj->SetId(resource.Id);
  mdsObj->SetObject(obj);
  mdsObj->SetDatabase(this->Synchronizer->GetDatabase());
  mdsObj->MarkAsDirty();
  mdsObj->Fetch();
  if(!obj->SetValue(key, value, append))
    {
    std::stringstream text;
    text << "Error: " << obj->GetTypeName() << "::SetValue failed for key="
      << key << " and value=" << value << std::endl;
    this->Log->Error(text.str());
    return -3;
    }
  if(!mdsObj->Commit())
    {
    std::stringstream text;
    text << "Error: mds::" << obj->GetTypeName() <<
      "::Commit failed" << std::endl;
    this->Log->Error(text.str());
    return -4;
    }

  std::stringstream text;
  text << obj->GetTypeName() << " metadata changed." << std::endl;
  this->Log->Message(text.str());

  delete obj;
  delete mdsObj;
  return 0;
}

//-------------------------------------------------------------------
void midasCLI::PrintUsage()
{
  std::cout << "MIDAS Command Line Interface" << std::endl
    << "Usage: MIDAScli [--database DATABASE_LOCATION] [--profile PROFILE]"
    " COMMAND [ARGS]" << std::endl << std::endl 
    << "Where COMMAND is one of the following:"
    << std::endl <<
    " add              Add a file into the local repository."
    << std::endl <<
    " clean            Clean the local repository."
    << std::endl <<
    " clone            Copy an entire MIDAS database locally."
    << std::endl <<
    " create_profile   Create an authentication profile."
    << std::endl <<
    " delete           Delete a local resource."
    << std::endl <<
    " pull             Copy part of a MIDAS database locally."
    << std::endl <<
    " push             Copy locally added resources to a MIDAS server."
    << std::endl <<
    " upload           Upload a file to a MIDAS server."
    << std::endl <<
    " status           List dirty resources pending push."
    << std::endl <<
    " set_metadata     Change metadata for a local resource."
    << std::endl <<
    " set_root_dir     Set where resources should be pulled to on disk."
    << std::endl << std::endl << "Use MIDAScli --help COMMAND for "
    "help with individual commands." << std::endl;
}

//-------------------------------------------------------------------
void midasCLI::PrintCommandHelp(std::string command)
{
  if(command == "pull")
    {
    std::cout << "Usage: MIDAScli ... pull [COMMAND_OPTIONS] RESOURCE_ID "
      "[URL]" << std::endl << "Where COMMAND_OPTIONS can be: "
      << std::endl << " -p         RESOURCE_ID is a path, not an id number"
      << std::endl << " -r         Copy recursively."
      << std::endl << " -C         For pulling a community."
      << std::endl << " -c         For pulling a collection."
      << std::endl << " -i         For pulling an item."
      << std::endl << " -b         For pulling a bitstream."
      << std::endl << " -d         Specify the root directory for this pull."
      << std::endl << "Exactly one type must be specified (-b, -i, -c, -C)"
      << std::endl << "unless path mode (-p) is used."
      << std::endl;
    }
  else if(command == "push")
    {
    std::cout << "Usage: MIDAScli ... push [URL] " << std::endl;
    }
  else if(command == "clone")
    {
    std::cout << "Usage: MIDAScli ... clone [COMMAND_OPTIONS] [URL]"
      << std::endl << "Where COMMAND_OPTIONS can be: "
      << std::endl << " -d      Specify the root directory for this clone."
      << std::endl;
    }
  else if(command == "clean")
    {
    std::cout << "Usage: MIDAScli ... clean" << std::endl;
    }
  else if(command == "delete")
    {
    std::cout << "Usage: MIDAScli ... delete [-d] PATH" << std::endl <<
      "PATH refers to the path on disk of the item to remove locally." <<
      std::endl << "The -d option will also delete the files on disk." <<
      std::endl;
    }
  else if(command == "add")
    {
    std::cout << "Usage: MIDAScli ... add [COMMAND_OPTIONS] PATH [URL]"
      << std::endl << "Where COMMAND_OPTIONS can be: "
      << std::endl << " -C         For adding a community."
      << std::endl << " -c         For adding a collection."
      << std::endl << " -i         For adding an item."
      << std::endl << " -b         For adding a bitstream."
      << std::endl << " --local    Do not push this resource to the server."
      << std::endl << " --parent PARENT_ID"
      << std::endl << "            Specify the id of the server-side parent."
      << std::endl << "Exactly one type must be specified (-b, -i, -c, -C)."
      << std::endl
      << "And PATH is a relative or absolute path to the dir/file to add."
      << std::endl;
    }
  else if(command == "create_profile")
    {
    std::cout << "Usage: MIDAScli ... create_profile PROPERTIES [URL]"
      << std::endl << "Where PROPERTIES must contain all of:"
      << std::endl << 
      " --name NAME          The name of the profile to create"
      << std::endl << 
      " --email EMAIL        The user's email for logging in"
      << std::endl << 
      " --api-key KEY        The API key generated by MIDAS"
      << std::endl << 
      " --app-name APPNAME   The application name for the given key"
      << std::endl << "And may also contain the following options:"
      << std::endl <<
      " --root-dir DIR       The root directory to pull into for this profile"
      << std::endl << std::endl <<
      "Calling create_profile with no arguments will list all available "
      "authentication profiles in the database." << std::endl;
    }
  else if(command == "set_root_dir")
    {
    std::cout << "Usage: MIDAScli ... set_root_dir DIR" << std::endl;
    }
  else if(command == "status")
    {
    std::cout << "Usage: MIDAScli ... status" << std::endl;
    }
  else if(command == "upload")
    {
    std::cout << "Usage: MIDAScli ... upload SOURCE DESTINATION"
      << std::endl << "Where SOURCE is the location on disk of a bitstream "
      "to upload," << std::endl << "and DESTINATION is the path on the "
      "MIDAS server of the item into which the bitstream will be uploaded."
      << std::endl;
    }
  else if(command == "set_metadata")
    {
    std::cout << "Usage: MIDAScli ... set_metadata [-a] PATH KEY VALUE"
      << std::endl << "Where PATH is the absolute or relative path to "
      "the local resource," << std::endl <<
      "KEY is the name of the metadata field you want to set," << std::endl
      << "and VALUE is the value to set it to." << std::endl <<
      "If -a is specified, the value will be appended to the current value "
      "instead of overwriting it." << std::endl << std::endl <<
      "Valid metadata keys, by type:" << std::endl <<
      " Community: name, description, copyright, introductorytext"
      << std::endl <<
      " Collection: name, description, copyright, introductorytext"
      << std::endl <<
      " Item: title, abstract, description, authors, keywords"
      << std::endl << std::endl <<
      "Key names are not case sensitive." << std::endl;
    }
  else
    {
    std::cerr << "Error: \"" << command << "\" is not a valid MIDAScli "
      "command." << std::endl;
    }
}
