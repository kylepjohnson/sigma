//
// File:         OldBackwardSubsumption.hpp
// Description:  Implements back subsumption using discrimination trees.
// Created:      Jan 14, 2001, 15:10
// Author:       Alexandre Riazanov
// mail:         riazanov@cs.man.ac.uk
//============================================================================
#ifndef OLD_BACKWARD_SUBSUMPTION_H
//=============================================================================
#define OLD_BACKWARD_SUBSUMPTION_H 
#include "jargon.hpp"
#include "VampireKernelDebugFlags.hpp"
#include "VampireKernelConst.hpp"
#include "OldBSIndex.hpp"
#include "OldBSMachine.hpp"
#include "ClauseNumIndex.hpp"
#include "Clause.hpp"
#include "Term.hpp"
#include "SubsumptionCheck.hpp" 
//============================================================================
#undef DEBUG_NAMESPACE
#ifdef DEBUG_OLD_BACKWARD_SUBSUMPTION
 #define DEBUG_NAMESPACE "OldBackwardSubsumption"
#endif
#include "debugMacros.hpp"
//============================================================================

namespace VK
{
  using namespace BK;
  using namespace Gem;
class OldBackwardSubsumption
{
 public:
  OldBackwardSubsumption(ClauseNumIndex* clauseNumIndex) 
   : _index(),
     _integrator(&_index),
     _removal(&_index),
     _retrieval(&_index),
     _machine(),        
     _termTraversal(),
     _clauseNumIndex(clauseNumIndex),
     _setMode(false),
     _optimisedMultisetMode(false)
     #ifdef DEBUG_NAMESPACE
      , _debugSubsumptionCheck()
     #endif 
  {
  };
  ~OldBackwardSubsumption() 
  {
   CALL("destructor ~OldBackwardSubsumption()");      
  };
  bool subsumptionAllowedInSetMode(const Clause* subsumingCl,const Clause* subsumedCl)
  {
   return subsumingCl->NumOfPosEq() <= subsumedCl->NumOfPosEq(); 
  };
  bool subsumptionAllowedInMultisetMode(const Clause* subsumingCl,const Clause* subsumedCl)
  {
   return true; 
  };
  void destroyIndex() { CALL("destroyIndex()"); _index.destroy(); };
  #ifndef NO_DEBUG
   bool clean() const { return _index.clean(); };
  #endif        
  void integrate(const Clause* cl);
  void remove(const Clause* cl);
  bool query(const Clause* cl);
  bool subsumeNext() 
  {
   if (_setMode) return subsumeNextSetMode();
   if (_optimisedMultisetMode && _queryClause->allMaximalSelected()) return subsumeNextSetMode();
   return subsumeNextMultisetMode();
  };
  bool subsumeNextSetMode();
  bool subsumeNextMultisetMode();
  const Clause* subsumed() const { return _recentlySubsumedCl; };
  void setSetMode(bool fl) { _setMode = fl; }; 
  void setOptimisedMultisetMode(bool fl) { _optimisedMultisetMode = fl; }; 
 private:
  void preproQuery(const Clause* cl);
  void preproQueryLit(const TERM* lit);
  void preproQuerySymLit(const TERM* lit);
  void preproQueryComplexTerm(const TERM* t);
  void preproQueryComplexTermArguments(const TERM* t);
  void preproQuerySymLitComplexTerm(const TERM* t);
  void preproQuerySymLitComplexTermArguments(const TERM* t);
 private:
  OldBSIndex _index;
  OldBSIndex::Integrator _integrator;
  OldBSIndex::Removal _removal;
  OldBSIndex::Retrieval _retrieval;
  OldBSMachine _machine;  
  TERM::Traversal _termTraversal; 
  ClauseNumIndex* _clauseNumIndex;
  const Clause* _recentlySubsumedCl;
  const Clause* _queryClause; 
  bool _setMode;
  bool _optimisedMultisetMode;
  #ifdef DEBUG_NAMESPACE
   SubsumptionCheck _debugSubsumptionCheck;
  #endif
}; // class OldBackwardSubsumption

inline void OldBackwardSubsumption::preproQuery(const Clause* cl)
{
 CALL("preproQuery(const Clause* cl)");
 _machine.resetQuery(cl); 

 for (LiteralList::Iterator iter(cl->LitList()); iter.notEnd(); iter.next()) preproQueryLit(iter.currentLiteral());
 _machine.endOfQuery();
}; // void OldBackwardSubsumption::preproQuery(const Clause* cl)


inline void OldBackwardSubsumption::preproQueryLit(const TERM* lit)
{
 CALL("preproQueryLit(const TERM* lit)");
}; // void OldBackwardSubsumption::preproQueryLit(const TERM* lit) 


inline void OldBackwardSubsumption::preproQueryComplexTermArguments(const TERM* t) 
{
 CALL("preproQueryComplexTermArguments(const TERM* t)");
 ASSERT(t->isComplex());
 ASSERT(t->arity());
}; // void OldBackwardSubsumption::preproQueryComplexTermArguments(const TERM* t)  


inline void OldBackwardSubsumption::preproQueryComplexTerm(const TERM* t)
{
 CALL("preproQueryComplexTerm(const TERM* t)");
 ASSERT(t->isComplex());
}; // void OldBackwardSubsumption::preproQueryComplexTerm(const TERM* t) 

inline void OldBackwardSubsumption::preproQuerySymLit(const TERM* lit)
{
 CALL("preproQuerySymLit(const TERM* lit)");
 ASSERT(lit->IsSymLitHeader());
}; // void OldBackwardSubsumption::preproQuerySymLit(const TERM* lit)

inline void OldBackwardSubsumption::preproQuerySymLitComplexTerm(const TERM* t)
{
 CALL("preproQuerySymLitComplexTerm(const TERM* t)");
 ASSERT(t->isComplex());
 
}; // void OldBackwardSubsumption::preproQuerySymLitComplexTerm(const TERM* t) 


inline void OldBackwardSubsumption::preproQuerySymLitComplexTermArguments(const TERM* t) 
{
 CALL("preproQuerySymLitComplexTermArguments(const TERM* t)");
 ASSERT(t->isComplex());
 ASSERT(t->arity());
}; // void OldBackwardSubsumption::preproQuerySymLitComplexTermArguments(const TERM* t)  

}; // namespace VK

 
//======================================================================
#undef DEBUG_NAMESPACE
//======================================================================
#endif
