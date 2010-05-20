/*=========================================================================

  Program:   Dionysus
  Module:    kwCrypt.h
  Language:  C++
  Date:      $Date$
  Version:   $Revision$

  Copyright (c) Kitware Inc. All rights reserved.
  See Copyright.txt or http://www.Kitware.com/Copyright.htm for details.

     This software is distributed WITHOUT ANY WARRANTY; without even
     the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR
     PURPOSE.  See the above copyright notices for more information.

=========================================================================*/
#ifndef __kwCrypt_H
#define __kwCrypt_H

#include <iostream>

class kwCrypt 
{
public:

  kwCrypt();
  ~kwCrypt();

  const char* Crypt(const char* buffer);
  const char* Decrypt(const char* buffer);

private:

  std::string key;
  std::string key2;
};

#endif //__kwCrypt_H
