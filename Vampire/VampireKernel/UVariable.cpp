//
// File:         UVariable.cpp
// Description:  New specialised representation of variables for unification.
//               See also UTerm.hpp 
// Created:      Mar 15, 2003, Alexandre Riazanov, riazanov@cs.man.ac.uk
// Status:       Sketch.
//============================================================================
#include "UVariable.hpp"
#include "UTerm.hpp"
using namespace VK;
//============================================================================

UVariable::Domain* UVariable::_currentDomain; // implicitely initialised to 0

//============================================================================
#ifdef DEBUG_U_VARIABLE
#  undef DEBUG_NAMESPACE
#  define DEBUG_NAMESPACE "UVariable"
#endif
#include "debugMacros.hpp"
//============================================================================

#ifndef NO_DEBUG_VIS

ostream& UVariable::output(ostream& str) const
{
  CALL("output(ostream& str) const");
  return str << "U" << absoluteNumber();
}; // ostream& UVariable::output(ostream& str) const

#endif

 
#ifndef NO_DEBUG
bool UVariable::isCycleFree()
{
  CALL("isCycleFree()");
  return 
    isFree() || 
    (!content()->containsVar(this));
};
#endif



//============================================================================
