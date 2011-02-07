#ifndef __FileOverwriteUI_H
#define __FileOverwriteUI_H

#include "ui_FileOverwriteUI.h"
#include <QDialog>

class MIDASDesktopUI;
class GUIFileOverwriteHandler;

class FileOverwriteUI : public QDialog,private Ui::FileOverwriteDialog
{
  Q_OBJECT
public:
  FileOverwriteUI(MIDASDesktopUI* parent, GUIFileOverwriteHandler* controller);
  ~FileOverwriteUI();

public slots:
  void setPath(const QString& path);
  void overwrite();
  void useExisting();
  void exec();

signals:
  void selectionMade(int val, bool applyToAll);

private:
  MIDASDesktopUI* m_Parent;
  GUIFileOverwriteHandler* m_Controller;
  std::string m_Path;
};

#endif
