//
// File:         WeightPolynomial.hpp
// Description:  Operations on weights of terms.
// Created:      Apr 11, 2002, Alexandre Riazanov, riazanov@cs.man.ac.uk
//===================================================================
#ifndef WEIGHT_POLYNOMIAL_H
#define WEIGHT_POLYNOMIAL_H
//===================================================================
#ifndef NO_DEBUG_VIS
#include <iostream>
#endif
#include "jargon.hpp"
#include "VampireKernelDebugFlags.hpp"
#include "VampireKernelConst.hpp"
#include "LinearPolynomial.hpp"
#include "Comparison.hpp"
#include "SharedLinearPolynomial.hpp"
#include "GlobAlloc.hpp"
#include "DestructionMode.hpp"
#include "TermWeightType.hpp"
//===================================================================
#undef DEBUG_NAMESPACE
#ifdef DEBUG_WEIGHT_POLYNOMIAL
 #define DEBUG_NAMESPACE "WeightPolynomial"
#endif
#include "debugMacros.hpp"
//===================================================================

namespace VK
{
class WeightPolynomial
{
 public:
  typedef BK::LinearCombination<TermWeightType,VampireKernelConst::MaxNumOfVariables * VampireKernelConst::MaxNumOfIndices> Monomials;
  typedef BK::SharedLinearPolynomial<BK::GlobAlloc,TermWeightType,VampireKernelConst::MaxNumOfVariables * VampireKernelConst::MaxNumOfIndices> Stored;
  typedef BK::LinearPolynomialSharing<BK::GlobAlloc,TermWeightType,VampireKernelConst::MaxNumOfVariables * VampireKernelConst::MaxNumOfIndices> Sharing;
  class SpecialisedComparison;
 public:
  WeightPolynomial() {};
  ~WeightPolynomial() {};

  void init()
  {
    CALL("init()");
    _linearPolynomial.init();
  };

  void destroy()
  {
    CALL("destroy()");
    _linearPolynomial.destroy();
  };

  static void setSharing(Sharing* s) 
    { 
      _sharing = s; 
    };
  void reset() { _linearPolynomial.reset(); };
  Monomials& monomials() { return _linearPolynomial.monomials(); };
  const Monomials& monomials() const { return _linearPolynomial.monomials(); };
  const TermWeightType& coefficient(ulong var) const 
    { 
      return monomials().coefficient(var); 
    };
  bool containsVariable(ulong var) const 
  { 
	  return coefficient(var) != (TermWeightType)0; 
  };
  bool isConstant() const { return monomials().isZero(); };
  bool isZero() const { return isConstant() && (!freeMember()); };
  ulong numberOfVariables() const { return monomials().length(); };

  TermWeightType& freeMember() { return _linearPolynomial.freeMember(); };
  const TermWeightType& freeMember() const { return _linearPolynomial.freeMember(); };
  TermWeightType minimalValue() const
    {
      // here we assume that the minimal term weight is 1
      return freeMember() + monomials().sumOfCoefficients();
    };
  BK::FunctionComparison compare(const WeightPolynomial& p) const;
  BK::FunctionComparison compareForGreaterOrEqual(const WeightPolynomial& p) const;
  BK::FunctionComparison compareForLessOrEqual(const WeightPolynomial& p) const;
  bool isAlwaysGreater(const WeightPolynomial& p) const;
  bool isNeverLess(const WeightPolynomial& p) const;
  bool isAlwaysLess(const WeightPolynomial& p) const
    {
      CALL("isAlwaysLess(const WeightPolynomial& p) const");
      return p.isAlwaysGreater(*this);
    };
  
  bool isNeverGreater(const WeightPolynomial& p) const
    {
      CALL("isNeverGreater(const WeightPolynomial& p) const");
      return p.isNeverLess(*this);
    };
  bool containsVariableWhichIsNotIn(const WeightPolynomial& wp) const
    {
      return _linearPolynomial.containsVariableWhichIsNotIn(wp._linearPolynomial);
    };

  void add(const TermWeightType& constant) { freeMember() += constant; };
  void add1() { ++freeMember(); };
  void add(const TermWeightType& coeff,ulong var) { monomials().add(coeff,var); };
  void add1(ulong var) { monomials().add1(var); };
  
  void add(const TermWeightType& coeff,const WeightPolynomial& p)
  {
    _linearPolynomial.add(coeff,p._linearPolynomial);
  };

  void divideBy(const TermWeightType& c)
  {
    _linearPolynomial.divideBy(c);
  };


  TermWeightType greatestCommonDivisorOfCoefficients() const
  {
    CALL("greatestCommonDivisorOfCoefficients() const");
    TermWeightType gcd;
    if (_linearPolynomial.freeMember())
      {
	gcd = _linearPolynomial.freeMember();
	for (const ulong* v = _linearPolynomial.monomials().begin();
	     v != _linearPolynomial.monomials().end();
	     ++v)
	  {
	    if (gcd == (TermWeightType)1) return (TermWeightType)1;
	    gcd = BK::Math::greatestCommonDivisor(gcd,_linearPolynomial.monomials().coefficient(*v));	    
	  };
      }
    else
      {
	if (_linearPolynomial.monomials().length())
	  {
	    const ulong* v = _linearPolynomial.monomials().begin();
	    gcd = _linearPolynomial.monomials().coefficient(*v);
	    for (++v; v != _linearPolynomial.monomials().end(); ++v)
	      {
		if (gcd == (TermWeightType)1) return (TermWeightType)1;
		gcd = 
		  BK::Math::greatestCommonDivisor(gcd,
							      _linearPolynomial.monomials().coefficient(*v));	    
	      };
	  }
	else
	  return (TermWeightType)1;
      };
    return gcd;
  }; // TermWeightType greatestCommonDivisorOfCoefficients()


  Stored* createStoredCopy() const;
  static void destroyStoredCopy(Stored* polynomial);
#ifndef NO_DEBUG_VIS
  ostream& output(ostream& str) const
    {
      return str << _linearPolynomial;
    };
#endif
  

 private:
  BK::LinearPolynomial<TermWeightType,VampireKernelConst::MaxNumOfVariables * VampireKernelConst::MaxNumOfIndices> _linearPolynomial;
  friend class SpecialisedComparison;
  static Sharing* _sharing;
}; // class WeightPolynomial 
}; // namespace VK

#ifndef NO_DEBUG_VIS

namespace std
{
inline
ostream& operator<<(ostream& str,
		    const VK::WeightPolynomial& wp)
{
  return wp.output(str);
};
}; // namespace std

#endif

//===================================================================
#undef DEBUG_NAMESPACE
#ifdef DEBUG_WEIGHT_POLYNOMIAL
 #define DEBUG_NAMESPACE "WeightPolynomial::SpecialisedComparison"
#endif
#include "debugMacros.hpp"
//===================================================================

namespace VK
{
class WeightPolynomial::SpecialisedComparison
{
 public:
  class Stored;
  class Command;
 public:
  SpecialisedComparison() {};
  ~SpecialisedComparison() {};


  void compare(const WeightPolynomial& weight1,const WeightPolynomial& weight2);
  const BK::FunctionComparison& category() const { return _category; };  
  const WeightPolynomial& positivePart() const { return _positivePart; };
  const WeightPolynomial& negativePart() const { return _negativePart; };
  ulong numberOfVariables() const 
  {
    return _positivePart.numberOfVariables() + _negativePart.numberOfVariables();
  };

  const BK::FunctionComparison& assumedRelation() const { return _assumedRelation; };
  void assumeRelation(const BK::FunctionComparison& rel) 
  {
    _assumedRelation = rel;
  };
  
  const SpecialisedComparison* next() const { return _next; };
  void setNext(SpecialisedComparison* sc) { _next = sc; };


  bool compileGreaterOrEqual(Command*& code,ulong maxCodeSize) const
    {
      return compileGreaterOrEqual(category(),positivePart(),negativePart(),
				   code,maxCodeSize);
    };
  
  bool simplifyByEqualities(const SpecialisedComparison* equalities,
			    SpecialisedComparison*& simplified,
			    SpecialisedComparison*& spareComparisonObject1,
			    SpecialisedComparison*& spareComparisonObject2) const;

  
  bool isAlwaysGreaterUnderAssumptions(const SpecialisedComparison* assumedRelations,
				       SpecialisedComparison*& spareComparisonObject1,
				       SpecialisedComparison*& spareComparisonObject2,
				       SpecialisedComparison*& spareComparisonObject3) const;


  bool isAlwaysLessUnderAssumptions(const SpecialisedComparison* assumedRelations,
				    SpecialisedComparison*& spareComparisonObject1,
				    SpecialisedComparison*& spareComparisonObject2,
				    SpecialisedComparison*& spareComparisonObject3) const;
  

 private:
  bool simplifyByEquality(const SpecialisedComparison* equality,
			  SpecialisedComparison*& simplified,
			  SpecialisedComparison*& spareComparisonObject) const;


  bool relaxAlwaysGreaterByInequality(const SpecialisedComparison* inequality,
				      SpecialisedComparison*& simplified,
				      SpecialisedComparison*& spareComparisonObject) const;

  bool relaxAlwaysLessByInequality(const SpecialisedComparison* inequality,
				   SpecialisedComparison*& simplified,
				   SpecialisedComparison*& spareComparisonObject) const;


  void applyRelation(const SpecialisedComparison* relation,
		     const TermWeightType& coeffForRelation,
		     const TermWeightType& coeffForThis,
		     SpecialisedComparison* result) const;

  static bool compileGreaterOrEqual(const BK::FunctionComparison& cat,
				    const WeightPolynomial& posPart,
				    const WeightPolynomial& negPart,
				    Command*& code,ulong maxCodeSize);
  void factoriseCoefficients();
 private:
  BK::FunctionComparison _category;
  WeightPolynomial _positivePart;
  WeightPolynomial _negativePart;
  BK::FunctionComparison _assumedRelation;
  SpecialisedComparison* _next;
}; // class WeightPolynomial::SpecialisedComparison

}; // namespace VK


#ifndef NO_DEBUG_VIS

namespace std
{
inline
ostream& operator<<(ostream& str,
		    const VK::WeightPolynomial::SpecialisedComparison& sc)
{
  str << "{" << sc.positivePart() << "} vs {"
      << sc.negativePart() << "} ~~ " << sc.category();
  return str;
};
}; // namespace std
#endif

//===================================================================
#undef DEBUG_NAMESPACE
#ifdef DEBUG_WEIGHT_POLYNOMIAL
 #define DEBUG_NAMESPACE "WeightPolynomial::SpecialisedComparison::Stored"
#endif
#include "debugMacros.hpp"
//===================================================================

namespace VK
{

class WeightPolynomial::SpecialisedComparison::Stored
{
 public:
  Stored() :
    _positivePart(0),
    _negativePart(0)
    { 
      CALL("constructor Stored()");
    };
  Stored(const WeightPolynomial::SpecialisedComparison& raw)
    { 
      CALL("constructor Stored(const WeightPolynomial::SpecialisedComparison& raw)");

      _category = raw.category();
      if (BK::strict(_category))
	{	  
          _positivePart = 0;
          _negativePart = 0;
	}
      else
	{
	  _positivePart = raw.positivePart().createStoredCopy();
	  ASSERT(_positivePart);
	  _negativePart = raw.negativePart().createStoredCopy();
	  ASSERT(_negativePart);
	};
    };
  Stored(const Stored& sc) 
    { 
      CALL("constructor Stored(const Stored& sc)");
      _category = sc.category();
      _positivePart = sc.positivePart();
      if (_positivePart) _positivePart->incReferenceCounter();
      _negativePart = sc.negativePart();
      if (_negativePart) _negativePart->incReferenceCounter();
    };
  ~Stored()
    {
      CALL("destructor ~Stored()");
      if (BK::DestructionMode::isThorough())
	{
	  WeightPolynomial::destroyStoredCopy(_positivePart);
	  WeightPolynomial::destroyStoredCopy(_negativePart);
	};
    };
  void init() 
    {
      CALL("init()");
      _positivePart = 0;
      _negativePart = 0;
    };
  void init(const WeightPolynomial::SpecialisedComparison& raw)
    {
      CALL("init(const WeightPolynomial::SpecialisedComparison& raw)");
      _category = raw.category();
      if (BK::strict(_category))
	{	  
          _positivePart = 0;
          _negativePart = 0;
	}
      else
	{
	  _positivePart = raw.positivePart().createStoredCopy();
	  ASSERT(_positivePart);
	  _negativePart = raw.negativePart().createStoredCopy();
	  ASSERT(_negativePart);
	};
    };
  void init(const Stored& sc)
    {
      CALL("init(const Stored& sc)");
      _category = sc.category();
      _positivePart = sc.positivePart();
      if (_positivePart) _positivePart->incReferenceCounter();
      _negativePart = sc.negativePart();
      if (_negativePart) _negativePart->incReferenceCounter();
    };
  void destroy()
    {
      CALL("destroy()");
      if (BK::DestructionMode::isThorough())
	{
	  WeightPolynomial::destroyStoredCopy(_positivePart);
	  WeightPolynomial::destroyStoredCopy(_negativePart);
	};
    };

  Stored& operator=(const Stored& sc)
  {
    CALL("operator=(const Stored& sc)");
    destroy();
    init(sc);
    return *this;
  };


  const BK::FunctionComparison& category() const 
    { 
      return _category; 
    };
  WeightPolynomial::Stored* positivePart() const { return _positivePart; };
  WeightPolynomial::Stored* negativePart() const { return _negativePart; };

 private:
  BK::FunctionComparison _category;
  WeightPolynomial::Stored* _positivePart;
  WeightPolynomial::Stored* _negativePart;
}; // class WeightPolynomial::SpecialisedComparison::Stored

}; // namespace VK

#ifndef NO_DEBUG_VIS


namespace std
{
inline
ostream& operator<<(ostream& str,
		    const VK::WeightPolynomial::SpecialisedComparison::Stored& comp)
{
  return
    str << "{" << comp.positivePart() << "} vs {" 
	<< comp.negativePart() << "} ~~ " << comp.category();
};

inline
ostream& operator<<(ostream& str,
		    const VK::WeightPolynomial::SpecialisedComparison::Stored* comp)
{	
  return str << *comp;
};
}; // namespace std
#endif


//===================================================================
#undef DEBUG_NAMESPACE
#ifdef DEBUG_WEIGHT_POLYNOMIAL
 #define DEBUG_NAMESPACE "WeightPolynomial"
#endif
#include "debugMacros.hpp"
//===================================================================

namespace VK
{
inline
BK::FunctionComparison 
WeightPolynomial::compare(const WeightPolynomial& p) const
{
  CALL("compare(const WeightPolynomial& p) const");

  TermWeightType minVal = minimalValue();
  TermWeightType pMinVal = p.minimalValue();
  
  if (minVal < pMinVal)
    {
      // the result is either AlwaysLess or AbsolutelyVolatile
      if (p.monomials().isGreaterOrEqual(monomials()))
	{
	  return BK::AlwaysLess;
	}
      else 
	return BK::AbsolutelyVolatile;
    };
  
  if (minVal > pMinVal)
    {
      // the result is either AlwaysGreater or AbsolutelyVolatile
      if (monomials().isGreaterOrEqual(p.monomials()))
	{
	  return BK::AlwaysGreater;
	}
      else 
	return BK::AbsolutelyVolatile;
    };

  // minVal == pMinVal
  // the result can only be AlwaysEqual, AbsolutelyVolatile, 
  // CanBeLessOrEqual or CanBeGreaterOrEqual
  
  switch (monomials().compare(p.monomials()))
    {
    case BK::Less: return BK::CanBeLessOrEqual;
    case BK::Equal: return BK::AlwaysEqual;
    case BK::Greater: return BK::CanBeGreaterOrEqual; 
    case BK::Incomparable: return BK::AbsolutelyVolatile;
    };
  
  ICP("ICP0");
 
#if (defined _SUPPRESS_WARNINGS_) || (defined DEBUG_NAMESPACE)
  return BK::FailureToCompare;
#endif

}; // BK::FunctionComparison WeightPolynomial::compare(const WeightPolynomial& p) const


inline
BK::FunctionComparison 
WeightPolynomial::compareForGreaterOrEqual(const WeightPolynomial& p) const
{
  CALL("compareForGreaterOrEqual(const WeightPolynomial& p) const");

  TermWeightType minVal = minimalValue();
  TermWeightType pMinVal = p.minimalValue();
  
  if (minVal < pMinVal)
    {
      // either AlwaysLess or AbsolutelyVolatile
      return BK::FailureToCompare; // sufficient
    };
  
  if (minVal > pMinVal)
    {
      // the result is either AlwaysGreater or AbsolutelyVolatile
      if (monomials().isGreaterOrEqual(p.monomials()))
	{
	  return BK::AlwaysGreater;
	}
      else 
	return BK::FailureToCompare;
    };

  // minVal == pMinVal
  // the result can only be AlwaysEqual, AbsolutelyVolatile, 
  // CanBeLessOrEqual or CanBeGreaterOrEqual
  
  switch (monomials().compare(p.monomials()))
    {
    case BK::Less: return BK::FailureToCompare;
    case BK::Equal: return BK::AlwaysEqual;
    case BK::Greater: return BK::CanBeGreaterOrEqual; 
    case BK::Incomparable: return BK::FailureToCompare;
    };
  
  ICP("ICP0");
 
#if (defined _SUPPRESS_WARNINGS_) || (defined DEBUG_NAMESPACE)
  return BK::FailureToCompare;
#endif

}; // BK::FunctionComparison WeightPolynomial::compareForGreaterOrEqual(const WeightPolynomial& p) const




inline
BK::FunctionComparison 
WeightPolynomial::compareForLessOrEqual(const WeightPolynomial& p) const
{
  CALL("compareForLessOrEqual(const WeightPolynomial& p) const");

  TermWeightType minVal = minimalValue();
  TermWeightType pMinVal = p.minimalValue();
  if (minVal < pMinVal)
    {
      // the result is either AlwaysLess or AbsolutelyVolatile
      if (p.monomials().isGreaterOrEqual(monomials()))
	{
	  return BK::AlwaysLess;
	}
      else 
	return BK::FailureToCompare;
    };
  
  if (minVal > pMinVal)
    {
      // either AlwaysGreater or AbsolutelyVolatile
      return BK::FailureToCompare; // sufficient
    };

  // minVal == pMinVal
  // the result can only be AlwaysEqual, AbsolutelyVolatile, 
  // CanBeLessOrEqual or CanBeGreaterOrEqual
  
  switch (monomials().compare(p.monomials()))
    {
    case BK::Less: return BK::CanBeLessOrEqual;
    case BK::Equal: return BK::AlwaysEqual;
    case BK::Greater: return BK::FailureToCompare;
    case BK::Incomparable: return BK::FailureToCompare;
    };
  
  ICP("ICP0");
 
#if (defined _SUPPRESS_WARNINGS_) || (defined DEBUG_NAMESPACE)
  return BK::FailureToCompare;
#endif

}; // BK::FunctionComparison WeightPolynomial::compareForLessOrEqual(const WeightPolynomial& p) const






inline
bool WeightPolynomial::isAlwaysGreater(const WeightPolynomial& p) const
{
  CALL("isAlwaysGreater(const WeightPolynomial& p)");
  if (minimalValue() <= p.minimalValue()) return false;
  // minimalValue() > p.minimalValue()
  return monomials().isGreaterOrEqual(p.monomials());
}; // bool WeightPolynomial::isAlwaysGreater(const WeightPolynomial& p) const


inline
bool WeightPolynomial::isNeverLess(const WeightPolynomial& p) const
{
  CALL("isNeverLess(const WeightPolynomial& p) const");
  if (minimalValue() < p.minimalValue()) return false;
  return monomials().isGreaterOrEqual(p.monomials());
}; // bool WeightPolynomial::isNeverLess(const WeightPolynomial& p) const

inline
WeightPolynomial::Stored* WeightPolynomial::createStoredCopy() const 
{
  CALL("createStoredCopy() const");

  if (isZero()) return 0;

  _sharing->resetIntegration();


  if (freeMember()) _sharing->integrConst(freeMember());

  const ulong* endOfVars = monomials().end();
  for (const ulong* v = monomials().begin(); v != endOfVars; ++v)
    { 
      _sharing->integrVar(coefficient(*v),*v);
    };


  Stored* res = _sharing->integrate();

  ASSERT(res->referenceCounter() >= 1L);

  res->incReferenceCounter();

  return res;
}; // WeightPolynomial::Stored* WeightPolynomial::createStoredCopy() const 

inline
void WeightPolynomial::destroyStoredCopy(Stored* polynomial)
{
  CALL("destroyStoredCopy(Stored* polynomial)");
  if (polynomial) 
    {
      ASSERT(polynomial->referenceCounter() >= 1L);
      polynomial->decReferenceCounter();
      if (polynomial->referenceCounter() == 1L) // only in the sharing index	
	_sharing->remove(polynomial);
    };
};
}; // namespace VK

//===================================================================
#endif
