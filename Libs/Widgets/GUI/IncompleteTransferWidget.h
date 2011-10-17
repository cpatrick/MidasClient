#ifndef __IncompleteTransferWidget_H
#define __IncompleteTransferWidget_H

#include <QTableWidget>
#include <QPushButton>

#include "midasStandardIncludes.h"
#include "mdsPartialDownload.h"

class midasSynchronizer;
class SynchronizerThread;

namespace mds
{
class PartialDownload;
class PartialUpload;
}

class IncompleteTransferWidget : public QWidget
{
  Q_OBJECT
public:
  IncompleteTransferWidget(QWidget* parent, midasSynchronizer* synch);
  ~IncompleteTransferWidget();
public slots:
  void Populate();

  void Clear();

  void SetEnabled(bool value);

protected slots:
  void ResumeDownloadPressed();

  void ResumeUploadPressed();

  void RemoveAllPartials();

  void ResumeDownloadCompleted(int);

  void ResumeUploadCompleted(int);

signals:
  void ActivateActions(bool);

  void DownloadStarted();

  void UploadStarted();

  void DownloadComplete();

  void UploadComplete();

protected:
  QTableWidget*                       m_Table;
  QPushButton*                        m_RemoveAllButton;
  midasSynchronizer*                  m_Synch;
  SynchronizerThread*                 m_SynchThread;
  std::vector<mds::PartialDownload *> m_Downloads;
  std::vector<mds::PartialUpload *>   m_Uploads;
  std::vector<QPushButton *>          m_ResumeDownloadButtons;
  std::vector<QPushButton *>          m_ResumeUploadButtons;
  bool                                m_Enable;
  mds::PartialDownload*               m_CurrentDownload;
  mds::PartialUpload*                 m_CurrentUpload;
};

#endif
