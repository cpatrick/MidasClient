#ifndef __ExpandTreeThread_H_
#define __ExpandTreeThread_H_

#include "midasStandardIncludes.h"

#include <QThread>
#include <QModelIndex>

class MidasTreeViewServer;
class MidasTreeModelServer;

class ExpandTreeThread : public QThread
{
  Q_OBJECT
public:
  ExpandTreeThread(MidasTreeViewServer* view,
                   MidasTreeModelServer* model,
                   std::string uuid);
  ~ExpandTreeThread();

  virtual void run();

signals:
  void threadComplete();
  void expand(const QModelIndex& index);
  void select(const QModelIndex& index);

private:
  MidasTreeViewServer*  m_ParentUI;
  MidasTreeModelServer* m_ParentModel;
  std::string           m_Uuid;
};

#endif
