//
// File:         UVariable.hpp
// Description:  New specialised representation of variables for unification.
//               See also UTerm.hpp 
// Created:      Mar 15, 2003, Alexandre Riazanov, riazanov@cs.man.ac.uk
// Status:       Sketch.
//============================================================================
#ifndef U_VARIABLE_H
#define U_VARIABLE_H
//============================================================================
#ifndef NO_DEBUG_VIS
#  include <iostream>
#endif
#include "jargon.hpp"
#include "DestructionMode.hpp"
#include "GlobAlloc.hpp"
#include "InfiniteStack.hpp"
#include "ObjectPool.hpp"
#include "VampireKernelDebugFlags.hpp"
namespace VK 
{
  class UTerm;
};

//============================================================================
#ifdef DEBUG_U_VARIABLE
#  undef DEBUG_NAMESPACE
#  define DEBUG_NAMESPACE "UVariable"
#endif
#include "debugMacros.hpp"
//============================================================================

namespace VK 
{
  class UVariable
  {
  public:
    class Domain;
    class Bank;
    typedef BK::InfiniteStack<BK::GlobAlloc,UVariable*,32,UVariable> Stack;

  public:
    static Domain* currentDomain() { return _currentDomain; }; 
    UTerm*& content() { return _content; };
    UTerm* content() const { return _content; };
    ulong absoluteNumber() const // number in domain
    { 
      return _absoluteNumber;
    }; 
    ulong relativeNumber() const // number in bank
    {
      return _relativeNumber;
    };
    bool isInstantiated() const { return content() != 0; };
    bool isFree() const { return content() == 0; };
    void instantiate(UTerm* term) { content() = term; };
    void makeFree() { content() = 0; };
    
    ulong& primaryTimeStamp()
    {
      return _primaryTimeStamp;
    };

#ifndef NO_DEBUG_VIS
    ostream& output(ostream& str) const;
#endif
        
#ifndef NO_DEBUG
    bool isCycleFree();
#endif

  private:
    UVariable() : _primaryTimeStamp(0UL)
    {
      CALL("UVariable()");
      makeFree();  
    };
    void* operator new(size_t)
    {
      CALL("operator new(size_t)");
      return BK::GlobAlloc::allocate(sizeof(UVariable));
    };
    void operator delete(void* obj)
    {
      CALL("operator delete(void* obj)");
      return BK::GlobAlloc::deallocate(obj,sizeof(UVariable));
    };
    void setAbsoluteNumber(ulong n) { _absoluteNumber = n; };
    void setRelativeNumber(ulong n) { _relativeNumber = n; };
    UVariable* nextInHash() { return _nextInHash; };
    void setNextInHash(UVariable* v) { _nextInHash = v; };
  private:
    UTerm* _content; 
    ulong _absoluteNumber;
    ulong _relativeNumber;
    UVariable* _nextInHash;
    ulong _primaryTimeStamp;
    static Domain* _currentDomain;
    friend class Bank;
  }; // class UVariable
}; // namespace VK 


//============================================================================
#ifdef DEBUG_U_VARIABLE
#  undef DEBUG_NAMESPACE
#  define DEBUG_NAMESPACE "UVariable::Domain"
#endif
#include "debugMacros.hpp"
//============================================================================

namespace VK 
{
  class UVariable::Domain
  {
  public:    
    Domain() : 
      _nextAbsoluteNumber(0UL)
    {
    };
    ~Domain() 
    {      
    };   
    void init()
    {
      CALL("init()");
      _nextAbsoluteNumber = 0UL;
    };
    void destroy()
    {
      CALL("destroy()");
    }; 

    
    void activate()
    {
      CALL("activate()");
      _previousActive = UVariable::_currentDomain;
      UVariable::_currentDomain = this;
    };

    void deactivate()
    {
      CALL("activate()");
      ASSERT(this);
      UVariable::_currentDomain = _previousActive;
    };

    UVariable::Stack* reserveStack() 
    {
      CALL("reserveStack()");
      return _stackPool.reserveObject();
    };
    
    void releaseStack(UVariable::Stack* stack)
    {
      CALL("releaseStack(UVariable::Stack* stack)");
      _stackPool.releaseObject(stack);
    };
    
  private:
    ulong bookAbsoluteNumber() 
    { 
      ++_nextAbsoluteNumber; 
      return _nextAbsoluteNumber - 1;
    };
  private:
    Domain* _previousActive;
    ulong _nextAbsoluteNumber;
    
    BK::ObjectPool<BK::GlobAlloc,UVariable::Stack> _stackPool;

    friend class UVariable::Bank;
  }; // class UVariable::Domain
}; // namespace VK 


//============================================================================
#ifdef DEBUG_U_VARIABLE
#  undef DEBUG_NAMESPACE
#  define DEBUG_NAMESPACE "UVariable::Bank"
#endif
#include "debugMacros.hpp"
//============================================================================

namespace VK 
{
  class UVariable::Bank
  {
  public:
    Bank(UVariable::Domain* domain) 
    {
      CALL("constructor Bank()");
      _domain = domain;
      for (ulong i = 0UL; i <= MaxCacheVarNumber; ++i)
	{
	  _cache[i].setAbsoluteNumber(_domain->bookAbsoluteNumber());
	  _cache[i].setRelativeNumber(i);
	  _cache[i].setNextInHash(0);
	};
    }; // Bank()

    ~Bank() 
    {
      CALL("destructor ~Bank()");
      if (BK::DestructionMode::isThorough())
	{
	  for (ulong i = 0UL; i <= MaxCacheVarNumber; ++i)
	    {
	      UVariable* next = _cache[i].nextInHash();
	      while (next)
		{
		  UVariable* tmp = next->nextInHash();
		  delete next;
		  next = tmp;
		};
	    };
	};      
    }; // ~Bank() 

    UVariable* var(ulong relativeVarNum)
    {
      CALL("var(ulong relativeVarNum)");
      if (relativeVarNum > MaxCacheVarNumber)
	{
	  UVariable* lastChecked = 
	    _cache + (relativeVarNum % (MaxCacheVarNumber + 1));
	  ASSERT(lastChecked->relativeNumber() < relativeVarNum);
	  UVariable* next;
	try_next:
	  next = lastChecked->nextInHash();
	  if (next)
	    {
	      if (next->relativeNumber() == relativeVarNum)
		return next;
	      if (next->relativeNumber() > relativeVarNum)
		{
		  UVariable* newVar = new UVariable();
		  newVar->setAbsoluteNumber(_domain->bookAbsoluteNumber());
		  newVar->setRelativeNumber(relativeVarNum);
		  newVar->setNextInHash(next);
		  lastChecked->setNextInHash(newVar);
		  return newVar;
		};
	      lastChecked = next;
	      goto try_next;
	    }
	  else
	    {
	      UVariable* newVar = new UVariable();
	      newVar->setAbsoluteNumber(_domain->bookAbsoluteNumber());
	      newVar->setRelativeNumber(relativeVarNum);
	      newVar->setNextInHash(0);
	      lastChecked->setNextInHash(newVar);
	      return newVar;
	    };
	}
      else // in cache
	return _cache + relativeVarNum;
    }; // UVariable* var(ulong relativeVarNum)

  private:
    enum
    {
      MaxCacheVarNumber = 31UL
    };
  private:
    Bank() 
    {
      CALL("constructor Bank()");
      ICP("ICP0");
    };
  private:
    UVariable::Domain* _domain;
    UVariable _cache[MaxCacheVarNumber + 1];
  }; // class UVariable::Bank
}; // namespace VK 


//============================================================================
#endif
