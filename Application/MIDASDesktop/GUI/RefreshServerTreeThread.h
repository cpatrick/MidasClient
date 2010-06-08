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
    m_Parent->displayStatus("Refreshing server tree...");
    m_Parent->setProgressIndeterminate();
    m_Parent->getTreeView()->Update();
    m_Parent->setProgressEmpty();
    m_Parent->displayStatus("");
    emit enableRefresh(true);
    }

signals:
  void enableRefresh(bool val);

private:
  MIDASDesktopUI* m_Parent;
};

#endif