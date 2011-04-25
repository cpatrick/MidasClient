#include "MirrorPickerUI.h"
#include "mdoAssetstore.h"
#include "mdoBitstream.h"

MirrorPickerUI::MirrorPickerUI(QWidget* parent)
: QDialog(parent)
{
  setupUi(this);
  this->setModal(true);
  mirrorListLayout->setAlignment(Qt::AlignTop);
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

  this->ClearMirrorList();

  for(std::vector<mdo::Assetstore*>::iterator i =
    bitstream->GetLocations().begin();
    i != bitstream->GetLocations().end(); ++i)
    {
    std::stringstream text;
    text << ((*i)->GetName() == "" ?
      "MIDAS Default Assetstore" : (*i)->GetName());

    text << " (";
    switch((*i)->GetType())
      {
      case mdo::Assetstore::ASSETSTORE_LOCAL:
        text << "Local - ";
        break;
      case mdo::Assetstore::ASSETSTORE_HTTP:
        text << "HTTP - ";
        break;
      case mdo::Assetstore::ASSETSTORE_REMOTE_MIDAS:
        text << "Remote MIDAS - ";
        break;
      default:
        break;
      }
    text << ((*i)->GetPath() == "" ?
      "internal" : (*i)->GetPath());
    text << ")";

    QRadioButton* button = new QRadioButton;
    button->setText(text.str().c_str());
    this->mirrorListLayout->addWidget(button);
    this->m_Mirrors[*i] = button;
    }
  this->infoLabel->setText(infoText.str().c_str());
  return QDialog::exec();
}

void MirrorPickerUI::ClearMirrorList()
{
  for(MirrorMap::iterator i = m_Mirrors.begin(); i != m_Mirrors.end(); ++i)
    {
    delete i->second;
    }
  m_Mirrors.clear();
}

mdo::Assetstore* MirrorPickerUI::GetSelectedLocation()
{
  for(MirrorMap::iterator i = m_Mirrors.begin(); i != m_Mirrors.end(); ++i)
    {
    if(i->second->isChecked())
      {
      return i->first;
      }
    }
  return NULL;
}

bool MirrorPickerUI::ApplyToAll()
{
  return this->applyToAllCheckbox->isChecked();
}
