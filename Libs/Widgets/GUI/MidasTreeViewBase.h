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
#ifndef __MidasTreeViewBase_H
#define __MidasTreeViewBase_H

#include <QTreeView>
#include <QWidget>

class midasSynchronizer;

class MidasTreeViewBase : public QTreeView
{
  Q_OBJECT
public:

  MidasTreeViewBase(QWidget* parent) : QTreeView(parent)
  {
    this->setAlternatingRowColors(true);
    this->setHeaderHidden(true);
  }

  virtual ~MidasTreeViewBase()
  {
  }

  virtual void SetSynchronizer(midasSynchronizer* synch) = 0;

  virtual void Clear() = 0;

  virtual void Initialize() = 0;

  virtual void Update() = 0;

};

#endif
