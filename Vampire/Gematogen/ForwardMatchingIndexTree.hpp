//
// File:         ForwardMatchingIndexTree.hpp
// Description:  Code trees for matching.
// Created:      Feb 15, 2000, 15:20
// Revised:      Jan 10, 2001
// Revised:      Jan 22, 2001 (made generic) 
// Author:       Alexandre Riazanov
// mail:         riazanov@cs.man.ac.uk
// Note:         This is a part of the Gematogen library. 
// Revised:      Apr 11, 2003, Alexandre Riazanov, riazanov@cs.man.ac.uk
//               Representation of instructions changed completely. 
// Revised:      Jun 01, 2003, Alexandre Riazanov, riazanov@cs.man.ac.uk
//               Some long definition are placed inside class declarations
//               to please VC++ 6.0. 
//====================================================
#ifndef FORWARD_MATCHING_INDEX_TREE_H
//====================================================
#define FORWARD_MATCHING_INDEX_TREE_H
#ifndef NO_DEBUG_VIS 
#include <iostream>
#endif
#include <climits>
#include "jargon.hpp"
#ifdef _SUPPRESS_WARNINGS_
#include "RuntimeError.hpp"
#endif
#include "GlobalStopFlag.hpp"
#include "GematogenDebugFlags.hpp"
#include "Stack.hpp"
#include "EquivalenceRelation.hpp"
#include "MultisetOfVariables.hpp"
#include "DestructionMode.hpp"
#include "RuntimeError.hpp"
using namespace std;
//================================================
#undef DEBUG_NAMESPACE
#ifdef DEBUG_FORWARD_MATCHING_INDEX_TREE
#define DEBUG_NAMESPACE "FMCodeTree<MaxTermSize,MaxNumOfVariables,Alloc,Symbol,Flatterm,LeafInfo,InstanceId>"
#endif
#include "debugMacros.hpp"
//=================================================


namespace Gem
{
  template <ulong MaxTermSize,ulong MaxNumOfVariables,class Alloc,class Symbol,class Flatterm,class LeafInfo,class InstanceId> 
  class FMCodeTree
  {
  public:
    
    class InstructionWithNext;
    class InstructionWithFork;
    class Func;
    class FuncNF;
    class Var;
    class VarNF;
    class Compare;
    class CompareNF;
    class Leaf;

    class Instruction
    {
    public:
      enum Tag 
      { 
	FuncTag, 
	FuncNFTag,
	VarTag, 
	VarNFTag, 
	CompareTag,
	CompareNFTag, 
	LeafTag 
      };
    public:
      Instruction(Tag t)  : _tag(t) {};
      ~Instruction() {};

      Tag tag() const { return _tag; };


#ifndef NO_DEBUG
      bool isLeaf() const { return _tag == LeafTag; };
      bool isCompare() const { return (_tag == CompareTag) || (_tag == CompareNFTag); };
#endif  

#if (defined DEBUG_NAMESPACE) || (!(defined NO_DEBUG_VIS)) 
      bool hasNext() const { return !isLeaf(); };
      bool hasFork() const 
      { 
	return (_tag == FuncTag) ||
	  (_tag == VarTag) ||
	  (_tag == CompareTag);
      };
#endif

      Instruction* next() const
      {
	CALL("next() const");
	ASSERT(hasNext());
	return (static_cast<const InstructionWithNext*>(this))->next();
      };
  
      Instruction** nextAddr() 
      {
	CALL("nextAddr()");
	ASSERT(hasNext());
	return (static_cast<InstructionWithNext*>(this))->nextAddr();
      };

      void setNext(Instruction* instr) 
      {
	CALL("setNext(Instruction* instr)");
	ASSERT(hasNext());
	(static_cast<InstructionWithNext*>(this))->setNext(instr);
      };



      Instruction* fork() const
      {
	CALL("fork() const");
	ASSERT(hasFork());
	return (static_cast<const InstructionWithFork*>(this))->fork();
      };
  
      Instruction** forkAddr() 
      {
	CALL("forkAddr()");
	ASSERT(hasFork());
	return (static_cast<InstructionWithFork*>(this))->forkAddr();
      };

      void setFork(Instruction* instr) 
      {
	CALL("setFork(Instruction* instr)");
	ASSERT(hasFork());
	(static_cast<InstructionWithFork*>(this))->setFork(instr);
      };

      InstructionWithFork* asInstrWithFork()
      {
	CALL("asInstrWithFork()");
	ASSERT(hasFork());
	return static_cast<InstructionWithFork*>(this);
      };

      const InstructionWithFork* asInstrWithFork() const 
      {
	CALL("asInstrWithFork() const");
	ASSERT(hasFork());
	return static_cast<const InstructionWithFork*>(this);
      };


      Func* asFunc() 
      { 
	CALL("asFunc()");
	ASSERT(tag() == FuncTag);
	return static_cast<Func*>(this); 
      };  
      const Func* asFunc() const  
      { 
	CALL("asFunc() const");
	ASSERT(tag() == FuncTag);
	return static_cast<const Func*>(this); 
      };
      FuncNF* asFuncNF() 
      { 
	CALL("asFuncNF()");
	ASSERT(tag() == FuncNFTag);
	return static_cast<FuncNF*>(this); 
      };
      const FuncNF* asFuncNF() const 
      { 
	CALL("asFuncNF() const");
	ASSERT(tag() == FuncNFTag);
	return static_cast<const FuncNF*>(this); 
      };
      Var* asVar()  
      {  
	CALL("asVar()");
	ASSERT(tag() == VarTag);
	return static_cast<Var*>(this);  
      };
      const Var* asVar() const  
      {  
	CALL("asVar() const");
	ASSERT(tag() == VarTag);
	return static_cast<const Var*>(this);  
      };
      VarNF* asVarNF()  
      {  
	CALL("asVarNF()");
	ASSERT(tag() == VarNFTag);
	return static_cast<VarNF*>(this);  
      };
      const VarNF* asVarNF() const
      {  
	CALL("asVarNF() const");
	ASSERT(tag() == VarNFTag);
	return static_cast<const VarNF*>(this);  
      };
      Compare* asCompare()  
      {  
	CALL("asCompare()");
	ASSERT(tag() == CompareTag);
	return static_cast<Compare*>(this);  
      };
      const Compare* asCompare() const 
      {  
	CALL("asCompare() const");
	ASSERT(tag() == CompareTag);
	return static_cast<const Compare*>(this);  
      };
      CompareNF* asCompareNF()  
      {  
	CALL("asCompareNF()");
	ASSERT(tag() == CompareNFTag);
	return static_cast<CompareNF*>(this);  
      };
      const CompareNF* asCompareNF()  const 
      {  
	CALL("asCompareNF() const");
	ASSERT(tag() == CompareNFTag);
	return static_cast<const CompareNF*>(this);  
      };
  
  
      Leaf* asLeaf()  
      {  
	CALL("asLeaf()");
	ASSERT(tag() == LeafTag);
	return static_cast<Leaf*>(this);  
      };
      const Leaf* asLeaf()  const 
      {  
	CALL("asLeaf() const");
	ASSERT(tag() == LeafTag);
	return static_cast<const Leaf*>(this);  
      };
  

    public:
#ifndef NO_DEBUG_VIS 
      ostream& output(ostream& str,const Flatterm* const * subst) const
      {
	CALL("output(ostream& str,const Flatterm* const * subst) const");
	str << '[' << (ulong)this << "] ";
	switch (tag())
	  { 
	  case FuncTag: str << "Func " << asFunc()->symbol(); break; 
	  case FuncNFTag: str << "FuncNF " << asFuncNF()->symbol(); break; 
	  case VarTag: str << "Var " << asVar()->var() - subst; break;
	  case VarNFTag: str << "VarNF " << asVarNF()->var() - subst; break;
	  case CompareTag: str << "Compare " 
			       << asCompare()->var1() - subst << ' ' 
			       << asCompare()->var2() - subst; break;
	  case CompareNFTag: str << "CompareNF " 
				 << asCompareNF()->var1() - subst << ' ' 
				 << asCompareNF()->var2() - subst; break;
	  case LeafTag: str << "Leaf "; break;
	  };
	if (hasNext()) str << " n[" << (ulong)next() << "] ";
	if (hasFork()) str << " f[" << (ulong)fork() << ']'; 
	return str;
      }; // ostream& output(const Flatterm* const * subst) const


#endif

    private:
      Instruction()
      {
	CALL("constructor Instruction()");
	ICP("ICP0");
      };
      void* operator new(size_t)
      {
	CALL("operator new(size_t)");
	ICP("ICP0");
	return 0;
      };
    public: void operator delete(void* obj) // POD
      {
	CALL("operator delete(void* obj)");
	ICP("ICP0");
      }; 
    private: // data
      Tag _tag;
    }; // class Instruction




    class InstructionWithNext : 
      public Instruction
    {
    public:
      InstructionWithNext(Tag t)  : Instruction(t), _next(0) {};
      Instruction** nextAddr() { return &_next; };
      Instruction* next() const { return _next; };
      void setNext(Instruction* instr) { _next = instr; };
    private:
      InstructionWithNext()
      {
	CALL("constructor InstructionWithNext()");
	ICP("ICP0");
      };
    private: 
      Instruction* _next;
    }; // class InstructionWithNext




    class InstructionWithFork : 
      public InstructionWithNext
    {
    public:
      InstructionWithFork(Tag t)  : InstructionWithNext(t), _fork(0) {};
      Instruction** forkAddr() { return &_fork; };
      Instruction* fork() const { return _fork; };
      void setFork(Instruction* instr) { _fork = instr; };
    private:
      InstructionWithFork()
      {
	CALL("constructor InstructionWithFork()");
	ICP("ICP0");
      };
    private: 
      Instruction* _fork;
    }; // class InstructionWithFork




    class Func : public InstructionWithFork
    {
    public:
      Func(const Symbol& f) : InstructionWithFork(FuncTag), _symbol(f) {};
      const Symbol& symbol() const { return _symbol; };
      void* operator new(size_t)
      {
	return Alloc::allocate(sizeof(Func));
      };
      void operator delete(void* obj)
      {
	Alloc::deallocate(obj,sizeof(Func));
      };

    private:
      Func()
      {
	CALL("constructor Func()");
	ICP("ICP0");
      };

    private:
      Symbol _symbol;
    }; // class Func


    class FuncNF : public InstructionWithNext
    {
    public:
      FuncNF(const Symbol& f) : InstructionWithNext(FuncNFTag), _symbol(f) {};
      const Symbol& symbol() const { return _symbol; };

      void* operator new(size_t)
      {
	return Alloc::allocate(sizeof(FuncNF));
      };
      void operator delete(void* obj)
      {
	Alloc::deallocate(obj,sizeof(FuncNF));
      };

      Func* copyWithFork(Instruction* frk) 
      {
	CALL("copyWithFork(Instruction* frk)");
	Func* res = new Func(_symbol);
	res->setNext(next());
	res->setFork(frk);
	return res;
      };

    private:
      FuncNF()
      {
	CALL("constructor FuncNF()");
	ICP("ICP0");
      };

    private:
      Symbol _symbol;
    }; // class FuncNF





    class Var : public InstructionWithFork
    {
    public:
      Var(const Flatterm** v) : InstructionWithFork(VarTag), _var(v) {};
      const Flatterm** var() const { return _var; };
      void* operator new(size_t)
      {
	return Alloc::allocate(sizeof(Var));
      };
      void operator delete(void* obj)
      {
	Alloc::deallocate(obj,sizeof(Var));
      };


    private:
      Var()
      {
	CALL("constructor Var()");
	ICP("ICP0");
      };

    private:
      const Flatterm** _var;
    }; // class Var




    class VarNF : public InstructionWithNext
    {
    public:
      VarNF(const Flatterm** v) : InstructionWithNext(VarNFTag), _var(v) {};
      const Flatterm** var() const { return _var; };
      void* operator new(size_t)
      {
	return Alloc::allocate(sizeof(VarNF));
      };
      void operator delete(void* obj)
      {
	Alloc::deallocate(obj,sizeof(VarNF));
      };


      Var* copyWithFork(Instruction* frk) 
      {
	CALL("copyWithFork(Instruction* frk)");
	Var* res = new Var(_var);
	res->setNext(next());
	res->setFork(frk);
	return res;
      };

    private:
      VarNF()
      {
	CALL("constructor VarNF()");
	ICP("ICP0");
      };

    private:
      const Flatterm** _var;
    }; // class VarNF





    class Compare : public InstructionWithFork
    {
    public:
      Compare(const Flatterm** v1,const Flatterm** v2) : 
	InstructionWithFork(CompareTag), 
	_var1(v1), 
	_var2(v2)
      {
      };
      const Flatterm** var1() const { return _var1; };
      const Flatterm** var2() const { return _var2; };
      void setVar1(const Flatterm** v) { _var1 = v; };
      void setVar2(const Flatterm** v) { _var2 = v; };
      void* operator new(size_t)
      {
	return Alloc::allocate(sizeof(Compare));
      };
      void operator delete(void* obj)
      {
	Alloc::deallocate(obj,sizeof(Compare));
      };

    private:
      Compare()
      {
	CALL("constructor Compare()");
	ICP("ICP0");
      };

    private:
      const Flatterm** _var1;
      const Flatterm** _var2;
    }; // class Compare

    class CompareNF : public InstructionWithNext
    {
    public:
      CompareNF(const Flatterm** v1,const Flatterm** v2) : 
	InstructionWithNext(CompareNFTag), 
	_var1(v1), 
	_var2(v2)
      {
      };
      const Flatterm** var1() const { return _var1; };
      const Flatterm** var2() const { return _var2; };
      void setVar1(const Flatterm** v) { _var1 = v; };
      void setVar2(const Flatterm** v) { _var2 = v; };
      void* operator new(size_t)
      {
	return Alloc::allocate(sizeof(CompareNF));
      };
      void operator delete(void* obj)
      {
	Alloc::deallocate(obj,sizeof(CompareNF));
      };

      Compare* copyWithFork(Instruction* frk) 
      {
	CALL("copyWithFork(Instruction* frk)");
	Compare* res = new Compare(_var1,_var2);
	res->setNext(next());
	res->setFork(frk);
	return res;
      };

    private:
      CompareNF()
      {
	CALL("constructor CompareNF()");
	ICP("ICP0");
      };

    private:
      const Flatterm** _var1;
      const Flatterm** _var2;
    }; // class CompareNF



    class Leaf : public Instruction
    {
    public:
      Leaf() : Instruction(LeafTag) {};
      ~Leaf() {};
      const LeafInfo& info() const { return _info; };
      LeafInfo& info() { return _info; };
      void* operator new(size_t)
      {
	return Alloc::allocate(sizeof(Leaf));
      };
      void operator delete(void* obj)
      {
	Alloc::deallocate(obj,sizeof(Leaf));
      };

    private:
      LeafInfo _info;
    }; // class Leaf





    class Integrator 
    // very costly constructor!
    {
    public:
      Integrator(const Flatterm** subst)
	: _constrToInsert("Integrator::_constrToInsert"),
	  _constrToShift("Integrator::_constrToShift"),
	  _constrPassed("Integrator::_constrPassed"),
	  _constrPassedInShift("Integrator::_constrPassedInShift"),
	  _constrIntegrated("Integrator::_constrIntegrated"),
	  _constrOld("Integrator::_constrOld"),
	  _matchingIndexSubstitution(subst),
	  _registeredVariables() 
      {
	CALL("constructor Integrator(const Flatterm** subst)");
      };

      ~Integrator() 
      {
	CALL("destructor ~Integrator()");
      };

      void init(const Flatterm** subst)
      {
	CALL("init(const Flatterm** subst)");
	_constrToInsert.init("_constrToInsert");
	_constrToShift.init("_constrToShift");
	_constrPassed.init("_constrPassed");
	_constrPassedInShift.init("_constrPassedInShift");
	_constrIntegrated.init("_constrIntegrated");
	_constrOld.init("_constrOld");
	_matchingIndexSubstitution = subst;
	_registeredVariables.init();
      }; // void init(const Flatterm** subst)

      void destroy()
      {
	CALL("destroy()");
	_registeredVariables.destroy();
	_constrOld.destroy();
	_constrIntegrated.destroy();
	_constrPassedInShift.destroy();
	_constrPassed.destroy();
	_constrToShift.destroy();
	_constrToInsert.destroy();
	BK_CORRUPT(*this);
      }; // void destroy()


      void Reset(Instruction** n)
      {
	_nodeAddr = n;
	_constrToInsert.Reset();
	_constrToShift.Reset();
	_constrPassed.Reset();
	//_constrPassedInShift.Reset();
	_constrIntegrated.Reset();
	_constrOld.Reset();
	_registeredVariables.Reset();
	_currentMatchingIndexVar = _matchingIndexSubstitution;
	_readModeFlag = ((*_nodeAddr) != 0);
	_currentDepth = 0UL;
	DOP(_debugNonlinearity = 0UL);
	DOP(_debugNumOfPassedCompares = 0UL);
      };

      BK::MultisetOfVariables<MaxNumOfVariables>& Vars() { return _registeredVariables; };

      const Flatterm** MapVar(ulong v) const { return _firstVariableOccurence[v]; }; 

      void Push(const Symbol& sym)
      { 
	if (sym.isVariable()) { PushVar(sym); } else PushFunc(sym); 
      };  

      void PushVar(const Symbol& sym)
      {  
	CALL("PushVar(const Symbol& sym)");
	ulong var = sym.var();
	ASSERT(var < MaxNumOfVariables); 

	if (_readModeFlag)
	  {
	    _readModeFlag = ReadVar();
	    ASSERT(debugInSubst(_currentMatchingIndexVar));
	    if (_readModeFlag)
	      {
		if (_registeredVariables.Occurences(var)) 
		  { 
		    DOP(++_debugNonlinearity);
		    ReadCompare(_firstVariableOccurence[var],_currentMatchingIndexVar); 
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
		    DOP(++_debugNonlinearity);
		    EnterWriteMode();
		    WriteCompare(_firstVariableOccurence[var],_currentMatchingIndexVar);
		  } 
		else // first occurence of the variable
		  {
		    _registeredVariables.Register(var);
		    _firstVariableOccurence[var] = _currentMatchingIndexVar;
		    EnterWriteMode();
		  };     
	      };
	    _currentMatchingIndexVar++;
	  }
	else // write mode
	  WriteVar(var);          
      }; // void PushVar(const Symbol& sym)

      void PushFunc(const Symbol& sym)
      {
	CALL("PushFunc(const Symbol& sym)");
	if (_readModeFlag)
	  {
	    _readModeFlag = ReadFunc(sym);
	    if (!_readModeFlag) 
	      {
		InsertFunc(sym);
		EnterWriteMode(); 
	      }; 
	  }
	else // write mode
	  {
	    WriteFunc(sym);
	  };   
      }; // void PushFunc(const Symbol& sym)


      Leaf* GetLeaf(bool& newLeaf)
      {
	CALL("GetLeaf()");
	_readModeFlag = _readModeFlag && ReadCollectedComps(); 
	newLeaf = !_readModeFlag;       
	if (_readModeFlag) 
	  {
	    ASSERT((*_nodeAddr)->isLeaf());
	    return (*_nodeAddr)->asLeaf();
	  };
	// write mode


	ASSERT(_debugNonlinearity == _debugNumOfPassedCompares);
      
	ASSERT(!(*_nodeAddr));
	*_nodeAddr = new Leaf();
	return (*_nodeAddr)->asLeaf();
      };
 
    private: 
      typedef BK::EquivalenceRelation<MaxTermSize> EqualityConstraint;

    private:
      void MoveDown() 
      { 
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
#ifdef DEBUG_NAMESPACE
      bool debugInSubst(const Flatterm** var) const 
      { 
	return (var >= _matchingIndexSubstitution) && (var < (_matchingIndexSubstitution + MaxTermSize));
      }; 
#endif

      void MakeShifted(const ulong& v1,const ulong& v2) 
      {
	CALL("MakeShifted(const ulong& v1,const ulong& v2)");
	ASSERT(v1 < v2);
	ASSERT(!_constrIntegrated.Equivalent(v1,v2));
	_constrToShift.MakeEquivalent(v1,v2);
      };

      void MakeIntegrated(const ulong& v1,const ulong& v2) 
      {
	CALL("MakeIntegrated(const ulong& v1,const ulong& v2)");
	ASSERT(v1 < v2);
	ASSERT(!_constrIntegrated.Equivalent(v1,v2));
	ASSERT(!_constrPassed.Equivalent(v1,v2));
	ASSERT(!_constrOld.Equivalent(v1,v2));
	_constrIntegrated.MakeEquivalent(v1,v2);
      };    

#ifdef DEBUG_NAMESPACE
      void CheckConstrIntegrity()
      { 
	CALL("CheckConstrIntegrity()");
	ASSERT(_constrToInsert.Implies(_constrIntegrated));      
	ASSERT(_constrPassed.Implies(_constrIntegrated));   
	ASSERT(_constrToShift.Implies(_constrOld));        
	ASSERT(_constrPassed.Implies(_constrOld));
	static EqualityConstraint debugER("debugER");  
	debugER.MakeComposition(_constrToShift,_constrPassed);
	ASSERT(_constrOld.Implies(debugER));
	debugER.MakeComposition(_constrToInsert,_constrPassed);
	ASSERT(_constrIntegrated.Implies(debugER));
      }; //void CheckConstrIntegrity() 
#endif 



      void PassUpperComp(CompareNF* instr)
      {
	// Trying to pass a CompareNF which is between some 
	// skeleton instructions. 
	CALL("PassUpperComp(CompareNF* instr)");
	DOP(CheckConstrIntegrity());
	const Flatterm** var1 = instr->var1();
	const Flatterm** var2 = instr->var2();
	const ulong var_num1 = VarNum(var1);
	const ulong var_num2 = VarNum(var2);

	ASSERT(!_constrToShift.Equivalent(var_num1,var_num2));
	ASSERT(!_constrOld.Equivalent(var_num1,var_num2));
	ASSERT(!_constrPassed.Equivalent(var_num1,var_num2));
   
	bool shared = _constrIntegrated.Equivalent(var_num1,var_num2);
    
	if (shared)
	  {
	    // the current instruction is shared
	    _constrPassed.MakeEquivalent(var_num1,var_num2);
	    DOP(++_debugNumOfPassedCompares);
	    _nodeAddr = instr->nextAddr();
	    ++_currentDepth;
	  }
	else // ! _constrIntegrated.Equivalent(var_num1,var_num2)
	  {
	    // the instruction itself must be shifted unless it can be replaced by an equivalent one 
	    *_nodeAddr = instr->next();
	    delete instr;
	  };

	Instruction* tail = *_nodeAddr;
	ASSERT(tail);
	EqualityConstraint::MergedPairs mergedPairs(_constrOld,var_num1,var_num2);
	ulong merged1;
	ulong merged2;

	while (mergedPairs.nextPair(merged1,merged2))
	  {
	    ASSERT(merged1 < merged2);     
	    ASSERT(!_constrOld.Equivalent(merged1,merged2));
	    if (_constrIntegrated.Equivalent(merged1,merged2) && (!_constrPassed.Equivalent(merged1,merged2)))
	      {
		// another Compare can be shared	 
		shared = true;
		_constrPassed.MakeEquivalent(merged1,merged2);
		DOP(++_debugNumOfPassedCompares);
		*_nodeAddr = new CompareNF(nativeVar(merged1),nativeVar(merged2));
		(*_nodeAddr)->setNext(tail);
		MoveDown();
	      };
	  };    
  
	_constrOld.MakeEquivalent(var_num1,var_num2);
	if (!shared) MakeShifted(var_num1,var_num2);
	ASSERT((*_nodeAddr) == tail);
	DOP(CheckConstrIntegrity());
      }; // void PassUpperComp(CompareNF* instr) 


      bool PassCompWF(Compare* instr)
      {
	// Trying to pass a Compare instruction that have a fork
	CALL("PassCompWF(Compare* instr)");
	ASSERT(_readModeFlag);
	const Flatterm** var1 = instr->var1();
	const Flatterm** var2 = instr->var2();
	ulong var_num1 = VarNum(var1);
	ulong var_num2 = VarNum(var2);
	ASSERT(!_constrToShift.Equivalent(var_num1,var_num2));
	ASSERT(!_constrOld.Equivalent(var_num1,var_num2)); 
	ASSERT(!_constrPassed.Equivalent(var_num1,var_num2));

	ASSERT((*_nodeAddr) == instr);

	ASSERT(instr->next());

	if (_constrIntegrated.Equivalent(var_num1,var_num2))
	  {
	    // this instruction can be shared
	    goto shift_and_insert_shared;      
	  }
	else // try to replace this Compare by an equivalent one which is also in _constrIntegrated
	  {
	    EqualityConstraint::MergedPairs mergedPairs(_constrOld,var_num1,var_num2);
	    ulong merged1;
	    ulong merged2; 
	    while (mergedPairs.nextPair(merged1,merged2))
	      {
		ASSERT(merged1 < merged2);     
		ASSERT(!_constrOld.Equivalent(merged1,merged2));
		if (_constrIntegrated.Equivalent(merged1,merged2) && (!_constrPassed.Equivalent(merged1,merged2)))
		  {
		    // appropriate replacement found
		    var_num1 = merged1;
		    var_num2 = merged2;
		    // modify the arguments of instr
		    instr->setVar1(nativeVar(var_num1));
		    instr->setVar2(nativeVar(var_num2));        
		    goto shift_and_insert_shared;
		  };
	      };
      
	    // no appropriate replacement found, must follow the fork
	    // if a fork exists.

	    if (!_constrToShift.Identity())
	      {
		_constrPassedInShift = _constrPassed;
		_constrPassedInShift.MakeEquivalent(var_num1,var_num2);
		CopyCompsToShift(instr->nextAddr());
	      };
	    MoveRightFrom(instr->asInstrWithFork());
	    DOP(CheckConstrIntegrity());
	    return false;
	  };

      shift_and_insert_shared:
	COP("shift_and_insert_shared");
	// make necessary shifts into the fork
	if (instr->fork()) 
	  {
	    if (!_constrToShift.Identity())
	      {
		_constrPassedInShift = _constrPassed; 
		CopyCompsToShift(instr->forkAddr());  
	      };      
	  };
	ASSERT(instr == (*_nodeAddr));
	_constrPassed.MakeEquivalent(var_num1,var_num2);
	DOP(++_debugNumOfPassedCompares);
	MoveDown();

	Instruction* tail = *_nodeAddr; 
	ASSERT(tail);

	// now find all Compares that can be shared due to (var_num1 ~ var_num2) added to _constrOld
 
    
	EqualityConstraint::MergedPairs mergedPairs(_constrOld,var_num1,var_num2);
	ulong merged1;
	ulong merged2; 

	while (mergedPairs.nextPair(merged1,merged2))
	  {
	    ASSERT(merged1 < merged2);     
	    ASSERT(!_constrOld.Equivalent(merged1,merged2));

	    if (_constrIntegrated.Equivalent(merged1,merged2) && (!_constrPassed.Equivalent(merged1,merged2)))
	      {
		// another Compare can be shared	
		_constrPassed.MakeEquivalent(merged1,merged2);	  
		DOP(++_debugNumOfPassedCompares);
		*_nodeAddr = new CompareNF(nativeVar(merged1),nativeVar(merged2));
		(*_nodeAddr)->setNext(tail);
		MoveDown();
	      };
	  };    

	ASSERT((*_nodeAddr) == tail);

	_constrOld.MakeEquivalent(var_num1,var_num2); 

	COP("end");
	DOP(CheckConstrIntegrity());
	ASSERT(*_nodeAddr);
	return true;
      }; // bool PassCompWF(Compare* instr)



      bool PassCompNF(CompareNF* instr)
      {
	// Trying to pass a CompareNF instruction
	CALL("PassCompNF(CompareNF* instr)");
	ASSERT(_readModeFlag);
	const Flatterm** var1 = instr->var1();
	const Flatterm** var2 = instr->var2();
	ulong var_num1 = VarNum(var1);
	ulong var_num2 = VarNum(var2);
	ASSERT(!_constrToShift.Equivalent(var_num1,var_num2));
	ASSERT(!_constrOld.Equivalent(var_num1,var_num2)); 
	ASSERT(!_constrPassed.Equivalent(var_num1,var_num2));

	ASSERT((*_nodeAddr) == instr);

	ASSERT(instr->next());

	if (_constrIntegrated.Equivalent(var_num1,var_num2))
	  {
	    // this instruction can be shared
	    goto shift_and_insert_shared;      
	  }
	else // try to replace this Compare/CompareNF by an equivalent one which is also in _constrIntegrated
	  {
	    EqualityConstraint::MergedPairs mergedPairs(_constrOld,var_num1,var_num2);
	    ulong merged1;
	    ulong merged2; 
	    while (mergedPairs.nextPair(merged1,merged2))
	      {
		ASSERT(merged1 < merged2);     
		ASSERT(!_constrOld.Equivalent(merged1,merged2));
		if (_constrIntegrated.Equivalent(merged1,merged2) && (!_constrPassed.Equivalent(merged1,merged2)))
		  {
		    // appropriate replacement found
		    var_num1 = merged1;
		    var_num2 = merged2;
		    // modify the arguments of instr
		    instr->setVar1(nativeVar(var_num1));
		    instr->setVar2(nativeVar(var_num2));        
		    goto shift_and_insert_shared;
		  };
	      };
      
	    // no appropriate replacement found, must follow the fork
	    // if a fork exists.

	    if (!_constrToShift.Identity())
	      {
		_constrPassedInShift = _constrPassed;
		_constrPassedInShift.MakeEquivalent(var_num1,var_num2);
		CopyCompsToShift(instr->nextAddr());
	      };
	    MoveRightFrom(instr);
	    DOP(CheckConstrIntegrity());
	    return false;
	  };

      shift_and_insert_shared:
	COP("shift_and_insert_shared");
	ASSERT(instr == (*_nodeAddr));
	_constrPassed.MakeEquivalent(var_num1,var_num2);	  
	DOP(++_debugNumOfPassedCompares);
	MoveDown();

	Instruction* tail = *_nodeAddr;
	ASSERT(tail);
	// now find all Compares that can be shared due to (var_num1 ~ var_num2) added to _constrOld
     
	EqualityConstraint::MergedPairs mergedPairs(_constrOld,var_num1,var_num2);
	ulong merged1;
	ulong merged2; 

	while (mergedPairs.nextPair(merged1,merged2))
	  {
	    ASSERT(merged1 < merged2);     
	    ASSERT(!_constrOld.Equivalent(merged1,merged2));

	    if (_constrIntegrated.Equivalent(merged1,merged2) && (!_constrPassed.Equivalent(merged1,merged2)))
	      {
		// another Compare can be shared	
		_constrPassed.MakeEquivalent(merged1,merged2);
		*_nodeAddr = new CompareNF(nativeVar(merged1),nativeVar(merged2));
		(*_nodeAddr)->setNext(tail);
		MoveDown();
	      };
	  };    

	ASSERT((*_nodeAddr) == tail);

	_constrOld.MakeEquivalent(var_num1,var_num2); 

	COP("end");
	DOP(CheckConstrIntegrity()); 
	ASSERT(*_nodeAddr);    
	return true;
      }; // bool PassCompNF(CompareNF* instr)

      // Read mode
    private:

      bool ReadFunc(const Symbol& sym)
      { 
	CALL("ReadFunc(const Symbol& sym)")
	  Instruction* curr_node;  
      check_tag:   
	curr_node = *_nodeAddr; 
	ASSERT(curr_node);
	switch (curr_node->tag())
	  {
	  case Instruction::FuncTag:  
	    if (curr_node->asFunc()->symbol() == sym)
	      {
		DOP(CheckConstrIntegrity());   
		if (curr_node->fork()) 
		  {
		    if (!_constrToShift.Identity())  
		      {       
			_constrPassedInShift = _constrPassed; 
			CopyCompsToShift(curr_node->forkAddr());
		      };
		  };
		MoveDown();
		return true;
	      };
	    // different functor
	    if (curr_node->asFunc()->symbol() > sym)
	      {
		if (!_constrToShift.Identity())  
		  {
		    _constrPassedInShift = _constrPassed;
		    InsertCompsToShift(_nodeAddr);
		  };
		return false;
	      };
	    // curr_node->symbol() < sym 
	    // mismatch occured, try fork
	    DOP(CheckConstrIntegrity());
	    if (curr_node->fork())      
	      {
		if (!_constrToShift.Identity())
		  {
		    _constrPassedInShift = _constrPassed;  
		    CopyCompsToShift(curr_node->nextAddr());
		  };
		MoveRightFrom(curr_node->asInstrWithFork());
		goto check_tag;
	      };
	    // no more forks

	    if (!_constrToShift.Identity())
	      {
		_constrPassedInShift = _constrPassed; 
		InsertCompsToShift(curr_node->nextAddr());
	      };
	    MoveRightFrom(curr_node->asInstrWithFork());
	    return false;

	  case Instruction::FuncNFTag:  
	    if (curr_node->asFuncNF()->symbol() == sym)
	      {
		MoveDown();
		return true;
	      };
	    // different functor
	    if (curr_node->asFuncNF()->symbol() > sym)
	      {
		if (!_constrToShift.Identity())  
		  {
		    _constrPassedInShift = _constrPassed;
		    InsertCompsToShift(_nodeAddr);
		  };
		return false;
	      };
	    // curr_node->symbol() < sym 
	    // mismatch occured

	    if (!_constrToShift.Identity())
	      {
		_constrPassedInShift = _constrPassed; 
		InsertCompsToShift(curr_node->nextAddr());
	      };
	    MoveRightFrom(curr_node->asFuncNF());
	    return false; 


	  case Instruction::VarTag:
	    // mismatch occured, try fork
	    DOP(CheckConstrIntegrity());
	    if (curr_node->fork())      
	      {
		if (!_constrToShift.Identity())
		  {
		    _constrPassedInShift = _constrPassed;  
		    CopyCompsToShift(curr_node->nextAddr());
		  };
		MoveRightFrom(curr_node->asInstrWithFork());
		goto check_tag;
	      };
	    // no more forks

	    if (!_constrToShift.Identity())
	      {
		_constrPassedInShift = _constrPassed; 
		InsertCompsToShift(curr_node->nextAddr());
	      };
	    MoveRightFrom(curr_node->asInstrWithFork());
	    return false;

	  case Instruction::VarNFTag:
	    // mismatch occured
	    if (!_constrToShift.Identity())
	      {
		_constrPassedInShift = _constrPassed; 
		InsertCompsToShift(curr_node->nextAddr());
	      };
	    MoveRightFrom(curr_node->asVarNF());
	    return false;
 
	  case Instruction::CompareTag:
	    ICP("ICP0");
	    return false;

	  case Instruction::CompareNFTag:
	    PassUpperComp(curr_node->asCompareNF());
	    goto check_tag;

#ifdef DEBUG_NAMESPACE
	  default: ICP("X0"); return false;
#else 
#ifdef _SUPPRESS_WARNINGS_
	  default: return false;
#endif
#endif
	  };
      }; // bool ReadFunc(const Symbol& sym)

      bool ReadVar()
      {
	CALL("ReadVar()");
	Instruction* curr_node;
      check_tag:
	curr_node = *_nodeAddr;        
	switch (curr_node->tag())
	  {
	  case Instruction::FuncTag: 
	    // same as below
	  case Instruction::FuncNFTag:
	    // no functor can preceed variable in a tree
	    if (!_constrToShift.Identity())
	      {
		_constrPassedInShift = _constrPassed;
		InsertCompsToShift(_nodeAddr);
	      };   
	    return false; 

	  case Instruction::VarTag:
	    DOP(CheckConstrIntegrity());
	    if (curr_node->fork()) 
	      {
		if (!_constrToShift.Identity())
		  {
		    _constrPassedInShift = _constrPassed; 
		    CopyCompsToShift(curr_node->forkAddr()); 
		  };
	      };              
	    MoveDown();
	    return true;

	  case Instruction::VarNFTag:
	    MoveDown();
	    return true;

	  case Instruction::CompareTag:
	    ICP("ICP0");
	    return false;

	  case Instruction::CompareNFTag:
	    PassUpperComp(curr_node->asCompareNF());
	    goto check_tag;

#ifdef DEBUG_NAMESPACE
	  default: ICP("X0"); return false;
#else 
#ifdef _SUPPRESS_WARNINGS_
	  default: return false;
#endif
#endif
	  };  
      }; // bool ReadVar() 

      void ReadCompare(const Flatterm** v1,const Flatterm** v2)
      {
	CALL("ReadCompare(const Flatterm** v1,const Flatterm** v2)");
	ASSERT(v1<v2);  
	ASSERT(debugInSubst(v1));
	ASSERT(debugInSubst(v2));
	ulong var_num1 = VarNum(v1);
	ulong var_num2 = VarNum(v2);
	MakeIntegrated(var_num1,var_num2);
	_constrToInsert.MakeEquivalent(var_num1,var_num2);  
	DOP(CheckConstrIntegrity());
      }; // void ReadCompare(const Flatterm** v1,const Flatterm** v2) 

      bool ReadCollectedComps()
      {
	CALL("ReadCollectedComps()");
	ASSERT(_readModeFlag);
	ASSERT(*_nodeAddr);
	ASSERT((*_nodeAddr)->isLeaf() || (*_nodeAddr)->next());
	Instruction* curr_node;
      check_node:
	curr_node = *_nodeAddr;
	ASSERT(curr_node);
	if (curr_node->tag() == Instruction::CompareTag) 
	  {
	    ASSERT(curr_node->next());
	    if (PassCompWF(curr_node->asCompare()) || (*_nodeAddr)) 
	      {
		goto check_node;
	      };
	    DOP(CheckConstrIntegrity());
	    MoveCompsToInsert(_nodeAddr);
	    _constrToShift.Reset();
	    ASSERT(!(*_nodeAddr));
	    return false;
	  }
	else
	  if (curr_node->tag() == Instruction::CompareNFTag) 
	    {
	      ASSERT(curr_node->next());
	      if (PassCompNF(curr_node->asCompareNF()) || (*_nodeAddr)) 
		{
		  goto check_node;
		};
	      DOP(CheckConstrIntegrity());
	      MoveCompsToInsert(_nodeAddr);
	      _constrToShift.Reset();
	      ASSERT(!(*_nodeAddr));
	      return false;
	    };

	// !curr_node->isCompare(), must stop here

	if (_constrToShift.Identity())
	  {

	    Instruction** _nodeAddrBackup = _nodeAddr;
	    *_nodeAddr = 0;
	    DOP(CheckConstrIntegrity());
	    if (MoveCompsToInsert(_nodeAddr))
	      {
		ASSERT(!(*_nodeAddr));
		ASSERT((*_nodeAddrBackup)->tag() == Instruction::CompareNFTag);

		CompareNF* tmp = static_cast<CompareNF*>(*_nodeAddrBackup);
		(*_nodeAddrBackup) = tmp->copyWithFork(curr_node);
		if (_nodeAddr == tmp->nextAddr())
		  _nodeAddr = (*_nodeAddrBackup)->nextAddr();
		delete tmp;

		return false;
	      }
	    else // !MoveCompsToInsert(_nodeAddr) (did not insert anything)
	      {
		ASSERT(_nodeAddr == _nodeAddrBackup);
		ASSERT(!(*_nodeAddr));
		*_nodeAddr = curr_node;          
		return true; 
	      };      
	  }
	else // !_constrToShift.Identity(), simple case
	  { 
	    _constrPassedInShift = _constrPassed;
	    InsertCompsToShift(_nodeAddr);
	    ASSERT((*_nodeAddr)->tag() == Instruction::CompareNFTag);       
	    ASSERT((!_constrIntegrated.Equivalent(VarNum((*_nodeAddr)->asCompareNF()->var1()),VarNum((*_nodeAddr)->asCompareNF()->var2()))));
	    MoveRightFrom((*_nodeAddr)->asCompareNF());
	    MoveCompsToInsert(_nodeAddr);
	    ASSERT(!(*_nodeAddr));
	    return false; 
	  };
      }; // bool ReadCollectedComps()


    
      // Write mode 
    private:

      void InsertVar(const Flatterm** v)
      {
	CALL("InsertVar(const Flatterm** v)");
	ASSERT(debugInSubst(v));
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
      }; // void InsertVar(const Flatterm** v)

      void InsertFunc(const Symbol& func)
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
      }; // void InsertFunc(const Symbol& func)  

      void EnterWriteMode()
      {
	CALL("EnterWriteMode()");
	ASSERT(!(*_nodeAddr));
	// must be used if ReadVar or ReadFunc failed
	ASSERT(_constrToShift.Identity());

	ASSERT(_constrToInsert.Implies(_constrIntegrated));
	ASSERT(_constrPassed.Implies(_constrIntegrated));
	ASSERT(_constrPassed.Implies(_constrOld));     

	//_constrToInsert.Subtract(_constrPassed);

	MoveCompsToInsert(_nodeAddr);
	ASSERT(!(*_nodeAddr));  
      }; // void EnterWriteMode() 

      void WriteVar(const ulong& v)
      {
	CALL("WriteVar(const ulong& v)");
	ASSERT(debugInSubst(_currentMatchingIndexVar));
	if (_currentDepth < _startDepthForSkeletonNF)
	  {
	    *_nodeAddr = new Var(_currentMatchingIndexVar);
	  }
	else
	  *_nodeAddr = new VarNF(_currentMatchingIndexVar);
	MoveDown();
	if (_registeredVariables.Occurences(v)) 
	  {
	    DOP(++_debugNonlinearity);
	    WriteCompare(_firstVariableOccurence[v],_currentMatchingIndexVar); 
	  }
	else // first occurence
	  {
	    _registeredVariables.Register(v);
	    _firstVariableOccurence[v] = _currentMatchingIndexVar;
	  };   
	_currentMatchingIndexVar++;
      }; // void WriteVar(ulong v) 

      void WriteFunc(const Symbol& func)
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
      }; // void WriteFunc(const Symbol& func) 


      void WriteCompare(const Flatterm** v1,const Flatterm** v2)
      {
	CALL("WriteCompare(const Flatterm** v1,const Flatterm** v2)");
	ASSERT(debugInSubst(v1));
	ASSERT(debugInSubst(v2));
	*_nodeAddr = new CompareNF(v1,v2); 
	MoveDown();	  
	DOP(++_debugNumOfPassedCompares);
      }; // void WriteCompare(const Flatterm** v1,const Flatterm** v2)


      
      // Insertion of constraints 
    private:

      void CopyCompsToShift(Instruction** addr)
      {
	CALL("CopyCompsToShift(Instruction** addr)");
	DOP(bool debugAtLeastOneShifted = _constrToShift.Identity());  
	ASSERT(_constrPassed.Implies(_constrPassedInShift));
	ulong vn1,vn2;

	EqualityConstraint::Iterator iter(_constrToShift);

	Instruction* old_node = *addr;
	ASSERT(old_node);
	Instruction* new_node; 
	while (iter.Next(vn2,vn1))
	  {
	    ASSERT(vn1<vn2);
	    ASSERT(debugInSubst(nativeVar(vn1)));
	    ASSERT(debugInSubst(nativeVar(vn2)));
	    ASSERT(!_constrIntegrated.Equivalent(vn1,vn2));
	    ASSERT(_constrOld.Equivalent(vn1,vn2)); 

	    if (!_constrPassedInShift.Equivalent(vn1,vn2))
	      {
		DOP(debugAtLeastOneShifted = true);
		_constrPassedInShift.MakeEquivalent(vn1,vn2);  
		new_node = new CompareNF(nativeVar(vn1),nativeVar(vn2));
		*addr = new_node;
		addr = new_node->nextAddr(); 
	      };
	  };
	*addr = old_node;
	ASSERT(debugAtLeastOneShifted);
      }; // void CopyCompsToShift(Instruction** addr)

      void InsertCompsToShift(Instruction** addr)
      {
	CALL("InsertCompsToShift(Instruction** addr)");
	DOP(bool debugAtLeastOneShifted = _constrToShift.Identity());  
	DOP(CheckConstrIntegrity());     
	ASSERT(_constrPassed.Implies(_constrPassedInShift));
	ulong vn1,vn2;
	Instruction* old_node = *addr;
	ASSERT(old_node);
	Instruction* new_node; 
	while (_constrToShift.Split(vn2,vn1))
	  {    
	    ASSERT(vn1<vn2);
	    ASSERT(debugInSubst(nativeVar(vn1)));
	    ASSERT(debugInSubst(nativeVar(vn2)));

	    ASSERT(!_constrIntegrated.Equivalent(vn1,vn2));
	    ASSERT(_constrOld.Equivalent(vn1,vn2));

	    if (!_constrPassedInShift.Equivalent(vn1,vn2))
	      {
		DOP(debugAtLeastOneShifted = true);  
		_constrPassedInShift.MakeEquivalent(vn1,vn2);
		new_node = new CompareNF(nativeVar(vn1),nativeVar(vn2));
		*addr = new_node;
		addr = new_node->nextAddr(); 
	      };
	  };
	ASSERT(_constrToShift.Identity());
	*addr = old_node;
	ASSERT(debugAtLeastOneShifted);
      }; // void InsertCompsToShift(Instruction** addr)


      bool MoveCompsToInsert(Instruction**& addr)
      {
	CALL("MoveCompsToInsert(Instruction**& addr)");
	ulong vn1,vn2;
	ASSERT(!(*addr));
	Instruction* new_node;
	bool res = false;
	while (_constrToInsert.Split(vn2,vn1))
	  {
	    ASSERT(vn1<vn2);
	    ASSERT(debugInSubst(nativeVar(vn1)));
	    ASSERT(debugInSubst(nativeVar(vn2)));
	    ASSERT(_constrIntegrated.Equivalent(vn1,vn2));

	    if (!_constrPassed.Equivalent(vn1,vn2))
	      {
		ASSERT(!_constrOld.Equivalent(vn1,vn2));
		_constrPassed.MakeEquivalent(vn1,vn2);	  
		DOP(++_debugNumOfPassedCompares);
		new_node = new CompareNF(nativeVar(vn1),nativeVar(vn2));
		*addr = new_node;
		addr = new_node->nextAddr(); 
		++_currentDepth;
		res = true;   
	      };
	  };
	ASSERT(_constrToInsert.Identity());
	*addr = 0;

	ASSERT(_constrToInsert.Identity());
	ASSERT(_constrPassed.Implies(_constrIntegrated));
	ASSERT(_constrIntegrated.Implies(_constrPassed));
	return res;
      }; // bool MoveCompsToInsert(Instruction**& addr)



    private: 
#ifndef NO_DEBUG_VIS
      ostream& outputComps(ostream& str) 
      {
	_constrIntegrated.Normalize();
	_constrOld.Normalize();
	_constrPassed.Normalize();
	_constrToShift.Normalize();
	_constrToInsert.Normalize();
	return str << _constrIntegrated << '\n'
		   << _constrOld << '\n'
		   << _constrPassed << '\n'
		   << _constrToShift << '\n'
		   << _constrToInsert << '\n';      

      };
#endif

    private: // data

      Instruction** _nodeAddr;

      EqualityConstraint _constrToInsert;
      EqualityConstraint _constrToShift;
      EqualityConstraint _constrPassed;
   
      EqualityConstraint _constrPassedInShift;
      EqualityConstraint _constrIntegrated;
      EqualityConstraint _constrOld;   

      const Flatterm** _matchingIndexSubstitution;
  
      const Flatterm** _firstVariableOccurence[MaxNumOfVariables];
      BK::MultisetOfVariables<MaxNumOfVariables> _registeredVariables; 
      const Flatterm** _currentMatchingIndexVar;
      bool _readModeFlag;
   
      ulong _currentDepth;

      enum 
      {
	_startDepthForSkeletonNF = 14UL
      };
  
#ifdef DEBUG_NAMESPACE 
      ulong _debugNonlinearity;
      ulong _debugNumOfPassedCompares;
#endif  

    }; // class Integrator


    class Removal 
    // very costly constructor!
    {  
    public:
      Removal(const Flatterm** subst)
	: _constr(),
	  _constrPassed(),
	  _matchingIndexSubstitution(subst),
	  _registeredVariables()   
      {};
      ~Removal() {};

      void init(const Flatterm** subst)
      {
	_constr.init();
	_constrPassed.init();
	_matchingIndexSubstitution = subst;
	_registeredVariables.init();   
      };


      void destroy()
      {
	CALL("destroy()");
	_registeredVariables.destroy();
	_constrPassed.destroy();
	_constr.destroy();
	BK_CORRUPT(*this);
      };


      void Reset(Instruction** n)
      {
	CALL("Reset(Instruction** n)");
	// here *n must be nonnull!
	ASSERT(*n);
	_nodeAddr = n;
	_branchToDestroy = n;
	_constr.Reset();
	_constrPassed.Reset();
	_registeredVariables.Reset();
	_currentMatchingIndexVar = _matchingIndexSubstitution; 
      };
      bool Skip(const Symbol& sym)
      {
	if (sym.isVariable()) { return SkipVar(sym.var()); }
	else return SkipFunc(sym);
      };    
   

      bool SkipFunc(const Symbol& func)
      {
	CALL("SkipFunc(const Symbol& func)");
	ASSERT(*_nodeAddr);
	const Flatterm** var1;
	const Flatterm** var2;
	ulong var_num1;
	ulong var_num2;
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
	  try_fork:
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


	  case Instruction::VarTag: goto try_fork;

	  case Instruction::VarNFTag: return false;

	  case Instruction::CompareTag:
	    ICP("ICP0");
	    return false;
 
	  case Instruction::CompareNFTag: 
	    var1 = curr_node->asCompareNF()->var1();
	    var2 = curr_node->asCompareNF()->var2();     
	    var_num1 = VarNum(var1);
	    var_num2 = VarNum(var2);
	    if (_constr.Equivalent(var_num1,var_num2))
	      {
		_constrPassed.MakeEquivalent(var_num1,var_num2);
		MoveDown();
		goto check_node;
	      }       
	    else return false;  

#ifdef DEBUG_NAMESPACE
	  default: ICP("X0"); return false;
#else 
#ifdef _SUPPRESS_WARNINGS_
	  default: return false;
#endif
#endif        
	  };  
      }; // bool SkipFunc(const Symbol& func)

      bool SkipComps()
      {
	CALL("SkipComps()");
	ASSERT(*_nodeAddr);
	const Flatterm** var1;
	const Flatterm** var2;
	ulong var_num1;
	ulong var_num2;
	Instruction* curr_node;
      check_tag:
	curr_node = *_nodeAddr;
	if (curr_node->tag() == Instruction::CompareTag)
	  {
	    var1 = curr_node->asCompare()->var1();
	    var2 = curr_node->asCompare()->var2();
	    var_num1 = VarNum(var1);
	    var_num2 = VarNum(var2);
	    if (_constr.Equivalent(var_num1,var_num2))
	      {
		_constrPassed.MakeEquivalent(var_num1,var_num2);
		if (curr_node->fork()) _branchToDestroy = _nodeAddr;       
		MoveDown();       
		goto check_tag;
	      };
	    if (curr_node->fork())
	      {
		MoveRight();
		_branchToDestroy = _nodeAddr;
		goto check_tag;
	      };
	    return false;
	  }
	else
	  if (curr_node->tag() == Instruction::CompareNFTag)
	    {
	      var1 = curr_node->asCompareNF()->var1();
	      var2 = curr_node->asCompareNF()->var2();
	      var_num1 = VarNum(var1);
	      var_num2 = VarNum(var2);
	      if (_constr.Equivalent(var_num1,var_num2))
		{
		  _constrPassed.MakeEquivalent(var_num1,var_num2);      
		  MoveDown();       
		  goto check_tag;
		};
	      return false;
	    };
	// !curr_node->isCompare()
	_constr.Subtract(_constrPassed);  
	return _constr.Identity();  
      }; // bool SkipComps()

      bool SkipVar(const Symbol& var) { return SkipVar(var.var()); };
      Leaf* GetLeaf() const 
      {
	CALL("GetLeaf()");
	ASSERT((*_nodeAddr)->isLeaf());
	return (*_nodeAddr)->asLeaf();
      };
      Instruction** BranchToDestroy() const { return _branchToDestroy; }; 
  
    private: 
      typedef BK::EquivalenceRelation<MaxTermSize> EqualityConstraint;

    private:
      void MoveDown() { _nodeAddr = (*_nodeAddr)->nextAddr(); };
      void MoveRight() { _nodeAddr = (*_nodeAddr)->forkAddr(); }; 
      ulong VarNum(const Flatterm** v) const { return v - _matchingIndexSubstitution; };   

   
      bool SkipVar(ulong var)
      {
	CALL(" FMCodeTree<MaxTermSize,MaxNumOfVariables,Alloc,Symbol,Flatterm,LeafInfo,InstanceId>::Removal::SkipVar(ulong var)");
	if (_registeredVariables.Occurences(var))
	  {
	    _constr.MakeEquivalent(VarNum(_firstVariableOccurence[var]),VarNum(_currentMatchingIndexVar)); 
	  }
	else // first occurence
	  {
	    _registeredVariables.Register(var);
	    _firstVariableOccurence[var] = _currentMatchingIndexVar;
	  };

	_currentMatchingIndexVar++;

	return SkipVar();    
      }; // bool SkipVar(ulong var) 

      bool SkipVar()
      {
	CALL("SkipVar()");
	ASSERT(*_nodeAddr);
	const Flatterm** var1;
	const Flatterm** var2;
	ulong var_num1;
	ulong var_num2;
	Instruction* curr_node;  
      check_node:
	curr_node = *_nodeAddr;
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
	    ICP("ICP0");
	    return false;

	  case Instruction::CompareNFTag: 
	    var1 = curr_node->asCompareNF()->var1();
	    var2 = curr_node->asCompareNF()->var2();
	    var_num1 = VarNum(var1);
	    var_num2 = VarNum(var2);
	    if (_constr.Equivalent(var_num1,var_num2))
	      {
		_constrPassed.MakeEquivalent(var_num1,var_num2);
		MoveDown();
		goto check_node;
	      }       
	    else return false;
     

#ifdef DEBUG_NAMESPACE

	  default:   
	    ICP("ICP0"); return false;
#else 
#ifdef _SUPPRESS_WARNINGS_
	  default: return false;
#endif
#endif 
	  };
      }; // bool SkipVar() 



    private:
      Instruction** _nodeAddr;
      Instruction** _branchToDestroy;
      EqualityConstraint _constr; 
      EqualityConstraint _constrPassed;

      const Flatterm** _matchingIndexSubstitution;  
      const Flatterm** _firstVariableOccurence[MaxNumOfVariables];
      BK::MultisetOfVariables<MaxNumOfVariables> _registeredVariables; 
      const Flatterm** _currentMatchingIndexVar;

    }; // class Removal

    class Retrieval
    {   
    public:
      Retrieval() 
	: backtrack_nodes("Retrieval::backtrack_nodes"),
	  backtrack_cursors("Retrieval::backtrack_cursors")
      {
	CALL("constructor Retrieval()");
      };
      ~Retrieval() 
      {
	CALL("destructor ~Retrieval()");
      };
      void init()
      {
	CALL("init()");
	backtrack_nodes.init("Retrieval::backtrack_nodes");
	backtrack_cursors.init("Retrieval::backtrack_cursors");
      };
  
      void destroy()
      {
	CALL("destroy()");
	backtrack_cursors.destroy();
	backtrack_nodes.destroy();
	BK_CORRUPT(*this);
      };



      Leaf* Match(const Flatterm* query,Instruction* tree)
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
	  func_check_node:
	    if (curr_node->tag() == Instruction::FuncNFTag)
	      goto funcNF_compare_sym;
	    if (curr_node->tag() == Instruction::FuncTag)
	      goto func_compare_sym;
	    ASSERT(curr_node->tag() == Instruction::CompareNFTag);

	    // upper Compare, specialised treatment possible            
	    if ((*((static_cast<CompareNF*>(curr_node))->var1()))->equal(*((static_cast<CompareNF*>(curr_node))->var2())))
	      {
		curr_node = curr_node->next();
		goto func_check_node;
	      };
	    goto backtrack;


      
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
      }; // Leaf* Match(const Flatterm* query,Instruction* tree) 

      Leaf* MatchAgain()
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
	  func_check_node:
	    if (curr_node->tag() == Instruction::FuncNFTag)
	      goto funcNF_compare_sym;
	    if (curr_node->tag() == Instruction::FuncTag)
	      goto func_compare_sym;
	    ASSERT(curr_node->tag() == Instruction::CompareNFTag);

	    // upper Compare, specialised treatment possible            
	    if ((*((static_cast<CompareNF*>(curr_node))->var1()))->equal(*((static_cast<CompareNF*>(curr_node))->var2())))
	      {
		curr_node = curr_node->next();
		goto func_check_node;
	      };
	    goto backtrack;


      
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
	COP("backtrack");
	if (backtrack_nodes.empty())
	  {
	    // failure
	    return 0;
	  };
	curr_node = backtrack_nodes.pop(); 
	cursor = backtrack_cursors.pop();
	goto check_node;
      }; // Leaf* MatchAgain()




    private:
      BK::Stack<Instruction*,MaxTermSize> backtrack_nodes;
      BK::Stack<const Flatterm*,MaxTermSize> backtrack_cursors;
    }; // class Retrieval




    class Output
    {  
    public:
      Output()
	: vars(0), 
	_leafInfo(&LeafInfoDefault)
      {};
      ~Output() {};

      void init()
      {
	vars = 0; 
	_leafInfo = &LeafInfoDefault;
      };
  
      void destroy()
      {
	BK_CORRUPT(*this);
      };

      //void Init() { vars = 0; _leafInfo = &LeafInfoDefault; };
    private:
      static void LeafInfoDefault(Instruction* instr) {};
      ostream& Var(ostream& str,const Flatterm** v) const { return str << v - vars; };    
    public:
      void SetSubst(const Flatterm* const * subst) { vars = subst; };
      void SetLeafInfo(void (*f)(Instruction* instr)) { _leafInfo = f; };
#ifndef NO_DEBUG_VIS             
      ostream& instruction(ostream& str,const Instruction* node) const
      {
	CALL("instruction(ostream& str,const Instruction* node) const");
	return node->output(str,vars);
      };


  
      ostream& Tree(ostream& str,const Instruction* tree) const
      {
	CALL("Tree(ostream& str,const Instruction* tree) const");
	BK::Stack<const Instruction*,MaxTermSize> forks("FMCodeTree<MaxTermSize,MaxNumOfVariables,Alloc,Symbol,Flatterm,LeafInfo,InstanceId>::Output::Tree(..)::forks");    
      next_node:
	instruction(str,tree) << '\n';
	if (tree->hasFork() && tree->fork()) forks.push(tree);
	if (tree->hasNext())
	  {
	    tree = tree->next();
	    if (tree) goto next_node;
	    str << "!!!!!!!!!!! NULL next() !!!!!!!!\n";
	  };
	if (forks.empty()) return str;
	tree = forks.pop();
	str << "fork from [" << (ulong)tree << "]\n";
	tree = tree->fork();
	goto next_node;   
      }; // ostream& Tree(ostream& str,const Instruction* tree) const

  
#endif
    private: 
      const Flatterm* const * vars;
      void (*_leafInfo)(Instruction* instr);
    }; // class Output





  public:
    static void Destroy(Instruction* tree);  
    static void DestroyBranch(Instruction** branch);
  }; // class FMCodeTree
}; // namespace Gem





//================================================
#undef DEBUG_NAMESPACE
#ifdef DEBUG_FORWARD_MATCHING_INDEX_TREE
#define DEBUG_NAMESPACE "FMCodeTree<MaxTermSize,MaxNumOfVariables,Alloc,Symbol,Flatterm,LeafInfo,InstanceId>"
#endif
#include "debugMacros.hpp"
//=================================================
namespace Gem
{
  template <ulong MaxTermSize,ulong MaxNumOfVariables,class Alloc,class Symbol,class Flatterm,class LeafInfo,class InstanceId> 
  void FMCodeTree<MaxTermSize,MaxNumOfVariables,Alloc,Symbol,Flatterm,LeafInfo,InstanceId>::Destroy(Instruction* tree)
  {
    CALL("Destroy(Instruction* tree)");
    if (!tree) return;
    switch (tree->tag())
      {
      case Instruction::FuncTag:
	Destroy(tree->fork());
	Destroy(tree->next());
	delete tree->asFunc();
	break;
      case Instruction::FuncNFTag:
	Destroy(tree->next());
	delete tree->asFuncNF();
	break;      
      case Instruction::VarTag:
	Destroy(tree->fork());
	Destroy(tree->next());
	delete tree->asVar();
	break;
      case Instruction::VarNFTag: 
	Destroy(tree->next());
	delete tree->asVarNF();
	break;
      case Instruction::CompareTag:
	Destroy(tree->fork());
	Destroy(tree->next());
	delete tree->asCompare();
	break;
      case Instruction::CompareNFTag: 
	Destroy(tree->next());
	delete tree->asCompareNF();
	break;
      case Instruction::LeafTag:
	delete tree->asLeaf();
	break;
#ifdef DEBUG_NAMESPACE
      default: ICP("ICP0"); return;
#else
#  ifdef _SUPPRESS_WARNINGS_
      default: BK::RuntimeError::report("Bad tag in FMCodeTree<..>::Destroy(Instruction* tree)");
#  endif
#endif
      };
  }; // void FMCodeTree<MaxTermSize,MaxNumOfVariables,Alloc,Symbol,Flatterm,LeafInfo,InstanceId>::Destroy(Instruction* tree)

  template <ulong MaxTermSize,ulong MaxNumOfVariables,class Alloc,class Symbol,class Flatterm,class LeafInfo,class InstanceId> 
  void FMCodeTree<MaxTermSize,MaxNumOfVariables,Alloc,Symbol,Flatterm,LeafInfo,InstanceId>::DestroyBranch(Instruction** branch)
  {
    CALL("DestroyBranch(Instruction** branch)");  
  
    Instruction* fork;
    switch ((*branch)->tag())
      {
      case Instruction::FuncTag:
	fork = (*branch)->fork();
	Destroy((*branch)->next());
	delete (*branch)->asFunc();
	*branch = fork;
	break;
      case Instruction::FuncNFTag:
	Destroy(*branch);
	*branch = 0;
	break;
      case Instruction::VarTag:
	fork = (*branch)->fork();
	Destroy((*branch)->next());
	delete (*branch)->asVar();
	*branch = fork;
	break;
      case Instruction::VarNFTag: 
	Destroy(*branch);
	*branch = 0;
	break;
      case Instruction::CompareTag:
	fork = (*branch)->fork();
	Destroy((*branch)->next());
	delete (*branch)->asCompare();
	*branch = fork;
	break;
      case Instruction::CompareNFTag:
	Destroy(*branch);
	*branch = 0;
	break;
      case Instruction::LeafTag:
	delete (*branch)->asLeaf();
	*branch = 0;
	break;
      
#ifdef DEBUG_NAMESPACE
      default: ICP("ICP0"); return;
#else
#  ifdef _SUPPRESS_WARNINGS_
      default: BK::RuntimeError::report("Bad tag in FMCodeTree<..>::DestroyBranch(Instruction** branch)");
#  endif
#endif
      };

  }; // void FMCodeTree<MaxTermSize,MaxNumOfVariables,Alloc,Symbol,Flatterm,LeafInfo,InstanceId>::DestroyBranch(Instruction** branch)
}; // namespace Gem




//=============================================================
#endif
