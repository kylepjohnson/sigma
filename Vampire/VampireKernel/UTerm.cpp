//
// File:         UTerm.cpp
// Description:  New specialised term representation for unification. 
// Created:      Mar 15, 2003, Alexandre Riazanov, riazanov@cs.man.ac.uk
// Status:       Sketch.
//============================================================================
#include "UTerm.hpp"
using namespace BK;
using namespace VK;
//============================================================================

UTerm::Domain* UTerm::_currentDomain; // implicitely initialised to 0

//============================================================================
#ifdef DEBUG_U_TERM
#undef DEBUG_NAMESPACE
 #define DEBUG_NAMESPACE "UTerm"
#endif
#include "debugMacros.hpp"
//============================================================================

bool UTerm::containsVar(const UVariable* var)
{
  CALL("containsVar(const UVariable* var)");
  return true; // temporary
}; // bool UTerm::containsVar(const UVariable* var)


 

#ifndef NO_DEBUG_VIS
ostream& UTerm::output(ostream& str)
{
  CALL("output(ostream& str)");

  // temporary! assumes that after() is always OK!

  if (isOrdinaryCompound())
    {
      str << symbol() << '(';
      for (UTerm* arg = next(); arg != after(); arg = arg->after())
	{
	  arg->output(str);
	  if (arg->after() != after())
	    str << ',';
	};
      return str << ')';
    }
  else
    if (isVariable())
      {
	if (var()->isFree())
	  {
	    return var()->output(str);
	  }
	else
	  return deref(var())->output(str);
      }
    else
      {
	ASSERT(isConstant());
	return str << symbol();
      };
}; // ostream& UTerm::output(ostream& str)

#endif



#ifndef NO_DEBUG
bool UTerm::isSyntacticallyEqual(UTerm* term)
{
  CALL("isSyntacticallyEqual(UTerm* term)");
  return false; // temporary
}; // bool UTerm::isSyntacticallyEqual(UTerm* term)
#endif



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
  ostream& operator<<(ostream& str,VK::UTerm* term)
  {
    CALL("operator<<(ostream& str,VK::UTerm* term)");
    return term->output(str);
  };

}; // namespace std
#endif



//============================================================================










