//
// File:         PathIndex.hpp
// Description:  Tries for optimised path indexing.
// Created:      Sep 12, 2001, Alexandre Riazanov, riazanov@cs.man.ac.uk
// Revised:      Dec 11, 2002, Alexandre Riazanov, riazanov@cs.man.ac.uk
//               Some method definitions moved inside the class declarations
//               to please VC++ 6.0
// Note:         This file is a part of the Gematogen library.
//============================================================================
#ifndef PATH_INDEX_H
//=============================================================================
#define PATH_INDEX_H 
#ifndef NO_DEBUG_VIS
#  include <iostream>
#endif
#include "jargon.hpp"
#include "GematogenDebugFlags.hpp" 
#include "Stack.hpp"
#ifdef DEBUG_ALLOC_OBJ_TYPE 
#  include "ClassDesc.hpp"
#endif
#include "DestructionMode.hpp"
//============================================================================
#undef DEBUG_NAMESPACE
#ifdef DEBUG_PATH_INDEX
 #define DEBUG_NAMESPACE "PathIndex<Alloc,Symbol,FuncIndObj,ConstIndObj,MaxTermDepth,MaxTermSize>"
#endif
#include "debugMacros.hpp"
//============================================================================

namespace Gem
{
template <class Alloc,class Symbol,class FuncIndObj,class ConstIndObj,ulong MaxTermDepth,ulong MaxTermSize>
// Requirements: 
// operator {FuncIndObj,ConstIndObj}::bool() const 
// void {FuncIndObj,ConstIndObj}::{init,destroy}()

class PathIndex
{
 public: 
  class Integrator;
  class Retrieval;
  class Removal;
  class Trie;
  //class Destruction;
 public:
  class Node
  {
   public:
    Node(const Symbol& f) : _func(f), _altern(0) // for nonconst. functors only
    {
     CALL("Node::(constructor Node(const Symbol& f))");
     ASSERT(_func.isComplex());
     ulong arity = f.arity();
     ASSERT(arity);
     Node** deeperPos = deeper();
     for (ulong argn = 0; argn < arity; argn++) deeperPos[argn] = 0; 
     indObj().init();
    }; 
    Node(const Symbol& c,bool) 
     : _func(c), _altern(0) // for constants only
    {
     CALL("Node::(constructor Node(const Symbol& c))");
     ASSERT(_func.isComplex());
     ASSERT(!(_func.arity()));
     constIndObj().init();
    };

    ~Node() 
      {
	CALL("Node::destructor ~Node()");
	if (BK::DestructionMode::isThorough())
	  {
	    if (isConstant())
	      {
		constIndObj().destroy();
	      }
	    else
	      indObj().destroy();
	  };
	  
      };
    void* operator new(size_t,ulong arity) // for nonconst. functors only
    {
      CALL("operator new(size_t,ulong arity)");
      ASSERT(arity);
#ifdef DEBUG_ALLOC_OBJ_TYPE
      ALWAYS(_funcClassDesc.registerAllocated(sizeof(FuncIndObj) + sizeof(Node) + arity*sizeof(Node*)));
      return static_cast<void*>((static_cast<FuncIndObj*>(Alloc::allocate(sizeof(FuncIndObj) + sizeof(Node) + arity*sizeof(Node*),&_funcClassDesc))) + 1); 
#else
      return static_cast<void*>((static_cast<FuncIndObj*>(Alloc::allocate(sizeof(FuncIndObj) + sizeof(Node) + arity*sizeof(Node*)))) + 1);
#endif
    }; // void* operator new(size_t,ulong arity)

    void* operator new(size_t) // for constants only
    { 
      CALL("operator new(size_t)");
#ifdef DEBUG_ALLOC_OBJ_TYPE
      ALWAYS(_constClassDesc.registerAllocated(sizeof(ConstIndObj) + sizeof(Node)));
      return static_cast<void*>((static_cast<ConstIndObj*>(Alloc::allocate(sizeof(ConstIndObj) + sizeof(Node),&_constClassDesc))) + 1); 
#else 
      return static_cast<void*>((static_cast<ConstIndObj*>(Alloc::allocate(sizeof(ConstIndObj) + sizeof(Node)))) + 1); 
#endif
    }; // void* operator new(size_t) // for constants only

    void operator delete(void* obj)
	{
      CALL("operator delete(void* obj)");
      ulong arity = (static_cast<Node*>(obj))->func().arity();
      if (arity)
	  {
#ifdef DEBUG_ALLOC_OBJ_TYPE
        ALWAYS(_funcClassDesc.registerDeallocated(sizeof(FuncIndObj) + sizeof(Node) + arity*sizeof(Node*)));
        Alloc::deallocate(static_cast<void*>((static_cast<FuncIndObj*>(obj)) - 1),sizeof(FuncIndObj) + sizeof(Node) + arity*sizeof(Node*),&_funcClassDesc);
#else
        Alloc::deallocate(static_cast<void*>((static_cast<FuncIndObj*>(obj)) - 1),sizeof(FuncIndObj) + sizeof(Node) + arity*sizeof(Node*));
#endif
	  }
      else
	  {
#ifdef DEBUG_ALLOC_OBJ_TYPE
        ALWAYS(_constClassDesc.registerDeallocated(sizeof(ConstIndObj) + sizeof(Node)));
        Alloc::deallocate(static_cast<void*>((static_cast<ConstIndObj*>(obj)) - 1),sizeof(ConstIndObj) + sizeof(Node),&_constClassDesc);
#else
        Alloc::deallocate(static_cast<void*>((static_cast<ConstIndObj*>(obj)) - 1),sizeof(ConstIndObj) + sizeof(Node));
#endif
	  };
	}; // void operator delete(void* obj)


    void operator delete(void* obj,ulong arity)// nonconst functors only
	{
      CALL("operator delete(void* obj,ulong arity)");
      ASSERT(arity);
#ifdef DEBUG_ALLOC_OBJ_TYPE
        ALWAYS(_funcClassDesc.registerDeallocated(sizeof(FuncIndObj) + sizeof(Node) + arity*sizeof(Node*)));
        Alloc::deallocate(static_cast<void*>((static_cast<FuncIndObj*>(obj)) - 1),sizeof(FuncIndObj) + sizeof(Node) + arity*sizeof(Node*),&_funcClassDesc);
#else
        Alloc::deallocate(static_cast<void*>((static_cast<FuncIndObj*>(obj)) - 1),sizeof(FuncIndObj) + sizeof(Node) + arity*sizeof(Node*));
#endif
	}; // void operator delete(void* obj,ulong arity)



    static ulong funcMinSize() { return sizeof(FuncIndObj) + sizeof(Node) + sizeof(Node*); };
    static ulong funcMaxSize() { return sizeof(FuncIndObj) + sizeof(Node) + Symbol::maxArity()*sizeof(Node*); };
    static ulong constMinSize() { return sizeof(ConstIndObj) + sizeof(Node); };
    static ulong constMaxSize() { return sizeof(ConstIndObj) + sizeof(Node); };

    const Symbol& func() const { return _func; }; 
    Node*& altern() { return _altern; };
    const Node* const & altern() const { return _altern; };
    Node** alternAddr() { return &_altern; };
    const Node* const * alternAddr() const { return &_altern; };

    FuncIndObj& indObj() // for nonconstants only
    {
     CALL("Node::indObj()");
     ASSERT(!isConstant());
     return *((static_cast<FuncIndObj*>(static_cast<void*>(this))) - 1);
    };
    ConstIndObj& constIndObj() // for constants only
    {
     CALL("Node::constIndObj()");
     ASSERT(isConstant());
     return *((static_cast<ConstIndObj*>(static_cast<void*>(this))) - 1);
    };    
    const FuncIndObj& indObj() const // for nonconstants only
    {
     CALL("Node::indObj() const");
     ASSERT(!isConstant());
     return *((static_cast<const FuncIndObj*>(static_cast<const void*>(this))) - 1);
    };
    const ConstIndObj& constIndObj() const// for constants only
    {
     CALL("Node::constIndObj() const");
     ASSERT(isConstant());
     return *((static_cast<const ConstIndObj*>(static_cast<const void*>(this))) - 1);
    };
    const FuncIndObj* indObjAddr() const // for nonconstants only
    {
     CALL("Node::indObjAddr() const");
     ASSERT(!isConstant());
     return (static_cast<const FuncIndObj*>(static_cast<const void*>(this))) - 1;
    };

    static Node* node(const FuncIndObj* indObjAddr)
    {
      return static_cast<Node*>(static_cast<void*>(indObjAddr + 1));
    };

    const ConstIndObj* constIndObjAddr() const // for constants only
    {
     CALL("Node::constIndObjAddr() const");
     ASSERT(isConstant());
     return (static_cast<const ConstIndObj*>(static_cast<const void*>(this))) - 1;
    };

    static const Node* node(const ConstIndObj* constIndObjAddr)
    {
      return static_cast<const Node*>(static_cast<void*>(constIndObjAddr + 1));
    };

    Node** deeper() // for nonconst. functors only
    {
     CALL("Node::deeper()");
     ASSERT(isNonconst());
     return static_cast<Node**>(static_cast<void*>(this + 1));
    };   
    const Node* const * deeper() const // for nonconst. functors only
    {
     CALL("Node::deeper() const");
     ASSERT(isNonconst());
     return static_cast<const Node* const *>(static_cast<const void*>(this + 1));
    };
    ulong arity() const { return func().arity(); };
    bool isConstant() const { return !(func().arity()); };
    bool isNonconst() const { return func().arity() != 0UL; };

#ifndef NO_DEBUG
    Node*& debugPreviousClosedPath() { return _debugPreviousClosedPath; };
    const Node* debugPreviousClosedPath() const { return _debugPreviousClosedPath; };
    ulong& debugPreviousArgNum() { return _debugPreviousArgNum; };
    ulong debugPreviousArgNum() const { return _debugPreviousArgNum; };
#endif    

   private:
    Symbol _func;
    Node* _altern;
#ifndef NO_DEBUG
    Node* _debugPreviousClosedPath;
    ulong _debugPreviousArgNum;
#endif    

    #ifdef DEBUG_ALLOC_OBJ_TYPE
     static BK::ClassDesc _funcClassDesc;
     static BK::ClassDesc _constClassDesc;
    #endif 
   }; // class Node

 public:
  class Trie
  {
   public:
    Trie() : _content(0) {};
    ~Trie() 
    {
      CALL("destructor ~Trie()");
      destroy();
    };
    void init() { _content = 0; }; 
    void destroy() 
    { 
      CALL("destroy()");
      if (BK::DestructionMode::isThorough())
	{
	  destroyTree(_content);
	};
      BK_CORRUPT(*this);
    };
    operator bool() const { return (bool)_content; };
   private:
    Node** contentAddr() { return &_content; };   
   private:
    Node* _content;
   friend class Integrator;
   friend class Retrieval;
   friend class Removal;
    //friend class Destruction;
  }; // class Trie

  class Integrator
  {
   public:
    Integrator() :
      _backtrackNodeAddr("PathIndex::Integrator::_backtrackNodeAddr")
#ifndef NO_DEBUG
      
      , _debugBacktrackPrevNode("PathIndex::Integrator::_debugBacktrackPrevNode"),
      _debugBacktrackPrevArgNum("PathIndex::Integrator::_debugBacktrackPrevArgNum")
#endif
    {
      CALL("constructor Integrator()");
    };
    ~Integrator() 
    {
      CALL("destructor ~Integrator()");
    };
    void init()
    {
      CALL("init()");
      _backtrackNodeAddr.init("PathIndex::Integrator::_backtrackNodeAddr");
#ifndef NO_DEBUG
      _debugBacktrackPrevNode.init("PathIndex::Integrator::_debugBacktrackPrevNode");
      _debugBacktrackPrevArgNum.init("PathIndex::Integrator::_debugBacktrackPrevArgNum");
#endif
      
    };
    void destroy()
    {
      CALL("destroy()");
#ifndef NO_DEBUG
      _debugBacktrackPrevArgNum.destroy();
      _debugBacktrackPrevNode.destroy();
#endif
      _backtrackNodeAddr.destroy();
      BK_CORRUPT(*this);
    };
    

    void reset(Trie& tree)
    { 
      _nodeAddr = tree.contentAddr();
      _backtrackNodeAddr.reset();
#ifndef NO_DEBUG
      _debugPreviousNode = 0;
      _debugBacktrackPrevNode.reset();
      _debugBacktrackPrevArgNum.reset();
#endif
    }; // void reset(Trie& tree)


    void nonconstFunc(const Symbol& f)
    {
      CALL("nonconstFunc(const Symbol& f)");
      ulong arity = f.arity();
      ASSERT(arity);

    check_node:
      if (!(*_nodeAddr)) 
	{
	  _lastNode = new (arity) Node(f);
#ifndef NO_DEBUG
	  _lastNode->debugPreviousClosedPath() = _debugPreviousNode;
	  _lastNode->debugPreviousArgNum() = _debugPreviousArgNum;
#endif
	  *_nodeAddr = _lastNode;
	node_found:
	  _nodeAddr = _lastNode->deeper();
	  for (ulong argn = arity - 1; argn; argn--)
	    {
	      _backtrackNodeAddr.push(_lastNode->deeper() + argn);
#ifndef NO_DEBUG
	      _debugBacktrackPrevNode.push(_lastNode);
	      _debugBacktrackPrevArgNum.push(argn);
#endif
	    };
#ifndef NO_DEBUG
	  _debugPreviousNode = _lastNode;
	  _debugPreviousArgNum = 0UL;
#endif
	  return;
	};   
      _lastNode = *_nodeAddr;
      ASSERT(_lastNode->debugPreviousClosedPath() == _debugPreviousNode);
      if (_lastNode->func() == f) goto node_found;
      _nodeAddr = _lastNode->alternAddr();  
      goto check_node;
    }; // void nonconstFunc(const Symbol& f)


    void constant(const Symbol& c)
    {
      CALL("constant(const Symbol& c)");    
      ASSERT(!c.arity());
    check_node:
      if (!(*_nodeAddr)) 
	{
	  _lastNode = new Node(c,true); 
#ifndef NO_DEBUG
	  _lastNode->debugPreviousClosedPath() = _debugPreviousNode;
	  _lastNode->debugPreviousArgNum() = _debugPreviousArgNum;
#endif   
	  *_nodeAddr = _lastNode;
	node_found: 

	  //DF; outputPath(cout,_lastNode) << "\n";

	  backtrack();
	  return;
	};
      _lastNode = *_nodeAddr;
      if (_lastNode->func() == c) goto node_found;
      ASSERT(_lastNode->debugPreviousClosedPath() == _debugPreviousNode);  
      _nodeAddr = _lastNode->alternAddr();  
      goto check_node;
    }; // void constant(const Symbol& c)

    void variable()
    {
      CALL("variable()");
      backtrack();  
    };

    FuncIndObj& indexedObj() const { return _lastNode->indObj(); };
    ConstIndObj& constIndObj() const { return _lastNode->constIndObj(); };

   private:
    void backtrack()
    {
      CALL("backtrack()");
      if (_backtrackNodeAddr) 
	{
	  _nodeAddr = _backtrackNodeAddr.pop();
#ifndef NO_DEBUG
	  _debugPreviousNode = _debugBacktrackPrevNode.pop();
	  _debugPreviousArgNum = _debugBacktrackPrevArgNum.pop();
#endif
	};
    }; // void backtrack()

   private:
    Node** _nodeAddr;
    Node* _lastNode;
    BK::Stack<Node**,MaxTermSize> _backtrackNodeAddr;
#ifndef NO_DEBUG
    Node* _debugPreviousNode;
    ulong _debugPreviousArgNum;
    BK::Stack<Node*,MaxTermSize> _debugBacktrackPrevNode;
    BK::Stack<ulong,MaxTermSize> _debugBacktrackPrevArgNum;
#endif
  }; // class Integrator


 class Retrieval
 {
  public:
   Retrieval() :
     _backtrackNodeAddr("PathIndex::Retrieval::_backtrackNodeAddr")
   {
     CALL("Retrieval::constructor Retrieval()");
   };
   ~Retrieval() 
   {
     CALL("Retrieval::destructor ~Retrieval()");
   };
   void init()
   {
     CALL("Retrieval::init()");
     _backtrackNodeAddr.init("PathIndex::Retrieval::_backtrackNodeAddr");
   };

   void destroy()
   {
     CALL("Retrieval::destroy()");
     _backtrackNodeAddr.destroy();
   };
   
   bool reset(Trie& tree)
   {     
     CALL("Retrieval::reset(Trie& tree)");
     _nodeAddr = tree.contentAddr();
     _backtrackNodeAddr.reset();
     DOP(debugEndOfTerm = false);
     return (*_nodeAddr) != 0;
   }; // bool reset(Trie& tree)
   

   bool nonconstFunc(const Symbol& f)
   {
     CALL("Retrieval::nonconstFunc(const Symbol& f)");
     ASSERT(!debugEndOfTerm);
     ASSERT(f.arity());
   check_node: 
     if (!(*_nodeAddr)) return false;
     _lastNode = *_nodeAddr;  
     if (_lastNode->func() == f)
       {
	 _nodeAddr = _lastNode->deeper();
	 ulong arity = f.arity();
	 for (ulong argn = arity - 1; argn; argn--) _backtrackNodeAddr.push(_lastNode->deeper() + argn); 
	 return true; 
       };
     _nodeAddr = _lastNode->alternAddr();
     goto check_node;
   }; // bool nonconstFunc(const Symbol& f)


   bool constant(const Symbol& c)
   {
     CALL("Retrieval::constant(const Symbol& c)"); 
     ASSERT(!debugEndOfTerm);   
     ASSERT(!c.arity());
   check_node: 
     if (!(*_nodeAddr)) return false;
     _lastNode = *_nodeAddr;  
     if (_lastNode->func() == c)
       {
	 backtrack();
	 return true; 
       };
     _nodeAddr = _lastNode->alternAddr();
     goto check_node;

     
   }; // bool constant(const Symbol& c)

   void after() { CALL("Retrieval::after()"); ASSERT(!debugEndOfTerm); backtrack(); };

   const FuncIndObj& indObj() const { return _lastNode->indObj(); };
   const ConstIndObj& constIndObj() const { return _lastNode->constIndObj(); };  
   const FuncIndObj* indObjAddr() const { return _lastNode->indObjAddr(); };
   const ConstIndObj* constIndObjAddr() const { return _lastNode->constIndObjAddr(); };  

  private:
   void backtrack()
   {
     CALL("Retrieval::backtrack()");
     if (_backtrackNodeAddr) { _nodeAddr = _backtrackNodeAddr.pop(); }
     else 
       {
	 DOP(debugEndOfTerm = true);
       };     
   }; // void backtrack()

  private:
   Node** _nodeAddr;
   Node* _lastNode;
   BK::Stack<Node**,MaxTermSize> _backtrackNodeAddr;
   #ifdef DEBUG_NAMESPACE
    bool debugEndOfTerm;  
   #endif
 }; // class Retrieval

 class Removal
 {
  public:
   Removal() :
     _backtrackNodeAddr("PathIndex::Removal::_backtrackNodeAddr"),
     _candidatesForRecycling("PathIndex::Removal::_candidatesForRecycling")
   {
     CALL("Removal::constructor Removal()");
   };
   ~Removal() 
   {
     CALL("Removal::destructor ~Removal()");
   };
   void init()
   {
     CALL("Removal::init()");
     _backtrackNodeAddr.init("PathIndex::Removal::_backtrackNodeAddr");
     _candidatesForRecycling.init("PathIndex::Removal::_candidatesForRecycling");
   };

   void destroy()
   {
     CALL("Removal::destroy()");
     _candidatesForRecycling.destroy();
     _backtrackNodeAddr.destroy();
   };

   void reset(Trie& tree)
   {     
     CALL("Removal::reset(Trie& tree)");
     _nodeAddr = tree.contentAddr();
     _backtrackNodeAddr.reset();
     _candidatesForRecycling.reset(); 
     DOP(debugEndOfTerm = false);
   }; // void reset(Trie& tree)

   bool nonconstFunc(const Symbol& f)
   {
     CALL("Removal::nonconstFunc(const Symbol& f)");
     ASSERT(!debugEndOfTerm);
     ASSERT(f.arity());
   check_node: 
     if (!(*_nodeAddr)) return false;
     _lastNode = *_nodeAddr;  
     if (_lastNode->func() == f)
       {
	 _candidatesForRecycling.push(_nodeAddr);
	 _nodeAddr = _lastNode->deeper();
	 ulong arity = f.arity();
	 for (ulong argn = arity - 1; argn; argn--) _backtrackNodeAddr.push(_lastNode->deeper() + argn); 
	 return true; 
       };
     _nodeAddr = _lastNode->alternAddr();
     goto check_node;
     
   }; // bool nonconstFunc(const Symbol& f)


   bool constant(const Symbol& c)
   {
     CALL("Removal::constant(const Symbol& c)"); 
     ASSERT(!debugEndOfTerm);   
     ASSERT(!c.arity());
   check_node: 
     if (!(*_nodeAddr)) return false;
     _lastNode = *_nodeAddr;  
     if (_lastNode->func() == c)
       {
	 _candidatesForRecycling.push(_nodeAddr);
	 backtrack();
	 return true; 
       };
     _nodeAddr = _lastNode->alternAddr();
     goto check_node;

   }; // bool constant(const Symbol& c)

   void after() { CALL("Removal::after()"); ASSERT(!debugEndOfTerm); backtrack(); }; 

   FuncIndObj& indexedObj() const 
   { 
    CALL("Removal::indexedObj() const");   
    return _lastNode->indObj(); 
   };
   ConstIndObj& constIndObj() const 
   { 
    CALL("Removal::constIndObj() const"); 
    return _lastNode->constIndObj(); 
   };  

   void recycleObsoleteNodes()
   {
    CALL("Removal::recycleObsoleteNodes()"); 
    while (_candidatesForRecycling.nonempty()) recycleObsolete(*(_candidatesForRecycling.pop()));
   };

  private:
   void backtrack()
   {
     CALL("Removal::backtrack()");

     if (_backtrackNodeAddr) { _nodeAddr = _backtrackNodeAddr.pop(); }
     else 
       {
	 DOP(debugEndOfTerm = true);
       };
   }; // void backtrack()

  private:
   Node** _nodeAddr;
   Node* _lastNode;
   BK::Stack<Node**,MaxTermSize> _backtrackNodeAddr;
   BK::Stack<Node**,MaxTermSize> _candidatesForRecycling;
   #ifdef DEBUG_NAMESPACE
    bool debugEndOfTerm;  
   #endif
 }; // class Removal

  /*******
 class Destruction
 {
  public:
   Destruction() : _backtrackNodeAddr() {};
   ~Destruction() {};
   void destroyThoroughly() {};
   bool reset(Trie& tree);

   void recycle() 
   {
     CALL("Destruction::recycle()");
     destroyTree(*_tree); 
   };

   bool next(bool& constant);

   FuncIndObj& indexedObj() const { return _lastNode->indObj(); };
   ConstIndObj& constIndObj() const { return _lastNode->constIndObj(); };
  private:
   Node** _tree;
   Node** _nodeAddr;
   Node* _lastNode;
   BK::Stack<Node**,MaxTermSize + MaxTermDepth> _backtrackNodeAddr;
 }; // class Destruction

  *****/

 public: // for class PathIndex
#ifndef NO_DEBUG_VIS
  static ostream& outputPath(ostream& str,const Node* nd);
#endif
 private: // for class PathIndex
  // Releasing memory  
  static void destroyTree(Node*& tree);
  static void recycleObsolete(Node*& node);
 
  friend class Removal;
  friend class Trie;
  // friend class Destruction;
}; // template <class Alloc,class Symbol,class FuncIndObj,class ConstIndObj,ulong MaxTermDepth> class PathIndex

}; // namespace Gem


//============================================================================
#undef DEBUG_NAMESPACE
#ifdef DEBUG_PATH_INDEX
 #define DEBUG_NAMESPACE "PathIndex<Alloc,Symbol,FuncIndObj,ConstIndObj,MaxTermDepth,MaxTermSize>"
#endif
#include "debugMacros.hpp"
//============================================================================

namespace Gem
{

template <class Alloc,class Symbol,class FuncIndObj,class ConstIndObj,ulong MaxTermDepth,ulong MaxTermSize>
inline void PathIndex<Alloc,Symbol,FuncIndObj,ConstIndObj,MaxTermDepth,MaxTermSize>::destroyTree(Node*& tree)
{   
 CALL("destroyTree(Node*& tree)");


 if (tree)
  {

   destroyTree(tree->altern());   
   if (tree->isNonconst())
    {             
     for (ulong a = 0; a < tree->arity(); a++) 
      {
       destroyTree(tree->deeper()[a]);
      };
    };

   //DF; cout << "%%%%%% destr tree " << (long)tree << " in " << (long)(&tree) << "\n";

   delete tree;
   tree = 0;
  };
}; // void PathIndex<..>::destroyTree(Node*& tree)


template <class Alloc,class Symbol,class FuncIndObj,class ConstIndObj,ulong MaxTermDepth,ulong MaxTermSize>
inline void PathIndex<Alloc,Symbol,FuncIndObj,ConstIndObj,MaxTermDepth,MaxTermSize>::recycleObsolete(Node*& node)
{
 CALL("recycleObsolete(Node*& node)");
 Node* tmp;
 if (node) 
  {
   if (node->isConstant()) 
    {
     if (!node->constIndObj()) // obsolete node
      {
       tmp = node;       
       node = tmp->altern();
       delete tmp;
      };
    }
   else // !node->isConstant()
    { 
     if (!node->indObj())
      {
       ulong arity = node->arity();
       for (ulong argn = 0; argn < arity; argn++) 
	if (node->deeper()[argn]) return;  // not obsolete
       // obsolete node
       tmp = node;       
       node = tmp->altern();
       delete tmp; 
      };
    };
  };
}; // void PathIndex<..>::recycleObsolete(Node*& node) 

#ifndef NO_DEBUG_VIS
template <class Alloc,class Symbol,class FuncIndObj,class ConstIndObj,ulong MaxTermDepth,ulong MaxTermSize>
inline 
ostream& PathIndex<Alloc,Symbol,FuncIndObj,ConstIndObj,MaxTermDepth,MaxTermSize>::outputPath(ostream& str,const Node* nd)
{
  if (!nd) return str;
  if (nd->debugPreviousClosedPath())
    {
      outputPath(str,nd->debugPreviousClosedPath());
      str << "." << nd->debugPreviousArgNum() << ".";
    }
  str << nd->func();
  return str;
}; // ostream& PathIndex<..>::outputPath(ostream& str,const Node* nd)
#endif

}; // namespace Gem


/*******************

//============================================================================
#undef DEBUG_NAMESPACE
#ifdef DEBUG_PATH_INDEX
 #define DEBUG_NAMESPACE "PathIndex<Alloc,Symbol,FuncIndObj,ConstIndObj,MaxTermDepth,MaxTermSize>::Destruction"
#endif
#include "debugMacros.hpp"
//============================================================================

namespace Gem
{

template <class Alloc,class Symbol,class FuncIndObj,class ConstIndObj,ulong MaxTermDepth,ulong MaxTermSize>
inline bool PathIndex<Alloc,Symbol,FuncIndObj,ConstIndObj,MaxTermDepth,MaxTermSize>::Destruction::reset(Trie& tree) 
{ 
 CALL("reset(Trie& tree,bool& constant)");
 _nodeAddr = tree.contentAddr();
 _tree = _nodeAddr;
 return (bool)(*_nodeAddr);
}; // bool PathIndex<..>::Destruction::reset(Trie& tree) 

template <class Alloc,class Symbol,class FuncIndObj,class ConstIndObj,ulong MaxTermDepth,ulong MaxTermSize>
inline bool PathIndex<Alloc,Symbol,FuncIndObj,ConstIndObj,MaxTermDepth,MaxTermSize>::Destruction::next(bool& constant)
{
 CALL("next(bool& constant)"); 
 Node** deeper;
 ulong arity;
 _lastNode = *_nodeAddr;
 if (_lastNode)
  {
   process_node:
    constant = _lastNode->isConstant();
    _nodeAddr = _lastNode->alternAddr();
    if (!constant)
     {
      if (*_nodeAddr) _backtrackNodeAddr.push(_nodeAddr);
      arity = _lastNode->arity();
      deeper = _lastNode->deeper();
      for (ulong argn = arity - 1; argn; argn--) 
       {
        _nodeAddr = deeper + argn;
        if (*_nodeAddr) _backtrackNodeAddr.push(_nodeAddr); 
       };
      _nodeAddr = deeper;
     };
    return true;
  }
 else // !_lastNode
  {
   if (_backtrackNodeAddr)
    {
     _nodeAddr = _backtrackNodeAddr.pop();
     _lastNode = *_nodeAddr;
     ASSERT(_lastNode);
     goto process_node;
    }
   else return false;
  };
}; // bool PathIndex<..>::Destruction::next(bool& constant) 


}; // namespace Gem

****************/

//======================================================================
#endif
