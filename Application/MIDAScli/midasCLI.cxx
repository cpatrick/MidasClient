/*=========================================================================

  Copyright (c) Kitware Inc. All rights reserved.
  See Copyright.txt or http://www.Kitware.com/Copyright.htm for details.

     This software is distributed WITHOUT ANY WARRANTY; without even
     the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR
     PURPOSE.  See the above copyright notices for more information.

=========================================================================*/

#include "midasCLI.h"
#include "midasDotProgressReporter.h"

midasCLI::midasCLI()
{
  this->DatabaseLocation =
    kwsys::SystemTools::GetCurrentWorkingDirectory() + "/midas.db";
  this->Synchronizer = new midasSynchronizer(this);
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
std::string midasCLI::GetDatabaseLocation()
{
  return this->DatabaseLocation;
}

//-------------------------------------------------------------------
void midasCLI::SetDatabaseLocation(std::string path)
{
  this->DatabaseLocation = path;
}

//-------------------------------------------------------------------
int midasCLI::Perform(std::vector<std::string> args)
{
  if(args.size() == 0)
    {
    this->PrintUsage();
    return -1;
    }

  for(unsigned i = 0; i < args.size(); i++)
    {
    if(args[i] == "clone")
      {
      std::vector<std::string> postOpArgs(args.begin() + i + 1, args.end());
      this->ParseClone(postOpArgs);
      break;
      }
    else if(args[i] == "push")
      {
      std::vector<std::string> postOpArgs(args.begin() + i + 1, args.end());
      this->ParsePush(postOpArgs);
      break;
      }
    else if(args[i] == "pull")
      {
      std::vector<std::string> postOpArgs(args.begin() + i + 1, args.end());
      this->ParsePull(postOpArgs);
      break;
      }
    else if(args[i] == "--database" && i + 1 < args.size())
      {
      i++;
      this->DatabaseLocation = args[i];
      }
    else if(args[i] == "--help")
      {
      if(i + 2 < args.size())
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
  return this->Synchronizer->Perform();
}

void midasCLI::ParseClone(std::vector<std::string> args)
{
  std::cout << "Clone not yet implemented.\n";
/*  i++;
  this->Synchronizer->SetOperation(midasSynchronizer::OPERATION_CLONE);
  this->Synchronizer->SetServerURL(args[i]);*/
}

//-------------------------------------------------------------------
void midasCLI::ParsePull(std::vector<std::string> args)
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
      this->Synchronizer->SetPullType(midasSynchronizer::TYPE_COLLECTION);
      }
    else if(args[i] == "-C")
      {
      this->Synchronizer->SetPullType(midasSynchronizer::TYPE_COMMUNITY);
      }
    else if(args[i] == "-i")
      {
      this->Synchronizer->SetPullType(midasSynchronizer::TYPE_ITEM);
      }
    else if(args[i] == "-b")
      {
      this->Synchronizer->SetPullType(midasSynchronizer::TYPE_BITSTREAM);
      }
    else
      {
      break;
      }
    }

  if(i + 1 < args.size() &&
    this->Synchronizer->GetPullType() != midasSynchronizer::TYPE_NONE)
    {
    this->Synchronizer->SetServerURL(args[i]);
    i++;
    this->Synchronizer->SetResourceHandle(args[i]);
    }
  else
    {
    this->PrintCommandHelp("pull");
    }
}

//-------------------------------------------------------------------
void midasCLI::ParsePush(std::vector<std::string> args)
{
  std::cout << "Push not yet implemented.\n";
/*  i++;
  this->Synchronizer->SetOperation(midasSynchronizer::OPERATION_PUSH);
  this->Synchronizer->SetServerURL(args[i]);
  i++;
  this->Synchronizer->SetResourceHandle(args[i]);*/
}

//-------------------------------------------------------------------
void midasCLI::PrintUsage()
{
  std::cout << "MIDAS Command Line Interface" << std::endl
    << "Usage: MIDAScli [--database DATABASE_LOCATION] COMMAND [ARGS]"
    << std::endl << std::endl << "Where COMMAND is one of the following:"
    << std::endl << " clone      Copy an entire MIDAS database locally."
    << std::endl << " pull       Copy part of a MIDAS database locally."
    << std::endl << " push       Copy local objects to a MIDAS server."
    << std::endl << std::endl << "Use MIDAScli --help COMMAND for "
    "help with individual commands." << std::endl;
}

//-------------------------------------------------------------------
void midasCLI::PrintCommandHelp(std::string command)
{
  if(command == "pull")
    {
    std::cout << "Usage: MIDAScli ... pull [COMMAND-OPTIONS] URL "
      "RESOURCE_ID" << std::endl << "Where COMMAND-OPTIONS can be: "
      << std::endl << " -r         Copy recursively."
      << std::endl << " -C         For pulling a community."
      << std::endl << " -c         For pulling a collection."
      << std::endl << " -i         For pulling an item."
      << std::endl << " -b         For pulling a bitstream."
      << std::endl <<"Exactly one type must be specified (-b, -i, -c, -C)."
      << std::endl;
    }
}
