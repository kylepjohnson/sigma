
//
// File:         LiteralSelection.cpp
// Description:  Defines various literal selection functions
// Created:      Dec 12, 2001, Alexandre Riazanov, riazanov@cs.man.ac.uk
// Note:         Was a part of NewClause.
// Revised:      May 15, 2002, Alexandre Riazanov, riazanov@cs.man.ac.uk 
//               A number of bugs fixed, mostly in the bidirectional selection
//               functions.  
// Revised:      Jun 01, 2002, Alexandre Riazanov, riazanov@cs.man.ac.uk
//               Bug fixed in function1(..).
//============================================================================
#include "VampireKernelDebugFlags.hpp"
#include "LiteralSelection.hpp"
#include "Term.hpp"
#include "SymbolInfo.hpp"
#include "Signature.hpp"
#include "TermWeightType.hpp"
//================================================
#undef DEBUG_NAMESPACE
#ifdef DEBUG_LITERAL_SELECTION
#define DEBUG_NAMESPACE "LiteralSelection"
#endif 
#include "debugMacros.hpp"
//=================================================


  using namespace BK;
  using namespace Gem;
  using namespace VK;

const LiteralSelection* LiteralSelection::_currentObject; 


void 
LiteralSelection::select(TmpLitList& lits,TmpLitList& selectedLits,TmpLitList& nonselectedLits,bool& negSelectionUsed) const
{
  CALL("select(TmpLitList& lits,TmpLitList& selectedLits,TmpLitList& nonselectedLits,bool& negSelectionUsed) const");

  TmpLitList::Statistics stat; 

  if (_splittingIsEnabled)
    {
      lits.collectStatisticsWithSplitting(stat,_inversePolarity);  
      if (stat.answer_literals)
	{
	  unselectAnswerLiterals(lits,selectedLits,nonselectedLits,stat.answer_literals);
	  ASSERT(!lits.containsAnswerLiterals());
	  if (!lits.numOfAllLiterals()) return;
          if (lits.numOfAllLiterals() == 1) { selectedLits.push(lits.pop()); return; };
          stat.reset();
          lits.collectStatisticsWithSplitting(stat,_inversePolarity);  
	}
      else
	{
	  ASSERT(!lits.containsAnswerLiterals());
	  if (lits.numOfAllLiterals() == 1) { selectedLits.push(lits.pop()); return; };
	};
      _currentObject = this;
      switch (_splittingStyle)
	{
	case 1L: selectWithNonblockingSplitting(lits,selectedLits,nonselectedLits,negSelectionUsed,stat); return;
	case 2L: selectWithBlockingSplitting(lits,selectedLits,nonselectedLits,negSelectionUsed,stat); return;
	default: 
	  RuntimeError::report("Wrong splitting style number."); 
	  return;
	};
    };
  // no splitting
  lits.collectStatistics(stat,_inversePolarity);
  if (stat.answer_literals)
    {
      unselectAnswerLiterals(lits,selectedLits,nonselectedLits,stat.answer_literals);
      ASSERT(!lits.containsAnswerLiterals());
      if (!lits.numOfAllLiterals()) return;
      if (lits.numOfAllLiterals() == 1) { selectedLits.push(lits.pop()); return; };
      stat.reset();
      lits.collectStatistics(stat,_inversePolarity);
    }
  else
    {
      ASSERT(!lits.containsAnswerLiterals());
      if (lits.numOfAllLiterals() == 1) { selectedLits.push(lits.pop()); return; };
    };

  _currentObject = this;
 
  if (stat.inherently_selected) 
    {
      selectInherentlySelected(lits,stat.inherently_selected,selectedLits,nonselectedLits);
      return;  
    };

#ifdef VKERNEL_FOR_STEP_RESOLUTION
  if (_stepResolutionLiteralSelectionFlag)
    {
      bool containsNextStateLiterals = false;
      bool containsCurrentStateLiterals = false;
      TmpLiteral* el = lits.first();
    check_lit:
      if (el)
	{
	  if (isNextStepLiteral(el)) 
	    {
	      containsNextStateLiterals = true;
	    }
	  else
	    if (isCurrentStepLiteral(el)) 
	      containsCurrentStateLiterals = true;
	  if (containsNextStateLiterals && containsCurrentStateLiterals)
	    {
	      unselectCurrentStateLiterals(lits,selectedLits,nonselectedLits);
	      ASSERT(lits.numOfAllLiterals());
	      if (lits.numOfAllLiterals() == 1) { selectedLits.push(lits.pop()); return; };
	      stat.reset();
	      lits.collectStatisticsWithSplitting(stat,_inversePolarity);
	    }
	  else
	    {
	      el = el->next();
	      goto check_lit;
	    };
	};	
    };
#endif



  _basicSelFun(lits,selectedLits,nonselectedLits,negSelectionUsed,stat);
}; // void LiteralSelection::select(TmpLitList& lits,TmpLitList& selectedLits,TmpLitList& nonselectedLits,bool& negSelectionUsed)



void 
LiteralSelection::selectForSupport(TmpLitList& lits,
				   TmpLitList& selectedLits,
				   TmpLitList& nonselectedLits) const
{
  CALL("selectForSupport(..)");
  if (lits.empty()) return;
  TmpLitList::Statistics stat; 
  lits.collectStatistics(stat,_inversePolarity);
  if (stat.answer_literals)
    {
      unselectAnswerLiterals(lits,selectedLits,nonselectedLits,stat.answer_literals);
      ASSERT(!lits.containsAnswerLiterals());
      if (!lits.numOfAllLiterals()) return;
      if (lits.numOfAllLiterals() == 1) { selectedLits.push(lits.pop()); return; };
      stat.reset();
      lits.collectStatistics(stat,_inversePolarity);
    }
  else
    {
      ASSERT(!lits.containsAnswerLiterals());
      if (lits.numOfAllLiterals() == 1) { selectedLits.push(lits.pop()); return; };
    };

  if (stat.negative_nonanswer_lits || stat.noneq_nonanswer_lits)
    {
      // select everything apart from positive equalities
      while (lits.nonempty())
	{
	  TmpLiteral* el = lits.pop();
	  if (el->isEquality() && el->isPositive())
	    {
	      nonselectedLits.push(el);
	    }
	  else
	    selectedLits.push(el);
	};

    }
  else // only positive equalities left
    {
      _currentObject = this;
      bool dummyBool;
      _basicSelFun(lits,selectedLits,nonselectedLits,dummyBool,stat);
    };
  
}; // void LiteralSelection::selectForSupport(..) const




inline 
void 
LiteralSelection::selectWithNonblockingSplitting(TmpLitList& lits,TmpLitList& selectedLits,
						 TmpLitList& nonselectedLits,
						 bool& negSelectionUsed,
						 const TmpLitList::Statistics& stat) const
{
  CALL("selectWithNonblockingSplitting(TmpLitList& lits,TmpLitList& selectedLits,TmpLitList& nonselectedLits,bool& negSelectionUsed,const TmpLitList::Statistics& stat) const");
  ASSERT(!lits.containsAnswerLiterals());
  ASSERT(lits.numOfAllLiterals() > 1);
  if (lits.numOfAllLiterals() > (stat.negative_splitting_branches + stat.positive_splitting_branches))
    {
      // a nonsplitting literal exist, all splitting branches are nonselected
      TmpLiteral* el = lits.first();
      while (el)
	{
	  if (el->header().IsSplitting())     
	    {	      
	      el->setInherentlySelected(false);
	      TmpLiteral* tmp = el->previous();
	      lits.remove(el);
	      nonselectedLits.push(el);
	      if (tmp) { el = tmp->next(); }
	      else
		el = lits.first();
	    }
	  else
	    el = el->next();
	};
      

      ASSERT(lits.nonempty());
      if (lits.numOfAllLiterals() == 1) 
	{
	  selectedLits.push(lits.pop());
	  negSelectionUsed = false;
	}
      else // lits.numOfAllLiterals() > 1
	{
	  TmpLitList::Statistics newStat;
	  lits.collectStatistics(newStat,_inversePolarity);
	  if (newStat.inherently_selected) 
	    {
	      selectInherentlySelected(lits,newStat.inherently_selected,selectedLits,nonselectedLits);
	      return;  
	    };
	  _basicSelFun(lits,selectedLits,nonselectedLits,negSelectionUsed,newStat); 
	};
    }
  else // all literals are splitting branches, select the youngest among them  
    {
      TmpLiteral* youngestBranch = lits.pop();
      ASSERT(youngestBranch->header().IsSplitting());
      TmpLiteral* currLit;
      do 
	{
	  currLit = lits.pop();
	  ASSERT(currLit->header().IsSplitting());
	  if (currLit->header().functor() > youngestBranch->header().functor())
	    {
	      youngestBranch->setInherentlySelected(false);
	      nonselectedLits.push(youngestBranch);
	      youngestBranch = currLit;
	    }
	  else 
	    {
	      currLit->setInherentlySelected(false);
	      nonselectedLits.push(currLit);
	    };
	}     
      while (lits.nonempty());
      negSelectionUsed = false;
      selectedLits.push(youngestBranch);
    };
}; // void LiteralSelection::selectWithNonblockingSplitting(..)


inline 
void 
LiteralSelection::selectWithBlockingSplitting(TmpLitList& lits,TmpLitList& selectedLits,
					      TmpLitList& nonselectedLits,
					      bool& negSelectionUsed,
					      const TmpLitList::Statistics& stat) const
{
  CALL("selectWithBlockingSplitting(TmpLitList& lits,TmpLitList& selectedLits,TmpLitList& nonselectedLits,bool& negSelectionUsed,const TmpLitList::Statistics& stat) const");
  ASSERT(!lits.containsAnswerLiterals());
  ASSERT(lits.numOfAllLiterals() > 1);
  if (stat.negative_splitting_branches) // a negative branch exists, select the youngest among the negative branches
    {
      TmpLiteral* youngestNegBranch = 0;
      TmpLiteral* currLit = lits.pop(); 
      if (currLit->header().IsSplitting() && isNegativeForSelection(currLit->header())) 
	{ 
	  youngestNegBranch = currLit;
	}
      else 
	{
	  currLit->setInherentlySelected(false);
	  nonselectedLits.push(currLit);
	};
    
      do
	{   
	  currLit = lits.pop();
	  if (currLit->header().IsSplitting() && isNegativeForSelection(currLit->header())) 
	    {
	      if (!youngestNegBranch) 
		{ 
		  youngestNegBranch = currLit; 
		}
	      else 
		if (currLit->header().functor() > youngestNegBranch->header().functor())
		  {
		    youngestNegBranch->setInherentlySelected(false);
		    nonselectedLits.push(youngestNegBranch);
		    youngestNegBranch = currLit; 
		  }
		else 
		  {
		    currLit->setInherentlySelected(false);
		    nonselectedLits.push(currLit);
		  };
	    }
	  else 
	    {
	      currLit->setInherentlySelected(false);
	      nonselectedLits.push(currLit);
	    };
	}
      while (lits.nonempty());
      selectedLits.push(youngestNegBranch);
      negSelectionUsed = false;
      // that's it
    }
  else // no negative splitting branches 
    {
      if (lits.numOfAllLiterals() > stat.positive_splitting_branches) // a literal with input predicate exists
	{	      	  
	  // a nonsplitting literal exist, all splitting branches are nonselected
	  TmpLiteral* el = lits.first();
	  while (el)
	    {
	      if (el->header().IsSplitting())     
		{	      
		  el->setInherentlySelected(false);
		  TmpLiteral* tmp = el->previous();
		  lits.remove(el);
		  nonselectedLits.push(el);
		  if (tmp) { el = tmp->next(); }
		  else
		    el = lits.first();
		}
	      else
		el = el->next();
	    };


	  ASSERT(lits.nonempty());
	  if (lits.numOfAllLiterals() == 1) 
	    {
	      selectedLits.push(lits.pop());
	      negSelectionUsed = false;
	    }
	  else // lits.numOfAllLiterals() > 1
	    { 
              TmpLitList::Statistics newStat;
	      lits.collectStatistics(newStat,_inversePolarity);	  
	      if (newStat.inherently_selected) 
		{
		  selectInherentlySelected(lits,newStat.inherently_selected,selectedLits,nonselectedLits);
		  return;  
		};
	      _basicSelFun(lits,selectedLits,nonselectedLits,negSelectionUsed,newStat); 
	    };

	}
      else // lits contains only positive splitting branches, select the youngest among them
	{
	  TmpLiteral* youngestPosBranch = lits.pop();
	  ASSERT(isPositiveForSelection(youngestPosBranch->header()));
	  ASSERT(youngestPosBranch->header().IsSplitting());
	  TmpLiteral* currLit;
	  do 
	    {
	      currLit = lits.pop();
	      ASSERT(isPositiveForSelection(currLit->header()));
	      ASSERT(currLit->header().IsSplitting());
	      if (currLit->header().functor() > youngestPosBranch->header().functor())
		{
		  youngestPosBranch->setInherentlySelected(false);
		  nonselectedLits.push(youngestPosBranch);
		  youngestPosBranch = currLit;
		}
	      else 
		{
		  currLit->setInherentlySelected(false);
		  nonselectedLits.push(currLit);
		};
	    }     
	  while (lits.nonempty());
	  negSelectionUsed = false;
	  selectedLits.push(youngestPosBranch);
	};
    };
};  // void LiteralSelection::selectWithBlockingSplitting(..)



inline 
void 
LiteralSelection::selectInherentlySelected(TmpLitList& lits,ulong numOfSelected,
					   TmpLitList& selectedLits,
					   TmpLitList& nonselectedLits)
{
  CALL("selectInherentlySelected(TmpLitList& lits,ulong numOfSelected,TmpLitList& selectedLits,TmpLitList& nonselectedLits)");
  ASSERT(!lits.containsAnswerLiterals());
  ASSERT(numOfSelected); 
  ASSERT(lits.numOfAllLiterals() >= numOfSelected);
  do
    {
      ASSERT(lits.numOfAllLiterals() >= numOfSelected);
      TmpLiteral* lit = lits.pop();
      if (lit->isInherentlySelected()) { selectedLits.push(lit); numOfSelected--; }
      else nonselectedLits.push(lit);    
    }
  while (numOfSelected);
 
  while (lits.nonempty()) 
    {
#ifdef DEBUG_NAMESPACE
      TmpLiteral* lit = lits.pop();
      ASSERT(!lit->isInherentlySelected());
      nonselectedLits.push(lit); 
#else  
      nonselectedLits.push(lits.pop()); 
#endif
    };
 
}; // void LiteralSelection::selectInherentlySelected(TmpLitList& lits,ulong numOfSelected,TmpLitList& selectedLits,TmpLitList& nonselectedLits)



inline 
bool 
LiteralSelection::satisfactorySelection1(TmpLitList& selectedLits,TmpLitList& nonselectedLits) const
{
  CALL("satisfactorySelection1(TmpLitList& selectedLits,TmpLitList& nonselectedLits) const");
  ASSERT(!selectedLits.containsAnswerLiterals());
  if (selectedLits.numOfAllLiterals() > 1UL) return false;
  ASSERT(selectedLits.numOfAllLiterals() == 1UL); 
 
  TmpLiteral* maxLit = selectedLits.first(); 
  ulong maxLitWeight = maxLit->size();
  for (const TmpLiteral* el = nonselectedLits.last();
       el;
       el = el->previous())
    {
      if ((!el->isAnswerLiteral()) &&
	  isNegativeForSelection(el->header()) && 
	  (el->size() > maxLitWeight)) 
	return false;  
    };
  return true;
}; // bool LiteralSelection::satisfactorySelection1(TmpLitList& selectedLits,TmpLitList& nonselectedLits)


inline 
bool 
LiteralSelection::satisfactorySelection2(TmpLitList& selectedLits,TmpLitList& nonselectedLits) const
{
  CALL("satisfactorySelection2(TmpLitList& selectedLits,TmpLitList& nonselectedLits) const");
  ASSERT(!selectedLits.containsAnswerLiterals());
  if (selectedLits.numOfAllLiterals() > 1) return false;
  ASSERT(selectedLits.numOfAllLiterals() == 1UL); 
  TmpLiteral* maxLit = selectedLits.first();
  ulong maxLitWeight = maxLit->size();
  if (isPositiveForSelection(maxLit->header()))
    {
      if (maxLit->isEquality()) return false;
      // maxLit is a positive and is not equality
      for (const TmpLiteral* el = nonselectedLits.last();  
	   el;
	   el = el->previous())
	{
	  if ((!el->isAnswerLiteral()) &&
	      (isNegativeForSelection(el->header())))
	    {
	      if (el->isEquality() || 
		  (el->size() < maxLitWeight)) 
		return false;
	    };
	};
    }
  else // !isPositiveForSelection(maxLit->Header())
    return false;
  return true;
}; // bool LiteralSelection::satisfactorySelection2(TmpLitList& selectedLits,TmpLitList& nonselectedLits)



inline 
bool 
LiteralSelection::satisfactorySelection3(TmpLitList& selectedLits,TmpLitList& nonselectedLits) const
{
  CALL("satisfactorySelection3(TmpLitList& selectedLits,TmpLitList& nonselectedLits) const");
  ASSERT(!selectedLits.containsAnswerLiterals());
  if (selectedLits.numOfAllLiterals() > 1UL) return false;
  ASSERT(selectedLits.numOfAllLiterals() == 1UL); 
  TmpLiteral* maxLit = selectedLits.first();
  if (isPositiveForSelection(maxLit->header()))
    {
      if (maxLit->isEquality()) return false;
      // maxLit is a positive and is not equality
      ulong numOfDiffVars = maxLit->numOfDifferentVariables();
      static TmpLiteral::VarDepthProfile maxVarDepthProfile;
      maxLit->collectVarDepthProfile(maxVarDepthProfile);
      ulong maxLitWeight = maxLit->size();

      for (TmpLiteral* el = nonselectedLits.last();
	   el;
	   el = el->previous())
	{
	  if ((!el->isAnswerLiteral()) && 
	      (isNegativeForSelection(el->header())))
	    {
	      if (el->numOfDifferentVariables() > numOfDiffVars) return false;
	      if (el->numOfDifferentVariables() == numOfDiffVars)
		{
		  // compare the depths of variable occurences
		  static TmpLiteral::VarDepthProfile varDepthProfile;
		  el->collectVarDepthProfile(varDepthProfile);
		  switch (maxVarDepthProfile.compareByInverselyInducedOrder(varDepthProfile))
		    {
		    case Less: return false;
		    case Equal: 
		      // compare the sizes
		      if (maxLitWeight < el->size()) return false;
		      break;
#ifdef DEBUG_NAMESPACE
		    case Greater: break; 
		    default: ICP("ICP0"); break;
#else 
#ifdef _SUPPRESS_WARNINGS_
		    default: break;
#endif 
#endif         
		    };
		};
	    };
	};
    }
  else // !isPositiveForSelection(maxLit->Header())
    return false;
  return true;
}; // bool LiteralSelection::satisfactorySelection3(TmpLitList& selectedLits,TmpLitList& nonselectedLits)



inline 
void 
LiteralSelection::selectNegativeWithMaximalWeight(TmpLitList& lits,TmpLitList& selectedLits,TmpLitList& nonselectedLits) const
{
  CALL("selectNegativeWithMaximalWeight(TmpLitList& lits,TmpLitList& selectedLits,TmpLitList& nonselectedLits) const");

  // lits contains at least 1 negative non-answer literal
  // find first negative literal
  TmpLiteral* cand = lits.pop();
  while (isPositiveForSelection(cand->header()) || 
	 cand->isAnswerLiteral())
    {         
      nonselectedLits.push(cand);
      ASSERT(lits.nonempty());
      cand = lits.pop(); 
    };
  ulong cand_weight = cand->size();
  TmpLiteral* next_lit;
  while (lits.nonempty())
    {
      next_lit = lits.pop();
      if (isNegativeForSelection(next_lit->header()) && 
	  (!next_lit->isAnswerLiteral()) && 
	  (next_lit->size() > cand_weight)) 
	{
	  nonselectedLits.push(cand);
	  cand = next_lit;
	  cand_weight = cand->size();
	}
      else nonselectedLits.push(next_lit);
    };
  selectedLits.push(cand);    
}; // void LiteralSelection::selectNegativeWithMaximalWeight(TmpLitList& lits,TmpLitList& selectedLits,TmpLitList& nonselectedLits)



inline 
void 
LiteralSelection::selectNegativeLeastProlific(TmpLitList& lits,TmpLitList& selectedLits,TmpLitList& nonselectedLits) const
{
  CALL("selectNegativeLeastProlific(TmpLitList& lits,TmpLitList& selectedLits,TmpLitList& nonselectedLits) const");

  // lits contains at least 1 negative non-answer literal
  // find first negative literal
  TmpLiteral* cand = lits.pop();
  while (isPositiveForSelection(cand->header()) ||
	 cand->isAnswerLiteral())
    {         
      nonselectedLits.push(cand);
      ASSERT(lits.nonempty());
      cand = lits.pop(); 
    };

  TmpLiteral* next_lit;
  while (lits.nonempty())
    {
      next_lit = lits.pop();
      if (isNegativeForSelection(next_lit->header()) && 
	  (!next_lit->isAnswerLiteral()) && 
	  (next_lit->preferredNegative(*cand)))  
	{
	  nonselectedLits.push(cand);
	  cand = next_lit;
	}
      else nonselectedLits.push(next_lit);
    };
  selectedLits.push(cand);     
}; // void LiteralSelection::selectNegativeLeastProlific(TmpLitList& lits,TmpLitList& selectedLits,TmpLitList& nonselectedLits)  



inline 
void 
LiteralSelection::selectNegativeMaximallyNongroundWithMaximalWeight(TmpLitList& lits,TmpLitList& selectedLits,TmpLitList& nonselectedLits) const
{
  CALL("selectNegativeMaximallyNongroundWithMaximalWeight(TmpLitList& lits,TmpLitList& selectedLits,TmpLitList& nonselectedLits) const");
  
  // lits may contain answer literals! 
  // First, get rid of all positive and answer literals.
 
  TmpLiteral* el = lits.first();
  while (el)
    {
      if (isPositiveForSelection(el->header()) ||
	  el->isAnswerLiteral())
	{	      
	  el->setInherentlySelected(false);
	  TmpLiteral* tmp = el->previous();
	  lits.remove(el);
	  nonselectedLits.push(el);
	  if (tmp) { el = tmp->next(); }
	  else
	    el = lits.first();
	}
      else
	el = el->next();
    };
 
  // lits contains at least 1 negative literal
  ASSERT(lits.numOfAllLiterals());
  if (lits.numOfAllLiterals() == 1) { selectedLits.push(lits.pop()); return; };
  ulong maxNumOfDiffVars = lits.last()->numOfDifferentVariables();

  // get rid of the literals that do not have the maximal number of different variables

  el = lits.last();
  while (el)
    {
      // compare the number of different variables 
      ulong numOfDiffVars = el->numOfDifferentVariables();
      if (numOfDiffVars > maxNumOfDiffVars)
	{
	  maxNumOfDiffVars = numOfDiffVars;
	  do { nonselectedLits.push(lits.pop()); } while (lits.last() != el); 
	  el = el->previous();	  
	}
      else 
	if (numOfDiffVars < maxNumOfDiffVars)
	  {
	    TmpLiteral* tmp = el->next();
	    lits.remove(el);
	    nonselectedLits.push(el);
	    if (tmp) { el = tmp->previous(); }
	    else
	      el = lits.last();
	  }
	else
	  el = el->previous();
    };


  // now all the literals have the maximal number of different variables 
  if (lits.numOfAllLiterals() == 1) { selectedLits.push(lits.pop()); return; }; 

  // additionally, compare the multisets of depths of variable occurences    
 
  static TmpLiteral::VarDepthProfile varDepthProfile1;
  static TmpLiteral::VarDepthProfile varDepthProfile2;
  TmpLiteral::VarDepthProfile* maxVarDepthProfile = &varDepthProfile1;
  TmpLiteral::VarDepthProfile* litVarDepthProfile = &varDepthProfile2; 
  lits.last()->collectVarDepthProfile(*maxVarDepthProfile);

  for (el = lits.last()->previous();
       el;
       el = el->previous())
    {
      ASSERT(el->next());
      el->collectVarDepthProfile(*litVarDepthProfile); 
      // compare the profiles
      switch (maxVarDepthProfile->compareByInverselyInducedOrder(*litVarDepthProfile))
	{
	case Less:
	  maxVarDepthProfile = litVarDepthProfile;

	  litVarDepthProfile = ((litVarDepthProfile == &varDepthProfile1) ? &varDepthProfile2 : &varDepthProfile1);
	  do { nonselectedLits.push(lits.pop()); } while (lits.last() != el);  
	  break;   

	case Greater :  
	  {
	    TmpLiteral* tmp = el->next();
	    ASSERT(tmp);
	    lits.remove(el);
	    nonselectedLits.push(el);
	    el = tmp;
	  };
	  break;

	case Equal : break;
     
#ifdef DEBUG_NAMESPACE
	default : ICP("ICP0"); return;
#else
#ifdef _SUPPRESS_WARNINGS_
	default: RuntimeError::report("Illegal control point in  LiteralSelection::selectNegativeMaximallyNongroundWithMaximalWeight(..)");
	  return;
#endif
#endif 
	};
    };

  if (lits.numOfAllLiterals() == 1) { selectedLits.push(lits.pop()); return; }; 

  // select literals with greater size

  long maxSize = lits.last()->size();
  for (el = lits.last()->previous();
       el;
       el = el->previous())
    {
      ASSERT(el->next());
      long litSize = el->size();
      if (litSize > maxSize)
	{
	  maxSize = litSize;
	  do { nonselectedLits.push(lits.pop()); } while (lits.last() != el);     
	}
      else 
	if (litSize < maxSize)
	  {
	    TmpLiteral* tmp = el->next();
	    ASSERT(tmp);
	    lits.remove(el);
	    nonselectedLits.push(el);
	    el = tmp;
	  };
    };
  // now all the literals in lits are of the greatest size 
  if (lits.numOfAllLiterals() == 1) { selectedLits.push(lits.pop()); return; };
  
  // now select on "don't care" basis
  selectedLits.push(lits.pop());
  while (lits.nonempty()) nonselectedLits.push(lits.pop());
 
}; // void LiteralSelection::selectNegativeMaximallyNongroundWithMaximalWeight(TmpLitList& lits,TmpLitList& selectedLits,TmpLitList& nonselectedLits)



inline 
void 
LiteralSelection::unselectAnswerLiterals(TmpLitList& lits,
					 TmpLitList& selectedLits,
					 TmpLitList& nonselectedLits,
					 ulong numOfAnswerLits) const
{
  CALL("unselectAnswerLiterals(..)");
  
  TmpLiteral* el = lits.first();
  while (numOfAnswerLits)
    {
      ASSERT(el);
      if (el->isAnswerLiteral())
	{
	  TmpLiteral* tmp = el->previous();
	  lits.remove(el);
	  nonselectedLits.push(el);
	  if (tmp) { el = tmp->next(); }
	  else
	    el = lits.first();
          numOfAnswerLits--;
	}
      else
	el = el->next();
    };

}; // void LiteralSelection::unselectAnswerLiterals(..)




#ifdef VKERNEL_FOR_STEP_RESOLUTION

inline
bool 
LiteralSelection::isNextStepLiteral(const TmpLiteral* lit)
{
  return lit->arity() && 
    (!lit->isEquality()) && 
    lit->flatterm()->next()->arity();
};


inline
bool 
LiteralSelection::isCurrentStepLiteral(const TmpLiteral* lit)
{
  return lit->arity() && 
    (!lit->isEquality()) && 
    (!lit->flatterm()->next()->arity());
};

void 
inline
LiteralSelection::unselectCurrentStateLiterals(TmpLitList& lits,
					       TmpLitList& selectedLits,
					       TmpLitList& nonselectedLits) const
{
  CALL("unselectCurrentStateLiterals(TmpLitList& lits,...) const");
  
  TmpLiteral* el = lits.first();
  while (el)
    {
      if (isCurrentStepLiteral(el))
	{	  
	  TmpLiteral* tmp = el->previous();
	  lits.remove(el);
	  nonselectedLits.push(el);
	  if (tmp) { el = tmp->next(); }
	  else
	    el = lits.first();
	}
      else
	el = el->next();
    };
  
}; // LiteralSelection::unselectCurrentStateLiterals(TmpLitList& lits,...) const
#endif






bool 
LiteralSelection::selectForElimination(TmpLitList& lits,TmpLitList& selectedLits,
                                       TmpLitList& nonselectedLits,  
                                       const VK::SymbolInfo* eliminationTable) const
{
  CALL("selectForElimination(..) const");
  TmpLiteral* el = lits.last();
  while (el)
    {
      if (eliminationTable->isToBeEliminated(el->header().functor()))
	{
	  ASSERT(!el->isAnswerLiteral());	  
	  lits.remove(el);
	  selectedLits.push(el);
	  
	  while (lits.nonempty()) 
	    {
	      TmpLiteral* nonmax_lit = lits.pop(); 
	      nonmax_lit->setInherentlySelected(false);
	      ASSERT(!(eliminationTable->isToBeEliminated(nonmax_lit->header().functor())));        
	      nonselectedLits.push(nonmax_lit);
	    }
	  return true;
	};        
      
      el = el->previous();
    };
  return false; 
}; // bool LiteralSelection::selectForElimination(TmpLitList& lits,...





void 
LiteralSelection::function1(TmpLitList& lits,TmpLitList& selectedLits,TmpLitList& nonselectedLits,bool& negSelectionUsed,const TmpLitList::Statistics& stat)
{ 
  CALL("function1(TmpLitList& lits,TmpLitList& selectedLits,TmpLitList& nonselectedLits,bool& negSelectionUsed,const TmpLitList::Statistics& stat)");

  /*
    Selects all maximal literals.
  */

  ASSERT(!lits.containsAnswerLiterals());
  ASSERT(lits.numOfAllLiterals() > 1);
  ASSERT(!stat.inherently_selected);
  ASSERT(!lits.containsSplittingBranches());

  negSelectionUsed = false; 
  TmpLiteral* cand;
  TERM max_header; 
  if (_currentObject->_literalComparisonMode == 0L) goto compare_atoms;

  // filter out literals with nonmaximal headers
  max_header = lits.last()->header();
  TmpLiteral* el;
  for (el = lits.last()->previous();
       el;
       el = el->previous())
    {
      if ((_currentObject->_literalComparisonMode == 2L) 
	  && (el->header().functor() == max_header.functor()))
	{     
	  // do nothing
	}
      else
	{
	  switch (Signature::current()->compareHeaderPrecedences(el->header().polarity(),
							el->header().functor(),
							max_header.polarity(),
							max_header.functor())) 
	    {
	    case Greater :
	      max_header = el->header();
	      do 
		{ 
		  nonselectedLits.push(lits.pop()); 
		} 
	      while (lits.last() != el);
	      break;
 
	    case Less :
	      {
		TmpLiteral* tmp = el->next();
		ASSERT(tmp);
		lits.remove(el);
		nonselectedLits.push(el);
		el = tmp;
	      };
	      break;

	    case Equal :
	      ASSERT((el->header() == max_header) || 
		     (el->header().IsEquality() &&  
		      max_header.IsEquality() && 
		      (el->header().polarity() == max_header.polarity())));
	      break;

	    case Incomparable : 
#ifdef DEBUG_NAMESPACE
	      ICP("E0"); 
#endif
	      break; 
	    };
	};
    };
 
  // all remaining literals have the maximal header
  // or _currentObject->_literalComparisonMode == 0L     

 compare_atoms:
  ASSERT(lits.nonempty());

  cand = lits.pop();
  ASSERT((_currentObject->_literalComparisonMode != 1L) 
	 || (cand->header() == max_header) 
	 || (cand->header().IsEquality() 
	     &&  max_header.IsEquality() 
	     && (cand->header().polarity() == max_header.polarity()))); 
  ASSERT((_currentObject->_literalComparisonMode != 2L) 
         || (cand->header().functor() == max_header.functor())
         || (cand->header().IsEquality() 
	     &&  max_header.IsEquality())); 
  if (lits.empty()) { selectedLits.push(cand); return; };

  
  el = lits.last();
  while (el)
    {
      ASSERT((_currentObject->_literalComparisonMode != 1L) 
	     || (el->header() == max_header) 
	     || (el->header().IsEquality() 
		 &&  max_header.IsEquality() 
		 && (el->header().polarity() == max_header.polarity()))); 
      ASSERT((_currentObject->_literalComparisonMode != 2L) 
	     || (cand->header().functor() == max_header.functor())
	     || (cand->header().IsEquality() 
		 &&  max_header.IsEquality())); 
      
      switch (el->compare(*cand))
	{
	case Greater:
	  nonselectedLits.push(cand); 
	  goto compare_atoms;
	case Less:
	  {
	    TmpLiteral* tmp = el->next();
	    lits.remove(el);
	    nonselectedLits.push(el);
	    if (tmp) 
              {
                el = tmp->previous();
              }
            else
              el = lits.last(); 
	  };
	  break; 
	case Incomparable: 
	  el = el->previous();
	  break;

#ifdef DEBUG_NAMESPACE
    
	case Equal:
	  ICP("ICP0"); break;
	default: ICP("ICP1"); break;
#else  
#ifdef _SUPPRESS_WARNINGS_
        default: break;
#endif
#endif
	};
      
    };  
  selectedLits.push(cand);
  if (lits.empty()) return;
  goto compare_atoms;
  ASSERT(selectedLits.nonempty());
}; // void LiteralSelection::function1(TmpLitList& lits,TmpLitList& selectedLits,TmpLitList& nonselectedLits,bool& negSelectionUsed,const TmpLitList::Statistics& stat)


void 
LiteralSelection::function2(TmpLitList& lits,TmpLitList& selectedLits,TmpLitList& nonselectedLits,bool& negSelectionUsed,const TmpLitList::Statistics& stat)
{ 
  CALL("function2(TmpLitList& lits,TmpLitList& selectedLits,TmpLitList& nonselectedLits,bool& negSelectionUsed,const TmpLitList::Statistics& stat)");
 
  /* 
     If lits contains negative literals, one of them is selected.
     Negative literals with greater weights are prefered.
     If there is no negative literal, we proceed with selection 1.  
  */
 
  ASSERT(!lits.containsAnswerLiterals());
  ASSERT(lits.numOfAllLiterals() > 1);
  ASSERT(!stat.inherently_selected);
  ASSERT(!lits.containsSplittingBranches());

  if (stat.negative_nonanswer_lits)
    {
      negSelectionUsed = true;
      _currentObject->selectNegativeWithMaximalWeight(lits,selectedLits,nonselectedLits);
    }
  else // no negative literals
    { 
      function1(lits,selectedLits,nonselectedLits,negSelectionUsed,stat);
    };  
}; // void LiteralSelection::function2(TmpLitList& lits,TmpLitList& selectedLits,TmpLitList& nonselectedLits,bool& negSelectionUsed,const TmpLitList::Statistics& stat)

void 
LiteralSelection::function3(TmpLitList& lits,TmpLitList& selectedLits,TmpLitList& nonselectedLits,bool& negSelectionUsed,const TmpLitList::Statistics& stat)
{ 
  CALL("function3(TmpLitList& lits,TmpLitList& selectedLits,TmpLitList& nonselectedLits,bool& negSelectionUsed,const TmpLitList::Statistics& stat)");

  /*
    If lits contains negative literals, one of them is selected.
    To choose between negative literals we heuristically estimate the number of possible
    inferences and a literal promising less inferences is selected.
    If there is no negative literal, we proceed with selection 1. 
  */

  ASSERT(!lits.containsAnswerLiterals());
  ASSERT(lits.numOfAllLiterals() > 1);
  ASSERT(!stat.inherently_selected);
  ASSERT(!lits.containsSplittingBranches());

  if (stat.negative_nonanswer_lits)
    {
      negSelectionUsed = true;
      _currentObject->selectNegativeLeastProlific(lits,selectedLits,nonselectedLits);
    }
  else // no negative literals
    { 
      function1(lits,selectedLits,nonselectedLits,negSelectionUsed,stat);
    };    
}; // void LiteralSelection::function3(TmpLitList& lits,TmpLitList& selectedLits,TmpLitList& nonselectedLits,bool& negSelectionUsed,const TmpLitList::Statistics& stat)

void 
LiteralSelection::function4(TmpLitList& lits,TmpLitList& selectedLits,TmpLitList& nonselectedLits,bool& negSelectionUsed,const TmpLitList::Statistics& stat)
{ 
  CALL("function4(TmpLitList& lits,TmpLitList& selectedLits,TmpLitList& nonselectedLits,bool& negSelectionUsed,const TmpLitList::Statistics& stat)");
 
  /* 
     We first select maximal literals. Then we try to improve selection 
     by selecting a negative non-answer literal. This is based on the assumption that 
     selecting one literal is better than selecting several literals and 
     selecting a negative literal with greater weight is better than selecting 
     a maximal literal.
  */ 
 

  ASSERT(!lits.containsAnswerLiterals());
  ASSERT(lits.numOfAllLiterals() > 1);
  ASSERT(!stat.inherently_selected);
  ASSERT(!lits.containsSplittingBranches());

  function1(lits,selectedLits,nonselectedLits,negSelectionUsed,stat); 
  if (stat.negative_nonanswer_lits) // quality of selection can be questioned 
    {
      if (!_currentObject->satisfactorySelection1(selectedLits,nonselectedLits))
	{
	  lits.collectFrom(nonselectedLits);
	  lits.collectFrom(selectedLits);
	  _currentObject->selectNegativeWithMaximalWeight(lits,selectedLits,nonselectedLits);
	  negSelectionUsed = true;  
	};
    };
}; // void LiteralSelection::function4(TmpLitList& lits,TmpLitList& selectedLits,TmpLitList& nonselectedLits,bool& negSelectionUsed,const TmpLitList::Statistics& stat)


void 
LiteralSelection::function5(TmpLitList& lits,TmpLitList& selectedLits,TmpLitList& nonselectedLits,bool& negSelectionUsed,const TmpLitList::Statistics& stat)
{ 
  CALL("function5(TmpLitList& lits,TmpLitList& selectedLits,TmpLitList& nonselectedLits,bool& negSelectionUsed,const TmpLitList::Statistics& stat)");
  /* 
     We first select maximal literals. Then we try to improve selection by selecting
     a negative literal. This is based on the assumption that selecting one literal
     is better than selecting several literals and selecting a negative literal 
     with less possible inferences (heuristicaly estimated) is better than 
     selecting a maximal literal.
  */ 
 
  ASSERT(!lits.containsAnswerLiterals());
  ASSERT(lits.numOfAllLiterals() > 1);
  ASSERT(!stat.inherently_selected); 
  ASSERT(!lits.containsSplittingBranches());


  function1(lits,selectedLits,nonselectedLits,negSelectionUsed,stat);
  if (stat.negative_nonanswer_lits) // quality of selection can be questioned 
    {
      if (!_currentObject->satisfactorySelection2(selectedLits,nonselectedLits))
	{
	  lits.collectFrom(nonselectedLits);
	  lits.collectFrom(selectedLits);
	  _currentObject->selectNegativeLeastProlific(lits,selectedLits,nonselectedLits);
	  negSelectionUsed = true;
	};
    };
}; // void LiteralSelection::function5(TmpLitList& lits,TmpLitList& selectedLits,TmpLitList& nonselectedLits,bool& negSelectionUsed,const TmpLitList::Statistics& stat)



void 
LiteralSelection::function6(TmpLitList& lits,TmpLitList& selectedLits,TmpLitList& nonselectedLits,bool& negSelectionUsed,const TmpLitList::Statistics& stat)
{ 
  CALL("function6(TmpLitList& lits,TmpLitList& selectedLits,TmpLitList& nonselectedLits,bool& negSelectionUsed,const TmpLitList::Statistics& stat)");
 
  /* 
     If lits contains negative literals, one of them is selected.
     Among the negative literals we always select a literal 
     with the maximal number of different variables. Among such literals 
     we prefer those having smaller depths of variable occurences.
     Negative literals with greater weights are prefered.
     If there is no negative literal, we proceed with selection 1.  
  */

  ASSERT(!lits.containsAnswerLiterals());
  ASSERT(lits.numOfAllLiterals() > 1);
  ASSERT(!stat.inherently_selected);
  ASSERT(!lits.containsSplittingBranches());

  if (stat.negative_nonanswer_lits)
    {
      negSelectionUsed = true;
      _currentObject->selectNegativeMaximallyNongroundWithMaximalWeight(lits,selectedLits,nonselectedLits);
    }
  else // no negative literals
    { 
      function1(lits,selectedLits,nonselectedLits,negSelectionUsed,stat);
    };  
}; // void LiteralSelection::function6(TmpLitList& lits,TmpLitList& selectedLits,TmpLitList& nonselectedLits,bool& negSelectionUsed,const TmpLitList::Statistics& stat)


void 
LiteralSelection::function7(TmpLitList& lits,TmpLitList& selectedLits,TmpLitList& nonselectedLits,bool& negSelectionUsed,const TmpLitList::Statistics& stat)
{ 
  CALL("function7(TmpLitList& lits,TmpLitList& selectedLits,TmpLitList& nonselectedLits,bool& negSelectionUsed,const TmpLitList::Statistics& stat)");
 
  /*  
      We first select maximal literals. Then we try to improve selection 
      by selecting a negative literal. This is based on the assumption that 
      selecting one literal is better than selecting several literals and 
      selecting a negative literal with more different variables is 
      better than selecting 
      a maximal literal. If we decide to use negative selection, among 
      the negative literals
      we chose those having more different variable. Among these 
      literals we chose 
      those with smaller depths of variable occurences and then 
      select those having greater size.   
  */ 
 
  ASSERT(!lits.containsAnswerLiterals());
  ASSERT(lits.numOfAllLiterals() > 1);
  ASSERT(!stat.inherently_selected);
  ASSERT(!lits.containsSplittingBranches());

  function1(lits,selectedLits,nonselectedLits,negSelectionUsed,stat); 
  if (stat.negative_nonanswer_lits) // quality of selection can be questioned 
    {
      if (!_currentObject->satisfactorySelection3(selectedLits,nonselectedLits))
	// satisfactorySelection1 used by mistake earlier
	{
	  lits.collectFrom(nonselectedLits);
	  lits.collectFrom(selectedLits);
	  _currentObject->selectNegativeMaximallyNongroundWithMaximalWeight(lits,selectedLits,nonselectedLits);
	  negSelectionUsed = true;  
	};
    };
}; // void LiteralSelection::function7(TmpLitList& lits,TmpLitList& selectedLits,TmpLitList& nonselectedLits,bool& negSelectionUsed,const TmpLitList::Statistics& stat)



void 
LiteralSelection::function1002(TmpLitList& lits,TmpLitList& selectedLits,TmpLitList& nonselectedLits,bool& negSelectionUsed,const TmpLitList::Statistics& stat)
{ 
  CALL("function1002(TmpLitList& lits,TmpLitList& selectedLits,TmpLitList& nonselectedLits,bool& negSelectionUsed,const TmpLitList::Statistics& stat)");
  /*
    Arbitrary selection (almost always makes everything incomplete). 
    If lits contains a literal which is not a positive equality,
    we never select a positive equality. 
    Among the literals available for selection we always select a literal 
    with the maximal number of different variables. Among such literals we prefer those having 
    smaller depths of variable occurences. Among such literals we select those with maximal size.
  */

  ASSERT(!lits.containsAnswerLiterals());
  ASSERT(lits.numOfAllLiterals() > 1);
  ASSERT(!stat.inherently_selected);
  ASSERT(!lits.containsSplittingBranches());
 
  bool selectPositiveEq = (!stat.negative_nonanswer_lits) && (!stat.noneq_nonanswer_lits);

  if (!selectPositiveEq)
    {
      while (lits.last()->isEquality() && 
	     lits.last()->isPositive())
	nonselectedLits.push(lits.pop());
    };
  
  ASSERT(lits.numOfAllLiterals());
  if (lits.numOfAllLiterals() == 1) { selectedLits.push(lits.pop()); return; };
 
  ulong maxNumOfDiffVars = lits.last()->numOfDifferentVariables();

  TmpLiteral* el;
  for (el = lits.last()->previous();
       el;
       el = el->previous())
    {
      ASSERT(el->next());
      if ((!selectPositiveEq) && 
	  (el->isEquality()) && 
	  (el->isPositive()))
	{
	  TmpLiteral* tmp = el->next();
	  ASSERT(tmp);
	  lits.remove(el);
	  nonselectedLits.push(el);
	  el = tmp;   
	}
      else
	{
	  // compare the number of different variables 
	  ulong numOfDiffVars = el->numOfDifferentVariables();
	  if (numOfDiffVars > maxNumOfDiffVars)
	    {
	      maxNumOfDiffVars = numOfDiffVars;
	      do 
		{ 
		  nonselectedLits.push(lits.pop()); 
		} 
	      while (lits.last() != el);     
	    }
	  else 
	    if (numOfDiffVars < maxNumOfDiffVars)
	      {
		TmpLiteral* tmp = el->next();
		ASSERT(tmp);
		lits.remove(el);
		nonselectedLits.push(el);
		el = tmp;
	      };
	};
    };
  // now all the literals have the maximal number of different variables 
  ASSERT(lits.numOfAllLiterals());
  if (lits.numOfAllLiterals() == 1) { selectedLits.push(lits.pop()); return; };

  // additionally, compare the multisets of depths of variable occurences    
 
  static TmpLiteral::VarDepthProfile varDepthProfile1;
  static TmpLiteral::VarDepthProfile varDepthProfile2;
  TmpLiteral::VarDepthProfile* maxVarDepthProfile = &varDepthProfile1;
  TmpLiteral::VarDepthProfile* litVarDepthProfile = &varDepthProfile2; 
  lits.last()->collectVarDepthProfile(*maxVarDepthProfile);
  for (el = lits.last()->previous();
       el;
       el = el->previous())
    {
      ASSERT(el->next());
      ASSERT(selectPositiveEq || (!el->isEquality()) || el->isNegative());
      el->collectVarDepthProfile(*litVarDepthProfile); 
      // compare the profiles
      switch (maxVarDepthProfile->compareByInverselyInducedOrder(*litVarDepthProfile))
	{
	case Less :
	  maxVarDepthProfile = litVarDepthProfile;
	  litVarDepthProfile = ((litVarDepthProfile == &varDepthProfile1) ? &varDepthProfile2 : &varDepthProfile1);
	  do { nonselectedLits.push(lits.pop()); } while (lits.last() != el);  
	  break;   

	case Greater :  
	  {
	    TmpLiteral* tmp = el->next();
	    ASSERT(tmp);
	    lits.remove(el);
	    nonselectedLits.push(el);
	    el = tmp;
	  };
	  break;

	case Equal : break;
     
#ifdef DEBUG_NAMESPACE
	default : ICP("ICP0"); return;
#else
#ifdef _SUPPRESS_WARNINGS_
	default: RuntimeError::report("Illegal control point in  LiteralSelection::function1002(..)");
	  return;
#endif
#endif 
	};
    };


  ASSERT(lits.numOfAllLiterals());
  if (lits.numOfAllLiterals() == 1) { selectedLits.push(lits.pop()); return; };

  // select literals with greater size 
  long maxSize = lits.last()->size();
  for (el = lits.last()->previous();
       el;
       el = el->previous())
    {
      ASSERT(el->next());
      ASSERT(selectPositiveEq || (!el->isEquality()) || el->isNegative());

      long litSize = el->size();
      if (litSize > maxSize)
	{
	  maxSize = litSize;
	  do { nonselectedLits.push(lits.pop()); } while (lits.last() != el);     
	}
      else 
	if (litSize < maxSize)
	  {
	    TmpLiteral* tmp = el->next();
	    ASSERT(tmp);
	    lits.remove(el);
	    nonselectedLits.push(el);
	    el = tmp;
	  };
    };
  // now all the literals in lits are of the greatest size 
  ASSERT(lits.numOfAllLiterals());
  if (lits.numOfAllLiterals() == 1) { selectedLits.push(lits.pop()); return; };
  
  // now select on "don't care" basis
  selectedLits.push(lits.pop());
  while (lits.nonempty()) nonselectedLits.push(lits.pop());
 
}; // void LiteralSelection::function1002(TmpLitList& lits,TmpLitList& selectedLits,TmpLitList& nonselectedLits,bool& negSelectionUsed,const TmpLitList::Statistics& stat)


void 
LiteralSelection::function1003(TmpLitList& lits,TmpLitList& selectedLits,TmpLitList& nonselectedLits,bool& negSelectionUsed,const TmpLitList::Statistics& stat)
{ 
  CALL("function1003(TmpLitList& lits,TmpLitList& selectedLits,TmpLitList& nonselectedLits,bool& negSelectionUsed,const TmpLitList::Statistics& stat)");
  /*
    Arbitrary selection (almost always makes everything incomplete). 
    If lits contains a literal which is not a positive equality,
    we never select a positive equality. 
    Among the literals available for selection we always select a literal 
    of the maximal size. Among several such literals we select by the function 1002.        
  */

  ASSERT(!lits.containsAnswerLiterals());
  ASSERT(lits.numOfAllLiterals() > 1);
  ASSERT(!stat.inherently_selected);
  ASSERT(!lits.containsSplittingBranches());
 
  bool selectPositiveEq = (!stat.negative_nonanswer_lits) && (!stat.noneq_nonanswer_lits);


  if (!selectPositiveEq)
    {
      while (lits.last()->isEquality() && 
	     lits.last()->isPositive())
	nonselectedLits.push(lits.pop());
    };
 
  long maxSize = lits.last()->size();
  for (TmpLiteral* el = lits.last()->previous();
       el;
       el = el->previous())
    {
      ASSERT(el->next());
      if ((!selectPositiveEq) && 
	  (el->isEquality()) && 
	  (el->isPositive()))
	{
	  TmpLiteral* tmp = el->next();
	  ASSERT(tmp);
	  lits.remove(el);
	  nonselectedLits.push(el);
	  el = tmp;
	}
      else
	{
	  long litSize = el->size();
	  if (litSize > maxSize)
	    {
	      maxSize = litSize;
	      do { nonselectedLits.push(lits.pop()); } while (lits.last() != el);     
	    }
	  else 
	    if (litSize < maxSize)
	      {
		TmpLiteral* tmp = el->next();
		ASSERT(tmp);
		lits.remove(el);
		nonselectedLits.push(el);
		el = tmp;
	      };
	};
    };
  // now all the literals in lits are of the greatest size 
  ASSERT(lits.numOfAllLiterals());
  if (lits.numOfAllLiterals() == 1) { selectedLits.push(lits.pop()); return; };
  TmpLitList::Statistics newStat;
  lits.collectStatistics(newStat,_currentObject->_inversePolarity);
 
  function1002(lits,selectedLits,nonselectedLits,negSelectionUsed,newStat);

}; // void LiteralSelection::function1003(TmpLitList& lits,TmpLitList& selectedLits,TmpLitList& nonselectedLits,bool& negSelectionUsed,const TmpLitList::Statistics& stat)




void 
LiteralSelection::function1004(TmpLitList& lits,TmpLitList& selectedLits,TmpLitList& nonselectedLits,bool& negSelectionUsed,const TmpLitList::Statistics& stat)
{ 
  CALL("function1004(TmpLitList& lits,TmpLitList& selectedLits,TmpLitList& nonselectedLits,bool& negSelectionUsed,const TmpLitList::Statistics& stat)");
  /*
    Arbitrary selection (almost always makes everything incomplete). 
    If lits contains a literal which is not a positive equality,
    we never select a positive equality. 
    Among the literals available for selection we always select a literal 
    with the minimal number of different variables.
    Among such literals we select those having minimal total number of variable
    occurences. Among such literals we select one of the greatest size. 
  */

  ASSERT(!lits.containsAnswerLiterals());
  ASSERT(lits.numOfAllLiterals() > 1);
  ASSERT(!stat.inherently_selected);
  ASSERT(!lits.containsSplittingBranches());
 
 
  bool selectPositiveEq = (!stat.negative_nonanswer_lits) && (!stat.noneq_nonanswer_lits);

  if (!selectPositiveEq)
    {
      while (lits.last()->isEquality() && lits.last()->isPositive())
	nonselectedLits.push(lits.pop());
    };
 
  ASSERT(lits.numOfAllLiterals());
  if (lits.numOfAllLiterals() == 1) { selectedLits.push(lits.pop()); return; };
 
  ulong minNumOfDiffVars = lits.last()->numOfDifferentVariables();
  TmpLiteral* el;
  for (el = lits.last()->previous();
       el;
       el = el->previous())
    {
      ASSERT(el->next());
      if ((!selectPositiveEq) && 
	  (el->isEquality()) && 
	  (el->isPositive()))
	{
	  TmpLiteral* tmp = el->next();
	  ASSERT(tmp);
	  lits.remove(el);
	  nonselectedLits.push(el);
	  el = tmp;
	}
      else
	{
	  // compare the number of different variables 
	  ulong numOfDiffVars = el->numOfDifferentVariables();
	  if (numOfDiffVars < minNumOfDiffVars)
	    {
	      minNumOfDiffVars = numOfDiffVars;
	      do { nonselectedLits.push(lits.pop()); } while (lits.last() != el);     
	    }
	  else 
	    if (numOfDiffVars > minNumOfDiffVars)
	      {
		TmpLiteral* tmp = el->next();
		ASSERT(tmp);
		lits.remove(el);
		nonselectedLits.push(el);
		el = tmp;
	      };
	};
    };
  // now all the literals have the minimal number of different variables 
  ASSERT(lits.numOfAllLiterals());
  if (lits.numOfAllLiterals() == 1) { selectedLits.push(lits.pop()); return; };

  // compare the total number of variable occurences  
  TermWeightType  minNumOfVarOccurences =  lits.last()->numberOfVariableOccurences();
   
  for (el = lits.last()->previous();
       el;
       el = el->previous())
    {
      ASSERT(el->next());
      ASSERT(selectPositiveEq || (!el->isEquality()) || el->isNegative());

      // compare the number of variable occurences 
      TermWeightType numOfVarOccurences = el->numberOfVariableOccurences();
      if (numOfVarOccurences < minNumOfVarOccurences)
	{
	  minNumOfVarOccurences = numOfVarOccurences;
	  do { nonselectedLits.push(lits.pop()); } while (lits.last() != el);     
	}
      else 
	if (numOfVarOccurences > minNumOfVarOccurences)
	  {
	    TmpLiteral* tmp = el->next();
	    ASSERT(tmp);
	    lits.remove(el);
	    nonselectedLits.push(el);
	    el = tmp;
	  };
    };
  // now all the literals have the minimal number of variable occurences 
  ASSERT(lits.numOfAllLiterals());
  if (lits.numOfAllLiterals() == 1) { selectedLits.push(lits.pop()); return; };
 
  // now select literals with maximal size
  long maxSize = lits.last()->size();
  for (el = lits.last()->previous();
       el;
       el = el->previous())
    {
      ASSERT(el->next());
      ASSERT(selectPositiveEq || (!el->isEquality()) || el->isNegative());
      long litSize = el->size();
      if (litSize > maxSize)
	{
	  maxSize = litSize;
	  do { nonselectedLits.push(lits.pop()); } while (lits.last() != el);     
	}
      else 
	if (litSize < maxSize)
	  {
	    TmpLiteral* tmp = el->next();
	    ASSERT(tmp);
	    lits.remove(el);
	    nonselectedLits.push(el);
	    el = tmp;
	  };
    };
  // now all the literals in lits are of the greatest size 
  ASSERT(lits.numOfAllLiterals());
  if (lits.numOfAllLiterals() == 1) { selectedLits.push(lits.pop()); return; };
 
  // select on "don't care" basis 

  selectedLits.push(lits.pop());
  while (lits.nonempty()) nonselectedLits.push(lits.pop());
 
}; // void LiteralSelection::function1004(TmpLitList& lits,TmpLitList& selectedLits,TmpLitList& nonselectedLits,bool& negSelectionUsed,const TmpLitList::Statistics& stat)



void 
LiteralSelection::function1005(TmpLitList& lits,TmpLitList& selectedLits,TmpLitList& nonselectedLits,bool& negSelectionUsed,const TmpLitList::Statistics& stat)
{ 
  CALL("function1005(TmpLitList& lits,TmpLitList& selectedLits,TmpLitList& nonselectedLits,bool& negSelectionUsed,const TmpLitList::Statistics& stat)");
  /*
    Arbitrary selection (almost always makes everything incomplete). 
    If lits contains a literal which is not a positive equality,
    we never select a positive equality. 
    Among the literals available for selection we always select
    either a "maximally ground" negative literal, or a "maximally nonground" positive literal. 
    "Maximally ground" means having minimal total number of variable occurences 
    among literals having minimal number of different variables. "Maximally non ground" 
    means having maximal total number of variable occurences among the literals 
    having maximal number of different variables. 
    Among such literals we select one with the maximal size.
  */

  ASSERT(!lits.containsAnswerLiterals());
  ASSERT(lits.numOfAllLiterals() > 1);
  ASSERT(!stat.inherently_selected);
  ASSERT(!lits.containsSplittingBranches());
 
  bool selectPositiveEq = (!stat.negative_nonanswer_lits) && (!stat.noneq_nonanswer_lits);

  if (!selectPositiveEq)
    {
      while (lits.last()->isEquality() && 
	     lits.last()->isPositive())
	nonselectedLits.push(lits.pop());
    };

  ASSERT(lits.numOfAllLiterals());
  if (lits.numOfAllLiterals() == 1) { selectedLits.push(lits.pop()); return; };

  ulong minNegLitNumOfDiffVars;
  TermWeightType minNegLitNumOfVarOccurences;
  ulong maxPosLitNumOfDiffVars;
  TermWeightType maxPosLitNumOfVarOccurences;
  if (_currentObject->isNegativeForSelection(lits.last()->header())) 
    {
      minNegLitNumOfDiffVars = lits.last()->numOfDifferentVariables();
      minNegLitNumOfVarOccurences = lits.last()->numberOfVariableOccurences(); 
      maxPosLitNumOfDiffVars = 0;
      maxPosLitNumOfVarOccurences = 0;
    }
  else
    {
      minNegLitNumOfDiffVars = VampireKernelConst::MaxNumOfVariables;
      minNegLitNumOfVarOccurences = VampireKernelConst::MaxTermSize;
      maxPosLitNumOfDiffVars = lits.last()->numOfDifferentVariables(); 
      maxPosLitNumOfVarOccurences = lits.last()->numberOfVariableOccurences(); 
    }; 
  TmpLiteral* el;
  for (el = lits.last()->previous();
       el;
       el = el->previous())
    {
      ASSERT(el->next());
      if ((!selectPositiveEq) && (el->isEquality()) && (el->isPositive()))
	{
	  TmpLiteral* tmp = el->next();
	  ASSERT(tmp);
	  lits.remove(el);
	  nonselectedLits.push(el);
	  el = tmp; 
	}
      else
	{
	  if (_currentObject->isNegativeForSelection(el->header()))
	    {
	      ulong numOfDiffVars = el->numOfDifferentVariables();
	      if (numOfDiffVars < minNegLitNumOfDiffVars)
		{
		  minNegLitNumOfDiffVars = numOfDiffVars;
		  minNegLitNumOfVarOccurences = el->numberOfVariableOccurences();
         
		  TmpLiteral* anotherEl = lits.last();
		  while (anotherEl != el)
		    {
		      if (_currentObject->isNegativeForSelection(anotherEl->header()))
			{
			  ASSERT(anotherEl->numOfDifferentVariables() > numOfDiffVars);
			  TmpLiteral* tmp = anotherEl->next();
			  lits.remove(anotherEl);
			  nonselectedLits.push(anotherEl);
			  if (tmp)
			    {
			      anotherEl = tmp->previous();
			    }
			  else
			    anotherEl = lits.last();
			}
		      else
			anotherEl = anotherEl->previous();
		      
		    };
		}
	      else
		if (numOfDiffVars > minNegLitNumOfDiffVars)
		  {
		    TmpLiteral* tmp = el->next();
		    ASSERT(tmp);
		    lits.remove(el);
		    nonselectedLits.push(el);
		    el = tmp;
		  }
		else // numOfDiffVars == minNegLitNumOfDiffVars 
		  {
		    TermWeightType numOfVarOccurences = el->numberOfVariableOccurences();
		    if (numOfVarOccurences < minNegLitNumOfVarOccurences)
		      {
			minNegLitNumOfVarOccurences = numOfVarOccurences; 
			TmpLiteral* anotherEl = lits.last();
			while (anotherEl != el)
			  {
			    if (_currentObject->isNegativeForSelection(anotherEl->header()))
			      {
				ASSERT(anotherEl->numOfDifferentVariables() == minNegLitNumOfDiffVars);
				ASSERT(anotherEl->numberOfVariableOccurences() > numOfVarOccurences);
				
				TmpLiteral* tmp = anotherEl->next();
				lits.remove(anotherEl);
				nonselectedLits.push(anotherEl);
				if (tmp)
				  {
				    anotherEl = tmp->previous();
				  }
				else
				  anotherEl = lits.last();
			      }
			    else
			      anotherEl = anotherEl->previous();
			    
			  };
		      }
		    else
		      if (numOfVarOccurences > minNegLitNumOfVarOccurences)
			{
			  TmpLiteral* tmp = el->next();
			  ASSERT(tmp);
			  lits.remove(el);
			  nonselectedLits.push(el);
			  el = tmp;            
			};
		  };
	    }
	  else // !_currentObject->isNegativeForSelection(el->header())
	    {
	      ulong numOfDiffVars = el->numOfDifferentVariables();
	      if (numOfDiffVars > maxPosLitNumOfDiffVars)
		{
		  maxPosLitNumOfDiffVars = numOfDiffVars;
		  maxPosLitNumOfVarOccurences = el->numberOfVariableOccurences();

		  TmpLiteral* anotherEl = lits.last();
		  while (anotherEl != el)
		    {
		      if (!_currentObject->isNegativeForSelection(anotherEl->header()))
			{
			  ASSERT(anotherEl->numOfDifferentVariables() < numOfDiffVars);
			  
			  TmpLiteral* tmp = anotherEl->next();
			  lits.remove(anotherEl);
			  nonselectedLits.push(anotherEl);
			  if (tmp)
			    {
			      anotherEl = tmp->previous();
			    }
			  else
			    anotherEl = lits.last();
			}
		      else
			anotherEl = anotherEl->previous();
		      
		    };
		}
	      else
		if (numOfDiffVars < maxPosLitNumOfDiffVars)
		  {
		    TmpLiteral* tmp = el->next();
		    ASSERT(tmp);
		    lits.remove(el);
		    nonselectedLits.push(el);
		    el = tmp;
		  }
		else // numOfDiffVars == maxPosLitNumOfDiffVars 
		  {
		    TermWeightType numOfVarOccurences = el->numberOfVariableOccurences();
		    if (numOfVarOccurences > maxPosLitNumOfVarOccurences)
		      {
			maxPosLitNumOfVarOccurences = numOfVarOccurences; 

			TmpLiteral* anotherEl = lits.last();
			while (anotherEl != el)
			  {
			    if (!_currentObject->isNegativeForSelection(anotherEl->header()))
			      {
				ASSERT(anotherEl->numOfDifferentVariables() == maxPosLitNumOfDiffVars);
				ASSERT(anotherEl->numberOfVariableOccurences() < numOfVarOccurences);
				TmpLiteral* tmp = anotherEl->next();
				lits.remove(anotherEl);
				nonselectedLits.push(anotherEl);
				if (tmp)
				  {
				    anotherEl = tmp->previous();
				  }
				else
				  anotherEl = lits.last();
			      }
			    else
			      anotherEl = anotherEl->previous();
			  };

		      }
		    else
		      if (numOfVarOccurences < maxPosLitNumOfVarOccurences)
			{
			  TmpLiteral* tmp = el->next();
			  ASSERT(tmp);
			  lits.remove(el);
			  nonselectedLits.push(el);
			  el = tmp;
			};
		  };
	    };


	};
    };
  // now all negative literals are maximally ground
  // and all positive literals are maximally nonground
  ASSERT(lits.numOfAllLiterals());
  if (lits.numOfAllLiterals() == 1) { selectedLits.push(lits.pop()); return; }; 

  // now select literals with maximal size
  long maxSize = lits.last()->size();
  for (el = lits.last()->previous();
       el;
       el = el->previous())
    {
      ASSERT(selectPositiveEq || (!el->isEquality()) || el->isNegative());
      long litSize = el->size();
      if (litSize > maxSize)
	{
	  maxSize = litSize;
	  do { nonselectedLits.push(lits.pop()); } while (lits.last() != el);     
	}
      else 
	if (litSize < maxSize)
	  {
	    TmpLiteral* tmp = el->next();
	    ASSERT(tmp);
	    lits.remove(el);
	    nonselectedLits.push(el);
	    el = tmp;
	  };
    };
  // now all the literals in lits are of the greatest size 
  ASSERT(lits.numOfAllLiterals());
  if (lits.numOfAllLiterals() == 1) { selectedLits.push(lits.pop()); return; };
 
  // select on "don't care" basis 

  selectedLits.push(lits.pop());
  while (lits.nonempty()) nonselectedLits.push(lits.pop());   

}; // void LiteralSelection::function1005(TmpLitList& lits,TmpLitList& selectedLits,TmpLitList& nonselectedLits,bool& negSelectionUsed,const TmpLitList::Statistics& stat) 


void 
LiteralSelection::function1006(TmpLitList& lits,TmpLitList& selectedLits,TmpLitList& nonselectedLits,bool& negSelectionUsed,const TmpLitList::Statistics& stat)
{ 
  CALL("function1006(TmpLitList& lits,TmpLitList& selectedLits,TmpLitList& nonselectedLits,bool& negSelectionUsed,const TmpLitList::Statistics& stat)");
  /*
    Bidirectional selection. 
    In a rule, i.e. a clause containing both 
    positive and negative literals we always select one positive and one negative
    literal. If we have several literals of the same polarity, we select 
    a bigger of the maximally nonground ones. In a fact, i.e. a clause with all literals 
    of the same polarity, we select a bigger maximally nonground literal.     
  */

  ASSERT(!lits.containsAnswerLiterals());
  ASSERT(lits.numOfAllLiterals() > 1);
  ASSERT(!stat.inherently_selected);
  ASSERT(!lits.containsSplittingBranches()); 

  ulong maxNegLitNumOfDiffVars;
  TermWeightType maxNegLitNumOfVarOccurences;
  ulong maxNegLitSize;
  ulong maxPosLitNumOfDiffVars;
  TermWeightType maxPosLitNumOfVarOccurences;
  ulong maxPosLitSize;
  if (_currentObject->isNegativeForSelection(lits.last()->header())) 
    {
      maxNegLitNumOfDiffVars = lits.last()->numOfDifferentVariables();
      maxNegLitNumOfVarOccurences = lits.last()->numberOfVariableOccurences(); 
      maxNegLitSize = lits.last()->size(); 
      maxPosLitNumOfDiffVars = 0;
      maxPosLitNumOfVarOccurences = 0;
      maxPosLitSize = 0;
    }
  else
    {
      maxNegLitNumOfDiffVars = 0;
      maxNegLitNumOfVarOccurences = 0; 
      maxNegLitSize = 0;
      maxPosLitNumOfDiffVars = lits.last()->numOfDifferentVariables(); 
      maxPosLitNumOfVarOccurences = lits.last()->numberOfVariableOccurences(); 
      maxPosLitSize = lits.last()->size(); 
    }; 
 
  for (TmpLiteral* el = lits.last()->previous();
       el;
       el = el->previous())
    {
      ASSERT(el->next());
      if (_currentObject->isNegativeForSelection(el->header()))
	{
	  ulong numOfDiffVars = el->numOfDifferentVariables();
	  if (numOfDiffVars > maxNegLitNumOfDiffVars)
	    {
	      maxNegLitNumOfDiffVars = numOfDiffVars;
	      maxNegLitNumOfVarOccurences = el->numberOfVariableOccurences();
	      maxNegLitSize = el->size();

	      TmpLiteral* anotherEl = lits.last();
	      while (anotherEl != el)
		{
		  if (_currentObject->isNegativeForSelection(anotherEl->header()))
		    {
		      ASSERT(anotherEl->numOfDifferentVariables() < numOfDiffVars);
		      TmpLiteral* tmp = anotherEl->next();
		      lits.remove(anotherEl);
		      nonselectedLits.push(anotherEl);
		      if (tmp)
			{
			  anotherEl = tmp->previous();
			}
		      else
			anotherEl = lits.last();
		    }
		  else
		    anotherEl = anotherEl->previous();
		};
	    }
	  else
	    if (numOfDiffVars < maxNegLitNumOfDiffVars)
	      { 
		TmpLiteral* tmp = el->next();
		ASSERT(tmp);
		lits.remove(el);
		nonselectedLits.push(el);
		el = tmp;
	      }
	    else // numOfDiffVars == maxNegLitNumOfDiffVars 
	      {
		TermWeightType numOfVarOccurences = el->numberOfVariableOccurences();
		if (numOfVarOccurences > maxNegLitNumOfVarOccurences)
		  {
		    maxNegLitSize = el->size();
		    maxNegLitNumOfVarOccurences = numOfVarOccurences; 

		    TmpLiteral* anotherEl = lits.last();
		    while (anotherEl != el)
		      {
			if (_currentObject->isNegativeForSelection(anotherEl->header()))
			  {
			    ASSERT(anotherEl->numOfDifferentVariables() == maxNegLitNumOfDiffVars);
			    ASSERT(anotherEl->numberOfVariableOccurences() < numOfVarOccurences);
			    
			    TmpLiteral* tmp = anotherEl->next();
			    lits.remove(anotherEl);
			    nonselectedLits.push(anotherEl);
			    if (tmp)
			      {
				anotherEl = tmp->previous();
			      }
			    else
			      anotherEl = lits.last();
			  }
			else
			  anotherEl = anotherEl->previous();
		    };

		  }
		else
		  if (numOfVarOccurences < maxNegLitNumOfVarOccurences)
		    {
		      TmpLiteral* tmp = el->next();
		      ASSERT(tmp);
		      lits.remove(el);
		      nonselectedLits.push(el);
		      el = tmp;
		    }
		  else // numOfVarOccurence ==  maxNegLitNumOfVarOccurences
		    {
		      ulong litSize = el->size();
		      if (litSize > maxNegLitSize)
			{
			  maxNegLitSize = litSize;


			  TmpLiteral* anotherEl = lits.last();
			  while (anotherEl != el)
			    {
			      if (_currentObject->isNegativeForSelection(anotherEl->header()))
				{
				  ASSERT(anotherEl->numOfDifferentVariables() == maxNegLitNumOfDiffVars);
				  ASSERT(anotherEl->numberOfVariableOccurences() == maxNegLitNumOfVarOccurences);
				  
				  TmpLiteral* tmp = anotherEl->next();
				  lits.remove(anotherEl);
				  nonselectedLits.push(anotherEl);
				  if (tmp)
				    {
				      anotherEl = tmp->previous();
				    }
				  else
				    anotherEl = lits.last();
				}
			      else
				anotherEl = anotherEl->previous();
			    };

			}            
		      else
			if (litSize < maxNegLitSize)
			  {
			    TmpLiteral* tmp = el->next();
			    ASSERT(tmp);
			    lits.remove(el);
			    nonselectedLits.push(el);
			    el = tmp;
			  };
		    };
	      };
	}
      else // !_currentObject->isNegativeForSelection(el->header())
	{
	  ulong numOfDiffVars = el->numOfDifferentVariables();
	  if (numOfDiffVars > maxPosLitNumOfDiffVars)
	    {
	      maxPosLitNumOfDiffVars = numOfDiffVars;
	      maxPosLitNumOfVarOccurences = el->numberOfVariableOccurences();
	      maxPosLitSize = el->size();

	      TmpLiteral* anotherEl = lits.last();
	      while (anotherEl != el)
		{
		  if (!_currentObject->isNegativeForSelection(anotherEl->header()))
		    {		      
		      ASSERT(anotherEl->numOfDifferentVariables() < numOfDiffVars);
		      TmpLiteral* tmp = anotherEl->next();
		      lits.remove(anotherEl);
		      nonselectedLits.push(anotherEl);
		      if (tmp)
			{
			  anotherEl = tmp->previous();
			}
		      else
			anotherEl = lits.last();
		    }
		  else
		    anotherEl = anotherEl->previous();
		};
	    }
	  else
	    if (numOfDiffVars < maxPosLitNumOfDiffVars)
	      {
		TmpLiteral* tmp = el->next();
		ASSERT(tmp);
		lits.remove(el);
		nonselectedLits.push(el);
		el = tmp;
	      }
	    else // numOfDiffVars == maxPosLitNumOfDiffVars 
	      {
		TermWeightType numOfVarOccurences = el->numberOfVariableOccurences();
		if (numOfVarOccurences > maxPosLitNumOfVarOccurences)
		  {
		    maxPosLitSize = el->size();
		    maxPosLitNumOfVarOccurences = numOfVarOccurences; 

		    TmpLiteral* anotherEl = lits.last();
		    while (anotherEl != el)
		      {
			if (!_currentObject->isNegativeForSelection(anotherEl->header()))
			  {
			    ASSERT(anotherEl->numOfDifferentVariables() == maxPosLitNumOfDiffVars);
			    ASSERT(anotherEl->numberOfVariableOccurences() < numOfVarOccurences);
			    
			    TmpLiteral* tmp = anotherEl->next();
			    lits.remove(anotherEl);
			    nonselectedLits.push(anotherEl);
			    if (tmp)
			      {
				anotherEl = tmp->previous();
			      }
			    else
			      anotherEl = lits.last();
			  }
			else
			  anotherEl = anotherEl->previous();
		      };
		  }
		else
		  if (numOfVarOccurences < maxPosLitNumOfVarOccurences)
		    {
		      TmpLiteral* tmp = el->next();
		      ASSERT(tmp);
		      lits.remove(el);
		      nonselectedLits.push(el);
		      el = tmp;
		    }
		  else // numOfVarOccurence == maxPosLitNumOfVarOccurences 
		    {
		      ulong litSize = el->size();
		      if (litSize > maxPosLitSize)
			{
			  maxPosLitSize = litSize;
			  TmpLiteral* anotherEl = lits.last();
			  while (anotherEl != el)
			    {
			      if (!_currentObject->isNegativeForSelection(anotherEl->header()))
				{				  
				  ASSERT(anotherEl->numOfDifferentVariables() == maxPosLitNumOfDiffVars);
				  ASSERT(anotherEl->numberOfVariableOccurences() == maxPosLitNumOfVarOccurences);
				  TmpLiteral* tmp = anotherEl->next();
				  lits.remove(anotherEl);
				  nonselectedLits.push(anotherEl);
				  if (tmp)
				    {
				      anotherEl = tmp->previous();
				    }
				  else
				    anotherEl = lits.last();
				}
			      else
				anotherEl = anotherEl->previous();
			    };
			}            
		      else
			if (litSize < maxPosLitSize)
			  {
			    TmpLiteral* tmp = el->next();
			    ASSERT(tmp);
			    lits.remove(el);
			    nonselectedLits.push(el);
			    el = tmp;
			  };
		    };
	      };
	};
    }; // for (..
  // all negative literals are maximally nonground and have the same size
  // all positive literals are maximally nonground and have the same size
  ASSERT(lits.numOfAllLiterals());
  if (lits.numOfAllLiterals() == 1) { selectedLits.push(lits.pop()); return; };
 


  // select 1 positive, if exists, and 1 negative, if exists, on "don't care" basis 
  bool positiveHasBeenSelected = false;
  bool negativeHasBeenSelected = false;
  while (lits.nonempty())
    {
      TmpLiteral* lit = lits.pop();
      if (_currentObject->isNegativeForSelection(lit->header()))
	{
	  if (negativeHasBeenSelected) { nonselectedLits.push(lit); }
	  else 
	    {
	      negativeHasBeenSelected = true;
	      selectedLits.push(lit);
	    };
	}
      else 
	{
	  if (positiveHasBeenSelected) { nonselectedLits.push(lit); }
	  else 
	    {
	      positiveHasBeenSelected = true;
	      selectedLits.push(lit);
	    };
	};
    };

 
}; // void LiteralSelection::function1006(TmpLitList& lits,TmpLitList& selectedLits,TmpLitList& nonselectedLits,bool& negSelectionUsed,const TmpLitList::Statistics& stat)


void 
LiteralSelection::function1007(TmpLitList& lits,TmpLitList& selectedLits,TmpLitList& nonselectedLits,bool& negSelectionUsed,const TmpLitList::Statistics& stat)
{ 
  CALL("function1007(TmpLitList& lits,TmpLitList& selectedLits,TmpLitList& nonselectedLits,bool& negSelectionUsed,const TmpLitList::Statistics& stat)");
  /*
    Bidirectional selection. 
    In a rule, i.e. a clause containing both 
    positive and negative literals we always select one positive and one negative
    literal. If we have several literals of the same polarity, we select 
    those with maximal size. In a fact, i.e. a clause with all literals 
    of the same polarity, we select one of the biggest literals.     
  */

  ASSERT(!lits.containsAnswerLiterals());
  ASSERT(lits.numOfAllLiterals() > 1);
  ASSERT(!stat.inherently_selected);
  ASSERT(!lits.containsSplittingBranches()); 


  ulong maxNegLitSize;
  ulong maxPosLitSize;
  if (_currentObject->isNegativeForSelection(lits.last()->header())) 
    {
      maxNegLitSize = lits.last()->size(); 
      maxPosLitSize = 0;
    }
  else
    {
      maxNegLitSize = 0;
      maxPosLitSize = lits.last()->size(); 
    }; 
 
  for (TmpLiteral* el = lits.last()->previous();
       el;
       el = el->previous())
    {
      ASSERT(el->next());
      if (_currentObject->isNegativeForSelection(el->header()))
	{
	  ulong litSize = el->size();
	  if (litSize > maxNegLitSize)
	    {
	      maxNegLitSize = litSize;

	      
	      TmpLiteral* anotherEl = lits.last();
	      while (anotherEl != el)
		{
		  if (_currentObject->isNegativeForSelection(anotherEl->header()))
		    {
		      ASSERT(anotherEl->size() < litSize);		      
		      TmpLiteral* tmp = anotherEl->next();
		      lits.remove(anotherEl);
		      nonselectedLits.push(anotherEl);
		      if (tmp)
			{
			  anotherEl = tmp->previous();
			}
		      else
			anotherEl = lits.last();
		    }
		  else
		    anotherEl = anotherEl->previous();
		};
	    }            
	  else
	    if (litSize < maxNegLitSize)
	      {
		TmpLiteral* tmp = el->next();
		ASSERT(tmp);
		lits.remove(el);
		nonselectedLits.push(el);
		el = tmp;
	      };
	}
      else // !_currentObject->isNegativeForSelection(el->header())
	{
	  ulong litSize = el->size();
	  if (litSize > maxPosLitSize)
	    {
	      maxPosLitSize = litSize;


	      TmpLiteral* anotherEl = lits.last();
	      while (anotherEl != el)
		{
		  if (!_currentObject->isNegativeForSelection(anotherEl->header()))
		    {
		      ASSERT(anotherEl->size() < litSize);
		      TmpLiteral* tmp = anotherEl->next();
		      lits.remove(anotherEl);
		      nonselectedLits.push(anotherEl);
		      if (tmp)
			{
			  anotherEl = tmp->previous();
			}
		      else
			anotherEl = lits.last();
		    }
		  else
		    anotherEl = anotherEl->previous();
		};
	    }            
	  else
	    if (litSize < maxPosLitSize)
	      {
		TmpLiteral* tmp = el->next();
		ASSERT(tmp);
		lits.remove(el);
		nonselectedLits.push(el);
		el = tmp;
	      };
	};
    }; // for (..
  // all negative literals have maximal size
  // all positive literals have maximal size
  ASSERT(lits.numOfAllLiterals());
  if (lits.numOfAllLiterals() == 1) { selectedLits.push(lits.pop()); return; };
 

  // select 1 positive, if exists, and 1 negative, if exists, on "don't care" basis 
  bool positiveHasBeenSelected = false;
  bool negativeHasBeenSelected = false;
  while (lits.nonempty())
    {
      TmpLiteral* lit = lits.pop();
      if (_currentObject->isNegativeForSelection(lit->header()))
	{
	  if (negativeHasBeenSelected) { nonselectedLits.push(lit); }
	  else 
	    {
	      negativeHasBeenSelected = true;
	      selectedLits.push(lit);
	    };
	}
      else 
	{
	  if (positiveHasBeenSelected) { nonselectedLits.push(lit); }
	  else 
	    {
	      positiveHasBeenSelected = true;
	      selectedLits.push(lit);
	    };
	};
    };

}; // void LiteralSelection::function1007(TmpLitList& lits,TmpLitList& selectedLits,TmpLitList& nonselectedLits,bool& negSelectionUsed,const TmpLitList::Statistics& stat)


void 
LiteralSelection::function1008(TmpLitList& lits,TmpLitList& selectedLits,TmpLitList& nonselectedLits,bool& negSelectionUsed,const TmpLitList::Statistics& stat)
{ 
  CALL("function1008(TmpLitList& lits,TmpLitList& selectedLits,TmpLitList& nonselectedLits,bool& negSelectionUsed,const TmpLitList::Statistics& stat)");
  /*
    Bidirectional selection. 
    In a rule, i.e. a clause containing both 
    positive and negative literals we always select one positive and one negative
    literal. If we have several literals of the same polarity, we select 
    those that are maximally ground. In a fact, i.e. a clause with all literals 
    of the same polarity, we select one maximally ground literal.     
  */


  ASSERT(!lits.containsAnswerLiterals());
  ASSERT(lits.numOfAllLiterals() > 1);
  ASSERT(!stat.inherently_selected);
  ASSERT(!lits.containsSplittingBranches()); 

  ulong minNegLitNumOfDiffVars;
  TermWeightType minNegLitNumOfVarOccurences;
  ulong maxNegLitSize;
  ulong minPosLitNumOfDiffVars;
  TermWeightType minPosLitNumOfVarOccurences;
  ulong maxPosLitSize;
  if (_currentObject->isNegativeForSelection(lits.last()->header())) 
    {
      minNegLitNumOfDiffVars = lits.last()->numOfDifferentVariables();
      minNegLitNumOfVarOccurences = lits.last()->numberOfVariableOccurences(); 
      maxNegLitSize = lits.last()->size(); 
      minPosLitNumOfDiffVars = VampireKernelConst::MaxNumOfVariables;
      minPosLitNumOfVarOccurences = VampireKernelConst::MaxTermSize;
      maxPosLitSize = 0;
    }
  else
    {
      minNegLitNumOfDiffVars = VampireKernelConst::MaxNumOfVariables;
      minNegLitNumOfVarOccurences = VampireKernelConst::MaxTermSize; 
      maxNegLitSize = 0;
      minPosLitNumOfDiffVars = lits.last()->numOfDifferentVariables(); 
      minPosLitNumOfVarOccurences = lits.last()->numberOfVariableOccurences(); 
      maxPosLitSize = lits.last()->size(); 
    }; 
 
  for (TmpLiteral* el = lits.last()->previous();
       el;
       el = el->previous())
    {
      ASSERT(el->next());
      if (_currentObject->isNegativeForSelection(el->header()))
	{
	  ulong numOfDiffVars = el->numOfDifferentVariables();
	  if (numOfDiffVars < minNegLitNumOfDiffVars)
	    {
	      minNegLitNumOfDiffVars = numOfDiffVars;
	      minNegLitNumOfVarOccurences = el->numberOfVariableOccurences();
	      maxNegLitSize = el->size();

	      TmpLiteral* anotherEl = lits.last();
	      while (anotherEl != el)
		{
		  if (_currentObject->isNegativeForSelection(anotherEl->header()))
		    {		      
		      ASSERT(anotherEl->numOfDifferentVariables() > numOfDiffVars);
		      TmpLiteral* tmp = anotherEl->next();
		      lits.remove(anotherEl);
		      nonselectedLits.push(anotherEl);
		      if (tmp)
			{
			  anotherEl = tmp->previous();
			}
		      else
			anotherEl = lits.last();
		    }
		  else
		    anotherEl = anotherEl->previous();
		};
	    }
	  else
	    if (numOfDiffVars > minNegLitNumOfDiffVars)
	      { 
		TmpLiteral* tmp = el->next();
		ASSERT(tmp);
		lits.remove(el);
		nonselectedLits.push(el);
		el = tmp;
	      }
	    else // numOfDiffVars == minNegLitNumOfDiffVars 
	      {
		TermWeightType numOfVarOccurences = el->numberOfVariableOccurences();
		if (numOfVarOccurences < minNegLitNumOfVarOccurences)
		  {
		    maxNegLitSize = el->size();
		    minNegLitNumOfVarOccurences = numOfVarOccurences; 


		    TmpLiteral* anotherEl = lits.last();
		    while (anotherEl != el)
		      {
			if (_currentObject->isNegativeForSelection(anotherEl->header()))
			  {
			    
			    ASSERT(anotherEl->numOfDifferentVariables() == minNegLitNumOfDiffVars);
			    ASSERT(anotherEl->numberOfVariableOccurences() > numOfVarOccurences);
			    TmpLiteral* tmp = anotherEl->next();
			    lits.remove(anotherEl);
			    nonselectedLits.push(anotherEl);
			    if (tmp)
			      {
				anotherEl = tmp->previous();
			      }
			    else
			      anotherEl = lits.last();
			  }
			else
			  anotherEl = anotherEl->previous();
		      };

		  }
		else
		  if (numOfVarOccurences > minNegLitNumOfVarOccurences)
		    {
		      TmpLiteral* tmp = el->next();
		      ASSERT(tmp);
		      lits.remove(el);
		      nonselectedLits.push(el);
		      el = tmp;
		    }
		  else // numOfVarOccurence ==  minNegLitNumOfVarOccurences
		    {
		      ulong litSize = el->size();
		      if (litSize > maxNegLitSize)
			{
			  maxNegLitSize = litSize;

			  TmpLiteral* anotherEl = lits.last();
			  while (anotherEl != el)
			    {
			      if (_currentObject->isNegativeForSelection(anotherEl->header()))
				{
				  ASSERT(anotherEl->numOfDifferentVariables() == minNegLitNumOfDiffVars);
				  ASSERT(anotherEl->numberOfVariableOccurences() == minNegLitNumOfVarOccurences);
				  
				  TmpLiteral* tmp = anotherEl->next();
				  lits.remove(anotherEl);
				  nonselectedLits.push(anotherEl);
				  if (tmp)
				    {
				      anotherEl = tmp->previous();
				    }
				  else
				    anotherEl = lits.last();
				}
			      else
				anotherEl = anotherEl->previous();
			    };			  
			}            
		      else
			if (litSize < maxNegLitSize)
			  {
			    TmpLiteral* tmp = el->next();
			    ASSERT(tmp);
			    lits.remove(el);
			    nonselectedLits.push(el);
			    el = tmp;
			  };
		    };
	      };
	}
      else // !_currentObject->isNegativeForSelection(el->header())
	{

	  COP("COP300");

	  ulong numOfDiffVars = el->numOfDifferentVariables();
	  if (numOfDiffVars < minPosLitNumOfDiffVars)
	    {
	      minPosLitNumOfDiffVars = numOfDiffVars;
	      minPosLitNumOfVarOccurences = el->numberOfVariableOccurences();
	      maxPosLitSize = el->size();

	      TmpLiteral* anotherEl = lits.last();
	      while (anotherEl != el)
		{
		  if (!_currentObject->isNegativeForSelection(anotherEl->header()))
		    {
		      ASSERT(anotherEl->numOfDifferentVariables() > numOfDiffVars);		      
		      TmpLiteral* tmp = anotherEl->next();
		      lits.remove(anotherEl);
		      nonselectedLits.push(anotherEl);
		      if (tmp)
			{
			  anotherEl = tmp->previous();
			}
		      else
			anotherEl = lits.last();
		    }
		  else
		    anotherEl = anotherEl->previous();
		};
	    }
	  else
	    if (numOfDiffVars > minPosLitNumOfDiffVars)
	      {
		TmpLiteral* tmp = el->next();
		ASSERT(tmp);
		lits.remove(el);
		nonselectedLits.push(el);
		el = tmp;
	      }
	    else // numOfDiffVars == minPosLitNumOfDiffVars 
	      {
		TermWeightType numOfVarOccurences = el->numberOfVariableOccurences();
		if (numOfVarOccurences < minPosLitNumOfVarOccurences)
		  {
		    minPosLitNumOfVarOccurences = numOfVarOccurences; 
		    maxPosLitSize = el->size();

		    
		    TmpLiteral* anotherEl = lits.last();
		    while (anotherEl != el)
		      {
			if (!_currentObject->isNegativeForSelection(anotherEl->header()))
			  {
			    
			    ASSERT(anotherEl->numOfDifferentVariables() == minPosLitNumOfDiffVars);
			    ASSERT(anotherEl->numberOfVariableOccurences() > numOfVarOccurences);
			    TmpLiteral* tmp = anotherEl->next();
			    lits.remove(anotherEl);
			    nonselectedLits.push(anotherEl);
			    if (tmp)
			      {
				anotherEl = tmp->previous();
			      }
			    else
			      anotherEl = lits.last();
			  }
			else
			  anotherEl = anotherEl->previous();
		      };

		  }
		else
		  if (numOfVarOccurences > minPosLitNumOfVarOccurences)
		    {
		      TmpLiteral* tmp = el->next();
		      ASSERT(tmp);
		      lits.remove(el);
		      nonselectedLits.push(el);
		      el = tmp;
		    }
		  else // numOfVarOccurence == minPosLitNumOfVarOccurences 
		    {
		      ulong litSize = el->size();
		      if (litSize > maxPosLitSize)
			{
			  maxPosLitSize = litSize;


			  TmpLiteral* anotherEl = lits.last();
			  while (anotherEl != el)
			    {
			      if (!_currentObject->isNegativeForSelection(anotherEl->header()))
				{
				  ASSERT(anotherEl->numOfDifferentVariables() == minPosLitNumOfDiffVars);
				  ASSERT(anotherEl->numberOfVariableOccurences() == minPosLitNumOfVarOccurences);
				  TmpLiteral* tmp = anotherEl->next();
				  lits.remove(anotherEl);
				  nonselectedLits.push(anotherEl);
				  if (tmp)
				    {
				      anotherEl = tmp->previous();
				    }
				  else
				    anotherEl = lits.last();
				}
			      else
				anotherEl = anotherEl->previous();
			    };

			}            
		      else
			if (litSize < maxPosLitSize)
			  {
			    TmpLiteral* tmp = el->next();
			    ASSERT(tmp);
			    lits.remove(el);
			    nonselectedLits.push(el);
			    el = tmp;
			  };
		    };
	      };
	};
    }; // for (..
  // all negative literals are maximally ground and have the same size
  // all positive literals are maximally ground and have the same size
  ASSERT(lits.numOfAllLiterals());
  if (lits.numOfAllLiterals() == 1) { selectedLits.push(lits.pop()); return; };
 

  COP("COP500");


  // select 1 positive, if exists, and 1 negative, if exists, on "don't care" basis 
  bool positiveHasBeenSelected = false;
  bool negativeHasBeenSelected = false;
  while (lits.nonempty())
    {
      TmpLiteral* lit = lits.pop();
      if (_currentObject->isNegativeForSelection(lit->header()))
	{
	  if (negativeHasBeenSelected) { nonselectedLits.push(lit); }
	  else 
	    {
	      negativeHasBeenSelected = true;
	      selectedLits.push(lit);
	    };
	}
      else 
	{
	  if (positiveHasBeenSelected) { nonselectedLits.push(lit); }
	  else 
	    {
	      positiveHasBeenSelected = true;
	      selectedLits.push(lit);
	    };
	};
    };

}; // void LiteralSelection::function1008(TmpLitList& lits,TmpLitList& selectedLits,TmpLitList& nonselectedLits,bool& negSelectionUsed,const TmpLitList::Statistics& stat)


void 
LiteralSelection::function1009(TmpLitList& lits,TmpLitList& selectedLits,TmpLitList& nonselectedLits,bool& negSelectionUsed,const TmpLitList::Statistics& stat)
{ 
  CALL("function1009(TmpLitList& lits,TmpLitList& selectedLits,TmpLitList& nonselectedLits,bool& negSelectionUsed,const TmpLitList::Statistics& stat)");
  /*
    Bidirectional selection. 
    In a rule, i.e. a clause containing both 
    positive and negative literals we always select one positive and one negative
    literal. If we have several positive literals, we select 
    those that are maximally nonground. If we have several negative literals, 
    we select those that are maximally ground. 
    In a positive fact, i.e. a clause with only positive literals, 
    we select one maximally nonground literal. 
    In a negative fact, i.e. a clause with only negative literals, 
    we select one maximally ground literal.           
  */

  ASSERT(!lits.containsAnswerLiterals());
  ASSERT(lits.numOfAllLiterals() > 1);
  ASSERT(!stat.inherently_selected);
  ASSERT(!lits.containsSplittingBranches()); 


  ulong minNegLitNumOfDiffVars;
  TermWeightType minNegLitNumOfVarOccurences;
  ulong maxNegLitSize;
  ulong maxPosLitNumOfDiffVars;
  TermWeightType maxPosLitNumOfVarOccurences;
  ulong maxPosLitSize;
  if (_currentObject->isNegativeForSelection(lits.last()->header())) 
    {
      minNegLitNumOfDiffVars = lits.last()->numOfDifferentVariables();
      minNegLitNumOfVarOccurences = lits.last()->numberOfVariableOccurences(); 
      maxNegLitSize = lits.last()->size(); 
      maxPosLitNumOfDiffVars = 0;
      maxPosLitNumOfVarOccurences = 0;
      maxPosLitSize = 0;
    }
  else
    {
      minNegLitNumOfDiffVars = VampireKernelConst::MaxNumOfVariables;
      minNegLitNumOfVarOccurences = VampireKernelConst::MaxTermSize; 
      maxNegLitSize = 0;
      maxPosLitNumOfDiffVars = lits.last()->numOfDifferentVariables(); 
      maxPosLitNumOfVarOccurences = lits.last()->numberOfVariableOccurences(); 
      maxPosLitSize = lits.last()->size(); 
    }; 
 
  for (TmpLiteral* el = lits.last()->previous();
       el;
       el = el->previous())
    {
      ASSERT(el->next());
      if (_currentObject->isNegativeForSelection(el->header()))
	{
	  ulong numOfDiffVars = el->numOfDifferentVariables();
	  if (numOfDiffVars < minNegLitNumOfDiffVars)
	    {
	      minNegLitNumOfDiffVars = numOfDiffVars;
	      minNegLitNumOfVarOccurences = el->numberOfVariableOccurences();
	      maxNegLitSize = el->size();

	      TmpLiteral* anotherEl = lits.last();
	      while (anotherEl != el)
		{
		  if (_currentObject->isNegativeForSelection(anotherEl->header()))
		    {
		      ASSERT(anotherEl->numOfDifferentVariables() > numOfDiffVars);
		      TmpLiteral* tmp = anotherEl->next();
		      lits.remove(anotherEl);
		      nonselectedLits.push(anotherEl);
		      if (tmp)
			{
			  anotherEl = tmp->previous();
			}
		      else
			anotherEl = lits.last();
		    }
		  else
		    anotherEl = anotherEl->previous();
		};
	    }
	  else
	    if (numOfDiffVars > minNegLitNumOfDiffVars)
	      { 
		TmpLiteral* tmp = el->next();
		ASSERT(tmp);
		lits.remove(el);
		nonselectedLits.push(el);
		el = tmp;
	      }
	    else // numOfDiffVars == minNegLitNumOfDiffVars 
	      {
		TermWeightType numOfVarOccurences = el->numberOfVariableOccurences();
		if (numOfVarOccurences < minNegLitNumOfVarOccurences)
		  {
		    minNegLitNumOfVarOccurences = numOfVarOccurences; 
		    maxNegLitSize = el->size();

		    TmpLiteral* anotherEl = lits.last();
		    while (anotherEl != el)
		      {
			if (_currentObject->isNegativeForSelection(anotherEl->header()))
			  {
			    
			    ASSERT(anotherEl->numOfDifferentVariables() == minNegLitNumOfDiffVars);
			    ASSERT(anotherEl->numberOfVariableOccurences() > numOfVarOccurences);
			    TmpLiteral* tmp = anotherEl->next();
			    lits.remove(anotherEl);
			    nonselectedLits.push(anotherEl);
			    if (tmp)
			      {
				anotherEl = tmp->previous();
			      }
			    else
			      anotherEl = lits.last();
			  }
			else
			  anotherEl = anotherEl->previous();
		      };
		  }
		else
		  if (numOfVarOccurences > minNegLitNumOfVarOccurences)
		    {
		      TmpLiteral* tmp = el->next();
		      ASSERT(tmp);
		      lits.remove(el);
		      nonselectedLits.push(el);
		      el = tmp;
		    }
		  else // numOfVarOccurence ==  minNegLitNumOfVarOccurences
		    {
		      ulong litSize = el->size();
		      if (litSize > maxNegLitSize)
			{
			  maxNegLitSize = litSize;

			  TmpLiteral* anotherEl = lits.last();
			  while (anotherEl != el)
			    {
			      if (_currentObject->isNegativeForSelection(anotherEl->header()))
				{
				  ASSERT(anotherEl->numOfDifferentVariables() == minNegLitNumOfDiffVars);
				  ASSERT(anotherEl->numberOfVariableOccurences() == minNegLitNumOfVarOccurences);
				  
				  TmpLiteral* tmp = anotherEl->next();
				  lits.remove(anotherEl);
				  nonselectedLits.push(anotherEl);
				  if (tmp)
				    {
				      anotherEl = tmp->previous();
				    }
				  else
				    anotherEl = lits.last();
				}
			      else
				anotherEl = anotherEl->previous();
			    };
			}            
		      else
			if (litSize < maxNegLitSize)
			  {
			    TmpLiteral* tmp = el->next();
			    ASSERT(tmp);
			    lits.remove(el);
			    nonselectedLits.push(el);
			    el = tmp;
			  };
		    };
	      };
	}
      else // !_currentObject->isNegativeForSelection(el->header())
	{
	  ulong numOfDiffVars = el->numOfDifferentVariables();
	  if (numOfDiffVars > maxPosLitNumOfDiffVars)
	    {
	      maxPosLitNumOfDiffVars = numOfDiffVars;
	      maxPosLitNumOfVarOccurences = el->numberOfVariableOccurences();
	      maxPosLitSize = el->size();

	      TmpLiteral* anotherEl = lits.last();
	      while (anotherEl != el)
		{
		  if (!_currentObject->isNegativeForSelection(anotherEl->header()))
		    {		      
		      ASSERT(anotherEl->numOfDifferentVariables() < numOfDiffVars);
		      TmpLiteral* tmp = anotherEl->next();
		      lits.remove(anotherEl);
		      nonselectedLits.push(anotherEl);
		      if (tmp)
			{
			  anotherEl = tmp->previous();
			}
		      else
			anotherEl = lits.last();
		    }
		  else
		    anotherEl = anotherEl->previous();
		};
	    }
	  else
	    if (numOfDiffVars < maxPosLitNumOfDiffVars)
	      {
		TmpLiteral* tmp = el->next();
		ASSERT(tmp);
		lits.remove(el);
		nonselectedLits.push(el);
		el = tmp;
	      }
	    else // numOfDiffVars == maxPosLitNumOfDiffVars 
	      {
		TermWeightType numOfVarOccurences = el->numberOfVariableOccurences();
		if (numOfVarOccurences > maxPosLitNumOfVarOccurences)
		  {
		    maxPosLitNumOfVarOccurences = numOfVarOccurences; 
		    maxPosLitSize = el->size();

		    TmpLiteral* anotherEl = lits.last();
		    while (anotherEl != el)
		      {
			if (!_currentObject->isNegativeForSelection(anotherEl->header()))
			  {
			    ASSERT(anotherEl->numOfDifferentVariables() == maxPosLitNumOfDiffVars);
			    ASSERT(anotherEl->numberOfVariableOccurences() < numOfVarOccurences);
			    
			    TmpLiteral* tmp = anotherEl->next();
			    lits.remove(anotherEl);
			    nonselectedLits.push(anotherEl);
			    if (tmp)
			      {
				anotherEl = tmp->previous();
			      }
			    else
			      anotherEl = lits.last();
			  }
			else
			  anotherEl = anotherEl->previous();
		      };
		  }
		else
		  if (numOfVarOccurences < maxPosLitNumOfVarOccurences)
		    {
		      TmpLiteral* tmp = el->next();
		      ASSERT(tmp);
		      lits.remove(el);
		      nonselectedLits.push(el);
		      el = tmp;
		    }
		  else // numOfVarOccurence == maxPosLitNumOfVarOccurences 
		    {
		      ulong litSize = el->size();
		      if (litSize > maxPosLitSize)
			{
			  maxPosLitSize = litSize;

			  TmpLiteral* anotherEl = lits.last();
			  while (anotherEl != el)
			    {
			      if (!_currentObject->isNegativeForSelection(anotherEl->header()))
				{
				  
				  ASSERT(anotherEl->numOfDifferentVariables() == maxPosLitNumOfDiffVars);
				  ASSERT(anotherEl->numberOfVariableOccurences() == maxPosLitNumOfVarOccurences);
				  TmpLiteral* tmp = anotherEl->next();
				  lits.remove(anotherEl);
				  nonselectedLits.push(anotherEl);
				  if (tmp)
				    {
				      anotherEl = tmp->previous();
				    }
				  else
				    anotherEl = lits.last();
				}
			      else
				anotherEl = anotherEl->previous();
			    };
			}            
		      else
			if (litSize < maxPosLitSize)
			  {
			    TmpLiteral* tmp = el->next();
			    ASSERT(tmp);
			    lits.remove(el);
			    nonselectedLits.push(el);
			    el = tmp;
			  };
		    };
	      };
	};
    }; // for (..
  // all negative literals are maximally nonground and have the same size
  // all positive literals are maximally nonground and have the same size
  ASSERT(lits.numOfAllLiterals());
  if (lits.numOfAllLiterals() == 1) { selectedLits.push(lits.pop()); return; };
 

  // select 1 positive, if exists, and 1 negative, if exists, on "don't care" basis 
  bool positiveHasBeenSelected = false;
  bool negativeHasBeenSelected = false;
  while (lits.nonempty())
    {
      TmpLiteral* lit = lits.pop();
      if (_currentObject->isNegativeForSelection(lit->header()))
	{
	  if (negativeHasBeenSelected) { nonselectedLits.push(lit); }
	  else 
	    {
	      negativeHasBeenSelected = true;
	      selectedLits.push(lit);
	    };
	}
      else 
	{
	  if (positiveHasBeenSelected) { nonselectedLits.push(lit); }
	  else 
	    {
	      positiveHasBeenSelected = true;
	      selectedLits.push(lit);
	    };
	};
    };

}; // void LiteralSelection::function1009(TmpLitList& lits,TmpLitList& selectedLits,TmpLitList& nonselectedLits,bool& negSelectionUsed,const TmpLitList::Statistics& stat)

inline
void 
LiteralSelection::unselectLiteralsFromNonmaximalLayers(TmpLitList& lits,
						       TmpLitList& nonselectedLits) const
{
  CALL("unselectLiteralsFromNonmaximalLayers(..)");
  ASSERT(_literalOrdering->isStratified()); // waste of time otherwise
  ASSERT(lits.numOfAllLiterals() > 1);
  TmpLiteral* litFromMaxLayer = lits.last();
  for (TmpLiteral* el = litFromMaxLayer->previous();
       el;
       el = el->previous())
    {
      switch (_literalOrdering->compareLayers(el,litFromMaxLayer))
	{
	case Greater:
	  litFromMaxLayer = el;	  
	  do 
	    { 
	      nonselectedLits.push(lits.pop()); 
	    } 
	  while (lits.last() != el);
	  break;

	case Less:
	  {
	    TmpLiteral* tmp = el->next();
	    ASSERT(tmp);
	    lits.remove(el);
	    nonselectedLits.push(el);
	    el = tmp;
	  };
	  break;

	case Equal:
	  break;
	  
	case Incomparable:
	  break;
	}; // switch 
    };
  
}; // void LiteralSelection::unselectLiteralsFromNonmaximalLayers(..)




//=================================================



