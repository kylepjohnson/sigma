//
// File:         OriginalFMCodeTree.hpp
// Description:  Original code trees 
//               for forward matching (search for generalisations).
//               See A. Voronkov, 
//               The Anatomy of Vampire: Implementing Bottom-up Procedures with 
//               Code, Trees Journal of Automated Reasoning, 15(2), 1995
// Created:      Apr 07, 2003, Alexandre Riazanov, riazanov@cs.man.ac.uk
// Note:         This is a part of the Gematogen library.
// Revised:      Apr 11, 2003, Alexandre Riazanov, riazanov@cs.man.ac.uk 
//               Restore gone. 
//====================================================
#ifndef ORIGINAL_FM_CODE_TREES_H
//==================================================================
#define ORIGINAL_FM_CODE_TREES_H
#ifndef NO_DEBUG_VIS
#include <iostream>
#endif
#include "jargon.hpp"
#include "GematogenDebugFlags.hpp" 
#include "MultisetOfVariables.hpp"
#include "Array.hpp"
#include "DestructionMode.hpp"
#include "GlobalStopFlag.hpp"
//================================================
#undef DEBUG_NAMESPACE
#ifdef DEBUG_FORWARD_MATCHING_INDEX
#define DEBUG_NAMESPACE "OriginalFMCodeTree<MaxTermSize,MaxNumOfVariables,Alloc,Symbol,Term,SuccessInfo,InstanceId>"
#endif 
#include "debugMacros.hpp"
//=================================================

namespace Gem
{
  template <ulong MaxTermSize,ulong MaxNumOfVariables,class Alloc,class Symbol,class Term,class SuccessInfo,class InstanceId>
  class OriginalFMCodeTree
  { 
  public:
    class Instruction
    {
    public:
      enum Tag
      {
	CheckTag,
	DownTag,
	RightTag,
	PushTag,
	PopTag,
	PutTag,
        CompareTag,
	ForkTag,
	SuccessTag
      };
    public:
      Instruction(const Tag& t) : _tag(t) {};
      const Tag& tag() const { return _tag; };
    private:
      Tag _tag;
    }; // class Instruction

    class InstructionWithNext : public Instruction
    {
    public:
      InstructionWithNext(const Tag& t) : Instruction(t) 
      {
	CALL("InstructionWithNext::constructor InstructionWithNext(const Tag& t)");
	ASSERT(t != SuccessTag);
	DOP(_next = 0);
      };
      Instruction*& next() 
      { 
	CALL("InstructionWithNext::next()");
	ASSERT(tag() != SuccessTag);
	return _next; 
      };
      const Instruction* next() const 
      { 
	CALL("InstructionWithNext::next() const");
	ASSERT(tag() != SuccessTag);
	return _next; 
      };
      Instruction** nextAddr() 
      { 
	CALL("InstructionWithNext::nextAddr()");
	ASSERT(tag() != SuccessTag);
	return &_next; 
      };
    private:
      Instruction* _next;
    }; // class InstructionWithNext

    class Check : public InstructionWithNext
    {
    public:
      Check(const Symbol& sym) : 
	InstructionWithNext(CheckTag),
	_symbol(sym)
      {
      };
      const Symbol& symbol() const 
      {
	CALL("Check::symbol() const");
	ASSERT(tag() == CheckTag);
	return _symbol; 
      };
      void* operator new(size_t)
      {
	CALL("Check::operator new(size_t)");
	return Alloc::allocate(sizeof(Check));
      };
      void operator delete(void* obj)
      {
	CALL("Check::operator delete(void* obj)");
	return Alloc::deallocate(obj,sizeof(Check));
      };
    private:
      Symbol _symbol;
    }; // class Check 

    class Down : public InstructionWithNext
    {
    public:
      Down() : InstructionWithNext(DownTag) {};
      void* operator new(size_t)
      {
	CALL("Down::operator new(size_t)");
	return Alloc::allocate(sizeof(Down));
      };
      void operator delete(void* obj)
      {
	CALL("Down::operator delete(void* obj)");
	return Alloc::deallocate(obj,sizeof(Down));
      };
    private:
      // no data here
    }; // class Down

    class Right : public InstructionWithNext
    {
    public:
      Right() : InstructionWithNext(RightTag) {};
      void* operator new(size_t)
      {
	CALL("Right::operator new(size_t)");
	return Alloc::allocate(sizeof(Right));
      };
      void operator delete(void* obj)
      {
	CALL("Right::operator delete(void* obj)");
	return Alloc::deallocate(obj,sizeof(Right));
      };
    private:
      // no data here
    }; // class Right  


    class Push : public InstructionWithNext
    {
    public:
      Push(ulong position) : InstructionWithNext(PushTag), _position(position) {};
      void* operator new(size_t)
      {
	CALL("Push::operator new(size_t)");
	return Alloc::allocate(sizeof(Push));
      };
      void operator delete(void* obj)
      {
	CALL("Push::operator delete(void* obj)");
	return Alloc::deallocate(obj,sizeof(Push));
      };
      ulong position() const 
      {
	CALL("Pop::position() const");
	ASSERT(tag() == PushTag);
	return _position; 
      };
    private:
      ulong _position;
    }; // class Push  


    class Pop : public InstructionWithNext
    {
    public:
      Pop(ulong position) : InstructionWithNext(PopTag), _position(position) {};
      void* operator new(size_t)
      {
	CALL("Pop::operator new(size_t)");
	return Alloc::allocate(sizeof(Pop));
      };
      void operator delete(void* obj)
      {
	CALL("Pop::operator delete(void* obj)");
	return Alloc::deallocate(obj,sizeof(Pop));
      };
      ulong position() const 
      { 
	CALL("Pop::position() const");
	ASSERT(tag() == PopTag);
	return _position; 
      };
    private:
      ulong _position;
    }; // class Pop  


    class Put : public InstructionWithNext
    {
    public:
      Put(const Term** var) : InstructionWithNext(PutTag), _var(var) {};
      void* operator new(size_t)
      {
	CALL("Put::operator new(size_t)");
	return Alloc::allocate(sizeof(Put));
      };
      void operator delete(void* obj)
      {
	CALL("Put::operator delete(void* obj)");
	return Alloc::deallocate(obj,sizeof(Put));
      };
      const Term** var() 
      {
	CALL("Put::var()");
	ASSERT(tag() == PutTag);
	return _var; 
      };
      const Term* const * var() const 
      { 
	CALL("Put::var() const");
	ASSERT(tag() == PutTag);
	return _var; 
      };
    private:
      const Term** _var;
    }; // class Put

    class Compare : public InstructionWithNext
    {
    public:
      Compare(const Term** var1,const Term** var2) : 
	InstructionWithNext(CompareTag),
	_var1(var1),
	_var2(var2)
      {
      };
      
      void* operator new(size_t)
      {
	CALL("Compare::operator new(size_t)");
	return Alloc::allocate(sizeof(Compare));
      };
      void operator delete(void* obj)
      {
	CALL("Compare::operator delete(void* obj)");
	return Alloc::deallocate(obj,sizeof(Compare));
      };
      const Term** var1() 
      {
	CALL("Compare::var1()");
	ASSERT(tag() == CompareTag);
	return _var1; 
      };
      const Term* const * var1() const 
      { 
	CALL("Compare::var1() const");
	ASSERT(tag() == CompareTag);
	return _var1; 
      };
      const Term** var2() 
      { 
	CALL("Compare::var2()");
	ASSERT(tag() == CompareTag);
	return _var2; 
      };
      const Term* const * var2() const 
      { 
	CALL("Compare::var2() const");
	ASSERT(tag() == CompareTag);
	return _var2; 
      };

    private:
      const Term** _var1;
      const Term** _var2;    
    }; // class Compare 

    class Fork : public InstructionWithNext
    {
    public:
      Fork() : InstructionWithNext(ForkTag) 
      {
	CALL("Fork::constructor Fork()");
	DOP(_fork = 0);
      };
      
      void* operator new(size_t)
      {
	CALL("Fork::operator new(size_t)");
	return Alloc::allocate(sizeof(Fork));
      };
      void operator delete(void* obj)
      {
	CALL("Fork::operator delete(void* obj)");
	return Alloc::deallocate(obj,sizeof(Fork));
      };
      Instruction*& fork() 
      {
	CALL("Fork::fork()");
	ASSERT(tag() == ForkTag);
	return _fork; 
      };
      const Instruction* fork() const 
      { 
	CALL("Fork::fork() const");
	ASSERT(tag() == ForkTag);
	return _fork; 
      };
      Instruction** forkAddr() 
      { 
	CALL("Fork::forkAddr()");
	ASSERT(tag() == ForkTag);
	return &_fork; 
      };
    private:
      Instruction* _fork;
    }; // class Fork


    class Success : public Instruction
    {
    public:
      Success() : Instruction(SuccessTag) {};      
      void* operator new(size_t)
      {
	CALL("Success::operator new(size_t)");
	return Alloc::allocate(sizeof(Success));
      };
      void operator delete(void* obj)
      {
	CALL("Success::operator delete(void* obj)");
	return Alloc::deallocate(obj,sizeof(Success));
      };
      SuccessInfo& info() 
      {
	CALL("Success::info()");
	ASSERT(tag() == SuccessTag);
	return _info; 
      };
      const SuccessInfo& info() const 
      { 
	CALL("Success::info() const");
	ASSERT(tag() == SuccessTag);
	return _info; 
      };
      
    private:
      SuccessInfo _info;
    }; // class Success 

    class Integrator
    {
    public:
      Integrator(const Term** subst) : _subst(subst) {};
      void reset(const Symbol& f,Instruction** tree);
      void variable(ulong var);
      void function(const Symbol& f);
      Success* successInstr(bool& newSuccInstr);
      BK::MultisetOfVariables<MaxNumOfVariables>& variables() { return _registeredVariables; };
      const Term** mapVariable(ulong var) 
      {
	CALL("mapVariable(ulong var)");
	ASSERT(_registeredVariables.Occurences(var));
	return _firstVariableOccurence[var]; 
      };
    private:
      void moveDown()
      {
	CALL("moveDown()");
	_nodeAddr = 
	  (static_cast<InstructionWithNext*>(*_nodeAddr))->nextAddr();
      };

      void moveDown2()
      {
	CALL("moveDown2()");
	_nodeAddr = 
	  (static_cast<InstructionWithNext*>((static_cast<InstructionWithNext*>(*_nodeAddr))->next()))->nextAddr();
      };

      void moveRight()
      {
	CALL("moveRight()");
	ASSERT((*_nodeAddr)->tag() == Instruction::ForkTag);
	_nodeAddr = (static_cast<Fork*>(*_nodeAddr))->forkAddr();
      };

      void forkDown()
      {
	CALL("forkDown()");
	Fork* fork = new Fork();
	fork->fork() = *_nodeAddr;
	*_nodeAddr = fork;
	_nodeAddr = fork->nextAddr();
      };
      void forkRight()
      {
	CALL("forkRight()");
	Fork* fork = new Fork();
	fork->next() = *_nodeAddr;
	*_nodeAddr = fork;
	_nodeAddr = fork->forkAddr();
      };
      Instruction* instrBelow()
      {
	return (static_cast<InstructionWithNext*>(*_nodeAddr))->next();
      };
    private:
      const Term** _subst;
      const Term** _firstVariableOccurence[MaxNumOfVariables];
      Instruction** _nodeAddr;
      BK::MultisetOfVariables<MaxNumOfVariables> _registeredVariables;      
      const Term** _nextTechnicalVar;
      BK::Stack<const Term**,2*MaxTermSize> _compares;
      bool _readModeFlag;

      ulong _currentHoles;
      BK::Stack<ulong,MaxTermSize> _holes;
      ulong _nextPushPosition;
      BK::Stack<ulong,MaxTermSize> _pushPositions;
    }; // class Integrator
    
    class Removal
    {
    public:
      Removal(const Term** subst) : _subst(subst) {};
      bool reset(const Symbol& f,Instruction** tree);
      bool variable(ulong var);
      bool function(const Symbol& f);
      bool finalise();
      Success* successInstr() { return _successInstr; };
      void destroyBranch();
    private:
      void moveDown()
      {
	CALL("moveDown()");
	_nodeAddr = 
	  (static_cast<InstructionWithNext*>(*_nodeAddr))->nextAddr();
      };
      void moveRight()
      {
	CALL("moveRight()");
	ASSERT((*_nodeAddr)->tag() == Instruction::ForkTag);
	_nodeAddr = (static_cast<Fork*>(*_nodeAddr))->forkAddr();
      };
      Instruction* instrBelow()
      {
	return (static_cast<InstructionWithNext*>(*_nodeAddr))->next();
      };
    private:
      const Term** _subst;
      const Term** _firstVariableOccurence[MaxNumOfVariables];
      Instruction** _nodeAddr;
      BK::MultisetOfVariables<MaxNumOfVariables> _registeredVariables;      
      const Term** _nextTechnicalVar;
      BK::Stack<const Term**,2*MaxTermSize> _compares;

      OriginalFMCodeTree::Instruction** _branchToDestroy;
      bool _destroyFork;
      Success* _successInstr;
    }; // class Removal

    class Retrieval
    {
    public:
      Retrieval() {};
      Success* match(const Term* query,Instruction* instr);
      Success* matchAgain();
    private:
      const Term* _q;     
      const Term* _post[MaxTermSize];
      BK::Stack<const Term*,2*MaxTermSize> _backtrackPointsInQuery;
      BK::Stack<Instruction*,2*MaxTermSize> _backtrackPointsInTree;
      
    }; // class Retrieval
    
#ifndef NO_DEBUG_VIS
    class Output
    {
    public:
      Output() {};
      void setSubst(const Term** subst) { _subst = subst; };
      ostream& tree(ostream& str,const Instruction* tree) const; 
    private:
      const Term** _subst;
    }; // class Output
#endif

  public: // methods
    static void destroyTree(Instruction* tree);


  }; // class OriginalFMCodeTree<MaxTermSize,MaxNumOfVariables,Alloc,Symbol,Term,SuccessInfo,InstanceId>

}; // namespace Gem


//================================================
#undef DEBUG_NAMESPACE
#ifdef DEBUG_FORWARD_MATCHING_INDEX
#define DEBUG_NAMESPACE "OriginalFMCodeTree<MaxTermSize,MaxNumOfVariables,Alloc,Symbol,Term,SuccessInfo,InstanceId>::Integrator"
#endif 
#include "debugMacros.hpp"
//=================================================
namespace Gem
{
  
  template <ulong MaxTermSize,ulong MaxNumOfVariables,class Alloc,class Symbol,class Term,class SuccessInfo,class InstanceId>
  void OriginalFMCodeTree<MaxTermSize,MaxNumOfVariables,Alloc,Symbol,Term,SuccessInfo,InstanceId>::Integrator::reset(const Symbol& f,Instruction** tree)
  {
    CALL("reset(const Symbol& f,Instruction** tree)");
    _nodeAddr = tree;
    _registeredVariables.Reset();
    _nextTechnicalVar = _subst;
    _compares.reset();
    _readModeFlag = ((*tree) != 0);
    _currentHoles = f.arity();
    _holes.reset();
    _nextPushPosition = 0UL;
    _pushPositions.reset();
  }; // Integrator::reset(const Symbol& f,Instruction** tree)

  template <ulong MaxTermSize,ulong MaxNumOfVariables,class Alloc,class Symbol,class Term,class SuccessInfo,class InstanceId>
  void OriginalFMCodeTree<MaxTermSize,MaxNumOfVariables,Alloc,Symbol,Term,SuccessInfo,InstanceId>::Integrator::variable(ulong var)
  {
    CALL("variable(ulong var)");
    ASSERT(var < MaxNumOfVariables); 
    ASSERT(_currentHoles);

    if (_readModeFlag)
      {
	switch ((*_nodeAddr)->tag())
	  {
	  case Instruction::CheckTag:
	    _readModeFlag = false;
	    forkDown();
	    goto write_put;

	  case Instruction::PutTag:
	  skip_put:
	    ASSERT((static_cast<Put*>(*_nodeAddr))->var() == _nextTechnicalVar);
	    moveDown();
	    ASSERT(*_nodeAddr);
	    if (_registeredVariables.Occurences(var))
	      {
		_compares.push(_firstVariableOccurence[var]);
		_compares.push(_nextTechnicalVar);
	      }
	    else // new variable
	      {
		_registeredVariables.Register(var);
		_firstVariableOccurence[var] = _nextTechnicalVar;
	      };

	    ++_nextTechnicalVar;
	    ASSERT(_readModeFlag);

	    --_currentHoles;
	    if (_currentHoles)
	      {
		// skip Right
		ASSERT((*_nodeAddr)->tag() == Instruction::RightTag);
		moveDown();
	      }
	    else // see if a Pop is needed
	      {
		if (_holes.nonempty())
		  {
		    ASSERT(_pushPositions.nonempty());
		    _currentHoles = _holes.pop();
		    ASSERT(_currentHoles);
		    ASSERT((*_nodeAddr)->tag() == Instruction::PopTag);	
#ifdef DEBUG_NAMESPACE
		    ulong pos = _pushPositions.pop();
#else
		    _pushPositions.pop();
#endif
		    ASSERT((static_cast<Pop*>(*_nodeAddr))->position() == pos);
		    moveDown();
		  };
	      };
	    return;

	  case Instruction::ForkTag:
	    // Here we assume that forks essentially form 
	    // a sorted list of Put and Check instructions,
	    // and a Put can only be in the beginning of the list.
	    if (instrBelow()->tag() == Instruction::PutTag)
	      {		
		moveDown();
		goto skip_put;
	      }
	    else // new Fork should be iserted
	      {
		ASSERT(instrBelow()->tag() == Instruction::CheckTag);
		_readModeFlag = false;
		forkDown();
		goto write_put;
	      };
		

#ifdef DEBUG_NAMESPACE
	  default: ICP("ICP0"); return;
#else
#  ifdef _SUPPRESS_WARNINGS_
	  default: 
	    BK::RuntimeError::report("Bad tag in OriginalFMCodeTree<..>::Integrator::variable(ulong var)");
	    return;
#  endif
#endif
	  }; // switch ((*_nodeAddr)->tag())
	
      }
    else // write mode
      {
      write_put:
	ASSERT(!_readModeFlag);
	*_nodeAddr = new Put(_nextTechnicalVar);
	moveDown();
	if (_registeredVariables.Occurences(var))
	  {
	    _compares.push(_firstVariableOccurence[var]);
	    _compares.push(_nextTechnicalVar);
	  }
	else // new variable
	  {
	    _registeredVariables.Register(var);
	    _firstVariableOccurence[var] = _nextTechnicalVar;
	  };
	++_nextTechnicalVar;


	--_currentHoles;
	if (_currentHoles)
	  {
	    // write Right
	    *_nodeAddr = new Right();
	    moveDown();
	  }
	else // see if a Pop is needed
	  {
	    if (_holes.nonempty())
	      {
		ASSERT(_pushPositions.nonempty());
		// write Pop
		_currentHoles = _holes.pop();	
		ASSERT(_currentHoles);
		ulong pos = _pushPositions.pop();
		*_nodeAddr = new Pop(pos);
		moveDown();	
	      };
	  };	
	return;
      };
  }; // Integrator::variable(ulong var)

  template <ulong MaxTermSize,ulong MaxNumOfVariables,class Alloc,class Symbol,class Term,class SuccessInfo,class InstanceId>
  void OriginalFMCodeTree<MaxTermSize,MaxNumOfVariables,Alloc,Symbol,Term,SuccessInfo,InstanceId>::Integrator::function(const Symbol& f)
  {
    CALL("function(const Symbol& f)");
    ASSERT(_currentHoles);
    // Here we assume that forks essentially form 
    // a sorted list of Put and Check instructions,
    // and a Put can only be in the beginning of the list.
    if (_readModeFlag)
      {
      check_tag:
	switch ((*_nodeAddr)->tag())
	  {
	  case Instruction::CheckTag:
	    if ((static_cast<Check*>(*_nodeAddr))->symbol() == f)
	      {
		moveDown();
	      sort_out_holes_and_return:
		if (f.arity())
		  {
		    --_currentHoles;
		    if (_currentHoles)
		      {
			// skip Push
			ASSERT((*_nodeAddr)->tag() == Instruction::PushTag);
			ASSERT((static_cast<Push*>(*_nodeAddr))->position() == _nextPushPosition);
			_holes.push(_currentHoles);
			_pushPositions.push(_nextPushPosition);
			++_nextPushPosition;
			moveDown();
		      };
		    _currentHoles = f.arity();
		    // skip Down
		    ASSERT((*_nodeAddr)->tag() == Instruction::DownTag);
		    moveDown();
		  }
		else // constant
		  {
		    --_currentHoles;
		    if (_currentHoles)
		      {
			// skip Right
			ASSERT((*_nodeAddr)->tag() == Instruction::RightTag);
			moveDown();
		      }
		    else // see if a Pop is needed
		      {
			if (_holes.nonempty())
			  {
			    ASSERT(_pushPositions.nonempty());
			    _currentHoles = _holes.pop();
			    ASSERT(_currentHoles);
			    ASSERT((*_nodeAddr)->tag() == Instruction::PopTag);
#ifdef DEBUG_NAMESPACE
			    ulong pos = _pushPositions.pop();
#else
			    _pushPositions.pop();
#endif
			    ASSERT((static_cast<Pop*>(*_nodeAddr))->position() == pos);
			    moveDown();
			  };
		      };
		  };		
		return;
	      }
	    else // new fork should be inserted
	      {
		_readModeFlag = false;
		if ((static_cast<Check*>(*_nodeAddr))->symbol() < f)
		  {
		    forkRight();
		  }
		else 
		  {
		    forkDown();
		  };
		goto write_check;
	      };

	  case Instruction::PutTag:
	    // new fork should be inserted
	    _readModeFlag = false;
	    forkRight();
	    goto write_check;

	  case Instruction::ForkTag:	    
	    if (instrBelow()->tag() == Instruction::PutTag)
	      {
		moveRight();
		goto check_tag;
	      }
	    else 
	      {
		ASSERT(instrBelow()->tag() == Instruction::CheckTag);

		Check* instr = static_cast<Check*>(instrBelow());
		if (instr->symbol() == f)
		  {
		    moveDown2();
		    goto sort_out_holes_and_return;
		  }
		else
		  if (instr->symbol() < f)
		    {
		      moveRight();		      
		      goto check_tag;
		    }
		  else // instr->symbol() > f, new fork needed
		    {		      
		      _readModeFlag = false;
		      forkDown();
		      goto write_check;
		    };
	      };

#ifdef DEBUG_NAMESPACE
	  default: ICP("ICP0"); return;
#else
#  ifdef _SUPPRESS_WARNINGS_
	  default: 
	    BK::RuntimeError::report("Bad tag in OriginalFMCodeTree<..>::Integrator::function(const Symbol& f)");
	    return;
#  endif
#endif
	  }; // switch ((*_nodeAddr)->tag())
      }
    else // write mode
      {
      write_check:
	ASSERT(!_readModeFlag);
	*_nodeAddr = new Check(f);
	moveDown();

	if (f.arity())
	  {
	    
	    --_currentHoles;
	    if (_currentHoles)
	      {
		// write Push
		*_nodeAddr = new Push(_nextPushPosition);
		_holes.push(_currentHoles);
		_pushPositions.push(_nextPushPosition);
		++_nextPushPosition;
		moveDown();
	      };
	    _currentHoles = f.arity();
	    // write Down
	    *_nodeAddr = new Down();
	    moveDown();
	  }
	else // constant 
	  {	    
	    --_currentHoles;
	    if (_currentHoles)
	      {
		// write Right
		*_nodeAddr = new Right();
		moveDown();
	      }
	    else // see if a Pop is needed
	      {
		if (_holes.nonempty())
		  {
		    ASSERT(_pushPositions.nonempty());
		    // write Pop
		    _currentHoles = _holes.pop();
		    ASSERT(_currentHoles);
		    ulong pos = _pushPositions.pop();
		    *_nodeAddr = new Pop(pos);
		    moveDown();	
		  };
	      };	
	  };
	return;
      };

  }; // Integrator::function(const Symbol& f)

  template <ulong MaxTermSize,ulong MaxNumOfVariables,class Alloc,class Symbol,class Term,class SuccessInfo,class InstanceId>
  OriginalFMCodeTree<MaxTermSize,MaxNumOfVariables,Alloc,Symbol,Term,SuccessInfo,InstanceId>::Success* OriginalFMCodeTree<MaxTermSize,MaxNumOfVariables,Alloc,Symbol,Term,SuccessInfo,InstanceId>::Integrator::successInstr(bool& newSuccInstr)
  {
    CALL("successInstr(bool& newSuccInstr)");
    const Term*** remainingCompares = _compares.begin();
    
    if (_readModeFlag)
      {
	while (remainingCompares != _compares.end())
	  {
	    const Term** var1 = *remainingCompares;
	    const Term** var2 = *(remainingCompares + 1);
	  check_tag:
	    switch ((*_nodeAddr)->tag())
	    {
	    case Instruction::CompareTag:
	      if (((static_cast<Compare*>(*_nodeAddr))->var1() == var1) &&
		  ((static_cast<Compare*>(*_nodeAddr))->var2() == var2))
		{
		  moveDown();
		  break;
		}
	      else
		{
		  _readModeFlag = false;
		  forkRight();
		  goto write_remaining_compares;
		};

	    case Instruction::ForkTag:
	      ASSERT((instrBelow()->tag() == Instruction::CompareTag) ||
		     (instrBelow()->tag() == Instruction::SuccessTag));
	      if ((instrBelow()->tag() == Instruction::CompareTag) &&
                  ((static_cast<Compare*>(instrBelow()))->var1() == var1) &&
                  ((static_cast<Compare*>(instrBelow()))->var2() == var2))
		{
		  moveDown2();
		  break;
		}
	      else
		{
		  moveRight();
		  goto check_tag;
		};

	    case Instruction::SuccessTag:
	      _readModeFlag = false;
	      forkRight();
	      goto write_remaining_compares;	      
	      
#ifdef DEBUG_NAMESPACE
	    default: ICP("ICP0"); return 0;
#else
#  ifdef _SUPPRESS_WARNINGS_
	    default: 
	      BK::RuntimeError::report("Bad tag in OriginalFMCodeTree<..>::Integrator::successInstr(bool& newSuccInstr)");
	      return 0;
#  endif
#endif

	    }; // switch ((*_nodeAddr)->tag());

	    remainingCompares += 2;
	  }; // while (remainingCompares != _compares.end())

	// no more compares for the current term
	
      check_tag1:
	switch ((*_nodeAddr)->tag())
	  {
	  case Instruction::CompareTag:
	    _readModeFlag = false;
	    forkRight();
	    goto write_success;

	  case Instruction::ForkTag:
	    if (instrBelow()->tag() == Instruction::SuccessTag)
	      {
		moveDown();
		newSuccInstr = false;
		return static_cast<Success*>(*_nodeAddr);
	      }
	    else 
	      {
		ASSERT(instrBelow()->tag() == Instruction::CompareTag);
		moveRight();
		goto check_tag1;
	      };

	  case Instruction::SuccessTag:
	    newSuccInstr = false;
	    return static_cast<Success*>(*_nodeAddr);
	  	      
	      
#ifdef DEBUG_NAMESPACE
	  default: ICP("ICP1"); return 0;
#else
#  ifdef _SUPPRESS_WARNINGS_
	  default: 
	    BK::RuntimeError::report("Bad tag in OriginalFMCodeTree<..>::Integrator::successInstr(bool& newSuccInstr)");
	    return 0;
#  endif
#endif

	  }; // switch ((*_nodeAddr)->tag());


      }
    else // write mode
      {
      write_remaining_compares:
	ASSERT(!_readModeFlag);
	while (remainingCompares != _compares.end())
	  {
	    const Term** var1 = *remainingCompares;
	    const Term** var2 = *(remainingCompares + 1);
	    *_nodeAddr = new Compare(var1,var2);
	    moveDown();
	    remainingCompares += 2;
	  }; // while (remainingCompares != _compares.end())

	
      write_success:
	ASSERT(!_readModeFlag);
	*_nodeAddr = new Success();
	newSuccInstr = true;
	return static_cast<Success*>(*_nodeAddr);
      };

  }; // Integrator::successInstr(bool& newSuccInstr)

}; // namespace Gem

//================================================
#undef DEBUG_NAMESPACE
#ifdef DEBUG_FORWARD_MATCHING_INDEX
#define DEBUG_NAMESPACE "OriginalFMCodeTree<MaxTermSize,MaxNumOfVariables,Alloc,Symbol,Term,SuccessInfo,InstanceId>::Removal"
#endif 
#include "debugMacros.hpp"
//=================================================
namespace Gem
{
template <ulong MaxTermSize,ulong MaxNumOfVariables,class Alloc,class Symbol,class Term,class SuccessInfo,class InstanceId>
bool OriginalFMCodeTree<MaxTermSize,MaxNumOfVariables,Alloc,Symbol,Term,SuccessInfo,InstanceId>::Removal::reset(const Symbol& f,Instruction** tree)
{
  CALL("reset(const Symbol& f,Instruction** tree)");
  if (!(*tree)) return false;
  _nodeAddr = tree;
  _registeredVariables.Reset();
  _nextTechnicalVar = _subst;
  _compares.reset();
  _branchToDestroy = tree;
  DOP(_successInstr = 0);
  return true; 
}; // Removal::reset(const Symbol& f,Instruction** tree)

template <ulong MaxTermSize,ulong MaxNumOfVariables,class Alloc,class Symbol,class Term,class SuccessInfo,class InstanceId>
bool OriginalFMCodeTree<MaxTermSize,MaxNumOfVariables,Alloc,Symbol,Term,SuccessInfo,InstanceId>::Removal::variable(ulong var)
{
  CALL("variable(ulong var)");
  ASSERT(var < MaxNumOfVariables); 
  
  if (_registeredVariables.Occurences(var))
    {
      _compares.push(_firstVariableOccurence[var]);
      _compares.push(_nextTechnicalVar);
    }
  else // new variable
    {
      _registeredVariables.Register(var);
      _firstVariableOccurence[var] = _nextTechnicalVar;
    };
  ++_nextTechnicalVar;

  switch ((*_nodeAddr)->tag())
    {
    case Instruction::CheckTag: return false;

    case Instruction::PutTag:
      ASSERT((static_cast<Put*>(*_nodeAddr))->var() == (_nextTechnicalVar - 1));
      moveDown();
      if (((*_nodeAddr)->tag() == Instruction::RightTag) ||
	  ((*_nodeAddr)->tag() == Instruction::PopTag))
	{
	  moveDown();
	};
      return true;

    case Instruction::ForkTag:
      if (instrBelow()->tag() == Instruction::PutTag)
	{	  
	  ASSERT((static_cast<Put*>(instrBelow()))->var() == (_nextTechnicalVar - 1));
	  _branchToDestroy = _nodeAddr;
	  _destroyFork = false;
	  moveDown();
	  moveDown();
	  if (((*_nodeAddr)->tag() == Instruction::RightTag) ||
	      ((*_nodeAddr)->tag() == Instruction::PopTag))
	    {
	      moveDown();
	    };
	  return true;
	}
      else
	{
	  ASSERT(instrBelow()->tag() == Instruction::CheckTag);
	  return false;
	};
	

#ifdef DEBUG_NAMESPACE
	  default: ICP("ICP0"); return false;
#else
#  ifdef _SUPPRESS_WARNINGS_
	  default: 
	    BK::RuntimeError::report("Bad tag in OriginalFMCodeTree<..>::Removal::variable(ulong var)");
	    return false;
#  endif
#endif
    }; // switch ((*_nodeAddr)->tag())


}; // Removal::variable(ulong var)



template <ulong MaxTermSize,ulong MaxNumOfVariables,class Alloc,class Symbol,class Term,class SuccessInfo,class InstanceId>
bool OriginalFMCodeTree<MaxTermSize,MaxNumOfVariables,Alloc,Symbol,Term,SuccessInfo,InstanceId>::Removal::function(const Symbol& f)
{
  CALL("function(const Symbol& f)");
 
 check_tag:
  switch ((*_nodeAddr)->tag())
    {
    case Instruction::CheckTag: 
      if ((static_cast<Check*>(*_nodeAddr))->symbol() == f)
	{
	  moveDown();
	skip_auxilliary:
	  switch ((*_nodeAddr)->tag())
	    {
	    case Instruction::DownTag:
	      // same as below
	    case Instruction::RightTag:
	      // same as below
	    case Instruction::PushTag:
	      // same as below
	    case Instruction::PopTag:
	      moveDown();
	      goto skip_auxilliary;
	    }; // switch ((*_nodeAddr)->tag())
	  return true;
	}
      else
	return false;
      
    case Instruction::PutTag: return false;

    case Instruction::ForkTag:
      // sorted list of Put and Check
      ASSERT((instrBelow()->tag() == Instruction::PutTag) || 
	     (instrBelow()->tag() == Instruction::CheckTag));
      if ((instrBelow()->tag() == Instruction::PutTag) || 
	  (static_cast<Check*>(instrBelow()))->symbol() < f)
	{
	  _branchToDestroy = _nodeAddr;
	  _destroyFork = true;
	  moveRight();
	  goto check_tag;
	};
      if ((static_cast<Check*>(instrBelow()))->symbol() == f)
	{
	  _branchToDestroy = _nodeAddr;
	  _destroyFork = false;
	  moveDown();
	  moveDown();
	  goto skip_auxilliary;
	}
      else // (static_cast<Check*>(instrBelow()))->symbol() > f
	return false;

#ifdef DEBUG_NAMESPACE
	  default: ICP("ICP0"); return false;
#else
#  ifdef _SUPPRESS_WARNINGS_
	  default: 
	    BK::RuntimeError::report("Bad tag in OriginalFMCodeTree<..>::Removal::function(const Symbol& f)");
	    return false;
#  endif
#endif
    }; // switch ((*_nodeAddr)->tag())
}; // Removal::function(const Symbol& f)



template <ulong MaxTermSize,ulong MaxNumOfVariables,class Alloc,class Symbol,class Term,class SuccessInfo,class InstanceId>
bool OriginalFMCodeTree<MaxTermSize,MaxNumOfVariables,Alloc,Symbol,Term,SuccessInfo,InstanceId>::Removal::finalise()
{
  CALL("finalise()");

  const Term*** remainingCompares = _compares.begin();
  
  while (remainingCompares != _compares.end())
    {
      const Term** var1 = *remainingCompares;
      const Term** var2 = *(remainingCompares + 1);

    check_tag:
      switch ((*_nodeAddr)->tag())
	{
	case Instruction::CompareTag:
	  if (((static_cast<Compare*>(*_nodeAddr))->var1() == var1) &&
	      ((static_cast<Compare*>(*_nodeAddr))->var2() == var2))
	    {
	      moveDown();
	      break;
	    }
	  else
	    {

	      return false;
	    };

	case Instruction::ForkTag:
	  if ((instrBelow()->tag() == Instruction::CompareTag) &&
	      ((static_cast<Compare*>(instrBelow()))->var1() == var1) &&
	      ((static_cast<Compare*>(instrBelow()))->var2() == var2))
	    {
	      _branchToDestroy = _nodeAddr;
	      _destroyFork = false;
	      moveDown();
	      moveDown();
	      break;
	    }
	  else
	    {
	      _branchToDestroy = _nodeAddr;
	      _destroyFork = true;
	      moveRight();
	      goto check_tag;
	    };

	case Instruction::SuccessTag: 
	  return false;
	  
#ifdef DEBUG_NAMESPACE
	    default: ICP("ICP0"); return false;
#else
#  ifdef _SUPPRESS_WARNINGS_
	    default: 
	      BK::RuntimeError::report("Bad tag in OriginalFMCodeTree<..>::Removal::finalise()");
	      return 0;
#  endif
#endif
	}; // switch ((*_nodeAddr)->tag())

      remainingCompares += 2;
    }; // while (remainingCompares != _compares.end())

  // No more Compares for the term.
  // Now look for an appropriate Success.
   
 check_tag1:
  switch ((*_nodeAddr)->tag())
    {
    case Instruction::CompareTag: return false;
    case Instruction::ForkTag:
      if (instrBelow()->tag() == Instruction::SuccessTag)
	{
	  _branchToDestroy = _nodeAddr;
	  _destroyFork = false;
	  _successInstr = static_cast<Success*>(instrBelow());
	  return true;
	}
      else
	{
	  _branchToDestroy = _nodeAddr;
	  _destroyFork = true;
	  moveRight();
	  goto check_tag1;
	};

    case Instruction::SuccessTag:
      _successInstr = static_cast<Success*>(*_nodeAddr);
      return true;

#ifdef DEBUG_NAMESPACE
    default: ICP("ICP1"); return false;
#else
#  ifdef _SUPPRESS_WARNINGS_
    default: 
      BK::RuntimeError::report("Bad tag in OriginalFMCodeTree<..>::Removal::finalise()");
      return false;
#  endif
#endif
    }; // switch ((*_nodeAddr)->tag());

}; // Removal::finalise()


template <ulong MaxTermSize,ulong MaxNumOfVariables,class Alloc,class Symbol,class Term,class SuccessInfo,class InstanceId>
void OriginalFMCodeTree<MaxTermSize,MaxNumOfVariables,Alloc,Symbol,Term,SuccessInfo,InstanceId>::Removal::destroyBranch()
{
  CALL("destroyBranch()");
  ASSERT(*_branchToDestroy);
  if ((*_branchToDestroy)->tag() == Instruction::ForkTag)
    {
      Fork* fork = static_cast<Fork*>(*_branchToDestroy);
      if (_destroyFork)
	{
	  destroyTree(fork->fork());
	  *_branchToDestroy = fork->next();
	}
      else 
	{
	  destroyTree(fork->next());
	  *_branchToDestroy = fork->fork();
	};
      delete fork;
    }
  else
    {
      destroyTree(*_branchToDestroy);
      *_branchToDestroy = 0;
    };

}; // Removal::destroyBranch()


}; // namespace Gem

//================================================
#undef DEBUG_NAMESPACE
#ifdef DEBUG_FORWARD_MATCHING_INDEX
#define DEBUG_NAMESPACE "OriginalFMCodeTree<MaxTermSize,MaxNumOfVariables,Alloc,Symbol,Term,SuccessInfo,InstanceId>::Retrieval"
#endif 
#include "debugMacros.hpp"
//=================================================
namespace Gem
{

template <ulong MaxTermSize,ulong MaxNumOfVariables,class Alloc,class Symbol,class Term,class SuccessInfo,class InstanceId>
OriginalFMCodeTree<MaxTermSize,MaxNumOfVariables,Alloc,Symbol,Term,SuccessInfo,InstanceId>::Success*
OriginalFMCodeTree<MaxTermSize,MaxNumOfVariables,Alloc,Symbol,Term,SuccessInfo,InstanceId>::Retrieval::match(const Term* query,Instruction* instr)
{
  CALL("match(const Term* query,Instruction* instr)");
  ASSERT(query->isComplex());
  ASSERT(instr);
  const Term* qDown; // optimisation
  DOP(qDown = 0);
  _q = query->Args();
  _backtrackPointsInQuery.reset();
  _backtrackPointsInTree.reset();
 interpret_instr:
  switch (instr->tag())
    {
    case Instruction::CheckTag:
      COP("CHECK");

      //DF; cout << "CHECK " << (static_cast<Check*>(instr))->symbol() << "\n";  

      ASSERT(_q->isVariable() || _q->IsReference());    
      if (_q->isVariable()) goto backtrack;
      qDown = _q->First();
      if ((static_cast<Check*>(instr))->symbol() == *qDown)
	{
	  instr = (static_cast<InstructionWithNext*>(instr))->next();
	  goto interpret_instr;
	}
      else
	{
	  DOP(qDown = 0);
	  goto backtrack;
	};

    case Instruction::DownTag:
      COP("DOWN");

      //DF; cout << "DOWN\n";      

      ASSERT(_q->IsReference());
      ASSERT(qDown);
      ASSERT(qDown->isComplex());
      _q = qDown->Args();
      ASSERT(_q->isVariable() || _q->IsReference()); 
      DOP(qDown = 0);
      instr = (static_cast<InstructionWithNext*>(instr))->next();
      goto interpret_instr;

    case Instruction::RightTag:
      COP("RIGHT");

      //DF; cout << "RIGHT\n";      

      DOP(qDown = 0);
      _q = _q->Next();

 
     ASSERT(_q->isVariable() || _q->IsReference());
      instr = (static_cast<InstructionWithNext*>(instr))->next();
      goto interpret_instr;
      

    case Instruction::PushTag:
      COP("PUSH");

      //DF; cout << "PUSH " << (static_cast<Push*>(instr))->position() << "\n";      

      ASSERT(_q->isVariable() || _q->IsReference()); 
      ASSERT(_q->Next()->isVariable() || _q->Next()->IsReference());
      _post[(static_cast<Push*>(instr))->position()] = _q->Next();
      instr = (static_cast<InstructionWithNext*>(instr))->next();
      goto interpret_instr;

    case Instruction::PopTag:
      COP("POP");

      //DF; cout << "POP " << (static_cast<Pop*>(instr))->position() << "\n";      

      _q = _post[(static_cast<Pop*>(instr))->position()];
      ASSERT(_q->isVariable() || _q->IsReference()); 
      instr = (static_cast<InstructionWithNext*>(instr))->next();
      goto interpret_instr;

    case Instruction::PutTag:
      COP("PUT");

      //DF; cout << "PUT\n";      

      DOP(qDown = 0);
      ASSERT(_q->isVariable() || _q->IsReference()); 
      *((static_cast<Put*>(instr))->var()) = _q;
      instr = (static_cast<InstructionWithNext*>(instr))->next();
      goto interpret_instr;

    case Instruction::CompareTag:
      COP("COMPARE");

      //DF; cout << "COMPARE\n";      

      DOP(qDown = 0);
      if ((*(static_cast<Compare*>(instr)->var1()))->isSyntacticallyEqualTo(**(static_cast<Compare*>(instr)->var2())))
	{
	  instr = (static_cast<InstructionWithNext*>(instr))->next();
	  goto interpret_instr;
	}
      else
	goto backtrack;

    case Instruction::ForkTag:
      COP("FORK");

      //DF; cout << "FORK\n";      

      DOP(qDown = 0);
      ASSERT(_q->isVariable() || _q->IsReference()); 
      _backtrackPointsInQuery.push(_q);
      _backtrackPointsInTree.push((static_cast<Fork*>(instr))->fork());
      instr = (static_cast<InstructionWithNext*>(instr))->next();
      goto interpret_instr;
      

    case Instruction::SuccessTag:
      COP("SUCCESS");

      //DF; cout << "SUCCESS\n";      

      DOP(qDown = 0);
      return static_cast<Success*>(instr);
      
#ifdef DEBUG_NAMESPACE
    default: ICP("ICP0"); return 0;
#else
#  ifdef _SUPPRESS_WARNINGS_
    default: 
      BK::RuntimeError::report("Bad tag in OriginalFMCodeTree<..>::Retrieval::match(const Term* query,Instruction* instr)");
      return 0;
#  endif
#endif
    }; // switch (instr->tag())

 backtrack: 
  COP("BACKTRACK");

  //DF; cout << "             BACKTRACK\n";      

  BK::GlobalStopFlag::check();
  if (_backtrackPointsInTree.empty()) return 0;
  instr = _backtrackPointsInTree.pop();
  
  DOP(qDown = 0);
  _q = _backtrackPointsInQuery.pop();
  ASSERT(_q->isVariable() || _q->IsReference()); 

  goto interpret_instr;
}; // Retrieval::match(const Term* query,Instruction* instr)

template <ulong MaxTermSize,ulong MaxNumOfVariables,class Alloc,class Symbol,class Term,class SuccessInfo,class InstanceId>
OriginalFMCodeTree<MaxTermSize,MaxNumOfVariables,Alloc,Symbol,Term,SuccessInfo,InstanceId>::Success*
OriginalFMCodeTree<MaxTermSize,MaxNumOfVariables,Alloc,Symbol,Term,SuccessInfo,InstanceId>::Retrieval::matchAgain()
{
  CALL("matchAgain()");
  Instruction* instr;
  const Term* qDown; // optimisation
  DOP(qDown = 0);
  goto backtrack;


 interpret_instr:
  switch (instr->tag())
    {
    case Instruction::CheckTag:
      COP("CHECK");

      //DF; cout << "CHECK " << (static_cast<Check*>(instr))->symbol() << "\n";  

      ASSERT(_q->isVariable() || _q->IsReference());    
      if (_q->isVariable()) goto backtrack;
      qDown = _q->First();
      if ((static_cast<Check*>(instr))->symbol() == *qDown)
	{
	  instr = (static_cast<InstructionWithNext*>(instr))->next();
	  goto interpret_instr;
	}
      else
	{
	  DOP(qDown = 0);
	  goto backtrack;
	};

    case Instruction::DownTag:
      COP("DOWN");

      //DF; cout << "DOWN\n";      

      ASSERT(_q->IsReference());
      ASSERT(qDown);
      ASSERT(qDown->isComplex());
      _q = qDown->Args();
      ASSERT(_q->isVariable() || _q->IsReference()); 
      DOP(qDown = 0);
      instr = (static_cast<InstructionWithNext*>(instr))->next();
      goto interpret_instr;

    case Instruction::RightTag:
      COP("RIGHT");

      //DF; cout << "RIGHT\n";      

      DOP(qDown = 0);
      _q = _q->Next();

 
     ASSERT(_q->isVariable() || _q->IsReference());
      instr = (static_cast<InstructionWithNext*>(instr))->next();
      goto interpret_instr;
      

    case Instruction::PushTag:
      COP("PUSH");

      //DF; cout << "PUSH " << (static_cast<Push*>(instr))->position() << "\n";      

      ASSERT(_q->isVariable() || _q->IsReference()); 
      ASSERT(_q->Next()->isVariable() || _q->Next()->IsReference());
      _post[(static_cast<Push*>(instr))->position()] = _q->Next();
      instr = (static_cast<InstructionWithNext*>(instr))->next();
      goto interpret_instr;

    case Instruction::PopTag:
      COP("POP");

      //DF; cout << "POP " << (static_cast<Pop*>(instr))->position() << "\n";      

      _q = _post[(static_cast<Pop*>(instr))->position()];
      ASSERT(_q->isVariable() || _q->IsReference()); 
      instr = (static_cast<InstructionWithNext*>(instr))->next();
      goto interpret_instr;

    case Instruction::PutTag:
      COP("PUT");

      //DF; cout << "PUT\n";      

      DOP(qDown = 0);
      ASSERT(_q->isVariable() || _q->IsReference()); 
      *((static_cast<Put*>(instr))->var()) = _q;
      instr = (static_cast<InstructionWithNext*>(instr))->next();
      goto interpret_instr;

    case Instruction::CompareTag:
      COP("COMPARE");

      //DF; cout << "COMPARE\n";      

      DOP(qDown = 0);
      if ((*(static_cast<Compare*>(instr)->var1()))->isSyntacticallyEqualTo(**(static_cast<Compare*>(instr)->var2())))
	{
	  instr = (static_cast<InstructionWithNext*>(instr))->next();
	  goto interpret_instr;
	}
      else
	goto backtrack;

    case Instruction::ForkTag:
      COP("FORK");

      //DF; cout << "FORK\n";      

      DOP(qDown = 0);
      ASSERT(_q->isVariable() || _q->IsReference()); 
      _backtrackPointsInQuery.push(_q);
      _backtrackPointsInTree.push((static_cast<Fork*>(instr))->fork());
      instr = (static_cast<InstructionWithNext*>(instr))->next();
      goto interpret_instr;
      

    case Instruction::SuccessTag:
      COP("SUCCESS");

      //DF; cout << "SUCCESS\n";      

      DOP(qDown = 0);
      return static_cast<Success*>(instr);
      
#ifdef DEBUG_NAMESPACE
    default: ICP("ICP0"); return 0;
#else
#  ifdef _SUPPRESS_WARNINGS_
    default: 
      BK::RuntimeError::report("Bad tag in OriginalFMCodeTree<..>::Retrieval::match(const Term* query,Instruction* instr)");
      return 0;
#  endif
#endif
    }; // switch (instr->tag())

 backtrack: 
  COP("BACKTRACK");

  //DF; cout << "             BACKTRACK\n";      

  BK::GlobalStopFlag::check();
  if (_backtrackPointsInTree.empty()) return 0;
  instr = _backtrackPointsInTree.pop();

  DOP(qDown = 0);
  _q = _backtrackPointsInQuery.pop();
  ASSERT(_q->isVariable() || _q->IsReference()); 

  goto interpret_instr;
}; // Retrieval::matchAgain()

}; // namespace Gem

//================================================
#undef DEBUG_NAMESPACE
#ifdef DEBUG_FORWARD_MATCHING_INDEX
#define DEBUG_NAMESPACE "OriginalFMCodeTree<MaxTermSize,MaxNumOfVariables,Alloc,Symbol,Term,SuccessInfo,InstanceId>::Output"
#endif 
#include "debugMacros.hpp"
//=================================================
#ifndef NO_DEBUG_VIS
namespace Gem
{

template <ulong MaxTermSize,ulong MaxNumOfVariables,class Alloc,class Symbol,class Term,class SuccessInfo,class InstanceId>
ostream& OriginalFMCodeTree<MaxTermSize,MaxNumOfVariables,Alloc,Symbol,Term,SuccessInfo,InstanceId>::Output::tree(ostream& str,const Instruction* tree) const
{
  CALL("tree(ostream& str,const Instruction* tree) const");
  return str;
}; // Output::tree(ostream& str,const Instruction* tree) const

}; // namespace Gem
#endif


//================================================
#undef DEBUG_NAMESPACE
#ifdef DEBUG_FORWARD_MATCHING_INDEX
#define DEBUG_NAMESPACE "OriginalFMCodeTree<MaxTermSize,MaxNumOfVariables,Alloc,Symbol,Term,SuccessInfo,InstanceId>"
#endif 
#include "debugMacros.hpp"
//=================================================

namespace Gem
{

template <ulong MaxTermSize,ulong MaxNumOfVariables,class Alloc,class Symbol,class Term,class SuccessInfo,class InstanceId>
void OriginalFMCodeTree<MaxTermSize,MaxNumOfVariables,Alloc,Symbol,Term,SuccessInfo,InstanceId>::destroyTree(Instruction* tree)
{
  CALL("destroyTree(Instruction* tree)");
  ASSERT(tree);
  Check* check;
  Down* down;
  Right* right;
  Push* push;
  Pop* pop;
  Put* put;
  Compare* compare;
  Fork* fork;
  Success* success;

  switch (tree->tag())
    { 
    case Instruction::CheckTag:
      check = static_cast<Check*>(tree);
      destroyTree(check->next());
      delete check;
      break;

    case Instruction::DownTag:
      down = static_cast<Down*>(tree);
      destroyTree(down->next());
      delete down;
      break;

    case Instruction::RightTag:
      right = static_cast<Right*>(tree);
      destroyTree(right->next());
      delete right;
      break;

    case Instruction::PushTag:
      push = static_cast<Push*>(tree);
      destroyTree(push->next());
      delete push;
      break;

    case Instruction::PopTag:
      pop = static_cast<Pop*>(tree);
      destroyTree(pop->next());
      delete pop;
      break;

    case Instruction::PutTag:
      put = static_cast<Put*>(tree);
      destroyTree(put->next());
      delete put;
      break;

    case Instruction::CompareTag:
      compare = static_cast<Compare*>(tree);
      destroyTree(compare->next());
      delete compare;
      break;

    case Instruction::ForkTag:
      fork = static_cast<Fork*>(tree);
      destroyTree(fork->next());
      destroyTree(fork->fork());
      delete fork;
      break;

    case Instruction::SuccessTag:
      success = static_cast<Success*>(tree);
      delete success;
      break;

#ifdef DEBUG_NAMESPACE
    default: ICP("ICP0"); return;
#else
#  ifdef _SUPPRESS_WARNINGS_
    default: 
      BK::RuntimeError::report("Bad tag in OriginalFMCodeTree<..>::destroyTree(Instruction* tree)");
      return ;
#  endif
#endif

    }; // switch (tree->tag())
    
}; // destroyTree(Instruction* tree)

}; // namespace Gem

//==================================================================
#endif
