//
// File:         NonrecursiveKBOrdering.hpp
// Description:  Nonrecursive Knuth-Bendix ordering.  
// Created:      Apr 16, 2002, Alexandre Riazanov, riazanov@cs.man.ac.uk
//=================================================================
#ifndef NONRECURSIVE_KB_ORDERING_H
#define NONRECURSIVE_KB_ORDERING_H
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
#include "WeightPolynomialSpecialisedComparisonCommand.hpp"
#include "ForwardMatchingSubstitutionCache.hpp"
#include "InstanceRetrievalForDemodulation.hpp"
#include "TermWeightType.hpp"
//===================================================================
#undef DEBUG_NAMESPACE
#ifdef DEBUG_NONRECURSIVE_KB_ORDERING
#  define DEBUG_NAMESPACE "NonrecursiveKBOrdering"
#endif
#include "debugMacros.hpp"
//===================================================================

namespace VK
{
  class NonrecursiveKBOrdering : public SimplificationOrdering
  {
  public:
    NonrecursiveKBOrdering();
    ~NonrecursiveKBOrdering();

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

  private:
    static BK::Comparison compareLexicographically(const Flatterm* term1,
					       const Flatterm* term2);

    static BK::Comparison compareAtomsLexicographically(const Flatterm* lit1,
						    const Flatterm* lit2);

    static BK::Comparison compareLexicographically(const TERM& term1,
					       const TERM& term2);

    static BK::Comparison compareLexOnCurrentUnifier(PrefixSym* complexTerm1,
						 PrefixSym* complexTerm2);

    static BK::Comparison compareLexOnCurrentUnifier(const TERM& term1,
						 const TERM& term2,
						 ulong variableIndex);

    static BK::Comparison compareLexicographically(const TERM* complexTerm1,
					       const TERM* complexTerm2,
					       const InstRet::Substitution* subst);

    static BK::Comparison compareLexicographically(const TERM& term1,
					       const TERM& term2,
					       const ForwardMatchingSubstitutionCache* subst);
  private:

    SpecialisedComparison* _specialisedComparisonPool;

    ConstraintCheckOnUnifier* _constraintCheckOnUnifierPool;
  
    ConstraintCheckOnForwardMatchingSubst* _constraintCheckOnForwardMatchingSubstPool;

    CompiledConstraintForUnifiers* _compiledConstraintOnUnifiersPool;

    CompiledConstraintForInstanceRetrieval* _compiledConstraintForInstanceRetrievalPool;

    WeightPolynomial _weight1;
    WeightPolynomial _weight2;

    friend class SpecialisedComparison;
    friend class ConstraintCheckOnUnifier;
	friend class ConstraintCheckOnForwardMatchingSubst;
  }; // class NonrecursiveKBOrdering : public SimplificationOrdering

}; // namespace VK

//===================================================================
#undef DEBUG_NAMESPACE
#ifdef DEBUG_NONRECURSIVE_KB_ORDERING
#  define DEBUG_NAMESPACE "NonrecursiveKBOrdering::SpecialisedComparison"
#endif
#include "debugMacros.hpp"
//===================================================================

namespace VK
{
  class NonrecursiveKBOrdering::SpecialisedComparison :
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
    SpecialisedComparison() : 
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
    void setNextInPool(NonrecursiveKBOrdering::SpecialisedComparison* sc) { _nextInPool = sc; };
    void specialise();
    NonrecursiveKBOrdering::StoredConstraint* storedConstraintGreater();
    NonrecursiveKBOrdering::StoredConstraint* storedConstraintGreaterOrEqual();
    NonrecursiveKBOrdering::StoredConstraint* storedConstraintLess();
    NonrecursiveKBOrdering::StoredConstraint* storedConstraintLessOrEqual();
    const WeightPolynomial::SpecialisedComparison& weightComparison() const
    {
      return _weightComparison;
    };
  
    BK::Comparison lexicographicComparisonResult()
    {
      return _lexicographicComparisonResult;
    };

  private:
    TERM _lhs;
    TERM _rhs;
    WeightPolynomial _lhsWeight;
    WeightPolynomial _rhsWeight;
    bool _specialised;
    WeightPolynomial::SpecialisedComparison _weightComparison;
    BK::Comparison _lexicographicComparisonResult;
    SpecialisedComparison* _nextInPool;
    NonrecursiveKBOrdering::StoredConstraint* _constraintGreater;
    NonrecursiveKBOrdering::StoredConstraint* _constraintGreaterOrEqual;
    NonrecursiveKBOrdering::StoredConstraint* _constraintLess;
    NonrecursiveKBOrdering::StoredConstraint* _constraintLessOrEqual;
    friend class NonrecursiveKBOrdering;
    friend class NonrecursiveKBOrdering::CompiledConstraintForUnifiers;
    friend class NonrecursiveKBOrdering::CompiledConstraintForInstanceRetrieval;
  }; // class NonrecursiveKBOrdering::SpecialisedComparison

}; // namespace VK

#ifndef NO_DEBUG_VIS

namespace std
{
  inline
  ostream& operator<<(ostream& str,
		      const VK::NonrecursiveKBOrdering::SpecialisedComparison& sc)
  {
    return sc.output(str);
  };

  inline
  ostream& operator<<(ostream& str,
		      const VK::NonrecursiveKBOrdering::SpecialisedComparison* sc)
  {
    return sc->output(str);
  };
}; // namespace std

#endif



//===================================================================
#undef DEBUG_NAMESPACE
#ifdef DEBUG_SIMPLIFICATION_ORDERING
#  define DEBUG_NAMESPACE "NonrecursiveKBOrdering::ConstraintCheckOnUnifier"
#endif
#include "debugMacros.hpp"
//===================================================================

namespace VK
{

  class NonrecursiveKBOrdering::ConstraintCheckOnUnifier :
    public SimplificationOrdering::ConstraintCheckOnUnifier
  {
  public:
    bool holds(const SimplificationOrdering::StoredConstraint* constr,
	       ulong variableIndex);
  private:
    ConstraintCheckOnUnifier() : _nextInPool(0) {};
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
    void setNextInPool(ConstraintCheckOnUnifier* ccu) 
	{ 
		_nextInPool = ccu; 
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
    ConstraintCheckOnUnifier* _nextInPool;
    ulong _currentVariableIndex;
    StdTermWithSubst _lhsIter;
    StdTermWithSubst _rhsIter;
    WeightPolynomial _weightPositivePartInstance;
    WeightPolynomial _weightNegativePartInstance;
    friend class NonrecursiveKBOrdering;
  }; // class SimplificationOrdering::ConstraintCheckOnUnifier

}; // namespace VK



//===================================================================
#undef DEBUG_NAMESPACE
#ifdef DEBUG_SIMPLIFICATION_ORDERING
#  define DEBUG_NAMESPACE "NonrecursiveKBOrdering::ConstraintCheckOnForwardMatchingSubst"
#endif
#include "debugMacros.hpp"
//===================================================================

namespace VK
{
  class NonrecursiveKBOrdering::ConstraintCheckOnForwardMatchingSubst :
    public SimplificationOrdering::ConstraintCheckOnForwardMatchingSubst
  {
  public:
    bool holds(const SimplificationOrdering::StoredConstraint* constr,
	       const ForwardMatchingSubstitutionCache* subst);
  private:
    ConstraintCheckOnForwardMatchingSubst() : 
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
    void setNextInPool(ConstraintCheckOnForwardMatchingSubst* ccfms) 
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
    ConstraintCheckOnForwardMatchingSubst* _nextInPool;
    const ForwardMatchingSubstitutionCache* _subst;
    ForwardMatchingSubstitutionCache::Instance _lhsInstance;
    ForwardMatchingSubstitutionCache::Instance _rhsInstance;
    WeightPolynomial _weightPositivePartInstance;
    WeightPolynomial _weightNegativePartInstance;
    friend class NonrecursiveKBOrdering;
  }; // class NonrecursiveKBOrdering::ConstraintCheckOnForwardMatchingSubst

}; // namespace VK

//===================================================================
#undef DEBUG_NAMESPACE
#ifdef DEBUG_NONRECURSIVE_KB_ORDERING
#  define DEBUG_NAMESPACE "NonrecursiveKBOrdering::StoredConstraint"
#endif
#include "debugMacros.hpp"
//===================================================================

namespace VK
{

  class NonrecursiveKBOrdering::StoredConstraint
  {
  public:
    enum TargetRelation { Greater, GreaterOrEqual, Less, LessOrEqual };
  public:
    StoredConstraint(TargetRelation tr,
		     const WeightPolynomial::SpecialisedComparison& wc,
		     BK::Comparison lc,
		     const TERM& lhs,
		     const TERM& rhs) :
      _referenceCounter(0L),
      _targetRelation(tr),
      _weightComparison(wc),
      _lexicographicComparisonResult(lc),
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
    const BK::Comparison& lexicographicComparisonResult() const 
    {
      return _lexicographicComparisonResult;
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
      str << " LEX = " << lexicographicComparisonResult();
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
    BK::Comparison _lexicographicComparisonResult;
    TERM _lhs;
    TERM _rhs;
  }; // class NonrecursiveKBOrdering::StoredConstraint


}; // namespace VK

#ifndef NO_DEBUG_VIS

namespace std
{
  inline
  ostream& operator<<(ostream& str,
		      const VK::NonrecursiveKBOrdering::StoredConstraint& constr)
  {
    return constr.output(str);
  };

  inline
  ostream& operator<<(ostream& str,
		      const VK::NonrecursiveKBOrdering::StoredConstraint* constr)
  {
    return constr->output(str);
  };
}; // namespace std
#endif

//===================================================================
#undef DEBUG_NAMESPACE
#ifdef DEBUG_NONRECURSIVE_KB_ORDERING
#  define DEBUG_NAMESPACE "NonrecursiveKBOrdering::CompiledConstraintForUnifiers"
#endif
#include "debugMacros.hpp"
//===================================================================

namespace VK
{
  class NonrecursiveKBOrdering::CompiledConstraintForUnifiers :
    public SimplificationOrdering::CompiledConstraintForUnifiers
  {
  public:
    CompiledConstraintForUnifiers() : 
      _nextInPool(0),
      _weightComparisonCode(DOP("NonrecursiveKBOrdering::CompiledConstraintForUnifiers::_weightComparisonCode")),
      _mainCode(DOP("NonrecursiveKBOrdering::CompiledConstraintForUnifiers::_mainCode"))
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

	IfWCFailRetFail_IfWCAlwGrRetSucc,
	/*
	  if WC(@) = FailureToCompare return Failure;
	  if WC(@) = AlwaysGreater return Success;
	  ASSERT(WC(@) = AlwaysEqual || WC(@) = CanBeGreaterOrEqual);
	*/
	
	IfLexSTEqIncRetFail_IfLexSTEqLeRetFail_RetSucc,
	/*
	  if lex(S@,T@) = Incomparable return Failure;
	  if lex(S@,T@) = Less return Failure;
	  return Success;
	*/
	
	IfLexXYEqIncRetFail_IfLexXYEqLeRetFail_IfLexXYEqGrRetSucc,
	/*
	  if lex(X@,Y@) = Incomparable return Failure;
	  if lex(X@,Y@) = Less return Failure;
	  if lex(X@,Y@) = Greater return Success;
	  ASSERT(lex(X@,Y@) = Equal);
	*/

	IfLexXTEqIncRetFail_IfLexXTEqLeRetFail_IfLexXTEqGrRetSucc,
	/*
	  if lex(X@,T@) = Incomparable return Failure;
	  if lex(X@,T@) = Less return Failure;
	  if lex(X@,T@) = Greater return Success;
	  ASSERT(lex(X@,T@) = Equal);
	*/
	
	IfLexSYEqIncRetFail_IfLexSYEqLeRetFail_IfLexTYEqGrRetSucc
	/*
	  if lex(S@,Y@) = Incomparable return Failure;
	  if lex(S@,Y@) = Less return Failure;
	  if lex(S@,Y@) = Greater return Success;
	  ASSERT(lex(S@,Y@) = Equal);
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
      Unifier::Variable* var1() const { return _var1; };
      void setVar1(Unifier::Variable* v) { _var1 = v; };
      Unifier::Variable* var2() const { return _var2; };
      void setVar2(Unifier::Variable* v) { _var2 = v; };
      PrefixSym* complexTerm1() const { return _complexTerm1; };
      void setComplexTerm1(PrefixSym* term) { _complexTerm1 = term; };
      PrefixSym* complexTerm2() const { return _complexTerm2; };
      void setComplexTerm2(PrefixSym* term) { _complexTerm2 = term; };
      bool requiresWeightComparison() const
      {
	return (tag() == IfWCFailRetFail_IfWCAlwGrRetSucc);
      };
#ifndef NO_DEBUG_VIS
      ostream& output(ostream& str) const;
#endif
    private:
      Tag _tag;
      Unifier::Variable* _var1;
      Unifier::Variable* _var2;
      PrefixSym* _complexTerm1;
      PrefixSym* _complexTerm2;
    }; // class Command

  private:
    typedef VK::WeightPolynomial::SpecialisedComparison::Command WeightComparisonCommand;
  
#ifndef NO_DEBUG
    enum TargetRelation
    {
      GreaterOrEqual
    };
#endif

  private:
    bool compileLexGreaterOrEqual(Command*& code,ulong maxCodeSize);
    BK::Comparison compareLexicographically(PrefixSym* term1,PrefixSym* term2);
    BK::Comparison compareLexicographically(Unifier::Variable* var,PrefixSym* term);
    BK::Comparison compareLexicographically(Unifier::Variable* var1,Unifier::Variable* var2);
    BK::FunctionComparison interpretWeightComparisonCode();

  private:
    NonrecursiveKBOrdering::CompiledConstraintForUnifiers* _nextInPool;

#ifndef NO_DEBUG
    TargetRelation _targetRelation;
#endif

    PrefixSym* _lhs;
    PrefixSym* _rhs;
  
    Prefix::Iter _lhsIter;
    Prefix::Iter _rhsIter;

    WeightPolynomial _lhsWeight;
    WeightPolynomial _rhsWeight;
    WeightPolynomial::SpecialisedComparison _weightComparison;

    BK::Array<BK::GlobAlloc,
      WeightComparisonCommand,
      32UL,
      CompiledConstraintForUnifiers> _weightComparisonCode;
  
    BK::Array<BK::GlobAlloc,Command,32UL,CompiledConstraintForUnifiers> _mainCode;

    // weight comparison abstract machine

    bool _grnd;
    TermWeightType _acc;
    WeightPolynomial _posinst;
    WeightPolynomial _neginst;

  }; // class NonrecursiveKBOrdering::CompiledConstraintForUnifiers


}; // namespace VK



//===================================================================
#undef DEBUG_NAMESPACE
#ifdef DEBUG_NONRECURSIVE_KB_ORDERING
#  define DEBUG_NAMESPACE "NonrecursiveKBOrdering::CompiledConstraintForInstanceRetrieval"
#endif
#include "debugMacros.hpp"
//===================================================================

namespace VK
{

  class NonrecursiveKBOrdering::CompiledConstraintForInstanceRetrieval :
    public SimplificationOrdering::CompiledConstraintForInstanceRetrieval
  {
  public:
    CompiledConstraintForInstanceRetrieval() : 
      _nextInPool(0),
      _weightComparisonCode(DOP("NonrecursiveKBOrdering::CompiledConstraintForInstanceRetrieval::_weightComparisonCode")),
      _mainCode(DOP("NonrecursiveKBOrdering::CompiledConstraintForInstanceRetrieval::_mainCode"))
    {
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

	IfWCFailRetFail_IfWCAlwGrRetSucc,
	/*
	  if WC(@) = FailureToCompare return Failure;
	  if WC(@) = AlwaysGreater return Success;
	  ASSERT(WC(@) = AlwaysEqual || WC(@) = CanBeGreaterOrEqual);
	*/
	
	IfLexSTEqIncRetFail_IfLexSTEqLeRetFail_RetSucc,
	/*
	  if lex(S@,T@) = Incomparable return Failure;
	  if lex(S@,T@) = Less return Failure;
	  return Success;
	*/
	
	IfLexXYEqIncRetFail_IfLexXYEqLeRetFail_IfLexXYEqGrRetSucc,
	/*
	  if lex(X@,Y@) = Incomparable return Failure;
	  if lex(X@,Y@) = Less return Failure;
	  if lex(X@,Y@) = Greater return Success;
	  ASSERT(lex(X@,Y@) = Equal);
	*/

	IfLexXTEqIncRetFail_IfLexXTEqLeRetFail_IfLexXTEqGrRetSucc,
	/*
	  if lex(X@,T@) = Incomparable return Failure;
	  if lex(X@,T@) = Less return Failure;
	  if lex(X@,T@) = Greater return Success;
	  ASSERT(lex(X@,T@) = Equal);
	*/
	
	IfLexSYEqIncRetFail_IfLexSYEqLeRetFail_IfLexTYEqGrRetSucc
	/*
	  if lex(S@,Y@) = Incomparable return Failure;
	  if lex(S@,Y@) = Less return Failure;
	  if lex(S@,Y@) = Greater return Success;
	  ASSERT(lex(S@,Y@) = Equal);
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
      const TERM* var1() const { return _var1; };
      void setVar1(const TERM* v) { _var1 = v; };
      const TERM* var2() const { return _var2; };
      void setVar2(const TERM* v) { _var2 = v; };
#ifndef NO_DEBUG
      ulong varNum1() const { return _varNum1; };
      void setVarNum1(ulong v) { _varNum1 = v; };
      ulong varNum2() const { return _varNum2; };
      void setVarNum2(ulong v) { _varNum2 = v; };
#endif

      const TERM* complexTerm1() const { return _complexTerm1; };
      void setComplexTerm1(const TERM* term) { _complexTerm1 = term; };
      const TERM* complexTerm2() const { return _complexTerm2; };
      void setComplexTerm2(const TERM* term) { _complexTerm2 = term; };
      bool requiresWeightComparison() const
      {
	return (tag() == IfWCFailRetFail_IfWCAlwGrRetSucc);
      };
#ifndef NO_DEBUG_VIS
      ostream& output(ostream& str) const;
#endif
    private:
      Tag _tag;      
#ifndef NO_DEBUG
      ulong _varNum1;
      ulong _varNum2;
#endif
      const TERM* _var1;
      const TERM* _var2;
      const TERM* _complexTerm1;
      const TERM* _complexTerm2;
    }; // class Command

  private:
    typedef WeightPolynomial::SpecialisedComparison::Command WeightComparisonCommand;
  
    enum TargetRelation
    {
      Greater
    };

  private:
    bool compileLexGreater(Command*& code,ulong maxCodeSize);
    BK::Comparison compareLexComplexComplex(const TERM* complexTerm1,
					const TERM* complexTerm2);
    BK::Comparison compareLexVarComplex(const TERM* var,const TERM* complexTerm);
    BK::Comparison compareLexVarVar(const TERM* var1,const TERM* var2);
    BK::FunctionComparison interpretWeightComparisonCode();

  private:
    NonrecursiveKBOrdering::CompiledConstraintForInstanceRetrieval* _nextInPool;
    TargetRelation _targetRelation;
    const InstRet::Substitution* _subst;

    const TERM* _lhs;
    const TERM* _rhs;
  
    TERM::Iterator _lhsIter;
    TERM::Iterator _rhsIter;

    InstRet::Substitution::Instance _instance1;
    InstRet::Substitution::Instance _instance2;


    WeightPolynomial _lhsWeight;
    WeightPolynomial _rhsWeight;
    WeightPolynomial::SpecialisedComparison _weightComparison;

    BK::Array<BK::GlobAlloc,
      WeightComparisonCommand,
      32UL,
      CompiledConstraintForInstanceRetrieval> _weightComparisonCode;
  
    BK::Array<BK::GlobAlloc,Command,32UL,CompiledConstraintForInstanceRetrieval> _mainCode;

    // weight comparison abstract machine

    bool _grnd;
    TermWeightType _acc;
    WeightPolynomial _posinst;
    WeightPolynomial _neginst;
  }; // class NonrecursiveKBOrdering::CompiledConstraintForInstanceRetrieval


}; // namespace VK

//===================================================================

#endif
