/*=========================================================================

  Copyright (c) Kitware Inc. All rights reserved.
  See Copyright.txt or http://www.Kitware.com/Copyright.htm for details.

     This software is distributed WITHOUT ANY WARRANTY; without even
     the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR
     PURPOSE.  See the above copyright notices for more information.

=========================================================================*/
#include "mdsSQLiteDatabase.h"
#include <stdio.h>
#include <string.h>
#include <sstream>
#include <iostream>

namespace mds{

/** Constructor */
SQLiteDatabase::SQLiteDatabase()
{
  m_Database = 0;
  m_Statement = 0;
  m_Query = "";
  m_ErrorMessage = "";
  m_InitialFetch = false;
  m_Active = false;
  m_InitialFetchResult=SQLITE_DONE;
}
  
/** Destructor */
SQLiteDatabase::~SQLiteDatabase()
{
  
}

/** Open a database connection */
bool SQLiteDatabase::Open(const char* dbname)
{
  int result = sqlite3_open(dbname, &m_Database);
  if(result)
    {
    m_ErrorMessage = sqlite3_errmsg(m_Database);
    sqlite3_close(m_Database);
    return false;
    }
  return true;  
}

/** Close the current database */
bool SQLiteDatabase::Close()
{
  // Finalize any statements;
  sqlite3_finalize(m_Statement);
  m_Statement = NULL;
      
  int result = sqlite3_close(m_Database);
  if(result)
    {
    m_ErrorMessage = sqlite3_errmsg(m_Database);
    return false;
    }
  return true; 
}
  
/** Set the query */
bool SQLiteDatabase::SetQuery(const char* query)
{
  if(m_Statement)
    {
    int finalizeStatus = sqlite3_finalize(m_Statement);
    m_Statement = NULL;
    if (finalizeStatus != SQLITE_OK)
      {
      m_ErrorMessage = "SetQuery(): Finalize returned unexpected code ";
      return false;
      }
    }
    
  const char *unused_statement;
  int prepareStatus = sqlite3_prepare_v2(m_Database, 
                                         query,
                                         static_cast<int>(strlen(query)),
                                         &m_Statement,
                                         &unused_statement);
    
  if (prepareStatus != SQLITE_OK)
    {
    m_ErrorMessage =  "Cannot prepare transaction";
    return false;
    }
  m_ErrorMessage = "";  
  return true;
}

/** Execute */
bool SQLiteDatabase::Execute()
{
  if(m_Statement == NULL)
    {
    m_ErrorMessage =  "Execute(): Query is not null but prepared statement is.  There may have been an error during SetQuery().";
    return false;
    }
  else
    {
    sqlite3_reset(m_Statement);
    }
    
  m_InitialFetch = true;
  m_Active = true;
  
  int result = sqlite3_step(m_Statement);
  m_InitialFetchResult = result;
  
  if (result == SQLITE_DONE)
    {
    m_ErrorMessage = "";
    }
  else if (result != SQLITE_ROW)
    {
    m_Active = false;
    m_ErrorMessage = sqlite3_errmsg(m_Database);
    return false;
    }
    
  m_ErrorMessage = "";  
  return true;  
}

/** Run Set query and execute in one command */
bool SQLiteDatabase::ExecuteQuery(const char* query)
{
  bool success = this->SetQuery(query);
  if(success)
    {
    success = this->Execute();
    }
  return success;
} 

/** Get the autoincremented id of the last insert */
int SQLiteDatabase::GetLastInsertId()
{
  this->ExecuteQuery("SELECT last_insert_rowid()");
  return this->GetNextRow() ? this->GetValueAsInt(0) : -1;
}
  
/** Get number of columns */
const char* SQLiteDatabase::GetFieldName(unsigned int column)
{
  if(!m_Active)
    {
    return NULL;
    }
    
  if (column >= this->GetNumberOfFields())
    {
    m_ErrorMessage = "GetFieldName(): Illegal field index ";
    return NULL;
    }
    
  return sqlite3_column_name(m_Statement, column);
}


/** Get number of columns */
unsigned int SQLiteDatabase::GetNumberOfFields()
{
  if(!m_Active)
    {
    return 0;
    }
    
  if(m_Statement)
    {
    return sqlite3_column_count(m_Statement);
    }
  return 0;  
}
  
/** Fetch the next row */
bool SQLiteDatabase::GetNextRow()
{
  if(!m_Active)
    {
    m_ErrorMessage = "Query is not active";
    return false;
    }

  if(m_InitialFetch)
    {
    m_InitialFetch = false;
    if(m_InitialFetchResult == SQLITE_DONE)
      {
      return false;
      }
    else
      {
      return true;
      }
    }
  else
    {
    int result = sqlite3_step(m_Statement);
    if (result == SQLITE_DONE)
      {
      return false;
      }
    else if (result == SQLITE_ROW)
      {
      return true;
      }
    else
      {
      m_ErrorMessage = sqlite3_errmsg(m_Database);
      m_Active = false;
      return false;
      }
    }
  return true;  
}
  
/** Get the column value */
int SQLiteDatabase::GetValueAsInt(unsigned int column)
{
  if(m_Active == false)
    {
    m_ErrorMessage = "Query is not active";
    return 0;
    }
  else if (column < 0 || column >= this->GetNumberOfFields())
    {
    m_ErrorMessage = "DataValue() called with out-of-range column index ";
    return 0;
    }
  else
    {
    switch (sqlite3_column_type(m_Statement, column))
      {
      case SQLITE_INTEGER:
        return sqlite3_column_int(m_Statement, column);
      }
    }
  
  m_ErrorMessage = "Wrong column type";  
  return 0;
}

/** Get the column value */
float SQLiteDatabase::GetValueAsFloat(unsigned int column)
{
  if(m_Active == false)
    {
    m_ErrorMessage = "Query is not active";
    return 0;
    }
  else if (column < 0 || column >= this->GetNumberOfFields())
    {
    m_ErrorMessage = "DataValue() called with out-of-range column index ";
    return 0;
    }
  else
    {
    switch (sqlite3_column_type(m_Statement, column))
      {
      case SQLITE_FLOAT:
        return (float)sqlite3_column_double(m_Statement, column);
      }
    }
  m_ErrorMessage = "Wrong column type";  
  return 0;
}

const char* SQLiteDatabase::GetValueAsString(unsigned int column)
{
  if(m_Active == false)
    {
    m_ErrorMessage = "Query is not active";
    return NULL;
    }
  else if (column < 0 || column >= this->GetNumberOfFields())
    {
    m_ErrorMessage = "DataValue() called with out-of-range column index ";
    return NULL;
    }
  else
    {
    switch (sqlite3_column_type(m_Statement, column))
      {
      case SQLITE_TEXT:
        return (const char*)sqlite3_column_text(m_Statement, column);
      }
    }
  
  m_ErrorMessage = "Wrong column type";  
  return NULL;
}


} // end namespace
