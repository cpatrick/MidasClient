#ifndef __Utils_H
#define __Utils_H

#include <stdio.h>
#include <sstream>
#include <string>
#include <vector>
#include "md5.h"

namespace kwutils 
  {
  template <class T>
  inline std::string to_string (const T& t)
    {
    std::stringstream ss;
    ss << t;
    return ss.str();
    }

  template <class T>
  T to_numeric(const std::string& s, 
               std::ios_base& (*f)(std::ios_base&)=std::dec)
    {
    T t; 
    std::istringstream iss(s);
    if ( (iss >> f >> t).fail() )
      {
      //throw DetailedException(__FILE__, __LINE__, __func__, "Failed to convert '" + s + "' to numeric");
      }
    return t; 
    }

  static std::string concatenate(const std::vector<std::string> &data, const std::string & separator = ", ")
    {
    std::string result; 
    std::vector<std::string>::const_iterator it = data.begin();
    while(it != data.end())
      {
      result.append(*it);
      if( it != (data.end()-1) )
        {
        result.append( separator ); 
        }
      it++; 
      }
    return result; 
    }

  //inspired from http://www.codeproject.com/KB/stl/stdstringtrim.aspx
  static void trim(std::string& str)
    {
    std::string::size_type pos = str.find_last_not_of(' ');
    if(pos != std::string::npos) 
      {
      str.erase(pos + 1);
      pos = str.find_first_not_of(' ');
      if(pos != std::string::npos) str.erase(0, pos);
      }
    else 
      {
      str.erase(str.begin(), str.end());
      }
    }

  // inspired from http://oopweb.com/CPP/Documents/CPPHOWTO/Volume/C++Programming-HOWTO-7.html
  static void tokenize(const std::string& str,
                        std::vector<std::string>& tokens,
                        const std::string& delimiters = " ", bool trimspaces = true)
    {
    // Skip delimiters at beginning.
    std::string::size_type lastPos = str.find_first_not_of(delimiters, 0);
    // Find first "non-delimiter".
    std::string::size_type pos     = str.find_first_of(delimiters, lastPos);

    while (std::string::npos != pos || std::string::npos != lastPos)
      {
      std::string token = str.substr(lastPos, pos - lastPos); 
      if ( trimspaces ) { trim( token ); }
      // Found a token, add it to the vector.
      tokens.push_back( token );
      // Skip delimiters.  Note the "not_of"
      lastPos = str.find_first_not_of(delimiters, pos);
      // Find next "non-delimiter"
      pos = str.find_first_of(delimiters, lastPos);
      }
  }

  static std::string computeMD5(const std::string& data)
    {
    char * dataStr = const_cast<char*>(data.c_str()); 
    unsigned char* digest = new unsigned char[16];
    unsigned int i;
    // initialize digest array
    for(i=0;i<16;i++)
      {
      digest[i] = 0;
      }
    
    // Init the MD5 function
    MD5Context context;
    MD5Init(&context);
    MD5Update(&context,(const md5byte *)dataStr, strlen(dataStr));
    MD5Final(&context, digest);
    
    std::string md5 = "";
    for(i=0;i<16;i++)
      {
      char* value = new char[10];
      sprintf(value,"%x",digest[i]);
      if(strlen(value)==1)
        {
        value[1]=value[0];
        value[0]='0';  
        value[2]='\0';
        }
      md5 += value;
      }     
    return md5;
    }
  }
#endif // __Utils_H
