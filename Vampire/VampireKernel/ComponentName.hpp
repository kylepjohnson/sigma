// Revised:     May 28, 2002, Alexandre Riazanov, riazanov@cs.man.ac.uk 
//              Representation of indexed objects changed: ulong* ---> ulong
//===========================================================
#ifndef COMPONENT_NAME_H
//===========================================================
#define COMPONENT_NAME_H
#ifndef NO_DEBUG_VIS
#include <iostream>
#endif
#include "jargon.hpp"
#include "VampireKernelDebugFlags.hpp"
#include "VampireKernelConst.hpp"
#include "fs_code.hpp"
#include "fs_command.hpp"
#include "ExpandingStack.hpp"
#include "ObjectPool.hpp"
#include "TmpLitList.hpp"
#include "GlobAlloc.hpp"
#include "Array.hpp"
//================================================
#undef DEBUG_NAMESPACE
#ifdef DEBUG_COMPONENT_NAME
#define DEBUG_NAMESPACE "CN_QUERY"
#endif 
#include "debugMacros.hpp"
//=================================================
namespace VK
{

class CN_QUERY
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
      void Set(const Flatterm* a,LIT* nl) { arg1 = a; next = nl; };
      void Set(const Flatterm* a1,const Flatterm* a2,LIT* nl) 
	{ arg1 = a1; arg2 = a2; next = nl; };
      void Capture() { captured = true; };
      void Release() { captured = false; };
      bool Captured() const { return captured; };
    public:
      ostream& output(ostream& str) const { return (Args() - 1)->outputTerm(str); };
    private:
      const Flatterm* arg1;
      const Flatterm* arg2;
      bool captured;
      LIT* next;
    }; // class LIT

 public:
  CN_QUERY();

  ~CN_QUERY() { CALL("destructor ~CN_QUERY()"); };

  void init();
  void destroy();

  void ResetQuery();
  void SetCollectSignature(bool fl) 
    { 
      collect_signature = fl; 
    };
  void EndOfQuery();
  void PushLit(const Flatterm* lit);
  void PushUnordEq(TERM header,const Flatterm* arg1,const Flatterm* arg2);
  void PushOrdEq(TERM header,const Flatterm* arg1,const Flatterm* arg2);
    
  LIT* FirstLit(ulong header_num)  
    {
      CALL("FirstLit(ulong header_num)");
      if (header_num >= _litsWithHeader.size()) return 0;
      return _litsWithHeader[header_num];
    }; 
   

  const FunSet* Signature() const { return signature; }; 

  ulong NumOfDiffVars() const { return numOfDiffVars; };
  ulong NumOfLits() const { return numOfLits; }
  void Load(TmpLitList& component,ulong numOfVars);

 private:
  void LinkEquations();
  void AddFunctor(ulong fun_num)
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
  ostream& output(ostream& str) const
    {
      for (ulong p = 0; p < _presentHeaders.size(); p++)
	for (const LIT* lit = _litsWithHeader[_presentHeaders.nth(p)]; lit; lit = lit->Next())
	  lit->output(str) << '\n';
      return str;
    };

 
 private: // structure 
  BK::ObjectPool<BK::GlobAlloc,LIT> _litPool;

  BK::Array<BK::GlobAlloc,LIT*,32UL,CN_QUERY> _litsWithHeader;
  ulong _posOrdEqHeaderNum;
  ulong _negOrdEqHeaderNum;

  BK::ExpandingStack<BK::GlobAlloc,ulong,32UL,CN_QUERY> _presentHeaders;

  LIT* last_pos_unord_eq;
  LIT* last_neg_unord_eq;
  bool collect_signature;
  FunSet* signature;
  ulong numOfDiffVars;
  ulong numOfLits;
}; // class CN_QUERY
 
}; // namespace VK

//================================================
#undef DEBUG_NAMESPACE
#ifdef DEBUG_COMPONENT_NAME
#define DEBUG_NAMESPACE "ComponentName"
#endif 
#include "debugMacros.hpp"
//=================================================
namespace VK
{
class ComponentName 
{
 public:
  ComponentName();
  ~ComponentName();
  void init(); 
  void destroy();
  

  bool findName(TmpLitList& component,ulong& name);

  
  bool findName(const Flatterm* term,ulong& name)
    {
      CALL("findName(const Flatterm* term,ulong& name)");
      makeDummyLit(term);
      return findName(dummyComponent,name); 
    };
  

  void integrate(TmpLitList& component,ulong name)
    {
      CALL("integrate(TmpLitList& component,ulong name)");
      variantIndex.Integrate(component,reinterpret_cast<void*>(name));
    };

  void integrate(const Flatterm* term,ulong name)
    {
      CALL("integrate(const Flatterm* term,ulong name)");
      makeDummyLit(term);
      integrate(dummyComponent,name);
    };

  bool nameFor(TmpLitList& component,ulong& name,bool& newName);

  bool nameFor(const Flatterm* term,ulong& name,bool& newName);

#ifndef NO_DEBUG_VIS 
  ostream& outputQuery(ostream& str) { return query.output(str); };
#endif

 private: 
  bool findName(TmpLitList& component,ulong numOfVars,ulong& name);
   
  bool findVariant(ulong& name);  
 
  void makeDummyLit(const Flatterm* term)
    {
      CALL("makeDummyLit(const Flatterm* term)");
      dummyTmpLiteral->reset();
      dummyTmpLiteral->pushNonpropHeader(TERM::dummyPositiveHeader1());
      dummyTmpLiteral->pushTerm(term); // very inefficient
      dummyTmpLiteral->endOfLiteral();
    };
 private:
  const Flatterm* subst[VampireKernelConst::MaxClauseSize]; 
  FS_CODE variantIndex;
  CN_QUERY query;
  TmpLiteral* dummyTmpLiteral;
  TmpLitList dummyComponent;

  BK::ExpandingStack<BK::GlobAlloc,CN_QUERY::LIT*,32UL,ComponentName> backtrack_lit;

  BK::Stack<FS_COMMAND*,MAX_FS_CODE_DEPTH> backtrack_instr;
  BK::Stack<const Flatterm*,MAX_FS_CODE_DEPTH> backtrack_cursor; 
}; // class ComponentName 
}; // namespace VK
//================================================
#undef DEBUG_NAMESPACE
//===============================================================
#endif
