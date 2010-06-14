#ifndef __ExpandTreeThread_H_
#define __ExpandTreeThread_H_

#include "midasStandardIncludes.h"

#include <QThread>
#include <QModelIndex>

class MidasTreeView;
class MidasTreeModel;

namespace mdo {
  class Object;
}

class ExpandTreeThread : public QThread
{
  Q_OBJECT
public:
  ExpandTreeThread();
  ~ExpandTreeThread();

  void SetParentUI(MidasTreeView* parent);
  void SetParentModel(MidasTreeModel* parent);
  void SetObject(mdo::Object* object);

  virtual void run();

signals:
  void threadComplete();
  void expand(const QModelIndex& index);
  void select(const QModelIndex& index);

private:
  MidasTreeView*  m_ParentUI;
  MidasTreeModel* m_ParentModel;
  mdo::Object*    m_Object;
};

#endif
