#ifndef __DetailedException_H
#define __DetailedException_H

#include <exception>
#include <string>

class DetailedException: public std::exception
{
public:
  DetailedException(const char * file, int line, const char * func, const std::string& message); 
  DetailedException(const char * file, int line, const char * func, const std::string& message, const std::exception &e); 

  virtual ~DetailedException() throw(){}

  virtual const char* message() const throw()
    {
    return STR2CHARS(this->_message); 
    }
  virtual const char* what() const throw()
    {
    return STR2CHARS(this->_what);
    }

  virtual const char* file() const throw()
    {
    return STR2CHARS(this->_file);
    }

  virtual const int line() const throw()
    {
    return this->_line;
    }

  virtual const char* func() const throw()
    {
    return this->_func.c_str();
    }
private:
  std::string _what; 
  std::string _file; 
  int _line; 
  std::string _func;
  std::string _message; 
};

class 
DetailedNullException : public DetailedException 
{
public:
  DetailedNullException(const char *  file, int line, const char *  func, const std::string& arg):
      DetailedException(file, line, func, "Variable "+arg+" is NULL"), _arg(arg){}
  ~DetailedNullException() throw(){}
  virtual const char* arg() const throw()
    {
    return this->_arg.c_str();
    }
private:
  std::string _arg; 
}; 

class BrokenConnectionException : public DetailedException 
{
public:
  BrokenConnectionException(const char *  file, int line, const char *  func, const std::string& arg):
      DetailedException(file, line, func, "Connection to '"+arg+"' server impossible"), _arg(arg){}
  ~BrokenConnectionException() throw(){}

  virtual const char* arg() const throw()
    {
    return _arg.c_str();
    }

private:
  std::string _arg; 
};

#endif //__DetailedException_H
