//
// File:         StandardKBOrdering.hpp
// Description:  Standard Knuth-Bendix ordering.  
// Created:      Jun 28, 2002, Alexandre Riazanov, riazanov@cs.man.ac.uk
//=================================================================
#ifndef STANDARD_KB_ORDERING_H
#define STANDARD_KB_ORDERING_H
//===================================================================
#ifndef NO_DEBUG_VIS
#include <iostream>
#endif
#include "jargon.hpp"
#include "Array.hpp"
#include "VampireKernelDebugFlags.hpp"
#include "VampireKernelInterceptMacros.hpp"
#include "SimplificationOrdering.hpp"
#include "Flatterm.hpp"
#include "Term.hpp"
#include "prefix.hpp"
#include "WeightPolynomial.hpp"
#include "Comparison.hpp"
#include "GlobAlloc.hpp"
#include "ObjectPool.hpp"
#include "WeightPolynomialSpecialisedComparisonCommand.hpp"
#include "ForwardMatchingSubstitutionCache.hpp"
#include "InstanceRetrievalForDemodulation.hpp"
#include "LocalUnification.hpp"
#include "TermWeightType.hpp"
//===================================================================
#undef DEBUG_NAMESPACE
#ifdef DEBUG_STANDARD_KB_ORDERING
#define DEBUG_NAMESPACE "StandardKBOrdering"
#endif
#include "debugMacros.hpp"
//===================================================================

namespace VK
{  
  class StandardKBOrdering : public SimplificationOrdering
  {
  public:
    StandardKBOrdering();
    ~StandardKBOrdering();
    void* operator new(size_t);
    void operator delete(void* obj);
    BK::Comparison compare(const Flatterm* term1,
		       const WeightPolynomial& weight1,
		       const Flatterm* term2,
		       const WeightPolynomial& weight2);

    BK::Comparison compareAtoms(const Flatterm* lit1,
			    const WeightPolynomial& weight1,
			    const Flatterm* lit2,
			    const WeightPolynomial& weight2);

    BK::Comparison compareOnCurrentUnifier(PrefixSym* complexTerm1,
				       PrefixSym* complexTerm2);

    BK::Comparison compareOnCurrentUnifier(const TERM& term1,
				       const TERM& term2,
				       ulong variableIndex);

    BK::Comparison compare(const TERM* complexTerm1,
		       const TERM* complexTerm2,
		       const InstRet::Substitution* subst);

    BK::Comparison compare(const TERM& term1,
		       const TERM& term2,
		       const ForwardMatchingSubstitutionCache* subst);

    SimplificationOrdering::SpecialisedComparison* reserveSpecialisedComparison();

    void releaseSpecialisedComparison(SimplificationOrdering::SpecialisedComparison* sc);

    SimplificationOrdering::StoredConstraint*
    storedConstraintGreater(SimplificationOrdering::SpecialisedComparison* sc);
    SimplificationOrdering::StoredConstraint*
    storedConstraintGreaterOrEqual(SimplificationOrdering::SpecialisedComparison* sc);
    SimplificationOrdering::StoredConstraint*
    storedConstraintLess(SimplificationOrdering::SpecialisedComparison* sc);
    SimplificationOrdering::StoredConstraint*
    storedConstraintLessOrEqual(SimplificationOrdering::SpecialisedComparison* sc);

    SimplificationOrdering::StoredConstraint* copy(SimplificationOrdering::StoredConstraint* constr);

    void releaseConstraint(SimplificationOrdering::StoredConstraint* constr);

    SimplificationOrdering::ConstraintCheckOnUnifier* reserveConstraintCheckOnUnifier();
    void releaseConstraintCheckOnUnifier(SimplificationOrdering::ConstraintCheckOnUnifier* ccu);

 
    SimplificationOrdering::ConstraintCheckOnForwardMatchingSubst* 
    reserveConstraintCheckOnForwardMatchingSubst();

    void 
    releaseConstraintCheckOnForwardMatchingSubst(SimplificationOrdering::ConstraintCheckOnForwardMatchingSubst* ccfms);
  

    SimplificationOrdering::CompiledConstraintForUnifiers* reserveCompiledConstraintForUnifiers();
    void releaseCompiledConstraintForUnifiers(SimplificationOrdering::CompiledConstraintForUnifiers* cc);

    SimplificationOrdering::CompiledConstraintForInstanceRetrieval* reserveCompiledConstraintForInstanceRetrieval();
    void releaseCompiledConstraintForInstanceRetrieval(SimplificationOrdering::CompiledConstraintForInstanceRetrieval* cc);

    BK::Comparison compareWithTransposition(const Flatterm* term);

 
 
#ifdef NO_DEBUG
  private:
#else
  public:
#endif
    class SpecialisedComparison;
    class ConstraintCheckOnUnifier;
    class ConstraintCheckOnForwardMatchingSubst;
    class StoredConstraint;
    class CompiledConstraintForUnifiers;
    class CompiledConstraintForInstanceRetrieval;
    typedef BK::ObjectPool<BK::GlobAlloc,Prefix::Iter> PrefixIterPool;
    typedef BK::ObjectPool<BK::GlobAlloc,StdTermWithSubst> StTermWithSubstPool;
  private:

    BK::Comparison compare(const Flatterm* term1,
		       const Flatterm* term2);

    BK::Comparison compareLexicographically(const Flatterm* term1,
					const Flatterm* term2);

    BK::Comparison compareAtomsLexicographically(const Flatterm* lit1,
					     const Flatterm* lit2);

    BK::Comparison compareLexicographically(const TERM* complexTerm1,
					const TERM* complexTerm2);

    BK::Comparison compareLexicographically(const TERM& term1,
					const TERM& term2);

    bool termWithUnifierContainsVariable(PrefixSym* complexTerm,
					 Unifier::Variable* var);

    BK::Comparison compareLexOnCurrentUnifier(PrefixSym* complexTerm1,
					  PrefixSym* complexTerm2);

    bool termWithUnifierContainsVariable(const TERM* complexTerm,
					 ulong variableIndex,
					 Unifier::Variable* var);

    bool termWithUnifierContainsVariable(const TERM& term,
					 ulong variableIndex,
					 Unifier::Variable* var);

    BK::Comparison compareOnCurrentUnifier(PrefixSym* complexTerm1,
				       const TERM* complexTerm2,
				       ulong variableIndex);

    BK::Comparison compareLexOnCurrentUnifier(PrefixSym* complexTerm1,
					  const TERM* complexTerm2,
					  ulong variableIndex);

    BK::Comparison compareLexOnCurrentUnifier(PrefixSym* complexTerm1,
					  const TERM& term2,
					  ulong variableIndex);


    BK::Comparison compareLexOnCurrentUnifier(const TERM& term1,
					  const TERM& term2,
					  ulong variableIndex);

    BK::Comparison compareLexicographically(const TERM* complexTerm1,
					const InstRet::Substitution* subst,
					const TERM* complexTerm2);

    BK::Comparison compareLexicographically(const TERM* complexTerm1,
					const InstRet::Substitution* subst,
					const TERM& term2);

    

    BK::Comparison compare(const TERM* complexTerm1,
		       const InstRet::Substitution* subst,
		       const TERM& term2);

    BK::Comparison compare(const TERM* complexTerm1,const TERM* complexTerm2);

    BK::Comparison compare(const TERM& term1,const TERM& term2);
		     

    BK::Comparison compare(const TERM& term1,
		       const TERM& term2,
		       const InstRet::Substitution* subst);

    BK::Comparison compareLexicographically(const TERM* complexTerm1,
					const TERM* complexTerm2,
					const InstRet::Substitution* subst);

    BK::Comparison compare(const TERM& term1,
		       const ForwardMatchingSubstitutionCache* subst,
		       const Flatterm* term2);

    BK::Comparison compareLexicographically(const TERM* complexTerm1,
					const ForwardMatchingSubstitutionCache* subst,
					const Flatterm* term2);

    BK::Comparison compareLexicographically(const TERM& term1,
					const TERM& term2,
					const ForwardMatchingSubstitutionCache* subst);

    WeightPolynomial::SpecialisedComparison*
    reserveSpecialisedWeightComparison();
    
    void 
    releaseSpecialisedWeightComparison(WeightPolynomial::SpecialisedComparison* swc);

  private:

    SpecialisedComparison* _specialisedComparisonPool;

    ConstraintCheckOnUnifier* _constraintCheckOnUnifierPool;
  
    ConstraintCheckOnForwardMatchingSubst* _constraintCheckOnForwardMatchingSubstPool;

    CompiledConstraintForUnifiers* _compiledConstraintOnUnifiersPool;

    CompiledConstraintForInstanceRetrieval* _compiledConstraintForInstanceRetrievalPool;

    BK::ObjectPool<BK::GlobAlloc,WeightPolynomial::SpecialisedComparison> 
    _specialisedWeightComparisonPool;

    WeightPolynomial _weight1;
    WeightPolynomial _weight2;

    PrefixIterPool _prefixIterPool;
    StTermWithSubstPool _stTermWithSubstPool;

    ulong _numberOfCompiledConstraintsForInstanceRetrieval;
    ulong _numberOfConstraintChecksForInstanceRetrieval;
    ulong _numberOfCompiledConstraintsOnUnifiers;
    ulong _numberOfConstraintChecksOnUnifiers;

    friend class SpecialisedComparison;
    friend class ConstraintCheckOnUnifier;
    friend class ConstraintCheckOnForwardMatchingSubst;
    friend class CompiledConstraintForUnifiers;
    friend class CompiledConstraintForInstanceRetrieval;
  }; // class StandardKBOrdering : public SimplificationOrdering

}; // namespace VK

//===================================================================
#undef DEBUG_NAMESPACE
#ifdef DEBUG_STANDARD_KB_ORDERING
#define DEBUG_NAMESPACE "StandardKBOrdering::SpecialisedComparison"
#endif
#include "debugMacros.hpp"
//===================================================================

namespace VK
{
  class StandardKBOrdering::SpecialisedComparison :
    public SimplificationOrdering::SpecialisedComparison
  {
  public:
    void load(const TERM& lhs,const TERM& rhs);
    bool lhsVarInstMayOrder();
    bool rhsVarInstMayOrder();
    void specialiseGreater();
    void specialiseGreaterOrEqual();
    void specialiseLess();
    void specialiseLessOrEqual();


#ifndef NO_DEBUG_VIS
    ostream& output(ostream& str) const;
#endif

  private:
    SpecialisedComparison(StandardKBOrdering* ord) : 
      _ordering(ord),
      _nextInPool(0),
      _constraintGreater(0),
      _constraintGreaterOrEqual(0),
      _constraintLess(0),
      _constraintLessOrEqual(0)
    {
    };
    ~SpecialisedComparison();
    void* operator new(size_t)
    {
      return BK::GlobAlloc::allocate(sizeof(SpecialisedComparison));
    };
    void operator delete(void* obj)
    {
      BK::GlobAlloc::deallocate(obj,sizeof(SpecialisedComparison));
    };
    SpecialisedComparison* getNextInPool() { return _nextInPool; };
    void setNextInPool(StandardKBOrdering::SpecialisedComparison* sc) { _nextInPool = sc; };
    void specialise();
    StandardKBOrdering::StoredConstraint* storedConstraintGreater();
    StandardKBOrdering::StoredConstraint* storedConstraintGreaterOrEqual();
    StandardKBOrdering::StoredConstraint* storedConstraintLess();
    StandardKBOrdering::StoredConstraint* storedConstraintLessOrEqual();
    const WeightPolynomial::SpecialisedComparison& weightComparison() const
    {
      return _weightComparison;
    };
  

  private:
    StandardKBOrdering* _ordering;
    TERM _lhs;
    TERM _rhs;
    WeightPolynomial _lhsWeight;
    WeightPolynomial _rhsWeight;
    bool _specialised;
    WeightPolynomial::SpecialisedComparison _weightComparison;
    SpecialisedComparison* _nextInPool;
    StandardKBOrdering::StoredConstraint* _constraintGreater;
    StandardKBOrdering::StoredConstraint* _constraintGreaterOrEqual;
    StandardKBOrdering::StoredConstraint* _constraintLess;
    StandardKBOrdering::StoredConstraint* _constraintLessOrEqual;
    friend class StandardKBOrdering;
    friend class StandardKBOrdering::CompiledConstraintForUnifiers;
    friend class StandardKBOrdering::CompiledConstraintForInstanceRetrieval;
  }; // class StandardKBOrdering::SpecialisedComparison

}; // namespace VK

#ifndef NO_DEBUG_VIS

namespace std
{
  inline
  ostream& operator<<(ostream& str,
		      const VK::StandardKBOrdering::SpecialisedComparison& sc)
  {
    return sc.output(str);
  };

  inline
  ostream& operator<<(ostream& str,
		      const VK::StandardKBOrdering::SpecialisedComparison* sc)
  {
    return sc->output(str);
  };
}; // namespace std
#endif



//===================================================================
#undef DEBUG_NAMESPACE
#ifdef DEBUG_SIMPLIFICATION_ORDERING
#define DEBUG_NAMESPACE "StandardKBOrdering::ConstraintCheckOnUnifier"
#endif
#include "debugMacros.hpp"
//===================================================================

namespace VK
{

  class StandardKBOrdering::ConstraintCheckOnUnifier :
    public SimplificationOrdering::ConstraintCheckOnUnifier
  {
  public:
    bool holds(const SimplificationOrdering::StoredConstraint* constr,
	       ulong variableIndex);
  private:
    ConstraintCheckOnUnifier(StandardKBOrdering* ord) : 
      _ordering(ord),
      _nextInPool(0) 
    {
    };
    ~ConstraintCheckOnUnifier();   
    void* operator new(size_t)
    {
      return BK::GlobAlloc::allocate(sizeof(ConstraintCheckOnUnifier));
    };
    void operator delete(void* obj)
    {
      BK::GlobAlloc::deallocate(obj,sizeof(ConstraintCheckOnUnifier));
    };  
    ConstraintCheckOnUnifier* getNextInPool() { return _nextInPool; };
    void setNextInPool(StandardKBOrdering::ConstraintCheckOnUnifier* ccu) { _nextInPool = ccu; };
  
    bool holdsGreater(const StoredConstraint* constr);
    bool holdsGreaterOrEqual(const StoredConstraint* constr);
    bool holdsLess(const StoredConstraint* constr);
    bool holdsLessOrEqual(const StoredConstraint* constr);

    BK::FunctionComparison 
    checkGreaterOrEqual(const WeightPolynomial::SpecialisedComparison::Stored& weightComparison);
    BK::FunctionComparison 
    checkLessOrEqual(const WeightPolynomial::SpecialisedComparison::Stored& weightComparison);

    bool lexicorgaphicallyGreater(const TERM& lhs,const TERM& rhs);
    bool lexicorgaphicallyGreaterOrEqual(const TERM& lhs,const TERM& rhs);
    bool lexicorgaphicallyLess(const TERM& lhs,const TERM& rhs);
    bool lexicorgaphicallyLessOrEqual(const TERM& lhs,const TERM& rhs);

  
  private:
    ConstraintCheckOnUnifier() 
    {
      CALL("constructor ConstraintCheckOnUnifier()");
      ICP("ICP0");
    };
  private:
    StandardKBOrdering* _ordering;
    ConstraintCheckOnUnifier* _nextInPool;
    ulong _currentVariableIndex;
    StdTermWithSubst _lhsIter;
    StdTermWithSubst _rhsIter;
    WeightPolynomial _weightPositivePartInstance;
    WeightPolynomial _weightNegativePartInstance;
    friend class StandardKBOrdering;
  }; // class SimplificationOrdering::ConstraintCheckOnUnifier

}; // namespace VK



//===================================================================
#undef DEBUG_NAMESPACE
#ifdef DEBUG_SIMPLIFICATION_ORDERING
#define DEBUG_NAMESPACE "StandardKBOrdering::ConstraintCheckOnForwardMatchingSubst"
#endif
#include "debugMacros.hpp"
//===================================================================

namespace VK
{
  class StandardKBOrdering::ConstraintCheckOnForwardMatchingSubst :
    public SimplificationOrdering::ConstraintCheckOnForwardMatchingSubst
  {
  public:
    bool holds(const SimplificationOrdering::StoredConstraint* constr,
	       const ForwardMatchingSubstitutionCache* subst);
  private:
    ConstraintCheckOnForwardMatchingSubst(StandardKBOrdering* ord) : 
      _ordering(ord),
      _nextInPool(0)
    {
    };
    ~ConstraintCheckOnForwardMatchingSubst();
    void* operator new(size_t)
    {
      return BK::GlobAlloc::allocate(sizeof(ConstraintCheckOnForwardMatchingSubst));
    };
    void operator delete(void* obj)
    {
      BK::GlobAlloc::deallocate(obj,sizeof(ConstraintCheckOnForwardMatchingSubst));
    };  
    ConstraintCheckOnForwardMatchingSubst* getNextInPool() { return _nextInPool; };
    void setNextInPool(StandardKBOrdering::ConstraintCheckOnForwardMatchingSubst* ccfms) 
    { 
      _nextInPool = ccfms; 
    };
  
    bool holdsGreater(const StoredConstraint* constr);
    bool holdsGreaterOrEqual(const StoredConstraint* constr);
    bool holdsLess(const StoredConstraint* constr);
    bool holdsLessOrEqual(const StoredConstraint* constr);

    BK::FunctionComparison 
    checkGreaterOrEqual(const WeightPolynomial::SpecialisedComparison::Stored& weightComparison);
    BK::FunctionComparison 
    checkLessOrEqual(const WeightPolynomial::SpecialisedComparison::Stored& weightComparison);

    bool lexicorgaphicallyGreater(const TERM& lhs,const TERM& rhs);
    bool lexicorgaphicallyGreaterOrEqual(const TERM& lhs,const TERM& rhs);
    bool lexicorgaphicallyLess(const TERM& lhs,const TERM& rhs);
    bool lexicorgaphicallyLessOrEqual(const TERM& lhs,const TERM& rhs);


  private:
    ConstraintCheckOnForwardMatchingSubst() 
    {
      CALL("constructor ConstraintCheckOnForwardMatchingSubst()");
      ICP("ICP0");
    };

  private:
    StandardKBOrdering* _ordering;
    ConstraintCheckOnForwardMatchingSubst* _nextInPool;
    const ForwardMatchingSubstitutionCache* _subst;
    ForwardMatchingSubstitutionCache::Instance _lhsInstance;
    ForwardMatchingSubstitutionCache::Instance _rhsInstance;
    WeightPolynomial _weightPositivePartInstance;
    WeightPolynomial _weightNegativePartInstance;
    friend class StandardKBOrdering;
  }; // class StandardKBOrdering::ConstraintCheckOnForwardMatchingSubst

}; // namespace VK

//===================================================================
#undef DEBUG_NAMESPACE
#ifdef DEBUG_STANDARD_KB_ORDERING
#define DEBUG_NAMESPACE "StandardKBOrdering::StoredConstraint"
#endif
#include "debugMacros.hpp"
//===================================================================

namespace VK
{

  class StandardKBOrdering::StoredConstraint
  {
  public:
    enum TargetRelation { Greater, GreaterOrEqual, Less, LessOrEqual };
  public:
    StoredConstraint(TargetRelation tr,
		     const WeightPolynomial::SpecialisedComparison& wc,
		     const TERM& lhs,
		     const TERM& rhs) :
      _referenceCounter(0L),
      _targetRelation(tr),
      _weightComparison(wc),
      _lhs(lhs),
      _rhs(rhs)
    {
      CALL("constructor StoredConstraint(TargetRelation tr,..)");
    };
    
    ~StoredConstraint() 
    {
      CALL("destructor ~StoredConstraint()");
      ASSERT(!referenceCounter());
    };

    void* operator new(size_t)
    {
      INTERCEPT_FORWARD_CONSTR_CHECK_MEMORY_PLUS(-16);
      return BK::GlobAlloc::allocate(sizeof(StoredConstraint));
    };  
    void operator delete(void* obj)
    {
      INTERCEPT_FORWARD_CONSTR_CHECK_MEMORY_PLUS(16);
      BK::GlobAlloc::deallocate(obj,sizeof(StoredConstraint));
    };

    const long& referenceCounter() const { return _referenceCounter; };
    void incReferenceCounter() 
    {
      CALL("incReferenceCounter()");
      ++_referenceCounter; 
    };
    void decReferenceCounter() 
    {
      CALL("decReferenceCounter()");
      --_referenceCounter;
      ASSERT(_referenceCounter >= 0L);
    };

    const TargetRelation& targetRelation() const { return _targetRelation; };
    const WeightPolynomial::SpecialisedComparison::Stored& weightComparison() const
    {
      return _weightComparison;
    };
    const TERM& lhs() const { return _lhs; };
    const TERM& rhs() const { return _rhs; };

#ifndef NO_DEBUG_VIS
    ostream& output(ostream& str) const
    {
      switch (targetRelation())
	{
	case Greater:        str << "Greater        "; break;
	case GreaterOrEqual: str << "GreaterOrEqual "; break;
	case Less:           str << "Less           "; break;
	case LessOrEqual:    str << "LessOrEqual    "; break;
	default:             str << "?????????????? ";
	};
      str << lhs() << " vs " << rhs() << " --> " << weightComparison();
      return str;
    };
#endif

  private:
    StoredConstraint() 
    {
      CALL("constructor StoredConstraint()");
      ICP("ICP0");
    };
    StoredConstraint(const StoredConstraint&) 
    { 
      CALL("constructor StoredConstraint(const StoredConstraint&)");
      ICP("ICP0");
    };
    StoredConstraint& operator=(const StoredConstraint&) 
    {
      CALL("operator=(const StoredConstraint&)");
      ICP("ICP0");
      return *this;
    };
  private:
    long _referenceCounter;
    TargetRelation _targetRelation;
    WeightPolynomial::SpecialisedComparison::Stored _weightComparison;
    TERM _lhs;
    TERM _rhs;
  }; // class StandardKBOrdering::StoredConstraint


}; // namespace VK

#ifndef NO_DEBUG_VIS

namespace std
{
  inline
  ostream& operator<<(ostream& str,
		      const VK::StandardKBOrdering::StoredConstraint& constr)
  {
    return constr.output(str);
  };

  inline
  ostream& operator<<(ostream& str,
		      const VK::StandardKBOrdering::StoredConstraint* constr)
  {
    return constr->output(str);
  };
}; // namespace std
#endif

//===================================================================
#undef DEBUG_NAMESPACE
#ifdef DEBUG_STANDARD_KB_ORDERING
#define DEBUG_NAMESPACE "StandardKBOrdering::CompiledConstraintForUnifiers"
#endif
#include "debugMacros.hpp"
//===================================================================

namespace VK
{

  class StandardKBOrdering::CompiledConstraintForUnifiers :
    public SimplificationOrdering::CompiledConstraintForUnifiers
  {
  public:
    CompiledConstraintForUnifiers(StandardKBOrdering* ord) :
      _ordering(ord),
      _nextInPool(0),
      _weightComparisonCode(DOP("StandardKBOrdering::CompiledConstraintForUnifiers::_weightComparisonCode")),
      _mainCode(DOP("StandardKBOrdering::CompiledConstraintForUnifiers::_mainCode"))
    {
      DOP(_weightComparisonCode.freeze());
      _mainCode.expand();
      DOP(_mainCode.freeze());
    };
    ~CompiledConstraintForUnifiers();  
    void* operator new(size_t)
    {
      return BK::GlobAlloc::allocate(sizeof(CompiledConstraintForUnifiers));
    };
    void operator delete(void* obj)
    {
      BK::GlobAlloc::deallocate(obj,sizeof(CompiledConstraintForUnifiers));
    }; 
    CompiledConstraintForUnifiers* getNextInPool() const { return _nextInPool; };
    void setNextInPool(CompiledConstraintForUnifiers* cc) { _nextInPool = cc; };

    void load(PrefixSym* lhs,PrefixSym* rhs);
    bool lhsVarInstMayOrder();
    bool rhsVarInstMayOrder();
    void compileGreaterOrEqual();
    bool holdsOnCurrentUnifier();

#ifndef NO_DEBUG_VIS
    ostream& output(ostream& str) const;
#endif


  private:
    class Command
    {
    public:
      enum Tag
      {

	RetSuccess,
	RetFailure,

	CompareST_IfCmpGrRetSucc_IfCmpNotEqRetFail,
	/*
	  cmp = compare(S@,T@);
	  if (cmp == Greater) return Success;	  
	  if (cmp != Equal) return Failure;
	 */

	CompareSY_IfCmpGrRetSucc_IfCmpNotEqRetFail,
	/*
	  cmp = compare(S@,Y@);
	  if (cmp == Greater) return Success;	  
	  if (cmp != Equal) return Failure;
	 */

	CompareXT_IfCmpGrRetSucc_IfCmpNotEqRetFail,
	/*
	  cmp = compare(X@,T@);
	  if (cmp == Greater) return Success;	  
	  if (cmp != Equal) return Failure;
	 */
	
	CompareXY_IfCmpGrRetSucc_IfCmpNotEqRetFail,
	/*
	  cmp = compare(X@,Y@);
	  if (cmp == Greater) return Success;	  
	  if (cmp != Equal) return Failure;
	 */


	

	IfWCFailRetFail_IfWCAlwGrRetSucc_IfWCCanBeGrOrEqGotoL,
	/*
	  if WC(@) = FailureToCompare return Failure;
	  if WC(@) = AlwaysGreater return Success;
	  ASSERT(WC(@) = AlwaysEqual || WC(@) = CanBeGreaterOrEqual);
	  if WC(@) = CanBeGreaterOrEqual goto L;
	*/

	IfWCFailRetFail
	/*
	  if WC(@) = FailureToCompare return Failure;
	  ASSERT(WC(@) = AlwaysEqual);
	*/

      };
    public:
      Command() {};
      ~Command() {};
      const Tag& tag() const { return _tag; };
      void setTag(const Tag& t) 
      { 
	_tag = t;
      };
      bool isTerminal() const;
      bool requiresLabel() const;
      PrefixSym* termS() const { return _termS; };
      void setTermS(PrefixSym* t)
      {
	CALL("Command::setTermS(PrefixSym* t)");
	ASSERT(t->Head().IsFunctor());
	_termS = t;
      };
      PrefixSym* termT() const { return _termT; };
      void setTermT(PrefixSym* t)
      {
	CALL("Command::setTermT(PrefixSym* t)");
	ASSERT(t->Head().IsFunctor());
	_termT = t;
      };

      Unifier::Variable* varX() const { return _varX; };
      void setVarX(Unifier::Variable* v) 
      { 
	_varX = v; 
	DOP(_varXNum = v->absoluteVarNum());
      };
      Unifier::Variable* varY() const { return _varY; };
      void setVarY(Unifier::Variable* v) 
      { 
	_varY = v; 
	DOP(_varYNum = v->absoluteVarNum());
      };

#ifndef NO_DEBUG
      ulong varXNum() const { return _varXNum; };
      ulong varYNum() const { return _varYNum; };
#endif
      
      
      WeightPolynomial::SpecialisedComparison::Command* weightComparisonSubroutine() const 
      {
	return _weightComparisonSubroutine;
      };

      void setWeightComparisonSubroutine(WeightPolynomial::SpecialisedComparison::Command* lab)
      {
	_weightComparisonSubroutine = lab;
      };
      
      Command* labelL() const { return _labelL; };
      void setLabelL(Command* l) { _labelL = l; };


#ifndef NO_DEBUG_VIS
      ostream& output(ostream& str,const Command* base) const;
#endif
    private:
      Tag _tag;       
#ifndef NO_DEBUG
      ulong _varXNum;
      ulong _varYNum;
#endif
      Unifier::Variable* _varX;
      Unifier::Variable* _varY;
      PrefixSym* _termS;
      PrefixSym* _termT;

      WeightPolynomial::SpecialisedComparison::Command* _weightComparisonSubroutine;
      Command* _labelL;
    }; // class Command

  private:
    typedef WeightPolynomial::SpecialisedComparison::Command WeightComparisonCommand;
  
#ifndef NO_DEBUG
    enum TargetRelation
    {
      GreaterOrEqual
    };
#endif

  private:
    CompiledConstraintForUnifiers()
    {
      CALL("constructor CompiledConstraintForUnifiers()");
      ICP("ICP0");
    };
    BK::Comparison compareLexicographically(PrefixSym* term1,PrefixSym* term2);
    BK::Comparison compareLexicographically(Unifier::Variable* var,PrefixSym* term);
    BK::Comparison compareLexicographically(Unifier::Variable* var1,Unifier::Variable* var2);
    BK::FunctionComparison interpretWeightComparisonCode(WeightComparisonCommand* subroutine);

    ulong currentSpecialisationStrength() const
    {
      return 
	_specialisationStrengthBase + 
	(_ordering->_numberOfConstraintChecksOnUnifiers /
	 _ordering->_numberOfCompiledConstraintsOnUnifiers) /
	_inverseSpecialisationStrengthRate;
    };

    bool compileGreaterOrEqual(PrefixSym* lhs,
			       PrefixSym* rhs,
			       WeightPolynomial::SpecialisedComparison* assumedWeightRelations,
			       Command*& mainCode,
			       const Command* endOfMainCode,
			       WeightComparisonCommand*& weightComparisonCode,
			       const WeightComparisonCommand* endOfWeightComparisonCode,
			       bool& complete,
			       ulong& specialisationStrength);

    bool compileGreaterOrEqualLex(PrefixSym* lhsComplexTerm,
				  PrefixSym* rhsComplexTerm,
				  WeightPolynomial::SpecialisedComparison* assumedWeightRelations,
				  Command*& mainCode,
				  const Command* endOfMainCode,
				  WeightComparisonCommand*& weightComparisonCode,
				  const WeightComparisonCommand* endOfWeightComparisonCode,
				  bool& complete,
				  ulong& specialisationStrength);
  private:
    StandardKBOrdering* _ordering;
    StandardKBOrdering::CompiledConstraintForUnifiers* _nextInPool;

#ifndef NO_DEBUG
    TargetRelation _targetRelation;
#endif

    PrefixSym* _lhs;
    PrefixSym* _rhs;
  
    Prefix::Iter _lhsIter;
    Prefix::Iter _rhsIter;

    WeightPolynomial _lhsWeight;
    WeightPolynomial _rhsWeight;
    WeightPolynomial::SpecialisedComparison _topLevelWeightComparison;

    BK::Array<BK::GlobAlloc,
      WeightComparisonCommand,
      64UL,
      CompiledConstraintForUnifiers> _weightComparisonCode;
  
    BK::Array<BK::GlobAlloc,Command,32UL,CompiledConstraintForUnifiers> _mainCode;

    // weight comparison abstract machine

    bool _grnd;
    TermWeightType _acc;
    WeightPolynomial _posinst;
    WeightPolynomial _neginst;

    // auxilliary objects for compilation
    
    WeightPolynomial _weight1;
    WeightPolynomial _weight2;

    // tuning parameters
    enum 
    { 
      _specialisationStrengthBase = 0UL,
      _inverseSpecialisationStrengthRate = 4UL
    };
  }; // class StandardKBOrdering::CompiledConstraintForUnifiers


}; // namespace VK






//===================================================================
#undef DEBUG_NAMESPACE
#ifdef DEBUG_STANDARD_KB_ORDERING
#define DEBUG_NAMESPACE "StandardKBOrdering::CompiledConstraintForInstanceRetrieval"
#endif
#include "debugMacros.hpp"
//===================================================================

namespace VK
{
  class StandardKBOrdering::CompiledConstraintForInstanceRetrieval :
    public SimplificationOrdering::CompiledConstraintForInstanceRetrieval
  {
  public:
    CompiledConstraintForInstanceRetrieval(StandardKBOrdering* ord) :
      _ordering(ord),
      _nextInPool(0),
      _weightComparisonCode(DOP("StandardKBOrdering::CompiledConstraintForInstanceRetrieval::_weightComparisonCode")),
      _mainCode(DOP("StandardKBOrdering::CompiledConstraintForInstanceRetrieval::_mainCode"))
    {
      _weightComparisonCode.expand();
      DOP(_weightComparisonCode.freeze());
      _mainCode.expand();
      DOP(_mainCode.freeze());
    };
    ~CompiledConstraintForInstanceRetrieval();  
    void* operator new(size_t)
    {
      return BK::GlobAlloc::allocate(sizeof(CompiledConstraintForInstanceRetrieval));
    };
    void operator delete(void* obj)
    {
      BK::GlobAlloc::deallocate(obj,sizeof(CompiledConstraintForInstanceRetrieval));
    }; 
    CompiledConstraintForInstanceRetrieval* getNextInPool() const { return _nextInPool; };
    void setNextInPool(CompiledConstraintForInstanceRetrieval* cc) { _nextInPool = cc; };

    void loadGreater(const TERM* lhs,const TERM* rhs);
    bool lhsVarInstMayOrder();
    bool rhsVarInstMayOrder();
    void compile(const InstRet::Substitution* subst);
    bool holds();

#ifndef NO_DEBUG_VIS
    ostream& output(ostream& str) const;
#endif


  private:
    class Command
    {
    public:
      enum Tag
      {
	RetSuccess,
	RetFailure,

	CompareST_IfCmpGrRetSucc_IfCmpNotEqRetFail,
	/*
	  cmp = compare(S@,T@);
	  if (cmp == Greater) return Success;	  
	  if (cmp != Equal) return Failure;
	 */

	CompareSY_IfCmpGrRetSucc_IfCmpNotEqRetFail,
	/*
	  cmp = compare(S@,Y@);
	  if (cmp == Greater) return Success;	  
	  if (cmp != Equal) return Failure;
	 */

	CompareXT_IfCmpGrRetSucc_IfCmpNotEqRetFail,
	/*
	  cmp = compare(X@,T@);
	  if (cmp == Greater) return Success;	  
	  if (cmp != Equal) return Failure;
	 */
	
	CompareXY_IfCmpGrRetSucc_IfCmpNotEqRetFail,
	/*
	  cmp = compare(X@,Y@);
	  if (cmp == Greater) return Success;	  
	  if (cmp != Equal) return Failure;
	 */


	CompareLexXY_IfCmpGrRetSucc_IfCmpNotEqRetFail,
	/*
	  cmp = compareLex(X@,Y@);
	  if (cmp == Greater) return Success;	  
	  if (cmp != Equal) return Failure;
	*/

	CompareLexXT_IfCmpGrRetSucc_IfCmpNotEqRetFail,
	/*
	  cmp = compareLex(X@,T@);
	  if (cmp == Greater) return Success;	  
	  if (cmp != Equal) return Failure;
	*/

	CompareLexSY_IfCmpGrRetSucc_IfCmpNotEqRetFail,
	/*
	  cmp = compareLex(S@,Y@);
	  if (cmp == Greater) return Success;	  
	  if (cmp != Equal) return Failure;
	*/
	

	IfWCFailRetFail_IfWCAlwGrRetSucc_IfWCCanBeGrOrEqGotoL,
	/*
	  if WC(@) = FailureToCompare return Failure;
	  if WC(@) = AlwaysGreater return Success;
	  ASSERT(WC(@) = AlwaysEqual || WC(@) = CanBeGreaterOrEqual);
	  if WC(@) = CanBeGreaterOrEqual goto L;
	*/

	IfWCFailRetFail
	/*
	  if WC(@) = FailureToCompare return Failure;
	  ASSERT(WC(@) = AlwaysEqual);
	*/

	


      };
    public:
      Command() {};
      ~Command() {};
      const Tag& tag() const { return _tag; };
      void setTag(const Tag& t) 
      { 
	_tag = t;
      };
      bool isTerminal() const;
      bool requiresLabel() const 
      { 
	return _tag == IfWCFailRetFail_IfWCAlwGrRetSucc_IfWCCanBeGrOrEqGotoL;
      };


      const TERM& termS() const { return _termS; };
      void setTermS(const TERM& t) 
      {
	CALL("setTermS(const TERM& t)");
	ASSERT(t.IsReference());
	_termS = t; 
      };
      const TERM& termT() const { return _termT; };
      void setTermT(const TERM& t) 
      {
	CALL("setTermT(const TERM& t)");
	ASSERT(t.IsReference());
	_termT = t; 
      };
      
      const TERM* varX() const { return _varX; };
      void setVarX(const TERM* v) { _varX = v; };
      const TERM* varY() const { return _varY; };
      void setVarY(const TERM* v) { _varY = v; };

#ifndef NO_DEBUG
      ulong varXNum() const { return _varXNum; };
      void setVarXNum(ulong v) { _varXNum = v; };
      ulong varYNum() const { return _varYNum; };
      void setVarYNum(ulong v) { _varYNum = v; };
#endif
      

      WeightPolynomial::SpecialisedComparison::Command* weightComparisonSubroutine() const 
      {
	return _weightComparisonSubroutine;
      };

      void setWeightComparisonSubroutine(WeightPolynomial::SpecialisedComparison::Command* lab)
      {
	_weightComparisonSubroutine = lab;
      };
      
      Command* labelL() const { return _labelL; };
      void setLabelL(Command* l) { _labelL = l; };

#ifndef NO_DEBUG_VIS
      ostream& output(ostream& str,const Command* base) const;
      ostream& outputShort(ostream& str,const Command* base) const;
#endif
    private:
      Tag _tag;       
#ifndef NO_DEBUG
      ulong _varXNum;
      ulong _varYNum;
#endif
      const TERM* _varX;
      const TERM* _varY; 
      TERM _termS;
      TERM _termT;
      
      WeightPolynomial::SpecialisedComparison::Command* _weightComparisonSubroutine;
      Command* _labelL;

    }; // class Command

  private:
    typedef WeightPolynomial::SpecialisedComparison::Command WeightComparisonCommand;
  
    enum TargetRelation
    {
      Greater
    };

  private:
    CompiledConstraintForInstanceRetrieval()
    {
      CALL("constructor CompiledConstraintForInstanceRetrieval()");
      ICP("ICP0");
    };
    BK::FunctionComparison interpretWeightComparisonCode(WeightComparisonCommand* subroutine);


    bool compileGreaterOrEqual(const TERM& lhs,
			       const TERM& rhs,
			       WeightPolynomial::SpecialisedComparison* assumedWeightRelations,
			       Command*& mainCode,
			       const Command* endOfMainCode,
			       WeightComparisonCommand*& weightComparisonCode,
			       const WeightComparisonCommand* endOfWeightComparisonCode,
			       bool& complete,
			       ulong& specialisationStrength);

    bool compileGreaterOrEqualLex(const TERM& lhs,
				  const TERM& rhs,
				  WeightPolynomial::SpecialisedComparison* assumedWeightRelations,
				  Command*& mainCode,
				  const Command* endOfMainCode,
				  WeightComparisonCommand*& weightComparisonCode,
				  const WeightComparisonCommand* endOfWeightComparisonCode,
				  bool& complete,
				  ulong& specialisationStrength);

    ulong currentSpecialisationStrength() const
    {
      return 
	_specialisationStrengthBase + 
	(_ordering->_numberOfConstraintChecksForInstanceRetrieval /
	 _ordering->_numberOfCompiledConstraintsForInstanceRetrieval) /
	_inverseSpecialisationStrengthRate;
    };

  private:
    StandardKBOrdering* _ordering;
    StandardKBOrdering::CompiledConstraintForInstanceRetrieval* _nextInPool;
    TargetRelation _targetRelation;
    const InstRet::Substitution* _subst;

    TERM _lhs;
    TERM _rhs;
  
    WeightPolynomial _lhsWeight;
    WeightPolynomial _rhsWeight;
    WeightPolynomial::SpecialisedComparison _topLevelWeightComparison;

    BK::Array<BK::GlobAlloc,
      WeightComparisonCommand,
      64UL,
      CompiledConstraintForInstanceRetrieval> _weightComparisonCode;
  
    BK::Array<BK::GlobAlloc,Command,64UL,CompiledConstraintForInstanceRetrieval> _mainCode;

    // weight comparison abstract machine

    bool _grnd;
    TermWeightType _acc;
    WeightPolynomial _posinst;
    WeightPolynomial _neginst;

    // auxilliary objects for compilation
    
    LocalUnification _assumedUnifier;
    WeightPolynomial _weight1;
    WeightPolynomial _weight2;

    // tuning parameters
    enum 
    { 
      _specialisationStrengthBase = 0UL,
      _inverseSpecialisationStrengthRate = 4UL
    };
  }; // class StandardKBOrdering::CompiledConstraintForInstanceRetrieval


}; // namespace VK

//===================================================================

#endif
