//
// File:         BackDemodulation.hpp
// Description:  Back simplification by ordered unit equalities.
// Created:      Apr 16, 2000, Alexandre Riazanov, riazanov@cs.man.ac.uk
// Revised:      Dec 23, 2001, Alexandre Riazanov, riazanov@cs.man.ac.uk
//               typedef Multiset<ulong,HellConst::MaxNumOfDifferentHeaders> 
//                 SetOfSplittingBranches
//                
//               replaced by 
// 
//               typedef ExpandingMultiset<GlobAlloc,ulong,1UL,BackDemodulation> 
//                 SetOfSplittingBranches
// Revised:      May 13, 2002, Alexandre Riazanov, riazanov@cs.man.ac.uk
//               1) Indexing for backward matching is now implemented 
//               with parh indexing and joins.
//               2) Ordering constraints reimplemented. 
//====================================================
#ifndef BACK_DEMODULATION_H
//====================================================
#define BACK_DEMODULATION_H
#include "jargon.hpp"
#include "GlobAlloc.hpp"
#include "VampireKernelDebugFlags.hpp"
#include "ExpandingMultiset.hpp"
#include "Term.hpp"
#include "NewClause.hpp"   
#include "InstanceRetrievalForDemodulation.hpp"
#include "LiteralList.hpp"
#include "RuntimeError.hpp"
//====================================================
#undef DEBUG_NAMESPACE
#ifdef DEBUG_BACK_DEMODULATION
#define DEBUG_NAMESPACE "BackDemodulation"
#endif
#include "debugMacros.hpp"
//====================================================

namespace VK
{
class Clause;
class ClauseNumIndex;
class SHARING_INDEX;
class BackDemodulation
{
 public:
  typedef BK::ExpandingMultiset<BK::GlobAlloc,ulong,32UL,BackDemodulation> SetOfSplittingBranches;

 public:
  BackDemodulation(NewClause* freshClause,ClauseNumIndex* clauseNumIndex);

  ~BackDemodulation();
  
  void init(NewClause* freshClause,ClauseNumIndex* clauseNumIndex);

  void destroy();
  void reset();

  void setDegree(long d) { _degree = d; };
  void setBDOnSplittingBranchesFlag(bool fl) 
    {
      if (_bDOnSplittingBranchesFlag && (!fl)) BK::RuntimeError::report("Switching _bDOnSplittingBranchesFlag off.");
      _bDOnSplittingBranchesFlag = fl; 
    };
  bool query(const Clause* cl);
  bool nextSimplified();
  bool collectInference();
  
  Clause* simplified() { return _recentlySimplified; }; 
  void integrate(Clause* cl);
  void remove(Clause* cl);
  ulong statBDOnSplittingBranches() const { return _numOfBDOnSplittingBranches; };
 private:
  bool query(const TERM* lhsref,const TERM* rhsref);
   
  bool appropriateDegree(const SimplificationOrdering::CompiledConstraintForInstanceRetrieval::Category& cat)
    {
      switch (cat)
	{	  
	case SimplificationOrdering::CompiledConstraintForInstanceRetrieval::NearlyTautology:
	  return  (_degree >= 2);
	case SimplificationOrdering::CompiledConstraintForInstanceRetrieval::Volatile:
	  return (_degree >= 3);
	case SimplificationOrdering::CompiledConstraintForInstanceRetrieval::NearlyContradiction:
	  return (_degree >= 4);
	default: return false;
	};
    };

  bool rewrite(const TERM::OccList::Node* occ);
  bool rewriteLit(TERM* lit,bool inhSel);  

  bool collectRHSInstance();
  
private:
  BackDemodulation() {};

 private:
  bool _swapQuerySides;
  long _degree;
  bool _bDOnSplittingBranchesFlag;
  NewClause* _resolvent;

  InstRet _index;
  InstRet::Substitution::Instance _rhsInstance;

  SimplificationOrdering::CompiledConstraintForInstanceRetrieval* _compiledConstraint;

  SimplificationOrdering::CompiledConstraintForInstanceRetrieval* _currentConstraint;



  ClauseNumIndex* _clauseNumIndex;
  const Clause* _query;
  ulong _queryNum;
  const TERM* _queryLHS;
  TERM _queryRHS;
  TERM::Iterator _termIter;
  const TERM::OccList::Node* _candidates; 
  Clause* _recentlySimplified;
  const TERM* _redex;
  TERM::Iterator _rewrittenLitIter;
  TERM::Iterator _RHSIter;
  SetOfSplittingBranches _splittingBranches;
  ulong _numOfBDOnSplittingBranches;
}; // class BackDemodulation


}; // namespace VK
//====================================================
#undef DEBUG_NAMESPACE
//====================================================
#endif
