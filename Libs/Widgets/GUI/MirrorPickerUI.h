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
#ifndef __MirrorPickerUI_H
#define __MirrorPickerUI_H

#include "ui_MirrorPickerUI.h"
#include "midasStandardIncludes.h"
#include <QRadioButton>

namespace mdo
{
class Assetstore;
class Bitstream;
}

typedef std::map<mdo::Assetstore *, QRadioButton *> MirrorMap;

/**
 * Modal dialog that presents the user with a list of available mirrors
 */
class MirrorPickerUI :  public QDialog, private Ui::MirrorPickerDialog
{
  Q_OBJECT
public:
  MirrorPickerUI(QWidget* parent);
  ~MirrorPickerUI();

  mdo::Assetstore * GetSelectedLocation();

  bool ApplyToAll();

public slots:

  int exec(mdo::Bitstream* bitstream);

protected:
  void ClearMirrorList();

  MirrorMap m_Mirrors;

};

#endif
