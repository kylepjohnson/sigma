
//
// File:         BuiltInFloatingPointArithmetic.cpp
// Description:  Built-in floating point arithmetic.  
// Created:      Jul 06, 2002, Alexandre Riazanov, riazanov@cs.man.ac.uk
//==================================================================
#include <cstring>
#include "GlobAlloc.hpp"
#include "BuiltInFloatingPointArithmetic.hpp"
#include "Clause.hpp"
#include "tmp_literal.hpp"
#include "Flatterm.hpp"
#include "DestructionMode.hpp"
//===================================================================
#undef DEBUG_NAMESPACE
#ifdef DEBUG_BUILT_IN_FLOATING_POINT_ARITHMETIC
 #define DEBUG_NAMESPACE "BuiltInFloatingPointArithmetic"
#endif
#include "debugMacros.hpp"
//===================================================================

using namespace BK;
using namespace VK;

BuiltInFloatingPointArithmetic::BuiltInFloatingPointArithmetic(const char* name,
							       Clause::NewNumber* clauseNumberGenerator) : 
  BuiltInTheory(name),
  _clauseNumberGenerator(clauseNumberGenerator),
  _fact(0),
#ifdef NO_DEBUG
  _symbolInterpretation(static_cast<NativeSymbolDescriptor*>(0),0)
#else
  _symbolInterpretation(static_cast<NativeSymbolDescriptor*>(0),"BuiltInFloatingPointArithmetic::_symbolInterpretation")
#endif
{
  CALL("constructor BuiltInFloatingPointArithmetic(const char* name,Clause::NewNumber* clauseNumberGenerator)");

  _binaryPlusDesc.setNativeName("binary_plus");  
  _binaryPlusDesc.setArity(2UL);
  _binaryPlusDesc.setInterpretation(binaryPlusInterpretation);

  _unaryPlusDesc.setNativeName("unary_plus");  
  _unaryPlusDesc.setArity(1UL);
  _unaryPlusDesc.setInterpretation(unaryPlusInterpretation);

  _binaryMinusDesc.setNativeName("binary_minus");
  _binaryMinusDesc.setArity(2UL);
  _binaryMinusDesc.setInterpretation(binaryMinusInterpretation);

  _unaryMinusDesc.setNativeName("unary_minus");
  _unaryMinusDesc.setArity(1UL);
  _unaryMinusDesc.setInterpretation(unaryMinusInterpretation);

  _multiplyDesc.setNativeName("multiply");
  _multiplyDesc.setArity(2UL);
  _multiplyDesc.setInterpretation(multiplyInterpretation);

  _divideDesc.setNativeName("divide"); 
  _divideDesc.setArity(2UL); 
  _divideDesc.setInterpretation(divideInterpretation);

  _min2Desc.setNativeName("min2"); 
  _min2Desc.setArity(2UL); 
  _min2Desc.setInterpretation(min2Interpretation);


  _max2Desc.setNativeName("max2"); 
  _max2Desc.setArity(2UL); 
  _max2Desc.setInterpretation(max2Interpretation);



  _greaterDesc.setNativeName("greater"); 
  _greaterDesc.setArity(2UL); 
  _greaterDesc.setInterpretation(greaterInterpretation);

  _greaterOrEqualDesc.setNativeName("greater_or_equal"); 
  _greaterOrEqualDesc.setArity(2UL); 
  _greaterOrEqualDesc.setInterpretation(greaterOrEqualInterpretation);

  _lessDesc.setNativeName("less"); 
  _lessDesc.setArity(2UL); 
  _lessDesc.setInterpretation(lessInterpretation);

  _lessOrEqualDesc.setNativeName("less_or_equal"); 
  _lessOrEqualDesc.setArity(2UL); 
  _lessOrEqualDesc.setInterpretation(lessOrEqualInterpretation);

}; // BuiltInFloatingPointArithmetic::BuiltInFloatingPointArithmetic(const char* name,Clause::NewNumber* clauseNumberGenerator)

BuiltInFloatingPointArithmetic::~BuiltInFloatingPointArithmetic()
{
  CALL("destructor ~BuiltInFloatingPointArithmetic()");
  if (DestructionMode::isThorough())
    {
      if (_fact) delete _fact;
    };
};

void* BuiltInFloatingPointArithmetic::operator new(size_t)
{
  CALL("operator new(size_t)");
  return GlobAlloc::allocate(sizeof(BuiltInFloatingPointArithmetic));
}; // void* BuiltInFloatingPointArithmetic::operator new(size_t)
      
void BuiltInFloatingPointArithmetic::operator delete(void* obj)
{
  CALL("operator delete(void* obj)");
  GlobAlloc::deallocate(obj,sizeof(BuiltInFloatingPointArithmetic));
}; // void BuiltInFloatingPointArithmetic::operator delete(void* obj)


bool BuiltInFloatingPointArithmetic::interpretAs(ulong inputSymbolId,
					       const char* nativeSymbol)
{
  CALL("interpretAs(ulong inputSymbolId,const char* nativeSymbol)");

  NativeSymbolDescriptor* desc;
  if (!strcmp(nativeSymbol,"binary_plus"))
    {
      desc = &_binaryPlusDesc;
      desc->setId(inputSymbolId);
      _symbolInterpretation.sub(inputSymbolId) = desc;
      return true;
    };
  if (!strcmp(nativeSymbol,"unary_plus"))
    {
      desc = &_unaryPlusDesc;
      desc->setId(inputSymbolId);
      _symbolInterpretation.sub(inputSymbolId) = desc;
      return true;
    };
  if (!strcmp(nativeSymbol,"binary_minus"))
    {
      desc = &_binaryMinusDesc;
      desc->setId(inputSymbolId);
      _symbolInterpretation.sub(inputSymbolId) = desc;
      return true;
    };
  if (!strcmp(nativeSymbol,"unary_minus"))
    {
      desc = &_unaryMinusDesc;
      desc->setId(inputSymbolId);
      _symbolInterpretation.sub(inputSymbolId) = desc;
      return true;
    };
  if (!strcmp(nativeSymbol,"multiply"))
    {
      desc = &_multiplyDesc;
      desc->setId(inputSymbolId);
      _symbolInterpretation.sub(inputSymbolId) = desc;
      return true;
    };

  if (!strcmp(nativeSymbol,"divide"))
    {
      desc = &_divideDesc;
      desc->setId(inputSymbolId);
      _symbolInterpretation.sub(inputSymbolId) = desc;
      return true;
    };

  if (!strcmp(nativeSymbol,"min2"))
    {
      desc = &_min2Desc;
      desc->setId(inputSymbolId);
      _symbolInterpretation.sub(inputSymbolId) = desc;
      return true;
    };  
  
  if (!strcmp(nativeSymbol,"max2"))
    {
      desc = &_max2Desc;
      desc->setId(inputSymbolId);
      _symbolInterpretation.sub(inputSymbolId) = desc;
      return true;
    };

  if (!strcmp(nativeSymbol,"greater"))
    {
      desc = &_greaterDesc;
      desc->setId(inputSymbolId);
      _symbolInterpretation.sub(inputSymbolId) = desc;
      return true;
    };

  if (!strcmp(nativeSymbol,"greater_or_equal"))
    {
      desc = &_greaterOrEqualDesc;
      desc->setId(inputSymbolId);
      _symbolInterpretation.sub(inputSymbolId) = desc;
      return true;
    };

  if (!strcmp(nativeSymbol,"less"))
    {
      desc = &_lessDesc;
      desc->setId(inputSymbolId);
      _symbolInterpretation.sub(inputSymbolId) = desc;
      return true;
    };

  if (!strcmp(nativeSymbol,"less_or_equal"))
    {
      desc = &_lessOrEqualDesc;
      desc->setId(inputSymbolId);
      _symbolInterpretation.sub(inputSymbolId) = desc;
      return true;
    };

  return false; // no such native symbol
}; // bool BuiltInFloatingPointArithmetic::interpretAs(ulong inputSymbolId,const char* nativeSymbol)

inline
BuiltInFloatingPointArithmetic::NativeSymbolDescriptor* 
BuiltInFloatingPointArithmetic::interpretation(ulong symbolId)
{
  if (symbolId < _symbolInterpretation.size())
    return _symbolInterpretation[symbolId];
  return 0;
}; // NativeSymbolDescriptor* BuiltInFloatingPointArithmetic::interpretation(ulong symbolId)

inline
bool BuiltInFloatingPointArithmetic::canBeEvaluated(const Flatterm* term)
{
  CALL("canBeEvaluated(const Flatterm* term)");
  if (term->isVariable()) return false;

  if (!term->symbolRef().IsEquality())
    {
      NativeSymbolDescriptor* interpr = interpretation(term->functor());
      if (!interpr) return false;
    };

  Flatterm::ConstIterator iter(term);
  iter.NextSym();
  while (iter.NotEndOfTerm())
    {
      if ((iter.CurrPos()->isVariable()) || 
	  (!Signature::current()->isNumericConstant(iter.CurrPos()->functor())))
	return false;		  
      iter.NextSym();
    };
  return true;
}; // bool BuiltInFloatingPointArithmetic::canBeEvaluated(const Flatterm* term)


bool BuiltInFloatingPointArithmetic::evaluateTerm(const Flatterm* term,TmpLiteral* newLit)
{
  CALL("evaluateTerm(const Flatterm* term,TmpLiteral* newLit)");
  ASSERT(canBeEvaluated(term));  
  NativeSymbolDescriptor* interpr = interpretation(term->functor());
  ASSERT(interpr);
  double value; 
  
  switch (term->arity())
    {
    case 1UL:
      {
	double arg = Signature::current()->numericConstantValue(term->next()->functor());
	value = interpr->evaluateOn(arg);
	break;
      };
    case 2UL:
      {
	double arg1 = 
	  Signature::current()->numericConstantValue(term->next()->functor());
	double arg2 = 
	  Signature::current()->numericConstantValue(term->next()->next()->functor());
	value = interpr->evaluateOn(arg1,arg2);
	break;
      };
#ifdef DEBUG_NAMESPACE
    default:
      ICP("ICP0");
      return false;
#endif 
    };


  ulong resultConstId = Signature::current()->numericConstantNumber(value);
  TERM resultSymbol((TERM::Arity)0,(TERM::Functor)resultConstId,TERM::PositivePolarity);
  newLit->pushConst(resultSymbol);
  return true;
}; // bool BuiltInFloatingPointArithmetic::evaluateTerm(const Flatterm* term,TmpLiteral* newLit)


bool BuiltInFloatingPointArithmetic::evaluateAtom(const Flatterm* atom,bool& value)
{
  CALL("evaluateAtom(const Flatterm* atom,bool& value)");
  ASSERT(canBeEvaluated(atom));

  
  if (atom->symbolRef().IsEquality())
    {
      value =
	(atom->next()->functor() == atom->next()->next()->functor());
    }
  else
    {
      NativeSymbolDescriptor* interpr = interpretation(atom->functor());
      ASSERT(interpr);
      switch (atom->arity()) 
	{
	case 2UL:
	  {
	    double arg1 = 
	      Signature::current()->numericConstantValue(atom->next()->functor());
	    double arg2 = 
	      Signature::current()->numericConstantValue(atom->next()->next()->functor());
	    value = interpr->evaluatePredicateOn(arg1,arg2);
	    break;
	  };
#ifdef DEBUG_NAMESPACE
	default:
	  ICP("ICP0");
	  return false;
#endif 
	};
    };
  return true;
}; // bool BuiltInFloatingPointArithmetic::evaluateAtom(const Flatterm* atom,bool& value)





Clause* BuiltInFloatingPointArithmetic::simplify(TmpLiteral* lit,TmpLiteral* newLit)
{
  CALL("simplify(TmpLiteral* lit,TmpLiteral* newLit)");

  if (canBeEvaluated(lit->flatterm()))
    {
      bool atomValue;
      if (!evaluateAtom(lit->flatterm(),atomValue)) return 0;
      bool literalValue;
      if (lit->isPositive()) 
	{
	  literalValue = atomValue;
	}
      else
	literalValue = !atomValue;

      TERM propLiteral = (literalValue) ? TERM::builtInTrue() : TERM::builtInFalse();	
      newLit->reset();
      newLit->pushPropLit(propLiteral);
      newLit->endOfLiteral();
      if (!_fact)
	{
	  _fact = new Clause(static_cast<LiteralList*>(0));
	  _fact->markAsBuiltInTheoryFact();
	  _fact->SetNumber(_clauseNumberGenerator->generate());
	};
      
      return _fact;
    };

  Flatterm::Iterator iter1(lit->flatterm());
  iter1.NextSym(); // skipping the header
  while (iter1.NotEndOfTerm())
    {

      if (canBeEvaluated(iter1.CurrPos()))
	{ 

	  newLit->reset();
	  TERM header = lit->header();
	  if (header.IsEquality() && header.IsOrderedEq()) 
	    header.MakeUnordered();
	  newLit->pushNonpropHeader(header);       
	  Flatterm::Iterator iter2(lit->flatterm());	  
	  iter2.NextSym(); // skipping the header
	  while (iter2.NotEndOfTerm())
	    {
	      if (iter2.CurrPos() == iter1.CurrPos())
		{
		  if (!evaluateTerm(iter1.CurrPos(),newLit)) 
		    goto try_next_subterm;
		  iter2.after();
		  while (iter2.NotEndOfTerm())
		    {
		      if (!newLit->pushSym(iter2.CurrPos()->symbolRef())) 
			goto try_next_subterm;
		      iter2.NextSym();		      
		    };
		  newLit->endOfLiteral();

		  if (!_fact)
		    {
		      _fact = new Clause(static_cast<LiteralList*>(0));
		      _fact->markAsBuiltInTheoryFact();
		      _fact->SetNumber(_clauseNumberGenerator->generate());
		    };
		  
		  return _fact;
		}
	      else
		{
		  if (!newLit->pushSym(iter2.CurrPos()->symbolRef())) 
		    goto try_next_subterm;
		};
	      iter2.NextSym();
	    };
	};
    try_next_subterm:
      iter1.NextSym();
    };
  return 0;
}; // Clause* BuiltInFloatingPointArithmetic::simplify(TmpLiteral* lit,TmpLiteral* newLit)


double 
BuiltInFloatingPointArithmetic::binaryPlusInterpretation(const double& arg1,
							 const double& arg2)
{
  return arg1 + arg2;
}; // double BuiltInFloatingPointArithmetic::binaryPlusInterpretation(const double& arg1,const double& arg2)


double 
BuiltInFloatingPointArithmetic::unaryPlusInterpretation(const double& arg)
{
  return arg;
}; // double BuiltInFloatingPointArithmetic::unaryPlusInterpretation(const double& arg1,const double& arg2)


double 
BuiltInFloatingPointArithmetic::binaryMinusInterpretation(const double& arg1,
							 const double& arg2)
{
  return arg1 - arg2;
}; // double BuiltInFloatingPointArithmetic::binaryMinusInterpretation(const double& arg1,const double& arg2)

double 
BuiltInFloatingPointArithmetic::unaryMinusInterpretation(const double& arg)
{
  return -arg;
}; // double BuiltInFloatingPointArithmetic::unaryMinusInterpretation(const double& arg1,const double& arg2)

double 
BuiltInFloatingPointArithmetic::multiplyInterpretation(const double& arg1,
							 const double& arg2)
{
  return arg1 * arg2;
}; // double BuiltInFloatingPointArithmetic::multiplyInterpretation(const double& arg1,const double& arg2)


double 
BuiltInFloatingPointArithmetic::divideInterpretation(const double& arg1,
						     const double& arg2)
{
  return arg1/arg2;
}; // double BuiltInFloatingPointArithmetic::divideInterpretation(const double& arg1,const double& arg2)


double 
BuiltInFloatingPointArithmetic::min2Interpretation(const double& arg1,
						   const double& arg2)
{
  if (arg1 < arg2) return arg1;
  return arg2; 
}; // double BuiltInFloatingPointArithmetic::min2Interpretation(const double& arg1,const double& arg2)

double 
BuiltInFloatingPointArithmetic::max2Interpretation(const double& arg1,
						   const double& arg2)
{
  if (arg1 > arg2) return arg1;
  return arg2; 
}; // double BuiltInFloatingPointArithmetic::max2Interpretation(const double& arg1,const double& arg2)


bool
BuiltInFloatingPointArithmetic::greaterInterpretation(const double& arg1,
						      const double& arg2)
{
  return arg1 > arg2;
}; // double BuiltInFloatingPointArithmetic::greaterInterpretation(const double& arg1,const double& arg2)


bool
BuiltInFloatingPointArithmetic::greaterOrEqualInterpretation(const double& arg1,
						      const double& arg2)
{
  return arg1 >= arg2;
}; // double BuiltInFloatingPointArithmetic::greaterOrEqualInterpretation(const double& arg1,const double& arg2)

bool
BuiltInFloatingPointArithmetic::lessInterpretation(const double& arg1,
						   const double& arg2)
{
  return arg1 < arg2;
}; // double BuiltInFloatingPointArithmetic::lessInterpretation(const double& arg1,const double& arg2)


bool
BuiltInFloatingPointArithmetic::lessOrEqualInterpretation(const double& arg1,
						   const double& arg2)
{
  return arg1 <= arg2;
}; // double BuiltInFloatingPointArithmetic::lessOrEqualInterpretation(const double& arg1,const double& arg2)






//=================================================================
