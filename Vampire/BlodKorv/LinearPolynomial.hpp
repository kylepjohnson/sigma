//
// File:         LinearPolynomial.hpp
// Description:  Small linear polynomials
//               with integer coefficients.
// Created:      Apr 11, 2002, Alexandre Riazanov, riazanov@cs.man.ac.uk
//===================================================================
#ifndef LINEAR_POLYNOMIAL_H
#define LINEAR_POLYNOMIAL_H
//===================================================================
#ifndef NO_DEBUG_VIS
#include <iostream>
#endif
#include "jargon.hpp"
#include "BlodKorvDebugFlags.hpp"
#include "LinearCombination.hpp"
#include "Math.hpp"
//===================================================================
#undef DEBUG_NAMESPACE
#ifdef DEBUG_LINEAR_POLYNOMIAL
#define DEBUG_NAMESPACE "LinearPolynomial<CoeffType,MaxNumOfVariables>"
#endif
#include "debugMacros.hpp"
//===================================================================

namespace BK 
{

template <class CoeffType,ulong MaxNumOfVariables>
class LinearPolynomial
{
 public:
  LinearPolynomial() :
    _freeMember(static_cast<CoeffType>(0))
    {
    };
  ~LinearPolynomial() 
  {
    CALL("destructor ~LinearPolynomial()");
    BK_CORRUPT(_freeMember);
  };
  
  void init()
  {
    CALL("init()");
    _monomials.init();
    _freeMember = static_cast<CoeffType>(0);
  };

  void destroy()
  {
    CALL("destroy()");
    BK_CORRUPT(_freeMember);
    _monomials.destroy();
  };

  void reset() { _monomials.reset(); _freeMember = static_cast<CoeffType>(0); };
  LinearCombination<CoeffType,MaxNumOfVariables>& monomials() 
    { 
      return _monomials; 
    };
  const LinearCombination<CoeffType,MaxNumOfVariables>& monomials() const 
    { 
      return _monomials; 
    };
  CoeffType& freeMember() { return _freeMember; };
  const CoeffType& freeMember() const { return _freeMember; };
  
  bool containsVariableWhichIsNotIn(const LinearPolynomial& lp) const
    {
      return monomials().containsVariableWhichIsNotIn(lp.monomials());
    };
  
  void add(const CoeffType& coeff,const LinearPolynomial& lp)
  {
    _freeMember += (coeff * lp._freeMember);
    _monomials.add(coeff,lp._monomials);
  };

  void divideBy(const CoeffType& c) 
  {
    CALL("divideBy(const CoeffType& c)");
    ASSERT(c);
    _freeMember = _freeMember / c;
    _monomials.divideBy(c);
  };
 

 private:
  LinearCombination<CoeffType,MaxNumOfVariables> _monomials;
  CoeffType _freeMember;
}; // class LinearPolynomial<class CoeffType,ulong MaxNumOfVariables>

}; // namespace BK



#ifndef NO_DEBUG_VIS

namespace std
{
template <class CoeffType,ulong MaxNumOfVariables>
inline
ostream& operator<<(ostream& str,
		    const BK::LinearPolynomial<CoeffType,MaxNumOfVariables>& lp)
{
  str << lp.freeMember();
  if (!lp.monomials().isZero())
    str << " + " << lp.monomials();
  return str;
};
}; // namespace std
#endif




//===================================================================
#endif
