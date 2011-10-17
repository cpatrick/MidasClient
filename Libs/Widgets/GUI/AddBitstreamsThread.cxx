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
#include "AddBitstreamsThread.h"

#include "mdsDatabaseAPI.h"
#include "mdoVersion.h"
#include "mdsBitstream.h"
#include "mdoBitstream.h"
#include "m3dsBitstream.h"
#include "m3doBitstream.h"
#include "mdoItem.h"
#include "m3doItem.h"
#include "MidasItemTreeItem.h"
#include "Midas3ItemTreeItem.h"

#include <QFileInfo>
#include <QDateTime>

AddBitstreamsThread::AddBitstreamsThread()
{
  m_ParentItem = NULL;
  m_ParentItem3 = NULL;
}

AddBitstreamsThread::~AddBitstreamsThread()
{
}

void AddBitstreamsThread::SetFiles(const QStringList& files)
{
  m_Files = files;
}

void AddBitstreamsThread::SetParentItem(MidasItemTreeItem* parentItem)
{
  m_ParentItem = parentItem;
}

void AddBitstreamsThread::SetParentItem(Midas3ItemTreeItem* parentItem)
{
  m_ParentItem3 = parentItem;
}

void AddBitstreamsThread::run()
{
  emit         enableActions(false);
  unsigned int millis = midasUtils::CurrentTime();

  srand(millis); // seed RNG (must be done in each thread)

  mds::DatabaseAPI db;
  // TODO copy them into the local tree!
  // bool copyIntoTree = db.GetSettingBool(mds::DatabaseAPI::UNIFIED_TREE);

  int current = 0;
  int total = m_Files.size();
  for( QStringList::iterator i = m_Files.begin(); i != m_Files.end(); ++i )
    {
    QFileInfo   fileInfo(*i);
    std::string path = fileInfo.absoluteFilePath().toStdString();
    std::string name = fileInfo.fileName().toStdString();

    std::stringstream size;
    size << fileInfo.size();

    if( m_ParentItem3 )
      {
      // Don't delete this object - tree will delete it
      m3do::Bitstream* bitstream = new m3do::Bitstream;
      bitstream->SetName(name.c_str() );
      bitstream->SetSize(size.str() );
      bitstream->SetLastModified(QFileInfo(path.c_str() ).lastModified().toTime_t() );
      bitstream->SetChecksum(midasUtils::ComputeFileChecksum(path) );
      bitstream->SetPath(path);
      bitstream->SetParentId(m_ParentItem3->GetItem()->GetId() );
      bitstream->SetParentItem(m_ParentItem3->GetItem() );
      m3ds::Bitstream mdsBitstream;
      mdsBitstream.SetObject(bitstream);
      // TODO? mdsBitstream.MarkAsDirty();
      mdsBitstream.Commit();
      }
    else
      {
      mdo::Item* item = new mdo::Item;
      item->SetId(m_ParentItem->GetItem()->GetId() );
      item->SetUuid(m_ParentItem->GetItem()->GetUuid().c_str() );

      // Don't delete this object - tree will delete it
      mdo::Bitstream* bitstream = new mdo::Bitstream;
      bitstream->SetName(name.c_str() );
      bitstream->SetSize(size.str() );
      bitstream->SetLastModified(QFileInfo(path.c_str() ).lastModified().toTime_t() );
      bitstream->SetUuid(midasUtils::GenerateUUID().c_str() );
      bitstream->SetPath(path);
      bitstream->SetParentId(m_ParentItem->GetItem()->GetId() );
      bitstream->SetParentItem(item);
      mds::Bitstream mdsBitstream;
      mdsBitstream.SetObject(bitstream);
      mdsBitstream.MarkAsDirty();
      mdsBitstream.Commit();
      }

    current++;
    emit progress(current, total, name.c_str() );
    }
  emit enableActions(true);
}
