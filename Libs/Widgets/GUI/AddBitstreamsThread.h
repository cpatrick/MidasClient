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
#ifndef __AddBitstreamsThread_H_
#define __AddBitstreamsThread_H_

#include "midasStandardIncludes.h"

#include <QThread>
#include <QStringList>

class MidasItemTreeItem;
class Midas3ItemTreeItem;

class AddBitstreamsThread : public QThread
{
  Q_OBJECT
public:
  AddBitstreamsThread();
  ~AddBitstreamsThread();

  void SetFiles(const QStringList& files);

  void SetParentItem(MidasItemTreeItem* parentItem);

  void SetParentItem(Midas3ItemTreeItem* parentItem);

  virtual void run();

signals:
  void enableActions(bool);

  void progress(int, int, const QString &);

private:
  MidasItemTreeItem*  m_ParentItem;
  Midas3ItemTreeItem* m_ParentItem3;
  QStringList         m_Files;
};

#endif
