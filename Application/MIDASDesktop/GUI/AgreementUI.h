#ifndef __AgreementUI_H
#define __AgreementUI_H

#include "ui_AgreementUI.h"
#include <QDialog>

class MIDASDesktopUI;
class GUIAgreement;

class AgreementUI : public QDialog,private Ui::AgreementDialog
{
  Q_OBJECT
public:
  AgreementUI(MIDASDesktopUI* parent, GUIAgreement* controller);

public slots:
  void exec();
  void openBrowser();

private:
  MIDASDesktopUI* m_Parent;
  GUIAgreement*   m_Controller;
};

#endif
