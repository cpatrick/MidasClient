#ifndef __SynchronizerThread_H
#define __SynchronizerThread_H

#include <QThread>
#include "midasSynchronizer.h"

/**
 * Thread for pulling and pushing
 */
class SynchronizerThread : public QThread
{
  Q_OBJECT
public:
  SynchronizerThread() : m_Delete(false) {}
  ~SynchronizerThread() {}

  void SetSynchronizer(midasSynchronizer* synch) { m_Synchronizer = synch; }
  void SetDelete(bool shouldDelete) { m_Delete = shouldDelete; }

  virtual void run()
    {
    emit enableActions(false);
    emit performReturned(m_Synchronizer->Perform());

    if(m_Delete)
      {
      delete m_Synchronizer;
      }
    emit enableActions(true);
    emit threadComplete();
    }

signals:
  void enableActions(bool val);
  void performReturned(int rc);
  void threadComplete();

private:
  midasSynchronizer* m_Synchronizer;
  bool m_Delete;
};

#endif
