//
// File:         OldBackwardSubsumption.cpp
// Description:  Implements back subsumption using discrimination trees.
// Created:      Jan 14, 2001, 18:00
// Author:       Alexandre Riazanov
// mail:         riazanov@cs.man.ac.uk
//============================================================================
#include "OldBackwardSubsumption.hpp"
//============================================================================
#undef DEBUG_NAMESPACE
#ifdef DEBUG_OLD_BACKWARD_SUBSUMPTION
 #define DEBUG_NAMESPACE "OldBackwardSubsumption"
#endif
#include "debugMacros.hpp"
//============================================================================

  using namespace BK;
  using namespace Gem;
using namespace VK;

void OldBackwardSubsumption::integrate(const Clause* cl)
{
 CALL("integrate(const Clause* cl)");
 _integrator.clause(cl->Number());
 ulong ln = 0;
 for (LiteralList::Iterator iter(cl->LitList()); iter.notEnd(); iter.next())
  {
   const TERM* lit = iter.currentLiteral();
   if (lit->arity()) { _integrator.literal(lit,ln); }
   else _integrator.propLit(lit);
   ln++;
  };
}; // void OldBackwardSubsumption::integrate(const Clause* cl) 


//============================================================================
