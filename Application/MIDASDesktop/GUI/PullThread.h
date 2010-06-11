#ifndef __PullThread_H
#define __PullThread_H

#include <QThread>
#include "MIDASDesktopUI.h"
#include "midasSynchronizer.h"

class PullThread : public QThread
{
  Q_OBJECT
public:

  void SetParentUI(MIDASDesktopUI* parent) { m_Parent = parent; }

  virtual void run()
    {
    emit enableActions(false);
    emit performReturned(m_Parent->getSynchronizer()->Perform() == 0);
    emit enableActions(true);
    emit threadComplete();
    }

signals:
  void enableActions(bool val);
  void performReturned(bool val);
  void threadComplete();

private:
  MIDASDesktopUI* m_Parent;
};

#endif