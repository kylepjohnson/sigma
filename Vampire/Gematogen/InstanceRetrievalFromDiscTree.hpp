//
// File:         InstanceRetrievalFromDiscTree.hpp
// Description:  Instance retrieval from a simpled 
//               discrimination tree. 
// Created:      Sep 04, 2001, Alexandre Riazanov, riazanov@cs.man.ac.uk
// Note:         This file is a part of the Gematogen library.
//============================================================================
#ifndef INSTANCE_RETRIEVAL_FROM_DISC_TREE_H
#define INSTANCE_RETRIEVAL_INDEX_H 
//=============================================================================
#ifndef NO_DEBUG_VIS
 #include <iostream>
#endif 
#include "jargon.hpp"
#include "Gematogen.hpp"
#include "GematogenDebugFlags.hpp"
#include "RuntimeError.hpp"
#include "Stack.hpp"
#include "Multiset.hpp"
//============================================================================
#undef DEBUG_NAMESPACE
#ifdef DEBUG_INSTANCE_RETRIEVAL_FROM_DISC_TREE
 #define DEBUG_NAMESPACE "InstanceRetrievalFromDiscTree<Alloc,Symbol,IndexedObj,DiscTreeNode,MaxTermDepth,MaxTermSize,MaxNumOfVariables>"
#endif
#include "debugMacros.hpp"
//============================================================================ 


namespace Gem
{

template <class Alloc,class Symbol,class IndexedObj,class DiscTreeNode,ulong MaxTermDepth,ulong MaxTermSize,ulong MaxNumOfVariables>
class InstanceRetrievalFromDiscTree
{ 
 public:
  class SubstTerm
  {
   public:
    class Traversal 
    {
     public:
      enum State { Func, Var, End };
     public: 
      Traversal() {};
      ~Traversal() {};
      inline void reset(const SubstTerm* term);
      const State& state() const { return _state; };
      operator bool() const { return _state != End; };
      inline void next();
      const Symbol& symbol() const { return _symbol; };
     private:
      State _state;
      Symbol _symbol;       
      const DiscTreeNode* const * _cursor;
      const DiscTreeNode* const * _end;     
    };     
 
   private:
    void initialiseInstantiation(const DiscTreeNode**& memory,const DiscTreeNode*& node);
    bool alterInstantiation(const DiscTreeNode*& node);
    bool skipInTree(const DiscTreeNode*& node) const; 

   private: 
    const DiscTreeNode* const * _begin;
    const DiscTreeNode* const * _end;
    long _numOfHoles;
   friend class InstanceRetrievalFromDiscTree;
   friend class Traversal; 
  }; // class SubstTerm

 public:
  InstanceRetrievalFromDiscTree() : _registeredVars()
  { 
   _code[0].tag = AbstractMachineInstruction::Start;
  };
  ~InstanceRetrievalFromDiscTree() {};
  // Compiling the query
  
  bool queryReset(const Symbol& topFunc,const DiscTreeNode* const & tree);
  bool queryNonconstFunc(const Symbol& f);  
  bool queryConstant(const Symbol& c);
  bool queryVariable(ulong var);
  bool queryEnd();

   // Retrieval  

  bool nextMatch(); 
  const IndexedObj* indexedObject() const;
  const SubstTerm* subst(ulong var) const { return _substitution + var; };

  // Implementation info

  
  static const char* about() 
  { 
   return GEMATOGEN_VERSION 
          ", inst. ret. from simple disc. tree" 
          ;
  };

 private:
  class AbstractMachineInstruction
  {
   public: 
    enum Tag
    {
     Start,
     Functor,
     FirstVariableOccurence,
     InstantiatedVariable,
     Success,
     Backtrack 
    };
   public:
    AbstractMachineInstruction() {}; 
    ~AbstractMachineInstruction() {}; 

    #ifndef NO_DEBUG_VIS
     static ostream& output(ostream& str,const Tag& tag);
     ostream& output(ostream& str,const SubstTerm* substitution,const AbstractMachineInstruction* codeBase) const;
    #endif
   public:
    Tag tag;
    Symbol functor;
    SubstTerm* subst;
    AbstractMachineInstruction* backtrackPoint;  
  }; // class AbstractMachineInstruction

 private:
  #ifndef NO_DEBUG_VIS
   ostream& outputMachineCode(ostream& str) const;
  #endif
 private:

   // Substitution
  const DiscTreeNode* _substMemory[MaxTermSize];
  const DiscTreeNode** _freeSubstMemory;
  SubstTerm _substitution[MaxNumOfVariables];
  
   // Compilation
  BK::Multiset<ulong,MaxNumOfVariables> _registeredVars; 
  AbstractMachineInstruction* _lastBacktrackPoint;

   // Abstract machine memory
  AbstractMachineInstruction _code[MaxTermSize+2];   
  AbstractMachineInstruction* _currentInstruction;
  const DiscTreeNode* _currentNode;
}; // class InstanceRetrievalFromDiscTree<Alloc,Symbol,IndexedObj,DiscTreeNode,MaxTermDepth,MaxTermSize,MaxNumOfVariables>


}; // namespace Gem

//============================================================================
#undef DEBUG_NAMESPACE
#ifdef DEBUG_INSTANCE_RETRIEVAL_FROM_DISC_TREE
 #define DEBUG_NAMESPACE "InstanceRetrievalFromDiscTree<Alloc,Symbol,IndexedObj,DiscTreeNode,MaxTermDepth,MaxTermSize,MaxNumOfVariables>"
#endif
#include "debugMacros.hpp"
//============================================================================ 

namespace Gem
{

template <class Alloc,class Symbol,class IndexedObj,class DiscTreeNode,ulong MaxTermDepth,ulong MaxTermSize,ulong MaxNumOfVariables>
inline bool InstanceRetrievalFromDiscTree<Alloc,Symbol,IndexedObj,DiscTreeNode,MaxTermDepth,MaxTermSize,MaxNumOfVariables>::queryReset(const Symbol& topFunc,const DiscTreeNode* const & tree)
{
 CALL("queryReset(const Symbol& topFunc,const DiscTreeNode* const & tree)");
 if (!tree) return false;
 _currentNode = tree;
 check_node: 
  if (_currentNode->val() == topFunc) 
   {
    _currentNode = _currentNode->next();
    _currentInstruction = _code + 1;
    _registeredVars.reset();
    _lastBacktrackPoint = (AbstractMachineInstruction*)0;
    return true;
   }
  else
   {
    if (_currentNode->val() < topFunc)
     {
      _currentNode = _currentNode->alt();
      if (_currentNode) goto check_node;
     };
    // (_currentNode->val() > topFunc) or (!_currentNode)
    return false;
   };
} // bool InstanceRetrievalFromDiscTree<..>::queryReset(const Symbol& topFunc,const DiscTreeNode* const & tree)

template <class Alloc,class Symbol,class IndexedObj,class DiscTreeNode,ulong MaxTermDepth,ulong MaxTermSize,ulong MaxNumOfVariables>
inline bool InstanceRetrievalFromDiscTree<Alloc,Symbol,IndexedObj,DiscTreeNode,MaxTermDepth,MaxTermSize,MaxNumOfVariables>::queryNonconstFunc(const Symbol& f)
{
 CALL("queryNonconstFunc(const Symbol& f)");
 _currentInstruction->tag = AbstractMachineInstruction::Functor;
 _currentInstruction->functor = f;
 _currentInstruction->backtrackPoint = _lastBacktrackPoint; 
 _currentInstruction++;
 return true;  
} // bool InstanceRetrievalFromDiscTree<..>::queryNonconstFunc(const Symbol& f)

template <class Alloc,class Symbol,class IndexedObj,class DiscTreeNode,ulong MaxTermDepth,ulong MaxTermSize,ulong MaxNumOfVariables>  
inline bool InstanceRetrievalFromDiscTree<Alloc,Symbol,IndexedObj,DiscTreeNode,MaxTermDepth,MaxTermSize,MaxNumOfVariables>::queryConstant(const Symbol& c)
{
 CALL("queryConstant(const Symbol& c)");
 _currentInstruction->tag = AbstractMachineInstruction::Functor;
 _currentInstruction->functor = c;
 _currentInstruction->backtrackPoint = _lastBacktrackPoint; 
 _currentInstruction++;
 return true; 
} // bool InstanceRetrievalFromDiscTree<..>::queryConstant(const Symbol& c)

template <class Alloc,class Symbol,class IndexedObj,class DiscTreeNode,ulong MaxTermDepth,ulong MaxTermSize,ulong MaxNumOfVariables>
inline bool InstanceRetrievalFromDiscTree<Alloc,Symbol,IndexedObj,DiscTreeNode,MaxTermDepth,MaxTermSize,MaxNumOfVariables>::queryVariable(ulong var)
{
 CALL("queryVariable(ulong var)");
 _currentInstruction->backtrackPoint = _lastBacktrackPoint; 
 if (_registeredVars.scoreOf(var))
  {
   _currentInstruction->tag = AbstractMachineInstruction::InstantiatedVariable;
  } 
 else
  {
   _registeredVars.add(var);
   _currentInstruction->tag = AbstractMachineInstruction::FirstVariableOccurence;
   _lastBacktrackPoint = _currentInstruction;
  }; 
 _currentInstruction->subst = _substitution + var;
 _currentInstruction++;
 return true;
} //  bool InstanceRetrievalFromDiscTree<..>::queryVariable(ulong var)

template <class Alloc,class Symbol,class IndexedObj,class DiscTreeNode,ulong MaxTermDepth,ulong MaxTermSize,ulong MaxNumOfVariables>  
inline bool InstanceRetrievalFromDiscTree<Alloc,Symbol,IndexedObj,DiscTreeNode,MaxTermDepth,MaxTermSize,MaxNumOfVariables>::queryEnd()
{
 CALL("queryEnd()");
 _currentInstruction->tag = AbstractMachineInstruction::Success;
 _currentInstruction++;
 _currentInstruction->tag = AbstractMachineInstruction::Backtrack;
 _currentInstruction->backtrackPoint = _lastBacktrackPoint; 

 //DF; outputMachineCode(cout << "\n\n") << "\n";

 _currentInstruction = _code;
 return true;
} // bool InstanceRetrievalFromDiscTree<..>::queryEnd()



template <class Alloc,class Symbol,class IndexedObj,class DiscTreeNode,ulong MaxTermDepth,ulong MaxTermSize,ulong MaxNumOfVariables>  
inline bool InstanceRetrievalFromDiscTree<Alloc,Symbol,IndexedObj,DiscTreeNode,MaxTermDepth,MaxTermSize,MaxNumOfVariables>::nextMatch()
{
 CALL("nextMatch()"); 
 
 Symbol sym;

 perform_instr:

 //DF;  _currentInstruction->output(cout << "   ",_substitution,_code) << "\n"; 

  COP("perform_instr");
  switch (_currentInstruction->tag)
   {
    case AbstractMachineInstruction::Start:
     ASSERT(_currentNode);  
     ASSERT(!_currentNode->isLeaf());
     _freeSubstMemory = _substMemory;
     _currentInstruction++; 
     goto perform_instr;
     
    case AbstractMachineInstruction::Functor:  
     sym = _currentInstruction->functor;
     ASSERT(_currentNode);  
     ASSERT(!_currentNode->isLeaf()); 
     while (_currentNode->val() < sym)
      {        
       _currentNode = _currentNode->alt();
       if (!_currentNode) goto backtrack;      
      };
     if (_currentNode->val() == sym)
      { 
       _currentNode = _currentNode->next();
       _currentInstruction++;
       goto perform_instr;
      };      
     // _currentNode->val() > sym
     goto backtrack;      

    case AbstractMachineInstruction::FirstVariableOccurence:
     ASSERT(_currentNode);  
     ASSERT(!_currentNode->isLeaf()); 
     _currentInstruction->subst->initialiseInstantiation(_freeSubstMemory,_currentNode);
     ASSERT(_currentNode); 
     _currentInstruction++;
     goto perform_instr;

    case AbstractMachineInstruction::InstantiatedVariable:
     ASSERT(_currentNode);   
     ASSERT(!_currentNode->isLeaf());
     if (_currentInstruction->subst->skipInTree(_currentNode))
      {
       _currentInstruction++;
       goto perform_instr;
      }
     else goto backtrack;


    case AbstractMachineInstruction::Success:
     ASSERT(_currentNode); 
     ASSERT(_currentNode->isLeaf());
     _currentInstruction++;
     return true;   
    
    case AbstractMachineInstruction::Backtrack: goto backtrack;

    #ifdef DEBUG_NAMESPACE 
     default: ICP("ICP0"); return false; 
    #else 
     #ifdef _SUPPRESS_WARNINGS_
      default: 
       RuntimeError::report("Bad instruction tag in InstanceRetrievalFromDiscTree<..>::nextMatch()");  
       return false; 
     #endif 
    #endif
   };

 backtrack: 
  COP("backtrack");
  _currentInstruction = _currentInstruction->backtrackPoint;
  if (_currentInstruction)
   {
    ASSERT(_currentInstruction->tag == AbstractMachineInstruction::FirstVariableOccurence);
    if (_currentInstruction->subst->alterInstantiation(_currentNode))
     {
      _freeSubstMemory = (const DiscTreeNode**)_currentInstruction->subst->_end;
      _currentInstruction++;
      goto perform_instr;
     }     
    else goto backtrack;   
   }
  else return false;
} // bool InstanceRetrievalFromDiscTree<..>::nextMatch()


template <class Alloc,class Symbol,class IndexedObj,class DiscTreeNode,ulong MaxTermDepth,ulong MaxTermSize,ulong MaxNumOfVariables>  
inline const IndexedObj* InstanceRetrievalFromDiscTree<Alloc,Symbol,IndexedObj,DiscTreeNode,MaxTermDepth,MaxTermSize,MaxNumOfVariables>::indexedObject() const
{
 CALL("indexedObject() const");
 ASSERT(_currentNode && _currentNode->isLeaf());
 return *(_currentNode->indexedObjAddr());
} 


#ifndef NO_DEBUG_VIS

 template <class Alloc,class Symbol,class IndexedObj,class DiscTreeNode,ulong MaxTermDepth,ulong MaxTermSize,ulong MaxNumOfVariables>  
 inline ostream& InstanceRetrievalFromDiscTree<Alloc,Symbol,IndexedObj,DiscTreeNode,MaxTermDepth,MaxTermSize,MaxNumOfVariables>::outputMachineCode(ostream& str) const
 {
  CALL("outputMachineCode(ostream& str) const");
  const AbstractMachineInstruction* instr = _code; 
  do 
   {
    instr->output(str,_substitution,_code) << "\n";
    instr++;
   }
  while (instr->tag != AbstractMachineInstruction::Backtrack);
  instr->output(str,_substitution,_code) << "\n";
  return str; 
 }

#endif



}; // namespace Gem


//============================================================================
#undef DEBUG_NAMESPACE
#ifdef DEBUG_INSTANCE_RETRIEVAL_FROM_DISC_TREE
 #define DEBUG_NAMESPACE "InstanceRetrievalFromDiscTree<Alloc,Symbol,IndexedObj,DiscTreeNode,MaxTermDepth,MaxTermSize,MaxNumOfVariables>::AbstractMachineInstruction"
#endif
#include "debugMacros.hpp"
//============================================================================ 

namespace Gem
{

#ifndef NO_DEBUG_VIS

 template <class Alloc,class Symbol,class IndexedObj,class DiscTreeNode,ulong MaxTermDepth,ulong MaxTermSize,ulong MaxNumOfVariables>
 inline ostream& InstanceRetrievalFromDiscTree<Alloc,Symbol,IndexedObj,DiscTreeNode,MaxTermDepth,MaxTermSize,MaxNumOfVariables>::AbstractMachineInstruction::output(ostream& str,const Tag& tag)
 {
  CALL("output(ostream& str,const Tag& tag)");
  switch(tag)
   {
    case Start: return str << "Start";
    case Functor: return str << "Functor";
    case FirstVariableOccurence: return str << "FirstVariableOccurence";
    case InstantiatedVariable: return str << "InstantiatedVariable";
    case Success: return str << "Success";
    case Backtrack: return str << "Backtrack"; 
    default: return str << "UnknownInstructionType";
   };  
 }

 template <class Alloc,class Symbol,class IndexedObj,class DiscTreeNode,ulong MaxTermDepth,ulong MaxTermSize,ulong MaxNumOfVariables>
 inline ostream& InstanceRetrievalFromDiscTree<Alloc,Symbol,IndexedObj,DiscTreeNode,MaxTermDepth,MaxTermSize,MaxNumOfVariables>::AbstractMachineInstruction::output(ostream& str,const SubstTerm* substitution,const AbstractMachineInstruction* codeBase) const
 {
  CALL("output(ostream& str,const SubstTerm* substitution,const AbstractMachineInstruction* codeBase) const");
  output(str << (this - codeBase) << " ",tag) << " "; 
  switch(tag)
   {
    case Start: break; 
    case Functor:
     str << functor; 
     str << " ["; 
     if (backtrackPoint) { str << (backtrackPoint - codeBase); } else str << "NULL";
     str << "]";
     break;
    case FirstVariableOccurence: 
     str << (subst - substitution);
     str << " ["; 
     if (backtrackPoint) { str << (backtrackPoint - codeBase); } else str << "NULL";
     str << "]";
     break;
    case InstantiatedVariable:  
     str << (subst - substitution);
     str << " ["; 
     if (backtrackPoint) { str << (backtrackPoint - codeBase); } else str << "NULL";
     str << "]";
     break;
    case Success:
     break;
    case Backtrack:
     str << " ["; 
     if (backtrackPoint) { str << (backtrackPoint - codeBase); } else str << "NULL";
     str << "]";
     break;
    default: break;
   }; 
  return str;
 }

#endif


}; // namespace Gem


//============================================================================
#undef DEBUG_NAMESPACE
#ifdef DEBUG_INSTANCE_RETRIEVAL_FROM_DISC_TREE
 #define DEBUG_NAMESPACE "InstanceRetrievalFromDiscTree<Alloc,Symbol,IndexedObj,DiscTreeNode,MaxTermDepth,MaxTermSize,MaxNumOfVariables>::SubstTerm"
#endif
#include "debugMacros.hpp"
//============================================================================ 

namespace Gem
{
template <class Alloc,class Symbol,class IndexedObj,class DiscTreeNode,ulong MaxTermDepth,ulong MaxTermSize,ulong MaxNumOfVariables>
inline void InstanceRetrievalFromDiscTree<Alloc,Symbol,IndexedObj,DiscTreeNode,MaxTermDepth,MaxTermSize,MaxNumOfVariables>::SubstTerm::initialiseInstantiation(const DiscTreeNode**& memory,const DiscTreeNode*& node)
{
 CALL("initialiseInstantiation(const DiscTreeNode**& memory,const DiscTreeNode*& node)");
 ASSERT(node);
 _begin = memory;
 *memory = node;
 memory++;
 _numOfHoles = node->val().arity();
 node = node->next();
 while (_numOfHoles)
  {
   ASSERT(node);
   ASSERT(!node->isLeaf());  
   ASSERT(_numOfHoles > 0);
   *memory = node;
   memory++;
   (_numOfHoles += node->val().arity())--;
   node = node->next();
  };
 ASSERT(node);
 _end = memory;
} // void InstanceRetrievalFromDiscTree<..>::SubstTerm::initialiseInstantiation(const DiscTreeNode**& memory,const DiscTreeNode*& node)

    
template <class Alloc,class Symbol,class IndexedObj,class DiscTreeNode,ulong MaxTermDepth,ulong MaxTermSize,ulong MaxNumOfVariables>
inline bool InstanceRetrievalFromDiscTree<Alloc,Symbol,IndexedObj,DiscTreeNode,MaxTermDepth,MaxTermSize,MaxNumOfVariables>::SubstTerm::alterInstantiation(const DiscTreeNode*& node)
{
 CALL("alterInstantiation(const DiscTreeNode*& node)");
 ASSERT(!_numOfHoles); 
 ASSERT(_end > _begin);
 const DiscTreeNode** memory = (const DiscTreeNode**)(_end - 1);
 ASSERT(!(*memory)->val().arity());
 node = *memory;   
 (_numOfHoles -= node->val().arity())++;
 while (!node->alt())
  {
   if (memory == _begin) return false; // nowhere to backtrack
   memory--;
   node = *memory;  
   ASSERT(node);
   (_numOfHoles -= node->val().arity())++;
  };
 // here node->alt() != 0
 ASSERT(_numOfHoles > 0); 
 node=node->alt();
 *memory = node;
 memory++;
 (_numOfHoles += node->val().arity())--;
 node = node->next(); 
 while (_numOfHoles)
  {    
   ASSERT(_numOfHoles > 0);
   *memory = node;
   memory++;
   (_numOfHoles += node->val().arity())--;
   node = node->next();
  };
 _end = memory;
 return true;
} // bool InstanceRetrievalFromDiscTree<..>::SubstTerm::alterInstantiation(const DiscTreeNode*& node)
    

template <class Alloc,class Symbol,class IndexedObj,class DiscTreeNode,ulong MaxTermDepth,ulong MaxTermSize,ulong MaxNumOfVariables>
inline bool InstanceRetrievalFromDiscTree<Alloc,Symbol,IndexedObj,DiscTreeNode,MaxTermDepth,MaxTermSize,MaxNumOfVariables>::SubstTerm::skipInTree(const DiscTreeNode*& node) const
{
 CALL("skipInTree(const DiscTreeNode*& node) const");
 ASSERT(node);
 ASSERT(!_numOfHoles); 
 ASSERT(_end > _begin);
 Symbol sym; 
 for (const DiscTreeNode* const * p = _begin; p < _end; p++)
  { 
   sym = (*p)->val();
   check_node:
    ASSERT(node && (!node->isLeaf())); 
    if (node->val() == sym)
     {
      node = node->next();
     }
    else
     { 
      if (node->val() < sym)
       {
        node = node->alt();
        if (node) goto check_node;
       };
      // !node or node->val() > sym
      return false;
     };
  };
 return true;
} // bool InstanceRetrievalFromDiscTree<..>::SubstTerm::skipInTree(const DiscTreeNode*& node) const


}; // namespace Gem


//============================================================================
#undef DEBUG_NAMESPACE
#ifdef DEBUG_INSTANCE_RETRIEVAL_FROM_DISC_TREE
 #define DEBUG_NAMESPACE "InstanceRetrievalFromDiscTree<Alloc,Symbol,IndexedObj,DiscTreeNode,MaxTermDepth,MaxTermSize,MaxNumOfVariables>::SubstTerm::Traversal"
#endif
#include "debugMacros.hpp"
//===========================================================================

namespace Gem
{

template <class Alloc,class Symbol,class IndexedObj,class DiscTreeNode,ulong MaxTermDepth,ulong MaxTermSize,ulong MaxNumOfVariables>
inline void InstanceRetrievalFromDiscTree<Alloc,Symbol,IndexedObj,DiscTreeNode,MaxTermDepth,MaxTermSize,MaxNumOfVariables>::SubstTerm::Traversal::reset(const SubstTerm* term) 
{ 
 CALL("reset(const SubstTerm* term)");
 _cursor = term->_begin; 
 _end = term->_end;
 ASSERT(_cursor < _end);
 ASSERT(!(*_cursor)->isLeaf());
 _symbol = (*_cursor)->val();
 _state = (_symbol.isVariable()) ? Var : Func; 
} // void InstanceRetrievalFromDiscTree<..>::SubstTerm::Traversal::reset(const SubstTerm* term)

template <class Alloc,class Symbol,class IndexedObj,class DiscTreeNode,ulong MaxTermDepth,ulong MaxTermSize,ulong MaxNumOfVariables>
inline void InstanceRetrievalFromDiscTree<Alloc,Symbol,IndexedObj,DiscTreeNode,MaxTermDepth,MaxTermSize,MaxNumOfVariables>::SubstTerm::Traversal::next()
{
 CALL("next()");
 ASSERT(_cursor < _end);
 _cursor++;
 if (_cursor == _end) { _state = End; return; };
 ASSERT(!(*_cursor)->isLeaf());
 _symbol = (*_cursor)->val();
 _state = (_symbol.isVariable()) ? Var : Func; 
} // void InstanceRetrievalFromDiscTree<..>::SubstTerm::Traversal::next()


}; // namespace Gem

//======================================================================
#endif
