#ifndef __SynchronizerThread_H
#define __SynchronizerThread_H

#include <QThread>
#include "MIDASDesktopUI.h"
#include "midasSynchronizer.h"

/**
 * Thread for pulling and pushing
 */
class SynchronizerThread : public QThread
{
  Q_OBJECT
public:

  void SetParentUI(MIDASDesktopUI* parent) { m_Parent = parent; }

  virtual void run()
    {
    emit enableActions(false);
    emit performReturned(m_Parent->getSynchronizer()->Perform());
    emit enableActions(true);
    emit threadComplete();
    }

signals:
  void enableActions(bool val);
  void performReturned(int rc);
  void threadComplete();

private:
  MIDASDesktopUI* m_Parent;
};

#endif
