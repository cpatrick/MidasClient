#include "DetailedException.h"

#include "Logger.h"
#include "Utils.h"

DetailedException::DetailedException(const char * file, int line, const char * func, const std::string& message):
      _file(file), _line(line), _func(func), _message(message)
  {
  _what = _file + ":" + INT2STR(_line) + ", " + _func + ", " + _message;
  LOGGER_FUNC_RESET(); 
  }

DetailedException::DetailedException(const char * file, int line, const char * func, const std::string& message, const std::exception &e):
      _file(file), _line(line), _func(func), _message(message)
  {
  _what = _file + ":" + INT2STR(_line) + ", " + _func + ", " + _message + " - "+STR(e.what());
  LOGGER_FUNC_RESET(); 
  }
