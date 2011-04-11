#include "GUIMirrorHandler.h"
#include "MirrorPickerUI.h"

#if defined(_WIN32)
# include <windows.h>
#else
# include <unistd.h>
#endif

GUIMirrorHandler::GUIMirrorHandler(MirrorPickerUI* dialog)
{
  m_MirrorDialog = dialog;
}

GUIMirrorHandler::~GUIMirrorHandler()
{
}

mdo::Assetstore* GUIMirrorHandler::HandleMirroredBitstream(
  mdo::Bitstream* bitstream)
{
  m_Done = false;
  emit prompt(bitstream);

  while(!m_Done)
    {
    #if defined(_WIN32)
      Sleep(25);
    #else
      usleep(25000);
    #endif
    }
  //TODO handle apply to all functionality
  return m_MirrorDialog->GetSelectedLocation();
}

void GUIMirrorHandler::dialogAccepted()
{
  m_Done = true;
}
