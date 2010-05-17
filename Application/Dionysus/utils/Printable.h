#ifndef __Printable_H
#define __Printable_H

#include <string>

class Printable
{
public:
  Printable(){}
  virtual ~Printable(){}
  virtual const std::string toString() =0;
};


#endif // __Printable_H