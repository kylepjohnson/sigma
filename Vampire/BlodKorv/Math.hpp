//
// File:         Math.hpp
// Description:  Mathematical functions. 
// Created:      Oct 24, 2002, Alexandre Riazanov, riazanov@cs.man.ac.uk  
//============================================================================
#ifndef MATH_H
//=============================================================================
#define MATH_H 
#include "jargon.hpp"
#include "BlodKorvDebugFlags.hpp"
//============================================================================
#undef DEBUG_NAMESPACE
#ifdef DEBUG_MATH
 #define DEBUG_NAMESPACE "Math"
#endif
#include "debugMacros.hpp"
//============================================================================

namespace BK
{
class Math
{
public:
  static long greatestCommonDivisor(long m,long n);
}; // class Math
}; // namespace BK
//============================================================================
#undef DEBUG_NAMESPACE
#ifdef DEBUG_MATH
 #define DEBUG_NAMESPACE "Math"
#endif
#include "debugMacros.hpp"
//============================================================================

namespace BK
{
inline
long Math::greatestCommonDivisor(long m,long n)
{
  CALL("greatestCommonDivisor(long m,long n)");
  if ((m <= 1L) || (n <= 1L)) return 1L;
  while (m != n) 
    {
      if (m > n)
	{
	  m -= n;
	}
      else // n > m
 	{
	  n -= m;
	};
    };
  return m;
}; // long Math::greatestCommonDivisor(const long&m,const long n)
}; // namespace BK

//============================================================================
#endif
