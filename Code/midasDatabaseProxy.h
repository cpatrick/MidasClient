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
#include "midasStatus.h"
#include "mdsSQLiteDatabase.h"

struct midasResourceRecord
{
  midasResourceRecord() :
    Type(midasResourceType::TYPE_ERROR), Id(0), Parent(0), Path("") {}
  int Type;
  int Id;
  int Parent;
  std::string Path;
};

class midasDatabaseProxy
{
public:
  midasDatabaseProxy(std::string database);
  ~midasDatabaseProxy();

  mds::SQLiteDatabase* GetDatabase();

  bool Open();
  bool Close();

  /**
   * Clean entries in the database
   */
  void Clean();

  std::string GetLastUsedURL();
  void SetLastUsedURL(std::string url);

  std::string GetName(int type, int id);

  int GetParentId(int type, int id);

  /**
   * Creates a new resource and uuid record, and its
   * corresponding parent/child entry, iff it doesn't exist.
   * If it does exist, returns its id.
   */
  int AddResource(int type, std::string uuid, std::string path,
    std::string name, int parentType, int parentId, int serverParent);
  int AddResource(int type, std::string uuid, std::string path,
    std::string name, std::string parentUuid, int serverParent);

  bool AddAuthProfile(std::string user, std::string appName,
    std::string apiKey, std::string profileName);
  bool GetAuthProfile(std::string name, std::string& user,
    std::string& appName, std::string& apiKey);

  /**
   * Returns whether or not a resource with the given uuid exists
   */
  bool ResourceExists(std::string uuid);

  int GetIdForUuid(std::string uuid);
  midasResourceRecord GetRecordByUuid(std::string uuid);

  /**
   * Add a child/parent relationship to the database
   */
  bool AddChild(int parentType, int parentId, int childType, int childId);
  bool AddChild(std::string parentUuid, int childType, int childId);

  std::string GetUuidFromPath(std::string path);
  std::string GetUuid(int type, int id);

  void MarkDirtyResource(std::string uuid, int dirtyAction);
  void ClearDirtyResource(std::string uuid);
  int IsResourceDirty(std::string uuid);

  /**
   * Returns a list of dirty resources on the client
   */
  std::vector<midasStatus> GetStatusEntries();
protected:
  void InsertResourceRecord(int type, int id,
                            std::string path, std::string uuid, int parentId);

  int InsertBitstream(std::string path, std::string name);
  int InsertCollection(std::string name);
  int InsertCommunity(std::string name);
  int InsertItem(std::string name);

  mds::SQLiteDatabase* Database;
  std::string DatabasePath;
};

#endif