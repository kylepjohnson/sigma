//
// File:         BuiltInTheory.cpp
// Description:  Built-in theory abstraction.  
// Created:      Jul 06, 2002, Alexandre Riazanov, riazanov@cs.man.ac.uk
//==================================================================
#include "RuntimeError.hpp"
#include "BuiltInTheory.hpp"
#include "VampireKernelConst.hpp"
//===================================================================
#undef DEBUG_NAMESPACE
#ifdef DEBUG_BUILT_IN_THEORY
 #define DEBUG_NAMESPACE "BuiltInTheory"
#endif
#include "debugMacros.hpp"
//===================================================================

using namespace VK;
using namespace BK;

BuiltInTheory::~BuiltInTheory()
{
};


bool BuiltInTheory::interpret(ulong inputSymbolId,const char* nativeSymbol)
{
  CALL("interpret(ulong inputSymbolId,const char* nativeSymbol)");
  if (VampireKernelConst::isEquality(inputSymbolId))
    {
      return
        interpretAs(VampireKernelConst::UnordEqNum,nativeSymbol) &&
        interpretAs(VampireKernelConst::OrdEqNum,nativeSymbol);
    }
  else
    return interpretAs(inputSymbolId,nativeSymbol);
}; // bool BuiltInTheory::interpret(ulong inputSymbolId,const char* nativeSymbol)


//==================================================================
