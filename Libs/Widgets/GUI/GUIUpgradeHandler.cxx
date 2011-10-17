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
#include "GUIUpgradeHandler.h"
#include "UpgradeUI.h"
#include "mdoVersion.h"
#include "mdsUpgrade.h"

GUIUpgradeHandler::GUIUpgradeHandler(UpgradeUI* ui)
{
  m_Dialog = ui;
}

GUIUpgradeHandler::~GUIUpgradeHandler()
{
}

bool GUIUpgradeHandler::Upgrade(const std::string& path,
                                mdo::Version dbVersion,
                                mdo::Version productVersion)
{
  m_Dialog->SetDbVersion(dbVersion.VersionString() );
  m_Dialog->SetProductVersion(productVersion.VersionString() );

  if( m_Dialog->exec() == QDialog::Accepted )
    {
    Log->Message("Upgrading database schema to version " + productVersion.VersionString() );
    if( mds::Upgrade::UpgradeDatabase(path, dbVersion) )
      {
      Log->Message("Database upgrade successful");
      return true;
      }
    else
      {
      Log->Error("Database upgrade failed");
      return false;
      }
    }
  return false;
}

