//
// File:         Comparison.hpp
// Description:  
// Created:      Jan 26, 2001, 11:00
// Author:       Alexandre Riazanov
// mail:         riazanov@cs.man.ac.uk
//==================================================================
#ifndef COMPARISON_H
//===================================================================
#define COMPARISON_H
#include <iostream>
#include "jargon.hpp"
#include "BlodKorvDebugFlags.hpp"
//================================================
#undef DEBUG_NAMESPACE
#ifdef DEBUG_COMPARISON
 #define DEBUG_NAMESPACE "Comparison"
#endif
#include "debugMacros.hpp"
//=================================================
namespace BK 
{

enum Comparison 
 {
  Less = -1,
  Equal = 0,
  Greater = 1,
  Incomparable = 2 
 };

inline Comparison inverse(const Comparison& cmp)
{
 CALL("(global) inverse(const Comparison& cmp)");
 switch (cmp)
  {
   case Less: return Greater;
   case Equal: return Equal;
   case Greater: return Less;
   case Incomparable: return Incomparable;  
   #ifdef DEBUG_NAMESPACE
    default: ICP("ICP0"); return Incomparable; 
   #elif defined _SUPPRESS_WARNINGS_
    default: return Incomparable; 
   #endif              
  };
 #ifdef DEBUG_NAMESPACE
  ICP("ICP1"); 
  return Incomparable;
 #elif defined _SUPPRESS_WARNINGS_
  return Incomparable; 
 #endif    
}; 

}; // namespace BK

namespace std
{
inline ostream& operator<<(ostream& str,const BK::Comparison& cmp) 
{
 switch (cmp)
  {
   case BK::Less: return str << "Less";
   case BK::Equal: return str << "Equal"; 
   case BK::Greater: return str << "Greater";
   case BK::Incomparable: return str << "Incomparable";     
  };
 return str << "UNKNOWN";    
};
};

//================================================
#undef DEBUG_NAMESPACE
#ifdef DEBUG_COMPARISON
 #define DEBUG_NAMESPACE "FunctionComparison"
#endif
#include "debugMacros.hpp"
//=================================================

namespace BK 
{

enum FunctionComparison
{   
  FailureToCompare, // to establish some target relation
  AlwaysLess,
  AlwaysEqual,
  AlwaysGreater,
  CanBeLessOrEqual,
  CanBeGreaterOrEqual,
  VolatileButNearlyAlwaysLess,
  VolatileButNearlyAlwaysGreater,
  AbsolutelyVolatile
}; // enum FunctionComparison

inline bool strict(const FunctionComparison& cmp)
{
  CALL("(global) strict(const FunctionComparison& cmp)");
  switch (cmp)
    {
    case FailureToCompare: 
    case AlwaysLess:
    case AlwaysEqual:
    case AlwaysGreater:
      return true;
    case CanBeLessOrEqual:
    case CanBeGreaterOrEqual:
    case VolatileButNearlyAlwaysLess:
    case VolatileButNearlyAlwaysGreater:
    case AbsolutelyVolatile:
      return false;
    };
  ICP("ICP0");
#if (defined _SUPPRESS_WARNINGS_) || (defined DEBUG_NAMESPACE)
  return false;
#endif
}; // bool strict(const FunctionComparison& cmp)


inline FunctionComparison inverse(const FunctionComparison& cmp)
{
  CALL("(global) inverse(const FunctionComparison& cmp)");
  switch (cmp)
    {
    case FailureToCompare: return FailureToCompare;
    case AlwaysLess: return AlwaysGreater;
    case AlwaysEqual: return AlwaysEqual;
    case AlwaysGreater: return AlwaysLess;
    case CanBeLessOrEqual: return CanBeGreaterOrEqual;
    case CanBeGreaterOrEqual: return CanBeLessOrEqual;
    case VolatileButNearlyAlwaysLess: return VolatileButNearlyAlwaysGreater;
    case VolatileButNearlyAlwaysGreater: return VolatileButNearlyAlwaysLess;
    case AbsolutelyVolatile: return AbsolutelyVolatile;
    };
  ICP("ICP0");
#if (defined _SUPPRESS_WARNINGS_) || (defined DEBUG_NAMESPACE)
  return FailureToCompare;
#endif 

}; // FunctionComparison inverse(const FunctionComparison& cmp)

}; // namespace BK

namespace std
{
  inline ostream& operator<<(ostream& str,const BK::FunctionComparison& cmp)
  {
    switch (cmp)
      {
      case BK::FailureToCompare: str << "FailureToCompare"; break;
      case BK::AlwaysLess: str << "AlwaysLess"; break;
      case BK::AlwaysEqual: str << "AlwaysEqual"; break;
      case BK::AlwaysGreater: str << "AlwaysGreater"; break;
      case BK::CanBeLessOrEqual: str << "CanBeLessOrEqual"; break;
      case BK::CanBeGreaterOrEqual: str << "CanBeGreaterOrEqual"; break;
      case BK::VolatileButNearlyAlwaysLess: str << "VolatileButNearlyAlwaysLess"; break;
      case BK::VolatileButNearlyAlwaysGreater: str << "VolatileButNearlyAlwaysGreater"; break;
      case BK::AbsolutelyVolatile: str << "AbsolutelyVolatile"; break;
      default: str << "????????";
      };
    return str;
  }; // ostream& operator<<(ostream& str,const FunctionComparison& cmp)
};

//=================================================
#undef DEBUG_NAMESPACE 
//=================================================
#endif 
