//
// File:         premise_factoring.cpp
// Description:  OR premises with factoring.
// Created:      Feb 15, 2000, 16:50
// Author:       Alexandre Riazanov
// mail:         riazanov@cs.man.ac.uk
// Revised:      Dec 10, 2001. 
//               The arrays OR_PREMISE::{max_literal,mark} replaced by 
//               extending arrays OR_PREMISE::{_maxLiteral,_litIsMarked}
// Revised:      Dec 11, 2001. 
//               The stack OR_PREMISE_WITH_FACTORING::factored replaced by 
//               extending stack OR_PREMISE_WITH_FACTORING::_factoredLits
//====================================================
#include "premise_factoring.hpp"
//================================================
#undef DEBUG_NAMESPACE
#ifdef DEBUG_PREMISE_FACTORING
 #define DEBUG_NAMESPACE "OR_PREMISE_WITH_FACTORING"
#endif
#include "debugMacros.hpp"
//=================================================

using namespace VK;


OR_PREMISE_WITH_FACTORING::OR_PREMISE_WITH_FACTORING() 
  : OR_PREMISE(),
    _factoredLits(DOP("_factoredLits"))
{
  CALL("constructor OR_PREMISE_WITH_FACTORING()");
  DOP(_factoredLits.freeze());
}; // OR_PREMISE_WITH_FACTORING::OR_PREMISE_WITH_FACTORING()


OR_PREMISE_WITH_FACTORING::OR_PREMISE_WITH_FACTORING(NewClause* res) 
  : OR_PREMISE(res),
    _factoredLits(DOP("_factoredLits"))
{
  CALL("constructor OR_PREMISE_WITH_FACTORING(NewClause* res)"); 
  DOP(_factoredLits.freeze());
}; // OR_PREMISE_WITH_FACTORING::OR_PREMISE_WITH_FACTORING(NewClause* res) 

OR_PREMISE_WITH_FACTORING::~OR_PREMISE_WITH_FACTORING() 
{ 
  CALL("destructor ~OR_PREMISE_WITH_FACTORING()"); 
};

void OR_PREMISE_WITH_FACTORING::init() 
{
  CALL("init()");
  OR_PREMISE::init();
  _factoredLits.init(DOP("_factoredLits"));
  DOP(_factoredLits.freeze());
}; // void OR_PREMISE_WITH_FACTORING::init() 

void OR_PREMISE_WITH_FACTORING::init(NewClause* res) 
{
  CALL("init(NewClause* res)");
  OR_PREMISE::init(res);
  _factoredLits.init(DOP("_factoredLits"));
  DOP(_factoredLits.freeze());
}; // void OR_PREMISE_WITH_FACTORING::init(NewClause* res) 

void OR_PREMISE_WITH_FACTORING::destroy()
{
  CALL("destroy()");
  _factoredLits.destroy();
  OR_PREMISE::destroy();
};



bool OR_PREMISE_WITH_FACTORING::GetFirstFactor(ulong first_candidate)
{
  if (FactoringPossible())
    {
      _factoredLits.reset();
      //factored.reset();
      if (FindFirstFactor(first_candidate))
	{
	  last_factor = first_candidate;
	  was_factored = true;
	  return true;
	};
    };
  return false;
}; // bool OR_PREMISE_WITH_FACTORING::GetFirstFactor(ulong first_candidate)
 
bool OR_PREMISE_WITH_FACTORING::GetNextFactor()
{
  ulong candidate = last_factor + 1;
 next_factor:
  if (FindFirstFactor(candidate))
    {
      _factoredLits.push(last_factor);
      //factored.push(last_factor);
      last_factor = candidate;
      return true;
    }
  else // backtrack needed
    {
      Unifier::current()->UnbindLastSection();
      OR_PREMISE::_litIsMarked[last_factor] = false;
      candidate = last_factor + 1;
      if (_factoredLits.empty())
	//if (factored.empty())
	{
	  if (FindFirstFactor(candidate))
	    {
	      last_factor = candidate;
	      return true;
	    }
	  else // no more factors at all
	    {
	      return false;
	    };
	}
      else 
	{
	  last_factor = _factoredLits.pop();
	  //last_factor = factored.pop();
	  goto next_factor;        
	};
    };
}; // bool OR_PREMISE_WITH_FACTORING::GetNextFactor()
 
bool OR_PREMISE_WITH_FACTORING::FindFirstFactor(ulong& candidate)
{
  while (candidate <= last_max)
    {
      if ((candidate != cut_lit) 
	  && (UNIFICATION::UnifyComplex(OR_PREMISE::_maxLiteral[candidate],cut_literal)))   
	{
	  // factor found
	  OR_PREMISE::_litIsMarked[candidate] = true;
	  return true;
	};
      // continue search
      candidate++;
    };
  // no factor found 
  return false;
}; // bool OR_PREMISE_WITH_FACTORING::FindFirstFactor(ulong& candidate)



//======================================================================
