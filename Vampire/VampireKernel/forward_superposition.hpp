//
// File:         forward_superposition.hpp
// Description:  Representation of stored terms.
// Created:      Feb 14, 2000, Alexandre Riazanov, riazanov@cs.man.ac.uk
// Revised:      Dec 04, 2001, Alexandre Riazanov, riazanov@cs.man.ac.uk 
//               The array FPARA_PREMISE::_inherentlySelected
//               has been made extendable.
// Revised:      Dec 22, 2001, Alexandre Riazanov, riazanov@cs.man.ac.uk
//               In FPARA_PREMISE it was implicitely assumed that 
//               <num of lits>*HellConst::MaxTermSize <= HellConst::MaxClauseSize
//               which is now wrong since HellConst::MaxClauseSize is
//               not defined via HellConst::MaxNumOfLiterals.
// Revised:      Apr 28, 2002, Alexandre Riazanov, riazanov@cs.man.ac.uk
//               SimplificationOrdering::StoredConstraint 
//               replaced SharedKBConstr
// Revised:      May 09, 2002, Alexandre Riazanov, riazanov@cs.man.ac.uk
//               SimplificationOrdering::CompiledConstraintForUnifiers
//               replaced ORD_CONSTR

//====================================================
#ifndef FORWARD_SUPERPOSITION_H
//============================================================
#define FORWARD_SUPERPOSITION_H
#include "jargon.hpp"
#include "VampireKernelDebugFlags.hpp"
#include "VampireKernelConst.hpp"
#include "Stack.hpp"
#include "para_machine.hpp"
#include "NewClause.hpp"
#include "weight_formula.hpp"
#include "Array.hpp"
#include "ExpandingStack.hpp"
#include "GlobAlloc.hpp"
#include "SimplificationOrdering.hpp"
//================================================
#undef DEBUG_NAMESPACE
#ifdef DEBUG_FORWARD_SUPERPOSITION
#define DEBUG_NAMESPACE "FPARA_MACH"
#endif
#include "debugMacros.hpp"
//=================================================
namespace VK
{
class OR_INDEX;
class FPARA_MACH 
{
 private:
  enum STATE { INIT,  
	       PARA_FROM_COMPLEX, PARA_FROM_VAR };
 private:   
  STATE state;
  PARA_MACH para_mach;
  PrefixSym* goal;
  TERM pseudo_header; 
  OR_INDEX* _unifIndex;
 public:
  FPARA_MACH(OR_INDEX* unifIndex);
  ~FPARA_MACH();
  void init(OR_INDEX* unifIndex);
  void destroy();

  void SetIndex(ulong ind) 
    {
      CALL("SetIndex(ulong ind)");
      para_mach.SetIndex(ind); 
    };
  ulong GetIndex() const { return para_mach.GetIndex(); };
  PrefixSym* Goal() const { return goal; };
  void LoadTermToRewrite(PrefixSym* t)
    {
      CALL("LoadTermToRewrite(PrefixSym* t)");
      // t must be a plain complex term with all brackets set
      goal = t;
      para_mach.CompileComplexTerm(goal);
      state = INIT;
    };
    
  RES_PAIR_LIST* NextCandidate(SimplificationOrdering::CompiledConstraintForUnifiers* constr);
}; // class FPARA_MACH 
}; // namespace VK

//================================================
#undef DEBUG_NAMESPACE
#ifdef DEBUG_FORWARD_SUPERPOSITION
#define DEBUG_NAMESPACE "FPARA_MACHINE"
#endif
#include "debugMacros.hpp"
//================================================= 
 
namespace VK
{
class FPARA_MACHINE : public FPARA_MACH
{
 public:
  FPARA_MACHINE(NewClause* res,OR_INDEX* unifIndex);
  ~FPARA_MACHINE();
  void init(NewClause* res,OR_INDEX* unifIndex);
  void destroy();

  void setNoRulesFlag(bool fl) { _noRulesFlag = fl; };
  void SetIndex(ulong ind) 
    {
      CALL("SetIndex(ulong ind)");
      index = ind; 
      FPARA_MACH::SetIndex(ind); 
    };
  const ulong& GetIndex() const { return index; };
  RES_PAIR_LIST* Candidate() const { return candidate; };
  bool LoadLiteral(PrefixSym* lit,bool inhSel);
  bool LoadRestOfLit();
  bool Paramodulate();

  bool CollectResult()
    {
      CALL("CollectResult()");
      return CollectRewrittenLiteral() && CollectRestOfTheRewritingClause();
    };
 private:
  bool CollectRewrittenLiteral()
    {
      CALL("CollectRewrittenLiteral()");
      return _resolvent->PushRewrittenLit(literal,
					  FPARA_MACH::Goal(),
					  candidate->ReplacingTerm(),
					  index,
					  inherentlySelected);
    };  
    
  bool CollectRestOfTheRewritingClause();  
 
 private:
  BK::Stack<PrefixSym*,VampireKernelConst::MaxTermSize> complex_positions;
  PrefixSym* literal;
  bool inherentlySelected;
  RES_PAIR_LIST* candidate;
  ulong index;

   
  SimplificationOrdering::CompiledConstraintForUnifiers* _compiledConstraint;

  SimplificationOrdering::CompiledConstraintForUnifiers* _currentConstraint;


  PrefixSym* _eqArg1;
  PrefixSym* _eqArg2;
  bool _swapArguments;

  NewClause* _resolvent;
  bool _noRulesFlag;
}; // class FPARA_MACHINE : public FPARA_MACH
}; // namespace VK


 
//================================================
#undef DEBUG_NAMESPACE
#ifdef DEBUG_FORWARD_SUPERPOSITION
#define DEBUG_NAMESPACE "FPARA_PREMISE"
#endif
#include "debugMacros.hpp"
//=================================================

namespace VK
{
class FPARA_PREMISE
{
 private:
  Clause* clause;
  ulong index;
  PrefixSym max_literal[VampireKernelConst::MaxClauseSize];
 
  PrefixSym* _freeMaxLitMemory; 

  BK::ExpandingStack<BK::GlobAlloc,PrefixSym*,32UL,FPARA_PREMISE> _maxLiterals;
    

  BK::Array<BK::GlobAlloc,bool,32UL,FPARA_PREMISE> _inherentlySelected;


  ulong num_of_max_literals;
   
  LiteralList* _allLiterals;
  LiteralList::Element* _firstNonselectedLit;

  ulong next_max_lit;
  bool eq_res_applicable;
  WEIGHT_FORMULA weight_form;
  NewClause* _resolvent;
 public:
  FPARA_PREMISE(NewClause* res);
  ~FPARA_PREMISE();
  void init(NewClause* res);
  void destroy();

  void SetIndex(ulong ind) 
    {
      CALL("SetIndex(ulong ind)");
      index = ind; 
      weight_form.SetIndex(ind); 
    };
  Clause* Query() const { return clause; };
  void ResetIter() { next_max_lit = num_of_max_literals; };
  bool EqResApplicable() const { return eq_res_applicable; };
  void CreateWeightFormula()
    {
      CALL("CreateWeightFormula()");
      weight_form.Reset();

      for(LiteralList::Iterator iter(_allLiterals,_firstNonselectedLit);
	  iter.notEnd();
	  iter.next())
	if (!iter.currentLiteral()->isAnswerLiteral())
	  weight_form.Compile(iter.currentLiteral());


     
      ResetIter();
      for (PrefixSym* m_lit = NextMaxLitMem(); m_lit; m_lit = NextMaxLitMem())
	{
	  TERM header = m_lit->Head().Func();
	  if (header.IsEquality() && header.IsOrderedEq()) weight_form.Compile(m_lit->Arg2()); 
	};
      
    };
   
  void Load(Clause* cl);

  PrefixSym* NextMaxLit(bool& inherentlySelected)
    {
      if (next_max_lit)
	{
	  next_max_lit--;
	  return MaxLit(next_max_lit,inherentlySelected);
	};
      return 0;
    }; 

  PrefixSym* NextMaxLitMem()
    {
      if (next_max_lit)
	{
	  next_max_lit--;
	  return MaxLitMem(next_max_lit);
	};
      return 0;
    }; 

  long MinResultWeight()
    {
      return weight_form.Compute();
    };
    
  bool CollectLiterals();
 private:
  PrefixSym* MaxLit(ulong n,bool& inherentlySelected) 
    { 
      CALL("MaxLit(ulong n,bool& inherentlySelected)");
      inherentlySelected = _inherentlySelected[n];
      return _maxLiterals.nth(n);
      //return max_literal + (n*VampireKernelConst::MaxTermSize); 
    };
  PrefixSym* MaxLitMem(ulong n) 
    { 
      CALL("MaxLitMem(ulong n)");
      return _maxLiterals.nth(n);
      //return max_literal + (n*VampireKernelConst::MaxTermSize); 
    };
}; // class FPARA_PREMISE
}; // namespace VK

//================================================
#undef DEBUG_NAMESPACE
#ifdef DEBUG_FORWARD_SUPERPOSITION
#define DEBUG_NAMESPACE "FORWARD_SUPERPOSITION"
#endif
#include "debugMacros.hpp"
//=================================================

namespace VK
{
class FORWARD_SUPERPOSITION
{
 public:
  FORWARD_SUPERPOSITION(NewClause* res,OR_INDEX* unifIndex);
  ~FORWARD_SUPERPOSITION();
  void init(NewClause* res,OR_INDEX* unifIndex);
  void destroy();

  void setNoInfBetweenRulesFlag(bool fl)
    {
      _noInfBetweenRulesFlag = fl;     
    };
  bool Load(Clause* cl);
  bool NextInference()
    { 
      CALL("NextInference()");
      switch (_continuation)
	{
	case ContFSup:
	  while (NextFSupInference())
	    if (CollectResult()) return true;
	  // no more superpositions, start equality resolution 
	  if (!fpara_premise.EqResApplicable()) return false;
	  fpara_premise.ResetIter();
	  _continuation = ContEqRes;
	  // no break here, proceed with the next case
	case ContEqRes:
	  while (NextEqRes())
	    {
	      if (CollectEqResResult())
		{
		  UndoEqRes();         
		  return true;
		};
	      UndoEqRes(); 
	    }; 
	  // no more inferences
	  return false;
	};
      ICP("ICP1");
      return false;
    };
 private:
  bool NextFSupInference();
  void UndoEqRes()
    {
      Unifier::current()->UnbindLastSection();
    }; 
  bool NextEqRes();
  bool CollectResult();
  bool CollectEqResResult();
  
 private: 
  enum Continuation { ContFSup, ContEqRes };
 private:
  FPARA_MACHINE fpara_machine;
  FPARA_PREMISE fpara_premise;
  NewClause* _resolvent;
  Continuation _continuation;
  SimplificationOrdering::ConstraintCheckOnUnifier* _constraintCheck;
  bool _noInfBetweenRulesFlag;
}; // class FORWARD_SUPERPOSITION
}; // namespace VK

//=================================================================
#endif
