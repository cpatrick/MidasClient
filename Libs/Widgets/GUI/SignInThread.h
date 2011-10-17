#ifndef __SignInThread_H_
#define __SignInThread_H_

#include <QThread>
#include <QString>

class midasSynchronizer;

class SignInThread : public QThread
{
  Q_OBJECT
public:
  SignInThread(midasSynchronizer* synch);
  ~SignInThread();

  void SetProfile(QString profile);

  virtual void run();

signals:
  void initialized(bool ok);

private:
  midasSynchronizer* m_Synch;
  QString            m_Profile;
};

#endif
