#ifndef __Logger_H
#define __Logger_H

#include <exception>
#include <string>
#include <vector>

#define LOGGER_FUNC_START() { \
  Logger::indent(); \
  Logger::debug(__func__); \
  }
#define LOGGER_FUNC_END() (Logger::unindent())
#define LOGGER_FUNC() (Logger::debug(__func__))
#define LOGGER_FUNC_RESET() (Logger::resetindent())

class Printable; 

class Logger
{
public:
  enum LOG_LEVEL { l_FATAL, l_ERROR, l_WARNING, l_INFO, l_DEBUG, l_LOG, };

  static void log(std::string message, const std::exception * e = NULL);
  static void log(const std::exception * e);
  
  static void log(std::vector<std::string> &messages, std::string desc = ""); 
  static void log(std::vector<Printable*> &messages, std::string desc = ""); 

  static void debug(std::string message, const std::exception * e = NULL);
  static void debug(const std::exception * e);

  static void info(std::string message, const std::exception * e = NULL);
  static void info(const std::exception * e);

  static void warning(std::string message, const std::exception * e = NULL);
  static void warning(const std::exception * e);

  static void error(std::string message, const std::exception * e = NULL);
  static void error(const std::exception * e);

  static void fatal(std::string message, const std::exception * e = NULL);
  static void fatal(const std::exception * e);
  
  static void log(LOG_LEVEL level, std::string message , const std::exception * e = NULL, bool levelTag = true); 

  static bool is(LOG_LEVEL log_level); 

  static void indent(){ _indent++; } 
  static void unindent(){ _indent--; } 
  static void resetindent(){ _indent = 0; }

private:
  static const char * toStr(LOG_LEVEL level);
  static LOG_LEVEL effective_log_level; 

  static unsigned int _indent; 

  Logger(){}
  ~Logger(){}
};


#endif //__Logger_H
