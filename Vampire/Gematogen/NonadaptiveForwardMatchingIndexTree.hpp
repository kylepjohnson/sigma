//
// File:         NonadaptiveForwardMatchingIndexTree.hpp
// Description:  Nonadaptive code trees for forward matching 
//               (aka search for generalisations).
// Created:      Mar 24, 2003, Alexandre Riazanov, riazanov@cs.man.ac.uk
// Note:         This is a part of the Gematogen library. 
// Revised:      Apr 11, 2003, Alexandre Riazanov, riazanov@cs.man.ac.uk
//               Adjusted to the new interface of instruction representation.
//====================================================
#ifndef NONADAPTIVE_FORWARD_MATCHING_INDEX_TREE_H
//=============================================================
#define NONADAPTIVE_FORWARD_MATCHING_INDEX_TREE_H
#ifndef NO_DEBUG_VIS 
#include <iostream>
#endif
#include <climits>
#include "jargon.hpp"
#include "Stack.hpp"
#include "GlobalStopFlag.hpp"
#include "GematogenDebugFlags.hpp"
#include "ForwardMatchingIndexTree.hpp"
#include "MultisetOfVariables.hpp"
#include "DestructionMode.hpp"
#include "RuntimeError.hpp"
using namespace std;

//================================================
#undef DEBUG_NAMESPACE
#ifdef DEBUG_FORWARD_MATCHING_INDEX_TREE
#  define DEBUG_NAMESPACE "FMCodeTreeNonadaptiveIntegrator"
#endif 
#include "debugMacros.hpp"
//=================================================
namespace Gem
{

  template <ulong MaxTermSize,ulong MaxNumOfVariables,class Alloc,class Symbol,class Flatterm,class LeafInfo,class InstanceId> 
  class FMCodeTreeNonadaptiveIntegrator // objects must be static - too costly constructor
  {
  public: 
    typedef FMCodeTree<MaxTermSize,MaxNumOfVariables,Alloc,Symbol,Flatterm,LeafInfo,InstanceId>::Instruction Instruction;
    typedef FMCodeTree<MaxTermSize,MaxNumOfVariables,Alloc,Symbol,Flatterm,LeafInfo,InstanceId>::InstructionWithFork InstructionWithFork;
    typedef FMCodeTree<MaxTermSize,MaxNumOfVariables,Alloc,Symbol,Flatterm,LeafInfo,InstanceId>::Func Func;
    typedef FMCodeTree<MaxTermSize,MaxNumOfVariables,Alloc,Symbol,Flatterm,LeafInfo,InstanceId>::FuncNF FuncNF;
    typedef FMCodeTree<MaxTermSize,MaxNumOfVariables,Alloc,Symbol,Flatterm,LeafInfo,InstanceId>::Var Var;
    typedef FMCodeTree<MaxTermSize,MaxNumOfVariables,Alloc,Symbol,Flatterm,LeafInfo,InstanceId>::VarNF VarNF;
    typedef FMCodeTree<MaxTermSize,MaxNumOfVariables,Alloc,Symbol,Flatterm,LeafInfo,InstanceId>::Compare Compare;
    typedef FMCodeTree<MaxTermSize,MaxNumOfVariables,Alloc,Symbol,Flatterm,LeafInfo,InstanceId>::CompareNF CompareNF;
    typedef FMCodeTree<MaxTermSize,MaxNumOfVariables,Alloc,Symbol,Flatterm,LeafInfo,InstanceId>::Leaf Leaf; 
  public:
    FMCodeTreeNonadaptiveIntegrator(const Flatterm** subst) : 
      _matchingIndexSubstitution(subst)
    {
      CALL("constructor FMCodeTreeNonadaptiveIntegrator(const Flatterm** subst)");
    };

    ~FMCodeTreeNonadaptiveIntegrator() 
    {
      CALL("destructor ~FMCodeTreeNonadaptiveIntegrator()");
    };

    void init(const Flatterm** subst)
    {
      CALL("init(const Flatterm** subst)");
      _matchingIndexSubstitution = subst;
      _registeredVariables.init();
      _compares.init();
    }; // void init(const Flatterm** subst)

    void destroy()
    {
      CALL("destroy()");
      _compares.destroy();
      _registeredVariables.destroy();
      BK_CORRUPT(*this);
    }; // void destroy()



    void Reset(Instruction** n)
    {
      CALL("Reset(Instruction** n)");
      _nodeAddr = n;
      _registeredVariables.Reset();
      _currentMatchingIndexVar = _matchingIndexSubstitution;
      _compares.reset();
      _readModeFlag = ((*_nodeAddr) != 0);
      _currentDepth = 0UL;
    };

    BK::MultisetOfVariables<MaxNumOfVariables>& Vars() { return _registeredVariables; };

    const Flatterm** MapVar(ulong v) const { return _firstVariableOccurence[v]; }; 

    void Push(const Symbol& sym)
    { 
      if (sym.isVariable()) { PushVar(sym); } else PushFunc(sym); 
    };  
    void PushVar(const Symbol& sym);
    void PushFunc(const Symbol& sym);

    Leaf* GetLeaf(bool& newLeaf)
    {
      CALL("GetLeaf()");
      _remainingCompares = _compares.begin();
      if (_readModeFlag)
	{
	  _readModeFlag = ReadCollectedComps(); 
	  if (!_readModeFlag) goto write_remaining_compares_and_leaf;
	  newLeaf = false;
	  ASSERT((*_nodeAddr)->isLeaf());
	  return static_cast<Leaf*>(static_cast<void*>(*_nodeAddr));
	}
      else // write mode
	{
	write_remaining_compares_and_leaf:
	  WriteCollectedComps();
	  newLeaf = true;
	  *_nodeAddr = new Leaf();
	  return static_cast<Leaf*>(static_cast<void*>(*_nodeAddr));
	};	  
    }; // Leaf* GetLeaf(bool& newLeaf)

  private:
    
    void MoveDown() 
    {
      CALL("MoveDown()");
      _nodeAddr = (*_nodeAddr)->nextAddr(); 
      ++_currentDepth;
    };
  
    void MoveRightFrom(InstructionWithFork* instr)
    {
      CALL("MoveRightFrom(InstructionWithFork* instr)");
      ASSERT(instr->next());
      _nodeAddr = instr->forkAddr();
    };
    
    void MoveRightFrom(FuncNF* instr)
    {
      CALL("MoveRightFrom(FuncNF* instr)");
      ASSERT(instr->next());
      *_nodeAddr = instr->copyWithFork(0);
      _nodeAddr = (*_nodeAddr)->forkAddr();
      delete instr;
    };

    void MoveRightFrom(VarNF* instr)
    {
      CALL("MoveRightFrom(VarNF* instr)");
      ASSERT(instr->next());
      *_nodeAddr = instr->copyWithFork(0);
      _nodeAddr = (*_nodeAddr)->forkAddr();
      delete instr;
    };

    void MoveRightFrom(CompareNF* instr)
    {
      CALL("MoveRightFrom(CompareNF* instr)");
      ASSERT(instr->next());
      *_nodeAddr = instr->copyWithFork(0);
      _nodeAddr = (*_nodeAddr)->forkAddr();
      delete instr;
    };


    ulong VarNum(const Flatterm** v) const { return v - _matchingIndexSubstitution; };
    const Flatterm** nativeVar(const ulong& var_num) const 
    {
      CALL("nativeVar(ulong var_num) const");
      ASSERT(var_num < MaxTermSize); 
      return _matchingIndexSubstitution + var_num; 
    };


    // Read mode
  private:
    bool ReadFunc(const Symbol& func);
    bool ReadVar();
    bool ReadCompare(const Flatterm** v1,const Flatterm** v2);
    bool ReadCollectedComps();
    
    // Write mode 
  private:
    void WriteFunc(const Symbol& func);
    void WriteVar(const ulong& v);
    void WriteCompare(const Flatterm** v1,const Flatterm** v2);
    void WriteCollectedComps();
    void InsertVar(const Flatterm** v);
    void InsertFunc(const Symbol& func);
      

  private: // data

    Instruction** _nodeAddr;

    const Flatterm** _matchingIndexSubstitution;
  
    const Flatterm** _firstVariableOccurence[MaxNumOfVariables];
    BK::MultisetOfVariables<MaxNumOfVariables> _registeredVariables; 
    const Flatterm** _currentMatchingIndexVar;
    BK::Stack<const Flatterm**,2*MaxTermSize> _compares;
    const Flatterm*** _remainingCompares;
    bool _readModeFlag;
   
    ulong _currentDepth;
    
    enum 
    {
      _startDepthForSkeletonNF = 14UL
    };

  }; // class FMCodeTreeNonadaptiveIntegrator


}; // namespace Gem

//================================================
#undef DEBUG_NAMESPACE
#ifdef DEBUG_FORWARD_MATCHING_INDEX_TREE
#define DEBUG_NAMESPACE "FMCodeTreeNonadaptiveRemoval<MaxTermSize,MaxNumOfVariables,Alloc,Symbol,Flatterm,LeafInfo,InstanceId>"
#endif
#include "debugMacros.hpp"
//=================================================

namespace Gem
{
  template <ulong MaxTermSize,ulong MaxNumOfVariables,class Alloc,class Symbol,class Flatterm,class LeafInfo,class InstanceId> 
  class FMCodeTreeNonadaptiveRemoval // objects must be static - too costly constructor
  {  
  public:  
    typedef FMCodeTree<MaxTermSize,MaxNumOfVariables,Alloc,Symbol,Flatterm,LeafInfo,InstanceId>::Instruction Instruction;
    typedef FMCodeTree<MaxTermSize,MaxNumOfVariables,Alloc,Symbol,Flatterm,LeafInfo,InstanceId>::InstructionWithFork InstructionWithFork;
    typedef FMCodeTree<MaxTermSize,MaxNumOfVariables,Alloc,Symbol,Flatterm,LeafInfo,InstanceId>::Func Func;
    typedef FMCodeTree<MaxTermSize,MaxNumOfVariables,Alloc,Symbol,Flatterm,LeafInfo,InstanceId>::FuncNF FuncNF;
    typedef FMCodeTree<MaxTermSize,MaxNumOfVariables,Alloc,Symbol,Flatterm,LeafInfo,InstanceId>::Var Var;
    typedef FMCodeTree<MaxTermSize,MaxNumOfVariables,Alloc,Symbol,Flatterm,LeafInfo,InstanceId>::VarNF VarNF;
    typedef FMCodeTree<MaxTermSize,MaxNumOfVariables,Alloc,Symbol,Flatterm,LeafInfo,InstanceId>::Compare Compare;
    typedef FMCodeTree<MaxTermSize,MaxNumOfVariables,Alloc,Symbol,Flatterm,LeafInfo,InstanceId>::CompareNF CompareNF;
    typedef FMCodeTree<MaxTermSize,MaxNumOfVariables,Alloc,Symbol,Flatterm,LeafInfo,InstanceId>::Leaf Leaf; 

  public:
    FMCodeTreeNonadaptiveRemoval(const Flatterm** subst) : 
      _matchingIndexSubstitution(subst)
    {};
    ~FMCodeTreeNonadaptiveRemoval() {};

    void init(const Flatterm** subst)
    {
      CALL("init(const Flatterm** subst)");
      _matchingIndexSubstitution = subst;
      _registeredVariables.init();   
      _compares.init();
    };


    void destroy()
    {
      CALL("destroy()");
      _compares.destroy();
      _registeredVariables.destroy();    
      BK_CORRUPT(*this);
    };


    void Reset(Instruction** n)
    {
      CALL("Reset(Instruction** n)");
      // here *n must be nonnull!
      ASSERT(*n);
      _nodeAddr = n;
      _branchToDestroy = n;
      _registeredVariables.Reset();
      _currentMatchingIndexVar = _matchingIndexSubstitution; 
      _compares.reset();
    };
    bool Skip(const Symbol& sym)
    {
      if (sym.isVariable()) { return SkipVar(sym.var()); }
      else return SkipFunc(sym);
    };    
   
    bool SkipFunc(const Symbol& func);
    bool SkipVar(const Symbol& var) { return SkipVar(var.var()); };
    bool SkipComps(); 
    Leaf* GetLeaf() const 
    {
      CALL("GetLeaf()");
      ASSERT((*_nodeAddr)->isLeaf());
      return static_cast<Leaf*>(static_cast<void*>(*_nodeAddr)); 
    };
    Instruction** BranchToDestroy() const { return _branchToDestroy; }; 
  
  private:
    void MoveDown() 
    {
      CALL("MoveDown()");
      _nodeAddr = (*_nodeAddr)->nextAddr(); 
    };
    void MoveRight() 
    { 
      CALL("MoveRight()");
      _nodeAddr = (*_nodeAddr)->forkAddr(); 
    }; 
    ulong VarNum(const Flatterm** v) const { return v - _matchingIndexSubstitution; };   
    bool SkipVar(ulong var);
    bool SkipVar();

  private:
    Instruction** _nodeAddr;
    Instruction** _branchToDestroy;

    const Flatterm** _matchingIndexSubstitution;  
    const Flatterm** _firstVariableOccurence[MaxNumOfVariables];
    BK::MultisetOfVariables<MaxNumOfVariables> _registeredVariables; 
    const Flatterm** _currentMatchingIndexVar;
    BK::Stack<const Flatterm**,2*MaxTermSize> _compares;

  }; // class FMCodeTreeNonadaptiveRemoval<MaxTermSize,MaxNumOfVariables,Alloc,Symbol,Flatterm,LeafInfo,InstanceId>
}; // namespace Gem




//================================================
#undef DEBUG_NAMESPACE
#ifdef DEBUG_FORWARD_MATCHING_INDEX_TREE
#define DEBUG_NAMESPACE "FMCodeTreeNonadaptiveRetrieval<MaxTermSize,MaxNumOfVariables,Alloc,Symbol,Flatterm,LeafInfo,InstanceId>"
#endif
#include "debugMacros.hpp"
//=================================================

namespace Gem
{
  template <ulong MaxTermSize,ulong MaxNumOfVariables,class Alloc,class Symbol,class Flatterm,class LeafInfo,class InstanceId> 
  class FMCodeTreeNonadaptiveRetrieval
  {   
  public:
    typedef FMCodeTree<MaxTermSize,MaxNumOfVariables,Alloc,Symbol,Flatterm,LeafInfo,InstanceId>::Instruction Instruction;
    typedef FMCodeTree<MaxTermSize,MaxNumOfVariables,Alloc,Symbol,Flatterm,LeafInfo,InstanceId>::InstructionWithFork InstructionWithFork;
    typedef FMCodeTree<MaxTermSize,MaxNumOfVariables,Alloc,Symbol,Flatterm,LeafInfo,InstanceId>::Func Func;
    typedef FMCodeTree<MaxTermSize,MaxNumOfVariables,Alloc,Symbol,Flatterm,LeafInfo,InstanceId>::FuncNF FuncNF;
    typedef FMCodeTree<MaxTermSize,MaxNumOfVariables,Alloc,Symbol,Flatterm,LeafInfo,InstanceId>::Var Var;
    typedef FMCodeTree<MaxTermSize,MaxNumOfVariables,Alloc,Symbol,Flatterm,LeafInfo,InstanceId>::VarNF VarNF;
    typedef FMCodeTree<MaxTermSize,MaxNumOfVariables,Alloc,Symbol,Flatterm,LeafInfo,InstanceId>::Compare Compare;
    typedef FMCodeTree<MaxTermSize,MaxNumOfVariables,Alloc,Symbol,Flatterm,LeafInfo,InstanceId>::CompareNF CompareNF;
    typedef FMCodeTree<MaxTermSize,MaxNumOfVariables,Alloc,Symbol,Flatterm,LeafInfo,InstanceId>::Leaf Leaf;  
    
  public:
    FMCodeTreeNonadaptiveRetrieval() 
      : backtrack_nodes("FMCodeTreeNonadaptiveRetrieval::backtrack_nodes"),
	backtrack_cursors("FMCodeTreeNonadaptiveRetrieval::backtrack_cursors")
    {
      CALL("constructor FMCodeTreeNonadaptiveRetrieval");
    };
    ~FMCodeTreeNonadaptiveRetrieval() 
    {
      CALL("destructor ~FMCodeTreeNonadaptiveRetrieval()");
    };
    void init()
    {
      CALL("init()");
      backtrack_nodes.init("FMCodeTreeNonadaptiveRetrieval::backtrack_nodes");
      backtrack_cursors.init("FMCodeTreeNonadaptiveRetrieval::backtrack_cursors");
    };
  
    void destroy()
    {
      CALL("destroy()");
      backtrack_cursors.destroy();
      backtrack_nodes.destroy();
      BK_CORRUPT(*this);
    };

    Leaf* Match(const Flatterm* query,Instruction* tree);
    Leaf* MatchAgain();
    
  private:
    BK::Stack<Instruction*,MaxTermSize> backtrack_nodes;
    BK::Stack<const Flatterm*,MaxTermSize> backtrack_cursors;
  }; // template <ulong MaxTermSize,ulong MaxNumOfVariables,class Alloc,class Symbol,class Flatterm,class LeafInfo,class InstanceId> class Retrieval

}; // namespace Gem







//================================================
#undef DEBUG_NAMESPACE
#ifdef DEBUG_FORWARD_MATCHING_INDEX_TREE
#define DEBUG_NAMESPACE "FMCodeTreeNonadaptiveIntegrator<MaxTermSize,MaxNumOfVariables,Alloc,Symbol,Flatterm,LeafInfo,InstanceId>"
#endif
#include "debugMacros.hpp"
//=================================================
namespace Gem
{

  template <ulong MaxTermSize,ulong MaxNumOfVariables,class Alloc,class Symbol,class Flatterm,class LeafInfo,class InstanceId> 
  inline void FMCodeTreeNonadaptiveIntegrator<MaxTermSize,MaxNumOfVariables,Alloc,Symbol,Flatterm,LeafInfo,InstanceId>::PushVar(const Symbol& sym)
  {  
    CALL("PushVar(const Symbol& sym)");
    ulong var = sym.var();
    ASSERT(var < MaxNumOfVariables); 

    if (_readModeFlag)
      {
	_readModeFlag = ReadVar();
	if (_readModeFlag)
	  {
	    if (_registeredVariables.Occurences(var)) 
	      { 
		if (!ReadCompare(_firstVariableOccurence[var],_currentMatchingIndexVar))
		  {
		    _readModeFlag = false;
		    WriteCompare(_firstVariableOccurence[var],_currentMatchingIndexVar);
		  };
	      }
	    else // first occurence of the variable
	      {
		_registeredVariables.Register(var);
		_firstVariableOccurence[var] = _currentMatchingIndexVar;
	      };        
	  } 
	else // variable must be inserted 
	  {
	    InsertVar(_currentMatchingIndexVar);
	    if (_registeredVariables.Occurences(var))
	      {
		WriteCompare(_firstVariableOccurence[var],_currentMatchingIndexVar);
	      } 
	    else // first occurence of the variable
	      {
		_registeredVariables.Register(var);
		_firstVariableOccurence[var] = _currentMatchingIndexVar;
	      };     
	  };
	_currentMatchingIndexVar++;
      }
    else // write mode
      WriteVar(var);          
  }; // void FMCodeTreeNonadaptiveIntegrator<MaxTermSize,MaxNumOfVariables,Alloc,Symbol,Flatterm,LeafInfo,InstanceId>::PushVar(const Symbol& sym)

  template <ulong MaxTermSize,ulong MaxNumOfVariables,class Alloc,class Symbol,class Flatterm,class LeafInfo,class InstanceId> 
  inline void FMCodeTreeNonadaptiveIntegrator<MaxTermSize,MaxNumOfVariables,Alloc,Symbol,Flatterm,LeafInfo,InstanceId>::PushFunc(const Symbol& sym)
  {
    CALL("PushFunc(const Symbol& sym)");
    if (_readModeFlag)
      {
	_readModeFlag = ReadFunc(sym);
	if (!_readModeFlag) 
	  {
	    InsertFunc(sym);
	  }; 
      }
    else // write mode
      {
	WriteFunc(sym);
      };   
  }; // void FMCodeTreeNonadaptiveIntegrator<MaxTermSize,MaxNumOfVariables,Alloc,Symbol,Flatterm,LeafInfo,InstanceId>::PushFunc(const Symbol& sym)

}; // namespace Gem




//================================================
#undef DEBUG_NAMESPACE
#ifdef DEBUG_FORWARD_MATCHING_INDEX_TREE
#define DEBUG_NAMESPACE "FMCodeTreeNonadaptiveIntegrator<MaxTermSize,MaxNumOfVariables,Alloc,Symbol,Flatterm,LeafInfo,InstanceId>"
#endif
#include "debugMacros.hpp"
//=================================================
namespace Gem
{
  template <ulong MaxTermSize,ulong MaxNumOfVariables,class Alloc,class Symbol,class Flatterm,class LeafInfo,class InstanceId> 
  inline bool FMCodeTreeNonadaptiveIntegrator<MaxTermSize,MaxNumOfVariables,Alloc,Symbol,Flatterm,LeafInfo,InstanceId>::ReadFunc(const Symbol& sym)
  { 
    CALL("ReadFunc(const Symbol& sym)");
    Instruction* curr_node;  
  check_tag:   
    curr_node = *_nodeAddr; 
    ASSERT(curr_node);
    switch (curr_node->tag())
      {
      case Instruction::FuncTag:  
	if (curr_node->asFunc()->symbol() == sym)
	  {
	    MoveDown();
	    return true;
	  };
	// different functor
	if (curr_node->asFunc()->symbol() > sym) return false;

	// curr_node->asFunc()->symbol() < sym 
	// mismatch occured, try fork
	if (curr_node->fork())      
	  {
	    MoveRightFrom(curr_node->asInstrWithFork());
	    goto check_tag;
	  };

	// no more forks
	MoveRightFrom(curr_node->asInstrWithFork());
	return false; 

      case Instruction::FuncNFTag:  
	if (curr_node->asFuncNF()->symbol() == sym)
	  {
	    MoveDown();
	    return true;
	  };
	// different functor
	if (curr_node->asFuncNF()->symbol() > sym) return false;

	// curr_node->asFuncNF()->symbol() < sym 
	MoveRightFrom(curr_node->asFuncNF());
	return false; 


      case Instruction::VarTag:
	// mismatch occured, try fork
	if (curr_node->fork())      
	  {
	    MoveRightFrom(curr_node->asInstrWithFork());
	    goto check_tag;
	  };

	// no more forks
	MoveRightFrom(curr_node->asInstrWithFork());
	return false; 

      case Instruction::VarNFTag:
	// mismatch occured
	MoveRightFrom(curr_node->asVarNF());
	return false; 

      case Instruction::CompareTag: 
	// same as below
      case Instruction::CompareNFTag: 
	// same as below
      case Instruction::LeafTag: 
	ICP("ICP0");
	return false;

#ifdef DEBUG_NAMESPACE
      default: ICP("X0"); return false;
#else 
#ifdef _SUPPRESS_WARNINGS_
      default: return false;
#endif
#endif
      };
  }; // bool FMCodeTreeNonadaptiveIntegrator<MaxTermSize,MaxNumOfVariables,Alloc,Symbol,Flatterm,LeafInfo,InstanceId>::ReadFunc(const Symbol& sym)

  template <ulong MaxTermSize,ulong MaxNumOfVariables,class Alloc,class Symbol,class Flatterm,class LeafInfo,class InstanceId> 
  inline bool FMCodeTreeNonadaptiveIntegrator<MaxTermSize,MaxNumOfVariables,Alloc,Symbol,Flatterm,LeafInfo,InstanceId>::ReadVar()
  {
    CALL("ReadVar()");
    Instruction* curr_node;
    curr_node = *_nodeAddr;        
    switch (curr_node->tag())
      {
      case Instruction::FuncTag:
	// no functor can preceed a variable in a tree
	return false; 

      case Instruction::FuncNFTag:
	// no functor can preceed a variable in a tree
	return false; 

      case Instruction::VarTag:
	MoveDown();
	return true;

      case Instruction::VarNFTag:
	MoveDown();
	return true;	
	
      case Instruction::CompareTag: 
	// same as below
      case Instruction::CompareNFTag: 
	// same as below
      case Instruction::LeafTag: 
	ICP("ICP0");
	return false;


#ifdef DEBUG_NAMESPACE
      default: ICP("X0"); return false;
#else 
#ifdef _SUPPRESS_WARNINGS_
      default: return false;
#endif
#endif
      };  
  }; // bool FMCodeTreeNonadaptiveIntegrator<MaxTermSize,MaxNumOfVariables,Alloc,Symbol,Flatterm,LeafInfo,InstanceId>::ReadVar() 


  template <ulong MaxTermSize,ulong MaxNumOfVariables,class Alloc,class Symbol,class Flatterm,class LeafInfo,class InstanceId> 
  inline bool FMCodeTreeNonadaptiveIntegrator<MaxTermSize,MaxNumOfVariables,Alloc,Symbol,Flatterm,LeafInfo,InstanceId>::ReadCompare(const Flatterm** v1,const Flatterm** v2)
  {
    CALL("ReadCompare(const Flatterm** v1,const Flatterm** v2)");
    _compares.push(v1);
    _compares.push(v2);   
    return true;
  }; // bool FMCodeTreeNonadaptiveIntegrator<MaxTermSize,MaxNumOfVariables,Alloc,Symbol,Flatterm,LeafInfo,InstanceId>::ReadCompare(const Flatterm** v1,const Flatterm** v2)


  template <ulong MaxTermSize,ulong MaxNumOfVariables,class Alloc,class Symbol,class Flatterm,class LeafInfo,class InstanceId> 
  inline bool FMCodeTreeNonadaptiveIntegrator<MaxTermSize,MaxNumOfVariables,Alloc,Symbol,Flatterm,LeafInfo,InstanceId>::ReadCollectedComps()
  {
    CALL("ReadCollectedComps()");
    while (_remainingCompares < _compares.end())
      {
	const Flatterm** v1 = *_remainingCompares;
	const Flatterm** v2 = *(_remainingCompares + 1);


	Instruction* curr_node;  
      check_tag:   
	curr_node = *_nodeAddr; 
	ASSERT(curr_node);
	switch (curr_node->tag())
	  {
	  case Instruction::FuncTag: 
	    // same as below
	  case Instruction::FuncNFTag: 
	    // same as below
	  case Instruction::VarTag: 
	    // same as below
	  case Instruction::VarNFTag: 
	    // same as below
	    ICP("ICP0");
	    return false;

	  case Instruction::CompareTag:
	    if ((curr_node->asCompare()->var1() == v1) &&
		(curr_node->asCompare()->var2() == v2))
	      {
		MoveDown();
		_remainingCompares += 2;
		break;
	      }
	    else
	      {
		MoveRightFrom(curr_node->asInstrWithFork());
		if (*_nodeAddr) goto check_tag;
		return false;
	      };
	      
	  case Instruction::CompareNFTag:
	    if ((curr_node->asCompareNF()->var1() == v1) &&
		(curr_node->asCompareNF()->var2() == v2))
	      {
		MoveDown();
		_remainingCompares += 2;
		break;
	      }
	    else
	      {
		MoveRightFrom(curr_node->asCompareNF());
		if (*_nodeAddr) goto check_tag;
		return false;
	      };
	      
	  case Instruction::LeafTag:
	    return false;

#ifdef DEBUG_NAMESPACE
	  default: ICP("X0"); return false;
#else 
#ifdef _SUPPRESS_WARNINGS_
	  default: return false;
#endif
#endif
	  }; // switch (curr_node->tag())	

      }; // while (_remainingCompares < _compares.end())

    
  check_leaf:
    if ((*_nodeAddr)->tag() == Instruction::LeafTag)
      return true;
    ASSERT((*_nodeAddr)->isCompare());
    if ((*_nodeAddr)->tag() == Instruction::CompareTag)
      {
	MoveRightFrom((*_nodeAddr)->asInstrWithFork());
	if (!(*_nodeAddr)) return false;
	goto check_leaf;
      }
    else
      {
	ASSERT((*_nodeAddr)->tag() == Instruction::CompareNFTag);
	MoveRightFrom((*_nodeAddr)->asCompareNF());
	ASSERT(!(*_nodeAddr));
	return false;
      };
  }; // bool FMCodeTreeNonadaptiveIntegrator<MaxTermSize,MaxNumOfVariables,Alloc,Symbol,Flatterm,LeafInfo,InstanceId>::ReadCollectedComps()




  template <ulong MaxTermSize,ulong MaxNumOfVariables,class Alloc,class Symbol,class Flatterm,class LeafInfo,class InstanceId> 
  inline void FMCodeTreeNonadaptiveIntegrator<MaxTermSize,MaxNumOfVariables,Alloc,Symbol,Flatterm,LeafInfo,InstanceId>::InsertVar(const Flatterm** v)
  {
    CALL("InsertVar(const Flatterm** v)");
    Instruction* tmp = *_nodeAddr;
    if (tmp || (_currentDepth < _startDepthForSkeletonNF))
      {
	*_nodeAddr = new Var(v);
	(*_nodeAddr)->setFork(tmp);
      }
    else
      {
	*_nodeAddr = new VarNF(v);
      };
    MoveDown(); 
  }; // void FMCodeTreeNonadaptiveIntegrator<MaxTermSize,MaxNumOfVariables,Alloc,Symbol,Flatterm,LeafInfo,InstanceId>::InsertVar(const Flatterm** v)

  template <ulong MaxTermSize,ulong MaxNumOfVariables,class Alloc,class Symbol,class Flatterm,class LeafInfo,class InstanceId> 
  inline void FMCodeTreeNonadaptiveIntegrator<MaxTermSize,MaxNumOfVariables,Alloc,Symbol,Flatterm,LeafInfo,InstanceId>::InsertFunc(const Symbol& func)
  {
    CALL("InsertFunc(const Symbol& func)");
    Instruction* tmp = *_nodeAddr;
    if (tmp || (_currentDepth < _startDepthForSkeletonNF))
      {
	*_nodeAddr = new Func(func);
	(*_nodeAddr)->setFork(tmp);
      }
    else
      *_nodeAddr = new FuncNF(func);
    MoveDown();
  }; // void FMCodeTreeNonadaptiveIntegrator<MaxTermSize,MaxNumOfVariables,Alloc,Symbol,Flatterm,LeafInfo,InstanceId>::InsertFunc(const Symbol& func)  

  template <ulong MaxTermSize,ulong MaxNumOfVariables,class Alloc,class Symbol,class Flatterm,class LeafInfo,class InstanceId> 
  inline void FMCodeTreeNonadaptiveIntegrator<MaxTermSize,MaxNumOfVariables,Alloc,Symbol,Flatterm,LeafInfo,InstanceId>::WriteVar(const ulong& v)
  {
    CALL("WriteVar(const ulong& v)");   
    if (_currentDepth < _startDepthForSkeletonNF)
      {
	*_nodeAddr = new Var(_currentMatchingIndexVar);
      }
    else
      *_nodeAddr = new VarNF(_currentMatchingIndexVar);
    MoveDown();
    if (_registeredVariables.Occurences(v)) { WriteCompare(_firstVariableOccurence[v],_currentMatchingIndexVar); }
    else // first occurence
      {
	_registeredVariables.Register(v);
	_firstVariableOccurence[v] = _currentMatchingIndexVar;
      };   
    _currentMatchingIndexVar++;

  }; // void FMCodeTreeNonadaptiveIntegrator<MaxTermSize,MaxNumOfVariables,Alloc,Symbol,Flatterm,LeafInfo,InstanceId>::WriteVar(ulong v) 

  template <ulong MaxTermSize,ulong MaxNumOfVariables,class Alloc,class Symbol,class Flatterm,class LeafInfo,class InstanceId> 
  inline void FMCodeTreeNonadaptiveIntegrator<MaxTermSize,MaxNumOfVariables,Alloc,Symbol,Flatterm,LeafInfo,InstanceId>::WriteFunc(const Symbol& func)
  {
    CALL("WriteFunc(const Symbol& func)");
    if (_currentDepth < _startDepthForSkeletonNF)
      {
	*_nodeAddr = new Func(func);
      }
    else
      {
	*_nodeAddr = new FuncNF(func);
      };
    MoveDown();
  }; // void FMCodeTreeNonadaptiveIntegrator<MaxTermSize,MaxNumOfVariables,Alloc,Symbol,Flatterm,LeafInfo,InstanceId>::WriteFunc(const Symbol& func) 


  template <ulong MaxTermSize,ulong MaxNumOfVariables,class Alloc,class Symbol,class Flatterm,class LeafInfo,class InstanceId> 
  inline void FMCodeTreeNonadaptiveIntegrator<MaxTermSize,MaxNumOfVariables,Alloc,Symbol,Flatterm,LeafInfo,InstanceId>::WriteCompare(const Flatterm** v1,const Flatterm** v2)
  {
    CALL("WriteCompare(const Flatterm** v1,const Flatterm** v2)");
    _compares.push(v1);
    _compares.push(v2);      
  }; // void FMCodeTreeNonadaptiveIntegrator<MaxTermSize,MaxNumOfVariables,Alloc,Symbol,Flatterm,LeafInfo,InstanceId>::WriteCompare(const Flatterm** v1,const Flatterm** v2)

  template <ulong MaxTermSize,ulong MaxNumOfVariables,class Alloc,class Symbol,class Flatterm,class LeafInfo,class InstanceId> 
  inline void FMCodeTreeNonadaptiveIntegrator<MaxTermSize,MaxNumOfVariables,Alloc,Symbol,Flatterm,LeafInfo,InstanceId>::WriteCollectedComps()
  {
    CALL("WriteCollectedComps()");
    if (_remainingCompares < _compares.end())
      {
	const Flatterm** v1 = *_remainingCompares;
	const Flatterm** v2 = *(_remainingCompares + 1);
	_remainingCompares += 2;
	Instruction* tmp = *_nodeAddr;
	if (tmp)
	  {
	    *_nodeAddr = new Compare(v1,v2);
	    (*_nodeAddr)->setFork(tmp);
	  }
	else
	  *_nodeAddr = new CompareNF(v1,v2);
	MoveDown();
	while (_remainingCompares < _compares.end())
	  {
	    v1 = *_remainingCompares;
	    v2 = *(_remainingCompares + 1);
	    _remainingCompares += 2;
	    *_nodeAddr = new CompareNF(v1,v2);
	    MoveDown();
	  };
      };
    
  }; // void FMCodeTreeNonadaptiveIntegrator<MaxTermSize,MaxNumOfVariables,Alloc,Symbol,Flatterm,LeafInfo,InstanceId>::WriteCollectedComps()
}; // namespace Gem



//================================================
#undef DEBUG_NAMESPACE
#ifdef DEBUG_FORWARD_MATCHING_INDEX_TREE
#define DEBUG_NAMESPACE "FMCodeTreeNonadaptiveRemoval<MaxTermSize,MaxNumOfVariables,Alloc,Symbol,Flatterm,LeafInfo,InstanceId>"
#endif
#include "debugMacros.hpp"
//=================================================
namespace Gem
{
  template <ulong MaxTermSize,ulong MaxNumOfVariables,class Alloc,class Symbol,class Flatterm,class LeafInfo,class InstanceId> 
  inline bool FMCodeTreeNonadaptiveRemoval<MaxTermSize,MaxNumOfVariables,Alloc,Symbol,Flatterm,LeafInfo,InstanceId>::SkipVar(ulong var)
  {
    if (_registeredVariables.Occurences(var))
      {
	_compares.push(_firstVariableOccurence[var]);
	_compares.push(_currentMatchingIndexVar);  
      }
    else // first occurence
      {
	_registeredVariables.Register(var);
	_firstVariableOccurence[var] = _currentMatchingIndexVar;
      };

    _currentMatchingIndexVar++;

    return SkipVar();    
  }; // bool FMCodeTreeNonadaptiveRemoval<MaxTermSize,MaxNumOfVariables,Alloc,Symbol,Flatterm,LeafInfo,InstanceId>::SkipVar(ulong var) 

  template <ulong MaxTermSize,ulong MaxNumOfVariables,class Alloc,class Symbol,class Flatterm,class LeafInfo,class InstanceId> 
  inline bool FMCodeTreeNonadaptiveRemoval<MaxTermSize,MaxNumOfVariables,Alloc,Symbol,Flatterm,LeafInfo,InstanceId>::SkipVar()
  {
    CALL("SkipVar()");
    ASSERT(*_nodeAddr);
    Instruction* curr_node = *_nodeAddr;
    switch (curr_node->tag())
      {
      case Instruction::FuncTag: return false; // no functor can preceed a variable
      case Instruction::FuncNFTag: return false; // no functor can preceed a variable
     
      case Instruction::VarTag:    
	if (curr_node->fork()) _branchToDestroy = _nodeAddr;
	MoveDown();
	return true;  

      case Instruction::VarNFTag:    
	MoveDown();
	return true;  
      
      case Instruction::CompareTag: 
	// same as below
      case Instruction::CompareNFTag: 
	// same as below
      case Instruction::LeafTag: 
	ICP("ICP0");
	return false;

#ifdef DEBUG_NAMESPACE
      default: ICP("X0"); return false;
#else 
#ifdef _SUPPRESS_WARNINGS_
      default: return false;
#endif
#endif 
      };
  }; // bool FMCodeTreeNonadaptiveRemoval<MaxTermSize,MaxNumOfVariables,Alloc,Symbol,Flatterm,LeafInfo,InstanceId>::SkipVar() 

  template <ulong MaxTermSize,ulong MaxNumOfVariables,class Alloc,class Symbol,class Flatterm,class LeafInfo,class InstanceId> 
  inline bool FMCodeTreeNonadaptiveRemoval<MaxTermSize,MaxNumOfVariables,Alloc,Symbol,Flatterm,LeafInfo,InstanceId>::SkipFunc(const Symbol& func)
  {
    CALL("SkipFunc(const Symbol& func)");
    ASSERT(*_nodeAddr);
    Instruction* curr_node;
  check_node:
    curr_node = *_nodeAddr;
    switch (curr_node->tag())
      {
      case Instruction::FuncTag:
	if (curr_node->asFunc()->symbol() == func) 
	  {
	    if (curr_node->fork()) _branchToDestroy = _nodeAddr;
	    MoveDown();
	    return true;
	  };
	if (curr_node->fork())
	  {
	    MoveRight();
	    _branchToDestroy = _nodeAddr;
	    goto check_node;
	  }        
	else return false; 

      case Instruction::FuncNFTag:
	if (curr_node->asFuncNF()->symbol() == func) 
	  {
	    MoveDown();
	    return true;
	  };
	return false; 



      case Instruction::VarTag: 
	if (curr_node->fork())
	  {
	    MoveRight();
	    _branchToDestroy = _nodeAddr;
	    goto check_node;
	  }        
	else return false; 


      case Instruction::VarNFTag: 
	return false; 

      case Instruction::CompareTag:
	// same as below
      case Instruction::CompareNFTag:
	// same as below
      case Instruction::LeafTag:
	ICP("ICP0");
	return false;

#ifdef DEBUG_NAMESPACE
      default: ICP("X0"); return false;
#else 
#ifdef _SUPPRESS_WARNINGS_
      default: return false;
#endif
#endif        
      };  
  }; // bool FMCodeTreeNonadaptiveRemoval<MaxTermSize,MaxNumOfVariables,Alloc,Symbol,Flatterm,LeafInfo,InstanceId>::SkipFunc(const Symbol& func)

  template <ulong MaxTermSize,ulong MaxNumOfVariables,class Alloc,class Symbol,class Flatterm,class LeafInfo,class InstanceId> 
  inline bool FMCodeTreeNonadaptiveRemoval<MaxTermSize,MaxNumOfVariables,Alloc,Symbol,Flatterm,LeafInfo,InstanceId>::SkipComps()
  {
    CALL("SkipComps()");
    ASSERT(*_nodeAddr);
    const Flatterm*** remainingCompares = _compares.begin();
    Instruction* curr_node; 
#if (defined DEBUG_NAMESPACE) || (defined _SUPPRESS_WARNINGS_)
    curr_node = 0;
#endif

    while (remainingCompares < _compares.end())
      {
	const Flatterm** v1 = *remainingCompares;
	const Flatterm** v2 = *(remainingCompares + 1);
      
      check_tag:   
	curr_node = *_nodeAddr; 
	ASSERT(curr_node);
	switch (curr_node->tag())
	  {
	  case Instruction::FuncTag:
	    // same as below
	  case Instruction::FuncNFTag:
	    // same as below
	  case Instruction::VarTag:
	    // same as below
	  case Instruction::VarNFTag:
	    ICP("ICP0");
	    return false;

	  case Instruction::CompareTag:
	    if ((curr_node->asCompare()->var1() == v1) &&
		(curr_node->asCompare()->var2() == v2))
	      {
		if (curr_node->fork()) _branchToDestroy = _nodeAddr;
		MoveDown();
		ASSERT(*_nodeAddr);
		break;
	      }
	    else
	      {
		MoveRight();
		_branchToDestroy = _nodeAddr;
		if (*_nodeAddr) goto check_tag;
		return false;
	      };
	
	  case Instruction::CompareNFTag:
	    if ((curr_node->asCompareNF()->var1() == v1) &&
		(curr_node->asCompareNF()->var2() == v2))
	      {
		MoveDown();
		ASSERT(*_nodeAddr);
		break;
	      };
	    return false;
	      
	  case Instruction::LeafTag:
	    return false;

#ifdef DEBUG_NAMESPACE
	  default: ICP("X0"); return false;
#else 
#ifdef _SUPPRESS_WARNINGS_
	  default: return false;
#endif
#endif
	  }; // switch (curr_node->tag())	


	remainingCompares += 2;
      }; // while (remainingCompares < _compares.end())
  

  check_leaf:
    if ((*_nodeAddr)->tag() == Instruction::LeafTag)
      return true;
    ASSERT((*_nodeAddr)->isCompare());
    if ((*_nodeAddr)->tag() == Instruction::CompareTag)
      {
	if (!(*_nodeAddr)->fork()) return false;
	MoveRight();	
	_branchToDestroy = _nodeAddr;
	goto check_leaf;
      }
    else
      {
	ASSERT((*_nodeAddr)->tag() == Instruction::CompareNFTag);
	return false;
      };
  }; // bool FMCodeTreeNonadaptiveRemoval<MaxTermSize,MaxNumOfVariables,Alloc,Symbol,Flatterm,LeafInfo,InstanceId>::SkipComps()

}; // namespace Gem




//================================================
#undef DEBUG_NAMESPACE
#ifdef DEBUG_FORWARD_MATCHING_INDEX_TREE
#define DEBUG_NAMESPACE "FMCodeTreeNonadaptiveRetrieval<MaxTermSize,MaxNumOfVariables,Alloc,Symbol,Flatterm,LeafInfo,InstanceId>"
#endif
#include "debugMacros.hpp"
//=================================================

namespace Gem
{
  template <ulong MaxTermSize,ulong MaxNumOfVariables,class Alloc,class Symbol,class Flatterm,class LeafInfo,class InstanceId> 
  inline FMCodeTree<MaxTermSize,MaxNumOfVariables,Alloc,Symbol,Flatterm,LeafInfo,InstanceId>::Leaf* FMCodeTreeNonadaptiveRetrieval<MaxTermSize,MaxNumOfVariables,Alloc,Symbol,Flatterm,LeafInfo,InstanceId>::Match(const Flatterm* query,Instruction* tree)
  {
    CALL("Match(const Flatterm* query,Instruction* tree)");
    if (!tree) return 0;

    backtrack_nodes.reset();  
    backtrack_cursors.reset();
  
    const Flatterm* cursor = query;
    Instruction* curr_node = tree;
    Symbol curr_sym;

    COP("before check_node");

  check_node:
    COP("check_node");
    ASSERT(curr_node); 
    switch (curr_node->tag()) 
      {
      case Instruction::FuncTag:
	COP("Func");
	curr_sym = cursor->symbolRef();                 
	if (curr_sym.isVariable()) goto backtrack;   
      func_compare_sym:
	if (curr_sym == (static_cast<Func*>(curr_node))->symbol())
	  {
	    cursor = cursor->next();
	    curr_node = curr_node->next();
	    goto check_node;  
	  };      
	// different symbols
	if (curr_sym < (static_cast<Func*>(curr_node))->symbol()) goto backtrack;
	// here curr_sym > curr_node->symbol()     
	curr_node = curr_node->fork();
	if (!curr_node) goto backtrack;
	if (curr_node->tag() == Instruction::FuncNFTag)
	  goto funcNF_compare_sym;
	ASSERT(curr_node->tag() == Instruction::FuncTag);
	goto func_compare_sym;
	
      
      case Instruction::FuncNFTag:
	COP("FuncNF");
	curr_sym = cursor->symbolRef();                 
	if (curr_sym.isVariable()) goto backtrack;   
 
      funcNF_compare_sym:
	if (curr_sym == (static_cast<FuncNF*>(curr_node))->symbol())
	  {
	    cursor = cursor->next();
	    curr_node = curr_node->next();
	    goto check_node;  
	  };      
	// different symbols
	goto backtrack;

      case Instruction::VarTag:
	COP("Var");
	*((static_cast<Var*>(curr_node))->var()) = cursor;  
	if (curr_node->fork() && cursor->symbolRef().isNotVariable())
	  {
	    backtrack_nodes.push(curr_node->fork());
	    backtrack_cursors.push(cursor);
	  };
	cursor = cursor->after();
	curr_node = curr_node->next();
	goto check_node;     

      case Instruction::VarNFTag:
	COP("VarNF");
	*((static_cast<VarNF*>(curr_node))->var()) = cursor; 
	cursor = cursor->after();
	curr_node = curr_node->next();
	goto check_node;     


      case Instruction::CompareTag:
	COP("Compare");
	if ((*((static_cast<Compare*>(curr_node))->var1()))->equal(*((static_cast<Compare*>(curr_node))->var2())))
	  {
	    if (curr_node->fork())
	      {
		backtrack_nodes.push(curr_node->fork());
		backtrack_cursors.push(cursor);
	      };
	    curr_node = curr_node->next();
	    goto check_node;
	  }
	else // Compare failed, try fork
	  {
	    curr_node = curr_node->fork();
	    if (curr_node) goto check_node;
	    goto backtrack;
	  };

      case Instruction::CompareNFTag:
	COP("CompareNF");
	if ((*((static_cast<CompareNF*>(curr_node))->var1()))->equal(*((static_cast<CompareNF*>(curr_node))->var2())))
	  {
	    curr_node = curr_node->next();
	    goto check_node;
	  };
	goto backtrack;

      case Instruction::LeafTag:
	COP("Leaf"); 
	return (static_cast<Leaf*>(curr_node)); // success

#ifdef DEBUG_NAMESPACE
      default: COP("default1"); ICP("X0"); return 0;
#else
#ifdef _SUPPRESS_WARNINGS_
      default: COP("default2"); return 0;  
#endif
#endif
      };
 
    //*************** Backtrack section: **********
    COP("before bakctrack");
  backtrack:

    //DF; cout << "---------- BACKTRACK\n";

    COP("backtrack");
    BK::GlobalStopFlag::check();
    if (backtrack_nodes.empty())
      {   
	// failure
	return 0;
      };
    curr_node = backtrack_nodes.pop(); 
    cursor = backtrack_cursors.pop();
    goto check_node;
  }; // Leaf* FMCodeTreeNonadaptiveRetrieval<MaxTermSize,MaxNumOfVariables,Alloc,Symbol,Flatterm,LeafInfo,InstanceId>::Match(const Flatterm* query,Instruction* tree) 

  template <ulong MaxTermSize,ulong MaxNumOfVariables,class Alloc,class Symbol,class Flatterm,class LeafInfo,class InstanceId> 
  inline FMCodeTree<MaxTermSize,MaxNumOfVariables,Alloc,Symbol,Flatterm,LeafInfo,InstanceId>::Leaf* FMCodeTreeNonadaptiveRetrieval<MaxTermSize,MaxNumOfVariables,Alloc,Symbol,Flatterm,LeafInfo,InstanceId>::MatchAgain()
  {
    CALL("MatchAgain()");
    const Flatterm* cursor;
    Instruction* curr_node;
    Symbol curr_sym;

    goto backtrack;

 check_node:
    COP("check_node");
    ASSERT(curr_node); 
    switch (curr_node->tag()) 
      {
      case Instruction::FuncTag:
	COP("Func");
	curr_sym = cursor->symbolRef();                 
	if (curr_sym.isVariable()) goto backtrack;   
      func_compare_sym:
	if (curr_sym == (static_cast<Func*>(curr_node))->symbol())
	  {
	    cursor = cursor->next();
	    curr_node = curr_node->next();
	    goto check_node;  
	  };      
	// different symbols
	if (curr_sym < (static_cast<Func*>(curr_node))->symbol()) goto backtrack;
	// here curr_sym > curr_node->symbol()     
	curr_node = curr_node->fork();
	if (!curr_node) goto backtrack;
	if (curr_node->tag() == Instruction::FuncNFTag)
	  goto funcNF_compare_sym;
	ASSERT(curr_node->tag() == Instruction::FuncTag);
	goto func_compare_sym;
	
      
      case Instruction::FuncNFTag:
	COP("FuncNF");
	curr_sym = cursor->symbolRef();                 
	if (curr_sym.isVariable()) goto backtrack;   
 
      funcNF_compare_sym:
	if (curr_sym == (static_cast<FuncNF*>(curr_node))->symbol())
	  {
	    cursor = cursor->next();
	    curr_node = curr_node->next();
	    goto check_node;  
	  };      
	// different symbols
	goto backtrack;

      case Instruction::VarTag:
	COP("Var");
	*((static_cast<Var*>(curr_node))->var()) = cursor;  
	if (curr_node->fork() && cursor->symbolRef().isNotVariable())
	  {
	    backtrack_nodes.push(curr_node->fork());
	    backtrack_cursors.push(cursor);
	  };
	cursor = cursor->after();
	curr_node = curr_node->next();
	goto check_node;     

      case Instruction::VarNFTag:
	COP("VarNF");
	*((static_cast<VarNF*>(curr_node))->var()) = cursor; 
	cursor = cursor->after();
	curr_node = curr_node->next();
	goto check_node;     


      case Instruction::CompareTag:
	COP("Compare");
	if ((*((static_cast<Compare*>(curr_node))->var1()))->equal(*((static_cast<Compare*>(curr_node))->var2())))
	  {
	    if (curr_node->fork())
	      {
		backtrack_nodes.push(curr_node->fork());
		backtrack_cursors.push(cursor);
	      };
	    curr_node = curr_node->next();
	    goto check_node;
	  }
	else // Compare failed, try fork
	  {
	    curr_node = curr_node->fork();
	    if (curr_node) goto check_node;
	    goto backtrack;
	  };

      case Instruction::CompareNFTag:
	COP("CompareNF");
	if ((*((static_cast<CompareNF*>(curr_node))->var1()))->equal(*((static_cast<CompareNF*>(curr_node))->var2())))
	  {
	    curr_node = curr_node->next();
	    goto check_node;
	  };
	goto backtrack;

      case Instruction::LeafTag:
	COP("Leaf"); 
	return (static_cast<Leaf*>(curr_node)); // success

#ifdef DEBUG_NAMESPACE
      default: COP("default1"); ICP("X0"); return 0;
#else
#ifdef _SUPPRESS_WARNINGS_
      default: COP("default2"); return 0;  
#endif
#endif
      };

    //*************** Backtrack section: **********
    COP("before bakctrack");
  backtrack:

    // DF; cout << "---------- BACKTRACK\n";

    COP("backtrack");
    if (backtrack_nodes.empty())
      {
	// failure
	return 0;
      };
    curr_node = backtrack_nodes.pop(); 
    cursor = backtrack_cursors.pop();
    goto check_node;
  }; // Leaf* FMCodeTreeNonadaptiveRetrieval<MaxTermSize,MaxNumOfVariables,Alloc,Symbol,Flatterm,LeafInfo,InstanceId>::MatchAgain()

}; // namespace Gem



//=============================================================
#endif


