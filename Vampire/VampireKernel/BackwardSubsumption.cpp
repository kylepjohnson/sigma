//
// File:         BackwardSubsumption.cpp
// Description:  Interface for several BS related modules.
// Created:      Feb 26, 2000, 20:20
// Author:       Alexandre Riazanov
// mail:         riazanov@cs.man.ac.uk
//============================================================================
#include "BackwardSubsumption.hpp"
#include "ClauseNumIndex.hpp"
#include "Clause.hpp"
//============================================================================
#undef DEBUG_NAMESPACE
#ifdef DEBUG_BACKWARD_SUBSUMPTION
#define DEBUG_NAMESPACE "BackwardSubsumption"
#endif
#include "debugMacros.hpp"
//============================================================================
 
  using namespace BK;
  using namespace Gem;
  using namespace VK;

inline bool BackwardSubsumption::preproQuery(const Clause* cl)
{
  CALL("preproQuery(const Clause* cl)");
  _machine.resetQuery(cl,_index.sizes()); 

  for (LiteralList::Iterator iter(cl->LitList()); iter.notEnd(); iter.next())
    {
      TERM* lit = iter.currentLiteral();
      if (!preproQueryLit(lit)) return false;
    };
  return _machine.endOfQuery();
}; // bool BackwardSubsumption::preproQuery(const Clause* cl)


inline bool BackwardSubsumption::preproQueryLit(const TERM* lit)
{
  CALL("preproQueryLit(const TERM* lit)");
  if (lit->arity())
    { 
      if (lit->IsSymLitHeader()) 
	{ 
	  return (preproQuerySymLit(lit) && _machine.queryEndOfSymLit()); 
	}
      else // ordinary literal
	{
	  if (!_retrieval.litHeader(*lit)) return false;
	  _machine.queryNextLit();     
	  return  (preproQueryComplexTermArguments(lit) && _machine.queryEndOfLit());  
	};
    }
  else // propositional
    {
      const CSkipList* plcands;
      return ((_retrieval.propLit(*lit,plcands)) && (_machine.queryPropLit(plcands)));
    };
}; // bool BackwardSubsumption::preproQueryLit(const TERM* lit) 


inline bool BackwardSubsumption::preproQueryComplexTermArguments(const TERM* t) 
{
  CALL("preproQueryComplexTermArguments(const TERM* t)");
  ASSERT(t->isComplex());
  ASSERT(t->arity());
  const CLTSkipList* vcands;
  bool allVars = true;
  const TERM* argRef;
  for (argRef = t->Args(); ((!argRef->IsNull()) && allVars); argRef = argRef->Next())
    allVars = argRef->isVariable();
 
  // first argument
  argRef = t->Args();
  if (argRef->isVariable())
    {
      // primary only if all arguments are variables  
      if ((!_retrieval.variable(vcands)) || (!_machine.queryVariable(*argRef,vcands,allVars)))
	return false;  
    }
  else 
    if (!preproQueryComplexTerm(argRef->First())) return false;

  // other arguments
  argRef = argRef->Next();
  while (!argRef->IsNull())
    {
      if (argRef->isVariable())
	{
	  // always secondary  
	  if ((!_retrieval.variable(vcands)) || (!_machine.queryVariable(*argRef,vcands,false)))
	    return false;  
	}
      else 
	if (!preproQueryComplexTerm(argRef->First())) return false; 
      argRef = argRef->Next();    
    };   
  return true;
}; // bool BackwardSubsumption::preproQueryComplexTermArguments(const TERM* t)  


inline bool BackwardSubsumption::preproQueryComplexTerm(const TERM* t)
{
  CALL("preproQueryComplexTerm(const TERM* t)");
  ASSERT(t->isComplex());
  const CLSkipList* ccands;
  if (t->arity())
    {
      return _retrieval.nonconstFunc(*t) && preproQueryComplexTermArguments(t);
    }
  else // constant
    return _retrieval.constant(*t,ccands) && _machine.queryConstant(*t,ccands);
}; // bool BackwardSubsumption::preproQueryComplexTerm(const TERM* t) 

inline bool BackwardSubsumption::preproQuerySymLit(const TERM* lit)
{
  CALL("preproQuerySymLit(const TERM* lit)");
  ASSERT(lit->IsSymLitHeader());
  if (!_retrieval.symLitHeader(*lit)) return false;
  _machine.queryNextSymLit();
  const TERM* argRef1 = lit->Args();
  const TERM* argRef2 = argRef1->Next(); 
  const CLATSkipList* slvcands;

  // first argument
  if (argRef1->isVariable()) 
    {   
      // primary only if the second argument is also a variable
      if ((!_retrieval.symLitVariable(slvcands)) 
	  || (!_machine.querySymLitVariable(*argRef1,slvcands,argRef2->isVariable())))
	return false;      
    }
  else 
    if (!preproQuerySymLitComplexTerm(argRef1->First())) return false;

  // second argument
  if (!_retrieval.symLitSecondArg()) return false;
  _machine.querySymLitSecondArg();

  if (argRef2->isVariable()) 
    {   
      return (_retrieval.symLitVariable(slvcands)) 
	&& (_machine.querySymLitVariable(*argRef2,slvcands,false)); // always secondary position
    }
  else return preproQuerySymLitComplexTerm(argRef2->First()); 
}; // bool BackwardSubsumption::preproQuerySymLit(const TERM* lit)

inline bool BackwardSubsumption::preproQuerySymLitComplexTerm(const TERM* t)
{
  CALL("preproQuerySymLitComplexTerm(const TERM* t)");
  ASSERT(t->isComplex());
  const CLASkipList* slccands;

  if (t->arity())
    {
      return _retrieval.symLitNonconstFunc(*t) && preproQuerySymLitComplexTermArguments(t);
    }
  else // constant
    return _retrieval.symLitConstant(*t,slccands) && _machine.querySymLitConstant(*t,slccands);
}; // bool BackwardSubsumption::preproQuerySymLitComplexTerm(const TERM* t) 


inline bool BackwardSubsumption::preproQuerySymLitComplexTermArguments(const TERM* t) 
{
  CALL("preproQuerySymLitComplexTermArguments(const TERM* t)");
  ASSERT(t->isComplex());
  ASSERT(t->arity());
  const CLATSkipList* slvcands;

  bool allVars = true;
  const TERM* argRef;
  for (argRef = t->Args(); ((!argRef->IsNull()) && allVars); argRef = argRef->Next())
    allVars = argRef->isVariable();
 
  // first argument
  argRef = t->Args();          
  if (argRef->isVariable())
    {
      // primary only if all arguments are variables  
      if ((!_retrieval.symLitVariable(slvcands)) || (!_machine.querySymLitVariable(*argRef,slvcands,allVars)))
	return false;  
    }
  else 
    if (!preproQuerySymLitComplexTerm(argRef->First())) return false;

  // other arguments
  argRef = argRef->Next();
  while (!argRef->IsNull())
    {
      if (argRef->isVariable())
	{
	  // always secondary  
	  if ((!_retrieval.symLitVariable(slvcands)) || (!_machine.querySymLitVariable(*argRef,slvcands,false)))
	    return false;  
	}
      else 
	if (!preproQuerySymLitComplexTerm(argRef->First())) return false; 
      argRef = argRef->Next();    
    };   
  return true;
}; // bool BackwardSubsumption::preproQuerySymLitComplexTermArguments(const TERM* t)  



bool BackwardSubsumption::query(const Clause* cl) 
{
  CALL("query(const Clause* cl)");
  INTERCEPT_BACKWARD_SUBSUMPTION_INDEXING_RETRIEVAL_QUERY(cl);
  _queryClause = cl;
  bool res = preproQuery(cl); 
#ifdef INTERCEPT_BACKWARD_SUBSUMPTION_INDEXING
  if (!res) INTERCEPT_BACKWARD_SUBSUMPTION_INDEXING_RETRIEVAL_TOTAL;
#endif
  return res;
}; // bool BackwardSubsumption::query(const Clause* cl)

bool BackwardSubsumption::subsumeNextSetMode() 
{ 
  CALL("subsumeNextSetMode()");
  ulong subsumedClNum;

  //COP("COP10");

  while (_machine.subsumeNextSetMode(subsumedClNum)) 
    {
      //COP("COP20");
      _recentlySubsumedCl = _clauseNumIndex->find(subsumedClNum);
      //COP("COP30");
      ASSERT(_recentlySubsumedCl);    
      //COP("COP40");
#ifdef DEBUG_NAMESPACE
      _debugSubsumptionCheck.resetSubst();
      //COP("COP50");
      _machine.resetSubstitutionCollection();
      //COP("COP60");
      ulong debugVar;
      TERM debugTerm;
      while (_machine.substNextPair(debugVar,debugTerm)) 
	{
	  //COP("COP70");
	  _debugSubsumptionCheck.substPair(debugVar,debugTerm);
	};
      //COP("COP80");
      //ASSERT(_debugSubsumptionCheck.allVarsAssigned(_queryClause));
      //COP("COP90");
      //ASSERT(_debugSubsumptionCheck.subsumes(_queryClause,_recentlySubsumedCl));
      //COP("COP100");
#endif
      //COP("COP110");
      INTERCEPT_BACKWARD_SUBSUMPTION_INDEXING_RETRIEVAL_RESULT(_recentlySubsumedCl);
      //COP("COP120");
      if ((_recentlySubsumedCl->Active())
	  && (subsumptionAllowedInSetMode(_queryClause,_recentlySubsumedCl)))
	// this check must be done earlier in _machine, right after clause equalizing
	return true;
    };
  return false; 
}; // bool BackwardSubsumption::subsumeNextSetMode()


bool BackwardSubsumption::subsumeNextMultisetMode() 
{ 
  CALL("subsumeNextMultisetMode()");
  ulong subsumedClNum;
  while (_machine.subsumeNextMultisetMode(subsumedClNum)) 
    {
      _recentlySubsumedCl = _clauseNumIndex->find(subsumedClNum);

      ASSERT(_recentlySubsumedCl);    
#ifdef DEBUG_NAMESPACE
      _debugSubsumptionCheck.resetSubst();
      _machine.resetSubstitutionCollection();
      ulong debugVar;
      TERM debugTerm;
      while (_machine.substNextPair(debugVar,debugTerm)) 
	_debugSubsumptionCheck.substPair(debugVar,debugTerm);
      ASSERT(_debugSubsumptionCheck.allVarsAssigned(_queryClause));
      ASSERT(_debugSubsumptionCheck.subsumes(_queryClause,_recentlySubsumedCl));
#endif
      INTERCEPT_BACKWARD_SUBSUMPTION_INDEXING_RETRIEVAL_RESULT(_recentlySubsumedCl);
      if ((_recentlySubsumedCl->Active())
	  && (subsumptionAllowedInMultisetMode(_queryClause,_recentlySubsumedCl)))
	// this check must be done earlier in _machine, right after clause equalizing
	return true;
    };
 
  return false; 
}; // bool BackwardSubsumption::subsumeNextMultisetMode()



void BackwardSubsumption::integrate(const Clause* cl)
{
  CALL("integrate(const Clause* cl)");
  ASSERT(_clauseNumIndex->find(cl->Number()) == cl); 
  //DF; cout << "            WRP-6   " << _termTraversal.wrapper[0].Content() << "\n";
 

  INTERCEPT_BACKWARD_SUBSUMPTION_INDEXING_INTEGRATE(cl);

  _integrator.clause(cl->Number());
  TERM header;

  for(LiteralList::Iterator iter(cl->LitList());iter.notEnd();iter.next())
    { 
      header = *(iter.currentLiteral());
      if (header.arity())
	{
	  if (header.IsSymLitHeader())
	    {
	      _integrator.symLitHeader(header);
	      // integrating first argument

	      _termTraversal.reset(*(iter.currentLiteral()->Args()));
	      ASSERT(((bool)(_termTraversal)));

	      while (_termTraversal)       
		{
		  _integrator.symLitSubterm(_termTraversal.term());  
		  _termTraversal.next();
              
		};
	      // integrating second argument
	      _integrator.symLitSecondArg();
	      _termTraversal.reset(*(iter.currentLiteral()->Args()->Next()));
	      ASSERT(((bool)(_termTraversal)));
	      while (_termTraversal)       
		{
		  _integrator.symLitSubterm(_termTraversal.term());         
		  _termTraversal.next();
		};
	    }
	  else // ordinary literal
	    {
	      _integrator.litHeader(header);
	      _termTraversal.reset(iter.currentElement()->literalRef());
	      _termTraversal.next();
	      ASSERT(((bool)(_termTraversal)));
	      while (_termTraversal)       
		{
		  _integrator.subterm(_termTraversal.term());         
		  _termTraversal.next();
		};
	    };
	} 
      else // prop. lit.
	_integrator.propLit(header);
    };
  _integrator.endOfClause();
  ASSERT(preproQuery(cl));
}; // void BackwardSubsumption::integrate(const Clause* cl)


void BackwardSubsumption::remove(const Clause* cl)
{
  CALL("remove(const Clause* cl)"); 
  ASSERT(_clauseNumIndex->find(cl->Number()) == cl);   

  INTERCEPT_BACKWARD_SUBSUMPTION_INDEXING_REMOVE(cl);
 
  _removal.clause(cl->Number());
  TERM header;
  for(LiteralList::Iterator iter(cl->LitList());iter.notEnd();iter.next())
    { 
      header = *(iter.currentLiteral());   
      if (!header.isAnswerHeader())
	{  
	  if (header.arity())
	    {
	      if (header.IsSymLitHeader())
		{
		  if (_removal.symLitHeader(header))  
		    {     
		      // removing first argument
		      _termTraversal.reset(*(iter.currentLiteral()->Args()));
		      ASSERT(((bool)(_termTraversal)));
		      while (_termTraversal)       
			if (_removal.symLitSymbol(_termTraversal.symbol())) { _termTraversal.next(); }
			else { _removal.symLitAfter(); _termTraversal.after(); };
         
		      // removing second argument
		      _termTraversal.reset(*(iter.currentLiteral()->Args()->Next()));
		      _removal.symLitSecondArg();
		      ASSERT(((bool)(_termTraversal)));
		      while (_termTraversal)       
			if (_removal.symLitSymbol(_termTraversal.symbol())) { _termTraversal.next(); }
			else { _removal.symLitAfter(); _termTraversal.after(); }; 
         
		      // recycle
		      _removal.endOfSymLit();  
		    };    
		}
	      else // ordinary literal
		if (_removal.litHeader(header))
		  { 
		    _termTraversal.reset(iter.currentElement()->literalRef());
		    _termTraversal.next();
		    ASSERT(((bool)(_termTraversal)));
		    while (_termTraversal)
		      if (_removal.symbol(_termTraversal.symbol())) { _termTraversal.next(); }
		      else { _removal.after(); _termTraversal.after(); };
		    // recycle
		    _removal.endOfLit();  
		  };
	    } 
	  else // prop. lit.
	    _removal.propLit(header);
	};
    };
}; // void BackwardSubsumption::remove(const Clause* cl)

//============================================================================

