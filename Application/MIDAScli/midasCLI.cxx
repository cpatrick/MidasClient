/*=========================================================================

  Copyright (c) Kitware Inc. All rights reserved.
  See Copyright.txt or http://www.Kitware.com/Copyright.htm for details.

     This software is distributed WITHOUT ANY WARRANTY; without even
     the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR
     PURPOSE.  See the above copyright notices for more information.

=========================================================================*/

#include "midasCLI.h"
#include "midasDotProgressReporter.h"
#include "midasStatus.h"

midasCLI::midasCLI()
{
  this->Synchronizer = new midasSynchronizer();
  this->Synchronizer->SetDatabase(
    kwsys::SystemTools::GetCurrentWorkingDirectory() + "/midas.db");
  this->Synchronizer->SetProgressReporter(
    reinterpret_cast<midasProgressReporter*>(
    new midasDotProgressReporter(30)));
}

midasCLI::~midasCLI()
{
  this->Synchronizer->DeleteProgressReporter();
  delete this->Synchronizer;
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
      std::vector<std::string> postOpArgs(args.begin() + i + 1, args.end());
      ok = this->ParseAdd(postOpArgs);
      break;
      }
    else if(args[i] == "clean")
      {
      std::vector<std::string> postOpArgs(args.begin() + i + 1, args.end());
      ok = this->ParseClean(postOpArgs);
      break;
      }
    else if(args[i] == "clone")
      {
      std::vector<std::string> postOpArgs(args.begin() + i + 1, args.end());
      ok = this->ParseClone(postOpArgs);
      break;
      }
    else if(args[i] == "create_profile")
      {
      std::vector<std::string> postOpArgs(args.begin() + i + 1, args.end());
      return this->PerformCreateProfile(postOpArgs);
      }
    else if(args[i] == "push")
      {
      std::vector<std::string> postOpArgs(args.begin() + i + 1, args.end());
      ok = this->ParsePush(postOpArgs);
      break;
      }
    else if(args[i] == "pull")
      {
      std::vector<std::string> postOpArgs(args.begin() + i + 1, args.end());
      ok = this->ParsePull(postOpArgs);
      break;
      }
    else if(args[i] == "status")
      {
      std::vector<std::string> postOpArgs(args.begin() + i + 1, args.end());
      ok = this->ParseStatus(postOpArgs);
      break;
      }
    else if(args[i] == "--database" && i + 1 < args.size())
      {
      i++;
      this->Synchronizer->SetDatabase(args[i]);
      }
    else if(args[i] == "--profile" && i + 1 < args.size())
      {
      i++;
      this->Synchronizer->GetAuthenticator()->SetProfile(args[i]);
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
  return ok ? this->Synchronizer->Perform() : -1;
}

//-------------------------------------------------------------------
int midasCLI::PerformCreateProfile(std::vector<std::string> args)
{
  unsigned i;

  std::string name, user, apiKey, appName;
  for(i = 0; i < args.size(); i++)
    {
    if(args[i] == "-u" || args[i] == "--email"
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
    else
      {
      this->Synchronizer->SetServerURL(args[i]);
      }
    }

  if(name == "" || user == "" || apiKey == "" || appName == "")
    {
    this->PrintCommandHelp("create_profile");
    return -1;
    }

  this->

  std::cout << "Adding authentication profile '" << name << "'" << std::endl;
  bool ok = this->Synchronizer->GetAuthenticator()->AddAuthProfile(
    user, appName, apiKey, name);
  
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
    else if(args[i] == "--parent")
      {
      i++;
      if(i + 1 < args.size())
        {
        this->Synchronizer->SetParentId(atoi(args[i].c_str()));
        }
      else
        {
        this->PrintCommandHelp("add");
        return false;
        }
      }
    else
      {
      break;
      }
    }

  if(args.size() &&
     this->Synchronizer->GetResourceType() != midasResourceType::NONE)
    {
    this->Synchronizer->SetResourceHandle(args[i]);
    return true;
    }
  else
    {
    this->PrintCommandHelp("add");
    return false;
    }
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
  
  if(args.size())
    {
    this->Synchronizer->SetServerURL(args[0]);
    }
  else if(this->Synchronizer->GetServerURL() == "")
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
    else
      {
      break;
      }
    }

  if(this->Synchronizer->GetResourceType() == midasResourceType::NONE)
    {
    this->PrintCommandHelp("pull");
    return false;
    }

  if(i < args.size())
    {
    i++;
    this->Synchronizer->SetResourceHandle(args[i]);
    }
  else
    {
    this->PrintCommandHelp("pull");
    return false;
    }

  if(i < args.size())
    {
    i++;
    this->Synchronizer->SetServerURL(args[i]);
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
    this->Synchronizer->SetServerURL(args[0]);
    }
  return true;
}

//-------------------------------------------------------------------
bool midasCLI::ParseStatus(std::vector<std::string> args)
{
  std::vector<midasStatus> stats = this->Synchronizer->GetStatusEntries();
  for(std::vector<midasStatus>::iterator i = stats.begin(); i != stats.end();
      ++i)
    {
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
    " pull             Copy part of a MIDAS database locally."
    << std::endl <<
    " push             Copy local resources to a MIDAS server."
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
      << std::endl << " -r         Copy recursively."
      << std::endl << " -C         For pulling a community."
      << std::endl << " -c         For pulling a collection."
      << std::endl << " -i         For pulling an item."
      << std::endl << " -b         For pulling a bitstream."
      << std::endl << "Exactly one type must be specified (-b, -i, -c, -C)."
      << std::endl;
    }
  else if(command == "push")
    {
    std::cout << "Usage: MIDAScli ... push [URL] " << std::endl;
    }
  else if(command == "clone")
    {
    std::cout << "Usage: MIDAScli ... clone [URL]" << std::endl;
    }
  else if(command == "clean")
    {
    std::cout << "Usage: MIDAScli ... clean" << std::endl;
    }
  else if(command == "add")
    {
    std::cout << "Usage: MIDAScli ... add [COMMAND_OPTIONS] PATH "
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
      << std::endl << "Where PROPERTIES must contain all of: "
      << std::endl << 
      " --name NAME          The name of the profile to create"
      << std::endl << 
      " --email EMAIL        The user's email for logging in"
      << std::endl << 
      " --api-key KEY        The API key generated by MIDAS"
      << std::endl << 
      " --app-name APPNAME   The application name for the given key"
      << std::endl;
    }
}
