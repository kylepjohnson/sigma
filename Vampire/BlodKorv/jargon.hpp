//
// File:         jargon.hpp
// Description:  Frequently used definitions.
// Created:      May 5, 2000, 20:20
// Author:       Alexandre Riazanov
// mail:         riazanov@cs.man.ac.uk
//============================================================================
#ifndef JARGON_H
#define JARGON_H
//============================================================================
         // Target operational environment and compiler

// Supported compilers

#define BK_COMPILER_GCC_2_95_GENERIC 0
#define BK_COMPILER_GCC_3_GENERIC 1
#define BK_COMPILER_MSVCPP_GENERIC 2

// Supported compiler families

#define BK_COMPILER_FAMILY_GCC_2_95 0
#define BK_COMPILER_FAMILY_GCC_3 1
#define BK_COMPILER_FAMILY_MSVCPP 2

// Supported operational environments

#define BK_TARGET_OPERATIONAL_ENVIRONMENT_UNIX_GENERIC 0
#define BK_TARGET_OPERATIONAL_ENVIRONMENT_MSWIN32_GENERIC 1

// Supported operational environment types

#define BK_TARGET_OPERATIONAL_ENVIRONMENT_TYPE_UNIX 0
#define BK_TARGET_OPERATIONAL_ENVIRONMENT_TYPE_MSWIN32 1

// Identify the compiler family and version

#undef BK_COMPILER
#undef BK_COMPILER_FAMILY 


#if defined __GNUG__
#  if ((__GNUC__ == 2) && (__GNUC_MINOR__ == 95))
#    define BK_COMPILER BK_COMPILER_GCC_2_95_GENERIC
#    define BK_COMPILER_FAMILY BK_COMPILER_FAMILY_GCC_2_95
#  elif (__GNUC__ == 3)
#    define BK_COMPILER BK_COMPILER_GCC_3_GENERIC
#    define BK_COMPILER_FAMILY BK_COMPILER_FAMILY_GCC_3
#  else
#    error "Currently unsupported GCC version. Contact the BlodKorv library maintainers."
#  endif



#elif (defined _MSC_VER) 
#  if (_MSC_VER >= 1200)
#    define BK_COMPILER BK_COMPILER_MSVCPP_GENERIC
#    define BK_COMPILER_FAMILY BK_COMPILER_FAMILY_MSVCPP
#  else
#    error "Currently unsupported version of Microsoft C/C++ compiler. Contact the BlodKorv library maintainers."
#  endif

#else
#  error "Currently unsupported compiler family. Contact the BlodKorv library maintainers."
#endif


// Identify operational environment

#undef BK_TARGET_OPERATIONAL_ENVIRONMENT
#undef BK_TARGET_OPERATIONAL_ENVIRONMENT_TYPE

#if ((defined unix) || (defined __unix__) || (defined linux) || (defined __linux__))
#  define BK_TARGET_OPERATIONAL_ENVIRONMENT BK_TARGET_OPERATIONAL_ENVIRONMENT_UNIX_GENERIC
#  define BK_TARGET_OPERATIONAL_ENVIRONMENT_TYPE BK_TARGET_OPERATIONAL_ENVIRONMENT_TYPE_UNIX
#elif (defined _WIN32)
#  define BK_TARGET_OPERATIONAL_ENVIRONMENT BK_TARGET_OPERATIONAL_ENVIRONMENT_MSWIN32_GENERIC
#  define BK_TARGET_OPERATIONAL_ENVIRONMENT_TYPE BK_TARGET_OPERATIONAL_ENVIRONMENT_TYPE_MSWIN32

#else
#  error "Currently unsupported target operational environment. Contact the BlodKorv library maintainers."

#endif


//============================================================================


typedef unsigned long ulong;

namespace BK 
{

class DummyArgument
{
 public:
  DummyArgument() {};
  void touch() const {};
};

static DummyArgument dummyArgument;

  /********

inline
void init(long& x) {};
inline
void init(long& x,const long& y) 
{
  x = y;
};
inline
void init(long& x,const DummyArgument& da) 
{
#if (!defined NO_DEBUG) || (defined _SUPPRESS_WARNINGS_)
  da.touch();
#endif
  x = 0L;
};

inline
void destroy(long& x) {};
inline
void relocateFrom(long& x,long& y)
{
  x = y;
};

inline
void init(ulong& x) {};
inline
void init(ulong& x,const ulong& y) 
{
  x = y;
};
inline
void init(ulong& x,const DummyArgument& da) 
{
#if (!defined NO_DEBUG) || (defined _SUPPRESS_WARNINGS_)
  da.touch();
#endif
  x = 0UL;
};
inline
void destroy(ulong& x) {};
inline
void relocateFrom(ulong& x,ulong& y)
{
  x = y;
};

template <class T>
inline
void init(T*& x) {};

template <class T>
inline
void init(T*& x,const T*& y) 
{
  x = y;
};

template <class T>
inline
void init(T*& x,const DummyArgument& da) 
{
#if (!defined NO_DEBUG) || (defined _SUPPRESS_WARNINGS_)
  da.touch();
#endif
  x = (T*)0;
};

template <class T>
inline
void destroy(T*& x) {};

template <class T>
inline
void relocateFrom(T*& x,T*& y)
{
  x = y;
};


template <class T>
inline
void touch(const T& x) 
{
};

********/

}; // namespace BK




//============================================================================
#endif
