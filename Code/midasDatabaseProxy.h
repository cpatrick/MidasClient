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

namespace mdo
{
  class Community;
  class Collection;
  class Item;
  class Bitstream;
}

struct midasResourceRecord
{
  midasResourceRecord() :
    Type(midasResourceType::TYPE_ERROR), Id(0), Parent(0), Path("") {}
  int Type;
  int Id;
  int Parent;
  std::string Path;
};

struct midasAuthProfile
{
  std::string Name;
  std::string Url;
  std::string AppName;
  std::string ApiKey;
  std::string User;

  bool IsAnonymous() { return User == ""; }
};

class midasDatabaseProxy
{
public:
  midasDatabaseProxy(std::string database);
  ~midasDatabaseProxy();

  enum MidasAppSetting
    {
    LAST_URL,
    LAST_FETCH_TIME,
    AUTO_REFRESH_INTERVAL,
    AUTO_REFRESH_SETTING
    };

  mds::SQLiteDatabase* GetDatabase();

  bool Open();
  bool Close();

  /**
   * Clean entries in the database
   */
  void Clean();

  std::string GetSetting(MidasAppSetting setting);
  void SetSetting(MidasAppSetting setting, std::string value);
  void SetSetting(MidasAppSetting setting, int value);

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

  /**
   * Delete a resource from the database
   */
  bool DeleteResource(std::string uuid, bool deleteFiles = false);

  bool FillCommunity(mdo::Community* community);
  bool FillCollection(mdo::Collection* collection);
  bool FillItem(mdo::Item* item);
  bool FillBitstream(mdo::Bitstream* bitstream);

  bool AddAuthProfile(std::string user, std::string appName,
    std::string apiKey, std::string profileName, std::string url);
  midasAuthProfile GetAuthProfile(std::string name);
  std::vector<std::string> GetAuthProfiles();
  void DeleteProfile(std::string name);

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
  bool IsResourceDirty(std::string uuid);

  /**
   * Returns a list of dirty resources on the client
   */
  std::vector<midasStatus> GetStatusEntries();

  std::vector<mdo::Community*> GetTopLevelCommunities(bool buildTree);
  void Populate(mdo::Community* node, bool recurse = true,
                bool checkDirty = true);
  void Populate(mdo::Collection* node, bool recurse = true,
                bool checkDirty = true);
  void Populate(mdo::Item* node, bool checkDirty = true);
protected:
  void InsertResourceRecord(int type, int id,
                            std::string path, std::string uuid, int parentId);

  int InsertBitstream(std::string path, std::string name);
  int InsertCollection(std::string name);
  int InsertCommunity(std::string name);
  int InsertItem(std::string name);

  std::string GetKeyName(MidasAppSetting setting);

  mds::SQLiteDatabase* Database;
  std::string DatabasePath;
};

#endif