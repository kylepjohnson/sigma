//
// File:         old_fs_code.cpp
// Description:  Old-style fs code for flatterm query representation.
// Created:      Dec 23, 1999, 15:45
// Author:       Alexandre Riazanov
// mail:         riazanov@cs.man.ac.uk
// Revised:      Feb 13, 2000, 21:30
//================================================
#include "old_fs_code.hpp"
#include "Term.hpp"
#include "Clause.hpp"
#include "Flatterm.hpp"
#include "TmpLitList.hpp"
#include "DestructionMode.hpp"
using namespace BK;
using namespace VK;
//================================================
#undef DEBUG_NAMESPACE
#ifdef DEBUG_OLD_FS_CODE
#define DEBUG_NAMESPACE "OLD_FS_CODE"
#endif
#include "debugMacros.hpp"
//=================================================




OLD_FS_CODE::OLD_FS_CODE(const Flatterm** sbst) 
  : tree(0),
    subst(sbst),
    integrator(subst), 
    remover(subst)  
{
};

OLD_FS_CODE::~OLD_FS_CODE() 
{ 
  CALL("destructor ~OLD_FS_CODE()"); 
  if (DestructionMode::isThorough())
    if (tree) FS_COMMAND::DestroyTree(tree);
};

void OLD_FS_CODE::init(const Flatterm** sbst) 
{
  CALL("init(const Flatterm** sbst)");
  tree = 0;
  subst = sbst;
  integrator.init(subst); 
  remover.init(subst); 
}; // void OLD_FS_CODE::init(const Flatterm** sbst) 


void OLD_FS_CODE::destroy() 
{ 
  CALL("destroy()");
  if (DestructionMode::isThorough())
    if (tree) FS_COMMAND::DestroyTree(tree); 
};

void OLD_FS_CODE::reset()
{
  CALL("reset()");
  if (tree) FS_COMMAND::DestroyTree(tree); 
  tree = 0; 
};

void OLD_FS_CODE::integrate(TmpLitList& component,void* name)
{
  integrator.reset(&tree); 
  for (TmpLiteral* el = component.first(); el; el = el->next())
    integrateLit(el);

  integrator.endOfComponent(name,component.numOfAllLiterals());
}; //void OLD_FS_CODE::integrate(TmpLitList& component,void* name)
 

void OLD_FS_CODE::integrate(Clause* cl)
{
  integrator.reset(&tree); 
   
  for(LiteralList::Iterator iter(cl->LitList());iter.notEnd();iter.next())
    {
      TERM* lit = iter.currentLiteral();
      integrateLit(lit);
    };
  integrator.endOfClause(cl);
}; // void OLD_FS_CODE::Integrate(Clause* cl)


void OLD_FS_CODE::integrateLit(TmpLiteral* lit)
{
  Flatterm::Iterator iter;
  TERM sym;
  integrator.beginLit();
  TERM header = lit->header();
  if (header.IsEquality())
    {
      if (header.IsOrderedEq()) { integrator.ordEq(header); }
      else integrator.unordEq(header);

      iter.Reset(lit->arg1());
      while (iter.NotEndOfTerm())
	{
	  sym = iter.CurrPos()->symbolRef();
	  if (sym.isVariable()) { integrator.var(sym.var()); }
	  else integrator.func(sym); 
	  iter.NextSym();     
	};

      integrator.secondEqArg();

      iter.Reset(lit->arg2());
      while (iter.NotEndOfTerm())
	{
	  sym = iter.CurrPos()->symbolRef();
	  if (sym.isVariable()) { integrator.var(sym.var()); }
	  else integrator.func(sym);  
	  iter.NextSym();     
	};
    } 
  else
    {
      if (header.arity())
	{
	  integrator.litHeader(header);
	  iter.Reset(lit->flatterm()); 
	  iter.NextSym();    
	  while (iter.NotEndOfTerm())
	    {
	      sym = iter.CurrPos()->symbolRef();
	      if (sym.isVariable()) { integrator.var(sym.var()); }
	      else integrator.func(sym);   
	      iter.NextSym();    
	    };
	}
      else // propositional
	integrator.propLit(header); 
    }; 
  integrator.endOfLit();  
}; //OLD_FS_CODE::integrateLit(TmpLiteral* lit)  


void OLD_FS_CODE::integrateLit(const TERM* lit)
{
  TERM::Iterator iter;
  integrator.beginLit();
  if (lit->IsEquality())
    {
      if (lit->IsOrderedEq()) { integrator.ordEq(*lit); }
      else integrator.unordEq(*lit);
      const TERM* arg = lit->Args();
      if (arg->isVariable()) { integrator.var(arg->var()); }
      else 
	{
	  arg = arg->First();
	  iter.Reset(arg);
	  integrator.func(*arg);
	  while (iter.Next())
	    if (iter.symbolRef().isVariable()) { integrator.var(iter.symbolRef().var()); }
	    else integrator.func(iter.symbolRef()); 
	};
      // second argument
      integrator.secondEqArg();
      arg = lit->Args()->Next();
      if (arg->isVariable()) { integrator.var(arg->var()); }
      else 
	{
	  arg = arg->First();
	  iter.Reset(arg);
	  integrator.func(*arg);
	  while (iter.Next())
	    if (iter.symbolRef().isVariable()) { integrator.var(iter.symbolRef().var()); }
	    else integrator.func(iter.symbolRef()); 
	}; 
    }
  else 
    if (lit->arity())
      {
	integrator.litHeader(*lit);
	iter.Reset(lit); 
	while (iter.Next())
	  if (iter.symbolRef().isVariable()) { integrator.var(iter.symbolRef().var()); }
	  else integrator.func(iter.symbolRef());
      }
    else // propositional
      integrator.propLit(*lit);
       
  integrator.endOfLit();
}; // void OLD_FS_CODE::integrateLit(const TERM* lit)

bool OLD_FS_CODE::remove(Clause* cl)
{
  if (!(remover.reset(&tree))) return false; 
  for(LiteralList::Iterator iter(cl->LitList());iter.notEnd();iter.next())
    {
      TERM* lit = iter.currentLiteral();
      if (!(removeLit(lit))) return false;     
    };
  return remover.endOfClause(cl);
}; // bool OLD_FS_CODE::remove(Clause* cl)

bool OLD_FS_CODE::removeLit(const TERM* lit)
{
  TERM::Iterator iter;
  if (!(remover.beginLit())) return false;
  if (lit->IsEquality())
    {
      if (lit->IsOrderedEq()) 
	{ 
	  if (!(remover.ordEq(*lit))) return false; 
	}
      else if (!(remover.unordEq(*lit))) return false; 
      const TERM* arg = lit->Args();
      if (arg->isVariable()) 
	{ 
	  if (!(remover.var(arg->var()))) return false; 
	}
      else 
	{
	  arg = arg->First();
	  iter.Reset(arg);
	  if (!(remover.func(*arg))) return false;
	  while (iter.Next())
	    if (iter.symbolRef().isVariable()) 
	      { 
		if (!(remover.var(iter.symbolRef().var()))) return false; 
	      }
	    else if (!(remover.func(iter.symbolRef()))) return false; 
	};
      // second argument
      if (!(remover.secondEqArg())) return false;
      arg = lit->Args()->Next();
      if (arg->isVariable()) 
	{ 
	  if (!(remover.var(arg->var()))) return false; 
	}
      else 
	{
	  arg = arg->First();
	  iter.Reset(arg);
	  if (!(remover.func(*arg))) return false;
	  while (iter.Next())
	    if (iter.symbolRef().isVariable()) 
	      { 
		if (!(remover.var(iter.symbolRef().var()))) return false; 
	      }
	    else if (!(remover.func(iter.symbolRef()))) return false;
	}; 
    }
  else 
    if (lit->arity())
      {
	if (!(remover.litHeader(*lit))) return false;
	iter.Reset(lit); 
	while (iter.Next())
	  if (iter.symbolRef().isVariable()) 
	    { 
	      if (!(remover.var(iter.symbolRef().var()))) return false; 
	    }
	  else if (!(remover.func(iter.symbolRef()))) return false;
      }
    else // propositional
      if (!(remover.propLit(*lit))) return false;       
  return remover.endOfLit();
}; // bool OLD_FS_CODE::removeLit(const TERM* lit)


#ifndef NO_DEBUG_VIS
ostream& OLD_FS_CODE::outputCode(ostream& str) const
{
  if (!tree) return str << "<<< Empty FS_CODE (old style) >>>\n";
  str << "<<< FS_CODE(old style) >>>\n";  
  return FS_COMMAND::outputTree(str,tree,subst) 
    << "<<< End of FS_CODE(old style) >>>\n";
}; // ostream& OLD_FS_CODE::outputCode(ostream& str) const
#endif



//================================================
#undef DEBUG_NAMESPACE
#ifdef DEBUG_OLD_FS_CODE
#define DEBUG_NAMESPACE "OLD_FS_CODE::INTEGRATOR"
#endif
#include "debugMacros.hpp"
//=================================================
void OLD_FS_CODE::INTEGRATOR::litHeader(const TERM& header)
{
  CALL("litHeader(const TERM& header)");
  if (writeMode)
    {
    write:
      *node_addr = FS_COMMAND::NewLIT_HEADER(header);
      moveDown();
      return;
    };
  // read mode
  FS_COMMAND* curr_instr;
  unsigned long header_num = header.HeaderNum();
 check_tag:
  curr_instr = *node_addr;
  switch (curr_instr->Tag())
    {
    case FS_COMMAND::LIT_HEADER :
      if (curr_instr->HeaderNum() == header_num) 
	{
	  moveDown();
	  return;
	};
      // different header
      // no break here! go to the next case 
    case FS_COMMAND::PROP_LIT :  // no break here! go to the next case 
    case FS_COMMAND::UNORD_EQ :  // no break here! go to the next case 
    case FS_COMMAND::ORD_EQ :   
    case FS_COMMAND::SUCCESS :     
      // mismatch occured, try fork
      if (curr_instr->Fork())
	{
	  moveRight();
	  goto check_tag;
	};    
      // no more forks
      moveRight();
      writeMode = true;
      goto write;

#ifdef DEBUG_NAMESPACE 
    default : ICP("X0"); return;
#else
#ifdef _SUPPRESS_WARNINGS_
    default : return;
#endif 
#endif  
    }; 
      
}; // void OLD_FS_CODE::INTEGRATOR::litHeader(const TERM& header)

void OLD_FS_CODE::INTEGRATOR::propLit(const TERM& header)
{
  CALL("propLit(const TERM& header)");
  if (writeMode)
    {
    write:
      *node_addr = FS_COMMAND::NewPROP_LIT(header);
      moveDown();
      return;
    };
  // read mode
  FS_COMMAND* curr_instr;
  unsigned long header_num = header.HeaderNum();
 check_tag:
  curr_instr = *node_addr;
  switch (curr_instr->Tag())
    {
    case FS_COMMAND::PROP_LIT :
      if (curr_instr->HeaderNum() == header_num) 
	{
	  moveDown();
	  return;
	};
      // different header
      // no break here! go to the next case 
    case FS_COMMAND::LIT_HEADER :  // no break here! go to the next case 
    case FS_COMMAND::UNORD_EQ :  // no break here! go to the next case 
    case FS_COMMAND::ORD_EQ :   
    case FS_COMMAND::SUCCESS :     
      // mismatch occured, try fork
      if (curr_instr->Fork())
	{
	  moveRight();
	  goto check_tag;
	};    
      // no more forks
      moveRight();
      writeMode = true;
      goto write;

#ifdef DEBUG_NAMESPACE 
    default : ICP("X0"); return;
#else
#ifdef _SUPPRESS_WARNINGS_
    default : return;
#endif 
#endif  
    }; 
}; // void OLD_FS_CODE::INTEGRATOR::propLit(const TERM& header)

void OLD_FS_CODE::INTEGRATOR::ordEq(const TERM& header)
{
  CALL("ordEq(const TERM& header)");
  if (writeMode)
    {
    write:
      *node_addr = FS_COMMAND::NewORD_EQ(header);
      moveDown();
      return;
    };
  // read mode
  FS_COMMAND* curr_instr;
  unsigned long header_num = header.HeaderNum();
 check_tag:
  curr_instr = *node_addr;
  switch (curr_instr->Tag())
    {
    case FS_COMMAND::ORD_EQ :
      if (curr_instr->HeaderNum() == header_num) 
	{
	  moveDown();
	  return;
	};
      // different header
      // no break here! go to the next case 
    case FS_COMMAND::LIT_HEADER :
    case FS_COMMAND::PROP_LIT :  // no break here! go to the next case 
    case FS_COMMAND::UNORD_EQ :  // no break here! go to the next case
    case FS_COMMAND::SUCCESS :     
      // mismatch occured, try fork
      if (curr_instr->Fork())
	{
	  moveRight();
	  goto check_tag;
	};    
      // no more forks
      moveRight();
      writeMode = true;
      goto write;

#ifdef DEBUG_NAMESPACE 
    default : ICP("X0"); return;
#else
#ifdef _SUPPRESS_WARNINGS_
    default : return;
#endif 
#endif  
    }; 
}; // void OLD_FS_CODE::INTEGRATOR::ordEq(const TERM& header)

void OLD_FS_CODE::INTEGRATOR::unordEq(const TERM& header)
{
  CALL("unordEq(const TERM& header)");
  if (writeMode)
    {
    write:
      *node_addr = FS_COMMAND::NewUNORD_EQ(header);
      moveDown();
      return;
    };
  // read mode
  FS_COMMAND* curr_instr;
  unsigned long header_num = header.HeaderNum();
 check_tag:
  curr_instr = *node_addr;
  switch (curr_instr->Tag())
    {
    case FS_COMMAND::UNORD_EQ :
      if (curr_instr->HeaderNum() == header_num) 
	{
	  moveDown();
	  return;
	};
      // different header
      // no break here! go to the next case 
    case FS_COMMAND::LIT_HEADER :
    case FS_COMMAND::PROP_LIT :  // no break here! go to the next case 
    case FS_COMMAND::ORD_EQ :  // no break here! go to the next case
    case FS_COMMAND::SUCCESS :     
      // mismatch occured, try fork
      if (curr_instr->Fork())
	{
	  moveRight();
	  goto check_tag;
	};    
      // no more forks
      moveRight();
      writeMode = true;
      goto write;

#ifdef DEBUG_NAMESPACE 
    default : ICP("X0"); return;
#else
#ifdef _SUPPRESS_WARNINGS_
    default : return;
#endif 
#endif  
    };  
}; // void OLD_FS_CODE::INTEGRATOR::unordEq(const TERM& hd)

void OLD_FS_CODE::INTEGRATOR::secondEqArg()
{
  CALL("secondEqArg()");
  if (writeMode)
    {
      *node_addr = FS_COMMAND::NewSECOND_EQ_ARG();
      moveDown();
      return;
    };
  // read mode
  ASSERT((*node_addr)->Tag() == FS_COMMAND::SECOND_EQ_ARG);
  ASSERT(!((*node_addr)->Fork()));
  moveDown();  
}; // void OLD_FS_CODE::INTEGRATOR::secondEqArg()

void OLD_FS_CODE::INTEGRATOR::endOfLit()
{
  CALL("endOfLit()");
  const Flatterm** var1;
  const Flatterm** var2;

  const Flatterm*** iter = compares.begin();
  const Flatterm*** iterEnd = compares.end();

  //compares.ResetIterator();
  if (writeMode) 
    {
    write:
      //while (compares.IterNotEnd())
      while (iter != iterEnd)
	{
	  //var1 = *(compares.Next());
	  var1 = *iter;
	  //CHECK7(compares.IterNotEnd(),"Y0 in OLD_FS_CODE::INTEGRATOR::endOfLit()");
	  //var2 = *(compares.Next());
	  iter++;
	  ASSERT(iter != iterEnd);
	  var2 = *iter;
	  iter++;
	  *node_addr = FS_COMMAND::NewCOMPARE(var1,var2);
	  moveDown();
	};
      return;
    };

  // read mode
  //while (compares.IterNotEnd())
  while (iter != iterEnd)
    {
      //var1 = *(compares.Next());
      //CHECK7(compares.IterNotEnd(),"Y1 in OLD_FS_CODE::INTEGRATOR::endOfLit()");
      //var2 = *(compares.Next());
      var1 = *iter;
      iter++;
      ASSERT(iter != iterEnd);
      var2 = *iter;
      iter++;

      if (findCompare(var1,var2))
	{
	  ASSERT((*node_addr)->IsCompare());
	  moveDown(); 
	}
      else 
	{
	  ASSERT((*node_addr) ? (*node_addr)->NextLitOrSuccess() : true);
	  FS_COMMAND* newInstr = FS_COMMAND::NewCOMPARE(var1,var2);
	  newInstr->SetFork(*node_addr);
	  *node_addr = newInstr;
	  moveDown();
	  writeMode = true; 
	  goto write;
	};
    };   
  // skip all the remaining compares
  while ((*node_addr)->IsCompare())
    {
      moveRight();
      if (!(*node_addr)) { writeMode = true; return; };   
    };
  ASSERT((*node_addr)->NextLitOrSuccess());
}; // void OLD_FS_CODE::INTEGRATOR::endOfLit()


inline void OLD_FS_CODE::INTEGRATOR::moveDown() 
{ 
  node_addr = (*node_addr)->NextAddr(); 
};

inline void OLD_FS_CODE::INTEGRATOR::moveRight() 
{ 
  node_addr = (*node_addr)->ForkAddr(); 
};

bool OLD_FS_CODE::INTEGRATOR::findCompare(const Flatterm** v1,const Flatterm** v2)
{
  CALL("findCompare(const Flatterm** v1,const Flatterm** v2)");
  ASSERT(v1 < v2);
  FS_COMMAND* curr_instr;
 check_tag:
  curr_instr = *node_addr;
  if (curr_instr->Tag() == FS_COMMAND::COMPARE)
    {
      if (curr_instr->SameVars(v1,v2)) return true;
      // try fork
      if (curr_instr->Fork()) { moveRight(); goto check_tag; };
      // no more forks 
      moveRight();
      return false;
    }
  else return false;
}; // bool OLD_FS_CODE::INTEGRATOR::findCompare(const Flatterm** v1,const Flatterm** v2)

void OLD_FS_CODE::INTEGRATOR::func(const TERM& sym)
{
  CALL("func(const TERM& sym)");
  if (writeMode)
    {
    write:
      *node_addr = FS_COMMAND::NewFUNC(sym);
      moveDown();
      return;
    }; 
  // read mode
  FS_COMMAND* curr_instr;
 check_tag:
  curr_instr = *node_addr;
  switch (curr_instr->Tag())
    {
    case FS_COMMAND::FUNC :
      if (curr_instr->Func() == sym) 
	{
	  moveDown();
	  return;
	};
      // other functor
      if (curr_instr->Func() > sym)
	{
	  FS_COMMAND* newInstr = FS_COMMAND::NewFUNC(sym);  
	  newInstr->SetFork(curr_instr);        
	  *node_addr = newInstr;
	  moveDown();
	  writeMode = true;
	  return;
	};
      // curr_instr->Func() < sym here
      // no break here! go to the next case 
    case FS_COMMAND::VAR :
      // mismatch occured, try fork
      if (curr_instr->Fork())
	{
	  moveRight();
	  goto check_tag;
	}; 
      //no more forks
      moveRight();
      writeMode = true;
      goto write;      

#ifdef DEBUG_NAMESPACE 
    default : ICP("X0"); return; 
#else
#ifdef _SUPPRESS_WARNINGS_
    default : return;
#endif 
#endif 
    };
}; // void OLD_FS_CODE::INTEGRATOR::func(const TERM& sym)

void OLD_FS_CODE::INTEGRATOR::var(unsigned long v)
{
  CALL("var(unsigned long v)");
  const Flatterm** techVar = nextTechnicalVar;
  nextTechnicalVar++;
  if (registeredVars.Occurences(v))
    {  
      compares.push(varFirstOcc[v]);
      compares.push(techVar);
    }
  else // first occurence of the variable
    {
      registeredVars.Register(v);
      varFirstOcc[v] = techVar; 
    };

  if (writeMode) 
    {
      *node_addr = FS_COMMAND::NewVAR(techVar);
      moveDown();
      return;   
    };  
  // read mode
  FS_COMMAND* curr_instr;
  curr_instr = *node_addr;
  switch (curr_instr->Tag())
    {
    case FS_COMMAND::VAR :
      ASSERT(curr_instr->Var() == techVar);
      moveDown();
      return;     

    case FS_COMMAND::FUNC :
      *node_addr = FS_COMMAND::NewVAR(techVar);
      (*node_addr)->SetFork(curr_instr);
      moveDown();
      writeMode = true;
      return;       

#ifdef DEBUG_NAMESPACE 
    default : ICP("X0"); return;
#else
#ifdef _SUPPRESS_WARNINGS_
    default : return;
#endif 
#endif     
    };
}; // void OLD_FS_CODE::INTEGRATOR::var(unsigned long v)

void OLD_FS_CODE::INTEGRATOR::endOfClause(Clause* cl)
{
  FS_COMMAND* newInstr = FS_COMMAND::NewSUCCESS(cl);
  newInstr->SetFork(*node_addr);
  *node_addr = newInstr;   
}; // void OLD_FS_CODE::INTEGRATOR::endOfClause(Clause* cl)

void OLD_FS_CODE::INTEGRATOR::endOfComponent(void* name,unsigned long numOfLits)
{
  FS_COMMAND* newInstr = FS_COMMAND::NewSUCCESS(static_cast<Clause*>(name));
  newInstr->SetFork(*node_addr);
  *node_addr = newInstr;
  newInstr->SetNumOfDiffVars(registeredVars.NumOfDifferent());  
  newInstr->SetNumOfLits(numOfLits);  
}; //void OLD_FS_CODE::INTEGRATOR::endOfComponent(void* name,unsigned long numOfLits) 


//================================================
#undef DEBUG_NAMESPACE
#ifdef DEBUG_OLD_FS_CODE
#define DEBUG_NAMESPACE "OLD_FS_CODE::REMOVER"
#endif
#include "debugMacros.hpp"
//=================================================
bool OLD_FS_CODE::REMOVER::litHeader(const TERM& header)
{
  CALL("litHeader(const TERM& header)");
  unsigned long header_num = header.HeaderNum();
 check_tag:
  ASSERT((*node_addr)->NextLitOrSuccess());
  if ((*node_addr)->Tag() == FS_COMMAND::LIT_HEADER) 
    {
      if ((*node_addr)->HeaderNum() == header_num) 
	{
	  moveDown();
	  return true;
	};
    };
  // try fork
  if ((*node_addr)->Fork()) { moveRight(); goto check_tag; }
  else return false; 
}; // bool OLD_FS_CODE::REMOVER::litHeader(const TERM& header)

bool OLD_FS_CODE::REMOVER::propLit(const TERM& header)
{
  CALL("propLit(const TERM& header)");
  unsigned long header_num = header.HeaderNum();
 check_tag: 
  ASSERT((*node_addr)->NextLitOrSuccess());
  if ((*node_addr)->Tag() == FS_COMMAND::PROP_LIT) 
    {
      if ((*node_addr)->HeaderNum() == header_num) 
	{
	  moveDown();
	  return true;
	};
    };
  // try fork
  if ((*node_addr)->Fork()) { moveRight(); goto check_tag; }
  else return false; 
}; // bool OLD_FS_CODE::REMOVER::propLit(const TERM& header)

bool OLD_FS_CODE::REMOVER::ordEq(const TERM& header)
{
  CALL("ordEq(const TERM& header)");
  unsigned long header_num = header.HeaderNum();
 check_tag:
  ASSERT((*node_addr)->NextLitOrSuccess());
  if ((*node_addr)->Tag() == FS_COMMAND::ORD_EQ) 
    {
      if ((*node_addr)->HeaderNum() == header_num) 
	{
	  moveDown();
	  return true;
	};
    };
  // try fork
  if ((*node_addr)->Fork()) { moveRight(); goto check_tag; }
  else return false; 
}; // bool OLD_FS_CODE::REMOVER::ordEq(const TERM& header)

bool OLD_FS_CODE::REMOVER::unordEq(const TERM& header)
{
  CALL("unordEq(const TERM& header)");
  unsigned long header_num = header.HeaderNum();
 check_tag: 
  ASSERT((*node_addr)->NextLitOrSuccess());
  if ((*node_addr)->Tag() == FS_COMMAND::UNORD_EQ) 
    {
      if ((*node_addr)->HeaderNum() == header_num) 
	{
	  moveDown();
	  return true;
	};
    };
  // try fork
  if ((*node_addr)->Fork()) { moveRight(); goto check_tag; }
  else return false; 
}; // bool OLD_FS_CODE::REMOVER::unordEq(const TERM& header)


bool OLD_FS_CODE::REMOVER::endOfLit()
{
  CALL("endOfLit()");
  const Flatterm** tv1;
  const Flatterm** tv2;

 skip_next_compare:
  if (compares.empty()) 
    {   
      // skip all the remaining compares in the tree
      while ((*node_addr)->IsCompare())
	{
	  if ((*node_addr)->Fork()) { moveRight(); }
	  else 
	    {
	      return false;
	    };
	};         
      ASSERT((*node_addr)->NextLitOrSuccess());
      return true;
    }
  else
    {
      ASSERT(compares.size() >= 2);
      tv1 = compares.head(); 
      compares.dequeue();
      tv2 = compares.head(); 
      compares.dequeue();
      ASSERT(tv1 < tv2);
    check_tag:
      if ((*node_addr)->IsCompare())
	{
	  if ((tv1 == (*node_addr)->Var1()) && (tv2 == (*node_addr)->Var2())) 
	    {
	      moveDown();
	      goto skip_next_compare;   
	    };  
	  // try fork
	  if ((*node_addr)->Fork()) { moveRight(); goto check_tag; };        
	};    
      return false;
    };
}; // bool OLD_FS_CODE::REMOVER::endOfLit()   

bool OLD_FS_CODE::REMOVER::func(const TERM& sym)
{
  CALL("func(const TERM& sym)");
 check_tag: 
  ASSERT((*node_addr)->VarOrFunc());
  if ((*node_addr)->Tag() == FS_COMMAND::FUNC)
    {
      if ((*node_addr)->Func() == sym) { moveDown(); return true; };   
    };
  // try fork
  if ((*node_addr)->Fork()) { moveRight(); goto check_tag; }
  else return false; 
}; // bool OLD_FS_CODE::REMOVER::func(const TERM& sym)

bool OLD_FS_CODE::REMOVER::var(unsigned long v)
{
  CALL("var(unsigned long v)");
  ASSERT((*node_addr)->VarOrFunc());
  const Flatterm** techVar = nextTechnicalVar; 
  nextTechnicalVar++;
  if (registeredVars.Occurences(v))
    {
      COP("A1"); 
      ASSERT(!(compares.full()));
      compares.tail() = firstOccTechVar[v];
      compares.enqueue(); 
      COP("A2"); 
      ASSERT(!(compares.full()));
      compares.tail() = techVar;
      compares.enqueue();
    }
  else // first occurence
    {
      registeredVars.Register(v);
      firstOccTechVar[v] = techVar;
    }; 

  if ((*node_addr)->Tag() == FS_COMMAND::VAR) { moveDown(); return true; };
  return false;   
}; // bool OLD_FS_CODE::REMOVER::var(unsigned long v)  
   
bool OLD_FS_CODE::REMOVER::endOfClause(Clause* cl)
{
  CALL("endOfClause(Clause* cl)");
 check_tag:  
  ASSERT((*node_addr)->NextLitOrSuccess());
  if ((*node_addr)->Tag() == FS_COMMAND::SUCCESS)
    if ((*node_addr)->TheClause() == cl)
      {
	if ((*node_addr)->Fork()) linBranch = node_addr;
	FS_COMMAND* lb = *linBranch;
	*linBranch = lb->Fork();
	lb->SetFork(0);
	ASSERT(linearBranch(lb)); 
	FS_COMMAND::DestroyTree(lb);    
	return true;
      };       
  // try fork
  if ((*node_addr)->Fork()) { moveRight(); goto check_tag; };
  // no more forks
  return false;
}; // bool OLD_FS_CODE::REMOVER::endOfClause(Clause* cl)

//============================================================================
