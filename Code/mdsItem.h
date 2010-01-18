/*=========================================================================

  Copyright (c) Kitware Inc. All rights reserved.
  See Copyright.txt or http://www.Kitware.com/Copyright.htm for details.

     This software is distributed WITHOUT ANY WARRANTY; without even
     the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR
     PURPOSE.  See the above copyright notices for more information.

=========================================================================*/

#ifndef _mdsItem_h_
#define _mdsItem_h_

#include <string>
#include <vector>

#include "mdsObject.h"
#include "mdsBitstream.h"

namespace mdo{
  class Item;
}

namespace mds{

/** This class represent an item on the MIDAS server. 
 *  An item has many bitstreams. */
class Item : public Object
{
public:

  Item();
  ~Item();

  // Fetch data from the MIDAS server
  bool Fetch();
  
  // Commit the data on the server 
  bool Commit();
  
  // Add the object
  void SetObject(mdo::Object* object);

  // Upload a bitstream to MIDAS
  const char* UploadBitstream(const char* filename,const char* uuid=NULL);
  
  // Download a bitstream from MIDAS
  std::string DownloadBitstream(const char* uuid,const char* filename);
   
protected:

  // Create a unique filename in the assetstore
  std::string GenerateAssetstoreFilename(const char* assetstore);

  mdo::Item* m_Item;

};

} //end namespace

#endif //_mdsItem_h_
