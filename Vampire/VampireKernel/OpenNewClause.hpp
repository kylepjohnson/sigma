// Revised:  May 15, 2002, Alexandre Riazanov, riazanov@cs.man.ac.uk
//           Bug fix in OpenNonpropLiteral(const TERM& header,bool inherentlySelected),
//           PushPropLit(const TERM& header,bool inherentlySelected)  
//           and PushNonconstFun(const TERM& f). 
//           Clause size was not controlled properly. 
//==================================================================
#ifndef OPEN_NEW_CLAUSE_H
//===============================================================
#define OPEN_NEW_CLAUSE_H
#include <iostream>
#include "jargon.hpp"
#include "VampireKernelDebugFlags.hpp"
#include "VampireKernelConst.hpp"
#include "Term.hpp"
#include "Flatterm.hpp"
#include "tmp_literal.hpp"
#include "TmpLitList.hpp"
#include "DestructionMode.hpp"
//================================================
#undef DEBUG_NAMESPACE
#ifdef DEBUG_OPEN_NEW_CLAUSE
#define DEBUG_NAMESPACE "OpenNewClause"
#endif 
#include "debugMacros.hpp"
//=================================================

namespace VK
{
class OpenNewClause
{
 public:
  struct Statistics
  { 
    public: 
     Statistics() { reset(); };
    void init() { reset(); };
    void destroy() {};
     ~Statistics() {};
    void reset() 
    {
      tooHeavy = 0;
      tooHeavyLiterals = 0;
      tooManyVariables = 0;
      tooLong = 0;
      tooDeep = 0;
      propTautologies = 0;
      eqTautologies = 0;  
    };
    ulong tooHeavy;
    ulong tooHeavyLiterals;
    ulong tooManyVariables;
    ulong tooLong;
    ulong tooDeep;
    ulong propTautologies; 
    ulong eqTautologies;       
  };
 public:
  OpenNewClause() :
    max_weight(VampireKernelConst::MaxClauseSize),
    max_lit_weight(VampireKernelConst::MaxTermSize),
    max_length(VampireKernelConst::MaxlNumOfLits)
    {
      _newLiteral = 0;
      Reset();
    };
  ~OpenNewClause() 
    { 
      CALL("destructor ~OpenNewClause()"); 
      if (BK::DestructionMode::isThorough())
	{
	  if (_newLiteral) _newLiteral->recycle();
	};
    };
  
  void init()
  {
    CALL("init()");
    statistics.init();
    _allLiterals.init();
    max_weight = VampireKernelConst::MaxClauseSize;
    max_lit_weight = VampireKernelConst::MaxTermSize;
    max_length = VampireKernelConst::MaxlNumOfLits;
    _newLiteral = 0;
    Reset();
  };


  void destroy()
  {
    CALL("destroy()");
    if (BK::DestructionMode::isThorough())
      {
	if (_newLiteral) _newLiteral->recycle();
      };
    _allLiterals.destroy();
    statistics.destroy();
  }; // void destroy()


    void Reset() // should be renamed not to confuse it with reset()
    {
      CALL("Reset()");
      _allLiterals.reset(); 
      weight = 0;
      size = 0;
      was_tautology = false;
      num_of_unordered_eq_lits = 0;
    }; 
  
  void AnotherTooHeavyClause() { statistics.tooHeavy++;  };
  void AnotherTooHeavyTerm() { statistics.tooHeavyLiterals++; };
  void AnotherClauseWithTooManyVariables() { statistics.tooManyVariables++; };
  void AnotherTooLongClause() { statistics.tooLong++; };
  void AnotherTooDeep() { statistics.tooDeep++; };
  void AnotherPropTautology() { was_tautology = true; statistics.propTautologies++; };
  void AnotherEqTautology() { was_tautology = true; statistics.eqTautologies++; };
  void SetMaxWeight(ulong mw) 
    { 
      max_weight = (mw > VampireKernelConst::MaxClauseSize) ? VampireKernelConst::MaxClauseSize : mw; 
    };
  void SetMaxLitWeight(ulong mlw) 
    { 
      max_lit_weight = (mlw > VampireKernelConst::MaxTermSize) ? VampireKernelConst::MaxTermSize : mlw; 
    };
  void SetMaxLength(ulong ml) 
    { 
      max_length = (ml > VampireKernelConst::MaxlNumOfLits) ? VampireKernelConst::MaxlNumOfLits : ml; 
    };  
    
  
  ulong numOfAllLiterals() const { return _allLiterals.numOfAllLiterals(); };

  const bool& WasTautology() const { return was_tautology; };
     
  TmpLitList& AllLiterals() { return _allLiterals; };
 
  // collecting a new clause

  bool OpenNonpropLiteral(const TERM& header,bool inherentlySelected)
    {
      CALL("OpenNonpropLiteral(const TERM& header)");
      // header can't be a propositional literal
      ASSERT(header.arity());
      size += (header.arity() + 1);
      if (size > VampireKernelConst::MaxClauseSize)  
	{
	  AnotherTooHeavyClause();
	  return false;
	};
      _collectingNonanswerLiteral = !header.isAnswerHeader();
      if (_collectingNonanswerLiteral)
	{
	  weight += (header.arity() + 1);
	  ASSERT(weight <= size);
	  if (weight > max_weight)  
	    {
	      AnotherTooHeavyClause();
	      return false;
	    };
	};
      if (!_newLiteral) _newLiteral = TmpLiteral::freshLiteral();
      ASSERT(_newLiteral);
      _newLiteral->reset();

      _newLiteral->pushNonpropHeader(header);
      _newLiteral->setInherentlySelected(inherentlySelected);
      return true;
    };
   
  bool CloseEqLiteral()
    {
      CALL("CloseEqLiteral()");

      ASSERT(_newLiteral->isEquality());
      ASSERT(!_newLiteral->isAnswerLiteral());
      ASSERT(_collectingNonanswerLiteral);
      _newLiteral->endOfLiteral();
      // try to reduce equality or detect a tautology
      if (_newLiteral->argsAreEqual())
	{
	  if (_newLiteral->isPositive())
	    {
	      AnotherEqTautology();
	      return false;
	    }
	  else // trivial equality resolution
	    {
	      size -= _newLiteral->size();
	      weight -= _newLiteral->weightBase();
	      return true;
	    }; 
	};

      for (TmpLiteral* el = _allLiterals.first();
	   el;
	   el = el->next())
	{
	  if (el->isEquality())
	    {
	      if (_newLiteral->equalEqLit(*el)) 
		{
		  // propositional factoring
		  size -= _newLiteral->size();
		  weight -= _newLiteral->weightBase();
		  return true;
		};
          
	      if (_newLiteral->complementaryEqLit(*el))
		{
		  AnotherPropTautology();
		  return false;
		};
	    };  
	};
      
      ASSERT(_newLiteral->size() <= VampireKernelConst::MaxTermSize);
      if (_newLiteral->weightBase() > max_lit_weight)
	{
	  AnotherTooHeavyTerm();
	  return false;
	};
      if (weight > max_weight)
	{
	  AnotherTooHeavyClause();
	  return false;
	};
      if (numOfAllLiterals() >= max_length) 
	{
	  AnotherTooLongClause();
	  return false;
	}; 
      if (!_newLiteral->isOrderedEquality()) num_of_unordered_eq_lits++;
      _allLiterals.push(_newLiteral);
      _newLiteral = 0;
      return true; 
    }; // bool CloseEqLiteral()
   
  bool CloseOrdinaryLiteral()
    { 
      CALL("CloseOrdinaryLiteral()");
      ASSERT(_newLiteral->arity());
      _newLiteral->endOfLiteral();
      // check for identical or complementary literals
      for (TmpLiteral* el = _allLiterals.first();
	   el;
	   el = el->next())
	{
	  if (_newLiteral->equal(*el)) 
	    {
	      // propositional factoring
	      if (_collectingNonanswerLiteral) 
		{
		  ASSERT(!_newLiteral->isAnswerLiteral());
		  size -= _newLiteral->size();
		  weight -= _newLiteral->weightBase();
		};
	      return true;
	    }; 
	  if (_newLiteral->isComplementary(*el))
	    {
	      AnotherPropTautology();
	      return false;
	    };
	};

      ASSERT(_newLiteral->size() <= VampireKernelConst::MaxTermSize);
      if (_collectingNonanswerLiteral)
	{
	  ASSERT(!_newLiteral->isAnswerLiteral());
	  if (_newLiteral->weightBase() > max_lit_weight)
	    {	      
	      AnotherTooHeavyTerm();
	      return false;
	    };
	  if (weight > max_weight)
	    {
	      AnotherTooHeavyClause();
	      return false;
	    };	  
	};

      if (numOfAllLiterals() >= max_length) 
	{
	  AnotherTooLongClause();
	  return false;
	}; 

      _allLiterals.push(_newLiteral);
      _newLiteral = 0;
      return true;
    }; // bool CloseOrdinaryLiteral()
   
  bool CloseLiteral()
    {
      CALL("CloseLiteral()");
      // No _newLiteral->endOfLiteral() is needed here!
      ASSERT(_newLiteral->arity());
      if (_newLiteral->isEquality()) 
	{
	  return CloseEqLiteral();
	};
      return CloseOrdinaryLiteral();
    }; // bool CloseLiteral() 
   
  void EndOfClause()
    {
      CALL("EndOfClause()");
      for (TmpLiteral* el = _allLiterals.first();
	   el;
	   el = el->next())
	{
	  if (el->isEquality()) el->orderEqArgs();
	};
    };


  bool Weed()
    {
      CALL("Weed()");
      // can be used after simplifications
      bool eqTautology;
      if (_allLiterals.weed(eqTautology)) return true;
      if (eqTautology) { AnotherEqTautology(); }
      else AnotherPropTautology(); 
      return false; 
    }; 
     
  bool Weed(const Clause* const * commutativity_table)
    {
      CALL("Weed()");
      // can be used after simplifications
      bool eqTautology;
      if (!commutativity_table) return Weed();
      if (_allLiterals.weed(commutativity_table,eqTautology)) return true;
      if (eqTautology) { AnotherEqTautology(); }
      else AnotherPropTautology(); 
      return false;    
    };

  bool PushPropLit(const TERM& header,bool inherentlySelected)
    {
      CALL("PushPropLit(const TERM& header,bool inherentlySelected)");
      if (header == TERM::builtInTrue())
	{
	  AnotherPropTautology();
	  return false;
	};

      if (header == TERM::builtInFalse())
	{
	  return true;
	};
      

      _collectingNonanswerLiteral = !header.isAnswerHeader();
      TERM compl_header = header;
      compl_header.Inversepolarity();
      for (const TmpLiteral* el = _allLiterals.first();
	   el;
	   el = el->next())
	{
	  if (el->header() == header) return true;
	  if (el->header() == compl_header)
	    {
	      AnotherPropTautology();
	      return false;
	    };
	};
    
      size++;
      if (size > VampireKernelConst::MaxClauseSize)  
	{
	  AnotherTooHeavyClause();
	  return false;
	};
      if (_collectingNonanswerLiteral)
	{
	  weight++;
	  if (weight > max_weight)
	    {
	      AnotherTooHeavyClause();
	      return false;
	    }
	};

      if (numOfAllLiterals() >= max_length) 
	{
	  AnotherTooLongClause();
	  return false;
	};
     
      if (!_newLiteral) _newLiteral = TmpLiteral::freshLiteral();
      ASSERT(_newLiteral);
      _newLiteral->reset();
      _newLiteral->pushPropLit(header);
      _newLiteral->setInherentlySelected(inherentlySelected); 
      _allLiterals.push(_newLiteral);
      _newLiteral = 0;
      return true;
    }; // bool PushPropLit(const TERM& header,bool inherentlySelected)

  bool PushVar(ulong v) 
    {
      CALL("PushVar(ulong v)");
      // this check is too expensive and must be eliminated in the future!
      if (v >= VampireKernelConst::MaxNumOfVariables) 
	{
	  AnotherClauseWithTooManyVariables();
	  return false; 
	};
      _newLiteral->pushVar(v);  
      return true;  
    };
  void PushVar(const TERM& v) { _newLiteral->pushVar(v); };  
  void PushConst(const TERM& c) { _newLiteral->pushConst(c); };
  bool PushNonconstFun(const TERM& f) 
    {
      CALL("PushNonconstFun(const TERM& f)");
      size += f.arity();
      if (size > VampireKernelConst::MaxClauseSize)  
	{
	  AnotherTooHeavyClause();
	  return false;
	};
      if (_collectingNonanswerLiteral)
	{
	  weight += f.arity();
	  if (weight > max_weight) 
	    {
	      AnotherTooHeavyClause();
	      return false;
	    };
	};

      if (!_newLiteral->pushNonconstFun(f))
	{
	  AnotherTooDeep();
	  return false;
	};
      if (_collectingNonanswerLiteral)
	{
      
	  if (_newLiteral->weightBase() > max_lit_weight)
	    {
	      AnotherTooHeavyTerm();
	      return false;
	    }; 
	}
      else
	{
	  if (_newLiteral->size() > VampireKernelConst::MaxTermSize)
	    {
	      AnotherTooHeavyTerm();
	      return false;
	    }; 
	};
      return true;
    };
    
 public:
  ostream& output(ostream& str) const
    {
      return _allLiterals.output(str);  
    };

 public:
  Statistics statistics;
 private:
  TmpLitList _allLiterals;
   
  TmpLiteral* _newLiteral;
  bool _collectingNonanswerLiteral;

  ulong weight;
  ulong size;
  bool was_tautology;
  ulong num_of_unordered_eq_lits;
 private: // limits
  ulong max_weight;
  ulong max_lit_weight;
  ulong max_length;  
}; // class OpenNewClause

}; // namespace VK

namespace std
{
inline ostream& operator<<(ostream& str,const VK::OpenNewClause& cl) 
{
  return cl.output(str);
};

inline ostream& operator<<(ostream& str,const VK::OpenNewClause* cl) 
{
  return cl->output(str);
};
}; // namespace std

//================================================
#undef DEBUG_NAMESPACE 
//================================================
#endif
