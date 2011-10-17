/******************************************************************************
 * Copyright 2011 Kitware Inc.
 *
 * Licensed under the Apache License, Version 2.0 (the "License");
 * you may not use this file except in compliance with the License.
 * You may obtain a copy of the License at
 *
 *     http://www.apache.org/licenses/LICENSE-2.0
 *
 * Unless required by applicable law or agreed to in writing, software
 * distributed under the License is distributed on an "AS IS" BASIS,
 * WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
 * See the License for the specific language governing permissions and
 * limitations under the License.
 *****************************************************************************/
#include "midasCLIApplication.h"
#include "midasCLI.h"

midasCLIApplication::midasCLIApplication( int & argc, char * * argv) :
  QCoreApplication(argc, argv)
{
  for( int i = 1; i < argc; i++ )
    {
    this->m_Args.push_back(argv[i]);
    }

  m_CLI = new midasCLI;
}

midasCLIApplication::~midasCLIApplication()
{
  delete this->m_CLI;
}

int midasCLIApplication::exec()
{
  return m_CLI->Perform(this->m_Args);
}

bool midasCLIApplication::notify(QObject* receiver, QEvent* event)
{
  try
    {
    return QCoreApplication::notify(receiver, event);
    }
  catch( const std::exception& e )
    {
    std::cerr << "Caught exception during notify to object "
              << receiver->objectName().toStdString() << ". Message: " << e.what();
    }
  return false;
}

