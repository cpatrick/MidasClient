#ifndef __AgreementUI_H
#define __AgreementUI_H

#include "ui_AgreementUI.h"
#include <QDialog>
#include <QString>

class MIDASDesktopUI;

class AgreementUI : public QDialog,private Ui::AgreementDialog
{
  Q_OBJECT
public:
  AgreementUI(QWidget* parent);
  ~AgreementUI();

  void SetUrl(const QString& url);
  bool WasCanceled();

signals:
  void errorMessage(const QString&);

public slots:
  void reject();

  void exec();
  void openBrowser();

private:
  QString         m_Url;
  MIDASDesktopUI* m_Parent;
  bool            m_Canceled;
};

#endif
