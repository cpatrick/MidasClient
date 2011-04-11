#ifndef __MirrorPickerUI_H
#define __MirrorPickerUI_H
 
#include "ui_MirrorPickerUI.h"
#include "midasStandardIncludes.h"

namespace mdo {
  class Assetstore;
  class Bitstream;
}

/**
 * Modal dialog that presents the user with a list of available mirrors
 */
class MirrorPickerUI :  public QDialog, private Ui::MirrorPickerDialog
{
  Q_OBJECT
 
public:
  MirrorPickerUI(QWidget* parent);
  ~MirrorPickerUI();

  mdo::Assetstore* GetSelectedLocation();
  bool ApplyToAll();

public slots:

  int exec(mdo::Bitstream* bitstream);
protected:
  
};

#endif
