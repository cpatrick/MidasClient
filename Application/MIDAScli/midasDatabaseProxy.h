/*=========================================================================

  Copyright (c) Kitware Inc. All rights reserved.
  See Copyright.txt or http://www.Kitware.com/Copyright.htm for details.

     This software is distributed WITHOUT ANY WARRANTY; without even
     the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR
     PURPOSE.  See the above copyright notices for more information.

=========================================================================*/

#ifndef MIDASSQLITEPROXY_H
#define MIDASSQLITEPROXY_H

#include "midasStandardIncludes.h"
#include <mdsSQLiteDatabase.h>

class midasDatabaseProxy
{
public:
  midasDatabaseProxy(std::string database);
  ~midasDatabaseProxy();

  bool Open();
  bool Close();

  /**
   * Clean entries in the database
   */
  void Clean();

  /**
   * Get the absolute path to the given resource
   */
  std::string GetResourceLocation(std::string uuid);

  int InsertBitstream(std::string path, std::string name);
  int InsertCollection(std::string name);
  int InsertCommunity(std::string name);
  int InsertItem(std::string name);

  /**
   * Add a child/parent relationship to the database
   */
  bool AddChild(int parentType, int parentId, int childType, int childId);

  /** 
   * Add a resource record to the database
   */
  void InsertResourceRecord(int type, int id,
                            std::string path, std::string uuid);

  /**
   * Returns whether or not a resource with the given uuid exists
   */
  bool ResourceExists(std::string uuid);
protected:
  mds::SQLiteDatabase* Database;
  std::string DatabasePath;
};

#endif