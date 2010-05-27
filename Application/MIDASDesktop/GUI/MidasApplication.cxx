#include "MidasApplication.h"

#include <QPlastiqueStyle>
#include <QMessageBox>

#include "MIDASDesktopUI.h"
#include "Utils.h"
#include "Logger.h"
#include "MidasClientGlobal.h"

MidasApplication* MidasApplication::m_instance = NULL; 

MidasApplication * MidasApplication::instance()
  {
  if (MidasApplication::m_instance == NULL)
    {
    assert(qApp != NULL); 
    MidasApplication::m_instance = dynamic_cast<MidasApplication*>(qApp); 
    }
  assert(MidasApplication::m_instance != NULL); 
  return MidasApplication::m_instance; 
  }

MidasApplication::MidasApplication ( int & argc, char ** argv):
QApplication(argc, argv)
  {
  this->setStyle(new QPlastiqueStyle()); 
  }

int MidasApplication::exec()
  {
  Q_INIT_RESOURCE(MIDASDesktopIcons);

  this->MIDASDesktop = new MIDASDesktopUI();

  MIDASDesktop->show(); 

  //MIDASDesktop->signInOrOut(); 

  int code = QApplication::exec(); 
  delete this->MIDASDesktop; 
  return code; 
  }

bool MidasApplication::notify ( QObject * receiver, QEvent * event )
  {
  try
    {
    return QApplication::notify(receiver, event); 
    }
  catch (const std::exception& e)
    {
    Logger::error("Notify event", &e);
    assert( this->MIDASDesktop != NULL); 
    //this->MIDASDesktop->signOut();
    QMessageBox::critical(this->MIDASDesktop, "MIDAS Desktop error", STR2QSTR(e.what()));
    }
  return false; 
  }

