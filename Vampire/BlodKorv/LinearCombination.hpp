//
// File:         LinearCombination.hpp
// Description:  Small linear combinations of variables
//               with integer coefficients.
// Created:      Apr 11, 2002, Alexandre Riazanov, riazanov@cs.man.ac.uk
//===================================================================
#ifndef LINEAR_COMBINATION_H
#define LINEAR_COMBINATION_H
//===================================================================
#ifndef NO_DEBUG_VIS
#include <iostream>
#endif
#include "jargon.hpp"
#include "BlodKorvDebugFlags.hpp"
#include "Stack.hpp"
#include "Comparison.hpp"
//===================================================================
#undef DEBUG_NAMESPACE
#ifdef DEBUG_LINEAR_COMBINATION
#define DEBUG_NAMESPACE "LinearCombination<CoeffType,MaxNumOfVariables>"
#endif
#include "debugMacros.hpp"
//===================================================================
namespace BK 
{
template <class CoeffType,ulong MaxNumOfVariables>
class LinearCombination
{
 public:
  LinearCombination() :
    _registered("LinearCombination::_registered"),
    _sumOfCoefficients(static_cast<CoeffType>(0))
    {
      for (ulong i = 0; i < MaxNumOfVariables; ++i)
	_coefficient[i] = static_cast<CoeffType>(0);
    };
  ~LinearCombination() 
  {
    CALL("destructor ~LinearCombination()");
    BK_CORRUPT(_coefficient);
    BK_CORRUPT(_sumOfCoefficients);
  };
  void init()
  {
    CALL("init()");
    _registered.init("_registered");
    _sumOfCoefficients = static_cast<CoeffType>(0);
    
    for (ulong i = 0; i < MaxNumOfVariables; ++i)
      _coefficient[i] = static_cast<CoeffType>(0);
  };

  void destroy()
  {
    CALL("destroy()");
    BK_CORRUPT(_coefficient);
    BK_CORRUPT(_sumOfCoefficients);
    _registered.destroy();
  };
  
  
  void reset() 
    {
      CALL("reset()");
      while (_registered) _coefficient[_registered.pop()] = static_cast<CoeffType>(0);
      _sumOfCoefficients = static_cast<CoeffType>(0);
#ifdef DEBUG_NAMESPACE
      for (ulong i = 0; i < MaxNumOfVariables; ++i) 
	{
	  ASSERT(_coefficient[i] == static_cast<CoeffType>(0));
	};
#endif
    };
  const CoeffType& coefficient(ulong var) const 
    {
      CALL("coefficient(ulong var) const");
      ASSERT(var < MaxNumOfVariables);
      return _coefficient[var]; 
    };
  const CoeffType& sumOfCoefficients() const { return _sumOfCoefficients; };
  ulong length() const { return _registered.size(); }; // number of monomials
  bool isZero() const { return !_registered; };
  const ulong* begin() const { return _registered.begin(); };
  const ulong* last() const { return _registered.last(); };
  const ulong* end() const { return _registered.end(); };


  void add(CoeffType coeff,ulong var)
    {
      CALL("add(CoeffType coeff,ulong var)");
      ASSERT(var < MaxNumOfVariables);
      if (coeff != static_cast<CoeffType>(0))
	{
	  _sumOfCoefficients += coeff;
	  if (_coefficient[var] != static_cast<CoeffType>(0))
	    {
	      coeff += _coefficient[var];
	      if (coeff == static_cast<CoeffType>(0))
		{
		  // unregister the variable
		  ulong* v; 
		  for (v = _registered.begin(); (*v) != var; ++v) 
		    {
		      ASSERT(v != _registered.end());
		    };
		  *v = _registered.pop();
		};
	    }
	  else
	    {
	      _registered.push(var);
	    };
	  _coefficient[var] = coeff;
	};
      ASSERT_IN(checkIntegrity(),"END");
    }; // void add(CoeffType coeff,ulong var)
  
  void add1(ulong var) // same as add(1L,var)
    {
      CALL("add1(ulong var)");
      ASSERT(var < MaxNumOfVariables);
      if (_coefficient[var] != static_cast<CoeffType>(0))
	{	  
	  if (_coefficient[var] == static_cast<CoeffType>(-1))
	    {
	      // unregister the variable
	      ulong* v; 
	      for (v = _registered.begin(); (*v) != var; ++v) 
		{
		  ASSERT(v != _registered.end());
		};
	      *v = _registered.pop();
	      _coefficient[var] = static_cast<CoeffType>(0);
	    }
	  else
	    {
	      ++(_coefficient[var]);
	    };
	}
      else
	{
	  _registered.push(var);
	  _coefficient[var] = static_cast<CoeffType>(1);
	};
      ++_sumOfCoefficients;  
      ASSERT_IN(checkIntegrity(),"END");
    }; // void add1(ulong var)

  void add(const LinearCombination& lc)
    {
      CALL("add(const LinearCombination& lc)");
      for (const ulong* v = lc.begin(); v != lc.end(); ++v)
	add(lc.coefficient(*v),*v);
      ASSERT_IN(checkIntegrity(),"END");
    }; // void add(const LinearCombination& lc)

  void add(CoeffType coeff,const LinearCombination& lc)
    {
      CALL("add(CoeffType coeff,const LinearCombination& lc)");
      for (const ulong* v = lc.begin(); v != lc.end(); ++v)
	add(coeff*lc.coefficient(*v),*v);
      ASSERT_IN(checkIntegrity(),"END");
    }; // void add(const LinearCombination& lc)

  void divideBy(const CoeffType& c)
  {
    CALL("divideBy(const CoeffType& c)");
    ASSERT(c);
    _sumOfCoefficients = static_cast<CoeffType>(0);
    ulong* v = _registered.begin();
    while (v != _registered.end())
      {
	_coefficient[*v] = _coefficient[*v]/c;
	_sumOfCoefficients += _coefficient[*v];
	if (!_coefficient[*v])
	  {
	    // unregister the variable
	    *v = _registered.pop();
	    _coefficient[*v] = static_cast<CoeffType>(0);
	  }
	else
	  ++v;
      };
    ASSERT(checkIntegrity());
  }; // void divideBy(const CoeffType& c)


  void subtract(const LinearCombination& lc)
    {
      CALL("subtract(const LinearCombination& lc)");
      for (const ulong* v = lc.begin(); v != lc.end(); ++v)
	add(-(lc.coefficient(*v)),*v);
      ASSERT_IN(checkIntegrity(),"END");
    }; // void subtract(const LinearCombination& lc)


  void addDifference(const LinearCombination& lc1,
		     const LinearCombination& lc2)
    {
      CALL("addDifference(..)");
      add(lc1);
      subtract(lc2);
      ASSERT_IN(checkIntegrity(),"END");
    }; // void addDifference(..)

  bool isEqualTo(const LinearCombination& lc) const
    {
      CALL("isEqualTo(const LinearCombination& lc) const");
      if ((sumOfCoefficients() == lc.sumOfCoefficients()) &&
	  length() == lc.length())
	{
	  for (const ulong* v = begin(); v != end(); ++v)
	    if (coefficient(*v) != lc.coefficient(*v)) 
	      return false;

	  return true;
	};
      return false;
    }; // bool isEqualTo(const LinearCombination& lc) const

  
  bool isGreaterOrEqual(const LinearCombination& lc) const
    {
      CALL("isGreaterOrEqual(const LinearCombination& lc) const");
      if (sumOfCoefficients() == lc.sumOfCoefficients())
	{
	  // can not be greater
	  if (length() == lc.length())
	    {
	      for (const ulong* v = begin(); v != end(); ++v)
		if (coefficient(*v) != lc.coefficient(*v)) 
		  return false;
	      return true; // equal
	    };
	  return false;
	}
      else
	if (sumOfCoefficients() > lc.sumOfCoefficients())
	  {
	    if (length() >= lc.length())
	      {
            const ulong* v;
		for (v = begin(); v != end(); ++v)
		  if (coefficient(*v) < lc.coefficient(*v)) 
		    return false;

		for (v = lc.begin(); v != lc.end(); ++v)
		  if (coefficient(*v) < lc.coefficient(*v)) 
		    return false;

		return true;
	      }
	    else
	      return false;

	  }
	else
	  return false;
    }; // bool isGreaterOrEqual(const LinearCombination& lc) const

  bool isGreaterOrEqual(const LinearCombination& lc,bool& equal) const
    {
      CALL("isGreaterOrEqual(const LinearCombination& lc,bool& equal) const");
      if (sumOfCoefficients() == lc.sumOfCoefficients())
	{
	  // can not be greater
	  if (length() == lc.length())
	    {
	      for (ulong* v = begin(); v != end(); ++v)
		if (coefficient(*v) != lc.coefficient(*v)) 
		  return false;
	      equal = true;
	      return true; 
	    };
	  return false;
	}
      else
	if (sumOfCoefficients() > lc.sumOfCoefficients())
	  {
	    for (ulong* v = begin(); v != end(); ++v)
	      if (coefficient(*v) < lc.coefficient(*v)) 
		return false;

	    for (ulong* v = lc.begin(); v != lc.end(); ++v)
	      if (coefficient(*v) < lc.coefficient(*v)) 
		return false;

	    equal = false;
	    return true;
	  }
	else
	  return false;
    }; // bool isGreaterOrEqual(const LinearCombination& lc,bool& equal) const


  bool containsVariableWhichIsNotIn(const LinearCombination& lc) const
    {
      CALL("containsVariableWhichIsNotIn(const LinearCombination& lc) const");
      for (const ulong* v = begin(); v != end(); ++v)
	if (lc.coefficient(*v) == static_cast<CoeffType>(0))
	  return true;
      return false;
    }; // bool containsVariableWhichIsNotIn(const LinearCombination& lc) const


  Comparison compare(const LinearCombination& lc) const
    {
      CALL("compare(const LinearCombination& lc) const");
      if (sumOfCoefficients() < lc.sumOfCoefficients())
	{
	  if (lc.isGreaterOrEqual(*this))
	    {
	      return Less;
	    }
	  else
	    return Incomparable;
	};
      
      if (sumOfCoefficients() > lc.sumOfCoefficients())
	{
	  if (isGreaterOrEqual(lc))
	    {
	      return Greater;
	    }
	  else
	    return Incomparable;
	};
      
      // sumOfCoefficients() == lc.sumOfCoefficients()
      if (isEqualTo(lc))
	{
	  return Equal;
	}
      else
	return Incomparable;
    }; // Comparison compare(const LinearCombination& lc) const
  

  public: // for debugging
 
#ifndef NO_DEBUG
  bool checkIntegrity() const
    {
      CoeffType sumOfCoeff = static_cast<CoeffType>(0);
      for (const ulong* v = begin(); v != end(); ++v)
	{
	  if (coefficient(*v) == static_cast<CoeffType>(0)) 
	    {
	      return false;
	    };
	  sumOfCoeff += coefficient(*v);
	};
      return sumOfCoeff == _sumOfCoefficients;
    };
#endif

#ifndef NO_DEBUG_VIS
  ostream& output(ostream& str) const
    {
      for (const ulong* v = begin(); v != end(); ++v)
	{
	  str << coefficient(*v) << '*' << 'X' << *v;
	  if (v != last()) str << " + ";
	};
      return str;
    };
#endif

 private:
  Stack<ulong,MaxNumOfVariables> _registered;
  CoeffType _coefficient[MaxNumOfVariables];
  CoeffType _sumOfCoefficients;
}; // class LinearCombination<class CoeffType,ulong MaxNumOfVariables>

}; // namespace BK


#ifndef NO_DEBUG_VIS
namespace std
{
template <class CoeffType,ulong MaxNumOfVariables>
inline
ostream& operator<<(ostream& str,
		    const BK::LinearCombination<CoeffType,MaxNumOfVariables>& lc)


{
  return lc.output(str);
};
}; // namespace std
#endif

//===================================================================
#endif
