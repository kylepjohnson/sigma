#ifndef NEW_CLAUSE_H
//=========================================================================
#define NEW_CLAUSE_H
#include <iostream>
#include "jargon.hpp"
#include "VampireKernelDebugFlags.hpp"
#include "VampireKernelConst.hpp"
#include "Term.hpp"
#include "prefix.hpp"
#include "variables.hpp"
#include "OpenNewClause.hpp"
#include "ClauseBackground.hpp"
#include "ForwardDemodulation.hpp"
#include "ClauseQueue.hpp"
#include "PagedPassiveClauseStorage.hpp"
#include "Splitting.hpp"
#include "Clause.hpp"
namespace VK
{
class TmpLiteral;
class ForwardSubsumptionFrontEnd;
class ForwardDemodulation;
class ComponentName;
class ClauseAssembler;
}; // namespace VK
//================================================
#undef DEBUG_NAMESPACE
#ifdef DEBUG_NEW_CLAUSE
#define DEBUG_NAMESPACE "NewClause"
#endif 
#include "debugMacros.hpp"
//=================================================

namespace VK
{
class NewClause
{
 public:
  class Statistics
    {
    public:
      Statistics() { reset(); };
      ~Statistics() {};
      void init() { reset(); };
      void destroy() {};
      void reset() 
	{	  
	  generatedByPrepro = 0UL;
	  generatedByReanimation = 0UL;
	  generatedByResolution = 0UL;
	  generatedByForwSup = 0UL;
	  generatedByBackSup = 0UL;
	  generatedByEqRes = 0UL;
	  generatedByEqFact = 0UL;
	  generatedByBackDemod = 0UL;
	  forwardSubsumed = 0UL;
	  simplifiedByFSR = 0UL;
	  simplifiedByDemod = 0UL;
	  simplifiedByBuiltInTheories = 0UL;
	  fDOnSplittingBranches = 0UL;
	  simplifiedByEqRes = 0UL;
	  tooDeepInferences = 0UL;
	};

      ulong generatedByPrepro;
      ulong generatedByReanimation;
      ulong generatedByResolution;
      ulong generatedByForwSup;
      ulong generatedByBackSup;
      ulong generatedByEqRes;
      ulong generatedByEqFact;
      ulong generatedByBackDemod;

      ulong forwardSubsumed;
      ulong simplifiedByFSR;
      ulong simplifiedByDemod;
      ulong simplifiedByBuiltInTheories;
      ulong fDOnSplittingBranches;
      ulong simplifiedByEqRes;
      ulong tooDeepInferences;
    };

 public:
  NewClause();
  ~NewClause();   
  void init();
  void destroy();


  /*******  
  void Reset() // should be renamed not to confuse it with reset()
    {               
      _statistics.generated++;
      _openClause.Reset();
      _ancestors.Reset();
    };
  ***********/
  
  void openGenerated(ClauseBackground::Rule rule)
  {
    CALL("openGenerated(ClauseBackground::Rule rule)");
    _openClause.Reset();
    _ancestors.Reset();
    switch (rule)
      {
      case ClauseBackground::Prepro:
	++_statistics.generatedByPrepro;
	break;
      case ClauseBackground::BinRes:
	++_statistics.generatedByResolution;
	break;
      case ClauseBackground::ForwSup:
	++_statistics.generatedByForwSup;
	break;
      case ClauseBackground::BackSup:
	++_statistics.generatedByBackSup;
	break;
      case ClauseBackground::EqRes:
	++_statistics.generatedByEqRes;
	break;
      case ClauseBackground::EqFact:
	++_statistics.generatedByEqFact;
	break;
      case ClauseBackground::BackDemod:
	++_statistics.generatedByBackDemod;
	break;
      case ClauseBackground::Reanimation:
	++_statistics.generatedByReanimation;
	break;

#ifdef DEBUG_NAMESPACE
      default:
	ICP("ICP0");
#else
#  ifdef _SUPPRESS_WARNINGS_
	BK::RuntimeError::report("Wrong argument value in NewClause::openGenerated(ClauseBackground::Rule rule)");
#  endif
#endif
      };
  }; // void openGenerated(ClauseBackground::Rule rule)




  // setting options and parameters
  void SetClauseIndex(ClauseAssembler* ca) 
    { 
      _clauseAssembler = ca;
      _splitting.setClauseAssembler(ca); 
    }; 
  void SetComponentName(ComponentName* cn) 
    { 
      //_compName = cn;
      _splitting.setComponentNamingService(cn); 
    };
  void SetForwardSubsumption(ForwardSubsumptionFrontEnd* fs) 
    { 
      _splitting.setForwardSubsumption(fs);
      _forwardSubsumption = fs; 
    };
  void SetSimplificationByUEq(ForwardDemodulation* fsueq) { _forwardSimplificationByUEq = fsueq; };
  void SetFDOnSplittingBranchesFlag(bool fl) { _fDOnSplittingBranchesFlag = fl; };
  void SetNormalizationByCommutativity(const SymbolInfo* com_tab) { _commutativityTable = com_tab; }; 
  //void SetDefHeaderTable(const DefinedHeaderTable* t) { _defHeaderTab = t; }; 
   
  void SetForwardSubsumptionFlag(bool fl) 
    {
      _splitting.setForwardSubsumptionFlag(fl); 
      _forwardSubsumptionFlag = fl; 
    };

  void SetSimplificationByFS(long fl) 
    {
      if (fl > 0)
	{
	  _forwardSubsumptionResolution = (ulong)fl; 
	  _splitting.setForwardSubsumptionResolutionFlag(true);
	}
      else 
	{
	  _forwardSubsumptionResolution = 0;
	  _splitting.setForwardSubsumptionResolutionFlag(false);
	};
    };
  void SetSimplificationByUEq(long fl) { _forwardDemodulation = fl; };
  void SetSimplificationByEqRes(bool fl) { _simplificationByEqRes = fl; };
  void SetSplitting(bool fl) 
    { 
      _splittingFlag = fl;
    };
  void SetSplittingStyle(long num) 
    { 
      //_splittingStyle = num;
      _splitting.setStyle(num);
    };
  void SetSplittingWithNaming(bool fl) 
    { 
      _splitting.setSplittingWithNamingFlag(fl);
      //_splittingWithNamingFlag = fl; 
    };

  void SetNegEqSplitting(long w) 
    { 
      _negEqSplitting = w;
      if (w > 0L) 
	{
	  _splitting.setNegEqSplittingMinEligibleTermSize(w + 1);
	}
      else 
	{
	  _splitting.setNegEqSplittingMinEligibleTermSize(LONG_MAX);
	}; 
    };

  void SetEqualityPossible(bool fl) { _equalityPossible = fl; };
  void SetMaxWeight(ulong mw) 
    {
      _previousMaxWeight = _maxWeight;
      _maxWeight = (mw > VampireKernelConst::MaxClauseSize) ? VampireKernelConst::MaxClauseSize : mw;
      if (_maxWeight < VampireKernelConst::MinMaxWeight) _maxWeight = VampireKernelConst::MinMaxWeight;
      _openClause.SetMaxWeight(_maxWeight);
    };
  void setMaxInferenceDepth(long maxInferenceDepth)
    {
      _maxInferenceDepth = maxInferenceDepth;
    };
  void SetMaxLength(long ml) { _openClause.SetMaxLength(ml); };
  void SetMaxLitWeight(long mlw) { _openClause.SetMaxLitWeight(mlw); };

  // getting options and parameters 

  long MaxWeight() const { return _maxWeight; };
  long PreviousMaxWeight() const { return _previousMaxWeight; };


  // keeping statistics 

  void AnotherForwardSubsumed() { _statistics.forwardSubsumed++; };

  void AnotherSimplifiedByFSR() { _statistics.simplifiedByFSR++; };
  void AnotherSimplifiedByDemod() { _statistics.simplifiedByDemod++; };  
  void anotherSimplifiedByBuiltInTheories() { _statistics.simplifiedByBuiltInTheories++; };  
  void AnotherFDOnSplittingBranches() { _statistics.fDOnSplittingBranches++; };
  void AnotherSimplifiedByEqRes() { _statistics.simplifiedByDemod++; };  
  void AnotherTooHeavyClause() { _openClause.AnotherTooHeavyClause(); };
  void anotherClauseWithTooDeepInference()
    {
      ++_statistics.tooDeepInferences;
    };

  // getting statistics 


    ulong statNumOfClGeneratedByPrepro() const { return _statistics.generatedByPrepro; };
    ulong statNumOfClGeneratedByReanimation() const { return _statistics.generatedByReanimation; };
    ulong statNumOfClGeneratedByResolution() const { return _statistics.generatedByResolution; };    
    ulong statNumOfClGeneratedByForwSup() const { return _statistics.generatedByForwSup; };
    ulong statNumOfClGeneratedByBackSup() const { return _statistics.generatedByBackSup; };
    ulong statNumOfClGeneratedByEqRes() const { return _statistics.generatedByEqRes; };
    ulong statNumOfClGeneratedByEqFact() const { return _statistics.generatedByEqFact; };  
    ulong statNumOfClGeneratedByBackDemod() const { return _statistics.generatedByBackDemod; };
    



  ulong statTooDeepInferences() const
    {
      return _statistics.tooDeepInferences;
    };

  ulong statTooHeavy() const 
    { 
      return _openClause.statistics.tooHeavy; 
    };
  ulong statTooHeavyLiterals() const 
    { 
      return _openClause.statistics.tooHeavyLiterals;
    };
  ulong statTooManyVariables() const 
    { 
      return _openClause.statistics.tooManyVariables; 
    };
  ulong statTooLong() const 
    { 
      return _openClause.statistics.tooLong; 
    };
  ulong statTooDeep() const 
    { 
      return _openClause.statistics.tooDeep; 
    };
  ulong statEqTautologies() const 
    { 
      return _openClause.statistics.eqTautologies; 
    };
  ulong statPropTautologies() const 
    { 
      return _openClause.statistics.propTautologies; 
    };

  ulong statForwardSubsumed() const 
    { 
      return _statistics.forwardSubsumed + 
	_splitting.statForwardSubsumed(); 
    };
  ulong statSimplifiedByFSR() const 
    { 
      return _statistics.simplifiedByFSR +
	_splitting.statSimplifiedByFSR(); 
    };
  ulong statSimplifiedByDemod() const { return _statistics.simplifiedByDemod; };
  ulong statSimplifiedByBuiltInTheories() const
    {
      return _statistics.simplifiedByBuiltInTheories;
    };
  ulong statFDOnSplittingBranches() const { return _statistics.fDOnSplittingBranches; };
  ulong statSimplifiedByEqRes()  const { return _statistics.simplifiedByEqRes; };
  ulong statSplit() const 
    { 
      return _splitting.statSplit(); 
    };
  ulong statComponents() const 
    { 
      return _splitting.statComponents(); 
    };

  ulong statDifferentComponents() const 
    { 
      return _splitting.statDifferentComponents(); 
    };
   
  const bool& WasTautology() const { return _openClause.WasTautology(); };   

 public: // collecting a clause 

  void MakeVIP() { _ancestors.MakeVIP(); };
  bool PushSymbol(const TERM& sym)
    {
      if (sym.isVariable()) { PushVar(sym); return true; };
      if (sym.arity()) return PushNonconstFun(sym);
      PushConst(sym);
      return true;
    };   
  bool PushFunc(const TERM& f)
    {
      CALL("PushFunc(const TERM& f)");
      ASSERT(f.isComplex());
      if (f.arity()) return PushNonconstFun(f);
      PushConst(f); 
      return true;
    };
 
  bool OpenNonpropLiteral(const TERM& header,bool inherentlySelected) 
    { 
      return _openClause.OpenNonpropLiteral(header,inherentlySelected); 
    };
  bool CloseLiteral() { return _openClause.CloseLiteral(); };
  bool PushPropLit(const TERM& lit,bool inherentlySelected) 
    { 
      return _openClause.PushPropLit(lit,inherentlySelected); 
    }; 

  bool PushRewrittenLit(ulong index,const TERM* lit,const TERM* redex,PrefixSym* replacing_term,bool inherentlySelected);
  bool PushRewrittenLit(PrefixSym* lit,
			PrefixSym* redex,
			TERM replacing_term,
			ulong index,
			bool inherentlySelected);
  bool PushPrefixLit(PrefixSym* word,bool inherentlySelected); 
  bool PushPrefixEqLit(TERM header,PrefixSym* arg1,PrefixSym* arg2,bool inherentlySelected);
  bool PushPrefixComplexTerm(PrefixSym* word);
  bool PushPrefixTerm(PrefixSym* t);
  bool PushStandardLit(ulong index,TERM* t,bool inherentlySelected); 
  bool PushStandardTerm(ulong index,TERM t);
  bool PushStandardComplexTerm(ulong index,TERM* t);
  bool PushLiteral(TERM* t,bool inherentlySelected);   
  bool loadClause(const Clause* c,ClauseBackground::Rule rule);
  bool loadClause(PagedPassiveClauseStorage::ClauseLiterals& literals,ClauseBackground::Rule rule);

  void setClauseNumber(long num) { _clauseNumber = num; }; 
  void UsedRule(ClauseBackground::Rule r) { _ancestors.UsedRule(r); }; 
  void CancelRule(ClauseBackground::Rule r) { _ancestors.CancelRule(r); }; 
  void UsedRules(const ClauseBackground::Rules& rs) { _ancestors.UsedRules(rs); }; 
  void PushAncestor(Clause* cl) 
    {
      CALL("PushAncestor(Clause* cl)");
      if (cl->isSubgoal()) _ancestors.markAsSubgoal();
      _ancestors.pushAncestor(cl,cl->inferenceDepth()); 
    };
  void InverseAncestorOrder() { _ancestors.InverseOrder(); };
  void markAsSupport() { _ancestors.markAsSupport(); };
 public:  // processing the collected clause
  /***
  Clause* FSubsume(Clause* cl);
  *****/
  void ProcessCollected(ClauseQueue& newClauses);
  void ProcessCollectedSupport(ClauseQueue& newClauses);
  void ProcessCollected2(ClauseQueue& newClauses);
  void ProcessCollected3(Clause*& emptyClause,
			 PagedPassiveClauseStorage& storage,
			 ulong& numOfNewClauses,
			 ulong& numOfRejectedNewClauses,
			 Clause::NewNumber* clauseNumberGenerator,
			 bool showNewClauses); 
  Clause* Reanimate();

  bool transferToPagedPassive(PagedPassiveClauseStorage& storage,bool showNewClause);

 public: // output
  ostream& output(ostream& str) const;
  ostream& outputAncestors(ostream& str) const;
   
 private:  // collecting a clause 
  bool PushVar(const ulong v) { return _openClause.PushVar(v); };
  void PushVar(const TERM& v) { _openClause.PushVar(v); };  
  void PushConst(const TERM& c) { _openClause.PushConst(c); };
  bool PushNonconstFun(const TERM& f) { return _openClause.PushNonconstFun(f); };

 private: // processing collected clause
  void Simplify(bool& redundant);
  Clause* FSubsume();
  ulong SimplifyByFS();
  Clause* RewriteByUEq(TmpLiteral* lit,TmpLiteral* new_lit);
  bool SimplifyByUEq(TmpLiteral*& lit,TmpLiteral*& spare_lit);
  bool SimplifyByUEq();
  bool simplifyByBuiltInTheories(TmpLiteral*& lit,TmpLiteral*& spareLit);
  bool simplifyByBuiltInTheories();
  bool SimplifyByEqRes();
   
  bool NormalizeByCommutativity();  
  bool NormalizeByCommutativity(TmpLiteral* lit);
 
 private: // structure

  Statistics _statistics;  

  OpenNewClause _openClause;

  Splitting _splitting;

  long _clauseNumber;
  OpenClauseBackground _ancestors;
  VAR_TO_INT_RENAMING _varRenaming;
  INT_VAR_RENAMING _intVarRenaming;
  ClauseAssembler* _clauseAssembler;

  ForwardSubsumptionFrontEnd* _forwardSubsumption;
  ForwardDemodulation* _forwardSimplificationByUEq;
  bool _fDOnSplittingBranchesFlag;
  ForwardDemodulation::SetOfSplittingBranches _splittingBranches;

 private: // options and parameters
  bool _forwardSubsumptionFlag; 
  ulong _forwardSubsumptionResolution;
  long _forwardDemodulation;
  const SymbolInfo* _commutativityTable;
  bool _simplificationByEqRes;
  bool _splittingFlag;
  long _negEqSplitting;  
  bool _equalityPossible;
  long _maxWeight; 
  long _previousMaxWeight;
  long _maxInferenceDepth;
}; // class NewClause 

//=================================================================

inline ostream& operator<<(ostream& str,const NewClause& cl) 
{
  return cl.output(str);
};

inline ostream& operator<<(ostream& str,const NewClause* cl) 
{
  return cl->output(str);
};


inline bool NewClause::PushRewrittenLit(ulong index,const TERM* lit,const TERM* redex,PrefixSym* replacing_term,bool inherentlySelected)
{
  CALL("PushRewrittenLit(ulong index,const TERM* lit,const TERM* redex,PrefixSym* replacing_term,bool inherentlySelected)");
  TERM header = *lit;
  ASSERT(header.arity()); 
  if (header.IsEquality()) header.MakeUnordered();
  if (!(OpenNonpropLiteral(header,inherentlySelected))) return false;
  
  StdTermWithSubst iter;
  iter.Reset(index,lit);
  iter.SkipSym();
  PrefSym sym;
  
  while (!(iter.Empty()))
    {
      if (iter.CurrPosInBase() == redex)
	{
	  if (!PushPrefixTerm(replacing_term)) return false;
	  iter.SkipTerm();
	}
      else
	{ 
	  sym = iter.CurrSym();
	  if (sym.isVariable()) // free variable, must be renamed
	    {
	      if (!(PushVar(_varRenaming.Rename(sym.var())))) { return false; };
	    }
	  else
	    {
	      if (sym.Func().arity())
		{
		  if (!(PushNonconstFun(sym.Func()))) return false;
		}
	      else PushConst(sym.Func()); 
	    };
	  iter.SkipSym();
	};  
    };
   
  return CloseLiteral();
}; // bool NewClause::PushRewrittenLit(ulong index,const TERM* lit,const TERM* redex,PrefixSym* replacing_term,bool inherentlySelected)


inline bool NewClause::PushPrefixLit(PrefixSym* word,bool inherentlySelected)
{
  CALL("PushPrefixLit(PrefixSym* word,bool inherentlySelected)");
  ASSERT(Prefix::CorrectComplexTerm(word));
  TERM header = word->Head().Func();
  if (!(header.arity())) { return PushPropLit(header,inherentlySelected); };
  
  if (!(OpenNonpropLiteral(header,inherentlySelected))) return false;
  
  Prefix::Iter iter;
  iter.Reset(word);
  TERM sym;
  iter.SkipSym(); // skipping the header
  do
    {
      if (iter.CurrVar()) // free variable
	{ 
	  if (!(PushVar(_varRenaming.Rename(iter.CurrVar())))) { return false; };  
	}
      else // functor
	{
	  sym = iter.CurrSubterm()->Head().Func();
	  if (sym.arity())
	    {
	      if (!(PushNonconstFun(sym))) return false;
	    }
	  else // constant
	    PushConst(sym);
	};
      iter.SkipSym();
    }
  while (!(iter.Empty()));
  return CloseLiteral();
}; // bool NewClause::PushPrefixLit(PrefixSym* word,bool inherentlySelected)



inline bool NewClause::PushPrefixEqLit(TERM header,PrefixSym* arg1,PrefixSym* arg2,bool inherentlySelected)
{
  CALL("PushPrefixEqLit(TERM header,PrefixSym* arg1,PrefixSym* arg2,bool inherentlySelected)");
  ASSERT(header.IsEquality());   
  
  return OpenNonpropLiteral(header,inherentlySelected)
    && PushPrefixTerm(arg1)
    && PushPrefixTerm(arg2)
    && CloseLiteral();
}; // bool NewClause::PushPrefixEqLit(TERM header,PrefixSym* arg1,PrefixSym* arg2,bool inherentlySelected)


inline bool NewClause::PushPrefixComplexTerm(PrefixSym* word)
{
  CALL("PushPrefixComplexTerm(PrefixSym* word)");
  Prefix::Iter iter;
  iter.Reset(word);
  TERM sym;
  do
    {
      if (iter.CurrVar()) // free variable
	{ 
	  if (!(PushVar(_varRenaming.Rename(iter.CurrVar())))) return false; 
	}
      else // functor
	{
	  sym = iter.CurrSubterm()->Head().Func();
	  if (sym.arity())
	    {
	      if (!(PushNonconstFun(sym))) return false;
	    }
	  else // constant
	    PushConst(sym);
	};
      iter.SkipSym();
    }
  while (!(iter.Empty()));
  return true;
}; // bool NewClause::PushPrefixComplexTerm(PrefixSym* word)

inline bool NewClause::PushPrefixTerm(PrefixSym* t)
{
  CALL("PushPrefixTerm(PrefixSym* t)");
  if (t->Head().isVariable())
    {
      bool free_var;
      void* bind = t->Head().var()->Unref(free_var);
      if (free_var) return PushVar(_varRenaming.Rename(static_cast<Unifier::Variable*>(bind)));
      return PushPrefixComplexTerm(static_cast<PrefixSym*>(bind)); 
    };
  return PushPrefixComplexTerm(t);
}; // bool NewClause::PushPrefixTerm(PrefixSym* t)

inline bool NewClause::PushStandardTerm(ulong index,TERM t)
{
  CALL("PushStandardTerm(ulong index,TERM t)");
  // t is variable or reference 
  if (t.isVariable())
    {
      bool free_var;
      void* bind = Unifier::current()->variable(t.var(),index)->Unref(free_var);
    
      if (free_var) return PushVar(_varRenaming.Rename(static_cast<Unifier::Variable*>(bind)));
      return PushPrefixComplexTerm(static_cast<PrefixSym*>(bind));
    };
  // t is reference to a complex term
  return PushStandardComplexTerm(index,t.First());
}; // bool NewClause::PushStandardTerm(ulong index,TERM t)

inline bool NewClause::PushStandardComplexTerm(ulong index,TERM* t)
{
  CALL("PushStandardComplexTerm(ulong index,TERM* t)");
  StdTermWithSubst term;
  term.Reset(index,t);
  PrefSym sym;
  do
    {
      sym = term.CurrSym();
      if (sym.isVariable()) // free variable, must be renamed
	{
	  if (!(PushVar(_varRenaming.Rename(sym.var())))) return false; 
	}
      else
	{
	  if (sym.Func().arity())
	    {
	      if (!(PushNonconstFun(sym.Func()))) return false;
	    }
	  else PushConst(sym.Func()); 
	};
      term.SkipSym();
    }
  while (!(term.Empty()));
  return true; 
}; // inline bool NewClause::PushStandardComplexTerm(ulong index,TERM* t)

inline bool NewClause::PushStandardLit(ulong index,TERM* t,bool inherentlySelected)
{
  CALL("PushStandardLit(ulong index,TERM* t,bool inherentlySelected)");

  // Note: this function uses the global substitution to substitute the variables!
  TERM header = *t;

  if (!(header.arity())) { return PushPropLit(header,inherentlySelected); };

  if (!(OpenNonpropLiteral(header,inherentlySelected))) return false;
 
  StdTermWithSubst term;
  term.Reset(index,t);
  term.SkipSym();
  PrefSym sym;
  while (!(term.Empty()))
    {
      sym = term.CurrSym();
      if (sym.isVariable()) // free variable, must be renamed
	{
	  if (!(PushVar(_varRenaming.Rename(sym.var())))) { return false; };
	}
      else
	{
	  if (sym.Func().arity())
	    {
	      if (!(PushNonconstFun(sym.Func()))) return false;
	    }
	  else 
	    {
	      PushConst(sym.Func());
	    }; 
	};
      term.SkipSym();
    };
   
  return CloseLiteral();  
}; // bool NewClause::PushStandardLit(ulong index,TERM* t,bool inherentlySelected)

}; // namespace VK

//===========================================================================
#undef DEBUG_NAMESPACE
//=========================================================================
#endif
