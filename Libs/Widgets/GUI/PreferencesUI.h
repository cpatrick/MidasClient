#ifndef __PreferencesUI_H
#define __PreferencesUI_H

#include "ui_PreferencesUI.h"

class UnifyTreeThread;

class PreferencesUI :  public QDialog, private Ui::PreferencesDialog
{
  Q_OBJECT
public:
  PreferencesUI(QWidget* parent);
  ~PreferencesUI();
public slots:
  void enableActions(int index);

  void unifyTree();

  void selectWorkingDir();

  void unifyTreeDone();

  void reset();

  int exec();

  virtual void accept();

signals:
  void intervalChanged();

  void settingChanged();

  void unifyingTree(); // start

  void treeUnified();  // finish

protected:
  bool             m_UnifiedTree;
  UnifyTreeThread* m_UnifyTreeThread;
};

#endif
