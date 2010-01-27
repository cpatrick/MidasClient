#include "midasCLI.h"

midasCLI::midasCLI()
{
  this->DatabaseLocation =
    kwsys::SystemTools::GetCurrentWorkingDirectory() + "/midas.db";
  this->Synchronizer = new midasSynchronizer(this);
}

midasCLI::~midasCLI()
{
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
    if(args[i] == "clone" && i < args.size() - 1)
      {
      i++;
      this->Synchronizer->SetOperation(midasSynchronizer::OPERATION_CLONE);
      this->Synchronizer->SetServerURL(args[i]);
      }
    else if(args[i] == "push" && i < args.size() - 2)
      {
      i++;
      this->Synchronizer->SetOperation(midasSynchronizer::OPERATION_PUSH);
      this->Synchronizer->SetServerURL(args[i]);
      i++;
      this->Synchronizer->SetResourceId(args[i]);
      }
    else if(args[i] == "pull" && i < args.size() - 2)
      {
      i++;
      this->Synchronizer->SetOperation(midasSynchronizer::OPERATION_PULL);
      this->Synchronizer->SetServerURL(args[i]);
      i++;
      this->Synchronizer->SetResourceId(args[i]);
      }
    else if(args[i] == "--database" && i < args.size() - 1)
      {
      i++;
      this->DatabaseLocation = args[i];
      }
    else if(args[i] == "--help")
      {
      if(i < args.size() - 2)
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
//TODO document the cli for each command (push, pull, clone)
}
