#ifndef __FileOverwriteUI_H
#define __FileOverwriteUI_H

#include "ui_FileOverwriteUI.h"
#include <QDialog>

class FileOverwriteUI : public QDialog, private Ui::FileOverwriteDialog
{
  Q_OBJECT
public:
  FileOverwriteUI(QWidget* parent);
  ~FileOverwriteUI();

  bool ShouldOverwrite();

  bool ShouldApplyToAll();

public slots:
  void setPath(const std::string& path);

  void overwrite();

  void useExisting();

  void exec();

signals:
  void selectionMade(int val, bool applyToAll);

private:
  std::string m_Path;
  bool        m_Overwrite;
};

#endif
