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
#ifndef __midasCLIApplication_H
#define __midasCLIApplication_H

#include <QCoreApplication>

#include "midasStandardIncludes.h"

class midasCLI;

class midasCLIApplication : public QCoreApplication
{
  Q_OBJECT
public:
  midasCLIApplication( int & argc, char * * argv);
  ~midasCLIApplication();

  int exec();

  bool notify( QObject * receiver, QEvent* event );

private:
  midasCLI*                m_CLI;
  std::vector<std::string> m_Args;
};

#endif // __MidasApplication_H
