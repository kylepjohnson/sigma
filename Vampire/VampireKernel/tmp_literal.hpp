// Revised:  Apr 11, 2002, Alexandre Riazanov, riazanov@cs.man.ac.uk
//           1) Temporary code replaced by permanent 
//           in TmpLiteral::KBCompareEqOO(TmpLiteral& lit).
//           2) Minor changes in TmpLiteral::KBCompareEqOU(TmpLiteral& lit)
//              and TmpLiteral::KBCompareEqUU(TmpLiteral& lit)
//              to relax requirements on the reduction ordering 
//              when applied to terms with varibles. Namely,
//              we do not assume anymore that 
//              s > t and t > u contradicts s * u.   
// Revised:  Apr 19, 2002, Alexandre Riazanov, riazanov@cs.man.ac.uk
//           1) better names for members;
//           2) arbitrary simplification ordering.
//  
// Revised:  May 31, 2002, Alexandre Riazanov, riazanov@cs.man.ac.uk
//           arg1() and arg2() respect the ordering now. 
//===========================================================
#ifndef TMP_LITERAL_H
//===========================================================
#define TMP_LITERAL_H
#include <iostream>
#include "jargon.hpp"
#include "Comparison.hpp"
#include "Multiset.hpp"
#include "VampireKernelDebugFlags.hpp"
#include "VampireKernelConst.hpp"
#include "Term.hpp"
#include "WeightPolynomial.hpp"
#include "TermWeighting.hpp"
#include "SimplificationOrdering.hpp"
#include "Flatterm.hpp"
#include "Signature.hpp"
#include "DestructionMode.hpp"
#include "TermWeightType.hpp"
//================================================
#undef DEBUG_NAMESPACE
#ifdef DEBUG_NEW_CLAUSE
#define DEBUG_NAMESPACE "TmpLiteral"
#endif 
#include "debugMacros.hpp"
//=================================================
namespace VK
{
#ifndef NO_DEBUG
class TmpLitList;
#endif
class TmpLiteral
{
 public:
  typedef BK::MultisetWithMinMax<ulong,VampireKernelConst::MaxTermDepth + 1> VarDepthProfile;
  // ^ "+ 1" is essential!
  class InitStatic
    {
    public: 
      InitStatic() 
	{
	  if (!_count) TmpLiteral::initStatic(); 
	  _count++;    
	};
      ~InitStatic() 
	{
	  _count--;
	  if (!_count) TmpLiteral::destroyStatic();
	};
    private:
      static long _count;
    }; // class InitStatic

 public:   
  TmpLiteral() :
    _size(0), 
    _weightCollected(false),
    _variableOccurencesCollected(false),
    _inherentlySelected(false)
    {
      DOP(_debugLiteralComplete = false);
      DOP(_arg1 = 0);
      DOP(_arg2 = 0);
      BK_CORRUPT(_arg1Weight);
      BK_CORRUPT(_arg2Weight);
    };
  ~TmpLiteral() { CALL("destructor ~TmpLiteral()"); };

  void reset()
    {
      CALL("reset()");
      _openFlatterm.Reset();
      _size = 0;
      _weightCollected = false;
      _variableOccurencesCollected = false;
      DOP(_debugLiteralComplete = false);
      DOP(_arg1 = 0);
      DOP(_arg2 = 0);
      BK_CORRUPT(_arg1Weight);
      BK_CORRUPT(_arg2Weight);
      _inherentlySelected = false;
    };

  static void initStatic(); 
  static void destroyStatic();
  static void resetStatic()
    {
      _freeLiterals = 0;
    };

  const TmpLiteral* previous() const { return _previous; };
  TmpLiteral*& previous() { return _previous; };

  const TmpLiteral* next() const { return _next; };
  TmpLiteral*& next() { return _next; };

#ifndef NO_DEBUG
  TmpLitList*& currentList() { return _currentList; };
  TmpLitList* currentList() const { return _currentList; };
#endif

  static TmpLiteral* freshLiteral()
    {
      CALL("freshLiteral()");
      TmpLiteral* res;
      if (_freeLiterals)
	{
	  res = _freeLiterals;
	  _freeLiterals = _freeLiterals->_nextFree; 
	  res->reset();
	}
      else
	{
	  res = new TmpLiteral();       
	};

#ifndef NO_DEBUG
      res->currentList() = 0;
#endif
      return res;
    }; // TmpLiteral* freshLiteral()

  void recycle()
    {
      CALL("recycle()");
      ASSERT(_freeLiterals != this);
      ASSERT(!_currentList);
      _nextFree = _freeLiterals;
      _freeLiterals = this;
    }; // void recycle()



  void setInherentlySelected(bool fl) 
    {
      CALL("setInherentlySelected(bool fl)");
      _inherentlySelected = fl; 
    };

  bool isInherentlySelected() const { return _inherentlySelected; };
  bool isAnswerLiteral() const { return header().isAnswerHeader(); };
  const Flatterm* flatterm() const 
    {
      CALL("flatterm() const");
      ASSERT(_debugLiteralComplete);
      return _openFlatterm.Term(); 
    };
 
  Flatterm* flatterm() 
    {
      CALL("flatterm()");

      ASSERT(_debugLiteralComplete);
      return _openFlatterm.Term(); 
    }; 

  const TERM& header() const { return _header; };
  ulong arity() const { return _header.arity(); };
  unsigned long depth() { CALL("depth()"); return flatterm()->Depth(); };
  ulong size() const { return _size; };
  ulong weightBase() const // conceptually different from weight()! 
    {
      return (isAnswerLiteral()) ? 0UL : size(); 
    };
  

  const WeightPolynomial& weight()
    {
      CALL("weight()");
      if (!_weightCollected) collectWeight();
      return _weight; 
    };
  
  const WeightPolynomial::Monomials& variableOccurences() 
    {
      CALL("variableOccurences()");
      if (!_variableOccurencesCollected) collectVariableOccurences();
      return _variableOccurences;
    }; // const WeightPolynomial::Monomials& variableOccurences() 
 
  TermWeightType numberOfVariableOccurences()
    {
      CALL("numberOfVariableOccurences()");
      return variableOccurences().sumOfCoefficients();
    }; // TermWeightType numberOfVariableOccurences()

  void variables(const ulong*& begin,const ulong*& end)
    {
      CALL("variables(const ulong*& begin,const ulong*& end)");
      if (_weightCollected)
	{
	  begin = _weight.monomials().begin();
	  end = _weight.monomials().end();
	}
      else
	if (_variableOccurencesCollected)
	  {
	    begin = _variableOccurences.begin();
	    end = _variableOccurences.end();
	  }
	else
	  {
	    collectWeight();
	    begin = _weight.monomials().begin();
	    end = _weight.monomials().end();
	  };
    }; // void variables(const ulong*& begin,const ulong*& end)

  ulong numOfDifferentVariables() 
    {
      CALL("numOfDifferentVariables()");
      ASSERT(_debugLiteralComplete);
      if (_weightCollected)
	{
	  return _weight.monomials().length();
	}
      else
	if (_variableOccurencesCollected)
	  {
	    return _variableOccurences.length();
	  }
	else
	  {
	    collectWeight();
	    return _weight.monomials().length();
	  };
    }; // ulong numOfDifferentVariables()


  bool containsVariable(ulong var)
    {
      CALL("containsVariable(ulong var)");
      ASSERT(_debugLiteralComplete);
      if (_weightCollected)
	{
	  return _weight.monomials().coefficient(var) != (TermWeightType)0;
	}
      else
	if (_variableOccurencesCollected)
	  {
	    return _variableOccurences.coefficient(var) != (TermWeightType)0;
	  }
	else
	  {
	    collectWeight();
	    return _weight.monomials().coefficient(var) != (TermWeightType)0;
	  };
    }; // bool containsVariable(ulong var)


  const WeightPolynomial& arg1Weight()
    {
      CALL("arg1Weight()");
      ASSERT(!isAnswerLiteral());
      if (!_arg1Weight) collectArg1Weight();
      return *_arg1Weight; 
    };
  const WeightPolynomial& arg2Weight()
    { 
      CALL("arg2Weight()");
      ASSERT(!isAnswerLiteral());
      if (!_arg2Weight) collectArg2Weight();
      return *_arg2Weight; 
    };

  bool arg1ContainsVariable(ulong var)
    {
      CALL("arg1ContainsVariable(ulong var)");
      return arg1Weight().monomials().coefficient(var) != (TermWeightType)0;
    };
  
  bool arg2ContainsVariable(ulong var)
    {
      CALL("arg2ContainsVariable(ulong var)");
      return arg2Weight().monomials().coefficient(var) != (TermWeightType)0;
    };

  bool arg1IsGround()
    {
      CALL("arg1IsGround()");
      return arg1Weight().monomials().isZero();
    };

  bool arg2IsGround()
    {
      CALL("arg2IsGround()");
      return arg2Weight().monomials().isZero();
    };

  bool isEquality() const { return _header.IsEquality(); };
  bool isOrderedEquality() const { return _header.IsOrderedEq(); };
  bool isUnorderedEquality() const { return _header.IsUnorderedEq(); };
  bool isPositive() const { return _header.Positive(); };
  bool isNegative() const { return _header.Negative(); }; 
  TERM::Polarity polarity() const { return _header.polarity(); };

  const Flatterm* arg1() const 
    {
      CALL("arg1() const");
      ASSERT(_debugLiteralComplete);
      return _arg1;
    };

  Flatterm* arg1() 
    {
      CALL("arg1()");
      ASSERT(_debugLiteralComplete);
      return _arg1;
    };

  const Flatterm* arg2() const 
    { 
      CALL("arg2() const");
      ASSERT(_debugLiteralComplete);
      return _arg2;
    };

  Flatterm* arg2()
    { 
      CALL("arg2()");
      ASSERT(_debugLiteralComplete);
      return _arg2;
    };


  void linkEqArgs() 
    { 
      CALL("linkEqArgs()");
      ASSERT(_debugLiteralComplete);
      ASSERT(flatterm() < (flatterm()->after()));
      flatterm()->after()->SetBackJump(flatterm() + 1); 
    };



   
  void makeEqUnordered()
    {
      CALL("makeEqUnordered()");
      _header.MakeUnordered();
      flatterm()->symbolRef() = _header;
      resetArguments();
      resetArgWeights(); 
    };

  ulong minPosEqLHSSize() const
    {
      CALL("minPosEqLHSSize() const");
      ASSERT(_debugLiteralComplete);
      ASSERT(isEquality() && isPositive());
      if (isOrderedEquality())
	{
	  return arg2()->size();
	}
      else // unordered 
	{
	  long w1 = arg1()->size();
	  long w2 = arg2()->size();
	  return (w1 > w2) ? w2 : w1;
	}; 
    };
   
  void pushVar(const ulong& v)
    {
      CALL("pushVar(const ulong& v)");
      ASSERT(!_debugLiteralComplete);
      _openFlatterm.PushVar(TERM(TermVar,v));
    };

  void pushVar(const TERM& v) 
    {
      CALL("pushVar(const TERM& v)");
      ASSERT(!_debugLiteralComplete);
      _openFlatterm.PushVar(v); 
    };

  void pushConst(const TERM& c) 
    {
      CALL("pushConst(const TERM& c)");
      ASSERT(!_debugLiteralComplete);
      _openFlatterm.PushConst(c); 
    };

  bool pushNonconstFun(const TERM& f) 
    {
      CALL("pushNonconstFun(const TERM& f)");
      ASSERT(!_debugLiteralComplete);
      if (_openFlatterm.Depth() < VampireKernelConst::MaxTermDepth)
	{ 
	  _openFlatterm.PushNonconstFun(f); 
	  _size += f.arity();
	  return true;
	};
      return false;   
    };
   
  bool pushSym(const TERM& sym)
    {
      CALL("pushSym(const TERM& sym)");
      if (sym.isVariable()) { pushVar(sym); return true; };
      if (sym.arity()) return pushNonconstFun(sym);
      pushConst(sym);
      return true;   
    };

  bool pushTerm(const Flatterm* term)
    {
      CALL("pushTerm(const Flatterm* term)");
      ASSERT(!_debugLiteralComplete);
      Flatterm::ConstIterator iter(term); 
      do
	{
	  if (!pushSym(iter.CurrPos()->symbolRef())) return false;
	  iter.NextSym();
	}
      while (iter.NotEndOfTerm());
      return true;
    };   

  void pushNonpropHeader(const TERM& hd)
    {
      CALL("pushNonpropHeader(const TERM& hd)");
      ASSERT(!_debugLiteralComplete);
      _size = hd.arity() + 1;
      _openFlatterm.PushNonconstFun(hd);
      _header = hd;
    };
   
  void pushPropLit(const TERM& lit)
    {
      CALL("pushPropLit(const TERM& lit)");
      ASSERT(!_debugLiteralComplete);
      _size = 1;
      _openFlatterm.PushConst(lit);
      _header = lit;
      ASSERT(!_weightCollected);
      DOP(_debugLiteralComplete = true);
    };
   
  void endOfLiteral()
    {
      CALL("endOfLiteral()");
      DOP(_debugLiteralComplete = true);
      ASSERT(!_weightCollected);
      if (header().IsEquality())
	{
	  resetArguments();
	  resetArgWeights();
	};
    };




  bool argsAreEqual()
    {
      CALL("argsAreEqual()");
      ASSERT(isEquality());
      return arg1()->equal(arg2());
    };
    
  bool argsAreEqual(const Clause* const * commutativity_table)
    {
      CALL("argsAreEqual(const Clause* const * commutativity_table)");
      ASSERT(isEquality());
      return arg1()->equal(arg2(),commutativity_table);
    };

  bool equal(TmpLiteral& lit) const 
    {
      CALL("equal(TmpLiteral& lit) const");
      return flatterm()->equal(lit.flatterm()); 
    };

  bool equal(TmpLiteral& lit,const Clause* const * commutativity_table) const 
    { 
      CALL("equal(TmpLiteral& lit,const Clause* const * commutativity_table) const");
      return flatterm()->equal(lit.flatterm(),commutativity_table); 
    };

  bool isComplementary(TmpLiteral& lit) const 
    {
      CALL("isComplementary(TmpLiteral& lit) const"); 
      return flatterm()->Complementary(lit.flatterm()); 
    };
  bool isComplementary(TmpLiteral& lit,const Clause* const * commutativity_table) const 
    { 
      CALL("isComplementary(TmpLiteral& lit,const Clause* const * commutativity_table) const");
      return flatterm()->Complementary(lit.flatterm(),commutativity_table); 
    };

  bool complementaryEqLit(TmpLiteral& lit) const;
  bool complementaryEqLit(TmpLiteral& lit,const Clause* const * commutativity_table) const;  
  bool equalEqLit(TmpLiteral& lit) const;
  bool equalEqLit(TmpLiteral& lit,const Clause* const * commutativity_table) const;

  bool instantiationPossible(ulong var,unsigned long substDepth,long substSize)
    {
      CALL("instantiationPossible(ulong var,unsigned long substDepth,long substSize)");
      ASSERT(substDepth);
      ulong occs = (ulong)(variableOccurences().coefficient(var));
      if (occs)
	{     
	  ulong newSize = size() + (substSize * occs) - occs;
	  return (newSize <= VampireKernelConst::MaxTermSize) && 
	    (depth() + substDepth <= VampireKernelConst::MaxTermDepth);
	}
      else 
	return true;
    }; // instantiationPossible(ulong var,unsigned long substDepth,long substSize)


  bool instantiateVar(TmpLiteral& new_lit,ulong var,const Flatterm* subst_term);

  void renameVariables(INT_VAR_RENAMING& var_renaming)
    {
      if (isEquality())
	{
	  arg1()->renameVariables(var_renaming);
	  arg2()->renameVariables(var_renaming);
	}
      else flatterm()->renameVariables(var_renaming);
    };

  bool preferredNegative(TmpLiteral& lit);


  BK::Comparison compare(TmpLiteral& lit);
  BK::Comparison compareWithVarWeighting(TmpLiteral& lit,const VAR_WEIGHTING& var_weighting);

  void orderEqArgs();

  
  void collectVarDepthProfile(VarDepthProfile& profile)
    {
      CALL("collectVarDepthProfile(VarDepthProfile& profile)");
      profile.reset();    
      if (_weightCollected)
	{
	  if (_weight.monomials().isZero()) return;
	}
      else
	if (_variableOccurencesCollected)
	  {
	    if (_variableOccurences.isZero()) return; 
	  };
         
      Flatterm::Iterator iter(flatterm());
      SmartDepthCounter<VampireKernelConst::MaxTermDepth> depth_counter;
      depth_counter.Reset(1UL); 
      while (iter.NotEndOfTerm())
	{
	  depth_counter.NextSym(iter.CurrPos()->symbolRef());  
	  if (iter.CurrPos()->isVariable())
	    { 
	      profile.add(depth_counter.Depth());      
	    };
   
	  iter.NextSym();
	};
    }; // void collectVarDepthProfile(VarDepthProfile& profile)
 

  ostream& output(ostream& str) const;

 private:
  void* operator new(size_t);
  void operator delete(void*);
  void operator delete(void*,size_t);

  const Flatterm* rawArg1() const
    {
      CALL("rawArg1() const");
      ASSERT(_debugLiteralComplete);
      return flatterm() + 1;
    };  

  Flatterm* rawArg1()
    {
      CALL("rawArg1()");
      ASSERT(_debugLiteralComplete);
      return flatterm() + 1;
    };
  
  const Flatterm* rawArg2() const
    {
      CALL("rawArg2() const");
      ASSERT(_debugLiteralComplete);
      return rawArg1()->after();
    };   

  Flatterm* rawArg2()
    {
      CALL("rawArg2()");
      ASSERT(_debugLiteralComplete);
      return rawArg1()->after();
    };   

  bool argumentsSwapped() const { return _arg1 == rawArg2(); };

  void swapArguments() 
    { 
      CALL("swapArguments()");
      ASSERT(_arg1 == rawArg1());
      ASSERT(_arg2 == rawArg2());
      _arg1 = rawArg2(); 
      _arg2 = rawArg1(); 

      ASSERT((!_arg1Weight) || (_arg1Weight == &_rawArg1Weight));
      ASSERT((!_arg2Weight) || (_arg2Weight == &_rawArg2Weight));
      WeightPolynomial* tmp = _arg1Weight;
      _arg1Weight = _arg2Weight;
      _arg2Weight = tmp;
    }; 

  void resetArguments() 
    { 
      _arg1 = rawArg1();
      _arg2 = rawArg2();
    };

  void resetArgWeights()
    {
      _arg1Weight = 0;
      _arg2Weight = 0;
    };

  void makeEqOrdered()
    {
      CALL("makeEqOrdered()");
      ASSERT(isEquality());
      _header.MakeOrdered();
      flatterm()->symbolRef() = _header;
    };

  void collectWeight() 
    {
      CALL("collectWeight()");
      ASSERT(!_weightCollected);
      _weight.reset();
      TermWeighting::current()->collectWeight(_weight,flatterm());
      _weightCollected = true;
    };

  void collectVariableOccurences()
    {
      CALL("collectVariableOccurences()");
      ASSERT(!_variableOccurencesCollected);
      _variableOccurences.reset();
      for (Flatterm::Iterator iter(flatterm());
	   iter.NotEndOfTerm();
	   iter.NextSym())
	{
	  if (iter.CurrPos()->symbolRef().isVariable())
	    {
	      _variableOccurences.add1(iter.CurrPos()->symbolRef().var());
	    };
	};
      _variableOccurencesCollected = true;
    };
  

  void collectArg1Weight()
    {
      CALL("collectArg1Weight()");
      ASSERT(!_arg1Weight);
      if (argumentsSwapped()) { _arg1Weight = &_rawArg2Weight; }
      else
	_arg1Weight = &_rawArg1Weight;

      _arg1Weight->reset();
      TermWeighting::current()->collectWeight(*_arg1Weight,arg1());
    };  
  
  void collectArg2Weight()
    {
      CALL("collectArg2Weight()");
      ASSERT(!_arg2Weight);
      if (argumentsSwapped()) { _arg2Weight = &_rawArg1Weight; }
      else
	_arg2Weight = &_rawArg2Weight;
      _arg2Weight->reset();
      TermWeighting::current()->collectWeight(*_arg2Weight,arg2());
    };


  BK::Comparison compareEq(TmpLiteral& lit);
  BK::Comparison compareEqOO(TmpLiteral& lit);
  BK::Comparison compareEqOU(TmpLiteral& lit);
  BK::Comparison compareEqUU(TmpLiteral& lit);
  

  bool preferredNegativeEq(TmpLiteral& lit);
  bool preferredNegativeNoneq(TmpLiteral& lit);

 private:
  TERM _header;
  Flatterm::OpenWithMemory _openFlatterm;
  ulong _size;
   
  bool _weightCollected;
  WeightPolynomial _weight;
  bool _variableOccurencesCollected;
  WeightPolynomial::Monomials _variableOccurences;

  Flatterm* _arg1;
  Flatterm* _arg2;
  WeightPolynomial* _arg1Weight;
  WeightPolynomial* _arg2Weight;

  WeightPolynomial _rawArg1Weight;
  WeightPolynomial _rawArg2Weight;

#ifdef DEBUG_NAMESPACE
  bool _debugLiteralComplete;
#endif

  bool _inherentlySelected;

  TmpLiteral* _previous;
  TmpLiteral* _next;

#ifndef NO_DEBUG
  TmpLitList* _currentList;
#endif
   
  TmpLiteral* _nextFree;
  static TmpLiteral* _freeLiterals;
}; // class TmpLiteral 
 
}; // namespace VK

namespace std
{
inline ostream& operator<<(ostream& str,const VK::TmpLiteral& lit) 
{
  return lit.output(str);
};

inline ostream& operator<<(ostream& str,const VK::TmpLiteral* lit) 
{
  return lit->output(str);
};
};

//******************* Definitions for TmpLiteral: ************************** 


namespace VK
{

inline void TmpLiteral::orderEqArgs()
{
  CALL("orderEqArgs()");
  ASSERT(isEquality());

  if (isOrderedEquality()) return; // already ordered  
  
  ASSERT(arg1() == rawArg1());
  ASSERT(arg2() == rawArg2());
  
  switch (SimplificationOrdering::current()->compare(arg1(),arg1Weight(),
						     arg2(),arg2Weight()))
    {
    case BK::Less:
      swapArguments();
      makeEqOrdered();
      return;
  
    case BK::Equal: return;
        
    case BK::Greater:
      makeEqOrdered();
      return;

    case BK::Incomparable: return;
    };


#ifdef DEBUG_NAMESPACE
  ICP("ICP0");  
  return;          
#else
#ifdef _SUPPRESS_WARNINGS_
  return;
#endif
#endif
           
}; // void TmpLiteral::orderEqArgs()

inline bool TmpLiteral::equalEqLit(TmpLiteral& lit) const 
{
  CALL("equalEqLit(TmpLiteral& lit) const");
  // both this and lit must be equational literals
  if (polarity() != lit.polarity()) return false;

  if (arg1()->equal(lit.arg1()))
    {   
      return arg2()->equal(lit.arg2());
    }
  else
    {
      if ((isUnorderedEquality()) || (lit.isUnorderedEquality()))
	{
	  return (arg1()->equal(lit.arg2())) && (arg2()->equal(lit.arg1()));
	}
      else return false; 
    }; 
}; // bool TmpLiteral::equalEqLit(TmpLiteral& lit) const 
    
inline bool TmpLiteral::equalEqLit(TmpLiteral& lit,const Clause* const * commutativity_table) const 
{
  CALL("equalEqLit(TmpLiteral& lit,const Clause* const * commutativity_table) const");
  // both this and lit must be equational literals
  if (polarity() != lit.polarity()) return false;
  if (arg1()->equal(lit.arg1(),commutativity_table))
    {
      return arg2()->equal(lit.arg2(),commutativity_table);
    }
  else
    {
      if ((isUnorderedEquality()) || (lit.isUnorderedEquality()))
	{
	  return (arg1()->equal(lit.arg2(),commutativity_table)) 
	    && (arg2()->equal(lit.arg1(),commutativity_table));
	}
      else return false; 
    }; 
}; // bool TmpLiteral::equalEqLit(TmpLiteral& lit,const Clause* const * commutativity_table) const 


inline bool TmpLiteral::complementaryEqLit(TmpLiteral& lit) const 
{
  CALL("complementaryEqLit(TmpLiteral& lit) const");
  // both this and lit must be equational literals
  if (polarity() == lit.polarity()) return false;
  if (arg1()->equal(lit.arg1()))
    {
      return arg2()->equal(lit.arg2());
    }
  else
    {
      if ((isOrderedEquality()) && (lit.isOrderedEquality()))
	{
	  return false;
	}
      else return (arg1()->equal(lit.arg2())) && (arg2()->equal(lit.arg1()));
    };
}; // bool TmpLiteral::complementaryEqLit(TmpLiteral& lit) const 
 

inline bool TmpLiteral::complementaryEqLit(TmpLiteral& lit,const Clause* const * commutativity_table) const 
{
  CALL("complementaryEqLit(TmpLiteral& lit,const Clause* const * commutativity_table) const");
  // both this and lit must be equational literals
  if (polarity() == lit.polarity()) return false;
  if (arg1()->equal(lit.arg1(),commutativity_table))
    {
      return arg2()->equal(lit.arg2(),commutativity_table);
    }
  else
    {
      if ((isOrderedEquality()) && (lit.isOrderedEquality()))
	{
	  return false;
	}
      else return (arg1()->equal(lit.arg2(),commutativity_table)) && (arg2()->equal(lit.arg1(),commutativity_table));
    };
}; // bool TmpLiteral::complementaryEqLit(TmpLiteral& lit,const Clause* const * commutativity_table) const 



inline
BK::Comparison TmpLiteral::compare(TmpLiteral& lit)
{
  CALL("compare(TmpLiteral& lit) const");
  
  if (isEquality())
    {
      if ((lit.isEquality())
	  && (polarity() == lit.polarity()))
	return compareEq(lit); 
      return (isPositive()) ? BK::Less : BK::Greater;
    }
  else
    if (lit.isEquality())
      {
	return (lit.isPositive()) ? BK::Greater : BK::Less;
      };

  // both are not equations
 
  return SimplificationOrdering::current()->compareAtoms(flatterm(),
					      weight(),
					      lit.flatterm(),
					      lit.weight());

}; // BK::Comparison TmpLiteral::compare(TmpLiteral& lit)



inline 
BK::Comparison TmpLiteral::compareEq(TmpLiteral& lit)
{
  CALL("compareEq(TmpLiteral& lit)");
  ASSERT(isEquality()); 
  ASSERT(lit.isEquality());
  ASSERT(header().polarity() == lit.header().polarity());

  if (isOrderedEquality())
    {
      if (lit.isOrderedEquality()) return compareEqOO(lit); 
      return compareEqOU(lit);
    }; 
  // this is unordered  

  if (lit.isOrderedEquality()) return BK::inverse(lit.compareEqOU(*this)); 
  // both are unordered
  return compareEqUU(lit);  
}; // BK::Comparison TmpLiteral::compareEq(TmpLiteral& lit)


inline 
BK::Comparison TmpLiteral::compareEqOO(TmpLiteral& lit)
{
  CALL("compareEqOO(TmpLiteral& lit)");
  ASSERT(isEquality());
  ASSERT(isOrderedEquality());
  ASSERT(header() == lit.header());



  BK::Comparison cmp11 = 
    SimplificationOrdering::current()->compare(arg1(),arg1Weight(),
				    lit.arg1(),lit.arg1Weight()); 
  if (cmp11 != BK::Equal) return cmp11;
  return SimplificationOrdering::current()->compare(arg2(),arg2Weight(),
					 lit.arg2(),lit.arg2Weight()); 
}; // BK::Comparison TmpLiteral::compareEqOO(TmpLiteral& lit)




inline 
BK::Comparison TmpLiteral::compareEqOU(TmpLiteral& lit)
{
  CALL("compareEqOU(TmpLiteral& lit)");
  // in the comments below * means "incomparable"
  ASSERT(isEquality());
  ASSERT(lit.isEquality());
  ASSERT(isOrderedEquality());
  ASSERT(lit.isUnorderedEquality());
  ASSERT(isPositive() == lit.isPositive());

  // arg1() > arg2()
  switch (SimplificationOrdering::current()->compare(arg1(),arg1Weight(),
					  lit.arg1(),lit.arg1Weight()))
    {
    case BK::Less:
      // arg2() < arg1() < lit.arg1()
      return BK::Less;
	  
    case BK::Equal: 
      // arg1() = lit.arg1() * lit.arg2(), compare arg2() with lit.arg2()
      return SimplificationOrdering::current()->compare(arg2(),arg2Weight(),
					     lit.arg2(),lit.arg2Weight());	
    case BK::Greater: 
      // arg1() > lit.arg1(), compare arg1() with lit.arg2()
      return SimplificationOrdering::current()->compare(arg1(),arg1Weight(),
					     lit.arg2(),lit.arg2Weight());
    case BK::Incomparable:         
      // compare arg2() with lit.arg1() and lit.arg2()
      switch (SimplificationOrdering::current()->compare(arg2(),arg2Weight(),
					      lit.arg1(),lit.arg1Weight()))
	{
	case BK::Less: 
	  if (SimplificationOrdering::current()->lessOrEqual(arg1(),arg1Weight(),
						  lit.arg2(),lit.arg2Weight()))
	    {
	      return BK::Less;
	    }
	  else
	    return BK::Incomparable;

	case BK::Equal:
	  return SimplificationOrdering::current()->compare(arg1(),arg1Weight(),
						 lit.arg2(),lit.arg2Weight());

	case BK::Greater:
	  // arg1() * lit.arg1(), arg2() > lit.arg1()
	  switch (SimplificationOrdering::current()->compare(arg1(),arg1Weight(),
						  lit.arg2(),lit.arg2Weight()))
	    {
	    case BK::Less: return BK::Less;
	    case BK::Equal: return BK::Greater;
	    case BK::Greater: return BK::Greater;
	    case BK::Incomparable: 
	      if (SimplificationOrdering::current()->greaterOrEqual(arg2(),arg2Weight(),
							 lit.arg2(),lit.arg2Weight()))
		{
		  return BK::Greater;
		}
	      else
		return BK::Incomparable;
	    };

	case BK::Incomparable:
	  // arg1() * lit.arg1(), arg2() * lit.arg1()
	  if (SimplificationOrdering::current()->less(arg1(),arg1Weight(),
					   lit.arg2(),lit.arg2Weight()))
	    {
	      return BK::Less;
	    }
	  else
	    return BK::Incomparable;
	};

    };


#ifdef DEBUG_NAMESPACE 
  ICP("DE100");
  return BK::Incomparable; 
#else
#ifdef _SUPPRESS_WARNINGS_
  return BK::Incomparable; 
#endif  
#endif 
}; // BK::Comparison TmpLiteral::compareEqOU(TmpLiteral& lit)





inline 
BK::Comparison TmpLiteral::compareEqUU(TmpLiteral& lit)
{
  CALL("compareEqUU(TmpLiteral& lit)");
  // compare {arg1(), arg2()} with {lit.arg1(), lit.arg2()}
  // "*" in the comments means "incomparable"
  ASSERT(isEquality());
  ASSERT(lit.isEquality());
  ASSERT(isUnorderedEquality());
  ASSERT(lit.isUnorderedEquality());
  ASSERT(isPositive() == lit.isPositive());

  BK::Comparison cmp = SimplificationOrdering::current()->compare(arg1(),arg1Weight(),lit.arg1(),lit.arg1Weight());
  switch (cmp)
    {
    case BK::Greater:
      // arg1() > lit.arg1()
      cmp = SimplificationOrdering::current()->compare(arg1(),arg1Weight(),lit.arg2(),lit.arg2Weight()); 
      switch (cmp)
	{
	case BK::Greater:
	  // arg1() > lit.arg1(), arg1() > lit.arg2()
	  return BK::Greater;
        
	case BK::Less:
	  // arg1() > lit.arg1(), arg1() < lit.arg2(). Not an error since v2.73
	  return SimplificationOrdering::current()->compare(arg2(),arg2Weight(),lit.arg2(),lit.arg2Weight()); 
       
	case BK::Incomparable:
	  // arg1() > lit.arg1(), arg1() * lit.arg2()
	  cmp = SimplificationOrdering::current()->compare(arg2(),arg2Weight(),lit.arg2(),lit.arg2Weight()); 
	  switch (cmp)
	    {
	    case BK::Greater:
	      // arg1() > lit.arg1(), arg1() * lit.arg2(), arg2() > lit.arg2()
	      return BK::Greater;
            
	    case BK::Less:
	      // arg1() > lit.arg1(), arg1() * lit.arg2(), arg2() < lit.arg2()
	      return BK::Incomparable;
           
	    case BK::Incomparable:
	      // arg1() > lit.arg1(), arg1() * lit.arg2(), arg2() * lit.arg2()
	      return BK::Incomparable;
           
	    case BK::Equal:
	      // arg1() > lit.arg1(), arg1() * lit.arg2(), arg2() = lit.arg2()
	      return BK::Greater;
	    };
        
	case BK::Equal:
	  // arg1() > lit.arg1(), arg1() = lit.arg2() : error because lit.arg1() * lit.arg2()
#ifdef DEBUG_NAMESPACE 
	  ICP("DE20"); 
#endif
	  return BK::Incomparable;
	};

    case BK::Less:
      // arg1() < lit.arg1()
      cmp = SimplificationOrdering::current()->compare(arg2(),arg2Weight(),lit.arg1(),lit.arg1Weight()); 
      switch (cmp)
	{
	case BK::Greater:
	  // arg1() < lit.arg1(), arg2() > lit.arg1(). Not an error since v2.73
	  return SimplificationOrdering::current()->compare(arg2(),arg2Weight(),lit.arg2(),lit.arg2Weight()); 
        
	case BK::Less:
	  // arg1() < lit.arg1(), arg2() < lit.arg1()
	  return BK::Less; // was BK::Greater in the CASC-16 version (obvious incompleteness)
        
	case BK::Incomparable:
	  // arg1() < lit.arg1(), arg2() * lit.arg1()
	  cmp = SimplificationOrdering::current()->compare(arg2(),arg2Weight(),lit.arg2(),lit.arg2Weight()); 
	  switch (cmp)
	    {
	    case BK::Greater:
	      // arg1() < lit.arg1(), arg2() * lit.arg1(), arg2() > lit.arg2()
	      return BK::Incomparable;
           
	    case BK::Less:
	      // arg1() < lit.arg1(), arg2() * lit.arg1(), arg2() < lit.arg2()
	      return BK::Less;
           
	    case BK::Incomparable:
	      // arg1() < lit.arg1(), arg2() * lit.arg1(), arg2() * lit.arg2()
	      return BK::Incomparable;
           
	    case BK::Equal:
	      // arg1() < lit.arg1(), arg2() * lit.arg1(), arg2() = lit.arg2()
	      return BK::Less; 
	    }; 
        
	case BK::Equal:
	  // arg1() < lit.arg1(), arg2() = lit.arg1() : error because arg1() * arg2()
#ifdef DEBUG_NAMESPACE 
	  ICP("DE40");
#endif
	  return BK::Incomparable;
	};
      
    case BK::Incomparable:
      // arg1() * lit.arg1()
      cmp = SimplificationOrdering::current()->compare(arg1(),arg1Weight(),lit.arg2(),lit.arg2Weight()); 
      switch (cmp)
	{
	case BK::Greater:
	  // arg1() * lit.arg1(), arg1() > lit.arg2()
	  cmp = SimplificationOrdering::current()->compare(arg2(),arg2Weight(),lit.arg1(),lit.arg1Weight()); 
	  switch (cmp)
	    {
	    case BK::Greater:
	      // arg1() * lit.arg1(), arg1() > lit.arg2(), arg2() > lit.arg1()
	      return BK::Greater; 
           
	    case BK::Less:
	      // arg1() * lit.arg1(), arg1() > lit.arg2(), arg2() < lit.arg1()
	      return BK::Incomparable;
           
	    case BK::Incomparable:
	      // arg1() * lit.arg1(), arg1() > lit.arg2(), arg2() * lit.arg1()
	      return BK::Incomparable;
           
	    case BK::Equal:
	      // arg1() * lit.arg1(), arg1() > lit.arg2(), arg2() = lit.arg1()
	      return BK::Greater;
	    };
          
	case BK::Less:
	  // arg1() * lit.arg1(), arg1() < lit.arg2()
	  cmp = SimplificationOrdering::current()->compare(arg2(),arg2Weight(),lit.arg1(),lit.arg1Weight()); 
	  switch (cmp)
	    {
	    case BK::Greater:
	      // arg1() * lit.arg1(), arg1() < lit.arg2(), arg2() > lit.arg1()
	      cmp = SimplificationOrdering::current()->compare(arg2(),arg2Weight(),lit.arg2(),lit.arg2Weight());
	      ASSERT(cmp != BK::Equal);
	      return cmp;
          
	    case BK::Less:
	      // arg1() * lit.arg1(), arg1() < lit.arg2(), arg2() < lit.arg1()
	      return BK::Less;
            
	    case BK::Incomparable:
	      // arg1() * lit.arg1(), arg1() < lit.arg2(), arg2() * lit.arg1()
	      cmp = SimplificationOrdering::current()->compare(arg2(),arg2Weight(),lit.arg2(),lit.arg2Weight()); 
	      if (cmp == BK::Less) return BK::Less;
	      return BK::Incomparable;
            
	    case BK::Equal:
	      // arg1() * lit.arg1(), arg1() < lit.arg2(), arg2() = lit.arg1()
	      return BK::Less; 
	    }; 
         

	case BK::Incomparable:
	  // arg1() * lit.arg1(), arg1() * lit.arg2()
	  cmp = SimplificationOrdering::current()->compare(arg2(),arg2Weight(),lit.arg1(),lit.arg1Weight()); 
	  switch (cmp)
	    {
	    case BK::Greater:
	      // arg1() * lit.arg1(), arg1() * lit.arg2(), arg2() > lit.arg1()
	      cmp = SimplificationOrdering::current()->compare(arg2(),arg2Weight(),lit.arg2(),lit.arg2Weight()); 
	      if (cmp == BK::Greater) return BK::Greater;
	      ASSERT(cmp != BK::Equal);
	      return BK::Incomparable;
           
	    case BK::Less:
	      // arg1() * lit.arg1(), arg1() * lit.arg2(), arg2() < lit.arg1()
	      return BK::Incomparable;
           
	    case BK::Incomparable:
	      // arg1() * lit.arg1(), arg1() * lit.arg2(), arg2() * lit.arg1()
	      return BK::Incomparable;
           
	    case BK::Equal:
	      // arg1() * lit.arg1(), arg1() * lit.arg2(), arg2() = lit.arg1()
	      return BK::Incomparable;
	    }; 
        
	case BK::Equal:
	  // arg1() * lit.arg1(), arg1() = lit.arg2()
	  cmp = SimplificationOrdering::current()->compare(arg2(),arg2Weight(),lit.arg1(),lit.arg1Weight()); 
	  return cmp;
	};
      
    case BK::Equal:
      // arg1() = lit.arg1()
      cmp = SimplificationOrdering::current()->compare(arg2(),arg2Weight(),lit.arg2(),lit.arg2Weight()); 
      return cmp;
    };
#ifdef DEBUG_NAMESPACE 
  ICP("DE100");
  return BK::Incomparable; 
#else
#ifdef _SUPPRESS_WARNINGS_
  return BK::Incomparable; 
#endif  
#endif
   
}; // BK::Comparison TmpLiteral::compareEqUU(TmpLiteral& lit)






inline bool TmpLiteral::preferredNegativeEq(TmpLiteral& lit)
{
  CALL("preferredNegativeEq(TmpLiteral& lit)");
  ASSERT(isNegative());
  ASSERT(isEquality());
  ASSERT(lit.isNegative());
  ASSERT(lit.isEquality());
  ulong size1;
  ulong size2;
  if (isOrderedEquality()) { size1 = arg1()->size(); }
  else size1 = size() - 1; 
  if (lit.isOrderedEquality()) { size2 = lit.arg1()->size(); }
  else size2 = lit.size() - 1;   
  if (size1 > size2) return false;
  if (size1 < size2) return true;
  // size1 == size2
  return true; // can be refined  
}; // bool TmpLiteral::preferredNegativeEq(TmpLiteral& lit)

inline bool TmpLiteral::preferredNegativeNoneq(TmpLiteral& lit)
{
  CALL("preferredNegativeNoneq(TmpLiteral& lit)");
  ASSERT(_debugLiteralComplete);
  ASSERT(lit._debugLiteralComplete);
  ASSERT(!isEquality());
  ASSERT(!lit.isEquality());
  ulong size1;
  ulong size2;
  BK::Comparison lexCmp;
  // switch (GlobalHeaderWeighting.Compare(header(),lit.header()))
  switch (Signature::current()->compareHeaderPrecedences(header().polarity(),
						header().functor(),
						lit.header().polarity(),
						lit.header().functor()))
    {
    case BK::Greater: return true;
    case BK::Less: return false;
    case BK::Incomparable: ICP("ICP0"); return false;
    case BK::Equal: 
      size1 = size();
      size2 = lit.size();
      if (size1 > size2) return false;
      if (size1 < size2) return true;
      lexCmp = flatterm()->CompareLex(lit.flatterm());
      if (lexCmp == BK::Less) return true;
      if (lexCmp == BK::Greater) return false;
      return true; // can be refined
    };
#ifdef DEBUG_NAMESPACE
  ICP("ICP1");
  return false;
#else
#ifdef _SUPPRESS_WARNINGS_
  return false;
#endif
#endif
}; // bool TmpLiteral::preferredNegativeNoneq(TmpLiteral& lit) 


inline bool TmpLiteral::preferredNegative(TmpLiteral& lit)
{
  CALL("preferredNegative(TmpLiteral& lit)"); 
  ASSERT(_debugLiteralComplete);
  ASSERT(lit._debugLiteralComplete);
  if (isEquality())
    { 
      ASSERT(isNegative());
      if (lit.isEquality())
	{
	  ASSERT(lit.isNegative());
	  // both are negative equalities
	  return preferredNegativeEq(lit);     
	}
      else return true;
    }
  else
    if (lit.isEquality()) { return false; }
    else // both are not equalities
      {
	return preferredNegativeNoneq(lit);
      };
}; // bool TmpLiteral::preferredNegative(TmpLiteral& lit)
   
 
inline BK::Comparison TmpLiteral::compareWithVarWeighting(TmpLiteral& lit,const VAR_WEIGHTING& var_weighting)
{
  CALL("compareWithVarWeighting(TmpLiteral& lit,const VAR_WEIGHTING& var_weighting)");
  ASSERT(_debugLiteralComplete);
  ASSERT(lit._debugLiteralComplete);

  BK::Comparison hd_cmp = Signature::current()->compareHeaderPrecedences(header().polarity(),
							    header().functor(),
							    lit.header().polarity(),
							    lit.header().functor());

  if (hd_cmp != BK::Equal) return hd_cmp; 
  
  // here the headers are equal
  if (size() > lit.size()) return BK::Greater; 
  if (size() < lit.size()) return BK::Less; 
  

  // compare lexicographically
  BK::Comparison var_cmp;
  const Flatterm* fst = flatterm() + 1;
  const Flatterm* const end_of_fst = flatterm()->after();
  const Flatterm* snd = lit.flatterm();
 check_end:
  if (fst == end_of_fst) return BK::Incomparable;
  snd++;
  if (fst->Symbol() != snd->Symbol())
    {
      if (fst->Symbol().isVariable()) 
	{
	  if (snd->Symbol().isVariable()) 
	    {
	      // different variables
	      var_cmp = var_weighting.Compare(fst->Symbol().var(),snd->Symbol().var());
	      if (var_cmp != BK::Equal) { return var_cmp; };
	    }
	  else 
	    return BK::Less; // a variable is less than a functor
	}
      else 
	{
	  if (snd->Symbol().isVariable()) return BK::Greater;
	  // both are functors
	  return fst->Symbol().comparePrecedence(snd->Symbol()); 
	}; 
    };
  fst++;
  goto check_end;
}; // BK::Comparison TmpLiteral::compareWithVarWeighting(TmpLiteral& lit,const VAR_WEIGHTING& var_weighting)

static TmpLiteral::InitStatic tmpLiteralInitStatic;

}; // namespace VK



//===========================================================
#endif
