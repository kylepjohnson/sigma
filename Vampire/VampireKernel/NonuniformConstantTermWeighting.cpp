//
// File:         NonuniformConstantTermWeighting.cpp
// Description:  A term weighting scheme which assumes
//               that symbols may have different weights.
//               Not argument sensitive.    
// Created:      May 09, 2002, Alexandre Riazanov, riazanov@cs.man.ac.uk 
//===================================================================

#include "NonuniformConstantTermWeighting.hpp"
#include "prefix.hpp"
#include "ForwardMatchingSubstitutionCache.hpp"
#include "LocalUnification.hpp"
#include "TermWeightType.hpp"

//===================================================================
#undef DEBUG_NAMESPACE
#ifdef DEBUG_NONUNIFORM_CONSTANT_TERM_WEIGHTING
#  define DEBUG_NAMESPACE "NonuniformConstantTermWeighting"
#endif
#include "debugMacros.hpp"
//===================================================================


using namespace BK;
using namespace Gem;
using namespace VK;



  NonuniformConstantTermWeighting nonuniformConstantTermWeighting;

  NonuniformConstantTermWeighting::~NonuniformConstantTermWeighting() {};

  void* NonuniformConstantTermWeighting::operator new(size_t)
  {
    return GlobAlloc::allocate(sizeof(NonuniformConstantTermWeighting));
  };
  
  void NonuniformConstantTermWeighting::operator delete(void* obj)
  {
    GlobAlloc::deallocate(obj,sizeof(NonuniformConstantTermWeighting));
  };

  void NonuniformConstantTermWeighting::collectWeight(WeightPolynomial& weight,const Flatterm* term)
  {
    CALL("collectWeight(WeightPolynomial& weight,const Flatterm* term)");
    Flatterm::ConstIterator iter(term);
    do
      {
	if (iter.CurrPos()->isVariable())
	  {
	    weight.add1(iter.CurrPos()->Var());
	  }
	else
	  weight.add(iter.CurrPos()->symbolRef().constantWeightPart());
	iter.NextSym();
      }
    while (iter.NotEndOfTerm());
  }; //  void NonuniformConstantTermWeighting::collectWeight(WeightPolynomial& weight,const Flatterm* term)



  void NonuniformConstantTermWeighting::collectWeight(WeightPolynomial& weight,TermWeightType coefficient,const Flatterm* term)
  {
    CALL("collectWeight(WeightPolynomial& weight,TermWeightType coefficient,const Flatterm* term)");
    Flatterm::ConstIterator iter(term);
    TermWeightType sumOfFuncWeights = static_cast<TermWeightType>(0);
    do
      {
	if (iter.CurrPos()->isVariable())
	  {
	    weight.add(coefficient,iter.CurrPos()->Var());
	  }
	else
	  sumOfFuncWeights += iter.CurrPos()->symbolRef().constantWeightPart();
	iter.NextSym();
      }
    while (iter.NotEndOfTerm());
    weight.add(coefficient*sumOfFuncWeights);
  }; //  void NonuniformConstantTermWeighting::collectWeight(WeightPolynomial& weight,TermWeightType coefficient,const Flatterm* term)




  TermWeightType 
  NonuniformConstantTermWeighting::computeMinimalInstanceWeight(const Flatterm* term,
								bool& containsVariables)
  {
    CALL("computeMinimalInstanceWeight(const Flatterm* term,bool& containsVariables)");
    TermWeightType result = static_cast<TermWeightType>(0);
    containsVariables = false;
  
    Flatterm::ConstIterator iter(term);
    do
      {
	if (iter.CurrPos()->isVariable()) 
	  {
	    containsVariables = true;
	    ++result;
	  }
	else
	  result += iter.CurrPos()->symbolRef().constantWeightPart();
	iter.NextSym();
      }
    while (iter.NotEndOfTerm());  
    return result;
  }; // TermWeightType NonuniformConstantTermWeighting::computeMinimalInstanceWeight(const Flatterm* term,bool& containsVariables)


  TermWeightType 
  NonuniformConstantTermWeighting::computeWeightIfGroundAndLessOrEqual(TermWeightType coefficient,
								       const Flatterm* term,
								       TermWeightType weightLimit)
  {
    CALL("computeWeightIfGroundAndLessOrEqual(TermWeightType coefficient,const Flatterm* term,TermWeightType weightLimit)");

    TermWeightType result = static_cast<TermWeightType>(0);
    Flatterm::ConstIterator iter(term);
    do
      {
	if (iter.CurrPos()->isVariable()) return static_cast<TermWeightType>(0); // nonground
	result += coefficient * iter.CurrPos()->symbolRef().constantWeightPart();
	if (result > weightLimit) return static_cast<TermWeightType>(-1); // too heavy      
	iter.NextSym();
      }
    while (iter.NotEndOfTerm());
    ASSERT(result > static_cast<TermWeightType>(0));
    return result;

  }; // TermWeightType NonuniformConstantTermWeighting::computeWeightIfGroundAndLessOrEqual(TermWeightType coefficient,const Flatterm* term,TermWeightType weightLimit)




  void NonuniformConstantTermWeighting::collectWeight(WeightPolynomial& weight,const TERM& term)
  {
    CALL("collectWeight(WeightPolynomial& weight,const TERM& term");
    if (term.isVariable())
      {
	weight.add1(term.var());
      }
    else
      {
	ASSERT(term.IsReference());
	TERM::Iterator iter(term.First());
	do
	  {
	    if (iter.CurrentSymIsVar())
	      {
		weight.add1(iter.symbolRef().var());
	      }
	    else
	      weight.add(iter.symbolRef().constantWeightPart());
	  }
	while (iter.Next());
      };
  }; // void NonuniformConstantTermWeighting::collectWeight(WeightPolynomial& weight,const TERM& term)






  void NonuniformConstantTermWeighting::collectWeight(WeightPolynomial& weight,TermWeightType coefficient,const TERM& term)
  {
    CALL("collectWeight(WeightPolynomial& weight,TermWeightType coefficient,const TERM& term");
    if (term.isVariable())
      {
	weight.add(coefficient,term.var());
      }
    else
      {
	ASSERT(term.IsReference());
	TERM::Iterator iter(term.First());
	do
	  {
	    if (iter.CurrentSymIsVar())
	      {
		weight.add(coefficient,iter.symbolRef().var());
	      }
	    else
	      weight.add(coefficient * iter.symbolRef().constantWeightPart());
	  }
	while (iter.Next());
      };
  }; // void NonuniformConstantTermWeighting::collectWeight(WeightPolynomial& weight,TermWeightType coefficient,const TERM& term)




  void NonuniformConstantTermWeighting::collectWeight(WeightPolynomial& weight,TermWeightType coefficient,const PrefixSym* term)
  {
    CALL("collectWeight(WeightPolynomial& weight,TermWeightType coefficient,const PrefixSym* term)");
    if (term->Head().isVariable())
      {
	Unifier::Variable* var = term->Head().var();
	bool freeVar;
	void* instance = var->Unref(freeVar);
	if (freeVar)
	  {
	    weight.add(coefficient,(static_cast<Unifier::Variable*>(instance))->absoluteVarNum());
	    return;
	  }
	else
	  {
	    term = static_cast<PrefixSym*>(instance);
	  };
      };
    ASSERT(term->Head().IsFunctor());
    Prefix::ConstIter iter(term);
    TermWeightType sumOfFuncWeights = static_cast<TermWeightType>(0);
    do
      {
	if (iter.CurrVar())
	  {
	    weight.add(coefficient,iter.CurrVar()->absoluteVarNum());
	  }
	else
	  {
	    sumOfFuncWeights += iter.CurrSubterm()->Head().Func().constantWeightPart();
	  };
	iter.SkipSym();
      }
    while (iter.NotEmpty());
    weight.add(coefficient*sumOfFuncWeights);
  }; // void NonuniformConstantTermWeighting::collectWeight(WeightPolynomial& weight,TermWeightType coefficient,const PrefixSym* term)




  void NonuniformConstantTermWeighting::collectWeight(WeightPolynomial& weight,const PrefixSym* term)
  {
    CALL("collectWeight(WeightPolynomial& weight,const PrefixSym* term)");
    if (term->Head().isVariable())
      {
	Unifier::Variable* var = term->Head().var();
	bool freeVar;
	void* instance = var->Unref(freeVar);
	if (freeVar)
	  {
	    weight.add1((static_cast<Unifier::Variable*>(instance))->absoluteVarNum());
	    return;
	  }
	else
	  {
	    term = static_cast<PrefixSym*>(instance);
	  };
      };
    ASSERT(term->Head().IsFunctor());
    Prefix::ConstIter iter(term);
    do
      {
	if (iter.CurrVar())
	  {
	    weight.add1(iter.CurrVar()->absoluteVarNum());
	  }
	else
	  {
	    weight.add(iter.CurrSubterm()->Head().Func().constantWeightPart());
	  };
	iter.SkipSym();
      }
    while (iter.NotEmpty());
  
  }; // void NonuniformConstantTermWeighting::collectWeight(WeightPolynomial& weight,const PrefixSym* term)


  void NonuniformConstantTermWeighting::collectWeight(WeightPolynomial& weight,const TERM& term,ulong variableIndex)
  {
    CALL("collectWeight(WeightPolynomial& weight,const TERM& term,ulong variableIndex)");
    static StdTermWithSubst iter;
    iter.Reset(variableIndex,(TERM&)term);
    do
      {
	if (iter.CurrSym().isVariable())
	  {
	    weight.add1(iter.CurrSym().var()->absoluteVarNum());
	  }
	else
	  {
	    weight.add(iter.CurrSym().Func().constantWeightPart());
	  };
	iter.SkipSym();
      }
    while (!iter.Empty());

  }; // void NonuniformConstantTermWeighting::collectWeight(WeightPolynomial& weight,const TERM& term,ulong variableIndex)



  void 
  NonuniformConstantTermWeighting::collectWeight(WeightPolynomial& weight,
						 TermWeightType coefficient,
						 const TERM* complexTerm,
						 const InstRet::Substitution* subst)
  {
    CALL("collectWeight(WeightPolynomial& weight,TermWeightType coefficient,const TERM* complexTerm,const InstRet::Substitution* subst)");
   
    static InstRet::Substitution::Instance inst;
    inst.reset(complexTerm,subst);
    do
      {
	if (inst.symbol().isVariable())
	  {
	    weight.add(coefficient,inst.symbol().var());
	  }
	else
	  weight.add(coefficient * inst.symbol().constantWeightPart());
      }
    while (inst.next()); 
  }; // void NonuniformConstantTermWeighting::collectWeight(WeightPolynomial& weight,TermWeightType coefficient,const TERM* complexTerm,const InstRet::Substitution* subst)



  void 
  NonuniformConstantTermWeighting::collectWeight(WeightPolynomial& weight,
						 const TERM* complexTerm,
						 const InstRet::Substitution* subst)
  {
    CALL("collectWeight(WeightPolynomial& weight,const TERM* complexTerm,const InstRet::Substitution* subst)");
   
    static InstRet::Substitution::Instance inst;
    inst.reset(complexTerm,subst);
    do
      {
	if (inst.symbol().isVariable())
	  {
	    weight.add1(inst.symbol().var());
	  }
	else
	  weight.add(inst.symbol().constantWeightPart());
      }
    while (inst.next()); 
  }; // void NonuniformConstantTermWeighting::collectWeight(WeightPolynomial& weight,const TERM* complexTerm,const InstRet::Substitution* subst)





  void NonuniformConstantTermWeighting::collectWeight(WeightPolynomial& weight,
						      const TERM& term,
						      const ForwardMatchingSubstitutionCache* subst)
  {
    CALL("collectWeight(WeightPolynomial& weight,const TERM& term,const ForwardMatchingSubstitutionCache* subst)");
  
    static ForwardMatchingSubstitutionCache::Instance inst;
    inst.reset(term,subst);
    do
      {
	if (inst.symbol().isVariable())
	  {
	    weight.add1(inst.symbol().var());
	  }
	else
	  weight.add(inst.symbol().constantWeightPart());
      }
    while (inst.next());
  }; // void NonuniformConstantTermWeighting::collectWeight(WeightPolynomial& weight,const TERM& term,const ForwardMatchingSubstitutionCache* subst)




  void
  NonuniformConstantTermWeighting::collectWeight(WeightPolynomial& weight,
						 const TERM& term,
						 const LocalUnification* unifier)
  {
    CALL("collectWeight(WeightPolynomial& weight,const TERM& term,const LocalUnification* unifier)");
    LocalUnification::TermTraversal inst(term,unifier);
    do
      {
	if (inst.term().isVariable())
	  {
	    weight.add1(inst.term().var());
	  }
	else
	  weight.add(inst.term().First()->constantWeightPart());
	inst.next();
      }
    while (inst);
  }; // void NonuniformConstantTermWeighting::collectWeight(WeightPolynomial& weight,const TERM& term,const LocalUnification* unifier)



  TermWeightType NonuniformConstantTermWeighting::computeMinimalInstanceWeight(const PrefixSym* term,bool& containsVariables)
  {
    CALL("computeMinimalInstanceWeight(const PrefixSym* term,bool& containsVariables)");

    if (term->Head().isVariable())
      {
	Unifier::Variable* var = term->Head().var();
	bool freeVar;
	void* instance = var->Unref(freeVar);
	if (freeVar)
	  {
	    containsVariables = true;
	    return 1L; // minimal term weight
	  }
	else
	  {
	    term = static_cast<PrefixSym*>(instance);
	  };
      };

    ASSERT(term->Head().IsFunctor());  
    TermWeightType result = static_cast<TermWeightType>(0);
    containsVariables = false;
    Prefix::ConstIter iter(term);
    do
      {
	if (iter.CurrVar()) 
	  {
	    containsVariables = true;
	    ++result;
	  }
	else
	  result += iter.CurrSubterm()->Head().Func().constantWeightPart();
	iter.SkipSym();
      }
    while (iter.NotEmpty());
    return result;
  }; // TermWeightType NonuniformConstantTermWeighting::computeMinimalInstanceWeight(const PrefixSym* term,bool& containsVariables)



  TermWeightType NonuniformConstantTermWeighting::computeWeightIfGroundAndLessOrEqual(TermWeightType coefficient,const PrefixSym* term,TermWeightType weightLimit)
  {
    CALL("computeWeightIfGroundAndLessOrEqual(TermWeightType coefficient,const PrefixSym* term,TermWeightType weightLimit)");  
    // returns 0 if nonground and -1 if too heavy
    if (term->Head().isVariable())
      {
	Unifier::Variable* var = term->Head().var();
	bool freeVar;
	void* instance = var->Unref(freeVar);
	if (freeVar)
	  {
	    return static_cast<TermWeightType>(0);  // nonground
	  }
	else
	  {
	    term = static_cast<PrefixSym*>(instance);
	  };
      };

    ASSERT(term->Head().IsFunctor());  
    TermWeightType result = static_cast<TermWeightType>(0);
    Prefix::ConstIter iter(term);
    do
      {
	if (iter.CurrVar()) return static_cast<TermWeightType>(0); // nonground
	result += coefficient * iter.CurrSubterm()->Head().Func().constantWeightPart();
	if (result > weightLimit) return static_cast<TermWeightType>(-1); // too heavy
	iter.SkipSym();
      }
    while (iter.NotEmpty());
    ASSERT(result > static_cast<TermWeightType>(0));
    return result;
  }; // TermWeightType NonuniformConstantTermWeighting::computeWeightIfGroundAndLessOrEqual(TermWeightType coefficient,const PrefixSym* term,TermWeightType weightLimit)


  TermWeightType NonuniformConstantTermWeighting::computeWeightIfGroundAndLessOrEqual(const PrefixSym* term,TermWeightType weightLimit)
  {
    CALL("computeWeightIfGroundAndLessOrEqual(const PrefixSym* term,TermWeightType weightLimit)");  
    // returns 0 if nonground and -1 if too heavy
    if (term->Head().isVariable())
      {
	Unifier::Variable* var = term->Head().var();
	bool freeVar;
	void* instance = var->Unref(freeVar);
	if (freeVar)
	  {
	    return static_cast<TermWeightType>(0);  // nonground
	  }
	else
	  {
	    term = static_cast<PrefixSym*>(instance);
	  };
      };

    ASSERT(term->Head().IsFunctor());  
    TermWeightType result = static_cast<TermWeightType>(0);
    Prefix::ConstIter iter(term);
    do
      {
	if (iter.CurrVar()) return static_cast<TermWeightType>(0); // nonground
	result += iter.CurrSubterm()->Head().Func().constantWeightPart();
	if (result > weightLimit) return static_cast<TermWeightType>(-1); // too heavy
	iter.SkipSym();
      }
    while (iter.NotEmpty());
    ASSERT(result > static_cast<TermWeightType>(0));
    return result;
  }; // TermWeightType NonuniformConstantTermWeighting::computeWeightIfGroundAndLessOrEqual(const PrefixSym* term,TermWeightType weightLimit)


  TermWeightType 
  NonuniformConstantTermWeighting::computeMinimalInstanceWeightIfLessOrEqual(TermWeightType coefficient,
									     const PrefixSym* term,
									     TermWeightType weightLimit,
									     bool& ground)
  {
    CALL("computeMinimalInstanceWeightIfLessOrEqual(TermWeightType coefficient,const PrefixSym* term,..)");
    // ^ returns -1 if too heavy

    if (term->Head().isVariable())
      {
	Unifier::Variable* var = term->Head().var();
	bool freeVar;
	void* instance = var->Unref(freeVar);
	if (freeVar)
	  {
	    ground = false;
	    return (coefficient > weightLimit) ? static_cast<TermWeightType>(-1) : coefficient;
	  }
	else
	  {
	    term = static_cast<PrefixSym*>(instance);
	  };
      };

    ASSERT(term->Head().IsFunctor());  
    TermWeightType result = static_cast<TermWeightType>(0);
    ground = true;
    Prefix::ConstIter iter(term);
    do
      {
	if (iter.CurrVar()) 
	  {
	    ground = false;
	    result += coefficient;
	  }
	else
	  result += coefficient * iter.CurrSubterm()->Head().Func().constantWeightPart();
	if (result > weightLimit) return static_cast<TermWeightType>(-1); // too heavy
	iter.SkipSym();
      }
    while (iter.NotEmpty());
    ASSERT(result > static_cast<TermWeightType>(0));
    return result;

  }; // TermWeightType NonuniformConstantTermWeighting::computeMinimalInstanceWeightIfLessOrEqual(TermWeightType coefficient,const PrefixSym* term,..)


  TermWeightType 
  NonuniformConstantTermWeighting::computeMinimalInstanceWeightIfLessOrEqual(const PrefixSym* term,
									     TermWeightType weightLimit,
									     bool& ground)
  {
    CALL("computeMinimalInstanceWeightIfLessOrEqual(const PrefixSym* term,..)");
    // ^ returns -1 if too heavy

    if (weightLimit <= static_cast<TermWeightType>(0)) return static_cast<TermWeightType>(-1);
    if (term->Head().isVariable())
      {
	Unifier::Variable* var = term->Head().var();
	bool freeVar;
	void* instance = var->Unref(freeVar);
	if (freeVar)
	  {
	    ground = false;
	    return 1L; 
	  }
	else
	  {
	    term = static_cast<PrefixSym*>(instance);
	  };
      };

    ASSERT(term->Head().IsFunctor());  
    TermWeightType result = static_cast<TermWeightType>(0);
    ground = true;
    Prefix::ConstIter iter(term);
    do
      {
	if (iter.CurrVar()) 
	  {
	    ground = false;
	    ++result;
	  }
	else
	  result += iter.CurrSubterm()->Head().Func().constantWeightPart();
	
	if (result > weightLimit) return static_cast<TermWeightType>(-1); // too heavy
	iter.SkipSym();
      }
    while (iter.NotEmpty());
    ASSERT(result > static_cast<TermWeightType>(0));
    return result;

  }; // TermWeightType NonuniformConstantTermWeighting::computeMinimalInstanceWeightIfLessOrEqual(const PrefixSym* term,..)




  TermWeightType 
  NonuniformConstantTermWeighting::computeMinimalInstanceWeight(const TERM* term,bool& containsVariables)
  {
    CALL("computeMinimalInstanceWeight(const TERM* term,bool& containsVariables)");
    ASSERT(term->isComplex());
    _iter.Reset(term);
    TermWeightType result = static_cast<TermWeightType>(0);
    do
      {
	if (_iter.symbolRef().isVariable()) 
	  {
	    containsVariables = true;
	    ++result;
	  }
	else
	  result += _iter.symbolRef().constantWeightPart();
      }
    while (_iter.Next());
    return result;
  }; // long NonuniformConstantTermWeighting::computeMinimalInstanceWeight(const TERM* term,bool& containsVariables)


  TermWeightType 
  NonuniformConstantTermWeighting::computeWeightIfGroundAndLessOrEqual(TermWeightType coefficient,
								       const TERM* term,
								       TermWeightType weightLimit)
  {
    CALL("computeWeightIfGroundAndLessOrEqual(TermWeightType coefficient,const TERM* term,TermWeightType weightLimit)");  
    // returns 0 if nonground and -1 if too heavy
    ASSERT(term->isComplex());
    _iter.Reset(term);
    TermWeightType result = static_cast<TermWeightType>(0);
    do
      {
	if (_iter.symbolRef().isVariable()) return static_cast<TermWeightType>(0); // nonground
	result += coefficient * _iter.symbolRef().constantWeightPart();
	if (result > weightLimit) return static_cast<TermWeightType>(-1); // too heavy
      }
    while (_iter.Next());
    return result;

  }; // TermWeightType NonuniformConstantTermWeighting::computeWeightIfGroundAndLessOrEqual(TermWeightType coefficient,const TERM* term,TermWeightType weightLimit)



  TermWeightType 
  NonuniformConstantTermWeighting::computeWeightIfGroundAndLessOrEqual(const TERM* term,TermWeightType weightLimit)
  {
    CALL("computeWeightIfGroundAndLessOrEqual(const TERM* term,TermWeightType weightLimit)");  
    // returns 0 if nonground and -1 if too heavy
    ASSERT(term->isComplex());
    _iter.Reset(term);
    TermWeightType result = static_cast<TermWeightType>(0);
    do
      {
	if (_iter.symbolRef().isVariable()) return static_cast<TermWeightType>(0); // nonground
	result += _iter.symbolRef().constantWeightPart();
	if (result > weightLimit) return static_cast<TermWeightType>(-1); // too heavy
      }
    while (_iter.Next());
    return result;
  }; // TermWeightType NonuniformConstantTermWeighting::computeWeightIfGroundAndLessOrEqual(const TERM* term,TermWeightType weightLimit)


  TermWeightType 
  NonuniformConstantTermWeighting::computeMinimalInstanceWeightIfLessOrEqual(TermWeightType coefficient,
									     const TERM* term,
									     TermWeightType weightLimit,
									     bool& ground)
  {
    CALL("computeMinimalInstanceWeightIfLessOrEqual(TermWeightType coefficient,const TERM* term,..)");
    // ^ returns -1 if too heavy

    ASSERT(term->isComplex());
    _iter.Reset(term);
    TermWeightType result = static_cast<TermWeightType>(0);
    ground = true;
    do
      {
	if (_iter.symbolRef().isVariable()) 
	  {
	    ground = false;
	    result += coefficient;
	  }
	else
	  {
	    result += coefficient * _iter.symbolRef().constantWeightPart();
	  };
	if (result > weightLimit) return static_cast<TermWeightType>(-1); // too heavy
      }
    while (_iter.Next());
    return result;
  }; // TermWeightType NonuniformConstantTermWeighting::computeMinimalInstanceWeightIfLessOrEqual(TermWeightType coefficient,const TERM* term,..)



  TermWeightType 
  NonuniformConstantTermWeighting::computeMinimalInstanceWeightIfLessOrEqual(const TERM* term,
								
									     TermWeightType weightLimit,
									     bool& ground)
  {
    CALL("computeMinimalInstanceWeightIfLessOrEqual(const TERM* term,..)");
    // ^ returns -1 if too heavy

    ASSERT(term->isComplex());
    _iter.Reset(term);
    TermWeightType result = static_cast<TermWeightType>(0);
    ground = true;
    do
      {
	if (_iter.symbolRef().isVariable()) 
	  {
	    ground = false;
	    ++result;
	  }
	else
	  result += _iter.symbolRef().constantWeightPart();
	if (result > weightLimit) return static_cast<TermWeightType>(-1); // too heavy
      }
    while (_iter.Next());
    return result;
  }; // TermWeightType NonuniformConstantTermWeighting::computeMinimalInstanceWeightIfLessOrEqual(const TERM* term,..)


//===================================================================



 
