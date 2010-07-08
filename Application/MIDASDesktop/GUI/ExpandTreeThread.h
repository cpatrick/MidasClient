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
  ExpandTreeThread();
  ~ExpandTreeThread();

  void SetParentUI(MidasTreeViewServer* parent);
  void SetParentModel(MidasTreeModelServer* parent);
  void SetUuid(std::string uuid);

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
