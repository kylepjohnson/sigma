//
// File:         ProblemProfile.hpp
// Description:  Analysing simple problem characteristics.
// Created:      Apr 15, 2000, 19:00
// Author:       Alexandre Riazanov
// mail:         riazanov@cs.man.ac.uk
// Revised:      Dec 26, 2001.
//               Freed from HellConst::MaxNumOfFunctors.
// Revised:      Dec 27, 2001. Slightly reimplemented.
//====================================================
#ifndef PROBLEM_PROFILE_H
#include <iostream>      
#include "jargon.hpp"
#include "VampireKernelDebugFlags.hpp"
#include "GlobAlloc.hpp"
#include "Array.hpp"
#include "ClauseQueue.hpp"
#include "Clause.hpp"
#include "Term.hpp"
#include "SymbolInfo.hpp"
//================================================
#undef DEBUG_NAMESPACE
#ifdef DEBUG_PROBLEM_PROFILE
#define DEBUG_NAMESPACE "ProblemProfile"
#endif
#include "debugMacros.hpp"
//=================================================
namespace VK
{
class ProblemProfile
{
 public:
  enum Category { NNE, HNE, NEQ, HEQ, PEQ, UEQ };
  class PredInfo
    {
    public: 
      enum Status { CandidateForElimination, CanNotBeEliminated }; 
    public:
      PredInfo() : _symbol() 
	{
	  CALL("constructor PredInfo()");	  
	  _status = CandidateForElimination;
	  _registered = 0UL;
	  _positiveNondef = false;
	  _negativeNondef = false; 
	  _symmetric = false;
	  _total = false;
	};
      void init() 
	{ 
	  _status = CandidateForElimination;
	  _registered = 0UL;
	  _positiveNondef = false;
	  _negativeNondef = false; 
	  _symmetric = false;
	  _total = false;
	};
      static const char* spell(const Status& s)
	{
	  switch (s) 
	    { 
	    case CandidateForElimination : return "CanditateForElimination";
	    case CanNotBeEliminated : return "CanNotBeEliminated"; 
	    };
	  return "WrongValue";  
	};
      ostream& output(ostream& str) const 
	{
	  str << _symbol << "/" << _symbol.arity() << " : " << spell(_status);
	  if (_symmetric) str << " sym";
	  if (_total) str << " tot";
	  return str;
	};  
 
    private:
      void regPred(const TERM& sym) 
	{
	  _symbol = sym;
	  _symbol.makePositive();
	  _registered++;
	};  
 
      void positiveNondef()
	{
	  _positiveNondef = true;
	  if (_negativeNondef) _status = CanNotBeEliminated;
	}; 
      void negativeNondef()
	{
	  _negativeNondef = true;
	  if (_positiveNondef) _status = CanNotBeEliminated;
	}; 
      void nondef(const TERM& hd)
	{
	  if (hd.Positive()) { positiveNondef(); } else negativeNondef();
	};

      bool isNondef(const TERM& hd) const
	{
	  return (hd.Positive()) ? _positiveNondef : _negativeNondef;
	};
 
    private:
      TERM _symbol;
      Status _status;
      ulong _registered;  
      bool _positiveNondef;
      bool _negativeNondef;  
      bool _symmetric;
      bool _total;
      friend class ProblemProfile;
    }; // class ProblemProfile::PredInfo

  class FuncInfo
    {
    public:
      FuncInfo() : _symbol() 
	{
	  CALL("constructor FuncInfo()");
	  _registered = 0UL;
	  _commutative = false;
	};
      void init() 
	{ 
	  _registered = 0UL;
	  _commutative = false;
	};
      const TERM& symbol() const { return _symbol; };
      ulong registered() const { return _registered; };
      bool commutative() const { return _commutative; };
      ostream& output(ostream& str) const 
	{
	  str << _symbol << "/" << _symbol.arity();
	  if (_commutative) str << " : COM";
	  return str;
	}; 
    private:
      TERM _symbol;
      ulong _registered;
      bool _commutative;
      friend class ProblemProfile; 
    }; // class FuncInfo


 public:

  ProblemProfile();
  ~ProblemProfile();
  void init();
  void destroy();
  void reset();

  void load(const Clause* cl);
  void load(const ClauseQueue& clauses)
    {
      CALL("load(const ClauseQueue& clauses)");
 
      for (const ClauseQueue::Entry* qe = clauses.begin(); qe; qe = qe->next())
	{
	  this->load(qe->value());
	};
    };                     
      
  void summarise();                        
                     
  Category category() const 
    {
      if (_numOfEqLiterals)
	if (_numOfEqLiterals == _numOfLiterals) // only equality
	  {
	    if (_numOfUnit == _numOfClauses) return UEQ;    
	    return PEQ;
	  }
	else return (_numOfNonhorn) ? NEQ : HEQ;
      // no equality
      return (_numOfNonhorn) ? NNE : HNE;
    };   

  const SymbolInfo* eliminationTable() const { return &_symbolInfoTable; };

  const SymbolInfo* commutativityTable() const 
    {
      if (_symbolInfoTable.numOfCommutative()) 
	return &_symbolInfoTable;
      return 0;
    };

  const SymbolInfo* symmetryTable() const 
    {
      if (_symbolInfoTable.numOfSymmetric()) 
	return &_symbolInfoTable;
      return 0;   
    };

  static const char* spell(Category cat)
    {
      switch (cat)
	{
	case NNE: return "nne"; 
	case HNE: return "hne"; 
	case NEQ: return "neq"; 
	case HEQ: return "heq"; 
	case PEQ: return "peq"; 
	case UEQ: return "ueq";    
	};
      return "unk";
    };

  ulong numOfClauses() const { return _numOfClauses; };
  ulong numOfNonhorn() const { return _numOfNonhorn; };
  ulong numOfUnit() const { return _numOfUnit; };
  ulong numOfLiterals() const { return _numOfLiterals; };
  ulong numOfEqLiterals() const { return _numOfEqLiterals; }; 
  ulong numOfEliminated() const { return _symbolInfoTable.numOfEliminated(); };
  ulong numOfCommutative() const { return _symbolInfoTable.numOfCommutative(); };
 
  ostream& output(ostream& str) const;

 private: 
   
  PredInfo& predInfo(ulong pred) 
    {
      CALL("predInfo(ulong pred)");
      DOP(_predInfoTable.unfreeze());
      PredInfo* res = &(_predInfoTable.sub(pred));
      DOP(_predInfoTable.freeze());
      return *res;
    }; 

  FuncInfo& funcInfo(ulong func) 
    {
      CALL("funcInfo(ulong func)");
      DOP(_funcInfoTable.unfreeze());
      FuncInfo* res = &(_funcInfoTable.sub(func));
      DOP(_funcInfoTable.freeze());
      return *res;
    }; 

  void removeEliminationClashes(const Clause* cl);

 private:
  ConstClauseQueue _clauses;
  ulong _numOfClauses;
  ulong _numOfNonhorn;
  ulong _numOfUnit;
  ulong _numOfLiterals;
  ulong _numOfEqLiterals;
  ulong _numOfPred;
  ulong _totalPred;
  ulong _numOfFuncs;

  BK::Array<BK::GlobAlloc,PredInfo,128UL,ProblemProfile> _predInfoTable;
  BK::Array<BK::GlobAlloc,FuncInfo,128UL,ProblemProfile> _funcInfoTable;
 
  SymbolInfo _symbolInfoTable;
}; // class ProblemProfile

}; // namespace VK



//====================================================
#endif
                      
