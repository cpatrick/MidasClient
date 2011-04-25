#ifndef __UpdateTreeViewThread_H
#define __UpdateTreeViewThread_H

#include <QThread>

class MidasTreeView;

/**
 * Thread for updating a MIDAS tree view widget
 */
class UpdateTreeViewThread : public QThread
{
  Q_OBJECT
public:

  UpdateTreeViewThread(MidasTreeView* treeView);
  ~UpdateTreeViewThread();

  virtual void run();

signals:
  void enableActions(bool val);

protected:
  MidasTreeView* m_TreeView;
};

#endif
