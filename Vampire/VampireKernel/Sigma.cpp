//
// File:         Sigma.cpp
// Description:  Signature. 
// Created:      Oct 03, 2002, Alexandre Riazanov, riazanov@cs.man.ac.uk
// Status:       Sketch.
//============================================================================
#include "VampireKernelConst.hpp"
#include "Sigma.hpp"
//============================================================================
#ifdef DEBUG_SIGMA
#undef DEBUG_NAMESPACE
 #define DEBUG_NAMESPACE "Sigma"
#endif
#include "debugMacros.hpp"
//============================================================================
using namespace VK;
using namespace BK;

Sigma::Sigma() 
{
  CALL("constructor Sigma()");
  _nextFunctionId = 0UL;
  _nextPredicateId = 0UL;
  _functionPool.prepare(512);
  _predicatePool.prepare(512);
  _numberOfSkolemSymbols = 0UL;
  _maxlNumOfSkolemSymbols = ULONG_MAX;
  _nextSkolemPredicateSuffix = 0UL;
  _symbolWeightByArityCoefficient = 0L;
  _symbolPrecedenceByArityCoefficient = 0L;
  _defaultNumericConstantWeight = (TermWeightType)1;
  _defaultNumericConstantPrecedenceClass = 0L;
  _smallestRegisteredSymbolPrecedenceClass = 0L;
  _greatestRegisteredSymbolPrecedenceClass = 0L;
}; // Sigma::Sigma() 

Sigma::~Sigma()
{
  CALL("destructor ~Sigma()");
};


Sigma::Function* Sigma::function(const char* name,ulong arity)
{
  CALL("function(const char* name,ulong arity)");
  if (_functionPool.isEmpty()) _functionPool.prepare(512);
  Function* res = _functionPool.reserveObject();
  res->setIsNumeral(false);
  res->setName(name);
  res->setArity(arity);
  VoidSkipList<GlobAlloc,FunctionWrapper,2UL,28UL,Sigma>::Node* funListNode;
  if (_functions.insert(FunctionWrapper(res),funListNode))
    {
      res->resetProperties();
      res->setSymbolId(_nextFunctionId);
      res->setWeight(autoWeight(arity));
      res->setPrecedenceClass(registerSymbolPrecedenceClass(autoPrecedenceClass(arity)));
      ++_nextFunctionId;
      return res;
    }
  else // such function has been registered
    {
      _functionPool.releaseObject(res);
      return funListNode->key().function();
    };

}; // Function* Sigma::function(const char* name,ulong arity)

Sigma::Function* Sigma::numericConstant(const double& val)
{
  CALL("numericConstant(const double& val)");
  if (_functionPool.isEmpty()) _functionPool.prepare(512);
  Function* res = _functionPool.reserveObject();
  res->setIsNumeral(true);
  res->setNumericValue(val);
  VoidSkipList<GlobAlloc,FunctionWrapper,2UL,28UL,Sigma>::Node* funListNode;
  if (_functions.insert(FunctionWrapper(res),funListNode))
    {
      res->resetProperties();
      res->setSymbolId(_nextFunctionId);
      res->setWeight(_defaultNumericConstantWeight);
      res->setPrecedenceClass(registerSymbolPrecedenceClass(_defaultNumericConstantPrecedenceClass));
      ++_nextFunctionId;
      return res;
    }
  else // such numeric constant has been registered
    {
      _functionPool.releaseObject(res);
      return funListNode->key().function();
    };
}; // Function* Sigma::numericConstant(const double& val)

Sigma::Predicate* Sigma::predicate(const char* name,ulong arity)
{
  CALL("predicate(const char* name,ulong arity)");
  if (_predicatePool.isEmpty()) _predicatePool.prepare(512);
  Predicate* res = _predicatePool.reserveObject();
  res->setName(name);
  res->setArity(arity);
  VoidSkipList<GlobAlloc,PredicateWrapper,2UL,28UL,Sigma>::Node* predListNode;
  if (_predicates.insert(PredicateWrapper(res),predListNode))
    {
      res->resetProperties();
      res->setSymbolId(_nextPredicateId);
      res->setWeight(autoWeight(arity));
      res->setPrecedenceClass(registerSymbolPrecedenceClass(autoPrecedenceClass(arity)));
      ++_nextPredicateId;
      return res;
    }
  else
    {
      _predicatePool.releaseObject(res);
      return predListNode->key().predicate();
    };

}; // Predicate* Sigma::predicate(const char* name,ulong arity)



Sigma::Predicate* Sigma::skolemPredicate(ulong arity)
{
  CALL("skolemPredicate(ulong arity)");
  if (_numberOfSkolemSymbols >= _maxlNumOfSkolemSymbols) 
    return 0;

  if (_predicatePool.isEmpty()) _predicatePool.prepare(512);
  Predicate* res = _predicatePool.reserveObject();
  res->setArity(arity);
  VoidSkipList<GlobAlloc,PredicateWrapper,2UL,28UL,Sigma>::Node* predListNode;

  char nameBuffer[256];
  strcpy(nameBuffer,VampireKernelConst::SkolemPredicateNamePrefix);
  char* endOfPrefix = nameBuffer + strlen(VampireKernelConst::SkolemPredicateNamePrefix);
  char* suffix;

 try_suffix:
  suffix = endOfPrefix;
  print(_nextSkolemPredicateSuffix,suffix);
  suffix[0] = '\0';
  res->setName(nameBuffer);
  if (_predicates.contains(PredicateWrapper(res)))
    {
      // a predicate with this name and polarity exists, try bigger suffix
      ++_nextSkolemPredicateSuffix;
      goto try_suffix;
    };
  
  // this name is OK, register it

  ALWAYS(_predicates.insert(PredicateWrapper(res),predListNode));
  res->resetProperties();
  res->setSymbolId(_nextPredicateId);
  res->setIsInputSymbol(false);
  res->setWeight(autoWeight(arity));
  res->setPrecedenceClass(registerSymbolPrecedenceClass(autoPrecedenceClass(arity)));
  ++_nextPredicateId;
  ++_nextSkolemPredicateSuffix;  
  ++_numberOfSkolemSymbols;
  return res;
}; // Sigma::Predicate* Sigma::skolemPredicate(ulong arity)


ostream& Sigma::output(ostream& str) const
{
  str << "%===== Signature:\n";
  str << "%======== Functions:\n";
  if (_functions)
    {
      VoidSkipList<GlobAlloc,FunctionWrapper,2UL,28UL,Sigma>::Surfer functionSurfer(_functions);
      do
	{
	  ASSERT(functionSurfer.currNode());
	  Function* f = functionSurfer.currNode()->key().function();
	  str << f->symbolId() << ": ";
	  if (f->isNumeral())
	    {
	      str << f->numericValue();
	    }
	  else
	    {
	      str << f->name();
	    };
	  str << "/" << f->arity() << " ";
	  str << ((f->isInputSymbol()) ? "input  " : "skolem ");
	  str << "W=" << f->weight();
	  long precClass;
	  long numInPrecClass;
	  f->hasPrecedence(precClass,numInPrecClass);
	  str << " Prec=<" << precClass << "," << numInPrecClass << "> ";
	  if (f->isCommutative()) str << "commutative ";
	  str << "\n";
	}
      while (functionSurfer.next());      
    };

  str << "%======== Predicates:\n";
  if (_predicates)
    {
      VoidSkipList<GlobAlloc,PredicateWrapper,2UL,28UL,Sigma>::Surfer 
	predicateSurfer(_predicates);
      do
	{
	  ASSERT(predicateSurfer.currNode());
	  Predicate* p = predicateSurfer.currNode()->key().predicate();
	  str << p->symbolId() << ": "<< p->name() << "/" << p->arity() << " ";
	  str << ((p->isInputSymbol()) ? "input  " : "skolem ");
	  str << "W=" << p->weight();
	  long precClass;
	  long numInPrecClass;
	  p->hasPrecedence(precClass,numInPrecClass);
	  str << "Prec=<" << precClass << "," << numInPrecClass << "> ";
	  if (p->isSymmetric()) str << "symmetric ";
	  str << "\n";

	  LiteralHeader* posHeader = p->positiveHeader();
	  str << "   " << posHeader->headerId() << ": ++" << p->name() << " ";
	  posHeader->hasPrecedence(precClass,numInPrecClass);
	  str << "Prec=<" << precClass << "," << numInPrecClass << "> ";
	  str << "\n";

	  LiteralHeader* negHeader = p->negativeHeader();
	  str << "   " << negHeader->headerId() << ": --" << p->name() << " ";
	  negHeader->hasPrecedence(precClass,numInPrecClass);
	  str << "Prec=<" << precClass << "," << numInPrecClass << "> ";
	  str << "\n";
	}
      while (predicateSurfer.next());
    };


  str << "%===== End of signature.\n";
  return str;
}; // ostream& Sigma::output(ostream& str) const



void Sigma::print(ulong num,char*& target)
{
  if (!num) { target[0] = '0'; target++; return; }; 
  char lastDigit;
  switch (num % 10) 
    {   
    case 0UL: lastDigit = '0'; break;
    case 1UL: lastDigit = '1'; break;
    case 2UL: lastDigit = '2'; break;   
    case 3UL: lastDigit = '3'; break;
    case 4UL: lastDigit = '4'; break;
    case 5UL: lastDigit = '5'; break;   
    case 6UL: lastDigit = '6'; break;
    case 7UL: lastDigit = '7'; break;
    case 8UL: lastDigit = '8'; break;
    case 9UL: lastDigit = '9'; break;
    };
  num = num / 10;     
  if (num) print(num,target);
  target[0] = lastDigit;
  ++target;
}; // void Sigma::print(ulong num,char*& target) 

TermWeightType Sigma::autoWeight(ulong arity)
{
  if (_symbolWeightByArityCoefficient >= 0)
    {
      return (TermWeightType)(1 + _symbolWeightByArityCoefficient * arity);
    }
  else
    return 
      (TermWeightType)(1 - _symbolWeightByArityCoefficient*(VampireKernelConst::MaxSymbolArity - arity));
}; // TermWeightType Sigma::autoWeight(ulong arity)
 
long Sigma::autoPrecedenceClass(ulong arity)
{
  return _symbolPrecedenceByArityCoefficient * arity;
}; // long Sigma::autoPrecedenceClass(ulong arity)

inline
long Sigma::registerSymbolPrecedenceClass(long precClass)
{
  if (precClass < _smallestRegisteredSymbolPrecedenceClass)
    {
      _smallestRegisteredSymbolPrecedenceClass = precClass;
    }
  else
    if (precClass > _greatestRegisteredSymbolPrecedenceClass)
      {
	_greatestRegisteredSymbolPrecedenceClass = precClass;	
      };
  return precClass;
}; // long Sigma::registerSymbolPrecedenceClass(long precClass)

//============================================================================
