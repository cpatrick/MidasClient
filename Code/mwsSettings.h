/*=========================================================================

  Program:   Dionysus
  Module:    mwsSettings.h
  Language:  C++
  Date:      $Date$
  Version:   $Revision$

  Copyright (c) Kitware Inc. All rights reserved.
  See Copyright.txt or http://www.Kitware.com/Copyright.htm for details.

     This software is distributed WITHOUT ANY WARRANTY; without even
     the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR
     PURPOSE.  See the above copyright notices for more information.

=========================================================================*/

#ifndef __mwsSettings_h
#define __mwsSettings_h

#include <iostream>

namespace mws{

class Settings
{
public:

  enum SEARCHIN_OPTION
    {
    SEARCHIN_NOTSET       = 0x0,
    SEARCHIN_TITLES       = 0x1, 
    SEARCHIN_KEYWORDS     = 0x2, 
    SEARCHIN_ABSTRACTS    = 0x4, 
    SEARCHIN_SPONSORS     = 0x8, 
    SEARCHIN_AUTHORS      = 0x10, 
    SEARCHIN_DESCRIPTIONS = 0x20, 
    SEARCHIN_BITSTREAMS   = 0x40,
    }; 

  static Settings * GetInstance(); 

  /** Read/Write the settings */
  bool Load(const char* databasefilename);
  bool Write(const char* databasefilename=NULL);

  /** Set/Get the email */
  void SetEmail(const char* email) {m_Email=email;}
  std::string GetEmail() { return m_Email; }

  /** Set/Get the password */
  void SetPassword(const char* password);
  std::string GetPassword();
  
  /** Set/Get working directory */
  void SetWorkingDirectory(const char* value) { m_WorkingDirectory = value; }
  std::string GetWorkingDirectory() { return m_WorkingDirectory; }

  /** Set/Get ReadUpload agreement */
  void SetReadUploadAgreement(bool value) {m_ReadUploadAgreement = value; }
  bool GetReadUploadAgreement() { return m_ReadUploadAgreement;}

  /** Set/Get search option visible */
  void SetSearchOptionsVisible(bool value) {m_SearchOptionsVisible = value; }
  bool GetSearchOptionsVisible() { return m_SearchOptionsVisible;}

  /** Set/Get the server URL */
  std::string GetServerURL()     { return m_ServerURL; }
  void SetServerURL(const char* url) {m_ServerURL=url;}
  
  /** Search options */
  bool GetSearchInWords()        { return m_SearchInWords; }
  bool GetSearchInNotSet()       { return (m_SearchInOptions == SEARCHIN_NOTSET); }
  bool GetSearchInTitles()       { return (m_SearchInOptions & SEARCHIN_TITLES) == SEARCHIN_TITLES; }
  bool GetSearchInKeywords()     { return (m_SearchInOptions & SEARCHIN_KEYWORDS) == SEARCHIN_KEYWORDS; }
  bool GetSearchInAbstracts()    { return (m_SearchInOptions & SEARCHIN_ABSTRACTS) == SEARCHIN_ABSTRACTS; }
  bool GetSearchInSponsors()     { return (m_SearchInOptions & SEARCHIN_SPONSORS) == SEARCHIN_SPONSORS; }
  bool GetSearchInAuthors()      { return (m_SearchInOptions & SEARCHIN_AUTHORS) == SEARCHIN_AUTHORS; }
  bool GetSearchInDescriptions() { return (m_SearchInOptions & SEARCHIN_DESCRIPTIONS) == SEARCHIN_DESCRIPTIONS; }
  bool GetSearchInBitstreams()   { return (m_SearchInOptions & SEARCHIN_BITSTREAMS) == SEARCHIN_BITSTREAMS; }

  void SetSearchInWords(bool value)        { m_SearchInWords = value;}
  void SetSearchInTitles(bool value)       { m_SearchInOptions = static_cast<SEARCHIN_OPTION>(value ? m_SearchInOptions | SEARCHIN_TITLES : m_SearchInOptions & ~SEARCHIN_TITLES); }
  void SetSearchInKeywords(bool value)     { m_SearchInOptions = static_cast<SEARCHIN_OPTION>(value ? m_SearchInOptions | SEARCHIN_KEYWORDS : m_SearchInOptions & ~SEARCHIN_KEYWORDS); }
  void SetSearchInAbstracts(bool value)    { m_SearchInOptions = static_cast<SEARCHIN_OPTION>(value ? m_SearchInOptions | SEARCHIN_ABSTRACTS : m_SearchInOptions & ~SEARCHIN_ABSTRACTS); }
  void SetSearchInSponsors(bool value)     { m_SearchInOptions = static_cast<SEARCHIN_OPTION>(value ? m_SearchInOptions | SEARCHIN_SPONSORS : m_SearchInOptions & ~SEARCHIN_SPONSORS); }
  void SetSearchInAuthors(bool value)      { m_SearchInOptions = static_cast<SEARCHIN_OPTION>(value ? m_SearchInOptions | SEARCHIN_AUTHORS : m_SearchInOptions & ~SEARCHIN_AUTHORS); }
  void SetSearchInDescriptions(bool value) { m_SearchInOptions = static_cast<SEARCHIN_OPTION>(value ? m_SearchInOptions | SEARCHIN_DESCRIPTIONS : m_SearchInOptions & ~SEARCHIN_DESCRIPTIONS); }
  void SetSearchInBitstreams(bool value)   { m_SearchInOptions = static_cast<SEARCHIN_OPTION>(value ? m_SearchInOptions | SEARCHIN_BITSTREAMS : m_SearchInOptions & ~SEARCHIN_BITSTREAMS); }
  
protected:

  Settings();
  ~Settings();

private:

  static Settings * m_Instance; 
  std::string       m_DatabaseFilename; 
  std::string       m_Email;
  std::string       m_Password;
  std::string       m_ServerURL;
  bool              m_ReadUploadAgreement; 
  bool              m_SearchOptionsVisible; 
  std::string       m_WorkingDirectory; 
  SEARCHIN_OPTION   m_SearchInOptions; 
  bool              m_SearchInWords;
};

} // namespace

#endif
