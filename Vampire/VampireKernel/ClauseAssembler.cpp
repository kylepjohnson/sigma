 
//
// File:         ClauseAssembler.cpp
// Description:  Implements clause sharing.
// Created:      Dec 13, 2001, Alexandre Riazanov, riazanov@cs.man.ac.uk
// Note:         Was CLAUSE_INDEX in clause_index.h.
// Revised:      Feb 17, 2002, Alexandre Riazanov, riazanov@cs.man.ac.uk
//               Bug fix in assembleClause(..): wrong use of variable weighting.
//============================================================================
#include "ClauseAssembler.hpp"
#include "LiteralList.hpp"
#include "BitWord.hpp"
#include "sharing.hpp"
#include "dtree.hpp"
#include "LiteralSelection.hpp"
#include "tmp_literal.hpp"
#include "TmpLitList.hpp"
#include "Clause.hpp"
#include "PagedPassiveClauseStorage.hpp"
//============================================================================
#undef DEBUG_NAMESPACE
#ifdef DEBUG_CLAUSE_ASSEMBLER
#define DEBUG_NAMESPACE "ClauseAssembler"
#endif
#include "debugMacros.hpp"
//============================================================================

  using namespace BK;
  using namespace Gem;
  using namespace VK;


   ClauseAssembler::ClauseAssembler(SHARING_INDEX* si,const LiteralSelection* litSelection) 
    : _sharingIndex(si), 
    _litSharing(si), 
    _litSelection(litSelection),
    _inheritNegativeSelection(false),
    _equalityIsPossible(true),
    _eliminatedPredicateTable(0),
    _nongoalPenaltyCoefficient(1.0),
    _useNongoalPenaltyCoefficient(false)
    {
    };

void ClauseAssembler::init(SHARING_INDEX* si,const LiteralSelection* litSelection)
{
  CALL("init(SHARING_INDEX* si,const LiteralSelection* litSelection)");
  _sharingIndex = si; 
  _litSharing.init(si); 
  _intVarRenaming.init();
  _varWeighting.init();
  _openUnsharedTerm.init();
  _litSelection = litSelection;
  _inheritNegativeSelection = false;
  _equalityIsPossible = true;
  _eliminatedPredicateTable = 0;
  _nongoalPenaltyCoefficient = 1.0;
  _useNongoalPenaltyCoefficient = false;

}; // void ClauseAssembler::init(SHARING_INDEX* si,const LiteralSelection* litSelection)


void ClauseAssembler::destroy()
{
  CALL("destroy()");
  _openUnsharedTerm.destroy();
  _varWeighting.destroy();
  _intVarRenaming.destroy();
  _litSharing.destroy();
  BK_CORRUPT(*this);
}; // void ClauseAssembler::destroy()


void ClauseAssembler::reset()
{
  CALL("reset()");
  SHARING_INDEX* si = _sharingIndex;
  const LiteralSelection* litSelection = _litSelection;
  destroy();
  init(si,litSelection);
}; 


inline TERM* ClauseAssembler::shareTmpLiteral(const TmpLiteral* lit,Clause* cl) 
{
  CALL("shareTmpLiteral(const TmpLiteral* lit,const Clause* cl)");
  if (lit->isEquality())
    {
      return IntegrateEq(lit->header(),lit->arg1(),lit->arg2(),cl);
    }
  else 
    {
      return Integrate(lit->flatterm(),cl); 
    };
}; // TERM* ClauseAssembler::shareTmpLiteral(const TmpLiteral* lit,Clause* cl)

inline TERM* ClauseAssembler::Integrate(const Flatterm* lit,Clause* cl)
{
  CALL("Integrate(const Flatterm* lit,Clause* cl)");
#ifdef _SUPPRESS_WARNINGS_
  cl = cl;
#endif
    
  DTREE_NODE* leaf = _litSharing.Integrate(lit);
  leaf->NumOfClauses()++;
  return leaf->Term();
}; // TERM* ClauseAssembler::Integrate(const Flatterm* lit,Clause* cl)

inline TERM* ClauseAssembler::IntegrateEq(const TERM& header,const Flatterm* arg1,const Flatterm* arg2,Clause* cl)
{
#ifdef _SUPPRESS_WARNINGS_
  cl = cl;
#endif
    
  DTREE_NODE* leaf = _litSharing.IntegrateEq(header,arg1,arg2);
  leaf->NumOfClauses()++;
  return leaf->Term();
}; // TERM* ClauseAssembler::IntegrateEq(const TERM& header,const Flatterm* arg1,const Flatterm* arg2,Clause* cl) 


void ClauseAssembler::removeFromSharing(Clause* cl)
{  
  for (LiteralList::Iterator iter(cl->LitList());iter.notEnd();iter.next())
    removeFromSharing(iter.currentLiteral()); 
}; // void ClauseAssembler::removeFromSharing(Clause* cl)

inline void ClauseAssembler::removeFromSharing(TERM* lit)
{
  CALL("removeFromSharing(TERM* lit,Clause* cl)");
  if (lit->arity())
    {
      DTREE_NODE** branch_to_destroy;
      DTREE_NODE* leaf;
      DTREE_NODE* last_fork;
      _litSharing.InfoForDeletion(lit,branch_to_destroy,leaf,last_fork);

      ASSERT(leaf->IsLeaf());
    
      leaf->NumOfClauses()--;
    
      if (!(leaf->NumOfClauses())) // no more clauses with this literal
	{
	  TERM* lit_copy = leaf->Term();

	  DTREE::DestroyBranchWithoutForks(*branch_to_destroy);
	  if (last_fork)
	    {
	      DTREE::ReduceFork(last_fork);
	    }
	  else
	    {
	      *branch_to_destroy = 0;
	    }; 

     
	  _sharingIndex->DestroyLiteralCopy(lit_copy);
        
	}; 
     
    }
  else // propositional
    {
      ((*(_litSharing.Tree(*lit)))->NumOfClauses())--;
      if (!((*(_litSharing.Tree(*lit)))->NumOfClauses())) // no more clauses with this literal
	{
	  delete *(_litSharing.Tree(*lit));
      
	  *(_litSharing.Tree(*lit)) = 0;
	};
    };
}; // void ClauseAssembler::removeFromSharing(TERM* lit,Clause* cl,bool remove_from_sharing)




Clause* ClauseAssembler::assembleEmptyClause(OpenClauseBackground& ancestors) const
{
  CALL("assembleEmptyClause(OpenClauseBackground& ancestors) const");
  Clause* res = Clause::MakeEmptyClause();
  res->setSize(0);
  res->SetNumOfPosEq(0);
  res->setMinChildWeight(0);
  ancestors.writeNewBackgroundTo(res->background());
  return res;
}; // Clause* ClauseAssembler::assembleEmptyClause(OpenClauseBackground& ancestors) const


Clause* ClauseAssembler::assembleUnitClause(TmpLiteral* lit,OpenClauseBackground& ancestors) 
{
  CALL("assembleUnitClause(TmpLiteral* lit,OpenClauseBackground& ancestors)");
  _intVarRenaming.Reset();
  lit->renameVariables(_intVarRenaming);
  LiteralList* lit_list;
  long weightBase;
  bool isAnswer = lit->isAnswerLiteral();
  if (isAnswer)
    {
      lit_list = new (1) LiteralList(1,0);
      weightBase = 0L;
    }
  else
    {
      lit_list = new (1) LiteralList(1,1);
      weightBase = lit->size();

    };
  Clause* result = new Clause(lit_list);
  lit_list->firstElem()->setLiteral(shareTmpLiteral(lit,result));
  lit->setInherentlySelected(_inheritNegativeSelection 
                             && _litSelection->isNegativeForSelection(lit->header())
                             && !lit->isAnswerLiteral());
   
  if (lit->isInherentlySelected())
    {
      lit_list->firstElem()->setMark(LiteralList::InherentlySelected);
    };
  ancestors.writeNewBackgroundTo(result->background()); 
  result->setSize(lit->size());
  result->setWeightBase(weightBase);
  if (lit->isEquality() && lit->isPositive()) 
    { 
      result->SetNumOfPosEq(1); 
    }
  else 
    result->SetNumOfPosEq(0); 

  if (lit->isEquality() && lit->isPositive())
    { 
      result->setMinChildWeight(lit->minPosEqLHSSize() + 1); 
    }
  else 
    result->setMinChildWeight(0);

  result->setFlag(Clause::FlagInClauseIndex);
  result->setFlag(Clause::FlagLiteralsShared);
  if (isAnswer) result->setFlag(Clause::FlagContainsAnswerLiterals);
  return result;
}; // Clause* ClauseAssembler::assembleUnitClause(TmpLiteral* lit,OpenClauseBackground& ancestors)


Clause* ClauseAssembler::assembleClauseWithEvthngSlctd(TmpLitList& lits,OpenClauseBackground& ancestors)
{
  CALL("assembleClauseWithEvthngSlctd(TmpLitList& lits,OpenClauseBackground& ancestors)");
  if (lits.empty()) return assembleEmptyClause(ancestors);
  long size;
  long weightBase;
 
  lits.computeSizeAndWeightBase(size,weightBase);

  ulong num_of_max = lits.numOfAllLiterals(); 
  // ^ all literals are selected, even the answer literals. 
  _intVarRenaming.Reset();
  lits.renameVariables(_intVarRenaming);

  LiteralList* lit_list = new(num_of_max) LiteralList(num_of_max,num_of_max);
  Clause* result = new Clause(lit_list);

 
  LiteralList::Iterator iter(lit_list);
  TERM* lit;
  bool containsAnswerLiterals = false;
  do 
    {
      ASSERT(iter.notEnd());
      TmpLiteral* tmp_lit = lits.pop();
      lit = shareTmpLiteral(tmp_lit,result);
      iter.currentElement()->setLiteral(lit);
      containsAnswerLiterals = containsAnswerLiterals || lit->isAnswerLiteral();
      if (tmp_lit->isInherentlySelected()) iter.currentElement()->setMark(LiteralList::InherentlySelected); 
      iter.next();
      tmp_lit->recycle();
    }
  while (lits.nonempty()); 

  ASSERT(iter.end());
           
  ancestors.writeNewBackgroundTo(result->background()); 
  result->setSize(size);
  result->setWeightBase(weightBase);
  result->SetNumOfPosEq(0);
  result->setMinChildWeight(0);  
  result->setFlag(Clause::FlagInClauseIndex);
  result->setFlag(Clause::FlagLiteralsShared);
  if (containsAnswerLiterals) result->setFlag(Clause::FlagContainsAnswerLiterals);
  return result; 
}; // Clause* ClauseAssembler::assembleClauseWithEvthngSlctd(TmpLitList& lits,OpenClauseBackground& ancestors)


Clause* ClauseAssembler::assembleClause(TmpLitList& lits,OpenClauseBackground& ancestors)
{
  CALL("assembleClause(TmpLitList& lits,OpenClauseBackground& ancestors)");
  ASSERT(lits.checkIntegrity());
  
  if (lits.empty()) return assembleEmptyClause(ancestors);
  long size;
  long weightBase;
  long num_of_pos_eq;
  if (_equalityIsPossible)
    {
      lits.computeSizeAndWeightBaseAndNumOfPosEq(size,weightBase,num_of_pos_eq);
    }
  else
    { 
      lits.computeSizeAndWeightBase(size,weightBase);
      num_of_pos_eq = 0;
    }; 
   
  TmpLitList max_lits;
  TmpLitList nonmax_lits; 
  TmpLitList tmp_lits;
  TmpLitList* ordered_max_lits;
  TmpLitList* ordered_nonmax_lits; 

  DOP(ulong debugLitsLength = lits.numOfAllLiterals());
  bool negSelUsed = false;
  if (_eliminatedPredicateTable && 
      _litSelection->selectForElimination(lits,max_lits,nonmax_lits,_eliminatedPredicateTable))
    {
      // nothing to do here 
    }
  else
    if (ancestors.isSupport())
      {
	_litSelection->selectForSupport(lits,max_lits,nonmax_lits);
	negSelUsed = false;	      
      }
    else 
      {
	// order literals as usual
	_litSelection->select(lits,max_lits,nonmax_lits,negSelUsed);
      };
  // don't use lits anymore! it must be empty here!

  ASSERT(lits.empty());
  ASSERT((!max_lits.empty()) || nonmax_lits.containsOnlyAnswerLiterals());  
  
  ulong num_of_max = max_lits.numOfAllLiterals();
  ulong num_of_nonmax = nonmax_lits.numOfAllLiterals();

  ASSERT(debugLitsLength == num_of_max + num_of_nonmax);
  if (num_of_max > 1)
    {

      _varWeighting.Reset();
	  TmpLiteral* el;
      for (el = max_lits.first(); el; el = el->next())
	_varWeighting.AnotherTermVariables(el->variableOccurences());  
      for (el = nonmax_lits.first(); el; el = el->next())
	_varWeighting.AnotherTermVariables(el->variableOccurences());  

      max_lits.order(tmp_lits,_varWeighting);
      ordered_max_lits = &tmp_lits;
      if (num_of_nonmax > 1) 
	{
	  max_lits.reset(); 
	  nonmax_lits.order(max_lits,_varWeighting);
	  ordered_nonmax_lits = &max_lits; // this is not an error!
	}
      else ordered_nonmax_lits = &nonmax_lits;  
    }
  else
    {
      ordered_max_lits = &max_lits;
      if (num_of_nonmax > 1)
	{
	  _varWeighting.Reset();
	  TmpLiteral* el;
	  for (el = max_lits.first(); el; el = el->next())
	    _varWeighting.AnotherTermVariables(el->variableOccurences());  
	  for (el = nonmax_lits.first(); el; el = el->next())
	    _varWeighting.AnotherTermVariables(el->variableOccurences());  

	  nonmax_lits.order(tmp_lits,_varWeighting);
	  ordered_nonmax_lits = &tmp_lits;
	}
      else 
	{
	  ordered_nonmax_lits = &nonmax_lits;
	};
    };  
 
  _intVarRenaming.Reset();
  // order of renaming is significant!
  ordered_max_lits->renameVariables(_intVarRenaming);
  ordered_nonmax_lits->renameVariables(_intVarRenaming);
  
  long weight_base_of_nonmax = ordered_nonmax_lits->computeWeightBase();
  
  long min_size_of_max_pos_eq_lhs = 0;
  if (_equalityIsPossible)
    {
      min_size_of_max_pos_eq_lhs = ordered_max_lits->minlPosEqLHSSize();
      if (min_size_of_max_pos_eq_lhs) min_size_of_max_pos_eq_lhs++;
    };
  
  LiteralList* lit_list = new(num_of_max + num_of_nonmax) LiteralList(num_of_max + num_of_nonmax,num_of_max);
  Clause* result = new Clause(lit_list);

  LiteralList::Iterator iter(lit_list);
  TERM* lit;
  TmpLiteral* tmp_lit;
  bool containsAnswerLiterals = false;
  while (num_of_max)
    {
      tmp_lit = ordered_max_lits->pop();

      lit = shareTmpLiteral(tmp_lit,result);
      ASSERT(iter.notEnd());  
      iter.currentElement()->setLiteral(lit);
      containsAnswerLiterals = containsAnswerLiterals || lit->isAnswerLiteral();
      tmp_lit->setInherentlySelected(_inheritNegativeSelection
				     && _litSelection->isNegativeForSelection(tmp_lit->header()));
      if (tmp_lit->isInherentlySelected())  iter.currentElement()->setMark(LiteralList::InherentlySelected);  
      iter.next();
      num_of_max--;
      tmp_lit->recycle();
    };
           
  
  while (num_of_nonmax)
    {
      tmp_lit = ordered_nonmax_lits->pop();
      ASSERT(!tmp_lit->isInherentlySelected());
      lit = shareTmpLiteral(tmp_lit,result);
      ASSERT(iter.notEnd());  
      iter.currentElement()->setLiteral(lit);   
      containsAnswerLiterals = containsAnswerLiterals || lit->isAnswerLiteral();     
      iter.next();
      num_of_nonmax--;
      tmp_lit->recycle();
    };

  ASSERT(iter.end());  

  if (negSelUsed) 
    { 
      ancestors.UsedRule(ClauseBackground::NegSel);
      ancestors.writeNewBackgroundTo(result->background());
      ancestors.CancelRule(ClauseBackground::NegSel); 
    }
  else 
    ancestors.writeNewBackgroundTo(result->background());
  result->setSize(size);
  result->setWeightBase(weightBase);
  result->SetNumOfPosEq(num_of_pos_eq);
  result->setMinChildWeight(weight_base_of_nonmax + min_size_of_max_pos_eq_lhs);
  result->setFlag(Clause::FlagInClauseIndex);
  result->setFlag(Clause::FlagLiteralsShared);
  if (containsAnswerLiterals) result->setFlag(Clause::FlagContainsAnswerLiterals);
  return result; 
}; // Clause* ClauseAssembler::assembleClause(TmpLitList& lits,OpenClauseBackground& ancestors)



Clause* ClauseAssembler::assembleUnsharedClause(TmpLitList& lits,OpenClauseBackground& ancestors)
{
  CALL("assembleUnsharedClause(TmpLitList& lits,OpenClauseBackground& ancestors)");
  ulong num_of_lits = lits.numOfAllLiterals();
  long size;
  long weightBase;
  lits.computeSizeAndWeightBase(size,weightBase);
  LiteralList* lit_list = new (num_of_lits) LiteralList(num_of_lits,num_of_lits);
  Clause* result = new Clause(lit_list);
          
  bool containsAnswerLiterals = false;
  for(LiteralList::Iterator iter(lit_list);iter.notEnd();iter.next())
    { 
      TmpLiteral* tmp_lit = lits.pop(); 
      TERM* lit = assembleUnsharedLit(*tmp_lit);
      iter.currentElement()->setLiteral(lit);
      containsAnswerLiterals = containsAnswerLiterals || lit->isAnswerLiteral();  
      if (tmp_lit->isInherentlySelected()) 
	{ 
	  iter.currentElement()->setMark(LiteralList::InherentlySelected);
	};
      tmp_lit->recycle();
    };


  ancestors.writeNewBackgroundTo(result->background()); 
  result->setSize(size);
  result->setWeightBase(weightBase);
  result->SetNumOfPosEq(0);
  result->setMinChildWeight(0);  
  if (containsAnswerLiterals) result->setFlag(Clause::FlagContainsAnswerLiterals);
  return result;  
}; // Clause* ClauseAssembler::assembleUnsharedClause(TmpLitList& lits,OpenClauseBackground& ancestors) 




TERM* ClauseAssembler::assembleUnsharedLit(const TmpLiteral& lit) 
{
  CALL("assembleUnsharedLit(const TmpLiteral& lit)");

  _openUnsharedTerm.reset();
  TERM header(lit.header());
  _openUnsharedTerm.litHeader(header);
  if (header.IsEquality())
    {
      Flatterm::ConstIterator iter(lit.arg1());
      while (iter.NotEndOfTerm())
	{
	  _openUnsharedTerm.symbol(iter.CurrPos()->symbolRef()); 
	  iter.NextSym();
	};     
      iter.Reset(lit.arg2());
      while (iter.NotEndOfTerm())
	{
	  _openUnsharedTerm.symbol(iter.CurrPos()->symbolRef()); 
	  iter.NextSym();
	};  
    }
  else
    {
      Flatterm::ConstIterator iter(lit.flatterm());
      iter.NextSym();
      while (iter.NotEndOfTerm())
	{
	  _openUnsharedTerm.symbol(iter.CurrPos()->symbolRef()); 
	  iter.NextSym();
	};
    };
  return _openUnsharedTerm.result().First();
}; // TERM* ClauseAssembler::assembleUnsharedLit(const TmpLiteral& lit) 





bool ClauseAssembler::assembleClauseInPagedPassiveClauseStorage(PagedPassiveClauseStorage& storage,TmpLitList& lits,OpenClauseBackground& ancestors,long clauseNum,bool showNewClause,bool markAsName)
{
  CALL("assembleClauseInPagedPassiveClauseStorage(PagedPassiveClauseStorage& storage,TmpLitList& lits,OpenClauseBackground& ancestors,long clauseNum,bool showNewClause,bool markAsName");
  ASSERT(!lits.empty());

  //ulong size = lits.computeSize();
  long size;
  long weight; 
  lits.computeSizeAndWeightBase(size,weight);
  
  BitWord rules = ancestors.Rules();
  
  if ((_useNongoalPenaltyCoefficient) &&
      (!rules.bit(ClauseBackground::Subgoal)))
    {
      weight = (long)(weight*_nongoalPenaltyCoefficient);	  
    };
  
  if (weight > (long)VampireKernelConst::MaxClauseSize) 
    {
      weight = VampireKernelConst::MaxClauseSize;
    }
  else 
    if (weight < 1)
      {
	weight = 1;
      };
  
  if (markAsName) rules.set(ClauseBackground::Name);
  ulong numOfLits = lits.numOfAllLiterals();
  ulong numOfAncestors = ancestors.numberOfAncestors();
  PagedPassiveClauseStorage::StoredClauseHandler newClauseHandler;
 try_to_open:   
  storage.openClause(newClauseHandler,clauseNum,size,weight,rules,numOfLits,numOfAncestors);
  if (!newClauseHandler)
    {
      // try to cancel some bad clauses in storage
      const PagedPassiveClauseStorage::ClauseProfile* worstClauseProfile =
	storage.worstClauseProfile();
      if (worstClauseProfile)
	{
	  if (worstClauseProfile->clauseWeight() > weight)
	    {
	      ALWAYS(storage.cancelWorstClause());
	      goto try_to_open;
	    };
	};
      return false;
    };
 

  // now write the literals 
  writeIntoPagedPassiveClauseStorage(storage,lits);
 
  // now write the ancestors
  
  for (Clause* const * anc = ancestors.last(); anc >= ancestors.begin(); anc--)
    storage.writeAncestor(*anc); 

  storage.endOfClause();

  if (showNewClause)
    {
      PagedPassiveClauseStorage::StoredClause newClause;
      PagedPassiveClauseStorage::loadClause(newClauseHandler,newClause);
      newClause.output(cout << "% ") << "\n";
    };
  return true;
}; // bool ClauseAssembler::assembleClauseInPagedPassiveClauseStorage(PagedPassiveClauseStorage& storage,TmpLitList& lits,OpenClauseBackground& ancestors,long clauseNum,bool showNewClause,bool markAsName)



void ClauseAssembler::writeIntoPagedPassiveClauseStorage(PagedPassiveClauseStorage& storage,TmpLitList& lits)
{
  CALL("writeIntoPagedPassiveClauseStorage(PagedPassiveClauseStorage& storage,TmpLitList& lits)"); 

  for (TmpLiteral* el = lits.first(); el; el = el->next())
    writeIntoPagedPassiveClauseStorage(storage,el);   
 
}; // void ClauseAssembler::writeIntoPagedPassiveClauseStorage(PagedPassiveClauseStorage& storage,TmpLitList& lits)

void ClauseAssembler::writeIntoPagedPassiveClauseStorage(PagedPassiveClauseStorage& storage,TmpLiteral* lit)
{
  CALL("writeIntoPagedPassiveClauseStorage(PagedPassiveClauseStorage& storage,TmpLiteral* lit)");
  storage.openLiteral(lit->isInherentlySelected());
  TERM header = lit->header();
  Flatterm::Iterator iter;
  if (header.IsEquality()) 
    {
      storage.writeSymbol(header);
      iter.Reset(lit->arg1());
      do
	{
	  storage.writeSymbol(iter.CurrPos()->symbolRef());          
	  iter.NextSym(); 
	}
      while (iter.NotEndOfTerm());
      iter.Reset(lit->arg2());
      do
	{
	  storage.writeSymbol(iter.CurrPos()->symbolRef());          
	  iter.NextSym();
	}
      while (iter.NotEndOfTerm());
    }
  else 
    {
      iter.Reset(lit->flatterm());
      do
	{
	  storage.writeSymbol(iter.CurrPos()->symbolRef());          
	  iter.NextSym();
	}
      while (iter.NotEndOfTerm());
    };
  storage.closeLiteral(); 
};  // void ClauseAssembler::writeIntoPagedPassiveClauseStorage(PagedPassiveClauseStorage& storage,TmpLiteral* lit)



//============================================================================
