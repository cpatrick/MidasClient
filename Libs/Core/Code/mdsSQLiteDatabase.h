/*=========================================================================

  Copyright (c) Kitware Inc. All rights reserved.
  See Copyright.txt or http://www.Kitware.com/Copyright.htm for details.

     This software is distributed WITHOUT ANY WARRANTY; without even
     the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR
     PURPOSE.  See the above copyright notices for more information.

=========================================================================*/

#ifndef _mdsSQLiteDatabase_h_
#define _mdsSQLiteDatabase_h_

#include <string>
#include <vector>

#include "mdsDatabase.h"
#include <sqlite3.h>
#include <QMutex>

namespace mds{

/** This class is a wrapper to a SQLite database. */
class SQLiteDatabase : public Database
{
public:

  SQLiteDatabase();
  ~SQLiteDatabase();

  /** Open a database connection */
  bool Open(const char* dbname);
  bool Close();
  
  /** Set the query */
  bool SetQuery(const char* query);

  /** Execute */
  bool Execute();
  
  /** Run Set query and execute in one command */
  bool ExecuteQuery(const char* query);
  int GetLastInsertId();

  /** Get number of columns */
  unsigned int GetNumberOfFields();
  const char*  GetFieldName(unsigned int column);
  
  /** Fetch the next row */
  bool GetNextRow();
  
  /** Get the column value */
  int GetValueAsInt(unsigned int column);
  sqlite_int64 GetValueAsInt64(unsigned int column);
  float GetValueAsFloat(unsigned int column);
  const char* GetValueAsString(unsigned int column);
  
  /** Get the error message */
  const char* GetErrorMessage() {return m_ErrorMessage.c_str();}
  
protected:

  sqlite3*      m_Database;
  sqlite3_stmt* m_Statement;
  QMutex*       m_Mutex;
  std::string   m_Query;
  std::string   m_ErrorMessage;
  bool          m_InitialFetch;
  bool          m_Active;
  int           m_InitialFetchResult;
};

} //end namespace

#endif //_mdsObject_h_
