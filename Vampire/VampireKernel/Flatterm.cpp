//
// File:         Flatterm.cpp
// Description:  Flatterms.
// Created:      Sep 29, 2002, Alexandre Riazanov, riazanov@cs.man.ac.uk
//====================================================
#include "Flatterm.hpp"
//================================================
#undef DEBUG_NAMESPACE
#ifdef DEBUG_FLATTERM
#define DEBUG_NAMESPACE "Flatterm"
#endif
#include "debugMacros.hpp"
//=================================================

using namespace BK;
using namespace VK;

ulong Flatterm::Depth() const 
{
  CALL("Depth()");
  if (!(Symbol().arity())) return 1UL;
  ConstIterator iter(this);
  SmartDepthCounter<VampireKernelConst::MaxTermDepth> depth_counter;
  depth_counter.Reset(1UL);
  ulong depth = 0UL;
  do
    {
      depth_counter.NextSym(iter.CurrPos()->symbolRef());  
      if (depth_counter.Depth() > depth) 
	{
	  depth = depth_counter.Depth();
	  ASSERT(depth <= VampireKernelConst::MaxTermDepth);
	};
   
      iter.NextSym();
    }  
  while (iter.NotEndOfTerm());
  return depth;
}; // ulong Flatterm::Depth()




bool Flatterm::Nonground() const 
{
  CALL("Nonground() const");
  ConstIterator iter(this);
  do
    {
      if (iter.CurrPos()->symbolRef().isVariable()) return true;
      iter.NextSym();
    }
  while (iter.NotEndOfTerm());
  return false;
}; // bool Flatterm::Nonground() const 

bool Flatterm::containsSymbol(const TERM& sym) const 
{
  CALL("containsSymbol(const TERM& sym) const");
  ConstIterator iter(this);
  do
    {
      if (iter.CurrPos()->symbolRef() == sym) return true;
      iter.NextSym();
    }
  while (iter.NotEndOfTerm());
  return false;

}; // bool Flatterm::containsSymbol(const TERM& sym) const



Comparison Flatterm::CompareLex(const Flatterm* t) const
{
  CALL("CompareLex(const Flatterm* t) const");
  const Flatterm* sym1 = this;
  const Flatterm* sym2 = t;
  
  while (sym1 != _after)
    {

      if (sym1->Symbol() != sym2->Symbol())
	{
	  if ((sym1->Symbol().isVariable()) || (sym2->Symbol().isVariable()))
	    {
	      return Incomparable;
	    }
	  else
	    {
	      return sym1->Symbol().comparePrecedence(sym2->Symbol());
	    }; 
	};
      sym1++;
      sym2++;
    };


  return Equal; 
}; // Comparison Flatterm::CompareLex(const Flatterm* t) const
    

void Flatterm::renameVariables(INT_VAR_RENAMING& var_renaming)
{
  for (Flatterm* sym = this; sym < _after; sym++)
    if (sym->Symbol().isVariable())
      sym->symbolRef().SetVar(var_renaming.Rename(sym->Symbol().var()));
}; // void Flatterm::renameVariables(INT_VAR_RENAMING& var_renaming)


inline void Flatterm::CopyWithShift(const Flatterm* org,Flatterm* dest,const long& shift)
{
  CALL("CopyWithShift(const Flatterm* org,Flatterm* dest,const long& shift)");
  ASSERT((dest < org) || (dest >= org->after()));
  const Flatterm* endOfOrg = org->after();
  do 
    {
      *dest = *org;   
      dest->ShiftNextTerm(shift);  
      org++; 
      dest++;
    }
  while (org < endOfOrg);
}; //void Flatterm::CopyWithShift(const Flatterm* org,Flatterm* dest,const long& shift) 


void Flatterm::BinSwapArgs()
{
  CALL("BinSwapArgs()");
  ASSERT(isComplex());
  ASSERT(arity() == 2);
  Flatterm* arg1 = this + 1;
  Flatterm* arg2 = arg1->after();
  long arg1Weight = arg2 - arg1;
  long arg2Weight = arg2->size(); 
   
  Flatterm arg1Copy[VampireKernelConst::MaxTermSize]; 
  CopyWithShift(arg1,arg1Copy,arg2Weight);
  CopyWithShift(arg2,arg1,(-1)*arg1Weight);
  arg1 += arg2Weight;
  for (long i = 0; i < arg1Weight; i++) arg1[i] = arg1Copy[i];      
}; // void Flatterm::BinSwapArgs()



ostream& Flatterm::outputTerm(ostream& str) const
{
  ConstIterator iter(this);
  while (iter.NotEndOfTerm())
    {
      iter.CurrPos()->output(str) << " ";
      iter.NextSym();
    };   
  return str;
}; // ostream& Flatterm::outputTerm((ostream& str) const

//====================================================
