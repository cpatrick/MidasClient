#include "GUIMirrorHandler.h"
#include "MirrorPickerUI.h"

GUIMirrorHandler::GUIMirrorHandler(QWidget* parent)
: m_ParentWidget(parent)
{
  m_MirrorDialog = new MirrorPickerUI(parent);
}

GUIMirrorHandler::~GUIMirrorHandler()
{
  delete m_MirrorDialog;
}

mdo::Assetstore* GUIMirrorHandler::HandleMirroredBitstream(
  mdo::Bitstream* bitstream)
{
  m_MirrorDialog->exec(bitstream);
  //todo handle apply to all functionality
  return m_MirrorDialog->GetSelectedLocation();
}

