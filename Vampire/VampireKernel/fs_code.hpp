//
// File:         fs_code.hpp
// Description:  Maintainence of index for FS.
// Created:      Oct 15, 1999, 15:00
// Author:       Alexandre Riazanov
// mail:         riazanov@cs.man.ac.uk
//====================================================
#ifndef FS_CODE_H
//===================================================================
#define FS_CODE_H
#ifndef NO_DEBUG_VIS
 #include <iostream>
#endif
#include "jargon.hpp"
#include "VampireKernelDebugFlags.hpp"
#include "VampireKernelConst.hpp"
#include "MultisetOfVariables.hpp"
#include "EquivalenceRelation.hpp"
#include "fs_command.hpp"
#include "old_fs_code.hpp"
#include "TmpLitList.hpp"
//================================================
#undef DEBUG_NAMESPACE
#ifdef DEBUG_FS_CODE
 #define DEBUG_NAMESPACE "FS_CODE_INTEGRATOR"
#endif
#include "debugMacros.hpp"
//=================================================
namespace VK
{

class TERM;
class Clause;
class Flatterm;

typedef BK::EquivalenceRelation<VampireKernelConst::MaxClauseSize> FS_EQ_CONSTR; 

class FS_CODE_INTEGRATOR // must be static - too costly constructor
{  
 private:
  Clause* new_clause;
  FS_COMMAND** node_addr;
   
  FS_EQ_CONSTR constr_to_insert;
  FS_EQ_CONSTR constr_to_shift;
  FS_EQ_CONSTR constr_passed;
  FS_EQ_CONSTR constr_passed_in_shift; 
  FS_EQ_CONSTR constr_integrated;
  FS_EQ_CONSTR constr_old;  

   
  DOP(ulong debugLitNum);
   
  const Flatterm** fs_subst;
  
  const Flatterm** first_occ[VampireKernelConst::MaxNumOfVariables];
  BK::MultisetOfVariables<VampireKernelConst::MaxNumOfVariables> vars; 
  const Flatterm** fs_var;
  bool use_sig_filters;
  FunSet* sig_to_insert;
  FunSet* sig_to_shift;
 public:
  FS_CODE_INTEGRATOR(const Flatterm** subst);
  ~FS_CODE_INTEGRATOR();
  void init(const Flatterm** subst);
  void destroy();


  void SetUseSigFilters(bool fl) { use_sig_filters = fl; };
  void Reset(FS_COMMAND** n,Clause* cl) 
    {
      new_clause = cl; 
      node_addr = n; 
      constr_to_insert.Reset();
      constr_to_shift.Reset();
      constr_passed.Reset();
      //constr_passed_in_shift.Reset();
      constr_integrated.Reset();
      constr_old.Reset();
      DOP(debugLitNum = 0);
      vars.Reset();
      fs_var = fs_subst;
      if (use_sig_filters) 
	{
	  if (sig_to_insert) delete sig_to_insert;
	  sig_to_insert = new FunSet();
	  if (sig_to_shift) delete sig_to_shift;
	  sig_to_shift = new FunSet();       
	  CollectSignature(cl);       
	};
    };

  const Flatterm** mapVar(ulong var) const { return first_occ[var]; };

 private:
  void CollectSignature(const Clause* cl);
  void CollectSignature(const TERM* lit);
  void RegFunToInsert(ulong fun_num);
  void UnregFunToInsert(ulong fun_num);
  void UnregFunToShift(ulong fun_num);
  void MoveDown() { node_addr = (*node_addr)->NextAddr(); };
  void MoveRight() { node_addr = (*node_addr)->ForkAddr(); };
  ulong VarNum(const Flatterm** v) const { return v - fs_subst; };
  const Flatterm** Var(const ulong& var_num) const { return fs_subst + var_num; };

  inline void MakeShifted(const ulong& v1,const ulong& v2);

  inline void MakeIntegrated(const ulong& v1,const ulong& v2);

#ifdef DEBUG_NAMESPACE
  inline void CheckConstrIntegrity();
#endif 

  inline void PassCompNF(FS_COMMAND* instr);
  inline bool PassComp(FS_COMMAND* instr);

  inline void CopyCompsToShift(FS_COMMAND** addr);

  inline void InsertCompsToShift(FS_COMMAND** addr);


  inline bool MoveCompsToInsert(FS_COMMAND**& addr);


  inline void PassSigFilter();

  inline void ShiftSigFilterRight();
   
  inline void ShiftSigFilterDown();
    
  inline void InsertVar(const Flatterm** v);

 public:
  // Read mode
    
  bool ReadHeader(const TERM& header);
  bool ReadPropLit(const TERM& header);
  bool ReadUnordEq(const TERM& header);
  bool ReadOrdEq(TERM header);
  void ReadSecondEqArg();
  bool ReadFunc(const TERM& func);
  bool ReadVar();
  bool ReadVar(const ulong& var);
  void ReadCompare(const Flatterm** v1,const Flatterm** v2);
  bool ReadCollectedComps();
  bool ReadLiteral(TERM* lit);
  bool ReadEqLiteral(TERM* lit); 
  bool ReadComplexTerm(TERM* t);
  bool ReadTerm(const TERM& t)
    {
      CALL("ReadTerm(const TERM& t)");
      return (t.isVariable()) ? ReadVar(t.var()) : ReadComplexTerm(t.First());
    };
   
  // Write mode
    
  void WriteLiteral(TERM* lit);
  void WriteEqLiteral(TERM* lit);
   
  void EnterWriteMode();
   
  void WriteHeader(TERM header) 
    { 
      *node_addr = FS_COMMAND::NewLIT_HEADER(header);
      MoveDown();
    }; 
  void WritePropLit(TERM header)
    {
      *node_addr = FS_COMMAND::NewPROP_LIT(header);
      MoveDown();
    }; 
  void WriteUnordEq(TERM header)
    {
      *node_addr = FS_COMMAND::NewUNORD_EQ(header);
      MoveDown();
    };
  void WriteOrdEq(TERM header)
    {
      *node_addr = FS_COMMAND::NewORD_EQ(header);
      MoveDown();
    };
  void WriteSecondEqArg()
    {
      *node_addr = FS_COMMAND::NewSECOND_EQ_ARG();
      MoveDown();
    }; 
    
  void WriteFunc(TERM func)
    {
      *node_addr = FS_COMMAND::NewFUNC(func);
      MoveDown();
    }; 
   
  void InsertFunc(TERM func)
    {
      FS_COMMAND* tmp = *node_addr;
      *node_addr = FS_COMMAND::NewFUNC(func);
      (*node_addr)->SetFork(tmp);
      MoveDown();
    };
    
  void WriteVar(const Flatterm** v)
    {
      *node_addr = FS_COMMAND::NewVAR(v);
      MoveDown();
    };  
   
  void WriteVar(ulong var)
    {
      WriteVar(fs_var);
      if (vars.Occurences(var))
	{
	  WriteCompare(first_occ[var],fs_var);
	}
      else // first occurence
	{
	  vars.Register(var);
	  first_occ[var] = fs_var;
	}; 
      fs_var++;
    };
   
  void WriteComplexTerm(TERM* t)
    {
      TERM::Iterator iter;
      iter.Reset(t);
     
    write_curr_sym:
      if (iter.CurrentSymIsVar())
	{
	  WriteVar(iter.Symbol().var());
	}
      else // functor
	{
	  WriteFunc(iter.Symbol());
	};
      if (iter.Next()) goto write_curr_sym;
    }; 
    
  void WriteTerm(TERM t)
    {
      if (t.isVariable()) { WriteVar(t.var()); }
      else WriteComplexTerm(t.First());
    };
    
  void WriteCompare(const Flatterm** v1,const Flatterm** v2)
    {
      CALL("WriteCompare(const Flatterm** v1,const Flatterm** v2)");
      DOP(MakeIntegrated(VarNum(v1),VarNum(v2)));
      *node_addr = FS_COMMAND::NewCOMPARE_NF(v1,v2);
      MoveDown(); 
    };
  void WriteSuccess(Clause* cl)
    {
      *node_addr = FS_COMMAND::NewSUCCESS(cl);
    };
  void InsertSuccess(Clause* cl)
    {
      FS_COMMAND* tmp = *node_addr;
      *node_addr = FS_COMMAND::NewSUCCESS(cl);
      (*node_addr)->SetFork(tmp);
    };
 public: // for debug
#ifndef NO_DEBUG
  const FS_COMMAND* CurrInstr() const { return *node_addr; };  
#endif

#ifndef NO_DEBUG_VIS
  ostream& outputComps(ostream& str); 
#endif

 private: 
 
  // Processing clauses that are accidentally forward subsumed
  void AccidForwardSubsumed(Clause* subsumed,Clause* subsuming);
     
}; // class FS_CODE_INTEGRATOR

}; // namespace VK
//================================================
#undef DEBUG_NAMESPACE
#ifdef DEBUG_FS_CODE
#define DEBUG_NAMESPACE "FS_CODE_REMOVAL"
#endif
#include "debugMacros.hpp"
//================================================= 
namespace VK
{
class FS_CODE_REMOVAL
{
 private:
  Clause* removed_clause;
  FS_COMMAND** node_addr;
  FS_COMMAND** branch_to_destroy;
   
  FS_COMMAND* comps; // list of instructions, can be changed to a Stack
   
  FS_EQ_CONSTR constr;
   
  const Flatterm** fs_subst;
  
  const Flatterm** first_occ[VampireKernelConst::MaxNumOfVariables];
  BK::MultisetOfVariables<VampireKernelConst::MaxNumOfVariables> vars; 
  const Flatterm** fs_var; 
  bool use_sig_filters;
   
 private:
  void MoveDown() { node_addr = (*node_addr)->NextAddr(); };
  void MoveRight() { node_addr = (*node_addr)->ForkAddr(); };
  ulong VarNum(const Flatterm** v) const { return v - fs_subst; };
  const Flatterm** Var(ulong var_num) const { return fs_subst + var_num; }; 
 public:
  FS_CODE_REMOVAL(const Flatterm** subst);
  ~FS_CODE_REMOVAL();
  void init(const Flatterm** subst);
  void destroy();

  void Reset(FS_COMMAND** n,Clause* cl) 
    {
      removed_clause = cl;
      node_addr = n; 
      branch_to_destroy = n; 
      comps = 0;
      constr.Reset();
      vars.Reset();
      fs_var = fs_subst;
    };
   
  void SetUseSigFilters(bool fl) { use_sig_filters = fl; };

  bool SkipComplexTerm(TERM* t);
  bool SkipLiteral(TERM* lit); 
  bool SkipEqLiteral(TERM* lit); 
   
  bool SkipTerm(TERM t) 
    {
      return (t.isVariable()) ? SkipVar(t.var()) : SkipComplexTerm(t.First());
    };
   
    
  bool SkipHeader(const TERM& header);
  bool SkipPropLit(const TERM& header);
  bool SkipUnordEq(const TERM& header);
  bool SkipOrdEq(const TERM& header);
  bool SkipSecondEqArg();
   
  bool SkipFunc(TERM func);
  bool SkipVar();
  bool SkipVar(ulong var)
    {
      if (SkipVar())
	{
	  if (vars.Occurences(var))
	    {
	      SkipCompare(first_occ[var],fs_var);
	    } 
	  else // first occurence of the variable
	    {
	      vars.Register(var);
	      first_occ[var] = fs_var;
	    }; 
	  fs_var++;
	  return true;
	}
      else return false;
    }; 
  void SkipCompare(const Flatterm** v1,const Flatterm** v2); 
  bool FindSuccess();
  void DestroyBranch() 
    {
      FS_COMMAND* tmp = *branch_to_destroy;
      *branch_to_destroy = tmp->Fork();
      tmp->SetFork(0);
      FS_COMMAND::DestroyTree(tmp);
    };
 public: // for debugging
#ifndef NO_DEBUG
  FS_COMMAND* Node() const { return *node_addr; }; 
#endif
}; // class FS_CODE_REMOVAL 

}; // namespace VK

//================================================
#undef DEBUG_NAMESPACE
#ifdef DEBUG_FS_CODE
#define DEBUG_NAMESPACE "FS_CODE"
#endif
#include "debugMacros.hpp"
//=================================================
namespace VK
{

class FS_CODE 
{
 public:
  FS_CODE(const Flatterm** sbst) ;
  ~FS_CODE();
  void init(const Flatterm** sbst);
  void destroy();


  void SetOldStyle(bool fl) { oldStyle = fl; };
  void SetUseSigFilters(bool fl)
    {
      integrator.SetUseSigFilters(fl);
      removal.SetUseSigFilters(fl);
    };
  FS_COMMAND* Tree() const 
    {
      //DF; cout << "OLD STYLE " << oldStyle << "\n";
      if (oldStyle) return oldStyleCode.theTree();
      return tree; 
    }; 
  const Flatterm** Subst() { return subst; };
  const Flatterm** mapVar(ulong var) const { return integrator.mapVar(var); };
  void Integrate(Clause* cl);
  void Integrate(TmpLitList& component,void* name)
    {
      if (component.numOfAllLiterals() > _maxlNumOfLits) 
	_maxlNumOfLits = component.numOfAllLiterals();
      oldStyleCode.integrate(component,name);
    };
  void Remove(Clause* cl);
  ulong maxlNumOfLits() const { return _maxlNumOfLits; };
 public: // for debugging
#ifndef NO_DEBUG_VIS
  ostream& outputCode(ostream& str) const
    {
      if (oldStyle) return oldStyleCode.outputCode(str);    
      if (!tree) return str << "<<< Empty FS_CODE >>>\n"; 
      return FS_COMMAND::outputTree(str,tree,subst)
	<< "<<< End of FS_CODE >>>\n";  
    }; 
#endif

 private:
  bool oldStyle;
  OLD_FS_CODE oldStyleCode;
  FS_COMMAND* tree;
  const Flatterm** subst;
  FS_CODE_INTEGRATOR integrator; // quite heavy object (slow constructor), can not be local
  FS_CODE_REMOVAL removal; // quite heavy object (slow constructor), can not be local
  ulong _maxlNumOfLits;
}; // class FS_CODE

}; // namespace VK

//================================================
#undef DEBUG_NAMESPACE
#ifdef DEBUG_FS_CODE
#define DEBUG_NAMESPACE "FS_CODE_REMOVAL"
#endif
#include "debugMacros.hpp"
//=================================================
namespace VK
{
inline bool FS_CODE_REMOVAL::SkipHeader(const TERM& header)
{
  CALL("SkipHeader(const TERM& header)");
  const Flatterm** var1;
  const Flatterm** var2;
  ulong var_num1;
  ulong var_num2;
  FS_COMMAND* curr_instr;
  ulong header_num = header.HeaderNum();
  ASSERT(*node_addr);
 check_instr:
  curr_instr = *node_addr;
  switch (curr_instr->Tag())
    {
    case FS_COMMAND::LIT_HEADER :
      if (curr_instr->HeaderNum() == header_num) 
	{
	  if (curr_instr->Fork()) branch_to_destroy = node_addr;
	  node_addr = curr_instr->NextAddr();
	  return true;
	};
    try_fork: 
      if (curr_instr->Fork()) 
        {
	  node_addr = curr_instr->ForkAddr();
	  branch_to_destroy = node_addr;
	  goto check_instr;
        }; 
      return false; 
    case FS_COMMAND::COMPARE_NF : // no break here
    case FS_COMMAND::COMPARE :
      var1 = curr_instr->Var1();
      var2 = curr_instr->Var2();
      var_num1 = VarNum(var1);
      var_num2 = VarNum(var2);
      if (constr.Equivalent(var_num1,var_num2))
	{
	  if (curr_instr->Fork()) branch_to_destroy = node_addr;
	  node_addr = curr_instr->NextAddr();
	  goto check_instr; 
	}
      else { goto try_fork; };
    
    case FS_COMMAND::SIG_FILTER :
      node_addr = curr_instr->NextAddr();
      goto check_instr;   

    default : goto try_fork;
    };
}; // bool FS_CODE_REMOVAL::SkipHeader(const TERM& header)

inline bool FS_CODE_REMOVAL::SkipPropLit(const TERM& header)
{
  CALL("SkipPropLit(const TERM& header)");
  const Flatterm** var1;
  const Flatterm** var2;
  ulong var_num1;
  ulong var_num2;
 
  FS_COMMAND* curr_instr;
  ulong header_num = header.HeaderNum();
  ASSERT(*node_addr);
 check_instr:
  curr_instr = *node_addr;
  switch (curr_instr->Tag())
    {
    case FS_COMMAND::PROP_LIT :
      if (curr_instr->HeaderNum() == header_num) 
	{
	  if (curr_instr->Fork()) branch_to_destroy = node_addr;
	  node_addr = curr_instr->NextAddr();
	  return true;
	};
    try_fork: 
      if (curr_instr->Fork()) 
        {
	  node_addr = curr_instr->ForkAddr();
	  branch_to_destroy = node_addr;
	  goto check_instr;
        }; 
      return false; 

    case FS_COMMAND::COMPARE_NF : // no break here
    case FS_COMMAND::COMPARE :
      var1 = curr_instr->Var1();
      var2 = curr_instr->Var2();
      var_num1 = VarNum(var1);
      var_num2 = VarNum(var2);
      
      if (constr.Equivalent(var_num1,var_num2))
	{
	  if (curr_instr->Fork()) branch_to_destroy = node_addr;
	  node_addr = curr_instr->NextAddr();
	  goto check_instr; 
	}
      else { goto try_fork; };
     
    case FS_COMMAND::SIG_FILTER :
      node_addr = curr_instr->NextAddr();
      goto check_instr;   

    default : goto try_fork;
    };
}; // bool FS_CODE_REMOVAL::SkipPropLit(const TERM& header)

inline bool FS_CODE_REMOVAL::SkipUnordEq(const TERM& header)
{
  CALL("SkipUnordEq(const TERM& header)");
  const Flatterm** var1;
  const Flatterm** var2;
  ulong var_num1;
  ulong var_num2;
  FS_COMMAND* curr_instr;
  ulong header_num = header.HeaderNum();
  ASSERT(*node_addr);
 check_instr:
  curr_instr = *node_addr;
  switch (curr_instr->Tag())
    {
    case FS_COMMAND::UNORD_EQ :
      if (curr_instr->HeaderNum() == header_num) 
	{
	  if (curr_instr->Fork()) branch_to_destroy = node_addr;
	  node_addr = curr_instr->NextAddr();
	  return true;
	};
    try_fork: 
      if (curr_instr->Fork()) 
        {
	  node_addr = curr_instr->ForkAddr();
	  branch_to_destroy = node_addr;
	  goto check_instr;
        }; 
      return false; 
  
    case FS_COMMAND::COMPARE_NF : // no break here
    case FS_COMMAND::COMPARE :
      var1 = curr_instr->Var1();
      var2 = curr_instr->Var2();
      var_num1 = VarNum(var1);
      var_num2 = VarNum(var2); 
      if (constr.Equivalent(var_num1,var_num2))
	{
	  if (curr_instr->Fork()) branch_to_destroy = node_addr;
	  node_addr = curr_instr->NextAddr();
	  goto check_instr; 
	}
      else { goto try_fork; };
    
    case FS_COMMAND::SIG_FILTER :
      node_addr = curr_instr->NextAddr();
      goto check_instr;    

    default : goto try_fork;
    };
}; // bool FS_CODE_REMOVAL::SkipUnordEq(const TERM& header)


inline bool FS_CODE_REMOVAL::SkipOrdEq(const TERM& header)
{
  CALL("SkipOrdEq(const TERM& header)");
  const Flatterm** var1;
  const Flatterm** var2;
  ulong var_num1;
  ulong var_num2;
  FS_COMMAND* curr_instr;
  ulong header_num = header.HeaderNum();
  ASSERT(*node_addr);
 check_instr:
  curr_instr = *node_addr;
  switch (curr_instr->Tag())
    {
    case FS_COMMAND::ORD_EQ :
      if (curr_instr->HeaderNum() == header_num) 
	{
	  if (curr_instr->Fork()) branch_to_destroy = node_addr;
	  node_addr = curr_instr->NextAddr();
	  return true;
	};
    try_fork: 
      if (curr_instr->Fork()) 
        {
	  node_addr = curr_instr->ForkAddr();
	  branch_to_destroy = node_addr;
	  goto check_instr;
        }; 
      return false; 

    case FS_COMMAND::COMPARE_NF : // no break here
    case FS_COMMAND::COMPARE :
      var1 = curr_instr->Var1();
      var2 = curr_instr->Var2();
      var_num1 = VarNum(var1);
      var_num2 = VarNum(var2);
      if (constr.Equivalent(var_num1,var_num2))
	{
	  if (curr_instr->Fork()) branch_to_destroy = node_addr;
	  node_addr = curr_instr->NextAddr();
	  goto check_instr; 
	}
      else { goto try_fork; };

    case FS_COMMAND::SIG_FILTER :
      node_addr = curr_instr->NextAddr();
      goto check_instr;   

    default : goto try_fork;
    };
}; // bool FS_CODE_REMOVAL::SkipOrdEq(const TERM& header)

inline bool FS_CODE_REMOVAL::SkipSecondEqArg()
{
  CALL("SkipSecondEqArg()");
  const Flatterm** var1;
  const Flatterm** var2;
  ulong var_num1;
  ulong var_num2;
  
  FS_COMMAND* curr_instr;
 check_instr:
  curr_instr = *node_addr;
  ASSERT(!(curr_instr->Fork()));
  switch (curr_instr->Tag())
    {
    case FS_COMMAND::SECOND_EQ_ARG : 
      node_addr = curr_instr->NextAddr();
      return true;
    case FS_COMMAND::COMPARE_NF : // no break here
    case FS_COMMAND::COMPARE :
      var1 = curr_instr->Var1();
      var2 = curr_instr->Var2();
      var_num1 = VarNum(var1);
      var_num2 = VarNum(var2);
      
      if (constr.Equivalent(var_num1,var_num2))
	{
	  node_addr = curr_instr->NextAddr();
	  goto check_instr; 
	}
      else return false; 

    case FS_COMMAND::SIG_FILTER :
      node_addr = curr_instr->NextAddr();
      goto check_instr;   

#ifdef DEBUG_NAMESPACE
    default: ICP("X0"); return false;
#else
#ifdef _SUPPRESS_WARNINGS_
    default : return false;
#endif
#endif 
    };
}; // bool FS_CODE_REMOVAL::SkipSecondEqArg()


inline bool FS_CODE_REMOVAL::SkipFunc(TERM func)
{
  CALL("SkipFunc(TERM func)");
  const Flatterm** var1;
  const Flatterm** var2;
  ulong var_num1;
  ulong var_num2;
  
  FS_COMMAND* curr_instr;
  ASSERT(*node_addr);
 check_instr:
  curr_instr = *node_addr;
  switch (curr_instr->Tag())
    {
    case FS_COMMAND::FUNC :
      if (curr_instr->Func() == func)
	{
	  if (curr_instr->Fork()) branch_to_destroy = node_addr;
	  node_addr = curr_instr->NextAddr();
	  return true;
	};
    try_fork: 
      if (curr_instr->Fork()) 
        {
	  node_addr = curr_instr->ForkAddr();
	  branch_to_destroy = node_addr;
	  goto check_instr;
        }; 
      return false;
    case FS_COMMAND::COMPARE_NF : // no break here
    case FS_COMMAND::COMPARE :
      var1 = curr_instr->Var1();
      var2 = curr_instr->Var2();
      var_num1 = VarNum(var1);
      var_num2 = VarNum(var2);
      
      if (constr.Equivalent(var_num1,var_num2))
	{
	  ASSERT(!(curr_instr->Fork()));
	  node_addr = curr_instr->NextAddr();
	  goto check_instr; 
	}
      else return false;
 
    case FS_COMMAND::SIG_FILTER :
      node_addr = curr_instr->NextAddr();
      goto check_instr;   

    default : goto try_fork;  
    };
}; // bool FS_CODE_REMOVAL::SkipFunc(TERM func)

inline bool FS_CODE_REMOVAL::SkipVar()
{
  CALL("SkipVar()");
  const Flatterm** var1;
  const Flatterm** var2;
  ulong var_num1;
  ulong var_num2;
 
  FS_COMMAND* curr_instr;
  ASSERT(*node_addr);
 check_instr:
  curr_instr = *node_addr;
  switch (curr_instr->Tag())
    {
    case FS_COMMAND::VAR :
      if (curr_instr->Fork()) branch_to_destroy = node_addr;
      node_addr = curr_instr->NextAddr();
      return true;
    case FS_COMMAND::COMPARE_NF : // no break here
    case FS_COMMAND::COMPARE :
      var1 = curr_instr->Var1();
      var2 = curr_instr->Var2();
      var_num1 = VarNum(var1);
      var_num2 = VarNum(var2);
      
      if (constr.Equivalent(var_num1,var_num2))
	{
	  ASSERT(!(curr_instr->Fork()));
	  node_addr = curr_instr->NextAddr();
	  goto check_instr; 
	}
      else return false;

    case FS_COMMAND::SIG_FILTER :
      node_addr = curr_instr->NextAddr();
      goto check_instr;   

    default : 
      // try fork 
      if (curr_instr->Fork()) 
	{
	  node_addr = curr_instr->ForkAddr();
	  branch_to_destroy = node_addr;
	  goto check_instr;
	}; 
      return false;
    };
}; // bool FS_CODE_REMOVAL::SkipVar()

inline void FS_CODE_REMOVAL::SkipCompare(const Flatterm** v1,const Flatterm** v2)
{
  CALL("SkipCompare(const Flatterm** v1,const Flatterm** v2)"); 
  ASSERT(v1 < v2);
  constr.MakeEquivalent(VarNum(v1),VarNum(v2));
}; // void FS_CODE_REMOVAL::SkipCompare(const Flatterm** v1,const Flatterm** v2)


inline bool FS_CODE_REMOVAL::FindSuccess()
{
  CALL("FindSuccess()");
  const Flatterm** var1;
  const Flatterm** var2;
  ulong var_num1;
  ulong var_num2;
  
  FS_COMMAND* curr_instr;
  ASSERT(*node_addr);
 check_tag:
  curr_instr = *node_addr;
  if (curr_instr->IsCompare() || curr_instr->IsCompareNF())
    { 
      var1 = curr_instr->Var1();
      var2 = curr_instr->Var2();
      var_num1 = VarNum(var1);
      var_num2 = VarNum(var2);
      if (constr.Equivalent(var_num1,var_num2))
	{
	  if (curr_instr->Fork()) branch_to_destroy = node_addr;
	  MoveDown();
	  goto check_tag;
	};
    try_fork: 
      if (curr_instr->Fork())
        {
	  node_addr = curr_instr->ForkAddr();
	  branch_to_destroy = node_addr;
	  goto check_tag;
        };
      return false;  
    };
  // !(curr_instr->IsCompare() || curr_instr->IsCompareNF())
  if (curr_instr->IsSigFilter())
    { 
      node_addr = curr_instr->NextAddr();
      goto check_tag; 
    };
    
  // !(curr_instr->IsCompare() || curr_instr->IsCompareNF()) and !curr_instr->IsSigFilter()
  if (curr_instr->IsSuccess())
    {
      if (curr_instr->TheClause() == removed_clause)
	{
	  if (curr_instr->Fork()) branch_to_destroy = node_addr;
	  return true;
	};
    };
  goto try_fork;
}; // bool FS_CODE_REMOVAL::FindSuccess() 
}; // namespace VK
//================================================
#undef DEBUG_NAMESPACE
//===================================================================
#endif
