#include "GUIMirrorHandler.h"
#include "MirrorPickerUI.h"
#include "mdoAssetstore.h"
#include "mdoBitstream.h"

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

  // If the user has already selected to always use certain assetstores,
  // we will skip the dialog and use it if possible
  for(std::set<int>::iterator i = m_PreferredLocations.begin();
      i != m_PreferredLocations.end(); ++i)
    {
    for(std::vector<mdo::Assetstore*>::iterator loc =
        bitstream->GetLocations().begin();
        loc != bitstream->GetLocations().end(); ++loc)
      {
      if((*loc)->GetId() == *i)
        {
        return *loc;
        }
      }
    }
  emit prompt(bitstream);

  while(!m_Done)
    {
    #if defined(_WIN32)
      Sleep(25);
    #else
      usleep(25000);
    #endif
    }
  mdo::Assetstore* selectedLocation = m_MirrorDialog->GetSelectedLocation();
  if(m_MirrorDialog->ApplyToAll())
    {
    m_PreferredLocations.insert(selectedLocation->GetId());
    }
  return selectedLocation;
}

void GUIMirrorHandler::dialogAccepted()
{
  m_Done = true;
}
