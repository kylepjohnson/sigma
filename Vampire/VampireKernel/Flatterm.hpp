//
// File:         Flatterm.hpp
// Description:  Flatterms.
// Created:      Feb 13, 2000, 20:50
// Author:       Alexandre Riazanov
// mail:         riazanov@cs.man.ac.uk
//====================================================
#ifndef FLATTERM_H
//=================================================================
#define FLATTERM_H
#include <iostream>
#include "jargon.hpp"
#include "VampireKernelDebugFlags.hpp"
#include "VampireKernelConst.hpp"
#include "Term.hpp"
#include "MultisetOfVariables.hpp"
#include "variables.hpp"
#include "Stack.hpp"
#include "Comparison.hpp"
namespace VK
{
  class Clause;
}; // namespace VK

//================================================
#undef DEBUG_NAMESPACE
#ifdef DEBUG_FLATTERM
#define DEBUG_NAMESPACE "Flatterm"
#endif
#include "debugMacros.hpp"
//=================================================

namespace VK
{
  class Flatterm    
    { 
    public: 
      class Iterator;
      class ConstIterator;
      class Open;
      class OpenWithMemory;
    public:
      Flatterm() {};
      ~Flatterm() {};
      TERM Symbol() const { return symbol; };
      TERM& symbolRef() { return symbol; };
      const TERM& symbolRef() const { return symbol; };
      bool isVariable() const { return symbol.isVariable(); };
      bool isComplex() const 
	{
	  CALL("isComplex() const");
	  return symbol.isComplex(); 
	};
      ulong Var() const 
	{
	  CALL("Var() const");
	  ASSERT(isVariable());
	  return symbol.var(); 
	};
      ulong functor() const 
	{ 
	  CALL("functor() const");
	  ASSERT(isComplex());
	  return symbol.functor(); 
	};
      ulong arity() const { return symbol.arity(); };
      void SetSymbol(const TERM& sym) { symbol = sym; };
      void SetVar(ulong var) { symbol.MakeVar(var); _after = this + 1; };
      void SetVar(const TERM& var) { symbol = var; _after = this + 1; };
      void SetConst(const TERM& csym) { symbol = csym; _after = this + 1; }; 
      const Flatterm* after() const { return _after; };
      Flatterm* after() { return _after; };
      const Flatterm* next() const { return (this + 1); }; 
      Flatterm* next() { return (this + 1); };
      const Flatterm* previous() const { return (this - 1); }; 
      Flatterm* previous() { return (this - 1); };
      const Flatterm* lastSymbol() const { return after() - 1; };
      Flatterm* lastSymbol() { return after() - 1; };
      void SetNextTerm(Flatterm* nt) { _after = nt; };
      //long Weight() const { return (_after - this); };
      long size() const { return (_after - this); };
      ulong Depth() const;
      bool Nonground() const;
      bool containsSymbol(const TERM& sym) const;
      bool equal(const Flatterm* t) const;
      bool equal(const Flatterm* t,const Clause* const * commutativity_table) const;
      bool EqualArguments(const Flatterm* t) const;
      bool EqualArguments(const Flatterm* t,const Clause* const * commutativity_table) const;
      bool Complementary(const Flatterm* t) const;
      bool Complementary(const Flatterm* t,const Clause* const * commutativity_table) const;

      BK::Comparison CompareLex(const Flatterm* t) const;

      void renameVariables(INT_VAR_RENAMING& var_renaming);
      bool IsBackJump() const { return _after < this; };
      void SetBackJump(Flatterm* jmp) { _after = jmp; };
      void BinSwapArgs();  

#ifndef NO_DEBUG
      bool CheckIntegrity() const { return (after() > this); };
#endif
      ostream& output(ostream& str) const { return str << symbol; };
      ostream& outputTerm(ostream& str) const;

    private:
      void ShiftNextTerm(const long& shift) { _after += shift; };
      static void CopyWithShift(const Flatterm* org,Flatterm* dest,const long& shift);
      
    private:
      Flatterm* _after;
      TERM symbol;
    }; // class Flatterm
}; // namespace VK

namespace std
{
inline ostream& operator<<(ostream& str,const VK::Flatterm* t) 
{    
  return t->outputTerm(str);
};
};

//================================================
#undef DEBUG_NAMESPACE
#ifdef DEBUG_FLATTERM
#define DEBUG_NAMESPACE "Flatterm::Open"
#endif
#include "debugMacros.hpp"
//=================================================
namespace VK
{
  class Flatterm::Open
    {
    private:
      Flatterm* curr_pos;
      BK::Stack<Flatterm*,VampireKernelConst::MaxTermSize> open_parentheses;
      BracketCounter<VampireKernelConst::MaxTermDepth> bracket_counter;
      DepthCounter depth_counter;
      DOP(Flatterm* debug_fst_pos);
    private: // auxilliary methods
      void CloseNParentheses(unsigned n)
	{
	  depth_counter.Up(n);
	  while (n)
	    {
	      open_parentheses.pop()->SetNextTerm(curr_pos);
	      n--;
	    };
	}; 
    public:
      Open() 
	: open_parentheses("open_parentheses"),
	bracket_counter(),
	depth_counter()
	{};
      Open(Flatterm* mem) 
	: open_parentheses("open_parentheses"),
	bracket_counter()
	{ 
	  Reset(mem);
	};
      ~Open() {};
      void Reset(Flatterm* mem) 
	{
	  CALL("Reset(Flatterm* mem)");
	  DOP(debug_fst_pos = mem);
	  curr_pos = mem; 
	  open_parentheses.reset();
	  bracket_counter.Reset();
	  depth_counter.Reset(1UL);
	};

      ulong Depth() const { return depth_counter.Depth(); };
      void PushVar(ulong var)
	{
	  CALL("PushVar(ulong var)");
	  ASSERT(curr_pos - debug_fst_pos < (long)VampireKernelConst::MaxTermSize);
	  curr_pos->SetVar(var);
	  curr_pos++;
	  CloseNParentheses(bracket_counter.Close());
	}; 
    
      void PushVar(const TERM& var)
	{
	  CALL("PushVar(const TERM& var)");
	  ASSERT(curr_pos - debug_fst_pos < (long)VampireKernelConst::MaxTermSize);
	  curr_pos->SetVar(var);
	  curr_pos++;
	  CloseNParentheses(bracket_counter.Close());
	};
      void PushVar(const TERM& var,ulong varOffset)
	{
	  CALL("PushVar(const TERM& var,ulong varOffset)");
	  ASSERT(curr_pos - debug_fst_pos < (long)VampireKernelConst::MaxTermSize);
	  curr_pos->SetVar(var.var() + varOffset);
	  curr_pos++;
	  CloseNParentheses(bracket_counter.Close());
	}; 
      void PushConst(const TERM& csym)
	{
	  CALL("PushConst(const TERM& csym)");     
	  ASSERT(curr_pos - debug_fst_pos < (long)VampireKernelConst::MaxTermSize);
	  curr_pos->SetConst(csym);
	  curr_pos++;
	  CloseNParentheses(bracket_counter.Close());
	}; 
      void PushNonconstFun(const TERM& fsym)
	{
	  CALL("PushNonconstFun(const TERM& fsym)");
	  ASSERT(curr_pos - debug_fst_pos < (long)VampireKernelConst::MaxTermSize);
	  curr_pos->SetSymbol(fsym);
	  // fsym.arity() > 0 here
	  bracket_counter.Open(fsym.arity());
	  open_parentheses.push(curr_pos);
	  depth_counter.Down();
	  curr_pos++;
	}; 
      void Push(const TERM& sym) 
	{
	  if (sym.isVariable()) { PushVar(sym); }
	  else if (sym.arity()) { PushNonconstFun(sym); }
	  else PushConst(sym);
	};
      void Push(const TERM& sym,ulong varOffset)
	{
	  if (sym.isVariable()) { PushVar(sym,varOffset); }
	  else if (sym.arity()) { PushNonconstFun(sym); }
	  else PushConst(sym);
	};
      Flatterm* CurrPos() const { return curr_pos; };
   
      void PushStandardComplexTerm(const TERM* t,ulong varOffset)
	{ 
	  CALL("PushStandardComplexTerm(const TERM* t,ulong varOffset)");
	  ASSERT(t->isComplex());
	  TERM::Iterator iter(t);  
	  do
	    {    
	      Push(iter.symbolRef(),varOffset);
	    }
	  while (iter.Next()); 
	};
    }; // class Flatterm::Open

}; // namespace VK
//================================================
#undef DEBUG_NAMESPACE
#ifdef DEBUG_FLATTERM
#define DEBUG_NAMESPACE "Flatterm::OpenWithMemory"
#endif
#include "debugMacros.hpp"
//=================================================
namespace VK
{
  class Flatterm::OpenWithMemory : public Flatterm::Open
    {
    private:
      Flatterm memory[VampireKernelConst::MaxTermSize+2];
    public:
      OpenWithMemory() : Open(memory) {};
      void Reset() { Open::Reset(memory); };
      const Flatterm* Term() const { return memory; }; 
      Flatterm* Term() { return memory; }; 
      long currentSize() const { return CurrPos() - memory; };
    }; // class Flatterm::OpenWithMemory : public Flatterm::Open

}; // namespace VK
//================================================
#undef DEBUG_NAMESPACE
#ifdef DEBUG_FLATTERM
#define DEBUG_NAMESPACE "Flatterm::Iterator"
#endif
#include "debugMacros.hpp"
//=================================================

namespace VK
{
  class Flatterm::Iterator
    {
    private:
      Flatterm* curr_pos;
      Flatterm* end_of_term;
    public:
      Iterator() {};
      Iterator(Flatterm* t) { Reset(t); };
      void init() {};
      void destroy() {};
      void Reset(Flatterm* t)
	{
	  curr_pos = t;
	  end_of_term = t->after();
	};
      Flatterm* CurrPos() const { return curr_pos; };
      bool EndOfTerm() const { return (curr_pos == end_of_term); };
      bool NotEndOfTerm() const { return (curr_pos < end_of_term); };
      void NextSym() { curr_pos++; };
      void after() { curr_pos = curr_pos->after(); }; 
    }; // class Flatterm::Iterator

}; // namespace VK

//================================================
#undef DEBUG_NAMESPACE
#ifdef DEBUG_FLATTERM
#define DEBUG_NAMESPACE "Flatterm::ConstIterator"
#endif
#include "debugMacros.hpp"
//=================================================

namespace VK
{

  class Flatterm::ConstIterator
    {
    private:
      const Flatterm* curr_pos;
      const Flatterm* end_of_term;
    public:
      ConstIterator() {};
      ConstIterator(const Flatterm* t) { Reset(t); };
      void init() {};
      void destroy() {};
      void Reset(const Flatterm* t)
	{
	  curr_pos = t;
	  end_of_term = t->after();
	};
      const Flatterm* CurrPos() const { return curr_pos; };
      bool EndOfTerm() const { return (curr_pos == end_of_term); };
      bool NotEndOfTerm() const { return (curr_pos < end_of_term); };
      void NextSym() { curr_pos++; };
      void after() { curr_pos = curr_pos->after(); }; 
    }; // class Flatterm::ConstIterator

}; // namespace VK

//================================================
#undef DEBUG_NAMESPACE
#ifdef DEBUG_FLATTERM
#define DEBUG_NAMESPACE "Flatterm"
#endif
#include "debugMacros.hpp"
//=================================================
namespace VK
{

  inline bool Flatterm::equal(const Flatterm* t) const
    {
      CALL("equal(const Flatterm* t) const");
      // can be applied to variables
      if (symbolRef() != t->symbolRef()) return false; 
      register long n = size();
      if (n != t->size()) return false;
      n--;
      while (n)
	{
	  if ((this[n].symbolRef()) != (t[n].symbolRef())) return false;   
	  n--;  
	};
      return true;
    }; // bool Flatterm::equal(const Flatterm* t) const


  inline bool Flatterm::equal(const Flatterm* t,const Clause* const * commutativity_table) const
    {
      CALL("equal(const Flatterm* t,const Clause* const * commutativity_table) const");
      if (size() != t->size()) return false;

      ConstIterator iter1(this);

      ConstIterator iter2(t);
      const Flatterm* arg11;
      const Flatterm* arg21;
      const Flatterm* arg12;
      const Flatterm* arg22;

      do
	{
	  if (iter1.CurrPos()->symbolRef() != iter2.CurrPos()->symbolRef()) return false;

	  if ((iter1.CurrPos()->isComplex()) && (commutativity_table[iter1.CurrPos()->functor()]))
	    {    
	      // recursion     
	      arg11 = iter1.CurrPos() + 1;
	      arg21 = iter2.CurrPos() + 1; 
	      if (arg11->equal(arg21,commutativity_table))
		{      
		  COP("equal 1st args");
		  iter1.NextSym();
		  iter1.after();
		  iter2.NextSym();
		  iter2.after();      
		}
	      else // try to swap arguments
		{
		  arg12 = arg11->after();
		  arg22 = arg21->after();
		  if (arg12->equal(arg21,commutativity_table)
		      && arg22->equal(arg11,commutativity_table))
		    {
		      iter1.after();         
		      iter2.after();
		    }
		  else return false;
		};
	    }
	  else
	    {
	      iter1.NextSym();
	      iter2.NextSym();
	    };
	}
      while (iter1.NotEndOfTerm());
      return true;
    }; // bool Flatterm::equal(const Flatterm* t,const Clause* const * commutativity_table) const


  inline bool Flatterm::EqualArguments(const Flatterm* t) const
    {
      CALL("EqualArguments(const Flatterm* t) const");
      const Flatterm* sym1 = this + 1;
      const Flatterm* sym2 = t + 1;
      while (sym1 != _after)
	{
	  if (sym1->Symbol() != sym2->Symbol()) return false;
	  sym1++;
	  sym2++;
	};
      return true; 
    }; // bool Flatterm::EqualArguments(const Flatterm* t) const

  inline bool Flatterm::EqualArguments(const Flatterm* t,const Clause* const * commutativity_table) const
    {
      CALL("EqualArguments(const Flatterm* t,const Clause* const * commutativity_table) const");
      ConstIterator iter1(this);
      ConstIterator iter2(t);
      iter1.NextSym();
      iter2.NextSym();
      while (iter1.NotEndOfTerm())
	{
	  if (!iter1.CurrPos()->equal(iter2.CurrPos(),commutativity_table)) return false;
	  iter1.after();
	  iter2.after();
	};
      return true;

      const Flatterm* sym1 = this + 1;
      const Flatterm* sym2 = t + 1;
      while (sym1 != _after)
	{
	  if (sym1->Symbol() != sym2->Symbol()) return false;
	  sym1++;
	  sym2++;
	};
      return true; 
    }; // bool Flatterm::EqualArguments(const Flatterm* t,const Clause* const * commutativity_table) const



  inline bool Flatterm::Complementary(const Flatterm* t) const
    {
      return (Symbol().ComplementaryHeader(t->Symbol()))
	&& (EqualArguments(t));
    }; // bool Flatterm::Complementary(const Flatterm* t) const

  inline bool Flatterm::Complementary(const Flatterm* t,const Clause* const * commutativity_table) const
    {
      return (Symbol().ComplementaryHeader(t->Symbol()))
	&& (EqualArguments(t,commutativity_table));
    }; // bool Flatterm::Complementary(const Flatterm* t,const Clause* const * commutativity_table) const



}; // namespace VK
//=============================================================
#endif
