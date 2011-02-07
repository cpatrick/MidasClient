/*=========================================================================

  Copyright (c) Kitware Inc. All rights reserved.
  See Copyright.txt or http://www.Kitware.com/Copyright.htm for details.

     This software is distributed WITHOUT ANY WARRANTY; without even
     the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR
     PURPOSE.  See the above copyright notices for more information.

=========================================================================*/

#ifndef __GUIFileOverwriteHandler_H
#define __GUIFileOverwriteHandler_H

#include "midasFileOverwriteHandler.h"
#include <QObject>

class FileOverwriteUI;
class MIDASDesktopUI;
class midasSynchronizer;

class GUIFileOverwriteHandler : public QObject, public midasFileOverwriteHandler
{
  Q_OBJECT
public:
  GUIFileOverwriteHandler(MIDASDesktopUI* parent);
  ~GUIFileOverwriteHandler();

  Action HandleConflict(std::string path);

public slots:
  void actionChosen(Action action, bool applyToAll);
  void chooseAction(int choice, bool applyToAll);
signals:
  void displayDialog(const QString& path);

private:
  MIDASDesktopUI* m_Parent;
  bool            m_ApplyToAll;
  bool            m_Done;
  Action          m_Action;
};

#endif
