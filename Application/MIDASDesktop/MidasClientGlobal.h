#ifndef __MIDASCLIENTGLOBAL_H
#define __MIDASCLIENTGLOBAL_H

#define MIDAS_CLIENT_VERSION (STR("1.0 beta"))

#define MIDAS_CLIENT_VERSION_STR (STR("MIDASDesktop ") + MIDAS_CLIENT_VERSION)

#define MIDAS_KEYWORDS_DELIMITER "/"
#define MIDAS_AUTHORS_DELIMITER ";"

#include "assert.h"
#include "string.h"

#ifndef UNREFERENCED_PARAM
#define UNREFERENCED_PARAM(param) ((void)param)
#endif //UNREFERENCED_PARAM

#define TO_IMPLEMENT() (Logger::warning(STR(__func__) + " <Not yet implemented !>"))

#define STR(s) (std::string(s))
#define QSTR(s) (QString(s))

#define STR2QSTR(s)   (QString::fromStdString(s))
#define STR2CHARS(s)  (s.c_str())
#define QSTR2STR(s)   (s.toStdString())
#define QSTR2CHARS(s) (s.toLatin1().data())

#define STR2BOOL(s) (kwutils::to_numeric<bool>(s))
#define BOOL2STR(s) (kwutils::to_string<bool>(s))

#define QSTR2BOOL(s) (s.toInt())
#define BOOL2QSTR(s) (QString::number(s))

#define STR2UINT(s) (kwutils::to_numeric<unsigned int>(s))
#define UINT2STR(s) (kwutils::to_string<unsigned int>(s))

#define STR2INT(s) (kwutils::to_numeric<int>(s))
#define INT2STR(s) (kwutils::to_string<int>(s))

#define QSTR2INT(s) (s.toInt())
#define INT2QSTR(s) (QString::number(s))

#define STR2DBL(s) (kwutils::to_numeric<double>(s))
#define DBL2STR(s) (kwutils::to_string<double>(s))

#define STR2FLT(s) (kwutils::to_numeric<float>(s))
#define FLT2STR(s) (kwutils::to_string<float>(s))

// ------------- __func__ -------------
/* Try to get a reasonable __func__ substitute in place. */
#if defined(_MSC_VER)
/* MSVC compilers before VC7 don't have __func__ at all; later ones call it
 * __FUNCTION__. */
# if _MSC_VER < 1300
#  define __func__ "???"
# else
#  define __func__ __FUNCTION__
# endif
#else
/* For platforms where autoconf works, make sure __func__ is defined
 * sanely. */
# ifndef HAVE_MACRO__func__
#  ifdef HAVE_MACRO__FUNCTION__
#   define __func__ __FUNCTION__
#  elif HAVE_MACRO__FUNC__
#   define __func__ __FUNC__
#  elif __STDC_VERSION__ < 199901L && __GNUC__ >= 2
#   define __func__ __PRETTY_FUNCTION__ // If too verbose replace '__PRETTY_FUNCTION__' by '__FUNCTION__'
#  else
#   define __func__ "???"
#  endif
# endif /* ifndef HAVE_MACRO__func__ */
#endif /* if not windows */
// ------------- __func__ -------------

#endif //__MIDASCLIENTGLOBAL_H
