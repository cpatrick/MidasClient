/*=========================================================================

  Copyright (c) Kitware Inc. All rights reserved.
  See Copyright.txt or http://www.Kitware.com/Copyright.htm for details.

     This software is distributed WITHOUT ANY WARRANTY; without even
     the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR
     PURPOSE.  See the above copyright notices for more information.

=========================================================================*/

#ifndef MIDASUTILS_H
#define MIDASUTILS_H

#include "midasStandardIncludes.h"

#if defined(_MSC_VER) || defined(__BORLANDC__)
  typedef __int64 int64;
  typedef unsigned __int64 uint64;
#else
  typedef long long int int64;
  typedef unsigned long long int uint64;
#endif

namespace mdo{
  class Object;
}

class midasUtils
{
public:
  static std::string GenerateUUID();
  static std::string EscapeForURL(std::string s);
  static std::string EscapeForSQL(std::string s);
  static std::string EscapeName(std::string name);
  static int64 GetFileLength(const char* filename);
  static std::string BytesToString(int64 bytes);
  static std::string BytesToString(double bytes);
  static std::string GetTypeName(int type);
  static bool IsDatabaseValid(std::string path);
  static bool CreateNewDatabase(std::string path);
  static mdo::Object* FetchByUuid(std::string uuid);
  static void StringTrim(std::string& str);
  static std::string TrimTrailingSlash(std::string);
  static int GetParentType(int type);
  static void Tokenize(const std::string& str,
                       std::vector<std::string>& tokens,
                       const std::string& delimiters = " ",
                       bool trimspaces = true);
};

#endif
