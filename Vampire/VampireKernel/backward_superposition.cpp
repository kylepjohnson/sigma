//
// File:         backward_superposition.cpp
// Description:  Classes dealing with backward superposition.
// Created:      May 06, 2002, Alexandre Riazanov, riazanov@cs.man.ac.uk
// Revised:      May 09, 2002, Alexandre Riazanov, riazanov@cs.man.ac.uk
//               SimplificationOrdering::CompiledConstraintForUnifiers
//               replaced ORD_CONSTR
// Revised:      May 16, 2002, Alexandre Riazanov, riazanov@cs.man.ac.uk
//               Bug fix in RecollectMaxWeightForm().
//==================================================================
#include "backward_superposition.hpp"
#include "DestructionMode.hpp"
#include "VampireKernelInterceptMacros.hpp"
using namespace BK;
//================================================
#undef DEBUG_NAMESPACE
#ifdef DEBUG_BACKWARD_SUPERPOSITION
#define DEBUG_NAMESPACE "BACKWARD_SUPERPOSITION"
#endif
#include "debugMacros.hpp"
//================================================


VK::BACKWARD_SUPERPOSITION::BACKWARD_SUPERPOSITION(NewClause* res,OR_INDEX* unifIndex)
  : bpara_premise(res),
    bpara_machine(res,unifIndex),
    _resolvent(res),
    _noInfBetweenRulesFlag(false)
{
  bpara_premise.SetIndex(0);
  bpara_machine.SetIndex(1);
}; 

VK::BACKWARD_SUPERPOSITION::~BACKWARD_SUPERPOSITION() 
{ 
  CALL("destructor ~BACKWARD_SUPERPOSITION()"); 
}; 
  
void VK::BACKWARD_SUPERPOSITION::init(NewClause* res,OR_INDEX* unifIndex)
{
  CALL("init()");
  bpara_premise.init(res);
  bpara_machine.init(res,unifIndex);
  _resolvent = res;
  _noInfBetweenRulesFlag = false;
  bpara_premise.SetIndex(0);
  bpara_machine.SetIndex(1);    
}; // void VK::BACKWARD_SUPERPOSITION::init(NewClause* res,OR_INDEX* unifIndex)


void VK::BACKWARD_SUPERPOSITION::destroy()
{
  CALL("destroy()");
  bpara_machine.destroy();
  bpara_premise.destroy();
}; // void VK::BACKWARD_SUPERPOSITION::destroy()



bool VK::BACKWARD_SUPERPOSITION::SetQuery(Clause* cl) 
{ 
  bpara_machine.setNoRulesFlag(_noInfBetweenRulesFlag && cl->IsRule());
  bpara_machine.SetQuery(cl);
  bpara_premise.Load(cl);
  _currentPosEq = bpara_premise.NextEq();
  if (!_currentPosEq) return false;
  bpara_machine.LoadEq(_currentPosEq,bpara_premise.GetIndex());
  return true; 
}; // bool VK::BACKWARD_SUPERPOSITION::SetQuery(Clause* cl) 

bool VK::BACKWARD_SUPERPOSITION::CollectResult()
{
  CALL("CollectResult()");
  if ((!bpara_premise.TheClause()->Active()) 
      || (!bpara_machine.candClause()->Active()))
    return false;
      
  if (bpara_premise.MinResultWeight()
      + bpara_machine.candClause()->MinResWeight()
      > _resolvent->MaxWeight())
    {
      _resolvent->AnotherTooHeavyClause();
      return false;
    };            
     
  _resolvent->openGenerated(ClauseBackground::BackSup);
  if (bpara_machine.CollectLiterals() && bpara_premise.CollectLiterals())
    {
      _resolvent->UsedRule(ClauseBackground::BackSup);
      _resolvent->PushAncestor(bpara_premise.TheClause());
      _resolvent->PushAncestor(bpara_machine.candClause());
      return true;
    }
  else return false;
}; // bool VK::BACKWARD_SUPERPOSITION::CollectResult()

bool VK::BACKWARD_SUPERPOSITION::NextInference()
{
 process_literal:
  while (bpara_machine.Paramodulate())
    { 
      if (CollectResult()) return true;
    };
  // try next literal
  _currentPosEq = bpara_premise.NextEq();       
  if (!_currentPosEq) return false;
  bpara_machine.LoadEq(_currentPosEq,bpara_premise.GetIndex());
  goto process_literal;
}; // bool VK::BACKWARD_SUPERPOSITION::NextInference()


//================================================
#undef DEBUG_NAMESPACE
#ifdef DEBUG_BACKWARD_SUPERPOSITION
#define DEBUG_NAMESPACE "BPARA_PREMISE"
#endif
#include "debugMacros.hpp"
//================================================

VK::BPARA_PREMISE::BPARA_PREMISE(NewClause* res) 
  : maxLitIter(), nonmax_weight_form(), max_weight_form(), _resolvent(res)
{};
 
VK::BPARA_PREMISE::~BPARA_PREMISE() { CALL("destructor ~BPARA_PREMISE()"); };

void VK::BPARA_PREMISE::init(NewClause* res)
{
  CALL("init(NewClause* res)");
  maxLitIter.init();
  nonmax_weight_form.init();
  max_weight_form.init();
  _resolvent = res;
}; // void VK::BPARA_PREMISE::init(NewClause* res)
  
void VK::BPARA_PREMISE::destroy() 
{
  CALL("destroy()");
  max_weight_form.destroy();
  nonmax_weight_form.destroy();
  maxLitIter.destroy();
}; // void VK::BPARA_PREMISE::destroy() 


inline void VK::BPARA_PREMISE::CollectNonmaxWeightForm()
{
  CALL("CollectNonmaxWeightForm()");
  nonmax_weight_form.Reset();

  for (LiteralList::Iterator iter(allLiterals,firstNonmaxLit);
       iter.notEnd();
       iter.next())
    if (!iter.currentLiteral()->isAnswerLiteral())
      nonmax_weight_form.Compile(iter.currentLiteral());
}; // void VK::BPARA_PREMISE::CollectNonmaxWeightForm()


inline void VK::BPARA_PREMISE::RecollectMaxWeightForm()
{
  CALL("RecollectMaxWeightForm()");
  max_weight_form.Reset();

  for (LiteralList::Iterator iter(allLiterals); 
       iter.currentElement() != firstNonmaxLit;
       iter.next())
    {
      ASSERT(!iter.currentLiteral()->isAnswerLiteral());
      if (iter.currentElement() == _currentMaxLitElement)
	{              
	  ASSERT(iter.currentElement()->isEquality());
	  ASSERT(iter.currentElement()->positive());
	  if (iter.currentElement()->isOrderedEq())
	    max_weight_form.Compile(*(iter.currentElement()->arg2()));
	}
      else 
	max_weight_form.Compile(iter.currentElement()->literal());
    }; 
}; // void VK::BPARA_PREMISE::RecollectMaxWeightForm()


inline void VK::BPARA_PREMISE::Load(Clause* cl) 
{
  CALL("Load(Clause* cl)");
  clause = cl;
  allLiterals = cl->LitList();
  firstNonmaxLit = allLiterals->nth(allLiterals->numOfSelectedLits());
  maxLitIter.reset(allLiterals);
}; // void VK::BPARA_PREMISE::Load(Clause* cl) 


inline const VK::TERM* VK::BPARA_PREMISE::NextEq()
{
  CALL("NextEq()");
  const TERM* lit;
 check_lit:
  if (maxLitIter.currentElement() == firstNonmaxLit) return 0;  
  _currentMaxLitElement = maxLitIter.currentElement();
  lit = maxLitIter.currentLiteral();
  maxLitIter.next();
  if (lit->Positive() && lit->IsEquality()) 
    {
      RecollectMaxWeightForm();
      return lit;
    }; 
  goto check_lit;
}; //  const TERM* VK::BPARA_PREMISE::NextEq()


inline bool VK::BPARA_PREMISE::CollectLiterals() 
{
  CALL("CollectLiterals()");
  // collect max. literals except the used equality

  //cout << "REWR RULE " << maxLitIter.currentLiteral() << "\n";

  for (LiteralList::Iterator iter(allLiterals); iter.currentElement() != firstNonmaxLit;iter.next())
    if (iter.currentElement()->next() != maxLitIter.currentElement())
      {      

	//cout << "PUSH MAX " << iter.currentLiteral() << "\n";

	if (!_resolvent->PushStandardLit(index,iter.currentLiteral(),iter.currentElement()->isInherentlySelected())) return false; 

	//_resolvent->output(cout << "Success, RES3 = ") << "\n";

      };      

  // collect nonmax. literals
  for (LiteralList::Iterator iter1(allLiterals,firstNonmaxLit);iter1.notEnd();iter1.next())
    {
      ASSERT(!(iter1.currentElement()->isInherentlySelected()));
      if (!_resolvent->PushStandardLit(index,iter1.currentLiteral(),false)) return false;   
    };  
  return true;
}; // bool VK::BPARA_PREMISE::CollectLiterals() 


//================================================
#undef DEBUG_NAMESPACE
#ifdef DEBUG_BACKWARD_SUPERPOSITION
#define DEBUG_NAMESPACE "BPARA_MACHINE"
#endif
#include "debugMacros.hpp"
//================================================= 


VK::BPARA_MACHINE::BPARA_MACHINE(NewClause* res,OR_INDEX* unifIndex) : 
  bpara_mach(unifIndex),     
  _resolvent(res)
{
      
  fst_arg_mem[0].Head().MkFunc(TERM((TERM::Arity)1,(TERM::Functor)0,TERM::PositivePolarity));
  snd_arg_mem[0].Head().MkFunc(TERM((TERM::Arity)1,(TERM::Functor)0,TERM::PositivePolarity));
  _constraintCheck = 0; 

  _compiledConstraint = 0; 
}; // VK::BPARA_MACHINE::BPARA_MACHINE(NewClause* res,OR_INDEX* unifIndex)

VK::BPARA_MACHINE::~BPARA_MACHINE() 
{ 
  CALL("destructor ~BPARA_MACHINE()");   
  if (DestructionMode::isThorough())
    {
      if (_constraintCheck)
	{
	  SimplificationOrdering::current()->releaseConstraintCheckOnUnifier(_constraintCheck);
	};
      if (_compiledConstraint)
	{
	  SimplificationOrdering::current()->releaseCompiledConstraintForUnifiers(_compiledConstraint);
	};
    };
}; // VK::BPARA_MACHINE::~BPARA_MACHINE() 

void VK::BPARA_MACHINE::init(NewClause* res,OR_INDEX* unifIndex)
{
  CALL("init()");
  bpara_mach.init(unifIndex);
  _resolvent = res;
    
  fst_arg_mem[0].Head().MkFunc(TERM((TERM::Arity)1,(TERM::Functor)0,TERM::PositivePolarity));
  snd_arg_mem[0].Head().MkFunc(TERM((TERM::Arity)1,(TERM::Functor)0,TERM::PositivePolarity));
  _constraintCheck = 0; 
    
  _compiledConstraint = 0; 
}; // void VK::BPARA_MACHINE::init(NewClause* res,OR_INDEX* unifIndex)


void VK::BPARA_MACHINE::destroy()
{
  CALL("destroy()");
  if (DestructionMode::isThorough())
    {
      if (_constraintCheck)
	{
	  SimplificationOrdering::current()->releaseConstraintCheckOnUnifier(_constraintCheck);
	};
      if (_compiledConstraint)
	{
	  SimplificationOrdering::current()->releaseCompiledConstraintForUnifiers(_compiledConstraint);
	};
    };
  bpara_mach.destroy();
}; // void VK::BPARA_MACHINE::destroy()



inline void VK::BPARA_MACHINE::LoadEq(const TERM* eq_lit,ulong eq_ind)
{
  CALL("LoadEq(const TERM* eq_lit,ulong eq_ind)");
  ASSERT(eq_lit->IsEquality());
  ASSERT(eq_lit->Positive());
  eq_index = eq_ind; 
  const TERM* arg_ref = eq_lit->Args();
  PrefixSym* mem = fst_arg_mem + 1;
  StandardTermToPrefix(eq_index,*arg_ref,mem);
  fst_arg_mem[0].SetBracket(mem);
     
  // another argument
  arg_ref = arg_ref->Next();
  mem = snd_arg_mem + 1;
  StandardTermToPrefix(eq_index,*arg_ref,mem);
  snd_arg_mem[0].SetBracket(mem);
     
  bpara_mach.LoadLHS(fst_arg_mem);
  rhs = snd_arg_mem + 1;
  unordered_eq = eq_lit->IsUnorderedEq();
  _swapArguments = unordered_eq;

  if (unordered_eq)
    {
      if (!_compiledConstraint)
	_compiledConstraint = SimplificationOrdering::current()->reserveCompiledConstraintForUnifiers();
      ASSERT(_compiledConstraint);
      _compiledConstraint->load(snd_arg_mem,fst_arg_mem);
      if (_compiledConstraint->rhsVarInstMayOrder())
	{
	  _compiledConstraint->compileGreaterOrEqual();
	  _currentConstraint = _compiledConstraint;
	}
      else
	{
	  _currentConstraint = 0;
	};
    }
  else
    {
      _currentConstraint = 0;
    };
     
}; //  void VK::BPARA_MACHINE::LoadEq(const TERM* eq_lit,ulong eq_ind)



inline bool VK::BPARA_MACHINE::Paramodulate()
{
  CALL("Paramodulate()");
 try_next_cand:
  if (bpara_mach.NextCandidate(_currentConstraint,_candClause,_candLitRedexPair)) 
    {
      if (_noRulesFlag && _candClause->IsRule()) goto try_next_cand;
      return true;
    };
  if (_swapArguments)
    {
      _swapArguments = false;
      bpara_mach.LoadLHS(snd_arg_mem);
      rhs = fst_arg_mem + 1;


      if (!_compiledConstraint)
	_compiledConstraint = 
	  SimplificationOrdering::current()->reserveCompiledConstraintForUnifiers();
      ASSERT(_compiledConstraint);
      _compiledConstraint->load(fst_arg_mem,snd_arg_mem);
      if (_compiledConstraint->rhsVarInstMayOrder())
	{
	  _compiledConstraint->compileGreaterOrEqual();
	  _currentConstraint = _compiledConstraint;
	}
      else
	{
	  _currentConstraint = 0;
	};

      goto try_next_cand;          
    };
  return false; 
}; // bool VK::BPARA_MACHINE::Paramodulate()

inline bool VK::BPARA_MACHINE::CollectLiterals() 
{
  CALL("CollectLiterals()");

  const SimplificationOrdering::StoredConstraint* constr = 
    _candLitRedexPair->constraint();
  if (constr) 
    {
      if (!_constraintCheck)
	_constraintCheck = SimplificationOrdering::current()->reserveConstraintCheckOnUnifier();
      ASSERT(_constraintCheck);
      if (_constraintCheck->holds(constr,index))
	return false;
    };
    
  ulong rewritten_lit_num = _candLitRedexPair->litNum();

  LiteralList::Element* rewritten_lit = _candClause->LitList()->nth(rewritten_lit_num);
  for (LiteralList::Iterator iter(_candClause->LitList());iter.notEnd();iter.next())
    {
      if (iter.currentElement() == rewritten_lit)
	{        
	  if (!_resolvent->PushRewrittenLit(index,iter.currentLiteral(),_candLitRedexPair->redex(),rhs,iter.currentElement()->isInherentlySelected()))
	    return false;
	}
      else
	{
	  if ((_simultaneousness > 0) && (iter.currentElement()->arity()))
	    {
	      if (!_resolvent->PushRewrittenLit(index,iter.currentLiteral(),_candLitRedexPair->redex(),rhs,iter.currentElement()->isInherentlySelected()))
		return false;   
	    }
	  else
	    if (!_resolvent->PushStandardLit(index,iter.currentElement()->literal(),iter.currentElement()->isInherentlySelected())) return false;  
	};  
    };    

  RARE((query->PreorderedPositiveUnitEquality() 
	&& (!query->flag(Clause::FlagContainsAnswerLiterals))
	&& (!bpara_mach.VariablesFromIndexAffected())
	&& (query != _candClause)));      

  return true;
};  // bool VK::BPARA_MACHINE::CollectLiterals()

//================================================
#undef DEBUG_NAMESPACE
#ifdef DEBUG_BACKWARD_SUPERPOSITION
#define DEBUG_NAMESPACE "BPARA_MACH"
#endif
#include "debugMacros.hpp"
//=================================================



VK::BPARA_MACH::BPARA_MACH(OR_INDEX* unifIndex) : 
  _unifIndex(unifIndex)
{
};

VK::BPARA_MACH::~BPARA_MACH() { CALL("destructor ~BPARA_MACH()"); };

void VK::BPARA_MACH::init(OR_INDEX* unifIndex)
{
  CALL("init()");
  para_mach.init();
  _unifIndex = unifIndex;
  _candidates.init();
};

void VK::BPARA_MACH::destroy()
{
  CALL("destroy()");
  _candidates.destroy();
  para_mach.destroy();
};


void VK::BPARA_MACH::LoadLHS(PrefixSym* t)
{
  CALL("LoadLHS(PrefixSym* t)");
  INTERCEPT_BACKWARD_SUPERPOSITION_INDEXING_RETRIEVAL_QUERY(t->Arg1());
  para_mach.CompileComplexTerm(t);     
  init_state = true;
}; // void VK::BPARA_MACH::LoadLHS(PrefixSym* t)
            
            
inline 
bool 
VK::BPARA_MACH::NextCandidate(SimplificationOrdering::CompiledConstraintForUnifiers* constr,
			  Clause*& candCl,
			  BackParaCandidates::LitRedexPair*& candLitRedexPair)
{
  CALL("NextCandidate(..)"); 

  if (init_state)
    {
      init_state = false;
      if (para_mach.FindFirstLeaf(*(_unifIndex->BParaTree())))
	{	       

	  INTERCEPT_BACKWARD_SUPERPOSITION_INDEXING_RETRIEVAL_RESULT(para_mach.CurrentLeaf()->Term().First());


	  if (constr && constr->holdsOnCurrentUnifier()) goto try_next_leaf;
	  _candidates.reset(*(para_mach.BPCandidatesFromLeaf()));            
	  ALWAYS(_candidates.next(candCl,candLitRedexPair));
	  return true;
	}
      else 
	{
	  INTERCEPT_BACKWARD_SUPERPOSITION_INDEXING_RETRIEVAL_TOTAL;
	  return false;
	};
    }
  else // !init_state
    {
      if (_candidates.next(candCl,candLitRedexPair)) 
	{
	  return true;
	};
    try_next_leaf: 

      if (para_mach.FindNextLeaf())
	{	        

	  INTERCEPT_BACKWARD_SUPERPOSITION_INDEXING_RETRIEVAL_RESULT(para_mach.CurrentLeaf()->Term().First());

	  if (constr && constr->holdsOnCurrentUnifier()) goto try_next_leaf;
	  _candidates.reset(*(para_mach.BPCandidatesFromLeaf()));            
	  ALWAYS(_candidates.next(candCl,candLitRedexPair));
	  return true;
	}
      else 
	{
	  INTERCEPT_BACKWARD_SUPERPOSITION_INDEXING_RETRIEVAL_TOTAL;
	  return false;  
	};
    };     
}; // bool VK::BPARA_MACH::NextCandidate(..) 

//==================================================================
