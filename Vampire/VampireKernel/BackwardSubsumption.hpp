//
// File:         BackwardSubsumption.hpp
// Description:  Interface for several BS related modules.
// Created:      Feb 26, 2000, Alexandre Riazanov, riazanov@cs.man.ac.uk
// Revised:      Dec 03, 2002, Alexandre Riazanov, riazanov@cs.man.ac.uk
//               Slightly messed the code up in order to compile it by VC++ 6.0.
//============================================================================
#ifndef BACKWARD_SUBSUMPTION_H
//=============================================================================
#define BACKWARD_SUBSUMPTION_H 
#include "jargon.hpp"
#include "VampireKernelDebugFlags.hpp"
#include "VampireKernelInterceptMacros.hpp"
#include "BSIndex.hpp"
#include "BSMachine.hpp"
#include "ClauseNumIndex.hpp"
#include "Clause.hpp"
#include "Term.hpp"
#ifndef NO_DEBUG
#  include "SubsumptionCheck.hpp" 
#endif
//============================================================================
#undef DEBUG_NAMESPACE
#ifdef DEBUG_BACKWARD_SUBSUMPTION
#define DEBUG_NAMESPACE "BackwardSubsumption"
#endif
#include "debugMacros.hpp"
//============================================================================


namespace VK
{

class BackwardSubsumption
{
 public:
  typedef BSIndex::LTPair LTPair;  
  
  typedef BSIndex::LAPair LAPair;
  typedef BSIndex::LATTriple LATTriple;
  typedef BSIndex::LList LList;
  typedef BSIndex::LTList LTList;
  typedef BSIndex::LAList LAList;
  typedef BSIndex::LATList LATList;
  typedef BSIndex::CSkipList CSkipList;
  typedef BSIndex::CLSkipList CLSkipList;
  typedef BSIndex::CLTSkipList CLTSkipList;
  typedef BSIndex::CLASkipList CLASkipList;
  typedef BSIndex::CLATSkipList CLATSkipList;
  
 public:
  // VC++ 6.0 breaks down when compiling any
  // constructor or destructor definition for BackwardSubsumption.

  /**************
  BackwardSubsumption(ClauseNumIndex* clauseNumIndex) : 
    _integrator(&_index),
    _removal(&_index),
    _retrieval(&_index),
    _termTraversal(DOP("BackwardSubsumption::_termTraversal")),  
    _clauseNumIndex(clauseNumIndex),
    _setMode(false),
    _optimisedMultisetMode(false)
  {
    CALL("constructor BackwardSubsumption(ClauseNumIndex* clauseNumIndex)");
  }; // BackwardSubsumption(ClauseNumIndex* clauseNumIndex)

  ~BackwardSubsumption()
  {
    CALL("destructor ~BackwardSubsumption()");
  };
  ***************/

  void init(ClauseNumIndex* clauseNumIndex)
  {
    CALL("init(ClauseNumIndex* clauseNumIndex)");
    _index.init();
    _integrator.init(&_index);
    _removal.init(&_index);
    _retrieval.init(&_index);
    _machine.init();
    _termTraversal.init(DOP("BackwardSubsumption::_termTraversal"));
    _clauseNumIndex = clauseNumIndex;
    _setMode = false;
    _optimisedMultisetMode = false;
#ifdef DEBUG_NAMESPACE
    _debugSubsumptionCheck.init();
#endif
    
  };

  /***********
  void init()
  {
    CALL("init()");
    _index.init();
    _integrator.init();
    _removal.init();
    _retrieval.init();
    _machine.init();
    _termTraversal.init(DOP("BackwardSubsumption::_termTraversal"));
#ifdef DEBUG_NAMESPACE
    _debugSubsumptionCheck.init();
#endif
  };
*********/

  void destroy()
  {
    CALL("destroy()");
#ifdef DEBUG_NAMESPACE
    _debugSubsumptionCheck.destroy();
#endif
    _termTraversal.destroy();
    _machine.destroy();
    _retrieval.destroy();
    _removal.destroy();
    _integrator.destroy();
    _index.destroy();
    BK_CORRUPT(*this);
  };

  void reset()
  {
    CALL("reset()");
    _index.destroy();
    _index.init();
  }; // void reset()
  
  
  bool subsumptionAllowedInSetMode(const Clause* subsumingCl,const Clause* subsumedCl)
    {
      return subsumingCl->NumOfPosEq() <= subsumedCl->NumOfPosEq(); 
    };
  bool subsumptionAllowedInMultisetMode(const Clause* subsumingCl,const Clause* subsumedCl)
    {
      return true; 
    };     
  void integrate(const Clause* cl);
  void remove(const Clause* cl);
  bool query(const Clause* cl);
  bool subsumeNext() 
    {
      CALL("subsumeNext()");
      bool res;
      if (_setMode) 
	{
	  res = subsumeNextSetMode();
	}
      else 
	if (_optimisedMultisetMode && _queryClause->allMaximalSelected())
	  {
	    res = subsumeNextSetMode();
	  }
	else 
	  res = subsumeNextMultisetMode();
  
#ifdef INTERCEPT_BACKWARD_SUBSUMPTION_INDEXING
      if (!res) { INTERCEPT_BACKWARD_SUBSUMPTION_INDEXING_RETRIEVAL_TOTAL; };
#endif 

      return res;
    };
  bool subsumeNextSetMode();
  bool subsumeNextMultisetMode();
  Clause* subsumed() const { return _recentlySubsumedCl; };
  void setSetMode(bool fl) { _setMode = fl; }; 
  void setOptimisedMultisetMode(bool fl) { _optimisedMultisetMode = fl; }; 

 private:
	
  bool preproQuery(const Clause* cl);
  bool preproQueryLit(const TERM* lit);
  bool preproQuerySymLit(const TERM* lit);
  bool preproQueryComplexTerm(const TERM* t);
  bool preproQueryComplexTermArguments(const TERM* t);
  bool preproQuerySymLitComplexTerm(const TERM* t);
  bool preproQuerySymLitComplexTermArguments(const TERM* t);
 private:
  BSIndex _index;
  BSIndex::Integrator _integrator;
  BSIndex::Removal _removal;
  BSIndex::Retrieval _retrieval;
  BSMachine _machine; 
  TERM::Traversal _termTraversal;
  ClauseNumIndex* _clauseNumIndex;
  Clause* _recentlySubsumedCl;
  const Clause* _queryClause; 
  bool _setMode;
  bool _optimisedMultisetMode;
#ifdef DEBUG_NAMESPACE
  SubsumptionCheck _debugSubsumptionCheck;
#endif
}; // class BackwardSubsumption

}; // namespace VK

//======================================================================
#endif
