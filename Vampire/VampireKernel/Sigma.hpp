//
// File:         Sigma.hpp
// Description:  Signature. 
// Created:      Oct 03, 2002, Alexandre Riazanov, riazanov@cs.man.ac.uk
// Status:       Sketch.
//============================================================================
#ifndef SIGMA_H
#define SIGMA_H
//============================================================================
#include <iostream>
#include "jargon.hpp"
#include "GlobAlloc.hpp"
#include "VampireKernelDebugFlags.hpp"
#include "String.hpp"
#include "TermWeightType.hpp"
#include "Comparison.hpp"
#include "SkipList.hpp"
#include "Symbol.hpp"
#include "ObjectPool.hpp"
//============================================================================
#ifdef DEBUG_SIGMA
#undef DEBUG_NAMESPACE
 #define DEBUG_NAMESPACE "Sigma"
#endif
#include "debugMacros.hpp"
//============================================================================

namespace VK 
{
  class Sigma
  {
  public:
    typedef Symbol<GlobAlloc> Symbol;
    typedef Function<GlobAlloc> Function;
    typedef Predicate<GlobAlloc> Predicate;
    typedef LiteralHeader<GlobAlloc> LiteralHeader;
  public:
    Sigma();
    ~Sigma();

    void setMaxlNumOfSkolemSymbols(long n) { _maxlNumOfSkolemSymbols = n; };
    void setSymbolWeightByArityCoefficient(long c) 
    { 
      _symbolWeightByArityCoefficient = c; 
    };
    void setSymbolPrecedenceByArityCoefficient(long c) 
    { 
      _symbolPrecedenceByArityCoefficient = c; 
    };
    
    void setDefaultNumericConstantWeight(const TermWeightType& w)
    {
      _defaultNumericConstantWeight = w;
    };
    
    void setDefaultNumericConstantPrecedenceClass(long pc)
    {
      _defaultNumericConstantPrecedenceClass = pc;
    };

    Function* function(const char* name,ulong arity);
    Function* numericConstant(const double& val);
    Predicate* predicate(const char* name,ulong arity);
    Predicate* skolemPredicate(ulong arity);
    bool canCreateNSkolemSymbols(ulong n)
    {  
      return _numberOfSkolemSymbols + n <= _maxlNumOfSkolemSymbols;  
    };


    ostream& output(ostream& str) const;

  private:
    class FunctionWrapper
    {
    public:
      FunctionWrapper() {};
      FunctionWrapper(Function* f) : _function(f) {};
      Function* function() const { return _function; };
      bool operator==(const FunctionWrapper& fw) const 
      { 
	return *_function == *fw._function;
      };
      bool operator!=(const FunctionWrapper& fw) const 
      { 
	return *_function != *fw._function;
      };
      bool operator>(const FunctionWrapper& fw) const 
      { 
	return *_function > *fw._function;
      };
      bool operator>=(const FunctionWrapper& fw) const 
      { 
	return *_function >= *fw._function;
      };
      bool operator<(const FunctionWrapper& fw) const 
      { 
	return *_function < *fw._function;
      };
      bool operator<=(const FunctionWrapper& fw) const 
      { 
	return *_function < *fw._function;
      };
    private:
      Function* _function;
    }; // class FunctionWrapper

    class PredicateWrapper
    {
    public:
      PredicateWrapper() {};
      PredicateWrapper(Predicate* f) : _predicate(f) {};
      Predicate* predicate() const { return _predicate; };
      bool operator==(const PredicateWrapper& fw) const 
      { 
	return *_predicate == *fw._predicate;
      };
      bool operator!=(const PredicateWrapper& fw) const 
      { 
	return *_predicate != *fw._predicate;
      };
      bool operator>(const PredicateWrapper& fw) const 
      { 
	return *_predicate > *fw._predicate;
      };
      bool operator>=(const PredicateWrapper& fw) const 
      { 
	return *_predicate >= *fw._predicate;
      };
      bool operator<(const PredicateWrapper& fw) const 
      { 
	return *_predicate < *fw._predicate;
      };
      bool operator<=(const PredicateWrapper& fw) const 
      { 
	return *_predicate < *fw._predicate;
      };
    private:
      Predicate* _predicate;
    }; // class PredicateWrapper
    
  private:
    static void print(ulong num,char*& target);
    TermWeightType autoWeight(ulong arity);
    long autoPrecedenceClass(ulong arity);
    long registerSymbolPrecedenceClass(long precClass);

  private:
    VoidSkipList<GlobAlloc,FunctionWrapper,2UL,28UL,Sigma> _functions;
    ulong _nextFunctionId;
    VoidSkipList<GlobAlloc,PredicateWrapper,2UL,28UL,Sigma> _predicates;
    ulong _nextPredicateId;

    // v temporary solution to the reference locality problem
    ObjectPool<GlobAlloc,Function> _functionPool; 
    ObjectPool<GlobAlloc,Predicate> _predicatePool;

    ulong _numberOfSkolemSymbols;
    ulong _maxlNumOfSkolemSymbols;
    ulong _nextSkolemPredicateSuffix;

    long _symbolWeightByArityCoefficient;
    long _symbolPrecedenceByArityCoefficient;
    TermWeightType _defaultNumericConstantWeight;
    long _defaultNumericConstantPrecedenceClass;

    long _smallestRegisteredSymbolPrecedenceClass;
    long _greatestRegisteredSymbolPrecedenceClass;

  }; // class Sigma
}; // namespace VK 

//============================================================================
#ifdef DEBUG_SYMBOL
#undef DEBUG_NAMESPACE
 #define DEBUG_NAMESPACE "std"
#endif
#include "debugMacros.hpp"
//============================================================================

namespace std
{
  ostream& operator<<(ostream& str,const VK::Sigma& sig)
  {
    return sig.output(str);
  };
};

//============================================================================
#endif
