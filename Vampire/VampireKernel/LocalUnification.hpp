//
// File:         LocalUnification.hpp
// Description:  Unification of standard terms over the same variable space. 
// Created:      Oct 25, 2002, Alexandre Riazanov, riazanov@cs.man.ac.uk
//===================================================================
#ifndef LOCAL_UNIFICATION_H
#define LOCAL_UNIFICATION_H
//===================================================================
#ifndef NO_DEBUG_VIS
#include <iostream>
#endif
#include "jargon.hpp"
#include "VampireKernelDebugFlags.hpp"
#include "VampireKernelConst.hpp"
#include "Term.hpp"
#include "Stack.hpp"
//===================================================================
#undef DEBUG_NAMESPACE
#ifdef DEBUG_WEIGHT_POLYNOMIAL
 #define DEBUG_NAMESPACE "LocalUnification"
#endif
#include "debugMacros.hpp"
//===================================================================

namespace VK
{
  class LocalUnification
  {
  public:
    class UnifierState {};
    class TermTraversal;
  public:
    LocalUnification();
    ~LocalUnification();
    void reset();
    bool unify(const TERM* complexTerm1,const TERM* complexTerm2);
    bool unify(const TERM& term1,const TERM& term2);
    const UnifierState* getUnifierState() const 
    { 
      return 
	static_cast<const UnifierState*>(static_cast<const void*>(_instantiated.end())); 
    };
    void rollUnifierBackTo(const UnifierState* notch);
#ifndef NO_DEBUG_VIS
    ostream& output(ostream& str) const;
#endif

  private:
    bool unify1(const TERM* complexTerm1,const TERM* complexTerm2);
    bool unify1(const TERM& term1,const TERM& term2);
    bool occursIn(const TERM& var,const TERM& term) const;
    void instantiate(ulong varNum,const TERM* instance);
    

  private:
    const TERM* _unifier[VampireKernelConst::MaxNumOfVariables];
    BK::Stack<const TERM**,VampireKernelConst::MaxNumOfVariables> _instantiated;
    friend class TermTraversal;
  }; // class LocalUnification
}; 

#ifndef NO_DEBUG_VIS
namespace std
{
  inline
  ostream& operator<<(ostream& str,const VK::LocalUnification& unifier)
  {
    return unifier.output(str);
  };
};
#endif

//===================================================================
#undef DEBUG_NAMESPACE
#ifdef DEBUG_WEIGHT_POLYNOMIAL
 #define DEBUG_NAMESPACE "LocalUnification::TermTraversal"
#endif
#include "debugMacros.hpp"
//===================================================================

namespace VK
{
  class LocalUnification::TermTraversal
  {
  public:
    TermTraversal();
    TermTraversal(const TERM& term,const LocalUnification* unifier);
    ~TermTraversal();
    void reset(const TERM& term,const LocalUnification* unifier);
    operator bool() const { return _term != 0; };
    void next();
    void after();
    const TERM& symbol() const { return (_term->isVariable()) ? (*_term) : (*_term->First()); };
    const TERM& term() const { return *_term; };

  private:
    void canonizeVariableInstance();

  private:
    const TERM* const * _unifier;
    TERM _dummyTerm[2];
    BK::Stack<const TERM*,VampireKernelConst::MaxTermSize> _backtrackPoints;   
    const TERM* _term;
    const TERM* _after;
  }; // class LocalUnification::TermTraversal
}; // namespace VK

//===================================================================
#undef DEBUG_NAMESPACE
#ifdef DEBUG_WEIGHT_POLYNOMIAL
 #define DEBUG_NAMESPACE "LocalUnification::TermTraversal"
#endif
#include "debugMacros.hpp"
//===================================================================


namespace VK
{
  inline
  void LocalUnification::TermTraversal::next()
  {
    CALL("next()");
    ASSERT(_term);
    if (_term->isVariable() || (!_term->First()->arity())) 
      {
	after(); 
      }
    else // compound term
      {
	if (!_after->IsNull()) _backtrackPoints.push(_after);
	_term = _term->First()->Args();
	_after = _term->Next();
	canonizeVariableInstance();
      };
  }; // void LocalUnification::TermTraversal::next()


  inline
  void LocalUnification::TermTraversal::after()
  {
    CALL("after()");
    ASSERT(_term);
    if (_after->IsNull())
      {
	if (_backtrackPoints)
	  {
	    _term = _backtrackPoints.pop();	  
	  }
	else // terminal position
	  {
	    _term = 0;
	    return;
	  };
      }
    else
      {
	_term = _after;
      };
    _after = _term->Next();
    canonizeVariableInstance();
  }; // void LocalUnification::TermTraversal::after()


  inline
  void LocalUnification::TermTraversal::canonizeVariableInstance()
  {
    CALL("canonizeVariableInstance()");
    while ((_term->isVariable()) && _unifier[_term->var()])
      {
	_term = _unifier[_term->var()];
      };  
  }; // void LocalUnification::TermTraversal::canonizeVariableInstance()

}; // namespace VK

//===================================================================
#endif
