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
#ifndef __CreateMidasResourceUI_H
#define __CreateMidasResourceUI_H

#include "ui_CreateMidasResourceUI.h"

#include <QFlags>

class midasSynchronizer;
class MidasTreeItem;
class Midas3TreeItem;

class CreateMidasResourceUI : public QDialog, private Ui::CreateMidasResourceDialog
{
  Q_OBJECT
public:
  enum Type
    {
    Community = 0x0,
    SubCommunity = 0x1,
    Collection = 0x2,
    Item = 0x3,
    Community3 = 0x4,
    Folder = 0x5,
    SubFolder = 0x6,
    Item3 = 0x7
    };
  Q_DECLARE_FLAGS(Types, Type) CreateMidasResourceUI(QWidget* parent, midasSynchronizer* synch);
  ~CreateMidasResourceUI();

  void SetParentResource(const MidasTreeItem* parent);

  void SetParentResource3(const Midas3TreeItem* parent);

  void AddCommunity(std::string path);

  void AddSubCommunity();

  void AddCollection();

  void AddItem();

  void AddCommunity3(std::string path);

  void AddFolder(std::string path);

  void AddSubFolder();

  void AddItem3();

  virtual bool ValidateName() const;

  virtual void SetType(Types type);

signals:
  void resourceCreated(); // emitted when a resource is added successfully

public slots:
  void reset();

  int exec();

  virtual void accept();

protected:
  Types              m_Type;
  midasSynchronizer* m_Synch;
  MidasTreeItem*     m_ParentResource;
  Midas3TreeItem*    m_ParentResource3;
};

Q_DECLARE_OPERATORS_FOR_FLAGS(CreateMidasResourceUI::Types)

#endif // __CreateMidasResourceUI_H
