#ifndef FORWARD_SUBSUMPTION_H
//===========================================================
#define FORWARD_SUBSUMPTION_H
#ifndef NO_DEBUG_VIS
#  include <iostream>
#endif
#include "jargon.hpp"
#include "VampireKernelDebugFlags.hpp"
#include "VampireKernelConst.hpp"
#include "fs_code.hpp"
#include "Stack.hpp"
#include "ExpandingStack.hpp"
#include "ObjectPool.hpp"
#include "Array.hpp"
#include "GlobAlloc.hpp"
#include "Clause.hpp"
//================================================
#undef DEBUG_NAMESPACE
#ifdef DEBUG_FORWARD_SUBSUMPTION
#  define DEBUG_NAMESPACE "FS_QUERY"
#endif 
#include "debugMacros.hpp"
//=================================================

namespace VK
{
class FS_COMMAND;
class FS_QUERY
{
 public:
  class LIT
    {    
    public:  
      LIT() {};
      ~LIT() {};
      void* operator new(size_t)
	{
	  CALL("LIT::operator new(size_t)");
	  return BK::GlobAlloc::allocate(sizeof(LIT));
	};
      void operator delete(void* obj)
	{
	  CALL("LIT::operator delete(void* obj)");
	  BK::GlobAlloc::deallocate(obj,sizeof(LIT));
	};
      const Flatterm* Args() const { return arg1; };
      void SetArgs(const Flatterm* a) { arg1 = a; };      
      const Flatterm* Arg1() const { return arg1; }; // for equations
      const Flatterm* Arg2() const { return arg2; }; // for equations
      void SetArgs(const Flatterm* a1,const Flatterm* a2)
	{ arg1 = a1; arg2 = a2; };
      LIT* Next() const { return next; };
      void SetNext(LIT* nl) { next = nl; };
      void Set(const Flatterm* a,LIT* nl) { arg1 = a; captured = 0L; next = nl; };
      void Set(const Flatterm* a1,const Flatterm* a2,LIT* nl) 
	{ arg1 = a1; arg2 = a2; captured = 0L; next = nl; };
      void Capture() { captured++; };
      void Release() { captured--; };
      long Captured() { return captured; };
    public:
#ifndef NO_DEBUG_VIS
      ostream& output(ostream& str) const { return (Args() - 1)->outputTerm(str); };
#endif
    private:
      const Flatterm* arg1;
      const Flatterm* arg2;
      long captured;
      LIT* next;
    }; // class LIT

 public:
  FS_QUERY();
  ~FS_QUERY();
  void init();
  void destroy();


  void ResetQuery()
    {
      CALL("ResetQuery()");

      _litPool.reset();
      while (_presentHeaders.nonempty())
	{
	  _litsWithHeader[_presentHeaders.pop()] = 0;
	}; 

      last_pos_unord_eq = 0;
      last_neg_unord_eq = 0;
      num_of_pos_eq = 0;
      if (collect_signature) 
	{
	  if (signature) delete signature;
	  signature = new FunSet();
	};
    };
  void SetCollectSignature(bool fl) 
    { 
      collect_signature = fl; 
    };
  void LinkEquations()
    {
      CALL("LinkEquations()");
      if (last_pos_unord_eq) 
	last_pos_unord_eq->SetNext(FirstLit(_posOrdEqHeaderNum));
      if (last_neg_unord_eq) 
	last_neg_unord_eq->SetNext(FirstLit(_negOrdEqHeaderNum));
    }; 
  void EndOfQuery()
    {
      LinkEquations(); 
    }; 
  void PushLit(const Flatterm* lit)
    {
      CALL("PushLit(const Flatterm* lit)");
      ASSERT(!lit->Symbol().IsEquality());

      DOP(_litsWithHeader.unfreeze());
      LIT** lit_lst_ptr = &(_litsWithHeader.sub(lit->Symbol().HeaderNum()));
      DOP(_litsWithHeader.freeze());


      LIT* currLit = _litPool.reserveObject();
      if (*lit_lst_ptr)
	{
	  currLit->Set(lit + 1,*lit_lst_ptr);
	}
      else // no literals with this header yet
	{
	  _presentHeaders.pushSafe(lit->Symbol().HeaderNum());

	  currLit->Set(lit + 1,0);
	};
      *lit_lst_ptr = currLit;
      if (collect_signature) CollectFunctors(lit);
    };   
    
  void PushUnordEq(TERM header,const Flatterm* arg1,const Flatterm* arg2)
    {
      CALL("PushUnordEq(TERM header,const Flatterm* arg1,const Flatterm* arg2)");
      ASSERT(header.IsUnorderedEq());
      if (header.Positive()) num_of_pos_eq++;


      DOP(_litsWithHeader.unfreeze());
      LIT** lit_lst_ptr = &(_litsWithHeader.sub(header.HeaderNum()));
      DOP(_litsWithHeader.freeze());


      LIT* currLit = _litPool.reserveObject();
      if (*lit_lst_ptr)
	{
	  currLit->Set(arg1,arg2,*lit_lst_ptr);
	}
      else // no literals with this header yet
	{
	  _presentHeaders.pushSafe(header.HeaderNum()); 


	  currLit->Set(arg1,arg2,0);
 
	  if (header.Negative()) 
	    { 
	      last_neg_unord_eq = currLit;
	    }
	  else // positive
	    {
	      last_pos_unord_eq = currLit;
	    };
	};

      *lit_lst_ptr = currLit;
      if (collect_signature) 
	{
	  AddFunctor(header.HeaderNum());
	  CollectFunctors(arg1); 
	  CollectFunctors(arg2);
	};
    };
    
  void PushOrdEq(TERM header,const Flatterm* arg1,const Flatterm* arg2)
    {
      CALL("PushOrdEq(TERM header,const Flatterm* arg1,const Flatterm* arg2)");
      ASSERT(header.IsEquality() && header.IsOrderedEq());  
      if (header.Positive()) num_of_pos_eq++;

      DOP(_litsWithHeader.unfreeze());
      LIT** lit_lst_ptr = &(_litsWithHeader.sub(header.HeaderNum()));
      DOP(_litsWithHeader.freeze());



      LIT* currLit = _litPool.reserveObject();
      if (*lit_lst_ptr)
	{
	  currLit->Set(arg1,arg2,*lit_lst_ptr);
	}
      else // no literals with this header yet
	{

	  _presentHeaders.pushSafe(header.HeaderNum());

	  currLit->Set(arg1,arg2,0);
	};
      *lit_lst_ptr = currLit;
      if (collect_signature) 
	{
	  header.MakeUnordered();
	  AddFunctor(header.HeaderNum());
	  CollectFunctors(arg1); 
	  CollectFunctors(arg2);
	};
    }; 
    
  LIT* FirstLit(ulong header_num)  
    {
      CALL("FirstLit(ulong header_num)");
      if (header_num >= _litsWithHeader.size()) return 0;
      return _litsWithHeader[header_num];
    }; 

  bool SubsumptionAllowedInSetMode(Clause* subsuming_cl)
    {
      // This doesn't guarantee completeness
      return subsuming_cl->NumOfPosEq() <= num_of_pos_eq;
    }; 

  bool SubsumptionAllowedInMultisetMode(Clause* subsuming_cl)
    {
      return true;
    };  
  bool SubsumptionAllowedInOptimisedMultisetMode(Clause* subsuming_cl,long numOfFactorings)
    {
      return (!numOfFactorings) 
	|| (subsuming_cl->allMaximalSelected());
    };  
  const FunSet* Signature() const { return signature; }; 

 private:

  void AddFunctor(unsigned long fun_num)
    {
      if (!(signature->write((size_t)fun_num)))
	{
	  FunSet* new_sig = signature->add((size_t)fun_num);
	  delete signature;
	  signature = new_sig; 
	};
    };
  void CollectFunctors(const Flatterm* t)
    {
      CALL("CollectFunctors(const Flatterm* t)");
      ASSERT(collect_signature);
      Flatterm::ConstIterator iter(t);
      TERM sym;
      while (iter.NotEndOfTerm())
	{
	  sym = iter.CurrPos()->symbolRef();
	  if (sym.isComplex()) AddFunctor(sym.HeaderNum());       
	  iter.NextSym();
	};
    };
    
 public:
#ifndef NO_DEBUG_VIS
  ostream& output(ostream& str) const;
#endif 

 private: // structure  
  BK::ObjectPool<BK::GlobAlloc,LIT> _litPool;

  BK::Array<BK::GlobAlloc,LIT*,128UL,FS_QUERY> _litsWithHeader;
  ulong _posOrdEqHeaderNum;
  ulong _negOrdEqHeaderNum;
  
  BK::ExpandingStack<BK::GlobAlloc,ulong,32UL,FS_QUERY> _presentHeaders;

  LIT* last_pos_unord_eq;
  LIT* last_neg_unord_eq;
  long num_of_pos_eq;
  bool collect_signature;
  FunSet* signature;
}; // class FS_QUERY
 
}; // namespace VK

//================================================
#undef DEBUG_NAMESPACE
#ifdef DEBUG_FORWARD_SUBSUMPTION
#define DEBUG_NAMESPACE "ForwardSubsumption"
#endif 
#include "debugMacros.hpp"
//=================================================
namespace VK
{
class ForwardSubsumption : public FS_CODE, public FS_QUERY 
{
 public:
  ForwardSubsumption();
  ~ForwardSubsumption();
  void init();
  void destroy();
  void reset();
  

  Clause* Subsume() 
    {
      CALL("Subsume()");
  
      ulong maxLitNum = FS_CODE::maxlNumOfLits();
    
      DOP(backtrack_lit.unfreeze()); 
      backtrack_lit.expand(maxLitNum);
      DOP(backtrack_lit.freeze());

      if (setMode) return SubsumeInSetMode();
      if (optimisedMultisetMode) return SubsumeInOptimisedMultisetMode();
      return SubsumeInMultisetMode();
    };
  void SetUseSigFilters(bool fl) 
    { 
      FS_QUERY::SetCollectSignature(fl);
      FS_CODE::SetUseSigFilters(fl);
    };
  void SetSetMode(bool fl) 
    { 
      setMode = fl; 
    };
  void SetOptimisedMultisetMode(bool fl)
    {
      optimisedMultisetMode = fl;
    }; 
#ifndef NO_DEBUG_VIS 
  ostream& outputQuery(ostream& str) { return FS_QUERY::output(str); };
#endif
 
 private:
  Clause* SubsumeInMultisetMode();
  Clause* SubsumeInOptimisedMultisetMode();
  Clause* SubsumeInSetMode();

 private:
  const Flatterm* subst[VampireKernelConst::MaxClauseSize];
  bool setMode;
  bool optimisedMultisetMode;

  BK::ExpandingStack<BK::GlobAlloc,LIT*,32UL,ForwardSubsumption> backtrack_lit;

  BK::Stack<FS_COMMAND*,MAX_FS_CODE_DEPTH> backtrack_instr;
  BK::Stack<const Flatterm*,MAX_FS_CODE_DEPTH> backtrack_cursor;
}; // class ForwardSubsumption  : public FS_CODE 
}; // namespace VK

//================================================
#endif
