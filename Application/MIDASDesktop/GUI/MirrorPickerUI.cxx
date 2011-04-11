#include "MirrorPickerUI.h"
#include "mdoBitstream.h"

MirrorPickerUI::MirrorPickerUI(QWidget* parent)
: QDialog(parent)
{
  setupUi(this);
}

MirrorPickerUI::~MirrorPickerUI()
{
}

int MirrorPickerUI::exec(mdo::Bitstream* bitstream)
{
  std::stringstream infoText;
  infoText << "The bitstream <b>" << bitstream->GetName() <<
    "</b> is mirrored in multiple locations.  Please select a download "
    "mirror from the list below.  Preferably choose the one closest to you.";

  this->infoLabel->setText(infoText.str().c_str());
  return QDialog::exec();
}

mdo::Assetstore* MirrorPickerUI::GetSelectedLocation()
{
  return NULL;
}

bool MirrorPickerUI::ApplyToAll()
{
  return this->applyToAllCheckbox->isChecked();
}
