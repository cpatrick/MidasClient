#include "IncompleteTransferWidget.h"

#include <QHBoxLayout>
#include <QVBoxLayout>
#include <QPixmap>
#include <QStringList>
#include <QLabel>
#include <QHeaderView>
#include <QFileInfo>

#include "mdsPartialDownload.h"
#include "mdsPartialUpload.h"
#include "mdsBitstream.h"
#include "mdoBitstream.h"
#include "midasStandardIncludes.h"
#include "midasSynchronizer.h"

#include "SynchronizerThread.h"

IncompleteTransferWidget::IncompleteTransferWidget(QWidget* parent, midasSynchronizer* synch)
  : QWidget(parent), m_Synch(synch), m_SynchThread(NULL)
{
  m_Table = new QTableWidget(this);
  m_Table->setColumnCount(3);

  QStringList list;
  list << "Type" << "File" << "Resume";
  m_Table->setHorizontalHeaderLabels(list);
  m_Table->verticalHeader()->setVisible(false);

  m_RemoveAllButton = new QPushButton();
  m_RemoveAllButton->setText("Remove All");
  m_RemoveAllButton->setIcon(QPixmap(":icons/delete.png") );

  connect(m_RemoveAllButton, SIGNAL( released() ),
          this, SLOT( RemoveAllPartials() ) );

  QWidget*     bottom = new QWidget(this);
  QHBoxLayout* bottomLayout = new QHBoxLayout(this);
  bottomLayout->addStretch();
  bottomLayout->addWidget(m_RemoveAllButton);
  bottom->setLayout(bottomLayout);

  this->setLayout(new QVBoxLayout(this) );
  this->layout()->setAlignment(Qt::AlignRight);
  this->layout()->addWidget(m_Table);
  this->layout()->addWidget(bottom);

  m_Enable = false;
  m_CurrentDownload = NULL;
  m_CurrentUpload = NULL;
}

IncompleteTransferWidget::~IncompleteTransferWidget()
{
  this->Clear();
  delete m_Table;
  delete m_RemoveAllButton;
  delete m_SynchThread;
  delete m_CurrentDownload;
  delete m_CurrentUpload;
}

void IncompleteTransferWidget::Populate()
{
  this->Clear();
  mds::PartialDownload::FetchAll(m_Downloads);
  mds::PartialUpload::FetchAll(m_Uploads);
  m_Table->setRowCount(m_Downloads.size() + m_Uploads.size() );

  int row = 0;
  for( std::vector<mds::PartialDownload *>::iterator i = m_Downloads.begin();
       i != m_Downloads.end(); ++i )
    {
    QPushButton* resumeButton = new QPushButton();
    resumeButton->setIcon(QPixmap(":icons/control_play_blue.png") );
    resumeButton->setEnabled(m_Enable);
    m_ResumeDownloadButtons.push_back(resumeButton);
    connect(resumeButton, SIGNAL( released() ),
            this, SLOT( ResumeDownloadPressed() ) );

    QFileInfo fileInfo( (*i)->GetPath().c_str() );

    QLabel* typeLabel = new QLabel(this);
    typeLabel->setPixmap(QPixmap(":icons/arrow_down.png") );
    m_Table->setCellWidget(row, 0, typeLabel);
    m_Table->setCellWidget(row, 1, new QLabel(fileInfo.fileName(), this) );
    m_Table->setCellWidget(row, 2, resumeButton);

    row++;
    }
  for( std::vector<mds::PartialUpload *>::iterator i = m_Uploads.begin();
       i != m_Uploads.end(); ++i )
    {
    QPushButton* resumeButton = new QPushButton();
    resumeButton->setIcon(QPixmap(":icons/control_play_blue.png") );
    resumeButton->setEnabled(m_Enable);
    m_ResumeUploadButtons.push_back(resumeButton);
    connect(resumeButton, SIGNAL( released() ),
            this, SLOT( ResumeUploadPressed() ) );

    mdo::Bitstream bitstream;
    bitstream.SetId( (*i)->GetBitstreamId() );

    mds::Bitstream mdsBitstream;
    mdsBitstream.SetObject(&bitstream);
    mdsBitstream.Fetch();

    QFileInfo fileInfo(bitstream.GetPath().c_str() );

    QLabel* typeLabel = new QLabel(this);
    typeLabel->setPixmap(QPixmap(":icons/arrow_up.png") );
    m_Table->setCellWidget(row, 0, typeLabel);
    m_Table->setCellWidget(row, 1, new QLabel(fileInfo.fileName(), this) );
    m_Table->setCellWidget(row, 2, resumeButton);

    row++;
    }
  m_Table->resizeColumnsToContents();
}

void IncompleteTransferWidget::Clear()
{
  for( std::vector<mds::PartialDownload *>::iterator i = m_Downloads.begin();
       i != m_Downloads.end(); ++i )
    {
    delete *i;
    }
  for( std::vector<QPushButton *>::iterator i = m_ResumeDownloadButtons.begin();
       i != m_ResumeDownloadButtons.end(); ++i )
    {
    delete *i;
    }
  for( std::vector<mds::PartialUpload *>::iterator i = m_Uploads.begin();
       i != m_Uploads.end(); ++i )
    {
    delete *i;
    }
  for( std::vector<QPushButton *>::iterator i = m_ResumeUploadButtons.begin();
       i != m_ResumeUploadButtons.end(); ++i )
    {
    delete *i;
    }
  m_Downloads.clear();
  m_Uploads.clear();
  m_ResumeDownloadButtons.clear();
  m_ResumeUploadButtons.clear();
  m_Table->clearContents();
  m_Table->setRowCount(0);
}

void IncompleteTransferWidget::ResumeDownloadPressed()
{
  // Find which button sent the resume signal
  QObject* sender = QObject::sender();
  int      index = 0;
  for( std::vector<QPushButton *>::iterator i = m_ResumeDownloadButtons.begin();
       i != m_ResumeDownloadButtons.end(); ++i )
    {
    if( *i == sender )
      {
      break;
      }
    index++;
    }
  delete m_CurrentDownload;
  m_CurrentDownload = new mds::PartialDownload();
  *m_CurrentDownload = *(m_Downloads[index]); // save persistent copy
  m_Synch->SetObject(m_CurrentDownload);
  m_Synch->SetOperation(midasSynchronizer::OPERATION_RESUME_DOWNLOAD);

  emit ActivateActions(false);

  delete m_SynchThread;
  m_SynchThread = new SynchronizerThread();
  m_SynchThread->SetSynchronizer(m_Synch);

  connect(m_SynchThread, SIGNAL( performReturned(int) ),
          this, SLOT( ResumeDownloadCompleted(int) ) );

  m_SynchThread->start();
  emit DownloadStarted();
}

void IncompleteTransferWidget::ResumeUploadPressed()
{
  // Find which button sent the resume signal
  QObject* sender = QObject::sender();
  int      index = 0;
  for( std::vector<QPushButton *>::iterator i = m_ResumeUploadButtons.begin();
       i != m_ResumeUploadButtons.end(); ++i )
    {
    if( *i == sender )
      {
      break;
      }
    index++;
    }
  delete m_CurrentUpload;
  m_CurrentUpload = new mds::PartialUpload();
  *m_CurrentUpload = *(m_Uploads[index]); // save persistent copy
  m_Synch->SetObject(m_CurrentUpload);
  m_Synch->SetOperation(midasSynchronizer::OPERATION_RESUME_UPLOAD);

  emit ActivateActions(false);

  delete m_SynchThread;
  m_SynchThread = new SynchronizerThread();
  m_SynchThread->SetSynchronizer(m_Synch);

  connect(m_SynchThread, SIGNAL( performReturned(int) ),
          this, SLOT( ResumeUploadCompleted(int) ) );

  m_SynchThread->start();
  emit UploadStarted();
}

void IncompleteTransferWidget::RemoveAllPartials()
{
  this->Clear();
  mds::PartialDownload::RemoveAll();
  mds::PartialUpload::RemoveAll();
}

void IncompleteTransferWidget::SetEnabled(bool value)
{
  for( std::vector<QPushButton *>::iterator i = m_ResumeDownloadButtons.begin();
       i != m_ResumeDownloadButtons.end(); ++i )
    {
    (*i)->setEnabled(value);
    }
  for( std::vector<QPushButton *>::iterator i = m_ResumeUploadButtons.begin();
       i != m_ResumeUploadButtons.end(); ++i )
    {
    (*i)->setEnabled(value);
    }
  m_Enable = value;
}

void IncompleteTransferWidget::ResumeDownloadCompleted(int rc)
{
  emit ActivateActions(true);

  if( rc == 0 ) // success
    {
    this->Populate();
    emit DownloadComplete();
    }
}

void IncompleteTransferWidget::ResumeUploadCompleted(int rc)
{
  emit ActivateActions(true);

  if( rc == 0 ) // success
    {
    this->Populate();
    emit UploadComplete();
    }
}

