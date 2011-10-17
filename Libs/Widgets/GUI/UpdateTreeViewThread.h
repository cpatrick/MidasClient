#ifndef __UpdateTreeViewThread_H
#define __UpdateTreeViewThread_H

#include <QThread>

class MidasTreeViewBase;

/**
 * Thread for updating a MIDAS tree view widget
 */
class UpdateTreeViewThread : public QThread
{
  Q_OBJECT
public:

  UpdateTreeViewThread(MidasTreeViewBase* treeView);
  ~UpdateTreeViewThread();

  virtual void run();

signals:
  void enableActions(bool val);

protected:
  MidasTreeViewBase* m_TreeView;
};

#endif
