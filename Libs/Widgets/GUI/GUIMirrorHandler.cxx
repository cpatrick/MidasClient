/******************************************************************************
 * Copyright 2011 Kitware Inc.
 *
 * Licensed under the Apache License, Version 2.0 (the "License");
 * you may not use this file except in compliance with the License.
 * You may obtain a copy of the License at
 *
 *     http://www.apache.org/licenses/LICENSE-2.0
 *
 * Unless required by applicable law or agreed to in writing, software
 * distributed under the License is distributed on an "AS IS" BASIS,
 * WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
 * See the License for the specific language governing permissions and
 * limitations under the License.
 *****************************************************************************/
#include "GUIMirrorHandler.h"
#include "MirrorPickerUI.h"
#include "mdoAssetstore.h"
#include "mdoBitstream.h"

GUIMirrorHandler::GUIMirrorHandler(MirrorPickerUI* dialog)
  : m_MirrorDialog(dialog)
{
}

GUIMirrorHandler::~GUIMirrorHandler()
{
}

mdo::Assetstore * GUIMirrorHandler::HandleMirroredBitstream(
  mdo::Bitstream* bitstream)
{
  // If the user has already selected to always use certain assetstores,
  // we will skip the dialog and use it if possible
  for( std::set<int>::iterator i = m_PreferredLocations.begin();
       i != m_PreferredLocations.end(); ++i )
    {
    for( std::vector<mdo::Assetstore *>::iterator loc =
           bitstream->GetLocations().begin();
         loc != bitstream->GetLocations().end(); ++loc )
      {
      if( (*loc)->GetId() == *i )
        {
        return *loc;
        }
      }
    }
  emit prompt(bitstream); // blocking connection

  mdo::Assetstore* selectedLocation = m_MirrorDialog->GetSelectedLocation();
  if( m_MirrorDialog->ApplyToAll() )
    {
    m_PreferredLocations.insert(selectedLocation->GetId() );
    }
  return selectedLocation;
}

