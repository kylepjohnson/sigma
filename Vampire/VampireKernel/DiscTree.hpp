//
// File:         DiscTree.hpp
// Description:  The simplest possible implementation of discrimination trees.
// Created:      Jan 11, 2001, 13:00
// Author:       Alexandre Riazanov
// mail:         riazanov@cs.man.ac.uk
//====================================================
#ifndef DISC_TREE_H
//=================================================
#define DISC_TREE_H
#include <cstddef>
#include <iostream>
#include "jargon.hpp"
#include "VampireKernelDebugFlags.hpp"
#include "VampireKernelConst.hpp"
#ifdef DEBUG_ALLOC_OBJ_TYPE 
 #include "ClassDesc.hpp"
#endif
#include "Term.hpp"
//=================================================

namespace VK
{
template <class Alloc,class LeafInfo>
class DiscTree
{
 public:
  class Node; 
  class Integrator; 
}; // template <class Alloc,class LeafInfo> class DiscTree

}; // namespace VK

//================================================
#undef DEBUG_NAMESPACE
#ifdef DEBUG_CLAUSE
 #define DEBUG_NAMESPACE "DiscTree<Alloc,LeafInfo>::Node"
#endif 
#include "debugMacros.hpp"
//=================================================        

namespace VK
{
template <class Alloc,class LeafInfo>
class DiscTree<Alloc,LeafInfo>::Node
{
 public:
  static Node* newNonleafNode(const TERM& t)
  {
   CALL("newNonleafNode(const TERM& t)");
   ASSERT(!t.IsReference());
   #ifdef DEBUG_ALLOC_OBJ_TYPE
    return new (sizeof(NonleafNode),&_nonleafNodeClassDesc) Node(t);  
   #else
    return new (sizeof(NonleafNode)) Node(t);  
   #endif 
  };
  static Node* newNonleafNode(const TERM& t,Node* nxt,Node* frk)
  {
   CALL("newNonleafNode(const TERM& t,Node* nxt,Node* frk)");
   ASSERT(!t.IsReference());
   #ifdef DEBUG_ALLOC_OBJ_TYPE
    return new (sizeof(NonleafNode),&_nonleafNodeClassDesc) Node(t,nxt,frk); 
   #else
    return new (sizeof(NonleafNode)) Node(t,nxt,frk); 
   #endif  
  };
  static Node* newLeaf(TERM* t) 
  {
   CALL("newLeaf(TERM* t)");
   #ifdef DEBUG_ALLOC_OBJ_TYPE
    return new (sizeof(Leaf),&_leafClassDesc) Node(t);
   #else
    return new (sizeof(Leaf)) Node(t);  
   #endif
  };
  void operator delete(void* obj) 
  {
   CALL("operator delete(void* obj)");
   ASSERT((static_cast<Node*>(obj))->checkObj());
   #ifdef DEBUG_ALLOC_OBJ_TYPE
    if ((static_cast<Node*>(obj))->tag().IsReference()) 
     {
      Alloc::deallocate(obj,sizeof(Leaf),_leafClassDesc);
     }
    else Alloc::deallocate(obj,sizeof(NonleafNode),_nonleafNodeClassDesc);    
   #else
    if ((static_cast<Node*>(obj))->tag().IsReference()) 
     {
      Alloc::deallocate(obj,sizeof(Leaf));
     }
    else Alloc::deallocate(obj,sizeof(NonleafNode));
   #endif
  };
  const TERM& tag() const 
  {
   CALL("const TERM& tag() const"); 
   ASSERT(checkObj());
   return _tag; 
  }; 
  bool isLeaf() const 
  {
   CALL("tag() const"); 
   ASSERT(checkObj());
   return _tag.IsReference(); 
  };
  const LeafInfo* leafInfoAddr() const
  {
   CALL("leafInfoAddr() const");
   ASSERT(checkObj());
   ASSERT(_tag.IsReference());
   return 
     static_cast<const LeafInfo*>(static_cast<const void*>((static_cast<const TagOnly*>(static_cast<const void*>(this))) + 1));
  };  
  
  LeafInfo* leafInfoAddr()
  {
   CALL("leafInfoAddr()");
   ASSERT(checkObj());
   ASSERT(_tag.IsReference());
   return 
     static_cast<LeafInfo*>(static_cast<void*>((static_cast<TagOnly*>(static_cast<void*>(this))) + 1));
  };  

  const LeafInfo& leafInfo() const 
  {
   CALL("leafInfo() const");
   ASSERT(checkObj());
   ASSERT(_tag.IsReference());
   return *(leafInfoAddr());
  };
  LeafInfo& leafInfo() 
  {
   CALL("leafInfo()"); 
   ASSERT(checkObj());
   ASSERT(_tag.IsReference());   
   return *(leafInfoAddr()); 
  };
  const Node* const * nextAddr() const
  {
   CALL("nextAddr() const");
   ASSERT(checkObj());
   ASSERT(!_tag.IsReference());
   return 
     static_cast<const Node* const *>(static_cast<const void*>((static_cast<const TagOnly*>(static_cast<const void*>(this))) + 1));
  };
  Node** nextAddr()
  {
   CALL("nextAddr()");
   ASSERT(checkObj());
   ASSERT(!_tag.IsReference());
   return 
     static_cast<Node**>(static_cast<void*>((static_cast<TagOnly*>(static_cast<void*>(this))) + 1));
  };  
  const Node* const & next() const 
  {
   CALL("next() const");  
   return *(nextAddr());
  };
  Node*& next() 
  {
   CALL("next()"); 
   return *(nextAddr()); 
  };
  
  const Node* const * forkAddr() const
  {
   CALL("forkAddr() const");
   ASSERT(checkObj());
   ASSERT(!_tag.IsReference());
   return 
     static_cast<const Node* const *>(static_cast<const void*>((static_cast<const TagAndNext*>(static_cast<const void*>(this))) + 1));
  }; 

  Node** forkAddr()
  {
   CALL("forkAddr()");
   ASSERT(checkObj());
   ASSERT(!_tag.IsReference());
   return 
     static_cast<Node**>(static_cast<void*>((static_cast<TagAndNext*>(static_cast<void*>(this))) + 1));
  }; 

  const Node* const & fork() const 
  {
   CALL("fork() const");
   ASSERT(checkObj());
   ASSERT(!_tag.IsReference());    
   return *(forkAddr());
  };
  Node*& fork() 
  {
   CALL("fork()");
   ASSERT(checkObj());
   ASSERT(!_tag.IsReference());    
   return *(forkAddr()); 
  };

  #ifndef NO_DEBUG_VIS
   ostream& output(ostream& str) const
   {
    if (isLeaf()) { return str << "Leaf"; }
    else 
     {
      str << '[' << (ulong)this << "] "
          << tag() << "  n[" << (ulong)next() << "] " 
          << "f[" << (ulong)fork() << ']';
      return str; 
     };
   };
  #endif

  #ifndef NO_DEBUG   
   bool checkObj() const 
   {
    #ifndef DEBUG_ALLOC
     return true; 
    #else
     ObjDesc* objDesc = GlobAlloc::hostObj(static_const<const void*>(this));
     if (!objDesc) 
      { 
       DMSG("DiscTree<Alloc,LeafInfo>::Node::checkObj()> Null pointer to object descriptor.\n");  
       return false; 
      };      
     bool res = true;  
     #ifdef DEBUG_ALLOC_OBJ_SIZE
      if (_tag.IsReference()) // must be a leaf
       {
        if (objDesc->size !=  sizeof(Leaf)) 
         {
          DMSG("DiscTree<Alloc,LeafInfo>::Node::checkObj()> objDesc->size !=  sizeof(Leaf)\n");
          res = false;
         };
       }
      else 
       {
        if (objDesc->size !=  sizeof(NonleafNode)) 
         {
          DMSG("DiscTree<Alloc,LeafInfo>::Node::checkObj()> objDesc->size !=  sizeof(NonleafNode)\n");
          res = false;
         };
       };
     #endif
     #ifdef DEBUG_ALLOC_OBJ_STATUS
      if (objDesc->status != ObjDesc::Persistent) 
       {
        DMSG("DiscTree<Alloc,LeafInfo>::Node::checkObj()> objDesc->status != ObjDesc::Persistent\n");
        res = false;
       };
     #endif
     #ifdef DEBUG_ALLOC_OBJ_TYPE
      if (_tag.IsReference()) // must be a leaf
       {
        if (objDesc->classDesc != &_leafClassDesc) 
         {
          DMSG("DiscTree<Alloc,LeafInfo>::Node::checkObj()> objDesc->classDesc != &_leafClassDesc\n");
          res = false;
         };
       }
      else
       {
        if (objDesc->classDesc != &_nonleafNodeClassDesc) 
         {
          DMSG("DiscTree<Alloc,LeafInfo>::Node::checkObj()> objDesc->classDesc != &_nonleafNodeClassDesc\n");
          res = false;
         };        
       };
     #endif
     if (!res) DOP(DMSG("DiscTree<Alloc,LeafInfo>::Node::checkObj()> host object ") << *objDesc << "\n"); 
     return res;
    #endif
   };
  #endif

 private:
  class TagOnly
  {
   private: 
    TERM _dummyTag;
  };
  class TagAndNext
  {
   TERM _dummyTag;
   Node* _dummyNext;
  };
  class NonleafNode // only to fix size
  {
   private:
    TERM _dummyTag;
    Node* _dummyNext;
    Node* _dummyFork;  
  }; 
  class Leaf  // only to fix size
  {
   private:
    TERM _dummyTag;
    LeafInfo _dummyInfo;
  };  
 private:
  Node(const TERM& t) : _tag(t) 
  {
   CALL("constructor Node(const TERM& t)");
   next() = 0;
   fork() = 0;
  };
  Node(const TERM& t,Node* nxt,Node* frk) : _tag(t) 
  {
   CALL("constructor Node(const TERM& t,Node* nxt,Node* frk)");
   next() = nxt;
   fork() = frk;
  };
  Node(TERM* t) : _tag(t) 
  {
   CALL("Node(TERM* t)"); 
  };  

  ~Node() {};
  #ifdef DEBUG_ALLOC_OBJ_TYPE
   void* operator new(size_t,size_t size,ClassDesc* clDesc)
   {
    CALL("operator new(size_t,size_t size,ClassDesc* clDesc)");
    return Alloc::allocate(size,clDesc);
   }; 
  #else
   void* operator new(size_t,size_t size)
   {
    CALL("void* operator new(size_t,size_t size)");
    return Alloc::allocate(size);
   }; 
  #endif
 private:
  TERM _tag;
  #ifdef DEBUG_ALLOC_OBJ_TYPE
   static ClassDesc _nonleafNodeClassDesc;
   static ClassDesc _leafClassDesc;
  #endif 
}; // template <class Alloc,class LeafInfo> class DiscTree<Alloc,LeafInfo>::Node

}; // namespace VK

//================================================
#undef DEBUG_NAMESPACE
#ifdef DEBUG_CLAUSE
 #define DEBUG_NAMESPACE "DiscTree<Alloc,LeafInfo>::Integrator"
#endif 
#include "debugMacros.hpp"
//=================================================    

namespace VK
{
template <class Alloc,class LeafInfo>
class DiscTree<Alloc,LeafInfo>::Integrator
{
 public:
  typedef DiscTree<Alloc,LeafInfo> DiscTree;
  typedef DiscTree::Node Node;
 public: 
  Integrator(){}; 
  ~Integrator(){}; 
  void reset(Node** n) 
  {
   CALL("reset(Node** n)");
   ASSERT(n);
   _currPos = n;
  };
  void push(const TERM& sym)
  {
   CALL("push(const TERM& sym)");
   Node* currNode;
   while (((currNode = *_currPos)) != 0)    
    {
     ASSERT(!currNode->isLeaf());
     if (sym < currNode->tag())
      {       
       _currPos = currNode->forkAddr();        
      }
     else 
      if (sym == currNode->tag())
       {
        _currPos = currNode->nextAddr(); 
	return;
       } 
      else // sym > currNode->tag()
       {
	*_currPos = Node::newNonleafNode(sym,static_cast<Node*>(0),currNode);  
        _currPos = (*_currPos)->nextAddr(); 
	return;      
       };
    };
   // here currNode == *_currPos == 0
   *_currPos = Node::newNonleafNode(sym);
   _currPos = (*_currPos)->nextAddr();
  };
  Node* getLeaf(bool& newLeaf)
  { 
   CALL("getLeaf(bool& newLeaf)");
   if ((newLeaf = ((*_currPos) == 0)))
    {
     *_currPos = Node::newLeaf(static_cast<TERM*>(0));
     ASSERT_IN((*_currPos) != 0,"0");
    };
   ASSERT_IN((*_currPos) != 0,"1");
   ASSERT((*_currPos)->isLeaf());
   return *_currPos;
  };
 private:  
  Node** _currPos;  
}; //  template <class Alloc,class LeafInfo> class DiscTree<Alloc,LeafInfo>::Integrator

}; // namespace VK

//==================================================
#endif
