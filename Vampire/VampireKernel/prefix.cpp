#include "prefix.hpp"
//================================================
#undef DEBUG_NAMESPACE
#ifdef DEBUG_PREFIX
#define DEBUG_NAMESPACE "Prefix"
#endif
#include "debugMacros.hpp"
//================================================= 

using namespace std;
using namespace VK;

#ifndef NO_DEBUG_VIS
ostream& Prefix::outputWord(ostream& str,const PrefixSym* word)
{
  SimpleConstIter w_iter;
  w_iter.Reset(word);
  while (!(w_iter.Empty()))
    {
      str << '<' << (unsigned long)w_iter.CurrPos() << "> ";
      w_iter.CurrPos()->output(str) << '\n';
      w_iter.SkipSym();
    };     
  return str;
}; // ostream& Prefix::outputWord(ostream& str,const PrefixSym* word)  

  
ostream& Prefix::outputTerm(ostream& str,const PrefixSym* word)
{
  if (word->Head().isVariable()) return word->Head().output(str);
  SimpleConstIter w_iter;
 
  ExpandingBracketCounter bracketCounter; // temporary solution, can not make it static now
  bracketCounter.reset();    

  w_iter.Reset(word);
  while (!(w_iter.Empty()))
    {
      w_iter.CurrPos()->Head().output(str);
      if ((w_iter.CurrPos()->Head().IsFunctor()) 
	  && (w_iter.CurrPos()->Head().Func().arity()))
	{
	  bracketCounter.openSafe(w_iter.CurrPos()->Head().Func().arity());
	  str << '(';
	  w_iter.SkipSym();
	}
      else
	{
	  ulong closed = bracketCounter.close();
	  while (closed) { str << ')';  closed--; };
	  w_iter.SkipSym();
	  if (!(w_iter.Empty())) str << ',';
	};
    };       
  return str;
}; // ostream& Prefix::outputTerm(ostream& str,const PrefixSym* word)
#endif
 

ostream& Prefix::outputWordWithSubst(ostream& str,const PrefixSym* word)
{
  ConstIter w_iter;
  w_iter.Reset(word);
  while (!(w_iter.Empty()))
    {
      if (w_iter.CurrVar())
	{
	  w_iter.CurrVar()->output(str);
	}
      else w_iter.CurrSubterm()->output(str);
      str << '\n';
      w_iter.SkipSym();
    };
  return str;
}; // ostream& Prefix::outputWordWithSubst(ostream& str,const PrefixSym* word)

//================================================
#undef DEBUG_NAMESPACE
#ifdef DEBUG_PREFIX
#define DEBUG_NAMESPACE "(Prefix)"
#endif
#include "debugMacros.hpp"
//=================================================  



void VK::StandardTermToPrefix(ulong index,const TERM* t,PrefixSym*& memory)
  // t must be a complex term here
{
  CALL("StandardTermToPrefix(ulong index,const TERM* t,PrefixSym*& memory)");

  DOP(PrefixSym* debugMemory = memory);
  ASSERT(t->isComplex());
  TERM::Iterator t_iter;
  BracketCounter<VampireKernelConst::MaxTermDepth> brack_counter;
  PrefixSym* left_bracket[VampireKernelConst::MaxTermSize];
  PrefixSym** next_left_bracket = left_bracket;
  t_iter.Reset(t);
  TERM sym;
  unsigned closed;
  unsigned var_offset = Unifier::variableOffset(index);


 next_sym:
  sym = t_iter.Symbol();
  if (sym.isVariable())
    {
      memory->Head().MkVar(Unifier::current()->variableBase(sym.var())+var_offset);
      memory->SetBracket(memory+1);
      closed = brack_counter.Close();    
      while (closed)
	{
	  next_left_bracket--;
	  (*next_left_bracket)->SetBracket(memory+1);
	  closed--;
	};
    }
  else // functor
    {
      memory->Head().MkFunc(sym);
      if (sym.arity())
	{
	  brack_counter.Open(sym.arity());
	  *next_left_bracket = memory;
	  next_left_bracket++;
	}
      else // constant
	{
	  memory->SetBracket(memory+1);
	  unsigned closed = brack_counter.Close();    
	  while (closed)
	    {
	      next_left_bracket--;
	      (*next_left_bracket)->SetBracket(memory+1);
	      closed--;
	    };  
	};
    };
  memory++;
  if (t_iter.Next())
    {
      goto next_sym;
    };
  
  ASSERT(next_left_bracket == left_bracket); 
  ASSERT(Prefix::CorrectComplexTerm(debugMemory)); 
}; // void StandardTermToPrefix(ulong index,const TERM* t,PrefixSym*& memory)
 



//================================================= 
