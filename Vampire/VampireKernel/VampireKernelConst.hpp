//
// File:         VampireKernelConst.hpp
// Description:  Global constants for VampireKernel.
// Created:      Feb 12, 2000, 17:50
// Author:       Alexandre Riazanov
// mail:         riazanov@cs.man.ac.uk
//====================================================
#ifndef HELL_CONST_H
//====================================================
#define HELL_CONST_H 
#include <climits>
#include <cstddef>
#include "jargon.hpp"
//====================================================
class VampireKernelConst
{
 public:
  // interface 
  static const char* SkolemConstantNamePrefix() { return "c__"; };
  static const char* SkolemFunctionNamePrefix() { return "f__"; };
  static const char* SkolemPredicateNamePrefix() { return "p__"; };
  static const char* CanonicalVariableNamePrefix() { return "X"; };
  
  enum 
  {
    // basic parameters
    MaxlNumOfLits = 65535UL, // can not be greater than 2^16 - 1 
    MaxSymbolArity = 1023UL, // can not be greater than 2^12 - 1, see term.h
#ifdef LARGE_TERMS
    MaxNumOfVariables = 128UL,
    MaxTermDepth = 48UL,
    MaxTermSize = 256UL,  
    MaxClauseSize = 512UL,
#else 
#ifdef HUGE_TERMS
    MaxNumOfVariables = 256UL,
    MaxTermDepth = 1024UL,
    MaxTermSize = 1024UL, 
    MaxClauseSize = 2048UL,
#else
    MaxNumOfVariables = 64UL,
    MaxTermDepth = 24UL,  
    MaxTermSize = 128UL,   
    MaxClauseSize = 256UL,
#endif
#endif
 
    MaxlSymbolNum = 65535UL, 
 
    MinMaxWeight = 1UL,

    MaxNumOfStandardIndices = 2UL, // for 2 premises in a rule

 // hard values: don't change this unless you know what you are doing!
    UnordEqNum = 0UL,
    OrdEqNum = 1UL,
    FirstNoneqFun = 2UL,
    BuiltInTrueNum = 2UL,
    ReservedPred0 = 3UL,
    ReservedPred1 = 4UL,
    ReservedPred2 = 5UL,

    // derived values
    MaxlHeaderNum = MaxlSymbolNum*2 + 1,
    MaxNumOfIndices = MaxNumOfStandardIndices + 0
  }; // enum 

  // checking integrity
  static bool checkIntegrity() 
    { 
      return (UnordEqNum == 0)
	&& (OrdEqNum == 1)
	&& (FirstNoneqFun == 2);
 
    };
  // functions
  static bool isEquality(const ulong& num) { return (num < FirstNoneqFun); }; 
  static bool isOrderedEq(const ulong& num) { return num != 0UL; }; 
  static bool isUnorderedEq(const ulong& num) { return num == 0UL; }; 
  static bool isBuiltInTrue(const ulong& num) { return num == BuiltInTrueNum; };
  static bool isReservedPred(const ulong& num) { return (num > 1) && (num < 5); };
}; // namespace VampireKernelConst
//====================================================
#endif



