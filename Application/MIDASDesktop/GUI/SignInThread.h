#ifndef __SignInThread_H_
#define __SignInThread_H_

#include <QThread>
#include <QString>

class MIDASDesktopUI;

class SignInThread : public QThread
{
  Q_OBJECT
public:
  SignInThread(MIDASDesktopUI* parent);
  ~SignInThread();

  void SetProfile(QString profile);

  virtual void run();

signals:
  void initialized(bool ok);

private:
  MIDASDesktopUI* m_Parent;
  QString         m_Profile;
};

#endif
