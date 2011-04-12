#ifndef __AddBitstreamsThread_H_
#define __AddBitstreamsThread_H_

#include "midasStandardIncludes.h"

#include <QThread>
#include <QStringList>

class MidasItemTreeItem;

class AddBitstreamsThread : public QThread
{
  Q_OBJECT
public:
  AddBitstreamsThread();
  ~AddBitstreamsThread();
                     
  void SetFiles(const QStringList& files);
  void SetParentItem(MidasItemTreeItem* parentItem);

  virtual void run();

signals:
  void threadComplete();
  void progress(int, int, const QString&);

private:
  MidasItemTreeItem*  m_ParentItem;
  QStringList         m_Files;
};

#endif
