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
#ifndef __MidasApplication_H
#define __MidasApplication_H

#include <QApplication>

class MIDASDesktopUI;

class MidasApplication : public QApplication
{
  Q_OBJECT
public:
  MidasApplication( int & argc, char * * argv);
  int exec();

  bool notify( QObject * receiver, QEvent * event );

  static MidasApplication * instance();

private:
  static MidasApplication* m_instance;

  MIDASDesktopUI * MIDASDesktop;
};

#endif // __MidasApplication_H
