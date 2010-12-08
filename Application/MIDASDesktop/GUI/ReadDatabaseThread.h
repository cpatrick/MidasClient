#ifndef __ReadDatabaseThread_H
#define __ReadDatabaseThread_H

#include <QThread>
class MIDASDesktopUI;

/**
 * Thread for grabbing the resource tree from the database at startup
 */
class ReadDatabaseThread : public QThread
{
  Q_OBJECT
public:

  void SetParentUI(MIDASDesktopUI* parent) { m_Parent = parent; }

  virtual void run();

signals:
  void enableActions(bool val);
  void threadComplete();

private:
  MIDASDesktopUI* m_Parent;
};

#endif
