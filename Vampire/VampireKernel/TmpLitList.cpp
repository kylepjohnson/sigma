//
// File:         TmpLitList.cpp
// Description:  Lists of temporary (buffer) literals.
// Created:      Dec 17, 2001 
// Author:       Alexandre Riazanov
// mail:         riazanov@cs.man.ac.uk
// Note:         Reimplementation of TMP_LIT_LIST (tmp_lit_list.h)
//============================================================================
#include "TmpLitList.hpp"
#include "Signature.hpp"
using namespace BK;
using namespace VK;
//============================================================================
#undef DEBUG_NAMESPACE
#ifdef DEBUG_TMP_LIT_LIST
 #define DEBUG_NAMESPACE "TmpLitList"
#endif
#include "debugMacros.hpp"
//============================================================================



bool TmpLitList::substPossible(ulong var,unsigned long substDepth,long substSize) 
{
  CALL("substPossible(ulong var,unsigned long substDepth,long substSize) const");
  ASSERT(substDepth);
  for (TmpLiteral* el = first(); el; el = el->next())
    if (!el->instantiationPossible(var,substDepth,substSize))
      return false;
  return true;
}; // bool TmpLitList::substPossible(ulong var,unsigned long substDepth,long substSize) 
  

long TmpLitList::computeWeightBase() const 
{
  CALL("computeWeightBase() const");
  long res = 0L;
  for (const TmpLiteral* el = first(); el; el = el->next())
    {
      res += el->weightBase(); 
    };
  return res;
}; // long computeWeightBase() const 

void TmpLitList::computeSizeAndWeightBase(long& sz,long& weightBase) const
{
  CALL("computeSizeAndAndWeightBaseNumOfPosEq(long& sz,long& weightBase) const");
  sz = 0L;
  weightBase = 0L; 
  for (const TmpLiteral* el = first(); el; el = el->next())
    {
      sz += el->size();
      weightBase += el->weightBase();
    };
}; // void TmpLitList::computeSizeAndAndWeightBase(long& sz,long& weightBase) const


void TmpLitList::computeSizeAndWeightBaseAndNumOfPosEq(long& sz,long& weightBase,long& numOfPosEq) const
{
  CALL("computeSizeAndWeightBaseAndNumOfPosEq(long& sz,long& weightBase,long& numOfPosEq) const");
  sz = 0L;
  weightBase = 0L; 
  numOfPosEq = 0L;
  for (const TmpLiteral* el = first(); el; el = el->next())
    {
      sz += el->size();
      weightBase += el->weightBase();
      if (el->isEquality() && el->isPositive())
	numOfPosEq++;     
    };

}; // void TmpLitList::computeSizeAndWeightBaseAndNumOfPosEq(long& sz,long& weightBase,long& numOfPosEq) const



long TmpLitList::minlPosEqLHSSize() const
{
  CALL("minlPosEqLHSSize() const");

  for (const TmpLiteral* el = first(); el; el = el->next())
    {
      if (el->isEquality() && el->isPositive()) 
	{
	  long res = el->minPosEqLHSSize();   
	  for (el = el->next(); el; el = el->next())
	    {  
	      if (el->isEquality() && el->isPositive()) 
		{
		  long pemlw = el->minPosEqLHSSize();
		  if (pemlw < res) res = pemlw;
		};
	    };
	  return res;   
	};
    };
  return 0L;
}; // long TmpLitList::minlPosEqLHSSize() const




void TmpLitList::collectStatistics(Statistics& stat,bool inversePolarityForSelection) const
{
  CALL("collectStatistics(Statistics& stat,bool inversePolarityForSelection) const"); 
  for (const TmpLiteral* el = first(); el; el = el->next())
    {
      if (el->isAnswerLiteral()) 
	{
	  ASSERT(!el->isInherentlySelected());
	  ASSERT(!el->isEquality());      
	  stat.answer_literals++;
	}
      else // non-answer literal
	{
	  if (el->isInherentlySelected()) stat.inherently_selected++;
	  if (el->isEquality()) { stat.equations++; }
	  else
	    {
	      if (stat.noneq_nonanswer_lits)
		{
		  //if (GlobalHeaderWeighting.Compare(el->header(),stat.max_noneq_header) == BK::Greater)
		  if (Signature::current()->compareHeaderPrecedences(el->header().polarity(),
							    el->header().functor(),
							    stat.max_noneq_header.polarity(),
							    stat.max_noneq_header.functor()) == BK::Greater)
		 
		    {
		      stat.max_noneq_header = el->header();
		    };
		}
	      else stat.max_noneq_header = el->header(); 
	      stat.noneq_nonanswer_lits++;
	    };
	  if (isNegativeForSelection(el->header(),inversePolarityForSelection)) stat.negative_nonanswer_lits++;
	  
	};
    };
      
}; // void TmpLitList::collectStatistics(Statistics& stat,bool inversePolarityForSelection) const

  

void TmpLitList::collectStatisticsWithSplitting(Statistics& stat,bool inversePolarityForSelection) const
{
  CALL("collectStatisticsWithSplitting(Statistics& stat,bool inversePolarityForSelection) const"); 
  for (const TmpLiteral* el = first(); el; el = el->next())
    {
      if (el->isAnswerLiteral()) 
	{
	  ASSERT(!el->isInherentlySelected());
	  ASSERT(!el->isEquality());    
	  stat.answer_literals++;
	}
      else
	{
	  if (el->isInherentlySelected()) stat.inherently_selected++;
	  if (el->isEquality()) { stat.equations++; }
	  else
	    {
	      if (stat.noneq_nonanswer_lits)
		{
		  //if (GlobalHeaderWeighting.Compare(el->header(),stat.max_noneq_header) == BK::Greater)
		  if (Signature::current()->compareHeaderPrecedences(el->header().polarity(),
							    el->header().functor(),
							    stat.max_noneq_header.polarity(),
							    stat.max_noneq_header.functor()) == BK::Greater)
		    {
		      stat.max_noneq_header = el->header();
		    };
		}
	      else stat.max_noneq_header = el->header(); 
	      stat.noneq_nonanswer_lits++;
	      if (el->header().IsSplitting())
		{
		  if (isNegativeForSelection(el->header(),inversePolarityForSelection))
		    {
		      stat.negative_splitting_branches++;
		    }
		  else stat.positive_splitting_branches++;
		};
	    };
	  if (isNegativeForSelection(el->header(),inversePolarityForSelection)) stat.negative_nonanswer_lits++;
	};
    };
}; // void TmpLitList::collectStatisticsWithSplitting(Statistics& stat,bool inversePolarityForSelection) const
  
void TmpLitList::renameVariables(INT_VAR_RENAMING& varRenaming)
{
  CALL("renameVariables(INT_VAR_RENAMING& varRenaming)");
  for (TmpLiteral* el = last(); el; el = el->previous()) 
    // ^ order is significant
    { 
      el->renameVariables(varRenaming);
    };
}; // void TmpLitList::renameVariables(INT_VAR_RENAMING& varRenaming)



bool TmpLitList::weed(bool& eqTautology)
{
  CALL("weed(bool& eqTautology)");
  ASSERT(checkIntegrity());

  TmpLitList remainingLits;
  remainingLits.collectFrom(*this);
  ASSERT(empty());
  while (remainingLits.nonempty())
    {
      TmpLiteral* lit = remainingLits.pop();
      TERM header = lit->header();
      if (header.IsEquality())
	{
	  if (lit->argsAreEqual())
	    {
	      if (header.Positive()) 
		{
		  push(lit);
		  collectFrom(remainingLits);
		  eqTautology = true; 
		  return false; 
		}
	      else
		lit->recycle();
	    }
	  else // different arguments
	    {
	      for (TmpLiteral* el = first(); el; el = el->next())
		if (el->isEquality())
		  {
		    if (lit->equalEqLit(*el))
		      {
			lit->recycle();
			goto next_lit;
		      }
		    else
		      {
			

			if (lit->complementaryEqLit(*el))
			  {
			    // propositional tautology
			    push(lit);
			    collectFrom(remainingLits);
			    eqTautology = false; 
			    return false; 
			  };
		      };
		  };
	      push(lit);
	      lit->orderEqArgs(); 
	    };
	}
      else // nonequality literal
	{
	  if (lit->header() == TERM::builtInTrue())
	    {
	      push(lit);
	      collectFrom(remainingLits);
	      eqTautology = false; 
	      return false; 
	    };
	  if (lit->header() == TERM::builtInFalse())
	    {
	      lit->recycle();
	      goto next_lit;
	    }
	  else
	    {
	      for (TmpLiteral* el = first(); el; el = el->next())
		if (lit->equal(*el))
		  {
		    lit->recycle();
		    goto next_lit;
		  }
		else
		  if (lit->isComplementary(*el))
		    {
		      // propositional tautology
		      push(lit);
		      collectFrom(remainingLits);
		      eqTautology = false; 
		      return false; 
		    };
	      push(lit);
	    };
	};
    next_lit: {}; // nothing to do here, just the end of the loop body
    };
  
  ASSERT(checkIntegrity());
  return true;  
}; // bool TmpLitList::weed(bool& eqTautology)


bool TmpLitList::weed(const Clause* const * commutativityTable,bool& eqTautology)
{
  CALL("weed(const Clause* const * commutativityTable,bool& eqTautology)");
  TmpLitList remainingLits;
  remainingLits.collectFrom(*this);
  ASSERT(empty());

  while (remainingLits.nonempty())
    {
      TmpLiteral* lit = remainingLits.pop();
      TERM header = lit->header();
      if (header.IsEquality())
	{
	  if (lit->argsAreEqual(commutativityTable))
	    {
	      if (header.Positive()) 
		{
		  push(lit);
		  collectFrom(remainingLits);
		  eqTautology = true; 
		  return false; 
		}
	      else
		lit->recycle();
	    }
	  else // different arguments
	    {
	      for (TmpLiteral* el = first(); el; el = el->next())
		if (el->isEquality())
		  {
		    if (lit->equalEqLit(*el,commutativityTable))
		      {
			lit->recycle();
			goto next_lit;
		      }
		    else
		      if (lit->complementaryEqLit(*el,commutativityTable))
			{
			  // propositional tautology
			  push(lit);
			  collectFrom(remainingLits);
			  eqTautology = false; 
			  return false; 
			};
		  };
	      push(lit);
	      lit->orderEqArgs(); 
	    };
	}
      else // nonequality literal
	{
	  if (lit->header() == TERM::builtInTrue())
	    {
	      push(lit);
	      collectFrom(remainingLits);
	      eqTautology = false; 
	      return false; 
	    };
	  if (lit->header() == TERM::builtInFalse())
	    {
	      lit->recycle();
	      goto next_lit;
	    }
	  else
	    {
	      for (TmpLiteral* el = first(); el; el = el->next())
		if (lit->equal(*el,commutativityTable))
		  {
		    lit->recycle();
		    goto next_lit;
		  }
		else
		  if (lit->isComplementary(*el,commutativityTable))
		    {
		      // propositional tautology
		      push(lit);
		      collectFrom(remainingLits);
		      eqTautology = false; 
		      return false; 
		    };
	      push(lit);
	    };
	};
    next_lit: {}; // nothing to do here, just the end of the loop body
    };
  
  ASSERT(checkIntegrity());
  return true;  

}; // bool TmpLitList::weed(const Clause* const * commutativityTable,bool& eqTautology)


void TmpLitList::order(TmpLitList& result,const VAR_WEIGHTING& varWeighting)
{
  CALL("order(TmpLitList& result,const VAR_WEIGHTING& varWeighting)");
  ASSERT(nonempty());
  if (numOfAllLiterals() == 1UL) { result.push(pop()); return; }; 
  if (numOfAllLiterals() == 2UL)
    { 
      TmpLiteral* fst = pop();
      TmpLiteral* snd = pop();
      if (fst->compareWithVarWeighting(*snd,varWeighting) == BK::Less) 
	{
	  result.push(fst); 
	  result.push(snd); 
	  return;
	};
      result.push(snd); 
      result.push(fst);
      ASSERT(result.checkIntegrity());
      return; 
    };
  // numOfAllLiterals() > 2UL, use the quicksort algorithm
  TmpLiteral* pivot = pop();  
  TmpLitList smaller;
  TmpLitList greater; 
  do
    {
      TmpLiteral* lit = pop();
      if (lit->compareWithVarWeighting(*pivot,varWeighting) == BK::Less) 
	{ 
	  smaller.push(lit); 
	} 
      else 
	greater.push(lit); 
    }
  while (nonempty()); 
  if (smaller.nonempty()) smaller.order(result,varWeighting);
  result.push(pivot);
  if (greater.nonempty()) greater.order(result,varWeighting);
  ASSERT(result.checkIntegrity());
}; // void TmpLitList::order(TmpLitList& result,const VAR_WEIGHTING& varWeighting)



ostream& TmpLitList::output(ostream& str) const 
{
  CALL("output(ostream& str) const");
  for (const TmpLiteral* el = first(); el; el = el->next())
    {
      str << el;
      if (el->next()) str << " % "; 
    };
  return str;
}; //  ostream& output(ostream& str) const 


//============================================================================
