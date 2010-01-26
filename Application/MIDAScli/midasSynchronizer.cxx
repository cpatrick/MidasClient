#include "midasSynchronizer.h"

midasSynchronizer::midasSynchronizer(midasCLI* cli)
{
  this->CLI = cli;
}

midasSynchronizer::~midasSynchronizer()
{
}

void midasSynchronizer::SetOperation(midasSynchronizer::SynchOperation op)
{
  this->Operation = op;
}

midasSynchronizer::SynchOperation midasSynchronizer::GetOperation()
{
  return this->Operation;
}

void midasSynchronizer::SetResourceId(std::string id)
{
  this->ResourceId = id;
}

std::string midasSynchronizer::GetResourceId()
{
  return this->ResourceId;
}

std::string midasSynchronizer::GetServerURL()
{
  return this->ServerURL;
}

void midasSynchronizer::SetServerURL(std::string url)
{
  this->ServerURL = url;
}

int midasSynchronizer::Perform()
{
  return 0;
}
