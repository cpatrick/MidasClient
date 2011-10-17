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


#ifndef _mdsDatabase_h_
#define _mdsDatabase_h_

#include <string>
#include <vector>

namespace mds
{

/** This class is a wrapper to a SQLite database. */
class Database
{
public:

  Database()
  {
  };
  virtual ~Database()
  {
  };

  /** Open a database connection */
  virtual bool Open(const char* dbname) = 0;

  virtual bool Close() = 0;

  /** Set the query */
  virtual bool SetQuery(const char* query) = 0;

  /** Execute */
  virtual bool Execute() = 0;

  /** Run Set query and execute in one command */
  virtual bool ExecuteQuery(const char* query) = 0;

  /** Get number of columns */
  virtual unsigned int GetNumberOfFields() = 0;

  virtual const char *  GetFieldName(unsigned int column) = 0;

  /** Fetch the next row */
  virtual bool GetNextRow() = 0;

  /** Get the column value */
  virtual int GetValueAsInt(unsigned int column) = 0;

  virtual float GetValueAsFloat(unsigned int column) = 0;

  virtual const char * GetValueAsString(unsigned int column) = 0;

  /** Get the error message */
  virtual const char * GetErrorMessage() = 0;

protected:

};

} // end namespace

#endif // _mdsDatabase_h_
