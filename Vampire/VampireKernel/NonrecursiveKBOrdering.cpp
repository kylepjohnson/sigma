//
// File:         NonrecursiveKBOrdering.cpp
// Description:  Nonrecursive Knuth-Bendix ordering.  
// Created:      Apr 16, 2002, Alexandre Riazanov, riazanov@cs.man.ac.uk
// Revised:      May 14, 2002, Alexandre Riazanov, riazanov@cs.man.ac.uk
//               Bug fix in 
//               NonrecursiveKBOrdering::CompiledConstraintForInstanceRetrieval::interpretWeightComparisonCode()
// Revised:      May 14, 2002, Alexandre Riazanov, riazanov@cs.man.ac.uk
//               Some weight comparison commands changed. 
// Revised:      May 15, 2002, Alexandre Riazanov, riazanov@cs.man.ac.uk
//               Bug fix in  
//               CompiledConstraintForInstanceRetrieval::interpretWeightComparisonCode()
//=================================================================
#include "NonrecursiveKBOrdering.hpp"
#include "TermWeighting.hpp"
#include "variables.hpp"
#include "ForwardMatchingSubstitutionCache.hpp"
#include "DestructionMode.hpp"
#include "TermWeightType.hpp"
#include "VampireKernelInterceptMacros.hpp"
//===================================================================
#undef DEBUG_NAMESPACE
#ifdef DEBUG_NONRECURSIVE_KB_ORDERING
#define DEBUG_NAMESPACE "NonrecursiveKBOrdering"
#endif
#include "debugMacros.hpp"
//===================================================================

using namespace BK;
using namespace VK;


NonrecursiveKBOrdering::NonrecursiveKBOrdering() : 
  _specialisedComparisonPool(0),
  _constraintCheckOnUnifierPool(0),
  _constraintCheckOnForwardMatchingSubstPool(0),
  _compiledConstraintOnUnifiersPool(0),
  _compiledConstraintForInstanceRetrievalPool(0)
{    
  setType(SimplificationOrdering::NonrecursiveKB);
}; // NonrecursiveKBOrdering::NonrecursiveKBOrdering()

NonrecursiveKBOrdering::~NonrecursiveKBOrdering()
{
  CALL("destructor ~NonrecursiveKBOrdering()");
  if (DestructionMode::isThorough()) 
    {
      
      while (_specialisedComparisonPool)
	{
	  SpecialisedComparison* tmp = _specialisedComparisonPool;
	  _specialisedComparisonPool = _specialisedComparisonPool->getNextInPool();
	  delete tmp;
	};

      while (_constraintCheckOnUnifierPool)
	{
	  ConstraintCheckOnUnifier* tmp = _constraintCheckOnUnifierPool;
	  _constraintCheckOnUnifierPool = _constraintCheckOnUnifierPool->getNextInPool();
	  delete tmp;
	};
  

      while (_constraintCheckOnForwardMatchingSubstPool)
	{
	  ConstraintCheckOnForwardMatchingSubst* tmp = _constraintCheckOnForwardMatchingSubstPool;
	  _constraintCheckOnForwardMatchingSubstPool = _constraintCheckOnForwardMatchingSubstPool->getNextInPool();
	  delete tmp;
	};

      while (_compiledConstraintOnUnifiersPool)
	{
	  CompiledConstraintForUnifiers* tmp = _compiledConstraintOnUnifiersPool;
	  _compiledConstraintOnUnifiersPool = _compiledConstraintOnUnifiersPool->getNextInPool();      
	  delete tmp;
	};


      while (_compiledConstraintForInstanceRetrievalPool)
	{
	  CompiledConstraintForInstanceRetrieval* tmp = _compiledConstraintForInstanceRetrievalPool;

	  _compiledConstraintForInstanceRetrievalPool = _compiledConstraintForInstanceRetrievalPool->getNextInPool();  
    
	  delete tmp;
	};  
    };
}; // NonrecursiveKBOrdering::~NonrecursiveKBOrdering()

  


void* NonrecursiveKBOrdering::operator new(size_t)
{
  return GlobAlloc::allocate(sizeof(NonrecursiveKBOrdering));
};

void NonrecursiveKBOrdering::operator delete(void* obj)
{
  GlobAlloc::deallocate(obj,sizeof(NonrecursiveKBOrdering));
}; 


Comparison NonrecursiveKBOrdering::compare(const Flatterm* term1,
					   const WeightPolynomial& weight1,
					   const Flatterm* term2,
					   const WeightPolynomial& weight2)
{
  CALL("compare(..)");
  Comparison lexCmp;
  switch (weight1.compare(weight2))
    {
    case BK::FailureToCompare:
      return BK::Incomparable;
    case BK::AlwaysLess:
      return BK::Less;
    case BK::AlwaysEqual:
      return compareLexicographically(term1,term2);
    case BK::AlwaysGreater:
      return BK::Greater;
    case BK::CanBeLessOrEqual:
      lexCmp = compareLexicographically(term1,term2);
      if (lexCmp == BK::Less) return BK::Less;
      ASSERT(lexCmp != BK::Equal);
      return BK::Incomparable;
    case BK::CanBeGreaterOrEqual:
      lexCmp = compareLexicographically(term1,term2);
      if (lexCmp == BK::Greater) return BK::Greater;
      ASSERT(lexCmp != BK::Equal);
      return BK::Incomparable;
    case BK::VolatileButNearlyAlwaysLess:
      // same as below
    case BK::VolatileButNearlyAlwaysGreater:
      // same as below
    case BK::AbsolutelyVolatile:
      return BK::Incomparable;
    };
  ICP("ICP0");

#if (defined _SUPPRESS_WARNINGS_) || (defined DEBUG_NAMESPACE)
  return BK::Incomparable;
#endif

}; // Comparison NonrecursiveKBOrdering::compare(..)




Comparison NonrecursiveKBOrdering::compareAtoms(const Flatterm* lit1,
						const WeightPolynomial& weight1,
						const Flatterm* lit2,
						const WeightPolynomial& weight2)
{
  CALL("compareAtoms(..)");
  Comparison lexCmp;
  switch (weight1.compare(weight2))
    {
    case BK::FailureToCompare:
      return BK::Incomparable;
    case BK::AlwaysLess:
      return Less;
    case BK::AlwaysEqual:
      return compareAtomsLexicographically(lit1,lit2);
    case BK::AlwaysGreater:
      return Greater;
    case BK::CanBeLessOrEqual:
      lexCmp = compareAtomsLexicographically(lit1,lit2);
      if (lexCmp == BK::Less) return BK::Less;
      ASSERT(lexCmp != BK::Equal);
      return BK::Incomparable;
    case BK::CanBeGreaterOrEqual:
      lexCmp = compareAtomsLexicographically(lit1,lit2);
      if (lexCmp == BK::Greater) return BK::Greater;
      ASSERT(lexCmp != BK::Equal);
      return BK::Incomparable;
    case BK::VolatileButNearlyAlwaysLess:
      // same as below
    case BK::VolatileButNearlyAlwaysGreater:
      // same as below
    case BK::AbsolutelyVolatile:
      return BK::Incomparable;
    };

  ICP("ICP0");
#if (defined _SUPPRESS_WARNINGS_) || (defined DEBUG_NAMESPACE)
  return BK::Incomparable;
#endif

}; // Comparison NonrecursiveKBOrdering::compareAtoms(..)



Comparison NonrecursiveKBOrdering::compareOnCurrentUnifier(PrefixSym* complexTerm1,
							   PrefixSym* complexTerm2)
{
  CALL("compareOnCurrentUnifier(..)");
  _weight1.reset();
  _weight2.reset();
  TermWeighting::current()->collectWeight(_weight1,complexTerm1);
  TermWeighting::current()->collectWeight(_weight2,complexTerm2);
  Comparison lexCmp;
  switch (_weight1.compare(_weight2))
    {
    case BK::FailureToCompare:
      return BK::Incomparable;
    case BK::AlwaysLess:
      return BK::Less;
    case BK::AlwaysEqual:
      return compareLexOnCurrentUnifier(complexTerm1,complexTerm2);
    case BK::AlwaysGreater:
      return BK::Greater;
    case BK::CanBeLessOrEqual:
      lexCmp = compareLexOnCurrentUnifier(complexTerm1,complexTerm2);
      if (lexCmp == BK::Less) return BK::Less;
      ASSERT(lexCmp != BK::Equal);
      return BK::Incomparable;
      
    case BK::CanBeGreaterOrEqual:
      lexCmp = compareLexOnCurrentUnifier(complexTerm1,complexTerm2);
      if (lexCmp == BK::Greater) return BK::Greater;
      ASSERT(lexCmp != BK::Equal);
      return BK::Incomparable;

    case BK::VolatileButNearlyAlwaysLess:
      // same as below
    case BK::VolatileButNearlyAlwaysGreater:
      // same as below
    case BK::AbsolutelyVolatile:
      return BK::Incomparable;
    };

  ICP("ICP0");
#if (defined _SUPPRESS_WARNINGS_) || (defined DEBUG_NAMESPACE)
  return BK::Incomparable;
#endif

}; // Comparison NonrecursiveKBOrdering::compareOnCurrentUnifier(..)


Comparison 
NonrecursiveKBOrdering::compareOnCurrentUnifier(const TERM& term1,
						const TERM& term2,
						ulong variableIndex)
{
  CALL("compareOnCurrentUnifier(const TERM& term1,const TERM& term2,ulong variableIndex)");
  _weight1.reset();
  _weight2.reset();
  TermWeighting::current()->collectWeight(_weight1,term1,variableIndex);
  TermWeighting::current()->collectWeight(_weight2,term2,variableIndex);
  Comparison lexCmp;
  switch (_weight1.compare(_weight2))
    {
    case BK::FailureToCompare:
      return BK::Incomparable;
    case BK::AlwaysLess:
      return BK::Less;
    case BK::AlwaysEqual:
      return compareLexOnCurrentUnifier(term1,term2,variableIndex);
    case BK::AlwaysGreater:
      return BK::Greater;
    case BK::CanBeLessOrEqual:
      lexCmp = compareLexOnCurrentUnifier(term1,term2,variableIndex);
      if (lexCmp == BK::Less) return BK::Less;
      ASSERT(lexCmp != BK::Equal);
      return BK::Incomparable;
      
    case BK::CanBeGreaterOrEqual:
      lexCmp = compareLexOnCurrentUnifier(term1,term2,variableIndex);
      if (lexCmp == BK::Greater) return BK::Greater;
      ASSERT(lexCmp != BK::Equal);
      return BK::Incomparable;

    case BK::VolatileButNearlyAlwaysLess:
      // same as below
    case BK::VolatileButNearlyAlwaysGreater:
      // same as below
    case BK::AbsolutelyVolatile:
      return BK::Incomparable;
    };

  ICP("ICP0");
#if (defined _SUPPRESS_WARNINGS_) || (defined DEBUG_NAMESPACE)
  return BK::Incomparable;
#endif

}; // Comparison NonrecursiveKBOrdering::compareOnCurrentUnifier(const TERM& term1,const TERM& term2,ulong variableIndex)



Comparison NonrecursiveKBOrdering::compare(const TERM* complexTerm1,
					   const TERM* complexTerm2,
					   const InstRet::Substitution* subst)
{
  CALL("compare(const TERM* complexTerm1,const TERM* complexTerm2,const InstRet::Substitution* subst)");
  _weight1.reset();
  _weight2.reset();
  
  TermWeighting::current()->collectWeight(_weight1,complexTerm1,subst);
  TermWeighting::current()->collectWeight(_weight2,complexTerm2,subst);
  Comparison lexCmp;
  switch (_weight1.compare(_weight2))
    {
    case BK::FailureToCompare:
      return BK::Incomparable;
    case BK::AlwaysLess:
      return BK::Less;
    case BK::AlwaysEqual:
      return compareLexicographically(complexTerm1,complexTerm2,subst);
    case BK::AlwaysGreater:
      return BK::Greater;
    case BK::CanBeLessOrEqual:
      lexCmp = compareLexicographically(complexTerm1,complexTerm2,subst);
      if (lexCmp == BK::Less) return BK::Less;
      ASSERT(lexCmp != BK::Equal);
      return BK::Incomparable;
      
    case BK::CanBeGreaterOrEqual:
      lexCmp = compareLexicographically(complexTerm1,complexTerm2,subst);
      if (lexCmp == BK::Greater) return BK::Greater;
      ASSERT(lexCmp != BK::Equal);
      return BK::Incomparable;

    case BK::VolatileButNearlyAlwaysLess:
      // same as below
    case BK::VolatileButNearlyAlwaysGreater:
      // same as below
    case BK::AbsolutelyVolatile:
      return BK::Incomparable;
    };

  ICP("ICP0");
#if (defined _SUPPRESS_WARNINGS_) || (defined DEBUG_NAMESPACE)
  return BK::Incomparable;
#endif
  

}; // Comparison NonrecursiveKBOrdering::compare(const TERM* complexTerm1,const TERM* complexTerm2,const InstRet::Substitution* subst)







Comparison NonrecursiveKBOrdering::compare(const TERM& term1,
					   const TERM& term2,
					   const ForwardMatchingSubstitutionCache* subst)
{
  CALL("compare(const TERM& term1,const TERM& term2,const ForwardMatchingSubstitutionCache* subst)");
  _weight1.reset();
  _weight2.reset();
  TermWeighting::current()->collectWeight(_weight1,term1,subst);
  TermWeighting::current()->collectWeight(_weight2,term2,subst);
  Comparison lexCmp;
  switch (_weight1.compare(_weight2))
    {
    case BK::FailureToCompare:
      return BK::Incomparable;
    case BK::AlwaysLess:
      return BK::Less;
    case BK::AlwaysEqual:
      return compareLexicographically(term1,term2,subst);
    case BK::AlwaysGreater:
      return BK::Greater;
    case BK::CanBeLessOrEqual:
      lexCmp = compareLexicographically(term1,term2,subst);
      if (lexCmp == BK::Less) return BK::Less;
      ASSERT(lexCmp != BK::Equal);
      return BK::Incomparable;
      
    case BK::CanBeGreaterOrEqual:
      lexCmp = compareLexicographically(term1,term2,subst);
      if (lexCmp == BK::Greater) return BK::Greater;
      ASSERT(lexCmp != BK::Equal);
      return BK::Incomparable;

    case BK::VolatileButNearlyAlwaysLess:
      // same as below
    case BK::VolatileButNearlyAlwaysGreater:
      // same as below
    case BK::AbsolutelyVolatile:
      return BK::Incomparable;
    };

  ICP("ICP0");
#if (defined _SUPPRESS_WARNINGS_) || (defined DEBUG_NAMESPACE)
  return BK::Incomparable;
#endif

}; // Comparison NonrecursiveKBOrdering::compare(const TERM& term1,const TERM& term2,const ForwardMatchingSubstitutionCache* subst)



SimplificationOrdering::SpecialisedComparison* 
NonrecursiveKBOrdering::reserveSpecialisedComparison()
{
  CALL("reserveSpecialisedComparison()");
  SpecialisedComparison* result = _specialisedComparisonPool;
  if (result)
    {
      _specialisedComparisonPool = _specialisedComparisonPool->getNextInPool();
    }
  else
    {
      result = new SpecialisedComparison();
    };
  
  return static_cast<SimplificationOrdering::SpecialisedComparison*>(result);
}; // SimplificationOrdering::SpecialisedComparison* NonrecursiveKBOrdering::reserveSpecialisedComparison()

void NonrecursiveKBOrdering::releaseSpecialisedComparison(SimplificationOrdering::SpecialisedComparison* sc)
{
  CALL("releaseSpecialisedComparison(SimplificationOrdering::SpecialisedComparison* sc)");
  (static_cast<NonrecursiveKBOrdering::SpecialisedComparison*>(sc))->setNextInPool(_specialisedComparisonPool);
  _specialisedComparisonPool = static_cast<NonrecursiveKBOrdering::SpecialisedComparison*>(sc);
  
}; //  NonrecursiveKBOrdering::releaseSpecialisedComparison(..)



SimplificationOrdering::StoredConstraint*
NonrecursiveKBOrdering::storedConstraintGreater(SimplificationOrdering::SpecialisedComparison* sc)
{
  CALL("storedConstraintGreater(SimplificationOrdering::SpecialisedComparison* sc)");
  INTERCEPT_FORWARD_CONSTR_CHECK_NEW_CONSTRAINED_EQ;
  INTERCEPT_FORWARD_CONSTR_CHECK_BEGIN2;
  // Timer overhead
  INTERCEPT_FORWARD_CONSTR_CHECK_END2;
  
  INTERCEPT_FORWARD_CONSTR_CHECK_MEMORY_COUNT_ON;
  INTERCEPT_FORWARD_CONSTR_CHECK_BEGIN1;
  StoredConstraint* result = (static_cast<SpecialisedComparison*>(sc))->storedConstraintGreater();
  result->incReferenceCounter();
  ASSERT(result->referenceCounter() >= 2L);
  INTERCEPT_FORWARD_CONSTR_CHECK_END1;
  INTERCEPT_FORWARD_CONSTR_CHECK_MEMORY_COUNT_OFF;
  return 
    static_cast<SimplificationOrdering::StoredConstraint*>(static_cast<void*>(result));
}; // SimplificationOrdering::StoredConstraint* NonrecursiveKBOrdering::storedConstraintGreater(SimplificationOrdering::SpecialisedComparison* sc)

SimplificationOrdering::StoredConstraint*
NonrecursiveKBOrdering::storedConstraintGreaterOrEqual(SimplificationOrdering::SpecialisedComparison* sc)
{
  CALL("storedConstraintGreaterOrEqual(SimplificationOrdering::SpecialisedComparison* sc)");
  INTERCEPT_FORWARD_CONSTR_CHECK_NEW_CONSTRAINED_EQ;
  INTERCEPT_FORWARD_CONSTR_CHECK_BEGIN2;
  // Timer overhead
  INTERCEPT_FORWARD_CONSTR_CHECK_END2;
  INTERCEPT_FORWARD_CONSTR_CHECK_MEMORY_COUNT_ON;
  INTERCEPT_FORWARD_CONSTR_CHECK_BEGIN1;
  StoredConstraint* result = 
    (static_cast<SpecialisedComparison*>(sc))->storedConstraintGreaterOrEqual();
  result->incReferenceCounter();
  ASSERT(result->referenceCounter() >= 2L);
  INTERCEPT_FORWARD_CONSTR_CHECK_END1;
  INTERCEPT_FORWARD_CONSTR_CHECK_MEMORY_COUNT_OFF;
  return 
    static_cast<SimplificationOrdering::StoredConstraint*>(static_cast<void*>(result));
}; // SimplificationOrdering::StoredConstraint* NonrecursiveKBOrdering::storedConstraintGreaterOrEqual(SimplificationOrdering::SpecialisedComparison* sc)

SimplificationOrdering::StoredConstraint*
NonrecursiveKBOrdering::storedConstraintLess(SimplificationOrdering::SpecialisedComparison* sc)
{
  CALL("storedConstraintLess(SimplificationOrdering::SpecialisedComparison* sc)");
  INTERCEPT_FORWARD_CONSTR_CHECK_NEW_CONSTRAINED_EQ;
  INTERCEPT_FORWARD_CONSTR_CHECK_BEGIN2;
  // Timer overhead
  INTERCEPT_FORWARD_CONSTR_CHECK_END2;
  INTERCEPT_FORWARD_CONSTR_CHECK_MEMORY_COUNT_ON;
  INTERCEPT_FORWARD_CONSTR_CHECK_BEGIN1;
  StoredConstraint* result = 
    (static_cast<SpecialisedComparison*>(sc))->storedConstraintLess();
  result->incReferenceCounter();
  ASSERT(result->referenceCounter() >= 2L);
  INTERCEPT_FORWARD_CONSTR_CHECK_END1;
  INTERCEPT_FORWARD_CONSTR_CHECK_MEMORY_COUNT_OFF;
  return 
    static_cast<SimplificationOrdering::StoredConstraint*>(static_cast<void*>(result));
}; // SimplificationOrdering::StoredConstraint* NonrecursiveKBOrdering::storedConstraintLess(SimplificationOrdering::SpecialisedComparison* sc)

SimplificationOrdering::StoredConstraint*
NonrecursiveKBOrdering::storedConstraintLessOrEqual(SimplificationOrdering::SpecialisedComparison* sc)
{
  CALL("storedConstraintLessOrEqual(SimplificationOrdering::SpecialisedComparison* sc)");
  INTERCEPT_FORWARD_CONSTR_CHECK_NEW_CONSTRAINED_EQ;
  INTERCEPT_FORWARD_CONSTR_CHECK_BEGIN2;
  // Timer overhead
  INTERCEPT_FORWARD_CONSTR_CHECK_END2;
  INTERCEPT_FORWARD_CONSTR_CHECK_MEMORY_COUNT_ON;
  INTERCEPT_FORWARD_CONSTR_CHECK_BEGIN1;
  StoredConstraint* result = 
    (static_cast<SpecialisedComparison*>(sc))->storedConstraintLessOrEqual();
  result->incReferenceCounter();
  ASSERT(result->referenceCounter() >= 2L);
  INTERCEPT_FORWARD_CONSTR_CHECK_END1;
  INTERCEPT_FORWARD_CONSTR_CHECK_MEMORY_COUNT_OFF;
  return 
    static_cast<SimplificationOrdering::StoredConstraint*>(static_cast<void*>(result));
}; // SimplificationOrdering::StoredConstraint* NonrecursiveKBOrdering::storedConstraintLessOrEqual(SimplificationOrdering::SpecialisedComparison* sc)

SimplificationOrdering::StoredConstraint* NonrecursiveKBOrdering::copy(SimplificationOrdering::StoredConstraint* constr)
{
  CALL("copy(SimplificationOrdering::StoredConstraint* constr)");
  INTERCEPT_FORWARD_CONSTR_CHECK_NEW_CONSTRAINED_EQ;
  INTERCEPT_FORWARD_CONSTR_CHECK_BEGIN2;
  // Timer overhead
  INTERCEPT_FORWARD_CONSTR_CHECK_END2;
  INTERCEPT_FORWARD_CONSTR_CHECK_MEMORY_COUNT_ON;
  INTERCEPT_FORWARD_CONSTR_CHECK_BEGIN1;
  ASSERT(constr);
  (static_cast<StoredConstraint*>(static_cast<void*>(constr)))->incReferenceCounter();
  INTERCEPT_FORWARD_CONSTR_CHECK_END1;
  INTERCEPT_FORWARD_CONSTR_CHECK_MEMORY_COUNT_OFF;
  return constr;
}; // SimplificationOrdering::StoredConstraint* NonrecursiveKBOrdering::copy(SimplificationOrdering::StoredConstraint* constr)


void NonrecursiveKBOrdering::releaseConstraint(SimplificationOrdering::StoredConstraint* constr)
{
  CALL("releaseConstraint(SimplificationOrdering::StoredConstraint* constr)");
  INTERCEPT_FORWARD_CONSTR_CHECK_REMOVE_CONSTRAINED_EQ;
  INTERCEPT_FORWARD_CONSTR_CHECK_BEGIN2;
  // Timer overhead
  INTERCEPT_FORWARD_CONSTR_CHECK_END2;
  INTERCEPT_FORWARD_CONSTR_CHECK_MEMORY_COUNT_ON;
  INTERCEPT_FORWARD_CONSTR_CHECK_BEGIN1;
  ASSERT(constr);
  (static_cast<StoredConstraint*>(static_cast<void*>(constr)))->decReferenceCounter();
  if (!(static_cast<StoredConstraint*>(static_cast<void*>(constr)))->referenceCounter())
    delete static_cast<StoredConstraint*>(static_cast<void*>(constr));
  INTERCEPT_FORWARD_CONSTR_CHECK_END1;
  INTERCEPT_FORWARD_CONSTR_CHECK_MEMORY_COUNT_OFF;
}; // void NonrecursiveKBOrdering::releaseConstraint(SimplificationOrdering::StoredConstraint* constr)



SimplificationOrdering::ConstraintCheckOnUnifier* NonrecursiveKBOrdering::reserveConstraintCheckOnUnifier()
{
  CALL("reserveConstraintCheckOnUnifier()");  
  ConstraintCheckOnUnifier* result = _constraintCheckOnUnifierPool;
  if (result)
    {
      _constraintCheckOnUnifierPool = _constraintCheckOnUnifierPool->getNextInPool();
    }
  else
    {
      result = new ConstraintCheckOnUnifier();
    };
  return 
    static_cast<SimplificationOrdering::ConstraintCheckOnUnifier*>(result);
}; // SimplificationOrdering::ConstraintCheckOnUnifier* NonrecursiveKBOrdering::reserveConstraintCheckOnUnifier()

void NonrecursiveKBOrdering::releaseConstraintCheckOnUnifier(SimplificationOrdering::ConstraintCheckOnUnifier* ccu)
{
  CALL("releaseConstraintCheckOnUnifier(SimplificationOrdering::ConstraintCheckOnUnifier* ccu)");

  (static_cast<NonrecursiveKBOrdering::ConstraintCheckOnUnifier*>(ccu))->setNextInPool(_constraintCheckOnUnifierPool);
  _constraintCheckOnUnifierPool = 
    static_cast<NonrecursiveKBOrdering::ConstraintCheckOnUnifier*>(ccu);
}; // void NonrecursiveKBOrdering::releaseConstraintCheckOnUnifier(SimplificationOrdering::ConstraintCheckOnUnifier* ccu)



SimplificationOrdering::ConstraintCheckOnForwardMatchingSubst* 
NonrecursiveKBOrdering::reserveConstraintCheckOnForwardMatchingSubst()
{
  CALL("reserveConstraintCheckOnForwardMatchingSubst()");
  ConstraintCheckOnForwardMatchingSubst* result = _constraintCheckOnForwardMatchingSubstPool;
  if (result)
    {
      _constraintCheckOnForwardMatchingSubstPool = 
	_constraintCheckOnForwardMatchingSubstPool->getNextInPool();
    }
  else
    {
      result = new ConstraintCheckOnForwardMatchingSubst();
    };
  return 
    static_cast<SimplificationOrdering::ConstraintCheckOnForwardMatchingSubst*>(result);
}; // SimplificationOrdering::ConstraintCheckOnForwardMatchingSubst* NonrecursiveKBOrdering::reserveConstraintCheckOnForwardMatchingSubst()

void 
NonrecursiveKBOrdering::releaseConstraintCheckOnForwardMatchingSubst(SimplificationOrdering::ConstraintCheckOnForwardMatchingSubst* ccfms)
{
  CALL("releaseConstraintCheckOnForwardMatchingSubst(..)");
  (static_cast<NonrecursiveKBOrdering::ConstraintCheckOnForwardMatchingSubst*>(ccfms))->
    setNextInPool(_constraintCheckOnForwardMatchingSubstPool);
  _constraintCheckOnForwardMatchingSubstPool = 
    static_cast<NonrecursiveKBOrdering::ConstraintCheckOnForwardMatchingSubst*>(ccfms);
}; // NonrecursiveKBOrdering::releaseConstraintCheckOnForwardMatchingSubst(SimplificationOrdering::ConstraintCheckOnForwardMatchingSubst* ccfms)


SimplificationOrdering::CompiledConstraintForUnifiers* 
NonrecursiveKBOrdering::reserveCompiledConstraintForUnifiers()
{
  CALL("reserveCompiledConstraintForUnifiers()");
  NonrecursiveKBOrdering::CompiledConstraintForUnifiers* result = _compiledConstraintOnUnifiersPool;
  if (result)
    {
      _compiledConstraintOnUnifiersPool = result->getNextInPool();
    }
  else
    {
      result = new CompiledConstraintForUnifiers();
    };
  ASSERT(result);
  return 
    static_cast<SimplificationOrdering::CompiledConstraintForUnifiers*>(result);
}; // SimplificationOrdering::CompiledConstraintForUnifiers* NonrecursiveKBOrdering::reserveCompiledConstraintForUnifiers()

void 
NonrecursiveKBOrdering::releaseCompiledConstraintForUnifiers(SimplificationOrdering::CompiledConstraintForUnifiers* cc)
{
  CALL("releaseCompiledConstraintForUnifiers(SimplificationOrdering::CompiledConstraintForUnifiers* cc)");
  
  (static_cast<NonrecursiveKBOrdering::CompiledConstraintForUnifiers*>(cc))->setNextInPool(_compiledConstraintOnUnifiersPool);
  _compiledConstraintOnUnifiersPool = static_cast<NonrecursiveKBOrdering::CompiledConstraintForUnifiers*>(cc);
}; // void NonrecursiveKBOrdering::releaseCompiledConstraintForUnifiers(SimplificationOrdering::CompiledConstraintForUnifiers* cc)



SimplificationOrdering::CompiledConstraintForInstanceRetrieval* 
NonrecursiveKBOrdering::reserveCompiledConstraintForInstanceRetrieval()
{
  CALL("reserveCompiledConstraintForInstanceRetrieval()");
  NonrecursiveKBOrdering::CompiledConstraintForInstanceRetrieval* result = _compiledConstraintForInstanceRetrievalPool;
  if (result)
    {
      _compiledConstraintForInstanceRetrievalPool = result->getNextInPool();
    }
  else
    {
      result = new CompiledConstraintForInstanceRetrieval();
    };
  ASSERT(result);
  return 
    static_cast<SimplificationOrdering::CompiledConstraintForInstanceRetrieval*>(result);
}; // SimplificationOrdering::CompiledConstraintForInstanceRetrieval* NonrecursiveKBOrdering::reserveCompiledConstraintForInstanceRetrieval()

void 
NonrecursiveKBOrdering::releaseCompiledConstraintForInstanceRetrieval(SimplificationOrdering::CompiledConstraintForInstanceRetrieval* cc)
{
  CALL("releaseCompiledConstraintForInstanceRetrieval(SimplificationOrdering::CompiledConstraintForInstanceRetrieval* cc)");

  (static_cast<NonrecursiveKBOrdering::CompiledConstraintForInstanceRetrieval*>(cc))->setNextInPool(_compiledConstraintForInstanceRetrievalPool);
  _compiledConstraintForInstanceRetrievalPool = 
    static_cast<NonrecursiveKBOrdering::CompiledConstraintForInstanceRetrieval*>(cc);
}; // void NonrecursiveKBOrdering::releaseCompiledConstraintForInstanceRetrieval(SimplificationOrdering::CompiledConstraintForInstanceRetrieval* cc)




Comparison NonrecursiveKBOrdering::compareWithTransposition(const Flatterm* term)
{  CALL("compareWithTransposition(const Flatterm* term)");
 ASSERT(term->isComplex());
 ASSERT(term->arity() == 2);
 const Flatterm* arg1 = term->next();
 return compareLexicographically(arg1,arg1->after());
}; // Comparison NonrecursiveKBOrdering::compareWithTransposition(const Flatterm* term)



inline
Comparison 
NonrecursiveKBOrdering::compareLexicographically(const Flatterm* term1,
						 const Flatterm* term2)
{
  CALL("compareLexicographically(const Flatterm* term1,..)");
  Flatterm::ConstIterator iter1(term1);
  Flatterm::ConstIterator iter2(term2);
  do 
    {
      if (iter1.CurrPos()->symbolRef() != iter2.CurrPos()->symbolRef())
	{
	  if (iter1.CurrPos()->symbolRef().isVariable() || iter2.CurrPos()->symbolRef().isVariable())
	    return Incomparable;
	  return iter1.CurrPos()->symbolRef().comparePrecedence(iter2.CurrPos()->symbolRef());
	};
      iter1.NextSym();
      iter2.NextSym();
    }
  while (iter1.NotEndOfTerm());
  ASSERT(iter2.EndOfTerm());
  return Equal;
}; // Comparison NonrecursiveKBOrdering::compareLexicographically(..)

inline
Comparison 
NonrecursiveKBOrdering::compareAtomsLexicographically(const Flatterm* lit1,
						      const Flatterm* lit2)
{
  CALL("compareAtomsLexicographically(..)");
  if (lit1->functor() != lit2->functor()) 
    return lit1->symbolRef().comparePrecedence(lit2->symbolRef());
  Flatterm::ConstIterator iter1(lit1);
  Flatterm::ConstIterator iter2(lit2);
  iter1.NextSym();
  iter2.NextSym();
  while (iter1.NotEndOfTerm())
    {
      if (iter1.CurrPos()->symbolRef() != iter2.CurrPos()->symbolRef())
	{
	  if (iter1.CurrPos()->symbolRef().isVariable() || iter2.CurrPos()->symbolRef().isVariable())
	    return Incomparable;
	  return iter1.CurrPos()->symbolRef().comparePrecedence(iter2.CurrPos()->symbolRef());
	};
      iter1.NextSym();
      iter2.NextSym();
    };
  ASSERT(iter2.EndOfTerm());
  return Equal;
}; // Comparison NonrecursiveKBOrdering::compareAtomsLexicographically(..)


inline
Comparison NonrecursiveKBOrdering::compareLexicographically(const TERM& term1,
							    const TERM& term2)
{
  CALL("compareLexicographically(const TERM& term1,..)");
  if (term1 == term2) return BK::Equal;
  if (term1.isVariable() || term2.isVariable()) return BK::Incomparable;
  ASSERT(term1.IsReference() && term2.IsReference());
  TERM::Iterator iter1(term1.First());
  TERM::Iterator iter2(term2.First());
  do
    {
      if (iter1.symbolRef() != iter2.symbolRef())
	{
	  if (iter1.symbolRef().isVariable() || iter2.symbolRef().isVariable())
	    return BK::Incomparable;
	  return iter1.symbolRef().comparePrecedence(iter2.symbolRef());
	};
      iter2.Next();
    }
  while (iter1.Next());
  return BK::Equal;
}; // Comparison NonrecursiveKBOrdering::compareLexicographically(const TERM& term1,..)


inline
Comparison 
NonrecursiveKBOrdering::compareLexOnCurrentUnifier(PrefixSym* complexTerm1,
						   PrefixSym* complexTerm2)
{
  CALL("compareLexOnCurrentUnifier(..)");
  static Prefix::Iter iter1;
  static Prefix::Iter iter2;
  iter1.Reset(complexTerm1);
  iter2.Reset(complexTerm2);
  
  do
    {
      ASSERT(iter2.NotEmpty());
      Unifier::Variable* lhsVar = iter1.CurrVar();
      Unifier::Variable* rhsVar = iter2.CurrVar();
      if (lhsVar)
	{
	  if (rhsVar)
	    {
	      if (lhsVar != rhsVar)
		{
		  return BK::Incomparable;
		};
	      iter1.SkipSym();
	      iter2.SkipSym();

	    }
	  else // lhsVar, but !rhsVar
	    {
	      return BK::Incomparable;
	    };
	}
      else
	if (rhsVar)
	  {
	    // !lhsVar, but rhsVar
	    return BK::Incomparable;
	  }
	else // !lhsVar and !rhsVar
	  {	    
	    ASSERT(iter1.CurrSubterm()->Head().IsFunctor());
	    ASSERT(iter2.CurrSubterm()->Head().IsFunctor());
	    if (iter1.CurrSubterm()->Head() != iter2.CurrSubterm()->Head())
	      {	       
		// stop here completely
		if (iter1.CurrSubterm()->Head().Func().hasGreaterPrecedenceThan(iter2.CurrSubterm()->Head().Func()))
		  {
		    return BK::Greater;
		  }
		else
		  {
		    return BK::Less;
		  };
	      }
	    else
	      {
		iter1.SkipSym();
		iter2.SkipSym();
	      };
	  };

    }
  while (iter1.NotEmpty());
  return BK::Equal;
}; // Comparison NonrecursiveKBOrdering::compareLexOnCurrentUnifier(..)


inline
Comparison 
NonrecursiveKBOrdering::compareLexOnCurrentUnifier(const TERM& term1,
						   const TERM& term2,
						   ulong variableIndex)
{
  CALL("compareLexOnCurrentUnifier(const TERM& term1,const TERM& term2,ulong variableIndex)");

  static StdTermWithSubst iter1;
  static StdTermWithSubst iter2;

  iter1.Reset(variableIndex,(TERM&)term1);
  iter2.Reset(variableIndex,(TERM&)term2);
  do
    {
      if (iter1.CurrSym() != iter2.CurrSym())
	{
	  if (iter1.CurrSym().isVariable() || iter2.CurrSym().isVariable())
	    return BK::Incomparable; // terms are incomparable
	  return iter1.CurrSym().Func().comparePrecedence(iter2.CurrSym().Func());
	};
	  
      iter1.SkipSym();
      iter2.SkipSym();
    }
  while (!iter1.Empty());
  ASSERT(iter2.Empty());
  return BK::Equal;
}; // Comparison NonrecursiveKBOrdering::compareLexOnCurrentUnifier(const TERM& term1,const TERM& term2,ulong variableIndex)


inline 
Comparison 
NonrecursiveKBOrdering::compareLexicographically(const TERM* complexTerm1,
						 const TERM* complexTerm2,
						 const InstRet::Substitution* subst)
{
  CALL("compareLexicographically(const TERM* complexTerm1,const TERM* complexTerm2,const InstRet::Substitution* subst)");
  static InstRet::Substitution::Instance inst1;
  static InstRet::Substitution::Instance inst2;
  inst1.reset(complexTerm1,subst);
  inst2.reset(complexTerm2,subst);
  do
    {
      if (inst1.symbol() != inst2.symbol())
	{
	  if (inst1.symbol().isVariable() || inst2.symbol().isVariable())
	    return BK::Incomparable;
	  return inst1.symbol().comparePrecedence(inst2.symbol());
	};
      inst2.next();
    }
  while (inst1.next());
  return BK::Equal; 


  
}; // Comparison NonrecursiveKBOrdering::compareLexicographically(const TERM* complexTerm1,const TERM* complexTerm2,const InstRet::Substitution* subst)








inline
Comparison 
NonrecursiveKBOrdering::compareLexicographically(const TERM& term1,
						 const TERM& term2,
						 const ForwardMatchingSubstitutionCache* subst)
{
  CALL("compareLexicographically(const TERM& term1,const TERM& term2,const ForwardMatchingSubstitutionCache* subst)");
  static ForwardMatchingSubstitutionCache::Instance inst1;
  static ForwardMatchingSubstitutionCache::Instance inst2;
  inst1.reset(term1,subst);
  inst2.reset(term2,subst);
  do
    {
      if (inst1.symbol() != inst2.symbol())
	{
	  if (inst1.symbol().isVariable() || inst2.symbol().isVariable())
	    return BK::Incomparable;
	  return inst1.symbol().comparePrecedence(inst2.symbol());
	};
      inst2.next();
    }
  while (inst1.next());
  return BK::Equal;
}; // Comparison NonrecursiveKBOrdering::compareLexicographically(const TERM& term1,const TERM& term2,const ForwardMatchingSubstitutionCache* subst)



//===================================================================
#undef DEBUG_NAMESPACE
#ifdef DEBUG_NONRECURSIVE_KB_ORDERING
#define DEBUG_NAMESPACE "NonrecursiveKBOrdering::SpecialisedComparison"
#endif
#include "debugMacros.hpp"
//===================================================================

NonrecursiveKBOrdering::SpecialisedComparison::~SpecialisedComparison()
{ 
  CALL("destructor ~SpecialisedComparison()");
  
  if (DestructionMode::isThorough())
    {  
      if (_constraintGreater)
	{
	  _constraintGreater->decReferenceCounter();
	  if (!_constraintGreater->referenceCounter())
	    delete _constraintGreater;
	};
      if (_constraintGreaterOrEqual)
	{
	  _constraintGreaterOrEqual->decReferenceCounter();
	  if (!_constraintGreaterOrEqual->referenceCounter())
	    delete _constraintGreaterOrEqual;
	};

      if (_constraintLess)
	{
	  _constraintLess->decReferenceCounter();
	  if (!_constraintLess->referenceCounter())
	    delete _constraintLess;
	};  
      if (_constraintLessOrEqual)
	{
	  _constraintLessOrEqual->decReferenceCounter();
	  if (!_constraintLessOrEqual->referenceCounter())
	    delete _constraintLessOrEqual;
	};
    };
}; // NonrecursiveKBOrdering::SpecialisedComparison::~SpecialisedComparison()

void NonrecursiveKBOrdering::SpecialisedComparison::load(const TERM& lhs,const TERM& rhs)
{
  CALL("load(const TERM& lhs,const TERM& rhs)");
  _lhs = lhs;
  _rhs = rhs;
  _lhsWeight.reset();
  _rhsWeight.reset();
  TermWeighting::current()->collectWeight(_lhsWeight,_lhs);
  TermWeighting::current()->collectWeight(_rhsWeight,_rhs);
  _specialised = false;

  if (_constraintGreater)
    {
      _constraintGreater->decReferenceCounter();
      if (!_constraintGreater->referenceCounter())
	delete _constraintGreater;
      _constraintGreater = 0;
    };
  
  if (_constraintGreaterOrEqual)
    {
      _constraintGreaterOrEqual->decReferenceCounter();
      if (!_constraintGreaterOrEqual->referenceCounter())
	delete _constraintGreaterOrEqual;
      _constraintGreaterOrEqual = 0;
    };

  if (_constraintLess)
    {
      _constraintLess->decReferenceCounter();
      if (!_constraintLess->referenceCounter())
	delete _constraintLess;
      _constraintLess = 0;
    };
  
  if (_constraintLessOrEqual)
    {
      _constraintLessOrEqual->decReferenceCounter();
      if (!_constraintLessOrEqual->referenceCounter())
	delete _constraintLessOrEqual;
      _constraintLessOrEqual = 0;
    };

  //DF; cout << "LOADED: " << *this << "\n";


}; // void NonrecursiveKBOrdering::SpecialisedComparison::load(const TERM& lhs,const TERM& rhs)

bool NonrecursiveKBOrdering::SpecialisedComparison::lhsVarInstMayOrder()
{
  CALL("lhsVarInstMayOrder()");
  return !_rhsWeight.containsVariableWhichIsNotIn(_lhsWeight);
}; // bool NonrecursiveKBOrdering::SpecialisedComparison::lhsVarInstMayOrder()

bool NonrecursiveKBOrdering::SpecialisedComparison::rhsVarInstMayOrder()
{
  CALL("rhsVarInstMayOrder()");
  return !_lhsWeight.containsVariableWhichIsNotIn(_rhsWeight);
}; // bool NonrecursiveKBOrdering::SpecialisedComparison::rhsVarInstMayOrder()


void NonrecursiveKBOrdering::SpecialisedComparison::specialiseGreater()
{
  specialise();
};

void NonrecursiveKBOrdering::SpecialisedComparison::specialiseGreaterOrEqual()
{
  specialise();
};

void NonrecursiveKBOrdering::SpecialisedComparison::specialiseLess()
{
  specialise();
};

void NonrecursiveKBOrdering::SpecialisedComparison::specialiseLessOrEqual()
{
  specialise();
};




inline
NonrecursiveKBOrdering::StoredConstraint* NonrecursiveKBOrdering::SpecialisedComparison::storedConstraintGreater()
{
  CALL("storedConstraintGreater()");
  if (!_constraintGreater) 
    {
      specialise();
      _constraintGreater = 
	new StoredConstraint(StoredConstraint::Greater,
			     _weightComparison,
			     _lexicographicComparisonResult,
			     _lhs,
			     _rhs);
      _constraintGreater->incReferenceCounter();      
    };

  return _constraintGreater;
}; // NonrecursiveKBOrdering::StoredConstraint* NonrecursiveKBOrdering::SpecialisedComparison::storedConstraintGreater() 


inline
NonrecursiveKBOrdering::StoredConstraint* NonrecursiveKBOrdering::SpecialisedComparison::storedConstraintGreaterOrEqual()
{
  CALL("storedConstraintGreaterOrEqual()");
  if (!_constraintGreaterOrEqual) 
    {
      specialise();
      _constraintGreaterOrEqual = 
	new StoredConstraint(StoredConstraint::GreaterOrEqual,
			     _weightComparison,
			     _lexicographicComparisonResult,
			     _lhs,
			     _rhs);
      _constraintGreaterOrEqual->incReferenceCounter();      
    };

  //DF; cout << "SPECIALISED (GREATER OR EQUAL): " << *this << "\n";
  //DF; cout << "STORED CONSTR (GREATER OR EQUAL): " << _constraintGreaterOrEqual << "\n";


  return _constraintGreaterOrEqual;
}; // NonrecursiveKBOrdering::StoredConstraint* NonrecursiveKBOrdering::SpecialisedComparison::storedConstraintGreaterOrEqual()


inline
NonrecursiveKBOrdering::StoredConstraint* 
NonrecursiveKBOrdering::SpecialisedComparison::storedConstraintLess()
{
  CALL("storedConstraintLess()");
  if (!_constraintLess) 
    {
      specialise();
      _constraintLess = 
	new StoredConstraint(StoredConstraint::Less,
			     _weightComparison,
			     _lexicographicComparisonResult,
			     _lhs,
			     _rhs);
      _constraintLess->incReferenceCounter();      
    };
  return _constraintLess;
}; // NonrecursiveKBOrdering::StoredConstraint* NonrecursiveKBOrdering::SpecialisedComparison::storedConstraintLess()

inline
NonrecursiveKBOrdering::StoredConstraint* 
NonrecursiveKBOrdering::SpecialisedComparison::storedConstraintLessOrEqual()
{
  CALL("storedConstraintLessOrEqual()");  
  if (!_constraintLessOrEqual) 
    {
      specialise();
      _constraintLessOrEqual = 
	new StoredConstraint(StoredConstraint::LessOrEqual,
			     _weightComparison,
			     _lexicographicComparisonResult,
			     _lhs,
			     _rhs);
      _constraintLessOrEqual->incReferenceCounter();      
    };

  //DF; cout << "SPECIALISED (LESS OR EQUAL): " << *this << "\n";
  //DF; cout << "STORED CONSTR (LESS OR EQUAL): " << _constraintLessOrEqual << "\n";

  return _constraintLessOrEqual;
}; // NonrecursiveKBOrdering::StoredConstraint* NonrecursiveKBOrdering::SpecialisedComparison::storedConstraintLessOrEqual()


#ifndef NO_DEBUG_VIS
ostream& NonrecursiveKBOrdering::SpecialisedComparison::output(ostream& str) const
{
  str << _lhs << " VS " << _rhs << "\n";
  if (_specialised)
    {
      str << "CATEGORY: " << category() << "\n";
      str << " WEIGHT: " << _weightComparison << "\n"
	  << " LEX: ";
      if (BK::strict(_weightComparison.category()))
	{
	  str << "IRRELEVANT";
	}
      else
	{
	  str << _lexicographicComparisonResult;
	}
      
    }
  else
    {
      str << " NOT SPECIALISED\n";
      str << "LHS WEIGHT: " << _lhsWeight << "\n";
      str << "RHS WEIGHT: " << _rhsWeight;
    };
  return str;
};
#endif



inline
void NonrecursiveKBOrdering::SpecialisedComparison::specialise()
{
  CALL("specialise()");
  if (_specialised) return;
  
  INTERCEPT_FORWARD_CONSTR_CHECK_SPECIALISATION_CALL;

  _weightComparison.compare(_lhsWeight,_rhsWeight);
  switch (_weightComparison.category())
    {
    case BK::FailureToCompare:
      ICP("ICP10");
      break;
    case BK::AlwaysLess: 
      _category = SimplificationOrdering::SpecialisedComparison::AlwaysLess;
      break;

    case BK::AlwaysEqual:
      // purely lexicographic
      _lexicographicComparisonResult = 
	NonrecursiveKBOrdering::compareLexicographically(_lhs,_rhs);
      switch (_lexicographicComparisonResult)
	{
	case BK::Less:
	  _category = SimplificationOrdering::SpecialisedComparison::AlwaysLess;
	  break;
	case BK::Equal:
	  _category = SimplificationOrdering::SpecialisedComparison::AlwaysEqual;
	  break;
	case BK::Greater:
	  _category = SimplificationOrdering::SpecialisedComparison::AlwaysGreater;
	  break;
	case BK::Incomparable:
	  _category = SimplificationOrdering::SpecialisedComparison::AbsolutelyVolatile;
	  break;
	};
      break;

    case BK::AlwaysGreater:
      _category = SimplificationOrdering::SpecialisedComparison::AlwaysGreater;
      break;

    case BK::CanBeLessOrEqual:
      _lexicographicComparisonResult = 
	NonrecursiveKBOrdering::compareLexicographically(_lhs,_rhs);
      switch (_lexicographicComparisonResult)
	{
	case BK::Less:
	  _category = SimplificationOrdering::SpecialisedComparison::AlwaysLess;
	  break;
	case BK::Equal:
	  ICP("ICP0");
	  break;
	case BK::Greater:
	  _category = 
	    SimplificationOrdering::SpecialisedComparison::VolatileButNearlyAlwaysLess;
	  break;
	case BK::Incomparable:
	  _category = 
	    SimplificationOrdering::SpecialisedComparison::VolatileButNearlyAlwaysLess;
	  break;
	};
      break;

    case BK::CanBeGreaterOrEqual:
      _lexicographicComparisonResult = 
	NonrecursiveKBOrdering::compareLexicographically(_lhs,_rhs);
      
      switch (_lexicographicComparisonResult)
	{
	case BK::Less:
	  _category = 
	    SimplificationOrdering::SpecialisedComparison::VolatileButNearlyAlwaysGreater;
	  break;
	case BK::Equal:
	  ICP("ICP1");
	  break;
	case BK::Greater:
	  _category = SimplificationOrdering::SpecialisedComparison::AlwaysGreater;
	  break;
	case BK::Incomparable:
	  _category = 
	    SimplificationOrdering::SpecialisedComparison::VolatileButNearlyAlwaysGreater;
	  break;
	};
      break;

    case BK::VolatileButNearlyAlwaysLess:
      _lexicographicComparisonResult = 
	NonrecursiveKBOrdering::compareLexicographically(_lhs,_rhs);
      _category = 
	SimplificationOrdering::SpecialisedComparison::VolatileButNearlyAlwaysLess;
      break;

    case BK::VolatileButNearlyAlwaysGreater:
      _lexicographicComparisonResult = 
	NonrecursiveKBOrdering::compareLexicographically(_lhs,_rhs);
      _category = 
	SimplificationOrdering::SpecialisedComparison::VolatileButNearlyAlwaysGreater;
      break;

    case BK::AbsolutelyVolatile:
      _lexicographicComparisonResult = 
	NonrecursiveKBOrdering::compareLexicographically(_lhs,_rhs);
      _category = 
	SimplificationOrdering::SpecialisedComparison::AbsolutelyVolatile;
      break;         
    };      
  _specialised = true;
}; // void NonrecursiveKBOrdering::SpecialisedComparison::specialise()






//===================================================================
#undef DEBUG_NAMESPACE
#ifdef DEBUG_SIMPLIFICATION_ORDERING
#define DEBUG_NAMESPACE "NonrecursiveKBOrdering::ConstraintCheckOnUnifier"
#endif
#include "debugMacros.hpp"
//===================================================================

NonrecursiveKBOrdering::ConstraintCheckOnUnifier::~ConstraintCheckOnUnifier()
{
};

bool NonrecursiveKBOrdering::ConstraintCheckOnUnifier::holds(const SimplificationOrdering::StoredConstraint* constr,
							     ulong variableIndex)
{
  CALL("holds(..)");

  //DF; cout << "HOLDS? " 
  //	   << (static_cast<const StoredConstraint*>(static_cast<const void*>(constr))) << "\n";
  
  ASSERT(constr);
  const StoredConstraint* nativeConstr = 
    static_cast<const StoredConstraint*>(static_cast<const void*>(constr));

#ifdef MEASURE_FORWARD_CONSTR_CHECK
  INTERCEPT_FORWARD_CONSTR_CHECK_BEGIN2;
  // Straightforward check:
  SimplificationOrdering::current()->compareOnCurrentUnifier(nativeConstr->lhs(),\
							     nativeConstr->rhs(),\
							     variableIndex);
  INTERCEPT_FORWARD_CONSTR_CHECK_END2;
#endif

  INTERCEPT_FORWARD_CONSTR_CHECK_NEW_CHECK;
  INTERCEPT_FORWARD_CONSTR_CHECK_BEGIN1;

  _currentVariableIndex = variableIndex;
  bool result;
#if (defined DEBUG_NAMESPACE) || (defined _SUPPRESS_WARNINGS_)
  result = false;
#endif
  DOP(Comparison debugCmp = \
      SimplificationOrdering::current()->compareOnCurrentUnifier(nativeConstr->lhs(),\
								 nativeConstr->rhs(),\
								 variableIndex));
  switch (nativeConstr->targetRelation())
    {
    case StoredConstraint::Greater:
      result = holdsGreater(nativeConstr);
      ASSERT(result == (debugCmp == BK::Greater));
      break;
    case StoredConstraint::GreaterOrEqual:
      result = holdsGreaterOrEqual(nativeConstr);
      ASSERT(result == ((debugCmp == BK::Greater) || (debugCmp == BK::Equal)));
      break;
    case StoredConstraint::Less:
      result = holdsLess(nativeConstr);
      ASSERT(result == (debugCmp == BK::Less));
      break;
    case StoredConstraint::LessOrEqual:
      result = holdsLessOrEqual(nativeConstr);
      ASSERT(result == ((debugCmp == BK::Less) || (debugCmp == BK::Equal)));
      break;
    };

  INTERCEPT_FORWARD_CONSTR_CHECK_END1;
  return result;
}; // bool NonrecursiveKBOrdering::ConstraintCheckOnUnifier::holds(..)



inline
bool NonrecursiveKBOrdering::ConstraintCheckOnUnifier::holdsGreater(const NonrecursiveKBOrdering::StoredConstraint* constr)
{
  CALL("holdsGreater(..)");
  switch (checkGreaterOrEqual(constr->weightComparison()))
    {
    case BK::FailureToCompare:
      INTERCEPT_FORWARD_CONSTR_CHECK_CONCLUSIVE_SHALLOW_CHECK;
      return false;

    case BK::AlwaysLess:
      ICP("ICP10");
      return false; 

    case BK::AlwaysEqual:
      // check lexicographically
      switch (constr->lexicographicComparisonResult())
	{
	case BK::Less: 
	  INTERCEPT_FORWARD_CONSTR_CHECK_CONCLUSIVE_SHALLOW_CHECK;
	  return false;
	case BK::Equal:
	  INTERCEPT_FORWARD_CONSTR_CHECK_CONCLUSIVE_SHALLOW_CHECK; 
	  return false;
	case BK::Greater:
	  INTERCEPT_FORWARD_CONSTR_CHECK_CONCLUSIVE_SHALLOW_CHECK; 
	  return true;
	case BK::Incomparable: return lexicorgaphicallyGreater(constr->lhs(),constr->rhs());
	};	      	      
      ICP("ICP20");
      RuntimeError::report("Illegal control point in NonrecursiveKBOrdering::ConstraintCheckOnUnifier::...");

    case BK::AlwaysGreater:
      INTERCEPT_FORWARD_CONSTR_CHECK_CONCLUSIVE_SHALLOW_CHECK;
      return true;

    case BK::CanBeLessOrEqual:
      ICP("ICP30");
      return false;

    case BK::CanBeGreaterOrEqual:
      // check lexicographically
      switch (constr->lexicographicComparisonResult())
	{
	case BK::Less: 
	  INTERCEPT_FORWARD_CONSTR_CHECK_CONCLUSIVE_SHALLOW_CHECK;
	  return false;
	case BK::Equal: ICP("ICP1"); 
	case BK::Greater: 
	  INTERCEPT_FORWARD_CONSTR_CHECK_CONCLUSIVE_SHALLOW_CHECK;
	  return true;
	case BK::Incomparable: return lexicorgaphicallyGreater(constr->lhs(),constr->rhs());
	};	      	      
      ICP("ICP40");
      RuntimeError::report("Illegal control point in NonrecursiveKBOrdering::ConstraintCheckOnUnifier::...");

    case BK::VolatileButNearlyAlwaysLess:
      // same as below
    case BK::VolatileButNearlyAlwaysGreater:
      // same as below
    case BK::AbsolutelyVolatile:
      ICP("ICP70");
      return false; 
    };
  ICP("ICP80");
#if (defined _SUPPRESS_WARNINGS_) || (defined DEBUG_NAMESPACE)
  return false;
#endif
}; // bool NonrecursiveKBOrdering::ConstraintCheckOnUnifier::holdsGreater(const NonrecursiveKBOrdering::StoredConstraint* constr)


inline
bool NonrecursiveKBOrdering::ConstraintCheckOnUnifier::holdsGreaterOrEqual(const NonrecursiveKBOrdering::StoredConstraint* constr)
{
  CALL("holdsGreaterOrEqual(..)");

  switch (checkGreaterOrEqual(constr->weightComparison()))
    {
    case BK::FailureToCompare:
      INTERCEPT_FORWARD_CONSTR_CHECK_CONCLUSIVE_SHALLOW_CHECK;
      return false;

    case BK::AlwaysLess:
      ICP("ICP10");
      return false; 

    case BK::AlwaysEqual:
      // check lexicographically
      switch (constr->lexicographicComparisonResult())
	{
	case BK::Less: 
	  INTERCEPT_FORWARD_CONSTR_CHECK_CONCLUSIVE_SHALLOW_CHECK;
	  return false;
	case BK::Equal: 
	  INTERCEPT_FORWARD_CONSTR_CHECK_CONCLUSIVE_SHALLOW_CHECK;
	  return true;
	case BK::Greater:
	  INTERCEPT_FORWARD_CONSTR_CHECK_CONCLUSIVE_SHALLOW_CHECK; 
	  return true;
	case BK::Incomparable: return lexicorgaphicallyGreaterOrEqual(constr->lhs(),constr->rhs());
	};	      	      
      ICP("ICP20");
      RuntimeError::report("Illegal control point in NonrecursiveKBOrdering::ConstraintCheckOnUnifier::...");

    case BK::AlwaysGreater:
      INTERCEPT_FORWARD_CONSTR_CHECK_CONCLUSIVE_SHALLOW_CHECK;
      return true;

    case BK::CanBeLessOrEqual:	      	      
      ICP("ICP30");
      return false;

    case BK::CanBeGreaterOrEqual:
      // check lexicographically
      switch (constr->lexicographicComparisonResult())
	{
	case BK::Less: 
	  INTERCEPT_FORWARD_CONSTR_CHECK_CONCLUSIVE_SHALLOW_CHECK;
	  return false;
	case BK::Equal: ICP("ICP1"); 
	case BK::Greater: 
	  INTERCEPT_FORWARD_CONSTR_CHECK_CONCLUSIVE_SHALLOW_CHECK;
	  return true;
	case BK::Incomparable: return lexicorgaphicallyGreaterOrEqual(constr->lhs(),constr->rhs());
	};	      	      
      ICP("ICP40");
      RuntimeError::report("Illegal control point in NonrecursiveKBOrdering::ConstraintCheckOnUnifier::...");

    case BK::VolatileButNearlyAlwaysLess:
      // same as below
    case BK::VolatileButNearlyAlwaysGreater:
      // same as below
    case BK::AbsolutelyVolatile:      	      
      ICP("ICP50");
      return false; 
    };
  ICP("ICP3");
#if (defined _SUPPRESS_WARNINGS_) || (defined DEBUG_NAMESPACE)
  return false;
#endif

}; // bool NonrecursiveKBOrdering::ConstraintCheckOnUnifier::holdsGreaterOrEqual(const NonrecursiveKBOrdering::StoredConstraint* constr)


inline
bool NonrecursiveKBOrdering::ConstraintCheckOnUnifier::holdsLess(const NonrecursiveKBOrdering::StoredConstraint* constr)
{
  CALL("holdsLess(..)");
  switch (checkLessOrEqual(constr->weightComparison()))
    {
    case BK::FailureToCompare:
      INTERCEPT_FORWARD_CONSTR_CHECK_CONCLUSIVE_SHALLOW_CHECK;
      return false;

    case BK::AlwaysLess:
      INTERCEPT_FORWARD_CONSTR_CHECK_CONCLUSIVE_SHALLOW_CHECK;
      return true; 

    case BK::AlwaysEqual:
      // check lexicographically
      switch (constr->lexicographicComparisonResult())
	{
	case BK::Less: 
	  INTERCEPT_FORWARD_CONSTR_CHECK_CONCLUSIVE_SHALLOW_CHECK;
	  return true;
	case BK::Equal: 
	  INTERCEPT_FORWARD_CONSTR_CHECK_CONCLUSIVE_SHALLOW_CHECK;
	  return false;
	case BK::Greater: 
	  INTERCEPT_FORWARD_CONSTR_CHECK_CONCLUSIVE_SHALLOW_CHECK;
	  return false;
	case BK::Incomparable: return lexicorgaphicallyLess(constr->lhs(),constr->rhs());
	};	      	      
      ICP("ICP10");
      RuntimeError::report("Illegal control point in NonrecursiveKBOrdering::ConstraintCheckOnUnifier::...");

    case BK::AlwaysGreater:      	      	      
      ICP("ICP20");
      return false;

    case BK::CanBeLessOrEqual:
      // check lexicographically
      switch (constr->lexicographicComparisonResult())
	{
	case BK::Less: 
	  INTERCEPT_FORWARD_CONSTR_CHECK_CONCLUSIVE_SHALLOW_CHECK;
	  return true;
	case BK::Equal: ICP("ICP1"); 
	case BK::Greater: 
	  INTERCEPT_FORWARD_CONSTR_CHECK_CONCLUSIVE_SHALLOW_CHECK;
	  return false;
	case BK::Incomparable: return lexicorgaphicallyLess(constr->lhs(),constr->rhs());
	};	      	      
      ICP("ICP1");
      RuntimeError::report("Illegal control point in NonrecursiveKBOrdering::ConstraintCheckOnUnifier::...");

    case BK::CanBeGreaterOrEqual: 
      // same as below
    case BK::VolatileButNearlyAlwaysLess:
      // same as below
    case BK::VolatileButNearlyAlwaysGreater:
      // same as below
    case BK::AbsolutelyVolatile:    	      	      
      ICP("ICP30");
      return false; 
    };
  ICP("ICP40");
#if (defined _SUPPRESS_WARNINGS_) || (defined DEBUG_NAMESPACE)
  return false;
#endif

}; // bool NonrecursiveKBOrdering::ConstraintCheckOnUnifier::holdsLess(const NonrecursiveKBOrdering::StoredConstraint* constr)



inline
bool NonrecursiveKBOrdering::ConstraintCheckOnUnifier::holdsLessOrEqual(const NonrecursiveKBOrdering::StoredConstraint* constr)
{
  CALL("holdsLessOrEqual(..)");


  switch (checkLessOrEqual(constr->weightComparison()))
    {
    case BK::FailureToCompare:
      INTERCEPT_FORWARD_CONSTR_CHECK_CONCLUSIVE_SHALLOW_CHECK;
      return false;

    case BK::AlwaysLess:
      INTERCEPT_FORWARD_CONSTR_CHECK_CONCLUSIVE_SHALLOW_CHECK;
      return true; 

    case BK::AlwaysEqual:
      // check lexicographically
      switch (constr->lexicographicComparisonResult())
	{
	case BK::Less: 
	  INTERCEPT_FORWARD_CONSTR_CHECK_CONCLUSIVE_SHALLOW_CHECK;
	  return true;
	case BK::Equal: 
	  INTERCEPT_FORWARD_CONSTR_CHECK_CONCLUSIVE_SHALLOW_CHECK;
	  return true;
	case BK::Greater: 
	  INTERCEPT_FORWARD_CONSTR_CHECK_CONCLUSIVE_SHALLOW_CHECK;
	  return false;
	case BK::Incomparable: return lexicorgaphicallyLessOrEqual(constr->lhs(),constr->rhs());
	};	      	      
      ICP("ICP10");
      RuntimeError::report("Illegal control point in NonrecursiveKBOrdering::ConstraintCheckOnUnifier::...");

    case BK::AlwaysGreater:         
      ICP("ICP20");
      return false;

    case BK::CanBeLessOrEqual:
      // check lexicographically
      switch (constr->lexicographicComparisonResult())
	{
	case BK::Less: 
	  INTERCEPT_FORWARD_CONSTR_CHECK_CONCLUSIVE_SHALLOW_CHECK;
	  return true;
	case BK::Equal: ICP("ICP1"); 
	case BK::Greater: 
	  INTERCEPT_FORWARD_CONSTR_CHECK_CONCLUSIVE_SHALLOW_CHECK;
	  return false;
	case BK::Incomparable: return lexicorgaphicallyLessOrEqual(constr->lhs(),constr->rhs());
	};	      	      
      ICP("ICP30");
      RuntimeError::report("Illegal control point in NonrecursiveKBOrdering::ConstraintCheckOnUnifier::...");

    case BK::CanBeGreaterOrEqual:
      // same as below
    case BK::VolatileButNearlyAlwaysLess:
      // same as below
    case BK::VolatileButNearlyAlwaysGreater: 
      // same as below    
    case BK::AbsolutelyVolatile: 	      
      ICP("ICP40");
      return false;  	
    };
  ICP("ICP50");
#if (defined _SUPPRESS_WARNINGS_) || (defined DEBUG_NAMESPACE)
  return false;
#endif

}; // bool NonrecursiveKBOrdering::ConstraintCheckOnUnifier::holdsLessOrEqual(const NonrecursiveKBOrdering::StoredConstraint* constr)


inline
FunctionComparison 
NonrecursiveKBOrdering::ConstraintCheckOnUnifier::checkGreaterOrEqual(const WeightPolynomial::SpecialisedComparison::Stored& weightComparison)
{
  CALL("checkGreaterOrEqual(const WeightPolynomial::SpecialisedComparison::Stored& weightComparison)");

  switch (weightComparison.category())
    {       
    case BK::FailureToCompare:
      return BK::FailureToCompare;
    case BK::AlwaysLess:
      return BK::FailureToCompare;
    case BK::AlwaysEqual:
      // no break, same as below
    case BK::AlwaysGreater:
      return weightComparison.category(); 
    case BK::CanBeLessOrEqual:
      // check if the substitution made it strictly equal
      ASSERT(weightComparison.positivePart());
      ASSERT(weightComparison.negativePart()); 
      ASSERT(weightComparison.positivePart()->hd().isConstant());
      ASSERT(weightComparison.positivePart()->hd().constant() > static_cast<TermWeightType>(0));
      ASSERT(!weightComparison.positivePart()->tl());
      {
	TermWeightType weightPosInst = weightComparison.positivePart()->hd().constant();
	
	for (WeightPolynomial::Stored* mem = weightComparison.negativePart();
	     mem;
	     mem = mem->tl())
	  {
	    if (weightPosInst == static_cast<TermWeightType>(0)) 
	      return BK::FailureToCompare;
	    
	    ASSERT(mem->hd().isVariable() && (mem->hd().coeff() > static_cast<TermWeightType>(0)));
	    TermWeightType coeff = mem->hd().coeff();
	    Unifier::Variable* var = Unifier::current()->variable(mem->hd().var(),_currentVariableIndex);
	    bool freeVar;
	    void* varInstance = var->Unref(freeVar);
	    if (freeVar)
	      {
		return BK::FailureToCompare;
	      }
	    else
	      {		
		TermWeightType w = TermWeighting::current()->computeWeightIfGroundAndLessOrEqual(coeff,
													      static_cast<PrefixSym*>(varInstance),
													      weightPosInst);
		if (w <= static_cast<TermWeightType>(0)) return BK::FailureToCompare;
		ASSERT(w <= weightPosInst);
		weightPosInst -= w;
	      };
	  };
	ASSERT(weightPosInst == static_cast<TermWeightType>(0));
	return BK::AlwaysEqual;
      };

    case BK::CanBeGreaterOrEqual:
      // try to make it AlwaysGreater 
      ASSERT(weightComparison.positivePart());
      ASSERT(weightComparison.negativePart()); 
      ASSERT(weightComparison.negativePart()->hd().isConstant());
      ASSERT(weightComparison.negativePart()->hd().constant() > static_cast<TermWeightType>(0));
      ASSERT(!weightComparison.negativePart()->tl());
      {
	TermWeightType weightNegInst = weightComparison.negativePart()->hd().constant();
	bool ground = true;
	for (WeightPolynomial::Stored* mem = weightComparison.positivePart();
	     mem;
	     mem = mem->tl())
	  {
	    
	    ASSERT(mem->hd().isVariable() && (mem->hd().coeff() > static_cast<TermWeightType>(0)));

	    TermWeightType coeff = mem->hd().coeff();
	    Unifier::Variable* var = Unifier::current()->variable(mem->hd().var(),_currentVariableIndex);
	    bool freeVar;
	    void* varInstance = var->Unref(freeVar);
	    if (freeVar)
	      {
		weightNegInst -= coeff;
		ground = false;
	      }
	    else
	      {		
		bool instanceContainsVariables;
		weightNegInst -= 
		  (coeff * 
		   TermWeighting::current()->computeMinimalInstanceWeight(static_cast<PrefixSym*>(varInstance),
									  instanceContainsVariables));
		if (instanceContainsVariables) ground = false;
	      };	   
	    if (weightNegInst < static_cast<TermWeightType>(0)) 
	      return BK::AlwaysGreater;
	  };
	ASSERT(weightNegInst == static_cast<TermWeightType>(0)); 
	if (ground)
	  return BK::AlwaysEqual;
	return BK::CanBeGreaterOrEqual;
      };


    case BK::VolatileButNearlyAlwaysLess:
      ASSERT(weightComparison.positivePart());
      ASSERT(weightComparison.negativePart()); 
      
      ASSERT(weightComparison.positivePart()->hd().isConstant());
      ASSERT(weightComparison.positivePart()->hd().constant() > static_cast<TermWeightType>(0));
      ASSERT(!weightComparison.positivePart()->tl());
      {
	TermWeightType weightPosInst = weightComparison.positivePart()->hd().constant();
	
	for (WeightPolynomial::Stored* mem = weightComparison.negativePart();
	     mem;
	     mem = mem->tl())
	  {
	    if (weightPosInst == static_cast<TermWeightType>(0)) 
	      return BK::FailureToCompare;
	    
	    ASSERT(mem->hd().isVariable() && (mem->hd().coeff() > static_cast<TermWeightType>(0)));
	    TermWeightType coeff = mem->hd().coeff();
	    Unifier::Variable* var = Unifier::current()->variable(mem->hd().var(),_currentVariableIndex);
	    bool freeVar;
	    void* varInstance = var->Unref(freeVar);
	    if (freeVar)
	      {
		return BK::FailureToCompare;
	      }
	    else
	      {		
		TermWeightType w = TermWeighting::current()->computeWeightIfGroundAndLessOrEqual(coeff,
													      static_cast<PrefixSym*>(varInstance),
													      weightPosInst);
		if (w <= 0) return BK::FailureToCompare;
		ASSERT(w <= weightPosInst);
		weightPosInst -= w;
	      };
	  };
	ASSERT(weightPosInst >= static_cast<TermWeightType>(0));
	// the substitution made both parts ground
	if (weightPosInst == static_cast<TermWeightType>(0)) 
	  return BK::AlwaysEqual;
	return BK::AlwaysGreater;
      };

    case BK::VolatileButNearlyAlwaysGreater:
      ASSERT(weightComparison.positivePart());
      ASSERT(weightComparison.negativePart()); 
      
      ASSERT(weightComparison.negativePart()->hd().isConstant());
      ASSERT(weightComparison.negativePart()->hd().constant() > static_cast<TermWeightType>(0));
      ASSERT(!weightComparison.negativePart()->tl());
      {
	TermWeightType weightNegInst = weightComparison.negativePart()->hd().constant();
	bool ground = true;
	for (WeightPolynomial::Stored* mem = weightComparison.positivePart();
	     mem;
	     mem = mem->tl())
	  {
	    ASSERT(mem->hd().isVariable() && (mem->hd().coeff() > static_cast<TermWeightType>(0)));
	    TermWeightType coeff = mem->hd().coeff();
	    Unifier::Variable* var = Unifier::current()->variable(mem->hd().var(),_currentVariableIndex);
	    bool freeVar;
	    void* varInstance = var->Unref(freeVar);
	    if (freeVar)
	      {
		weightNegInst -= coeff;
		ground = false;
	      }
	    else
	      {		
		bool instanceContainsVariables;
		weightNegInst -= 
		  (coeff * 
		   TermWeighting::current()->computeMinimalInstanceWeight(static_cast<PrefixSym*>(varInstance),
									  instanceContainsVariables));
		if (instanceContainsVariables) ground = false;
	      };
	    if (weightNegInst < static_cast<TermWeightType>(0)) 
	      return BK::AlwaysGreater;
	  };
	
	ASSERT(weightNegInst >= static_cast<TermWeightType>(0));
	if (weightNegInst == static_cast<TermWeightType>(0))
	  {
	    if (ground) 
	      return BK::AlwaysEqual;
	    return BK::CanBeGreaterOrEqual;
	  }
	else
	  return BK::FailureToCompare;
      };

    case BK::AbsolutelyVolatile:
      ASSERT(weightComparison.positivePart());
      ASSERT(weightComparison.negativePart()); 
      // First, collect the instance of the positive part
      _weightPositivePartInstance.reset();
      for (WeightPolynomial::Stored* mem = weightComparison.positivePart();
	   mem;
	   mem = mem->tl())
	{
	  if (mem->hd().isConstant())
	    {
	      _weightPositivePartInstance.add(mem->hd().constant());
	    }
	  else // monomial
	    {
	      ASSERT(mem->hd().isVariable() && (mem->hd().coeff() > static_cast<TermWeightType>(0)));
	      TermWeightType coeff = mem->hd().coeff();
	      Unifier::Variable* var = Unifier::current()->variable(mem->hd().var(),_currentVariableIndex);
	      bool freeVar;
	      void* varInstance = var->Unref(freeVar);
	      if (freeVar)
		{
		  ulong varNum;
		  ulong varIndex;
		  (static_cast<Unifier::Variable*>(varInstance))->NumAndIndex(varNum,varIndex);	    
		  _weightPositivePartInstance.add(coeff,
						  varNum + (varIndex*VampireKernelConst::MaxNumOfVariables));
		}
	      else
		{
		  TermWeighting::current()->collectWeight(_weightPositivePartInstance,
							  coeff,
							  static_cast<PrefixSym*>(varInstance));
		};
	    }; 
	};
      // The instance of the positive part collected.
      // Now, collect the instance of the negative part. 
      {
	
	_weightNegativePartInstance.reset();
	WeightPolynomial::Stored* mem = weightComparison.negativePart();
      next_member:
	ASSERT(mem);
	if (mem->hd().isConstant())
	  {
	    _weightNegativePartInstance.add(mem->hd().constant());
	  }
	else // monomial
	  {
	    ASSERT(mem->hd().isVariable() && (mem->hd().coeff() > static_cast<TermWeightType>(0)));
	    TermWeightType coeff = mem->hd().coeff();
	    Unifier::Variable* var = Unifier::current()->variable(mem->hd().var(),_currentVariableIndex);
	    bool freeVar;
	    void* varInstance = var->Unref(freeVar);
	    if (freeVar)
	      {
		ulong varNum;
		ulong varIndex;
		(static_cast<Unifier::Variable*>(varInstance))->NumAndIndex(varNum,varIndex);	    
		_weightNegativePartInstance.add(coeff,
						varNum + (varIndex*VampireKernelConst::MaxNumOfVariables));
	      }
	    else
	      {
		TermWeighting::current()->collectWeight(_weightNegativePartInstance,
							coeff,
							static_cast<PrefixSym*>(varInstance));
	      };
	    
	  };

	mem = mem->tl();
	if (mem) 
	  {
	    if (!_weightPositivePartInstance.isAlwaysGreater(_weightNegativePartInstance))
	      return BK::FailureToCompare;
	    goto next_member;
	  }
	else
	  {
	    // The instance of the negative part collected.
	    FunctionComparison cmp =
	      _weightPositivePartInstance.compareForGreaterOrEqual(_weightNegativePartInstance);
	    switch (cmp)
	      {
	      case BK::FailureToCompare: 
		return BK::FailureToCompare;
	      case BK::AlwaysLess:
		ICP("ICP10");
		return BK::FailureToCompare;
	      case BK::AlwaysEqual:
		return BK::AlwaysEqual;
	      case BK::AlwaysGreater:
		return BK::AlwaysGreater;
	      case BK::CanBeLessOrEqual:
		ICP("ICP20");
		return BK::FailureToCompare;
	      case BK::CanBeGreaterOrEqual:
		return BK::CanBeGreaterOrEqual;
	      case BK::VolatileButNearlyAlwaysLess:
		// same as below
	      case BK::VolatileButNearlyAlwaysGreater:
		// same as below
	      case BK::AbsolutelyVolatile:
		ICP("ICP30");
		return BK::FailureToCompare;
	      };
	  };	
      };
    }; // switch (weightComparison.category())

  ICP("END");
#if (defined _SUPPRESS_WARNINGS_) || (defined DEBUG_NAMESPACE) 
  return BK::FailureToCompare; 
#endif
}; // NonrecursiveKBOrdering::ConstraintCheckOnUnifier::checkGreaterOrEqual(..)





inline
FunctionComparison 
NonrecursiveKBOrdering::ConstraintCheckOnUnifier::checkLessOrEqual(const WeightPolynomial::SpecialisedComparison::Stored& weightComparison)
{
  CALL("checkLessOrEqual(const WeightPolynomial::SpecialisedComparison::Stored& weightComparison)");
  switch (weightComparison.category())
    {       
    case BK::FailureToCompare:
      return BK::FailureToCompare;
    case BK::AlwaysLess:
      // no break, same as below
    case BK::AlwaysEqual:
      return weightComparison.category(); 
    case BK::AlwaysGreater:
      return BK::FailureToCompare;

    case BK::CanBeLessOrEqual:
      // try to make it AlwaysLess 
      ASSERT(weightComparison.positivePart());
      ASSERT(weightComparison.negativePart()); 
      ASSERT(weightComparison.positivePart()->hd().isConstant());
      ASSERT(weightComparison.positivePart()->hd().constant() > static_cast<TermWeightType>(0));
      ASSERT(!weightComparison.positivePart()->tl());
      {
	TermWeightType weightPosInst = weightComparison.positivePart()->hd().constant();
	bool ground = true;
	for (WeightPolynomial::Stored* mem = weightComparison.negativePart();
	     mem;
	     mem = mem->tl())
	  {
	    ASSERT(mem->hd().isVariable() && (mem->hd().coeff() > static_cast<TermWeightType>(0)));
	    TermWeightType coeff = mem->hd().coeff();
	    Unifier::Variable* var = Unifier::current()->variable(mem->hd().var(),_currentVariableIndex);
	    bool freeVar;
	    void* varInstance = var->Unref(freeVar);
	    if (freeVar)
	      {
		weightPosInst -= coeff;
		ground = false;
	      }
	    else
	      {		
		bool instanceContainsVariables;
		weightPosInst -= 
		  (coeff * 
		   TermWeighting::current()->computeMinimalInstanceWeight(static_cast<PrefixSym*>(varInstance),
									  instanceContainsVariables));
		if (instanceContainsVariables) ground = false;
	      };	   
	    if (weightPosInst < static_cast<TermWeightType>(0)) 
	      return BK::AlwaysLess;
	  };
	ASSERT(weightPosInst == static_cast<TermWeightType>(0)); 
	if (ground)
	  return BK::AlwaysEqual;
	return BK::CanBeLessOrEqual;
      };

    case BK::CanBeGreaterOrEqual:
      // check if the substitution made it strictly equal
      ASSERT(weightComparison.positivePart());
      ASSERT(weightComparison.negativePart()); 
      ASSERT(weightComparison.negativePart()->hd().isConstant());
      ASSERT(weightComparison.negativePart()->hd().constant() > static_cast<TermWeightType>(0));
      ASSERT(!weightComparison.negativePart()->tl());
      {
	TermWeightType weightNegInst = weightComparison.negativePart()->hd().constant();
	
	for (WeightPolynomial::Stored* mem = weightComparison.positivePart();
	     mem;
	     mem = mem->tl())
	  {
	    if (weightNegInst == static_cast<TermWeightType>(0)) 
	      return BK::FailureToCompare;
	    
	    ASSERT(mem->hd().isVariable() && (mem->hd().coeff() > static_cast<TermWeightType>(0)));
	    TermWeightType coeff = mem->hd().coeff();
	    Unifier::Variable* var = Unifier::current()->variable(mem->hd().var(),_currentVariableIndex);
	    bool freeVar;
	    void* varInstance = var->Unref(freeVar);
	    if (freeVar)
	      {
		return BK::FailureToCompare;
	      }
	    else
	      {		
		TermWeightType w = TermWeighting::current()->computeWeightIfGroundAndLessOrEqual(coeff,
													      static_cast<PrefixSym*>(varInstance),
													      weightNegInst);
		if (w <= 0) return BK::FailureToCompare;
		ASSERT(w <= weightNegInst);
		weightNegInst -= w;
	      };
	  };
	ASSERT(weightNegInst == static_cast<TermWeightType>(0));
	return BK::AlwaysEqual;
      };


    case BK::VolatileButNearlyAlwaysLess:
      ASSERT(weightComparison.positivePart());
      ASSERT(weightComparison.negativePart()); 
      ASSERT(weightComparison.positivePart()->hd().isConstant());
      ASSERT(weightComparison.positivePart()->hd().constant() > static_cast<TermWeightType>(0));
      ASSERT(!weightComparison.positivePart()->tl());
      {
	TermWeightType weightPosInst = weightComparison.positivePart()->hd().constant();
	bool ground = true;
	for (WeightPolynomial::Stored* mem = weightComparison.negativePart();
	     mem;
	     mem = mem->tl())
	  {
	    ASSERT(mem->hd().isVariable() && (mem->hd().coeff() > static_cast<TermWeightType>(0)));
	    TermWeightType coeff = mem->hd().coeff();
	    Unifier::Variable* var = Unifier::current()->variable(mem->hd().var(),_currentVariableIndex);
	    bool freeVar;
	    void* varInstance = var->Unref(freeVar);
	    if (freeVar)
	      {
		weightPosInst -= coeff;
		ground = false;
	      }
	    else
	      {		
		bool instanceContainsVariables;
		weightPosInst -= 
		  (coeff * 
		   TermWeighting::current()->computeMinimalInstanceWeight(static_cast<PrefixSym*>(varInstance),
									  instanceContainsVariables));
		if (instanceContainsVariables) ground = false;
	      };
	    if (weightPosInst < static_cast<TermWeightType>(0)) 
	      return BK::AlwaysLess;
	  };
	
	ASSERT(weightPosInst >= static_cast<TermWeightType>(0));
	if (weightPosInst == static_cast<TermWeightType>(0))
	  {
	    if (ground) 
	      return BK::AlwaysEqual;
	    return BK::CanBeLessOrEqual;
	  }
	else
	  return BK::FailureToCompare;
      };



    case BK::VolatileButNearlyAlwaysGreater:
      ASSERT(weightComparison.positivePart());
      ASSERT(weightComparison.negativePart()); 
      ASSERT(weightComparison.negativePart()->hd().isConstant());
      ASSERT(weightComparison.negativePart()->hd().constant() > static_cast<TermWeightType>(0));
      ASSERT(!weightComparison.negativePart()->tl());
      {
	TermWeightType weightNegInst = weightComparison.negativePart()->hd().constant();
	
	for (WeightPolynomial::Stored* mem = weightComparison.positivePart();
	     mem;
	     mem = mem->tl())
	  {
	    if (weightNegInst == static_cast<TermWeightType>(0)) 
	      return BK::FailureToCompare;
	    
	    ASSERT(mem->hd().isVariable() && (mem->hd().coeff() > static_cast<TermWeightType>(0)));
	    TermWeightType coeff = mem->hd().coeff();
	    Unifier::Variable* var = Unifier::current()->variable(mem->hd().var(),_currentVariableIndex);
	    bool freeVar;
	    void* varInstance = var->Unref(freeVar);
	    if (freeVar)
	      {
		return BK::FailureToCompare;
	      }
	    else
	      {		
		TermWeightType w = TermWeighting::current()->computeWeightIfGroundAndLessOrEqual(coeff,
													      static_cast<PrefixSym*>(varInstance),
													      weightNegInst);
		if (w <= 0) return BK::FailureToCompare;
		ASSERT(w <= weightNegInst);
		weightNegInst -= w;
	      };
	  };
	ASSERT(weightNegInst >= static_cast<TermWeightType>(0));
	// the substitution made both parts ground
	if (weightNegInst == static_cast<TermWeightType>(0)) 
	  return BK::AlwaysEqual;
	return BK::AlwaysLess;
      };


    case BK::AbsolutelyVolatile:
      ASSERT(weightComparison.positivePart());
      ASSERT(weightComparison.negativePart()); 
      // First, collect the instance of the positive part
      _weightPositivePartInstance.reset();
      for (WeightPolynomial::Stored* mem = weightComparison.positivePart();
	   mem;
	   mem = mem->tl())
	{
	  if (mem->hd().isConstant())
	    {
	      _weightPositivePartInstance.add(mem->hd().constant());
	    }
	  else // monomial
	    {
	      ASSERT(mem->hd().isVariable() && (mem->hd().coeff() > static_cast<TermWeightType>(0)));
	      TermWeightType coeff = mem->hd().coeff();
	      Unifier::Variable* var = Unifier::current()->variable(mem->hd().var(),_currentVariableIndex);
	      bool freeVar;
	      void* varInstance = var->Unref(freeVar);
	      if (freeVar)
		{
		  ulong varNum;
		  ulong varIndex;
		  (static_cast<Unifier::Variable*>(varInstance))->NumAndIndex(varNum,varIndex);	    
		  _weightPositivePartInstance.add(coeff,
						  varNum + (varIndex*VampireKernelConst::MaxNumOfVariables));
		}
	      else
		{
		  TermWeighting::current()->collectWeight(_weightPositivePartInstance,
							  coeff,
							  static_cast<PrefixSym*>(varInstance));
		};
	    }; 
	};
      
      
      // The instance of the positive part collected.
      // Now, collect the instance of the negative part. 
      {	
	_weightNegativePartInstance.reset();
	WeightPolynomial::Stored* mem = weightComparison.negativePart();
      next_member:
	ASSERT(mem);
	if (mem->hd().isConstant())
	  {
	    _weightNegativePartInstance.add(mem->hd().constant());
	  }
	else // monomial
	  {
	    ASSERT(mem->hd().isVariable() && (mem->hd().coeff() > static_cast<TermWeightType>(0)));
	    TermWeightType coeff = mem->hd().coeff();
	    Unifier::Variable* var = Unifier::current()->variable(mem->hd().var(),_currentVariableIndex);
	    bool freeVar;
	    void* varInstance = var->Unref(freeVar);
	    if (freeVar)
	      {
		ulong varNum;
		ulong varIndex;
		(static_cast<Unifier::Variable*>(varInstance))->NumAndIndex(varNum,varIndex);	    
		_weightNegativePartInstance.add(coeff,
						varNum + (varIndex*VampireKernelConst::MaxNumOfVariables));
	      }
	    else
	      {
		TermWeighting::current()->collectWeight(_weightNegativePartInstance,
							coeff,
							static_cast<PrefixSym*>(varInstance));
	      };
	    
	  };

	mem = mem->tl();

	if (mem) 
	  {
	    if (_weightPositivePartInstance.isNeverGreater(_weightNegativePartInstance))
	      {
		return BK::AlwaysLess;
	      };
	    goto next_member;
	  }
	else
	  {
	    // The instance of the negative part collected.

	    FunctionComparison cmp =
	      _weightPositivePartInstance.compareForLessOrEqual(_weightNegativePartInstance);

	    switch (cmp)
	      {
	      case BK::FailureToCompare:		
		return BK::FailureToCompare;
	      case BK::AlwaysLess:
		return BK::AlwaysLess;
	      case BK::AlwaysEqual:
		return BK::AlwaysEqual;
	      case BK::AlwaysGreater:
		ICP("ICP10");
		return BK::FailureToCompare;
	      case BK::CanBeLessOrEqual:
		return BK::CanBeLessOrEqual;
	      case BK::CanBeGreaterOrEqual:
		// same as below
	      case BK::VolatileButNearlyAlwaysLess:
		// same as below
	      case BK::VolatileButNearlyAlwaysGreater:
		// same as below
	      case BK::AbsolutelyVolatile:
		ICP("ICP20");
		return BK::FailureToCompare;
	      };
	  };	
      };
    }; // switch (weightComparison.category())

  ICP("END");
#if (defined _SUPPRESS_WARNINGS_) || (defined DEBUG_NAMESPACE) 
  return BK::FailureToCompare; 
#endif

}; // NonrecursiveKBOrdering::ConstraintCheckOnUnifier::checkLessOrEqual(..)




inline
bool NonrecursiveKBOrdering::ConstraintCheckOnUnifier::lexicorgaphicallyGreater(const TERM& lhs,const TERM& rhs)
{
  CALL("lexicorgaphicallyGreater(const TERM& lhs,const TERM& rhs)");
  _lhsIter.Reset(_currentVariableIndex,(TERM&)lhs);
  _rhsIter.Reset(_currentVariableIndex,(TERM&)rhs);
  do
    {
      if (_lhsIter.CurrSym() != _rhsIter.CurrSym())
	{
	  if (_lhsIter.CurrSym().isVariable() || _rhsIter.CurrSym().isVariable())
	    return false; // terms are incomparable
	  return _lhsIter.CurrSym().Func().hasGreaterPrecedenceThan(_rhsIter.CurrSym().Func());
	};
	  
      _lhsIter.SkipSym();
      _rhsIter.SkipSym();
    }
  while (!_lhsIter.Empty());
  ASSERT(_rhsIter.Empty());
  return false; // equal terms 
}; // bool NonrecursiveKBOrdering::ConstraintCheckOnUnifier::lexicorgaphicallyGreater(const TERM& lhs,const TERM& rhs)

inline
bool NonrecursiveKBOrdering::ConstraintCheckOnUnifier::lexicorgaphicallyGreaterOrEqual(const TERM& lhs,const TERM& rhs)
{
  CALL("lexicorgaphicallyGreaterOrEqual(const TERM& lhs,const TERM& rhs)");
  _lhsIter.Reset(_currentVariableIndex,(TERM&)lhs);
  _rhsIter.Reset(_currentVariableIndex,(TERM&)rhs);
  do
    {
      if (_lhsIter.CurrSym() != _rhsIter.CurrSym())
	{
	  if (_lhsIter.CurrSym().isVariable() || _rhsIter.CurrSym().isVariable())
	    return false; // terms are incomparable
	  return _lhsIter.CurrSym().Func().hasGreaterPrecedenceThan(_rhsIter.CurrSym().Func());
	};
	  
      _lhsIter.SkipSym();
      _rhsIter.SkipSym();
    }
  while (!_lhsIter.Empty());
  ASSERT(_rhsIter.Empty());
  return true; // equal terms 
}; // bool NonrecursiveKBOrdering::ConstraintCheckOnUnifier::lexicorgaphicallyGreaterOrEqual(const TERM& lhs,const TERM& rhs)


inline
bool 
NonrecursiveKBOrdering::ConstraintCheckOnUnifier::lexicorgaphicallyLess(const TERM& lhs,const TERM& rhs)
{
  return lexicorgaphicallyGreater(rhs,lhs);
};

inline
bool 
NonrecursiveKBOrdering::ConstraintCheckOnUnifier::lexicorgaphicallyLessOrEqual(const TERM& lhs,const TERM& rhs)
{
  return lexicorgaphicallyGreaterOrEqual(rhs,lhs);
};


//===================================================================
#undef DEBUG_NAMESPACE
#ifdef DEBUG_SIMPLIFICATION_ORDERING
#define DEBUG_NAMESPACE "NonrecursiveKBOrdering::ConstraintCheckOnForwardMatchingSubst"
#endif
#include "debugMacros.hpp"
//===================================================================

NonrecursiveKBOrdering::ConstraintCheckOnForwardMatchingSubst::~ConstraintCheckOnForwardMatchingSubst()
{
};

bool 
NonrecursiveKBOrdering::ConstraintCheckOnForwardMatchingSubst::holds(const SimplificationOrdering::StoredConstraint* constr,
								     const ForwardMatchingSubstitutionCache* subst)
{
  CALL("holds(..)");

  //DF; cout << "HOLDS? " << static_cast<NonrecursiveKBOrdering::StoredConstraint*>(constr) << "\n";
  ASSERT(constr);
  const StoredConstraint* nativeConstr = 
    static_cast<const StoredConstraint*>(static_cast<const void*>(constr));


#ifdef MEASURE_FORWARD_CONSTR_CHECK
  INTERCEPT_FORWARD_CONSTR_CHECK_BEGIN2;
  // Straightforward check:
  SimplificationOrdering::current()->compare(nativeConstr->lhs(),\
					     nativeConstr->rhs(),\
					     subst);
  INTERCEPT_FORWARD_CONSTR_CHECK_END2;
#endif

  INTERCEPT_FORWARD_CONSTR_CHECK_NEW_CHECK;
  INTERCEPT_FORWARD_CONSTR_CHECK_BEGIN1;


  bool result;
  DOP(Comparison debugCmp = \
      SimplificationOrdering::current()->compare(nativeConstr->lhs(),\
						 nativeConstr->rhs(),\
						 subst));

  //DF; cout << "DEBUG CMP = " << debugCmp << "\n";

  _subst = subst;
  switch (nativeConstr->targetRelation())
    {
    case StoredConstraint::Greater:
      result = holdsGreater(nativeConstr);
      ASSERT(result == (debugCmp == BK::Greater));
      INTERCEPT_FORWARD_CONSTR_CHECK_END1;
      return result;
    case StoredConstraint::GreaterOrEqual:
      result = holdsGreaterOrEqual(nativeConstr);
      ASSERT(result == ((debugCmp == BK::Greater) || (debugCmp == BK::Equal)));
      INTERCEPT_FORWARD_CONSTR_CHECK_END1;
      return result;
    case StoredConstraint::Less:
      result = holdsLess(nativeConstr);
      ASSERT(result == (debugCmp == BK::Less));
      INTERCEPT_FORWARD_CONSTR_CHECK_END1;
      return result;
    case StoredConstraint::LessOrEqual:
      result = holdsLessOrEqual(nativeConstr);
      ASSERT(result == ((debugCmp == BK::Less) || (debugCmp == BK::Equal)));
      INTERCEPT_FORWARD_CONSTR_CHECK_END1;
      return result;
    };
  ICP("ICP0");
#if (defined _SUPPRESS_WARNINGS_) || (defined DEBUG_NAMESPACE)
  INTERCEPT_FORWARD_CONSTR_CHECK_END1;
  return false;
#endif
}; // bool NonrecursiveKBOrdering::ConstraintCheckOnForwardMatchingSubst::holds(..)

 
inline
bool 
NonrecursiveKBOrdering::ConstraintCheckOnForwardMatchingSubst::holdsGreater(const StoredConstraint* constr)
{
  CALL("holdsGreater(..)");
  switch (checkGreaterOrEqual(constr->weightComparison()))
    {
    case BK::FailureToCompare:
      return false;

    case BK::AlwaysLess:
      ICP("ICP10");
      return false; 

    case BK::AlwaysEqual:
      // check lexicographically
      switch (constr->lexicographicComparisonResult())
	{
	case BK::Less: return false;
	case BK::Equal: return false;
	case BK::Greater: return true;
	case BK::Incomparable: return lexicorgaphicallyGreater(constr->lhs(),constr->rhs());
	};	      	      
      ICP("ICP20");
      RuntimeError::report("Illegal control point in NonrecursiveKBOrdering::ConstraintCheckOnUnifier::...");

    case BK::AlwaysGreater:
      return true;

    case BK::CanBeLessOrEqual:
      ICP("ICP30");
      return false;

    case BK::CanBeGreaterOrEqual:
      // check lexicographically
      switch (constr->lexicographicComparisonResult())
	{
	case BK::Less: return false;
	case BK::Equal: ICP("ICP1"); 
	case BK::Greater: return true;
	case BK::Incomparable: return lexicorgaphicallyGreater(constr->lhs(),constr->rhs());
	};	      	      
      ICP("ICP40");
      RuntimeError::report("Illegal control point in NonrecursiveKBOrdering::ConstraintCheckOnUnifier::...");

    case BK::VolatileButNearlyAlwaysLess:
      // same as below
    case BK::VolatileButNearlyAlwaysGreater:
      // same as below
    case BK::AbsolutelyVolatile:     	      
      ICP("ICP50");
      return false;
    };
  ICP("ICP60");
#if (defined _SUPPRESS_WARNINGS_) || (defined DEBUG_NAMESPACE)
  return false;
#endif
}; // bool NonrecursiveKBOrdering::ConstraintCheckOnForwardMatchingSubst::holdsGreater(const StoredConstraint* constr)




inline
bool 
NonrecursiveKBOrdering::ConstraintCheckOnForwardMatchingSubst::holdsGreaterOrEqual(const StoredConstraint* constr)
{
  CALL("holdsGreaterOrEqual(..)");

  switch (checkGreaterOrEqual(constr->weightComparison()))
    {
    case BK::FailureToCompare:
      return false;

    case BK::AlwaysLess:
      ICP("ICP10");
      return false; 

    case BK::AlwaysEqual:
      // check lexicographically
      switch (constr->lexicographicComparisonResult())
	{
	case BK::Less: return false;
	case BK::Equal: return true;
	case BK::Greater: return true;
	case BK::Incomparable: return lexicorgaphicallyGreaterOrEqual(constr->lhs(),constr->rhs());
	};	      	      
      ICP("ICP20");
      RuntimeError::report("Illegal control point in NonrecursiveKBOrdering::ConstraintCheckOnUnifier::...");

    case BK::AlwaysGreater:
      return true;

    case BK::CanBeLessOrEqual:
      ICP("ICP30");
      return false;

    case BK::CanBeGreaterOrEqual:
      // check lexicographically
      switch (constr->lexicographicComparisonResult())
	{
	case BK::Less: return false;
	case BK::Equal: ICP("ICP1"); 
	case BK::Greater: return true;
	case BK::Incomparable: return lexicorgaphicallyGreaterOrEqual(constr->lhs(),constr->rhs());
	};	      	      
      ICP("ICP40");
      RuntimeError::report("Illegal control point in NonrecursiveKBOrdering::ConstraintCheckOnUnifier::...");

    case BK::VolatileButNearlyAlwaysLess:
      // same as below
    case BK::VolatileButNearlyAlwaysGreater:
      // same as below
    case BK::AbsolutelyVolatile:
      ICP("ICP50");
      return false; 
    };
  ICP("ICP60");
#if (defined _SUPPRESS_WARNINGS_) || (defined DEBUG_NAMESPACE)
  return false;
#endif

}; // bool NonrecursiveKBOrdering::ConstraintCheckOnForwardMatchingSubst::holdsGreaterOrEqual(const StoredConstraint* constr)

inline
bool 
NonrecursiveKBOrdering::ConstraintCheckOnForwardMatchingSubst::holdsLess(const StoredConstraint* constr)
{
  CALL("holdsLess(..)");

  switch (checkLessOrEqual(constr->weightComparison()))
    {
    case BK::FailureToCompare:
      return false;

    case BK::AlwaysLess:
      return true; 

    case BK::AlwaysEqual:
      // check lexicographically
      switch (constr->lexicographicComparisonResult())
	{
	case BK::Less: return true;
	case BK::Equal: return false;
	case BK::Greater: return false;
	case BK::Incomparable: return lexicorgaphicallyLess(constr->lhs(),constr->rhs());
	};	      	      
      ICP("ICP10");
      RuntimeError::report("Illegal control point in NonrecursiveKBOrdering::ConstraintCheckOnUnifier::...");

    case BK::AlwaysGreater:    	      
      ICP("ICP20");
      return false;

    case BK::CanBeLessOrEqual:
      // check lexicographically
      switch (constr->lexicographicComparisonResult())
	{
	case BK::Less: return true;
	case BK::Equal: ICP("ICP1"); 
	case BK::Greater: return false;
	case BK::Incomparable: return lexicorgaphicallyLess(constr->lhs(),constr->rhs());
	};	      	      
      ICP("ICP30");
      RuntimeError::report("Illegal control point in NonrecursiveKBOrdering::ConstraintCheckOnUnifier::...");

    case BK::CanBeGreaterOrEqual:
      // same as below
    case BK::VolatileButNearlyAlwaysLess:
      // same as below
    case BK::VolatileButNearlyAlwaysGreater:
      // same as below
    case BK::AbsolutelyVolatile:            	      
      ICP("ICP40");
      return false; 
    };
  ICP("ICP50");
#if (defined _SUPPRESS_WARNINGS_) || (defined DEBUG_NAMESPACE)
  return false;
#endif

}; // bool NonrecursiveKBOrdering::ConstraintCheckOnForwardMatchingSubst::holdsLess(const StoredConstraint* constr)

inline
bool 
NonrecursiveKBOrdering::ConstraintCheckOnForwardMatchingSubst::holdsLessOrEqual(const StoredConstraint* constr)
{
  CALL("holdsLessOrEqual(..)");

  switch (checkLessOrEqual(constr->weightComparison()))
    {
    case BK::FailureToCompare:
      return false;

    case BK::AlwaysLess:
      return true; 

    case BK::AlwaysEqual:
      // check lexicographically
      switch (constr->lexicographicComparisonResult())
	{
	case BK::Less: return true;
	case BK::Equal: return true;
	case BK::Greater: return false;
	case BK::Incomparable: return lexicorgaphicallyLessOrEqual(constr->lhs(),constr->rhs());
	};	      	      
      ICP("ICP10");
      RuntimeError::report("Illegal control point in NonrecursiveKBOrdering::ConstraintCheckOnUnifier::...");

    case BK::AlwaysGreater:   	      
      ICP("ICP20");
      return false;

    case BK::CanBeLessOrEqual:
      // check lexicographically
      switch (constr->lexicographicComparisonResult())
	{
	case BK::Less: return true;
	case BK::Equal: ICP("ICP1"); 
	case BK::Greater: return false;
	case BK::Incomparable: return lexicorgaphicallyLessOrEqual(constr->lhs(),constr->rhs());
	};	      	      
      ICP("ICP30");
      RuntimeError::report("Illegal control point in NonrecursiveKBOrdering::ConstraintCheckOnUnifier::...");

    case BK::CanBeGreaterOrEqual:
    case BK::VolatileButNearlyAlwaysLess:
    case BK::VolatileButNearlyAlwaysGreater:
    case BK::AbsolutelyVolatile:    	      
      ICP("ICP40");
      return false; 
    };
  ICP("ICP50");
#if (defined _SUPPRESS_WARNINGS_) || (defined DEBUG_NAMESPACE)
  return false;
#endif
}; // bool NonrecursiveKBOrdering::ConstraintCheckOnForwardMatchingSubst::holdsLessOrEqual(const StoredConstraint* constr)
  
inline
FunctionComparison 
NonrecursiveKBOrdering::ConstraintCheckOnForwardMatchingSubst::checkGreaterOrEqual(const WeightPolynomial::SpecialisedComparison::Stored& weightComparison)
{
  CALL("checkGreaterOrEqual(..)");

  switch (weightComparison.category())
    {       
    case BK::FailureToCompare:
      return BK::FailureToCompare;

    case BK::AlwaysLess:
      return BK::FailureToCompare;
    case BK::AlwaysEqual:
      // no break, same as below
    case BK::AlwaysGreater:
      // no break, same as below
      return weightComparison.category(); 
    case BK::CanBeLessOrEqual:
      // check if the substitution made it strictly equal
      ASSERT(weightComparison.positivePart());
      ASSERT(weightComparison.negativePart()); 
      ASSERT(weightComparison.positivePart()->hd().isConstant());
      ASSERT(weightComparison.positivePart()->hd().constant() > static_cast<TermWeightType>(0));
      ASSERT(!weightComparison.positivePart()->tl());
      {
	TermWeightType weightPosInst = weightComparison.positivePart()->hd().constant();
	
	for (WeightPolynomial::Stored* mem = weightComparison.negativePart();
	     mem;
	     mem = mem->tl())
	  {
	    if (weightPosInst == static_cast<TermWeightType>(0)) 
	      return BK::FailureToCompare;
	    
	    ASSERT(mem->hd().isVariable() && (mem->hd().coeff() > static_cast<TermWeightType>(0)));
	    TermWeightType coeff = mem->hd().coeff();
	    const Flatterm* varInstance = _subst->map(mem->hd().var());
	    if (varInstance->isVariable())
	      {
		return BK::FailureToCompare;
	      }
	    else
	      {			
		TermWeightType w = TermWeighting::current()->computeWeightIfGroundAndLessOrEqual(coeff,
													      varInstance,
													      weightPosInst);
		if (w <= 0) return BK::FailureToCompare;
		ASSERT(w <= weightPosInst);
		weightPosInst -= w;
	      };
	  };
	ASSERT(weightPosInst == static_cast<TermWeightType>(0));
	return BK::AlwaysEqual;
      };

    case BK::CanBeGreaterOrEqual:
      // try to make it AlwaysGreater 
      ASSERT(weightComparison.positivePart());
      ASSERT(weightComparison.negativePart()); 
      ASSERT(weightComparison.negativePart()->hd().isConstant());
      ASSERT(weightComparison.negativePart()->hd().constant() > static_cast<TermWeightType>(0));
      ASSERT(!weightComparison.negativePart()->tl());
      {
	TermWeightType weightNegInst = weightComparison.negativePart()->hd().constant();
	bool ground = true;
	for (WeightPolynomial::Stored* mem = weightComparison.positivePart();
	     mem;
	     mem = mem->tl())
	  {
	    ASSERT(mem->hd().isVariable() && (mem->hd().coeff() > static_cast<TermWeightType>(0)));
	    TermWeightType coeff = mem->hd().coeff();
	    const Flatterm* varInstance = _subst->map(mem->hd().var());
	    if (varInstance->isVariable())
	      {
		weightNegInst -= coeff;
		ground = false;
	      }
	    else
	      {
		bool instanceContainsVariables;
		weightNegInst -= 
		  (coeff * 
		   TermWeighting::current()->computeMinimalInstanceWeight(varInstance,
									  instanceContainsVariables));
		if (instanceContainsVariables) ground = false;
	      };	
	    if (weightNegInst < static_cast<TermWeightType>(0)) 
	      return BK::AlwaysGreater;
	  };
	ASSERT(weightNegInst == static_cast<TermWeightType>(0)); 
	if (ground)
	  return BK::AlwaysEqual;
	return BK::CanBeGreaterOrEqual;
      };


    case BK::VolatileButNearlyAlwaysLess:
      ASSERT(weightComparison.positivePart());
      ASSERT(weightComparison.negativePart()); 
      ASSERT(weightComparison.positivePart()->hd().isConstant());
      ASSERT(weightComparison.positivePart()->hd().constant() > static_cast<TermWeightType>(0));
      ASSERT(!weightComparison.positivePart()->tl());
      {
	TermWeightType weightPosInst = weightComparison.positivePart()->hd().constant();
	
	for (WeightPolynomial::Stored* mem = weightComparison.negativePart();
	     mem;
	     mem = mem->tl())
	  {
	    if (weightPosInst == static_cast<TermWeightType>(0)) 
	      return BK::FailureToCompare;
	    
	    ASSERT(mem->hd().isVariable() && (mem->hd().coeff() > static_cast<TermWeightType>(0)));
	    TermWeightType coeff = mem->hd().coeff();
	    const Flatterm* varInstance = _subst->map(mem->hd().var());
	    if (varInstance->isVariable())
	      {
		return BK::FailureToCompare;
	      }
	    else
	      {		
		TermWeightType w = TermWeighting::current()->computeWeightIfGroundAndLessOrEqual(coeff,
													      varInstance,
													      weightPosInst);
		if (w <= 0) return BK::FailureToCompare;
		ASSERT(w <= weightPosInst);
		weightPosInst -= w;
	      };
	  };
	ASSERT(weightPosInst >= static_cast<TermWeightType>(0));
	// the substitution made both terms ground
	if (weightPosInst == static_cast<TermWeightType>(0)) 
	  return BK::AlwaysEqual;
	return BK::AlwaysGreater;
      };

    case BK::VolatileButNearlyAlwaysGreater:
      ASSERT(weightComparison.positivePart());
      ASSERT(weightComparison.negativePart()); 
      ASSERT(weightComparison.negativePart()->hd().isConstant());
      ASSERT(weightComparison.negativePart()->hd().constant() > static_cast<TermWeightType>(0));
      ASSERT(!weightComparison.negativePart()->tl());
      {
	TermWeightType weightNegInst = weightComparison.negativePart()->hd().constant();
	bool ground = true;
	for (WeightPolynomial::Stored* mem = weightComparison.positivePart();
	     mem;
	     mem = mem->tl())
	  {
	    ASSERT(mem->hd().isVariable() && (mem->hd().coeff() > static_cast<TermWeightType>(0)));
	    TermWeightType coeff = mem->hd().coeff();
	    const Flatterm* varInstance = _subst->map(mem->hd().var());
	    if (varInstance->isVariable())
	      {
		weightNegInst -= coeff;
		ground = false;
	      }
	    else
	      {		
		bool instanceContainsVariables;
		weightNegInst -= 
		  (coeff * 
		   TermWeighting::current()->computeMinimalInstanceWeight(varInstance,
									  instanceContainsVariables));
		if (instanceContainsVariables) ground = false;
	      };
	    if (weightNegInst < static_cast<TermWeightType>(0)) 
	      return BK::AlwaysGreater;
	  };
	
	ASSERT(weightNegInst >= static_cast<TermWeightType>(0));
	if (weightNegInst == static_cast<TermWeightType>(0))
	  {
	    if (ground) 
	      return BK::AlwaysEqual;
	    return BK::CanBeGreaterOrEqual;
	  }
	else
	  return BK::FailureToCompare;
      };

    case BK::AbsolutelyVolatile:
      ASSERT(weightComparison.positivePart());
      ASSERT(weightComparison.negativePart()); 
      // First, collect the instance of the positive part
      _weightPositivePartInstance.reset();
      for (WeightPolynomial::Stored* mem = weightComparison.positivePart();
	   mem;
	   mem = mem->tl())
	{
	  if (mem->hd().isConstant())
	    {
	      _weightPositivePartInstance.add(mem->hd().constant());
	    }
	  else // monomial
	    {
	      ASSERT(mem->hd().isVariable() && (mem->hd().coeff() > static_cast<TermWeightType>(0)));
	      TermWeightType coeff = mem->hd().coeff();
	      const Flatterm* varInstance = _subst->map(mem->hd().var());

	      if (varInstance->isVariable())
		{ 
		  _weightPositivePartInstance.add(coeff,varInstance->Var());
		}
	      else
		{
		  TermWeighting::current()->collectWeight(_weightPositivePartInstance,
							  coeff,
							  varInstance);
		};
	    }; 
	};
      // The instance of the positive part collected.
      // Now, collect the instance of the negative part. 
      {
	
	_weightNegativePartInstance.reset();
	WeightPolynomial::Stored* mem = weightComparison.negativePart();
      next_member:
	ASSERT(mem);
	if (mem->hd().isConstant())
	  {
	    _weightNegativePartInstance.add(mem->hd().constant());
	  }
	else // monomial
	  {
	    ASSERT(mem->hd().isVariable() && (mem->hd().coeff() > static_cast<TermWeightType>(0)));
	    TermWeightType coeff = mem->hd().coeff();
	    const Flatterm* varInstance = _subst->map(mem->hd().var());

	    if (varInstance->isVariable())
	      {    
		_weightNegativePartInstance.add(coeff,varInstance->Var());
	      }
	    else
	      {
		TermWeighting::current()->collectWeight(_weightNegativePartInstance,
							coeff,
							varInstance);
	      };
	    
	  };

	mem = mem->tl();
	if (mem) 
	  {
	    if (!_weightPositivePartInstance.isAlwaysGreater(_weightNegativePartInstance))
	      return BK::FailureToCompare;
	    goto next_member;
	  }
	else
	  {
	    // The instance of the negative part collected.
	    FunctionComparison cmp =
	      _weightPositivePartInstance.compareForGreaterOrEqual(_weightNegativePartInstance);
	    switch (cmp)
	      {
	      case BK::FailureToCompare:
		return BK::FailureToCompare;
	      case BK::AlwaysLess:
		ICP("ICP10");
		return BK::FailureToCompare;
	      case BK::AlwaysEqual:
		return BK::AlwaysEqual;
	      case BK::AlwaysGreater:
		return BK::AlwaysGreater;
	      case BK::CanBeLessOrEqual:
		ICP("ICP20");
		return BK::FailureToCompare;
	      case BK::CanBeGreaterOrEqual:
		return BK::CanBeGreaterOrEqual;
	      case BK::VolatileButNearlyAlwaysLess:
		// same as below
	      case BK::VolatileButNearlyAlwaysGreater:
		// same as below
	      case BK::AbsolutelyVolatile:
		ICP("ICP30");
		return BK::FailureToCompare;
	      };
	  };	
      };
    }; // switch (weightComparison.category())

  ICP("END");
#if (defined _SUPPRESS_WARNINGS_) || (defined DEBUG_NAMESPACE) 
  return BK::FailureToCompare; 
#endif
}; // FunctionComparison NonrecursiveKBOrdering::ConstraintCheckOnForwardMatchingSubst::checkGreaterOrEqual(const WeightPolynomial::SpecialisedComparison::Stored& weightComparison)


inline
FunctionComparison 
NonrecursiveKBOrdering::ConstraintCheckOnForwardMatchingSubst::checkLessOrEqual(const WeightPolynomial::SpecialisedComparison::Stored& weightComparison)
{
  CALL("checkLessOrEqual(..)");

  switch (weightComparison.category())
    {       
    case BK::FailureToCompare:
      return BK::FailureToCompare;

    case BK::AlwaysLess:
      // no break, same as below
    case BK::AlwaysEqual:
      return weightComparison.category();
      
    case BK::AlwaysGreater:
      return BK::FailureToCompare;

    case BK::CanBeLessOrEqual:
      // try to make it AlwaysLess 
      COP("case BK::CanBeLessOrEqual");
      ASSERT(weightComparison.positivePart());
      ASSERT(weightComparison.negativePart()); 
      ASSERT(weightComparison.positivePart()->hd().isConstant());
      ASSERT(weightComparison.positivePart()->hd().constant() > static_cast<TermWeightType>(0));
      ASSERT(!weightComparison.positivePart()->tl());
      {
	TermWeightType weightPosInst = weightComparison.positivePart()->hd().constant();
	bool ground = true;
	for (WeightPolynomial::Stored* mem = weightComparison.negativePart();
	     mem;
	     mem = mem->tl())
	  {
	    ASSERT(mem->hd().isVariable() && (mem->hd().coeff() > static_cast<TermWeightType>(0)));
	    TermWeightType coeff = mem->hd().coeff();
	    const Flatterm* varInstance = _subst->map(mem->hd().var());

	    if (varInstance->isVariable())
	      {
		weightPosInst -= coeff;
		ground = false;
	      }
	    else
	      {		
		bool instanceContainsVariables;
		weightPosInst -= 
		  (coeff * 
		   TermWeighting::current()->computeMinimalInstanceWeight(varInstance,
									  instanceContainsVariables));
		if (instanceContainsVariables) ground = false;
	      };	   
	    if (weightPosInst < static_cast<TermWeightType>(0)) 
	      return BK::AlwaysLess;
	  };
	ASSERT(weightPosInst == static_cast<TermWeightType>(0)); 
	if (ground)
	  return BK::AlwaysEqual;
	return BK::CanBeLessOrEqual;
      };

    case BK::CanBeGreaterOrEqual:
      // check if the substitution made it strictly equal
      COP("case BK::CanBeGreaterOrEqual");
      ASSERT(weightComparison.positivePart());
      ASSERT(weightComparison.negativePart()); 
      ASSERT(weightComparison.negativePart()->hd().isConstant());
      ASSERT(weightComparison.negativePart()->hd().constant() > static_cast<TermWeightType>(0));
      ASSERT(!weightComparison.negativePart()->tl());
      {
	TermWeightType weightNegInst = weightComparison.negativePart()->hd().constant();
	
	for (WeightPolynomial::Stored* mem = weightComparison.positivePart();
	     mem;
	     mem = mem->tl())
	  {
	    if (weightNegInst == static_cast<TermWeightType>(0)) 
	      return BK::FailureToCompare;
	    
	    ASSERT(mem->hd().isVariable() && (mem->hd().coeff() > static_cast<TermWeightType>(0)));
	    TermWeightType coeff = mem->hd().coeff();
	    const Flatterm* varInstance = _subst->map(mem->hd().var());
	    if (varInstance->isVariable())
	      {
		return BK::FailureToCompare;
	      }
	    else
	      {		
		TermWeightType w = TermWeighting::current()->computeWeightIfGroundAndLessOrEqual(coeff,
													      varInstance,
													      weightNegInst);
		if (w <= 0) return BK::FailureToCompare;
		ASSERT(w <= weightNegInst);
		weightNegInst -= w;
	      };
	  };
	ASSERT(weightNegInst == static_cast<TermWeightType>(0));
	return BK::AlwaysEqual;
      };


    case BK::VolatileButNearlyAlwaysLess:
      COP("case BK::VolatileButNearlyAlwaysLess");
      ASSERT(weightComparison.positivePart());
      ASSERT(weightComparison.negativePart()); 
      ASSERT(weightComparison.positivePart()->hd().isConstant());
      ASSERT(weightComparison.positivePart()->hd().constant() > static_cast<TermWeightType>(0));
      ASSERT(!weightComparison.positivePart()->tl());
      {
	TermWeightType weightPosInst = weightComparison.positivePart()->hd().constant();
	bool ground = true;
	for (WeightPolynomial::Stored* mem = weightComparison.negativePart();
	     mem;
	     mem = mem->tl())
	  {
	    ASSERT(mem->hd().isVariable() && (mem->hd().coeff() > static_cast<TermWeightType>(0)));
	    TermWeightType coeff = mem->hd().coeff();
	    const Flatterm* varInstance = _subst->map(mem->hd().var());
	    if (varInstance->isVariable())
	      {
		weightPosInst -= coeff;
		ground = false;
	      }
	    else
	      {		
		bool instanceContainsVariables;
		weightPosInst -= 
		  (coeff * 
		   TermWeighting::current()->computeMinimalInstanceWeight(varInstance,
									  instanceContainsVariables));
		if (instanceContainsVariables) ground = false;
	      };
	    if (weightPosInst < static_cast<TermWeightType>(0)) 
	      return BK::AlwaysLess;
	  };
	
	ASSERT(weightPosInst >= static_cast<TermWeightType>(0));
	if (weightPosInst == static_cast<TermWeightType>(0))
	  {
	    if (ground) 
	      return BK::AlwaysEqual;
	    return BK::CanBeLessOrEqual;
	  }
	else
	  return BK::FailureToCompare;
      };



    case BK::VolatileButNearlyAlwaysGreater:
      COP("case BK::VolatileButNearlyAlwaysGreater");
      ASSERT(weightComparison.positivePart());
      ASSERT(weightComparison.negativePart()); 
      ASSERT(weightComparison.negativePart()->hd().isConstant());
      ASSERT(weightComparison.negativePart()->hd().constant() > static_cast<TermWeightType>(0));
      ASSERT(!weightComparison.negativePart()->tl());
      {
	TermWeightType weightNegInst = weightComparison.negativePart()->hd().constant();
	
	for (WeightPolynomial::Stored* mem = weightComparison.positivePart();
	     mem;
	     mem = mem->tl())
	  {
	    if (weightNegInst == static_cast<TermWeightType>(0)) 
	      return BK::FailureToCompare;
	    
	    ASSERT(mem->hd().isVariable() && (mem->hd().coeff() > static_cast<TermWeightType>(0)));
	    TermWeightType coeff = mem->hd().coeff();
	    const Flatterm* varInstance = _subst->map(mem->hd().var());
	    if (varInstance->isVariable())
	      {
		return BK::FailureToCompare;
	      }
	    else
	      {		
		TermWeightType w = TermWeighting::current()->computeWeightIfGroundAndLessOrEqual(coeff,
													      varInstance,
													      weightNegInst);
		if (w <= 0) return BK::FailureToCompare;
		ASSERT(w <= weightNegInst);
		weightNegInst -= w;
	      };
	  };
	ASSERT(weightNegInst >= static_cast<TermWeightType>(0));
	// the substitution made both parts ground
	if (weightNegInst == static_cast<TermWeightType>(0)) 
	  return BK::AlwaysEqual;
	return BK::AlwaysLess;
      };


    case BK::AbsolutelyVolatile:
      COP("case BK::AbsolutelyVolatile");
      ASSERT(weightComparison.positivePart());
      ASSERT(weightComparison.negativePart()); 
      // First, collect the instance of the positive part
      _weightPositivePartInstance.reset();
      for (WeightPolynomial::Stored* mem = weightComparison.positivePart();
	   mem;
	   mem = mem->tl())
	{
	  if (mem->hd().isConstant())
	    {
	      _weightPositivePartInstance.add(mem->hd().constant());
	    }
	  else // monomial
	    {
	      ASSERT(mem->hd().isVariable() && (mem->hd().coeff() > static_cast<TermWeightType>(0)));
	      TermWeightType coeff = mem->hd().coeff();
	      const Flatterm* varInstance = _subst->map(mem->hd().var());
	      if (varInstance->isVariable())
		{    
		  _weightPositivePartInstance.add(coeff,varInstance->Var());
		}
	      else
		{
		  TermWeighting::current()->collectWeight(_weightPositivePartInstance,
							  coeff,
							  varInstance);
		};
	    }; 
	};
      // The instance of the positive part collected.
      // Now, collect the instance of the negative part. 
      {
	
	_weightNegativePartInstance.reset();
	WeightPolynomial::Stored* mem = weightComparison.negativePart();
      next_member:
	ASSERT(mem);
	if (mem->hd().isConstant())
	  {
	    _weightNegativePartInstance.add(mem->hd().constant());
	  }
	else // monomial
	  {
	    ASSERT(mem->hd().isVariable() && (mem->hd().coeff() > static_cast<TermWeightType>(0)));
	    TermWeightType coeff = mem->hd().coeff();
	    const Flatterm* varInstance = _subst->map(mem->hd().var());
	    if (varInstance->isVariable())
	      {    
		_weightNegativePartInstance.add(coeff,varInstance->Var());
	      }
	    else
	      {
		TermWeighting::current()->collectWeight(_weightNegativePartInstance,
							coeff,
							varInstance);
	      };
	    
	  };

	mem = mem->tl();
	if (mem) 
	  {
	    if (_weightPositivePartInstance.isNeverGreater(_weightNegativePartInstance))
	      return BK::AlwaysLess;
	    goto next_member;
	  }
	else
	  {
	    // The instance of the negative part collected.
	    FunctionComparison cmp =
	      _weightPositivePartInstance.compareForLessOrEqual(_weightNegativePartInstance);
	    switch (cmp)
	      {
	      case BK::FailureToCompare:
		return BK::FailureToCompare;		
	      case BK::AlwaysLess:
		return BK::AlwaysLess;
	      case BK::AlwaysEqual:
		return BK::AlwaysEqual;
	      case BK::AlwaysGreater:
		ICP("ICP10");
		return BK::FailureToCompare;
	      case BK::CanBeLessOrEqual:
		return BK::CanBeLessOrEqual;
	      case BK::CanBeGreaterOrEqual:
		// same as below
	      case BK::VolatileButNearlyAlwaysLess:
		// same as below
	      case BK::VolatileButNearlyAlwaysGreater:
		// same as below
	      case BK::AbsolutelyVolatile:
		ICP("ICP20");
		return BK::FailureToCompare;		
	      };
	  };	
      };
    }; // switch (weightComparison.category())

  ICP("END");
#if (defined _SUPPRESS_WARNINGS_) || (defined DEBUG_NAMESPACE) 
  return BK::FailureToCompare; 
#endif

}; // FunctionComparison NonrecursiveKBOrdering::ConstraintCheckOnForwardMatchingSubst::checkLessOrEqual(const WeightPolynomial::SpecialisedComparison::Stored& weightComparison)




inline
bool NonrecursiveKBOrdering::ConstraintCheckOnForwardMatchingSubst::lexicorgaphicallyGreater(const TERM& lhs,const TERM& rhs)
{
  CALL("lexicorgaphicallyGreater(const TERM& lhs,const TERM& rhs)");
  _lhsInstance.reset(lhs,_subst);
  _rhsInstance.reset(rhs,_subst);
  do
    {
      if (_lhsInstance.symbol() != _rhsInstance.symbol())
	{
	  if (_lhsInstance.symbol().isVariable() || _rhsInstance.symbol().isVariable())
	    return false; // instances are incomparable
	  return _lhsInstance.symbol().hasGreaterPrecedenceThan(_rhsInstance.symbol());
	};
      _rhsInstance.next();
    }
  while (_lhsInstance.next());
  return false; // equal instances
}; // bool NonrecursiveKBOrdering::ConstraintCheckOnForwardMatchingSubst::lexicorgaphicallyGreater(const TERM& lhs,const TERM& rhs)

inline
bool NonrecursiveKBOrdering::ConstraintCheckOnForwardMatchingSubst::lexicorgaphicallyGreaterOrEqual(const TERM& lhs,const TERM& rhs)
{
  CALL("lexicorgaphicallyGreaterOrEqual(const TERM& lhs,const TERM& rhs)");
  _lhsInstance.reset(lhs,_subst);
  _rhsInstance.reset(rhs,_subst);
  do
    {
      if (_lhsInstance.symbol() != _rhsInstance.symbol())
	{
	  if (_lhsInstance.symbol().isVariable() || _rhsInstance.symbol().isVariable())
	    return false; // instances are incomparable
	  return _lhsInstance.symbol().hasGreaterPrecedenceThan(_rhsInstance.symbol());
	};
      _rhsInstance.next();
    }
  while (_lhsInstance.next());
  return true; // equal instances
}; // bool NonrecursiveKBOrdering::ConstraintCheckOnForwardMatchingSubst::lexicorgaphicallyGreaterOrEqual(const TERM& lhs,const TERM& rhs)

inline
bool 
NonrecursiveKBOrdering::ConstraintCheckOnForwardMatchingSubst::lexicorgaphicallyLess(const TERM& lhs,const TERM& rhs)
{
  return lexicorgaphicallyGreater(rhs,lhs);
};

inline
bool 
NonrecursiveKBOrdering::ConstraintCheckOnForwardMatchingSubst::lexicorgaphicallyLessOrEqual(const TERM& lhs,const TERM& rhs)
{
  return lexicorgaphicallyGreaterOrEqual(rhs,lhs);
};



//===================================================================
#undef DEBUG_NAMESPACE
#ifdef DEBUG_NONRECURSIVE_KB_ORDERING
#define DEBUG_NAMESPACE "NonrecursiveKBOrdering::CompiledConstraintForUnifiers"
#endif
#include "debugMacros.hpp"
//===================================================================


NonrecursiveKBOrdering::CompiledConstraintForUnifiers::~CompiledConstraintForUnifiers() 
{
};  

void NonrecursiveKBOrdering::CompiledConstraintForUnifiers::load(PrefixSym* lhs,PrefixSym* rhs)
{
  CALL("load(PrefixSym* lhs,PrefixSym* rhs)");
  INTERCEPT_BACKWARD_CONSTR_CHECK_BEGIN1;
  ASSERT(lhs->Head().IsFunctor());
  ASSERT(rhs->Head().IsFunctor());
  _lhs = lhs;
  _rhs = rhs;
  _lhsWeight.reset();
  _rhsWeight.reset();
  TermWeighting::current()->collectWeight(_lhsWeight,static_cast<TermWeightType>(1),_lhs);
  TermWeighting::current()->collectWeight(_rhsWeight,static_cast<TermWeightType>(1),_rhs);    
  INTERCEPT_BACKWARD_CONSTR_CHECK_END1;
}; // void NonrecursiveKBOrdering::CompiledConstraintForUnifiers::load(PrefixSym* lhs,PrefixSym* rhs)

bool NonrecursiveKBOrdering::CompiledConstraintForUnifiers::lhsVarInstMayOrder()
{
  CALL("lhsVarInstMayOrder()");
  return !_rhsWeight.containsVariableWhichIsNotIn(_lhsWeight);
}; // bool NonrecursiveKBOrdering::CompiledConstraintForUnifiers::lhsVarInstMayOrder()

 
bool NonrecursiveKBOrdering::CompiledConstraintForUnifiers::rhsVarInstMayOrder()
{
  CALL("rhsVarInstMayOrder()");
  return !_lhsWeight.containsVariableWhichIsNotIn(_rhsWeight);
}; // bool NonrecursiveKBOrdering::CompiledConstraintForUnifiers::rhsVarInstMayOrder()

void NonrecursiveKBOrdering::CompiledConstraintForUnifiers::compileGreaterOrEqual()
{
  CALL("compileGreaterOrEqual()");
  
  INTERCEPT_BACKWARD_CONSTR_CHECK_COMPILATION_CALL;
  INTERCEPT_BACKWARD_CONSTR_CHECK_BEGIN1;

#ifndef NO_DEBUG
  _targetRelation = GreaterOrEqual;
#endif
  _weightComparison.compare(_lhsWeight,_rhsWeight);
  Command* command = _mainCode.memory();
  bool compileWeightComparison = false;
  switch (_weightComparison.category())
    {
    case FailureToCompare:
      command->setTag(Command::RetFailure);
      INTERCEPT_BACKWARD_CONSTR_CHECK_END1;
      return;
    case AlwaysLess:
      command->setTag(Command::RetFailure);
      INTERCEPT_BACKWARD_CONSTR_CHECK_END1;
      return;
    case AlwaysEqual:
      break;
    case AlwaysGreater:
      command->setTag(Command::RetSuccess);
      INTERCEPT_BACKWARD_CONSTR_CHECK_END1;
      return;
    case CanBeLessOrEqual:
      compileWeightComparison = true;
      break;
    case CanBeGreaterOrEqual:
      compileWeightComparison = true;
      break;
    case VolatileButNearlyAlwaysLess:
      compileWeightComparison = true;
      break;
    case VolatileButNearlyAlwaysGreater:
      compileWeightComparison = true;
      break;
    case AbsolutelyVolatile:
      compileWeightComparison = true;
      break;
    };

  if (compileWeightComparison)
    {

      if (!_weightComparisonCode.size())
	{
	  DOP(_weightComparisonCode.unfreeze());
	  _weightComparisonCode.expand();
	  DOP(_weightComparisonCode.freeze());
	};
      WeightComparisonCommand* endOfWeightCode = 
	_weightComparisonCode.memory();
      while (!_weightComparison.compileGreaterOrEqual(endOfWeightCode,
						      _weightComparisonCode.size()))
	{
	  DOP(_weightComparisonCode.unfreeze());
	  _weightComparisonCode.expand();
	  DOP(_weightComparisonCode.freeze());
	  endOfWeightCode = _weightComparisonCode.memory();
	};      
      // naturalise the variables
      for (WeightComparisonCommand* c = _weightComparisonCode.memory();
	   c < endOfWeightCode;
	   ++c)
	if (c->requiresVar())
	  {
	    c->setNativeVar(static_cast<void*>(Unifier::current()->variable(c->var())));
	  };

    };
  
  // main code
 
  ulong maxSize;

 compile_main:
  command = _mainCode.memory();
  maxSize = _mainCode.size();
  if (compileWeightComparison)
    {
      command->setTag(Command::IfWCFailRetFail_IfWCAlwGrRetSucc);
      ++command;
      --maxSize;
    };
   
  if (!compileLexGreaterOrEqual(command,maxSize))
    {
      DOP(_mainCode.unfreeze());
      _mainCode.expand();
      DOP(_mainCode.freeze());
      goto compile_main;
    };
  
  INTERCEPT_BACKWARD_CONSTR_CHECK_END1;

  /***********
	      DF; cout << "CHECK: ";
	      DF; Prefix::outputTerm(cout,_lhs) << " >= ";
	      DF; Prefix::outputTerm(cout,_rhs) << "\n";
	      DF; cout << "WEIGHT COMP:\n" << _weightComparison << "\n";
	      DF; output(cout << "\n\n") << "\n\n";
  ************/

}; // void NonrecursiveKBOrdering::CompiledConstraintForUnifiers::compileGreaterOrEqual()


bool NonrecursiveKBOrdering::CompiledConstraintForUnifiers::holdsOnCurrentUnifier()
{
  CALL("holdsOnCurrentUnifier()");

  //DF; output(cout << "\n\n\nHOLDS?\n") << "\n\n";   
  //DF; Unifier::current()->output(cout << "SUBST ",Prefix::outputTerm) << "\n\n\n";  
#ifdef MEASURE_BACKWARD_CONSTR_CHECK
  INTERCEPT_BACKWARD_CONSTR_CHECK_BEGIN2;
  SimplificationOrdering::current()->greaterOrEqualWithCurrentUnifier(_lhs,_rhs);
  INTERCEPT_BACKWARD_CONSTR_CHECK_END2;
#endif

  INTERCEPT_BACKWARD_CONSTR_CHECK_NEW_CHECK;
  INTERCEPT_BACKWARD_CONSTR_CHECK_BEGIN1;

  Command* command = _mainCode.memory();
  Comparison lexCmp;
  FunctionComparison weightComp;
  
 next_command:
  switch (command->tag())
    {
    case Command::RetSuccess:
      goto return_true; 
    case Command::RetFailure:
      goto return_false;

    case Command::IfWCFailRetFail_IfWCAlwGrRetSucc:
      /*
	if WC(@) = FailureToCompare return Failure;
	if WC(@) = AlwaysGreater return Success;
	ASSERT(WC(@) = AlwaysEqual || WC(@) = CanBeGreaterOrEqual);
      */

      weightComp = interpretWeightComparisonCode();

      //DF; cout << "\n\n\nWEIGHT COMPARISON RETURNS: " << weightComp << "\n\n\n"; 

      if (weightComp == BK::FailureToCompare) goto return_false;
      if (weightComp == BK::AlwaysGreater) goto return_true;
      ASSERT((weightComp == BK::CanBeGreaterOrEqual) || (weightComp == BK::AlwaysEqual));
      ++command;
      goto next_command;
	
    case Command::IfLexSTEqIncRetFail_IfLexSTEqLeRetFail_RetSucc:
      /*
	if lex(S@,T@) = Incomparable return Failure;
	if lex(S@,T@) = Less return Failure;
	return Success;
      */
      lexCmp = compareLexicographically(command->complexTerm1(),command->complexTerm2());
      if ((lexCmp == BK::Greater) || (lexCmp == BK::Equal)) goto return_true;
      goto return_false;
	
    case Command::IfLexXYEqIncRetFail_IfLexXYEqLeRetFail_IfLexXYEqGrRetSucc:
      /*
	if lex(X@,Y@) = Incomparable return Failure;
	if lex(X@,Y@) = Less return Failure;
	if lex(X@,Y@) = Greater return Success;
	ASSERT(lex(X@,Y@) = Equal);
      */
      lexCmp = compareLexicographically(command->var1(),command->var2());
      if (lexCmp == BK::Greater) goto return_true;
      if (lexCmp != BK::Equal) goto return_false;
      ++command;
      goto next_command;

    case Command::IfLexXTEqIncRetFail_IfLexXTEqLeRetFail_IfLexXTEqGrRetSucc:
      /*
	if lex(X@,T@) = Incomparable return Failure;
	if lex(X@,T@) = Less return Failure;
	if lex(X@,T@) = Greater return Success;
	ASSERT(lex(X@,T@) = Equal);
      */
      lexCmp = compareLexicographically(command->var1(),command->complexTerm2());
      if (lexCmp == BK::Greater) goto return_true;
      if (lexCmp != BK::Equal) goto return_false;
      ++command;
      goto next_command;
	
    case Command::IfLexSYEqIncRetFail_IfLexSYEqLeRetFail_IfLexTYEqGrRetSucc:
      /*
	if lex(S@,Y@) = Incomparable return Failure;
	if lex(S@,Y@) = Less return Failure;
	if lex(S@,Y@) = Greater return Success;
	ASSERT(lex(S@,Y@) = Equal);
      */
      lexCmp = compareLexicographically(command->var2(),command->complexTerm1());
      if (lexCmp == BK::Less) goto return_true;
      if (lexCmp != BK::Equal) goto return_false;
      ++command;
      goto next_command;
      
#ifdef DEBUG_NAMESPACE
    default: ICP("ICP10"); goto return_false;
#else
#ifdef _SUPPRESS_WARNINGS_
    default: goto return_false;
#endif
#endif
    };
  
 return_true:

  ASSERT((_targetRelation != GreaterOrEqual) || SimplificationOrdering::current()->greaterOrEqualWithCurrentUnifier(_lhs,_rhs));
  INTERCEPT_BACKWARD_CONSTR_CHECK_END1;
  return true;

 return_false:
  // v Very strong assumption!  
  ASSERT((_targetRelation != GreaterOrEqual) || (!SimplificationOrdering::current()->greaterOrEqualWithCurrentUnifier(_lhs,_rhs)));
  INTERCEPT_BACKWARD_CONSTR_CHECK_END1;
  return false;
}; // bool NonrecursiveKBOrdering::CompiledConstraintForUnifiers::holdsOnCurrentUnifier()




#ifndef NO_DEBUG_VIS
ostream& NonrecursiveKBOrdering::CompiledConstraintForUnifiers::output(ostream& str) const
{
  Prefix::outputTerm(str << "CONSTRAINT: ",_lhs);
  switch (_targetRelation)
    {
    case GreaterOrEqual: str << " >= "; break;
    default: str << "???";
    };
  Prefix::outputTerm(str,_rhs) << "\n";
  DF; cout << "WEIGHT COMPARISON:\n" << _weightComparison << "\n";
  str << "********* MAIN CODE: *********************\n";
  const Command* command = _mainCode.memory();
  bool printWC = false;
 print_command:
  command->output(str);
  printWC = printWC || command->requiresWeightComparison();
  if (!command->isTerminal())
    {
      ++command;
      goto print_command;
    };

  if (printWC)
    {
      str << "********* WEIGHT COMPARISON CODE: ********\n";
      WeightPolynomial::SpecialisedComparison::Command::outputCode(str,_weightComparisonCode.memory()) 
	<< "\n";
    };
  str << "*********** END OF CODE ********\n";
  return str;
};
#endif



bool NonrecursiveKBOrdering::CompiledConstraintForUnifiers::compileLexGreaterOrEqual(Command*& code,ulong maxCodeSize)
{
  CALL("compileLexGreaterOrEqual(Command*& code,ulong maxCodeSize)");
  ASSERT(maxCodeSize);
  ASSERT(_lhs->Head().IsFunctor());
  ASSERT(_rhs->Head().IsFunctor());
  _lhsIter.Reset(_lhs);
  _rhsIter.Reset(_rhs);
  Command* codeStart = code;
  do
    {

      ASSERT(_rhsIter.NotEmpty());

      if ((code - codeStart) + 1 == (long)maxCodeSize)
	{
	  // enough space for one command only
	  code->setTag(Command::IfLexSTEqIncRetFail_IfLexSTEqLeRetFail_RetSucc);
	  code->setComplexTerm1(_lhs);
	  code->setComplexTerm2(_rhs);
	  ++code;
	  return false;
	};

      ASSERT((code - codeStart) + 1 < (long)maxCodeSize);

      Unifier::Variable* lhsVar = _lhsIter.CurrVar();
      Unifier::Variable* rhsVar = _rhsIter.CurrVar();
      if (lhsVar)
	{
	  if (rhsVar)
	    {
	      if (lhsVar != rhsVar)
		{
		  code->setTag(Command::IfLexXYEqIncRetFail_IfLexXYEqLeRetFail_IfLexXYEqGrRetSucc);
		  code->setVar1(lhsVar);
		  code->setVar2(rhsVar);
		  ++code;  
		};
	      _lhsIter.SkipSym();
	      _rhsIter.SkipSym();

	    }
	  else // lhsVar, but !rhsVar
	    {
	      code->setTag(Command::IfLexXTEqIncRetFail_IfLexXTEqLeRetFail_IfLexXTEqGrRetSucc);
	      code->setVar1(lhsVar);
	      ASSERT(_rhsIter.CurrSubterm()->Head().IsFunctor());
	      code->setComplexTerm2(_rhsIter.CurrSubterm());
	      ++code;

	      _lhsIter.SkipSym();
	      _rhsIter.SkipTerm();
	    };
	}
      else
	if (rhsVar)
	  {
	    // !lhsVar, but rhsVar
	    code->setTag(Command::IfLexSYEqIncRetFail_IfLexSYEqLeRetFail_IfLexTYEqGrRetSucc);
	    ASSERT(_lhsIter.CurrSubterm()->Head().IsFunctor());
	    code->setComplexTerm1(_lhsIter.CurrSubterm());
	    code->setVar2(rhsVar);
	    ++code;
	    _lhsIter.SkipTerm();
	    _rhsIter.SkipSym(); 
	  }
	else // !lhsVar and !rhsVar
	  {	    
	    ASSERT(_lhsIter.CurrSubterm()->Head().IsFunctor());
	    ASSERT(_rhsIter.CurrSubterm()->Head().IsFunctor());
	    if (_lhsIter.CurrSubterm()->Head() != _rhsIter.CurrSubterm()->Head())
	      {	       
		// stop here completely
		if (_lhsIter.CurrSubterm()->Head().Func().hasGreaterPrecedenceThan(_rhsIter.CurrSubterm()->Head().Func()))
		  {
		    code->setTag(Command::RetSuccess);
		    ++code;
		  }
		else
		  {
		    code->setTag(Command::RetFailure);
		    ++code;
		  };
		return true;
	      }
	    else
	      {
		_lhsIter.SkipSym();
		_rhsIter.SkipSym();
	      };
	  };

    }
  while (_lhsIter.NotEmpty());
  ASSERT(_rhsIter.Empty());
  ASSERT((code - codeStart) + 1 <= (long)maxCodeSize);
  code->setTag(Command::RetSuccess); // equal instances
  ++code;
  return true;
}; // bool NonrecursiveKBOrdering::CompiledConstraintForUnifiers::compileLexGreaterOrEqual(Command*& code,ulong maxCodeSize)


inline 
Comparison 
NonrecursiveKBOrdering::CompiledConstraintForUnifiers::compareLexicographically(PrefixSym* term1,PrefixSym* term2)
{
  CALL("compareLexicographically(PrefixSym* term1,PrefixSym* term2)");
  ASSERT(term1->Head().IsFunctor());
  ASSERT(term2->Head().IsFunctor());
  _lhsIter.Reset(term1);
  _rhsIter.Reset(term2);
  do
    {      
      Unifier::Variable* lhsVar = _lhsIter.CurrVar();
      Unifier::Variable* rhsVar = _rhsIter.CurrVar();
      if (lhsVar)
	{
	  if (rhsVar)
	    {
	      if (lhsVar != rhsVar)
		{
		  return BK::Incomparable;
		};
	      _lhsIter.SkipSym();
	      _rhsIter.SkipSym();

	    }
	  else // lhsVar, but !rhsVar
	    {
	      return BK::Incomparable;
	    };
	}
      else
	if (rhsVar)
	  {
	    // !lhsVar, but rhsVar
	    return BK::Incomparable;
	  }
	else // !lhsVar and !rhsVar
	  {	    
	    ASSERT(_lhsIter.CurrSubterm()->Head().IsFunctor());
	    ASSERT(_rhsIter.CurrSubterm()->Head().IsFunctor());
	    if (_lhsIter.CurrSubterm()->Head() != _rhsIter.CurrSubterm()->Head())
	      {	 
		return
		  _lhsIter.CurrSubterm()->Head().Func().comparePrecedence(_rhsIter.CurrSubterm()->Head().Func());
	      }
	    else
	      {
		_lhsIter.SkipSym();
		_rhsIter.SkipSym();
	      };
	  };

    }
  while (_lhsIter.NotEmpty());
  ASSERT(_rhsIter.Empty());
  return BK::Equal;
}; // Comparison NonrecursiveKBOrdering::CompiledConstraintForUnifiers::compareLexicographically(PrefixSym* term1,PrefixSym* term2)


inline 
Comparison 
NonrecursiveKBOrdering::CompiledConstraintForUnifiers::compareLexicographically(Unifier::Variable* var,PrefixSym* term)
{
  CALL("compareLexicographically(Unifier::Variable* var,PrefixSym* term)");
  ASSERT(term->Head().IsFunctor());
  bool freeVar;
  void* instance = var->Unref(freeVar);
  if (freeVar) return BK::Incomparable;
  return compareLexicographically(static_cast<PrefixSym*>(instance),term);
}; // Comparison NonrecursiveKBOrdering::CompiledConstraintForUnifiers::compareLexicographically(Unifier::Variable* var,PrefixSym* term)

inline
Comparison NonrecursiveKBOrdering::CompiledConstraintForUnifiers::compareLexicographically(Unifier::Variable* var1,Unifier::Variable* var2)
{
  CALL("compareLexicographically(Unifier::Variable* var1,Unifier::Variable* var2)");
  
  bool freeVar1;
  void* instance1 = var1->Unref(freeVar1);
  bool freeVar2;
  void* instance2 = var2->Unref(freeVar2);
  if (freeVar1)
    {
      if ((freeVar2) && (instance1 == instance2)) return BK::Equal;
      return BK::Incomparable;
    }
  else
    {
      if (freeVar2) return BK::Incomparable;
      return compareLexicographically(static_cast<PrefixSym*>(instance1),static_cast<PrefixSym*>(instance2));
    };

}; // Comparison NonrecursiveKBOrdering::CompiledConstraintForUnifiers::compareLexicographically(Unifier::Variable* var1,Unifier::Variable* var2)


inline
FunctionComparison 
NonrecursiveKBOrdering::CompiledConstraintForUnifiers::interpretWeightComparisonCode()
{
  CALL("interpretWeightComparisonCode()");
  const WeightComparisonCommand* command = _weightComparisonCode.memory();

 next_command:

  //DF; command->outputShort(cout << "DO ") << "\n";

  switch (command->tag())
    {
      
    case CompiledConstraintForUnifiers::WeightComparisonCommand::RetFailureToCompare: 
      return BK::FailureToCompare;
    case CompiledConstraintForUnifiers::WeightComparisonCommand::RetAlwaysLess: ICP("ICP10"); return BK::FailureToCompare;
    case CompiledConstraintForUnifiers::WeightComparisonCommand::RetAlwaysEqual: return BK::AlwaysEqual;
    case CompiledConstraintForUnifiers::WeightComparisonCommand::RetAlwaysGreater: return BK::AlwaysGreater;
    case CompiledConstraintForUnifiers::WeightComparisonCommand::RetCanBeLessOrEqual: ICP("ICP20"); return BK::FailureToCompare;
    case CompiledConstraintForUnifiers::WeightComparisonCommand::RetCanBeGreaterOrEqual: return BK::CanBeGreaterOrEqual;
    case CompiledConstraintForUnifiers::WeightComparisonCommand::RetVolatileButNearlyAlwaysLess: ICP("ICP30");  return BK::FailureToCompare;
    case CompiledConstraintForUnifiers::WeightComparisonCommand::RetVolatileButNearlyAlwaysGreater: ICP("ICP40"); return BK::FailureToCompare;
    case CompiledConstraintForUnifiers::WeightComparisonCommand::RetAbsolutelyVolatile: ICP("ICP50"); return BK::FailureToCompare;

      //----------------------------------------

    case CompiledConstraintForUnifiers::WeightComparisonCommand::IfXGr1RetFail_IfGrndRetAlwEq_RetFail:
      /*
	if min|X@| > 1 return FailureToCompare;
	if ground(X@) return AlwaysEqual;
	return FailureToCompare;
      */
      {
	bool freeVar;
	const void* instance = 
	  (static_cast<const Unifier::Variable*>(command->nativeVar()))->Unref(freeVar);
	if (freeVar) return BK::FailureToCompare; // nonground
	TermWeightType w = 
	  TermWeighting::current()->computeWeightIfGroundAndLessOrEqual(static_cast<const PrefixSym*>(instance),static_cast<TermWeightType>(1));
	if (w <= 0) return BK::FailureToCompare;
	return BK::AlwaysEqual;
      };

    case CompiledConstraintForUnifiers::WeightComparisonCommand::IfXGr1RetFail_IfNotGrndRetFail:
      /*
	if min|X@| > 1 return FailureToCompare;
	if !ground(X@) return FailureToCompare;
      */
      {
	bool freeVar;
	const void* instance = (static_cast<const Unifier::Variable*>(command->nativeVar()))->Unref(freeVar);
	if (freeVar) return BK::FailureToCompare; // nonground
	TermWeightType w = TermWeighting::current()->computeWeightIfGroundAndLessOrEqual(static_cast<const PrefixSym*>(instance),static_cast<TermWeightType>(1));
	if (w <= 0) return BK::FailureToCompare;
      };
      break;


      //----------------------------------------



    case CompiledConstraintForUnifiers::WeightComparisonCommand::IfXGr1RetAlwGr_IfGrndRetAlwEq_RetCanBeGrOrEq:
      /*
	if min|X@| > 1 return AlwaysGreater;
	if ground(X@) return AlwaysEqual;
	return CanBeGreaterOrEqual;
      */
      {
	bool freeVar;
	const void* instance = (static_cast<const Unifier::Variable*>(command->nativeVar()))->Unref(freeVar);
	if (freeVar) return BK::CanBeGreaterOrEqual;
	
	bool ground;
	TermWeightType w = TermWeighting::current()->computeMinimalInstanceWeightIfLessOrEqual(static_cast<const PrefixSym*>(instance),
													    static_cast<TermWeightType>(1),
													    ground);
	if (w < static_cast<TermWeightType>(0)) return BK::AlwaysGreater;
	ASSERT(w == static_cast<TermWeightType>(1));
	if (ground) return BK::AlwaysEqual;
	return BK::CanBeGreaterOrEqual;
      };

    case CompiledConstraintForUnifiers::WeightComparisonCommand::IfXGr1RetAlwGr_GrndToGRND:
      /*
	if min|X@| > 1 return AlwaysGreater;
	GRND := ground(X@);
      */
      {
	bool freeVar;
	const void* instance = (static_cast<const Unifier::Variable*>(command->nativeVar()))->Unref(freeVar);
	if (freeVar)
	  {
	    _grnd = false;
	  }
	else
	  {
	    bool ground;
	    TermWeightType w = TermWeighting::current()->computeMinimalInstanceWeightIfLessOrEqual(static_cast<const PrefixSym*>(instance),
														static_cast<TermWeightType>(1),
														ground);
	    if (w < static_cast<TermWeightType>(0)) return BK::AlwaysGreater;
	    _grnd = ground;
	  };
      };
      break;
      

    case CompiledConstraintForUnifiers::WeightComparisonCommand::IfXGr1RetAlwGr_UpdateGRND:
      /*
	if min|X@| > 1 return AlwaysGreater;
	GRND := GRND && ground(X@);
      */
      {
	bool freeVar;
	const void* instance = (static_cast<const Unifier::Variable*>(command->nativeVar()))->Unref(freeVar);
	if (freeVar)
	  {
	    _grnd = false;
	  }
	else
	  {
	    bool ground;
	    TermWeightType w = TermWeighting::current()->computeMinimalInstanceWeightIfLessOrEqual(static_cast<const PrefixSym*>(instance),
														static_cast<TermWeightType>(1),
														ground);
	    if (w < static_cast<TermWeightType>(0)) return BK::AlwaysGreater;
	    _grnd = _grnd && ground;
	  };
      };
      break;

    case CompiledConstraintForUnifiers::WeightComparisonCommand::IfXGr1RetAlwGr_UpdateGRND_IfGRNDRetAlwEq_RetCanBeGrOrEq:
      /*
	if min|X@| > 1 return AlwaysGreater;
	GRND := GRND && ground(X@);
	if GRND return AlwaysEqual;
	return CanBeGreaterOrEqual;
      */
      {
	bool freeVar;
	const void* instance = (static_cast<const Unifier::Variable*>(command->nativeVar()))->Unref(freeVar);
	if (freeVar)
	  {
	    return BK::CanBeGreaterOrEqual;
	  }
	else
	  {
	    bool ground;
	    TermWeightType w = TermWeighting::current()->computeMinimalInstanceWeightIfLessOrEqual(static_cast<const PrefixSym*>(instance),
														static_cast<TermWeightType>(1),
														ground);
	    if (w < static_cast<TermWeightType>(0)) return BK::AlwaysGreater;
	    _grnd = _grnd && ground;
	    if (_grnd) return BK::AlwaysEqual;
	    return BK::CanBeGreaterOrEqual;
	  };
      };



      //----------------------------------------


    case CompiledConstraintForUnifiers::WeightComparisonCommand::IfNotGrndRetFail_IfXGrWRetFail_IfXLeWRetAlwGr_RetAlwEq:
      /*
	if !ground(X@) return FailureToCompare;
	if min|X@| > W return FailureToCompare;
	if min|X@| < W return AlwaysGreater;
	return AlwaysEqual;
      */
      {
	bool freeVar;
	const void* instance = (static_cast<const Unifier::Variable*>(command->nativeVar()))->Unref(freeVar);
	if (freeVar)
	  {
	    return BK::FailureToCompare;
	  }
	else
	  {	    
	    TermWeightType w = TermWeighting::current()->computeWeightIfGroundAndLessOrEqual(static_cast<const PrefixSym*>(instance),
													  command->weight());
	    if (w <= static_cast<TermWeightType>(0)) return BK::FailureToCompare;
	    if (w < command->weight()) 
	      {
		return BK::AlwaysGreater;
	      }
	    else
	      {
		ASSERT(w == command->weight());
		return BK::AlwaysEqual;
	      };
	    
	  };
      };


    case CompiledConstraintForUnifiers::WeightComparisonCommand::IfNotGrndRetFail_IfCXGrWRetFail_IfCXLeWRetAlwGr_RetAlwEq:
      /*
	if !ground(X@) return FailureToCompare;
	if C*min|X@| > W return FailureToCompare;
	if C*min|X@| < W return AlwaysGreater;
	return AlwaysEqual;
      */
      {
	bool freeVar;
	const void* instance = (static_cast<const Unifier::Variable*>(command->nativeVar()))->Unref(freeVar);
	if (freeVar)
	  {
	    return BK::FailureToCompare;
	  }
	else
	  {	    
	    TermWeightType w = TermWeighting::current()->computeWeightIfGroundAndLessOrEqual(command->coefficient(),
													  static_cast<const PrefixSym*>(instance),
													  command->weight());
	    if (w <= static_cast<TermWeightType>(0)) return BK::FailureToCompare;
	    if (w < command->weight()) 
	      {
		return BK::AlwaysGreater;
	      }
	    else
	      {
		ASSERT(w == command->weight());
		return BK::AlwaysEqual;
	      };
	    
	  };
      };



    case CompiledConstraintForUnifiers::WeightComparisonCommand::IfNotGrndRetFail_WMinusXToACC_IfACCLe0RetFail:
      /*
	if !ground(X@) return FailureToCompare;
	ACC := W - min|X@|;
	if ACC < 0 return FailureToCompare;      
      */
      {
	bool freeVar;
	const void* instance = (static_cast<const Unifier::Variable*>(command->nativeVar()))->Unref(freeVar);
	if (freeVar)
	  {
	    return BK::FailureToCompare;
	  }
	else
	  {	
	    TermWeightType w = TermWeighting::current()->computeWeightIfGroundAndLessOrEqual(static_cast<const PrefixSym*>(instance),
													  command->weight());
	    if (w <= static_cast<TermWeightType>(0)) return BK::FailureToCompare;
	    _acc = command->weight() - w;
	    ASSERT(_acc >= static_cast<TermWeightType>(0));
	  };
      };
      break;



    case CompiledConstraintForUnifiers::WeightComparisonCommand::IfNotGrndRetFail_WMinusCXToACC_IfACCLe0RetFail:
      /*
	if !ground(X@) return FailureToCompare;
	ACC := W - C*min|X@|;
	if ACC < 0 return FailureToCompare;      
      */
      {
	bool freeVar;
	const void* instance = (static_cast<const Unifier::Variable*>(command->nativeVar()))->Unref(freeVar);
	if (freeVar)
	  {
	    return BK::FailureToCompare;
	  }
	else
	  {	
	    TermWeightType w = TermWeighting::current()->computeWeightIfGroundAndLessOrEqual(command->coefficient(),
													  static_cast<const PrefixSym*>(instance),
													  command->weight());
	    if (w <= static_cast<TermWeightType>(0)) return BK::FailureToCompare;
	    _acc = command->weight() - w;
	    ASSERT(_acc >= static_cast<TermWeightType>(0));
	  };
      };
      break;


    case CompiledConstraintForUnifiers::WeightComparisonCommand::IfNotGrndRetFail_ACCMinusXPlus1ToACC_IfACCLe0RetFail:
      /*
	if !ground(X@) return FailureToCompare;
	ACC := ACC - min|X@| + 1;
	if ACC < 0 return FailureToCompare;
      */    
      {
	bool freeVar;
	const void* instance = (static_cast<const Unifier::Variable*>(command->nativeVar()))->Unref(freeVar);
	if (freeVar)
	  {
	    return BK::FailureToCompare;
	  }
	else
	  {	
	    ++_acc;
	    TermWeightType w = TermWeighting::current()->computeWeightIfGroundAndLessOrEqual(static_cast<const PrefixSym*>(instance),
													  _acc);
	    if (w <= static_cast<TermWeightType>(0)) return BK::FailureToCompare;
	    _acc -= w;
	    ASSERT(_acc >= static_cast<TermWeightType>(0));
	  };
      };
      break;
    

    case CompiledConstraintForUnifiers::WeightComparisonCommand::IfNotGrndRetFail_ACCMinusCXPlusCToACC_IfACCLe0RetFail:
      /*
	if !ground(X@) return FailureToCompare;
	ACC := ACC - C*min|X@| + C;
	if ACC < 0 return FailureToCompare; 
      */    
      {
	bool freeVar;
	const void* instance = (static_cast<const Unifier::Variable*>(command->nativeVar()))->Unref(freeVar);
	if (freeVar)
	  {
	    return BK::FailureToCompare;
	  }
	else
	  {	
	    _acc += command->coefficient();
	    TermWeightType w = TermWeighting::current()->computeWeightIfGroundAndLessOrEqual(command->coefficient(),
													  static_cast<const PrefixSym*>(instance),
													  _acc);
	    if (w <= static_cast<TermWeightType>(0)) return BK::FailureToCompare;
	    _acc -= w;
	    ASSERT(_acc >= static_cast<TermWeightType>(0));
	  };
      };
      break;
      


    case CompiledConstraintForUnifiers::WeightComparisonCommand::IfNotGrndRetFail_ACCMinusXPlus1ToACC_IfACCLe0RetFail_IfACCEq0RetAlwEq_RetAlwGr:
      /*
	if !ground(X@) return FailureToCompare;
	ACC := ACC - min|X@| + 1;
	if ACC < 0 return FailureToCompare; 
	if ACC = 0 return AlwaysEqual;
	return AlwaysGreater;
      */    
      {
	bool freeVar;
	const void* instance = (static_cast<const Unifier::Variable*>(command->nativeVar()))->Unref(freeVar);
	if (freeVar)
	  {
	    return BK::FailureToCompare;
	  }
	else
	  {	
	    ++_acc;
	    TermWeightType w = TermWeighting::current()->computeWeightIfGroundAndLessOrEqual(static_cast<const PrefixSym*>(instance),
													  _acc);
	    if (w <= static_cast<TermWeightType>(0)) return BK::FailureToCompare;
	    if (_acc == w) return BK::AlwaysEqual;
	    return BK::AlwaysGreater;
	  };
      };


    case CompiledConstraintForUnifiers::WeightComparisonCommand::IfNotGrndRetFail_ACCMinusCXPlusCToACC_IfACCLe0RetFail_IfACCEq0RetAlwEq_RetAlwGr:
      /*
	if !ground(X@) return FailureToCompare;
	ACC := ACC - C*min|X@| + C;
	if ACC < 0 return FailureToCompare; 
	if ACC = 0 return AlwaysEqual;
	return AlwaysGreater;
      */    
      {
	bool freeVar;
	const void* instance = (static_cast<const Unifier::Variable*>(command->nativeVar()))->Unref(freeVar);
	if (freeVar)
	  {
	    return BK::FailureToCompare;
	  }
	else
	  {	
	    _acc += command->coefficient(); 
	    TermWeightType w = TermWeighting::current()->computeWeightIfGroundAndLessOrEqual(command->coefficient(),
													  static_cast<const PrefixSym*>(instance),
													  _acc);
	    if (w <= static_cast<TermWeightType>(0)) return BK::FailureToCompare;
	    if (_acc == w) return BK::AlwaysEqual;
	    return BK::AlwaysGreater;
	  };
      };


      //----------------------------------------



    case CompiledConstraintForUnifiers::WeightComparisonCommand::IfXGrWRetAlwGr_IfXLeWRetFail_IfGrndRetAlwEq_RetCanBeGrOrEq:
      /*
	ASSERT(W > 0L);
	if min|X@| > W return AlwaysGreater;
	if min|X@| < W return FailureToCompare;
	if ground(X@) return AlwaysEqual;
	return CanBeGreaterOrEqual;
      */
      {
	ASSERT(command->weight() > static_cast<TermWeightType>(0));
	bool freeVar;
	const void* instance = (static_cast<const Unifier::Variable*>(command->nativeVar()))->Unref(freeVar);
	if (freeVar)
	  {
	    // min|X@| == 1, ground(X@) == false
	    if (static_cast<TermWeightType>(1) < command->weight()) return BK::FailureToCompare;
	    return BK::CanBeGreaterOrEqual;
	  }
	else
	  {
	    bool ground;
	    TermWeightType w = 
	      TermWeighting::current()->computeMinimalInstanceWeightIfLessOrEqual(static_cast<const PrefixSym*>(instance),
										  command->weight(),
										  ground);
	    if (w < static_cast<TermWeightType>(0)) return BK::AlwaysGreater;
	    ASSERT((w > static_cast<TermWeightType>(0)) && (w <= command->weight()));
	    if (w < command->weight()) return BK::FailureToCompare;
	    if (ground) return BK::AlwaysEqual;
	    return BK::CanBeGreaterOrEqual;
	  };
      };
      
    
    case CompiledConstraintForUnifiers::WeightComparisonCommand::IfCXGrWRetAlwGr_IfCXLeWRetFail_IfGrndRetAlwEq_RetCanBeGrOrEq:
      /*
	ASSERT(W > static_cast<TermWeightType>(0));
	if C*min|X@| > W return AlwaysGreater;
	if C*min|X@| < W return FailureToCompare;
	if ground(X@) return AlwaysEqual;
	return CanBeGreaterOrEqual;
      */
      {
	ASSERT(command->weight() > static_cast<TermWeightType>(0));
	bool freeVar;
	const void* instance = (static_cast<const Unifier::Variable*>(command->nativeVar()))->Unref(freeVar);
	if (freeVar)
	  {
	    // min|X@| == 1, ground(X@) == false
	    if (command->coefficient() < command->weight()) return BK::FailureToCompare;
	    return BK::CanBeGreaterOrEqual;
	  }
	else
	  {
	    bool ground;
	    TermWeightType w = 
	      TermWeighting::current()->computeMinimalInstanceWeightIfLessOrEqual(command->coefficient(),
										  static_cast<const PrefixSym*>(instance),
										  command->weight(),
										  ground);
	    if (w < static_cast<TermWeightType>(0)) return BK::AlwaysGreater;
	    ASSERT((w > static_cast<TermWeightType>(0)) && (w <= command->weight()));
	    if (w < command->weight()) return BK::FailureToCompare;
	    if (ground) return BK::AlwaysEqual;
	    return BK::CanBeGreaterOrEqual;
	  };
      };


    case CompiledConstraintForUnifiers::WeightComparisonCommand::WMinusXToACC_IfACCLe0RetAlwGr_GrndToGRND:
      /*
	ACC := W - min|X@|;
	if ACC < 0 return AlwaysGreater;
	GRND := ground(X@);
      */
      {
	bool freeVar;
	const void* instance = (static_cast<const Unifier::Variable*>(command->nativeVar()))->Unref(freeVar);
	if (freeVar)
	  {
	    _acc = command->weight() - static_cast<TermWeightType>(1);
	    if (_acc < static_cast<TermWeightType>(0)) return BK::AlwaysGreater;
	    _grnd = false;
	  }
	else
	  {
	    bool ground;
	    TermWeightType w = 
	      TermWeighting::current()->computeMinimalInstanceWeightIfLessOrEqual(static_cast<const PrefixSym*>(instance),
										  command->weight(),
										  ground);
	    if (w < static_cast<TermWeightType>(0)) return BK::AlwaysGreater;
	    _acc = command->weight() - w;
	    _grnd = ground;
	  };
	
      };
      break;

    case CompiledConstraintForUnifiers::WeightComparisonCommand::WMinusCXToACC_IfACCLe0RetAlwGr_GrndToGRND:
      /*
	ACC := W - C*min|X@|;
	if ACC <= 0 return AlwaysGreater;
	GRND := ground(X@);
      */
      {
	bool freeVar;
	const void* instance = (static_cast<const Unifier::Variable*>(command->nativeVar()))->Unref(freeVar);
	if (freeVar)
	  {
	    _acc = command->weight() - command->coefficient();
	    if (_acc < static_cast<TermWeightType>(0)) return BK::AlwaysGreater;
	    _grnd = false;
	  }
	else
	  {
	    bool ground;
	    TermWeightType w = 
	      TermWeighting::current()->computeMinimalInstanceWeightIfLessOrEqual(command->coefficient(),
										  static_cast<const PrefixSym*>(instance),
										  command->weight(),
										  ground);
	    if (w < static_cast<TermWeightType>(0)) return BK::AlwaysGreater;
	    _acc = command->weight() - w;
	    _grnd = ground;
	  };
	
      };
      break;
    
    case CompiledConstraintForUnifiers::WeightComparisonCommand::ACCMinusXPlus1ToACC_IfACCLe0RetAlwGr_UpdateGRND:
      /*
	ACC := ACC - min|X@| + 1;
	if ACC < 0 return AlwaysGreater;
	GRND := GRND && ground(X@);
      */    
      {
	bool freeVar;
	const void* instance = (static_cast<const Unifier::Variable*>(command->nativeVar()))->Unref(freeVar);
	if (freeVar)
	  {
	    if (_acc < static_cast<TermWeightType>(0)) return BK::AlwaysGreater;
	    _grnd = false;
	  }
	else
	  {
	    ++_acc;
	    bool ground;
	    TermWeightType w = 
	      TermWeighting::current()->computeMinimalInstanceWeightIfLessOrEqual(static_cast<const PrefixSym*>(instance),
										  _acc,
										  ground);
	    if (w < static_cast<TermWeightType>(0)) return BK::AlwaysGreater;
	    _acc -= w;
	    ASSERT(_acc >= static_cast<TermWeightType>(0));
	    _grnd = _grnd && ground;
	  };
      };
      break;

    
    case CompiledConstraintForUnifiers::WeightComparisonCommand::ACCMinusCXPlusCToACC_IfACCLe0RetAlwGr_UpdateGRND:
      /*
	ACC := ACC - C*min|X@| + C;
	if ACC < 0 return AlwaysGreater;
	GRND := GRND && ground(X@);
      */    
      {
	bool freeVar;
	const void* instance = (static_cast<const Unifier::Variable*>(command->nativeVar()))->Unref(freeVar);
	if (freeVar)
	  {
	    if (_acc < static_cast<TermWeightType>(0)) return BK::AlwaysGreater;
	    _grnd = false;
	  }
	else
	  {
	    _acc += command->coefficient();
	    bool ground;
	    TermWeightType w = 
	      TermWeighting::current()->computeMinimalInstanceWeightIfLessOrEqual(command->coefficient(),
										  static_cast<const PrefixSym*>(instance),
										  _acc,
										  ground);
	    if (w < static_cast<TermWeightType>(0)) return BK::AlwaysGreater;
	    _acc -= w;
	    ASSERT(_acc >= static_cast<TermWeightType>(0));
	    _grnd = _grnd && ground;
	  };
      };
      break;


    case CompiledConstraintForUnifiers::WeightComparisonCommand::IfXGrACCPlus1RetAlwGr_IfXLeACCPlus1RetFail_UpdateGRND_IfGRNDRetAlwEq_RetCanBeGrOrEq:
      /*
	if min|X@| > ACC + 1 return AlwaysGreater;
	if min|X@| < ACC + 1 return FailureToCompare;      
	GRND := GRND && ground(X@);
	if GRND return AlwaysEqual;
	return CanBeGreaterOrEqual;
      */   
      {
	bool freeVar;
	const void* instance = (static_cast<const Unifier::Variable*>(command->nativeVar()))->Unref(freeVar);
	if (freeVar)
	  {
	    if (_acc < static_cast<TermWeightType>(0)) return BK::AlwaysGreater;
	    if (_acc > static_cast<TermWeightType>(0)) return BK::FailureToCompare;  
	    return BK::CanBeGreaterOrEqual;
	  }
	else
	  {
	    ++_acc;
	    bool ground;
	    TermWeightType w = 
	      TermWeighting::current()->computeMinimalInstanceWeightIfLessOrEqual(static_cast<const PrefixSym*>(instance),
										  _acc,
										  ground);
	    if (w < static_cast<TermWeightType>(0)) return BK::AlwaysGreater;
	    if (w < _acc) return BK::FailureToCompare; 
	    ASSERT(w == _acc);
	    if (_grnd && ground) return BK::AlwaysEqual;
	    return BK::CanBeGreaterOrEqual;
	  };
      };
    
    case CompiledConstraintForUnifiers::WeightComparisonCommand::IfCXGrACCPlusCRetAlwGr_IfCXLeACCPlusCRetFail_UpdateGRND_IfGRNDRetAlwEq_RetCanBeGrOrEq:
      /*
	if C*min|X@| > ACC + C return AlwaysGreater;
	if C*min|X@| < ACC + C return FailureToCompare;      
	GRND := GRND && ground(X@);
	if GRND return AlwaysEqual;
	return CanBeGreaterOrEqual;
      */   
      {
	bool freeVar;
	const void* instance = (static_cast<const Unifier::Variable*>(command->nativeVar()))->Unref(freeVar);
	if (freeVar)
	  {
	    if (_acc < static_cast<TermWeightType>(0)) return BK::AlwaysGreater;
	    if (_acc > static_cast<TermWeightType>(0)) return BK::FailureToCompare;  
	    return BK::CanBeGreaterOrEqual;
	  }
	else
	  {
	    _acc += command->coefficient();
	    bool ground;
	    TermWeightType w = 
	      TermWeighting::current()->computeMinimalInstanceWeightIfLessOrEqual(command->coefficient(),
										  static_cast<const PrefixSym*>(instance),
										  _acc,
										  ground);
	    if (w < static_cast<TermWeightType>(0)) return BK::AlwaysGreater;
	    if (w < _acc) return BK::FailureToCompare; 
	    ASSERT(w == _acc);
	    if (_grnd && ground) return BK::AlwaysEqual;
	    return BK::CanBeGreaterOrEqual;
	  };
      };
    
      //----------------------------------------

    case CompiledConstraintForUnifiers::WeightComparisonCommand::XPlusWToPOSINST_IfConstPOSINSAndPOSINSTLe0RetFail_IfConstPOSINSTGotoL:
      /*
	POSINST := |X@| + W;
	if constant(POSINST) && POSINST < 0 return FailureToCompare; 
	if constant(POSINST) goto L; 
      */
      {
	_posinst.reset();
	_posinst.add(command->weight());
	
	bool freeVar;
	const void* instance = (static_cast<const Unifier::Variable*>(command->nativeVar()))->Unref(freeVar);
	if (freeVar)
	  {
	    _posinst.add1((static_cast<const Unifier::Variable*>(instance))->absoluteVarNum());
	  }
	else
	  {	    
	    TermWeighting::current()->collectWeight(_posinst,static_cast<const PrefixSym*>(instance));
	    if (_posinst.isConstant())
	      {
		if (_posinst.freeMember() < static_cast<TermWeightType>(0)) return BK::FailureToCompare;  
		command = command->label();
		goto next_command;
	      };
	  };
      };
      break;


    case CompiledConstraintForUnifiers::WeightComparisonCommand::CXPlusWToPOSINST_IfConstPOSINSAndPOSINSTLe0RetFail_IfConstPOSINSTGotoL:
      /*
	POSINST := C*|X@| + W;
	if constant(POSINST) && POSINST < 0 return FailureToCompare; 
	if constant(POSINST) goto L;
      */
      {
	_posinst.reset();
	_posinst.add(command->weight());
	
	bool freeVar;
	const void* instance = (static_cast<const Unifier::Variable*>(command->nativeVar()))->Unref(freeVar);
	if (freeVar)
	  {
	    _posinst.add(command->coefficient(),(static_cast<const Unifier::Variable*>(instance))->absoluteVarNum());
	  }
	else
	  {	    
	    TermWeighting::current()->collectWeight(_posinst,command->coefficient(),static_cast<const PrefixSym*>(instance));
	    if (_posinst.isConstant())
	      {
		if (_posinst.freeMember() < static_cast<TermWeightType>(0)) return BK::FailureToCompare;  
		command = command->label();
		goto next_command;
	      };
	  };
      };
      break;


    case CompiledConstraintForUnifiers::WeightComparisonCommand::XPlusWToPOSINST:
      /*
	POSINST := |X@| + W;
      */
      {
	_posinst.reset();
	_posinst.add(command->weight());
	
	bool freeVar;
	const void* instance = (static_cast<const Unifier::Variable*>(command->nativeVar()))->Unref(freeVar);
	if (freeVar)
	  {
	    _posinst.add1((static_cast<const Unifier::Variable*>(instance))->absoluteVarNum());
	  }
	else
	  {	    
	    TermWeighting::current()->collectWeight(_posinst,static_cast<const PrefixSym*>(instance));
	  };
      };
      break;
    
    case CompiledConstraintForUnifiers::WeightComparisonCommand::CXPlusWToPOSINST:
      /*
	POSINST := C*|X@| + W;
      */
      {
	_posinst.reset();
	_posinst.add(command->weight());
	
	bool freeVar;
	const void* instance = (static_cast<const Unifier::Variable*>(command->nativeVar()))->Unref(freeVar);
	if (freeVar)
	  {
	    _posinst.add(command->coefficient(),(static_cast<const Unifier::Variable*>(instance))->absoluteVarNum());
	  }
	else
	  {	    
	    TermWeighting::current()->collectWeight(_posinst,command->coefficient(),static_cast<const PrefixSym*>(instance));
	  };
      };
      break;

    case CompiledConstraintForUnifiers::WeightComparisonCommand::POSINSTPlusXToPOSINST:
      /*
	POSINST := POSINST + |X@|;
      */
      {
	bool freeVar;
	const void* instance = (static_cast<const Unifier::Variable*>(command->nativeVar()))->Unref(freeVar);
	if (freeVar)
	  {
	    _posinst.add1((static_cast<const Unifier::Variable*>(instance))->absoluteVarNum());
	  }
	else
	  {	    
	    TermWeighting::current()->collectWeight(_posinst,static_cast<const PrefixSym*>(instance));
	  };
      };
      break;
    
    case CompiledConstraintForUnifiers::WeightComparisonCommand::POSINSTPlusCXToPOSINST:
      /*
	POSINST := POSINST + C*|X@|;
      */
      {
	bool freeVar;
	const void* instance = (static_cast<const Unifier::Variable*>(command->nativeVar()))->Unref(freeVar);
	if (freeVar)
	  {
	    _posinst.add(command->coefficient(),(static_cast<const Unifier::Variable*>(instance))->absoluteVarNum());
	  }
	else
	  {	    
	    TermWeighting::current()->collectWeight(_posinst,command->coefficient(),static_cast<const PrefixSym*>(instance));
	  };
      };
      break;

    
    case CompiledConstraintForUnifiers::WeightComparisonCommand::POSINSTPlusXToPOSINST_IfConstPOSINSTAndPOSINSTLe0RetFail_IfConstPOSINSTGotoL:
      /*
	POSINST := POSINST + |X@|;
	if constant(POSINST) && POSINST < 0 return FailureToCompare; 
	if constant(POSINST) goto L;
      */
      {
	bool freeVar;
	const void* instance = (static_cast<const Unifier::Variable*>(command->nativeVar()))->Unref(freeVar);
	if (freeVar)
	  {
	    _posinst.add1((static_cast<const Unifier::Variable*>(instance))->absoluteVarNum());
	  }
	else
	  {	    
	    TermWeighting::current()->collectWeight(_posinst,static_cast<const PrefixSym*>(instance));
	  };
	if (_posinst.isConstant())
	  {
	    if (_posinst.freeMember() < static_cast<TermWeightType>(0)) return BK::FailureToCompare; 
	    command = command->label();
	    goto next_command;
	  };
      };
      break;

    case CompiledConstraintForUnifiers::WeightComparisonCommand::POSINSTPlusCXToPOSINST_IfConstPOSINSTAndPOSINSTLe0RetFail_IfConstPOSINSTGotoL:
      /*
	POSINST := POSINST + C*|X@|;
	if constant(POSINST) && POSINST < 0 return FailureToCompare; 
	if constant(POSINST) goto L;
      */
      {
	bool freeVar;
	const void* instance = (static_cast<const Unifier::Variable*>(command->nativeVar()))->Unref(freeVar);
	if (freeVar)
	  {
	    _posinst.add(command->coefficient(),(static_cast<const Unifier::Variable*>(instance))->absoluteVarNum());
	  }
	else
	  {	    
	    TermWeighting::current()->collectWeight(_posinst,command->coefficient(),static_cast<const PrefixSym*>(instance));
	  };
	if (_posinst.isConstant())
	  {
	    if (_posinst.freeMember() < static_cast<TermWeightType>(0)) return BK::FailureToCompare; 
	    command = command->label();
	    goto next_command;
	  };
      };
      break;

    case CompiledConstraintForUnifiers::WeightComparisonCommand::XMinus1ToNEGINSTRetCompForGrOrEqPOSINSTvNEGINST:
      /*
	NEGINST := |X@| - 1;
	return compareForGreaterOrEqual(POSINST,NEGINST);
      */
      {
	_neginst.reset();
	_neginst.add(static_cast<TermWeightType>(-1));
	bool freeVar;
	const void* instance = (static_cast<const Unifier::Variable*>(command->nativeVar()))->Unref(freeVar);
	if (freeVar)
	  {
	    _neginst.add1((static_cast<const Unifier::Variable*>(instance))->absoluteVarNum());
	  }
	else
	  {
	    TermWeighting::current()->collectWeight(_neginst,static_cast<const PrefixSym*>(instance));
	  };
	return _posinst.compareForGreaterOrEqual(_neginst);
      };
    
    case CompiledConstraintForUnifiers::WeightComparisonCommand::CXMinusCToNEGINSTRetCompForGrOrEqPOSINSTvNEGINST:
      /*
	NEGINST := C*|X@| - C;
	return compareForGreaterOrEqual(POSINST,NEGINST);
      */
      {
	_neginst.reset();
	_neginst.add(-(command->coefficient()));
	bool freeVar;
	const void* instance = (static_cast<const Unifier::Variable*>(command->nativeVar()))->Unref(freeVar);
	if (freeVar)
	  {
	    _neginst.add(command->coefficient(),(static_cast<const Unifier::Variable*>(instance))->absoluteVarNum());
	  }
	else
	  {
	    TermWeighting::current()->collectWeight(_neginst,command->coefficient(),static_cast<const PrefixSym*>(instance));
	  };
	return _posinst.compareForGreaterOrEqual(_neginst);
      };


    case CompiledConstraintForUnifiers::WeightComparisonCommand::XMinus1ToNEGINST_IfCompForGrOrEqPOSINSTvNEGINSTEqFailRetFail:
      /*
	NEGINST := |X@| - 1;
	if compareForGreaterOrEqual(POSINST,NEGINST) == FailureToCompare return FailureToCompare;
      */
      {
	_neginst.reset();
	_neginst.add(static_cast<TermWeightType>(-1));
	bool freeVar;
	const void* instance = (static_cast<const Unifier::Variable*>(command->nativeVar()))->Unref(freeVar);
	if (freeVar)
	  {
	    _neginst.add1((static_cast<const Unifier::Variable*>(instance))->absoluteVarNum());
	  }
	else
	  {
	    TermWeighting::current()->collectWeight(_neginst,static_cast<const PrefixSym*>(instance));
	  };
	if (_posinst.compareForGreaterOrEqual(_neginst) == BK::FailureToCompare) 
	  return BK::FailureToCompare;
      };
      break;

    case CompiledConstraintForUnifiers::WeightComparisonCommand::CXMinusCToNEGINST_IfCompForGrOrEqPOSINSTvNEGINSTEqFailRetFail:
      /*
	NEGINST := C*|X@| - C;
	if compareForGreaterOrEqual(POSINST,NEGINST) == FailureToCompare return FailureToCompare;
      */
      {
	_neginst.reset();
	_neginst.add(-(command->coefficient()));
	bool freeVar;
	const void* instance = (static_cast<const Unifier::Variable*>(command->nativeVar()))->Unref(freeVar);
	if (freeVar)
	  {
	    _neginst.add(command->coefficient(),(static_cast<const Unifier::Variable*>(instance))->absoluteVarNum());
	  }
	else
	  {
	    TermWeighting::current()->collectWeight(_neginst,command->coefficient(),static_cast<const PrefixSym*>(instance));
	  };
	if (_posinst.compareForGreaterOrEqual(_neginst) == BK::FailureToCompare) 
	  return BK::FailureToCompare;
      };
      break;

    case CompiledConstraintForUnifiers::WeightComparisonCommand::NEGINSTPlusXMinus1ToNEGINST_IfCompForGrOrEqPOSINSTvNEGINSTEqFailRetFail:
      /*
	NEGINST := NEGINST + |X@| - 1;
	if compareForGreaterOrEqual(POSINST,NEGINST) == FailureToCompare return FailureToCompare;
      */
      {
	_neginst.add(static_cast<TermWeightType>(-1));
	bool freeVar;
	const void* instance = (static_cast<const Unifier::Variable*>(command->nativeVar()))->Unref(freeVar);
	if (freeVar)
	  {
	    _neginst.add1((static_cast<const Unifier::Variable*>(instance))->absoluteVarNum());
	  }
	else
	  {
	    TermWeighting::current()->collectWeight(_neginst,static_cast<const PrefixSym*>(instance));
	  };
	if (_posinst.compareForGreaterOrEqual(_neginst) == BK::FailureToCompare) 
	  return BK::FailureToCompare;
      };
      break;

    case CompiledConstraintForUnifiers::WeightComparisonCommand::NEGINSTPlusCXMinusCToNEGINST_IfCompForGrOrEqPOSINSTvNEGINSTEqFailRetFail:
      /*
	NEGINST := NEGINST + C*|X@| - C;
	if compareForGreaterOrEqual(POSINST,NEGINST) == FailureToCompare return FailureToCompare;
      */
      {
	_neginst.add(-(command->coefficient()));
	bool freeVar;
	const void* instance = (static_cast<const Unifier::Variable*>(command->nativeVar()))->Unref(freeVar);
	if (freeVar)
	  {
	    _neginst.add(command->coefficient(),(static_cast<const Unifier::Variable*>(instance))->absoluteVarNum());
	  }
	else
	  {
	    TermWeighting::current()->collectWeight(_neginst,command->coefficient(),static_cast<const PrefixSym*>(instance));
	  };
	if (_posinst.compareForGreaterOrEqual(_neginst) == BK::FailureToCompare) 
	  return BK::FailureToCompare;
      };
      break;

    case CompiledConstraintForUnifiers::WeightComparisonCommand::NEGINSTPlusXMinus1ToNEGINST_RetCompForGrOrEqPOSINSTvNEGINST:
      /*
	NEGINST := NEGINST + |X@| - 1;
	return compareForGreaterOrEqual(POSINST,NEGINST);
      */
      {
	_neginst.add(static_cast<TermWeightType>(-1));
	bool freeVar;
	const void* instance = (static_cast<const Unifier::Variable*>(command->nativeVar()))->Unref(freeVar);
	if (freeVar)
	  {
	    _neginst.add1((static_cast<const Unifier::Variable*>(instance))->absoluteVarNum());
	  }
	else
	  {
	    TermWeighting::current()->collectWeight(_neginst,static_cast<const PrefixSym*>(instance));
	  };
	return _posinst.compareForGreaterOrEqual(_neginst);
      };

    case CompiledConstraintForUnifiers::WeightComparisonCommand::NEGINSTPlusCXMinusCToNEGINST_RetCompForGrOrEqPOSINSTvNEGINST:
      /*
	NEGINST := NEGINST + C*|X@| - C;
	return compareForGreaterOrEqual(POSINST,NEGINST);
      */
      {
	_neginst.add(-(command->coefficient()));
	bool freeVar;
	const void* instance = (static_cast<const Unifier::Variable*>(command->nativeVar()))->Unref(freeVar);
	if (freeVar)
	  {
	    _neginst.add(command->coefficient(),(static_cast<const Unifier::Variable*>(instance))->absoluteVarNum());
	  }
	else
	  {
	    TermWeighting::current()->collectWeight(_neginst,command->coefficient(),static_cast<const PrefixSym*>(instance));
	  };
	return _posinst.compareForGreaterOrEqual(_neginst);
      };


    case CompiledConstraintForUnifiers::WeightComparisonCommand::IfNotGrndRetFail_IfXMinus1GrPOSINSTRetFail_IfXMinus1LePOSINSTRetAlwGr_RetAlwEq:
      /*
	ASSERT(constant(POSINST));
	ASSERT(POSINST >= 0);
	if !ground(X@) return FailureToCompare;
	if min|X@| - 1 > POSINST return FailureToCompare;
	if min|X@| - 1 < POSINST return AlwaysGreater;
	return AlwaysEqual;
      */
      {
	ASSERT(_posinst.isConstant());
	ASSERT(_posinst.freeMember() >= static_cast<TermWeightType>(0));
	bool freeVar;
	const void* instance = (static_cast<const Unifier::Variable*>(command->nativeVar()))->Unref(freeVar);
	if (freeVar)
	  {
	    return BK::FailureToCompare;
	  }
	else
	  {
	    _posinst.add1();
	    TermWeightType w = 
	      TermWeighting::current()->computeWeightIfGroundAndLessOrEqual(static_cast<const PrefixSym*>(instance),
									    _posinst.freeMember());
	    if (w <= static_cast<TermWeightType>(0)) return BK::FailureToCompare;
	    ASSERT(w <= _posinst.freeMember());
	    if (w < _posinst.freeMember()) return BK::AlwaysGreater;
	    return BK::AlwaysEqual;
	  };
      };

    case CompiledConstraintForUnifiers::WeightComparisonCommand::IfNotGrndRetFail_IfCXMinusCGrPOSINSTRetFail_IfCXMinusCLePOSINSTRetAlwGr_RetAlwEq:
      /*
	ASSERT(constant(POSINST));
	ASSERT(POSINST >= 0);
	if !ground(X@) return FailureToCompare;
	if C*min|X@| - C > POSINST return FailureToCompare;
	if C*min|X@| - C < POSINST return AlwaysGreater;
	return AlwaysEqual;
      */
      {
	ASSERT(_posinst.isConstant());
	ASSERT(_posinst.freeMember() >= static_cast<TermWeightType>(0));
	bool freeVar;
	const void* instance = (static_cast<const Unifier::Variable*>(command->nativeVar()))->Unref(freeVar);
	if (freeVar)
	  {
	    return BK::FailureToCompare;
	  }
	else
	  {
	    _posinst.add(command->coefficient());
	    TermWeightType w = 
	      TermWeighting::current()->computeWeightIfGroundAndLessOrEqual(command->coefficient(),
									    static_cast<const PrefixSym*>(instance),
									    _posinst.freeMember());
	    if (w <= static_cast<TermWeightType>(0)) return BK::FailureToCompare;
	    ASSERT(w <= _posinst.freeMember());
	    if (w < _posinst.freeMember()) return BK::AlwaysGreater;
	    return BK::AlwaysEqual;
	  };
      };


    case CompiledConstraintForUnifiers::WeightComparisonCommand::IfNotGrndRetFail_POSINSTPlus1MinusXToACC_IfACCLe0RetFail:
      /*
	ASSERT(constant(POSINST));
	ASSERT(POSINST >= 0);
	if !ground(X@) return FailureToCompare;
	ACC := POSINST + 1 - min|X@|;      
	if ACC < 0 return FailureToCompare; 
      */
      {	
	ASSERT(_posinst.isConstant());
	ASSERT(_posinst.freeMember() >= static_cast<TermWeightType>(0));
	bool freeVar;
	const void* instance = (static_cast<const Unifier::Variable*>(command->nativeVar()))->Unref(freeVar);
	if (freeVar)
	  {
	    return BK::FailureToCompare;
	  }
	else
	  {	    
	    _acc = _posinst.freeMember() + 1;
	    TermWeightType w = 
	      TermWeighting::current()->computeWeightIfGroundAndLessOrEqual(static_cast<const PrefixSym*>(instance),
									    _acc);
	    if (w <= static_cast<TermWeightType>(0)) return BK::FailureToCompare;
	    _acc -= w;
	    ASSERT(_acc >= static_cast<TermWeightType>(0));
	  };
      };
      break;

    case CompiledConstraintForUnifiers::WeightComparisonCommand::IfNotGrndRetFail_POSINSTPlusCMinusCXToACC_IfACCLe0RetFail:
      /*
	ASSERT(constant(POSINST));
	ASSERT(POSINST >= 0);
	if !ground(X@) return FailureToCompare;
	ACC := POSINST + C - C*min|X@|;      
	if ACC < 0 return FailureToCompare; 
      */
      {	
	ASSERT(_posinst.isConstant());
	ASSERT(_posinst.freeMember() >= static_cast<TermWeightType>(0));
	bool freeVar;
	const void* instance = (static_cast<const Unifier::Variable*>(command->nativeVar()))->Unref(freeVar);
	if (freeVar)
	  {
	    return BK::FailureToCompare;
	  }
	else
	  {	    
	    _acc = _posinst.freeMember() + command->coefficient();
	    TermWeightType w = 
	      TermWeighting::current()->computeWeightIfGroundAndLessOrEqual(command->coefficient(),
									    static_cast<const PrefixSym*>(instance),
									    _acc);
	    if (w <= static_cast<TermWeightType>(0)) return BK::FailureToCompare;
	    _acc -= w;
	    ASSERT(_acc >= static_cast<TermWeightType>(0));
	  };
      };
      break;




    case CompiledConstraintForUnifiers::WeightComparisonCommand::XPlusWToNEGINST_IfConstNEGINSTAndNEGINSTLe0RetAlwGr_IfConstNEGINSTGotoL:
      /*
	NEGINST := |X@| + W;
	if constant(NEGINST) && NEGINST < 0 return AlwaysGreater;
	if constant(NEGINST) goto L;
      */
      {
	_neginst.reset();
	_neginst.add(command->weight());
	bool freeVar;
	const void* instance = (static_cast<const Unifier::Variable*>(command->nativeVar()))->Unref(freeVar);
	if (freeVar)
	  {
	    _neginst.add1((static_cast<const Unifier::Variable*>(instance))->absoluteVarNum());
	  }
	else
	  {
	    TermWeighting::current()->collectWeight(_neginst,static_cast<const PrefixSym*>(instance));
	    if (_neginst.isConstant())
	      {
		if (_neginst.freeMember() < static_cast<TermWeightType>(0)) return BK::AlwaysGreater;
		command = command->label();
		goto next_command;
	      };
	  };
      };
      break;
     


    case CompiledConstraintForUnifiers::WeightComparisonCommand::CXPlusWToNEGINST_IfConstNEGINSTAndNEGINSTLe0RetAlwGr_IfConstNEGINSTGotoL:
      /*
	NEGINST := C*|X@| + W;
	if constant(NEGINST) && NEGINST < 0 return AlwaysGreater;
	if constant(NEGINST) goto L;
      */
      {
	_neginst.reset();
	_neginst.add(command->weight());
	bool freeVar;
	const void* instance = (static_cast<const Unifier::Variable*>(command->nativeVar()))->Unref(freeVar);
	if (freeVar)
	  {
	    _neginst.add(command->coefficient(),(static_cast<const Unifier::Variable*>(instance))->absoluteVarNum());
	  }
	else
	  {
	    TermWeighting::current()->collectWeight(_neginst,command->coefficient(),static_cast<const PrefixSym*>(instance));
	    if (_neginst.isConstant())
	      {
		if (_neginst.freeMember() < static_cast<TermWeightType>(0)) return BK::AlwaysGreater;
		command = command->label();
		goto next_command;
	      };
	  };
      };
      break;


    case CompiledConstraintForUnifiers::WeightComparisonCommand::XPlusWToNEGINST:
      /*
	NEGINST := |X@| + W;
      */
      {
	_neginst.reset();
	_neginst.add(command->weight());
	bool freeVar;
	const void* instance = (static_cast<const Unifier::Variable*>(command->nativeVar()))->Unref(freeVar);
	if (freeVar)
	  {
	    _neginst.add1((static_cast<const Unifier::Variable*>(instance))->absoluteVarNum());
	  }
	else
	  {
	    TermWeighting::current()->collectWeight(_neginst,static_cast<const PrefixSym*>(instance));
	  };
      };
      break;

    
    case CompiledConstraintForUnifiers::WeightComparisonCommand::CXPlusWToNEGINST:
      /*
	NEGINST := C*|X@| + W;
      */
      {
	_neginst.reset();
	_neginst.add(command->weight());
	bool freeVar;
	const void* instance = (static_cast<const Unifier::Variable*>(command->nativeVar()))->Unref(freeVar);
	if (freeVar)
	  {
	    _neginst.add(command->coefficient(),(static_cast<const Unifier::Variable*>(instance))->absoluteVarNum());
	  }
	else
	  {
	    TermWeighting::current()->collectWeight(_neginst,command->coefficient(),static_cast<const PrefixSym*>(instance));
	  };
      };
      break;

        
    case CompiledConstraintForUnifiers::WeightComparisonCommand::NEGINSTPlusXToNEGINST:
      /*
	NEGINST := NEGINST + |X@|;
      */
      {
	bool freeVar;
	const void* instance = (static_cast<const Unifier::Variable*>(command->nativeVar()))->Unref(freeVar);
	if (freeVar)
	  {
	    _neginst.add1((static_cast<const Unifier::Variable*>(instance))->absoluteVarNum());
	  }
	else
	  {
	    TermWeighting::current()->collectWeight(_neginst,static_cast<const PrefixSym*>(instance));
	  };
      };
      break;

    case CompiledConstraintForUnifiers::WeightComparisonCommand::NEGINSTPlusCXToNEGINST:
      /*
	NEGINST := NEGINST + C*|X@|;
      */
      {
	bool freeVar;
	const void* instance = (static_cast<const Unifier::Variable*>(command->nativeVar()))->Unref(freeVar);
	if (freeVar)
	  {
	    _neginst.add(command->coefficient(),(static_cast<const Unifier::Variable*>(instance))->absoluteVarNum());
	  }
	else
	  {
	    TermWeighting::current()->collectWeight(_neginst,command->coefficient(),static_cast<const PrefixSym*>(instance));
	  };
      };
      break;
    
    case CompiledConstraintForUnifiers::WeightComparisonCommand::NEGINSTPlusXToNEGINST_IfConstNEGINSTAndNEGINSTLe0RetAlwGr_IfConstNEGINSTGotoL:
      /*
	NEGINST := NEGINST + |X@|;
	if constant(NEGINST) && NEGINST < 0 return AlwaysGreater;
	if constant(NEGINST) goto L;
      */
      {
	bool freeVar;
	const void* instance = (static_cast<const Unifier::Variable*>(command->nativeVar()))->Unref(freeVar);
	if (freeVar)
	  {
	    _neginst.add1((static_cast<const Unifier::Variable*>(instance))->absoluteVarNum());
	  }
	else
	  {
	    TermWeighting::current()->collectWeight(_neginst,static_cast<const PrefixSym*>(instance));
	    if (_neginst.isConstant())
	      {
		if (_neginst.freeMember() < 0) return BK::AlwaysGreater;
		command = command->label();
		goto next_command;
	      };
	  };
      };
      break;
        
    case CompiledConstraintForUnifiers::WeightComparisonCommand::NEGINSTPlusCXToNEGINST_IfConstNEGINSTAndNEGINSTLe0RetAlwGr_IfConstNEGINSTGotoL:
      /*
	NEGINST := NEGINST + C*|X@|;
	if constant(NEGINST) && NEGINST < 0 return AlwaysGreater;
	if constant(NEGINST) goto L;
      */
      {
	bool freeVar;
	const void* instance = (static_cast<const Unifier::Variable*>(command->nativeVar()))->Unref(freeVar);
	if (freeVar)
	  {
	    _neginst.add(command->coefficient(),(static_cast<const Unifier::Variable*>(instance))->absoluteVarNum());
	  }
	else
	  {
	    TermWeighting::current()->collectWeight(_neginst,command->coefficient(),static_cast<const PrefixSym*>(instance));
	    if (_neginst.isConstant())
	      {
		if (_neginst.freeMember() < 0) return BK::AlwaysGreater;
		command = command->label();
		goto next_command;
	      };
	  };
      };
      break;
    

    case CompiledConstraintForUnifiers::WeightComparisonCommand::XMinus1ToPOSINST_IfCompForGrOrEqPOSINSTvNEGINSTEqAlwGrRetAlwGr:
      /*
	POSINST := |X@| - 1;
	if compareForGreaterOrEqual(POSINST,NEGINST) == AlwaysGreater return AlwaysGreater;
      */
      {
	_posinst.reset();
	_posinst.add(static_cast<TermWeightType>(-1));
	bool freeVar;
	const void* instance = (static_cast<const Unifier::Variable*>(command->nativeVar()))->Unref(freeVar);
	if (freeVar)
	  {
	    _posinst.add1((static_cast<const Unifier::Variable*>(instance))->absoluteVarNum());
	  }
	else
	  {
	    TermWeighting::current()->collectWeight(_posinst,static_cast<const PrefixSym*>(instance));
	  };
	
	if (_posinst.compareForGreaterOrEqual(_neginst) == BK::AlwaysGreater)
	  return BK::AlwaysGreater;
      };
      break;

    case CompiledConstraintForUnifiers::WeightComparisonCommand::CXMinusCToPOSINST_IfCompForGrOrEqPOSINSTvNEGINSTEqAlwGrRetAlwGr:
      /*
	POSINST := C*|X@| - C;
	if compareForGreaterOrEqual(POSINST,NEGINST) == AlwaysGreater return AlwaysGreater;
      */
      {
	_posinst.reset();
	_posinst.add(-(command->coefficient()));
	bool freeVar;
	const void* instance = (static_cast<const Unifier::Variable*>(command->nativeVar()))->Unref(freeVar);
	if (freeVar)
	  {
	    _posinst.add(command->coefficient(),(static_cast<const Unifier::Variable*>(instance))->absoluteVarNum());
	  }
	else
	  {
	    TermWeighting::current()->collectWeight(_posinst,command->coefficient(),static_cast<const PrefixSym*>(instance));
	  };
	if (_posinst.compareForGreaterOrEqual(_neginst) == BK::AlwaysGreater)
	  return BK::AlwaysGreater;
      };
      break;

    case CompiledConstraintForUnifiers::WeightComparisonCommand::POSINSTPlusXMinus1ToPOSINST_IfCompForGrOrEqPOSINSTvNEGINSTEqAlwGrRetAlwGr:
      /*
	POSINST := POSINST + |X@| - 1;
	if compareForGreaterOrEqual(POSINST,NEGINST) == AlwaysGreater return AlwaysGreater;
      */
      {
	_posinst.add(static_cast<TermWeightType>(-1));
	bool freeVar;
	const void* instance = (static_cast<const Unifier::Variable*>(command->nativeVar()))->Unref(freeVar);
	if (freeVar)
	  {
	    _posinst.add1((static_cast<const Unifier::Variable*>(instance))->absoluteVarNum());
	  }
	else
	  {
	    TermWeighting::current()->collectWeight(_posinst,static_cast<const PrefixSym*>(instance));
	  };
	if (_posinst.compareForGreaterOrEqual(_neginst) == BK::AlwaysGreater)
	  return BK::AlwaysGreater;
      };
      break;

    case CompiledConstraintForUnifiers::WeightComparisonCommand::POSINSTPlusCXMinusCToPOSINST_IfCompForGrOrEqPOSINSTvNEGINSTEqAlwGrRetAlwGr:
      /*
	POSINST := POSINST + C*|X@| - C;
	if compareForGreaterOrEqual(POSINST,NEGINST) == AlwaysGreater return AlwaysGreater;
      */
      {
	_posinst.add(-(command->coefficient()));
	bool freeVar;
	const void* instance = (static_cast<const Unifier::Variable*>(command->nativeVar()))->Unref(freeVar);
	if (freeVar)
	  {
	    _posinst.add(command->coefficient(),(static_cast<const Unifier::Variable*>(instance))->absoluteVarNum());
	  }
	else
	  {
	    TermWeighting::current()->collectWeight(_posinst,command->coefficient(),static_cast<const PrefixSym*>(instance));
	  };
	if (_posinst.compareForGreaterOrEqual(_neginst) == BK::AlwaysGreater)
	  return BK::AlwaysGreater;
      };
      break;

    case CompiledConstraintForUnifiers::WeightComparisonCommand::POSINSTPlusXMinus1ToPOSINST_RetCompForGrOrEqPOSINSTvNEGINST:
      /*
	POSINST := POSINST + |X@| - 1;
	return compareForGreaterOrEqual(POSINST,NEGINST);
      */
      {
	_posinst.add(static_cast<TermWeightType>(-1));
	bool freeVar;
	const void* instance = (static_cast<const Unifier::Variable*>(command->nativeVar()))->Unref(freeVar);
	if (freeVar)
	  {
	    _posinst.add1((static_cast<const Unifier::Variable*>(instance))->absoluteVarNum());
	  }
	else
	  {
	    TermWeighting::current()->collectWeight(_posinst,static_cast<const PrefixSym*>(instance));
	  };
	return _posinst.compareForGreaterOrEqual(_neginst);
      };

    case CompiledConstraintForUnifiers::WeightComparisonCommand::POSINSTPlusCXMinusCToPOSINST_RetCompForGrOrEqPOSINSTvNEGINST:
      /*
	POSINST := POSINST + C*|X@| - C;
	return compareForGreaterOrEqual(POSINST,NEGINST);
      */
      {
	_posinst.add(-(command->coefficient()));
	bool freeVar;
	const void* instance = (static_cast<const Unifier::Variable*>(command->nativeVar()))->Unref(freeVar);
	if (freeVar)
	  {
	    _posinst.add(command->coefficient(),(static_cast<const Unifier::Variable*>(instance))->absoluteVarNum());
	  }
	else
	  {
	    TermWeighting::current()->collectWeight(_posinst,command->coefficient(),static_cast<const PrefixSym*>(instance));
	  };
	return _posinst.compareForGreaterOrEqual(_neginst);
      };


    case CompiledConstraintForUnifiers::WeightComparisonCommand::NEGINSTPlus1MinusXToACC_IfACCLe0RetAlwGr_GrndToGRND:
      /*
	ASSERT(constant(NEGINST));
	ASSERT(NEGINST >= 0);
	ACC := NEGINST + 1 - min|X@|;
	if ACC < 0 return AlwaysGreater;
	GRND := ground(X@);
      */  
      {
	ASSERT(_neginst.isConstant());
	ASSERT(_neginst.freeMember() >= static_cast<TermWeightType>(0));
	bool freeVar;
	const void* instance = (static_cast<const Unifier::Variable*>(command->nativeVar()))->Unref(freeVar);
	if (freeVar)
	  {
	    _acc = _neginst.freeMember();
	    _grnd = false;
	  }
	else
	  {
	    _acc = _neginst.freeMember() + 1;
	    bool ground;
	    TermWeightType w = 
	      TermWeighting::current()->computeMinimalInstanceWeightIfLessOrEqual(static_cast<const PrefixSym*>(instance),
										  _acc,
										  ground);

	    if (w < static_cast<TermWeightType>(0)) return BK::AlwaysGreater;
	    _acc -= w;
	    _grnd = ground;
	  };

      };
      break;

    case CompiledConstraintForUnifiers::WeightComparisonCommand::NEGINSTPlusCMinusCXToACC_IfACCLe0RetAlwGr_GrndToGRND:
      /*
	ASSERT(constant(NEGINST));
	ASSERT(NEGINST >= 0);
	ACC := NEGINST + C - C*min|X@|;
	if ACC < 0 return AlwaysGreater;
	GRND := ground(X@);
      */  
      {
	ASSERT(_neginst.isConstant());
	ASSERT(_neginst.freeMember() >= static_cast<TermWeightType>(0));
	bool freeVar;
	const void* instance = (static_cast<const Unifier::Variable*>(command->nativeVar()))->Unref(freeVar);
	if (freeVar)
	  {
	    _acc = _neginst.freeMember();
	    _grnd = false;
	  }
	else
	  {
	    _acc = _neginst.freeMember() + command->coefficient();
	    bool ground;
	    TermWeightType w = 
	      TermWeighting::current()->computeMinimalInstanceWeightIfLessOrEqual(command->coefficient(),
										  static_cast<const PrefixSym*>(instance),
										  _acc,
										  ground);
	    if (w < static_cast<TermWeightType>(0)) return BK::AlwaysGreater;
	    _acc -= w;
	    _grnd = ground;
	  };

      };
      break;


#ifdef DEBUG_NAMESPACE
    default: ICP("Bad instruction tag");
#endif
    };
  ASSERT(!command->isTerminal());
  ++command;
  goto next_command;

  ICP("END");
}; // FunctionComparison NonrecursiveKBOrdering::CompiledConstraintForUnifiers::interpretWeightComparisonCode()



//===================================================================
#undef DEBUG_NAMESPACE
#ifdef DEBUG_NONRECURSIVE_KB_ORDERING
#define DEBUG_NAMESPACE "NonrecursiveKBOrdering::CompiledConstraintForUnifiers::Command"
#endif
#include "debugMacros.hpp"
//===================================================================



inline
bool 
NonrecursiveKBOrdering::CompiledConstraintForUnifiers::Command::isTerminal() const
{
  CALL("isTerminal() const");
  switch (tag())
    {
    case RetSuccess:
    case RetFailure:
    case IfLexSTEqIncRetFail_IfLexSTEqLeRetFail_RetSucc:
      return true;
    default: return false;
    };
}; // bool NonrecursiveKBOrdering::CompiledConstraintForUnifiers::Command::isTerminal() const


#ifndef NO_DEBUG_VIS

ostream& NonrecursiveKBOrdering::CompiledConstraintForUnifiers::Command::output(ostream& str) const
{
  switch (tag())
    {
    case RetSuccess: return str << "RetSuccess;\n";
    case RetFailure: return str << "RetFailure;\n";

    case IfWCFailRetFail_IfWCAlwGrRetSucc:
      str << "IfWCFailRetFail_IfWCAlwGrRetSucc\n"
	  << " {\n"
	  << "   if WC(@) = FailureToCompare return Failure;\n"
	  << "   if WC(@) = AlwaysGreater return Success;\n"
          << "   ASSERT(WC(@) = AlwaysEqual || WC(@) = CanBeGreaterOrEqual);\n"
	  << " }\n";
      return str;

    case IfLexSTEqIncRetFail_IfLexSTEqLeRetFail_RetSucc:
      str << "IfLexSTEqIncRetFail_IfLexSTEqLeRetFail_RetSucc\n";
      Prefix::outputTerm(str << " S = ",complexTerm1()) << "\n";   
      Prefix::outputTerm(str << " T = ",complexTerm2()) << "\n";   
      str << " {\n"
          << "   if lex(S@,T@) = Incomparable return Failure;"
	  << "   if lex(S@,T@) = Less return Failure;"
          << "   return Success;\n"
	  << " }\n";
      return str;
	
    case IfLexXYEqIncRetFail_IfLexXYEqLeRetFail_IfLexXYEqGrRetSucc:
      str << "IfLexXYEqIncRetFail_IfLexXYEqLeRetFail_IfLexXYEqGrRetSucc\n";
      str << " X = X" << var1()->absoluteVarNum() << "\n";   
      str << " Y = X" << var2()->absoluteVarNum() << "\n";   
      str << " {\n"
          << "   if lex(X@,Y@) = Incomparable return Failure;\n"
	  << "   if lex(X@,Y@) = Less return Failure;\n"
	  << "   if lex(X@,Y@) = Greater return Success;\n"
	  << "   ASSERT(lex(X@,Y@) = Equal);\n"
	  << " }\n";
      return str;

    case IfLexXTEqIncRetFail_IfLexXTEqLeRetFail_IfLexXTEqGrRetSucc:
      str << "IfLexXTEqIncRetFail_IfLexXTEqLeRetFail_IfLexXTEqGrRetSucc\n";  
      str << " X = X" << var1()->absoluteVarNum() << "\n";     
      Prefix::outputTerm(str << " T = ",complexTerm2()) << "\n";   
      str << " {\n"
          << "   if lex(X@,T@) = Incomparable return Failure;\n"
	  << "   if lex(X@,T@) = Less return Failure;\n"
	  << "   if lex(X@,T@) = Greater return Success;\n"
	  << "   ASSERT(lex(X@,T@) = Equal);\n"
	  << " }\n";
      return str;
	
    case IfLexSYEqIncRetFail_IfLexSYEqLeRetFail_IfLexTYEqGrRetSucc:
      str << "IfLexSYEqIncRetFail_IfLexSYEqLeRetFail_IfLexTYEqGrRetSucc\n";  
      Prefix::outputTerm(str << " S = ",complexTerm1()) << "\n";  
      str << " Y = X" << var2()->absoluteVarNum() << "\n";  
      str << " {\n"
	  << "   if lex(S@,Y@) = Incomparable return Failure;\n"
	  << "   if lex(S@,Y@) = Less return Failure;\n"
	  << "   if lex(S@,Y@) = Greater return Success;\n"
	  << "   ASSERT(lex(S@,Y@) = Equal);\n"
	  << " }\n";
      return str;

    default: return str << "????????????;\n";
    };
  return str;
}; // ostream& NonrecursiveKBOrdering::CompiledConstraintForUnifiers::Command::output(ostream& str) const
#endif



//===================================================================
#undef DEBUG_NAMESPACE
#ifdef DEBUG_NONRECURSIVE_KB_ORDERING
#define DEBUG_NAMESPACE "NonrecursiveKBOrdering::CompiledConstraintForInstanceRetrieval"
#endif
#include "debugMacros.hpp"
//===================================================================

NonrecursiveKBOrdering::CompiledConstraintForInstanceRetrieval::~CompiledConstraintForInstanceRetrieval()
{
};


void 
NonrecursiveKBOrdering::CompiledConstraintForInstanceRetrieval::loadGreater(const TERM* lhs,const TERM* rhs)
{
  CALL("loadGreater(const TERM* lhs,const TERM* rhs)");

  //DF; cout << "LOAD " << lhs << " > " << rhs << "\n";

  INTERCEPT_BACKWARD_CONSTR_CHECK_BEGIN1;
  ASSERT(lhs->isComplex());
  ASSERT(rhs->isComplex());
  _targetRelation = Greater;
  _lhs = lhs;
  _rhs = rhs;
  _lhsWeight.reset();
  _rhsWeight.reset();

  TermWeighting::current()->collectWeight(_lhsWeight,TERM(const_cast<TERM*>(_lhs)));
  TermWeighting::current()->collectWeight(_rhsWeight,TERM(const_cast<TERM*>(_rhs)));
  _weightComparison.compare(_lhsWeight,_rhsWeight);
  switch (_weightComparison.category())
    {
    case BK::FailureToCompare: 
      setCategory(SimplificationOrdering::CompiledConstraintForInstanceRetrieval::FailureToCompare); 
      break;
    case BK::AlwaysLess:
      setCategory(SimplificationOrdering::CompiledConstraintForInstanceRetrieval::Contradiction); 
      break;
    case BK::AlwaysEqual:
      setCategory(SimplificationOrdering::CompiledConstraintForInstanceRetrieval::Volatile); 
      break;
    case BK::AlwaysGreater:
      setCategory(SimplificationOrdering::CompiledConstraintForInstanceRetrieval::Tautology); 
      break;
    case BK::CanBeLessOrEqual:
      setCategory(SimplificationOrdering::CompiledConstraintForInstanceRetrieval::NearlyContradiction); 
      break;
    case BK::CanBeGreaterOrEqual:
      setCategory(SimplificationOrdering::CompiledConstraintForInstanceRetrieval::NearlyTautology); 
      break;
    case BK::VolatileButNearlyAlwaysLess:
      setCategory(SimplificationOrdering::CompiledConstraintForInstanceRetrieval::NearlyContradiction); 
      break;
    case BK::VolatileButNearlyAlwaysGreater:
      setCategory(SimplificationOrdering::CompiledConstraintForInstanceRetrieval::NearlyTautology); 
      break;
    case BK::AbsolutelyVolatile:
      setCategory(SimplificationOrdering::CompiledConstraintForInstanceRetrieval::Volatile); 
      break;
    };  
  INTERCEPT_BACKWARD_CONSTR_CHECK_END1;
}; // void NonrecursiveKBOrdering::CompiledConstraintForInstanceRetrieval::loadGreater(const TERM* lhs,const TERM* rhs)

bool NonrecursiveKBOrdering::CompiledConstraintForInstanceRetrieval::lhsVarInstMayOrder()
{
  CALL("lhsVarInstMayOrder()");
  return !_rhsWeight.containsVariableWhichIsNotIn(_lhsWeight);
}; // bool NonrecursiveKBOrdering::CompiledConstraintForInstanceRetrieval::lhsVarInstMayOrder()

 
bool NonrecursiveKBOrdering::CompiledConstraintForInstanceRetrieval::rhsVarInstMayOrder()
{
  CALL("rhsVarInstMayOrder()");
  return !_lhsWeight.containsVariableWhichIsNotIn(_rhsWeight);
}; // bool NonrecursiveKBOrdering::CompiledConstraintForInstanceRetrieval::rhsVarInstMayOrder()

void NonrecursiveKBOrdering::CompiledConstraintForInstanceRetrieval::compile(const InstRet::Substitution* subst)
{
  CALL("compile(const InstRet::Substitution* subst)");
  INTERCEPT_BACKWARD_CONSTR_CHECK_COMPILATION_CALL;
  INTERCEPT_BACKWARD_CONSTR_CHECK_BEGIN1;
  ASSERT(_targetRelation == Greater)
    _subst = subst;
  Command* command = _mainCode.memory();
  bool compileWeightComparison = false;
  switch (_weightComparison.category())
    {
    case BK::FailureToCompare:
      command->setTag(Command::RetFailure);
      INTERCEPT_BACKWARD_CONSTR_CHECK_END1;
      return;
    case BK::AlwaysLess:
      command->setTag(Command::RetFailure);
      INTERCEPT_BACKWARD_CONSTR_CHECK_END1;
      return;
    case BK::AlwaysEqual:
      break;
    case BK::AlwaysGreater:
      command->setTag(Command::RetSuccess);
      INTERCEPT_BACKWARD_CONSTR_CHECK_END1;
      return;
    case BK::CanBeLessOrEqual:
      compileWeightComparison = true;
      break;
    case BK::CanBeGreaterOrEqual:
      compileWeightComparison = true;
      break;
    case BK::VolatileButNearlyAlwaysLess:
      compileWeightComparison = true;
      break;
    case BK::VolatileButNearlyAlwaysGreater:
      compileWeightComparison = true;
      break;
    case BK::AbsolutelyVolatile:
      compileWeightComparison = true;
      break;
    };

  if (compileWeightComparison)
    {

      if (!_weightComparisonCode.size())
	{
	  DOP(_weightComparisonCode.unfreeze());
	  _weightComparisonCode.expand();
	  DOP(_weightComparisonCode.freeze());
	};
      WeightComparisonCommand* endOfWeightCode = 
	_weightComparisonCode.memory();
      while (!_weightComparison.compileGreaterOrEqual(endOfWeightCode,
						      _weightComparisonCode.size()))
	{
	  DOP(_weightComparisonCode.unfreeze());
	  _weightComparisonCode.expand();
	  DOP(_weightComparisonCode.freeze());
	  endOfWeightCode = _weightComparisonCode.memory();
	};      


      // naturalise the variables
      for (WeightComparisonCommand* c = _weightComparisonCode.memory();
	   c < endOfWeightCode;
	   ++c)
	if (c->requiresVar())
	  {
	    c->setNativeVar(const_cast<void*>(static_cast<const void*>(_subst->nativeVar(c->var()))));
	  };

    };
  
  // main code
 
  ulong maxSize;

 compile_main:
  command = _mainCode.memory();
  maxSize = _mainCode.size();
  if (compileWeightComparison)
    {
      command->setTag(Command::IfWCFailRetFail_IfWCAlwGrRetSucc);
      ++command;
      --maxSize;
    };
   
  if (!compileLexGreater(command,maxSize))
    {
      DOP(_mainCode.unfreeze());
      _mainCode.expand();
      DOP(_mainCode.freeze());
      goto compile_main;
    };

  
  INTERCEPT_BACKWARD_CONSTR_CHECK_END1;

  /***********
	      DF; cout << "CHECK: ";
	      DF; cout << _lhs << " > ";
	      DF; cout << _rhs << "\n";
	      DF; cout << "WEIGHT COMP:\n" << _weightComparison << "\n";
	      DF; output(cout << "\n\n") << "\n\n";
  ************/

}; // void NonrecursiveKBOrdering::CompiledConstraintForInstanceRetrieval::compile(const InstRet::Substitution* subst)



bool NonrecursiveKBOrdering::CompiledConstraintForInstanceRetrieval::holds()
{
  CALL("holds()");


#ifdef MEASURE_BACKWARD_CONSTR_CHECK
  INTERCEPT_BACKWARD_CONSTR_CHECK_BEGIN2;
  // Straightforward check:
  SimplificationOrdering::current()->greater(_lhs,_rhs,_subst);
  INTERCEPT_BACKWARD_CONSTR_CHECK_END2;
#endif

  INTERCEPT_BACKWARD_CONSTR_CHECK_NEW_CHECK;
  INTERCEPT_BACKWARD_CONSTR_CHECK_BEGIN1;

  // DF; output(cout << "\n\n\nHOLDS?\n") << "\n\n\n";   
  // DF; cout << "SUBST " << _subst << "\n"; 

  Command* command = _mainCode.memory();
  Comparison lexCmp;
  FunctionComparison weightComp;
  
 next_command:
  switch (command->tag())
    {
    case Command::RetSuccess:
      goto return_true; 
    case Command::RetFailure:
      goto return_false;

    case Command::IfWCFailRetFail_IfWCAlwGrRetSucc:
      /*
	if WC(@) = FailureToCompare return Failure;
	if WC(@) = AlwaysGreater return Success;
	ASSERT(WC(@) = AlwaysEqual || WC(@) = CanBeGreaterOrEqual);
      */

      weightComp = interpretWeightComparisonCode();

      //DF; cout << "\n\n\nWEIGHT COMPARISON RETURNS: " << weightComp << "\n\n\n"; 

      if (weightComp == BK::FailureToCompare) goto return_false;
      if (weightComp == BK::AlwaysGreater) goto return_true;
      ASSERT((weightComp == BK::CanBeGreaterOrEqual) || (weightComp == BK::AlwaysEqual));
      ++command;
      goto next_command;
	
    case Command::IfLexSTEqIncRetFail_IfLexSTEqLeRetFail_RetSucc:
      /*
	if lex(S@,T@) = Incomparable return Failure;
	if lex(S@,T@) = Less return Failure;
	return Success;
      */
      lexCmp = compareLexComplexComplex(command->complexTerm1(),command->complexTerm2());
      if ((lexCmp == BK::Greater) || (lexCmp == BK::Equal)) goto return_true;
      goto return_false;
	
    case Command::IfLexXYEqIncRetFail_IfLexXYEqLeRetFail_IfLexXYEqGrRetSucc:
      /*
	if lex(X@,Y@) = Incomparable return Failure;
	if lex(X@,Y@) = Less return Failure;
	if lex(X@,Y@) = Greater return Success;
	ASSERT(lex(X@,Y@) = Equal);
      */
      lexCmp = compareLexVarVar(command->var1(),command->var2());
      if (lexCmp == BK::Greater) goto return_true;
      if (lexCmp != BK::Equal) goto return_false;
      ++command;
      goto next_command;

    case Command::IfLexXTEqIncRetFail_IfLexXTEqLeRetFail_IfLexXTEqGrRetSucc:
      /*
	if lex(X@,T@) = Incomparable return Failure;
	if lex(X@,T@) = Less return Failure;
	if lex(X@,T@) = Greater return Success;
	ASSERT(lex(X@,T@) = Equal);
      */
      lexCmp = compareLexVarComplex(command->var1(),command->complexTerm2());

      if (lexCmp == BK::Greater) goto return_true;
      if (lexCmp != BK::Equal) goto return_false;
      ++command;
      goto next_command;
	
    case Command::IfLexSYEqIncRetFail_IfLexSYEqLeRetFail_IfLexTYEqGrRetSucc:
      /*
	if lex(S@,Y@) = Incomparable return Failure;
	if lex(S@,Y@) = Less return Failure;
	if lex(S@,Y@) = Greater return Success;
	ASSERT(lex(S@,Y@) = Equal);
      */
      lexCmp = compareLexVarComplex(command->var2(),command->complexTerm1());
      if (lexCmp == BK::Less) goto return_true;
      if (lexCmp != BK::Equal) goto return_false;
      ++command;
      goto next_command;
      
#ifdef DEBUG_NAMESPACE
    default: ICP("ICP10"); goto return_false;
#else
#ifdef _SUPPRESS_WARNINGS_
    default: goto return_false;
#endif
#endif
    };
  
 return_true:
  ASSERT((_targetRelation != Greater) || SimplificationOrdering::current()->greater(_lhs,_rhs,_subst));

  INTERCEPT_BACKWARD_CONSTR_CHECK_END1;
  return true;

 return_false:
  // v Very strong assumption!  
  ASSERT((_targetRelation != Greater) || (!SimplificationOrdering::current()->greater(_lhs,_rhs,_subst)));
  INTERCEPT_BACKWARD_CONSTR_CHECK_END1;
  return false;
}; // bool NonrecursiveKBOrdering::CompiledConstraintForInstanceRetrieval::holds()




#ifndef NO_DEBUG_VIS
ostream& NonrecursiveKBOrdering::CompiledConstraintForInstanceRetrieval::output(ostream& str) const
{
  str << "CONSTRAINT: " << _lhs;
  switch (_targetRelation)
    {
    case Greater: str << " > "; break;
    default: str << "???";
    };
  str << _rhs << "\n";
  cout << "WEIGHT COMPARISON:\n" << _weightComparison << "\n";
  str << "********* MAIN CODE: *********************\n";
  const Command* command = _mainCode.memory();
  bool printWC = false;
 print_command:
  command->output(str);
  printWC = printWC || command->requiresWeightComparison();
  if (!command->isTerminal())
    {
      ++command;
      goto print_command;
    };

  if (printWC)
    {
      str << "********* WEIGHT COMPARISON CODE: ********\n";
      WeightPolynomial::SpecialisedComparison::Command::outputCode(str,_weightComparisonCode.memory()) 
	<< "\n";
    };
  str << "*********** END OF CODE ********\n";
  return str;
};
#endif



bool NonrecursiveKBOrdering::CompiledConstraintForInstanceRetrieval::compileLexGreater(Command*& code,ulong maxCodeSize)
{
  CALL("compileLexGreater(Command*& code,ulong maxCodeSize)");
  ASSERT(maxCodeSize);
  ASSERT(_lhs->isComplex());
  ASSERT(_rhs->isComplex());
  _lhsIter.Reset(_lhs);
  _rhsIter.Reset(_rhs);
  Command* codeStart = code;
  bool tryNext;
  do
    {
      if ((code - codeStart) + 1 == (long)maxCodeSize)
	{
	  // enough space for one command only
	  code->setTag(Command::IfLexSTEqIncRetFail_IfLexSTEqLeRetFail_RetSucc);
	  code->setComplexTerm1(_lhs);
	  code->setComplexTerm2(_rhs);
	  ++code;
	  return false;
	};

      ASSERT((code - codeStart) + 1 < (long)maxCodeSize);
      
      if (_lhsIter.symbolRef() != _rhsIter.symbolRef())
	{
	  if (_lhsIter.CurrentSymIsVar())
	    {
	      if (_rhsIter.CurrentSymIsVar())
		{
		  code->setTag(Command::IfLexXYEqIncRetFail_IfLexXYEqLeRetFail_IfLexXYEqGrRetSucc);

#ifndef NO_DEBUG
		  code->setVarNum1(_lhsIter.symbolRef().var());
		  code->setVarNum2(_rhsIter.symbolRef().var());
#endif
		  code->setVar1(_subst->nativeVar(_lhsIter.symbolRef().var()));
		  code->setVar2(_subst->nativeVar(_rhsIter.symbolRef().var()));
		  ++code; 
		  tryNext = _lhsIter.Next();
		  _rhsIter.Next();
		}
	      else // _lhsIter.CurrentSymIsVar(), but !_rhsIter.CurrentSymIsVar()
		{
		  code->setTag(Command::IfLexXTEqIncRetFail_IfLexXTEqLeRetFail_IfLexXTEqGrRetSucc);

#ifndef NO_DEBUG
		  code->setVarNum1(_lhsIter.symbolRef().var());
#endif
		  code->setVar1(_subst->nativeVar(_lhsIter.symbolRef().var()));
		  ASSERT(_rhsIter.CurrPos()->isComplex());
		  code->setComplexTerm2(_rhsIter.CurrPos());
		  ++code;
		  tryNext = _lhsIter.Next();
		  _rhsIter.SkipTerm();		  
		};
	    }
	  else
	    if (_rhsIter.CurrentSymIsVar())
	      {
		// !_lhsIter.CurrentSymIsVar(), but _rhsIter.CurrentSymIsVar()
		code->setTag(Command::IfLexSYEqIncRetFail_IfLexSYEqLeRetFail_IfLexTYEqGrRetSucc);
		ASSERT(_lhsIter.CurrPos()->isComplex());
		code->setComplexTerm1(_lhsIter.CurrPos());

#ifndef NO_DEBUG
		code->setVarNum2(_rhsIter.symbolRef().var());
#endif
		code->setVar2(_subst->nativeVar(_rhsIter.symbolRef().var()));
		++code;
		
		tryNext = _lhsIter.SkipTerm();
		_rhsIter.Next();
	      }
	    else // !_lhsIter.CurrentSymIsVar() and !_rhsIter.CurrentSymIsVar()
	      {	        
		// stop here completely
		if (_lhsIter.symbolRef().hasGreaterPrecedenceThan(_rhsIter.symbolRef()))
		  {
		    code->setTag(Command::RetSuccess);
		    ++code;
		  }
		else
		  {
		    code->setTag(Command::RetFailure);
		    ++code;
		  };
		return true;
	      };
	}
      else // _lhsIter.symbolRef() == _rhsIter.symbolRef()
	{
	  tryNext = _lhsIter.Next();
	  _rhsIter.Next();
	};
    }
  while (tryNext);

  ASSERT((code - codeStart) + 1 <= (long)maxCodeSize);
  code->setTag(Command::RetFailure); // equal instances
  ++code;
  return true;
}; // bool NonrecursiveKBOrdering::CompiledConstraintForInstanceRetrieval::compileLexGreater(Command*& code,ulong maxCodeSize)




Comparison 
NonrecursiveKBOrdering::CompiledConstraintForInstanceRetrieval::compareLexComplexComplex(const TERM* complexTerm1,
											 const TERM* complexTerm2)
{
  CALL("compareLexComplexComplex(const TERM* complexTerm1,const TERM* complexTerm2)");
  ASSERT(complexTerm1->isComplex());
  ASSERT(complexTerm2->isComplex());
  _instance1.reset(complexTerm1,_subst);
  _instance2.reset(complexTerm2,_subst);
  do
    {
      if (_instance1.symbol() != _instance2.symbol())
	{
	  if (_instance1.symbol().isVariable() || _instance2.symbol().isVariable())
	    return BK::Incomparable;
	  return _instance1.symbol().comparePrecedence(_instance2.symbol());
	};
      _instance2.next();
    }
  while (_instance1.next());
  return BK::Equal;
}; // Comparison NonrecursiveKBOrdering::CompiledConstraintForInstanceRetrieval::compareLexComplexComplex(const TERM* complexTerm1,const TERM* complexTerm2)

Comparison 
NonrecursiveKBOrdering::CompiledConstraintForInstanceRetrieval::compareLexVarComplex(const TERM* var,
										     const TERM* complexTerm)
{
  CALL("compareLexVarComplex(const TERM* var,const TERM* complexTerm)");
  ASSERT(complexTerm->isComplex());
  if (var->isVariable()) return BK::Incomparable;
  ASSERT(var->IsReference());
  _lhsIter.Reset(var->First());
  _instance2.reset(complexTerm,_subst);
  do
    {     
      if (_lhsIter.symbolRef() != _instance2.symbol())
	{
	  if (_lhsIter.CurrentSymIsVar() || _instance2.symbol().isVariable())
	    return BK::Incomparable;
	  return _lhsIter.symbolRef().comparePrecedence(_instance2.symbol());
	};
      _instance2.next();
    }
  while (_lhsIter.Next());
  return BK::Equal;
}; // Comparison NonrecursiveKBOrdering::CompiledConstraintForInstanceRetrieval::compareLexVarComplex(const TERM* var,const TERM* complexTerm)

Comparison 
NonrecursiveKBOrdering::CompiledConstraintForInstanceRetrieval::compareLexVarVar(const TERM* var1,
										 const TERM* var2)
{
  CALL("compareLexVarVar(const TERM* var1,const TERM* var2)");
  if ((*var1) == (*var2)) return BK::Equal;
  if (var1->isVariable() || var2->isVariable()) return BK::Incomparable;
  ASSERT(var1->IsReference());
  ASSERT(var2->IsReference());
  _lhsIter.Reset(var1->First());
  _rhsIter.Reset(var2->First());
  do
    {
      if (_lhsIter.symbolRef() != _rhsIter.symbolRef())
	{
	  if (_lhsIter.CurrentSymIsVar() || _rhsIter.CurrentSymIsVar()) return BK::Incomparable;
	  return _lhsIter.symbolRef().comparePrecedence(_rhsIter.symbolRef());
	};
      _rhsIter.Next();
    }
  while (_lhsIter.Next());
  return BK::Equal;
}; // Comparison NonrecursiveKBOrdering::CompiledConstraintForInstanceRetrieval::compareLexVarVar(const TERM* var1,const TERM* var2)



inline
FunctionComparison 
NonrecursiveKBOrdering::CompiledConstraintForInstanceRetrieval::interpretWeightComparisonCode()
{
  CALL("interpretWeightComparisonCode()");
  const WeightComparisonCommand* command = _weightComparisonCode.memory();

 next_command:

  //DF; command->outputShort(cout << "DO ") << "\n"; 

  switch (command->tag())
    {
      
    case CompiledConstraintForInstanceRetrieval::WeightComparisonCommand::RetFailureToCompare: return BK::FailureToCompare;
    case CompiledConstraintForInstanceRetrieval::WeightComparisonCommand::RetAlwaysLess: ICP("ICP10"); return BK::FailureToCompare;
    case CompiledConstraintForInstanceRetrieval::WeightComparisonCommand::RetAlwaysEqual: return BK::AlwaysEqual;
    case CompiledConstraintForInstanceRetrieval::WeightComparisonCommand::RetAlwaysGreater: return BK::AlwaysGreater;
    case CompiledConstraintForInstanceRetrieval::WeightComparisonCommand::RetCanBeLessOrEqual: ICP("ICP20"); return BK::FailureToCompare;
    case CompiledConstraintForInstanceRetrieval::WeightComparisonCommand::RetCanBeGreaterOrEqual: return BK::CanBeGreaterOrEqual;
    case CompiledConstraintForInstanceRetrieval::WeightComparisonCommand::RetVolatileButNearlyAlwaysLess: ICP("ICP30");  return BK::FailureToCompare;
    case CompiledConstraintForInstanceRetrieval::WeightComparisonCommand::RetVolatileButNearlyAlwaysGreater: ICP("ICP40"); return BK::FailureToCompare;
    case CompiledConstraintForInstanceRetrieval::WeightComparisonCommand::RetAbsolutelyVolatile: ICP("ICP50"); return BK::FailureToCompare;

      //----------------------------------------

    case CompiledConstraintForInstanceRetrieval::WeightComparisonCommand::IfXGr1RetFail_IfGrndRetAlwEq_RetFail:
      /*
	if min|X@| > 1 return FailureToCompare;
	if ground(X@) return AlwaysEqual;
	return FailureToCompare;
      */
      {
	if ((static_cast<const TERM*>(command->nativeVar()))->isVariable())
	  return BK::FailureToCompare; // nonground
	
	
	ASSERT((static_cast<const TERM*>(command->nativeVar()))->IsReference());
	TermWeightType w = 
	  TermWeighting::current()->computeWeightIfGroundAndLessOrEqual((static_cast<const TERM*>(command->nativeVar()))->First(),
							     
									static_cast<TermWeightType>(1));
	if (w <= 0) return BK::FailureToCompare;
	return BK::AlwaysEqual;
      };

    case CompiledConstraintForInstanceRetrieval::WeightComparisonCommand::IfXGr1RetFail_IfNotGrndRetFail:
      /*
	if min|X@| > 1 return FailureToCompare;
	if !ground(X@) return FailureToCompare;
      */
      {
	if ((static_cast<const TERM*>(command->nativeVar()))->isVariable())
	  return BK::FailureToCompare; // nonground
	ASSERT((static_cast<const TERM*>(command->nativeVar()))->IsReference());
	TermWeightType w = 
	  TermWeighting::current()->computeWeightIfGroundAndLessOrEqual((static_cast<const TERM*>(command->nativeVar()))->First(),
							     
									static_cast<TermWeightType>(1));
	if (w <= 0) return BK::FailureToCompare;
      };
      break;


      //----------------------------------------



    case CompiledConstraintForInstanceRetrieval::WeightComparisonCommand::IfXGr1RetAlwGr_IfGrndRetAlwEq_RetCanBeGrOrEq:
      /*
	if min|X@| > 1 return AlwaysGreater;
	if ground(X@) return AlwaysEqual;
	return CanBeGreaterOrEqual;
      */
      {
	if ((static_cast<const TERM*>(command->nativeVar()))->isVariable())
	  return BK::CanBeGreaterOrEqual;
	
	bool ground;
	ASSERT((static_cast<const TERM*>(command->nativeVar()))->IsReference());
	TermWeightType w = 
	  TermWeighting::current()->computeMinimalInstanceWeightIfLessOrEqual((static_cast<const TERM*>(command->nativeVar()))->First(),
								   
									      static_cast<TermWeightType>(1),
									      ground);
	if (w < static_cast<TermWeightType>(0)) return BK::AlwaysGreater;
	ASSERT(w == static_cast<TermWeightType>(1));
	if (ground) return BK::AlwaysEqual;
	return BK::CanBeGreaterOrEqual;
      };

    case CompiledConstraintForInstanceRetrieval::WeightComparisonCommand::IfXGr1RetAlwGr_GrndToGRND:
      /*
	if min|X@| > 1 return AlwaysGreater;
	GRND := ground(X@);
      */
      {
	if ((static_cast<const TERM*>(command->nativeVar()))->isVariable())
	  {
	    _grnd = false;
	  }
	else
	  {
	    bool ground;
	    ASSERT((static_cast<const TERM*>(command->nativeVar()))->IsReference());
	    TermWeightType w = 
	      TermWeighting::current()->computeMinimalInstanceWeightIfLessOrEqual((static_cast<const TERM*>(command->nativeVar()))->First(),
								       
										  static_cast<TermWeightType>(1),
										  ground);
	    if (w < static_cast<TermWeightType>(0)) return BK::AlwaysGreater;
	    _grnd = ground;
	  };
      };
      break;
      

    case CompiledConstraintForInstanceRetrieval::WeightComparisonCommand::IfXGr1RetAlwGr_UpdateGRND:
      /*
	if min|X@| > 1 return AlwaysGreater;
	GRND := GRND && ground(X@);
      */
      {
	if ((static_cast<const TERM*>(command->nativeVar()))->isVariable())
	  {
	    _grnd = false;
	  }
	else
	  {
	    bool ground;
	    ASSERT((static_cast<const TERM*>(command->nativeVar()))->IsReference());
	    TermWeightType w = 
	      TermWeighting::current()->computeMinimalInstanceWeightIfLessOrEqual((static_cast<const TERM*>(command->nativeVar()))->First(),
								       
										  static_cast<TermWeightType>(1),
										  ground);
	    if (w < static_cast<TermWeightType>(0)) return BK::AlwaysGreater;
	    _grnd = _grnd && ground;
	  };
      };
      break;

    case CompiledConstraintForInstanceRetrieval::WeightComparisonCommand::IfXGr1RetAlwGr_UpdateGRND_IfGRNDRetAlwEq_RetCanBeGrOrEq:
      /*
	if min|X@| > 1 return AlwaysGreater;
	GRND := GRND && ground(X@);
	if GRND return AlwaysEqual;
	return CanBeGreaterOrEqual;
      */
      {

	if ((static_cast<const TERM*>(command->nativeVar()))->isVariable())
	  return BK::CanBeGreaterOrEqual;

	bool ground;
	ASSERT((static_cast<const TERM*>(command->nativeVar()))->IsReference());
	TermWeightType w = 
	  TermWeighting::current()->computeMinimalInstanceWeightIfLessOrEqual((static_cast<const TERM*>(command->nativeVar()))->First(),
								   
									      static_cast<TermWeightType>(1),
									      ground);
	if (w < static_cast<TermWeightType>(0)) return BK::AlwaysGreater;
	_grnd = _grnd && ground;
	if (_grnd) return BK::AlwaysEqual;
	return BK::CanBeGreaterOrEqual;
      };



      //----------------------------------------


    case CompiledConstraintForInstanceRetrieval::WeightComparisonCommand::IfNotGrndRetFail_IfXGrWRetFail_IfXLeWRetAlwGr_RetAlwEq:
      /*
	if !ground(X@) return FailureToCompare;
	if min|X@| > W return FailureToCompare;
	if min|X@| < W return AlwaysGreater;
	return AlwaysEqual;
      */
      {
	if ((static_cast<const TERM*>(command->nativeVar()))->isVariable())
	  return BK::FailureToCompare;

	ASSERT((static_cast<const TERM*>(command->nativeVar()))->IsReference());
	TermWeightType w = 
	  TermWeighting::current()->computeWeightIfGroundAndLessOrEqual((static_cast<const TERM*>(command->nativeVar()))->First(),
							     
									command->weight());
	if (w <= static_cast<TermWeightType>(0)) return BK::FailureToCompare;
	if (w < command->weight()) 
	  {
	    return BK::AlwaysGreater;
	  }
	else
	  {
	    ASSERT(w == command->weight());
	    return BK::AlwaysEqual;
	  };
      };


    case CompiledConstraintForInstanceRetrieval::WeightComparisonCommand::IfNotGrndRetFail_IfCXGrWRetFail_IfCXLeWRetAlwGr_RetAlwEq:
      /*
	if !ground(X@) return FailureToCompare;
	if C*min|X@| > W return FailureToCompare;
	if C*min|X@| < W return AlwaysGreater;
	return AlwaysEqual;
      */
      {
	
	if ((static_cast<const TERM*>(command->nativeVar()))->isVariable())
	  return BK::FailureToCompare;

	ASSERT((static_cast<const TERM*>(command->nativeVar()))->IsReference());
	TermWeightType w = 
	  TermWeighting::current()->computeWeightIfGroundAndLessOrEqual(command->coefficient(),
									(static_cast<const TERM*>(command->nativeVar()))->First(),
							     
									command->weight());
	if (w <= static_cast<TermWeightType>(0)) return BK::FailureToCompare;
	if (w < command->weight()) 
	  {
	    return BK::AlwaysGreater;
	  }
	else
	  {
	    ASSERT(w == command->weight());
	    return BK::AlwaysEqual;
	  };
      };



    case CompiledConstraintForInstanceRetrieval::WeightComparisonCommand::IfNotGrndRetFail_WMinusXToACC_IfACCLe0RetFail:
      /*
	if !ground(X@) return FailureToCompare;
	ACC := W - min|X@|;
	if ACC < 0 return FailureToCompare;      
      */
      {
	if ((static_cast<const TERM*>(command->nativeVar()))->isVariable())
	  return BK::FailureToCompare;

	ASSERT((static_cast<const TERM*>(command->nativeVar()))->IsReference());
	TermWeightType w = 
	  TermWeighting::current()->computeWeightIfGroundAndLessOrEqual((static_cast<const TERM*>(command->nativeVar()))->First(),
							     
									command->weight());
	if (w <= static_cast<TermWeightType>(0)) return BK::FailureToCompare;
	_acc = command->weight() - w;
	ASSERT(_acc >= static_cast<TermWeightType>(0));
      };
      break;



    case CompiledConstraintForInstanceRetrieval::WeightComparisonCommand::IfNotGrndRetFail_WMinusCXToACC_IfACCLe0RetFail:
      /*
	if !ground(X@) return FailureToCompare;
	ACC := W - C*min|X@|;
	if ACC < 0 return FailureToCompare;      
      */
      {
	if ((static_cast<const TERM*>(command->nativeVar()))->isVariable())
	  return BK::FailureToCompare;

	ASSERT((static_cast<const TERM*>(command->nativeVar()))->IsReference());
	TermWeightType w = 
	  TermWeighting::current()->computeWeightIfGroundAndLessOrEqual(command->coefficient(),
									(static_cast<const TERM*>(command->nativeVar()))->First(),
							     
									command->weight());
	if (w <= static_cast<TermWeightType>(0)) return BK::FailureToCompare;
	_acc = command->weight() - w;
	ASSERT(_acc >= static_cast<TermWeightType>(0));
      };
      break;


    case CompiledConstraintForInstanceRetrieval::WeightComparisonCommand::IfNotGrndRetFail_ACCMinusXPlus1ToACC_IfACCLe0RetFail:
      /*
	if !ground(X@) return FailureToCompare;
	ACC := ACC - min|X@| + 1;
	if ACC < 0 return FailureToCompare;
      */    
      {
	if ((static_cast<const TERM*>(command->nativeVar()))->isVariable())
	  return BK::FailureToCompare;

	++_acc;
	ASSERT((static_cast<const TERM*>(command->nativeVar()))->IsReference());
	TermWeightType w = 
	  TermWeighting::current()->computeWeightIfGroundAndLessOrEqual((static_cast<const TERM*>(command->nativeVar()))->First(),
							     
									_acc);
	if (w <= static_cast<TermWeightType>(0)) return BK::FailureToCompare;
	_acc -= w;
	ASSERT(_acc >= static_cast<TermWeightType>(0));
      };
      break;
    

    case CompiledConstraintForInstanceRetrieval::WeightComparisonCommand::IfNotGrndRetFail_ACCMinusCXPlusCToACC_IfACCLe0RetFail:
      /*
	if !ground(X@) return FailureToCompare;
	ACC := ACC - C*min|X@| + C;
	if ACC < 0 return FailureToCompare; 
      */    
      {
	if ((static_cast<const TERM*>(command->nativeVar()))->isVariable())
	  return BK::FailureToCompare;

	_acc += command->coefficient();
	ASSERT((static_cast<const TERM*>(command->nativeVar()))->IsReference());
	TermWeightType w = TermWeighting::current()->computeWeightIfGroundAndLessOrEqual(command->coefficient(),
												      (static_cast<const TERM*>(command->nativeVar()))->First(),
								    
												      _acc);
	if (w <= static_cast<TermWeightType>(0)) return BK::FailureToCompare;
	_acc -= w;
	ASSERT(_acc >= static_cast<TermWeightType>(0));
      };
      break;
      


    case CompiledConstraintForInstanceRetrieval::WeightComparisonCommand::IfNotGrndRetFail_ACCMinusXPlus1ToACC_IfACCLe0RetFail_IfACCEq0RetAlwEq_RetAlwGr:
      /*
	if !ground(X@) return FailureToCompare;
	ACC := ACC - min|X@| + 1;
	if ACC < 0 return FailureToCompare; 
	if ACC = 0 return AlwaysEqual;
	return AlwaysGreater;
      */    
      {
	if ((static_cast<const TERM*>(command->nativeVar()))->isVariable())
	  return BK::FailureToCompare;

	++_acc;
	ASSERT((static_cast<const TERM*>(command->nativeVar()))->IsReference());
	TermWeightType w = 
	  TermWeighting::current()->computeWeightIfGroundAndLessOrEqual((static_cast<const TERM*>(command->nativeVar()))->First(),
							     
									_acc);
	if (w <= static_cast<TermWeightType>(0)) return BK::FailureToCompare;
	if (_acc == w) return BK::AlwaysEqual;
	return BK::AlwaysGreater;
      };


    case CompiledConstraintForInstanceRetrieval::WeightComparisonCommand::IfNotGrndRetFail_ACCMinusCXPlusCToACC_IfACCLe0RetFail_IfACCEq0RetAlwEq_RetAlwGr:
      /*
	if !ground(X@) return FailureToCompare;
	ACC := ACC - C*min|X@| + C;
	if ACC < 0 return FailureToCompare; 
	if ACC = 0 return AlwaysEqual;
	return AlwaysGreater;
      */    
      {
	if ((static_cast<const TERM*>(command->nativeVar()))->isVariable())
	  return BK::FailureToCompare;
	ASSERT((static_cast<const TERM*>(command->nativeVar()))->IsReference());


	_acc += command->coefficient(); 
	TermWeightType w = 
	  TermWeighting::current()->computeWeightIfGroundAndLessOrEqual(command->coefficient(),
									(static_cast<const TERM*>(command->nativeVar()))->First(),
							     
									_acc);
	if (w <= static_cast<TermWeightType>(0)) return BK::FailureToCompare;
	if (_acc == w) return BK::AlwaysEqual;
	return BK::AlwaysGreater;
      };


      //----------------------------------------



    case CompiledConstraintForInstanceRetrieval::WeightComparisonCommand::IfXGrWRetAlwGr_IfXLeWRetFail_IfGrndRetAlwEq_RetCanBeGrOrEq:
      /*
	ASSERT(W > static_cast<TermWeightType>(0));
	if min|X@| > W return AlwaysGreater;
	if min|X@| < W return FailureToCompare;
	if ground(X@) return AlwaysEqual;
	return CanBeGreaterOrEqual;
      */
      {
	ASSERT(command->weight() > static_cast<TermWeightType>(0));
	
	if ((static_cast<const TERM*>(command->nativeVar()))->isVariable())
	  {
	    // min|X@| == 1, ground(X@) == false
	    if (static_cast<TermWeightType>(1) < command->weight()) return BK::FailureToCompare;
	    return BK::CanBeGreaterOrEqual;
	  }
	else
	  {
	    ASSERT((static_cast<const TERM*>(command->nativeVar()))->IsReference());
	    bool ground;
	    TermWeightType w = 
	      TermWeighting::current()->computeMinimalInstanceWeightIfLessOrEqual((static_cast<const TERM*>(command->nativeVar()))->First(),
								       
										  command->weight(),
										  ground);

	   
	    if (w < static_cast<TermWeightType>(0)) return BK::AlwaysGreater;
	    ASSERT((w > static_cast<TermWeightType>(0)) && (w <= command->weight()));
	    if (w < command->weight()) return BK::FailureToCompare;
	    if (ground) return BK::AlwaysEqual;
	    return BK::CanBeGreaterOrEqual;
	  };
      };
      
    
    case CompiledConstraintForInstanceRetrieval::WeightComparisonCommand::IfCXGrWRetAlwGr_IfCXLeWRetFail_IfGrndRetAlwEq_RetCanBeGrOrEq:
      /*
	ASSERT(W > static_cast<TermWeightType>(0));
	if C*min|X@| > W return AlwaysGreater;
	if C*min|X@| < W return FailureToCompare;
	if ground(X@) return AlwaysEqual;
	return CanBeGreaterOrEqual;
      */
      {
	ASSERT(command->weight() > static_cast<TermWeightType>(0));

	if ((static_cast<const TERM*>(command->nativeVar()))->isVariable())
	  {
	    // min|X@| == 1, ground(X@) == false
	    if (command->coefficient() < command->weight()) return BK::FailureToCompare;
	    return BK::CanBeGreaterOrEqual;
	  }
	else
	  {
	    ASSERT((static_cast<const TERM*>(command->nativeVar()))->IsReference());
	    bool ground;
	    TermWeightType w = 
	      TermWeighting::current()->computeMinimalInstanceWeightIfLessOrEqual(command->coefficient(),
										  (static_cast<const TERM*>(command->nativeVar()))->First(),
								       
										  command->weight(),
										  ground);
	    if (w < static_cast<TermWeightType>(0)) return BK::AlwaysGreater;
	    ASSERT((w > static_cast<TermWeightType>(0)) && (w <= command->weight()));
	    if (w < command->weight()) return BK::FailureToCompare;
	    if (ground) return BK::AlwaysEqual;
	    return BK::CanBeGreaterOrEqual;
	  };
      };


    case CompiledConstraintForInstanceRetrieval::WeightComparisonCommand::WMinusXToACC_IfACCLe0RetAlwGr_GrndToGRND:
      /*
	ACC := W - min|X@|;
	if ACC < 0 return AlwaysGreater;
	GRND := ground(X@);
      */
      {
	if ((static_cast<const TERM*>(command->nativeVar()))->isVariable())
	  {
	    _acc = command->weight() - static_cast<TermWeightType>(1);
	    if (_acc < static_cast<TermWeightType>(0)) return BK::AlwaysGreater;
	    _grnd = false;
	  }
	else
	  {
	    ASSERT((static_cast<const TERM*>(command->nativeVar()))->IsReference());
	    bool ground;
	    TermWeightType w = 
	      TermWeighting::current()->computeMinimalInstanceWeightIfLessOrEqual((static_cast<const TERM*>(command->nativeVar()))->First(),
								       
										  command->weight(),
										  ground);
	    if (w < static_cast<TermWeightType>(0)) return BK::AlwaysGreater;
	    _acc = command->weight() - w;
	    _grnd = ground;
	  };
	
      };
      break;

    case CompiledConstraintForInstanceRetrieval::WeightComparisonCommand::WMinusCXToACC_IfACCLe0RetAlwGr_GrndToGRND:
      /*
	ACC := W - C*min|X@|;
	if ACC <= 0 return AlwaysGreater;
	GRND := ground(X@);
      */
      {
	if ((static_cast<const TERM*>(command->nativeVar()))->isVariable())
	  {
	    _acc = command->weight() - command->coefficient();
	    if (_acc < static_cast<TermWeightType>(0)) return BK::AlwaysGreater;
	    _grnd = false;
	  }
	else
	  {
	    ASSERT((static_cast<const TERM*>(command->nativeVar()))->IsReference());
	    bool ground;
	    TermWeightType w = 
	      TermWeighting::current()->computeMinimalInstanceWeightIfLessOrEqual(command->coefficient(),
										  (static_cast<const TERM*>(command->nativeVar()))->First(),
								       
										  command->weight(),
										  ground);
	    if (w < static_cast<TermWeightType>(0)) return BK::AlwaysGreater;
	    _acc = command->weight() - w;
	    _grnd = ground;
	  };
	
      };
      break;
    
    case CompiledConstraintForInstanceRetrieval::WeightComparisonCommand::ACCMinusXPlus1ToACC_IfACCLe0RetAlwGr_UpdateGRND:
      /*
	ACC := ACC - min|X@| + 1;
	if ACC < 0 return AlwaysGreater;
	GRND := GRND && ground(X@);
      */    
      {
	if ((static_cast<const TERM*>(command->nativeVar()))->isVariable())
	  {
	    if (_acc < static_cast<TermWeightType>(0)) return BK::AlwaysGreater;
	    _grnd = false;
	  }
	else
	  {
	    ASSERT((static_cast<const TERM*>(command->nativeVar()))->IsReference());
	    ++_acc;
	    bool ground;
	    TermWeightType w = 
	      TermWeighting::current()->computeMinimalInstanceWeightIfLessOrEqual((static_cast<const TERM*>(command->nativeVar()))->First(),
								       
										  _acc,
										  ground);
	    if (w < static_cast<TermWeightType>(0)) return BK::AlwaysGreater;
	    _acc -= w;
	    ASSERT(_acc >= static_cast<TermWeightType>(0));
	    _grnd = _grnd && ground;
	  };
      };
      break;

    
    case CompiledConstraintForInstanceRetrieval::WeightComparisonCommand::ACCMinusCXPlusCToACC_IfACCLe0RetAlwGr_UpdateGRND:
      /*
	ACC := ACC - C*min|X@| + C;
	if ACC < 0 return AlwaysGreater;
	GRND := GRND && ground(X@);
      */    
      {
	if ((static_cast<const TERM*>(command->nativeVar()))->isVariable())
	  {
	    if (_acc < static_cast<TermWeightType>(0)) return BK::AlwaysGreater;
	    _grnd = false;
	  }
	else
	  {
	    ASSERT((static_cast<const TERM*>(command->nativeVar()))->IsReference());
	    _acc += command->coefficient();
	    bool ground;
	    TermWeightType w = 
	      TermWeighting::current()->computeMinimalInstanceWeightIfLessOrEqual(command->coefficient(),
										  (static_cast<const TERM*>(command->nativeVar()))->First(),
								       
										  _acc,
										  ground);
	    if (w < static_cast<TermWeightType>(0)) return BK::AlwaysGreater;
	    _acc -= w;
	    ASSERT(_acc >= static_cast<TermWeightType>(0));
	    _grnd = _grnd && ground;
	  };
      };
      break;


    case CompiledConstraintForInstanceRetrieval::WeightComparisonCommand::IfXGrACCPlus1RetAlwGr_IfXLeACCPlus1RetFail_UpdateGRND_IfGRNDRetAlwEq_RetCanBeGrOrEq:
      /*
	if min|X@| > ACC + 1 return AlwaysGreater;
	if min|X@| < ACC + 1 return FailureToCompare;      
	GRND := GRND && ground(X@);
	if GRND return AlwaysEqual;
	return CanBeGreaterOrEqual;
      */   
      {
	if ((static_cast<const TERM*>(command->nativeVar()))->isVariable())
	  {
	    if (_acc < static_cast<TermWeightType>(0)) return BK::AlwaysGreater;
	    if (_acc > static_cast<TermWeightType>(0)) return BK::FailureToCompare;  
	    return BK::CanBeGreaterOrEqual;
	  }
	else
	  {
	    ASSERT((static_cast<const TERM*>(command->nativeVar()))->IsReference());
	    ++_acc;
	    bool ground;
	    TermWeightType w = 
	      TermWeighting::current()->computeMinimalInstanceWeightIfLessOrEqual((static_cast<const TERM*>(command->nativeVar()))->First(),
								       
										  _acc,
										  ground);
	    if (w < static_cast<TermWeightType>(0)) return BK::AlwaysGreater;
	    if (w < _acc) return BK::FailureToCompare; 
	    ASSERT(w == _acc);
	    if (_grnd && ground) return BK::AlwaysEqual;
	    return BK::CanBeGreaterOrEqual;
	  };
      };
    
    case CompiledConstraintForInstanceRetrieval::WeightComparisonCommand::IfCXGrACCPlusCRetAlwGr_IfCXLeACCPlusCRetFail_UpdateGRND_IfGRNDRetAlwEq_RetCanBeGrOrEq:
      /*
	if C*min|X@| > ACC + C return AlwaysGreater;
	if C*min|X@| < ACC + C return FailureToCompare;      
	GRND := GRND && ground(X@);
	if GRND return AlwaysEqual;
	return CanBeGreaterOrEqual;
      */   
      {
	if ((static_cast<const TERM*>(command->nativeVar()))->isVariable())
	  {
	    if (_acc < static_cast<TermWeightType>(0)) return BK::AlwaysGreater;
	    if (_acc > static_cast<TermWeightType>(0)) return BK::FailureToCompare;  
	    return BK::CanBeGreaterOrEqual;
	  }
	else
	  {
	    ASSERT((static_cast<const TERM*>(command->nativeVar()))->IsReference());
	    _acc += command->coefficient();
	    bool ground;
	    TermWeightType w = 
	      TermWeighting::current()->computeMinimalInstanceWeightIfLessOrEqual(command->coefficient(),
										  (static_cast<const TERM*>(command->nativeVar()))->First(),
								       
										  _acc,
										  ground);
	    if (w < static_cast<TermWeightType>(0)) return BK::AlwaysGreater;
	    if (w < _acc) return BK::FailureToCompare; 
	    ASSERT(w == _acc);
	    if (_grnd && ground) return BK::AlwaysEqual;
	    return BK::CanBeGreaterOrEqual;
	  };
      };
    
      //----------------------------------------

    case CompiledConstraintForInstanceRetrieval::WeightComparisonCommand::XPlusWToPOSINST_IfConstPOSINSAndPOSINSTLe0RetFail_IfConstPOSINSTGotoL:
      /*
	POSINST := |X@| + W;
	if constant(POSINST) && POSINST < 0 return FailureToCompare; 
	if constant(POSINST) goto L; 
      */
      {
	_posinst.reset();
	_posinst.add(command->weight());

	if ((static_cast<const TERM*>(command->nativeVar()))->isVariable())
	  {
	    _posinst.add1((static_cast<const TERM*>(command->nativeVar()))->var());
	  }
	else
	  {
	    ASSERT((static_cast<const TERM*>(command->nativeVar()))->IsReference());	    
	    TermWeighting::current()->collectWeight(_posinst,*(static_cast<const TERM*>(command->nativeVar())));
	    if (_posinst.isConstant())
	      {
		if (_posinst.freeMember() < static_cast<TermWeightType>(0)) return BK::FailureToCompare;  
		command = command->label();
		goto next_command;
	      };
	  };
      };
      break;


    case CompiledConstraintForInstanceRetrieval::WeightComparisonCommand::CXPlusWToPOSINST_IfConstPOSINSAndPOSINSTLe0RetFail_IfConstPOSINSTGotoL:
      /*
	POSINST := C*|X@| + W;
	if constant(POSINST) && POSINST < 0 return FailureToCompare; 
	if constant(POSINST) goto L;
      */
      {
	_posinst.reset();
	_posinst.add(command->weight());

	if ((static_cast<const TERM*>(command->nativeVar()))->isVariable())
	  {
	    _posinst.add(command->coefficient(),(static_cast<const TERM*>(command->nativeVar()))->var());
	  }
	else
	  {	
	    ASSERT((static_cast<const TERM*>(command->nativeVar()))->IsReference());	    
	    TermWeighting::current()->collectWeight(_posinst,
						    command->coefficient(),
						    *(static_cast<const TERM*>(command->nativeVar())));
	    if (_posinst.isConstant())
	      {
		if (_posinst.freeMember() < static_cast<TermWeightType>(0)) return BK::FailureToCompare;  
		command = command->label();
		goto next_command;
	      };
	  };
      };
      break;


    case CompiledConstraintForInstanceRetrieval::WeightComparisonCommand::XPlusWToPOSINST:
      /*
	POSINST := |X@| + W;
      */
      {
	_posinst.reset();
	_posinst.add(command->weight());

	if ((static_cast<const TERM*>(command->nativeVar()))->isVariable())
	  {
	    _posinst.add1((static_cast<const TERM*>(command->nativeVar()))->var());
	  }
	else
	  {		
	    ASSERT((static_cast<const TERM*>(command->nativeVar()))->IsReference());	    
	    TermWeighting::current()->collectWeight(_posinst,
						    *(static_cast<const TERM*>(command->nativeVar())));
	  };
      };
      break;
    
    case CompiledConstraintForInstanceRetrieval::WeightComparisonCommand::CXPlusWToPOSINST:
      /*
	POSINST := C*|X@| + W;
      */
      {
	_posinst.reset();
	_posinst.add(command->weight());

	if ((static_cast<const TERM*>(command->nativeVar()))->isVariable())
	  {
	    _posinst.add(command->coefficient(),(static_cast<const TERM*>(command->nativeVar()))->var());
	  }
	else
	  {			
	    ASSERT((static_cast<const TERM*>(command->nativeVar()))->IsReference());	    
	    TermWeighting::current()->collectWeight(_posinst,
						    command->coefficient(),
						    *(static_cast<const TERM*>(command->nativeVar())));
	  };
      };
      break;

    case CompiledConstraintForInstanceRetrieval::WeightComparisonCommand::POSINSTPlusXToPOSINST:
      /*
	POSINST := POSINST + |X@|;
      */
      {
	if ((static_cast<const TERM*>(command->nativeVar()))->isVariable())
	  {
	    _posinst.add1((static_cast<const TERM*>(command->nativeVar()))->var());
	  }
	else
	  {				
	    ASSERT((static_cast<const TERM*>(command->nativeVar()))->IsReference());	    
	    TermWeighting::current()->collectWeight(_posinst,
						    *(static_cast<const TERM*>(command->nativeVar())));
	  };
      };
      break;
    
    case CompiledConstraintForInstanceRetrieval::WeightComparisonCommand::POSINSTPlusCXToPOSINST:
      /*
	POSINST := POSINST + C*|X@|;
      */
      {
	if ((static_cast<const TERM*>(command->nativeVar()))->isVariable())
	  {
	    _posinst.add(command->coefficient(),(static_cast<const TERM*>(command->nativeVar()))->var());
	  }
	else
	  {					
	    ASSERT((static_cast<const TERM*>(command->nativeVar()))->IsReference());	    
	    TermWeighting::current()->collectWeight(_posinst,
						    command->coefficient(),
						    *(static_cast<const TERM*>(command->nativeVar())));
	  };
      };
      break;

    
    case CompiledConstraintForInstanceRetrieval::WeightComparisonCommand::POSINSTPlusXToPOSINST_IfConstPOSINSTAndPOSINSTLe0RetFail_IfConstPOSINSTGotoL:
      /*
	POSINST := POSINST + |X@|;
	if constant(POSINST) && POSINST < 0 return FailureToCompare; 
	if constant(POSINST) goto L;
      */
      {
	if ((static_cast<const TERM*>(command->nativeVar()))->isVariable())
	  {
	    _posinst.add1((static_cast<const TERM*>(command->nativeVar()))->var());
	  }
	else
	  {						
	    ASSERT((static_cast<const TERM*>(command->nativeVar()))->IsReference());    
	    TermWeighting::current()->collectWeight(_posinst,
						    *(static_cast<const TERM*>(command->nativeVar())));
	  };
	if (_posinst.isConstant())
	  {
	    if (_posinst.freeMember() < static_cast<TermWeightType>(0)) return BK::FailureToCompare; 
	    command = command->label();
	    goto next_command;
	  };
      };
      break;

    case CompiledConstraintForInstanceRetrieval::WeightComparisonCommand::POSINSTPlusCXToPOSINST_IfConstPOSINSTAndPOSINSTLe0RetFail_IfConstPOSINSTGotoL:
      /*
	POSINST := POSINST + C*|X@|;
	if constant(POSINST) && POSINST < 0 return FailureToCompare; 
	if constant(POSINST) goto L;
      */
      {
	if ((static_cast<const TERM*>(command->nativeVar()))->isVariable())
	  {
	    _posinst.add(command->coefficient(),(static_cast<const TERM*>(command->nativeVar()))->var());
	  }
	else
	  {							
	    ASSERT((static_cast<const TERM*>(command->nativeVar()))->IsReference());      
	    TermWeighting::current()->collectWeight(_posinst,
						    command->coefficient(),
						    *(static_cast<const TERM*>(command->nativeVar())));
	  };
	if (_posinst.isConstant())
	  {
	    if (_posinst.freeMember() < static_cast<TermWeightType>(0)) return BK::FailureToCompare; 
	    command = command->label();
	    goto next_command;
	  };
      };
      break;

    case CompiledConstraintForInstanceRetrieval::WeightComparisonCommand::XMinus1ToNEGINSTRetCompForGrOrEqPOSINSTvNEGINST:
      /*
	NEGINST := |X@| - 1;
	return compareForGreaterOrEqual(POSINST,NEGINST);
      */
      {
	_neginst.reset();
	_neginst.add(static_cast<TermWeightType>(-1));

	if ((static_cast<const TERM*>(command->nativeVar()))->isVariable())
	  {
	    _neginst.add1((static_cast<const TERM*>(command->nativeVar()))->var());
	  }
	else
	  {							
	    ASSERT((static_cast<const TERM*>(command->nativeVar()))->IsReference()); 
	    TermWeighting::current()->collectWeight(_neginst,
						    *(static_cast<const TERM*>(command->nativeVar())));
	  };
	return _posinst.compareForGreaterOrEqual(_neginst);
      };
    
    case CompiledConstraintForInstanceRetrieval::WeightComparisonCommand::CXMinusCToNEGINSTRetCompForGrOrEqPOSINSTvNEGINST:
      /*
	NEGINST := C*|X@| - C;
	return compareForGreaterOrEqual(POSINST,NEGINST);
      */
      {
	_neginst.reset();
	_neginst.add(-(command->coefficient()));

	if ((static_cast<const TERM*>(command->nativeVar()))->isVariable())
	  {
	    _neginst.add(command->coefficient(),(static_cast<const TERM*>(command->nativeVar()))->var());
	  }
	else
	  {							
	    ASSERT((static_cast<const TERM*>(command->nativeVar()))->IsReference()); 
	    TermWeighting::current()->collectWeight(_neginst,
						    command->coefficient(),
						    *(static_cast<const TERM*>(command->nativeVar())));
	  };
	return _posinst.compareForGreaterOrEqual(_neginst);
      };


    case CompiledConstraintForInstanceRetrieval::WeightComparisonCommand::XMinus1ToNEGINST_IfCompForGrOrEqPOSINSTvNEGINSTEqFailRetFail:
      /*
	NEGINST := |X@| - 1;
	if compareForGreaterOrEqual(POSINST,NEGINST) == FailureToCompare return FailureToCompare;
      */
      {
	_neginst.reset();
	_neginst.add(static_cast<TermWeightType>(-1));

	if ((static_cast<const TERM*>(command->nativeVar()))->isVariable())
	  {
	    _neginst.add1((static_cast<const TERM*>(command->nativeVar()))->var());
	  }
	else
	  {							
	    ASSERT((static_cast<const TERM*>(command->nativeVar()))->IsReference()); 
	    TermWeighting::current()->collectWeight(_neginst,*(static_cast<const TERM*>(command->nativeVar())));
	  };
	if (_posinst.compareForGreaterOrEqual(_neginst) == BK::FailureToCompare) 
	  return BK::FailureToCompare;
      };
      break;

    case CompiledConstraintForInstanceRetrieval::WeightComparisonCommand::CXMinusCToNEGINST_IfCompForGrOrEqPOSINSTvNEGINSTEqFailRetFail:
      /*
	NEGINST := C*|X@| - C;
	if compareForGreaterOrEqual(POSINST,NEGINST) == FailureToCompare return FailureToCompare;
      */
      {
	_neginst.reset();
	_neginst.add(-(command->coefficient()));

	if ((static_cast<const TERM*>(command->nativeVar()))->isVariable())
	  {
	    _neginst.add(command->coefficient(),(static_cast<const TERM*>(command->nativeVar()))->var());
	  }
	else
	  {							
	    ASSERT((static_cast<const TERM*>(command->nativeVar()))->IsReference()); 
	    TermWeighting::current()->collectWeight(_neginst,
						    command->coefficient(),
						    *(static_cast<const TERM*>(command->nativeVar())));
	  };
	if (_posinst.compareForGreaterOrEqual(_neginst) == BK::FailureToCompare) 
	  return BK::FailureToCompare;
      };
      break;

    case CompiledConstraintForInstanceRetrieval::WeightComparisonCommand::NEGINSTPlusXMinus1ToNEGINST_IfCompForGrOrEqPOSINSTvNEGINSTEqFailRetFail:
      /*
	NEGINST := NEGINST + |X@| - 1;
	if compareForGreaterOrEqual(POSINST,NEGINST) == FailureToCompare return FailureToCompare;
      */
      {
	_neginst.add(static_cast<TermWeightType>(-1));

	if ((static_cast<const TERM*>(command->nativeVar()))->isVariable())
	  {
	    _neginst.add1((static_cast<const TERM*>(command->nativeVar()))->var());
	  }
	else
	  {							
	    ASSERT((static_cast<const TERM*>(command->nativeVar()))->IsReference()); 
	    TermWeighting::current()->collectWeight(_neginst,
						    *(static_cast<const TERM*>(command->nativeVar())));
	  };
	if (_posinst.compareForGreaterOrEqual(_neginst) == BK::FailureToCompare) 
	  return BK::FailureToCompare;
      };
      break;

    case CompiledConstraintForInstanceRetrieval::WeightComparisonCommand::NEGINSTPlusCXMinusCToNEGINST_IfCompForGrOrEqPOSINSTvNEGINSTEqFailRetFail:
      /*
	NEGINST := NEGINST + C*|X@| - C;
	if compareForGreaterOrEqual(POSINST,NEGINST) == FailureToCompare return FailureToCompare;
      */
      {
	_neginst.add(-(command->coefficient()));

	if ((static_cast<const TERM*>(command->nativeVar()))->isVariable())
	  {
	    _neginst.add(command->coefficient(),(static_cast<const TERM*>(command->nativeVar()))->var());
	  }
	else
	  {							
	    ASSERT((static_cast<const TERM*>(command->nativeVar()))->IsReference());
	    TermWeighting::current()->collectWeight(_neginst,
						    command->coefficient(),
						    *(static_cast<const TERM*>(command->nativeVar())));
	  };
	if (_posinst.compareForGreaterOrEqual(_neginst) == BK::FailureToCompare) 
	  return BK::FailureToCompare;
      };
      break;

    case CompiledConstraintForInstanceRetrieval::WeightComparisonCommand::NEGINSTPlusXMinus1ToNEGINST_RetCompForGrOrEqPOSINSTvNEGINST:
      /*
	NEGINST := NEGINST + |X@| - 1;
	return compareForGreaterOrEqual(POSINST,NEGINST);
      */
      {
	_neginst.add(static_cast<TermWeightType>(-1));

	if ((static_cast<const TERM*>(command->nativeVar()))->isVariable())
	  {
	    _neginst.add1((static_cast<const TERM*>(command->nativeVar()))->var());
	  }
	else
	  {							
	    ASSERT((static_cast<const TERM*>(command->nativeVar()))->IsReference());
	    TermWeighting::current()->collectWeight(_neginst,
						    *(static_cast<const TERM*>(command->nativeVar())));
	  };
	return _posinst.compareForGreaterOrEqual(_neginst);
      };

    case CompiledConstraintForInstanceRetrieval::WeightComparisonCommand::NEGINSTPlusCXMinusCToNEGINST_RetCompForGrOrEqPOSINSTvNEGINST:
      /*
	NEGINST := NEGINST + C*|X@| - C;
	return compareForGreaterOrEqual(POSINST,NEGINST);
      */
      {
	_neginst.add(-(command->coefficient()));

	if ((static_cast<const TERM*>(command->nativeVar()))->isVariable())
	  {
	    _neginst.add(command->coefficient(),(static_cast<const TERM*>(command->nativeVar()))->var());
	  }
	else
	  {							
	    ASSERT((static_cast<const TERM*>(command->nativeVar()))->IsReference());
	    TermWeighting::current()->collectWeight(_neginst,
						    command->coefficient(),
						    *(static_cast<const TERM*>(command->nativeVar())));
	  };
	return _posinst.compareForGreaterOrEqual(_neginst);
      };


    case CompiledConstraintForInstanceRetrieval::WeightComparisonCommand::IfNotGrndRetFail_IfXMinus1GrPOSINSTRetFail_IfXMinus1LePOSINSTRetAlwGr_RetAlwEq:
      /*
	ASSERT(constant(POSINST));
	ASSERT(POSINST >= 0);
	if !ground(X@) return FailureToCompare;
	if min|X@| - 1 > POSINST return FailureToCompare;
	if min|X@| - 1 < POSINST return AlwaysGreater;
	return AlwaysEqual;
      */
      {
	ASSERT(_posinst.isConstant());
	ASSERT(_posinst.freeMember() >= static_cast<TermWeightType>(0));

	if ((static_cast<const TERM*>(command->nativeVar()))->isVariable())
	  {
	    return BK::FailureToCompare;
	  }
	else
	  {							
	    ASSERT((static_cast<const TERM*>(command->nativeVar()))->IsReference());
	    _posinst.add1();
	    TermWeightType w = 
	      TermWeighting::current()->computeWeightIfGroundAndLessOrEqual((static_cast<const TERM*>(command->nativeVar()))->First(),
								 
									    _posinst.freeMember());
	    if (w <= static_cast<TermWeightType>(0)) return BK::FailureToCompare;
	    ASSERT(w <= _posinst.freeMember());
	    if (w < _posinst.freeMember()) return BK::AlwaysGreater;
	    return BK::AlwaysEqual;
	  };
      };

    case CompiledConstraintForInstanceRetrieval::WeightComparisonCommand::IfNotGrndRetFail_IfCXMinusCGrPOSINSTRetFail_IfCXMinusCLePOSINSTRetAlwGr_RetAlwEq:
      /*
	ASSERT(constant(POSINST));
	ASSERT(POSINST >= 0);
	if !ground(X@) return FailureToCompare;
	if C*min|X@| - C > POSINST return FailureToCompare;
	if C*min|X@| - C < POSINST return AlwaysGreater;
	return AlwaysEqual;
      */
      {
	ASSERT(_posinst.isConstant());
	ASSERT(_posinst.freeMember() >= static_cast<TermWeightType>(0));

	if ((static_cast<const TERM*>(command->nativeVar()))->isVariable())
	  {
	    return BK::FailureToCompare;
	  }
	else
	  {
	    ASSERT((static_cast<const TERM*>(command->nativeVar()))->IsReference());
	    _posinst.add(command->coefficient());
	    TermWeightType w = 
	      TermWeighting::current()->computeWeightIfGroundAndLessOrEqual(command->coefficient(),
									    (static_cast<const TERM*>(command->nativeVar()))->First(),
								 
									    _posinst.freeMember());
	    if (w <= static_cast<TermWeightType>(0)) return BK::FailureToCompare;
	    ASSERT(w <= _posinst.freeMember());
	    if (w < _posinst.freeMember()) return BK::AlwaysGreater;
	    return BK::AlwaysEqual;
	  };
      };


    case CompiledConstraintForInstanceRetrieval::WeightComparisonCommand::IfNotGrndRetFail_POSINSTPlus1MinusXToACC_IfACCLe0RetFail:
      /*
	ASSERT(constant(POSINST));
	ASSERT(POSINST >= 0);
	if !ground(X@) return FailureToCompare;
	ACC := POSINST + 1 - min|X@|;      
	if ACC < 0 return FailureToCompare; 
      */
      {	
	ASSERT(_posinst.isConstant());
	ASSERT(_posinst.freeMember() >= static_cast<TermWeightType>(0));

	if ((static_cast<const TERM*>(command->nativeVar()))->isVariable())
	  {
	    return BK::FailureToCompare;
	  }
	else
	  {	 
	    ASSERT((static_cast<const TERM*>(command->nativeVar()))->IsReference());   
	    _acc = _posinst.freeMember() + 1;
	    TermWeightType w = 
	      TermWeighting::current()->computeWeightIfGroundAndLessOrEqual((static_cast<const TERM*>(command->nativeVar()))->First(),
								 
									    _acc);
	    if (w <= static_cast<TermWeightType>(0)) return BK::FailureToCompare;
	    _acc -= w;
	    ASSERT(_acc >= static_cast<TermWeightType>(0));
	  };
      };
      break;

    case CompiledConstraintForInstanceRetrieval::WeightComparisonCommand::IfNotGrndRetFail_POSINSTPlusCMinusCXToACC_IfACCLe0RetFail:
      /*
	ASSERT(constant(POSINST));
	ASSERT(POSINST >= 0);
	if !ground(X@) return FailureToCompare;
	ACC := POSINST + C - C*min|X@|;      
	if ACC < 0 return FailureToCompare; 
      */
      {	
	ASSERT(_posinst.isConstant());
	ASSERT(_posinst.freeMember() >= static_cast<TermWeightType>(0));

	if ((static_cast<const TERM*>(command->nativeVar()))->isVariable())
	  {
	    return BK::FailureToCompare;
	  }
	else
	  {		 
	    ASSERT((static_cast<const TERM*>(command->nativeVar()))->IsReference());       
	    _acc = _posinst.freeMember() + command->coefficient();
	    TermWeightType w = 
	      TermWeighting::current()->computeWeightIfGroundAndLessOrEqual(command->coefficient(),
									    (static_cast<const TERM*>(command->nativeVar()))->First(),
								 
									    _acc);
	    if (w <= static_cast<TermWeightType>(0)) return BK::FailureToCompare;
	    _acc -= w;
	    ASSERT(_acc >= static_cast<TermWeightType>(0));
	  };
      };
      break;




    case CompiledConstraintForInstanceRetrieval::WeightComparisonCommand::XPlusWToNEGINST_IfConstNEGINSTAndNEGINSTLe0RetAlwGr_IfConstNEGINSTGotoL:
      /*
	NEGINST := |X@| + W;
	if constant(NEGINST) && NEGINST < 0 return AlwaysGreater;
	if constant(NEGINST) goto L;
      */
      {
	_neginst.reset();
	_neginst.add(command->weight());

	if ((static_cast<const TERM*>(command->nativeVar()))->isVariable())
	  {
	    _neginst.add1((static_cast<const TERM*>(command->nativeVar()))->var());
	  }
	else
	  {		 
	    ASSERT((static_cast<const TERM*>(command->nativeVar()))->IsReference());   
	    TermWeighting::current()->collectWeight(_neginst,
						    *(static_cast<const TERM*>(command->nativeVar())));
	    if (_neginst.isConstant())
	      {
		if (_neginst.freeMember() < static_cast<TermWeightType>(0)) return BK::AlwaysGreater;
		command = command->label();
		goto next_command;
	      };
	  };
      };
      break;
     


    case CompiledConstraintForInstanceRetrieval::WeightComparisonCommand::CXPlusWToNEGINST_IfConstNEGINSTAndNEGINSTLe0RetAlwGr_IfConstNEGINSTGotoL:
      /*
	NEGINST := C*|X@| + W;
	if constant(NEGINST) && NEGINST < 0 return AlwaysGreater;
	if constant(NEGINST) goto L;
      */
      {
	_neginst.reset();
	_neginst.add(command->weight());

	if ((static_cast<const TERM*>(command->nativeVar()))->isVariable())
	  {
	    _neginst.add(command->coefficient(),(static_cast<const TERM*>(command->nativeVar()))->var());
	  }
	else
	  {		 
	    ASSERT((static_cast<const TERM*>(command->nativeVar()))->IsReference());  
	    TermWeighting::current()->collectWeight(_neginst,
						    command->coefficient(),
						    *(static_cast<const TERM*>(command->nativeVar())));
	    if (_neginst.isConstant())
	      {
		if (_neginst.freeMember() < static_cast<TermWeightType>(0)) return BK::AlwaysGreater;
		command = command->label();
		goto next_command;
	      };
	  };
      };
      break;


    case CompiledConstraintForInstanceRetrieval::WeightComparisonCommand::XPlusWToNEGINST:
      /*
	NEGINST := |X@| + W;
      */
      {
	_neginst.reset();
	_neginst.add(command->weight());
	
	
	if ((static_cast<const TERM*>(command->nativeVar()))->isVariable())
	  {
	    _neginst.add1((static_cast<const TERM*>(command->nativeVar()))->var());
	  }
	else
	  {		 
	    ASSERT((static_cast<const TERM*>(command->nativeVar()))->IsReference());
	    TermWeighting::current()->collectWeight(_neginst,
						    *(static_cast<const TERM*>(command->nativeVar())));
	  };
      };
      break;

    
    case CompiledConstraintForInstanceRetrieval::WeightComparisonCommand::CXPlusWToNEGINST:
      /*
	NEGINST := C*|X@| + W;
      */
      {
	_neginst.reset();
	_neginst.add(command->weight());
	
	
	if ((static_cast<const TERM*>(command->nativeVar()))->isVariable())
	  {
	    _neginst.add(command->coefficient(),(static_cast<const TERM*>(command->nativeVar()))->var());
	  }
	else
	  {		 
	    ASSERT((static_cast<const TERM*>(command->nativeVar()))->IsReference());
	    TermWeighting::current()->collectWeight(_neginst,
						    command->coefficient(),
						    *(static_cast<const TERM*>(command->nativeVar())));
	  };
      };
      break;

        
    case CompiledConstraintForInstanceRetrieval::WeightComparisonCommand::NEGINSTPlusXToNEGINST:
      /*
	NEGINST := NEGINST + |X@|;
      */
      {
	
	
	if ((static_cast<const TERM*>(command->nativeVar()))->isVariable())
	  {
	    _neginst.add1((static_cast<const TERM*>(command->nativeVar()))->var());
	  }
	else
	  {		 
	    ASSERT((static_cast<const TERM*>(command->nativeVar()))->IsReference());
	    TermWeighting::current()->collectWeight(_neginst,
						    *(static_cast<const TERM*>(command->nativeVar())));
	  };
      };
      break;

    case CompiledConstraintForInstanceRetrieval::WeightComparisonCommand::NEGINSTPlusCXToNEGINST:
      /*
	NEGINST := NEGINST + C*|X@|;
      */
      {
	
	
	if ((static_cast<const TERM*>(command->nativeVar()))->isVariable())
	  {
	    _neginst.add(command->coefficient(),(static_cast<const TERM*>(command->nativeVar()))->var());
	  }
	else
	  {		 
	    ASSERT((static_cast<const TERM*>(command->nativeVar()))->IsReference());
	    TermWeighting::current()->collectWeight(_neginst,
						    command->coefficient(),
						    *(static_cast<const TERM*>(command->nativeVar())));
	  };
      };
      break;
    
    case CompiledConstraintForInstanceRetrieval::WeightComparisonCommand::NEGINSTPlusXToNEGINST_IfConstNEGINSTAndNEGINSTLe0RetAlwGr_IfConstNEGINSTGotoL:
      /*
	NEGINST := NEGINST + |X@|;
	if constant(NEGINST) && NEGINST < 0 return AlwaysGreater;
	if constant(NEGINST) goto L;
      */
      {
	
	
	if ((static_cast<const TERM*>(command->nativeVar()))->isVariable())
	  {
	    _neginst.add1((static_cast<const TERM*>(command->nativeVar()))->var());
	  }
	else
	  {		 
	    ASSERT((static_cast<const TERM*>(command->nativeVar()))->IsReference());
	    TermWeighting::current()->collectWeight(_neginst,
						    *(static_cast<const TERM*>(command->nativeVar())));
	    if (_neginst.isConstant())
	      {
		if (_neginst.freeMember() < 0) return BK::AlwaysGreater;
		command = command->label();
		goto next_command;
	      };
	  };
      };
      break;
        
    case CompiledConstraintForInstanceRetrieval::WeightComparisonCommand::NEGINSTPlusCXToNEGINST_IfConstNEGINSTAndNEGINSTLe0RetAlwGr_IfConstNEGINSTGotoL:
      /*
	NEGINST := NEGINST + C*|X@|;
	if constant(NEGINST) && NEGINST < 0 return AlwaysGreater;
	if constant(NEGINST) goto L;
      */
      {
	
	
	if ((static_cast<const TERM*>(command->nativeVar()))->isVariable())
	  {
	    _neginst.add(command->coefficient(),(static_cast<const TERM*>(command->nativeVar()))->var());
	  }
	else
	  {		 
	    ASSERT((static_cast<const TERM*>(command->nativeVar()))->IsReference());
	    TermWeighting::current()->collectWeight(_neginst,
						    command->coefficient(),
						    *(static_cast<const TERM*>(command->nativeVar())));
	    if (_neginst.isConstant())
	      {
		if (_neginst.freeMember() < 0) return BK::AlwaysGreater;
		command = command->label();
		goto next_command;
	      };
	  };
      };
      break;
    

    case CompiledConstraintForInstanceRetrieval::WeightComparisonCommand::XMinus1ToPOSINST_IfCompForGrOrEqPOSINSTvNEGINSTEqAlwGrRetAlwGr:
      /*
	POSINST := |X@| - 1;
	if compareForGreaterOrEqual(POSINST,NEGINST) == AlwaysGreater return AlwaysGreater;
      */
      {
	_posinst.reset();
	_posinst.add(static_cast<TermWeightType>(-1));
	
	
	if ((static_cast<const TERM*>(command->nativeVar()))->isVariable())
	  {
	    _posinst.add1((static_cast<const TERM*>(command->nativeVar()))->var());
	  }
	else
	  {		 
	    ASSERT((static_cast<const TERM*>(command->nativeVar()))->IsReference());
	    TermWeighting::current()->collectWeight(_posinst,
						    *(static_cast<const TERM*>(command->nativeVar())));
	  };
	
	if (_posinst.compareForGreaterOrEqual(_neginst) == BK::AlwaysGreater)
	  return BK::AlwaysGreater;
      };
      break;

    case CompiledConstraintForInstanceRetrieval::WeightComparisonCommand::CXMinusCToPOSINST_IfCompForGrOrEqPOSINSTvNEGINSTEqAlwGrRetAlwGr:
      /*
	POSINST := C*|X@| - C;
	if compareForGreaterOrEqual(POSINST,NEGINST) == AlwaysGreater return AlwaysGreater;
      */
      {
	_posinst.reset();
	_posinst.add(-(command->coefficient()));
	
	
	if ((static_cast<const TERM*>(command->nativeVar()))->isVariable())
	  {
	    _posinst.add(command->coefficient(),(static_cast<const TERM*>(command->nativeVar()))->var());
	  }
	else
	  {		 
	    ASSERT((static_cast<const TERM*>(command->nativeVar()))->IsReference());
	    TermWeighting::current()->collectWeight(_posinst,
						    command->coefficient(),
						    *(static_cast<const TERM*>(command->nativeVar())));
	  };
	if (_posinst.compareForGreaterOrEqual(_neginst) == BK::AlwaysGreater)
	  return BK::AlwaysGreater;
      };
      break;

    case CompiledConstraintForInstanceRetrieval::WeightComparisonCommand::POSINSTPlusXMinus1ToPOSINST_IfCompForGrOrEqPOSINSTvNEGINSTEqAlwGrRetAlwGr:
      /*
	POSINST := POSINST + |X@| - 1;
	if compareForGreaterOrEqual(POSINST,NEGINST) == AlwaysGreater return AlwaysGreater;
      */
      {
	_posinst.add(static_cast<TermWeightType>(-1));
	
	
	if ((static_cast<const TERM*>(command->nativeVar()))->isVariable())
	  {
	    _posinst.add1((static_cast<const TERM*>(command->nativeVar()))->var());
	  }
	else
	  {		 
	    ASSERT((static_cast<const TERM*>(command->nativeVar()))->IsReference());
	    TermWeighting::current()->collectWeight(_posinst,
						    *(static_cast<const TERM*>(command->nativeVar())));
	  };
	if (_posinst.compareForGreaterOrEqual(_neginst) == BK::AlwaysGreater)
	  return BK::AlwaysGreater;
      };
      break;

    case CompiledConstraintForInstanceRetrieval::WeightComparisonCommand::POSINSTPlusCXMinusCToPOSINST_IfCompForGrOrEqPOSINSTvNEGINSTEqAlwGrRetAlwGr:
      /*
	POSINST := POSINST + C*|X@| - C;
	if compareForGreaterOrEqual(POSINST,NEGINST) == AlwaysGreater return AlwaysGreater;
      */
      {
	_posinst.add(-(command->coefficient()));
	
	
	if ((static_cast<const TERM*>(command->nativeVar()))->isVariable())
	  {
	    _posinst.add(command->coefficient(),(static_cast<const TERM*>(command->nativeVar()))->var());
	  }
	else
	  {		 
	    ASSERT((static_cast<const TERM*>(command->nativeVar()))->IsReference());
	    TermWeighting::current()->collectWeight(_posinst,
						    command->coefficient(),
						    *(static_cast<const TERM*>(command->nativeVar())));
	  };
	if (_posinst.compareForGreaterOrEqual(_neginst) == BK::AlwaysGreater)
	  return BK::AlwaysGreater;
      };
      break;

    case CompiledConstraintForInstanceRetrieval::WeightComparisonCommand::POSINSTPlusXMinus1ToPOSINST_RetCompForGrOrEqPOSINSTvNEGINST:
      /*
	POSINST := POSINST + |X@| - 1;
	return compareForGreaterOrEqual(POSINST,NEGINST);
      */
      {
	_posinst.add(static_cast<TermWeightType>(-1));
	
	
	if ((static_cast<const TERM*>(command->nativeVar()))->isVariable())
	  {
	    _posinst.add1((static_cast<const TERM*>(command->nativeVar()))->var());
	  }
	else
	  {		 
	    ASSERT((static_cast<const TERM*>(command->nativeVar()))->IsReference());
	    TermWeighting::current()->collectWeight(_posinst,
						    *(static_cast<const TERM*>(command->nativeVar())));
	  };
	return _posinst.compareForGreaterOrEqual(_neginst);
      };

    case CompiledConstraintForInstanceRetrieval::WeightComparisonCommand::POSINSTPlusCXMinusCToPOSINST_RetCompForGrOrEqPOSINSTvNEGINST:
      /*
	POSINST := POSINST + C*|X@| - C;
	return compareForGreaterOrEqual(POSINST,NEGINST);
      */
      {
	_posinst.add(-(command->coefficient()));
	
	
	if ((static_cast<const TERM*>(command->nativeVar()))->isVariable())
	  {
	    _posinst.add(command->coefficient(),(static_cast<const TERM*>(command->nativeVar()))->var());
	  }
	else
	  {		 
	    ASSERT((static_cast<const TERM*>(command->nativeVar()))->IsReference());
	    TermWeighting::current()->collectWeight(_posinst,
						    command->coefficient(),
						    *(static_cast<const TERM*>(command->nativeVar())));
	  };
	return _posinst.compareForGreaterOrEqual(_neginst);
      };


    case CompiledConstraintForInstanceRetrieval::WeightComparisonCommand::NEGINSTPlus1MinusXToACC_IfACCLe0RetAlwGr_GrndToGRND:
      /*
	ASSERT(constant(NEGINST));
	ASSERT(NEGINST >= 0);
	ACC := NEGINST + 1 - min|X@|;
	if ACC < 0 return AlwaysGreater;
	GRND := ground(X@);
      */  
      {
	ASSERT(_neginst.isConstant());
	ASSERT(_neginst.freeMember() >= static_cast<TermWeightType>(0));
	
	
	if ((static_cast<const TERM*>(command->nativeVar()))->isVariable())
	  {
	    _acc = _neginst.freeMember();
	    _grnd = false;
	  }
	else
	  {		 
	    ASSERT((static_cast<const TERM*>(command->nativeVar()))->IsReference());
	    _acc = _neginst.freeMember() + 1;
	    bool ground;
	    TermWeightType w = 
	      TermWeighting::current()->computeMinimalInstanceWeightIfLessOrEqual((static_cast<const TERM*>(command->nativeVar()))->First(),
								       
										  _acc,
										  ground);
	    if (w < static_cast<TermWeightType>(0)) return BK::AlwaysGreater;
	    _acc -= w;
	    _grnd = ground;
	  };

      };
      break;

    case CompiledConstraintForInstanceRetrieval::WeightComparisonCommand::NEGINSTPlusCMinusCXToACC_IfACCLe0RetAlwGr_GrndToGRND:
      /*
	ASSERT(constant(NEGINST));
	ASSERT(NEGINST >= 0);
	ACC := NEGINST + C - C*min|X@|;
	if ACC < 0 return AlwaysGreater;
	GRND := ground(X@);
      */  
      {
	ASSERT(_neginst.isConstant());
	ASSERT(_neginst.freeMember() >= static_cast<TermWeightType>(0));
	
       
	if ((static_cast<const TERM*>(command->nativeVar()))->isVariable())
	  {
	    _acc = _neginst.freeMember();
	    _grnd = false;
	  }
	else
	  {		 
	    ASSERT((static_cast<const TERM*>(command->nativeVar()))->IsReference());
	    _acc = _neginst.freeMember() + command->coefficient();
	    bool ground;
	    TermWeightType w = 
	      TermWeighting::current()->computeMinimalInstanceWeightIfLessOrEqual(command->coefficient(),
										  (static_cast<const TERM*>(command->nativeVar()))->First(),
								       
										  _acc,
										  ground);
	    if (w < static_cast<TermWeightType>(0)) return BK::AlwaysGreater;
	    _acc -= w;
	    _grnd = ground;
	  };

      };
      break;


#ifdef DEBUG_NAMESPACE
    default: ICP("Bad instruction tag");
#endif
    };
  ASSERT(!command->isTerminal());
  ++command;
  goto next_command;

  ICP("END");
}; // FunctionComparison NonrecursiveKBOrdering::CompiledConstraintForInstanceRetrieval::interpretWeightComparisonCode()



//===================================================================
#undef DEBUG_NAMESPACE
#ifdef DEBUG_NONRECURSIVE_KB_ORDERING
#define DEBUG_NAMESPACE "NonrecursiveKBOrdering::CompiledConstraintForInstanceRetrieval::Command"
#endif
#include "debugMacros.hpp"
//===================================================================

inline
bool 
NonrecursiveKBOrdering::CompiledConstraintForInstanceRetrieval::Command::isTerminal() const
{
  CALL("isTerminal() const");
  switch (tag())
    {
    case RetSuccess:
    case RetFailure:
    case IfLexSTEqIncRetFail_IfLexSTEqLeRetFail_RetSucc:
      return true;
    default: return false;
    };
}; // bool NonrecursiveKBOrdering::CompiledConstraintForInstanceRetrieval::Command::isTerminal() const


#ifndef NO_DEBUG_VIS

ostream& NonrecursiveKBOrdering::CompiledConstraintForInstanceRetrieval::Command::output(ostream& str) const
{
  switch (tag())
    {
    case RetSuccess: return str << "RetSuccess;\n";
    case RetFailure: return str << "RetFailure;\n";

    case IfWCFailRetFail_IfWCAlwGrRetSucc:
      str << "IfWCFailRetFail_IfWCAlwGrRetSucc\n"
	  << " {\n"
	  << "   if WC(@) = FailureToCompare return Failure;\n"
	  << "   if WC(@) = AlwaysGreater return Success;\n"
          << "   ASSERT(WC(@) = AlwaysEqual || WC(@) = CanBeGreaterOrEqual);\n"
	  << " }\n";
      return str;

    case IfLexSTEqIncRetFail_IfLexSTEqLeRetFail_RetSucc:
      str << "IfLexSTEqIncRetFail_IfLexSTEqLeRetFail_RetSucc\n";
      str << " S = " << complexTerm1() << "\n";   
      str << " T = " << complexTerm2() << "\n";   
      str << " {\n"
          << "   if lex(S@,T@) = Incomparable return Failure;"
	  << "   if lex(S@,T@) = Less return Failure;"
          << "   return Success;\n"
	  << " }\n";
      return str;
	
    case IfLexXYEqIncRetFail_IfLexXYEqLeRetFail_IfLexXYEqGrRetSucc:
      str << "IfLexXYEqIncRetFail_IfLexXYEqLeRetFail_IfLexXYEqGrRetSucc\n";
      str << " X = X" << varNum1() << "\n";   
      str << " Y = X" << varNum2() << "\n";   
      str << " {\n"
          << "   if lex(X@,Y@) = Incomparable return Failure;\n"
	  << "   if lex(X@,Y@) = Less return Failure;\n"
	  << "   if lex(X@,Y@) = Greater return Success;\n"
	  << "   ASSERT(lex(X@,Y@) = Equal);\n"
	  << " }\n";
      return str;

    case IfLexXTEqIncRetFail_IfLexXTEqLeRetFail_IfLexXTEqGrRetSucc:
      str << "IfLexXTEqIncRetFail_IfLexXTEqLeRetFail_IfLexXTEqGrRetSucc\n";  
      str << " X = X" << varNum1() << "\n";     
      str << " T = " << complexTerm2() << "\n";   
      str << " {\n"
          << "   if lex(X@,T@) = Incomparable return Failure;\n"
	  << "   if lex(X@,T@) = Less return Failure;\n"
	  << "   if lex(X@,T@) = Greater return Success;\n"
	  << "   ASSERT(lex(X@,T@) = Equal);\n"
	  << " }\n";
      return str;
	
    case IfLexSYEqIncRetFail_IfLexSYEqLeRetFail_IfLexTYEqGrRetSucc:
      str << "IfLexSYEqIncRetFail_IfLexSYEqLeRetFail_IfLexTYEqGrRetSucc\n";  
      str << " S = " << complexTerm1() << "\n";  
      str << " Y = X" << varNum2() << "\n";  
      str << " {\n"
	  << "   if lex(S@,Y@) = Incomparable return Failure;\n"
	  << "   if lex(S@,Y@) = Less return Failure;\n"
	  << "   if lex(S@,Y@) = Greater return Success;\n"
	  << "   ASSERT(lex(S@,Y@) = Equal);\n"
	  << " }\n";
      return str;

    default: return str << "????????????;\n";
    };
  return str;
}; // ostream& NonrecursiveKBOrdering::CompiledConstraintForInstanceRetrieval::Command::output(ostream& str) const
#endif
//=================================================================
