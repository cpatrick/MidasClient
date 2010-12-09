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

  void SetDatabase(midasDatabaseProxy* db) { m_Database = db; }

  virtual void run();

  // Stop filesystem polling
  void Pause();
  // Resume filesystem polling
  void Resume();

signals:
  void needToRefresh();

private:
  midasDatabaseProxy* m_Database;
  bool m_Run;
};

#endif
