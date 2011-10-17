#ifndef __UpgradeUI_H
#define __UpgradeUI_H

#include "ui_UpgradeUI.h"
#include "midasStandardIncludes.h"
#include <QDialog>
#include <QString>

class UpgradeUI : public QDialog, private Ui::UpgradeDialog
{
  Q_OBJECT
public:
  UpgradeUI(QWidget* parent);
  ~UpgradeUI();

  void SetProductVersion(const std::string& version);
  void SetDbVersion(const std::string& version);

public slots:

  int exec();

signals:

  void errorMessage(const QString&);

protected:
  std::string m_ProductVersion;
  std::string m_DbVersion;
};

#endif
