#ifndef __UnifyTreeThread_H_
#define __UnifyTreeThread_H_

#include <QThread>
#include <QString>

class MIDASDesktopUI;

class UnifyTreeThread : public QThread
{
  Q_OBJECT
public:
  UnifyTreeThread(MIDASDesktopUI* parent);
  ~UnifyTreeThread();

  virtual void run();

  bool isCopy();
  void setCopy(bool val);

signals:
  void threadComplete();

private:
  MIDASDesktopUI* m_Parent;
  bool            m_Copy;
};

#endif
