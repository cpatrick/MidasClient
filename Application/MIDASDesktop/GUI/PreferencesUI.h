#ifndef __PreferencesUI_H
#define __PreferencesUI_H
 
#include "ui_PreferencesUI.h"

class MIDASDesktopUI;
class UnifyTreeThread;

class PreferencesUI :  public QDialog, private Ui::PreferencesDialog
{
  Q_OBJECT
 
public:
  PreferencesUI(MIDASDesktopUI *parent);
  ~PreferencesUI();

public slots:
  void enableActions(int index);
  void selectWorkingDir();
  void reset(); 
  int exec();
  virtual void accept();

signals:
  void intervalChanged();
  void settingChanged();

private:
  MIDASDesktopUI* m_parent;

  bool m_UnifiedTree;
  UnifyTreeThread* m_UnifyTreeThread;
};

#endif
