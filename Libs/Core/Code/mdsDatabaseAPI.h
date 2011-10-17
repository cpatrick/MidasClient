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


#ifndef _MDSDATABASEAPI_H
#define _MDSDATABASEAPI_H

#define DB_IS_MIDAS3 (mds::DatabaseAPI::GetCurrentDatabaseVersion().Major == 3)

#include "midasStandardIncludes.h"
#include "midasStatus.h"
#include "mdsSQLiteDatabase.h"
#include "midasLogAware.h"
#include "mdsDatabaseInfo.h"

namespace mdo
{
class Community;
class Collection;
class Item;
class Bitstream;
class Object;
class Version;
}

namespace mds
{
class Community;
class Collection;
class Item;
class Bitstream;
class Object;
class ResourceUpdateHandler;
}

namespace m3do
{
class Folder;
}

namespace m3ds
{
class Folder;
class Item;
class Bitstream;
}

/**
 * Identifier for any resource type (community,collection,item,bitstream)
 * Includes Type#, Id, Parent Id, Path on disk, and uuid
 */
struct midasResourceRecord
  {
  midasResourceRecord() :
    Type(midasResourceType::TYPE_ERROR), Id(0), Parent(0), Path("")
  {
  }
  int Type;
  int Id;
  int Parent;
  std::string Path;
  std::string Uuid;
  };

struct midasAuthProfile
  {
  std::string Name;
  std::string Url;
  std::string AppName;
  std::string ApiKey;
  std::string User;
  std::string RootDir;

  bool IsAnonymous()
  {
    return User == "";
  }
  bool HasRootDir()
  {
    return RootDir == "";
  }
  };

struct midasBitstreamStamp
  {
  std::string Path;
  long int LastModified;
  int Id;
  };

namespace mds
{

class DatabaseAPI : public midasLogAware
{
  friend class Community;
  friend class Collection;
  friend class Item;
  friend class Bitstream;
  friend class Version;
  friend class Upgrade;
  friend class PartialDownload;
  friend class PartialUpload;
  friend class m3ds::Folder;
  friend class m3ds::Item;
  friend class m3ds::Bitstream;
public:
  DatabaseAPI(const std::string& path = "");
  ~DatabaseAPI();

  enum MidasAppSetting
    {
    LAST_URL,
    LAST_FETCH_TIME,
    AUTO_REFRESH_INTERVAL,
    AUTO_REFRESH_SETTING,
    ROOT_DIR,
    UNIFIED_TREE
    };

  /**
   * Clean entries in the database
   */
  void Clean();

  std::string GetSetting(MidasAppSetting setting);

  int GetSettingInt(MidasAppSetting setting);

  bool GetSettingBool(MidasAppSetting setting);

  void SetSetting(MidasAppSetting setting, std::string value);

  void SetSetting(MidasAppSetting setting, int value);

  void SetSetting(MidasAppSetting setting, bool value);

  std::string GetName(int type, int id);

  int GetParentId(int type, int id);

  /** Search resources in the database */
  std::vector<mdo::Object *> Search(std::vector<std::string> tokens);

  /**
   * Creates a new resource and uuid record, and its
   * corresponding parent/child entry, iff it doesn't exist.
   * If it does exist, returns its id.
   */
  int AddResource(int type, std::string uuid, std::string path,
                  std::string name, int parentType, int parentId,
                  int serverParent);

  int AddResource(int type, std::string uuid, std::string path,
                  std::string name, std::string parentUuid,
                  int serverParent);

  /**
   * Delete a resource from the database
   */
  bool DeleteResource(std::string uuid, bool deleteFiles = false);

  bool AddAuthProfile(std::string user, std::string appName,
                      std::string apiKey, std::string profileName,
                      std::string rootDir, std::string url);

  midasAuthProfile GetAuthProfile(std::string name);

  std::vector<std::string> GetAuthProfiles();

  void DeleteProfile(std::string name);

  /**
   * Returns whether or not a resource with the given uuid exists
   */
  bool ResourceExists(std::string uuid);

  midasResourceRecord GetRecordByUuid(std::string uuid);

  /**
   * Add a child/parent relationship to the database
   */
  bool AddChild(int parentType, int parentId, int childType, int childId);

  std::string GetUuidFromPath(std::string path);

  std::string GetUuid(int type, int id);

  void MarkDirtyResource(std::string uuid, int dirtyAction);

  void ClearDirtyResource(std::string uuid);

  bool IsResourceDirty(std::string uuid);

  /**
   * Returns a list of dirty resources on the client
   */
  std::vector<midasStatus> GetStatusEntries();

  std::vector<mdo::Community *> GetTopLevelCommunities(bool buildTree);

  // Midas 3: get top level folders
  std::vector<m3do::Folder *> GetTopLevelFolders();

  /**
   * If any resources are located outside the current root on disk,
   * this will copy them underneath it and update their stored path
   */
  bool UnifyTree(bool copy = false);

  /**
   * Iterates over all the bitstreams known to the database
   * Checks if they have been modified since last push/pull.
   * If any were modified, marks them as dirty.
   * If any were deleted, removes them from control.
   * If any were modified or deleted, returns true,
   * Otherwise false.
   */
  bool CheckModifiedBitstreams();

  static mdo::Version GetCurrentDatabaseVersion();

protected:
  bool InsertResourceRecord(int type, int id, std::string path, std::string uuid, int parentId);

  int InsertBitstream(std::string path, std::string name);

  int InsertCollection(std::string name);

  int InsertCommunity(std::string name);

  int InsertItem(std::string name);

  bool Relocate(mdo::Community* comm, std::string parentDir, bool copy);

  bool Relocate(mdo::Collection* coll, std::string parentDir, bool copy);

  bool Relocate(mdo::Item* item, std::string parentDir, bool copy);

  bool Relocate(mdo::Bitstream* bitstream, std::string parentDir, bool copy);

  std::string GetKeyName(MidasAppSetting setting);

  bool Open();

  bool Close();

  SQLiteDatabase*        Database;
  std::string            DatabasePath;
  ResourceUpdateHandler* UpdateHandler;
};

} // end namespace
#endif
