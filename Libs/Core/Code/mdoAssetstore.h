/*=========================================================================

  Copyright (c) Kitware Inc. All rights reserved.
  See Copyright.txt or http://www.Kitware.com/Copyright.htm for details.

     This software is distributed WITHOUT ANY WARRANTY; without even
     the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR
     PURPOSE.  See the above copyright notices for more information.

=========================================================================*/

#ifndef _mdoAssetstore_h_
#define _mdoAssetstore_h_

#include "mdoObject.h"

namespace mdo
{

/** This class represents an assetstore on the MIDAS server. */
class Assetstore
{
public:
  // This must be kept in sync with the definitions on the server
  enum AssetstoreType
    {
    ASSETSTORE_LOCAL = 0,
    ASSETSTORE_MOUNTED_DRIVE = 1,
    ASSETSTORE_HTTP = 2,
    ASSETSTORE_S3 = 3,
    ASSETSTORE_BITTORRENT = 4,
    ASSETSTORE_REMOTE_MIDAS = 5
    };

  Assetstore();
  ~Assetstore();

  void SetId(int id);
  int GetId();

  void SetEnabled(bool val);
  bool IsEnabled();

  // Set/Get the name of the assetstore
  void SetName(std::string name);
  std::string & GetName();

  // Set/Get the path or URL of the assetstore
  void SetPath(std::string path);
  std::string GetPath();

  // Set/Get the type of the assetstore
  void SetType(AssetstoreType type);
  AssetstoreType GetType();

  // Set/Get the internal id of a bitstream in the assetstore
  // (used for bitstream location)
  void SetInternalId(std::string str);
  std::string GetInternalId();

protected:
  int            m_Id;
  bool           m_Enabled;
  std::string    m_Name;
  std::string    m_Path;
  AssetstoreType m_Type;
  std::string    m_InternalId;
};

} // end namespace

#endif // _mdoAssetstore_h_
