//
// File:         TmpLitList.hpp
// Description:  Lists of temporary (buffer) literals.
// Created:      Dec 17, 2001, Alexandre Riazanov, riazanov@cs.man.ac.uk
// Note:         Reimplementation of TMP_LIT_LIST (tmp_lit_list.h)
// Revised:      May 15, 2002, Alexandre Riazanov, riazanov@cs.man.ac.uk
//               Minor bug fix in TmpLitList::minlPosEqLHSSize(//============================================================================
#ifndef TMP_LIT_LIST_H
//=============================================================================

#define TMP_LIT_LIST_H 
#include "jargon.hpp"
#include "VampireKernelDebugFlags.hpp"
#include "VampireKernelConst.hpp"
#include "Term.hpp"
#include "tmp_literal.hpp"
#include "WeightPolynomial.hpp"
#include "DestructionMode.hpp"
//============================================================================
#undef DEBUG_NAMESPACE
#ifdef DEBUG_TMP_LIT_LIST
 #define DEBUG_NAMESPACE "TmpLitList"
#endif
#include "debugMacros.hpp"
//============================================================================

namespace VK
{

class TmpLitList
{
 public:
  class Statistics
  {
   public: 
    ulong negative_nonanswer_lits;
    ulong equations;
    ulong noneq_nonanswer_lits;
    ulong inherently_selected;
    ulong answer_literals;
    ulong negative_splitting_branches;
    ulong positive_splitting_branches;
    TERM max_noneq_header;

#ifdef VKERNEL_FOR_STEP_RESOLUTION
    ulong next_state_literals;
    ulong current_state_literals;
#endif

   public:
    Statistics() { reset(); };
    ~Statistics() {};
    void reset()
     {
      negative_nonanswer_lits = 0;
      equations = 0;
      noneq_nonanswer_lits = 0;
      inherently_selected = 0;
      answer_literals = 0;
      negative_splitting_branches = 0;
      positive_splitting_branches = 0;
     }; 
  }; // class Statistics
  


 public:
    
  TmpLitList()  
  {
   CALL("constructor TmpLitList()");  
   _first = 0;
   _last = 0;
   _length = 0UL;
  };
  
  
  ~TmpLitList() 
  {
   CALL("destructor ~TmpLitList()");
   if (BK::DestructionMode::isThorough()) 
     {
       while (nonempty()) pop()->recycle();       
     };
  };


  void init() 
  {
   CALL("init()");
   _first = 0;
   _last = 0;
   _length = 0UL;
  }; 

  void destroy()
  {
    CALL("destroy()");  
    if (BK::DestructionMode::isThorough()) 
      {
	while (nonempty()) pop()->recycle();       
      };
  };

  void reset()
  {
    CALL("reset()");
    while (nonempty()) pop()->recycle();
  };

  TmpLitList& operator=(const TmpLitList& tll) 
  {
    // needed for expantion of dynamic arrays 
   CALL("operator=(const TmpLitList& tll)"); 
   _first = tll._first;
   _last = tll._last;
   _length = tll._length;
#ifndef NO_DEBUG
   for (TmpLiteral* el = first(); el; el = el->next())
     el->currentList() = this;
#endif
   ASSERT(checkIntegrity());
   return *this;
  };
  
 public:
  bool empty() const { return !_first; };
  bool nonempty() const { return _first != 0; };

  
  ulong numOfAllLiterals() const { return _length; };

  const TmpLiteral* first() const { return _first; };
  TmpLiteral* first() { return _first; };
  const TmpLiteral* last() const { return _last; };
  TmpLiteral* last() { return _last; };

  void push(TmpLiteral* lit);
 
  TmpLiteral* pop(); 

  void collectFrom(TmpLitList& list);

  void remove(TmpLiteral* el);  

  void replaceBy(TmpLiteral* lit1,TmpLiteral* lit2)
    {
      CALL("replaceBy(TmpLiteral* lit1,TmpLiteral* lit2)");
      ASSERT(lit1->currentList() == this);
      ASSERT(contains(lit1));
      ASSERT(!lit2->currentList());
      if (lit1 == last())
	{
	  pop();
	  push(lit2);
	  return;
	};
      if (lit1 == first())
	{
	  lit2->previous() = 0;
	  lit2->next() = lit1->next();
	  ASSERT(lit2->next());
	  lit2->next()->previous() = lit2;
	  _first = lit2;
#ifndef NO_DEBUG
	  lit2->currentList() = this;
	  lit1->currentList() = 0;
#endif
	  ASSERT(checkIntegrity());	  
	  return;
	};
      
      // somewhere in the middle

      lit2->previous() = lit1->previous();
      lit2->next() = lit1->next();
      if (lit2->previous())
	lit2->previous()->next() = lit2;
      if (lit2->next())
	lit2->next()->previous() = lit2;
#ifndef NO_DEBUG
      lit2->currentList() = this;
      lit1->currentList() = 0;
#endif
      ASSERT(checkIntegrity());
    }; // void replaceBy(TmpLiteral* lit1,TmpLiteral* lit2)
 
  bool substPossible(ulong var,unsigned long substDepth,long substSize);

  long computeWeightBase() const; 
  void computeSizeAndWeightBase(long& sz,long& weightBase) const;
  void computeSizeAndWeightBaseAndNumOfPosEq(long& sz,long& weightBase,long& numOfPosEq) const;
  long minlPosEqLHSSize() const;

  void collectStatistics(Statistics& stat,bool inversePolarityForSelection) const;
  void collectStatisticsWithSplitting(Statistics& stat,bool inversePolarityForSelection) const;

  void order(TmpLitList& result,const VAR_WEIGHTING& varWeighting);

  void renameVariables(INT_VAR_RENAMING& varRenaming);

  void collectVariableOccurences(WeightPolynomial::Monomials& vars)
    {
      CALL("collectVariableOccurences(WeightPolynomial::Monomials& vars)");
      vars.add(first()->variableOccurences());
      for (TmpLiteral* el = first()->next(); el; el = el->next())
	vars.add(el->variableOccurences());
    }; // void collectVariableOccurences(WeightPolynomial::Monomials& vars)
  
  ulong countDifferentVariables()
    {
      CALL("countDifferentVariables()");
      if (numOfAllLiterals() == 1UL)
	{
	  return first()->numOfDifferentVariables();
	}
      else
	{
	  static WeightPolynomial::Monomials vars;
	  vars.reset();
	  collectVariableOccurences(vars);
	  return vars.length();
	};
    }; // ulong countDifferentVariables()


  bool weed(bool& eqTautology);
  bool weed(const Clause* const * commutativityTable,bool& eqTautology);


  bool containsOnlyAnswerLiterals() const 
    {
      CALL("containsOnlyAnswerLiterals() const");
      for (const TmpLiteral* el = _first; el; el = el->next())
	if (!el->isAnswerLiteral()) return false;
      return true; 
    };

  bool containsAnswerLiterals() const
    {
      CALL("containsAnswerLiterals() const");
      for (const TmpLiteral* el = _first; el; el = el->next())
	if (el->isAnswerLiteral()) return true;
      return false; 
    };

  bool containsSplittingBranches() const
    {
      CALL("containsSplittingBranches() const");
      for (const TmpLiteral* el = _first; el; el = el->next())
	if (el->header().IsSplitting()) return true;
      return false;
    };


  ostream& output(ostream& str) const;

  #ifndef NO_DEBUG
   bool contains(const TmpLiteral* lit) const
   {
    for (const TmpLiteral* el = _first; el; el = el->next())
     if (el == lit) return true;
    return false;
   };
   bool checkIntegrity() const
   {
    if (!_first) return (!numOfAllLiterals()) && (!_last);
    if (!_last) return (!numOfAllLiterals()) && (!_first);
    if (_first == _last) return numOfAllLiterals() == 1UL;
    return (computeLength() == numOfAllLiterals()) && (checkLinks());
   };
  #endif 
 
 private:
  TmpLitList(const TmpLitList& tll) 
  { 
   CALL("constructor TmpLitList(const TmpLitList& tll)");
   ICP("ICP0");
  };
  #ifndef NO_DEBUG
   ulong computeLength() const
   {
    ulong res = 0UL;
    for (const TmpLiteral* el = _first; el; el = el->next()) res++;
    return res;
   };
   bool checkLinks() const
   {
    if (nonempty())
     for (const TmpLiteral* el = _first; el; el = el->next())
       {
	 if (el->currentList() != this) 
	   {
	     return false;
	   }; 
	 if (el->next() && el->next()->previous() != el) 
	   {
	     return false;
	   };
       };
    return true;    
   };
  #endif  
  
  static bool isNegativeForSelection(const TERM& header,bool inversePolarityForSelection)
  {
   if (header.IsEquality() || header.IsSplitting() || (!inversePolarityForSelection)) 
    return header.Negative();
   return header.Positive();
  }; // bool isNegativeForSelection(const TERM& header,bool inversePolarityForSelection) 
  
 private: 
  
  TmpLiteral* _first;
  TmpLiteral* _last;
  ulong _length;

}; // class TmpLitList

}; // namespace VK
//============================================================================
#undef DEBUG_NAMESPACE
#ifdef DEBUG_TMP_LIT_LIST
 #define DEBUG_NAMESPACE "TmpLitList"
#endif
#include "debugMacros.hpp"
//============================================================================

namespace std
{
inline ostream& operator<<(ostream& str,const VK::TmpLitList& lst) 
{
 return lst.output(str);
};

inline ostream& operator<<(ostream& str,const VK::TmpLitList* lst) 
{
 return lst->output(str);
};
};


namespace VK
{

  
inline void TmpLitList::push(TmpLiteral* lit)
{
 CALL("push(TmpLiteral* lit)");
 ASSERT(!lit->currentList());
 lit->previous() = _last;
 lit->next() = 0;
 if (nonempty())
   {
     _last->next() = lit;
   }
 else
   {
     _first = lit;
   };
 _last = lit;
 _length++;

#ifndef NO_DEBUG
 lit->currentList() = this;
#endif
 ASSERT(checkIntegrity());
}; // void TmpLitList::push(TmpLiteral* lit)
 
inline TmpLiteral* TmpLitList::pop() 
{
 CALL("pop()");
 ASSERT(_last);
 ASSERT(_length);
 TmpLiteral* result = _last;
 _last = result->previous();
 if (_last)
   {
     _last->next() = 0;
   }
 else
   {
     _first = 0;
   };

 _length--;

#ifndef NO_DEBUG
 result->currentList() = 0;
#endif
 ASSERT(checkIntegrity());
 return result;
}; // TmpLiteral* TmpLitList::pop() 

inline void TmpLitList::collectFrom(TmpLitList& list)
{
 CALL("collectFrom(TmpLitList& list)"); 
 if (empty())
  {
   if (list.nonempty())
    {
     _first = list._first;
     _last = list._last;
     _length = list._length;
     list._first = 0; 
     list._last = 0; 
     list._length = 0UL;
#ifndef NO_DEBUG
     for (TmpLiteral* el = first(); el; el = el->next())
       el->currentList() = this;
#endif
    };
   COP("COP10");
   ASSERT(checkIntegrity());
   ASSERT(list.empty());
   return;
  };
 // !empty()
 if (list.nonempty())
  {
   // both nonempty
   _length += list._length;
   _last->next() = list._first;
   list._first->previous() = _last;
   _last = list._last;
   list._first = 0; 
   list._last = 0; 
   list._length = 0UL; 
#ifndef NO_DEBUG
   for (TmpLiteral* el = first(); el; el = el->next())
     el->currentList() = this;
#endif
   
  };
 ASSERT(list.empty());
 ASSERT(checkIntegrity());
}; // void TmpLitList::collectFrom(TmpLitList& list)

inline void TmpLitList::remove(TmpLiteral* el)
{
 CALL("remove(const TmpLiteral* el)");
 ASSERT(el);
 ASSERT(contains(el));
 ASSERT(el->currentList() == this);
 _length--;   
 if (el == _first)
  {
   _first = el->next();
   if (_first) 
    { 
     _first->previous() = 0;
    }
   else
    {
     ASSERT(!_length);
     _last = 0;
    };
  }
 else 
  if (el == _last)
   {
    _last = el->previous();
    ASSERT(_last); // since el != _first
    _last->next() = 0;
   }
  else // neither _first, nor _last
   {
    el->previous()->next() = el->next(); 
    el->next()->previous() = el->previous();
   }; 
#ifndef NO_DEBUG
 el->currentList() = 0;
#endif
 ASSERT(checkIntegrity());
}; // void TmpLitList::remove(TmpLiteral* el)


}; // namespace VK

//======================================================================
#endif
