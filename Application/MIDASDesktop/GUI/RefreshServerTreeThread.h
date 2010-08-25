#ifndef __RefreshServerTreeThread_H
#define __RefreshServerTreeThread_H

#include <QThread>
#include "MIDASDesktopUI.h"

class RefreshServerTreeThread : public QThread
{
  Q_OBJECT
public:

  void SetParentUI(MIDASDesktopUI* parent) { m_Parent = parent; }

  virtual void run()
    {
    emit enableActions(false);
    m_Parent->getTreeViewServer()->Update();
    emit enableActions(true);
    emit threadComplete();
    }

signals:
  void threadComplete();
  void enableActions(bool val);

private:
  MIDASDesktopUI* m_Parent;
};

#endif
