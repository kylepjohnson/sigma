//
// File:         StandardKBOrdering.cpp
// Description:  Standard Knuth-Bendix ordering.  
// Created:      Jun 28, 2002, Alexandre Riazanov, riazanov@cs.man.ac.uk
//=================================================================
#include "StandardKBOrdering.hpp"
#include "TermWeighting.hpp"
#include "variables.hpp"
#include "ForwardMatchingSubstitutionCache.hpp"
#include "DestructionMode.hpp"
#include "TermWeightType.hpp"
#include "VampireKernelInterceptMacros.hpp"
#include "unification.hpp"
//===================================================================
#undef DEBUG_NAMESPACE
#ifdef DEBUG_STANDARD_KB_ORDERING
#define DEBUG_NAMESPACE "StandardKBOrdering"
#endif
#include "debugMacros.hpp"
//===================================================================

using namespace VK;
using namespace BK;

StandardKBOrdering::StandardKBOrdering() : 
  _specialisedComparisonPool(0),
  _constraintCheckOnUnifierPool(0),
  _constraintCheckOnForwardMatchingSubstPool(0),
  _compiledConstraintOnUnifiersPool(0),
  _compiledConstraintForInstanceRetrievalPool(0),
  _numberOfCompiledConstraintsForInstanceRetrieval(1UL),
  _numberOfConstraintChecksForInstanceRetrieval(1UL),
  _numberOfCompiledConstraintsOnUnifiers(1UL),
  _numberOfConstraintChecksOnUnifiers(1UL)
{    
  setType(SimplificationOrdering::StandardKB);
};

StandardKBOrdering::~StandardKBOrdering()
{
  CALL("destructor ~StandardKBOrdering()");
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
}; // StandardKBOrdering::~StandardKBOrdering()




void* StandardKBOrdering::operator new(size_t)
{
  return GlobAlloc::allocate(sizeof(StandardKBOrdering));
};

void StandardKBOrdering::operator delete(void* obj)
{
  GlobAlloc::deallocate(obj,sizeof(StandardKBOrdering));
}; 


Comparison StandardKBOrdering::compare(const Flatterm* term1,
				       const WeightPolynomial& weight1,
				       const Flatterm* term2,
				       const WeightPolynomial& weight2)
{
  CALL("compare(..)");
  Comparison lexCmp;
  switch (weight1.compare(weight2))
    {
    case BK::FailureToCompare:
      return Incomparable;
    case BK::AlwaysLess:
      return Less;
    case BK::AlwaysEqual:
      return compareLexicographically(term1,term2);
    case BK::AlwaysGreater:
      return Greater;
    case BK::CanBeLessOrEqual:
      lexCmp = compareLexicographically(term1,term2);
      if (lexCmp == Less) return Less;
      ASSERT(lexCmp != Equal);
      return Incomparable;
    case BK::CanBeGreaterOrEqual:
      lexCmp = compareLexicographically(term1,term2);
      if (lexCmp == Greater) return Greater;
      ASSERT(lexCmp != Equal);
      return Incomparable;
    case BK::VolatileButNearlyAlwaysLess:
      // same as below
    case BK::VolatileButNearlyAlwaysGreater:
      // same as below
    case BK::AbsolutelyVolatile:
      return Incomparable;
    };
  ICP("ICP0");

#if (defined _SUPPRESS_WARNINGS_) || (defined DEBUG_NAMESPACE)
  return Incomparable;
#endif

}; // Comparison StandardKBOrdering::compare(..)




Comparison StandardKBOrdering::compareAtoms(const Flatterm* lit1,
					    const WeightPolynomial& weight1,
					    const Flatterm* lit2,
					    const WeightPolynomial& weight2)
{
  CALL("compareAtoms(..)");
  Comparison lexCmp;
  switch (weight1.compare(weight2))
    {
    case BK::FailureToCompare:
      return Incomparable;
    case BK::AlwaysLess:
      return Less;
    case BK::AlwaysEqual:
      return compareAtomsLexicographically(lit1,lit2);
    case BK::AlwaysGreater:
      return Greater;
    case BK::CanBeLessOrEqual:
      lexCmp = compareAtomsLexicographically(lit1,lit2);
      if (lexCmp == Less) return Less;
      ASSERT(lexCmp != Equal);
      return Incomparable;
    case BK::CanBeGreaterOrEqual:
      lexCmp = compareAtomsLexicographically(lit1,lit2);
      if (lexCmp == Greater) return Greater;
      ASSERT(lexCmp != Equal);
      return Incomparable;
    case BK::VolatileButNearlyAlwaysLess:
      // same as below
    case BK::VolatileButNearlyAlwaysGreater:
      // same as below
    case BK::AbsolutelyVolatile:
      return Incomparable;
    };

  ICP("ICP0");
#if (defined _SUPPRESS_WARNINGS_) || (defined DEBUG_NAMESPACE)
  return Incomparable;
#endif

}; // Comparison StandardKBOrdering::compareAtoms(..)



Comparison StandardKBOrdering::compareOnCurrentUnifier(PrefixSym* complexTerm1,
						       PrefixSym* complexTerm2)
{
  CALL("compareOnCurrentUnifier(PrefixSym* complexTerm1,PrefixSym* complexTerm2)");

  //DF; cout << "COMPARE ";
  //DF; Prefix::outputWordWithSubst(cout,complexTerm1);
  //DF; cout << " WITH ";
  //DF; Prefix::outputWordWithSubst(cout,complexTerm2) << "\n";
  //DF; Unifier::current()->output(cout << "SUBST ",Prefix::outputTerm) << "\n\n\n";  

  _weight1.reset();
  _weight2.reset();
  TermWeighting::current()->collectWeight(_weight1,complexTerm1);
  TermWeighting::current()->collectWeight(_weight2,complexTerm2);
  Comparison lexCmp;
  switch (_weight1.compare(_weight2))
    {
    case BK::FailureToCompare:
      return Incomparable;
    case BK::AlwaysLess:
      return Less;
    case BK::AlwaysEqual:
      return compareLexOnCurrentUnifier(complexTerm1,complexTerm2);
    case BK::AlwaysGreater:
      return Greater;
    case BK::CanBeLessOrEqual:
      lexCmp = compareLexOnCurrentUnifier(complexTerm1,complexTerm2);
      if (lexCmp == Less) return Less;
      ASSERT(lexCmp != Equal);
      return Incomparable;
      
    case BK::CanBeGreaterOrEqual:
      lexCmp = compareLexOnCurrentUnifier(complexTerm1,complexTerm2);
      if (lexCmp == Greater) return Greater;
      ASSERT(lexCmp != Equal);
      return Incomparable;

    case BK::VolatileButNearlyAlwaysLess:
      // same as below
    case BK::VolatileButNearlyAlwaysGreater:
      // same as below
    case BK::AbsolutelyVolatile:
      return Incomparable;
    };

  ICP("ICP0");
#if (defined _SUPPRESS_WARNINGS_) || (defined DEBUG_NAMESPACE)
  return Incomparable;
#endif

}; // Comparison StandardKBOrdering::compareOnCurrentUnifier(..)


Comparison 
StandardKBOrdering::compareOnCurrentUnifier(const TERM& term1,
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
      return Incomparable;
    case BK::AlwaysLess:
      return Less;
    case BK::AlwaysEqual:
      return compareLexOnCurrentUnifier(term1,term2,variableIndex);
    case BK::AlwaysGreater:
      return Greater;
    case BK::CanBeLessOrEqual:

      //DF; cout << "********WEIGHT COMP = CanBeLessOrEqual\n";

      lexCmp = compareLexOnCurrentUnifier(term1,term2,variableIndex);

      //DF; cout << "*********LEX CMP = " << lexCmp << "\n";

      if (lexCmp == Less) return Less;
      ASSERT(lexCmp != Equal);
      return Incomparable;
      
    case BK::CanBeGreaterOrEqual:
      lexCmp = compareLexOnCurrentUnifier(term1,term2,variableIndex);
      if (lexCmp == Greater) return Greater;
      ASSERT(lexCmp != Equal);
      return Incomparable;

    case BK::VolatileButNearlyAlwaysLess:
      // same as below
    case BK::VolatileButNearlyAlwaysGreater:
      // same as below
    case BK::AbsolutelyVolatile:
      return Incomparable;
    };

  ICP("ICP0");
#if (defined _SUPPRESS_WARNINGS_) || (defined DEBUG_NAMESPACE)
  return Incomparable;
#endif

}; // Comparison StandardKBOrdering::compareOnCurrentUnifier(const TERM& term1,const TERM& term2,ulong variableIndex)



Comparison StandardKBOrdering::compare(const TERM* complexTerm1,
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
      return Incomparable;
    case BK::AlwaysLess:
      return Less;
    case BK::AlwaysEqual:
      return compareLexicographically(complexTerm1,complexTerm2,subst);
    case BK::AlwaysGreater:
      return Greater;
    case BK::CanBeLessOrEqual:
      lexCmp = compareLexicographically(complexTerm1,complexTerm2,subst);
      if (lexCmp == Less) return Less;
      ASSERT(lexCmp != Equal);
      return Incomparable;
      
    case BK::CanBeGreaterOrEqual:
      lexCmp = compareLexicographically(complexTerm1,complexTerm2,subst);
      if (lexCmp == Greater) return Greater;
      ASSERT(lexCmp != Equal);
      return Incomparable;

    case BK::VolatileButNearlyAlwaysLess:
      // same as below
    case BK::VolatileButNearlyAlwaysGreater:
      // same as below
    case BK::AbsolutelyVolatile:
      return Incomparable;
    };

  ICP("ICP0");
#if (defined _SUPPRESS_WARNINGS_) || (defined DEBUG_NAMESPACE)
  return Incomparable;
#endif
  

}; // Comparison StandardKBOrdering::compare(const TERM* complexTerm1,const TERM* complexTerm2,const InstRet::Substitution* subst)







Comparison StandardKBOrdering::compare(const TERM& term1,
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
      return Incomparable;
    case BK::AlwaysLess:
      return Less;
    case BK::AlwaysEqual:
      return compareLexicographically(term1,term2,subst);
    case BK::AlwaysGreater:
      return Greater;
    case BK::CanBeLessOrEqual:
      lexCmp = compareLexicographically(term1,term2,subst);
      if (lexCmp == Less) return Less;
      ASSERT(lexCmp != Equal);
      return Incomparable;
      
    case BK::CanBeGreaterOrEqual:
      lexCmp = compareLexicographically(term1,term2,subst);
      if (lexCmp == Greater) return Greater;
      ASSERT(lexCmp != Equal);
      return Incomparable;

    case BK::VolatileButNearlyAlwaysLess:
      // same as below
    case BK::VolatileButNearlyAlwaysGreater:
      // same as below
    case BK::AbsolutelyVolatile:
      return Incomparable;
    };

  ICP("ICP0");
#if (defined _SUPPRESS_WARNINGS_) || (defined DEBUG_NAMESPACE)
  return Incomparable;
#endif

}; // Comparison StandardKBOrdering::compare(const TERM& term1,const TERM& term2,const ForwardMatchingSubstitutionCache* subst)



SimplificationOrdering::SpecialisedComparison* 
StandardKBOrdering::reserveSpecialisedComparison()
{
  CALL("reserveSpecialisedComparison()");
  SpecialisedComparison* result = _specialisedComparisonPool;
  if (result)
    {
      _specialisedComparisonPool = _specialisedComparisonPool->getNextInPool();
    }
  else
    {
      result = new SpecialisedComparison(this);
    };
  
  return 
    static_cast<SimplificationOrdering::SpecialisedComparison*>(result);
}; // SimplificationOrdering::SpecialisedComparison* StandardKBOrdering::reserveSpecialisedComparison()

void StandardKBOrdering::releaseSpecialisedComparison(SimplificationOrdering::SpecialisedComparison* sc)
{
  CALL("releaseSpecialisedComparison(SimplificationOrdering::SpecialisedComparison* sc)");
  (static_cast<StandardKBOrdering::SpecialisedComparison*>(sc))->setNextInPool(_specialisedComparisonPool);
  _specialisedComparisonPool = static_cast<StandardKBOrdering::SpecialisedComparison*>(sc);
}; //  StandardKBOrdering::releaseSpecialisedComparison(..)



WeightPolynomial::SpecialisedComparison*
StandardKBOrdering::reserveSpecialisedWeightComparison()
{
  CALL("reserveSpecialisedWeightComparison()");
  return _specialisedWeightComparisonPool.reserveObject();
}; // WeightPolynomial::SpecialisedComparison* StandardKBOrdering::reserveSpecialisedWeightComparison()
    
void 
StandardKBOrdering::releaseSpecialisedWeightComparison(WeightPolynomial::SpecialisedComparison* swc)
{
  CALL("releaseSpecialisedWeightComparison(WeightPolynomial::SpecialisedComparison* swc)");
  _specialisedWeightComparisonPool.releaseObject(swc);
}; // void StandardKBOrdering::releaseSpecialisedWeightComparison(WeightPolynomial::SpecialisedComparison* swc)



SimplificationOrdering::StoredConstraint*
StandardKBOrdering::storedConstraintGreater(SimplificationOrdering::SpecialisedComparison* sc)
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
  return static_cast<SimplificationOrdering::StoredConstraint*>(static_cast<void*>(result));
}; // SimplificationOrdering::StoredConstraint* StandardKBOrdering::storedConstraintGreater(SimplificationOrdering::SpecialisedComparison* sc)

SimplificationOrdering::StoredConstraint*
StandardKBOrdering::storedConstraintGreaterOrEqual(SimplificationOrdering::SpecialisedComparison* sc)
{
  CALL("storedConstraintGreaterOrEqual(SimplificationOrdering::SpecialisedComparison* sc)");
  INTERCEPT_FORWARD_CONSTR_CHECK_NEW_CONSTRAINED_EQ;
  INTERCEPT_FORWARD_CONSTR_CHECK_BEGIN2;
  // Timer overhead
  INTERCEPT_FORWARD_CONSTR_CHECK_END2;
  INTERCEPT_FORWARD_CONSTR_CHECK_MEMORY_COUNT_ON;
  INTERCEPT_FORWARD_CONSTR_CHECK_BEGIN1;
  StoredConstraint* result = (static_cast<SpecialisedComparison*>(sc))->storedConstraintGreaterOrEqual();
  result->incReferenceCounter();
  ASSERT(result->referenceCounter() >= 2L);
  INTERCEPT_FORWARD_CONSTR_CHECK_END1;
  INTERCEPT_FORWARD_CONSTR_CHECK_MEMORY_COUNT_OFF;
  return static_cast<SimplificationOrdering::StoredConstraint*>(static_cast<void*>(result));
}; // SimplificationOrdering::StoredConstraint* StandardKBOrdering::storedConstraintGreaterOrEqual(SimplificationOrdering::SpecialisedComparison* sc)

SimplificationOrdering::StoredConstraint*
StandardKBOrdering::storedConstraintLess(SimplificationOrdering::SpecialisedComparison* sc)
{
  CALL("storedConstraintLess(SimplificationOrdering::SpecialisedComparison* sc)");
  INTERCEPT_FORWARD_CONSTR_CHECK_NEW_CONSTRAINED_EQ;
  INTERCEPT_FORWARD_CONSTR_CHECK_BEGIN2;
  // Timer overhead
  INTERCEPT_FORWARD_CONSTR_CHECK_END2;
  INTERCEPT_FORWARD_CONSTR_CHECK_MEMORY_COUNT_ON;
  INTERCEPT_FORWARD_CONSTR_CHECK_BEGIN1;
  StoredConstraint* result = (static_cast<SpecialisedComparison*>(sc))->storedConstraintLess();
  result->incReferenceCounter();
  ASSERT(result->referenceCounter() >= 2L);
  INTERCEPT_FORWARD_CONSTR_CHECK_END1;
  INTERCEPT_FORWARD_CONSTR_CHECK_MEMORY_COUNT_OFF;
  return static_cast<SimplificationOrdering::StoredConstraint*>(static_cast<void*>(result));
}; // SimplificationOrdering::StoredConstraint* StandardKBOrdering::storedConstraintLess(SimplificationOrdering::SpecialisedComparison* sc)

SimplificationOrdering::StoredConstraint*
StandardKBOrdering::storedConstraintLessOrEqual(SimplificationOrdering::SpecialisedComparison* sc)
{
  CALL("storedConstraintLessOrEqual(SimplificationOrdering::SpecialisedComparison* sc)");
  INTERCEPT_FORWARD_CONSTR_CHECK_NEW_CONSTRAINED_EQ;
  INTERCEPT_FORWARD_CONSTR_CHECK_BEGIN2;
  // Timer overhead
  INTERCEPT_FORWARD_CONSTR_CHECK_END2;
  INTERCEPT_FORWARD_CONSTR_CHECK_MEMORY_COUNT_ON;
  INTERCEPT_FORWARD_CONSTR_CHECK_BEGIN1;
  StoredConstraint* result = (static_cast<SpecialisedComparison*>(sc))->storedConstraintLessOrEqual();
  result->incReferenceCounter();
  ASSERT(result->referenceCounter() >= 2L);
  INTERCEPT_FORWARD_CONSTR_CHECK_END1;
  INTERCEPT_FORWARD_CONSTR_CHECK_MEMORY_COUNT_OFF;
  return static_cast<SimplificationOrdering::StoredConstraint*>(static_cast<void*>(result));
}; // SimplificationOrdering::StoredConstraint* StandardKBOrdering::storedConstraintLessOrEqual(SimplificationOrdering::SpecialisedComparison* sc)

SimplificationOrdering::StoredConstraint* StandardKBOrdering::copy(SimplificationOrdering::StoredConstraint* constr)
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
}; // SimplificationOrdering::StoredConstraint* StandardKBOrdering::copy(SimplificationOrdering::StoredConstraint* constr)


void StandardKBOrdering::releaseConstraint(SimplificationOrdering::StoredConstraint* constr)
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
    delete (static_cast<StoredConstraint*>(static_cast<void*>(constr)));
  INTERCEPT_FORWARD_CONSTR_CHECK_END1;
  INTERCEPT_FORWARD_CONSTR_CHECK_MEMORY_COUNT_OFF;
}; // void StandardKBOrdering::releaseConstraint(SimplificationOrdering::StoredConstraint* constr)



SimplificationOrdering::ConstraintCheckOnUnifier* StandardKBOrdering::reserveConstraintCheckOnUnifier()
{
  CALL("reserveConstraintCheckOnUnifier()");  
  ConstraintCheckOnUnifier* result = _constraintCheckOnUnifierPool;
  if (result)
    {
      _constraintCheckOnUnifierPool = _constraintCheckOnUnifierPool->getNextInPool();
    }
  else
    {
      result = new ConstraintCheckOnUnifier(this);
    };
  return 
    static_cast<SimplificationOrdering::ConstraintCheckOnUnifier*>(result);
}; // SimplificationOrdering::ConstraintCheckOnUnifier* StandardKBOrdering::reserveConstraintCheckOnUnifier()

void StandardKBOrdering::releaseConstraintCheckOnUnifier(SimplificationOrdering::ConstraintCheckOnUnifier* ccu)
{
  CALL("releaseConstraintCheckOnUnifier(SimplificationOrdering::ConstraintCheckOnUnifier* ccu)");

  (static_cast<StandardKBOrdering::ConstraintCheckOnUnifier*>(ccu))->setNextInPool(_constraintCheckOnUnifierPool);
  _constraintCheckOnUnifierPool = static_cast<StandardKBOrdering::ConstraintCheckOnUnifier*>(ccu);
}; // void StandardKBOrdering::releaseConstraintCheckOnUnifier(SimplificationOrdering::ConstraintCheckOnUnifier* ccu)



SimplificationOrdering::ConstraintCheckOnForwardMatchingSubst* 
StandardKBOrdering::reserveConstraintCheckOnForwardMatchingSubst()
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
      result = new ConstraintCheckOnForwardMatchingSubst(this);
    };
  return 
    static_cast<SimplificationOrdering::ConstraintCheckOnForwardMatchingSubst*>(result);
}; // SimplificationOrdering::ConstraintCheckOnForwardMatchingSubst* StandardKBOrdering::reserveConstraintCheckOnForwardMatchingSubst()

void 
StandardKBOrdering::releaseConstraintCheckOnForwardMatchingSubst(SimplificationOrdering::ConstraintCheckOnForwardMatchingSubst* ccfms)
{
  CALL("releaseConstraintCheckOnForwardMatchingSubst(..)");
  (static_cast<StandardKBOrdering::ConstraintCheckOnForwardMatchingSubst*>(ccfms))->
    setNextInPool(_constraintCheckOnForwardMatchingSubstPool);
  _constraintCheckOnForwardMatchingSubstPool = 
    static_cast<StandardKBOrdering::ConstraintCheckOnForwardMatchingSubst*>(ccfms);
}; // StandardKBOrdering::releaseConstraintCheckOnForwardMatchingSubst(SimplificationOrdering::ConstraintCheckOnForwardMatchingSubst* ccfms)


SimplificationOrdering::CompiledConstraintForUnifiers* 
StandardKBOrdering::reserveCompiledConstraintForUnifiers()
{
  CALL("reserveCompiledConstraintForUnifiers()");
  StandardKBOrdering::CompiledConstraintForUnifiers* result = _compiledConstraintOnUnifiersPool;
  if (result)
    {
      _compiledConstraintOnUnifiersPool = result->getNextInPool();
    }
  else
    {
      result = new CompiledConstraintForUnifiers(this);
    };
  ASSERT(result);
  return 
    static_cast<SimplificationOrdering::CompiledConstraintForUnifiers*>(result);
}; // SimplificationOrdering::CompiledConstraintForUnifiers* StandardKBOrdering::reserveCompiledConstraintForUnifiers()

void 
StandardKBOrdering::releaseCompiledConstraintForUnifiers(SimplificationOrdering::CompiledConstraintForUnifiers* cc)
{
  CALL("releaseCompiledConstraintForUnifiers(SimplificationOrdering::CompiledConstraintForUnifiers* cc)");
  
  (static_cast<StandardKBOrdering::CompiledConstraintForUnifiers*>(cc))->setNextInPool(_compiledConstraintOnUnifiersPool);
  _compiledConstraintOnUnifiersPool = static_cast<StandardKBOrdering::CompiledConstraintForUnifiers*>(cc);
}; // void StandardKBOrdering::releaseCompiledConstraintForUnifiers(SimplificationOrdering::CompiledConstraintForUnifiers* cc)



SimplificationOrdering::CompiledConstraintForInstanceRetrieval* 
StandardKBOrdering::reserveCompiledConstraintForInstanceRetrieval()
{
  CALL("reserveCompiledConstraintForInstanceRetrieval()");
  
  StandardKBOrdering::CompiledConstraintForInstanceRetrieval* result = _compiledConstraintForInstanceRetrievalPool;
  if (result)
    {
      _compiledConstraintForInstanceRetrievalPool = result->getNextInPool();
    }
  else
    {
      result = new CompiledConstraintForInstanceRetrieval(this);
    };
  ASSERT(result);
  return
    static_cast<SimplificationOrdering::CompiledConstraintForInstanceRetrieval*>(result);

}; // SimplificationOrdering::CompiledConstraintForInstanceRetrieval* StandardKBOrdering::reserveCompiledConstraintForInstanceRetrieval()

void 
StandardKBOrdering::releaseCompiledConstraintForInstanceRetrieval(SimplificationOrdering::CompiledConstraintForInstanceRetrieval* cc)
{
  CALL("releaseCompiledConstraintForInstanceRetrieval(SimplificationOrdering::CompiledConstraintForInstanceRetrieval* cc)");

  (static_cast<StandardKBOrdering::CompiledConstraintForInstanceRetrieval*>(cc))->setNextInPool(_compiledConstraintForInstanceRetrievalPool);
  _compiledConstraintForInstanceRetrievalPool = static_cast<StandardKBOrdering::CompiledConstraintForInstanceRetrieval*>(cc);

}; // void StandardKBOrdering::releaseCompiledConstraintForInstanceRetrieval(SimplificationOrdering::CompiledConstraintForInstanceRetrieval* cc)




Comparison StandardKBOrdering::compareWithTransposition(const Flatterm* term)
{  
  CALL("compareWithTransposition(const Flatterm* term)");
 ASSERT(term->isComplex());
 ASSERT(term->arity() == 2);
 const Flatterm* arg1 = term->next();
 const Flatterm* arg2 = arg1->after();
 return compare(arg1,arg2);
}; // Comparison StandardKBOrdering::compareWithTransposition(const Flatterm* term)

inline
Comparison StandardKBOrdering::compare(const Flatterm* term1,
				       const Flatterm* term2)
{
  CALL("compare(const Flatterm* term1,const Flatterm* term2)");
  _weight1.reset();
  _weight2.reset();
  TermWeighting::current()->collectWeight(_weight1,term1);
  TermWeighting::current()->collectWeight(_weight2,term2);
  return compare(term1,_weight1,term2,_weight2);
}; // Comparison StandardKBOrdering::compare(const Flatterm* term1,const Flatterm* term2)



inline
Comparison 
StandardKBOrdering::compareLexicographically(const Flatterm* term1,
					     const Flatterm* term2)
{
  CALL("compareLexicographically(const Flatterm* term1,..)");
  if (term1->symbolRef().isVariable())
    {
      if (term1->symbolRef() == term2->symbolRef()) return Equal;
      if (term2->containsSymbol(term1->symbolRef()))
	{
	  return Less;
	};
      return Incomparable;
    }
  else
    if (term2->symbolRef().isVariable())
      {
        if (term1->containsSymbol(term2->symbolRef()))
	  {
	    return Greater;
	  };
	return Incomparable;
      }
    else
      {
	// both are complex
	if (term1->symbolRef() != term2->symbolRef())
	  return term1->symbolRef().comparePrecedence(term2->symbolRef());
	Flatterm::ConstIterator iter1(term1);
	Flatterm::ConstIterator iter2(term2);
	iter1.NextSym();
	iter2.NextSym();
        while (iter1.NotEndOfTerm())
	  {
	    Comparison cmp = compare(iter1.CurrPos(),iter2.CurrPos());
	    if (cmp != Equal) return cmp;
	    iter1.after();
	    iter2.after();
	  };
	ASSERT(iter2.EndOfTerm());
	return Equal;
      };

}; // Comparison StandardKBOrdering::compareLexicographically(..)

inline
Comparison 
StandardKBOrdering::compareAtomsLexicographically(const Flatterm* lit1,
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
      Comparison cmp = compare(iter1.CurrPos(),iter2.CurrPos());
      if (cmp != Equal) return cmp;
      iter1.after();
      iter2.after();
    };
  ASSERT(iter2.EndOfTerm());
  return Equal;
}; // Comparison StandardKBOrdering::compareAtomsLexicographically(..)



inline
Comparison StandardKBOrdering::compareLexicographically(const TERM* complexTerm1,
							const TERM* complexTerm2)
{
  CALL("compareLexicographically(const TERM* complexTerm1,const TERM* complexTerm2)");

  if ((*complexTerm1) != (*complexTerm2)) 
    return complexTerm1->comparePrecedence(*complexTerm2);
  const TERM* arg1 = complexTerm1->Args();
  const TERM* arg2 = complexTerm2->Args();
  while (!arg1->IsNull())
    {
      Comparison cmp = 
	compare(*arg1,*arg2);
      if (cmp != Equal) return cmp;
      arg1 = arg1->Next();
      arg2 = arg2->Next();
    };
  return Equal;

}; // Comparison StandardKBOrdering::compareLexicographically(..)

inline
Comparison StandardKBOrdering::compareLexicographically(const TERM& term1,
							const TERM& term2)
{
  CALL("compareLexicographically(const TERM& term1,..)");
  if (term1 == term2) return Equal;
  if (term1.isVariable())
    {
      if (term2.isVariable()) return Incomparable;
      if (term2.First()->containsSymbol(term1)) return Less;
      return Incomparable;
    }
  else
    if (term2.isVariable())
      {
	if (term1.First()->containsSymbol(term2)) return Greater;	
	return Incomparable;
      }
    else // both are references
      {
	ASSERT(term1.IsReference() && term2.IsReference());
	return compareLexicographically(term1.First(),term2.First());
      };
}; // Comparison StandardKBOrdering::compareLexicographically(const TERM& term1,..)


inline
bool 
StandardKBOrdering::termWithUnifierContainsVariable(PrefixSym* complexTerm,
						    Unifier::Variable* var)
{ 
  CALL("termWithUnifierContainsVariable(PrefixSym* complexTerm,Unifier::Variable* var)");
  Prefix::CorrectBracket(complexTerm);
  PrefixIterPool::AutoObject iter(_prefixIterPool);
  iter.object().Reset(complexTerm);
  iter.object().SkipSym();
  while (iter.object().NotEmpty())
    {
      if (iter.object().CurrVar() == var) return true;
      iter.object().SkipSym();
    };
  return false;
}; // bool StandardKBOrdering::termWithUnifierContainsVariable(PrefixSym* complexTerm,Unifier::Variable* var)

inline
Comparison 
StandardKBOrdering::compareLexOnCurrentUnifier(PrefixSym* complexTerm1,
					       PrefixSym* complexTerm2)
{
  CALL("compareLexOnCurrentUnifier(PrefixSym* complexTerm1,PrefixSym* complexTerm2)");

  //DF; cout << "COMP LEX\n";
  //DF; Prefix::outputWordWithSubst(cout,complexTerm1) << "\n   WITH\n";
  //DF; Prefix::outputWordWithSubst(cout,complexTerm2) << "\n";

  if (complexTerm1->Head() != complexTerm2->Head())
    return complexTerm1->Head().Func().comparePrecedence(complexTerm2->Head().Func());  
  if (!complexTerm1->Head().Func().arity()) return Equal;
  
  PrefixIterPool::AutoObject iter1(_prefixIterPool);
  PrefixIterPool::AutoObject iter2(_prefixIterPool);
  Prefix::CorrectBracket(complexTerm1);
  Prefix::CorrectBracket(complexTerm2);
  iter1.object().Reset(complexTerm1);
  iter2.object().Reset(complexTerm2);
  iter1.object().SkipSym();
  iter2.object().SkipSym();
  do
    {
      Unifier::Variable* var1 = iter1.object().CurrVar();
      Unifier::Variable* var2 = iter2.object().CurrVar();
      
      if (var1)
	{
	  if (var2)
	    {
	      if (var1 != var2) return Incomparable;
	      iter1.object().SkipSym();
	      iter2.object().SkipSym();
	    }
	  else
	    {
	      // check if iter2.object().CurrSubterm() contains var1
	      if (termWithUnifierContainsVariable(iter2.object().CurrSubterm(),var1))
		return Less;
	      return Incomparable;
	    };
	}
      else
	if (var2)
	  {
	    // check if iter1.object().CurrSubterm() contains var2
	    if (termWithUnifierContainsVariable(iter1.object().CurrSubterm(),var2))
	      return Greater;
	    return Incomparable;
	  }
	else // both subterms are complex
	  {
	    Prefix::CorrectBracket(iter1.object().CurrSubterm());
	    Prefix::CorrectBracket(iter2.object().CurrSubterm());
	    Comparison cmp = compareOnCurrentUnifier(iter1.object().CurrSubterm(),
						     iter2.object().CurrSubterm());
	    if (cmp != Equal) return cmp;
	    iter1.object().SkipTerm();
	    iter2.object().SkipTerm();
	  };
    }
  while (iter1.object().NotEmpty());
  ASSERT(iter2.object().Empty());
  return Equal;
}; // Comparison StandardKBOrdering::compareLexOnCurrentUnifier(..)


inline
bool 
StandardKBOrdering::termWithUnifierContainsVariable(const TERM* complexTerm,
						    ulong variableIndex,
						    Unifier::Variable* var)
{
  CALL("termWithUnifierContainsVariable(const TERM* complexTerm,ulong variableIndex,Unifier::Variable* var)");

  StTermWithSubstPool::AutoObject iter(_stTermWithSubstPool);
  iter.object().Reset(variableIndex,complexTerm);
  iter.object().SkipSym();
  while (!iter.object().Empty())
    {
      if (iter.object().CurrSym().isVariable() &&
	  (iter.object().CurrSym().var() == var))
	return true;
      iter.object().SkipSym();
    };
  return false;
}; // bool StandardKBOrdering::termWithUnifierContainsVariable(..)


inline
bool 
StandardKBOrdering::termWithUnifierContainsVariable(const TERM& term,
						    ulong variableIndex,
						    Unifier::Variable* var)
{
  CALL("termWithUnifierContainsVariable(const TERM& term,ulong variableIndex,Unifier::Variable* var)");
  if (term.isVariable())
    {
      Unifier::Variable* var = 
	Unifier::current()->variable(term.var(),variableIndex);
      bool freeVar;
      void* instance = var->Unref(freeVar);
      if (freeVar)
	{
	  return (static_cast<Unifier::Variable*>(instance)) == var;
	}
      else
	{
	  return termWithUnifierContainsVariable(static_cast<PrefixSym*>(instance),var);
	};

    } 
  else
    {
      return termWithUnifierContainsVariable(term.First(),
					     variableIndex,
					     var);
    };
  
}; // bool StandardKBOrdering::termWithUnifierContainsVariable(..)


inline
Comparison 
StandardKBOrdering::compareOnCurrentUnifier(PrefixSym* complexTerm1,
					    const TERM* complexTerm2,
					    ulong variableIndex)
{
  CALL("compareOnCurrentUnifier(PrefixSym* complexTerm1,const TERM* complexTerm2,ulong variableIndex)");
  _weight1.reset();
  _weight2.reset();
  
  TermWeighting::current()->collectWeight(_weight1,complexTerm1);
  TermWeighting::current()->collectWeight(_weight2,
					  TERM(const_cast<TERM*>(complexTerm2)),
					  variableIndex);

  Comparison lexCmp;
  switch (_weight1.compare(_weight2))
    {
    case BK::FailureToCompare:
      return Incomparable;
    case BK::AlwaysLess:
      return Less;
    case BK::AlwaysEqual:
      return compareLexOnCurrentUnifier(complexTerm1,complexTerm2,variableIndex);
    case BK::AlwaysGreater:
      return Greater;
    case BK::CanBeLessOrEqual:
      lexCmp = compareLexOnCurrentUnifier(complexTerm1,complexTerm2,variableIndex);
      if (lexCmp == Less) return Less;
      ASSERT(lexCmp != Equal);
      return Incomparable;
      
    case BK::CanBeGreaterOrEqual:
      lexCmp = compareLexOnCurrentUnifier(complexTerm1,complexTerm2,variableIndex);
      if (lexCmp == Greater) return Greater;
      ASSERT(lexCmp != Equal);
      return Incomparable;

    case BK::VolatileButNearlyAlwaysLess:
      // same as below
    case BK::VolatileButNearlyAlwaysGreater:
      // same as below
    case BK::AbsolutelyVolatile:
      return Incomparable;
    };

  ICP("ICP0");
#if (defined _SUPPRESS_WARNINGS_) || (defined DEBUG_NAMESPACE)
  return Incomparable;
#endif
}; // Comparison StandardKBOrdering::compareOnCurrentUnifier(..)


inline
Comparison 
StandardKBOrdering::compareLexOnCurrentUnifier(PrefixSym* complexTerm1,
					       const TERM* complexTerm2,
					       ulong variableIndex)
{
  CALL("compareLexOnCurrentUnifier(PrefixSym* complexTerm1,const TERM* complexTerm2,ulong variableIndex)");

  if (complexTerm1->Head().Func() != (*complexTerm2))
    return complexTerm1->Head().Func().comparePrecedence(*complexTerm2);
  
  Prefix::CorrectBracket(complexTerm1);
  PrefixIterPool::AutoObject iter1(_prefixIterPool);
  iter1.object().Reset(complexTerm1);
  iter1.object().SkipSym();
  const TERM* arg2 = complexTerm2->Args();
  while (iter1.object().NotEmpty())
    {
      Unifier::Variable* var1 = iter1.object().CurrVar();
      if (var1)
	{
          if (arg2->isVariable())
	    {
	      Unifier::Variable* var2 = 
		Unifier::current()->variable(arg2->var(),variableIndex);
	      bool freeVar2;
	      void* instance2 = var2->Unref(freeVar2);
	      if (freeVar2)
		{
		  if ((static_cast<Unifier::Variable*>(instance2)) == var1)
		    return Less;
		}
	      else
		{
		  if (termWithUnifierContainsVariable(static_cast<PrefixSym*>(instance2),
						      var1))
		    return Less;
		};	    
	      return Incomparable;  
	    }
	  else
	    {
	      if (termWithUnifierContainsVariable(arg2->First(),
						  variableIndex,
						  var1))
		return Less;
	      return Incomparable;
	    };	    
	}
      else // iter1.object().CurrSubterm() is complex
	{
	  Prefix::CorrectBracket(iter1.object().CurrSubterm());
	  if (arg2->isVariable())
	    {
	      Unifier::Variable* var2 = 
		Unifier::current()->variable(arg2->var(),variableIndex);
	      bool freeVar2;
	      void* instance2 = var2->Unref(freeVar2);
	      if (freeVar2)
		{
		  if (termWithUnifierContainsVariable(iter1.object().CurrSubterm(),
						      static_cast<Unifier::Variable*>(instance2)))
		    return Greater;
		  return Incomparable;
		}
	      else
		{
		  Comparison cmp = 
		    compareOnCurrentUnifier(iter1.object().CurrSubterm(),
					    static_cast<PrefixSym*>(instance2));
		  if (cmp != Equal) return cmp;
		};	

	    }
	  else // both arguments are complex
	    {
	      Comparison cmp = 
		compareOnCurrentUnifier(iter1.object().CurrSubterm(),
					arg2->First(),
					variableIndex); 
	      if (cmp != Equal) return cmp;
	    };
	};

      iter1.object().SkipTerm();
      arg2 = arg2->Next();
    };
  return Equal;
}; // Comparison StandardKBOrdering::compareLexOnCurrentUnifier(..)



inline
Comparison 
StandardKBOrdering::compareLexOnCurrentUnifier(PrefixSym* complexTerm1,
					       const TERM& term2,
					       ulong variableIndex)
{
  CALL("compareLexOnCurrentUnifier(PrefixSym* complexTerm1,const TERM& term2,ulong variableIndex)");
  if (term2.isVariable())
    {
      
      Unifier::Variable* var2 = 
	Unifier::current()->variable(term2.var(),variableIndex);
      bool freeVar2;
      void* instance2 = var2->Unref(freeVar2);
      if (freeVar2)
	{
	  if (termWithUnifierContainsVariable(complexTerm1,
					      static_cast<Unifier::Variable*>(instance2)))
	    return Greater;
	  return Incomparable;
	}
      else
	{
	  return compareLexOnCurrentUnifier(complexTerm1,
					    static_cast<PrefixSym*>(instance2));
	};
    }
  else
    return compareLexOnCurrentUnifier(complexTerm1,
				      term2.First(),
				      variableIndex);
}; // Comparison StandardKBOrdering::compareLexOnCurrentUnifier(..)



inline
Comparison 
StandardKBOrdering::compareLexOnCurrentUnifier(const TERM& term1,
					       const TERM& term2,
					       ulong variableIndex)
{
  CALL("compareLexOnCurrentUnifier(const TERM& term1,const TERM& term2,ulong variableIndex)");

  if (term1 == term2) return Equal;
  if (term1.isVariable())
    {
      Unifier::Variable* var1 = 
	Unifier::current()->variable(term1.var(),variableIndex);
      bool freeVar1;
      void* instance1 = var1->Unref(freeVar1);
      if (freeVar1)
	{
	  if (term2.isVariable())
	    {
	      Unifier::Variable* var2 = 
		Unifier::current()->variable(term2.var(),variableIndex);
	      bool freeVar2;
	      void* instance2 = var2->Unref(freeVar2);
	      if (freeVar2)
		{
		  return (instance1 == instance2) ? Equal : Incomparable;
		}
	      else
		{
		  if (termWithUnifierContainsVariable(static_cast<PrefixSym*>(instance2),
						      static_cast<Unifier::Variable*>(instance1)))
		    return Less;
		  return Incomparable;
		};

	    }
	  else // term2 is a reference
	    {
	      if (termWithUnifierContainsVariable(term2.First(),
						  variableIndex,
						  static_cast<Unifier::Variable*>(instance1)))
		return Less;
	      return Incomparable;
	    };
	}
      else
	{
	  return 
	    compareLexOnCurrentUnifier(static_cast<PrefixSym*>(instance1),
				       term2,
				       variableIndex);
	};
    }
  else // term1 is a reference
    if (term2.isVariable())
      {
	Unifier::Variable* var2 = 
	  Unifier::current()->variable(term2.var(),variableIndex);
	bool freeVar2;
	void* instance2 = var2->Unref(freeVar2);
	if (freeVar2)
	  {
	    if (termWithUnifierContainsVariable(term1.First(),
						variableIndex,
						static_cast<Unifier::Variable*>(instance2)))
	      return Greater;
	    return Incomparable;
	  }
	else
	  {
	    Comparison cmp =
	      compareLexOnCurrentUnifier(static_cast<PrefixSym*>(instance2),
					 term1.First(),
					 variableIndex);
	    return BK::inverse(cmp);
	  };
      }
    else // both are references
      {
	if ((*term1.First()) != (*term2.First()))
	  {
	    return term1.First()->comparePrecedence(*term2.First());
	  };
	const TERM* arg1 = term1.First()->Args();
	const TERM* arg2 = term2.First()->Args();
	while (!arg1->IsNull())
	  {
	    Comparison cmp = 
	      compareOnCurrentUnifier(*arg1,*arg2,variableIndex);
	    if (cmp != Equal) return cmp;
	    arg1 = arg1->Next();
	    arg2 = arg2->Next();
	  };
	return Equal;
      };
}; // Comparison StandardKBOrdering::compareLexOnCurrentUnifier(const TERM& term1,const TERM& term2,ulong variableIndex)

inline
Comparison 
StandardKBOrdering::compareLexicographically(const TERM* complexTerm1,
					     const InstRet::Substitution* subst,
					     const TERM* complexTerm2)
{
  CALL("compareLexicographically(const TERM* complexTerm1,const InstRet::Substitution* subst,const TERM* complexTerm2)");
  if ((*complexTerm1) != (*complexTerm2))
    return complexTerm1->comparePrecedence(*complexTerm2);
  const TERM* arg1 = complexTerm1->Args();
  const TERM* arg2 = complexTerm2->Args();
  while (!arg1->IsNull())
    {
      if (arg1->isVariable())
	{
	  TERM instance = subst->map(arg1->var());
	  Comparison cmp = compare(instance,*arg2);
	  if (cmp != Equal) return cmp;
	}
      else
	{
	  Comparison cmp = compare(arg1->First(),subst,*arg2);
	  if (cmp != Equal) return cmp;
	};
      arg1 = arg1->Next();      
      arg2 = arg2->Next();      
    };
  ASSERT(arg2->IsNull());
  return Equal;
}; // Comparison StandardKBOrdering::compareLexicographically(..)

inline
Comparison 
StandardKBOrdering::compareLexicographically(const TERM* complexTerm1,
					     const InstRet::Substitution* subst,
					     const TERM& term2)
{
  CALL("compareLexicographically(const TERM* complexTerm1,const InstRet::Substitution* subst,const TERM& term2)");
  
  if (term2.isVariable())
    {
      // check if complexTerm1 contains the variable
      InstRet::Substitution::Instance iter1;
      iter1.reset(complexTerm1,subst);
      while (iter1.next())
	{
	  if (iter1.symbol() == term2) return Greater;
	};
      return Incomparable;
    }
  else
    return compareLexicographically(complexTerm1,subst,term2.First());

}; // Comparison StandardKBOrdering::compareLexicographically(const TERM* complexTerm1,const InstRet::Substitution* subst,const TERM& term2)




inline
Comparison 
StandardKBOrdering::compare(const TERM* complexTerm1,
			    const InstRet::Substitution* subst,
			    const TERM& term2)
{
  CALL("compare(const TERM* complexTerm1,const InstRet::Substitution* subst,const TERM& term2)");
  if (term2.isVariable())
    {
      // check if complexTerm1 contains the variable
      InstRet::Substitution::Instance iter1;
      iter1.reset(complexTerm1,subst);
      while (iter1.next())
	{
	  if (iter1.symbol() == term2) return Greater;
	};
      return Incomparable;
    }
  else
    {
      _weight1.reset();
      _weight2.reset();
      TermWeighting::current()->collectWeight(_weight1,complexTerm1,subst);
      TermWeighting::current()->collectWeight(_weight2,term2);
      Comparison lexCmp;
      switch (_weight1.compare(_weight2))
	{
	case BK::FailureToCompare:
	  return Incomparable;
	case BK::AlwaysLess:
	  return Less;
	case BK::AlwaysEqual:
	  return compareLexicographically(complexTerm1,subst,term2.First());
	case BK::AlwaysGreater:
	  return Greater;
	case BK::CanBeLessOrEqual:
	  lexCmp = compareLexicographically(complexTerm1,subst,term2.First());
	  if (lexCmp == Less) return Less;
	  ASSERT(lexCmp != Equal);
	  return Incomparable;
      
	case BK::CanBeGreaterOrEqual:
	  lexCmp = compareLexicographically(complexTerm1,subst,term2.First());
	  if (lexCmp == Greater) return Greater;
	  ASSERT(lexCmp != Equal);
	  return Incomparable;

	case BK::VolatileButNearlyAlwaysLess:
	  // same as below
	case BK::VolatileButNearlyAlwaysGreater:
	  // same as below
	case BK::AbsolutelyVolatile:
	  return Incomparable;
	};
 
    };
  ICP("ICP0");
#if (defined _SUPPRESS_WARNINGS_) || (defined DEBUG_NAMESPACE)
  return Incomparable;
#endif

}; // Comparison StandardKBOrdering::compare(..)


inline 
Comparison 
StandardKBOrdering::compare(const TERM* complexTerm1,const TERM* complexTerm2)
{
  CALL("compare(const TERM* complexTerm1,const TERM* complexTerm2)");
  _weight1.reset();
  _weight2.reset();
  TermWeighting::current()->collectWeight(_weight1,TERM(const_cast<TERM*>(complexTerm1)));
  TermWeighting::current()->collectWeight(_weight2,TERM(const_cast<TERM*>(complexTerm2)));
  
  Comparison lexCmp;
  switch (_weight1.compare(_weight2))
    {
    case BK::FailureToCompare:
      return Incomparable;
    case BK::AlwaysLess:
      return Less;
    case BK::AlwaysEqual:
      return compareLexicographically(complexTerm1,complexTerm2);
    case BK::AlwaysGreater:
      return Greater;
    case BK::CanBeLessOrEqual:
      lexCmp = compareLexicographically(complexTerm1,complexTerm2);
      if (lexCmp == Less) return Less;
      ASSERT(lexCmp != Equal);
      return Incomparable;
      
    case BK::CanBeGreaterOrEqual:
      lexCmp = compareLexicographically(complexTerm1,complexTerm2);
      if (lexCmp == Greater) return Greater;
      ASSERT(lexCmp != Equal);
      return Incomparable;
      
    case BK::VolatileButNearlyAlwaysLess:
      // same as below
    case BK::VolatileButNearlyAlwaysGreater:
      // same as below
    case BK::AbsolutelyVolatile:
      return Incomparable;
    };

  ICP("ICP0");
#if (defined _SUPPRESS_WARNINGS_) || (defined DEBUG_NAMESPACE)
  return Incomparable;
#endif
}; // Comparison StandardKBOrdering::compare(..)


Comparison 
StandardKBOrdering::compare(const TERM& term1,
			    const TERM& term2)
{
  CALL("compare(const TERM& term1,const TERM& term2)");
  if (term1.isVariable())
    {
      if (term2.isVariable())
	{
	  if (term1 == term2) return Equal;
	}
      else
	{
	  if (term2.First()->containsSymbol(term1)) return Less;
	};
      return Incomparable;
    }
  else
    if (term2.isVariable())
      {
	if (term1.First()->containsSymbol(term2)) return Greater;
	return Incomparable;
      }
    else // both are references 
      return compare(term1.First(),term2.First());
}; // Comparison StandardKBOrdering::compare(const TERM& term1,const TERM& term2)


inline 
Comparison 
StandardKBOrdering::compare(const TERM& term1,
			    const TERM& term2,
			    const InstRet::Substitution* subst)
{
  CALL("compare(const TERM& term1,const TERM& term2,const InstRet::Substitution* subst)");
  if (term1.isVariable())
    {
      if (term2.isVariable())
	{
	  return compare(subst->map(term1.var()),subst->map(term2.var()));
	}
      else
	{
	  Comparison cmp =
	    compare(term2.First(),subst,subst->map(term1.var()));
	  return BK::inverse(cmp);
	};
    }
  else
    if (term2.isVariable())
      {
	return compare(term1.First(),subst,subst->map(term2.var()));
      }
    else
      return compare(term1.First(),term2.First(),subst);
      
}; // Comparison StandardKBOrdering::compare(..)


inline 
Comparison 
StandardKBOrdering::compareLexicographically(const TERM* complexTerm1,
					     const TERM* complexTerm2,
					     const InstRet::Substitution* subst)
{
  CALL("compareLexicographically(const TERM* complexTerm1,const TERM* complexTerm2,const InstRet::Substitution* subst)");
  if ((*complexTerm1) != (*complexTerm2))
    return complexTerm1->comparePrecedence(*complexTerm2);
  const TERM* arg1 = complexTerm1->Args();      
  const TERM* arg2 = complexTerm2->Args();
  while (!arg1->IsNull())
    {
      Comparison cmp = compare(*arg1,*arg2,subst);
      if (cmp != Equal) return cmp;
      arg1 = arg1->Next();
      arg2 = arg2->Next();
    };
  ASSERT(arg2->IsNull());
  return Equal;
}; // Comparison StandardKBOrdering::compareLexicographically(const TERM* complexTerm1,const TERM* complexTerm2,const InstRet::Substitution* subst)

inline
Comparison 
StandardKBOrdering::compare(const TERM& term1,
			    const ForwardMatchingSubstitutionCache* subst,
			    const Flatterm* term2)
{
  CALL("compare(const TERM& term1,const ForwardMatchingSubstitutionCache* subst,const Flatterm* term2)");

  //DF; cout << "COMPARE " << term1 << "\n";
  //DF; cout << "WITH " << term2 << "\n";
  //DF; cout << "AFTER SUBST " << subst << "\n";

  
  if (term1.isVariable())
    {
      return compare(subst->map(term1.var()),term2);
    }
  else
    {
      if (term2->isVariable())
	{
	  ForwardMatchingSubstitutionCache::Instance iter1;
	  iter1.reset(term1.First(),subst);
	  while (iter1.next())
	    {
	      if (iter1.symbol() == term2->symbolRef())
		return Greater;
	    };
	  return Incomparable;
	}
      else
	{
	  const TERM* complexTerm1 = term1.First();
	  _weight1.reset();
	  _weight2.reset();
	  TermWeighting::current()->collectWeight(_weight1,term1,subst);
	  TermWeighting::current()->collectWeight(_weight2,term2);
	  Comparison lexCmp;
	  switch (_weight1.compare(_weight2))
	    {
	    case BK::FailureToCompare:
	      return Incomparable;
	    case BK::AlwaysLess:
	      return Less;
	    case BK::AlwaysEqual:
	      return compareLexicographically(complexTerm1,subst,term2);
	    case BK::AlwaysGreater:
	      return Greater;
	    case BK::CanBeLessOrEqual:
	      lexCmp = compareLexicographically(complexTerm1,subst,term2);
	      if (lexCmp == Less) return Less;
	      ASSERT(lexCmp != Equal);
	      return Incomparable;
	      
	    case BK::CanBeGreaterOrEqual:
	      lexCmp = compareLexicographically(complexTerm1,subst,term2);
	      if (lexCmp == Greater) return Greater;
	      ASSERT(lexCmp != Equal);
	      return Incomparable;
	      
	    case BK::VolatileButNearlyAlwaysLess:
	      // same as below
	    case BK::VolatileButNearlyAlwaysGreater:
	      // same as below
	    case BK::AbsolutelyVolatile:
	      return Incomparable;
	    };  	  
	};
    };
  ICP("ICP0");
#if (defined _SUPPRESS_WARNINGS_) || (defined DEBUG_NAMESPACE)
  return Incomparable;
#endif
}; // Comparison StandardKBOrdering::compare(..)


inline
Comparison 
StandardKBOrdering::compareLexicographically(const TERM* complexTerm1,
					     const ForwardMatchingSubstitutionCache* subst,
					     const Flatterm* term2)
{
  CALL("compareLexicographically(const TERM* complexTerm1,const ForwardMatchingSubstitutionCache* subst,const Flatterm* term2)");
  if (term2->isVariable())
    {
      ForwardMatchingSubstitutionCache::Instance iter1;
      iter1.reset(complexTerm1,subst);
      while (iter1.next())
	{
	  if (iter1.symbol() == term2->symbolRef()) return Greater;
	};
      return Incomparable;
    }
  else
    {
      if ((*complexTerm1) != term2->symbolRef())
	return complexTerm1->comparePrecedence(term2->symbolRef());
      const TERM* arg1 = complexTerm1->Args();
      Flatterm::ConstIterator iter2(term2);
      iter2.NextSym();
      while (!arg1->IsNull())
	{
	  Comparison cmp = compare(*arg1,subst,iter2.CurrPos());
	  if (cmp != Equal) return cmp;
	  arg1 = arg1->Next();
	  iter2.after();
	};
      ASSERT(iter2.EndOfTerm());
      return Equal;
    };
}; // Comparison StandardKBOrdering::compareLexicographically(..)



inline
Comparison 
StandardKBOrdering::compareLexicographically(const TERM& term1,
					     const TERM& term2,
					     const ForwardMatchingSubstitutionCache* subst)
{
  CALL("compareLexicographically(const TERM& term1,const TERM& term2,const ForwardMatchingSubstitutionCache* subst)");
  if (term1.isVariable())
    {
      if (term2.isVariable())
	{
	  return compareLexicographically(subst->map(term1.var()),
					  subst->map(term2.var()));
	}
      else
	{
	  Comparison cmp = compareLexicographically(term2.First(),
						    subst,
						    subst->map(term1.var()));
	  return BK::inverse(cmp);
	};
    }
  else
    if (term2.isVariable())
      {
	return compareLexicographically(term1.First(),
					subst,
					subst->map(term2.var()));
      }
    else
      {
	const TERM* complexTerm1 = term1.First();
	const TERM* complexTerm2 = term2.First();
	if ((*complexTerm1) != (*complexTerm2))
	  return complexTerm1->comparePrecedence(*complexTerm2);
	const TERM* arg1 = complexTerm1->Args();
	const TERM* arg2 = complexTerm2->Args();
	while (!arg1->IsNull())
	  {
	    Comparison cmp = compare(*arg1,*arg2,subst);
	    if (cmp != Equal) return cmp;
	    arg1 = arg1->Next();
	    arg2 = arg2->Next();
	  };
	ASSERT(arg2->IsNull());
	return Equal;
      };     
}; // Comparison StandardKBOrdering::compareLexicographically(const TERM& term1,const TERM& term2,const ForwardMatchingSubstitutionCache* subst)



//===================================================================
#undef DEBUG_NAMESPACE
#ifdef DEBUG_STANDARD_KB_ORDERING
#define DEBUG_NAMESPACE "StandardKBOrdering::SpecialisedComparison"
#endif
#include "debugMacros.hpp"
//===================================================================

StandardKBOrdering::SpecialisedComparison::~SpecialisedComparison()
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
}; // StandardKBOrdering::SpecialisedComparison::~SpecialisedComparison()

void StandardKBOrdering::SpecialisedComparison::load(const TERM& lhs,const TERM& rhs)
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


}; // void StandardKBOrdering::SpecialisedComparison::load(const TERM& lhs,const TERM& rhs)

bool StandardKBOrdering::SpecialisedComparison::lhsVarInstMayOrder()
{
  CALL("lhsVarInstMayOrder()");
  return !_rhsWeight.containsVariableWhichIsNotIn(_lhsWeight);
}; // bool StandardKBOrdering::SpecialisedComparison::lhsVarInstMayOrder()

bool StandardKBOrdering::SpecialisedComparison::rhsVarInstMayOrder()
{
  CALL("rhsVarInstMayOrder()");
  return !_lhsWeight.containsVariableWhichIsNotIn(_rhsWeight);
}; // bool StandardKBOrdering::SpecialisedComparison::rhsVarInstMayOrder()


void StandardKBOrdering::SpecialisedComparison::specialiseGreater()
{
  specialise();
};

void StandardKBOrdering::SpecialisedComparison::specialiseGreaterOrEqual()
{
  specialise();
};

void StandardKBOrdering::SpecialisedComparison::specialiseLess()
{
  specialise();
};

void StandardKBOrdering::SpecialisedComparison::specialiseLessOrEqual()
{
  specialise();
};




inline
StandardKBOrdering::StoredConstraint* StandardKBOrdering::SpecialisedComparison::storedConstraintGreater()
{
  CALL("storedConstraintGreater()");
  if (!_constraintGreater) 
    {
      specialise();
      _constraintGreater = 
	new StoredConstraint(StoredConstraint::Greater,
			     _weightComparison,
			     _lhs,
			     _rhs);
      _constraintGreater->incReferenceCounter();      
    };

  return _constraintGreater;
}; // StandardKBOrdering::StoredConstraint* StandardKBOrdering::SpecialisedComparison::storedConstraintGreater() 


inline
StandardKBOrdering::StoredConstraint* StandardKBOrdering::SpecialisedComparison::storedConstraintGreaterOrEqual()
{
  CALL("storedConstraintGreaterOrEqual()");
  if (!_constraintGreaterOrEqual) 
    {
      specialise();
      _constraintGreaterOrEqual = 
	new StoredConstraint(StoredConstraint::GreaterOrEqual,
			     _weightComparison,
			     _lhs,
			     _rhs);
      _constraintGreaterOrEqual->incReferenceCounter();      
    };

  //DF; cout << "SPECIALISED (GREATER OR EQUAL): " << *this << "\n";
  //DF; cout << "STORED CONSTR (GREATER OR EQUAL): " << _constraintGreaterOrEqual << "\n";


  return _constraintGreaterOrEqual;
}; // StandardKBOrdering::StoredConstraint* StandardKBOrdering::SpecialisedComparison::storedConstraintGreaterOrEqual()


inline
StandardKBOrdering::StoredConstraint* 
StandardKBOrdering::SpecialisedComparison::storedConstraintLess()
{
  CALL("storedConstraintLess()");
  if (!_constraintLess) 
    {
      specialise();
      _constraintLess = 
	new StoredConstraint(StoredConstraint::Less,
			     _weightComparison,
			     _lhs,
			     _rhs);
      _constraintLess->incReferenceCounter();      
    };
  return _constraintLess;
}; // StandardKBOrdering::StoredConstraint* StandardKBOrdering::SpecialisedComparison::storedConstraintLess()

inline
StandardKBOrdering::StoredConstraint* 
StandardKBOrdering::SpecialisedComparison::storedConstraintLessOrEqual()
{
  CALL("storedConstraintLessOrEqual()");  
  if (!_constraintLessOrEqual) 
    {
      specialise();
      _constraintLessOrEqual = 
	new StoredConstraint(StoredConstraint::LessOrEqual,
			     _weightComparison,
			     _lhs,
			     _rhs);
      _constraintLessOrEqual->incReferenceCounter();      
    };

  //DF; cout << "SPECIALISED (LESS OR EQUAL): " << *this << "\n";
  //DF; cout << "STORED CONSTR (LESS OR EQUAL): " << _constraintLessOrEqual << "\n";

  return _constraintLessOrEqual;
}; // StandardKBOrdering::StoredConstraint* StandardKBOrdering::SpecialisedComparison::storedConstraintLessOrEqual()


#ifndef NO_DEBUG_VIS
ostream& StandardKBOrdering::SpecialisedComparison::output(ostream& str) const
{
  str << _lhs << " VS " << _rhs << "\n";
  if (_specialised)
    {
      str << "CATEGORY: " << category() << "\n";
      str << " WEIGHT: " << _weightComparison << "\n";
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
void StandardKBOrdering::SpecialisedComparison::specialise()
{
  CALL("specialise()");
  if (_specialised) return;

  INTERCEPT_FORWARD_CONSTR_CHECK_SPECIALISATION_CALL;
  
  //DF; cout << "SPECIALISE " << _lhs << " VS " << _rhs << "\n";

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
      _category = SimplificationOrdering::SpecialisedComparison::AbsolutelyVolatile;
      break;

    case BK::AlwaysGreater:
      _category = SimplificationOrdering::SpecialisedComparison::AlwaysGreater;
      break;

    case BK::CanBeLessOrEqual:
      _category = 
	SimplificationOrdering::SpecialisedComparison::VolatileButNearlyAlwaysLess;
      break;

    case BK::CanBeGreaterOrEqual:
      _category = 
	SimplificationOrdering::SpecialisedComparison::VolatileButNearlyAlwaysGreater;
      break;

    case BK::VolatileButNearlyAlwaysLess:
      _category = 
	SimplificationOrdering::SpecialisedComparison::VolatileButNearlyAlwaysLess;
      break;

    case BK::VolatileButNearlyAlwaysGreater:
      _category = 
	SimplificationOrdering::SpecialisedComparison::VolatileButNearlyAlwaysGreater;
      break;

    case BK::AbsolutelyVolatile:
      _category = 
	SimplificationOrdering::SpecialisedComparison::AbsolutelyVolatile;
      break;         
    };      
  _specialised = true;
}; // void StandardKBOrdering::SpecialisedComparison::specialise()






//===================================================================
#undef DEBUG_NAMESPACE
#ifdef DEBUG_SIMPLIFICATION_ORDERING
#define DEBUG_NAMESPACE "StandardKBOrdering::ConstraintCheckOnUnifier"
#endif
#include "debugMacros.hpp"
//===================================================================

StandardKBOrdering::ConstraintCheckOnUnifier::~ConstraintCheckOnUnifier()
{
};

bool StandardKBOrdering::ConstraintCheckOnUnifier::holds(const SimplificationOrdering::StoredConstraint* constr,
							 ulong variableIndex)
{
  CALL("holds(..)");

  
  ASSERT(constr);
  const StoredConstraint* nativeConstr = 
    static_cast<const StoredConstraint*>(static_cast<const void*>(constr));


  //DF; cout << "HOLDS? " << nativeConstr << "\n";
  //DF; Unifier::current()->output(cout << "SUBST ",Prefix::outputTerm) << "\n\n\n";  


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
      _ordering->compareOnCurrentUnifier((nativeConstr)->lhs(),\
					 (nativeConstr)->rhs(),\
					 variableIndex));

  
  //DF; cout << "DEBUG CMP = " << debugCmp << "\n";



  switch ((nativeConstr)->targetRelation())
    {
    case StoredConstraint::Greater:
      result = holdsGreater(nativeConstr);
      ASSERT(result == (debugCmp == Greater));
      break;
    case StoredConstraint::GreaterOrEqual:
      result = holdsGreaterOrEqual(nativeConstr);
      ASSERT(result == ((debugCmp == Greater) || (debugCmp == Equal)));
      break;
    case StoredConstraint::Less:
      result = holdsLess(nativeConstr);
      ASSERT(result == (debugCmp == Less));
      break;
    case StoredConstraint::LessOrEqual:
      result = holdsLessOrEqual(nativeConstr);

      //DF; cout << "RESULT = " << result << "\n";

      ASSERT(result == ((debugCmp == Less) || (debugCmp == Equal)));
      break;
    };
  
  INTERCEPT_FORWARD_CONSTR_CHECK_END1;
  return result;
}; // bool StandardKBOrdering::ConstraintCheckOnUnifier::holds(..)



inline
bool StandardKBOrdering::ConstraintCheckOnUnifier::holdsGreater(const StandardKBOrdering::StoredConstraint* constr)
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
      return lexicorgaphicallyGreater(constr->lhs(),constr->rhs());

    case BK::AlwaysGreater:
      INTERCEPT_FORWARD_CONSTR_CHECK_CONCLUSIVE_SHALLOW_CHECK;
      return true;

    case BK::CanBeLessOrEqual:
      ICP("ICP30");
      return false;

    case BK::CanBeGreaterOrEqual:
      // check lexicographically
      return lexicorgaphicallyGreater(constr->lhs(),constr->rhs());

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
}; // bool StandardKBOrdering::ConstraintCheckOnUnifier::holdsGreater(const StandardKBOrdering::StoredConstraint* constr)


inline
bool StandardKBOrdering::ConstraintCheckOnUnifier::holdsGreaterOrEqual(const StandardKBOrdering::StoredConstraint* constr)
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
      return lexicorgaphicallyGreaterOrEqual(constr->lhs(),constr->rhs());

    case BK::AlwaysGreater:
      INTERCEPT_FORWARD_CONSTR_CHECK_CONCLUSIVE_SHALLOW_CHECK;
      return true;

    case BK::CanBeLessOrEqual:	      	      
      ICP("ICP30");
      return false;

    case BK::CanBeGreaterOrEqual:
      // check lexicographically
      return lexicorgaphicallyGreaterOrEqual(constr->lhs(),constr->rhs());

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

}; // bool StandardKBOrdering::ConstraintCheckOnUnifier::holdsGreaterOrEqual(const StandardKBOrdering::StoredConstraint* constr)


inline
bool StandardKBOrdering::ConstraintCheckOnUnifier::holdsLess(const StandardKBOrdering::StoredConstraint* constr)
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
      return lexicorgaphicallyLess(constr->lhs(),constr->rhs());


    case BK::AlwaysGreater:      	      	      
      ICP("ICP20");
      return false;

    case BK::CanBeLessOrEqual:
      // check lexicographically
      return lexicorgaphicallyLess(constr->lhs(),constr->rhs());

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

}; // bool StandardKBOrdering::ConstraintCheckOnUnifier::holdsLess(const StandardKBOrdering::StoredConstraint* constr)



inline
bool StandardKBOrdering::ConstraintCheckOnUnifier::holdsLessOrEqual(const StandardKBOrdering::StoredConstraint* constr)
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
      return lexicorgaphicallyLessOrEqual(constr->lhs(),constr->rhs());

    case BK::AlwaysGreater:         
      ICP("ICP20");
      return false;

    case BK::CanBeLessOrEqual:
      // check lexicographically
      return lexicorgaphicallyLessOrEqual(constr->lhs(),constr->rhs());

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

}; // bool StandardKBOrdering::ConstraintCheckOnUnifier::holdsLessOrEqual(const StandardKBOrdering::StoredConstraint* constr)


inline
FunctionComparison 
StandardKBOrdering::ConstraintCheckOnUnifier::checkGreaterOrEqual(const WeightPolynomial::SpecialisedComparison::Stored& weightComparison)
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
		TermWeightType w = 
		  TermWeighting::current()->computeWeightIfGroundAndLessOrEqual(coeff,
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
      // try to make it BK::AlwaysGreater 
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
		TermWeightType w = 
		  TermWeighting::current()->computeWeightIfGroundAndLessOrEqual(coeff,
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
	    Unifier::Variable* var = 
	      Unifier::current()->variable(mem->hd().var(),_currentVariableIndex);
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
	      Unifier::Variable* var = 
		Unifier::current()->variable(mem->hd().var(),_currentVariableIndex);
	      bool freeVar;
	      void* varInstance = var->Unref(freeVar);
	      if (freeVar)
		{
		  ulong varNum;
		  ulong varIndex;
		  (static_cast<Unifier::Variable*>(varInstance))->NumAndIndex(varNum,varIndex);	    
		  _weightPositivePartInstance.add(coeff,
						  varNum + 
						  (varIndex*VampireKernelConst::MaxNumOfVariables));
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
	    Unifier::Variable* var = 
	      Unifier::current()->variable(mem->hd().var(),_currentVariableIndex);
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
}; // StandardKBOrdering::ConstraintCheckOnUnifier::checkGreaterOrEqual(..)





inline
FunctionComparison 
StandardKBOrdering::ConstraintCheckOnUnifier::checkLessOrEqual(const WeightPolynomial::SpecialisedComparison::Stored& weightComparison)
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
      // try to make it BK::AlwaysLess 
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

}; // StandardKBOrdering::ConstraintCheckOnUnifier::checkLessOrEqual(..)




inline
bool StandardKBOrdering::ConstraintCheckOnUnifier::lexicorgaphicallyGreater(const TERM& lhs,const TERM& rhs)
{
  CALL("lexicorgaphicallyGreater(const TERM& lhs,const TERM& rhs)");
  Comparison cmp = 
    _ordering->compareLexOnCurrentUnifier(lhs,rhs,_currentVariableIndex);
  return (cmp == Greater);
}; // bool StandardKBOrdering::ConstraintCheckOnUnifier::lexicorgaphicallyGreater(const TERM& lhs,const TERM& rhs)

inline
bool StandardKBOrdering::ConstraintCheckOnUnifier::lexicorgaphicallyGreaterOrEqual(const TERM& lhs,const TERM& rhs)
{
  CALL("lexicorgaphicallyGreaterOrEqual(const TERM& lhs,const TERM& rhs)");
  Comparison cmp = 
    _ordering->compareLexOnCurrentUnifier(lhs,rhs,_currentVariableIndex);
  return (cmp == Greater) || (cmp == Equal);   
}; // bool StandardKBOrdering::ConstraintCheckOnUnifier::lexicorgaphicallyGreaterOrEqual(const TERM& lhs,const TERM& rhs)


inline
bool 
StandardKBOrdering::ConstraintCheckOnUnifier::lexicorgaphicallyLess(const TERM& lhs,const TERM& rhs)
{
  return lexicorgaphicallyGreater(rhs,lhs);
};

inline
bool 
StandardKBOrdering::ConstraintCheckOnUnifier::lexicorgaphicallyLessOrEqual(const TERM& lhs,const TERM& rhs)
{
  return lexicorgaphicallyGreaterOrEqual(rhs,lhs);
};


//===================================================================
#undef DEBUG_NAMESPACE
#ifdef DEBUG_SIMPLIFICATION_ORDERING
#define DEBUG_NAMESPACE "StandardKBOrdering::ConstraintCheckOnForwardMatchingSubst"
#endif
#include "debugMacros.hpp"
//===================================================================

StandardKBOrdering::ConstraintCheckOnForwardMatchingSubst::~ConstraintCheckOnForwardMatchingSubst()
{
};

bool 
StandardKBOrdering::ConstraintCheckOnForwardMatchingSubst::holds(const SimplificationOrdering::StoredConstraint* constr,
								 const ForwardMatchingSubstitutionCache* subst)
{
  CALL("holds(..)");
  
  ASSERT(constr);
  const StoredConstraint* nativeConstr = 
    static_cast<const StoredConstraint*>(static_cast<const void*>(constr));

  //DF; cout << "HOLDS? " << nativeConstr << "\n";
  //DF; cout << "SUBST:\n" << subst << "\n";



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
      _ordering->compare(nativeConstr->lhs(),\
			 nativeConstr->rhs(),\
			 subst));

  //DF; cout << "DEBUG CMP = " << debugCmp << "\n";

  _subst = subst;
  switch (nativeConstr->targetRelation())
    {
    case StoredConstraint::Greater:
      result = holdsGreater(nativeConstr);

      //DF; cout << "GREATER = " << result << "\n";

      ASSERT(result == (debugCmp == Greater));
      INTERCEPT_FORWARD_CONSTR_CHECK_END1;
      return result;
    case StoredConstraint::GreaterOrEqual:
      result = holdsGreaterOrEqual(nativeConstr);
      ASSERT(result == ((debugCmp == Greater) || (debugCmp == Equal)));
      INTERCEPT_FORWARD_CONSTR_CHECK_END1;
      return result;
    case StoredConstraint::Less:
      result = holdsLess(nativeConstr);

      //DF; cout << "LESS = " << result << "\n";

      ASSERT(result == (debugCmp == Less));
      INTERCEPT_FORWARD_CONSTR_CHECK_END1;
      return result;
    case StoredConstraint::LessOrEqual:
      result = holdsLessOrEqual(nativeConstr);
      ASSERT(result == ((debugCmp == Less) || (debugCmp == Equal)));
      INTERCEPT_FORWARD_CONSTR_CHECK_END1;
      return result;
    };
  ICP("ICP0");
#if (defined _SUPPRESS_WARNINGS_) || (defined DEBUG_NAMESPACE)
  INTERCEPT_FORWARD_CONSTR_CHECK_END1;
  return false;
#endif
}; // bool StandardKBOrdering::ConstraintCheckOnForwardMatchingSubst::holds(..)

 
inline
bool 
StandardKBOrdering::ConstraintCheckOnForwardMatchingSubst::holdsGreater(const StoredConstraint* constr)
{
  CALL("holdsGreater(..)");

  //DF; cout << "HOLDS GREATER? " << constr << "\n";
  //DF; cout << "SUBST:\n" << _subst << "\n";

  switch (checkGreaterOrEqual(constr->weightComparison()))
    {
    case BK::FailureToCompare:
      return false;

    case BK::AlwaysLess:
      ICP("ICP10");
      return false; 

    case BK::AlwaysEqual:
      // check lexicographically
      return lexicorgaphicallyGreater(constr->lhs(),constr->rhs());

    case BK::AlwaysGreater:
      return true;

    case BK::CanBeLessOrEqual:
      ICP("ICP30");
      return false;

    case BK::CanBeGreaterOrEqual:
      // check lexicographically
      return lexicorgaphicallyGreater(constr->lhs(),constr->rhs());

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
}; // bool StandardKBOrdering::ConstraintCheckOnForwardMatchingSubst::holdsGreater(const StoredConstraint* constr)




inline
bool 
StandardKBOrdering::ConstraintCheckOnForwardMatchingSubst::holdsGreaterOrEqual(const StoredConstraint* constr)
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
      return lexicorgaphicallyGreaterOrEqual(constr->lhs(),constr->rhs());

    case BK::AlwaysGreater:
      return true;

    case BK::CanBeLessOrEqual:
      ICP("ICP30");
      return false;

    case BK::CanBeGreaterOrEqual:
      // check lexicographically
      return lexicorgaphicallyGreaterOrEqual(constr->lhs(),constr->rhs());


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

}; // bool StandardKBOrdering::ConstraintCheckOnForwardMatchingSubst::holdsGreaterOrEqual(const StoredConstraint* constr)

inline
bool 
StandardKBOrdering::ConstraintCheckOnForwardMatchingSubst::holdsLess(const StoredConstraint* constr)
{
  CALL("holdsLess(..)");

  //DF; cout << "HOLDS LESS? "; //<< constr << "\n";
  //DF; cout << "SUBST:\n" << _subst << "\n";

  FunctionComparison weightComp = 
    checkLessOrEqual(constr->weightComparison());

  //DF; cout << "WEIGHT COMP = " << weightComp << "\n";

  switch (weightComp)
    {
    case BK::FailureToCompare:
      return false;

    case BK::AlwaysLess:
      return true; 

    case BK::AlwaysEqual:
      // check lexicographically
      return lexicorgaphicallyLess(constr->lhs(),constr->rhs());

    case BK::AlwaysGreater:    	      
      ICP("ICP20");
      return false;

    case BK::CanBeLessOrEqual:
      // check lexicographically
      return lexicorgaphicallyLess(constr->lhs(),constr->rhs());


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

}; // bool StandardKBOrdering::ConstraintCheckOnForwardMatchingSubst::holdsLess(const StoredConstraint* constr)

inline
bool 
StandardKBOrdering::ConstraintCheckOnForwardMatchingSubst::holdsLessOrEqual(const StoredConstraint* constr)
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
      return lexicorgaphicallyLessOrEqual(constr->lhs(),constr->rhs());


    case BK::AlwaysGreater:   	      
      ICP("ICP20");
      return false;

    case BK::CanBeLessOrEqual:
      // check lexicographically
      return lexicorgaphicallyLessOrEqual(constr->lhs(),constr->rhs());
      

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
}; // bool StandardKBOrdering::ConstraintCheckOnForwardMatchingSubst::holdsLessOrEqual(const StoredConstraint* constr)
  
inline
FunctionComparison 
StandardKBOrdering::ConstraintCheckOnForwardMatchingSubst::checkGreaterOrEqual(const WeightPolynomial::SpecialisedComparison::Stored& weightComparison)
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
      // try to make it BK::AlwaysGreater 
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
}; // FunctionComparison StandardKBOrdering::ConstraintCheckOnForwardMatchingSubst::checkGreaterOrEqual(const WeightPolynomial::SpecialisedComparison::Stored& weightComparison)


inline
FunctionComparison 
StandardKBOrdering::ConstraintCheckOnForwardMatchingSubst::checkLessOrEqual(const WeightPolynomial::SpecialisedComparison::Stored& weightComparison)
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
      // try to make it BK::AlwaysLess 
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

}; // FunctionComparison StandardKBOrdering::ConstraintCheckOnForwardMatchingSubst::checkLessOrEqual(const WeightPolynomial::SpecialisedComparison::Stored& weightComparison)




inline
bool StandardKBOrdering::ConstraintCheckOnForwardMatchingSubst::lexicorgaphicallyGreater(const TERM& lhs,const TERM& rhs)
{
  CALL("lexicorgaphicallyGreater(const TERM& lhs,const TERM& rhs)");
  Comparison cmp =
    _ordering->compareLexicographically(lhs,rhs,_subst);
  return (cmp == Greater);
}; // bool StandardKBOrdering::ConstraintCheckOnForwardMatchingSubst::lexicorgaphicallyGreater(const TERM& lhs,const TERM& rhs)

inline
bool StandardKBOrdering::ConstraintCheckOnForwardMatchingSubst::lexicorgaphicallyGreaterOrEqual(const TERM& lhs,const TERM& rhs)
{
  CALL("lexicorgaphicallyGreaterOrEqual(const TERM& lhs,const TERM& rhs)");  
  Comparison cmp =
    _ordering->compareLexicographically(lhs,rhs,_subst);
  return (cmp == Greater) || (cmp == Equal);
}; // bool StandardKBOrdering::ConstraintCheckOnForwardMatchingSubst::lexicorgaphicallyGreaterOrEqual(const TERM& lhs,const TERM& rhs)

inline
bool 
StandardKBOrdering::ConstraintCheckOnForwardMatchingSubst::lexicorgaphicallyLess(const TERM& lhs,const TERM& rhs)
{
  return lexicorgaphicallyGreater(rhs,lhs);
};

inline
bool 
StandardKBOrdering::ConstraintCheckOnForwardMatchingSubst::lexicorgaphicallyLessOrEqual(const TERM& lhs,const TERM& rhs)
{
  return lexicorgaphicallyGreaterOrEqual(rhs,lhs);
};



//===================================================================
#undef DEBUG_NAMESPACE
#ifdef DEBUG_STANDARD_KB_ORDERING
#define DEBUG_NAMESPACE "StandardKBOrdering::CompiledConstraintForUnifiers"
#endif
#include "debugMacros.hpp"
//===================================================================


StandardKBOrdering::CompiledConstraintForUnifiers::~CompiledConstraintForUnifiers() 
{
};  

void StandardKBOrdering::CompiledConstraintForUnifiers::load(PrefixSym* lhs,PrefixSym* rhs)
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

  _topLevelWeightComparison.compare(_lhsWeight,_rhsWeight);
  INTERCEPT_BACKWARD_CONSTR_CHECK_END1;
}; // void StandardKBOrdering::CompiledConstraintForUnifiers::load(PrefixSym* lhs,PrefixSym* rhs)

bool StandardKBOrdering::CompiledConstraintForUnifiers::lhsVarInstMayOrder()
{
  CALL("lhsVarInstMayOrder()");
  return !_rhsWeight.containsVariableWhichIsNotIn(_lhsWeight);
}; // bool StandardKBOrdering::CompiledConstraintForUnifiers::lhsVarInstMayOrder()

 
bool StandardKBOrdering::CompiledConstraintForUnifiers::rhsVarInstMayOrder()
{
  CALL("rhsVarInstMayOrder()");
  return !_lhsWeight.containsVariableWhichIsNotIn(_rhsWeight);
}; // bool StandardKBOrdering::CompiledConstraintForUnifiers::rhsVarInstMayOrder()

void StandardKBOrdering::CompiledConstraintForUnifiers::compileGreaterOrEqual()
{
  CALL("compileGreaterOrEqual()");

  //DF; Prefix::outputTerm(cout << "\n\n\nCOMPILE ",_lhs);
  //DF; Prefix::outputTerm(cout << " >= ",_rhs) << "\n";
  //DF; cout << "STRENGTH = " << currentSpecialisationStrength() << "\n\n\n"; 

  INTERCEPT_BACKWARD_CONSTR_CHECK_COMPILATION_CALL;
  INTERCEPT_BACKWARD_CONSTR_CHECK_BEGIN1;
  ++(_ordering->_numberOfCompiledConstraintsOnUnifiers);

#ifndef NO_DEBUG
  _targetRelation = GreaterOrEqual;
#endif


  DOP(const void* debugUnifState = Unifier::current()->state());

  Command* mainCode;
  WeightComparisonCommand* weightComparisonCode;
  bool complete;
  ulong specialisationStrength;

 try_to_compile:
  mainCode = _mainCode.memory();
  weightComparisonCode = _weightComparisonCode.memory();
  specialisationStrength = currentSpecialisationStrength();

  if (!compileGreaterOrEqual(_lhs,
			     _rhs,
			     0,
			     mainCode,
			     _mainCode.memory() + _mainCode.size() - 1,
			     weightComparisonCode,
			     _weightComparisonCode.memory() + _weightComparisonCode.size(),
			     complete,
			     specialisationStrength))  
  {
    DOP(_mainCode.unfreeze());
    DOP(_weightComparisonCode.unfreeze());
    _mainCode.expand();
    _weightComparisonCode.expand();
    DOP(_mainCode.freeze());
    DOP(_weightComparisonCode.freeze());
    goto try_to_compile;    
  };
	  
	  
  if (!complete)  mainCode->setTag(Command::RetSuccess); // equal terms
  ASSERT(Unifier::current()->state() == debugUnifState);

  INTERCEPT_BACKWARD_CONSTR_CHECK_END1;

}; // void StandardKBOrdering::CompiledConstraintForUnifiers::compileGreaterOrEqual()



bool StandardKBOrdering::CompiledConstraintForUnifiers::compileGreaterOrEqual(PrefixSym* lhs,
									      PrefixSym* rhs,
									      WeightPolynomial::SpecialisedComparison* assumedWeightRelations,
									      Command*& mainCode,
									      const Command* endOfMainCode,
									      WeightComparisonCommand*& weightComparisonCode,
									      const WeightComparisonCommand* endOfWeightComparisonCode,
									      bool& complete,
									      ulong& specialisationStrength)
{
  CALL("compileGreaterOrEqual(PrefixSym* lhs,...)");
  ASSERT(mainCode <= endOfMainCode);
  ASSERT(weightComparisonCode <= endOfWeightComparisonCode);
  
  if (mainCode == endOfMainCode) return false;

  WeightComparisonCommand* c;

  bool result = true;
  complete = false;

  Command* lastGoto;
  bool lexComplete;
  ulong specialisationStrength1;
  Unifier::Variable* lhsVar = 0;
  PrefixSym* lhsComplexTerm;
  Unifier::Variable* rhsVar = 0;
  PrefixSym* rhsComplexTerm;
  if (lhs->Head().isVariable())
    {
      lhsVar = lhs->Head().var();
      bool freeVar;
      void* instance = lhsVar->Unref(freeVar);
      if (freeVar)
	{
	  lhsVar = static_cast<Unifier::Variable*>(instance);
	}
      else
	{
	  lhsComplexTerm = static_cast<PrefixSym*>(instance);
	};
    }
  else
    lhsComplexTerm = lhs;

  if (rhs->Head().isVariable())
    {
      rhsVar = rhs->Head().var();
      bool freeVar;
      void* instance = rhsVar->Unref(freeVar);
      if (freeVar)
	{
	  rhsVar = static_cast<Unifier::Variable*>(instance);
	}
      else
	{
	  rhsComplexTerm = static_cast<PrefixSym*>(instance);
	};
    }
  else
    rhsComplexTerm = rhs;
  
  ASSERT(mainCode < endOfMainCode);

  if (lhsVar)
    {     
      if (rhsVar)
	{
	  // X vs Y
	  if (lhsVar != rhsVar)
	    {
	      mainCode->setTag(Command::CompareXY_IfCmpGrRetSucc_IfCmpNotEqRetFail);
	      mainCode->setVarX(lhsVar);
	      mainCode->setVarY(rhsVar);  
	      ++mainCode;  
	    };
	}
      else // X vs T
	{
	  if (OccurCheck::Occurs(lhsVar,rhsComplexTerm))
	    {
	      mainCode->setTag(Command::RetFailure);
	      complete = true;
	    }
	  else
	    {
	      mainCode->setTag(Command::CompareXT_IfCmpGrRetSucc_IfCmpNotEqRetFail);
	      mainCode->setVarX(lhsVar);
	      mainCode->setTermT(rhsComplexTerm);	
	    };  
	  ++mainCode;  
	};
      return true;
    }
  else
    {
      if (rhsVar)
	{
	  // S vs Y
	  if (OccurCheck::Occurs(rhsVar,lhsComplexTerm))
	    {
	      mainCode->setTag(Command::RetSuccess);
	      complete = true;
	    }
	  else
	    {
	      mainCode->setTag(Command::CompareSY_IfCmpGrRetSucc_IfCmpNotEqRetFail);
	      mainCode->setTermS(lhsComplexTerm);
	      mainCode->setVarY(rhsVar);	  
	    };
	  ++mainCode;  
	  return true;
	};
    };

  ASSERT(!lhsVar);
  ASSERT(!rhsVar);
  Prefix::CorrectBracket(lhsComplexTerm);
  Prefix::CorrectBracket(rhsComplexTerm);
  if (!specialisationStrength)
    {
      mainCode->setTag(Command::CompareST_IfCmpGrRetSucc_IfCmpNotEqRetFail);
      mainCode->setTermS(lhsComplexTerm);
      mainCode->setTermT(rhsComplexTerm);  
      ++mainCode;  
      return true;
    };
    
  --specialisationStrength;
  
    
  WeightPolynomial::SpecialisedComparison* weightComparison = 
    _ordering->reserveSpecialisedWeightComparison();
  WeightPolynomial::SpecialisedComparison* simplifiedWeightComparison = 
    _ordering->reserveSpecialisedWeightComparison();
  WeightPolynomial::SpecialisedComparison* spareWeightComparison1 = 
    _ordering->reserveSpecialisedWeightComparison();
  WeightPolynomial::SpecialisedComparison* spareWeightComparison2 = 
    _ordering->reserveSpecialisedWeightComparison();
  
  WeightComparisonCommand* initWCCodePosition;

  _weight1.reset();
  _weight2.reset();
  
  TermWeighting::current()->collectWeight(_weight1,lhsComplexTerm);
  TermWeighting::current()->collectWeight(_weight2,rhsComplexTerm);
  
  weightComparison->compare(_weight1,_weight2);

  //DF; cout << "----WC = " << *weightComparison << "\n";
  
  switch (weightComparison->category())
    {
    case BK::FailureToCompare:
      // same as below
    case BK::AlwaysLess:
      ASSERT(mainCode < endOfMainCode);
      mainCode->setTag(Command::RetFailure);
      complete = true;
      ++mainCode;    
      goto finish;

    case BK::AlwaysEqual:
      result = 
	compileGreaterOrEqualLex(lhsComplexTerm,
				 rhsComplexTerm,
				 assumedWeightRelations,
				 mainCode,
				 endOfMainCode,
				 weightComparisonCode,
				 endOfWeightComparisonCode,
				 complete,
				 specialisationStrength);  	
      goto finish;

    case BK::AlwaysGreater:
      ASSERT(mainCode < endOfMainCode);
      mainCode->setTag(Command::RetSuccess);
      complete = true;
      ++mainCode;  
      goto finish;

    case BK::CanBeLessOrEqual: 
      break; 

    case BK::CanBeGreaterOrEqual: 
      // same as below
    case BK::VolatileButNearlyAlwaysLess: 
      // same as below
    case BK::VolatileButNearlyAlwaysGreater: 
      // same as below
    case BK::AbsolutelyVolatile: 

      // try to show that it is AlwaysGreater under the assumptions

      if (weightComparison->isAlwaysGreaterUnderAssumptions(assumedWeightRelations,
							    simplifiedWeightComparison, 
							    spareWeightComparison1,
							    spareWeightComparison2))
	{	  

	  //DF; cout << "----WC --> AlwaysGreater under the assumptions\n";
	  ASSERT(mainCode < endOfMainCode);
	  mainCode->setTag(Command::RetSuccess);
	  complete = true;
	  ++mainCode;  
	  goto finish;
	};
      break;
							
    }; // switch (weightComparison->category())



  switch (weightComparison->category())
    {	      
    case BK::CanBeLessOrEqual: 
      // same as below 
    case BK::VolatileButNearlyAlwaysLess: 
      // same as below
    case BK::VolatileButNearlyAlwaysGreater: 
      // same as below
    case BK::AbsolutelyVolatile:
      // try to show that it is AlwaysLess under the assumptions
      if (weightComparison->isAlwaysLessUnderAssumptions(assumedWeightRelations,
							 simplifiedWeightComparison, 
							 spareWeightComparison1,
							 spareWeightComparison2))
	{	  

	  //DF; cout << "----WC --> AlwaysLess under the assumptions\n";
	  ASSERT(mainCode < endOfMainCode);
	  mainCode->setTag(Command::RetFailure);
	  complete = true;
	  ++mainCode;  
	  goto finish;
	};
      break;

    default: 
      break;
    }; // switch (weightComparison->category())

  // simplify weightComparison here using assumed equalities from assumedWeightRelations
  
  
  if (weightComparison->simplifyByEqualities(assumedWeightRelations,
					     simplifiedWeightComparison,
					     spareWeightComparison1,
					     spareWeightComparison2))
    {
      WeightPolynomial::SpecialisedComparison* tmp = weightComparison;
      weightComparison = simplifiedWeightComparison;
      simplifiedWeightComparison = tmp;
    };
  


  //DF; cout << "----SIMPLIFIED WC = " << (*weightComparison) << "\n";
  
  switch (weightComparison->category())
    {
    case BK::FailureToCompare: 
      // same as below

    case BK::AlwaysLess:
      ASSERT(mainCode < endOfMainCode);
      mainCode->setTag(Command::RetFailure);
      complete = true;
      ++mainCode;    
      goto finish;

    case BK::AlwaysEqual:
      COP("COP200");
      result = 
	compileGreaterOrEqualLex(lhsComplexTerm,
				 rhsComplexTerm,
				 assumedWeightRelations,
				 mainCode,
				 endOfMainCode,
				 weightComparisonCode,
				 endOfWeightComparisonCode,
				 complete,
				 specialisationStrength);  	
      goto finish;

    case BK::AlwaysGreater:
      ASSERT(mainCode < endOfMainCode);
      mainCode->setTag(Command::RetSuccess);
      complete = true;
      ++mainCode;  
      goto finish;

    case BK::CanBeLessOrEqual: 
      // same as below
    case BK::CanBeGreaterOrEqual: 
      // same as below
    case BK::VolatileButNearlyAlwaysLess: 
      // same as below
    case BK::VolatileButNearlyAlwaysGreater: 
      // same as below
    case BK::AbsolutelyVolatile: 
      goto compile_weight_comparison;
    };

 compile_weight_comparison:

  initWCCodePosition = weightComparisonCode;

  if (endOfWeightComparisonCode - weightComparisonCode < 2)
    {
      result = false;
      goto finish;
    };  

  ASSERT(weightComparisonCode < endOfWeightComparisonCode);

  if (!weightComparison->compileGreaterOrEqual(weightComparisonCode,
					       endOfWeightComparisonCode - weightComparisonCode))
    {
      result = false;
      goto finish;
    };

  ASSERT(weightComparisonCode <= endOfWeightComparisonCode);
  
  // naturalise the variables
  for (c = initWCCodePosition;
       c < weightComparisonCode;
       ++c)
    if (c->requiresVar())
      {
	c->setNativeVar(static_cast<const void*>(Unifier::current()->variable(c->var())));
      };

  ASSERT(mainCode < endOfMainCode);
  if (weightComparison->category() == BK::CanBeLessOrEqual)
    {
      // in this case we know that the weight comparison 
      // can only produce AlwaysEqual or FailureToCompare
      mainCode->setTag(Command::IfWCFailRetFail);
    }
  else
    {
      mainCode->setTag(Command::IfWCFailRetFail_IfWCAlwGrRetSucc_IfWCCanBeGrOrEqGotoL);
    };
  mainCode->setWeightComparisonSubroutine(initWCCodePosition);
  lastGoto = mainCode;
  ++mainCode;  

  // assume weight equality
  
  weightComparison->assumeRelation(BK::AlwaysEqual);
  weightComparison->setNext(assumedWeightRelations);

  if (weightComparison->category() == BK::CanBeLessOrEqual)
    {
      specialisationStrength1 = specialisationStrength;
      specialisationStrength = 0UL; 
    }
  else
    {
      specialisationStrength1 = specialisationStrength/2;
      specialisationStrength -= specialisationStrength1;
    };


  result = 
    compileGreaterOrEqualLex(lhsComplexTerm,
			     rhsComplexTerm,
			     weightComparison,
			     mainCode,
			     endOfMainCode,
			     weightComparisonCode,
			     endOfWeightComparisonCode,
			     lexComplete,
			     specialisationStrength1);

  specialisationStrength += specialisationStrength1;

  ASSERT(weightComparisonCode <= endOfWeightComparisonCode);
  

  if (!result) goto finish;
  
  if (weightComparison->category() != BK::CanBeLessOrEqual)
    {
      ASSERT(lastGoto->requiresLabel());
      lastGoto->setLabelL(mainCode);

      // assume weight inequality
      weightComparison->assumeRelation(BK::CanBeGreaterOrEqual);
      weightComparison->setNext(assumedWeightRelations);


      result = 
	compileGreaterOrEqualLex(lhsComplexTerm,
				 rhsComplexTerm,
				 weightComparison,
				 mainCode,
				 endOfMainCode,
				 weightComparisonCode,
				 endOfWeightComparisonCode,
				 lexComplete,
				 specialisationStrength); 
    };


 finish:
  ASSERT(weightComparisonCode <= endOfWeightComparisonCode);
  _ordering->releaseSpecialisedWeightComparison(weightComparison);
  _ordering->releaseSpecialisedWeightComparison(simplifiedWeightComparison);
  _ordering->releaseSpecialisedWeightComparison(spareWeightComparison1);
  _ordering->releaseSpecialisedWeightComparison(spareWeightComparison2);
  return result;
}; // bool StandardKBOrdering::CompiledConstraintForUnifiers::compileGreaterOrEqual(PrefixSym* lhs,...)




bool 
StandardKBOrdering::CompiledConstraintForUnifiers::compileGreaterOrEqualLex(PrefixSym* lhsComplexTerm,
									    PrefixSym* rhsComplexTerm,
									    WeightPolynomial::SpecialisedComparison* assumedWeightRelations,
									    Command*& mainCode,
									    const Command* endOfMainCode,
									    WeightComparisonCommand*& weightComparisonCode,
									    const WeightComparisonCommand* endOfWeightComparisonCode,
									    bool& complete,
									    ulong& specialisationStrength)
{
  CALL("compileGreaterOrEqualLex(..)");
  ASSERT(mainCode <= endOfMainCode);
  ASSERT(weightComparisonCode <= endOfWeightComparisonCode);
  ASSERT(lhsComplexTerm->Head().IsFunctor());
  ASSERT(rhsComplexTerm->Head().IsFunctor());

  if (lhsComplexTerm->Head().Func() == rhsComplexTerm->Head().Func())
    {
      // recursion needed 
      if (lhsComplexTerm->Head().Func().arity())
	{
	  PrefixIterPool::AutoObject lhsIter(_ordering->_prefixIterPool);
	  PrefixIterPool::AutoObject rhsIter(_ordering->_prefixIterPool);
	  lhsIter.object().Reset(lhsComplexTerm);
	  rhsIter.object().Reset(rhsComplexTerm);
	  lhsIter.object().SkipSym();
	  rhsIter.object().SkipSym();
	  
	  Unifier::Variable*** unifierState = Unifier::current()->MarkState();

	  do 
	    {
	      Prefix::CorrectBracket(lhsIter.object().CurrSubterm());
	      Prefix::CorrectBracket(rhsIter.object().CurrSubterm());
	      if (!compileGreaterOrEqual(lhsIter.object().CurrSubterm(),
					 rhsIter.object().CurrSubterm(),
					 assumedWeightRelations,
					 mainCode,
					 endOfMainCode,
					 weightComparisonCode,
					 endOfWeightComparisonCode,
					 complete,
					 specialisationStrength)) 
		
		{
		  Unifier::current()->UnbindDownto(unifierState);
		  return false;
		};

	      
	      ASSERT(weightComparisonCode <= endOfWeightComparisonCode);

	      if (complete)
		{
		  Unifier::current()->UnbindDownto(unifierState);
		  return true;
		};


	      if (!UNIFICATION::UnifyArbitrary(lhsIter.object().CurrSubterm(),
					       rhsIter.object().CurrSubterm()))
		{
		  if (mainCode == endOfMainCode) 
		    {
		      Unifier::current()->UnbindDownto(unifierState);
		      return false;
		    };
		  mainCode->setTag(Command::RetFailure);
		  complete = true;
		  ++mainCode; 
		  Unifier::current()->UnbindDownto(unifierState);   
		  return true;
		};

	      lhsIter.object().SkipTerm();
	      rhsIter.object().SkipTerm();
	    }
	  while (lhsIter.object().NotEmpty());

	  ASSERT(rhsIter.object().Empty());
	  Unifier::current()->UnbindDownto(unifierState);
	};
    }
  else // different top functions
    {
      if (mainCode == endOfMainCode) return false;
      if (lhsComplexTerm->Head().Func().hasGreaterPrecedenceThan(rhsComplexTerm->Head().Func()))
	{
	  mainCode->setTag(Command::RetSuccess);
	}
      else
	{
	  mainCode->setTag(Command::RetFailure);
	};
      complete = true;
      ++mainCode;	
    };

  return true;
}; // bool StandardKBOrdering::CompiledConstraintForUnifiers::compileGreaterOrEqualLex(






bool StandardKBOrdering::CompiledConstraintForUnifiers::holdsOnCurrentUnifier()
{
  CALL("holdsOnCurrentUnifier()");

  ++(_ordering->_numberOfConstraintChecksOnUnifiers);

  //DF; output(cout << "\n\n\nHOLDS?\n") << "\n\n";   
  //DF; Unifier::current()->output(cout << "   SUBST:\n",Prefix::outputTerm) << "\n\n\n";  


#ifdef MEASURE_BACKWARD_CONSTR_CHECK
  INTERCEPT_BACKWARD_CONSTR_CHECK_BEGIN2;
  SimplificationOrdering::current()->greaterOrEqualWithCurrentUnifier(_lhs,_rhs);
  INTERCEPT_BACKWARD_CONSTR_CHECK_END2;
#endif

  INTERCEPT_BACKWARD_CONSTR_CHECK_NEW_CHECK;
  INTERCEPT_BACKWARD_CONSTR_CHECK_BEGIN1;


  Command* command = _mainCode.memory();
  FunctionComparison weightComp;
  Comparison cmp;
  bool freeVar1;
  void* instance1;
  bool freeVar2;
  void* instance2;


 next_command:

  switch (command->tag())
    {
    case Command::RetSuccess:
      goto return_true; 
    case Command::RetFailure:
      goto return_false;

    case Command::CompareST_IfCmpGrRetSucc_IfCmpNotEqRetFail:
      /*
	cmp = compare(S@,T@);
	if (cmp == Greater) return Success;	  
	if (cmp != Equal) return Failure;
      */
      cmp =
	_ordering->compareOnCurrentUnifier(command->termS(),
					   command->termT());
      if (cmp == BK::Greater) goto return_true;
      if (cmp != BK::Equal) goto return_false;      
      ++command;
      goto next_command;

      
    case Command::CompareSY_IfCmpGrRetSucc_IfCmpNotEqRetFail:
      /*
	cmp = compare(S@,Y@);
	if (cmp == Greater) return Success;	  
	if (cmp != Equal) return Failure;
      */
      instance2 = command->varY()->Unref(freeVar2);
      if (freeVar2)
	{
	  if (OccurCheck::Occurs(static_cast<Unifier::Variable*>(instance2),command->termS()))
	    {
	      goto return_true;
	    }
	  else
	    goto return_false;
	}
      else
	{	  
	  cmp =
	    _ordering->compareOnCurrentUnifier(command->termS(),
					       static_cast<PrefixSym*>(instance2));
	  if (cmp == BK::Greater) goto return_true;
	  if (cmp != BK::Equal) goto return_false;  
	  ++command;
	  goto next_command; 
	};
      ICP("ICP10");

    case Command::CompareXT_IfCmpGrRetSucc_IfCmpNotEqRetFail:
      /*
	cmp = compare(X@,T@);
	if (cmp == Greater) return Success;	  
	if (cmp != Equal) return Failure;
      */
      instance1 = command->varX()->Unref(freeVar1);
      if (freeVar1)
	{
	  goto return_false;
	}
      else
	{
	  cmp =
	    _ordering->compareOnCurrentUnifier(static_cast<PrefixSym*>(instance1),
					       command->termT());
	  if (cmp == BK::Greater) goto return_true;
	  if (cmp != BK::Equal) goto return_false;  
	  ++command;
	  goto next_command; 
	};
      ICP("ICP20");

    case Command::CompareXY_IfCmpGrRetSucc_IfCmpNotEqRetFail:
      /*
	cmp = compare(X@,Y@);
	if (cmp == Greater) return Success;	  
	if (cmp != Equal) return Failure;
      */
      
      instance1 = command->varX()->Unref(freeVar1);
      instance2 = command->varY()->Unref(freeVar2);
      if (freeVar1)
	{
	  if (instance1 != instance2) goto return_false;
	}
      else
	if (freeVar2)
	  {
	    if (OccurCheck::Occurs(static_cast<Unifier::Variable*>(instance2),
				   static_cast<PrefixSym*>(instance1)))
	      {
		goto return_true;
	      }
	    else
	      goto return_false;
	  }
	else
	  {
	    cmp =
	    _ordering->compareOnCurrentUnifier(static_cast<PrefixSym*>(instance1),
					       static_cast<PrefixSym*>(instance2));
	    if (cmp == BK::Greater) goto return_true;
	    if (cmp != BK::Equal) goto return_false;  	    
	  };
      ++command;
      goto next_command; 
					    



    case Command::IfWCFailRetFail_IfWCAlwGrRetSucc_IfWCCanBeGrOrEqGotoL:      
      /*
	if WC(@) = FailureToCompare return Failure;
	if WC(@) = AlwaysGreater return Success;
	ASSERT(WC(@) = AlwaysEqual || WC(@) = CanBeGreaterOrEqual);
	if WC(@) = CanBeGreaterOrEqual goto L;
      */
      
      weightComp = 
	interpretWeightComparisonCode(command->weightComparisonSubroutine());
      
      //DF; cout << "\n\n\nWEIGHT COMPARISON RETURNS: " << weightComp << "\n\n\n";
      if (weightComp == BK::FailureToCompare) goto return_false;
      if (weightComp == BK::AlwaysGreater) goto return_true;
      ASSERT((weightComp == BK::CanBeGreaterOrEqual) || 
	     (weightComp == BK::AlwaysEqual));
      if (weightComp == BK::CanBeGreaterOrEqual) 
	{
	  command = command->labelL();
	}
      else
	++command;
      goto next_command;

    case Command::IfWCFailRetFail:
      /*
	if WC(@) = FailureToCompare return Failure;
	ASSERT(WC(@) = AlwaysEqual);
      */ 
      weightComp = 
	interpretWeightComparisonCode(command->weightComparisonSubroutine());
      if (weightComp == BK::FailureToCompare) goto return_false;
      ASSERT(weightComp == BK::AlwaysEqual);
      ++command;
      goto next_command;


      
#ifdef DEBUG_NAMESPACE
    default: ICP("ICP50"); goto return_false;
#else
#ifdef _SUPPRESS_WARNINGS_
    default: goto return_false;
#endif
#endif
    };
  
 return_true:

  //DF; cout << "SUCCESS\n";


  ASSERT((_targetRelation != GreaterOrEqual) || (SimplificationOrdering::current()->greaterOrEqualWithCurrentUnifier(_lhs,_rhs)));

  INTERCEPT_BACKWARD_CONSTR_CHECK_END1;
  return true;

 return_false:

  //DF; cout << "FAILURE\n";

  // v Very strong assumption!  
  ASSERT((_targetRelation != GreaterOrEqual) || (!SimplificationOrdering::current()->greaterOrEqualWithCurrentUnifier(_lhs,_rhs)));

  INTERCEPT_BACKWARD_CONSTR_CHECK_END1;
  return false;

}; // bool StandardKBOrdering::CompiledConstraintForUnifiers::holdsOnCurrentUnifier()




#ifndef NO_DEBUG_VIS
ostream& StandardKBOrdering::CompiledConstraintForUnifiers::output(ostream& str) const
{
  Prefix::outputTerm(str << "CONSTRAINT: ",_lhs);
  switch (_targetRelation)
    {
    case GreaterOrEqual: str << " >= "; break;
    default: str << "???";
    };
  Prefix::outputTerm(str,_rhs) << "\n";

  cout << "TOP LEVEL WEIGHT COMPARISON:\n" << _topLevelWeightComparison << "\n";
  str << "*** CODE: **********************\n";
  const Command* mainCodeCommand = _mainCode.memory();
  ulong gotosPassed = 0UL;
 another_branch:
  while (!mainCodeCommand->isTerminal())
    {
      mainCodeCommand->output(str,_mainCode.memory());
      if (mainCodeCommand->requiresLabel())
	++gotosPassed;
      ++mainCodeCommand;
    };
   mainCodeCommand->output(str,_mainCode.memory());
   if (gotosPassed)
     {
       --gotosPassed;
       ++mainCodeCommand;
       goto another_branch;
     };
  str << "*********** END OF CODE ********\n";
  return str;
};
#endif




inline 
Comparison 
StandardKBOrdering::CompiledConstraintForUnifiers::compareLexicographically(PrefixSym* term1,PrefixSym* term2)
{
  CALL("compareLexicographically(PrefixSym* term1,PrefixSym* term2)");
  ASSERT(term1->Head().IsFunctor());
  ASSERT(term2->Head().IsFunctor());
  ASSERT(term1->Head() == term2->Head()); 
  return _ordering->compareLexOnCurrentUnifier(term1,term2);
}; // Comparison StandardKBOrdering::CompiledConstraintForUnifiers::compareLexicographically(PrefixSym* term1,PrefixSym* term2)


inline 
Comparison 
StandardKBOrdering::CompiledConstraintForUnifiers::compareLexicographically(Unifier::Variable* var,PrefixSym* term)
{
  CALL("compareLexicographically(Unifier::Variable* var,PrefixSym* term)");
  ASSERT(term->Head().IsFunctor());
  return Incomparable; // temporary
}; // Comparison StandardKBOrdering::CompiledConstraintForUnifiers::compareLexicographically(Unifier::Variable* var,PrefixSym* term)

inline
Comparison StandardKBOrdering::CompiledConstraintForUnifiers::compareLexicographically(Unifier::Variable* var1,Unifier::Variable* var2)
{
  CALL("compareLexicographically(Unifier::Variable* var1,Unifier::Variable* var2)");
  
  return Incomparable; // temporary

}; // Comparison StandardKBOrdering::CompiledConstraintForUnifiers::compareLexicographically(Unifier::Variable* var1,Unifier::Variable* var2)


inline
FunctionComparison 
StandardKBOrdering::CompiledConstraintForUnifiers::interpretWeightComparisonCode(WeightComparisonCommand* subroutine)
{
  CALL("interpretWeightComparisonCode(WeightComparisonCommand* subroutine)");
  const WeightComparisonCommand* command = subroutine;

 next_command:

  //DF; command->outputShort(cout << "DO ") << "\n";

  switch (command->tag())
    {
      
    case CompiledConstraintForUnifiers::WeightComparisonCommand::RetFailureToCompare: return BK::FailureToCompare;
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
	if min|X@| > 1 return BK::FailureToCompare;
	if ground(X@) return BK::AlwaysEqual;
	return BK::FailureToCompare;
      */
      {
	bool freeVar;
	const void* instance = (static_cast<const Unifier::Variable*>(command->nativeVar()))->Unref(freeVar);
	if (freeVar) return BK::FailureToCompare; // nonground
	TermWeightType w = TermWeighting::current()->computeWeightIfGroundAndLessOrEqual(static_cast<const PrefixSym*>(instance),static_cast<TermWeightType>(1));
	if (w <= 0) return BK::FailureToCompare;
	return BK::AlwaysEqual;
      };

    case CompiledConstraintForUnifiers::WeightComparisonCommand::IfXGr1RetFail_IfNotGrndRetFail:
      /*
	if min|X@| > 1 return BK::FailureToCompare;
	if !ground(X@) return BK::FailureToCompare;
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
	if min|X@| > 1 return BK::AlwaysGreater;
	if ground(X@) return BK::AlwaysEqual;
	return BK::CanBeGreaterOrEqual;
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
	if min|X@| > 1 return BK::AlwaysGreater;
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
	if min|X@| > 1 return BK::AlwaysGreater;
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
	if min|X@| > 1 return BK::AlwaysGreater;
	GRND := GRND && ground(X@);
	if GRND return BK::AlwaysEqual;
	return BK::CanBeGreaterOrEqual;
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
	if !ground(X@) return BK::FailureToCompare;
	if min|X@| > W return BK::FailureToCompare;
	if min|X@| < W return BK::AlwaysGreater;
	return BK::AlwaysEqual;
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
	if !ground(X@) return BK::FailureToCompare;
	if C*min|X@| > W return BK::FailureToCompare;
	if C*min|X@| < W return BK::AlwaysGreater;
	return BK::AlwaysEqual;
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
	if !ground(X@) return BK::FailureToCompare;
	ACC := W - min|X@|;
	if ACC < 0 return BK::FailureToCompare;      
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
	if !ground(X@) return BK::FailureToCompare;
	ACC := W - C*min|X@|;
	if ACC < 0 return BK::FailureToCompare;      
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
	if !ground(X@) return BK::FailureToCompare;
	ACC := ACC - min|X@| + 1;
	if ACC < 0 return BK::FailureToCompare;
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
	if !ground(X@) return BK::FailureToCompare;
	ACC := ACC - C*min|X@| + C;
	if ACC < 0 return BK::FailureToCompare; 
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
	if !ground(X@) return BK::FailureToCompare;
	ACC := ACC - min|X@| + 1;
	if ACC < 0 return BK::FailureToCompare; 
	if ACC = 0 return BK::AlwaysEqual;
	return BK::AlwaysGreater;
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
	if !ground(X@) return BK::FailureToCompare;
	ACC := ACC - C*min|X@| + C;
	if ACC < 0 return BK::FailureToCompare; 
	if ACC = 0 return BK::AlwaysEqual;
	return BK::AlwaysGreater;
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
	if min|X@| > W return BK::AlwaysGreater;
	if min|X@| < W return BK::FailureToCompare;
	if ground(X@) return BK::AlwaysEqual;
	return BK::CanBeGreaterOrEqual;
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
	if C*min|X@| > W return BK::AlwaysGreater;
	if C*min|X@| < W return BK::FailureToCompare;
	if ground(X@) return BK::AlwaysEqual;
	return BK::CanBeGreaterOrEqual;
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
	if ACC < 0 return BK::AlwaysGreater;
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
	if ACC <= 0 return BK::AlwaysGreater;
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
	if ACC < 0 return BK::AlwaysGreater;
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
	if ACC < 0 return BK::AlwaysGreater;
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
	if min|X@| > ACC + 1 return BK::AlwaysGreater;
	if min|X@| < ACC + 1 return BK::FailureToCompare;      
	GRND := GRND && ground(X@);
	if GRND return BK::AlwaysEqual;
	return BK::CanBeGreaterOrEqual;
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
	if C*min|X@| > ACC + C return BK::AlwaysGreater;
	if C*min|X@| < ACC + C return BK::FailureToCompare;      
	GRND := GRND && ground(X@);
	if GRND return BK::AlwaysEqual;
	return BK::CanBeGreaterOrEqual;
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
	if constant(POSINST) && POSINST < 0 return BK::FailureToCompare; 
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
	if constant(POSINST) && POSINST < 0 return BK::FailureToCompare; 
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
	if constant(POSINST) && POSINST < 0 return BK::FailureToCompare; 
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
	if constant(POSINST) && POSINST < 0 return BK::FailureToCompare; 
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
	if compareForGreaterOrEqual(POSINST,NEGINST) == BK::FailureToCompare return BK::FailureToCompare;
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
	if compareForGreaterOrEqual(POSINST,NEGINST) == BK::FailureToCompare return BK::FailureToCompare;
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
	if compareForGreaterOrEqual(POSINST,NEGINST) == BK::FailureToCompare return BK::FailureToCompare;
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
	if compareForGreaterOrEqual(POSINST,NEGINST) == BK::FailureToCompare return BK::FailureToCompare;
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
	if !ground(X@) return BK::FailureToCompare;
	if min|X@| - 1 > POSINST return BK::FailureToCompare;
	if min|X@| - 1 < POSINST return BK::AlwaysGreater;
	return BK::AlwaysEqual;
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
	if !ground(X@) return BK::FailureToCompare;
	if C*min|X@| - C > POSINST return BK::FailureToCompare;
	if C*min|X@| - C < POSINST return BK::AlwaysGreater;
	return BK::AlwaysEqual;
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
	if !ground(X@) return BK::FailureToCompare;
	ACC := POSINST + 1 - min|X@|;      
	if ACC < 0 return BK::FailureToCompare; 
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
	if !ground(X@) return BK::FailureToCompare;
	ACC := POSINST + C - C*min|X@|;      
	if ACC < 0 return BK::FailureToCompare; 
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
	if constant(NEGINST) && NEGINST < 0 return BK::AlwaysGreater;
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
	if constant(NEGINST) && NEGINST < 0 return BK::AlwaysGreater;
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
	if constant(NEGINST) && NEGINST < 0 return BK::AlwaysGreater;
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
	if constant(NEGINST) && NEGINST < 0 return BK::AlwaysGreater;
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
	if compareForGreaterOrEqual(POSINST,NEGINST) == BK::AlwaysGreater return BK::AlwaysGreater;
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
	if compareForGreaterOrEqual(POSINST,NEGINST) == BK::AlwaysGreater return BK::AlwaysGreater;
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
	if compareForGreaterOrEqual(POSINST,NEGINST) == BK::AlwaysGreater return BK::AlwaysGreater;
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
	if compareForGreaterOrEqual(POSINST,NEGINST) == BK::AlwaysGreater return BK::AlwaysGreater;
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
	if ACC < 0 return BK::AlwaysGreater;
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
	if ACC < 0 return BK::AlwaysGreater;
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
}; // FunctionComparison StandardKBOrdering::CompiledConstraintForUnifiers::interpretWeightComparisonCode()



//===================================================================
#undef DEBUG_NAMESPACE
#ifdef DEBUG_STANDARD_KB_ORDERING
#define DEBUG_NAMESPACE "StandardKBOrdering::CompiledConstraintForUnifiers::Command"
#endif
#include "debugMacros.hpp"
//===================================================================



inline
bool 
StandardKBOrdering::CompiledConstraintForUnifiers::Command::isTerminal() const
{
  CALL("isTerminal() const");
  switch (tag())
    {
    case RetSuccess:
    case RetFailure:
      return true;
    default: return false;
    };
}; // bool StandardKBOrdering::CompiledConstraintForUnifiers::Command::isTerminal() const

inline
bool 
StandardKBOrdering::CompiledConstraintForUnifiers::Command::requiresLabel() const
{
  CALL("requiresLabel() const");
  return _tag == IfWCFailRetFail_IfWCAlwGrRetSucc_IfWCCanBeGrOrEqGotoL;
}; // bool StandardKBOrdering::CompiledConstraintForUnifiers::Command::requiresLabel() const


#ifndef NO_DEBUG_VIS

ostream& StandardKBOrdering::CompiledConstraintForUnifiers::Command::output(ostream& str,const Command* base) const
{


  str << "[" << (this - base) << "] ";
  switch (tag())
    {
    case RetSuccess: return str << "RetSuccess;\n";
    case RetFailure: return str << "RetFailure;\n";      
    case CompareST_IfCmpGrRetSucc_IfCmpNotEqRetFail:
      str << "CompareST_IfCmpGrRetSucc_IfCmpNotEqRetFail\n";
      Prefix::outputTerm(str << " S = ",termS()) << "\n";                 
      Prefix::outputTerm(str << " T = ",termT()) << "\n";                 
      str << " {\n"
	  << "   cmp = compare(S@,T@);\n"
          << "   if (cmp == Greater) return Success;\n"
	  << "   if (cmp != AlwaysEqual) return Failure;\n"
	  << " }\n";
      return str;     
    case CompareSY_IfCmpGrRetSucc_IfCmpNotEqRetFail:
      str << "CompareSY_IfCmpGrRetSucc_IfCmpNotEqRetFail\n";
      Prefix::outputTerm(str << " S = ",termS()) << "\n";    
      str << " Y = " << "X" << _varYNum << "\n";         
      str << " {\n"
	  << "   cmp = compare(S@,Y@);\n"
          << "   if (cmp == Greater) return Success;\n"
	  << "   if (cmp != AlwaysEqual) return Failure;\n"
	  << " }\n";
      return str;     

    case CompareXT_IfCmpGrRetSucc_IfCmpNotEqRetFail:
      str << "CompareXT_IfCmpGrRetSucc_IfCmpNotEqRetFail\n";
      str << " X = " << "X" << _varXNum << "\n";        
      Prefix::outputTerm(str << " T = ",termT()) << "\n";                 
      str << " {\n"
	  << "   cmp = compare(X@,T@);\n"
          << "   if (cmp == Greater) return Success;\n"
	  << "   if (cmp != AlwaysEqual) return Failure;\n"
	  << " }\n";
      return str;     

    case CompareXY_IfCmpGrRetSucc_IfCmpNotEqRetFail:
      str << "CompareXY_IfCmpGrRetSucc_IfCmpNotEqRetFail\n";
      str << " X = " << "X" << _varXNum << "\n";  
      str << " Y = " << "X" << _varYNum << "\n";        
      str << " {\n"
	  << "   cmp = compare(X@,Y@);\n"
          << "   if (cmp == Greater) return Success;\n"
	  << "   if (cmp != AlwaysEqual) return Failure;\n"
	  << " }\n";
      return str;     


    case IfWCFailRetFail_IfWCAlwGrRetSucc_IfWCCanBeGrOrEqGotoL:
      str << "IfWCFailRetFail_IfWCAlwGrRetSucc_IfWCCanBeGrOrEqGotoL\n";
      str << " L = [" << (labelL() - base) << "]\n";
      str << " {\n"
	  << "   if WC(@) = FailureToCompare return Failure;\n"
	  << "   if WC(@) = AlwaysGreater return Success;\n"
          << "   ASSERT(WC(@) = AlwaysEqual || WC(@) = CanBeGreaterOrEqual);\n"
	  << "   if WC(@) = CanBeGreaterOrEqual goto L;\n"
	  << " }\n";
      str << "       |== WEIGHT COMPARISON SUBROUTINE: ========\n";
      WeightPolynomial::SpecialisedComparison::Command::outputCode(str,"       |  ",weightComparisonSubroutine());
      str << "       |== END OF WEIGHT COMPARISON SUBROUTINE ==\n";
      return str;

    case IfWCFailRetFail:
      str << "IfWCFailRetFail\n";
      str << " {\n"
	  << "   if WC(@) = FailureToCompare return Failure;\n"
	  << "   ASSERT(WC(@) = AlwaysEqual);\n"
	  << " }\n";
      return str;

    default:
      return str << "????????????;\n";
    };
  return str;

}; // ostream& StandardKBOrdering::CompiledConstraintForUnifiers::Command::output(ostream& str) const
#endif






//===================================================================
#undef DEBUG_NAMESPACE
#ifdef DEBUG_STANDARD_KB_ORDERING
#define DEBUG_NAMESPACE "StandardKBOrdering::CompiledConstraintForInstanceRetrieval"
#endif
#include "debugMacros.hpp"
//===================================================================

StandardKBOrdering::CompiledConstraintForInstanceRetrieval::~CompiledConstraintForInstanceRetrieval()
{
};


void 
StandardKBOrdering::CompiledConstraintForInstanceRetrieval::loadGreater(const TERM* lhs,const TERM* rhs)
{
  CALL("loadGreater(const TERM* lhs,const TERM* rhs)");

  //DF; cout << "LOAD " << lhs << " > " << rhs << "\n";

  INTERCEPT_BACKWARD_CONSTR_CHECK_BEGIN1;
  ASSERT(lhs->isComplex());
  ASSERT(rhs->isComplex());
  _targetRelation = Greater;
  _lhs.MakeReference(const_cast<TERM*>(lhs));
  _rhs.MakeReference(const_cast<TERM*>(rhs));

  _lhsWeight.reset();
  _rhsWeight.reset();

  TermWeighting::current()->collectWeight(_lhsWeight,_lhs);
  TermWeighting::current()->collectWeight(_rhsWeight,_rhs);
  _topLevelWeightComparison.compare(_lhsWeight,_rhsWeight);
  switch (_topLevelWeightComparison.category())
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
}; // void StandardKBOrdering::CompiledConstraintForInstanceRetrieval::loadGreater(const TERM* lhs,const TERM* rhs)


bool StandardKBOrdering::CompiledConstraintForInstanceRetrieval::lhsVarInstMayOrder()
{
  CALL("lhsVarInstMayOrder()");
  return !_rhsWeight.containsVariableWhichIsNotIn(_lhsWeight);
}; // bool StandardKBOrdering::CompiledConstraintForInstanceRetrieval::lhsVarInstMayOrder()

 
bool StandardKBOrdering::CompiledConstraintForInstanceRetrieval::rhsVarInstMayOrder()
{
  CALL("rhsVarInstMayOrder()");
  return !_lhsWeight.containsVariableWhichIsNotIn(_rhsWeight);
}; // bool StandardKBOrdering::CompiledConstraintForInstanceRetrieval::rhsVarInstMayOrder()

void StandardKBOrdering::CompiledConstraintForInstanceRetrieval::compile(const InstRet::Substitution* subst)
{
  CALL("compile(const InstRet::Substitution* subst)");
  INTERCEPT_BACKWARD_CONSTR_CHECK_COMPILATION_CALL;
  INTERCEPT_BACKWARD_CONSTR_CHECK_BEGIN1;

  ASSERT(_targetRelation == Greater);

  //DF; cout << "COMPILE " << _lhs << " > " << _rhs << "\n";

  //DF; cout << "STRENGTH = " << currentSpecialisationStrength() << "\n";

  ++(_ordering->_numberOfCompiledConstraintsForInstanceRetrieval);

  _subst = subst;
  _assumedUnifier.reset();
  DOP(const LocalUnification::UnifierState* debugUnifState = _assumedUnifier.getUnifierState());

  Command* mainCode;
  WeightComparisonCommand* weightComparisonCode;
  bool complete;
  ulong specialisationStrength;

 try_to_compile:
  mainCode = _mainCode.memory();
  weightComparisonCode = _weightComparisonCode.memory();
  specialisationStrength = currentSpecialisationStrength();

  if (!compileGreaterOrEqual(_lhs,
			     _rhs,
			     0,
			     mainCode,
			     _mainCode.memory() + _mainCode.size() - 1,
			     weightComparisonCode,
			     _weightComparisonCode.memory() + _weightComparisonCode.size(),
			     complete,
			     specialisationStrength))  
  {
    DOP(_mainCode.unfreeze());
    DOP(_weightComparisonCode.unfreeze());
    _mainCode.expand();
    _weightComparisonCode.expand();
    DOP(_mainCode.freeze());
    DOP(_weightComparisonCode.freeze());
    goto try_to_compile;    
  };
	  
	  
  if (!complete)  mainCode->setTag(Command::RetFailure);
  ASSERT(_assumedUnifier.getUnifierState() == debugUnifState);
  INTERCEPT_BACKWARD_CONSTR_CHECK_END1;
}; // void StandardKBOrdering::CompiledConstraintForInstanceRetrieval::compile(const InstRet::Substitution* subst)


bool 
StandardKBOrdering::CompiledConstraintForInstanceRetrieval::compileGreaterOrEqual(const TERM& lhs,
										  const TERM& rhs,
										  WeightPolynomial::SpecialisedComparison* assumedWeightRelations,
										  Command*& mainCode,
										  const Command* endOfMainCode,
										  WeightComparisonCommand*& weightComparisonCode,
										  const WeightComparisonCommand* endOfWeightComparisonCode,
										  bool& complete,
										  ulong& specialisationStrength)
{
  CALL("compileGreaterOrEqual(..)");

  //DF; cout << "--COMPILE " << lhs << " >= " << rhs  << "\n";

  ASSERT(mainCode <= endOfMainCode);
  ASSERT(weightComparisonCode <= endOfWeightComparisonCode);

  
  if (mainCode == endOfMainCode) return false;
 
  WeightComparisonCommand* c;

  bool result = true;
  complete = false;

  Command* lastGoto;
  bool lexComplete;
  ulong specialisationStrength1;

  if (!specialisationStrength)
    {
      ASSERT(mainCode < endOfMainCode);

      if (lhs.isVariable())
	{
	  if (rhs.isVariable())
	    {
	      // X vs Y
	      if (lhs != rhs)
		{
		  mainCode->setTag(Command::CompareXY_IfCmpGrRetSucc_IfCmpNotEqRetFail);
		  mainCode->setVarX(_subst->nativeVar(lhs.var()));
		  mainCode->setVarY(_subst->nativeVar(rhs.var()));  
		  ++mainCode; 
		};
	    }
	  else // X vs T
	    {
	      mainCode->setTag(Command::CompareXT_IfCmpGrRetSucc_IfCmpNotEqRetFail);
	      mainCode->setVarX(_subst->nativeVar(lhs.var()));
	      mainCode->setTermT(rhs);
	      ++mainCode; 
	    };
	}
      else
	if (rhs.isVariable())
	  {
	    // S vs Y
	    mainCode->setTag(Command::CompareSY_IfCmpGrRetSucc_IfCmpNotEqRetFail);
	    mainCode->setTermS(lhs);
	    mainCode->setVarY(_subst->nativeVar(rhs.var()));
	    ++mainCode; 
	  }
	else // S vs T
	  {
	    mainCode->setTag(Command::CompareST_IfCmpGrRetSucc_IfCmpNotEqRetFail);
	    mainCode->setTermS(lhs);
	    mainCode->setTermT(rhs);
	    ++mainCode; 
	  };	  
      return true;
    }; // if (!specialisationStrength) ..

  --specialisationStrength;


  WeightPolynomial::SpecialisedComparison* weightComparison = 
    _ordering->reserveSpecialisedWeightComparison();
  WeightPolynomial::SpecialisedComparison* simplifiedWeightComparison = 
    _ordering->reserveSpecialisedWeightComparison();
  WeightPolynomial::SpecialisedComparison* spareWeightComparison1 = 
    _ordering->reserveSpecialisedWeightComparison();
  WeightPolynomial::SpecialisedComparison* spareWeightComparison2 = 
    _ordering->reserveSpecialisedWeightComparison();
  
  WeightComparisonCommand* initWCCodePosition;

  _weight1.reset();
  _weight2.reset();

  TermWeighting::current()->collectWeight(_weight1,lhs,&_assumedUnifier);
  TermWeighting::current()->collectWeight(_weight2,rhs,&_assumedUnifier);
  
  weightComparison->compare(_weight1,_weight2);

  //DF; cout << "----WC = " << *weightComparison << "\n";


  switch (weightComparison->category())
    {
    case BK::FailureToCompare:
      // same as below
    case BK::AlwaysLess:
      ASSERT(mainCode < endOfMainCode);
      mainCode->setTag(Command::RetFailure);
      complete = true;
      ++mainCode;    
      goto finish;

    case BK::AlwaysEqual:
      result = 
	compileGreaterOrEqualLex(lhs,
				 rhs,
				 assumedWeightRelations,
				 mainCode,
				 endOfMainCode,
				 weightComparisonCode,
				 endOfWeightComparisonCode,
				 complete,
				 specialisationStrength);  	
      goto finish;

    case BK::AlwaysGreater:
      ASSERT(mainCode < endOfMainCode);
      mainCode->setTag(Command::RetSuccess);
      complete = true;
      ++mainCode;  
      goto finish;

    case BK::CanBeLessOrEqual: 
      break; 

    case BK::CanBeGreaterOrEqual: 
      // same as below
    case BK::VolatileButNearlyAlwaysLess: 
      // same as below
    case BK::VolatileButNearlyAlwaysGreater: 
      // same as below
    case BK::AbsolutelyVolatile: 

      // try to show that it is AlwaysGreater under the assumptions

      if (weightComparison->isAlwaysGreaterUnderAssumptions(assumedWeightRelations,
							    simplifiedWeightComparison, 
							    spareWeightComparison1,
							    spareWeightComparison2))
	{	  

	  //DF; cout << "----WC --> AlwaysGreater under the assumptions\n";
	  ASSERT(mainCode < endOfMainCode);
	  mainCode->setTag(Command::RetSuccess);
	  complete = true;
	  ++mainCode;  
	  goto finish;
	};
      break;
							
    }; // switch (weightComparison->category())


  switch (weightComparison->category())
    {	      
    case BK::CanBeLessOrEqual: 
      // same as below 
    case BK::VolatileButNearlyAlwaysLess: 
      // same as below
    case BK::VolatileButNearlyAlwaysGreater: 
      // same as below
    case BK::AbsolutelyVolatile:
      // try to show that it is AlwaysLess under the assumptions
      if (weightComparison->isAlwaysLessUnderAssumptions(assumedWeightRelations,
							 simplifiedWeightComparison, 
							 spareWeightComparison1,
							 spareWeightComparison2))
	{	  

	  //DF; cout << "----WC --> AlwaysLess under the assumptions\n";
	  ASSERT(mainCode < endOfMainCode);
	  mainCode->setTag(Command::RetFailure);
	  complete = true;
	  ++mainCode;  
	  goto finish;
	};
      break;

    default: 
      break;
    }; // switch (weightComparison->category())

  // simplify weightComparison here using assumed equalities from assumedWeightRelations
  
  
  if (weightComparison->simplifyByEqualities(assumedWeightRelations,
					     simplifiedWeightComparison,
					     spareWeightComparison1,
					     spareWeightComparison2))
    {
      WeightPolynomial::SpecialisedComparison* tmp = weightComparison;
      weightComparison = simplifiedWeightComparison;
      simplifiedWeightComparison = tmp;
    };
  


  //DF; cout << "----SIMPLIFIED WC = " << (*weightComparison) << "\n";
  
  switch (weightComparison->category())
    {
    case BK::FailureToCompare: 
      // same as below

    case BK::AlwaysLess:
      ASSERT(mainCode < endOfMainCode);
      mainCode->setTag(Command::RetFailure);
      complete = true;
      ++mainCode;    
      goto finish;

    case BK::AlwaysEqual:
      COP("COP200");
      result = 
	compileGreaterOrEqualLex(lhs,
				 rhs,
				 assumedWeightRelations,
				 mainCode,
				 endOfMainCode,
				 weightComparisonCode,
				 endOfWeightComparisonCode,
				 complete,
				 specialisationStrength);  	
      goto finish;

    case BK::AlwaysGreater:
      ASSERT(mainCode < endOfMainCode);
      mainCode->setTag(Command::RetSuccess);
      complete = true;
      ++mainCode;  
      goto finish;

    case BK::CanBeLessOrEqual: 
      // same as below
    case BK::CanBeGreaterOrEqual: 
      // same as below
    case BK::VolatileButNearlyAlwaysLess: 
      // same as below
    case BK::VolatileButNearlyAlwaysGreater: 
      // same as below
    case BK::AbsolutelyVolatile: 
      goto compile_weight_comparison;
    };

 compile_weight_comparison:

  initWCCodePosition = weightComparisonCode;
  if (weightComparisonCode >= endOfWeightComparisonCode - 1)
    {
      result = false;
      goto finish;
    };  

  ASSERT(weightComparisonCode <= endOfWeightComparisonCode);

  if (!weightComparison->compileGreaterOrEqual(weightComparisonCode,
					       endOfWeightComparisonCode - weightComparisonCode))
    {
      result = false;
      goto finish;
    };

  ASSERT(weightComparisonCode <= endOfWeightComparisonCode);
  
  // naturalise the variables
  for (c = initWCCodePosition;
       c < weightComparisonCode;
       ++c)
    if (c->requiresVar())
      {
	c->setNativeVar(static_cast<const void*>(_subst->nativeVar(c->var())));
      };

  ASSERT(mainCode < endOfMainCode);
  if (weightComparison->category() == BK::CanBeLessOrEqual)
    {
      // in this case we know that the weight comparison 
      // can only produce AlwaysEqual or FailureToCompare
      mainCode->setTag(Command::IfWCFailRetFail);
    }
  else
    {
      mainCode->setTag(Command::IfWCFailRetFail_IfWCAlwGrRetSucc_IfWCCanBeGrOrEqGotoL);
    };
  mainCode->setWeightComparisonSubroutine(initWCCodePosition);
  lastGoto = mainCode;
  ++mainCode;  

  // assume weight equality
  
  weightComparison->assumeRelation(BK::AlwaysEqual);
  weightComparison->setNext(assumedWeightRelations);

  if (weightComparison->category() == BK::CanBeLessOrEqual)
    {
      specialisationStrength1 = specialisationStrength;
      specialisationStrength = 0UL; 
    }
  else
    {
      specialisationStrength1 = specialisationStrength/2;
      specialisationStrength -= specialisationStrength1;
    };


  result = 
    compileGreaterOrEqualLex(lhs,
			     rhs,
			     weightComparison,
			     mainCode,
			     endOfMainCode,
			     weightComparisonCode,
			     endOfWeightComparisonCode,
			     lexComplete,
			     specialisationStrength1);

  specialisationStrength += specialisationStrength1;

  ASSERT(weightComparisonCode <= endOfWeightComparisonCode);
  

  if (!result) goto finish;
  
  if (weightComparison->category() != BK::CanBeLessOrEqual)
    {
      ASSERT(lastGoto->requiresLabel());
      lastGoto->setLabelL(mainCode);

      // assume weight inequality
      weightComparison->assumeRelation(BK::CanBeGreaterOrEqual);
      weightComparison->setNext(assumedWeightRelations);


      result = 
	compileGreaterOrEqualLex(lhs,
				 rhs,
				 weightComparison,
				 mainCode,
				 endOfMainCode,
				 weightComparisonCode,
				 endOfWeightComparisonCode,
				 lexComplete,
				 specialisationStrength); 
    };

 finish:
  ASSERT(weightComparisonCode <= endOfWeightComparisonCode);
  _ordering->releaseSpecialisedWeightComparison(weightComparison);
  _ordering->releaseSpecialisedWeightComparison(simplifiedWeightComparison);
  _ordering->releaseSpecialisedWeightComparison(spareWeightComparison1);
  _ordering->releaseSpecialisedWeightComparison(spareWeightComparison2);
  return result;
}; // bool StandardKBOrdering::CompiledConstraintForInstanceRetrieval::compileGreaterOrEqual(..)


bool 
StandardKBOrdering::CompiledConstraintForInstanceRetrieval::compileGreaterOrEqualLex(const TERM& lhs,
										     const TERM& rhs,
										     WeightPolynomial::SpecialisedComparison* assumedWeightRelations,
										     Command*& mainCode,
										     const Command* endOfMainCode,
										     WeightComparisonCommand*& weightComparisonCode,
										     const WeightComparisonCommand* endOfWeightComparisonCode,
										     bool& complete,
										     ulong& specialisationStrength)
{
  CALL("compileGreaterOrEqualLex(..)");
  ASSERT(mainCode <= endOfMainCode);
  ASSERT(weightComparisonCode <= endOfWeightComparisonCode);

 
  LocalUnification::TermTraversal lhsInst;
  LocalUnification::TermTraversal rhsInst;
  complete = false;

  lhsInst.reset(lhs,&_assumedUnifier);
  rhsInst.reset(rhs,&_assumedUnifier);
  if (lhsInst.term().isVariable())
    {
      if (rhsInst.term().isVariable())
	{
	  // both are variables
	  if (lhsInst.term() == rhsInst.term())
	    {
	      // no code is written
	    }
	  else // different variables
	    {
	      if (mainCode == endOfMainCode) return false;
	      mainCode->setTag(Command::CompareLexXY_IfCmpGrRetSucc_IfCmpNotEqRetFail);
	      mainCode->setVarX(_subst->nativeVar(lhsInst.term().var()));
	      mainCode->setVarY(_subst->nativeVar(rhsInst.term().var()));
#ifndef NO_DEBUG
	      mainCode->setVarXNum(lhsInst.term().var());
	      mainCode->setVarYNum(rhsInst.term().var());
#endif
	      ++mainCode;		     
	    };

	}
      else // lhs is a variable, rhs is complex
	{	  
	  if (mainCode == endOfMainCode) return false;
	  mainCode->setTag(Command::CompareLexXT_IfCmpGrRetSucc_IfCmpNotEqRetFail);
	  mainCode->setVarX(_subst->nativeVar(lhsInst.term().var()));
#ifndef NO_DEBUG
	  mainCode->setVarXNum(lhsInst.term().var());
#endif
	  mainCode->setTermT(rhsInst.term());
	  ++mainCode;		      
	};
    }
  else // lhs is complex
    {
      if (rhsInst.term().isVariable())
	{
	  // lhs is complex, rhs is a variable
	  if (mainCode == endOfMainCode) return false;
	  mainCode->setTag(Command::CompareLexSY_IfCmpGrRetSucc_IfCmpNotEqRetFail);
	  mainCode->setTermS(lhsInst.term());
	  mainCode->setVarY(_subst->nativeVar(rhsInst.term().var()));
#ifndef NO_DEBUG
	  mainCode->setVarYNum(rhsInst.term().var());
#endif
	  ++mainCode;	 	       
	}
      else // both are complex
	{
	  if (lhsInst.symbol() == rhsInst.symbol())
	    { 
	      // same top symbols
	      if (lhsInst.symbol().arity())
		{
		  const LocalUnification::UnifierState* unifierState = 
		    _assumedUnifier.getUnifierState();
		  lhsInst.next();
		  rhsInst.next();

		  COP("COP100");
		  do
		    {
		      if (!compileGreaterOrEqual(lhsInst.term(),
						 rhsInst.term(),
						 assumedWeightRelations,
						 mainCode,
						 endOfMainCode,
						 weightComparisonCode,
						 endOfWeightComparisonCode,
						 complete,
						 specialisationStrength)) 

			{
			  _assumedUnifier.rollUnifierBackTo(unifierState);
			  return false;
			};
		      
		      COP("AFTER compileGreaterOrEqual(..)");
		      ASSERT(weightComparisonCode <= endOfWeightComparisonCode);

		      if (complete)
			{
			  _assumedUnifier.rollUnifierBackTo(unifierState);
			  return true;
			};

		      if (!_assumedUnifier.unify(lhsInst.term(),rhsInst.term()))
			{
			  if (mainCode == endOfMainCode) 
			    {
			      _assumedUnifier.rollUnifierBackTo(unifierState);
			      return false;
			    };
			  mainCode->setTag(Command::RetFailure);
			  complete = true;
			  ++mainCode;    
			  _assumedUnifier.rollUnifierBackTo(unifierState);
			  return true;
			};

		      lhsInst.after();
		      rhsInst.after();
		    }
		  while (lhsInst);
		  
		  _assumedUnifier.rollUnifierBackTo(unifierState);
		};
	    }
	  else // different top symbols
	    {
	      if (mainCode == endOfMainCode) return false;
	      if (lhsInst.symbol().hasGreaterPrecedenceThan(rhsInst.symbol()))
		{
		  mainCode->setTag(Command::RetSuccess);
		}
	      else
		{
		  mainCode->setTag(Command::RetFailure);
		};
	      complete = true;
	      ++mainCode;	 
	    };
	};
    };

  return true;
}; // bool StandardKBOrdering::CompiledConstraintForInstanceRetrieval::compileGreaterOrEqualLex(


bool StandardKBOrdering::CompiledConstraintForInstanceRetrieval::holds()
{
  CALL("holds()"); 

  ++(_ordering->_numberOfConstraintChecksForInstanceRetrieval);

#ifdef MEASURE_BACKWARD_CONSTR_CHECK
  INTERCEPT_BACKWARD_CONSTR_CHECK_BEGIN2;
  // Straightforward check:
  SimplificationOrdering::current()->greater(_lhs.First(),_rhs.First(),_subst);
  INTERCEPT_BACKWARD_CONSTR_CHECK_END2;
#endif

  INTERCEPT_BACKWARD_CONSTR_CHECK_NEW_CHECK;
  INTERCEPT_BACKWARD_CONSTR_CHECK_BEGIN1;

  //DF; output(cout << "\n\n\nHOLDS?\n") << "\n\n\n";   
  //DF; cout << "SUBST " << _subst << "\n"; 

  Command* command = _mainCode.memory();
  FunctionComparison weightComp;
  Comparison cmp;
  TERM instanceForX;
  TERM instanceForY;

 next_command:

  switch (command->tag())
    {
    case Command::RetSuccess:
      goto return_true; 
    case Command::RetFailure:
      goto return_false;

    case Command::CompareST_IfCmpGrRetSucc_IfCmpNotEqRetFail:
      /*
	cmp = compare(S@,T@);
	if (cmp == Greater) return Success;	  
	if (cmp != Equal) return Failure;
      */
      cmp =
	_ordering->compare(command->termS(),
			   command->termT(),
			   _subst);
      if (cmp == BK::Greater) goto return_true;
      if (cmp != BK::Equal) goto return_false;      
      ++command;
      goto next_command;

      
    case Command::CompareSY_IfCmpGrRetSucc_IfCmpNotEqRetFail:
      /*
	cmp = compare(S@,Y@);
	if (cmp == Greater) return Success;	  
	if (cmp != Equal) return Failure;
      */
      cmp =
	_ordering->compare(command->termS().First(),
			   _subst,
			   *(command->varY()));
      if (cmp == BK::Greater) goto return_true;
      if (cmp != BK::Equal) goto return_false;      
      ++command;
      goto next_command;


    case Command::CompareXT_IfCmpGrRetSucc_IfCmpNotEqRetFail:
      /*
	cmp = compare(X@,T@);
	if (cmp == Greater) return Success;	  
	if (cmp != Equal) return Failure;
      */
      cmp =
	_ordering->compare(command->termT().First(),
			   _subst,
			   *(command->varX()));
      if (cmp == BK::Less) goto return_true;
      if (cmp != BK::Equal) goto return_false;      
      ++command;
      goto next_command;

    case Command::CompareXY_IfCmpGrRetSucc_IfCmpNotEqRetFail:
      /*
	cmp = compare(X@,Y@);
	if (cmp == Greater) return Success;	  
	if (cmp != Equal) return Failure;
      */
      cmp =
	_ordering->compare(*(command->varX()),*(command->varY()));
      if (cmp == BK::Greater) goto return_true;
      if (cmp != BK::Equal) goto return_false;      
      ++command;
      goto next_command;

    case Command::CompareLexXY_IfCmpGrRetSucc_IfCmpNotEqRetFail:
      /*
	cmp = compareLex(X@,Y@);
	if (cmp == Greater) return Success;	  
	if (cmp != Equal) return Failure;
      */
      cmp =
	_ordering->compareLexicographically(*(command->varX()),
					    *(command->varY()));
      
      if (cmp == BK::Greater) goto return_true;
      if (cmp != BK::Equal) goto return_false;      
      ++command;
      goto next_command;

    case Command::CompareLexXT_IfCmpGrRetSucc_IfCmpNotEqRetFail:
      /*
	cmp = compareLex(X@,T@);
	if (cmp == Greater) return Success;	  
	if (cmp != Equal) return Failure;
      */
      cmp = 
	_ordering->compareLexicographically(command->termT().First(),
					    _subst,
					    *(command->varX()));
      if (cmp == BK::Less) goto return_true;
      if (cmp != BK::Equal) goto return_false;      
      ++command;
      goto next_command;

    case Command::CompareLexSY_IfCmpGrRetSucc_IfCmpNotEqRetFail:
      /*
	cmp = compareLex(S@,Y@);
	if (cmp == Greater) return Success;	  
	if (cmp != Equal) return Failure;
      */
      cmp = 
	_ordering->compareLexicographically(command->termS().First(),
					    _subst,
					    *(command->varY()));      
      if (cmp == BK::Greater) goto return_true;
      if (cmp != BK::Equal) goto return_false;      
      ++command;
      goto next_command;
					    

    case Command::IfWCFailRetFail_IfWCAlwGrRetSucc_IfWCCanBeGrOrEqGotoL:      
      /*
	if WC(@) = FailureToCompare return Failure;
	if WC(@) = AlwaysGreater return Success;
	ASSERT(WC(@) = AlwaysEqual || WC(@) = CanBeGreaterOrEqual);
	if WC(@) = CanBeGreaterOrEqual goto L;
      */
      
      weightComp = 
	interpretWeightComparisonCode(command->weightComparisonSubroutine());
      
      //DF; cout << "\n\n\nWEIGHT COMPARISON RETURNS: " << weightComp << "\n\n\n";
      if (weightComp == BK::FailureToCompare) goto return_false;
      if (weightComp == BK::AlwaysGreater) goto return_true;
      ASSERT((weightComp == BK::CanBeGreaterOrEqual) || 
	     (weightComp == BK::AlwaysEqual));
      if (weightComp == BK::CanBeGreaterOrEqual) 
	{
	  command = command->labelL();
	}
      else
	++command;
      goto next_command;

    case Command::IfWCFailRetFail:
      /*
	if WC(@) = FailureToCompare return Failure;
	ASSERT(WC(@) = AlwaysEqual);
      */ 
      weightComp = 
	interpretWeightComparisonCode(command->weightComparisonSubroutine());
      if (weightComp == BK::FailureToCompare) goto return_false;
      ASSERT(weightComp == BK::AlwaysEqual);
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

  //DF; cout << "SUCCESS\n";


  ASSERT((_targetRelation != Greater) || (SimplificationOrdering::current()->greater(_lhs.First(),_rhs.First(),_subst)));

  INTERCEPT_BACKWARD_CONSTR_CHECK_END1;
  return true;

 return_false:

  //DF; cout << "FAILURE\n";

  // v Very strong assumption!  
  ASSERT((_targetRelation != Greater) || (!SimplificationOrdering::current()->greater(_lhs.First(),_rhs.First(),_subst)));

  INTERCEPT_BACKWARD_CONSTR_CHECK_END1;
  return false;
}; // bool StandardKBOrdering::CompiledConstraintForInstanceRetrieval::holds()




#ifndef NO_DEBUG_VIS
ostream& StandardKBOrdering::CompiledConstraintForInstanceRetrieval::output(ostream& str) const
{
  str << "CONSTRAINT: " << _lhs;
  switch (_targetRelation)
    {
    case Greater: str << " > "; break;
    default: str << "???";
    };
  str << _rhs << "\n";
  cout << "TOP LEVEL WEIGHT COMPARISON:\n" << _topLevelWeightComparison << "\n";
  str << "*** CODE: **********************\n";
  const Command* mainCodeCommand = _mainCode.memory();
  ulong gotosPassed = 0UL;
 another_branch:
  while (!mainCodeCommand->isTerminal())
    {
      mainCodeCommand->output(str,_mainCode.memory());
      if (mainCodeCommand->requiresLabel())
	++gotosPassed;
      ++mainCodeCommand;
    };
   mainCodeCommand->output(str,_mainCode.memory());
   if (gotosPassed)
     {
       --gotosPassed;
       ++mainCodeCommand;
       goto another_branch;
     };
  str << "*********** END OF CODE ********\n";
  return str;
};
#endif





inline
FunctionComparison 
StandardKBOrdering::CompiledConstraintForInstanceRetrieval::interpretWeightComparisonCode(WeightComparisonCommand* subroutine)
{
  CALL("interpretWeightComparisonCode(WeightComparisonCommand* subroutine)");
  const WeightComparisonCommand* command = subroutine;


 next_command:

  //DF; command->outputShort(cout << "            WC INSTR: ") << "\n"; 

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
	if min|X@| > 1 return BK::FailureToCompare;
	if ground(X@) return BK::AlwaysEqual;
	return BK::FailureToCompare;
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
	if min|X@| > 1 return BK::FailureToCompare;
	if !ground(X@) return BK::FailureToCompare;
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
	if min|X@| > 1 return BK::AlwaysGreater;
	if ground(X@) return BK::AlwaysEqual;
	return BK::CanBeGreaterOrEqual;
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
	if min|X@| > 1 return BK::AlwaysGreater;
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
	if min|X@| > 1 return BK::AlwaysGreater;
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
	if min|X@| > 1 return BK::AlwaysGreater;
	GRND := GRND && ground(X@);
	if GRND return BK::AlwaysEqual;
	return BK::CanBeGreaterOrEqual;
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
	if !ground(X@) return BK::FailureToCompare;
	if min|X@| > W return BK::FailureToCompare;
	if min|X@| < W return BK::AlwaysGreater;
	return BK::AlwaysEqual;
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
	if !ground(X@) return BK::FailureToCompare;
	if C*min|X@| > W return BK::FailureToCompare;
	if C*min|X@| < W return BK::AlwaysGreater;
	return BK::AlwaysEqual;
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
	if !ground(X@) return BK::FailureToCompare;
	ACC := W - min|X@|;
	if ACC < 0 return BK::FailureToCompare;      
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
	if !ground(X@) return BK::FailureToCompare;
	ACC := W - C*min|X@|;
	if ACC < 0 return BK::FailureToCompare;      
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
	if !ground(X@) return BK::FailureToCompare;
	ACC := ACC - min|X@| + 1;
	if ACC < 0 return BK::FailureToCompare;
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
	if !ground(X@) return BK::FailureToCompare;
	ACC := ACC - C*min|X@| + C;
	if ACC < 0 return BK::FailureToCompare; 
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
	if !ground(X@) return BK::FailureToCompare;
	ACC := ACC - min|X@| + 1;
	if ACC < 0 return BK::FailureToCompare; 
	if ACC = 0 return BK::AlwaysEqual;
	return BK::AlwaysGreater;
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
	if !ground(X@) return BK::FailureToCompare;
	ACC := ACC - C*min|X@| + C;
	if ACC < 0 return BK::FailureToCompare; 
	if ACC = 0 return BK::AlwaysEqual;
	return BK::AlwaysGreater;
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
	if min|X@| > W return BK::AlwaysGreater;
	if min|X@| < W return BK::FailureToCompare;
	if ground(X@) return BK::AlwaysEqual;
	return BK::CanBeGreaterOrEqual;
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
	if C*min|X@| > W return BK::AlwaysGreater;
	if C*min|X@| < W return BK::FailureToCompare;
	if ground(X@) return BK::AlwaysEqual;
	return BK::CanBeGreaterOrEqual;
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
	if ACC < 0 return BK::AlwaysGreater;
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
	if ACC <= 0 return BK::AlwaysGreater;
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
	if ACC < 0 return BK::AlwaysGreater;
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
	if ACC < 0 return BK::AlwaysGreater;
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
	if min|X@| > ACC + 1 return BK::AlwaysGreater;
	if min|X@| < ACC + 1 return BK::FailureToCompare;      
	GRND := GRND && ground(X@);
	if GRND return BK::AlwaysEqual;
	return BK::CanBeGreaterOrEqual;
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
	if C*min|X@| > ACC + C return BK::AlwaysGreater;
	if C*min|X@| < ACC + C return BK::FailureToCompare;      
	GRND := GRND && ground(X@);
	if GRND return BK::AlwaysEqual;
	return BK::CanBeGreaterOrEqual;
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
	if constant(POSINST) && POSINST < 0 return BK::FailureToCompare; 
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
	if constant(POSINST) && POSINST < 0 return BK::FailureToCompare; 
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
	if constant(POSINST) && POSINST < 0 return BK::FailureToCompare; 
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
	if constant(POSINST) && POSINST < 0 return BK::FailureToCompare; 
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
	if compareForGreaterOrEqual(POSINST,NEGINST) == BK::FailureToCompare return BK::FailureToCompare;
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
	if compareForGreaterOrEqual(POSINST,NEGINST) == BK::FailureToCompare return BK::FailureToCompare;
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
	if compareForGreaterOrEqual(POSINST,NEGINST) == BK::FailureToCompare return BK::FailureToCompare;
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
	if compareForGreaterOrEqual(POSINST,NEGINST) == BK::FailureToCompare return BK::FailureToCompare;
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
	if !ground(X@) return BK::FailureToCompare;
	if min|X@| - 1 > POSINST return BK::FailureToCompare;
	if min|X@| - 1 < POSINST return BK::AlwaysGreater;
	return BK::AlwaysEqual;
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
	if !ground(X@) return BK::FailureToCompare;
	if C*min|X@| - C > POSINST return BK::FailureToCompare;
	if C*min|X@| - C < POSINST return BK::AlwaysGreater;
	return BK::AlwaysEqual;
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
	if !ground(X@) return BK::FailureToCompare;
	ACC := POSINST + 1 - min|X@|;      
	if ACC < 0 return BK::FailureToCompare; 
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
	if !ground(X@) return BK::FailureToCompare;
	ACC := POSINST + C - C*min|X@|;      
	if ACC < 0 return BK::FailureToCompare; 
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
	if constant(NEGINST) && NEGINST < 0 return BK::AlwaysGreater;
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
	if constant(NEGINST) && NEGINST < 0 return BK::AlwaysGreater;
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
	if constant(NEGINST) && NEGINST < 0 return BK::AlwaysGreater;
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
	if constant(NEGINST) && NEGINST < 0 return BK::AlwaysGreater;
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
	if compareForGreaterOrEqual(POSINST,NEGINST) == BK::AlwaysGreater return BK::AlwaysGreater;
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
	if compareForGreaterOrEqual(POSINST,NEGINST) == BK::AlwaysGreater return BK::AlwaysGreater;
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
	if compareForGreaterOrEqual(POSINST,NEGINST) == BK::AlwaysGreater return BK::AlwaysGreater;
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
	if compareForGreaterOrEqual(POSINST,NEGINST) == BK::AlwaysGreater return BK::AlwaysGreater;
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
	if ACC < 0 return BK::AlwaysGreater;
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
	if ACC < 0 return BK::AlwaysGreater;
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
}; // FunctionComparison StandardKBOrdering::CompiledConstraintForInstanceRetrieval::interpretWeightComparisonCode(WeightComparisonCommand* subroutine)



//===================================================================
#undef DEBUG_NAMESPACE
#ifdef DEBUG_STANDARD_KB_ORDERING
#define DEBUG_NAMESPACE "StandardKBOrdering::CompiledConstraintForInstanceRetrieval::Command"
#endif
#include "debugMacros.hpp"
//===================================================================

inline
bool 
StandardKBOrdering::CompiledConstraintForInstanceRetrieval::Command::isTerminal() const
{
  CALL("isTerminal() const");
  switch (tag())
    {
    case RetSuccess:
    case RetFailure:
      return true;
    default: return false;
    };
}; // bool StandardKBOrdering::CompiledConstraintForInstanceRetrieval::Command::isTerminal() const


#ifndef NO_DEBUG_VIS

ostream& StandardKBOrdering::CompiledConstraintForInstanceRetrieval::Command::output(ostream& str,const Command* base) const
{
  str << "[" << (this - base) << "] ";
  switch (tag())
    {
    case RetSuccess: return str << "RetSuccess;\n";
    case RetFailure: return str << "RetFailure;\n";      
    case CompareST_IfCmpGrRetSucc_IfCmpNotEqRetFail:
      str << "CompareST_IfCmpGrRetSucc_IfCmpNotEqRetFail\n";
      str << " S = " << termS() << "\n";                 
      str << " T = " << termT() << "\n";                 
      str << " {\n"
	  << "   cmp = compare(S@,T@);\n"
          << "   if (cmp == Greater) return Success;\n"
	  << "   if (cmp != AlwaysEqual) return Failure;\n"
	  << " }\n";
      return str;     
    case CompareSY_IfCmpGrRetSucc_IfCmpNotEqRetFail:
      str << "CompareSY_IfCmpGrRetSucc_IfCmpNotEqRetFail\n";
      str << " S = " << termS() << "\n";    
      str << " Y = " << "X" << _varYNum << "\n";         
      str << " {\n"
	  << "   cmp = compare(S@,Y@);\n"
          << "   if (cmp == Greater) return Success;\n"
	  << "   if (cmp != AlwaysEqual) return Failure;\n"
	  << " }\n";
      return str;     

    case CompareXT_IfCmpGrRetSucc_IfCmpNotEqRetFail:
      str << "CompareXT_IfCmpGrRetSucc_IfCmpNotEqRetFail\n";
      str << " X = " << "X" << _varXNum << "\n";        
      str << " T = " << termT() << "\n";                 
      str << " {\n"
	  << "   cmp = compare(X@,T@);\n"
          << "   if (cmp == Greater) return Success;\n"
	  << "   if (cmp != AlwaysEqual) return Failure;\n"
	  << " }\n";
      return str;     

    case CompareXY_IfCmpGrRetSucc_IfCmpNotEqRetFail:
      str << "CompareXY_IfCmpGrRetSucc_IfCmpNotEqRetFail\n";
      str << " X = " << "X" << _varXNum << "\n";  
      str << " Y = " << "X" << _varYNum << "\n";        
      str << " {\n"
	  << "   cmp = compare(X@,Y@);\n"
          << "   if (cmp == Greater) return Success;\n"
	  << "   if (cmp != AlwaysEqual) return Failure;\n"
	  << " }\n";
      return str;     


    case CompareLexXY_IfCmpGrRetSucc_IfCmpNotEqRetFail:
      str << "CompareLexXY_IfCmpGrRetSucc_IfCmpNotEqRetFail\n";
      str << " X = " << "X" << _varXNum << "\n";   
      str << " Y = " << "X" << _varYNum << "\n";         
      str << " {\n"
	  << "   cmp = compareLex(X@,Y@);\n"
          << "   if (cmp == Greater) return Success;\n"
	  << "   if (cmp != AlwaysEqual) return Failure;\n"
	  << " }\n";
      return str;
    case CompareLexXT_IfCmpGrRetSucc_IfCmpNotEqRetFail:
      str << "CompareLexXT_IfCmpGrRetSucc_IfCmpNotEqRetFail\n";
      str << " X = " << "X" << _varXNum << "\n";  
      str << " T = " << termT() << "\n";           
      str << " {\n"
	  << "   cmp = compareLex(X@,T@);\n"
          << "   if (cmp == Greater) return Success;\n"
	  << "   if (cmp != AlwaysEqual) return Failure;\n"
	  << " }\n";
      return str;
    case CompareLexSY_IfCmpGrRetSucc_IfCmpNotEqRetFail:
      str << "CompareLexSY_IfCmpGrRetSucc_IfCmpNotEqRetFail\n";
      str << " S = " << termS() << "\n";     
      str << " Y = " << "X" << _varYNum << "\n";         
      str << " {\n"
	  << "   cmp = compareLex(S@,Y@);\n"
          << "   if (cmp == Greater) return Success;\n"
	  << "   if (cmp != AlwaysEqual) return Failure;\n"
	  << " }\n";
      return str;

    case IfWCFailRetFail_IfWCAlwGrRetSucc_IfWCCanBeGrOrEqGotoL:
      str << "IfWCFailRetFail_IfWCAlwGrRetSucc_IfWCCanBeGrOrEqGotoL\n";
      str << " L = [" << (labelL() - base) << "]\n";
      str << " {\n"
	  << "   if WC(@) = FailureToCompare return Failure;\n"
	  << "   if WC(@) = AlwaysGreater return Success;\n"
          << "   ASSERT(WC(@) = AlwaysEqual || WC(@) = CanBeGreaterOrEqual);\n"
	  << "   if WC(@) = CanBeGreaterOrEqual goto L;\n"
	  << " }\n";
      str << "       |== WEIGHT COMPARISON SUBROUTINE: ========\n";
      WeightPolynomial::SpecialisedComparison::Command::outputCode(str,"       |  ",weightComparisonSubroutine());
      str << "       |== END OF WEIGHT COMPARISON SUBROUTINE ==\n";
      return str;

    case IfWCFailRetFail:
      str << "IfWCFailRetFail\n";
      str << " {\n"
	  << "   if WC(@) = FailureToCompare return Failure;\n"
	  << "   ASSERT(WC(@) = AlwaysEqual);\n"
	  << " }\n";
      return str;

    default:
      return str << "????????????;\n";
    };
  return str;
}; // ostream& StandardKBOrdering::CompiledConstraintForInstanceRetrieval::Command::output(ostream& str,const Command* base) const



ostream& StandardKBOrdering::CompiledConstraintForInstanceRetrieval::Command::outputShort(ostream& str,const Command* base) const
{
  switch (tag())
    {
    case RetSuccess: return str << "RetSuccess\n";
    case RetFailure: return str << "RetFailure\n";

    case CompareST_IfCmpGrRetSucc_IfCmpNotEqRetFail:
      str << "CompareST_IfCmpGrRetSucc_IfCmpNotEqRetFail\n"; 
      str << " S = " << termS() << "\n";   
      str << " T = " << termT() << "\n";
      return str;
    case CompareSY_IfCmpGrRetSucc_IfCmpNotEqRetFail:
      str << "CompareSY_IfCmpGrRetSucc_IfCmpNotEqRetFail\n"; 
      str << " S = " << termS() << "\n"; 
      str << " Y = " << "X" << _varYNum << "\n"; 
      return str;
    case CompareXT_IfCmpGrRetSucc_IfCmpNotEqRetFail:
      str << "CompareXT_IfCmpGrRetSucc_IfCmpNotEqRetFail\n"; 
      str << " X = " << "X" << _varXNum << "\n";   
      str << " T = " << termT() << "\n";
      return str;
    case CompareXY_IfCmpGrRetSucc_IfCmpNotEqRetFail:
      str << "CompareXY_IfCmpGrRetSucc_IfCmpNotEqRetFail\n"; 
      str << " X = " << "X" << _varXNum << "\n";   
      str << " Y = " << "X" << _varYNum << "\n"; 
      return str;


    case CompareLexXY_IfCmpGrRetSucc_IfCmpNotEqRetFail:
      str << "CompareLexXY_IfCmpGrRetSucc_IfCmpNotEqRetFail\n";
      str << " X = " << "X" << _varXNum << "\n";   
      str << " Y = " << "X" << _varYNum << "\n"; 
      return str;
    case CompareLexXT_IfCmpGrRetSucc_IfCmpNotEqRetFail:
      str << "CompareLexXT_IfCmpGrRetSucc_IfCmpNotEqRetFail\n";
      str << " X = " << "X" << _varXNum << "\n";  
      str << " T = " << termT() << "\n";
      return str;
    case CompareLexSY_IfCmpGrRetSucc_IfCmpNotEqRetFail:
      str << "CompareLexSY_IfCmpGrRetSucc_IfCmpNotEqRetFail\n";
      str << " S = " << termS() << "\n";     
      str << " Y = " << "X" << _varYNum << "\n";
      return str;
    case IfWCFailRetFail_IfWCAlwGrRetSucc_IfWCCanBeGrOrEqGotoL:
      str << "IfWCFailRetFail_IfWCAlwGrRetSucc_IfWCCanBeGrOrEqGotoL\n";
      str << " L = [" << (labelL() - base) << "]\n";
      return str;
    case IfWCFailRetFail:
      str << "IfWCFailRetFail\n";
      return str;
    default: return str << "????????????;\n";
    };
  return str;
}; // ostream& StandardKBOrdering::CompiledConstraintForInstanceRetrieval::Command::outputShort(ostream& str,const Command* base) const



#endif


//=================================================================
