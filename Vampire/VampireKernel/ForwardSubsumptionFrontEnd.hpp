//
// File:         ForwardSubsumptionFrontEnd.hpp
// Description:  An interface to the forward subsumption core.
// Created:      Dec 15, 2001 
// Author:       Alexandre Riazanov
// mail:         riazanov@cs.man.ac.uk
//============================================================================
#ifndef FORWARD_SUBSUMPTION_FRONT_END_H
//============================================================================
#define FORWARD_SUBSUMPTION_FRONT_END_H 
#include "jargon.hpp"
#include "VampireKernelDebugFlags.hpp"
#include "forward_subsumption.hpp"
#include "tmp_literal.hpp"
#include "TmpLitList.hpp"
#include "ClauseBackground.hpp"
#include "LiteralList.hpp"
//============================================================================
#undef DEBUG_NAMESPACE
#ifdef DEBUG_FORWARD_SUBSUMPTION_FRONT_END
#define DEBUG_NAMESPACE "ForwardSubsumptionFrontEnd"
#endif
#include "debugMacros.hpp"
//============================================================================

namespace VK
{
class ForwardSubsumptionFrontEnd : public ForwardSubsumption
{
 public: 
  ForwardSubsumptionFrontEnd() 
    : _subsumptionResolutionMinLength(0UL) 
    {
    };
  ~ForwardSubsumptionFrontEnd() {};
  void init()
  {
    CALL("init()");
    ForwardSubsumption::init();
    _subsumptionResolutionMinLength = 0UL; 
  };
  void destroy()
  {
    CALL("destroy()");
    ForwardSubsumption::destroy();
  };
  void reset()
  {
    CALL("reset()");
    ForwardSubsumption::reset();
    _subsumptionResolutionMinLength = 0UL; 
  };
  void setSubsumptionResolutionMinLength(ulong ln) 
    {
      _subsumptionResolutionMinLength = ln;
    };
  Clause* subsume(TmpLitList& lits); 
  ulong simplifyBySubsumptionResolution(TmpLitList& lits,OpenClauseBackground& ancestors);

  //public: // temporary

 private: 
  void pushLiteral(TmpLiteral* lit);
 private:
  ulong _subsumptionResolutionMinLength;
}; // class ForwardSubsumptionFrontEnd

}; // namespace VK

//============================================================================
#undef DEBUG_NAMESPACE
#ifdef DEBUG_FORWARD_SUBSUMPTION_FRONT_END
#define DEBUG_NAMESPACE "ForwardSubsumptionFrontEnd"
#endif
#include "debugMacros.hpp"
//============================================================================

namespace VK
{
inline Clause* ForwardSubsumptionFrontEnd::subsume(TmpLitList& lits) 
{
  CALL("subsume(TmpLitList& lits)");
  
  ForwardSubsumption::ResetQuery(); 
  
  for (TmpLiteral* el = lits.first(); el; el = el->next())
    {  
      pushLiteral(el); 
    };
  Clause* subsumingClause = ForwardSubsumption::Subsume();
  if (subsumingClause) subsumingClause->MakeVIP();
  return subsumingClause;
}; //Clause* ForwardSubsumptionFrontEnd::subsume(TmpLitList& lits)  

inline void ForwardSubsumptionFrontEnd::pushLiteral(TmpLiteral* lit)
{
  CALL("pushLiteral(TmpLiteral* lit)");
  if (lit->isEquality())
    {       
      lit->linkEqArgs();

      if (lit->isOrderedEquality())
	{
	  ForwardSubsumption::PushOrdEq(lit->header(),lit->arg1(),lit->arg2());
	}
      else ForwardSubsumption::PushUnordEq(lit->header(),lit->arg1(),lit->arg2()); 
    }
  else ForwardSubsumption::PushLit(lit->flatterm()); 
}; // void ForwardSubsumptionFrontEnd::pushLiteral(TmpLiteral* lit)




inline ulong ForwardSubsumptionFrontEnd::simplifyBySubsumptionResolution(TmpLitList& lits,OpenClauseBackground& ancestors)
{
  CALL("simplifyBySubsumptionResolution(TmpLitList& lits,OpenClauseBackground& ancestors)");
  if (lits.numOfAllLiterals() < _subsumptionResolutionMinLength) return 0;
  ulong res = 0;
  

  TmpLiteral* candEl = lits.last();

 try_lit: 
  if (candEl->isEquality() ||
      candEl->isAnswerLiteral())
    {
      if (candEl == lits.first()) return res;
      candEl = candEl->previous();
      goto try_lit;
    };

  // try to simplify by resolving cand_lit
  candEl->flatterm()->symbolRef().Inversepolarity();
   
  ResetQuery(); 

  for (TmpLiteral* el = lits.first(); el; el = el->next())
    pushLiteral(el);
            
  Clause* simplifier = Subsume();
  if (simplifier)
    {
      simplifier->MakeVIP();
      res++;
      ancestors.pushAncestor(simplifier,simplifier->inferenceDepth());
      if (simplifier->isSubgoal()) ancestors.markAsSubgoal();
      simplifier->setFlag(Clause::FlagHasChildren);
      
      TmpLiteral* tmp = candEl;
      candEl = candEl->next();
      lits.remove(tmp);
      tmp->recycle();

      if ((lits.numOfAllLiterals() < _subsumptionResolutionMinLength) 
	  || (candEl == lits.first()))
	return res;
      if (candEl) { candEl = candEl->previous(); }
      else
	candEl = lits.last();
      goto try_lit;  
    }
  else 
    {
      candEl->flatterm()->symbolRef().Inversepolarity();
      if (candEl == lits.first()) return res;
      candEl = candEl->previous(); 
      goto try_lit;
    }; 
}; // ulong ForwardSubsumptionFrontEnd::simplifyBySubsumptionResolution(TmpLitList& lits,OpenClauseBackground& ancestors)


}; // namespace VK



//======================================================================
#endif
