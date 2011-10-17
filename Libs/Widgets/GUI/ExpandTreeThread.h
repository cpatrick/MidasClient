#ifndef __ExpandTreeThread_H_
#define __ExpandTreeThread_H_

#include "midasStandardIncludes.h"

#include <QThread>
#include <QModelIndex>

class MidasTreeViewServer;
class MidasTreeModelServer;

class Midas3TreeViewServer;
class Midas3TreeModelServer;

class ExpandTreeThread : public QThread
{
  Q_OBJECT
public:
  ExpandTreeThread(MidasTreeViewServer* view,
                   MidasTreeModelServer* model,
                   std::string uuid,
                   bool select);
  ExpandTreeThread(Midas3TreeViewServer* view,
                   Midas3TreeModelServer* model,
                   std::string uuid,
                   bool select);
  ~ExpandTreeThread();

  virtual void run();

signals:
  void expand(const QModelIndex& index);
  void select(const QModelIndex& index);
  void enableActions(bool val);

private:
  MidasTreeViewServer*   m_ParentUI;
  MidasTreeModelServer*  m_ParentModel;
  Midas3TreeViewServer*  m_ParentUI3;
  Midas3TreeModelServer* m_ParentModel3;
  std::string            m_Uuid;
  bool                   m_Select;
  int                    m_Version;
};

#endif
