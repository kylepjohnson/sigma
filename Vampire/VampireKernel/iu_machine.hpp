#ifndef IU_MACHINE_H
//==============================================================
#define IU_MACHINE_H
#ifndef NO_DEBUG_VIS
 #include <iostream>
#endif
#include "jargon.hpp"
#include "VampireKernelDebugFlags.hpp"
#include "VampireKernelConst.hpp"
#include "iu_command.hpp"
#include "iu_translator.hpp"
#include "prefix.hpp"
#include "variables.hpp"
#include "or_index.hpp"
#include "GlobAlloc.hpp"
#include "Array.hpp"
#include "ExpandingStack.hpp"
//================================================
#undef DEBUG_NAMESPACE
#ifdef DEBUG_FORWARD_SUPERPOSITION
 #define DEBUG_NAMESPACE "IU_MACH"
#endif
#include "debugMacros.hpp"
//=================================================
namespace VK
{
class IU_MACH
{
 private:
  enum BACKTRACK_ACTION
  {
    FAILURE,
    ALTER_BINDING_WITH_OC,
    ALTER_BINDING_WITH_OC_SAME_VAR,
    ALTER_BINDING,
    ALTER_UNIFIABLE,
    TRY_ALTERNATIVE_NODE,
    UNBIND_LAST_TRY_ALTERNATIVE_NODE,
    UNBIND_SECTION_TRY_ALTERNATIVE_NODE
  };
 private:
  IU_TRANSLATOR compiler;
  OR_PROCESSOR proc;

  BK::Array<BK::GlobAlloc,IU_COMMAND,256UL,IU_MACH> _codeMemory;
  IU_COMMAND* end_of_code;

  BK::Array<BK::GlobAlloc,PrefixSym,256UL,IU_MACH> _dataMemory;
  BK::ExpandingStack<BK::GlobAlloc,IU_COMMAND*,128UL,IU_MACH> _backtrackPoints;
  BK::ExpandingStack<BK::GlobAlloc,BACKTRACK_ACTION,128UL,IU_MACH> _backtrackActions;

  IU_COMMAND* command;
 private: // "sugar"
  Unifier::Variable*** subst_state;
  RES_PAIR_LIST* candidates;
 public:
  IU_MACH();
  ~IU_MACH();
  void init();
  void destroy();
  void reset() 
    {
      CALL("reset()");
      candidates = 0;
    };

  //static void ResetStatic() { OR_PROCESSOR::ResetStatic(); };
  void SetIndex(ulong index) 
    {
      CALL("SetIndex(ulong index)");
      proc.SetIndex(index); 
    };
  ulong GetIndex() const { return proc.GetIndex(); };

  IU_COMMAND* Code() { return _codeMemory.memory(); }; // quite unsafe
  const IU_COMMAND* Code() const { return _codeMemory.memory(); }; // quite unsafe
 
  void CompileLiteral(PrefixSym* lit)
    {
      CALL("CompileLiteral(PrefixSym* lit)");


      ASSERT(_dataMemory.size() >= _codeMemory.size());
      end_of_code = _codeMemory.memory();
      PrefixSym* dmem = _dataMemory.memory();
      while (!compiler.CompileLiteral(lit,end_of_code,dmem,_codeMemory.size()))
       {
	DOP(_codeMemory.unfreeze());
        DOP(_dataMemory.unfreeze());
        _codeMemory.expand();
        _dataMemory.expand(_codeMemory.size() - 1);
        DOP(_codeMemory.freeze());
        DOP(_dataMemory.freeze());
        end_of_code = _codeMemory.memory();
        dmem = _dataMemory.memory();
       };
      ASSERT(_dataMemory.size() >= _codeMemory.size());
      ResetBacktracking();
    };
   
  void CompilePseudoLiteral(TERM header,PrefixSym* arg)
    {
      CALL("CompilePseudoLiteral(TERM header,PrefixSym* arg)");

      ASSERT(_dataMemory.size() >= _codeMemory.size());
      end_of_code = _codeMemory.memory();
      PrefixSym* dmem = _dataMemory.memory();
      while (!compiler.CompilePseudoLiteral(header,arg,end_of_code,dmem,_codeMemory.size()))
       {
	DOP(_codeMemory.unfreeze());
        DOP(_dataMemory.unfreeze());
        _codeMemory.expand();
        _dataMemory.expand(_codeMemory.size() - 1);
        DOP(_codeMemory.freeze());
        DOP(_dataMemory.freeze());
        end_of_code = _codeMemory.memory();
        dmem = _dataMemory.memory();
       };
      ASSERT(_dataMemory.size() >= _codeMemory.size());
     
      ResetBacktracking();
    };

    
  void Inversepolarity()
    {
      Code()->Inversepolarity(); 
    };

  bool FindFirstLeaf(OR_INDEX_TREE_NODE* tree);
  bool FindNextLeaf();
  bool VariablesFromQueryAffected() 
    {
      IU_COMMAND* vars = Code()->AllVars();
      while (vars)
	{
	  if (vars->Symbol().var()->Binding()) { return true; };
	  vars = vars->NextVar();
	};
      return false;
    };
    
 
  bool VariablesFromIndexAffected()
    {
      return Unifier::current()->VariablesAffected(GetIndex());
    };

  void MemorizeState() { subst_state = Unifier::current()->MarkState(); };
  void RestoreState() { Unifier::current()->UnbindDownto(subst_state); };
  OR_INDEX_TREE_NODE* CurrentLeaf() const { return proc.CurrNode(); };
  void ResetCandidatesFromLeaf() 
    {      
      candidates = CurrentLeaf()->Clauses();
    };
  RES_PAIR_LIST* NextCandidate() 
    {
      RES_PAIR_LIST* res = candidates;
      if (candidates) { candidates = candidates->Next(); };
      return res;
    };

#ifndef NO_DEBUG_VIS
  ostream& output(ostream& str) const;
  ostream& outputCode(ostream& str) const;
#endif

 private:
  void ResetBacktracking()
    {
      CALL("ResetBacktracking()");
      if (_codeMemory.size())
       {
        DOP(_backtrackPoints.unfreeze());
        DOP(_backtrackActions.unfreeze());
        
        _backtrackPoints.expand(_codeMemory.size() - 1);
        _backtrackActions.expand(_codeMemory.size() - 1);

        DOP(_backtrackPoints.freeze());
        DOP(_backtrackActions.freeze());
       };
      _backtrackPoints.reset();
      _backtrackActions.reset();
    };

  bool Backtrack();
  bool CompleteSearch();

}; // class IU_MACH
}; // namespace VK

//==============================================================
#endif
