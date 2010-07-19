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

signals:
  void threadComplete(bool ok);
  void message(const QString& val);

private:
  MIDASDesktopUI* m_Parent;
};

#endif
