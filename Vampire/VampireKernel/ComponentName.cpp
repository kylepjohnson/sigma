// Revised:     May 28, 2002, Alexandre Riazanov, riazanov@cs.man.ac.uk 
//              Representation of indexed objects changed: ulong* ---> ulong
//===========================================================
#include "ComponentName.hpp"
#include "Signature.hpp"
#include "GlobalStopFlag.hpp"
//================================================
#undef DEBUG_NAMESPACE
#ifdef DEBUG_COMPONENT_NAME
#define DEBUG_NAMESPACE "ComponentName"
#endif 
#include "debugMacros.hpp"
//=================================================

using namespace BK;
using namespace Gem;
using namespace VK;



ComponentName::ComponentName() : 
  variantIndex(subst), 
  query(), 
  dummyComponent(),
  backtrack_lit(DOP("backtrack_lit")),
  backtrack_instr(DOP("backtrack_instr")),
  backtrack_cursor(DOP("backtrack_cursor"))
{
  CALL("constructor ComponentName()");
  dummyTmpLiteral = TmpLiteral::freshLiteral();
  variantIndex.SetOldStyle(false);
  variantIndex.SetOldStyle(true); // must be so, because the components are integrated in the old style
  variantIndex.SetUseSigFilters(false);
  query.SetCollectSignature(false); 
  dummyComponent.push(dummyTmpLiteral);
  DOP(backtrack_lit.freeze());
};

ComponentName::~ComponentName() 
{ 
  CALL("destructor ~ComponentName()"); 
};

void ComponentName::init() 
{
  CALL("init()");
  variantIndex.init(subst);
  query.init();
  dummyComponent.init();
  backtrack_lit.init(DOP("backtrack_lit"));
  backtrack_instr.init(DOP("backtrack_instr"));
  backtrack_cursor.init(DOP("backtrack_cursor"));
  dummyTmpLiteral = TmpLiteral::freshLiteral();
  variantIndex.SetOldStyle(false);
  variantIndex.SetOldStyle(true); // must be so, because the components are integrated in the old style
  variantIndex.SetUseSigFilters(false);
  query.SetCollectSignature(false); 
  dummyComponent.push(dummyTmpLiteral);
  DOP(backtrack_lit.freeze());
}; // void ComponentName::init() 

void ComponentName::destroy() 
{
  CALL("destroy()");
  backtrack_cursor.destroy();
  backtrack_instr.destroy();
  backtrack_lit.destroy();
  dummyComponent.destroy();
  query.destroy();
  variantIndex.destroy();
  BK_CORRUPT(*this);
}; // void ComponentName::destroy() 


bool ComponentName::findName(TmpLitList& component,ulong& name)
{
  CALL("findName(TmpLitList& component,ulong& name)");

  query.Load(component,component.countDifferentVariables());

  bool res = findVariant(name);

  return res;
}; // bool ComponentName::findName(TmpLitList& component,ulong& name)


bool ComponentName::nameFor(TmpLitList& component,ulong& name,bool& newName)
{
  CALL("nameFor(TmpLitList& component,ulong& name,bool& newName)");
  if (findName(component,component.countDifferentVariables(),name))
    {
      newName = false;
      return true;
    }; 
  // new name  
  if (!Signature::current()->createNewSkolemPredicateNumber(0UL,name))
    return false;
  //if (name >= VampireKernelConst::MaxNumOfFunctors) return false;
  if (name > TERM::maxFunctor()) return false;
  newName = true;
  integrate(component,name);
  return true; 
}; // bool ComponentName::nameFor(TmpLitList& component,ulong& name,bool& newName)


bool ComponentName::nameFor(const Flatterm* term,ulong& name,bool& newName)
{
  CALL("nameFor(const Flatterm* term,ulong& name,bool& newName)");   
  makeDummyLit(term);
  if (findName(dummyComponent,dummyComponent.countDifferentVariables(),name))
    {
      newName = false;
      return true;
    }; 
  // new name  
  if (!Signature::current()->createNewSkolemPredicateNumber(1UL,name))
    return false; 
  if (name > TERM::maxFunctor()) return false;

  newName = true;
  integrate(dummyComponent,name);
  return true;    
}; // bool ComponentName::nameFor(const Flatterm* term,ulong& name,bool& newName)


inline bool ComponentName::findName(TmpLitList& component,ulong numOfVars,ulong& name)
{
  CALL("findName(TmpLitList& component,ulong numOfVars,ulong& name)");
  query.Load(component,numOfVars);
  return  findVariant(name);
};

inline bool ComponentName::findVariant(unsigned long& name)
{ 
  CALL("ComponentName::findVariant(unsigned long& name)");
  FS_COMMAND* curr_instr = variantIndex.Tree();
  if (!(curr_instr)) return false;
 
  query.EndOfQuery();
  const Flatterm* cursor = 0;
  TERM curr_sym;
  TERM curr_sym2;
  CN_QUERY::LIT* curr_lit;

  DOP(backtrack_lit.unfreeze());
  backtrack_lit.expand(variantIndex.maxlNumOfLits());
  DOP(backtrack_lit.freeze());

  backtrack_lit.reset();
  backtrack_instr.reset();
  backtrack_cursor.reset();
  //-----------------------------------------------------------

 check_tag:
  switch (curr_instr->Tag())
    {
    case FS_COMMAND::LIT_HEADER:
      curr_lit = query.FirstLit(curr_instr->HeaderNum());
    check_curr_lit:
      if (curr_lit)
	{
	  if (!curr_lit->Captured())
	    { 
	      curr_lit->Capture();
	      backtrack_lit.push(curr_lit);
	      backtrack_instr.push(curr_instr);
	      cursor = curr_lit->Args();
	      curr_instr = curr_instr->Next();
	      goto check_tag;
	    }
	  else // already captured
	    {
	      curr_lit = curr_lit->Next();
	      goto check_curr_lit;
	    }; 
	}
      else // no appropriate literal with this header
	{	 
	  goto try_fork;
	};

    case FS_COMMAND::ORD_EQ:
      curr_lit = query.FirstLit(curr_instr->HeaderNum());
    check_curr_ord_eq_lit:
      if (curr_lit)
	{
	  if (!curr_lit->Captured())
	    { 
	      curr_lit->Capture();
	      backtrack_lit.push(curr_lit);
	      backtrack_instr.push(curr_instr);
	      cursor = curr_lit->Arg1();
	      curr_instr = curr_instr->Next();
	      goto check_tag;
	    }
	  else // already captured
	    {
	      curr_lit = curr_lit->Next();
	      goto check_curr_ord_eq_lit;
	    };
	}
      else // no appropriate ordered equations with this polarity
	goto try_fork;

    case FS_COMMAND::UNORD_EQ:  
      curr_lit = query.FirstLit(curr_instr->HeaderNum()); 
    check_curr_unord_eq_lit:
      if (curr_lit)
	{
	  if (!curr_lit->Captured())
	    { 
	      curr_lit->Capture();
	      curr_instr->SwapOn();
	      backtrack_lit.push(curr_lit);
	      backtrack_instr.push(curr_instr);
	      cursor = curr_lit->Arg1();
	      curr_instr = curr_instr->Next();
	      goto check_tag;
	    }
	  else // already captured
	    {
	      curr_lit = curr_lit->Next();
	      goto check_curr_unord_eq_lit;
	    };
	}
      else // no appropriate ordered equations with this polarity
	goto try_fork;

    case FS_COMMAND::PROP_LIT:
      if (query.FirstLit(curr_instr->HeaderNum())) // query contains such literal
	{
	  if (curr_instr->Fork()) 
	    {
	      backtrack_instr.push(curr_instr);
	    }; 
	  curr_instr = curr_instr->Next(); 
	  goto check_tag;
	}
      else // no such literal in the query, try fork
	goto try_fork; 

    case FS_COMMAND::SECOND_EQ_ARG:
      if (cursor->IsBackJump()) 
	{
	  cursor = cursor->after();
	};       
      curr_instr = curr_instr->Next(); 
      goto check_tag; 
       
    case FS_COMMAND::FUNC:
      curr_sym = cursor->Symbol();
      if (curr_sym.isVariable()) goto backtrack;      
    compare_func:
      if (curr_sym == curr_instr->Func()) 
        {
	  cursor++;
	  curr_instr = curr_instr->Next();
	  goto check_tag; 
        };
      if (curr_sym < curr_instr->Func()) goto backtrack;
      curr_instr = curr_instr->Fork();
      if (!curr_instr) goto backtrack;       
      if (curr_instr->IsFunc()) goto compare_func;
      goto check_tag;

    case FS_COMMAND::VAR:
      curr_sym = cursor->Symbol();
      if (!curr_sym.isVariable()) goto try_fork;
      *(curr_instr->Var()) = cursor;
      cursor = cursor->after();
      curr_instr = curr_instr->Next();
      goto check_tag;

    case FS_COMMAND::COMPARE: 
      curr_sym = (*(curr_instr->Var1()))->Symbol();            
      curr_sym2 = (*(curr_instr->Var2()))->Symbol();
      if ((curr_sym == curr_sym2) && (curr_sym.isVariable())) 
	{
	  if (curr_instr->Fork()) 
	    {
	      backtrack_instr.push(curr_instr);
	      backtrack_cursor.push(cursor);
	    };
	  curr_instr = curr_instr->Next(); 
	  goto check_tag; 
	}
      else goto try_fork;

    case FS_COMMAND::SUCCESS: 
      if ((curr_instr->NumOfDiffVars() == query.NumOfDiffVars())
	  && (curr_instr->NumOfLits() == query.NumOfLits()))
	{
	  name = reinterpret_cast<ulong>(curr_instr->IndexedObject());
	  return true;
	}
      else goto try_fork; 

#ifdef DEBUG_NAMESPACE  
    default: ICP("ICP1"); return false;
#else
#ifdef _SUPPRESS_WARNINGS_
    default: return false; 
#endif
#endif  
    }; // switch (curr_instr->Tag())

 try_fork:
  curr_instr = curr_instr->Fork();
  if (curr_instr) goto check_tag;
  
  // no fork, try to backtrack
 backtrack: 
  if (backtrack_instr.empty()) 
    {
      return false;
    };
   
  curr_instr = backtrack_instr.pop();   
  switch (curr_instr->Tag())
    {
    case FS_COMMAND::LIT_HEADER: 
      GlobalStopFlag::check();
      curr_lit = backtrack_lit.pop();
      curr_lit->Release();
    backtrack_next_lit:
      curr_lit = curr_lit->Next();
      if (curr_lit)
	{
	  if (curr_lit->Captured()) goto backtrack_next_lit;
	  curr_lit->Capture();
	  backtrack_lit.push(curr_lit);
	  backtrack_instr.push(curr_instr);
	  cursor = curr_lit->Args();
	  curr_instr = curr_instr->Next();
	  goto check_tag;         
        }
      else goto try_fork;    

    case FS_COMMAND::ORD_EQ: 
      GlobalStopFlag::check();
      curr_lit = backtrack_lit.pop();
      curr_lit->Release();
    backtrack_next_ord_eq_lit:
      curr_lit = curr_lit->Next();
      if (curr_lit)
	{
	  if (curr_lit->Captured()) goto backtrack_next_ord_eq_lit;
	  curr_lit->Capture();
	  backtrack_lit.push(curr_lit);
	  backtrack_instr.push(curr_instr);
	  cursor = curr_lit->Arg1();
	  curr_instr = curr_instr->Next();
	  goto check_tag;
        }
      else goto try_fork;  

    case FS_COMMAND::UNORD_EQ: 
      GlobalStopFlag::check();
      curr_lit = backtrack_lit.pop();
      if (curr_instr->SwapFlag()) 
	{
	  curr_instr->SwapOff();
	  backtrack_lit.push(curr_lit);
	  backtrack_instr.push(curr_instr); 
	  cursor = curr_lit->Arg2();
	  curr_instr = curr_instr->Next();
	  goto check_tag;
	}
      else
	{
	  curr_lit->Release();        
        backtrack_next_unord_eq_lit:
	  curr_lit = curr_lit->Next();
	  if (curr_lit)
	    {
	      if (curr_lit->Captured()) goto backtrack_next_unord_eq_lit;
	      curr_lit->Capture();
	      curr_instr->SwapOn();
	      backtrack_lit.push(curr_lit);
	      backtrack_instr.push(curr_instr);
	      cursor = curr_lit->Arg1();
	      curr_instr = curr_instr->Next();
	      goto check_tag;
	    }
	  else goto try_fork;       
	};

    case FS_COMMAND::PROP_LIT:
      curr_instr = curr_instr->Fork();
      goto check_tag; 

    case FS_COMMAND::COMPARE:
      cursor = backtrack_cursor.pop();
      curr_instr = curr_instr->Fork();
      goto check_tag; 

#ifdef DEBUG_NAMESPACE  
    default: ICP("ICP2"); return false;
#else
#ifdef _SUPPRESS_WARNINGS_
    default: return false; 
#endif
#endif
    }; // switch (curr_instr->Tag())
  
}; // bool ComponentName::findVariant(unsigned long& name)


//================================================
#undef DEBUG_NAMESPACE
#ifdef DEBUG_COMPONENT_NAME
#define DEBUG_NAMESPACE "CN_QUERY"
#endif 
#include "debugMacros.hpp"
//=================================================

CN_QUERY::CN_QUERY() : 
#ifdef NO_DEBUG 
  _litsWithHeader(static_cast<LIT*>(0),0),
#else
  _litsWithHeader(static_cast<LIT*>(0),"CN_QUERY::_litsWithHeader"),
#endif

  _posOrdEqHeaderNum(TERM::positiveOrdEqHeader().HeaderNum()),
  _negOrdEqHeaderNum(TERM::negativeOrdEqHeader().HeaderNum()),
  _presentHeaders(DOP("CN_QUERY::_presentHeaders")),
  collect_signature(true) // default
{
  CALL("constructor CN_QUERY()");
  DOP(_litsWithHeader.freeze());
  signature = 0; 
};


void CN_QUERY::init()
{
  CALL("init()");

  _litPool.init();
#ifdef NO_DEBUG 
  _litsWithHeader.init(static_cast<LIT*>(0),(int)0);
#else
  _litsWithHeader.init(static_cast<LIT*>(0),"CN_QUERY::_litsWithHeader");
#endif

  _posOrdEqHeaderNum = TERM::positiveOrdEqHeader().HeaderNum();
  _negOrdEqHeaderNum = TERM::negativeOrdEqHeader().HeaderNum();
  _presentHeaders.init(DOP("CN_QUERY::_presentHeaders"));
  collect_signature = true; // default
  DOP(_litsWithHeader.freeze());
  signature = 0; 
}; // void CN_QUERY::init()


void CN_QUERY::destroy()
{
  CALL("destroy()");
  _presentHeaders.destroy();
  _litsWithHeader.destroy();
  _litPool.destroy();
}; // void CN_QUERY::destroy()


void CN_QUERY::ResetQuery()
{
  CALL("ResetQuery()");
  _litPool.reset();
  while (_presentHeaders.nonempty())
    {
      _litsWithHeader[_presentHeaders.pop()] = 0;
    };

  last_pos_unord_eq = 0;
  last_neg_unord_eq = 0;
  if (collect_signature) 
    {
      if (signature) delete signature;
      signature = new FunSet();
    };
  numOfLits = 0;
}; // void CN_QUERY::ResetQuery()

inline void CN_QUERY::LinkEquations()
{
  CALL("LinkEquations()") 
    if (last_pos_unord_eq) 
      last_pos_unord_eq->SetNext(FirstLit(_posOrdEqHeaderNum));
  if (last_neg_unord_eq) 
    last_neg_unord_eq->SetNext(FirstLit(_negOrdEqHeaderNum));
}; 

void CN_QUERY::EndOfQuery()
{
  LinkEquations();
}; 


void CN_QUERY::PushLit(const Flatterm* lit)
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
      currLit->Set(lit + 1,static_cast<LIT*>(0));
    };
  *lit_lst_ptr = currLit; 
  currLit->Release(); 
  if (collect_signature) CollectFunctors(lit);
}; // void CN_QUERY::PushLit(const Flatterm* lit)
  
  
    
void CN_QUERY::PushUnordEq(TERM header,const Flatterm* arg1,const Flatterm* arg2)
{
  CALL("PushUnordEq(TERM header,const Flatterm* arg1,const Flatterm* arg2)");
  ASSERT(header.IsUnorderedEq());
     

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
      currLit->Set(arg1,arg2,static_cast<LIT*>(0));
      if (header.Negative()) { last_neg_unord_eq = currLit; }
      else // positive
	last_pos_unord_eq = currLit;
    };
  *lit_lst_ptr = currLit;
  currLit->Release();
  if (collect_signature) 
    {
      AddFunctor(header.HeaderNum());
      CollectFunctors(arg1); 
      CollectFunctors(arg2);
    };
}; // void CN_QUERY::PushUnordEq(TERM header,const Flatterm* arg1,const Flatterm* arg2)
    
void CN_QUERY::PushOrdEq(TERM header,const Flatterm* arg1,const Flatterm* arg2)
{
  CALL("PushOrdEq(TERM header,const Flatterm* arg1,const Flatterm* arg2)");
  ASSERT(header.IsEquality() && header.IsOrderedEq());

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
      currLit->Set(arg1,arg2,static_cast<LIT*>(0));
    };
  *lit_lst_ptr = currLit;
  currLit->Release(); 
  if (collect_signature) 
    {
      header.MakeUnordered();
      AddFunctor(header.HeaderNum());
      CollectFunctors(arg1); 
      CollectFunctors(arg2);
    };
}; // void CN_QUERY::PushOrdEq(TERM header,const Flatterm* arg1,const Flatterm* arg2)


void CN_QUERY::Load(TmpLitList& component,ulong numOfVars)
{
  CALL("Load(TmpLitList& component,ulong numOfVars)");
  ResetQuery(); 
  numOfDiffVars = numOfVars;
  numOfLits = component.numOfAllLiterals();
  for (const TmpLiteral* el = component.first();
       el;
       el = el->next())
    {
      if (el->isEquality())
	{	      
	  if (el->isOrderedEquality())   
	    {
	      PushOrdEq(el->header(),el->arg1(),el->arg2());
	    }      
	  else PushUnordEq(el->header(),el->arg1(),el->arg2());
	} 
      else PushLit(el->flatterm());
    };
}; // void CN_QUERY::Load(TmpLitList& component,ulong numOfVars)


//============================================================================

