//
// File:         GSortTree.hpp
// Description:  Simple generic binary sorting trees.
// Created:      Feb 13, 2000, 16:30
// Author:       Alexandre Riazanov
// mail:         riazanov@cs.man.ac.uk
//============================================================================
#ifndef G_SORT_TREE_H
//=============================================================================
#define G_SORT_TREE_H 
#include "jargon.hpp"
#include "BlodKorvDebugFlags.hpp"
#include "GList.hpp"
#ifdef DEBUG_ALLOC_OBJ_TYPE
#  include "ClassDesc.hpp"
#endif
#include "DestructionMode.hpp"
//============================================================================
#undef DEBUG_NAMESPACE
#ifdef DEBUG_G_SORT_TREE
 #define DEBUG_NAMESPACE "GSortTree<class Alloc,class ValType,class InstanceId>"
#endif
#include "debugMacros.hpp"
//============================================================================
namespace BK 
{
template <class Alloc,class ValType,class InstanceId>
class GSortTree
{
 public:
  class Node
  {
   public: 
    Node(const ValType& v) : val(v), count(1), left(0), right(0) 
    {
     CALL("constructor Node(const ValType& v)");
    };
    Node(const ValType& v,Node* l,Node* r) : val(v), count(1), left(l), right(r) 
    {
     CALL("constructor Node(const ValType& v,Node* l,Node* r)");
    };
    ~Node() {};
    void* operator new(size_t) 
    { 
     CALL("operator new(size_t)");
     #ifdef DEBUG_ALLOC_OBJ_TYPE
      ALWAYS(_classDesc.registerAllocated(sizeof(Node)));
      return Alloc::allocate(sizeof(Node),&_classDesc);  
     #else
      return Alloc::allocate(sizeof(Node));   
     #endif
    }; 
    void  operator delete(void* obj) 
    { 
     CALL("operator delete(void* obj)");
     #ifdef DEBUG_ALLOC_OBJ_TYPE
      ALWAYS(_classDesc.registerDeallocated(sizeof(Node)));
      Alloc::deallocate(obj,sizeof(Node),&_classDesc); 
     #else
      Alloc::deallocate(obj,sizeof(Node)); 
     #endif
    };
    static ulong minSize() { return sizeof(Node); };
    static ulong maxSize() { return sizeof(Node); };
    const ValType& value() const { return val; };
    ValType& value() { return val; };
    const unsigned long& score() const { return count; };
    unsigned long& score() { return count; };
    Node* const & less() const { return left; }; 
    Node*& less() { return left; }; 
    Node* const & greater() const { return right; }; 
    Node*& greater() { return right; };
   private:
    ValType val;
    unsigned long count;
    Node* left;
    Node* right;    
    #ifdef DEBUG_ALLOC_OBJ_TYPE
     static ClassDesc _classDesc;
    #endif
  }; // class Node

  class LeftRightIter
  {
  public: 
    // can not make it private because it has static data member(s)
	class Id {};	
    typedef GList<Alloc,const Node*,Id> NodeList;
   public:
    LeftRightIter() : currNode(0), stack(0) {};
    LeftRightIter(const GSortTree& tree) : stack(0)
    {
     currNode = tree.root;
     if (!currNode) return;
     while (currNode->less())
      {
       stack = new NodeList(currNode,stack);
       currNode = currNode->less();
      };
    }; 
    ~LeftRightIter() 
      { 
	destroy();
      };
    void init()
    {      
      currNode = 0;
      stack = 0;
    };

    void destroy()
    {
      CALL("destroy()");
      if (DestructionMode::isThorough()) 
	NodeList::destroyList(stack);
      BK_CORRUPT(*this);
    };


    void reset(const GSortTree& tree)
    {
     NodeList::destroyList(stack);
     stack = 0; 
     currNode = tree.root;
     if (!currNode) return;
     while (currNode->less())
      {
       stack = new NodeList(currNode,stack);
       currNode = currNode->less();
      };
    };
    operator bool() const { return currNode != 0; };
    void next()
    {
     if (currNode->greater())     
      {
       currNode = currNode->greater();       
       while (currNode->less())
        {
         stack = new NodeList(currNode,stack);
         currNode = currNode->less();
        };
      }
     else // try to pop the stack
      if (stack)
       {
	currNode = stack->hd();
        NodeList* tmp = stack;
        stack = stack->tl();
        delete tmp;        
       }	
      else currNode = 0;             
    };
    const ValType& value() const { return currNode->value(); };
    const unsigned long& score() const { return currNode->score(); };
   private:
    const Node* currNode;   
    NodeList* stack;
  }; // class LeftRightIter

  class RightLeftIter
  {
   public:
    // can not make it private because it has static data member(s)
	   class Id {};
    typedef GList<Alloc,const Node*,Id> NodeList;
   public:
    RightLeftIter() : currNode(0), stack(0) {};
    RightLeftIter(const GSortTree& tree) : stack(0)
    {
     reset(tree);
    }; 
    ~RightLeftIter() 
      { 
	destroy();
      };

    void init()
    {
      currNode = 0;
      stack = 0;
    };
    
    void destroy()
    {
      CALL("destroy()");
      if (DestructionMode::isThorough())
	NodeList::destroyList(stack);
      BK_CORRUPT(*this);
    };
   

    void reset(const GSortTree& tree)
    {
     NodeList::destroyList(stack);
     stack = 0; 
     currNode = tree.root;
     if (!currNode) return;
     while (currNode->greater())
      {
       stack = new NodeList(currNode,stack);
       currNode = currNode->greater();
      };
    };
    operator bool() const { return currNode != 0; };
    void next()
    {
     if (currNode->less())     
      {
       currNode = currNode->less();       
       while (currNode->greater())
        {
         stack = new NodeList(currNode,stack);
         currNode = currNode->greater();
        };
      }
     else // try to pop the stack
      if (stack)
       {
	currNode = stack->hd();
        NodeList* tmp = stack;
        stack = stack->tl();
        delete tmp;        
       }	
      else currNode = 0;             
    };
    const ValType& value() const { return currNode->value(); };
    const unsigned long& score() const { return currNode->score(); };
   private:
    const Node* currNode;   
    NodeList* stack;
  }; // class RightLeftIter

 public:
  GSortTree() : root(0) {};
  ~GSortTree() 
    { 
      CALL("destructor ~GSortTree()"); 
      destroy();
    };
  
  void init()
  {
    root = 0;    
  };

  void destroy() 
  {
    CALL("destroy()");
    if (DestructionMode::isThorough())
      destroyTree(root);
    BK_CORRUPT(*this);
  };
    

  void insert(const ValType& v)
  {
   Node** n = find(v,&root);
   if (*n) { ((*n)->score())++; }
   else *n = new Node(v);        
  };
  void insert(const ValType& v,unsigned long& oldScore)
  {
   Node** n = find(v,&root);
   if (*n) { oldScore = (*n)->score(); (*n)->score()++; }
   else { *n = new Node(v); oldScore = 0; };        
  };
  unsigned long score(const ValType& v) const
  {
   Node** n = find(v,&root);
   if (*n) return (*n)->score();
   return 0;
  };
 private:
  static Node** find(const ValType& v,Node** rt)
  {
   Node* r = *rt;
   if ((!r) || (r->value() == v)) return rt;
   if (r->value() > v) return find(v,&(r->less())); 
   return find(v,&(r->greater()));  
  };
  static void destroyTree(Node* rt) 
   {
    if (rt)
     {
      destroyTree(rt->less());
      destroyTree(rt->greater());
      delete rt; 
     }; 
   }; 
 private: 
  Node* root;
 friend class LeftRightIter;
 friend class RightLeftIter;
}; // template <class Alloc,class ValType,class InstanceId> class GSortTree

}; // namespace BK

//======================================================================
#endif
