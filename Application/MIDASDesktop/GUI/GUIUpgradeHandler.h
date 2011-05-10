/*=========================================================================

  Copyright (c) Kitware Inc. All rights reserved.
  See Copyright.txt or http://www.Kitware.com/Copyright.htm for details.

     This software is distributed WITHOUT ANY WARRANTY; without even
     the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR
     PURPOSE.  See the above copyright notices for more information.

=========================================================================*/

#ifndef __GUIUpgradeHandler_H
#define __GUIUpgradeHandler_H

#include "mdsUpgradeHandler.h"
#include <QObject>

class UpgradeUI;

class GUIUpgradeHandler : public QObject, public mds::UpgradeHandler
{
  Q_OBJECT
public:
  GUIUpgradeHandler(UpgradeUI* dialog);
  ~GUIUpgradeHandler();

  bool Upgrade(const std::string& path,
               mdo::Version dbVersion, mdo::Version productVersion);

signals:
  void displayDialog();
  void errorMessage(const QString&);

private:
  UpgradeUI* m_Dialog;
};

#endif
