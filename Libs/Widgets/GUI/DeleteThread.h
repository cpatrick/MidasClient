#ifndef __DeleteThread_H
#define __DeleteThread_H

#include <QString>
#include <QThread>

namespace mdo
{
class Object;
}

class MidasTreeItem;
class Midas3TreeItem;

/**
 * Thread used to delete a resource from the local tree
 */
class DeleteThread : public QThread
{
  Q_OBJECT
public:
  DeleteThread();
  ~DeleteThread();

  void SetResource(MidasTreeItem* resource);

  void SetResource3(Midas3TreeItem* resource);

  void SetDeleteOnDisk(bool val);

  virtual void run();

signals:
  void deletedResource(mdo::Object *);

  void enableActions(bool);

  void errorMessage(const QString &);

private:
  MidasTreeItem*  m_Resource;
  Midas3TreeItem* m_Resource3;
  bool            m_DeleteOnDisk;
};

#endif
