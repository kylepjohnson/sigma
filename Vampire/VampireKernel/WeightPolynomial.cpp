//
// File:         WeightPolynomial.cpp
// Description:  Operations on weights of terms.
// Created:      Apr 11, 2002, Alexandre Riazanov, riazanov@cs.man.ac.uk
// Revised:      May 14, 2002, Alexandre Riazanov, riazanov@cs.man.ac.uk
//               Some weight comparison commands changed. 
// Revised:      Nov 07, 2002, Alexandre Riazanov, riazanov@cs.man.ac.uk
//               Bug fix in compileGreaterOrEqual(..). The previous version
//               violated restrictions on the code size. 
//===================================================================
#include "WeightPolynomial.hpp"
#include "WeightPolynomialSpecialisedComparisonCommand.hpp"
#include "TermWeightType.hpp"
#include "Math.hpp"
using namespace BK;
using namespace VK;
//===================================================================

#ifdef DEBUG_ALLOC_OBJ_TYPE
#include "ClassDesc.hpp"

ClassDesc 
WeightPolynomial::Stored::_classDesc("WeightPolynomial::Stored",
				     WeightPolynomial::Stored::minSize(),
				     WeightPolynomial::Stored::maxSize());


ClassDesc 
WeightPolynomial::Sharing::DiscTree::Node::_classDesc("WeightPolynomial::Sharing::DiscTree::Node",
						      WeightPolynomial::Sharing::DiscTree::Node::minSize(),
						      WeightPolynomial::Sharing::DiscTree::Node::maxSize());
						      
#endif

WeightPolynomial::Sharing* WeightPolynomial::_sharing; // implicitely initialised to 0

//===================================================================
#undef DEBUG_NAMESPACE
#ifdef DEBUG_WEIGHT_POLYNOMIAL
 #define DEBUG_NAMESPACE "WeightPolynomial::SpecialisedComparison"
#endif
#include "debugMacros.hpp"
//===================================================================




bool 
WeightPolynomial::SpecialisedComparison::simplifyByEqualities(const SpecialisedComparison* relations,
							      SpecialisedComparison*& simplified,
							      SpecialisedComparison*& spareComparisonObject1,
							      SpecialisedComparison*& spareComparisonObject2) const
{
  CALL("simplifyByEqualities(..) const");
  
  bool result = false;
  const SpecialisedComparison* lastGoodEq = 0;
  bool simplifiedInInnerLoop;
  do
    {
      simplifiedInInnerLoop = false;
      for (const SpecialisedComparison* rel = relations; rel; rel = rel->next())
	{
	  if (rel->assumedRelation() == AlwaysEqual)
	    {
	      if ((rel == lastGoodEq) && (!simplifiedInInnerLoop))
		return result;
	    

	      bool eqWorks;
	      if (result)
		{
		  eqWorks = simplified->simplifyByEquality(rel,spareComparisonObject1,spareComparisonObject2);
		  if (eqWorks)
		    {
		      SpecialisedComparison* tmp = simplified;
		      simplified = spareComparisonObject1;
		      spareComparisonObject1 = tmp;
		      if (strict(simplified->category()))
			return true;
		      lastGoodEq = rel;
		    };
		}
	      else // no simplifications so far
		{
		  eqWorks = simplifyByEquality(rel,simplified,spareComparisonObject1);
		  if (eqWorks) 
		    {
		      if (strict(simplified->category()))
			return true;
		      result = true;
		      lastGoodEq = rel;
		    };
		};
	      simplifiedInInnerLoop = simplifiedInInnerLoop || eqWorks;
	    };
	};
    }
  while (simplifiedInInnerLoop);
  return result;

}; // bool WeightPolynomial::SpecialisedComparison::simplifyByEqualities(..) const


bool 
WeightPolynomial::SpecialisedComparison::isAlwaysGreaterUnderAssumptions(const SpecialisedComparison* assumedRelations,
									 SpecialisedComparison*& spareComparisonObject1,
									 SpecialisedComparison*& spareComparisonObject2,
									 SpecialisedComparison*& spareComparisonObject3) const
{
  CALL("isAlwaysGreaterUnderAssumptions(..)");
  bool someRelationWorked = false;
  const SpecialisedComparison* lastGoodRel = 0;
  bool simplifiedInInnerLoop;
  do
    {
      simplifiedInInnerLoop = false;
      for (const SpecialisedComparison* rel = assumedRelations; rel; rel = rel->next())
	{
	  
	  if ((rel == lastGoodRel) && (!simplifiedInInnerLoop))
	    return false;
	    
	  switch (rel->assumedRelation())
	    {
	    case AlwaysEqual:
	      {
		bool eqWorks;
		if (someRelationWorked)
		  {
		    eqWorks = spareComparisonObject1->simplifyByEquality(rel,spareComparisonObject2,spareComparisonObject3);
		    if (eqWorks)
		      {
			SpecialisedComparison* tmp = spareComparisonObject1;
			spareComparisonObject1 = spareComparisonObject2;
			spareComparisonObject2 = tmp;
			if (strict(spareComparisonObject1->category()))
			  {
			    return spareComparisonObject1->category() == AlwaysGreater;
			  };
			lastGoodRel = rel;
		      };
		  }
		else // no simplifications so far
		  {
		    eqWorks = simplifyByEquality(rel,spareComparisonObject1,spareComparisonObject2);
		    if (eqWorks) 
		      {
			if (strict(spareComparisonObject1->category()))
			  {
			    return spareComparisonObject1->category() == AlwaysGreater;
			  };
			  
			someRelationWorked = true;
			lastGoodRel = rel;
		      };
		  };
		simplifiedInInnerLoop = simplifiedInInnerLoop || eqWorks;
	      };
	      break;

	    case CanBeGreaterOrEqual:
	      {
		bool ineqWorks;
		if (someRelationWorked)
		  {
		    ineqWorks = 
		      spareComparisonObject1->relaxAlwaysGreaterByInequality(rel,
									     spareComparisonObject2,
									     spareComparisonObject3);
		    if (ineqWorks)
		      {
			SpecialisedComparison* tmp = spareComparisonObject1;
			spareComparisonObject1 = spareComparisonObject2;
			spareComparisonObject2 = tmp;
			if (spareComparisonObject1->category() == BK::AlwaysGreater)
			  return true;
			ASSERT(!strict(spareComparisonObject1->category()));
			lastGoodRel = rel;
		      };
		  }
		else // no simplifications so far
		  {
		    ineqWorks = relaxAlwaysGreaterByInequality(rel,spareComparisonObject1,spareComparisonObject2);
		    if (ineqWorks) 
		      {			
			if (spareComparisonObject1->category() == BK::AlwaysGreater)
			  return true;
			ASSERT(!strict(spareComparisonObject1->category()));
			someRelationWorked = true;
			lastGoodRel = rel;
		      };
		  };
		simplifiedInInnerLoop = simplifiedInInnerLoop || ineqWorks;
	      };
	      break;

	    default: 
	      break;
	    }; // switch ..
	}; // for ..
    }
  while (simplifiedInInnerLoop);
  return false;
}; // bool WeightPolynomial::SpecialisedComparison::isAlwaysGreaterUnderAssumptions(..)






bool 
WeightPolynomial::SpecialisedComparison::isAlwaysLessUnderAssumptions(const SpecialisedComparison* assumedRelations,
									 SpecialisedComparison*& spareComparisonObject1,
									 SpecialisedComparison*& spareComparisonObject2,
									 SpecialisedComparison*& spareComparisonObject3) const
{
  CALL("isAlwaysLessUnderAssumptions(..)");
  bool someRelationWorked = false;
  const SpecialisedComparison* lastGoodRel = 0;
  bool simplifiedInInnerLoop;
  do
    {
      simplifiedInInnerLoop = false;
      for (const SpecialisedComparison* rel = assumedRelations; rel; rel = rel->next())
	{
	  
	  if ((rel == lastGoodRel) && (!simplifiedInInnerLoop))
	    return false;
	    
	  switch (rel->assumedRelation())
	    {
	    case AlwaysEqual:
	      {
		bool eqWorks;
		if (someRelationWorked)
		  {
		    eqWorks = spareComparisonObject1->simplifyByEquality(rel,spareComparisonObject2,spareComparisonObject3);
		    if (eqWorks)
		      {
			SpecialisedComparison* tmp = spareComparisonObject1;
			spareComparisonObject1 = spareComparisonObject2;
			spareComparisonObject2 = tmp;
			if (strict(spareComparisonObject1->category()))
			  {
			    return spareComparisonObject1->category() == AlwaysLess;
			  };
			lastGoodRel = rel;
		      };
		  }
		else // no simplifications so far
		  {
		    eqWorks = simplifyByEquality(rel,spareComparisonObject1,spareComparisonObject2);
		    if (eqWorks) 
		      {
			if (strict(spareComparisonObject1->category()))
			  {
			    return spareComparisonObject1->category() == AlwaysLess;
			  };
			  
			someRelationWorked = true;
			lastGoodRel = rel;
		      };
		  };
		simplifiedInInnerLoop = simplifiedInInnerLoop || eqWorks;
	      };
	      break;

	    case CanBeGreaterOrEqual:
	      {
		bool ineqWorks;
		if (someRelationWorked)
		  {
		    ineqWorks = 
		      spareComparisonObject1->relaxAlwaysLessByInequality(rel,
									  spareComparisonObject2,
									  spareComparisonObject3);
		    if (ineqWorks)
		      {
			SpecialisedComparison* tmp = spareComparisonObject1;
			spareComparisonObject1 = spareComparisonObject2;
			spareComparisonObject2 = tmp;
			if (spareComparisonObject1->category() == BK::AlwaysLess)
			  return true;
			ASSERT(!strict(spareComparisonObject1->category()));
			lastGoodRel = rel;
		      };
		  }
		else // no simplifications so far
		  {
		    ineqWorks = 
		      relaxAlwaysLessByInequality(rel,
						  spareComparisonObject1,
						  spareComparisonObject2);
		    if (ineqWorks) 
		      {			
			if (spareComparisonObject1->category() == BK::AlwaysLess)
			  return true;
			ASSERT(!strict(spareComparisonObject1->category()));
			someRelationWorked = true;
			lastGoodRel = rel;
		      };
		  };
		simplifiedInInnerLoop = simplifiedInInnerLoop || ineqWorks;
	      };
	      break;

	    default: 
	      break;
	    }; // switch ..
	}; // for ..
    }
  while (simplifiedInInnerLoop);
  return false;
}; // bool WeightPolynomial::SpecialisedComparison::isAlwaysLessUnderAssumptions(..)






bool 
WeightPolynomial::SpecialisedComparison::simplifyByEquality(const SpecialisedComparison* equality,
							    SpecialisedComparison*& simplified,
							    SpecialisedComparison*& spareComparisonObject) const
{
  CALL("simplifyByEquality(..) const");
  ASSERT(equality->assumedRelation() == AlwaysEqual);

  TermWeightType simplifyingCoeffForEquality;
  TermWeightType simplifyingCoeffForThis;
  TermWeightType bestSimplifyingCoeffForEquality;
  TermWeightType bestSimplifyingCoeffForThis;
  ulong bestSimplificationStrength = 0UL;

  ulong origNumOfVariables = numberOfVariables();

  static WeightPolynomial simplifiedPositive;
  static WeightPolynomial simplifiedNegative;

  // check variables from the lhs of the equality
  const ulong* v;
  for (v = equality->_positivePart.monomials().begin();
       v != equality->_positivePart.monomials().end();
       ++v)
    {
      simplifyingCoeffForEquality = -_positivePart.monomials().coefficient(*v);
      if (!simplifyingCoeffForEquality)
	{
	  simplifyingCoeffForEquality = _negativePart.monomials().coefficient(*v);
	};

      if (simplifyingCoeffForEquality) // this variable can be eliminated
	{
	  simplifyingCoeffForThis = equality->_positivePart.monomials().coefficient(*v);

	  applyRelation(equality,simplifyingCoeffForEquality,simplifyingCoeffForThis,simplified);

	  if (strict(simplified->category()))
	    {
	      // best possible outcome
	      return true;
	    };
	  
	  // need to count the number of variables
	  ulong numOfVariablesInSimplified = simplified->numberOfVariables();
	  if ((numOfVariablesInSimplified < origNumOfVariables) &&
	      (origNumOfVariables - numOfVariablesInSimplified > bestSimplificationStrength))
	    {
	      bestSimplificationStrength = origNumOfVariables - numOfVariablesInSimplified;
	      bestSimplifyingCoeffForEquality = -simplifyingCoeffForEquality;
	      bestSimplifyingCoeffForThis = simplifyingCoeffForThis;
	      SpecialisedComparison* tmp = simplified;
	      simplified = spareComparisonObject;
	      spareComparisonObject = tmp;
	    };
	};
    }; // for ..
  
  // check variables from the rhs of the equality

  for (v = equality->_negativePart.monomials().begin();
       v != equality->_negativePart.monomials().end();
       ++v)
    {
      simplifyingCoeffForEquality = _positivePart.monomials().coefficient(*v);
      if (!simplifyingCoeffForEquality)
	{
	  simplifyingCoeffForEquality = -_negativePart.monomials().coefficient(*v);
	};

      if (simplifyingCoeffForEquality) // this variable can be eliminated
	{
	  simplifyingCoeffForThis = equality->_negativePart.monomials().coefficient(*v);

	  applyRelation(equality,simplifyingCoeffForEquality,simplifyingCoeffForThis,simplified);

	  if (strict(simplified->category()))
	    {
	      // best possible outcome
	      return true;
	    };
	  
	  // need to count the number of variables
	  ulong numOfVariablesInSimplified = simplified->numberOfVariables();
	  if ((numOfVariablesInSimplified < origNumOfVariables) &&
	      (origNumOfVariables - numOfVariablesInSimplified > bestSimplificationStrength))
	    {
	      bestSimplificationStrength = origNumOfVariables - numOfVariablesInSimplified;
	      bestSimplifyingCoeffForEquality = -simplifyingCoeffForEquality;
	      bestSimplifyingCoeffForThis = simplifyingCoeffForThis;
	      SpecialisedComparison* tmp = simplified;
	      simplified = spareComparisonObject;
	      spareComparisonObject = tmp;
	    };
	};
    }; // for ..

  if (bestSimplificationStrength) // some simplification has been achieved
    {      
      SpecialisedComparison* tmp = simplified;
      simplified = spareComparisonObject;
      spareComparisonObject = tmp;
      return true;
    }
  else
    return false;
}; // bool WeightPolynomial::SpecialisedComparison::simplifyByEquality(..) const



bool 
WeightPolynomial::SpecialisedComparison::relaxAlwaysGreaterByInequality(const SpecialisedComparison* inequality,
									SpecialisedComparison*& simplified,
									SpecialisedComparison*& spareComparisonObject) const
{
  CALL("relaxAlwaysGreaterByInequality(..)");
  
  ASSERT(inequality->assumedRelation() == CanBeGreaterOrEqual);

  TermWeightType simplifyingCoeffForInequality;
  TermWeightType simplifyingCoeffForThis;
  TermWeightType bestSimplifyingCoeffForInequality;
  TermWeightType bestSimplifyingCoeffForThis;
  ulong bestSimplificationStrength = 0UL;

  ulong origNumOfVariables = numberOfVariables();

  static WeightPolynomial simplifiedPositive;
  static WeightPolynomial simplifiedNegative;

   // check variables from the lhs of the inequality
  const ulong* v;
  for (v = inequality->_positivePart.monomials().begin();
       v != inequality->_positivePart.monomials().end();
       ++v)
    {
      simplifyingCoeffForInequality = -_positivePart.monomials().coefficient(*v);

      if (simplifyingCoeffForInequality) // this variable can be eliminated
	{
	  simplifyingCoeffForThis = inequality->_positivePart.monomials().coefficient(*v);
	  applyRelation(inequality,simplifyingCoeffForInequality,simplifyingCoeffForThis,simplified);

	  if (strict(simplified->category()))
	    {
	      if (simplified->category() == BK::AlwaysGreater)
		{
		  return true; // best possible outcome
		}
	      else
		return false; // this inequality can not help us at all
	    }
	  else
	    {
	      // need to count the number of variables
	      ulong numOfVariablesInSimplified = simplified->numberOfVariables();
	      if ((numOfVariablesInSimplified < origNumOfVariables) &&
		  (origNumOfVariables - numOfVariablesInSimplified > bestSimplificationStrength))
		{
		  bestSimplificationStrength = origNumOfVariables - numOfVariablesInSimplified;
		  bestSimplifyingCoeffForInequality = -simplifyingCoeffForInequality;
		  bestSimplifyingCoeffForThis = simplifyingCoeffForThis;
		  SpecialisedComparison* tmp = simplified;
		  simplified = spareComparisonObject;
		  spareComparisonObject = tmp;
		};
	    };
	};
    }; // for ..
   
  
  // check variables from the rhs of the equality

  for (v = inequality->_negativePart.monomials().begin();
       v != inequality->_negativePart.monomials().end();
       ++v)
    {
      simplifyingCoeffForInequality = -_negativePart.monomials().coefficient(*v);

      if (simplifyingCoeffForInequality) // this variable can be eliminated
	{
	  simplifyingCoeffForThis = inequality->_negativePart.monomials().coefficient(*v);
	  ASSERT(simplifyingCoeffForThis);
	  
	  applyRelation(inequality,simplifyingCoeffForInequality,simplifyingCoeffForThis,simplified);

	  if (strict(simplified->category()))
	    {	      
	      if (simplified->category() == BK::AlwaysGreater)
		{
		  return true; // best possible outcome
		}
	      else
		return false; // this inequality can not help us at all
	    }
	  else
	    {
	      // need to count the number of variables
	      ulong numOfVariablesInSimplified = simplified->numberOfVariables();
	      if ((numOfVariablesInSimplified < origNumOfVariables) &&
		  (origNumOfVariables - numOfVariablesInSimplified > bestSimplificationStrength))
		{
		  bestSimplificationStrength = origNumOfVariables - numOfVariablesInSimplified;
		  bestSimplifyingCoeffForInequality = -simplifyingCoeffForInequality;
		  bestSimplifyingCoeffForThis = simplifyingCoeffForThis;
		  SpecialisedComparison* tmp = simplified;
		  simplified = spareComparisonObject;
		  spareComparisonObject = tmp;
		};
	    };
	};
    }; // for ..

  if (bestSimplificationStrength) // some simplification has been achieved
    {      
      SpecialisedComparison* tmp = simplified;
      simplified = spareComparisonObject;
      spareComparisonObject = tmp;
      return true;
    }
  else
    return false;

}; // bool WeightPolynomial::SpecialisedComparison::relaxAlwaysGreaterByInequality(..)



bool 
WeightPolynomial::SpecialisedComparison::relaxAlwaysLessByInequality(const SpecialisedComparison* inequality,
									SpecialisedComparison*& simplified,
									SpecialisedComparison*& spareComparisonObject) const
{
  CALL("relaxAlwaysLessByInequality(..)");
  
  ASSERT(inequality->assumedRelation() == CanBeGreaterOrEqual);

  TermWeightType simplifyingCoeffForInequality;
  TermWeightType simplifyingCoeffForThis;
  TermWeightType bestSimplifyingCoeffForInequality;
  TermWeightType bestSimplifyingCoeffForThis;
  ulong bestSimplificationStrength = 0UL;

  ulong origNumOfVariables = numberOfVariables();

  static WeightPolynomial simplifiedPositive;
  static WeightPolynomial simplifiedNegative;

   // check variables from the lhs of the inequality
  const ulong* v;
  for (v = inequality->_positivePart.monomials().begin();
       v != inequality->_positivePart.monomials().end();
       ++v)
    {
      simplifyingCoeffForInequality = _negativePart.monomials().coefficient(*v);

      if (simplifyingCoeffForInequality) // this variable can be eliminated
	{
	  simplifyingCoeffForThis = inequality->_positivePart.monomials().coefficient(*v);
	  applyRelation(inequality,simplifyingCoeffForInequality,simplifyingCoeffForThis,simplified);

	  if (strict(simplified->category()))
	    {
	      if (simplified->category() == BK::AlwaysLess)
		{
		  return true; // best possible outcome
		}
	      else
		return false; // this inequality can not help us at all
	    }
	  else
	    {
	      // need to count the number of variables
	      ulong numOfVariablesInSimplified = simplified->numberOfVariables();
	      if ((numOfVariablesInSimplified < origNumOfVariables) &&
		  (origNumOfVariables - numOfVariablesInSimplified > bestSimplificationStrength))
		{
		  bestSimplificationStrength = origNumOfVariables - numOfVariablesInSimplified;
		  bestSimplifyingCoeffForInequality = simplifyingCoeffForInequality;
		  bestSimplifyingCoeffForThis = simplifyingCoeffForThis;
		  SpecialisedComparison* tmp = simplified;
		  simplified = spareComparisonObject;
		  spareComparisonObject = tmp;
		};
	    };
	};
    }; // for ..
   
  
  // check variables from the rhs of the equality

  for (v = inequality->_negativePart.monomials().begin();
       v != inequality->_negativePart.monomials().end();
       ++v)
    {
      simplifyingCoeffForInequality = _positivePart.monomials().coefficient(*v);

      if (simplifyingCoeffForInequality) // this variable can be eliminated
	{
	  simplifyingCoeffForThis = inequality->_negativePart.monomials().coefficient(*v);
	  ASSERT(simplifyingCoeffForThis);
	  
	  applyRelation(inequality,simplifyingCoeffForInequality,simplifyingCoeffForThis,simplified);

	  if (strict(simplified->category()))
	    {	      
	      if (simplified->category() == BK::AlwaysLess)
		{
		  return true; // best possible outcome
		}
	      else
		return false; // this inequality can not help us at all
	    }
	  else
	    {
	      // need to count the number of variables
	      ulong numOfVariablesInSimplified = simplified->numberOfVariables();
	      if ((numOfVariablesInSimplified < origNumOfVariables) &&
		  (origNumOfVariables - numOfVariablesInSimplified > bestSimplificationStrength))
		{
		  bestSimplificationStrength = origNumOfVariables - numOfVariablesInSimplified;
		  bestSimplifyingCoeffForInequality = simplifyingCoeffForInequality;
		  bestSimplifyingCoeffForThis = simplifyingCoeffForThis;
		  SpecialisedComparison* tmp = simplified;
		  simplified = spareComparisonObject;
		  spareComparisonObject = tmp;
		};
	    };
	};
    }; // for ..

  if (bestSimplificationStrength) // some simplification has been achieved
    {      
      SpecialisedComparison* tmp = simplified;
      simplified = spareComparisonObject;
      spareComparisonObject = tmp;
      return true;
    }
  else
    return false;

}; // bool WeightPolynomial::SpecialisedComparison::relaxAlwaysLessByInequality(..)




void 
WeightPolynomial::SpecialisedComparison::applyRelation(const SpecialisedComparison* relation,
						       const TermWeightType& coeffForRelation,
						       const TermWeightType& coeffForThis,
						       SpecialisedComparison* result) const
{
  CALL("applyRelation(..) const");
  
  ASSERT(coeffForRelation != (TermWeightType)0);
  ASSERT(coeffForThis > (TermWeightType)0);

  //DF; cout << " **** APP EQ " <<  coeffForRelation << " # " << (*relation) << "\n";
  //DF; cout << " **** TO " << coeffForThis  << " # " << (*this) << "\n";
 
  
  static WeightPolynomial resultPositive;
  static WeightPolynomial resultNegative;

  resultPositive.reset();
  resultNegative.reset();
  
  resultPositive.add(coeffForThis,_positivePart);
  resultNegative.add(coeffForThis,_negativePart);
  
  if (coeffForRelation > (TermWeightType)0)
    {
      resultPositive.add(coeffForRelation,relation->_positivePart);
      resultNegative.add(coeffForRelation,relation->_negativePart);
    }
  else
    {
      resultPositive.add(-coeffForRelation,relation->_negativePart);
      resultNegative.add(-coeffForRelation,relation->_positivePart);
    };

  result->compare(resultPositive,resultNegative);

  //DF; cout << " **** RESULT " << (*result) << "\n";
}; // void WeightPolynomial::SpecialisedComparison::applyRelation(..) const



void 
WeightPolynomial::SpecialisedComparison::compare(const WeightPolynomial& weight1,
						 const WeightPolynomial& weight2)
{
  CALL("compare(..)");
  _positivePart.reset();
  _negativePart.reset();
  const ulong* endOfVars = weight1.monomials().end();
  const ulong* v;
  for (v = weight1.monomials().begin();
       v != endOfVars;
       ++v)
    {
      TermWeightType coeffDiff = weight1.coefficient(*v) - weight2.coefficient(*v);
      if (coeffDiff > (TermWeightType)0)
	{
	  _positivePart.add(coeffDiff,*v);
	}
      else
	if (coeffDiff < (TermWeightType)0)
	  {
	    _negativePart.add(-coeffDiff,*v);
	  };
    };

  endOfVars = weight2.monomials().end();
  for (v = weight2.monomials().begin();
       v != endOfVars;
       ++v)
    {
      if (!weight1.containsVariable(*v))
	{
	  _negativePart.add(weight2.coefficient(*v),*v);
	};
    };

  if (weight1.freeMember() > weight2.freeMember())
    {
      _positivePart.add(weight1.freeMember() - weight2.freeMember());
    }
  else
    {
      _negativePart.add(weight2.freeMember() - weight1.freeMember());
    };

  // summarise
  if (_positivePart.isConstant())
    { 
      if (_negativePart.isConstant())
	{ 
	  if (_positivePart.freeMember() == _negativePart.freeMember())
	    {
	      ASSERT(!_positivePart.freeMember());
	      _category = AlwaysEqual;
	    }
	  else
	    if (_positivePart.freeMember())
	      {
		ASSERT(!_negativePart.freeMember());
		_category = AlwaysGreater;
	      }
	    else
	      {
		ASSERT(_negativePart.freeMember());
		_category = AlwaysLess;
	      };
	}
      else
	{
	  TermWeightType negMinVal = _negativePart.minimalValue();
	  if (_positivePart.freeMember() < negMinVal)
	    {
	      _category = AlwaysLess;
	    }
	  else
	    if (_positivePart.freeMember() == negMinVal)
	      {
		_category = CanBeLessOrEqual;
	      }
	    else
	      _category = VolatileButNearlyAlwaysLess;
	};
    }
  else
    if (_negativePart.isConstant())
      {    
	TermWeightType posMinVal = _positivePart.minimalValue();
	if (posMinVal > _negativePart.freeMember())
	  {
	    _category = AlwaysGreater;
	  }
	else
	  if (posMinVal == _negativePart.freeMember())
	    {
	      _category = CanBeGreaterOrEqual;
	    }
	  else
	    _category = VolatileButNearlyAlwaysGreater;
      }
    else
      _category = AbsolutelyVolatile;

  factoriseCoefficients();

}; // void WeightPolynomial::SpecialisedComparison::compare(..)



void WeightPolynomial::SpecialisedComparison::factoriseCoefficients()
{
  CALL("factoriseCoefficients()");
  
  TermWeightType gcdPos = _positivePart.greatestCommonDivisorOfCoefficients();
  TermWeightType gcdNeg = _negativePart.greatestCommonDivisorOfCoefficients();
  TermWeightType gcd = Math::greatestCommonDivisor(gcdPos,gcdNeg);
  if (gcd != (TermWeightType)1)
    {
      _positivePart.divideBy(gcd);
      _negativePart.divideBy(gcd);
    };
  
}; // void WeightPolynomial::SpecialisedComparison::factoriseCoefficients()




bool WeightPolynomial::SpecialisedComparison::compileGreaterOrEqual(const FunctionComparison& cat,
								    const WeightPolynomial& posPart,
								    const WeightPolynomial& negPart,
								    Command*& code,ulong maxCodeSize)
{
  CALL("compileGreaterOrEqual(const FunctionComparison& cat,..)");
  ASSERT(maxCodeSize);
  Command* start = code;
  const ulong* v;
  DOP(v = 0);
  switch (cat)
    {
    case FailureToCompare:
      code->setTag(Command::RetFailureToCompare);
      ++code;
      return true;
    case AlwaysLess:
      code->setTag(Command::RetFailureToCompare);
      ++code;
      return true;
    case AlwaysEqual:
      code->setTag(Command::RetAlwaysEqual);
      ++code;
      return true;
    case AlwaysGreater:
      code->setTag(Command::RetAlwaysGreater);
      ++code;
      return true;
      
    case CanBeLessOrEqual:
      {
	ASSERT((!negPart.isConstant()) && (!negPart.freeMember()));
	ASSERT(posPart.isConstant() && posPart.freeMember());	
	v = negPart.monomials().begin();

	// compile all the monomials except the last one
	while (v != negPart.monomials().last())
	  {
	    if ((code - start) + 1 == (long)maxCodeSize)
	      {
		code->setTag(Command::RetFailureToCompare);
		++code;
		return false;
	      };
	    code->setTag(Command::IfXGr1RetFail_IfNotGrndRetFail);
	    code->setVar(*v);
	    ++v;
	    ++code;
	  };
	
	// compile the last monomial
	ASSERT(v == negPart.monomials().last());
	if ((code - start) + 1 == (long)maxCodeSize)
	  {
	    code->setTag(Command::RetFailureToCompare);
	    ++code;
	    return false;
	  };
	code->setTag(Command::IfXGr1RetFail_IfGrndRetAlwEq_RetFail);
	code->setVar(*v);
	++code;
	ASSERT((code - start) <= (long)maxCodeSize);
	return true;
      }; // case CanBeLessOrEqual

    case CanBeGreaterOrEqual:
      {
	ASSERT((!posPart.isConstant()) && (!posPart.freeMember()));
	ASSERT(negPart.isConstant() && negPart.freeMember());
	v = posPart.monomials().begin();

	if (posPart.numberOfVariables() == 1UL)
	  {
	    // compile the only monomial
	    code->setTag(Command::IfXGr1RetAlwGr_IfGrndRetAlwEq_RetCanBeGrOrEq);
	    code->setVar(*v);
	    ++code;
	    return true;	    
	  }
	else // posPart.numberOfVariables() > 1UL
 	  {
	    if ((code - start) + 1 == (long)maxCodeSize)
	      {
		code->setTag(Command::RetCanBeGreaterOrEqual);
		++code;
		return false;
	      };
	    // compile the first monomial
	    code->setTag(Command::IfXGr1RetAlwGr_GrndToGRND);
	    code->setVar(*v);
	    ++code;
	    
	    // compile the monomials between the first one and the last one
	    ++v;
	    while (v != posPart.monomials().last())
	      {
		if ((code - start) + 1 == (long)maxCodeSize)
		  {
		    code->setTag(Command::RetCanBeGreaterOrEqual);
		    ++code;
		    return false;
		  };
		code->setTag(Command::IfXGr1RetAlwGr_UpdateGRND);
		code->setVar(*v);
		++v;
		++code;
	      };

	    // compile the last monomial
	    ASSERT(v == posPart.monomials().last());
	    if ((code - start) + 1 == (long)maxCodeSize)
	      {
		code->setTag(Command::RetCanBeGreaterOrEqual);
		++code;
		return false;
	      };
	    code->setTag(Command::IfXGr1RetAlwGr_UpdateGRND_IfGRNDRetAlwEq_RetCanBeGrOrEq);
	    code->setVar(*v);
	    ++code;
	    ASSERT((code - start) <= (long)maxCodeSize);
	    return true;
	  };
      }; // case CanBeGreaterOrEqual

    case VolatileButNearlyAlwaysLess:
      {
	ASSERT((!negPart.isConstant()) && (!negPart.freeMember()));
	ASSERT(posPart.isConstant() && posPart.freeMember());	
	v = negPart.monomials().begin();
	if (negPart.numberOfVariables() == 1UL)
	  {
	    // compile the only monomial
	    if (negPart.coefficient(*v) == (TermWeightType)1)
	      {
		code->setTag(Command::IfNotGrndRetFail_IfXGrWRetFail_IfXLeWRetAlwGr_RetAlwEq);
	      }
	    else
	      {
		code->setTag(Command::IfNotGrndRetFail_IfCXGrWRetFail_IfCXLeWRetAlwGr_RetAlwEq);
		code->setCoefficient(negPart.coefficient(*v));
	      };
	    code->setVar(*v);
	    code->setWeight(posPart.freeMember());
	    ++code;
	    return true;	
	  }
	else // negPart.numberOfVariables() > 1UL
	  {
	    if ((code - start) + 1 == (long)maxCodeSize)
	      {
		code->setTag(Command::RetFailureToCompare);
		++code;
		return false;
	      };
	    // compile the first monomial
	    if (negPart.coefficient(*v) == (TermWeightType)1)
	      {
		code->setTag(Command::IfNotGrndRetFail_WMinusXToACC_IfACCLe0RetFail);
	      }
	    else
	      {
		code->setTag(Command::IfNotGrndRetFail_WMinusCXToACC_IfACCLe0RetFail);
		code->setCoefficient(negPart.coefficient(*v));
	      };
	    code->setVar(*v);
	    code->setWeight(posPart.freeMember() - (negPart.minimalValue() - negPart.coefficient(*v)));
	    ++code;

	    // compile the monomials between the first one and the last one

	  vol_but_nearly_alw_less_all_monomials_except_first:
	    ++v;
	    while (v != negPart.monomials().last())
	      {
		if ((code - start) + 1 == (long)maxCodeSize)
		  {
		    code->setTag(Command::RetFailureToCompare);
		    ++code;
		    return false;
		  };
		if (negPart.coefficient(*v) == (TermWeightType)1)
		  {
		    code->setTag(Command::IfNotGrndRetFail_ACCMinusXPlus1ToACC_IfACCLe0RetFail);
		  }
		else
		  {
		    code->setTag(Command::IfNotGrndRetFail_ACCMinusCXPlusCToACC_IfACCLe0RetFail);
		    code->setCoefficient(negPart.coefficient(*v));
		  };
		code->setVar(*v);
		++v;
		++code;
	      };

	    // compile the last monomial

	    ASSERT(v == negPart.monomials().last());
	    if ((code - start) + 1 == (long)maxCodeSize)
	      {
		code->setTag(Command::RetFailureToCompare);
		++code;
		return false;
	      };
	    
	    if (negPart.coefficient(*v) == (TermWeightType)1)
	      {
		code->setTag(Command::IfNotGrndRetFail_ACCMinusXPlus1ToACC_IfACCLe0RetFail_IfACCEq0RetAlwEq_RetAlwGr);
	      }
	    else
	      {
		code->setTag(Command::IfNotGrndRetFail_ACCMinusCXPlusCToACC_IfACCLe0RetFail_IfACCEq0RetAlwEq_RetAlwGr);
		code->setCoefficient(negPart.coefficient(*v));
	      };
	    code->setVar(*v);
	    ++code;
	    ASSERT((code - start) <= (long)maxCodeSize);
	    return true;
	  };
      }; // case VolatileButNearlyAlwaysLess


    case VolatileButNearlyAlwaysGreater:
      {
	ASSERT((!posPart.isConstant()) && (!posPart.freeMember()));
	ASSERT(negPart.isConstant() && negPart.freeMember());
	v = posPart.monomials().begin();
	if (posPart.numberOfVariables() == 1UL)
	  {
	    // compile the only monomial
	    if (posPart.coefficient(*v) == (TermWeightType)1)
	      {
		code->setTag(Command::IfXGrWRetAlwGr_IfXLeWRetFail_IfGrndRetAlwEq_RetCanBeGrOrEq);
	      }
	    else
	      {
		code->setTag(Command::IfCXGrWRetAlwGr_IfCXLeWRetFail_IfGrndRetAlwEq_RetCanBeGrOrEq);
		code->setCoefficient(posPart.coefficient(*v));
	      };
	    code->setVar(*(posPart.monomials().begin()));
	    code->setWeight(negPart.freeMember());
	    ++code;
	    return true;
	  }
	else // posPart.numberOfVariables() > 1UL
 	  {
	    if ((code - start) + 1 == (long)maxCodeSize)
	      {
		code->setTag(Command::RetFailureToCompare);
		++code;
		return false;
	      };
	    // compile the first monomial
	    if (posPart.coefficient(*v) == (TermWeightType)1)
	      {
		code->setTag(Command::WMinusXToACC_IfACCLe0RetAlwGr_GrndToGRND);
	      }
	    else
	      {
		code->setTag(Command::WMinusCXToACC_IfACCLe0RetAlwGr_GrndToGRND);
		code->setCoefficient(posPart.coefficient(*v));
	      };
	    code->setVar(*v);
	    code->setWeight(negPart.freeMember() - (posPart.minimalValue() - posPart.coefficient(*v)));
	    ++code;
	    
	    // compile the monomials between the first one and the last one
	  vol_but_nearly_alw_greater_all_monomials_except_first:
	    ++v;
	    while (v != posPart.monomials().last())
	      {
		if ((code - start) + 1 == (long)maxCodeSize)
		  {
		    code->setTag(Command::RetFailureToCompare);
		    ++code;
		    return false;
		  };

		if (posPart.coefficient(*v) == (TermWeightType)1)
		  {
		    code->setTag(Command::ACCMinusXPlus1ToACC_IfACCLe0RetAlwGr_UpdateGRND);
		  }
		else
		  {
		    code->setTag(Command::ACCMinusCXPlusCToACC_IfACCLe0RetAlwGr_UpdateGRND);
		    code->setCoefficient(posPart.coefficient(*v));
		  };
		
		code->setVar(*v);
		++v;
		++code;
	      };

	    // compile the last monomial
	    ASSERT(v == posPart.monomials().last());
	    if ((code - start) + 1 == (long)maxCodeSize)
	      {
		code->setTag(Command::RetFailureToCompare);
		++code;
		return false;
	      };

	    if (posPart.coefficient(*v) == (TermWeightType)1)
	      {
		code->setTag(Command::IfXGrACCPlus1RetAlwGr_IfXLeACCPlus1RetFail_UpdateGRND_IfGRNDRetAlwEq_RetCanBeGrOrEq);
	      }
	    else
	      {
		code->setTag(Command::IfCXGrACCPlusCRetAlwGr_IfCXLeACCPlusCRetFail_UpdateGRND_IfGRNDRetAlwEq_RetCanBeGrOrEq);
		code->setCoefficient(posPart.coefficient(*v));
	      };
	    code->setVar(*v);
	    ++code;
	    ASSERT((code - start) <= (long)maxCodeSize);
	    return true;
	  };

      }; // case VolatileButNearlyAlwaysGreater

    case AbsolutelyVolatile: 
      {	
	ASSERT((!posPart.isConstant()) && (!negPart.isConstant()));
	ASSERT((!posPart.freeMember()) || (!negPart.freeMember()));
	Command* commandWithLabel;
	DOP(commandWithLabel = 0);

	// decide which part to compile first
	if (posPart.numberOfVariables() <= negPart.numberOfVariables())
	  {
	    // compile posPart first

	    TermWeightType constPart = posPart.freeMember() - negPart.minimalValue();
	    v = posPart.monomials().begin();
	    if (posPart.numberOfVariables() == 1UL)
	      {
		// compile the only monomial together with the constant
		if ((code - start) + 1 == (long)maxCodeSize)
		  {
		    code->setTag(Command::RetFailureToCompare);
		    ++code;
		    return false;
		  };
		if (posPart.coefficient(*v) == (TermWeightType)1)
		  {
		    code->setTag(Command::XPlusWToPOSINST_IfConstPOSINSAndPOSINSTLe0RetFail_IfConstPOSINSTGotoL);
		  }
		else
		  {
		    code->setTag(Command::CXPlusWToPOSINST_IfConstPOSINSAndPOSINSTLe0RetFail_IfConstPOSINSTGotoL);
		    code->setCoefficient(posPart.coefficient(*v));
		  };
		code->setWeight(constPart);
		code->setVar(*v);

		code->setLabel(code + 1); 
		// in case there is not enough space for the code
		// optimised for ground posPart instances
		
		commandWithLabel = code; 
		++code;
	      }
	    else // posPart.numberOfVariables() > 1UL
	      {		
		// compile the first monomial together with the constant
		if ((code - start) + 1 == (long)maxCodeSize)
		  {
		    code->setTag(Command::RetFailureToCompare);
		    ++code;
		    return false;
		  };
		if (posPart.coefficient(*v) == (TermWeightType)1)
		  {
		    code->setTag(Command::XPlusWToPOSINST);
		  }
		else
		  {
		    code->setTag(Command::CXPlusWToPOSINST);
		    code->setCoefficient(posPart.coefficient(*v));
		  };
		code->setVar(*v);
		code->setWeight(constPart);
		++code;

		// compile the monomials between the first one and the last one
		      
		++v;
		while (v != posPart.monomials().last())
		  { 		    
		    if ((code - start) + 1 == (long)maxCodeSize)
		      {
			code->setTag(Command::RetFailureToCompare);
			++code;
			return false;
		      };
		    if (posPart.coefficient(*v) == (TermWeightType)1)
		      {
			code->setTag(Command::POSINSTPlusXToPOSINST);
		      }
		    else
		      {			
			code->setTag(Command::POSINSTPlusCXToPOSINST);
			code->setCoefficient(posPart.coefficient(*v));
		      };

		    code->setVar(*v);
		    ++v;
		    ++code;
		  };  

		// compile the last monomial
		ASSERT(v == posPart.monomials().last());
		if ((code - start) + 1 == (long)maxCodeSize)
		  {
		    code->setTag(Command::RetFailureToCompare);
		    ++code;
		    return false;
		  };
		if (posPart.coefficient(*v) == (TermWeightType)1)
		  {
		    code->setTag(Command::POSINSTPlusXToPOSINST_IfConstPOSINSTAndPOSINSTLe0RetFail_IfConstPOSINSTGotoL);
		  }
		else
		  {			
		    code->setTag(Command::POSINSTPlusCXToPOSINST_IfConstPOSINSTAndPOSINSTLe0RetFail_IfConstPOSINSTGotoL);
		    code->setCoefficient(posPart.coefficient(*v));
		  };
		code->setVar(*v);
		
		// in case there is not enough space for the code
		// optimised for ground posPart instances
		code->setLabel(code + 1); 
		
		commandWithLabel = code; 
		++code;
	      }; // else // posPart.numberOfVariables() > 1UL

	    ASSERT(code - start == (long)posPart.numberOfVariables());
	    // compile negPart now

	    // Assume that the positive part instance is nonconstant here
	    v = negPart.monomials().begin();

	    if (negPart.numberOfVariables() == 1UL)
	      {
		// compile the only monomial 
		if ((code - start) + 1 == (long)maxCodeSize)
		  {
		    code->setTag(Command::RetFailureToCompare);
		    ++code;
		    return false;
		  };
		if (negPart.coefficient(*v) == (TermWeightType)1)
		  {
		    code->setTag(Command::XMinus1ToNEGINSTRetCompForGrOrEqPOSINSTvNEGINST);
		  }
		else
		  {
		    code->setTag(Command::CXMinusCToNEGINSTRetCompForGrOrEqPOSINSTvNEGINST);
		    code->setCoefficient(negPart.coefficient(*v));
		  }; 
		code->setVar(*v);
		++code;
	      }
	    else // negPart.numberOfVariables() > 1UL
	      {
		// compile the first monomial 
		if ((code - start) + 1 == (long)maxCodeSize)
		  {
		    code->setTag(Command::RetFailureToCompare);
		    ++code;
		    return false;
		  };
		if (negPart.coefficient(*v) == (TermWeightType)1)
		  {
		    code->setTag(Command::XMinus1ToNEGINST_IfCompForGrOrEqPOSINSTvNEGINSTEqFailRetFail);
		  }
		else
		  {
		    code->setTag(Command::CXMinusCToNEGINST_IfCompForGrOrEqPOSINSTvNEGINSTEqFailRetFail);
		    code->setCoefficient(negPart.coefficient(*v));
		  };
		code->setVar(*v);
		++code;

		// compile the monomials between the first one and the last one
		++v;
		while (v != negPart.monomials().last())
		  { 		    
		    if ((code - start) + 1 == (long)maxCodeSize)
		      {
			code->setTag(Command::RetFailureToCompare);
			++code;
			return false;
		      };
		    if (negPart.coefficient(*v) == (TermWeightType)1)
		      {
			code->setTag(Command::NEGINSTPlusXMinus1ToNEGINST_IfCompForGrOrEqPOSINSTvNEGINSTEqFailRetFail);
		      }
		    else
		      {			
			code->setTag(Command::NEGINSTPlusCXMinusCToNEGINST_IfCompForGrOrEqPOSINSTvNEGINSTEqFailRetFail);
			code->setCoefficient(negPart.coefficient(*v));
		      };

		    code->setVar(*v);
		    ++v;
		    ++code;
		  }; 

		// compile the last monomial  
		ASSERT(v == negPart.monomials().last());  
				    
		if ((code - start) + 1 == (long)maxCodeSize)
		  {
		    code->setTag(Command::RetFailureToCompare);
		    ++code;
		    return false;
		  };
		if (negPart.coefficient(*v) == (TermWeightType)1)
		  {
		    code->setTag(Command::NEGINSTPlusXMinus1ToNEGINST_RetCompForGrOrEqPOSINSTvNEGINST);
		  }
		else
		  {			
		    code->setTag(Command::NEGINSTPlusCXMinusCToNEGINST_RetCompForGrOrEqPOSINSTvNEGINST);
		    code->setCoefficient(negPart.coefficient(*v));
		  };
		code->setVar(*v);
		++code;
	      }; // else // negPart.numberOfVariables() > 1UL

	    ASSERT(code - start == (long)(posPart.numberOfVariables() + negPart.numberOfVariables()));
	    ASSERT((code - 1)->isTerminal());

	    // Now assume that the positive part instance is constant and positive
   
	    commandWithLabel->setLabel(code);
	    v = negPart.monomials().begin();
	    if (negPart.numberOfVariables() == 1UL)
	      {
		// compile the only monomial				    
		if ((code - start) + 1 == (long)maxCodeSize)
		  {
		    code->setTag(Command::RetFailureToCompare);
		    ++code;
		    return false;
		  };
		if (negPart.coefficient(*v) == (TermWeightType)1)
		  {
		    code->setTag(Command::IfNotGrndRetFail_IfXMinus1GrPOSINSTRetFail_IfXMinus1LePOSINSTRetAlwGr_RetAlwEq);		    
		  }
		else
		  {
		    code->setTag(Command::IfNotGrndRetFail_IfCXMinusCGrPOSINSTRetFail_IfCXMinusCLePOSINSTRetAlwGr_RetAlwEq);
		    code->setCoefficient(negPart.coefficient(*v));
		  };
		code->setVar(*v);
		++code;
		return true;
	      }
	    else // negPart.numberOfVariables() > 1UL
	      {
		// compile the first monimial			    
		if ((code - start) + 1 == (long)maxCodeSize)
		  {
		    code->setTag(Command::RetFailureToCompare);
		    ++code;
		    return false;
		  };
		
		if (negPart.coefficient(*v) == (TermWeightType)1)
		  {
		    code->setTag(Command::IfNotGrndRetFail_POSINSTPlus1MinusXToACC_IfACCLe0RetFail);		    
		  }
		else
		  {
		    code->setTag(Command::IfNotGrndRetFail_POSINSTPlusCMinusCXToACC_IfACCLe0RetFail);
		    code->setCoefficient(negPart.coefficient(*v));
		  };
		code->setVar(*v);
		++code;
		goto vol_but_nearly_alw_less_all_monomials_except_first;		
	      };
	    ICP("ICP10");
	  }
	else // compile negPart first
	  {
	    	    
	    	    
	    TermWeightType constPart = negPart.freeMember() - posPart.minimalValue();
	    v = negPart.monomials().begin();
	    if (negPart.numberOfVariables() == 1UL)
	      {
		// compile the only monomial together with the constant
		if ((code - start) + 1 == (long)maxCodeSize)
		  {
		    code->setTag(Command::RetFailureToCompare);
		    ++code;
		    return false;
		  };
		if (negPart.coefficient(*v) == (TermWeightType)1)
		  {
		    code->setTag(Command::XPlusWToNEGINST_IfConstNEGINSTAndNEGINSTLe0RetAlwGr_IfConstNEGINSTGotoL);
		    code->setWeight(constPart);		     
		  }
		else 
		  {		
		    code->setTag(Command::CXPlusWToNEGINST_IfConstNEGINSTAndNEGINSTLe0RetAlwGr_IfConstNEGINSTGotoL);
		    code->setWeight(constPart);
		    code->setCoefficient(negPart.coefficient(*v));
		  };
		code->setVar(*v);
		  
		code->setLabel(code + 1); 
		// in case there is not enough space for the code
		// optimised for ground posPart instances
		
		commandWithLabel = code; 
		++code;
	      }
	    else
	      {
		// compile the first monomial together with the constant
		
		if ((code - start) + 1 == (long)maxCodeSize)
		  {
		    code->setTag(Command::RetFailureToCompare);
		    ++code;
		    return false;
		  };
		if (negPart.coefficient(*v) == (TermWeightType)1)
		  {
		    code->setTag(Command::XPlusWToNEGINST);
		    code->setWeight(constPart);
		  }
		else 
		  {		   
		    code->setTag(Command::CXPlusWToNEGINST);
		    code->setWeight(constPart);
		    code->setCoefficient(negPart.coefficient(*v));
		  };
		code->setVar(*v);
		++code;

		// compile the monomials between the first one and the last one
		++v;
		while (v != negPart.monomials().last())
		  { 
		    
		    if ((code - start) + 1 == (long)maxCodeSize)
		      {
			code->setTag(Command::RetFailureToCompare);
			++code;
			return false;
		      };
		    if (negPart.coefficient(*v) == (TermWeightType)1)
		      {
			code->setTag(Command::NEGINSTPlusXToNEGINST);
		      }
		    else
		      {			
			code->setTag(Command::NEGINSTPlusCXToNEGINST);
			code->setCoefficient(negPart.coefficient(*v));
		      };
		    code->setVar(*v);
		    ++v;
		    ++code;
		  }; 

		// compile the last monomial
		ASSERT(v == negPart.monomials().last());  
		if ((code - start) + 1 == (long)maxCodeSize)
		  {
		    code->setTag(Command::RetFailureToCompare);
		    ++code;
		    return false;
		  };

		if (negPart.coefficient(*v) == (TermWeightType)1)
		  {
		    code->setTag(Command::NEGINSTPlusXToNEGINST_IfConstNEGINSTAndNEGINSTLe0RetAlwGr_IfConstNEGINSTGotoL);
		  }
		else
		  {			
		    code->setTag(Command::NEGINSTPlusCXToNEGINST_IfConstNEGINSTAndNEGINSTLe0RetAlwGr_IfConstNEGINSTGotoL);
		    code->setCoefficient(negPart.coefficient(*v));
		  };
		code->setVar(*v);
		  
		code->setLabel(code + 1); 
		// in case there is not enough space for the code
		// optimised for ground posPart instances
		
		commandWithLabel = code; 
		++code;
	      };


	    ASSERT(code - start == (long)negPart.numberOfVariables());
	    // compile posPart now

	    // Assume that the negative part instance is nonconstant here
	    ASSERT(posPart.numberOfVariables() > 1UL); // otherwise posPart would be compiled first

	    // compile the first monomial
	    if ((code - start) + 1 == (long)maxCodeSize)
	      {
		code->setTag(Command::RetFailureToCompare);
		++code;
		return false;
	      };
	    v = posPart.monomials().begin();
	    if (posPart.coefficient(*v) == (TermWeightType)1)
	      {
		code->setTag(Command::XMinus1ToPOSINST_IfCompForGrOrEqPOSINSTvNEGINSTEqAlwGrRetAlwGr);
	      }
	    else
	      {
		code->setTag(Command::CXMinusCToPOSINST_IfCompForGrOrEqPOSINSTvNEGINSTEqAlwGrRetAlwGr);
		code->setCoefficient(posPart.coefficient(*v));
	      };
	    
	    code->setVar(*v);
	    ++code;

	    // compile the monomials between the first one and the last one
	    ++v;
	    while (v != posPart.monomials().last())
	      { 
		if ((code - start) + 1 == (long)maxCodeSize)
		  {
		    code->setTag(Command::RetFailureToCompare);
		    ++code;
		    return false;
		  };
		if (posPart.coefficient(*v) == (TermWeightType)1)
		  {
		    code->setTag(Command::POSINSTPlusXMinus1ToPOSINST_IfCompForGrOrEqPOSINSTvNEGINSTEqAlwGrRetAlwGr);
		  }
		else
		  {			
		    code->setTag(Command::POSINSTPlusCXMinusCToPOSINST_IfCompForGrOrEqPOSINSTvNEGINSTEqAlwGrRetAlwGr);
		    code->setCoefficient(posPart.coefficient(*v));
		  };
		code->setVar(*v);
		++v;
		++code;
	      }; 
		
	    // compile the last monomial		
	    ASSERT(v == posPart.monomials().last()); 
	    if ((code - start) + 1 == (long)maxCodeSize)
	      {
		code->setTag(Command::RetFailureToCompare);
		++code;
		return false;
	      };
	    if (posPart.coefficient(*v) == (TermWeightType)1)
	      {
		code->setTag(Command::POSINSTPlusXMinus1ToPOSINST_RetCompForGrOrEqPOSINSTvNEGINST);
	      }
	    else
	      {			
		code->setTag(Command::POSINSTPlusCXMinusCToPOSINST_RetCompForGrOrEqPOSINSTvNEGINST);
		code->setCoefficient(posPart.coefficient(*v));
	      };
	    code->setVar(*v);
	    ++code; 
	    

	    ASSERT(code - start == (long)(posPart.numberOfVariables() + negPart.numberOfVariables()));
	    ASSERT((code - 1)->isTerminal());

	    // Now assume that the negative part instance is constant
	    ASSERT(posPart.numberOfVariables() > 1UL); // otherwise posPart would be compiled first

	    if ((code - start) + 1 == (long)maxCodeSize)
	      {
		code->setTag(Command::RetFailureToCompare);
		++code;
		return false;
	      };

	    commandWithLabel->setLabel(code);
	    // compile the first monomial
	    v = posPart.monomials().begin();
	    if (posPart.coefficient(*v) == (TermWeightType)1)
	      {
		code->setTag(Command::NEGINSTPlus1MinusXToACC_IfACCLe0RetAlwGr_GrndToGRND);
	      }
	    else
	      {			
		code->setTag(Command::NEGINSTPlusCMinusCXToACC_IfACCLe0RetAlwGr_GrndToGRND);
		code->setCoefficient(posPart.coefficient(*v));
	      };
	    code->setVar(*v);
	    ++code; 
	    goto vol_but_nearly_alw_greater_all_monomials_except_first;
	  };
	ICP("ICP20");
      }; // case AbsolutelyVolatile
    };

  ICP("END");

#if (defined _SUPPRESS_WARNINGS_) || (defined DEBUG_NAMESPACE)
  return false;
#endif

}; // bool WeightPolynomial::SpecialisedComparison::compileGreaterOrEqual(const FunctionComparison& cat,..)



//===================================================================
