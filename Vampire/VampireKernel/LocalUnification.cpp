//
// File:         LocalUnification.cpp
// Description:  Unification of standard terms over the same variable space. 
// Created:      Oct 25, 2002, Alexandre Riazanov, riazanov@cs.man.ac.uk
//===================================================================
#include "LocalUnification.hpp"
//===================================================================
#undef DEBUG_NAMESPACE
#ifdef DEBUG_WEIGHT_POLYNOMIAL
 #define DEBUG_NAMESPACE "LocalUnification"
#endif
#include "debugMacros.hpp"
//===================================================================

using namespace VK;

LocalUnification::LocalUnification() 
{
  for (ulong v = 0; v < VampireKernelConst::MaxNumOfVariables; ++v)
    _unifier[v] = 0;
};

LocalUnification::~LocalUnification() {};

void LocalUnification::reset()
{
  CALL("reset()");
  while (_instantiated)
    *(_instantiated.pop()) = 0;
}; // void LocalUnification::reset()


bool LocalUnification::unify(const TERM* complexTerm1,const TERM* complexTerm2)
{
  CALL("unify(const TERM* complexTerm1,const TERM* complexTerm2)");
  const UnifierState* initialUnifierState = getUnifierState();
  if (unify1(complexTerm1,complexTerm2))
    return true;
  rollUnifierBackTo(initialUnifierState);
  return false;
}; // bool LocalUnification::unify(const TERM* complexTerm1,const TERM* complexTerm2)

bool LocalUnification::unify(const TERM& term1,const TERM& term2)
{
  CALL("unify(const TERM& term1,const TERM& term2)");
  const UnifierState* initialUnifierState = getUnifierState();
  if (unify1(term1,term2))
    return true;
  rollUnifierBackTo(initialUnifierState);
  return false;
}; // bool LocalUnification::unify(const TERM& term1,const TERM& term2)



void LocalUnification::rollUnifierBackTo(const UnifierState* notch)
{
  CALL("rollUnifierBackTo(const UnifierState* notch)");
  while (_instantiated.end() != static_cast<const void*>(notch))
    {
      *(_instantiated.pop()) = 0;
    };
}; // void LocalUnification::rollUnifierBackTo(const UnifierState* notch)

 

inline
bool LocalUnification::unify1(const TERM* complexTerm1,const TERM* complexTerm2)
{
  CALL("unify1(const TERM* complexTerm1,const TERM* complexTerm2)");
  ASSERT(complexTerm1->isComplex());
  ASSERT(complexTerm2->isComplex());

  //DF; cout << "UNIF COMPL" << complexTerm1 << " WITH " << complexTerm2 << "\n";

  if (*complexTerm1 != *complexTerm2) 
    {

      //DF; cout << "DIFF TOPS\n";

      return false;
    };
  const TERM* arg1 = complexTerm1->Args();
  const TERM* arg2 = complexTerm2->Args();
  while (!arg1->IsNull())
    {
      if (!unify1(*arg1,*arg2)) 
	return false;
      arg1 = arg1->Next();
      arg2 = arg2->Next();
    };
  return true;
}; // bool LocalUnification::unify1(const TERM* complexTerm1,const TERM* complexTerm2)

inline
bool LocalUnification::unify1(const TERM& term1,const TERM& term2)
{
  CALL("unify1(const TERM& term1,const TERM& term2)");

  //DF; cout << "UNIF " << term1 << " WITH " << term2 << "\n";


  if (term1 == term2) return true;  

  if (term1.isVariable())
    {
      const TERM* instance = _unifier[term1.var()];
      if (instance)
	{
	  return unify1(*instance,term2);
	}
      else // term1 is a free variable
 	{
	  if (term2.isVariable())
	    {
	      instance = _unifier[term2.var()];
	      if (instance)
		{
		  return unify1(term1,*instance);
		}
	      else // term2 is a free variable
		{		  
		  instantiate(term1.var(),&term2);
		  return true;
		};

	    }
	  else
	    {
	      if (occursIn(term1,term2)) 
		{
		  
		  //DF; cout << term1 << " OCCURS IN " << term2 << "\n";

		  return false;
		};
	      instantiate(term1.var(),&term2);
	      return true;
	    };
	};
    }
  else
    if (term2.isVariable())
      {
	const TERM* instance = _unifier[term2.var()];
	if (instance)
	  {	   
	    return unify1(term1,*instance);
	  }
	else // term2 is a free variable
	  {
	    if (occursIn(term2,term1)) 
	      {
		//DF; cout << term2 << " OCCURS IN " << term1 << "\n";
		return false;
	      };
	    instantiate(term2.var(),&term1);
	    return true;
	  };
      }
    else
      return unify1(term1.First(),term2.First()); 
}; // bool LocalUnification::unify1(const TERM& term1,const TERM& term2)


inline
bool LocalUnification::occursIn(const TERM& var,const TERM& term) const
{
  CALL("occursIn(const TERM& var,const TERM& term) const");
  if (var == term) return true;
  if (term.isVariable())
    {
      const TERM* instance = _unifier[term.var()];
      if (instance)
	{
	  return occursIn(var,*instance);
	}
      else
	return false;
    }
  else
    {
      ASSERT(term.IsReference());
      for (const TERM* arg = term.First()->Args(); !arg->IsNull(); arg = arg->Next())
	if (occursIn(var,*arg)) return true;
      return false;
    };

}; // bool LocalUnification::occursIn(const TERM& var,const TERM& term) const

inline
void LocalUnification::instantiate(ulong varNum,const TERM* instance)
{
  CALL("instantiate(ulong varNum,const TERM* instance)");

  ASSERT(!_unifier[varNum]);
  _unifier[varNum] = instance;
  _instantiated.push(_unifier + varNum);
}; // void LocalUnification::instantiate(ulong varNum,const TERM* instance)




#ifndef NO_DEBUG_VIS
ostream& LocalUnification::output(ostream& str) const
{
  CALL("output(ostream& str) const");
  str << "{";
  for (const TERM** const * v = _instantiated.begin();
       v != _instantiated.end();
       ++v)
    {
      str << "X" << ((*v) - _unifier) << " --> "
	  << ***v;
      if (v + 1 != _instantiated.end())
	str << " | ";
    };

  return str << "}";
}; // ostream& LocalUnification::output(ostream& str) const
#endif


//===================================================================
#undef DEBUG_NAMESPACE
#ifdef DEBUG_WEIGHT_POLYNOMIAL
 #define DEBUG_NAMESPACE "LocalUnification::TermTraversal"
#endif
#include "debugMacros.hpp"
//===================================================================


LocalUnification::TermTraversal::TermTraversal() 
{
  _dummyTerm[0] = TERM::dummyPositiveHeader1();  
};

LocalUnification::TermTraversal::TermTraversal(const TERM& term,const LocalUnification* unifier) 
{
  CALL("constructor TermTraversal(const TERM& term,const LocalUnification* unifier)");
  _dummyTerm[0] = TERM::dummyPositiveHeader1();  
  _unifier = unifier->_unifier;
  _dummyTerm[1] = term;
  _term = _dummyTerm + 1;
  _after = _dummyTerm;
  canonizeVariableInstance();
};


LocalUnification::TermTraversal::~TermTraversal() {};


void LocalUnification::TermTraversal::reset(const TERM& term,const LocalUnification* unifier)
{
  CALL("reset(const TERM& term)");
  _unifier = unifier->_unifier;
  _dummyTerm[1] = term;
  _term = _dummyTerm + 1;
  _after = _dummyTerm;
  _backtrackPoints.reset();
  canonizeVariableInstance();
}; // void LocalUnification::TermTraversal::reset(const TERM& term)





//===================================================================
