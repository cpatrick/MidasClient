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
    emit enableRefresh(false);
    m_Parent->getTreeView()->Update();
    emit enableRefresh(true);
    emit threadComplete();
    }

signals:
  void enableRefresh(bool val);
  void threadComplete();

private:
  MIDASDesktopUI* m_Parent;
};

#endif