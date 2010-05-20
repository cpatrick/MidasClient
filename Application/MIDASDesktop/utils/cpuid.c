#include "cpuid.h"

#if !defined(__APPLE__)
#  define cpuid(invar,a,b,c,d)\
  asm("cpuid": "=a" (a), "=b" (b), "=c" (c), "=d" (d) : "a" (invar));
#else
  cpuid() {}; 
  //cpuid(invar, *a, *b, *c, *d);
#endif

void vtkKWCPUID(unsigned long invar, 
                 unsigned long *a, unsigned long *b, unsigned long *c, unsigned long *d)
{
#ifdef _WIN32
  _asm {  
    MOV EAX, invar
    CPUID 
    MOV a, EAX 
    MOV b, EBX 
    MOV c, ECX
    MOV d, EDX
  }
#else
  cpuid(invar, *a, *b, *c, *d);
#endif
} 

