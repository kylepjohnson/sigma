//
// File:         TermBlock.hpp
// Description:  Building blocks for terms. 
// Created:      Oct 23, 2002, Alexandre Riazanov, riazanov@cs.man.ac.uk
// Status:       Sketch.
//============================================================================
#ifndef TERM_BLOCK_H
#define TERM_BLOCK_H
//============================================================================
#include <iostream>
#include "jargon.hpp"
#include "VampireKernelDebugFlags.hpp"
#include "VampireKernelConst.hpp"
#include "Stack.hpp"
#include "RuntimeError.hpp"
//============================================================================
#ifdef DEBUG_TERM_BLOCK
#undef DEBUG_NAMESPACE
 #define DEBUG_NAMESPACE "TermBlock"
#endif
#include "debugMacros.hpp"
//============================================================================

namespace VK 
{
  using namespace BK;
  
  class TermBlock
  {
  public:
    enum Type 
    {
      Variable = 0,
      SymbolicConstant,
      Numeral,
      CompoundTerm,
      NonconstantFunction
    };
  public:
    const Type& type() const { return _type; };   
    bool isVariable() const { return !_type; };
  protected:
    TermBlock(const Type& t) : _type(t) {};
  private:
    Type _type;    
  }; // class TermBlock

  class TermBase : public TermBlock
  {
  protected:
    TermBase(const TermBlock::Type& t) : TermBlock(t)
    {
      CALL("constructor TermBase(const TermBlock::Type& t)");
      ASSERT((t == TermBlock::Variable) ||
	     (t == TermBlock::SymbolicConstant) ||
	     (t == TermBlock::Numeral) ||
	     (t == TermBlock::CompoundTerm));
    };
  }; // class TermBase

  
  class VariableBase : public TermBase
  {
  public:
    VariableBase() : TermBase(TermBlock::Variable)
    {
    };
    ulong number() const { return _number; }; 
  protected:
    void setNumber(ulong n) { _number = n; };
  private:
    ulong _number;
  }; // class VariableBase

  class NonconstantFunctionBase : public TermBlock
  {
  public:
    NonconstantFunctionBase() : TermBlock(TermBlock::NonconstantFunction) {};
    ulong arity() const { return _arity; };
  private:
    ulong _arity;
  }; // class NonconstantFunctionBase

  class TermArguments
  {
  public:
    TermArguments() {};
    TermBase*& operator[](ulong n) { return *(_args - n); };
  private:
    TermArguments(TermBase** a) : _args(a) {};
  private:
    TermBase** _args;
    friend class CompoundTermBase;
  }; // class TermArguments


  class ConstTermArguments
  {
  public:
    ConstTermArguments() {};
    const TermBase* const & operator[](ulong n) { return *(_args - n); };
  private:
    ConstTermArguments(const TermBase* const * a) : _args(a) {};
  private:
    const TermBase* const * _args;
    friend class CompoundTermBase;
  }; // class TermArguments


  class TermArgument
  {
  public:
    TermArgument() {};
    operator bool() const { return (*_arg)->type() != TermBlock::NonconstantFunction; };
    TermBase*& term() { return (TermBase*&)(*_arg); };
    bool operator==(const TermArgument& a) { return _arg == a._arg; };
    bool operator!=(const TermArgument& a) { return _arg != a._arg; };
    bool operator<(const TermArgument& a) { return _arg > a._arg; };
    bool operator<=(const TermArgument& a) { return _arg >= a._arg; };
    bool operator>(const TermArgument& a) { return _arg < a._arg; };
    bool operator>=(const TermArgument& a) { return _arg <= a._arg; };
    TermArgument& operator++() { --_arg; return *this; };
  private:
    TermArgument(TermBlock** a) : _arg(a) {};
    TermArgument& operator=(TermBlock** a) { _arg = a; return *this; };
  private:
    TermBlock** _arg;
    friend class CompoundTermBase;
  }; // class TermArgument


  class ConstTermArgument
  {
  public:
    ConstTermArgument() {};
    operator bool() const { return (*_arg)->type() != TermBlock::NonconstantFunction; };
    const TermBase* term() { return (const TermBase*)(*_arg); };
    bool operator==(const ConstTermArgument& a) { return _arg == a._arg; };
    bool operator!=(const ConstTermArgument& a) { return _arg != a._arg; };
    bool operator<(const ConstTermArgument& a) { return _arg > a._arg; };
    bool operator<=(const ConstTermArgument& a) { return _arg >= a._arg; };
    bool operator>(const ConstTermArgument& a) { return _arg < a._arg; };
    bool operator>=(const ConstTermArgument& a) { return _arg <= a._arg; };
    ConstTermArgument& operator++() { --_arg; return *this; };
  private:
    ConstTermArgument(const TermBlock* const * a) : _arg(a) {};
  private:
    const TermBlock* const * _arg;
    friend class CompoundTermBase;
  }; // class ConstTermArgument


  class CompoundTermBase : public TermBase
  {
  public:
    CompoundTermBase(NonconstantFunctionBase* f) : 
      TermBase(TermBlock::CompoundTerm)
    {
      topSymbol() = f;
    };
    const NonconstantFunctionBase* topSymbol() const 
    { 
      return *((const NonconstantFunctionBase* const *)body()); 
    };
    ulong arity() const { return topSymbol()->arity(); };
    
    TermArguments arguments() 
    { 
      return TermArguments((TermBase**)args());
    };

    ConstTermArguments arguments() const
    { 
      return ConstTermArguments((const TermBase* const *)args());
    };

    TermArgument firstArgument() 
    { 
      return TermArgument(args()); 
    };

    TermArgument lastArgument() 
    {
      return TermArgument(body() + 1);
    };

    TermArgument endOfArguments()
    {
      return TermArgument(body());
    };

    ConstTermArgument firstArgument() const 
    { 
      return ConstTermArgument(args()); 
    };
    ConstTermArgument lastArgument() const
    {
      return ConstTermArgument(body() + 1);
    };
    ConstTermArgument endOfArguments() const
    {
      return ConstTermArgument(body());
    };

  private:
    TermBlock** body() { return (TermBlock**)(this + 1); };
    const TermBlock* const * body() const { return (const TermBlock* const *)(this + 1);};
    NonconstantFunctionBase*& topSymbol() { return *((NonconstantFunctionBase**)body()); };
    TermBlock** args() { return body() + arity(); };
    const TermBlock* const * args() const { return body() + arity(); };
    friend class ConstTermBaseTraversal;
  }; // class CompoundTermBase



  class ConstTermBaseTraversal
  {
  public:
    ConstTermBaseTraversal() : _dummyFunction()
    {
      _dummyTermBody[0] = (const TermBlock*)(&_dummyFunction);
    };

    ConstTermBaseTraversal(const TermBase* term)
    {
      _currentSubtermLocation = _dummyTermBody + 1;
      *_currentSubtermLocation = (const TermBlock*)term;
      _remainingSubterms.push((const TermBlock**)0);
    };

    void reset(const TermBase* term)
    {
      _currentSubtermLocation = _dummyTermBody + 1;
      *_currentSubtermLocation = (const TermBlock*)term;
      _remainingSubterms.reset();
      _remainingSubterms.push((const TermBlock**)0);
    };

    operator bool() { return _currentSubtermLocation != 0; };
    const TermBase* currentSubterm() 
    {
      CALL("currentSubterm()");
      ASSERT(_currentSubtermLocation);
      return (const TermBase*)(*_currentSubtermLocation); 
    };

    ConstTermBaseTraversal& operator++() 
    {
      CALL("operator++()");
      ASSERT(_currentSubtermLocation);
      ASSERT(((*_currentSubtermLocation)->type() == TermBlock::Variable) ||
	     ((*_currentSubtermLocation)->type() == TermBlock::SymbolicConstant) ||
	     ((*_currentSubtermLocation)->type() == TermBlock::Numeral) ||
	     ((*_currentSubtermLocation)->type() == TermBlock::CompoundTerm));

      if ((*_currentSubtermLocation)->type() == TermBlock::CompoundTerm)
	{
	  const TermBlock** tmp = _currentSubtermLocation - 1;
	  _currentSubtermLocation = 
	    (const TermBlock**)
	    ((CompoundTermBase*)(*_currentSubtermLocation))->args();

	  if ((*tmp)->type() != TermBlock::NonconstantFunction)
	    _remainingSubterms.push(tmp);
	}
      else
	{
	  --_currentSubtermLocation;
	  if ((*_currentSubtermLocation)->type() == TermBlock::NonconstantFunction)
	    {
	      ASSERT(_remainingSubterms.nonempty());
	      _currentSubtermLocation = _remainingSubterms.pop();
	    };
	};
      return *this;
    }; // ConstTermBaseTraversal& operator++() 

  private:
    NonconstantFunctionBase _dummyFunction;
    const TermBlock* _dummyTermBody[2];
    const TermBlock** _currentSubtermLocation;
    Stack<const TermBlock**,VampireKernelConst::MaxTermSize + 1> _remainingSubterms;
  }; // class ConstTermBaseTraversal

}; // namespace VK

//=========================================================================
#endif
