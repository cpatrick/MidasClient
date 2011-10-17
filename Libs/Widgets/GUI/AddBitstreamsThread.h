#ifndef __AddBitstreamsThread_H_
#define __AddBitstreamsThread_H_

#include "midasStandardIncludes.h"

#include <QThread>
#include <QStringList>

class MidasItemTreeItem;
class Midas3ItemTreeItem;

class AddBitstreamsThread : public QThread
{
  Q_OBJECT
public:
  AddBitstreamsThread();
  ~AddBitstreamsThread();
                     
  void SetFiles(const QStringList& files);
  void SetParentItem(MidasItemTreeItem* parentItem);
  void SetParentItem(Midas3ItemTreeItem* parentItem);

  virtual void run();

signals:
  void enableActions(bool);
  void progress(int, int, const QString&);

private:
  MidasItemTreeItem*  m_ParentItem;
  Midas3ItemTreeItem* m_ParentItem3;
  QStringList         m_Files;
};

#endif
