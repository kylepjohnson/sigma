//
// File:         tmp_literal.cpp
// Description:  Temporary (buffer) literals.
// Created:      Dec 21, 2001 
// Author:       Alexandre Riazanov
// mail:         riazanov@cs.man.ac.uk
//============================================================================
#include "tmp_literal.hpp"

using namespace BK;
using namespace VK;
//================================================
#undef DEBUG_NAMESPACE
#ifdef DEBUG_NEW_CLAUSE
 #define DEBUG_NAMESPACE "TmpLiteral"
#endif 
#include "debugMacros.hpp"
//=================================================

long TmpLiteral::InitStatic::_count; // implicitely initialised to 0L
TmpLiteral* TmpLiteral::_freeLiterals;
void TmpLiteral::initStatic() { _freeLiterals = 0; };

void TmpLiteral::destroyStatic() 
{      
  CALL("destroyStatic()");
  if (DestructionMode::isThorough())
    {
      while (_freeLiterals)
	{
	  TmpLiteral* tmp = _freeLiterals;
	  _freeLiterals = _freeLiterals->_nextFree;
	  delete tmp;
	};
    }
  else
   _freeLiterals = 0; 
}; // void TmpLiteral::destroyStatic()



ostream& TmpLiteral::output(ostream& str) const
{
  if (_inherentlySelected) str << "[[InhSel]] ";
  if (isEquality())
    { 
      str << '(';
      arg1()->outputTerm(str) << ')' << header() << '(';
      arg2()->outputTerm(str);
      str << ')';
    }
  else 
    flatterm()->outputTerm(str);
  return str; 
}; // ostream& TmpLiteral::output(ostream& str) const


void* TmpLiteral::operator new(size_t)
{
  CALL("operator new(size_t)");
  void* res = static_cast<void*>(GlobAlloc::allocate(sizeof(TmpLiteral)));
  return res;
}; // void* TmpLiteral::operator new(size_t)

void TmpLiteral::operator delete(void* obj)
{
  CALL("operator delete(void*)");
  GlobAlloc::deallocate(obj,sizeof(TmpLiteral));
}; // void TmpLiteral::operator delete(void*)

void TmpLiteral::operator delete(void* obj,size_t)
{
  CALL("operator delete(void*,size_t)");
  GlobAlloc::deallocate(obj,sizeof(TmpLiteral));
}; // void TmpLiteral::operator delete(void*)



bool TmpLiteral::instantiateVar(TmpLiteral& new_lit,ulong var,const Flatterm* subst_term)
{
  CALL("instantiateVar(TmpLiteral& new_lit,ulong var,const Flatterm* subst_term)");
  TERM new_header = header();
  ASSERT(new_header.arity());
  if(new_header.IsEquality()) new_header.MakeUnordered();
  new_lit.reset();
  new_lit.pushNonpropHeader(new_header);
  Flatterm::Iterator iter(flatterm());
  Flatterm::ConstIterator subst_iter;
  iter.NextSym(); // skipping the header
  while (iter.NotEndOfTerm())
    {
      TERM sym = iter.CurrPos()->Symbol();  
      if (sym.isVariable() && sym.var() == var) 
	{
	  subst_iter.Reset(subst_term);             
	  do
	    {
	      if (!new_lit.pushSym((subst_iter.CurrPos())->symbolRef())) return false;             
	      subst_iter.NextSym();
	    }
	  while (subst_iter.NotEndOfTerm());
	}
      else if (!new_lit.pushSym(sym)) return false;
      iter.NextSym();
    };
  new_lit.endOfLiteral();
  return true;
}; // bool TmpLiteral::instantiateVar(TmpLiteral& new_lit,ulong var,const Flatterm* subst_term)


//============================================================================
