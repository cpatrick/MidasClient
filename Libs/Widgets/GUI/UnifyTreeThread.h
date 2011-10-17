#ifndef __UnifyTreeThread_H_
#define __UnifyTreeThread_H_

#include <QThread>
#include <QString>

class UnifyTreeThread : public QThread
{
  Q_OBJECT
public:
  UnifyTreeThread();
  ~UnifyTreeThread();

  virtual void run();

  bool isCopy();
  void setCopy(bool val);

private:
  bool            m_Copy;
};

#endif
