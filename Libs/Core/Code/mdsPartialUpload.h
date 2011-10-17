/*=========================================================================

  Copyright (c) Kitware Inc. All rights reserved.
  See Copyright.txt or http://www.Kitware.com/Copyright.htm for details.

     This software is distributed WITHOUT ANY WARRANTY; without even
     the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR
     PURPOSE.  See the above copyright notices for more information.

=========================================================================*/

#ifndef __MDSPARTIALUPLOAD_H
#define __MDSPARTIALUPLOAD_H

#include "midasStandardIncludes.h"

namespace mds
{

class PartialUpload
{
public:
  PartialUpload();
  ~PartialUpload();

  // Create a partial Upload record for a file
  bool Commit();

  // Remove a partial Upload record for a file
  bool Remove();

  // Return a list of all incomplete Uploads
  static bool FetchAll(std::vector<mds::PartialUpload *>& list);

  // Clear all incomplete Uploads
  static bool RemoveAll();

  void SetId(int id);

  int GetId();

  void SetBitstreamId(int id);

  int GetBitstreamId();

  void SetToken(const std::string& token);

  std::string GetToken();

  void SetUserId(int id);

  int GetUserId();

  void SetParentItem(int id);

  int GetParentItem();

protected:
  int         Id;
  int         BitstreamId;
  int         ParentItem;
  int         UserId;
  std::string Token;
};

} // end namespace mds
#endif
