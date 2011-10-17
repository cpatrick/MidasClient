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
#include "MidasApplication.h"

#include <QMessageBox>

#include "MIDASDesktopUI.h"
#include "MIDASConfig.h"

MidasApplication * MidasApplication::m_instance = NULL;

MidasApplication * MidasApplication::instance()
{
  if( MidasApplication::m_instance == NULL )
    {
    MidasApplication::m_instance = dynamic_cast<MidasApplication *>(qApp);
    }
  return MidasApplication::m_instance;
}

MidasApplication::MidasApplication( int & argc, char * * argv) :
  QApplication(argc, argv)
{
}

int MidasApplication::exec()
{
  Q_INIT_RESOURCE(MIDASDesktopIcons);

  this->MIDASDesktop = new MIDASDesktopUI();

  MIDASDesktop->show();

  int code = QApplication::exec();
  delete this->MIDASDesktop;
  return code;
}

bool MidasApplication::notify(QObject* receiver, QEvent* event)
{
  try
    {
    return QApplication::notify(receiver, event);
    }
  catch( const std::exception& e )
    {
    std::stringstream text;
    text << "Caught exception during notify to object "
         << receiver->objectName().toStdString();
    text << ". Message: " << e.what();
    this->MIDASDesktop->GetLog()->Error(text.str() );
    }
  return false;
}
