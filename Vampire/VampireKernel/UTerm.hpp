//
// File:         UTerm.hpp
// Description:  New specialised term representation for unification. 
// Created:      Mar 13, 2003, Alexandre Riazanov, riazanov@cs.man.ac.uk
// Status:       Sketch.
//============================================================================
#ifndef U_TERM_H
#define U_TERM_H
//============================================================================
#ifndef NO_DEBUG_VIS
#  include <iostream>
#endif
#include "jargon.hpp"
#include "RuntimeError.hpp"
#include "DestructionMode.hpp"
#include "GlobAlloc.hpp"
#include "InfiniteStack.hpp"
#include "ObjectPool.hpp"
#include "VampireKernelDebugFlags.hpp"
#include "Term.hpp"  
#include "UVariable.hpp"
//============================================================================
#ifdef DEBUG_U_TERM
#  undef DEBUG_NAMESPACE
#  define DEBUG_NAMESPACE "UTerm"
#endif
#include "debugMacros.hpp"
//============================================================================

namespace VK 
{
  class UTerm
  {
  public:
    typedef TERM Function;
    enum Tag
    {
      QVarTag = 0UL,
      QVarFirstOccTag = 1UL,

      TVarTag = 2UL,
      TVarFirstOccTag = 3UL,


      QConstTag = 4UL,      
      TConstTag = 5UL,

      QOrdinaryCompoundTag = 6UL,
      TOrdinaryCompoundTag = 7UL
    };

    class Domain;
    class Traversal;
    class QueryWriter;
    
  public:
    UTerm() {};
    ~UTerm() {};

    const Tag& tag() const { return _tag; };

    bool isVariable() const 
    {
      return _tag <= TVarFirstOccTag;
    };
    bool isCompound() const 
    {
      return (_tag >= QConstTag) && (_tag <= TOrdinaryCompoundTag);
    };
    bool isConstant() const
    {
      return 
	(_tag == QConstTag) ||
        (_tag == TConstTag); 
    };
    bool isOrdinaryCompound() const
    {
      return 
	(_tag == QOrdinaryCompoundTag) ||
        (_tag == TOrdinaryCompoundTag); 
    };

    UTerm*& next() { return _next; };
    UTerm*& after() { return _after; };

    void makeQVar(UVariable* v)
    {
      CALL("makeQVar(UVariable* v)");
      _tag = QVarTag;
      _after = _next;
      _var = v;
    };

    void makeQVarFirstOcc(UVariable* v)
    {
      CALL("makeQVarFirstOcc(UVariable* v)");
      _tag = QVarFirstOccTag;
      _after = _next;
      _var = v;
    };

    void makeTVar(UVariable* v)
    {
      CALL("makeTVar(UVariable* v)");
      _tag = TVarTag;
      _after = _next;
      _var = v;
    };

    void makeTVarFirstOcc(UVariable* v)
    {
      CALL("makeTVarFirstOcc(UVariable* v)");
      _tag = TVarFirstOccTag;
      _after = _next;
      _var = v;
    };

    void makeQConst(const Function& c)
    {
      CALL("makeQConst(const Function& c)");
      _tag = QConstTag;
      _after = _next;
      _symbol = c;
    };


    void makeTConst(const Function& c)
    {
      CALL("makeTConst(const Function& c)");
      _tag = TConstTag;
      _after = _next;
      _symbol = c;
    };

   
    void makeQOrdinaryCompound(const Function& f)
    {
      CALL("makeQOrdinaryCompound(const Function& f)");
      _tag = QOrdinaryCompoundTag;
      _symbol = f;
    };


    void makeTOrdinaryCompound(const Function& f)
    {
      CALL("makeTOrdinaryCompound(const Function& f)");
      _tag = TOrdinaryCompoundTag;
      _symbol = f;
    };

   
    const Function& symbol() const
    {
      CALL("symbol() const");
      ASSERT(isCompound());
      return _symbol;
    };

    UVariable* var() const
    {
      CALL("var() const");
      ASSERT(isVariable());
      return _var;
    };

    static UTerm* deref(const UVariable* var)
    {
      CALL("deref(const UVariable* var)");
      ASSERT(var->isInstantiated());
      UTerm* res;
    get_content:
      res = var->content();
      if (res->isVariable())
	{
	  var = res->var();
	  if (var->isInstantiated()) goto get_content;
	};
      return res;
    };

    bool containsVar(const UVariable* var);


#ifndef NO_DEBUG_VIS
    ostream& output(ostream& str); // substitution is respected
#endif

#ifndef NO_DEBUG
    bool isSyntacticallyEqual(UTerm* term);
#endif

  private:
    typedef BK::InfiniteStack<BK::GlobAlloc,UTerm*,8UL,Domain> ContinuationStack;

  private:
    static Domain* currentDomain() { return _currentDomain; }; 
  private:
    Tag _tag;
    UTerm* _next;
    UTerm* _after;
    Function _symbol;
    UVariable* _var;

    static Domain* _currentDomain;
    friend class Domain;
    friend class Traversal;
    friend class QueryWriter;
  }; // class UTerm
}; // namespace VK 



//============================================================================
#ifdef DEBUG_U_TERM
#undef DEBUG_NAMESPACE
 #define DEBUG_NAMESPACE "UTerm::Domain"
#endif
#include "debugMacros.hpp"
//============================================================================


namespace VK 
{
  class UTerm::Domain
  {    
  public:
    Domain() {};
    ~Domain() 
    {      
      if (BK::DestructionMode::isThorough())
	{
	  
	};
    };
    void init()
    {
      CALL("init()");
      _continuationStackPool.init();
    };
    void destroy()
    {
      CALL("destroy()");
      if (BK::DestructionMode::isThorough())
	{
	  
	};
      _continuationStackPool.destroy();
    }; 
    void activate()
    {
      CALL("activate()");
      _previousActive = UTerm::_currentDomain;
      UTerm::_currentDomain = this;
    };

    void deactivate()
    {
      CALL("activate()");
      ASSERT(this);
      UTerm::_currentDomain = _previousActive;
    };

  private:
    typedef BK::ObjectPool<BK::GlobAlloc,ContinuationStack> ContinuationStackPool;

  private:
    ContinuationStack* reserveContinuationStack()
    {
      CALL("reserveContinuationStack()");
      return _continuationStackPool.reserveObject();
    };

    void releaseContinuationStack(ContinuationStack* stack)
    {
      CALL("releaseContinuationStack(ContinuationStack* stack)");
      _continuationStackPool.releaseObject(stack);
    };
      

  private:
    Domain* _previousActive;
    ContinuationStackPool _continuationStackPool;

    friend class UTerm::Traversal;
    friend class UTerm::QueryWriter;
  }; // class UTerm::Domain
}; // namespace VK 


//============================================================================
#ifdef DEBUG_U_TERM
#undef DEBUG_NAMESPACE
 #define DEBUG_NAMESPACE "UTerm::Traversal"
#endif
#include "debugMacros.hpp"
//============================================================================


namespace VK 
{
  class UTerm::Traversal
  {    
  public:
    Traversal() : _continuations(0)
    {
      CALL("constructor Traversal()");
    };
    
    ~Traversal() 
    {
      CALL("destructor ~Traversal()");
      if (BK::DestructionMode::isThorough())
	{
	  if (_continuations)
	    UTerm::currentDomain()->releaseContinuationStack(_continuations);
	};
    };
    
    void reset(UTerm* term)
    {
      CALL("reset(UTerm* term)");
      if (!_continuations)
	_continuations = UTerm::currentDomain()->reserveContinuationStack();
      
      // temporary

    }; // void reset(UTerm* term)

    UTerm* currentSubterm() 
    { 
      return _currentSubterm;
    };

    bool next()
    {
      CALL("next()");
      return false; // temporary
    }; // bool next()


    bool after()
    {
      CALL("after()");

      return false; // temporary
      
    }; // bool after()
    

  private:
    UTerm* _currentSubterm;
    UTerm::ContinuationStack* _continuations;
  }; // class UTerm::Traversal
}; // namespace VK 




//============================================================================
#ifdef DEBUG_U_TERM
#undef DEBUG_NAMESPACE
 #define DEBUG_NAMESPACE "UTerm::QueryWriter"
#endif
#include "debugMacros.hpp"
//============================================================================


namespace VK 
{
  class UTerm::QueryWriter
  {    
  public:
    QueryWriter() 
    {
      CALL("constructor QueryWriter()");
    };
    ~QueryWriter() 
    {
      CALL("constructor QueryWriter()");
      if (BK::DestructionMode::isThorough())
	{
	};
    }; 

    void reset()
    {
      CALL("reset()");
      // temporary
    }; 

    void var(UVariable* v)
    {
      CALL("var(UVariable* v)");
      // temporary
    }; // void var(UVariable* v)

    void varFirstOcc(UVariable* v)
    {
      CALL("varFirstOcc(UVariable* v)");
      // temporary
    }; // void varFirstOcc(UVariable* v)

    void constant(const Function& c)
    {
      CALL("constant(const Function& c)");
      // temporary
    }; // void constant(const Function& c)

    void ordinaryFunc(const Function& f)
    {
      CALL("ordinaryFunc(const Function& f)");
      // temporary
    }; // void ordinaryFunc(const Function& f)
 

    UTerm* result() const 
    {
      return 0; // temporary
    };
    
  private:
    
  }; // UTerm::QueryWriter
}; // namespace VK 


//============================================================================
#ifdef DEBUG_U_TERM
#  undef DEBUG_NAMESPACE
#  define DEBUG_NAMESPACE "std"
#endif
#include "debugMacros.hpp"
//============================================================================
#ifndef NO_DEBUG_VIS
namespace std
{
  ostream& operator<<(ostream& str,VK::UTerm* term);
}; // namespace std
#endif

//============================================================================
#endif
