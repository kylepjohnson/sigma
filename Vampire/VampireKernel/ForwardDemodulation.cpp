//
// File:         ForwardDemodulation.cpp
// Description:  
// Created:      ???, Alexandre Riazanov, riazanov@cs.man.ac.uk
// Revised:      Apr 29, 2001, Alexandre Riazanov, riazanov@cs.man.ac.uk
//               KBConstraint and SharedKBConstraint are no longer used. 
//               Generic SimplificationOrdering is used instead. 
//====================================================
#include "ForwardDemodulation.hpp"
#include "Clause.hpp"
#include "SimplificationOrdering.hpp"
#include "DestructionMode.hpp"
//=================================================
#undef DEBUG_NAMESPACE
#ifdef DEBUG_FORWARD_DEMODULATION
#define DEBUG_NAMESPACE "ForwardDemodulation"
#endif 
#include "debugMacros.hpp"
//=================================================

using namespace VK;
using namespace BK;
using namespace Gem;

#ifdef DEBUG_ALLOC_OBJ_TYPE
#include "ClassDesc.hpp"
ClassDesc ForwardDemodulation::Index::StoredSubstitution::_classDesc("ForwardDemodulation::Index::StoredSubstitution",
								     ForwardDemodulation::Index::StoredSubstitution::minSize(),
								     ForwardDemodulation::Index::StoredSubstitution::maxSize());
ClassDesc ForwardDemodulation::Index::ConstrRuleList::_classDesc("ForwardDemodulation::Index::ConstrRuleList",
								 ForwardDemodulation::Index::ConstrRuleList::minSize(),
								 ForwardDemodulation::Index::ConstrRuleList::maxSize());

#endif


ForwardDemodulation::ForwardDemodulation() : 
  fDOnSplittingBranchesFlag(false),
  _constrCheck(0)
{
  CALL("constructor ForwardDemodulation()");
};

ForwardDemodulation::~ForwardDemodulation() 
{
  CALL("destructor ~ForwardDemodulation()");
  if (DestructionMode::isThorough()) 
    if (_constrCheck)
      {
	SimplificationOrdering::current()->releaseConstraintCheckOnForwardMatchingSubst(_constrCheck);
      };
};

void ForwardDemodulation::init()
{
  index.init();
  _ruleRHSInstance.init();
  fDOnSplittingBranchesFlag = false;
  _substCache.init();
  _constrCheck = 0;
};

void ForwardDemodulation::destroy() 
{
  CALL("destroy()");
  if (DestructionMode::isThorough()) 
    if (_constrCheck)
      {
	SimplificationOrdering::current()->releaseConstraintCheckOnForwardMatchingSubst(_constrCheck);
	_constrCheck = 0;
      };
  _substCache.destroy(); 
  index.destroy(); 
}; // void ForwardDemodulation::destroy() 

void ForwardDemodulation::reset()
{
  CALL("reset()");
  destroy();
  init();
}; // void ForwardDemodulation::reset()


void ForwardDemodulation::Integrate(Clause* cl) 
{ 
  if (CanBeRewritingRule(cl)) index.Integrate(cl); 
}; 

void ForwardDemodulation::Remove(Clause* cl) 
{ 
  if (CanBeRewritingRule(cl)) index.Remove(cl); 
};


bool ForwardDemodulation::RewriteByConstrained(const Flatterm* query,const SetOfSplittingBranches& splittingBranches)
{
  CALL("RewriteByConstrained(const Flatterm* query,const SetOfSplittingBranches& splittingBranches)");

  ASSERT(query->Symbol().isComplex());
  INTERCEPT_INDEXED_SEARCH_FOR_GENERALISATIONS_RETRIEVAL_QUERY(query);

  for (FMCodeTree<VampireKernelConst::MaxTermSize,VampireKernelConst::MaxNumOfVariables,GlobAlloc,TERM,Flatterm,Index::LeafInfo,Index>::Leaf* 
	 leaf = index.Match(query); 
       leaf; 
       leaf = index.MatchAgain())
    {
      ASSERT(leaf->isLeaf());  
      Index::ConstrRuleList* crl =  leaf->info().rules(); 
      ASSERT(crl);  
      do
	{
#ifdef DEBUG_ALLOC
	  ASSERT(crl->checkObj());
#endif
	  INTERCEPT_INDEXED_SEARCH_FOR_GENERALISATIONS_RETRIEVAL_RESULT(crl->hd().lhs);

	  rule = crl->hd().clause;

	  if (rule->Active())
	    {
	      // check the constraint
	      SimplificationOrdering::StoredConstraint* constr = crl->hd().constr;
	      collectSubstCache(crl->hd().subst);

	      if (!constr) // preordered 
		{
		  ASSERT(rule->PreorderedPositiveUnitEquality() || fDOnSplittingBranchesFlag);
		  ASSERT(crl->hd().lhs == rule->LitList()->firstLit()->Arg1()->First()); 

		  if (CheckSplittingBranches(rule,splittingBranches))
		    {
		      TERM rule_rhs_term = *(rule->LitList()->firstLit()->Arg2());
		      _ruleRHSInstance.reset(rule_rhs_term,&_substCache);
		      INTERCEPT_INDEXED_SEARCH_FOR_GENERALISATIONS_RETRIEVAL_ABORT;
		      return true;
		    };
		}
	      else
		{
		  // constrained rule, check the constraint and splitting branches
		  if ((checkConstraint(constr)) && 
		      (CheckSplittingBranches(rule,splittingBranches)))
		    {
		      const TERM* lhs = crl->hd().lhs;
		      ASSERT(lhs);
		      ASSERT(lhs->isComplex());
		      TERM rule_rhs_term;
		      if (rule->LitList()->firstLit()->Arg1()->First() == lhs)
			{
			  rule_rhs_term = *(rule->LitList()->firstLit()->Arg2());
			}          
		      else
			{
			  ASSERT(rule->LitList()->firstLit()->Arg2()->First() == lhs);
			  rule_rhs_term = *(rule->LitList()->firstLit()->Arg1());
			  ASSERT(rule_rhs_term.First()->isComplex());
			};

		      _ruleRHSInstance.reset(rule_rhs_term,&_substCache);  
		      
		      INTERCEPT_INDEXED_SEARCH_FOR_GENERALISATIONS_RETRIEVAL_ABORT;
		      return true;      
		    };              
		};
	    };
	  crl = crl->tl();
	}
      while (crl);
    }; // for ...

  INTERCEPT_INDEXED_SEARCH_FOR_GENERALISATIONS_RETRIEVAL_TOTAL;
  
  return false;      
}; // bool ForwardDemodulation::RewriteByConstrained(const Flatterm* query,const SetOfSplittingBranches& splittingBranches) 

ForwardDemodulation::Index::ConstrRuleList* 
ForwardDemodulation::removeConstrRule(Clause* cl,
				      TERM* t,
				      Index::ConstrRuleList* lst
#ifdef INTERCEPT_INDEXED_SEARCH_FOR_GENERALISATIONS
				      , bool& reallyRemoved
#endif
				      )
{
  CALL("removeConstrRule(Clause* cl,TERM* t,Index::ConstrRuleList* lst)");
#ifdef INTERCEPT_INDEXED_SEARCH_FOR_GENERALISATIONS
      reallyRemoved = false;
#endif
  if (!lst) return 0;
  if ((lst->hd().clause == cl) && (lst->hd().lhs == t))
    {
#ifdef INTERCEPT_INDEXED_SEARCH_FOR_GENERALISATIONS
      reallyRemoved = true;
#endif
      Index::ConstrRuleList* res = lst->tl();
   
      //Index::StoredSubstitution::destroyList(lst->hd().subst);

      delete lst;
      return res;
    };
#ifdef INTERCEPT_INDEXED_SEARCH_FOR_GENERALISATIONS
  lst->tl() = removeConstrRule(cl,t,lst->tl(),reallyRemoved);
#else
  lst->tl() = removeConstrRule(cl,t,lst->tl());
#endif
  return lst;
}; // ForwardDemodulation::Index::ConstrRuleList* ForwardDemodulation::removeConstrRule(Clause* cl,TERM* t,Index::ConstrRuleList* lst)

ForwardDemodulation::Index::ConstrRuleList* 
ForwardDemodulation::insertConstrRule(Clause* cl,
				      SimplificationOrdering::StoredConstraint* constr,
				      Index::StoredSubstitution* subst,
				      TERM* t,
				      Index::ConstrRuleList* lst)
{
  CALL("insertConstrRule(..)");
  //return new Index::ConstrRuleList(Index::ConstrRule(cl,constr,subst,t),lst); 
  Index::ConstrRuleList* res = new Index::ConstrRuleList(lst); 
  res->hd().init(cl,constr,subst,t); 
  return res;
}; // ForwardDemodulation::Index::ConstrRuleList* ForwardDemodulation::insertConstrRule(..)


inline bool ForwardDemodulation::CanBeRewritingRule(const Clause* cl)
{
  CALL("CanBeRewritingRule(const Clause* cl)");
  // cl is either unit positive equality or a clause with exactly one selected positive equality literal
  // and a number of literals that are splitting branches. 
  // At the momemnt no answer literals are allowed in rewriting rules.
  if (cl->flag(Clause::FlagContainsAnswerLiterals)) return false;
  TERM* lit; 
  if (cl->Unit())
    {
      lit = cl->LitList()->firstLit();
      return (lit->Positive() && lit->IsEquality());
    }
  else  
    {
      if (!fDOnSplittingBranchesFlag) return false;
      lit = cl->LitList()->firstLit();
      if (lit->Negative() || (!lit->IsEquality()) || (cl->LitList()->numOfSelectedLits() != 1)) return false;
      LiteralList::Iterator iter(cl->LitList());
      iter.next();
      ASSERT(iter.notEnd());
      do
	{
	  if (!iter.currentLiteral()->IsSplitting()) return false;
	  iter.next();
	}
      while (iter.notEnd());
      return true;
    };
}; // bool ForwardDemodulation::CanBeRewritingRule(const Clause* cl) 


inline bool ForwardDemodulation::CheckSplittingBranches(const Clause* cl,const SetOfSplittingBranches& splittingBranches)
{
  CALL("CheckSplittingBranches(const Clause* cl,const SetOfSplittingBranches& splittingBranches)");
  if ((!fDOnSplittingBranchesFlag) || cl->Unit())
    {
      ASSERT(cl->Unit());
      return true; 
    }; 
  LiteralList::Iterator iter(cl->LitList());
  iter.next(); 
  while (iter.notEnd()) 
    {
      if (!splittingBranches.safeScoreOf(iter.currentLiteral()->HeaderNum())) return false;
      iter.next();
    };
  branchesUsed = true;
  return true;
}; // bool ForwardDemodulation::CheckSplittingBranches(const Clause* cl,const SetOfSplittingBranches& splittingBranches)



inline
bool 
ForwardDemodulation::checkConstraint(const SimplificationOrdering::StoredConstraint* constr)
{
  CALL("checkConstraint(..)");
  ASSERT(constr);
  if (!_constrCheck)
    _constrCheck = SimplificationOrdering::current()->reserveConstraintCheckOnForwardMatchingSubst();
  ASSERT(_constrCheck);
  return _constrCheck->holds(constr,&_substCache);
}; // bool ForwardDemodulation::checkConstraint(const SimplificationOrdering::StoredConstraint* constr)


inline
void 
ForwardDemodulation::collectSubstCache(const ForwardDemodulation::Index::StoredSubstitution* storedSubst)
{
  CALL("collectSubstCache(const ForwardDemodulation::Index::StoredSubstitution* storedSubst)");
  _substCache.reset(); 
  while (storedSubst) 
    {
#ifdef DEBUG_ALLOC
  ASSERT(storedSubst->checkObj());
#endif
      _substCache.associate(storedSubst->hd().el1,*(storedSubst->hd().el2));
      storedSubst = storedSubst->tl();
    }; 
  _substCache.endOfCollection();
}; // void ForwardDemodulation::collectSubstCache(const ForwardDemodulation::Index::StoredSubstitution* storedSubst)


//================================================
#undef DEBUG_NAMESPACE
#ifdef DEBUG_FORWARD_DEMODULATION
#define DEBUG_NAMESPACE "ForwardDemodulation::Index"
#endif 
#include "debugMacros.hpp"
//=================================================


void ForwardDemodulation::Index::IntegrateConstrained(Clause* cl)
{
  CALL("IntegrateConstrained(Clause* cl)");

#ifdef DEBUG_NAMESPACE
  if (cl->numOfNonanswerLiterals() > 1)
    {
      LiteralList::Iterator iter(cl->LitList());
      iter.next(); 
      do
	{
	  if ((!iter.currentLiteral()->IsSplitting()) &&
	      (!iter.currentLiteral()->isAnswerLiteral()))
	    {
	      ICP("E1");
	    };
	  iter.next();
	}
      while (iter.notEnd());
    };  
#endif          
  
  ASSERT(cl->LitList()->numOfSelectedLits() == 1); 

  TERM* lit = cl->LitList()->firstLit();

  ASSERT(lit->Positive());
  ASSERT(lit->IsEquality());


  if (lit->IsOrderedEq())
    {
      ASSERT(lit->Args()->IsReference());
      TERM* lhs = lit->Args()->First();
      FMCodeTree<VampireKernelConst::MaxTermSize,VampireKernelConst::MaxNumOfVariables,GlobAlloc,TERM,Flatterm,Index::LeafInfo,Index>::Leaf* leaf = Integrate(lhs); 
      INTERCEPT_FORWARD_MATCHING_INDEXING_INTEGRATE(lhs);    

      Index::ConstrRuleList* crl = leaf->info().rules();   

#ifdef DEBUG_ALLOC
      ASSERT((!crl) || crl->checkObj());
#endif


      crl = new Index::ConstrRuleList(crl); 

      crl->hd().init(cl,0,createStoredSubstitution(),lhs);

      leaf->info().rules() = crl;
    }      
  else
    {
      TERM* arg1ref = lit->Args();
      TERM* arg2ref = arg1ref->Next();
      if ((arg1ref->isVariable()) || (arg2ref->isVariable())) return;        
    
      SimplificationOrdering::SpecialisedComparison* specialisedArgComparison = 
	SimplificationOrdering::current()->reserveSpecialisedComparison();
      
      ASSERT(specialisedArgComparison);
      specialisedArgComparison->load(*arg1ref,*arg2ref);
      if (specialisedArgComparison->lhsVarInstMayOrder())
	{
	  specialisedArgComparison->specialiseGreater();
	  if (appropriateDegreeForGreater(specialisedArgComparison->category()))
	    {
	      SimplificationOrdering::StoredConstraint* constraint = 
		SimplificationOrdering::current()->storedConstraintGreater(specialisedArgComparison);
	      if (constraint)
		{
		  IntegrateConstrained(cl,arg1ref->First(),constraint);

		  SimplificationOrdering::current()->releaseConstraint(constraint);
		};
	    };

	};
      if (specialisedArgComparison->rhsVarInstMayOrder())
	{
	  specialisedArgComparison->specialiseLess();
	  if (appropriateDegreeForLess(specialisedArgComparison->category()))
	    {
	      SimplificationOrdering::StoredConstraint* constraint = 
		SimplificationOrdering::current()->storedConstraintLess(specialisedArgComparison);

	      if (constraint)
		{
		  IntegrateConstrained(cl,arg2ref->First(),constraint);
		  SimplificationOrdering::current()->releaseConstraint(constraint);
		};
	    };
	};

      SimplificationOrdering::current()->releaseSpecialisedComparison(specialisedArgComparison);
    };
}; // void ForwardDemodulation::Index::IntegrateConstrained(Clause* cl)

void ForwardDemodulation::Index::IntegrateConstrained(Clause* cl,
						      TERM* t,
						      SimplificationOrdering::StoredConstraint* constr)
{
  CALL("IntegrateConstrained(Clause* cl,..)");
  
  FMCodeTree<VampireKernelConst::MaxTermSize,VampireKernelConst::MaxNumOfVariables,GlobAlloc,TERM,Flatterm,Index::LeafInfo,Index>::Leaf* 
    leaf = Integrate(t);
  INTERCEPT_FORWARD_MATCHING_INDEXING_INTEGRATE(t);

  Index::ConstrRuleList* crl = leaf->info().rules();  

#ifdef DEBUG_ALLOC
      ASSERT((!crl) || crl->checkObj());
#endif 
  crl = insertConstrRule(cl,constr,createStoredSubstitution(),t,crl);

  leaf->info().rules() = crl;
}; //  void ForwardDemodulation::Index::IntegrateConstrained(Clause* cl,..)


void ForwardDemodulation::Index::RemoveConstrained(Clause* cl) 
{ 
  CALL("RemoveConstrained(Clause* cl)"); 
  
#ifdef DEBUG_NAMESPACE
  if (cl->numOfNonanswerLiterals() > 1)
    {
      LiteralList::Iterator iter(cl->LitList());
      iter.next(); 
      do
	{
	  if ((!iter.currentLiteral()->IsSplitting()) &&
	      (!iter.currentLiteral()->isAnswerLiteral()))
	    {
	      ICP("E1");
	    };
	  iter.next();
	}
      while (iter.notEnd());
    };  
#endif          
  
  ASSERT(cl->LitList()->numOfSelectedLits() == 1); 

  TERM* lit = cl->LitList()->firstLit();

  ASSERT(lit->Positive());
  ASSERT(lit->IsEquality());

  if (lit->IsOrderedEq())
    { 
      RemoveConstrained(cl,lit->Args()->First());
    }
  else
    {       
      TERM* arg1ref = lit->Arg1();
      TERM* arg2ref = lit->Arg2();
      if (arg1ref->isVariable() || arg2ref->isVariable()) return;
      TERM* arg1 = arg1ref->First();
      TERM* arg2 = arg2ref->First();
      RemoveConstrained(cl,arg1);
      RemoveConstrained(cl,arg2);    
    };  
}; // void ForwardDemodulation::Index::RemoveConstrained(Clause* cl)
 
void ForwardDemodulation::Index::RemoveConstrained(Clause* cl,TERM* lhs) 
{ 
  CALL("RemoveConstrained(Clause* cl,TERM* lhs)");
  FMCodeTree<VampireKernelConst::MaxTermSize,VampireKernelConst::MaxNumOfVariables,GlobAlloc,TERM,Flatterm,Index::LeafInfo,Index>::Leaf* leaf;
  FMCodeTree<VampireKernelConst::MaxTermSize,VampireKernelConst::MaxNumOfVariables,GlobAlloc,TERM,Flatterm,Index::LeafInfo,Index>::Instruction** branch_to_destroy;
  if (Remove(lhs,leaf,branch_to_destroy))
    { 
      ASSERT(leaf->isLeaf());
      INTERCEPT_FORWARD_MATCHING_INDEXING_REMOVE(lhs);
      Index::ConstrRuleList* crl = leaf->info().rules();      
      ASSERT(crl);   
#ifdef INTERCEPT_INDEXED_SEARCH_FOR_GENERALISATIONS
      bool reallyRemoved;
      crl = removeConstrRule(cl,lhs,crl,reallyRemoved);
      if (reallyRemoved)
	{	  
	  INTERCEPT_INDEXED_SEARCH_FOR_GENERALISATIONS_REMOVE(lhs);
	};
#else
      crl = removeConstrRule(cl,lhs,crl);
#endif
      leaf->info().rules() = crl;
      if (!crl) // the branch must be destroyed
	{
	  FMCodeTree<VampireKernelConst::MaxTermSize,VampireKernelConst::MaxNumOfVariables,GlobAlloc,TERM,Flatterm,Index::LeafInfo,Index>::DestroyBranch(branch_to_destroy);
	};
    };
}; // void ForwardDemodulation::Index::RemoveConstrained(Clause* cl,TERM* lhs)



inline bool ForwardDemodulation::Index::appropriateDegreeForGreater(const SimplificationOrdering::SpecialisedComparison::Category& cat) const 
{
  CALL("appropriateDegreeForGreater(const SimplificationOrdering::SpecialisedComparison::Category& cat) const");
  switch (cat)
    {
    case SimplificationOrdering::SpecialisedComparison::AlwaysLess:
      // same as below
    case SimplificationOrdering::SpecialisedComparison::AlwaysEqual:
      return false;
    case SimplificationOrdering::SpecialisedComparison::AlwaysGreater:
      return true;
    case SimplificationOrdering::SpecialisedComparison::CanBeLessOrEqual:
      return (degree >= 4);
    case SimplificationOrdering::SpecialisedComparison::CanBeGreaterOrEqual:
      return (degree >= 2);
    case SimplificationOrdering::SpecialisedComparison::VolatileButNearlyAlwaysLess:
      return (degree >= 4);
    case SimplificationOrdering::SpecialisedComparison::VolatileButNearlyAlwaysGreater:
      return (degree >= 2);
    case SimplificationOrdering::SpecialisedComparison::AbsolutelyVolatile:
      return (degree >= 3);
    };

  ICP("ICP1");

#if (defined _SUPPRESS_WARNINGS_) || (defined DEBUG_NAMESPACE)
  return false;
#endif
}; // bool ForwardDemodulation::Index::appropriateDegreeForGreater(const SimplificationOrdering::SpecialisedComparison::Category& cat) const 



inline bool ForwardDemodulation::Index::appropriateDegreeForLess(const SimplificationOrdering::SpecialisedComparison::Category& cat) const 
{
  CALL("appropriateDegreeForLess(const SimplificationOrdering::SpecialisedComparison::Category& cat) const");
  switch (cat)
    {
    case SimplificationOrdering::SpecialisedComparison::AlwaysLess:
      return true;
    case SimplificationOrdering::SpecialisedComparison::AlwaysEqual:
      // same as below
    case SimplificationOrdering::SpecialisedComparison::AlwaysGreater:
      return false;
    case SimplificationOrdering::SpecialisedComparison::CanBeLessOrEqual:
      return (degree >= 2);
    case SimplificationOrdering::SpecialisedComparison::CanBeGreaterOrEqual:
      return (degree >= 4);
    case SimplificationOrdering::SpecialisedComparison::VolatileButNearlyAlwaysLess:
      return (degree >= 2);
    case SimplificationOrdering::SpecialisedComparison::VolatileButNearlyAlwaysGreater:
      return (degree >= 4);
    case SimplificationOrdering::SpecialisedComparison::AbsolutelyVolatile:
      return (degree >= 3);
    };

  ICP("ICP1");

#if (defined _SUPPRESS_WARNINGS_) || (defined DEBUG_NAMESPACE)
  return false;
#endif
}; // bool ForwardDemodulation::Index::appropriateDegreeForLess(const SimplificationOrdering::SpecialisedComparison::Category& cat) const 


inline FMCodeTree<VampireKernelConst::MaxTermSize,VampireKernelConst::MaxNumOfVariables,GlobAlloc,TERM,Flatterm,ForwardDemodulation::Index::LeafInfo,ForwardDemodulation::Index>::Leaf* ForwardDemodulation::Index::Integrate(TERM* t)
{
  CALL("Integrate(TERM* t)");
  ASSERT(t->isComplex()); 

  INTERCEPT_INDEXED_SEARCH_FOR_GENERALISATIONS_INTEGRATE(t);

  _matchingIndex.integrationReset(*t);
  TERM::Iterator iter(t); 
  while (iter.Next()) 
    if (iter.CurrentSymIsVar()) 
      { _matchingIndex.integrationVar(iter.symbolRef()); }
    else _matchingIndex.integrationFunc(iter.symbolRef());

  bool newLeaf;
  FMCodeTree<VampireKernelConst::MaxTermSize,VampireKernelConst::MaxNumOfVariables,GlobAlloc,TERM,Flatterm,ForwardDemodulation::Index::LeafInfo,Index>::Leaf* res =  _matchingIndex.integrationGetLeaf(newLeaf);
  return res;
}; // FMCodeTree<VampireKernelConst::MaxTermSize,VampireKernelConst::MaxNumOfVariables,GlobAlloc,TERM,Flatterm,ForwardDemodulation::Index::LeafInfo,ForwardDemodulation::Index>::Leaf* ForwardDemodulation::Index::Integrate(TERM* t)

inline bool ForwardDemodulation::Index::Remove(TERM* t,FMCodeTree<VampireKernelConst::MaxTermSize,VampireKernelConst::MaxNumOfVariables,GlobAlloc,TERM,Flatterm,ForwardDemodulation::Index::LeafInfo,ForwardDemodulation::Index>::Leaf*& leaf,FMCodeTree<VampireKernelConst::MaxTermSize,VampireKernelConst::MaxNumOfVariables,GlobAlloc,TERM,Flatterm,ForwardDemodulation::Index::LeafInfo,ForwardDemodulation::Index>::Instruction**& branch_to_destroy)
{
  CALL("Remove(TERM* t,FMCodeTree<VampireKernelConst::MaxTermSize,VampireKernelConst::MaxNumOfVariables,GlobAlloc,TERM,Flatterm,ForwardDemodulation::Index::LeafInfo,ForwardDemodulation::Index>::Leaf*& leaf,FMCodeTree<VampireKernelConst::MaxTermSize,VampireKernelConst::MaxNumOfVariables,GlobAlloc,TERM,Flatterm,ForwardDemodulation::Index::LeafInfo,ForwardDemodulation::Index>::Instruction**& branch_to_destroy)");


  if (!_matchingIndex.removalReset(*t)) return false;
  TERM::Iterator iter(t);
  while (iter.Next())  
    {    
      if (iter.CurrentSymIsVar())
	{
	  if (!_matchingIndex.removalVar(iter.symbolRef())) return false;
	}
      else if (!_matchingIndex.removalFunc(iter.symbolRef())) return false;
    };

  return _matchingIndex.removalFinish(leaf,branch_to_destroy);
}; // bool ForwardDemodulation::Index::Remove(TERM* t,FMCodeTree<VampireKernelConst::MaxTermSize,VampireKernelConst::MaxNumOfVariables,GlobAlloc,TERM,Flatterm,ForwardDemodulation::Index::LeafInfo,ForwardDemodulation::Index>::Leaf*& leaf,FMCodeTree<VampireKernelConst::MaxTermSize,VampireKernelConst::MaxNumOfVariables,GlobAlloc,TERM,Flatterm,ForwardDemodulation::Index::LeafInfo,ForwardDemodulation::Index>::Instruction**& branch_to_destroy) 


inline void ForwardDemodulation::Index::Integrate(Clause* cl)
{
  IntegrateConstrained(cl);
};

inline void ForwardDemodulation::Index::Remove(Clause* cl)
{
  RemoveConstrained(cl);
};    

inline FMCodeTree<VampireKernelConst::MaxTermSize,VampireKernelConst::MaxNumOfVariables,GlobAlloc,TERM,Flatterm,ForwardDemodulation::Index::LeafInfo,ForwardDemodulation::Index>::Leaf* ForwardDemodulation::Index::Match(const Flatterm* query) 
{ 
  CALL("Match(const Flatterm* query)");
  return INTERCEPT_FORWARD_MATCHING_INDEXING_RETRIEVAL_RESULT(_matchingIndex.Match(INTERCEPT_FORWARD_MATCHING_INDEXING_RETRIEVAL_QUERY(query)));
}; 

inline FMCodeTree<VampireKernelConst::MaxTermSize,VampireKernelConst::MaxNumOfVariables,GlobAlloc,TERM,Flatterm,ForwardDemodulation::Index::LeafInfo,ForwardDemodulation::Index>::Leaf* ForwardDemodulation::Index::MatchAgain() 
{ 
  return _matchingIndex.MatchAgain();    
}; 

inline
ForwardDemodulation::Index::StoredSubstitution* 
ForwardDemodulation::Index::createStoredSubstitution()
{
  CALL("createStoredSubstitution()");
  StoredSubstitution* result = 0;
  _matchingIndex.integrationGetVariables().ResetIterator();
  for (const ulong* v = _matchingIndex.integrationGetVariables().NextVar(); 
       v; 
       v = _matchingIndex.integrationGetVariables().NextVar())
    {
      result = 
	new StoredSubstitution(VarInstanceLocationPair(*v,_matchingIndex.integrationMapVariable(*v)),
			       result);
    };
  return result;
}; // ForwardDemodulation::Index::StoredSubstitution* ForwardDemodulation::Index::createStoredSubstitution()


//=================================================














