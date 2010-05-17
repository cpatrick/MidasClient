#include "ProcessingStatusUI.h"

#include <QMovie>

#include "MidasClientGlobal.h"
#include "Utils.h"
#include "DionysusUI.h"
#include "Logger.h"

ProcessingStatusUI * ProcessingStatusUI::_instance = NULL;

void ProcessingStatusUI::init(DionysusUI *parent)
  {
  assert( _instance == NULL );
  if (_instance == NULL)
    {
    _instance = new ProcessingStatusUI( parent ); 
    }
  else 
    {
    Logger::warning(STR(__func__) + " should be called one time!");
    }
  }

void ProcessingStatusUI::finalize()
  {
  delete _instance ;
  }

void ProcessingStatusUI::activate()
  {
  assert( _instance != NULL );
  if ( _instance != NULL )
    {
    _instance->show(); 
    _instance->progress->start(); 
    _instance->repaint(); 
    }
  }

void ProcessingStatusUI::desactivate()
  {
  assert( _instance != NULL );
  if ( _instance != NULL )
    {
    _instance->hide(); 
    _instance->progress->stop();
    }
  }

ProcessingStatusUI::ProcessingStatusUI(DionysusUI *parent): 
  QDialog(parent, Qt::FramelessWindowHint), parent(parent)
  {
  setupUi(this); // this sets up GUI
  this->progress = new QMovie(":icons/ajax-loader.gif"); 
  this->label->setMovie( this->progress  ); 
  }

ProcessingStatusUI::~ProcessingStatusUI()
  {
  delete this->progress;
  }

//int ProcessingStatusUI::exec()
//  {
//  return QDialog::exec(); 
//  }