//
// File:         OrdinaryUnification.hpp
// Description:  Ordinary unification for UTerm.
//               See also UTerm.hpp, UVariable.hpp
// Created:      Mar 18, 2003, Alexandre Riazanov, riazanov@cs.man.ac.uk
// Status:       Sketch.
//============================================================================
#ifndef ORDINARY_UNIFICATION_H
#define ORDINARY_UNIFICATION_H
//============================================================================
#ifndef NO_DEBUG_VIS
#  include <iostream>
#endif
#include "jargon.hpp"
#include "DestructionMode.hpp"
#include "GlobAlloc.hpp"
#include "VampireKernelDebugFlags.hpp"
#include "UTerm.hpp"
#include "UVariable.hpp"
//============================================================================
#ifdef DEBUG_ORDINARY_UNIFICATION
#  undef DEBUG_NAMESPACE
#  define DEBUG_NAMESPACE "OrdinaryUnification"
#endif
#include "debugMacros.hpp"
//============================================================================

namespace VK 
{
  class OrdinaryUnification
  {
  public:
    OrdinaryUnification() 
    {
      CALL("OrdinaryUnification()");
      _boundVariables = UVariable::currentDomain()->reserveStack();
    };
    ~OrdinaryUnification() 
    {
      CALL("~OrdinaryUnification()");
      if (BK::DestructionMode::isThorough())
	{
	  UVariable::currentDomain()->releaseStack(_boundVariables);
	};
    };
    bool unify(UTerm* term1,UTerm* term2);
    void undo();
  private:
    void bind(UVariable* var,UTerm* term)
    {
      CALL("bind(UVariable* var,UTerm* term)");
      ASSERT(var->isFree());
      var->instantiate(term);
      _boundVariables->push(var);
    };
#ifdef DEBUG_NAMESPACE
    bool substIsCycleFree() const
    {
      CALL("substIsCycleFree() const");  
      const UVariable::Stack::Entry* end = _boundVariables->end();
      for (const UVariable::Stack::Entry* e = _boundVariables->begin();
	   e != end;
	   e = e->next())
	if (!e->value()->isCycleFree())
	  return false;
      return true;
    };
#endif
  private:
    UTerm::Traversal _traversal1;
    UTerm::Traversal _traversal2;
    UVariable::Stack* _boundVariables;
  }; // class OrdinaryUnification
}; // namespace VK

//============================================================================
#ifdef DEBUG_ORDINARY_UNIFICATION
#  undef DEBUG_NAMESPACE
#  define DEBUG_NAMESPACE "OrdinaryUnification"
#endif
#include "debugMacros.hpp"
//============================================================================

namespace VK 
{

  inline
  void OrdinaryUnification::undo()
  {
    CALL("undo()");
    while (_boundVariables->nonempty()) _boundVariables->pop()->makeFree();
  }; // void undo()
 
}; // namespace VK

//============================================================================
#endif
