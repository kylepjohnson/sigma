//
// File:         OrdList.hpp
// Description:  Generic ordered association lists.
// Created:      Feb 25, 2000
// Author:       Alexandre Riazanov
// mail:         riazanov@cs.man.ac.uk
// Revised:      Feb 02, 2002, Alexandre Riazanov, riazanov@cs.man.ac.uk
//               Parameter "class InstanceId" added to the templates.
//============================================================================
#ifndef ORD_LIST_H
//=============================================================================
#define ORD_LIST_H
#include "jargon.hpp"
#include "BlodKorvDebugFlags.hpp"
#include "DestructionMode.hpp"
//============================================================================
#undef DEBUG_NAMESPACE
#ifdef DEBUG_ORD_LIST
#define DEBUG_NAMESPACE "OrdList<class Alloc,class ValType,class KeyType,class InstanceId>"
#endif
#include "debugMacros.hpp"
//============================================================================
namespace BK 
{

template <class Alloc,class ValType,class KeyType,class InstanceId>
class OrdList
{
 public:
  class Surfer;
  class Destruction;
  class Node
    {
    public:
      Node(const KeyType& k) : _next(0), _key(k){};     
      Node(const KeyType& k,Node* nxt) : _next(nxt), _key(k) {};    
      Node(const KeyType& k,const ValType& val,Node* nxt) : _value(val), _next(nxt), _key(k) {};
      ~Node() {};
      void* operator new(size_t) 
	{ 
#ifdef DEBUG_ALLOC_OBJ_TYPE
	  ALWAYS(_classDesc.registerAllocated(sizeof(Node)));
	  return Alloc::allocate(sizeof(Node),&_classDesc); 
#else 
	  return Alloc::allocate(sizeof(Node)); 
#endif
	};
      void operator delete(void* obj) 
	{
#ifdef DEBUG_ALLOC_OBJ_TYPE
	  ALWAYS(_classDesc.registerDeallocated(sizeof(Node)));
	  Alloc::deallocate(obj,sizeof(Node),&_classDesc); 
#else
	  Alloc::deallocate(obj,sizeof(Node)); 
#endif
	};
      Node*& next() { return _next; };
      const Node * const & next() const { return _next; }; 
      static ulong minSize() { return sizeof(Node); }; 
      static ulong maxSize() { return sizeof(Node); };
      const ValType& value() const { return _value; };
      ValType& value() { return _value; };
      const KeyType& key() const { return _key; };
      KeyType& key() { return _key; };
    private:
      Node() {};
    private:
      ValType _value;
      Node* _next;
      KeyType _key;  
#ifdef DEBUG_ALLOC_OBJ_TYPE
      static ClassDesc _classDesc;
#endif   
    }; // class Node

  class Surfer
    {
    public:
      Surfer() {};
      Surfer(const OrdList& lst) { reset(lst); };    
      ~Surfer() {};
      void reset(const OrdList& lst)
	{
	  _currNode = lst.first();
	};
      const Node* currNode() const { return _currNode; };
      const Node* const * currNodeAddr() const { return &_currNode; }; 
      bool next()
	{
	  CALL("next()"); 
	  _currNode = _currNode->next();
	  return (bool)_currNode;
	}; // bool next()

      bool find(const KeyType& searchKey) { return find(searchKey,_currNode); };
    
      void getTo(const KeyType& searchKey) // we are sure that searchKey is in the list 
	{     
	  CALL("getTo(const KeyType& searchKey)");
	  while (_currNode->key() > searchKey) 
	    {        
	      _currNode = _currNode->next();
	    };
	  ASSERT(_currNode->key() == searchKey);  
	};

      bool find(const KeyType& searchKey,const Node*& node)
	{     
	check_node:
	  if (_currNode)
	    {     
	      if (_currNode->key() > searchKey) { _currNode = _currNode->next(); goto check_node; };
	      // _currNode->key() <= searchKey      
	      if (_currNode->key() == searchKey) { node = _currNode; return true; };
	      // _currNode->key() < searchKey  
	      node = _currNode;
	      return false;
	    };
	  node = 0;
	  return false;
	};
    private:
      const Node* _currNode;
    }; // class Surfer

  class Destruction
    {
    public:
      Destruction() {};
      Destruction(OrdList& ol) { reset(ol); }; 
      ~Destruction() {};
      void reset(OrdList& ol) 
	{ 
	  _node = ol.first();
	  ol._size = 0;
	  ol._list = 0;
	};
      bool next(ValType& val) 
	{
	  if (!_node) return false;
	  val = _node->value();
	  Node* tmp = _node;
	  _node = _node->next();
	  delete tmp;
	  return true;
	};
    private: 
      Node* _node;
    };

 public:
  OrdList() { init(); };  
  ~OrdList() 
    { 
      CALL("destructor ~OrdList()");  
      destroy(); 
    };
  void init() 
    {
      CALL("init()");
      _list = 0;
      _size = 0; 
    };
  void destroy();

  void* operator new(size_t) 
    {
#ifdef DEBUG_ALLOC_OBJ_TYPE 
      ALWAYS(_classDesc.registerAllocated(sizeof(OrdList)));
      return Alloc::allocate(sizeof(OrdList),&_classDesc); 
#else
      return Alloc::allocate(sizeof(OrdList)); 
#endif
    };
  void operator delete(void* obj) 
    { 
#ifdef DEBUG_ALLOC_OBJ_TYPE
      ALWAYS(_classDesc.registerDeallocated(sizeof(OrdList)));
      Alloc::deallocate(obj,sizeof(OrdList),&_classDesc); 
#else
      Alloc::deallocate(obj,sizeof(OrdList)); 
#endif
    };
  static ulong minSize() { return sizeof(OrdList); }; 
  static ulong maxSize() { return sizeof(OrdList); };
  void reset() 
    {
      destroy();
      init();
    };
  operator bool() const { return (bool)_list; }; 
  const Node* first() const { return const_cast<const Node*>(_list); };
  Node* first() { return _list; };
  unsigned long size() const { return _size; }; 
  unsigned long count() const 
    {
      unsigned long res = 0; 
      for (const Node* n = first(); n; n = n->next()) res++;
      return res;
    };
  bool contains(const KeyType& searchKey) const
    {
      Surfer surfer(*this);
      const Node* dummy;
      return surfer.find(searchKey,dummy);
    }; // bool contains (const KeyType& searchKey) const

  Node* insert(const KeyType& searchKey,bool& newNode); 
  bool remove(const KeyType& searchKey,ValType& val);
#ifndef NO_DEBUG
  bool checkIntegrity() const { return (count() == size()); };
#endif
 private:
  OrdList(const OrdList&)
    {
      CALL("constructor OrdList(const OrdList&)");
      ICP("ICP0");
    };
  OrdList& operator=(const OrdList&)
  {
    CALL("operator=(const OrdList&)");
    ICP("ICP0");
    return *this;
  };
 private:
  Node* _list;
  unsigned long _size; 
#ifdef DEBUG_ALLOC_OBJ_TYPE
  static ClassDesc _classDesc;
#endif
  friend class Surfer;
  friend class Destruction;   
}; // template <class Alloc,class ValType,class KeyType,class InstanceId> class OrdList

template <class Alloc,class ValType,class KeyType,class InstanceId>
OrdList<Alloc,ValType,KeyType,InstanceId>::Node* OrdList<Alloc,ValType,KeyType,InstanceId>::insert(const KeyType& searchKey,bool& newNode)
{
  CALL("insert(const KeyType& searchKey,bool& newNode)");
  Node** addr = &_list;
  Node* node = _list;
  KeyType nodeKey;
 check_node:
  if (node)
    {
      nodeKey = node->key();       
      if (nodeKey > searchKey)
	{
	  addr = &(node->next()); 
	  node = node->next();
	  goto check_node;
	};
      // nodeKey <= searchKey
      if (nodeKey == searchKey) { newNode = false; return node; };
    };
  ASSERT(!node || (node->key() < searchKey));
  // create new node

  Node* res = new Node(searchKey,node);
  *addr = res;
  newNode = true;
  _size++;
  return res; 
}; // OrdList<Alloc,ValType,KeyType,InstanceId>::Node* OrdList<Alloc,ValType,KeyType,InstanceId>::insert(const KeyType& searchKey,bool& newNode)
 
template <class Alloc,class ValType,class KeyType,class InstanceId>
bool OrdList<Alloc,ValType,KeyType,InstanceId>::remove(const KeyType& searchKey,ValType& val)
{
  CALL("remove(const KeyType& searchKey,ValType& val)");
  Node** addr = &_list;
  Node* node = _list;
  KeyType nodeKey;
 check_node:
  if (node)
    {
      nodeKey = node->key();       
      if (nodeKey > searchKey)
	{
	  addr = &(node->next()); 
	  node = node->next();
	  goto check_node;
	};
      // nodeKey <= searchKey
      if (nodeKey == searchKey) 
	{
	  val = node->value(); 
	  *addr = node->next();
	  delete node;
	  _size--;
	  return true; 
	};    
    };
  ASSERT(!node || (node->key() < searchKey));
  return false;
}; // bool OrdList<Alloc,ValType,KeyType,InstanceId>::remove(const KeyType& searchKey,ValType& val) 

template <class Alloc,class ValType,class KeyType,class InstanceId>
void OrdList<Alloc,ValType,KeyType,InstanceId>::destroy()
{
  CALL("destroy()");
  if (DestructionMode::isThorough())
    {
      Node* tmp;
      while (_list) 
	{
	  tmp = _list;
	  _list = _list->next();
	  delete tmp; 
	};
    }; 
  BK_CORRUPT(*this);
}; // void OrdList<Alloc,ValType,KeyType,InstanceId>::destroy()

}; // namespace BK


//============================================================================
#undef DEBUG_NAMESPACE
#ifdef DEBUG_ORD_LIST
#define DEBUG_NAMESPACE "VoidOrdList<class Alloc,class KeyType,class InstanceId>"
#endif
#include "debugMacros.hpp"
//============================================================================

namespace BK 
{

template <class Alloc,class KeyType,class InstanceId>
class VoidOrdList
{
 public:
  class Surfer;
  class Node
    {
    public:
      Node(const KeyType& k) : _next(0), _key(k){};     
      Node(const KeyType& k,Node* nxt) : _next(nxt), _key(k) {};  
      void* operator new(size_t) 
	{ 
#ifdef DEBUG_ALLOC_OBJ_TYPE
	  ALWAYS(_classDesc.registerAllocated(sizeof(Node)));
	  return Alloc::allocate(sizeof(Node),&_classDesc); 
#else 
	  return Alloc::allocate(sizeof(Node)); 
#endif
	};
      ~Node() {};
      void operator delete(void* obj) 
	{
#ifdef DEBUG_ALLOC_OBJ_TYPE
	  ALWAYS(_classDesc.registerDeallocated(sizeof(Node)));
	  Alloc::deallocate(obj,sizeof(Node),&_classDesc); 
#else
	  Alloc::deallocate(obj,sizeof(Node)); 
#endif
	};
      Node*& next() { return _next; };
      const Node * const & next() const 
      { 
	const Node * const & res = _next; // due to a bug in gcc 3.04
        return res;
	//return _next; 
      };
      static ulong minSize() { return sizeof(Node); }; 
      static ulong maxSize() { return sizeof(Node); };
      const KeyType& key() const { return _key; };
      KeyType& key() { return _key; };
    private:
      Node() {};
    private:
      Node* _next;
      KeyType _key;
#ifdef DEBUG_ALLOC_OBJ_TYPE
      static ClassDesc _classDesc;
#endif
    }; // class Node

  class Surfer
    {
    public:
      Surfer() {};
      Surfer(const VoidOrdList& lst) { reset(lst); };   
      ~Surfer() {};
      void reset(const VoidOrdList& lst)
	{
	  _currNode = lst.first();
	};
      const Node* currNode() const { return _currNode; };
      const Node* const * currNodeAddr() const { return &_currNode; };  
      bool next()
	{
	  CALL("next()");   
	  _currNode = _currNode->next();
	  return (bool)_currNode;
	}; // bool next()

      bool find(const KeyType& searchKey) { return find(searchKey,_currNode); };

      void getTo(const KeyType& searchKey) // we are sure that searchKey is in the list 
	{     
	  CALL("getTo(const KeyType& searchKey)");
	  while (_currNode->key() > searchKey) 
	    {        
	      _currNode = _currNode->next();
	    };
	  ASSERT(_currNode->key() == searchKey);  
	};

      bool find(const KeyType& searchKey,const Node*& node)
	{     
	check_node:
	  if (_currNode)
	    {     
	      if (_currNode->key() > searchKey) { _currNode = _currNode->next(); goto check_node; };
	      // _currNode->key() <= searchKey      
	      if (_currNode->key() == searchKey) { node = _currNode; return true; };
	      // _currNode->key() < searchKey  
	      node = _currNode;
	      return false;
	    };
	  node = 0;
	  return false;
	};
    private:
      const Node* _currNode;
    }; // class Surfer

 public:
  VoidOrdList() { init(); };
  ~VoidOrdList() 
    { 
      CALL("destructor ~VoidOrdList()"); 
      destroy(); 
    };
  void init() 
    {
      CALL("init()");
      _list = 0; 
      _size = 0;
    };
  void destroy();
  void* operator new(size_t) 
    {
#ifdef DEBUG_ALLOC_OBJ_TYPE 
      ALWAYS(_classDesc.registerAllocated(sizeof(VoidOrdList)));
      return Alloc::allocate(sizeof(VoidOrdList),&_classDesc); 
#else
      return Alloc::allocate(sizeof(VoidOrdList)); 
#endif
    };
  void operator delete(void* obj) 
    { 
#ifdef DEBUG_ALLOC_OBJ_TYPE
      ALWAYS(_classDesc.registerDeallocated(sizeof(VoidOrdList)));
      Alloc::deallocate(obj,sizeof(VoidOrdList),&_classDesc); 
#else
      Alloc::deallocate(obj,sizeof(VoidOrdList)); 
#endif
    };

  static ulong minSize() { return sizeof(VoidOrdList); }; 
  static ulong maxSize() { return sizeof(VoidOrdList); };

  void fastReset() 
    {
      CALL("fastReset()");
      init();
    };
  void reset() 
    {
      destroy();
      init();
    };
  operator bool() const { return (bool)_list; }; 
  const Node* first() const { return const_cast<const Node*>(_list); };
  Node* first() { return _list; };
  bool isNonempty() const { return (bool)_list; };
  unsigned long size() const { return _size; }; 
  unsigned long count() const 
    {
      unsigned long res = 0; 
      for (const Node* n = first(); n; n = n->next()) res++;
      return res;
    };
  bool contains(const KeyType& searchKey) const
    {
      Surfer surfer(*this);
      const Node* dummy;
      return surfer.find(searchKey,dummy);
    }; // bool contains (const KeyType& searchKey) const

  bool insert(const KeyType& searchKey); 
  bool remove(const KeyType& searchKey);
#ifndef NO_DEBUG
  bool checkIntegrity() const { return (count() == size()); };
#endif
 private:
  Node* _list;
  unsigned long _size;
#ifdef DEBUG_ALLOC_OBJ_TYPE
  static ClassDesc _classDesc;
#endif 
  friend class Surfer; 
}; // template <class Alloc,class KeyType,class InstanceId> class VoidOrdList

template <class Alloc,class KeyType,class InstanceId>
bool VoidOrdList<Alloc,KeyType,InstanceId>::insert(const KeyType& searchKey)
{
  CALL("insert(const KeyType& searchKey)");
  Node** addr = &_list;
  Node* node = _list;
  KeyType nodeKey;
 check_node:
  if (node)
    {
      nodeKey = node->key();       
      if (nodeKey > searchKey)
	{
	  addr = &(node->next()); 
	  node = node->next();
	  goto check_node;
	};
      // nodeKey <= searchKey
      if (nodeKey == searchKey) return false;
    };
  ASSERT(!node || (node->key() < searchKey));
  // create new node

  Node* res = new Node(searchKey,node);
  *addr = res;
  _size++;
  return true; 
}; // bool VoidOrdList<Alloc,KeyType,InstanceId>::insert(const KeyType& searchKey)
 
template <class Alloc,class KeyType,class InstanceId>
bool VoidOrdList<Alloc,KeyType,InstanceId>::remove(const KeyType& searchKey)
{
  CALL("remove(const KeyType& searchKey)");
  Node** addr = &_list;
  Node* node = _list;
  KeyType nodeKey;
 check_node:
  if (node)
    {
      nodeKey = node->key();       
      if (nodeKey > searchKey)
	{
	  addr = &(node->next()); 
	  node = node->next();
	  goto check_node;
	};
      // nodeKey <= searchKey
      if (nodeKey == searchKey) 
	{
	  *addr = node->next();
	  delete node;
	  _size--;
	  return true; 
	};    
    };
  ASSERT(!node || (node->key() < searchKey));
  return false;
}; // bool VoidOrdList<Alloc,KeyType,InstanceId>::remove(const KeyType& searchKey)

template <class Alloc,class KeyType,class InstanceId>
void VoidOrdList<Alloc,KeyType,InstanceId>::destroy()
{
  CALL("destroy()");
  if (DestructionMode::isThorough())
    {
      Node* tmp;
      while (_list) 
	{
	  tmp = _list;
	  _list = _list->next();
	  delete tmp; 
	}; 
    };
}; // void VoidOrdList<Alloc,KeyType,InstanceId>::destroy()



}; // namespace BK




//======================================================================
#undef DEBUG_NAMESPACE
//======================================================================
#endif
