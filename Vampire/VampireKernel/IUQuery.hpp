//
// File:         IUQuery.hpp
// Description:  Representation of queries for indexed unification.
// Created:      Mar 20, 2003, Alexandre Riazanov, riazanov@cs.man.ac.uk
// Status:       Sketch.
//============================================================================
#ifndef IU_QUERY_H
#define IU_QUERY_H
//============================================================================
#ifndef NO_DEBUG_VIS
#  include <iostream>
#endif
#include "jargon.hpp"
#include "DestructionMode.hpp"
#include "GlobAlloc.hpp"
#include "CacheAllocator.hpp"
#include "VampireKernelDebugFlags.hpp"
#include "UTerm.hpp"
#include "UVariable.hpp"
#include "Term.hpp"
//============================================================================
#ifdef DEBUG_IU_QUERY
#  undef DEBUG_NAMESPACE
#  define DEBUG_NAMESPACE "IUQuery"
#endif
#include "debugMacros.hpp"
//============================================================================

namespace VK 
{
  class IUQuery
  {
  public:
    IUQuery(UVariable::Domain* varDomain) :
      _variableBank(varDomain),
      _timeStamp(1UL)
    {
    }; 
    ~IUQuery() {};    
    void reset()
    {
      CALL("reset()");
      _termAllocator.reclaimAllMemory();
      _termWriter.reset(&_termAllocator);
      if (_timeStamp == ULONG_MAX)
	{
	  _timeStamp = 1UL;
	}
      else
	++_timeStamp;
    };
    void constant(const TERM& c)
    {
      CALL("constant(const TERM& c)");
      _termWriter.constant(c);
    };
    void nonconstFunc(const TERM& f)
    {
      CALL("nonconstFunc(const TERM& f)");
      _termWriter.ordinaryFunc(f);
    };
    void function(const TERM& f)
    {
      CALL("function(const TERM& f)");
      if (f.arity()) 
	{
	  nonconstFunc(f);
	}
      else
	constant(f);
    }; // void function(const TERM& f)

    UVariable* variable(ulong relativeVarNum)
    {
      CALL("variable(ulong relativeVarNum)");

      UVariable* var = _variableBank.var(relativeVarNum);
      if (var->primaryTimeStamp() == _timeStamp)
	{
	  _termWriter.var(var);
	}
      else // first occurence
	{
	  var->primaryTimeStamp() = _timeStamp;
	  _termWriter.varFirstOcc(var);
	};
      return var;
    }; // void variable(ulong relativeVarNum)
    
    void endOfQuery()
    {
      CALL("endOfQuery()");
      
    }; // void endOfQuery()

    UTerm* term() { return _termWriter.result(); };

  private:
    class TermAllocator : public UTerm::AbstractQueryAllocator
    {
    public: 
      TermAllocator() {};
      ~TermAllocator();
      void* allocate(size_t size);
      void reclaimAllMemory() { _memory.reclaimAllMemory(); };
    private:
      BK::CacheAllocator<BK::GlobAlloc,1012,IUQuery> _memory;
    }; // class TermAllocator : public UTerm::AbstractQueryAllocator

  private:
    UVariable::Bank _variableBank;
    UTerm::QueryWriter _termWriter;
    TermAllocator _termAllocator;
    ulong _timeStamp;
  }; // class IUQuery
}; // namespace VK

//============================================================================
#endif
