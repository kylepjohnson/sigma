//
// File:         Input.hpp
// Description:  Input signature, terms and clauses.
// Created:      Jun 04, 2002, Alexandre Riazanov, riazanov@cs.man.ac.uk
//======================================================================
#ifndef INPUT_H
#define INPUT_H
//======================================================================
#ifndef NO_DEBUG_VIS
#include <iostream>
#endif
#include <cstring>
#include <cstdio>
#include "jargon.hpp"
#include "VampireKernelDebugFlags.hpp"
#include "DestructionMode.hpp"
#include "GList.hpp"
#include "Malloc.hpp"
#include "VampireKernel.hpp"
//======================================================================
#undef DEBUG_NAMESPACE
#ifdef DEBUG_INPUT
 #define DEBUG_NAMESPACE "Input"
#endif
#include "debugMacros.hpp"
//======================================================================

namespace VK
{
class Input
{
 public:
  class Symbol;
  class Signature;
  class Term;
  class Literal;
  class Clause;
  class Interpretation;
  typedef BK::GList<BK::Malloc,Term*,Input> TermList;
  typedef BK::GList<BK::Malloc,Literal*,Input> LiteralList;
  typedef BK::GList<BK::Malloc,Clause*,Input> ClauseList;
  typedef BK::GList<BK::Malloc,Interpretation,Input> InterpretationList;
 public:
  static void destroyTermList(TermList* lst);
  static void destroyLiteralList(LiteralList* lst);
  static void destroyClauseList(ClauseList* lst);
};
}; // namespace VK


#ifndef NO_DEBUG_VIS
namespace std
{
ostream& operator<<(ostream& str,const VK::Input::ClauseList* lst);
};
#endif

//======================================================================
#undef DEBUG_NAMESPACE
#ifdef DEBUG_INPUT
 #define DEBUG_NAMESPACE "Input::Interpretation"
#endif
#include "debugMacros.hpp"
//======================================================================

namespace VK
{
  class Input::Interpretation
    {
    public:
      Interpretation();      
      ~Interpretation();
      void init(const char* theory,const char* nativeSymbol);
      void destroy();
      const char* theoryName() const { return _theoryName; };
      const char* nativeSymbolName() const { return _nativeSymbolName; };
    private:
      char* _theoryName;
      char* _nativeSymbolName;
    };

}; // namespace VK

//======================================================================
#undef DEBUG_NAMESPACE
#ifdef DEBUG_INPUT
 #define DEBUG_NAMESPACE "Input::Symbol"
#endif
#include "debugMacros.hpp"
//======================================================================

namespace VK
{
class Input::Symbol
{
 public:
  enum Type { Variable, Function, Predicate };
  const Type& type() const { return _type; };
  const char* printName() const { return _printName; };
  ulong number() const { return _number; };
  void assignNumber(ulong n) { _number = n; };
  ulong arity() const 
    {
      CALL("arity() const");
      return _arity; 
    };
  ulong variableNumber() const { return _normalisedVariableNumber; };
  bool precedenceAssigned() const { return _precedenceAssigned; };
  long precedence() const 
    {
      CALL("precedence() const");
      ASSERT(precedenceAssigned());
      return _precedence;
    };

  void addInterpretation(const char* theory,const char* nativeSymbol)
    {
      CALL("addInterpretation(const char* theory,const char* nativeSymbol)");
      _interpretations = 
	new InterpretationList(_interpretations);
      _interpretations->hd().init(theory,nativeSymbol);
    };

  const InterpretationList* interpretations() const { return _interpretations; };

  bool isAnswer() const { return _isAnswer; };

  bool weightAssigned() const { return _weightAssigned; };
  VampireKernel::TermWeightType weightConstPart() const { return _weightConstPart; };


  void setWeightConstPart(VampireKernel::TermWeightType c)
    {
      _weightConstPart = c;
      _weightAssigned = true;
    };

  Symbol* next() { return _next; };

 private:
  Symbol(const Type& t,const char* nm,ulong a) :
    _type(t),
    _arity(a),
    _precedenceAssigned(false),
    _isAnswer(false),
    _weightConstPart(1L),
    _weightAssigned(false),
    _interpretations(0),
    _variableTimeStamp(0UL),
    _normalisedVariable(0),
    _smallerNames(0),
    _greaterNames(0),
    _smallerPrecedences(0),
    _greaterPrecedences(0),
    _next(0)
    {
      CALL("constructor Symbol(const Type& t,const char* nm,ulong a)");
      _printName = static_cast<char*>(BK::Malloc::allocate(strlen(nm) + 1));
      strcpy(_printName,nm);
    };
  ~Symbol()
    {
      CALL("destructor ~Symbol()");
      if (BK::DestructionMode::isThorough())
	{
		  BK::Malloc::deallocate(static_cast<void*>(_printName),strlen(_printName) + 1);
	  InterpretationList::destroyList(_interpretations);
	};
    };
  
  void* operator new(size_t)
    {
      CALL("operator new(size_t)");
      return BK::Malloc::allocate(sizeof(Symbol));
    };
  
  void operator delete(void* obj)
    {
      CALL("operator delete(void* obj)");
	  BK::Malloc::deallocate(obj,sizeof(Symbol));
    };

  void setType(const Type& t) { _type = t; };

  ulong& normalisedVariableNumber() 
    {
      CALL("normalisedVariableNumber()");
      ASSERT(_type == Variable);
      return _normalisedVariableNumber; 
    };
  ulong& variableTimeStamp() 
    {
      CALL("variableTimeStamp()");
      ASSERT(_type == Variable);
      return _variableTimeStamp; 
    };
  Symbol*& normalisedVariable() 
    {
      CALL("normalisedVariable()");
      ASSERT(_type == Variable);
      return _normalisedVariable; 
    };
  Symbol*& smallerNames() { return _smallerNames; };
  Symbol*& greaterNames() { return _greaterNames; };
  Symbol*& smallerPrecedences() { return _smallerPrecedences; };
  Symbol*& greaterPrecedences() { return _greaterPrecedences; };
  
  void assignPrecedence(long prec) 
    {
      CALL("assignPrecedence(long prec)");
      ASSERT(!precedenceAssigned());
      _precedenceAssigned = true;
      _precedence = prec;
    };
  
  void markAsAnswer() { _isAnswer = true; };
  void setNext(Symbol* s) { _next = s; };

 private:
  Type _type;
  char* _printName;
  ulong _number;
  ulong _arity;
  bool _precedenceAssigned;
  long _precedence;
  bool _isAnswer;
  VampireKernel::TermWeightType _weightConstPart;
  bool _weightAssigned;
  InterpretationList* _interpretations;
  ulong _normalisedVariableNumber;
  ulong _variableTimeStamp;
  Symbol* _normalisedVariable;
  Symbol* _smallerNames;
  Symbol* _greaterNames;
  Symbol* _smallerPrecedences;
  Symbol* _greaterPrecedences;
  Symbol* _next;
  friend class Input::Signature;  
}; // class Input::Symbol

}; // namespace VK

#ifndef NO_DEBUG_VIS
namespace std
{
inline
ostream& operator<<(ostream& str,const VK::Input::Symbol* s)
{
  return str << s->printName();
};
};
#endif

//======================================================================
#undef DEBUG_NAMESPACE
#ifdef DEBUG_INPUT
 #define DEBUG_NAMESPACE "Input::Signature"
#endif
#include "debugMacros.hpp"
//======================================================================

namespace VK
{
class Input::Signature
{
 public:
  Signature() : 
    _symbols(0),
    _normalisedVariables(0),
    _nextNormalisedVariable(&_normalisedVariables),
    _nextNormalisedVariableNumber(0UL),
    _currentVariableTimeStamp(0UL),
    _precedences(0),
    _symbolList(0)
    {
    };
  ~Signature()
    {
      CALL("destructor ~Signature()");
      if (BK::DestructionMode::isThorough())
	{
	  destroyTree(_symbols);
	  destroyTree(_normalisedVariables);
	};
    };
  
  void resetVariables() 
    {
      _nextNormalisedVariable = &_normalisedVariables;
      ++_currentVariableTimeStamp;
    };

  const Symbol* registerVariable(const char* name);
  const Symbol* registerFunction(const char* name,ulong arity);
  const Symbol* registerPredicate(const char* name,ulong arity);
  Symbol* registerSymbol(bool predicate,const char* name,ulong arity);
  bool registerPrecedence(bool predicate,const char* name,ulong arity,long prec);
  bool registerAnswerPredicate(const char* name,ulong arity);
  Symbol* symbolList() { return _symbolList; };

 private:
  Symbol* nextNormalisedVariable();
  static const char* normalisedVariableName(ulong varNum);
  static void destroyTree(Symbol* tree);
  static Symbol** lookUp(bool predicate,const char* name,ulong arity,Symbol** tree);
  static Symbol** lookUpPrecedence(long precedence,Symbol** tree);
 private:
  Symbol* _symbols;
  Symbol* _normalisedVariables;
  Symbol** _nextNormalisedVariable;
  ulong _nextNormalisedVariableNumber;
  ulong _currentVariableTimeStamp;
  Symbol* _precedences;
  Symbol* _symbolList;
}; // class Input::Signature

}; // namespace VK

//======================================================================
#undef DEBUG_NAMESPACE
#ifdef DEBUG_INPUT
 #define DEBUG_NAMESPACE "Input::Term"
#endif
#include "debugMacros.hpp"
//======================================================================

namespace VK
{
class Input::Term
{
 public:
  Term(const Symbol* t) : _top(t), _arguments(0) {};
  Term(const Symbol* t,TermList* args) : _top(t), _arguments(args) {};
  ~Term()
    {
      CALL("destructor ~Term()");
      if (BK::DestructionMode::isThorough())
	{
	  destroyTermList(_arguments);
	};
    };
  void* operator new(size_t)
    {
      CALL("operator new(size_t)");
      return BK::Malloc::allocate(sizeof(Term));
    };
  void operator delete(void* obj)
    {
      CALL("operator delete(void* obj)");
      BK::Malloc::deallocate(obj,sizeof(Term));
    };
  const Symbol* top() const { return _top; };
  const TermList* arguments() const { return _arguments; };

 private:
  Term() {};
 private:
  const Symbol* _top;
  TermList* _arguments;
}; // class Input::Term
}; // namespace VK

#ifndef NO_DEBUG_VIS
namespace std
{
ostream& operator<<(ostream& str,const VK::Input::Term* t);
};
#endif

//======================================================================
#undef DEBUG_NAMESPACE
#ifdef DEBUG_INPUT
 #define DEBUG_NAMESPACE "Input::Literal"
#endif
#include "debugMacros.hpp"
//======================================================================

namespace VK
{
class Input::Literal
{
 public:
  Literal(bool positive,Term* at) : 
    _isPositive(positive),
    _atom(at)
    {
    };
  ~Literal() 
    {
      CALL("destructor ~Literal()");
      if (BK::DestructionMode::isThorough())
	{
	  delete _atom;
	};
    };   
  void* operator new(size_t)
    {
      CALL("operator new(size_t)");
      return BK::Malloc::allocate(sizeof(Literal));
    };
  void operator delete(void* obj)
    {
      CALL("operator delete(void* obj)");
      BK::Malloc::deallocate(obj,sizeof(Literal));
    };
  bool isPositive() const { return _isPositive; };
  const Term* atom() const { return _atom; };
 private:
  Literal() {}; 
 private:
  bool _isPositive;
  Term* _atom;
}; // class Input::Literal
}; // namespace VK

#ifndef NO_DEBUG_VIS
namespace std
{
inline
ostream& operator<<(ostream& str,const VK::Input::Literal* lit)
{
  if (!lit->isPositive()) str << "~";
  return str << lit->atom();
};
};
#endif

//======================================================================
#undef DEBUG_NAMESPACE
#ifdef DEBUG_INPUT
 #define DEBUG_NAMESPACE "Input::Clause"
#endif
#include "debugMacros.hpp"
//======================================================================


namespace VK
{
class Input::Clause
{
 public:
  enum Category { Unclassified, Axiom, Conjecture, Hypothesis };
 public:
  Clause(const char* nm,LiteralList* lits) : 
    _literals(lits), 
    _category(Unclassified) 
    {
      _name = static_cast<char*>(BK::Malloc::allocate(strlen(nm) + 1));
      strcpy(_name,nm);
    };
  ~Clause()
    {
      CALL("destructor ~Clause()");
      if (BK::DestructionMode::isThorough())
	{
	  destroyLiteralList(_literals);
	  BK::Malloc::deallocate(static_cast<void*>(_name),strlen(_name) + 1);
	};
    };
  void* operator new(size_t)
    {
      CALL("operator new(size_t)");
      return BK::Malloc::allocate(sizeof(Clause));
    };
  void operator delete(void* obj)
    {
      CALL("operator delete(void* obj)");
      BK::Malloc::deallocate(obj,sizeof(Clause));
    }; 
  const LiteralList* literals() const { return _literals; };
  const Category& getCategory() const { return _category; };
  void setCategory(const Category& cat) { _category = cat; };
  const char* name() const { return _name; };
 private:
  Clause() {};
 private:
  LiteralList* _literals;
  Category _category;
  char* _name;
}; // class Input::Clause

}; // namespace VK



#ifndef NO_DEBUG_VIS
namespace std
{
ostream& operator<<(ostream& str,const VK::Input::Clause* cl);
};
#endif


//======================================================================
#endif
