#ifndef __DeleteThread_H
#define __DeleteThread_H

#include <QString>
#include <QThread>

namespace mdo {
  class Object;
}

class MidasTreeItem;

/**
 * Thread used to delete a resource from the local tree
 */
class DeleteThread : public QThread
{
  Q_OBJECT
public:
  DeleteThread();
  ~DeleteThread();
                     
  void SetResource(MidasTreeItem* parentItem);
  void SetDeleteOnDisk(bool val);

  virtual void run();

signals:
  void deletedResource(mdo::Object*);
  void enableActions(bool);
  void errorMessage(const QString&);
  void threadComplete();

private:
  MidasTreeItem* m_Resource;
  bool           m_DeleteOnDisk;
};

#endif
