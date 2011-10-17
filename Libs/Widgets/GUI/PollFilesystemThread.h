#ifndef __PollFilesystemThread_H
#define __PollFilesystemThread_H

#include <QThread>
class midasDatabaseProxy;

/**
 * This thread is always running while the GUI is not minimized.
 * It continually polls the filesystem to see if files were
 * deleted or modified, and emits the needToRefresh() signal
 * if changes were made to any bitstreams under the database's
 * control.
 */
class PollFilesystemThread : public QThread
{
  Q_OBJECT
public:
  PollFilesystemThread();
  ~PollFilesystemThread();

  virtual void run();

public slots:
  // Stop filesystem polling and wait for current poll to finish
  void Pause();

  // Resume filesystem polling
  void Resume();

  // End this thread permanently
  void Terminate();

signals:
  void needToRefresh();

  void paused();

private:
  bool m_Run;
  bool m_Terminate;
  bool m_DatabaseLocked;
};

#endif
