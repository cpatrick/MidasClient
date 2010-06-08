#include "Logger.h"

#include <string>
#include <iostream>
#include <typeinfo>

#include "Printable.h"

Logger::LOG_LEVEL Logger::effective_log_level = Logger::l_LOG;  

unsigned int Logger::_indent = 0; 

void Logger::log(Logger::LOG_LEVEL level, std::string message, const std::exception * e, bool levelTag)
  {
  std::ostream * logstream = &std::cout; 
  if (effective_log_level < level) return;
  if (level <= l_WARNING)
    {
    logstream = &std::cerr;
    }
  std::string logMsg = ( levelTag ? ("["+std::string(Logger::toStr(level)) + "]\t") : "\t" );
  logMsg.append( std::string(_indent*2, ' ') );
  logMsg.append(message.empty() ? "" : message);
  if (e!=NULL)
    {
    logMsg.append( (message.empty() ? "" : " - ") + std::string("Exception: ")); 
    logMsg.append( typeid(*e).name()); 
    logMsg.append( e->what()==NULL?"":(" - " + std::string(e->what()) ) ); 
    }
  (*logstream) << logMsg << std::endl; 
  }

void Logger::log(const std::exception * e)
  {
  Logger::log(Logger::l_LOG, "", e);
  }
void Logger::log(std::string message, const std::exception * e)
  {
  Logger::log(Logger::l_LOG, message, e); 
  }

void Logger::log(std::vector<std::string>& messages, std::string desc)
  {
  std::vector<std::string>::const_iterator it = messages.begin();
  while(it != messages.end())
    {
    Logger::log(Logger::l_LOG, desc + "<" + *it + ">", NULL, it==messages.begin());
    it++; 
    }
  }

void Logger::log(std::vector<Printable*>& messages, std::string desc)
  {
  std::vector<Printable*>::const_iterator it = messages.begin();
  while(it != messages.end())
    {
    Logger::log(Logger::l_LOG, desc + "<" + (*it)->toString() + ">", NULL, it==messages.begin());
    it++; 
    }
  }

void Logger::debug(const std::exception * e)
  {
  Logger::log(Logger::l_DEBUG, "", e);
  }
void Logger::debug(std::string message, const std::exception * e)
  {
  Logger::log(Logger::l_DEBUG, message, e); 
  }

void Logger::info(const std::exception * e)
  {
  Logger::log(Logger::l_INFO, "", e);
  }
void Logger::info(std::string message, const std::exception * e)
  {
  Logger::log(Logger::l_INFO, message, e); 
  }

void Logger::warning(const std::exception * e)
  {
  Logger::log(Logger::l_WARNING, NULL, e);
  }
void Logger::warning(std::string message, const std::exception * e)
  {
  Logger::log(Logger::l_WARNING, message, e); 
  }

void Logger::error(const std::exception * e)
  {
  Logger::log(Logger::l_ERROR, "", e);
  }
void Logger::error(std::string message, const std::exception * e)
  {
  Logger::log(Logger::l_ERROR, message, e); 
  }

void Logger::fatal(const std::exception * e)
  {
  Logger::log(Logger::l_FATAL, "", e);
  }
void Logger::fatal(std::string message, const std::exception * e)
  {
  Logger::log(Logger::l_FATAL, message, e); 
  }

const char *  Logger::toStr(LOG_LEVEL level)
  {
  switch (level)
    {
    case Logger::l_LOG:
      return "LOG"; 
    case Logger::l_DEBUG:
      return "DEBUG"; 
    case Logger::l_INFO:
      return "INFO"; 
    case Logger::l_WARNING:
      return "WARNING"; 
    case Logger::l_ERROR:
      return "ERROR"; 
    case Logger::l_FATAL:
      return "FATAL"; 
    default:
      return "UNKNOWN";
    }
  }

bool Logger::is(LOG_LEVEL log_level)
  {
  return ( Logger::effective_log_level == log_level ); 
  }
