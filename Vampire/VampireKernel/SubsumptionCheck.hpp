//
// File:         SubsumptionCheck.hpp
// Description:  Checking whether a clause subsumes another one with a given substitution.
// Created:      Apr 13, 2000, 15:00
// Author:       Alexandre Riazanov
// mail:         riazanov@cs.man.ac.uk
//================================================  
#ifndef NO_DEBUG
#ifndef SUBSUMPTION_CHECK_H
//================================================
#define SUBSUMPTION_CHECK_H
#include "jargon.hpp"
#include "VampireKernelDebugFlags.hpp"
#include "Term.hpp"
#include "Clause.hpp"
#include "SimpleSubstitution.hpp"
//================================================
#undef DEBUG_NAMESPACE
#ifdef DEBUG_SUBSUMPTION_CHECK
#define DEBUG_NAMESPACE "SubsumptionCheck"
#endif 
#include "debugMacros.hpp"
//================================================
namespace VK
{
class SubsumptionCheck
{
 public:
  SubsumptionCheck() : 
    _termTraversal1(&_subst), 
    _termTraversal2(DOP("SubsumptionCheck::_termTraversal2")) 
    {
    };
  ~SubsumptionCheck() {};
  void init()
  {
    CALL("init()");
    _subst.init();
    _termTraversal1.init(&_subst);
    _termTraversal2.init(DOP("SubsumptionCheck::_termTraversal2"));
  };
  void destroy()
  {
    CALL("destroy()");
    _termTraversal2.destroy();
    _termTraversal1.destroy();
    _subst.destroy();
  };

  void resetSubst() { _subst.reset(); };
  void substPair(ulong var,const TERM& term) { _subst.nextPair(var,term); };
  bool allVarsAssigned(const Clause* subsumingCl)
    {
      CALL("allVarsAssigned(const Clause* subsumingCl)");

      for (LiteralList::Iterator iter(subsumingCl->LitList());iter.notEnd();iter.next())
	{    
	  _termTraversal2.reset(iter.currentElement()->literalRef());
	  _termTraversal2.next();
	  while (_termTraversal2)
	    { 
	      if ((_termTraversal2.state() == TERM::Traversal::Var)
		  && (!_subst.assigned(_termTraversal2.symbol().var()))) 
		return false;
	      _termTraversal2.next();
	    };
	};
      return true;
    };

  bool subsumes(const Clause* subsumingCl,const Clause* subsumedCl) 
    {
      CALL("subsumes(const Clause* subsumingCl,const Clause* subsumedCl)");   
      for (LiteralList::Iterator iter1(subsumingCl->LitList());iter1.notEnd();iter1.next())
	{
	  if (!iter1.currentLiteral()->isAnswerLiteral())
	    {
	      LiteralList::Iterator iter2(subsumedCl->LitList());
	    check_lits:
	      if (!litSubsumes(iter1.currentElement()->literalRef(),iter2.currentElement()->literalRef()))  
		{ 
		  iter2.next();
		  if (iter2.end()) return false;
		  goto check_lits;
		};
	    };
	};
      return true;
    }; // bool subsumes(const Clause* subsumingCl,const Clause* subsumedCl) 

  bool subsumesItself(const Clause* cl) // for debugging
    {
      // idiotic, but helps to check certain things 
      resetSubst();
      // assigning variables to themselves 
      for (LiteralList::Iterator iter(cl->LitList());iter.notEnd();iter.next())
	{    
	  _termTraversal2.reset(iter.currentElement()->literalRef());
	  _termTraversal2.next();
	  while (_termTraversal2)
	    { 
	      if ((_termTraversal2.state() == TERM::Traversal::Var)
		  && (!_subst.assigned(_termTraversal2.symbol().var())))
		substPair(_termTraversal2.symbol().var(),_termTraversal2.symbol());
	      _termTraversal2.next();
	    };
	};
      
      return subsumes(cl,cl);      
    }; // bool subsumesItself(const Clause* cl) // for debugging 

 private:
  bool litSubsumes(const TERM& lit1,const TERM& lit2)
    {
      CALL("subsumes(const TERM& lit1,const TERM& lit2)");
      if (*(lit1.First()) != *(lit2.First())) return false;
      if (lit1.First()->IsSymLitHeader()) 
	{
	  ASSERT(lit1.First()->arity() == 2);
	  return symLitSubsumes(*(lit1.First()->Args()),
				*(lit1.First()->Args()->Next()),
				*(lit2.First()->Args()),
				*(lit2.First()->Args()->Next()));
	};
      // ordinary literals
      return (!(lit1.First()->arity()) || termSubsumes(lit1,lit2));
    }; // bool litSubsumes(const TERM* lit1,const TERM* lit2) 
  
  bool symLitSubsumes(const TERM& arg11,const TERM& arg12,const TERM& arg21,const TERM& arg22)
    {
      return ((termSubsumes(arg11,arg21) && termSubsumes(arg12,arg22)) 
	      || (termSubsumes(arg11,arg22) && termSubsumes(arg12,arg21)));
    }; // bool symLitSubsumes(const TERM& arg11,const TERM& arg12,const TERM& arg21,const TERM& arg22) 

  bool termSubsumes(const TERM& t1,const TERM& t2)
    {
      CALL("termSubsumes(const TERM& t1,const TERM& t2)");
      ALWAYS(_termTraversal1.reset(t1));
      _termTraversal2.reset(t2);
      do
	{
	  if (_termTraversal1.symbol() != _termTraversal2.symbol()) return false;
	  ALWAYS(_termTraversal1.next());
	  _termTraversal2.next();
	}
      while (_termTraversal2);
      return true;  
    }; // bool termSubsumes(const TERM& t1,const TERM& t2)

 private:
  SimpleSubstitution _subst;
  SimpleSubstitution::TermTraversal _termTraversal1;
  TERM::Traversal _termTraversal2; 
}; // class SubsumptionCheck

}; // namespace VK

//================================================
#endif
#endif
