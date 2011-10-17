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
#ifndef __DeleteThread_H
#define __DeleteThread_H

#include <QString>
#include <QThread>

namespace mdo
{
class Object;
}

class MidasTreeItem;
class Midas3TreeItem;

/**
 * Thread used to delete a resource from the local tree
 */
class DeleteThread : public QThread
{
  Q_OBJECT
public:
  DeleteThread();
  ~DeleteThread();

  void SetResource(MidasTreeItem* resource);

  void SetResource3(Midas3TreeItem* resource);

  void SetDeleteOnDisk(bool val);

  virtual void run();

signals:
  void deletedResource(mdo::Object *);

  void enableActions(bool);

  void errorMessage(const QString &);

private:
  MidasTreeItem*  m_Resource;
  Midas3TreeItem* m_Resource3;
  bool            m_DeleteOnDisk;
};

#endif
