//
// File:         OrdinaryUnification.cpp
// Description:  Ordinary unification for UTerm.
//               See also UTerm.hpp, UVariable.hpp
// Created:      Mar 18, 2003, Alexandre Riazanov, riazanov@cs.man.ac.uk
// Status:       Sketch.
//============================================================================
#include "RuntimeError.hpp"
#include "OrdinaryUnification.hpp"
using namespace BK;
using namespace VK;
//============================================================================
#ifdef DEBUG_ORDINARY_UNIFICATION
#  undef DEBUG_NAMESPACE
#  define DEBUG_NAMESPACE "OrdinaryUnification"
#endif
#include "debugMacros.hpp"
//============================================================================


bool OrdinaryUnification::unify(UTerm* term1,UTerm* term2)
{
  CALL("unify(UTerm* term1,UTerm* term2)");
  _boundVariables->reset();
  _traversal1.reset(term1);
  _traversal2.reset(term2);
  
  UTerm* subterm1;
  UTerm* subterm2;

 check_current:
  subterm1 = _traversal1.currentSubterm();
  subterm2 = _traversal2.currentSubterm();

  if (subterm1 == subterm2)
    {      
      if (_traversal1.after())
	{
	  ALWAYS(_traversal2.after());
	  goto check_current;
	};
      ASSERT(substIsCycleFree());
      ASSERT(term1->isSyntacticallyEqual(term2));
      return true;
    };

  switch (subterm1->tag())
    {
    case UTerm::QVarTag: // same as below
    case UTerm::QVarFirstOccTag:  // same as below
    case UTerm::TVarTag:  // same as below
    case UTerm::TVarFirstOccTag: 
      switch (subterm2->tag())
	{
	case UTerm::QVarTag: // same as below
	case UTerm::QVarFirstOccTag:  // same as below
	case UTerm::TVarTag:  // same as below
	case UTerm::TVarFirstOccTag: 
	  // two free variables
	  if (subterm1->asGenericVar()->var() != 
	      subterm2->asGenericVar()->var())
	    {
	      // different variables
	      bind(subterm1->asGenericVar()->var(),subterm2);
	    };
	  break;

	case UTerm::QConstTag: // same as below 
	case UTerm::TConstTag:
	  bind(subterm1->asGenericVar()->var(),subterm2);
	  break;
	  
	case UTerm::QOrdinaryCompoundTag: // same as below
	case UTerm::TOrdinaryCompoundTag:
	  if (subterm2->containsVar(subterm1->asGenericVar()->var()))
	    {
	      // cycle
	    fail:
	      undo();
	      return false;
	    };
	  bind(subterm1->asGenericVar()->var(),subterm2);
	  break;

#ifdef DEBUG_NAMESPACE
	default: ICP("ICP0"); return false;
#else
#  ifdef _SUPPRESS_WARNINGS_
	default: 
	  RuntimeError::report("Wrong tag in OrdinaryUnification::unify(UTerm* term1,UTerm* term2)"); 
	  return false;
#  endif
#endif
	}; // switch (subterm2->tag())
      break;
      
    case UTerm::QConstTag: // same as below 
    case UTerm::TConstTag:       
      switch (subterm2->tag())
	{
	case UTerm::QVarTag: // same as below
	case UTerm::QVarFirstOccTag:  // same as below
	case UTerm::TVarTag:  // same as below
	case UTerm::TVarFirstOccTag: 
	  bind(subterm2->asGenericVar()->var(),subterm1);
	  break;

	case UTerm::QConstTag: // same as below 
	case UTerm::TConstTag:
	  if (static_cast<UTerm::GenericCompound*>(subterm1)->topSymbol() != 
	      static_cast<UTerm::GenericCompound*>(subterm2)->topSymbol())
	    {
	      // function clash
	      goto fail;
	    };
	  break;

	case UTerm::QOrdinaryCompoundTag: // same as below
	case UTerm::TOrdinaryCompoundTag:
	  // function clash
	  ASSERT(static_cast<UTerm::GenericCompound*>(subterm1)->topSymbol() != 
		 static_cast<UTerm::GenericCompound*>(subterm2)->topSymbol());
	  goto fail;

#ifdef DEBUG_NAMESPACE
	default: ICP("ICP0"); return false;
#else
#  ifdef _SUPPRESS_WARNINGS_
	default: 
	  RuntimeError::report("Wrong tag in OrdinaryUnification::unify(UTerm* term1,UTerm* term2)"); 
	  return false;
#  endif
#endif
	}; // switch (subterm2->tag())
      break;

    case UTerm::QOrdinaryCompoundTag: // same as below
    case UTerm::TOrdinaryCompoundTag:       
      switch (subterm2->tag())
	{
	case UTerm::QVarTag: // same as below
	case UTerm::QVarFirstOccTag:  // same as below
	case UTerm::TVarTag:  // same as below
	case UTerm::TVarFirstOccTag:
	  if (subterm1->containsVar(subterm2->asGenericVar()->var()))
	    {
	      // cycle
	      goto fail;
	    };
	  bind(subterm2->asGenericVar()->var(),subterm1);
	  break;

	case UTerm::QConstTag: // same as below 
	case UTerm::TConstTag:
	  // function clash
	  ASSERT(static_cast<UTerm::GenericCompound*>(subterm1)->topSymbol() != 
		 static_cast<UTerm::GenericCompound*>(subterm2)->topSymbol());
	  goto fail;

	case UTerm::QOrdinaryCompoundTag: // same as below
	case UTerm::TOrdinaryCompoundTag:
	  if (static_cast<UTerm::GenericCompound*>(subterm1)->topSymbol() != 
	      static_cast<UTerm::GenericCompound*>(subterm2)->topSymbol())
	    {
	      // function clash
	      goto fail;
	    };
	  if (_traversal1.next())
	    {
	      ALWAYS(_traversal2.next());
	      goto check_current;
	    };

	  ASSERT(substIsCycleFree());
	  ASSERT(term1->isSyntacticallyEqual(term2));
	  return true;

#ifdef DEBUG_NAMESPACE
	default: ICP("ICP0"); return false;
#else
#  ifdef _SUPPRESS_WARNINGS_
	default: 
	  RuntimeError::report("Wrong tag in OrdinaryUnification::unify(UTerm* term1,UTerm* term2)"); 
	  return false;
#  endif
#endif
	}; // switch (subterm2->tag())
      break;
      
#ifdef DEBUG_NAMESPACE
    default: ICP("ICP0"); return false;
#else
#  ifdef _SUPPRESS_WARNINGS_
    default: 
      RuntimeError::report("Wrong tag in OrdinaryUnification::unify(UTerm* term1,UTerm* term2)"); 
      return false;
#  endif
#endif
    }; // switch (subterm1->tag())

  // skip terms

  if (_traversal1.after())
    {
      ALWAYS(_traversal2.after());
      goto check_current;
    };
  ASSERT(substIsCycleFree());
  ASSERT(term1->isSyntacticallyEqual(term2));
  return true;
}; // bool OrdinaryUnification::unify(UTerm* term1,UTerm* term2)


//============================================================================
