//
// File:         AutoFunctorPrecedence.cpp
// Description:  Automatic assignment of precedences to the functors and headers of the signature.
// Created:      Apr 24, 2001, 17:20
// Author:       Alexandre Riazanov
// mail:         riazanov@cs.man.ac.uk
//====================================================
#include "AutoFunctorPrecedence.hpp"
//=================================================
#undef DEBUG_NAMESPACE
#ifdef DEBUG_AUTO_FUNCTOR_PRECEDENCE
 #define DEBUG_NAMESPACE "AutoFunctorPrecedence"
#endif 
#include "debugMacros.hpp"
//=================================================  

using namespace VK;

long AutoFunctorPrecedence::InitStatic::_count; // Implicitely initialised to 0L

#ifdef DEBUG_ALLOC_OBJ_TYPE
#include "ClassDesc.hpp"
using namespace BK;
ClassDesc 
AutoFunctorPrecedence::ArityFuncSortTree::LeftRightIter::NodeList::_classDesc("AutoFunctorPrecedence::ArityFuncSortTree::LeftRightIter::NodeList",
									      AutoFunctorPrecedence::ArityFuncSortTree::LeftRightIter::NodeList::minSize(),
									      AutoFunctorPrecedence::ArityFuncSortTree::LeftRightIter::NodeList::maxSize());

ClassDesc 
AutoFunctorPrecedence::ArityFuncSortTree::RightLeftIter::NodeList::_classDesc("AutoFunctorPrecedence::ArityFuncSortTree::RightLeftIter::NodeList",
									      AutoFunctorPrecedence::ArityFuncSortTree::RightLeftIter::NodeList::minSize(),
									      AutoFunctorPrecedence::ArityFuncSortTree::RightLeftIter::NodeList::maxSize());

ClassDesc 
AutoFunctorPrecedence::ArityFuncSortTree::Node::_classDesc("AutoFunctorPrecedence::ArityFuncSortTree::Node",
							       AutoFunctorPrecedence::ArityFuncSortTree::Node::minSize(),
							       AutoFunctorPrecedence::ArityFuncSortTree::Node::maxSize());

 
#endif


AutoFunctorPrecedence::AutoFunctorPrecedence() 
  : _dependenceOnArity(0L),
    _inversePolarity(false),
    _kinkyHeaderPrecedence(false),
    _somePrecedenceSet(false),
    _minimalFixedPrecedence(0L),
    _maximalFixedPrecedence(0L),
    _minFunctorNumber(0UL),
    _maxFunctorNumber(0UL),
#ifdef DEBUG_NAMESPACE
    _precedence(0L,"AutoFunctorPrecedence::_precedence"), 
    _precedenceIsFixed(false,"AutoFunctorPrecedence::_precedenceIsFixed"),
    _arity(0UL,"AutoFunctorPrecedence::_arity"),
    _registeredFunctor(false,"AutoFunctorPrecedence::_registeredFunctor")
#else
  _precedence(0L,0), 
  _precedenceIsFixed(false,0),
  _arity(0UL,0),
  _registeredFunctor(false,0)
#endif
{
  CALL("constructor AutoFunctorPrecedence()");   
};



AutoFunctorPrecedence::~AutoFunctorPrecedence()
{
  CALL("destructor ~AutoFunctorPrecedence()");
};


void AutoFunctorPrecedence::init() 
{
  CALL("constructor AutoFunctorPrecedence()");   
  
  _dependenceOnArity = 0L;
  _inversePolarity = false;
  _kinkyHeaderPrecedence = false;
  _somePrecedenceSet = false;
  _minimalFixedPrecedence = 0L;
  _maximalFixedPrecedence = 0L;
  _minFunctorNumber = 0UL;
  _maxFunctorNumber = 0UL;
#ifdef DEBUG_NAMESPACE
  _precedence.init(0L,"AutoFunctorPrecedence::_precedence");
  _precedenceIsFixed.init(false,"AutoFunctorPrecedence::_precedenceIsFixed");
  _arity.init(0UL,"AutoFunctorPrecedence::_arity");
  _registeredFunctor.init(false,"AutoFunctorPrecedence::_registeredFunctor");
#else
  _precedence.init(0L,0);
  _precedenceIsFixed.init(false,0);
  _arity.init(0UL,0);
  _registeredFunctor.init(false,0);
#endif
}; // void AutoFunctorPrecedence::init() 


void AutoFunctorPrecedence::destroy()
{
  CALL("destroy()");
  _registeredFunctor.destroy();
  _arity.destroy();
  _precedenceIsFixed.destroy();
  _precedence.destroy();
  BK_CORRUPT(*this);
}; // void AutoFunctorPrecedence::destroy()


bool AutoFunctorPrecedence::precedenceIsReserved(long precedence,ulong& funNum)
{
  CALL("precedenceIsReserved(long precedence,ulong& funNum)");
  if (!_somePrecedenceSet) return false;
  long normPrec = normalisedPrecedence(precedence);  
  if ((normPrec < _minimalFixedPrecedence) ||
      (normPrec > _maximalFixedPrecedence))
    {
      return false;
    };    

  for (ulong fn = 0; fn < _registeredFunctor.size(); fn++)
    if ((_registeredFunctor[fn]) && 
	(_precedenceIsFixed.sub(fn)))
      {
	if (_precedence[fn] == normPrec)
	  {
	    funNum = fn;
	    return true;
	  };
      };
  return false;
}; // bool AutoFunctorPrecedence::precedenceIsReserved(long precedence,ulong& funNum)


void AutoFunctorPrecedence::assignFixed(ulong funNum,long precedence) 
{ 
  CALL("assignFixed(ulong funNum,long precedence)");

  //DF; cout << "AF FN = " << funNum << "\n";

  _registeredFunctor.sub(funNum) = true;
  long normPrec = normalisedPrecedence(precedence);  
  if (!_somePrecedenceSet) 
    { 
      _somePrecedenceSet = true;
      _minimalFixedPrecedence = normPrec;
      _maximalFixedPrecedence = normPrec;
    }
  else 
    if (normPrec > _maximalFixedPrecedence) { _maximalFixedPrecedence = normPrec; }
    else 
      if (normPrec < _minimalFixedPrecedence) { _minimalFixedPrecedence = normPrec; };
   
  _precedence.sub(funNum) = normPrec;    
  _precedenceIsFixed.sub(funNum) = true;   
};

void AutoFunctorPrecedence::assignMaximalPrecedence(ulong funNum)
{

  //DF; cout << "AMP FN = " << funNum << "\n";

  _registeredFunctor.sub(funNum) = true;
  _somePrecedenceSet = true;
  _precedence.sub(funNum) = maximalPrecedence();
  _precedenceIsFixed.sub(funNum) = true;    
};

void AutoFunctorPrecedence::assignArity(ulong funNum,ulong arity) 
{ 
  CALL("assignArity(ulong funNum,ulong arity)");

  //DF; cout << "AAr FN = " << funNum << "\n";

  _registeredFunctor.sub(funNum) = true; 
  _arity.sub(funNum) = arity;
};
  
void AutoFunctorPrecedence::assignAllPrecedences() 
{ 
  CALL("assignAllPrecedences()");
  
  _maximalFixedPrecedence = maximalOf(absval(_maximalFixedPrecedence),absval(_minimalFixedPrecedence));
  _minimalFixedPrecedence = -_maximalFixedPrecedence;

  if (_dependenceOnArity)
    { 
      ArityFuncSortTree sortTree;   
      for (ulong fn = 0; fn < _registeredFunctor.size(); fn++)
	if ((_registeredFunctor[fn]) && 
	    (!_precedenceIsFixed.sub(fn))) 
	  {
	    sortTree.insert(ArityFuncPair(_arity.sub(fn),fn));
	  };

      if (_dependenceOnArity > 0L)     
	{
	  // greater arity => greater precedence 
	  ArityFuncSortTree::RightLeftIter sortTreeIter(sortTree);      
	  while (sortTreeIter)
	    { 
	      assignSmallerPrecedence(sortTreeIter.value().el2); 
	      sortTreeIter.next();     
	    };
	}
      else // _dependenceOnArity < 0L 
	{
	  // greater arity => smaller precedence   
	  ArityFuncSortTree::LeftRightIter sortTreeIter(sortTree);
	  while (sortTreeIter)
	    { 
	      assignSmallerPrecedence(sortTreeIter.value().el2); 
	      sortTreeIter.next();     
	    };
	};
    }  
  else // !_dependenceOnArity
    {
      for (ulong fn = 0; fn < _registeredFunctor.size(); fn++)
	if ((_registeredFunctor[fn]) && 
	    (!_precedenceIsFixed.sub(fn)))
	  assignSmallerPrecedence(fn);
    };
}; // void AutoFunctorPrecedence::assignAllPrecedences() 

void AutoFunctorPrecedence::assignSmallerPrecedence(ulong funNum) 
{ 
  CALL("assignSmallerPrecedence(ulong funNum)");
  _minimalFixedPrecedence = previousNormalisedPrecedence(_minimalFixedPrecedence);
  _precedence.sub(funNum) = _minimalFixedPrecedence;
};


//====================================================
