//
// File:         BuiltInTheory.hpp
// Description:  Built-in theory abstraction.  
// Created:      Jul 06, 2002, Alexandre Riazanov, riazanov@cs.man.ac.uk
//==================================================================
#ifndef BUILT_IN_THEORY_H
#define BUILT_IN_THEORY_H
//==================================================================
#include <cstddef>
#include "jargon.hpp"
#include "VampireKernelDebugFlags.hpp"
//===================================================================
#undef DEBUG_NAMESPACE
#ifdef DEBUG_BUILT_IN_THEORY
 #define DEBUG_NAMESPACE "BuiltInTheory"
#endif
#include "debugMacros.hpp"
//===================================================================

namespace VK
{
  class Clause;
  class TmpLiteral;
  class BuiltInTheory
    {
    public:
      BuiltInTheory(const char* name) : _name(name)
	{
	};
      const char* name() { return _name; };
      virtual ~BuiltInTheory();
      bool interpret(ulong inputSymbolId,
		     const char* nativeSymbol);
      virtual Clause* simplify(TmpLiteral* lit,TmpLiteral* newLit) = 0;
    protected:
      BuiltInTheory() : _name(0) {};
    private:
      virtual bool interpretAs(ulong inputSymbolId,
			       const char* nativeSymbol) = 0;
    private:
      const char* _name;
    }; // class BuiltInTheory

}; // namespace VK


//=================================================================
#endif
