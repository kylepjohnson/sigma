//
// File:         fs_code.cpp
// Description:  Maintainence of index for FS.
// Created:      Oct 15, 1999, 15:00
// Author:       Alexandre Riazanov
// mail:         riazanov@cs.man.ac.uk
//====================================================
#include "fs_code.hpp"
#include "Term.hpp"
#include "Clause.hpp"
#include "Flatterm.hpp"
#include "DestructionMode.hpp"
using namespace BK;
using namespace VK;
//====================================================
#undef DEBUG_NAMESPACE
#ifdef DEBUG_FS_CODE
#define DEBUG_NAMESPACE "FS_CODE"
#endif
#include "debugMacros.hpp"
//===================================================




FS_CODE::FS_CODE(const Flatterm** sbst) 
  : oldStyle(false),
    oldStyleCode(sbst),
    tree(0),
    subst(sbst),
    integrator(subst), 
    removal(subst),
    _maxlNumOfLits(0UL) 
{
  CALL("constructor FS_CODE(const Flatterm** sbst)");
};

FS_CODE::~FS_CODE() 
{ 
  CALL("destructor ~FS_CODE()"); 
  if (DestructionMode::isThorough())
    {
      if (oldStyle) 
	{ 
	  oldStyleCode.reset(); 
	}
      else
	{
	  if (tree) FS_COMMAND::DestroyTree(tree);
	};
    };
};

void FS_CODE::init(const Flatterm** sbst) 
{
  CALL("init(const Flatterm** sbst)");
  oldStyle = false;
  oldStyleCode.init(sbst);
  tree = 0;
  subst = sbst;
  integrator.init(subst); 
  removal.init(subst);
  _maxlNumOfLits = 0UL; 
}; // void FS_CODE::init(const Flatterm** sbst) 


void FS_CODE::destroy()
{
  CALL("destroy()");
  if (DestructionMode::isThorough())
    {
      if (oldStyle) 
	{ 
	  oldStyleCode.reset(); 
	}
      else
	{
	  if (tree) FS_COMMAND::DestroyTree(tree);
	};
    };
  removal.destroy();
  integrator.destroy();
}; // void FS_CODE::destroy()

void FS_CODE::Remove(Clause* cl) 
{
  CALL("Remove(Clause* cl)"); 
  
  //DF;  cout << "REMOVE " << cl << "\n"; DF;

  if (oldStyle) 
    {     
#ifdef DEBUG_NAMESPACE
      bool removed = oldStyleCode.remove(cl); 
#else 
      oldStyleCode.remove(cl); 
#endif      

      ASSERT(removed);   
      return; 
    };

  if (!tree) return;
  removal.Reset(&tree,cl);
   
  LiteralList::Iterator iter(cl->LitList());
  TERM* lit;
 next_lit:
  if (iter.end()) 
    {
      // end of clause
      if (removal.FindSuccess())
	{
	  removal.DestroyBranch();
	}
      else
	{ 
	  COP("COP1");
	  DMSG("Removed clause is not found in the FS index:\n"); 
	  DMSG(cl->Number());
	  DMSG("\n");
	  ICP(""); 
	};
      return;
    };
  lit = iter.currentLiteral();
  iter.next();
  if (removal.SkipLiteral(lit)) goto next_lit;

  COP("COP2");
  DMSG("Removed clause is not found in the FS index:\n"); 
  DMSG(cl->Number());
  DMSG("\n");
  ICP(""); 
}; // void FS_CODE::Remove(Clause* cl) 


void FS_CODE::Integrate(Clause* cl) 
{
  CALL("Integrate(Clause* cl)");


  if (cl->numOfAllLiterals() > _maxlNumOfLits) _maxlNumOfLits = cl->numOfAllLiterals();
  
  if (oldStyle) { oldStyleCode.integrate(cl); return; };  

  integrator.Reset(&tree,cl);
  LiteralList::Iterator iter(cl->LitList());
  TERM* lit;
  if (!tree) goto write_mode_next_lit;
  
 read_mode_next_lit:
  if (iter.end()) 
    {
      // end of clause
      integrator.InsertSuccess(cl);
      return;
    }; 
  lit = iter.currentLiteral();
  iter.next();
  if (integrator.ReadLiteral(lit)) 
    {
      goto read_mode_next_lit;
    }
  else
    {
      ASSERT(!integrator.CurrInstr());
      goto write_mode_next_lit;
    }; 
    
 write_mode_next_lit:
  if (iter.end()) 
    {
      // end of clause
      integrator.WriteSuccess(cl);

      return;
    }; 
  lit = iter.currentLiteral();
  iter.next();
  integrator.WriteLiteral(lit);
  goto write_mode_next_lit;
}; // void FS_CODE::Integrate(Clause* cl)  

//====================================================
#include "fs_code.hpp"
#undef DEBUG_NAMESPACE
#ifdef DEBUG_FS_CODE
#define DEBUG_NAMESPACE "FS_CODE_INTEGRATOR"
#endif
#include "debugMacros.hpp"
//===================================================


FS_CODE_INTEGRATOR::FS_CODE_INTEGRATOR(const Flatterm** subst) 
  : constr_to_insert("constr_to_insert"),
    constr_to_shift("constr_to_shift"),
    constr_passed("constr_passed"),
    constr_passed_in_shift("constr_passed_in_shift"),
    constr_integrated("constr_integrated"),
    constr_old("constr_old"),
    fs_subst(subst),
    vars(),
    use_sig_filters(true) // default
{ 
  CALL("constructor FS_CODE_INTEGRATOR(const Flatterm** subst)");
  sig_to_insert = 0;
  sig_to_shift = 0;
};
FS_CODE_INTEGRATOR::~FS_CODE_INTEGRATOR() { CALL("destructor ~FS_CODE_INTEGRATOR()"); };

void FS_CODE_INTEGRATOR::init(const Flatterm** subst) 
{
  CALL("init(const Flatterm** subst)");
  constr_to_insert.init("constr_to_insert");
  constr_to_shift.init("constr_to_shift");
  constr_passed.init("constr_passed");
  constr_passed_in_shift.init("constr_passed_in_shift");
  constr_integrated.init("constr_integrated");
  constr_old.init("constr_old");
  fs_subst = subst;
  vars.init();
  use_sig_filters = true; // default
  sig_to_insert = 0;
  sig_to_shift = 0;
}; // void FS_CODE_INTEGRATOR::init(const Flatterm** subst) 

void FS_CODE_INTEGRATOR::destroy()
{
  CALL("destroy()");
  vars.destroy();
  constr_old.destroy();;
  constr_integrated.destroy();;
  constr_passed_in_shift.destroy();;
  constr_passed.destroy();;
  constr_to_shift.destroy();;
  constr_to_insert.destroy();;
}; // void FS_CODE_INTEGRATOR::destroy()


void FS_CODE_INTEGRATOR::WriteEqLiteral(TERM* lit)
{
  CALL("WriteEqLiteral(TERM* lit)");
  ASSERT(lit->IsEquality());
  TERM* args = lit->Args();
  TERM arg1 = *args;
  TERM arg2 = *(args->Next());
  
  if (lit->IsOrderedEq())
    {
      WriteOrdEq(*lit); 
    }
  else
    {
      WriteUnordEq(*lit); 
    }; 
  
  WriteTerm(arg1);
  WriteSecondEqArg();
  WriteTerm(arg2);
}; // void FS_CODE_INTEGRATOR::WriteEqLiteral(TERM* lit)

void FS_CODE_INTEGRATOR::WriteLiteral(TERM* lit)
{ 
  
  if (lit->IsEquality()) { WriteEqLiteral(lit); return; };
  TERM::Iterator iter;
  TERM sym;

  if (lit->arity())
    {
      WriteHeader(*lit);
    
      iter.Reset(lit);
    write_mode_next_sym:
      if (!(iter.Next())) return;
      
      sym = iter.Symbol();
      if (iter.CurrentSymIsVar())
	{
	  WriteVar(sym.var());
	}
      else // functor
	{
	  WriteFunc(sym);
	}; 
      goto write_mode_next_sym; 
    }
  else // propositional
    {
      WritePropLit(*lit);
      return;
    }; 
}; // void FS_CODE_INTEGRATOR::WriteLiteral(TERM* lit)

bool FS_CODE_INTEGRATOR::ReadVar(const ulong& var)
{
  CALL("ReadVar(ulong var)");
  if (!(ReadVar())) 
    { 
      InsertVar(fs_var);
      if (vars.Occurences(var))
	{
	  COP("was before");
	  EnterWriteMode(); 
	  WriteCompare(first_occ[var],fs_var);
	}
      else // first occurence
	{
	  COP("first occ");
	  vars.Register(var);
	  first_occ[var] = fs_var;
	  EnterWriteMode(); 
	}; 
      fs_var++;
      return false; 
    };
          
  if (vars.Occurences(var))
    {
      ReadCompare(first_occ[var],fs_var);
    } 
  else // first occurence of the variable
    {
      vars.Register(var);
      first_occ[var] = fs_var;
    }; 
  fs_var++;
  return true;
}; // bool FS_CODE_INTEGRATOR::ReadVar(ulong var)

bool FS_CODE_INTEGRATOR::ReadComplexTerm(TERM* t)
{
  CALL("ReadComplexTerm(TERM* t)");
  TERM::Iterator iter;
  TERM sym;
  iter.Reset(t);
  if (ReadFunc(*t)) 
    {
    read_mode_next_sym:
      if (!iter.Next()) return true; 
      sym = iter.Symbol();
      if (iter.CurrentSymIsVar())
	{
	  if (!(ReadVar(sym.var()))) goto write_mode_next_sym; 
	}
      else // functor
	{
	  if (!(ReadFunc(sym))) 
	    { 
	      InsertFunc(sym);
	      COP("func inserted 1");
	      EnterWriteMode(); 
	      goto write_mode_next_sym; 
	    };
	}; 
      goto read_mode_next_sym;
    };
  InsertFunc(*t);
  COP("func inserted 2");
  EnterWriteMode();  
  
 write_mode_next_sym:
  if (!iter.Next()) return false;
  sym = iter.Symbol();
  if (iter.CurrentSymIsVar())
    {
      WriteVar(sym.var());
    }
  else // functor
    {
      WriteFunc(sym);
    }; 
  goto write_mode_next_sym; 
}; // bool FS_CODE_INTEGRATOR::ReadComplexTerm(TERM* t)

bool FS_CODE_INTEGRATOR::ReadEqLiteral(TERM* lit)
{
  CALL("ReadEqLiteral(TERM* lit)");
  ASSERT(lit->IsEquality());
  TERM* args = lit->Args();
  TERM arg1 = *args;
  TERM arg2 = *(args->Next());
  
  if (lit->IsOrderedEq())
    {
      if (!ReadOrdEq(*lit)) { WriteOrdEq(*lit); goto write_mode_first_arg; };
    }
  else
    {
      if (!ReadUnordEq(*lit)) { WriteUnordEq(*lit); goto write_mode_first_arg; };
    }; 
  
  // read mode, first argument 
  if (!ReadTerm(arg1)) goto write_mode_second_arg; 
  
  // read mode, second argument
  ReadSecondEqArg(); 
  return (ReadTerm(arg2)) && (ReadCollectedComps()); 
  
 write_mode_first_arg:
  WriteTerm(arg1);
 write_mode_second_arg:
  WriteSecondEqArg();
  WriteTerm(arg2);
  return false;
}; // bool FS_CODE_INTEGRATOR::ReadEqLiteral(TERM* lit) 

bool FS_CODE_INTEGRATOR::ReadLiteral(TERM* lit)
{
  CALL("ReadLiteral(TERM* lit)"); 
  ASSERT((!debugLitNum) || constr_integrated.Implies(constr_old));
  ASSERT((!debugLitNum) || constr_old.Implies(constr_passed));
  ASSERT((!debugLitNum) || constr_passed.Implies(constr_integrated));
  DOP(debugLitNum++);

  if (lit->IsEquality()) return ReadEqLiteral(lit);
  
  TERM::Iterator iter;
  TERM sym;
  
  if (lit->arity())
    {
      if (ReadHeader(*lit))
	{      
	  iter.Reset(lit);
	read_mode_next_sym:
	  if (!(iter.Next())) return ReadCollectedComps();
	  sym = iter.Symbol();
	  if (iter.CurrentSymIsVar())
	    {
	      if (!(ReadVar(sym.var()))) goto write_mode_next_sym; 
	    }
	  else // functor
	    {
	      if (!(ReadFunc(sym))) 
		{ 
		  InsertFunc(sym);
		  EnterWriteMode(); 
		  ASSERT_IN(!CurrInstr(),"A30");
		  goto write_mode_next_sym; 
		};
	    }; 
	  goto read_mode_next_sym; 
	} 
      else // the header was not read
	{
	  ASSERT_IN(!CurrInstr(),"A40");
	  WriteHeader(*lit);
	  goto write_mode_lit_args;
	};  
    }
  else // propositional literal
    {
      if (ReadPropLit(*lit)) return true;
      // the literal was not read
      ASSERT_IN(!CurrInstr(),"A50");
      WritePropLit(*lit);
      return false;
    }; 
  
  // write mode: 
 write_mode_lit_args: 
  iter.Reset(lit);
 write_mode_next_sym:       
  if (!(iter.Next())) return false;
       
  sym = iter.Symbol();
  if (iter.CurrentSymIsVar())
    {
      WriteVar(sym.var());
    }
  else // functor
    {
      WriteFunc(sym);
    }; 
  goto write_mode_next_sym; 
   
}; // bool FS_CODE_INTEGRATOR::ReadLiteral(TERM* lit)

bool FS_CODE_INTEGRATOR::ReadHeader(const TERM& header)
{
  CALL("ReadHeader(const TERM& header)");
  ASSERT(*node_addr);
  ASSERT(constr_to_shift.Identity());
  ASSERT(constr_to_insert.Identity());
  ASSERT(!header.IsEquality()); 

  FS_COMMAND* curr_instr;
  ulong header_num = header.HeaderNum();

 check_tag:
  curr_instr = *node_addr;
  switch (curr_instr->Tag())
    {
    case FS_COMMAND::LIT_HEADER :
      if (curr_instr->HeaderNum() == header_num) 
	{
	  ShiftSigFilterRight();
	  MoveDown(); 
	  UnregFunToInsert(header_num);
	  UnregFunToShift(header_num);
	  return true; 
	};
      // different header
      // no break here! go to the next case 
      
    case FS_COMMAND::PROP_LIT :  // no break here! go to the next case 
    case FS_COMMAND::UNORD_EQ :  // no break here! go to the next case 
    case FS_COMMAND::ORD_EQ :  
      
      // mismatch occured, try fork
      ShiftSigFilterDown();
      if (curr_instr->Fork())
	{
	  MoveRight();
	  goto check_tag;
	};
      // no more forks 
      MoveRight();
      UnregFunToInsert(header_num); 
      return false;

    case FS_COMMAND::SIG_FILTER :
      PassSigFilter(); 
      goto check_tag;      
   
    case FS_COMMAND::SUCCESS : 
      
      // curr_instr->TheClause() subsumes the clause currently being integrated
      DWARN("Clause being integrated into FS index is subsumed by already integrated");
      DMSG("Clause\n ");
      DMSG(new_clause->Number());
      DMSG("\n is subsumed by\n ");
      DMSG(curr_instr->TheClause()->Number());  
      MoveRight();
      if (*node_addr) { goto check_tag; };
      return false;
       
#ifdef DEBUG_NAMESPACE 
    default : ICP("X0"); return false;
#else
#ifdef _SUPPRESS_WARNINGS_
    default : return false;
#endif 
#endif  
    };
}; // bool FS_CODE_INTEGRATOR::ReadHeader(const TERM& header)
 

bool FS_CODE_INTEGRATOR::ReadPropLit(const TERM& header)
{
  CALL("ReadPropLit(const TERM& header)");
  ASSERT(*node_addr);
  ASSERT(constr_to_shift.Identity());
  ASSERT(constr_to_insert.Identity());
  
  FS_COMMAND* curr_instr;
  ulong header_num = header.HeaderNum();
  
 check_tag:
  curr_instr = *node_addr;
  switch (curr_instr->Tag())
    {
    case FS_COMMAND::PROP_LIT :
      if (curr_instr->HeaderNum() == header_num) 
	{
	  ShiftSigFilterRight();
	  MoveDown(); 
	  UnregFunToInsert(header_num);
	  UnregFunToShift(header_num);
	  return true; 
	};
      // different header
      // no break here! go to the next case 
      
    case FS_COMMAND::LIT_HEADER : // no break here! go to the next case 
    case FS_COMMAND::UNORD_EQ : // no break here! go to the next case 
    case FS_COMMAND::ORD_EQ :
      ShiftSigFilterDown();   
      // mismatch occured, try fork
      if (curr_instr->Fork())
	{
	  MoveRight();
	  goto check_tag;
	};
      // no more forks 
      MoveRight();
      UnregFunToInsert(header_num);
      return false;
  
    case FS_COMMAND::SIG_FILTER :   
      PassSigFilter();      
      goto check_tag; 

    case FS_COMMAND::SUCCESS : 
      
      // curr_instr->TheClause() subsumes the clause currently being integrated
      DWARN("Clause being integrated into FS index is subsumed by already integrated");
      DMSG("Clause\n ");
      DMSG(new_clause->Number());
      DMSG("\n is subsumed by\n ");
      DMSG(curr_instr->TheClause()->Number());  

      MoveRight();
      if (*node_addr) { goto check_tag; };
      return false;
       
#ifdef DEBUG_NAMESPACE 
    default : ICP("X0"); return false;
#else
#ifdef _SUPPRESS_WARNINGS_
    default : return false;
#endif 
#endif  
    };
}; // bool FS_CODE_INTEGRATOR::ReadPropLit(const TERM& header)
 

bool FS_CODE_INTEGRATOR::ReadUnordEq(const TERM& header)
{
  CALL("ReadUnordEq(const TERM& header)");
  ASSERT(*node_addr);
  ASSERT(constr_to_shift.Identity());
  ASSERT(constr_to_insert.Identity());
  
  FS_COMMAND* curr_instr;
  ulong header_num = header.HeaderNum();
  
 check_tag:
  curr_instr = *node_addr;
  switch (curr_instr->Tag())
    {
    case FS_COMMAND::UNORD_EQ :
      if (curr_instr->HeaderNum() == header_num) 
	{
	  ShiftSigFilterRight();
	  MoveDown(); 
	  UnregFunToInsert(header_num);
	  UnregFunToShift(header_num);
	  return true; 
	};
      // different header
      // no break here! go to the next case 
      
    case FS_COMMAND::LIT_HEADER : // no break here! go to the next case 
    case FS_COMMAND::PROP_LIT : // no break here! go to the next case 
    case FS_COMMAND::ORD_EQ :   
      // mismatch occured, try fork
      ShiftSigFilterDown();
      if (curr_instr->Fork())
	{
	  MoveRight();
	  goto check_tag;
	};
      // no more forks 
      MoveRight();
      UnregFunToInsert(header_num);
      return false;
     
    case FS_COMMAND::SIG_FILTER :
      PassSigFilter();
      goto check_tag; 

    case FS_COMMAND::SUCCESS : 
      
      // curr_instr->TheClause() subsumes the clause currently being integrated
      DWARN("Clause being integrated into FS index is subsumed by already integrated");
      DMSG("Clause\n ");
      DMSG(new_clause->Number());
      DMSG("\n is subsumed by\n ");
      DMSG(curr_instr->TheClause()->Number());  

      MoveRight();
      if (*node_addr) { goto check_tag; };
      return false;
       
#ifdef DEBUG_NAMESPACE 
    default : ICP("X0"); return false;
#else
#ifdef _SUPPRESS_WARNINGS_
    default : return false;
#endif 
#endif  
    };
}; // bool FS_CODE_INTEGRATOR::ReadUnordEq(const TERM& header)

bool FS_CODE_INTEGRATOR::ReadOrdEq(TERM header)
{
  CALL("ReadOrdEq(TERM header)");
  ASSERT(*node_addr);
  ASSERT(constr_to_shift.Identity());
  ASSERT(constr_to_insert.Identity());
  
  FS_COMMAND* curr_instr;
  ulong header_num = header.HeaderNum();
  
 check_tag:
  curr_instr = *node_addr;
  switch (curr_instr->Tag())
    {
    case FS_COMMAND::ORD_EQ :
      if (curr_instr->HeaderNum() == header_num) 
	{
	  ShiftSigFilterRight();
	  MoveDown(); 
	  header.MakeUnordered();
	  UnregFunToInsert(header.HeaderNum());
	  UnregFunToShift(header.HeaderNum());
	  return true; 
	};
      // different header
      // no break here! go to the next case 
      
    case FS_COMMAND::LIT_HEADER : // no break here! go to the next case 
    case FS_COMMAND::PROP_LIT : // no break here! go to the next case 
    case FS_COMMAND::UNORD_EQ :   
      // mismatch occured, try fork
      ShiftSigFilterDown();
      if (curr_instr->Fork())
	{
	  MoveRight();
	  goto check_tag;
	};
      // no more forks 
      MoveRight();
      header.MakeUnordered();
      UnregFunToInsert(header.HeaderNum());
      return false;

    case FS_COMMAND::SIG_FILTER :
      PassSigFilter();
      goto check_tag;

    case FS_COMMAND::SUCCESS : 
      
      // curr_instr->TheClause() subsumes the clause currently being integrated
      DWARN("Clause being integrated into FS index is subsumed by already integrated");
      DMSG("Clause\n ");
      DMSG(new_clause->Number());
      DMSG("\n is subsumed by\n ");
      DMSG(curr_instr->TheClause()->Number());  

      MoveRight();
      if (*node_addr) { goto check_tag; };
      return false;
       
#ifdef DEBUG_NAMESPACE
    default : ICP("X0"); return false;
#else
#ifdef _SUPPRESS_WARNINGS_
    default : return false;
#endif 
#endif  
    };
}; // bool FS_CODE_INTEGRATOR::ReadOrdEq(TERM header)

void FS_CODE_INTEGRATOR::ReadSecondEqArg()
{
  CALL("ReadSecondEqArg()");

  FS_COMMAND* curr_instr;
  
 check_tag:
  curr_instr = *node_addr;
  switch (curr_instr->Tag())
    {
    case FS_COMMAND::SECOND_EQ_ARG : MoveDown(); return;
    case FS_COMMAND::COMPARE_NF :
      PassCompNF(curr_instr);
      goto check_tag; 

    case FS_COMMAND::SIG_FILTER :
      PassSigFilter();
      goto check_tag;

#ifdef DEBUG_NAMESPACE 
    default : ICP("X0"); return;
#else
#ifdef _SUPPRESS_WARNINGS_
    default : return;
#endif 
#endif 
    }; 
}; // void FS_CODE_INTEGRATOR::ReadSecondEqArg()

bool FS_CODE_INTEGRATOR::ReadFunc(const TERM& func)
{
  CALL("ReadFunc(const TERM& func)");
  FS_COMMAND* curr_instr;
  
 check_tag:
  curr_instr = *node_addr;
  switch (curr_instr->Tag())
    {
    case FS_COMMAND::FUNC :
      if (curr_instr->Func() == func) 
	{
	  DOP(CheckConstrIntegrity());
	  if (curr_instr->Fork()) 
	    {
	      if (!constr_to_shift.Identity())  
		{       
		  constr_passed_in_shift = constr_passed; 
		  CopyCompsToShift(curr_instr->ForkAddr()); 
		};
	    };
	  ShiftSigFilterRight();       
	  MoveDown(); 
	  UnregFunToInsert(func.HeaderNum()); 
	  UnregFunToShift(func.HeaderNum()); 
	  return true; 
	};
      // different symbol
      if (curr_instr->Func() > func)
	{
	  if (!constr_to_shift.Identity())  
	    {
	      constr_passed_in_shift = constr_passed;
	      InsertCompsToShift(node_addr);
	    };

	  curr_instr = *node_addr;
	  if ((use_sig_filters) && (sig_to_shift->nonempty()))
	    { 
	      if (!sig_to_shift->containsAtLeast(2)) // singletone
		{
		  ulong hd_num;
		  if ((curr_instr->SkeletonHeaderNum(hd_num))
		      && (sig_to_shift->contains(hd_num)))
		    goto terminate_with_failure;  
		};
	      FunSet* new_filter = sig_to_shift->copy();
	      FS_COMMAND* new_comm = FS_COMMAND::NewSIG_FILTER(new_filter);
	      new_comm->SetNext(*node_addr);
	      *node_addr = new_comm;
	    }; 
	
        terminate_with_failure:
	  UnregFunToInsert(func.HeaderNum());
	  return false;
	};  
      // curr_instr->Func() < func here
      // no break here! go to the next case 
      
    case FS_COMMAND::VAR :
      // mismatch occured, try fork
      DOP(CheckConstrIntegrity());
      ShiftSigFilterDown();
      if (curr_instr->Fork())
	{
	  if (!constr_to_shift.Identity())
	    {
	      constr_passed_in_shift = constr_passed;
	      CopyCompsToShift(curr_instr->NextAddr());
	    };
	  MoveRight();
	  goto check_tag;
	};
      // no more forks 
      if (!constr_to_shift.Identity())
	{
	  constr_passed_in_shift = constr_passed;
	  InsertCompsToShift(curr_instr->NextAddr());
	};    

      MoveRight();
      UnregFunToInsert(func.HeaderNum());
      return false;
     
    case FS_COMMAND::COMPARE_NF :
      PassCompNF(curr_instr);
      goto check_tag;

    case FS_COMMAND::SIG_FILTER :
      PassSigFilter();
      goto check_tag;

 
#ifdef DEBUG_NAMESPACE 
    default : ICP("X0"); return false;
#else
#ifdef _SUPPRESS_WARNINGS_
    default : return false;
#endif 
#endif 
    };
}; // bool FS_CODE_INTEGRATOR::ReadFunc(const TERM& func)


bool FS_CODE_INTEGRATOR::ReadVar()
{
  CALL("ReadVar()");
  FS_COMMAND* curr_instr;
  
 check_tag:
  curr_instr = *node_addr;
  switch (curr_instr->Tag())
    {
    case FS_COMMAND::VAR : 
      DOP(CheckConstrIntegrity());
      if (curr_instr->Fork()) 
	{ 
	  if (!constr_to_shift.Identity())
	    {
	      constr_passed_in_shift = constr_passed; 
	      CopyCompsToShift(curr_instr->ForkAddr()); 
	    };
	};
      ShiftSigFilterRight();
      MoveDown(); 
      return true;
      
    case FS_COMMAND::FUNC :
      // a FUNC can not precede a VAR in a tree

      if (!constr_to_shift.Identity())
	{
	  constr_passed_in_shift = constr_passed;
	  InsertCompsToShift(node_addr);
	};

      curr_instr = *node_addr;
      if ((use_sig_filters) && (sig_to_shift->nonempty()))
	{
	  if (!sig_to_shift->containsAtLeast(2)) // singletone
	    {
	      ulong hd_num;
	      if ((curr_instr->SkeletonHeaderNum(hd_num))
		  && (sig_to_shift->contains(hd_num)))
		return false;
	    }; 
	  FunSet* new_filter = sig_to_shift->copy();
	  FS_COMMAND* new_comm = FS_COMMAND::NewSIG_FILTER(new_filter);
	  new_comm->SetNext(*node_addr);
	  *node_addr = new_comm;
	};   
      return false;
       
    case FS_COMMAND::COMPARE_NF :
      PassCompNF(curr_instr);
      goto check_tag;
     
    case FS_COMMAND::SIG_FILTER :
      PassSigFilter();
      goto check_tag;
  
#ifdef DEBUG_NAMESPACE 
    default : ICP("X0"); return false;
#else
#ifdef _SUPPRESS_WARNINGS_
    default : return false;
#endif 
#endif 
    };
}; // bool FS_CODE_INTEGRATOR::ReadVar()

void FS_CODE_INTEGRATOR::ReadCompare(const Flatterm** v1,const Flatterm** v2)
{
  CALL("ReadCompare(const Flatterm** v1,const Flatterm** v2)");
  ASSERT(v1 < v2);
  ulong var_num1 = VarNum(v1);
  ulong var_num2 = VarNum(v2);
  MakeIntegrated(var_num1,var_num2);
  constr_to_insert.MakeEquivalent(var_num1,var_num2); 
  DOP(CheckConstrIntegrity());
}; // void FS_CODE_INTEGRATOR::ReadCompare(const Flatterm** v1,const Flatterm** v2)


bool FS_CODE_INTEGRATOR::ReadCollectedComps()
{
  CALL("ReadCollectedComps()");
  DOP(CheckConstrIntegrity());


  FS_COMMAND* curr_instr;
 check_instr:
  curr_instr = *node_addr;
  if (curr_instr->IsCompare() || curr_instr->IsCompareNF())
    {
      if (PassComp(curr_instr) || (*node_addr)) goto check_instr;
      DOP(CheckConstrIntegrity());
      MoveCompsToInsert(node_addr);
      constr_to_shift.Reset();
      ASSERT(!(*node_addr));
      return false;
    };
  
  // ! (curr_instr->IsCompare() || curr_instr->IsCompareNF()) 

  if (curr_instr->IsSigFilter()) { PassSigFilter(); goto check_instr; };
   
  // !(curr_instr->IsCompare() || curr_instr->IsCompareNF()) and !curr_instr->IsSigFilter() 
  
  if (constr_to_shift.Identity())
    {      
      FS_COMMAND** node_addr_backup = node_addr;
      *node_addr = 0;
      DOP(CheckConstrIntegrity());
      if (MoveCompsToInsert(node_addr))
	{
	  ASSERT(!(*node_addr));
	  ASSERT((*node_addr_backup)->IsCompareNF());
	  (*node_addr_backup)->SetTag(FS_COMMAND::COMPARE);
	  (*node_addr_backup)->SetFork(curr_instr);
	  if ((use_sig_filters) && (sig_to_shift->nonempty()))
	    { 
	      if (!sig_to_shift->containsAtLeast(2)) // singletone 
		{ 
		  ulong hd_num;
		  if ((curr_instr->SkeletonHeaderNum(hd_num))
		      && (sig_to_shift->contains(hd_num))) 
		    // no need to shift the sig. filter, the next command cancels it
		    return false;         
		};
	      // the sig. filter must be shifted
	      FunSet* new_filter = sig_to_shift->copy();
	      FS_COMMAND* new_comm = FS_COMMAND::NewSIG_FILTER(new_filter);           
	      new_comm->SetNext(curr_instr);
	      (*node_addr_backup)->SetFork(new_comm);
	    };
	  return false;
	}
      else // !MoveCompsToInsert(node_addr) (did not insert anything)
	{
	  ASSERT(node_addr == node_addr_backup);
	  ASSERT(!(*node_addr));
	  *node_addr = curr_instr;          
	  return true; 
	};
    }
  else // !constr_to_shift.Identity(), simple case
    {    
      constr_passed_in_shift = constr_passed;
      InsertCompsToShift(node_addr);
      ShiftSigFilterDown();
      ASSERT((*node_addr)->IsCompareNF());    
      (*node_addr)->SetTag(FS_COMMAND::COMPARE);       
      ASSERT((!constr_integrated.Equivalent(VarNum((*node_addr)->Var1()),VarNum((*node_addr)->Var2()))));
      MoveRight();
      MoveCompsToInsert(node_addr);
      ASSERT(!(*node_addr));
      return false; 
    };

}; // bool FS_CODE_INTEGRATOR::ReadCollectedComps()



inline void FS_CODE_INTEGRATOR::CollectSignature(const Clause* cl)
{
  for(LiteralList::Iterator iter(cl->LitList());iter.notEnd();iter.next())
    CollectSignature(iter.currentElement()->literal());
}; // void FS_CODE_INTEGRATOR::CollectSignature(const Clause* cl)


inline void FS_CODE_INTEGRATOR::CollectSignature(const TERM* lit)
{
  TERM header = *lit;
  if (header.IsEquality()) header.MakeUnordered();
  RegFunToInsert(header.HeaderNum()); 
  TERM::Iterator iter;
  iter.Reset(lit); 
  while (iter.Next()) 
    {
      header = iter.symbolRef();
      if (header.isComplex()) RegFunToInsert(header.HeaderNum());        
    };
}; // void FS_CODE_INTEGRATOR::CollectSignature(const TERM* lit)

inline void FS_CODE_INTEGRATOR::RegFunToInsert(ulong fun_num)
{
  if (!sig_to_insert->write(fun_num))
    {
      FunSet* new_sig = sig_to_insert->add(fun_num);
      delete sig_to_insert;
      sig_to_insert = new_sig;
    };
}; // void FS_CODE_INTEGRATOR::RegFunToInsert(ulong fun_num)

inline void FS_CODE_INTEGRATOR::UnregFunToInsert(ulong fun_num)
{
  if (use_sig_filters)
    {
      if (!sig_to_insert->erase(fun_num))
	{
	  FunSet* new_sig = sig_to_insert->remove(fun_num);
	  delete sig_to_insert;
	  sig_to_insert = new_sig;
	};
    };
}; // void FS_CODE_INTEGRATOR::UnregFunToInsert(ulong fun_num)

inline void FS_CODE_INTEGRATOR::UnregFunToShift(ulong fun_num)
{
  if (use_sig_filters)
    {
      if (!sig_to_shift->erase(fun_num))
	{
	  FunSet* new_sig = sig_to_shift->remove(fun_num);
	  delete sig_to_shift;
	  sig_to_shift = new_sig;
	};
    };
}; // void FS_CODE_INTEGRATOR::UnregFunToShift(ulong fun_num)


inline void FS_CODE_INTEGRATOR::EnterWriteMode()
{
  CALL("EnterWriteMode()");
  // must be used if ReadVar or ReadFunc failed
  ASSERT(constr_to_shift.Identity());

  ASSERT(constr_to_insert.Implies(constr_integrated));
  ASSERT(constr_passed.Implies(constr_integrated));
  ASSERT(constr_passed.Implies(constr_old));

  //constr_to_insert.Subtract(constr_passed);
    

  if ((use_sig_filters) && (sig_to_insert->nonempty()))
    {
      if ((!sig_to_insert->containsAtLeast(2)) && (constr_to_insert.Identity()))
	{
	  COP("END0");
	  ASSERT(!(*node_addr)); 
	  return; // nothing to insert
	};
      FS_COMMAND* filter = FS_COMMAND::NewSIG_FILTER(sig_to_insert->copy());
      filter->SetNext(*node_addr);   
      *node_addr = filter;
      node_addr = filter->NextAddr();
    };   

  MoveCompsToInsert(node_addr);
  COP("END1");
  ASSERT(!(*node_addr));
}; // void FS_CODE_INTEGRATOR::EnterWriteMode()



inline void FS_CODE_INTEGRATOR::MakeShifted(const ulong& v1,const ulong& v2) 
{
  CALL("MakeShifted(const ulong& v1,const ulong& v2)");
  ASSERT(v1 < v2);
  ASSERT(!constr_integrated.Equivalent(v1,v2));
  constr_to_shift.MakeEquivalent(v1,v2);
}; // void FS_CODE_INTEGRATOR::MakeShifted(const ulong& v1,const ulong& v2) 

inline void FS_CODE_INTEGRATOR::MakeIntegrated(const ulong& v1,const ulong& v2) 
{
  CALL("MakeIntegrated(const ulong& v1,const ulong& v2)");
  ASSERT(v1 < v2);
  ASSERT(!constr_integrated.Equivalent(v1,v2));
  ASSERT(!constr_passed.Equivalent(v1,v2));
  ASSERT(!constr_old.Equivalent(v1,v2));
  constr_integrated.MakeEquivalent(v1,v2);
}; // void FS_CODE_INTEGRATOR::MakeIntegrated(const ulong& v1,const ulong& v2)

#ifdef DEBUG_NAMESPACE
inline void FS_CODE_INTEGRATOR::CheckConstrIntegrity()
{ 
  CALL("CheckConstrIntegrity()");
  ASSERT(constr_to_insert.Implies(constr_integrated));      
  ASSERT(constr_passed.Implies(constr_integrated));   
  ASSERT(constr_to_shift.Implies(constr_old));        
  ASSERT(constr_passed.Implies(constr_old));
  static FS_EQ_CONSTR debugER("debugER");  
  debugER.MakeComposition(constr_to_shift,constr_passed);
  ASSERT(constr_old.Implies(debugER));
  debugER.MakeComposition(constr_to_insert,constr_passed);
  ASSERT(constr_integrated.Implies(debugER));
}; //void FS_CODE_INTEGRATOR::CheckConstrIntegrity() 
#endif 

inline void FS_CODE_INTEGRATOR::PassCompNF(FS_COMMAND* instr)
{
  CALL("PassCompNF(FS_COMMAND* instr)");
  DOP(CheckConstrIntegrity());
  ASSERT(instr->IsCompareNF());
  const Flatterm** var1 = instr->Var1();
  const Flatterm** var2 = instr->Var2();
  const ulong var_num1 = VarNum(var1);
  const ulong var_num2 = VarNum(var2);

  ASSERT(!(instr->Fork()));
  ASSERT(!constr_to_shift.Equivalent(var_num1,var_num2));
  ASSERT(!constr_old.Equivalent(var_num1,var_num2));
  ASSERT(!constr_passed.Equivalent(var_num1,var_num2));
   
  bool shared = constr_integrated.Equivalent(var_num1,var_num2);
    
  if (shared)
    {
      // the current instruction is shared
      constr_passed.MakeEquivalent(var_num1,var_num2);
      node_addr = instr->NextAddr();
    }
  else // ! constr_integrated.Equivalent(var_num1,var_num2)
    {
      // the instruction itself must be shifted unless it can be replaced by an equivalent one 
      *node_addr = instr->Next();
      delete instr;
    };

  instr = *node_addr;
  FS_EQ_CONSTR::MergedPairs mergedPairs(constr_old,var_num1,var_num2);
  ulong merged1;
  ulong merged2;

  while (mergedPairs.nextPair(merged1,merged2))
    {
      ASSERT(merged1 < merged2);     
      ASSERT(!constr_old.Equivalent(merged1,merged2));
      if (constr_integrated.Equivalent(merged1,merged2) && (!constr_passed.Equivalent(merged1,merged2)))
	{
	  // another COMPARE can be shared	 
	  shared = true;
	  constr_passed.MakeEquivalent(merged1,merged2);
	  *node_addr = FS_COMMAND::NewCOMPARE_NF(Var(merged1),Var(merged2));
	  (*node_addr)->SetNext(instr);
	  MoveDown();
	};
    };    
  
  constr_old.MakeEquivalent(var_num1,var_num2);
  if (!shared) MakeShifted(var_num1,var_num2);
  ASSERT((*node_addr) == instr);
  DOP(CheckConstrIntegrity());
}; // void FS_CODE_INTEGRATOR::PassCompNF(FS_COMMAND* instr) 

inline bool FS_CODE_INTEGRATOR::PassComp(FS_COMMAND* instr)
{
  CALL("PassComp(FS_COMMAND* instr)");
  DOP(CheckConstrIntegrity()); 
  ASSERT(instr->IsCompareNF() ||instr->IsCompare() );
  const Flatterm** var1 = instr->Var1();
  const Flatterm** var2 = instr->Var2();
  ulong var_num1 = VarNum(var1);
  ulong var_num2 = VarNum(var2);
  ASSERT(!constr_to_shift.Equivalent(var_num1,var_num2));
  ASSERT(!constr_old.Equivalent(var_num1,var_num2)); 
  ASSERT(!constr_passed.Equivalent(var_num1,var_num2));

  ASSERT((*node_addr) == instr);

  if (constr_integrated.Equivalent(var_num1,var_num2))
    {
      goto shift_and_insert_shared;      
    }
  else // try to replace this COMPARE by an equivalent one which is also in constr_integrated
    {
      FS_EQ_CONSTR::MergedPairs mergedPairs(constr_old,var_num1,var_num2);
      ulong merged1;
      ulong merged2; 
      while (mergedPairs.nextPair(merged1,merged2))
	{
	  ASSERT(merged1 < merged2);     
	  ASSERT(!constr_old.Equivalent(merged1,merged2));
	  if (constr_integrated.Equivalent(merged1,merged2) && (!constr_passed.Equivalent(merged1,merged2)))
	    {
	      // appropriate replacement found
	      var_num1 = merged1;
	      var_num2 = merged2;
	      // modify the arguments of instr
	      instr->SetVar1(Var(var_num1));
	      instr->SetVar2(Var(var_num2));         
	      goto shift_and_insert_shared;
	    };
	};
      
      // no appropriate replacement found, must follow the fork

      if (instr->IsCompareNF()) instr->SetTag(FS_COMMAND::COMPARE); 
      if (!constr_to_shift.Identity())
	{
	  constr_passed_in_shift = constr_passed;
	  constr_passed_in_shift.MakeEquivalent(var_num1,var_num2);
	  CopyCompsToShift(instr->NextAddr());
	};
      ShiftSigFilterDown();
      MoveRight();  
      DOP(CheckConstrIntegrity());
      return false;
    };

 shift_and_insert_shared:
  COP("shift_and_insert_shared");
  // make necessary shifts into the fork
  if (instr->Fork()) 
    {
      if (!constr_to_shift.Identity())
        {
	  constr_passed_in_shift = constr_passed; 
	  CopyCompsToShift(instr->ForkAddr());  
        };      
      ShiftSigFilterRight();
    };
  constr_passed.MakeEquivalent(var_num1,var_num2);
  MoveDown();
  instr = *node_addr; 
  // now find all COMPARES that can be shared due to (var_num1 ~ var_num2) added to constr_old
 
    
  FS_EQ_CONSTR::MergedPairs mergedPairs(constr_old,var_num1,var_num2);
  ulong merged1;
  ulong merged2; 

  while (mergedPairs.nextPair(merged1,merged2))
    {
      ASSERT(merged1 < merged2);     
      ASSERT(!constr_old.Equivalent(merged1,merged2));

      if (constr_integrated.Equivalent(merged1,merged2) && (!constr_passed.Equivalent(merged1,merged2)))
        {
	  // another COMPARE can be shared	
	  constr_passed.MakeEquivalent(merged1,merged2);
	  *node_addr = FS_COMMAND::NewCOMPARE_NF(Var(merged1),Var(merged2));
	  (*node_addr)->SetNext(instr);
	  MoveDown();
        };
    };    

  ASSERT((*node_addr) == instr);

  constr_old.MakeEquivalent(var_num1,var_num2); 

  COP("end");
  DOP(CheckConstrIntegrity());     
  return true;
}; // bool FS_CODE_INTEGRATOR::PassComp(FS_COMMAND* instr)

inline void FS_CODE_INTEGRATOR::CopyCompsToShift(FS_COMMAND** addr)
{
  CALL("CopyCompsToShift(FS_COMMAND** addr)");
  DOP(bool debugAtLeastOneShifted = constr_to_shift.Identity());  
  ASSERT(constr_passed.Implies(constr_passed_in_shift));
  ulong vn1;
  ulong vn2;
  FS_EQ_CONSTR::Iterator iter(constr_to_shift);
  FS_COMMAND* old_instr = *addr;
  while (iter.Next(vn2,vn1)) 
    {
      ASSERT(vn1 < vn2); 
      ASSERT(!constr_integrated.Equivalent(vn1,vn2));
      ASSERT(constr_old.Equivalent(vn1,vn2));
      
      if (!constr_passed_in_shift.Equivalent(vn1,vn2))
	{
	  DOP(debugAtLeastOneShifted = true);  
	  constr_passed_in_shift.MakeEquivalent(vn1,vn2);
	  FS_COMMAND* new_instr = FS_COMMAND::NewCOMPARE_NF(Var(vn1),Var(vn2));
	  *addr = new_instr;
	  addr = new_instr->NextAddr();
        }; 
    };
  *addr = old_instr;
  ASSERT(debugAtLeastOneShifted);
}; // void FS_CODE_INTEGRATOR::CopyCompsToShift(FS_COMMAND** addr)


inline void FS_CODE_INTEGRATOR::InsertCompsToShift(FS_COMMAND** addr)
{
  CALL("InsertCompsToShift(FS_COMMAND** addr)");
  DOP(bool debugAtLeastOneShifted = constr_to_shift.Identity());  
  DOP(CheckConstrIntegrity());     
  ASSERT(constr_passed.Implies(constr_passed_in_shift));
  ulong vn1;
  ulong vn2;
  FS_COMMAND* old_instr = *addr;
  while (constr_to_shift.Split(vn2,vn1)) 
    { 
      ASSERT(vn1 < vn2);
      ASSERT(!constr_integrated.Equivalent(vn1,vn2));
      ASSERT(constr_old.Equivalent(vn1,vn2));

      if (!constr_passed_in_shift.Equivalent(vn1,vn2))
	{
	  DOP(debugAtLeastOneShifted = true);  
	  constr_passed_in_shift.MakeEquivalent(vn1,vn2);
	  FS_COMMAND* new_instr = FS_COMMAND::NewCOMPARE_NF(Var(vn1),Var(vn2));
	  *addr = new_instr;
	  addr = new_instr->NextAddr(); 
        };
    };
  ASSERT(constr_to_shift.Identity());
  *addr = old_instr;
  ASSERT(debugAtLeastOneShifted);
}; // void FS_CODE_INTEGRATOR::InsertCompsToShift(FS_COMMAND** addr)


inline bool FS_CODE_INTEGRATOR::MoveCompsToInsert(FS_COMMAND**& addr)
{
  CALL("MoveCompsToInsert(FS_COMMAND**& addr)");
  ulong vn1;
  ulong vn2;
  FS_COMMAND* old_instr = *addr;
  bool res = false;
  while (constr_to_insert.Split(vn2,vn1)) 
    { 
      ASSERT(constr_integrated.Equivalent(vn1,vn2));
      ASSERT(vn1 < vn2);  
      if (!constr_passed.Equivalent(vn1,vn2))
	{
	  ASSERT(!constr_old.Equivalent(vn1,vn2));
	  constr_passed.MakeEquivalent(vn1,vn2);
	  FS_COMMAND* new_instr = FS_COMMAND::NewCOMPARE_NF(Var(vn1),Var(vn2));
	  *addr = new_instr;
	  addr = new_instr->NextAddr();
	  res = true;
        };
    };
  ASSERT(constr_to_insert.Identity());
  *addr = old_instr;

  COP("end");
  ASSERT(constr_to_insert.Identity());
  ASSERT(constr_passed.Implies(constr_integrated));
  ASSERT(constr_integrated.Implies(constr_passed));
  return res;
}; // bool FS_CODE_INTEGRATOR::MoveCompsToInsert(FS_COMMAND**& addr)
   


inline void FS_CODE_INTEGRATOR::PassSigFilter()
{
  CALL("PassSigFilter()");
  ASSERT(CurrInstr()->IsSigFilter());
  ASSERT(!CurrInstr()->Fork()); 

  FS_COMMAND* curr_comm = *node_addr;
  FunSet* curr_filter = curr_comm->Functors();
     
  FunSet* intersection = sig_to_insert->intersection(*curr_filter);
  if (intersection->nonempty()) 
    {         
      curr_comm->SetFunctors(intersection);
      node_addr = curr_comm->NextAddr();
     
      FunSet* diff_to_shift = curr_filter->difference(*intersection);
      delete curr_filter;
      FunSet* new_sig_to_shift = sig_to_shift->sum(*diff_to_shift);
      delete diff_to_shift;
      delete sig_to_shift;
      sig_to_shift = new_sig_to_shift;
       
      FunSet* new_sig_to_insert = sig_to_insert->difference(*intersection);
      delete sig_to_insert;
      sig_to_insert = new_sig_to_insert;
    }
  else // intersection->empty()
    {
      delete intersection;
      *node_addr = curr_comm->Next();               
      FunSet* new_sig_to_shift = sig_to_shift->sum(*curr_filter);
      delete sig_to_shift;
      sig_to_shift = new_sig_to_shift;
      delete curr_comm;  
    };
}; // void FS_CODE_INTEGRATOR::PassSigFilter()    

inline void FS_CODE_INTEGRATOR::ShiftSigFilterRight()
{
  FS_COMMAND* fork = (*node_addr)->Fork();
  if ((fork) 
      && (use_sig_filters) 
      && (sig_to_shift->nonempty())) 
    {
      if (!sig_to_shift->containsAtLeast(2)) // singletone
	{
	  ulong hd_num;
	  if ((fork->SkeletonHeaderNum(hd_num)) 
	      && (sig_to_shift->contains(hd_num)))
	    return; // there is no sense to insert the filter  
        };
      FS_COMMAND* filter = FS_COMMAND::NewSIG_FILTER(sig_to_shift->copy());
      filter->SetNext(fork);
      (*node_addr)->SetFork(filter);
    };
}; // void FS_CODE_INTEGRATOR::ShiftSigFilterRight()
   
inline void FS_CODE_INTEGRATOR::ShiftSigFilterDown()
{
  CALL("ShiftSigFilterDown()");
  if (use_sig_filters)
    {
      FS_COMMAND* curr_comm = *node_addr;
      ulong hd_num;
      FunSet* new_filter;
      if (curr_comm->SkeletonHeaderNum(hd_num)) { new_filter = sig_to_shift->remove(hd_num); }
      else new_filter = sig_to_shift->copy();
      if (new_filter->nonempty())
        {
	  if (!new_filter->containsAtLeast(2)) // singletone
	    {
	      if ((curr_comm->Next()->SkeletonHeaderNum(hd_num)) 
		  && (new_filter->contains(hd_num)))
		{ // there is no sense to insert the filter  
		  delete new_filter;
		  return;
		};
	    };
	  FS_COMMAND* new_comm = FS_COMMAND::NewSIG_FILTER(new_filter);
	  new_comm->SetNext(curr_comm->Next());
	  curr_comm->SetNext(new_comm);
        }
      else delete new_filter;
    };
}; //  void FS_CODE_INTEGRATOR::ShiftSigFilterDown() 
    
inline void FS_CODE_INTEGRATOR::InsertVar(const Flatterm** v)
{
  FS_COMMAND* tmp = *node_addr;
  *node_addr = FS_COMMAND::NewVAR(v);
  (*node_addr)->SetFork(tmp);
  MoveDown();
}; // void FS_CODE_INTEGRATOR::InsertVar(const Flatterm** v)



#ifndef NO_DEBUG_VIS
ostream& FS_CODE_INTEGRATOR::outputComps(ostream& str) 
{
  constr_integrated.Normalize();
  constr_old.Normalize();
  constr_passed.Normalize();
  constr_to_shift.Normalize();
  constr_to_insert.Normalize();
  return str << constr_integrated << '\n'
	     << constr_old << '\n'
	     << constr_passed << '\n'
	     << constr_to_shift << '\n'
	     << constr_to_insert << '\n';      

}; // ostream& FS_CODE_INTEGRATOR::outputComps(ostream& str) 
#endif

void FS_CODE_INTEGRATOR::AccidForwardSubsumed(Clause* subsumed,Clause* subsuming)
{
  // temporary
#ifdef DEBUG_NAMESPACE     
  DWARN("  Clause ");       
  DWARN(subsumed);
  DWARN("  being integrated for forward subsumption");
  DWARN("  was subsumed by already integrated one:");
  DWARN(subsuming);
#else 
#ifdef _SUPPRESS_WARNINGS_
  Clause* dummy = subsumed; dummy = subsuming; 
#endif   
#endif     
}; // void FS_CODE_INTEGRATOR::AccidForwardSubsumed(Clause* subsumed,Clause* subsuming)


//================================================
#undef DEBUG_NAMESPACE
#ifdef DEBUG_FS_CODE
#define DEBUG_NAMESPACE "FS_CODE_REMOVAL"
#endif
#include "debugMacros.hpp"
//=================================================

FS_CODE_REMOVAL::FS_CODE_REMOVAL(const Flatterm** subst)
  : constr(), 
  fs_subst(subst),
  vars(),
  use_sig_filters(true) // default
{
  CALL("constructor FS_CODE_REMOVAL(const Flatterm** subst)");
};

FS_CODE_REMOVAL::~FS_CODE_REMOVAL() { CALL("destructor ~FS_CODE_REMOVAL()"); };

void FS_CODE_REMOVAL::init(const Flatterm** subst)
{
  CALL("init(const Flatterm** subst)");
  constr.init();
  fs_subst = subst;
  vars.init();
  use_sig_filters = true;
}; // void FS_CODE_REMOVAL::init(const Flatterm** subst)

void FS_CODE_REMOVAL::destroy()
{
  CALL("destroy()");
  vars.destroy();
  constr.destroy();
}; // void FS_CODE_REMOVAL::destroy()


bool FS_CODE_REMOVAL::SkipComplexTerm(TERM* t)
{
  TERM::Iterator iter;
  iter.Reset(t);
 skip_sym:
  if (iter.CurrentSymIsVar())
    {
      if (!SkipVar(iter.Symbol().var())) return false;
    }
  else // functor
    {
      if (!(SkipFunc(iter.Symbol()))) return false;
    };
  if (iter.Next()) goto skip_sym; 
  return true; 
}; // bool FS_CODE_REMOVAL::SkipComplexTerm(TERM* t)

bool FS_CODE_REMOVAL::SkipEqLiteral(TERM* lit)
{
  ASSERT(lit->IsEquality());               
  if (lit->IsOrderedEq())
    {
      if (!(SkipOrdEq(*lit))) return false;
    }
  else
    {
      if (!(SkipUnordEq(*lit))) return false;
    }; 
  TERM* args = lit->Args();
  TERM arg1 = *args;
  TERM arg2 = *(args->Next());
  
  return (SkipTerm(arg1)) && (SkipSecondEqArg()) && (SkipTerm(arg2));
  
}; // bool FS_CODE_REMOVAL::SkipEqLiteral(TERM* lit)


bool FS_CODE_REMOVAL::SkipLiteral(TERM* lit)
{
  if (lit->IsEquality()) return SkipEqLiteral(lit);
 
  TERM::Iterator iter;
  TERM sym;
  
  if (lit->arity())
    {
      if (SkipHeader(*lit))
	{
	  iter.Reset(lit);
	next_sym:
	  if (!(iter.Next())) return true;
	  sym = iter.Symbol();
	  if (iter.CurrentSymIsVar())
	    {
	      if (!SkipVar(sym.var())) return false;
	    }
	  else // functor
	    {
	      if (!(SkipFunc(sym))) return false;
	    };
	  goto next_sym;   
	}
      else return false;
    }
  else // propositional literal
    {
      return SkipPropLit(*lit);
    };
}; // bool FS_CODE_REMOVAL::SkipLiteral(TERM* lit)







//===========================================================================
