//
// File:         Input.cpp
// Description:  Input signature, terms and clauses.
// Created:      Jun 04, 2002, Alexandre Riazanov, riazanov@cs.man.ac.uk
//======================================================================
#include "Input.hpp"
//======================================================================

using namespace VK;

using namespace BK;


#ifdef DEBUG_ALLOC_OBJ_TYPE
ClassDesc Input::TermList::_classDesc("Input::TermList",
				      Input::TermList::minSize(),
				      Input::TermList::maxSize());


ClassDesc Input::LiteralList::_classDesc("Input::LiteralList",
					 Input::LiteralList::minSize(),
					 Input::LiteralList::maxSize());

ClassDesc Input::ClauseList::_classDesc("Input::ClauseList",
					Input::ClauseList::minSize(),
					Input::ClauseList::maxSize());

ClassDesc Input::InterpretationList::_classDesc("Input::InterpretationList",
						Input::InterpretationList::minSize(),
						Input::InterpretationList::maxSize());

#endif	
			      
//======================================================================
#undef DEBUG_NAMESPACE
#ifdef DEBUG_INPUT
 #define DEBUG_NAMESPACE "Input::Interpretation"
#endif
#include "debugMacros.hpp"
//======================================================================

Input::Interpretation::Interpretation() :
  _theoryName(0),
  _nativeSymbolName(0) 
{
};
      
Input::Interpretation::~Interpretation()
{
  CALL("destructor ~Interpretation()");
  if (DestructionMode::isThorough())
    {
      if (_theoryName)
	Malloc::deallocate(static_cast<void*>(_theoryName),strlen(_theoryName)+1);
      if (_nativeSymbolName)
	Malloc::deallocate(static_cast<void*>(_nativeSymbolName),strlen(_nativeSymbolName)+1);
    };
}; // Input::Interpretation::~Interpretation()

void Input::Interpretation::init(const char* theory,const char* nativeSymbol) 
{
  CALL("init(const char* theory,const char* nativeSymbol)");
  _theoryName = static_cast<char*>(Malloc::allocate(strlen(theory) + 1));
  strcpy(_theoryName,theory);
  _nativeSymbolName = static_cast<char*>(Malloc::allocate(strlen(nativeSymbol) + 1));
  strcpy(_nativeSymbolName,nativeSymbol);
}; // void Input::Interpretation::init(const char* theory,const char* nativeSymbol)

void Input::Interpretation::destroy()
{
  CALL("destroy()");
	
  if (DestructionMode::isThorough())
    {
      if (_theoryName)
	Malloc::deallocate(static_cast<void*>(_theoryName),strlen(_theoryName)+1);
      if (_nativeSymbolName)
	Malloc::deallocate(static_cast<void*>(_nativeSymbolName),strlen(_nativeSymbolName)+1);
    };
}; // void Input::Interpretation::destroy()

//======================================================================
#undef DEBUG_NAMESPACE
#ifdef DEBUG_INPUT
 #define DEBUG_NAMESPACE "Input::Signature"
#endif
#include "debugMacros.hpp"
//======================================================================

const Input::Symbol* Input::Signature::registerVariable(const char* name) 
{
  CALL("registerVariable(const char* name)");
  Symbol** pos = lookUp(false,name,0UL,&_symbols);
  if (!(*pos))
    {
      *pos = new Symbol(Symbol::Variable,name,0UL);
      (*pos)->setNext(_symbolList);
      _symbolList = *pos;
      (*pos)->variableTimeStamp() = _currentVariableTimeStamp;
      (*pos)->normalisedVariable() = nextNormalisedVariable();
      return (*pos)->normalisedVariable();
    };
  if ((*pos)->type() != Symbol::Variable) return 0;
  if ((*pos)->variableTimeStamp() == _currentVariableTimeStamp)
    {
      return (*pos)->normalisedVariable();
    };
  (*pos)->variableTimeStamp() = _currentVariableTimeStamp;
  (*pos)->normalisedVariable() = nextNormalisedVariable();
  return (*pos)->normalisedVariable();
}; // Symbol* Input::Signature::registerVariable(const char* name) 
  
const Input::Symbol* Input::Signature::registerFunction(const char* name,ulong arity)
{
  CALL("registerFunction(const char* name,ulong arity)");
  Symbol** pos = lookUp(false,name,arity,&_symbols);
  if (!(*pos))
    {
      *pos = new Symbol(Symbol::Function,name,arity);
      (*pos)->setNext(_symbolList);
      _symbolList = *pos;
      return *pos;
    };
  ASSERT((*pos)->type() == Symbol::Function);
  return *pos;
}; // Symbol* Input::Signature::registerFunction(const char* name,ulong arity)
  
const Input::Symbol* Input::Signature::registerPredicate(const char* name,ulong arity)
{
  CALL("registerPredicate(const char* name,ulong arity)");
  Symbol** pos = lookUp(true,name,arity,&_symbols);
  if (!(*pos))
    {
      *pos = new Symbol(Symbol::Predicate,name,arity);
      (*pos)->setNext(_symbolList);
      _symbolList = *pos;
      return *pos;
    };
  ASSERT((*pos)->type() == Symbol::Predicate);
  return *pos;
}; // Symbol* Input::Signature::registerPredicate(const char* name,ulong arity)

Input::Symbol* Input::Signature::registerSymbol(bool predicate,const char* name,ulong arity)
{
  CALL("registerSymbol(bool predicate,const char* name,ulong arity)");
  Symbol** pos = lookUp(predicate,name,arity,&_symbols);
  if (!(*pos))
    {
      if (predicate)
	{
	  *pos = new Symbol(Symbol::Predicate,name,arity);
	}
      else
	{
	  *pos = new Symbol(Symbol::Function,name,arity);
	};
      (*pos)->setNext(_symbolList);
      _symbolList = *pos;
    };
  return *pos;
}; // Input::Symbol* Input::Signature::registerSymbol(bool predicate,const char* name,ulong arity)

bool Input::Signature::registerPrecedence(bool predicate,const char* name,ulong arity,long prec)
{
  CALL("registerPrecedence(bool predicate,const char* name,ulong arity,long prec)");
  Symbol* symbol = registerSymbol(predicate,name,arity);
  if (symbol->precedenceAssigned())
    return (symbol->precedence() == prec);
  
  Symbol** pos = lookUpPrecedence(prec,&_precedences);
  if (*pos) return false; // this precedence has been reserved for another symbol
  
  *pos = symbol;
  symbol->assignPrecedence(prec);
  return true;
}; // bool Input::Signature::registerPrecedence(bool predicate,const char* name,ulong arity,long prec)

bool Input::Signature::registerAnswerPredicate(const char* name,ulong arity)
{
  CALL("registerAnswerPredicate(const char* name,ulong arity)");
  Symbol* symbol = registerSymbol(true,name,arity);
  ASSERT(symbol->type() == Symbol::Predicate);
  symbol->markAsAnswer();
  return true;
}; // bool Input::Signature::registerAnswerPredicate(const char* name,ulong arity)


Input::Symbol* Input::Signature::nextNormalisedVariable()
{
  CALL("nextNormalisedVariable()");
  Symbol* res = *_nextNormalisedVariable;
  if (res)
    {
      _nextNormalisedVariable = &(res->greaterNames());
      return res;
    };
  res = new Symbol(Symbol::Variable,
		   normalisedVariableName(_nextNormalisedVariableNumber),
		   0UL);
  res->normalisedVariableNumber() = _nextNormalisedVariableNumber;
  ++_nextNormalisedVariableNumber;
  *_nextNormalisedVariable = res;
  _nextNormalisedVariable = &(res->greaterNames());
  return res; 
}; // Symbol* Input::Signature::nextNormalisedVariable()
  
const char* Input::Signature::normalisedVariableName(ulong varNum)
{
  CALL("normalisedVariableName(ulong varNum)");
  static char memory[256];
  sprintf(memory,"X%lu",varNum);
  return memory;
}; // const char* Input::Signature::normalisedVariableName(ulong varNum)

void Input::Signature::destroyTree(Symbol* tree)
{
  CALL("destroyTree(Symbol* tree)");
  if (tree)
    {
      destroyTree(tree->smallerNames());
      destroyTree(tree->greaterNames());
      delete tree;
    };
}; // void Input::Signature::destroyTree(Symbol* tree)
  
Input::Symbol** Input::Signature::lookUp(bool predicate,
					 const char* name,
					 ulong arity,
					 Symbol** tree)
{
  CALL("lookUp(bool predicate,const char* name,ulong arity,Symbol** tree)");
  Symbol* root = *tree;
  if (!root) return tree;
  int cmp = strcmp(name,root->printName());
  if (cmp < 0) return lookUp(predicate,name,arity,&(root->smallerNames()));
  if (cmp > 0) return lookUp(predicate,name,arity,&(root->greaterNames()));
  if (root->arity() > arity) return lookUp(predicate,name,arity,&(root->smallerNames()));
  if (root->arity() < arity) return lookUp(predicate,name,arity,&(root->greaterNames()));
  if (predicate && (root->type() != Input::Symbol::Predicate))
    return lookUp(predicate,name,arity,&(root->smallerNames()));
  if (!predicate && (root->type() == Input::Symbol::Predicate)) 
    return lookUp(predicate,name,arity,&(root->greaterNames()));     
  return tree;
}; // Symbol** Input::Signature::lookUp(bool predicate,const char* name,ulong arity,Symbol** tree) 

Input::Symbol** Input::Signature::lookUpPrecedence(long precedence,Symbol** tree)
{
  CALL("lookUpPrecedence(long precedence,Symbol** tree)");  
  Symbol* root = *tree;
  if (!root) return tree;
  if (root->precedence() == precedence) return tree;
  if (root->precedence() > precedence)
    return lookUpPrecedence(precedence,&(root->smallerPrecedences()));
  return lookUpPrecedence(precedence,&(root->greaterPrecedences()));
}; // Input::Symbol** Input::Signature::lookUpPrecedence(long precedence,Symbol** tree)




//======================================================================
#undef DEBUG_NAMESPACE
#ifdef DEBUG_INPUT
 #define DEBUG_NAMESPACE "Input::Term"
#endif
#include "debugMacros.hpp"
//======================================================================

#ifndef NO_DEBUG_VIS
namespace std
{
ostream& operator<<(ostream& str,const VK::Input::Term* t)
{
  CALL("operator<<(ostream& str,const VK::Input::Term* t)");
  str << t->top();
  if (t->arguments())
    {
      str << "(";
      for (const Input::TermList* arg = t->arguments(); arg; arg = arg->tl())
	{
	  str << arg->hd();
	  if (arg->tl()) str << ",";
	};
      str << ")";
    };
  return str;
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


#ifndef NO_DEBUG_VIS
namespace std
{
ostream& operator<<(ostream& str,const VK::Input::Clause* cl)
{
  CALL("operator<<(ostream& str,const VK::Input::Clause* cl)");
  str << cl->name() << ": ";
  for (const Input::LiteralList* lit = cl->literals(); lit; lit = lit->tl())
    {
      str << lit->hd();
      if (lit->tl()) str << ", ";
    };
  return str;
};
};
#endif


//======================================================================
#undef DEBUG_NAMESPACE
#ifdef DEBUG_INPUT
 #define DEBUG_NAMESPACE "Input"
#endif
#include "debugMacros.hpp"
//======================================================================

void Input::destroyTermList(TermList* lst)
{
  CALL("destroyTermList(TermList* lst)");
  if (lst)
    {
      destroyTermList(lst->tl());
      delete lst->hd();
      delete lst;
    };
}; // void Input::destroyTermList(TermList* lst)


void Input::destroyLiteralList(LiteralList* lst)
{
  CALL("destroyLiteralList(LiteralList* lst)");
  if (lst)
    {
      destroyLiteralList(lst->tl());
      delete lst->hd();
      delete lst;
    };
}; // void Input::destroyLiteralList(LiteralList* lst)


void Input::destroyClauseList(ClauseList* lst)
{
  CALL("destroyClauseList(ClauseList* lst)");
  if (lst)
    {
      destroyClauseList(lst->tl());
      delete lst->hd();
      delete lst;
    };
}; // void Input::destroyClauseList(ClauseList* lst)

#ifndef NO_DEBUG_VIS
ostream& operator<<(ostream& str,const Input::ClauseList* lst)
{
  while (lst)
    {
      str << lst->hd() << "\n";
      lst = lst->tl();
    };
  return str;
}; // ostream& operator<<(ostream& str,const Input::ClauseList* lst)

#endif

//======================================================================
