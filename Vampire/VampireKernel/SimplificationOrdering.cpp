//
// File:         SimplificationOrdering.cpp
// Description:  Simplification ordering abstraction.  
// Created:      Apr 19, 2002, Alexandre Riazanov, riazanov@cs.man.ac.uk
//=================================================================
#include "SimplificationOrdering.hpp"
#include "NonrecursiveKBOrdering.hpp"
//===================================================================
#undef DEBUG_NAMESPACE
#ifdef DEBUG_SIMPLIFICATION_ORDERING
 #define DEBUG_NAMESPACE "SimplificationOrdering"
#endif
#include "debugMacros.hpp"
//===================================================================

using namespace VK;

SimplificationOrdering* SimplificationOrdering::_current;

SimplificationOrdering::~SimplificationOrdering()
{
  CALL("destructor ~SimplificationOrdering()");
};


//===================================================================
#undef DEBUG_NAMESPACE
#ifdef DEBUG_SIMPLIFICATION_ORDERING
 #define DEBUG_NAMESPACE "SimplificationOrdering::SpecialisedComparison"
#endif
#include "debugMacros.hpp"
//===================================================================


SimplificationOrdering::SpecialisedComparison::~SpecialisedComparison()
{
  CALL("destructor ~SpecialisedComparison()");
};

//===================================================================
#undef DEBUG_NAMESPACE
#ifdef DEBUG_SIMPLIFICATION_ORDERING
#define DEBUG_NAMESPACE "SimplificationOrdering::ConstraintCheckOnUnifier"
#endif
#include "debugMacros.hpp"
//===================================================================

SimplificationOrdering::ConstraintCheckOnUnifier::~ConstraintCheckOnUnifier()
{
  CALL("destructor ~ConstraintCheckOnUnifier()");
};

//===================================================================
#undef DEBUG_NAMESPACE
#ifdef DEBUG_SIMPLIFICATION_ORDERING
#define DEBUG_NAMESPACE "SimplificationOrdering::ConstraintCheckOnForwardMatchingSubst"
#endif
#include "debugMacros.hpp"
//===================================================================

SimplificationOrdering::ConstraintCheckOnForwardMatchingSubst::~ConstraintCheckOnForwardMatchingSubst()
{
  CALL("destructor ~ConstraintCheckOnForwardMatchingSubst()");
};
//===================================================================
#undef DEBUG_NAMESPACE
#ifdef DEBUG_SIMPLIFICATION_ORDERING
 #define DEBUG_NAMESPACE "SimplificationOrdering::StoredConstraint"
#endif
#include "debugMacros.hpp"
//===================================================================

SimplificationOrdering::StoredConstraint::~StoredConstraint()
{
  CALL("destructor StoredConstraint::~StoredConstraint()");
}; 


//===================================================================
#undef DEBUG_NAMESPACE
#ifdef DEBUG_SIMPLIFICATION_ORDERING
 #define DEBUG_NAMESPACE "SimplificationOrdering::CompiledConstraintForUnifiers"
#endif
#include "debugMacros.hpp"
//===================================================================

SimplificationOrdering::CompiledConstraintForUnifiers::~CompiledConstraintForUnifiers()
{  
  CALL("destructor ~CompiledConstraintForUnifiers()");
};

//===================================================================
#undef DEBUG_NAMESPACE
#ifdef DEBUG_SIMPLIFICATION_ORDERING
 #define DEBUG_NAMESPACE "SimplificationOrdering::CompiledConstraintForInstanceRetrieval"
#endif
#include "debugMacros.hpp"
//===================================================================


SimplificationOrdering::CompiledConstraintForInstanceRetrieval::~CompiledConstraintForInstanceRetrieval()
{
  CALL("destructor ~CompiledConstraintForInstanceRetrieval()");
};
//=================================================================
