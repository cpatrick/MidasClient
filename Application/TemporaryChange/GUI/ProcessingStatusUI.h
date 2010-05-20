#ifndef __ProcessingStatusUI_H
#define __ProcessingStatusUI_H

#include "ui_ProcessingStatusUI.h"

class MIDASDesktopUI; 
class QMovie; 

class ProcessingStatusUI :  public QDialog, private Ui::ProcessingStatusWidget
{
  Q_OBJECT

public:
  static void init(MIDASDesktopUI *parent);
  static void finalize();

  static void activate(); 
  static void desactivate();

public slots:
  //int exec();

protected:
  ProcessingStatusUI(MIDASDesktopUI *parent);
  ~ProcessingStatusUI(); 
  static ProcessingStatusUI * _instance; 
  QMovie * progress; 
  MIDASDesktopUI * parent; 
};

#endif //__ProcessingStatusUI_H