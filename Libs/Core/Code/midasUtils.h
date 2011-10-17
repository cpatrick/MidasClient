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


#ifndef MIDASUTILS_H
#define MIDASUTILS_H

#include "midasStandardIncludes.h"

#include <QCryptographicHash>

namespace mdo
{
class Object;
}

namespace mds
{
class UpgradeHandler;
}

class midasUtils
{
public:
  static std::string GenerateUUID();

  static std::string EscapeForSQL(std::string s);

  static std::string EscapeName(std::string name);

  static int64 GetFileLength(const char* filename);

  static std::string BytesToString(double bytes, const std::string& units = "B");

  static std::string FormatTimeString(double seconds);

  static double StringToDouble(const std::string& num);

  static std::string GetTypeName(int type);

  static bool IsDatabaseValid(std::string path, mds::UpgradeHandler* handler);

  static bool CreateNewDatabase(std::string path, bool midas3 = false);

  static mdo::Object * FetchByUuid(std::string uuid);

  static void StringTrim(std::string& str);

  static std::string TrimTrailingSlash(std::string);

  static int GetParentType(int type);

  static void Tokenize(const std::string& str, std::vector<std::string>& tokens, const std::string& delimiters = " ",
                       bool trimspaces = true);

  static std::string CreateDefaultAPIKey(const std::string& email, const std::string& password,
                                         const std::string& appName);

  static bool ValidateBitstreamName(const std::string& name);

  static std::string ComputeStringMD5(const char* input);

  static std::string ComputeStringSHA1(const char* input);

  static std::string ComputeFileChecksum(const std::string& filename,
                                         QCryptographicHash::Algorithm alg = QCryptographicHash::Md5);

  static bool RenameFile(const char* oldname, const char* newname);

  static bool RemoveDir(const std::string& dir);

  static double CurrentTime();

};

#endif
