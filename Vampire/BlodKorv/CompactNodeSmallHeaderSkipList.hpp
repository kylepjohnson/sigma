//
// File:         CompactNodeSmallHeaderSkipList.hpp
// Description:  Generic skip lists with small (flexible size) headers.
//               The internal nodes do not have a special field for
//               height. Instead, the last bits in the jump pointers
//               are used to mark the end of the pointer tower.
// Created:      Jan 15, 2003, Alexandre Riazanov, riazanov@cs.man.ac.uk
// Note:         Based on SmallHeaderSkipList.hpp.
//============================================================================
#ifndef COMPACT_NODE_SMALL_HEADER_SKIP_LIST_H
//=============================================================================
#define COMPACT_NODE_SMALL_HEADER_SKIP_LIST_H 
#include <climits>
#include "jargon.hpp"
#include "BlodKorvDebugFlags.hpp"
#ifdef DEBUG_ALLOC_OBJ_TYPE
 #include "ClassDesc.hpp"       
#endif
#include "DestructionMode.hpp"
//============================================================================
#undef DEBUG_NAMESPACE
#ifdef DEBUG_COMPACT_NODE_SMALL_HEADER_SKIP_LIST
 #define DEBUG_NAMESPACE "CNSHSkList<class Alloc,class ValType,class KeyType,ulong Base,class InstanceId>"
#endif
#include "debugMacros.hpp"
//============================================================================

namespace BK 
{
template <class Alloc,class ValType,class KeyType,ulong Base,class InstanceId>
class CNSHSkList
{
 public:
  class Surfer;
  class SurferWithMemory;
  class Destruction;
  class Node
  {
   public:
    ~Node() {};
    KeyType key() const 
    {
     CALL("key() const"); 
     ASSERT(checkObj());
     return _key; 
    };
    const ValType& value() const { return _value; };
    ValType& value() { return _value; };
    Node* next() { return stripJump(*(jump())); };
    const Node* next() const { return stripJump(*(jump())); };    
    static ulong minSize() { return sizeof(Node) + sizeof(Node*); }; 
    static ulong maxSize() { return ULONG_MAX; }; 

   private:
    static bool isHighestJump(const Node* ptr) 
    {
      return (reinterpret_cast<ulong>(ptr) << 30) != 0UL;
    };
    static Node* markAsHighestJump(Node* ptr)
    {
      return 
	reinterpret_cast<Node*>(reinterpret_cast<ulong>(ptr) | 0x00000001);
    };
    static const Node* markAsHighestJump(const Node* ptr)
    {
      return 
	reinterpret_cast<const Node*>(reinterpret_cast<ulong>(ptr) | 0x00000001);
    };
    static Node* stripJump(Node* ptr)
    {
      return 
        reinterpret_cast<Node*>((reinterpret_cast<ulong>(ptr) >> 1) << 1);
    };
    static const Node* stripJump(const Node* ptr)
    {
      return 
        reinterpret_cast<const Node*>((reinterpret_cast<ulong>(ptr) >> 1) << 1);
    };
    ulong computeHeight() const
    {
      CALL("Node::computeHeight() const");
      const Node* const * jmp = jump();
      while (!isHighestJump(*jmp))
	{
	  ++jmp;
	};
      return jmp - jump();
    }; 
    

    Node** jump() { return static_cast<Node**>(static_cast<void *>(this + 1)); }; 
    const Node* const * jump() const { return static_cast<const Node* const *>(static_cast<const void *>(this + 1)); };

    Node(const KeyType& k) : _key(k) {};
      
    static const Node* node(const Node * const * jmp)  // This is a hack! Must be removed.
    {
     return (static_cast<const Node*>(static_cast<const void*>(jmp))) - 1;
    };

    static Node* node(Node ** jmp)           // This is a hack! Must be removed.
    {
      return (static_cast<Node*>(static_cast<void*>(jmp))) - 1;
    };
  
    void* operator new(size_t size,ulong ht) 
    {    
     CALL("operator new(size_t size,ulong ht)");  
     #ifdef DEBUG_ALLOC_OBJ_TYPE
      ALWAYS(_classDesc.registerAllocated(size + (ht+1)*sizeof(Node*)));
      return Alloc::allocate(size + (ht+1)*sizeof(Node*),&_classDesc);
     #else
      return Alloc::allocate(size + (ht+1)*sizeof(Node*));
     #endif
    };
    void operator delete(void* obj)
    {
     CALL("operator delete(void* obj)");
     #ifdef DEBUG_ALLOC_OBJ_TYPE
      ALWAYS(_classDesc.registerDeallocated(sizeof(Node) + ((static_cast<Node*>(obj))->computeHeight() + 1)*sizeof(Node*)));
      Alloc::deallocate(obj,sizeof(Node) + ((static_cast<Node*>(obj))->computeHeight() + 1)*sizeof(Node*),&_classDesc);
     #else
      Alloc::deallocate(obj,sizeof(Node) + ((static_cast<Node*>(obj))->computeHeight() + 1)*sizeof(Node*));
     #endif
    };   
    
    void operator delete(void* obj,ulong ht)
    {
     CALL("operator delete(void* obj,ulong ht)");
     #ifdef DEBUG_ALLOC_OBJ_TYPE
      ALWAYS(_classDesc.registerDeallocated(sizeof(Node) + (ht + 1)*sizeof(Node*)));
      Alloc::deallocate(obj,sizeof(Node) + (ht + 1)*sizeof(Node*),&_classDesc);
     #else
      Alloc::deallocate(obj,sizeof(Node) + (ht + 1)*sizeof(Node*));
     #endif
    };   
    


    #ifndef NO_DEBUG 
     bool checkObj() const
     {
      #ifndef DEBUG_ALLOC
       return true;
      #else 
       ObjDesc* objDesc = GlobAlloc::hostObj(static_cast<const void*>(this));
       if (!objDesc) 
        { 
         DMSG("CNSHSkList<..>::Node::checkObj()> Null pointer to object descriptor.\n");  
         return false; 
        }; 
       bool res = true;   
       #ifdef DEBUG_ALLOC_OBJ_SIZE
        if (objDesc->size !=  (sizeof(Node) + (computeHeight() + 1)*sizeof(Node*)))
         {
          DMSG("CNSHSkList<..>::Node::checkObj()> objDesc->size != (sizeof(Node) + (computeHeight() + 1)*sizeof(Node*)) \n");
          res = false;
         };
       #endif
       #ifdef DEBUG_ALLOC_OBJ_STATUS
        if (objDesc->status != ObjDesc::Persistent) 
         {
          DMSG("CNSHSkList<..>::Node::checkObj()> objDesc->status != ObjDesc::Persistent\n");
          res = false;
         };
       #endif
       #ifdef DEBUG_ALLOC_OBJ_TYPE
        if (objDesc->classDesc != &_classDesc) 
         {
          DMSG("CNSHSkList<..>::Node::checkObj()> objDesc->classDesc != &_classDesc\n");
          res = false;
         };
       #endif
       if (!res) DOP(DMSG("CNSHSkList<..>::Node::checkObj()> host object ") << *objDesc << "\n"); 
       return res;
      #endif
     };
    #endif

   private:  
    KeyType _key;
    ValType _value;
    #ifdef DEBUG_ALLOC_OBJ_TYPE
     static ClassDesc _classDesc;
    #endif   
   friend class CNSHSkList<Alloc,ValType,KeyType,Base,InstanceId>;
   friend class Surfer;
   friend class SurferWithMemory;
   friend class Destruction;
  }; // class Node

  class Surfer
  {
   public:
    Surfer() {};
    Surfer(const CNSHSkList& sl) { reset(sl); };
    ~Surfer() {};
    void reset(const CNSHSkList& sl) 
    {
     CALL("Surfer::reset(const CNSHSkList& sl)");     
     ASSERT(sl.checkIntegrity());
     _currMaxLevel = sl._currMaxLevel;
     _jump = sl._topIndex;
     _currNode = Node::stripJump(*_jump);
    };

    const Node* currNode() const { return _currNode; };
    const Node* const * currNodeAddr() const { return &_currNode; }; 

    bool next()
    {
     CALL("next()"); 
     _currNode = _currNode->next();
     return _currNode != 0;
    }; // bool next()

    bool find(const KeyType& searchKey)
    {
      return find(searchKey,_currNode); 
    }; // bool find(const KeyType& searchKey)

    void getTo(const KeyType& searchKey) // we are sure that searchKey is in the list 
    { 
      CALL("getTo(const KeyType& searchKey)");
      ALWAYS(find(searchKey,_currNode));       
    }; // void getTo(const KeyType& searchKey)

    bool find(const KeyType searchKey,const Node*& node)
    {
     CALL("Surfer::find(const KeyType& searchKey,const Node*& node)");
     ml_next_node:
      node = Node::stripJump(_jump[_currMaxLevel]);
      if (node) 
       {
    	ASSERT(node->computeHeight() == _currMaxLevel); 
        if (node->key() < searchKey)
	 {
          if (!_currMaxLevel) return false;
          ulong currLevel = _currMaxLevel - 1;
          const Node* const * jump_ = _jump;        
	  next_node:
	   node = Node::stripJump(jump_[currLevel]);	
           ASSERT(node->computeHeight() >= currLevel);
           if (node->key() == searchKey) return true;
           if (node->key() < searchKey)
	    { // try smaller level  
	     if (!currLevel) return false;   
             currLevel--;  
            }
           else // node->key() > searchKey, proceed with the same level            
            jump_ = node->jump();
           goto next_node;
         };
        // node->key() >= searchKey 
        _jump = node->jump();
        if (node->key() == searchKey) return true;
        goto ml_next_node;
       }
      else // !node         
       {
	if (!_currMaxLevel) return false;
        _currMaxLevel--;
        goto ml_next_node;
       };      
    }; // bool find(const KeyType& searchKey,const Node*& node)
  private:
    ulong _currMaxLevel;
    const Node* const * _jump;
    const Node* _currNode;
  }; // class Surfer  
 

  class SurferWithMemory
  {
   public:
    SurferWithMemory() {};
    SurferWithMemory(const CNSHSkList& sl) { reset(sl); };
    ~SurferWithMemory() {};
    void reset(const CNSHSkList& sl) 
    {
     CALL("SurferWithMemory::reset(const CNSHSkList& sl)");
     ASSERT(sl.checkIntegrity());
     _currMaxLevel = sl._currMaxLevel;
     const Node* const * top = sl._topIndex;
     _currNode = *top;
     ASSERT(!Node::isHighestJump(_currNode));
     if (_currNode)
      {	
	for (ulong lv = 1UL; lv <= _currMaxLevel; ++lv)
	  _jump[lv] = top[lv];
      };
     ASSERT(checkIntegrity());
    }; // void reset(const CNSHSkList& sl) 

    const Node* currNode() const { return _currNode; };
    const Node* const * currNodeAddr() const { return &_currNode; }; 

    bool next()
    {
     CALL("SurferWithMemory::next()");
     ASSERT_IN(checkIntegrity(),"START");
     _currNode = _currNode->next();
     ASSERT_IN(checkIntegrity(),"END");
     return (_currNode != 0);
    }; // bool next()

    bool find(const KeyType searchKey)
    {
      CALL("SurferWithMemory::find(const KeyType& searchKey)");
      ASSERT(checkIntegrity());
      if (_currNode->key() <= searchKey) 
	{
	  return _currNode->key() == searchKey;
	};

      const Node* node;
      const Node* nextNode;
      ulong lv = _currMaxLevel;
      // operate within _jump while it is possible
      if (lv)
	{
	  node = _jump[lv];
	  ASSERT(!Node::isHighestJump(node));
	  ASSERT(node);
	  if (node->key() < searchKey)
	    {
	      --lv;
	      goto check_lv_overjump_exists;
	    };
	  if (node->key() > searchKey)
	    {
	      // no longer can operate within _jump
	      // traverse nodes
	    jump_no_overjumps_yet:
	      nextNode = Node::stripJump(node->jump()[lv]);
	      if (!nextNode)
		{
		  --lv;
		  _currMaxLevel = lv;
		  if (lv) goto jump_no_overjumps_yet;
		  _currNode = node;
		  goto level_zero_no_overjumps_yet;
		};
	      if (nextNode->key() < searchKey)
		{
		  _jump[lv] = nextNode;
		  --lv;
		  if (lv) goto jump_overjump_exists;		  
		  _currNode = node;
		  goto level_zero_overjump_exists;
		};
	      if (nextNode->key() > searchKey)
		{
		  node = nextNode;
		  goto jump_no_overjumps_yet;
		};
	      // nextNode->key() == searchKey
	      _currNode = nextNode;
	      ASSERT(_currNode->key() == searchKey);
	      ASSERT(checkIntegrity());
	      return true;
	    };
	  // node->key() == searchKey
	  _currNode = node;
	  ASSERT(_currNode->key() == searchKey);
	  ASSERT(checkIntegrity());
	  return true;
	}
      else // lv == 0UL
	{
	level_zero_no_overjumps_yet:
	  ASSERT(_currNode);
	  ASSERT(_currNode->key() > searchKey);
	  _currNode = _currNode->next();
	  if ((!_currNode) || (_currNode->key() < searchKey)) 	    
	    return false;
	  ASSERT(_currNode->computeHeight() == 0UL);
	  if (_currNode->key() == searchKey) 
	    {
	      ASSERT(checkIntegrity());
	      return true;
	    };
	  // _currNode->key() > searchKey
	  goto level_zero_no_overjumps_yet;
	};

      // Here we know that there was an overjump.
      // This allows us to avoid some unnecessary tests. 
    check_lv_overjump_exists:
      if (lv)
	{
	  node = _jump[lv];
	  ASSERT(!Node::isHighestJump(node));
	  ASSERT(node);
	  if (node->key() < searchKey)
	    {
	      --lv;
	      goto check_lv_overjump_exists;
	    };
	  if (node->key() > searchKey)
	    {
	      // no longer can operate within _jump
	      // traverse nodes
	    jump_overjump_exists:
	      nextNode = Node::stripJump(node->jump()[lv]);
	      ASSERT(nextNode);
	      if (nextNode->key() < searchKey)
		{
		  _jump[lv] = nextNode;
		  --lv;
		  if (lv) goto jump_overjump_exists;		  
		  _currNode = node;
		  goto level_zero_overjump_exists;
		};
	      if (nextNode->key() > searchKey)
		{
		  node = nextNode;
		  goto jump_overjump_exists;
		};
	      // nextNode->key() == searchKey
	      _currNode = nextNode;
	      ASSERT(_currNode->key() == searchKey);
	      ASSERT(checkIntegrity());
	      return true;
	    };
	  // node->key() == searchKey
	  _currNode = node;
	  ASSERT(_currNode->key() == searchKey);
	  ASSERT(checkIntegrity());
	  return true;
	}
      else // lv == 0UL
	{
	level_zero_overjump_exists:
	  ASSERT(_currNode);
	  ASSERT(_currNode->key() > searchKey);
	  _currNode = _currNode->next();
	  ASSERT(_currNode);
	  if (_currNode->key() < searchKey) 	    
	    return false;
	  ASSERT(_currNode->computeHeight() == 0UL);
	  if (_currNode->key() == searchKey) 
	    {
	      ASSERT(checkIntegrity());
	      return true;
	    };
	  // _currNode->key() > searchKey
	  goto level_zero_overjump_exists;
	};
    }; // bool find(const KeyType searchKey)

    void getTo(const KeyType searchKey)
    {
      CALL("SurferWithMemory::getTo(const KeyType& searchKey)");
      ASSERT(checkIntegrity());
      ASSERT(_currNode->key() >= searchKey);
      if (_currNode->key() == searchKey) 
	{
	  return;
	};

      const Node* node;
      const Node* nextNode;
      ulong lv = _currMaxLevel;
      // operate within _jump while it is possible
      if (lv)
	{
	  node = _jump[lv];
	  ASSERT(!Node::isHighestJump(node));
	  ASSERT(node);
	  if (node->key() < searchKey)
	    {
	      --lv;
	      goto check_lv_overjump_exists;
	    };
	  if (node->key() > searchKey)
	    {
	      // no longer can operate within _jump
	      // traverse nodes
	    jump_no_overjumps_yet:
	      nextNode = Node::stripJump(node->jump()[lv]);
	      if (!nextNode)
		{
		  --lv;
		  _currMaxLevel = lv;
		  if (lv) goto jump_no_overjumps_yet;
		  _currNode = node;
		  goto level_zero_no_overjumps_yet;
		};
	      if (nextNode->key() < searchKey)
		{
		  _jump[lv] = nextNode;
		  --lv;
		  if (lv) goto jump_overjump_exists;		  
		  _currNode = node;
		  goto level_zero_overjump_exists;
		};
	      if (nextNode->key() > searchKey)
		{
		  node = nextNode;
		  goto jump_no_overjumps_yet;
		};
	      // nextNode->key() == searchKey
	      _currNode = nextNode;
	      ASSERT(_currNode->key() == searchKey);
	      ASSERT(checkIntegrity());
	      return;
	    };
	  // node->key() == searchKey
	  _currNode = node;
	  ASSERT(_currNode->key() == searchKey);
	  ASSERT(checkIntegrity());
	  return;
	}
      else // lv == 0UL
	{
	level_zero_no_overjumps_yet:
	  ASSERT(_currNode);
	  ASSERT(_currNode->key() > searchKey);
	  _currNode = _currNode->next();
	  ASSERT(_currNode);
	  ASSERT(_currNode->key() >= searchKey);
	  ASSERT(_currNode->computeHeight() == 0UL);
	  if (_currNode->key() == searchKey) 
	    {
	      ASSERT(checkIntegrity());
	      return;
	    };
	  // _currNode->key() > searchKey
	  goto level_zero_no_overjumps_yet;
	};

      // Here we know that there was an overjump.
      // This allows us to avoid some unnecessary tests. 
    check_lv_overjump_exists:
      if (lv)
	{
	  node = _jump[lv];
	  ASSERT(!Node::isHighestJump(node));
	  ASSERT(node);
	  if (node->key() < searchKey)
	    {
	      --lv;
	      goto check_lv_overjump_exists;
	    };
	  if (node->key() > searchKey)
	    {
	      // no longer can operate within _jump
	      // traverse nodes
	    jump_overjump_exists:
	      nextNode = Node::stripJump(node->jump()[lv]);
	      ASSERT(nextNode);
	      if (nextNode->key() < searchKey)
		{
		  _jump[lv] = nextNode;
		  --lv;
		  if (lv) goto jump_overjump_exists;		  
		  _currNode = node;
		  goto level_zero_overjump_exists;
		};
	      if (nextNode->key() > searchKey)
		{
		  node = nextNode;
		  goto jump_overjump_exists;
		};
	      // nextNode->key() == searchKey
	      _currNode = nextNode;
	      ASSERT(_currNode->key() == searchKey);
	      ASSERT(checkIntegrity());
	      return;
	    };
	  // node->key() == searchKey
	  _currNode = node;
	  ASSERT(_currNode->key() == searchKey);
	  ASSERT(checkIntegrity());
	  return;
	}
      else // lv == 0UL
	{
	level_zero_overjump_exists:
	  ASSERT(_currNode);
	  ASSERT(_currNode->key() > searchKey);
	  _currNode = _currNode->next();
	  ASSERT(_currNode);
	  ASSERT(_currNode->key() >= searchKey);
	  ASSERT(_currNode->computeHeight() == 0UL);
	  if (_currNode->key() == searchKey) 
	    {
	      ASSERT(checkIntegrity());
	      return;
	    };
	  // _currNode->key() > searchKey
	  goto level_zero_overjump_exists;
	};
    }; // void getTo(const KeyType searchKey)


    #ifdef DEBUG_NAMESPACE
     bool checkIntegrity()
     {
      if (!_currNode) return true;
      ulong currNodeHeight = _currNode->computeHeight();
      if (currNodeHeight > _currMaxLevel) 
       {
        DMSG("CNSHSkList<..>::SurferWithMemory::checkIntegrity()> currNodeHeight > _currMaxLevel\n");
        return false;  
       };
      for (ulong lv = 1UL; lv <= _currMaxLevel; ++lv)
	{
	  if (Node::isHighestJump(_jump[lv]))
	    {
	      DMSG("CNSHSkList<..>::SurferWithMemory::checkIntegrity()> Node::isHighestJump(_jump[lv])\n");
	      return false;
	    };
	  if (!_jump[lv])
	    {
	      DMSG("CNSHSkList<..>::SurferWithMemory::checkIntegrity()> !_jump[lv]\n");
	      return false;
	    };
	  if (lv <= currNodeHeight)
	    {
	      if (_jump[lv]->key() < _currNode->key())
		{
		  DMSG("CNSHSkList<..>::SurferWithMemory::checkIntegrity()> _jump[lv]->key() < _currNode->key()\n");
		  return false;
		};
	    }
	  else
	    {
	      if (lv > 1UL)
		{
		  if (_jump[lv]->key() > _jump[lv-1]->key())
		    {
		      DMSG("CNSHSkList<..>::SurferWithMemory::checkIntegrity()> _jump[lv]->key() > _jump[lv-1]->key()\n");
		      return false;
		    };
		};
	    };
	};
      return true;
     };
    #endif    

   private:
    ulong _currMaxLevel;
    const Node* _jump[28]; // is there a list longer than 2^28 ?
    const Node* _currNode;
  }; // class SurferWithMemory  





  class Destruction
  {
   public:
    Destruction() {};
    Destruction(CNSHSkList& sl) { reset(sl); }; 
    ~Destruction() {};
    void reset(CNSHSkList& sl) 
    { 
     _node = sl.first();
     sl._insertCount = 0;
     sl._size = 0;

     for (long i = 0; i <= sl._currMaxLevel; ++i)
       {
	 sl._topIndex[i] = (Node*)0;
       };
     sl._currMaxLevel = 0;
     // sl._topIndex will be deallocated upon the destructor call for sl
    }; // void reset(CNSHSkList& sl) 

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
  }; // class Destruction

 public:
  CNSHSkList() 
   {
    CALL("constructor CNSHSkList()"); 
    init();
   };

  ~CNSHSkList() 
   {
    CALL("destructor ~CNSHSkList()");
    destroy();
   };

  void init() 
  {
   CALL("init()");
   _currMaxLevel = 0;
   _sizeOfTopIndex = 1;
   #ifdef DEBUG_ALLOC_OBJ_TYPE
     ALWAYS(_headerClassDesc.registerAllocated(_sizeOfTopIndex * sizeof(Node*))); 
     _topIndex = (Node**)Alloc::allocate(_sizeOfTopIndex * sizeof(Node*),&_headerClassDesc); 
    #else
     _topIndex = (Node**)Alloc::allocate(_sizeOfTopIndex * sizeof(Node*)); 
    #endif
    _topIndex[0] = (Node*)0;
   _insertCount = 0;
   _size = 0;
   ASSERT(checkIntegrity());
  }; // void init() 

  void destroy();

  void* operator new(size_t) 
  {
   CALL("operator new(size_t)"); 
   #ifdef DEBUG_ALLOC_OBJ_TYPE
    ALWAYS(_classDesc.registerAllocated(sizeof(CNSHSkList)));
    return Alloc::allocate(sizeof(CNSHSkList),&_classDesc);
   #else
    return Alloc::allocate(sizeof(CNSHSkList));
   #endif 
  };
  void operator delete(void* obj) 
  {
   CALL("operator delete(void* obj)"); 
   #ifdef DEBUG_ALLOC_OBJ_TYPE
    ALWAYS(_classDesc.registerDeallocated(sizeof(CNSHSkList)));
    Alloc::deallocate(obj,sizeof(CNSHSkList),&_classDesc);
   #else
    Alloc::deallocate(obj,sizeof(CNSHSkList));
   #endif 
  };
  static ulong minSize() { return sizeof(CNSHSkList); }; // has nothing to do with size()
  static ulong maxSize() { return sizeof(CNSHSkList); }; // has nothing to do with size()
  static ulong headerMinSize() { return sizeof(Node*); }; // has nothing to do with size()
  static ulong headerMaxSize() { return ULONG_MAX; }; 

  void reset() 
  {
   CALL("reset()");
   destroy();
   init();
  };

  operator bool() const 
  {
   CALL("operator bool() const"); 
   return _topIndex[0] != (Node*)0; 
  }; 
  const Node* first() const 
  {
   CALL("first() const"); 
   return _topIndex[0]; 
  };
  Node* first() 
  {
   CALL("first()");
   return _topIndex[0]; 
  };

  ulong size() const { return _size; }; 

  ulong count() const 
  {   
   ulong res = 0; 
   for (const Node* n = first(); n; n = n->next()) 
    { 
     res++;
    };
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
  bool remove(const KeyType& searchKey);

  #ifndef NO_DEBUG
   bool checkIntegrity() const
   {
    if (!_topIndex) return false; 
    
    #ifdef DEBUG_ALLOC
     bool res = true;
     ObjDesc* objDesc = GlobAlloc::hostObj(static_cast<void*>(_topIndex));          
     if (!objDesc) 
      { 
       DMSG("CNSHVoidSkList<..>::checkIntegrity()> Null pointer to object descriptor for *_topIndex.\n");  
       return false; 
      };     
     #ifdef DEBUG_ALLOC_OBJ_SIZE
      if (objDesc->size !=  _sizeOfTopIndex * sizeof(Node*)) 
       {
        DMSG("CNSHVoidSkList<..>::checkIntegrity()> Wrong size of *_topIndex.\n");
        res = false;
       };
     #endif
     #ifdef DEBUG_ALLOC_OBJ_STATUS
      if (objDesc->status != ObjDesc::Persistent) 
       {
        DMSG("CNSHVoidSkList<..>::checkIntegrity()> objDesc->status != ObjDesc::Persistent for *_topIndex.\n");
        res = false; 
       };
     #endif      
     #ifdef DEBUG_ALLOC_OBJ_TYPE
      if (objDesc->classDesc != &_headerClassDesc) 
       {
        DMSG("CNSHVoidSkList<..>::checkIntegrity()> Wrong objDesc->classDesc with *_topIndex.\n");
        res = false;
       };
     #endif
     if (!res) return false;
    #endif

     for (ulong lv = 1UL; lv <= _currMaxLevel; ++lv)
       {
	 if (Node::isHighestJump(_topIndex[lv]))
	   {
	     DMSG("CNSHVoidSkList<..>::checkIntegrity()> Node::isHighestJump(_topIndex[lv])\n");
	     return false;
	   };
	 if (!_topIndex[lv])
	   {
	     DMSG("CNSHVoidSkList<..>::checkIntegrity()> !_topIndex[lv]\n");
	     return false;
	   };
	 if (_topIndex[lv]->key() > _topIndex[lv-1]->key())
	   {
	     DMSG("CNSHVoidSkList<..>::checkIntegrity()> _topIndex[lv]->key() > _topIndex[lv-1]->key()\n");
	     return false;
	   };
       };
     if (count() != size())
       {
	 DMSG("CNSHVoidSkList<..>::checkIntegrity()> count() != size()\n");
	 return false;
       };
    return true; 
   }; // bool checkIntegrity() const
  #endif

  ostream& output(ostream& ostr) const
  {
   for (const Node* node = first(); node; node = node->next())
    {
     node->output(ostr,_currMaxLevel) << "\n";
    };
   return ostr;
  };
  ostream& outputKeys(ostream& ostr) const
  {
   for (const Node* node = first(); node; node = node->next())
    {
     ostr << node->key() << "\n";
    };
   return ostr;
  };

 private:
  CNSHSkList(const CNSHSkList& sl)
  {
   CALL("constructor CNSHSkList(const CNSHSkList& sl)");
   ICP("ICP0");
  };
  
  CNSHSkList& operator=(const CNSHSkList& sl)
  {
   CALL("operator=(const CNSHSkList& sl)");
   ICP("ICP0");
   return *this;
  };

  ulong randomHeight() const
  { 
   ulong res = 0;
   ulong tmp = _insertCount;
   while (!(tmp % Base))
    { 
     res++;
     tmp = tmp/Base;
    };
   return res;
  }; // randomHeight() 

 private:
  ulong _sizeOfTopIndex;
  Node** _topIndex;
  ulong _currMaxLevel;
  ulong _insertCount;
  ulong _size;
  #ifdef DEBUG_ALLOC_OBJ_TYPE
   static ClassDesc _classDesc;
   static ClassDesc _headerClassDesc;
  #endif
 friend class Surfer; 
 friend class SurferWithMemory; 
 friend class Destruction;   
}; // template <class Alloc,class ValType,class KeyType,ulong Base,class InstanceId> class CNSHSkList


}; // namespace BK

//============================================================================
#undef DEBUG_NAMESPACE
#ifdef DEBUG_COMPACT_NODE_SMALL_HEADER_SKIP_LIST
 #define DEBUG_NAMESPACE "CNSHSkList<class Alloc,class ValType,class KeyType,ulong Base,class InstanceId>"
#endif
#include "debugMacros.hpp"
//============================================================================


namespace BK 
{

template <class Alloc,class ValType,class KeyType,ulong Base,class InstanceId>
inline
CNSHSkList<Alloc,ValType,KeyType,Base,InstanceId>::Node* CNSHSkList<Alloc,ValType,KeyType,Base,InstanceId>::insert(const KeyType& searchKey,bool& newNode)  
{
 CALL("insert(const KeyType& searchKey,bool& newNode)");

 ASSERT_IN(checkIntegrity(),"START");

 Node* update[32];

 Node* currNode = Node::node(_topIndex);      // tricky!
 ulong currLevel = _currMaxLevel;
 Node* nextNode = currNode->jump()[currLevel];  // tricky!
 ASSERT(!Node::isHighestJump(nextNode));

 KeyType nodeKey;
 check_next_node:
  if (nextNode)
   {
     ASSERT(nextNode->computeHeight() <= currLevel);
    nodeKey = nextNode->key();
    if (nodeKey > searchKey) 
     {
      currNode = nextNode;
      nextNode = Node::stripJump(nextNode->jump()[currLevel]);        
      goto check_next_node;
     };
    // nodeKey <= searchKey
    if (nodeKey == searchKey) 
      { 
	newNode = false;
	ASSERT(checkIntegrity()); 
	return nextNode; 
      };
    // nodeKey < searchKey, "if (nextNode)" is not needed anymore
    update[currLevel] = currNode;
    while (currLevel) 
     { 
      currLevel--;
      nextNode = currNode->jump()[currLevel]; 

      ASSERT(!Node::isHighestJump(nextNode));
      nodeKey = nextNode->key();
      while (nodeKey > searchKey)
       {
	ASSERT(nextNode->computeHeight() == currLevel);
	currNode = nextNode;
        nextNode = Node::stripJump(nextNode->jump()[currLevel]);
        nodeKey = nextNode->key();
       };
      // nodeKey <= searchKey
      if (nodeKey == searchKey) 
	{ 
	  newNode = false;
	  ASSERT(checkIntegrity()); 
	  return nextNode; 
	};
      // nodeKey < searchKey, "if (nextNode)" is not needed anymore
      update[currLevel] = currNode;
     };
    ASSERT(nextNode->key() < searchKey);
   }
  else // !nextNode
   {
    update[currLevel] = currNode;
    if (currLevel) 
     {
      currLevel--;
      nextNode = currNode->jump()[currLevel];
      ASSERT(!Node::isHighestJump(nextNode));
      goto check_next_node;
     };
   };
 
  ASSERT(!currLevel);    

  // insert new node
  ASSERT(_insertCount < ULONG_MAX);
  _insertCount++;  
  _size++;
  currLevel = randomHeight();

  currNode = new (currLevel) Node(searchKey); 
  Node** nodeJump = currNode->jump();  
  Node** jumpAddr; 
  Node* jump;

  if (currLevel > _currMaxLevel)
   {
    // _topIndex must be expanded 

    #ifdef DEBUG_ALLOC_OBJ_TYPE
     ALWAYS(_headerClassDesc.registerAllocated((currLevel + 1) * sizeof(Node*)));
     Node** newTopIndex = (Node**)Alloc::allocate((currLevel + 1) * sizeof(Node*),&_headerClassDesc);
    #else
     Node** newTopIndex = (Node**)Alloc::allocate((currLevel + 1) * sizeof(Node*));
    #endif
	
    ulong lev;
    for (lev = 0UL; lev <= _currMaxLevel; ++lev)
      newTopIndex[lev] = _topIndex[lev];
    
   
    #ifdef DEBUG_ALLOC_OBJ_TYPE
     ALWAYS(_headerClassDesc.registerDeallocated(_sizeOfTopIndex * sizeof(Node*)));
     Alloc::deallocate(_topIndex,_sizeOfTopIndex * sizeof(Node*),&_headerClassDesc);
    #else
     Alloc::deallocate(_topIndex,_sizeOfTopIndex * sizeof(Node*));   
    #endif
      

    Node* pseudonode = Node::node(_topIndex);      

    for (lev = 0UL; lev <= _currMaxLevel; ++lev)
      {
        ASSERT(!Node::isHighestJump(update[lev]));
	if (update[lev] == pseudonode)      
	  {
	    ASSERT((!newTopIndex[lev]) || (newTopIndex[lev]->key() < searchKey));
	    nodeJump[lev] = newTopIndex[lev];
	    newTopIndex[lev] = currNode;
	  }
	else
	  {
	    jumpAddr = update[lev]->jump() + lev;
	    jump = *jumpAddr;	
	    if (Node::isHighestJump(jump))
	      {
		ASSERT(update[lev]->computeHeight() == lev);
		*jumpAddr = Node::markAsHighestJump(currNode);	
		nodeJump[lev] = Node::stripJump(jump);
	      }
	    else
	      {
		ASSERT(update[lev]->computeHeight() > lev);
		*jumpAddr = currNode;	
		nodeJump[lev] = jump;
	      };
	  };
      }; // for (lev = 0UL;...
    
    _sizeOfTopIndex = currLevel + 1;
    _topIndex = newTopIndex;

    ++_currMaxLevel;
    while (_currMaxLevel < currLevel)
     {
      _topIndex[_currMaxLevel] = currNode;
      nodeJump[_currMaxLevel] = 0;
      ++_currMaxLevel;
     };
    _topIndex[_currMaxLevel] = currNode;
    nodeJump[_currMaxLevel] = Node::markAsHighestJump(static_cast<Node*>(0));
    ASSERT(currNode->computeHeight() == _currMaxLevel);
   }
  else // currLevel <= _currMaxLevel
   {       
     ulong lev;
     for (lev = 0UL; lev < currLevel; ++lev)
       {
	 ASSERT(!Node::isHighestJump(update[lev]));
	 jumpAddr = update[lev]->jump() + lev;
	 jump = *jumpAddr;		
	 if (Node::isHighestJump(jump))
	   {
	     ASSERT(update[lev]->computeHeight() == lev);
	     *jumpAddr = Node::markAsHighestJump(currNode);
	     nodeJump[lev] = Node::stripJump(jump);
	   }
	 else
	   {
	     ASSERT(update[lev]->computeHeight() > lev);
	     *jumpAddr = currNode;
	     nodeJump[lev] = jump;
	   };
       };
     
     ASSERT(!Node::isHighestJump(update[lev]));
     jumpAddr = update[lev]->jump() + lev;
     jump = *jumpAddr;		
     if (Node::isHighestJump(jump))
       {
	 ASSERT(update[lev]->computeHeight() == lev);
	 *jumpAddr = Node::markAsHighestJump(currNode);
	 nodeJump[lev] = jump;
       }
     else
       {
	 ASSERT(update[lev]->computeHeight() > lev);
	 *jumpAddr = currNode;
	 nodeJump[lev] = Node::markAsHighestJump(jump);
       };
     ASSERT(currNode->computeHeight() == currLevel);
   }; 
  newNode = true;
  ASSERT_IN(checkIntegrity(),"END");
  return currNode;
}; // Node* CNSHSkList<..>::insert(const KeyType& searchKey,bool& newNode) 



template <class Alloc,class ValType,class KeyType,ulong Base,class InstanceId>
inline
bool CNSHSkList<Alloc,ValType,KeyType,Base,InstanceId>::remove(const KeyType& searchKey,ValType& val) 
{
  CALL("remove(const KeyType& searchKey,ValType& val)");
 ASSERT(checkIntegrity());
 Node** jumpAddr = _topIndex;
 bool highest = false;
 ulong currLevel = _currMaxLevel; 
 Node* node;
 next_node:   
  node = jumpAddr[currLevel];
  ASSERT(highest == Node::isHighestJump(node));
  if (highest) node = Node::stripJump(node);
  
  if (node)
   {
    ASSERT(node->computeHeight() >= currLevel);  
    if (node->key() == searchKey)
     { 
       if (highest)
	 {
	   jumpAddr[currLevel] = Node::markAsHighestJump(node->jump()[currLevel]);
	 }
       else
	 {
	   jumpAddr[currLevel] = Node::stripJump(node->jump()[currLevel]);
	 };
      if (currLevel) 
	{ 
	  currLevel--;
	  highest = false;
	  goto next_node; 
	};

      while ((!(_topIndex[_currMaxLevel])) && _currMaxLevel)
	_currMaxLevel--; 

      val = node->value();
      delete node;
      _size--;
      ASSERT(checkIntegrity());
      ASSERT(!contains(searchKey));
      return true;
     };
    if (node->key() > searchKey) 
     {
      jumpAddr = node->jump();
      highest = true;
      goto next_node;
     };
    // node->key() < searchKey, try smaller level
    if (currLevel) 
      { 
	currLevel--;	
	highest = false;
	goto next_node; 
      }; 
    ASSERT(checkIntegrity());    
    ASSERT(!contains(searchKey)); 
    return false;
   };
    
  // !node
  if (currLevel) 
    { 
      currLevel--;      
      highest = false;
      goto next_node; 
    };
  ASSERT(checkIntegrity());
  ASSERT(!contains(searchKey));
  return false;
}; // bool CNSHSkList<..>::remove(const KeyType& searchKey,ValType& val)  



template <class Alloc,class ValType,class KeyType,ulong Base,class InstanceId>
inline
bool CNSHSkList<Alloc,ValType,KeyType,Base,InstanceId>::remove(const KeyType& searchKey) 
{
 CALL("remove(const KeyType& searchKey)");
 ASSERT(checkIntegrity());
 Node** jumpAddr = _topIndex;
 bool highest = false;
 ulong currLevel = _currMaxLevel; 
 Node* node;
 next_node:   
  node = jumpAddr[currLevel];
  ASSERT(highest == Node::isHighestJump(node));
  if (highest) node = Node::stripJump(node);
  
  if (node)
   {
    ASSERT(node->computeHeight() >= currLevel);  
    if (node->key() == searchKey)
     { 
       if (highest)
	 {
	   jumpAddr[currLevel] = Node::markAsHighestJump(node->jump()[currLevel]);
	 }
       else
	 {
	   jumpAddr[currLevel] = Node::stripJump(node->jump()[currLevel]);
	 };
      if (currLevel) 
	{ 
	  currLevel--;
	  highest = false;
	  goto next_node; 
	};

      while ((!(_topIndex[_currMaxLevel])) && _currMaxLevel)
	_currMaxLevel--; 


      delete node;
      _size--;
      ASSERT(checkIntegrity());
      ASSERT(!contains(searchKey));
      return true;
     };
    if (node->key() > searchKey) 
     {
      jumpAddr = node->jump();
      highest = true;
      goto next_node;
     };
    // node->key() < searchKey, try smaller level
    if (currLevel) 
      { 
	currLevel--;	
	highest = false;
	goto next_node; 
      }; 
    ASSERT(checkIntegrity());    
    ASSERT(!contains(searchKey)); 
    return false;
   };
    
  // !node
  if (currLevel) 
    { 
      currLevel--;      
      highest = false;
      goto next_node; 
    };
  ASSERT(checkIntegrity());
  ASSERT(!contains(searchKey));
  return false;
}; // bool CNSHSkList<..>::remove(const KeyType& searchKey)  



template <class Alloc,class ValType,class KeyType,ulong Base,class InstanceId>
void CNSHSkList<Alloc,ValType,KeyType,Base,InstanceId>::destroy()
{
 CALL("destroy()");
 ASSERT(checkIntegrity());
 if (DestructionMode::isThorough())
   {
     Node* node = _topIndex[0];
     Node* tmp;
     while (node) 
       { 
	 tmp = node;   
	 node = node->next(); 
	 delete tmp;
       };  
#ifdef DEBUG_ALLOC_OBJ_TYPE
     ALWAYS(_headerClassDesc.registerDeallocated(_sizeOfTopIndex * sizeof(Node*)));
     Alloc::deallocate(_topIndex,_sizeOfTopIndex * sizeof(Node*),&_headerClassDesc);
     
#else 
     Alloc::deallocate(_topIndex,_sizeOfTopIndex * sizeof(Node*));
#endif
   };
 BK_CORRUPT(_topIndex);
}; // void CNSHSkList<Alloc,ValType,KeyType,Base,InstanceId>::destroy()

}; // namespace BK



//============ Specialised version for ValType = void
//============================================================================
#undef DEBUG_NAMESPACE
#ifdef DEBUG_COMPACT_NODE_SMALL_HEADER_SKIP_LIST
 #define DEBUG_NAMESPACE "CNSHVoidSkList<class Alloc,class KeyType,ulong Base,class InstanceId>"
#endif
#include "debugMacros.hpp"
//============================================================================

namespace BK 
{

template <class Alloc,class KeyType,ulong Base,class InstanceId>
class CNSHVoidSkList
{
 public:
  class Surfer;
  class SurferWithMemory;
  class Node
  {
   public:
    ~Node() {};
    KeyType key() const 
    {
     CALL("key() const"); 
     ASSERT(checkObj());
     return _key; 
    };
    Node* next() { return stripJump(*(jump())); };
    const Node* next() const { return stripJump(*(jump())); };    
    static ulong minSize() { return sizeof(Node) + sizeof(Node*); }; 
    static ulong maxSize() { return ULONG_MAX; }; 



   private:
    static bool isHighestJump(const Node* ptr) 
    {
      return (reinterpret_cast<ulong>(ptr) << 30) != 0UL;
    };
    static Node* markAsHighestJump(Node* ptr)
    {
      return 
	reinterpret_cast<Node*>(reinterpret_cast<ulong>(ptr) | 0x00000001);
    };
    static const Node* markAsHighestJump(const Node* ptr)
    {
      return 
	reinterpret_cast<const Node*>(reinterpret_cast<ulong>(ptr) | 0x00000001);
    };
    static Node* stripJump(Node* ptr)
    {
      return 
        reinterpret_cast<Node*>((reinterpret_cast<ulong>(ptr) >> 1) << 1);
    };
    static const Node* stripJump(const Node* ptr)
    {
      return 
        reinterpret_cast<const Node*>((reinterpret_cast<ulong>(ptr) >> 1) << 1);
    };
    ulong computeHeight() const
    {
      CALL("Node::computeHeight() const");
      const Node* const * jmp = jump();
      while (!isHighestJump(*jmp))
	{
	  ++jmp;
	};
      return jmp - jump();
    };

    Node** jump() { return (Node**)(this + 1); }; 
    const Node* const * jump() const { return (const Node* const *)(this + 1); };
   
    Node(const KeyType& k) :  _key(k) {};
      
    static const Node* node(const Node * const * jmp)  // This is a hack! Must be removed.
    {
     return ((Node*)jmp) - 1;
    };

    static Node* node(Node ** jmp)           // This is a hack! Must be removed.
    {
     return ((Node*)jmp) - 1;
    };
  
    void* operator new(size_t,ulong ht) 
    {    
     CALL("operator new(size_t,ulong ht)");  
     #ifdef DEBUG_ALLOC_OBJ_TYPE
      ALWAYS(_classDesc.registerAllocated(sizeof(Node) + (ht+1)*sizeof(Node*)));
      return Alloc::allocate(sizeof(Node) + (ht+1)*sizeof(Node*),&_classDesc);
     #else
      return Alloc::allocate(sizeof(Node) + (ht+1)*sizeof(Node*));
     #endif
    };
    void operator delete(void* obj)
    {
     CALL("operator delete(void* obj)");
     #ifdef DEBUG_ALLOC_OBJ_TYPE
      ALWAYS(_classDesc.registerDeallocated(sizeof(Node) + ((static_cast<Node*>(obj))->computeHeight() + 1)*sizeof(Node*)));
      Alloc::deallocate(obj,sizeof(Node) + ((static_cast<Node*>(obj))->computeHeight() + 1)*sizeof(Node*),&_classDesc);
     #else
      Alloc::deallocate(obj,sizeof(Node) + ((static_cast<Node*>(obj))->computeHeight() + 1)*sizeof(Node*));
     #endif
    };   

    void operator delete(void* obj,ulong ht)
    {
     CALL("operator delete(void* obj,ulong ht)");
     #ifdef DEBUG_ALLOC_OBJ_TYPE
      ALWAYS(_classDesc.registerDeallocated(sizeof(Node) + (ht + 1)*sizeof(Node*)));
      Alloc::deallocate(obj,sizeof(Node) + (ht + 1)*sizeof(Node*),&_classDesc);
     #else
      Alloc::deallocate(obj,sizeof(Node) + (ht + 1)*sizeof(Node*));
     #endif
    };   


    #ifndef NO_DEBUG 
     bool checkObj() const
     {
      #ifndef DEBUG_ALLOC
       return true;
      #else 
       ObjDesc* objDesc = GlobAlloc::hostObj(static_cast<const void*>(this));
       if (!objDesc) 
        { 
         DMSG("CNSHVoidSkList<..>::Node::checkObj()> Null pointer to object descriptor.\n");  
         return false; 
        }; 
       bool res = true;   
       #ifdef DEBUG_ALLOC_OBJ_SIZE
        if (objDesc->size !=  (sizeof(Node) + (computeHeight() + 1)*sizeof(Node*)))
         {
          DMSG("CNSHVoidSkList<..>::Node::checkObj()> objDesc->size != (sizeof(Node) + (computeHeight() + 1)*sizeof(Node*)) \n");
          res = false;
         };
       #endif
       #ifdef DEBUG_ALLOC_OBJ_STATUS
        if (objDesc->status != ObjDesc::Persistent) 
         {
          DMSG("CNSHVoidSkList<..>::Node::checkObj()> objDesc->status != ObjDesc::Persistent\n");
          res = false;
         };
       #endif
       #ifdef DEBUG_ALLOC_OBJ_TYPE
        if (objDesc->classDesc != &_classDesc) 
         {
          DMSG("CNSHVoidSkList<..>::Node::checkObj()> objDesc->classDesc != &_classDesc\n");
          res = false;
         };
       #endif
       if (!res) DOP(DMSG("CNSHVoidSkList<..>::Node::checkObj()> host object ") << *objDesc << "\n"); 
       return res;
      #endif
     };
    #endif

   private:
    KeyType _key;    
    #ifdef DEBUG_ALLOC_OBJ_TYPE
     static ClassDesc _classDesc;
    #endif   
   friend class CNSHVoidSkList<Alloc,KeyType,Base,InstanceId>;
   friend class Surfer;
   friend class SurferWithMemory;
  }; // class Node

  class Surfer
  {
   public:
    Surfer() {};
    Surfer(const CNSHVoidSkList& sl) { reset(sl); };
    ~Surfer() {};
    void reset(const CNSHVoidSkList& sl) 
    {
     CALL("Surfer::reset(const CNSHVoidSkList& sl)");
     ASSERT(sl.checkIntegrity());
     _currMaxLevel = sl._currMaxLevel;
     _jump = sl._topIndex;
     _currNode = Node::stripJump(*_jump);
    };
    const Node* currNode() const { return _currNode; };
    const Node* const * currNodeAddr() const { return &_currNode; }; 

    bool next()
    {
     CALL("next()"); 
     _currNode = _currNode->next();
     return _currNode != 0;
    }; // bool next()

    bool find(const KeyType& searchKey)
    {
      return find(searchKey,_currNode); 
    }; // bool find(const KeyType& searchKey)

    void getTo(const KeyType& searchKey) // we are sure that searchKey is in the list 
    { 
      CALL("getTo(const KeyType& searchKey)");
      ALWAYS(find(searchKey,_currNode));       
    }; // void getTo(const KeyType& searchKey)

    bool find(const KeyType searchKey,const Node*& node)
    {
     CALL("Surfer::find(const KeyType& searchKey,const Node*& node)");
     ml_next_node:
      node = Node::stripJump(_jump[_currMaxLevel]);
      if (node) 
       {
    	ASSERT(node->computeHeight() == _currMaxLevel); 
        if (node->key() < searchKey)
	 {
          if (!_currMaxLevel) return false;
          ulong currLevel = _currMaxLevel - 1;
          const Node* const * jump_ = _jump;        
	  next_node:
	   node = Node::stripJump(jump_[currLevel]);	
           ASSERT(node->computeHeight() >= currLevel);
           if (node->key() == searchKey) return true;
           if (node->key() < searchKey)
	    { // try smaller level  
	     if (!currLevel) return false;   
             currLevel--;  
            }
           else // node->key() > searchKey, proceed with the same level            
            jump_ = node->jump();
           goto next_node;
         };
        // node->key() >= searchKey 
        _jump = node->jump();
        if (node->key() == searchKey) return true;
        goto ml_next_node;
       }
      else // !node         
       {
	if (!_currMaxLevel) return false;
        _currMaxLevel--;
        goto ml_next_node;
       };      
    }; // bool find(const KeyType& searchKey,const Node*& node) 
   private:
    ulong _currMaxLevel;
    const Node* const * _jump;
    const Node* _currNode;
  }; // class Surfer  


  class SurferWithMemory
  {
   public:
    SurferWithMemory() {};
    SurferWithMemory(const CNSHVoidSkList& sl) { reset(sl); };
    ~SurferWithMemory() {};
    void reset(const CNSHVoidSkList& sl) 
    {
     CALL("SurferWithMemory::reset(const CNSHVoidSkList& sl)");
     ASSERT(sl.checkIntegrity());
     _currMaxLevel = sl._currMaxLevel;
     const Node* const * top = sl._topIndex;
     _currNode = *top;
     ASSERT(!Node::isHighestJump(_currNode));
     if (_currNode)
      {	
	for (ulong lv = 1UL; lv <= _currMaxLevel; ++lv)
	  _jump[lv] = top[lv];
      };
     ASSERT(checkIntegrity());
    }; // void reset(const CNSHVoidSkList& sl) 

    const Node* currNode() const { return _currNode; };
    const Node* const * currNodeAddr() const { return &_currNode; }; 

    bool next()
    {
     CALL("SurferWithMemory::next()");
     ASSERT_IN(checkIntegrity(),"START");
     _currNode = _currNode->next();
     ASSERT_IN(checkIntegrity(),"END");
     return (_currNode != 0);
    }; // bool next()

    bool find(const KeyType searchKey)
    {
      CALL("SurferWithMemory::find(const KeyType& searchKey)");
      ASSERT(checkIntegrity());
      if (_currNode->key() <= searchKey) 
	{
	  return _currNode->key() == searchKey;
	};

      const Node* node;
      const Node* nextNode;
      ulong lv = _currMaxLevel;
      // operate within _jump while it is possible
      if (lv)
	{
	  node = _jump[lv];
	  ASSERT(!Node::isHighestJump(node));
	  ASSERT(node);
	  if (node->key() < searchKey)
	    {
	      --lv;
	      goto check_lv_overjump_exists;
	    };
	  if (node->key() > searchKey)
	    {
	      // no longer can operate within _jump
	      // traverse nodes
	    jump_no_overjumps_yet:
	      nextNode = Node::stripJump(node->jump()[lv]);
	      if (!nextNode)
		{
		  --lv;
		  _currMaxLevel = lv;
		  if (lv) goto jump_no_overjumps_yet;
		  _currNode = node;
		  goto level_zero_no_overjumps_yet;
		};
	      if (nextNode->key() < searchKey)
		{
		  _jump[lv] = nextNode;
		  --lv;
		  if (lv) goto jump_overjump_exists;		  
		  _currNode = node;
		  goto level_zero_overjump_exists;
		};
	      if (nextNode->key() > searchKey)
		{
		  node = nextNode;
		  goto jump_no_overjumps_yet;
		};
	      // nextNode->key() == searchKey
	      _currNode = nextNode;
	      ASSERT(_currNode->key() == searchKey);
	      ASSERT(checkIntegrity());
	      return true;
	    };
	  // node->key() == searchKey
	  _currNode = node;
	  ASSERT(_currNode->key() == searchKey);
	  ASSERT(checkIntegrity());
	  return true;
	}
      else // lv == 0UL
	{
	level_zero_no_overjumps_yet:
	  ASSERT(_currNode);
	  ASSERT(_currNode->key() > searchKey);
	  _currNode = _currNode->next();
	  if ((!_currNode) || (_currNode->key() < searchKey)) 	    
	    return false;
	  ASSERT(_currNode->computeHeight() == 0UL);
	  if (_currNode->key() == searchKey) 
	    {
	      ASSERT(checkIntegrity());
	      return true;
	    };
	  // _currNode->key() > searchKey
	  goto level_zero_no_overjumps_yet;
	};

      // Here we know that there was an overjump.
      // This allows us to avoid some unnecessary tests. 
    check_lv_overjump_exists:
      if (lv)
	{
	  node = _jump[lv];
	  ASSERT(!Node::isHighestJump(node));
	  ASSERT(node);
	  if (node->key() < searchKey)
	    {
	      --lv;
	      goto check_lv_overjump_exists;
	    };
	  if (node->key() > searchKey)
	    {
	      // no longer can operate within _jump
	      // traverse nodes
	    jump_overjump_exists:
	      nextNode = Node::stripJump(node->jump()[lv]);
	      ASSERT(nextNode);
	      if (nextNode->key() < searchKey)
		{
		  _jump[lv] = nextNode;
		  --lv;
		  if (lv) goto jump_overjump_exists;		  
		  _currNode = node;
		  goto level_zero_overjump_exists;
		};
	      if (nextNode->key() > searchKey)
		{
		  node = nextNode;
		  goto jump_overjump_exists;
		};
	      // nextNode->key() == searchKey
	      _currNode = nextNode;
	      ASSERT(_currNode->key() == searchKey);
	      ASSERT(checkIntegrity());
	      return true;
	    };
	  // node->key() == searchKey
	  _currNode = node;
	  ASSERT(_currNode->key() == searchKey);
	  ASSERT(checkIntegrity());
	  return true;
	}
      else // lv == 0UL
	{
	level_zero_overjump_exists:
	  ASSERT(_currNode);
	  ASSERT(_currNode->key() > searchKey);
	  _currNode = _currNode->next();
	  ASSERT(_currNode);
	  if (_currNode->key() < searchKey) 	    
	    return false;
	  ASSERT(_currNode->computeHeight() == 0UL);
	  if (_currNode->key() == searchKey) 
	    {
	      ASSERT(checkIntegrity());
	      return true;
	    };
	  // _currNode->key() > searchKey
	  goto level_zero_overjump_exists;
	};
    }; // bool find(const KeyType searchKey)

    void getTo(const KeyType searchKey)
    {
      CALL("SurferWithMemory::getTo(const KeyType& searchKey)");
      ASSERT(checkIntegrity());
      ASSERT(_currNode->key() >= searchKey);
      if (_currNode->key() == searchKey) 
	{
	  return;
	};

      const Node* node;
      const Node* nextNode;
      ulong lv = _currMaxLevel;
      // operate within _jump while it is possible
      if (lv)
	{
	  node = _jump[lv];
	  ASSERT(!Node::isHighestJump(node));
	  ASSERT(node);
	  if (node->key() < searchKey)
	    {
	      --lv;
	      goto check_lv_overjump_exists;
	    };
	  if (node->key() > searchKey)
	    {
	      // no longer can operate within _jump
	      // traverse nodes
	    jump_no_overjumps_yet:
	      nextNode = Node::stripJump(node->jump()[lv]);
	      if (!nextNode)
		{
		  --lv;
		  _currMaxLevel = lv;
		  if (lv) goto jump_no_overjumps_yet;
		  _currNode = node;
		  goto level_zero_no_overjumps_yet;
		};
	      if (nextNode->key() < searchKey)
		{
		  _jump[lv] = nextNode;
		  --lv;
		  if (lv) goto jump_overjump_exists;		  
		  _currNode = node;
		  goto level_zero_overjump_exists;
		};
	      if (nextNode->key() > searchKey)
		{
		  node = nextNode;
		  goto jump_no_overjumps_yet;
		};
	      // nextNode->key() == searchKey
	      _currNode = nextNode;
	      ASSERT(_currNode->key() == searchKey);
	      ASSERT(checkIntegrity());
	      return;
	    };
	  // node->key() == searchKey
	  _currNode = node;
	  ASSERT(_currNode->key() == searchKey);
	  ASSERT(checkIntegrity());
	  return;
	}
      else // lv == 0UL
	{
	level_zero_no_overjumps_yet:
	  ASSERT(_currNode);
	  ASSERT(_currNode->key() > searchKey);
	  _currNode = _currNode->next();
	  ASSERT(_currNode);
	  ASSERT(_currNode->key() >= searchKey);
	  ASSERT(_currNode->computeHeight() == 0UL);
	  if (_currNode->key() == searchKey) 
	    {
	      ASSERT(checkIntegrity());
	      return;
	    };
	  // _currNode->key() > searchKey
	  goto level_zero_no_overjumps_yet;
	};

      // Here we know that there was an overjump.
      // This allows us to avoid some unnecessary tests. 
    check_lv_overjump_exists:
      if (lv)
	{
	  node = _jump[lv];
	  ASSERT(!Node::isHighestJump(node));
	  ASSERT(node);
	  if (node->key() < searchKey)
	    {
	      --lv;
	      goto check_lv_overjump_exists;
	    };
	  if (node->key() > searchKey)
	    {
	      // no longer can operate within _jump
	      // traverse nodes
	    jump_overjump_exists:
	      nextNode = Node::stripJump(node->jump()[lv]);
	      ASSERT(nextNode);
	      if (nextNode->key() < searchKey)
		{
		  _jump[lv] = nextNode;
		  --lv;
		  if (lv) goto jump_overjump_exists;		  
		  _currNode = node;
		  goto level_zero_overjump_exists;
		};
	      if (nextNode->key() > searchKey)
		{
		  node = nextNode;
		  goto jump_overjump_exists;
		};
	      // nextNode->key() == searchKey
	      _currNode = nextNode;
	      ASSERT(_currNode->key() == searchKey);
	      ASSERT(checkIntegrity());
	      return;
	    };
	  // node->key() == searchKey
	  _currNode = node;
	  ASSERT(_currNode->key() == searchKey);
	  ASSERT(checkIntegrity());
	  return;
	}
      else // lv == 0UL
	{
	level_zero_overjump_exists:
	  ASSERT(_currNode);
	  ASSERT(_currNode->key() > searchKey);
	  _currNode = _currNode->next();
	  ASSERT(_currNode);
	  ASSERT(_currNode->key() >= searchKey);
	  ASSERT(_currNode->computeHeight() == 0UL);
	  if (_currNode->key() == searchKey) 
	    {
	      ASSERT(checkIntegrity());
	      return;
	    };
	  // _currNode->key() > searchKey
	  goto level_zero_overjump_exists;
	};
    }; // void getTo(const KeyType searchKey)

    #ifdef DEBUG_NAMESPACE
     bool checkIntegrity()
     {
      if (!_currNode) return true;
      ulong currNodeHeight = _currNode->computeHeight();
      if (currNodeHeight > _currMaxLevel) 
       {
        DMSG("CNSHVoidSkList<..>::SurferWithMemory::checkIntegrity()> currNodeHeight > _currMaxLevel\n");
        return false;  
       };
      for (ulong lv = 1UL; lv <= _currMaxLevel; ++lv)
	{
	  if (Node::isHighestJump(_jump[lv]))
	    {
	      DMSG("CNSHVoidSkList<..>::SurferWithMemory::checkIntegrity()> Node::isHighestJump(_jump[lv])\n");
	      return false;
	    };
	  if (!_jump[lv])
	    {
	      DMSG("CNSHVoidSkList<..>::SurferWithMemory::checkIntegrity()> !_jump[lv]\n");
	      return false;
	    };
	  if (lv <= currNodeHeight)
	    {
	      if (_jump[lv]->key() < _currNode->key())
		{
		  DMSG("CNSHVoidSkList<..>::SurferWithMemory::checkIntegrity()> _jump[lv]->key() < _currNode->key()\n");
		  return false;
		};
	    }
	  else
	    {
	      if (lv > 1UL)
		{
		  if (_jump[lv]->key() > _jump[lv-1]->key())
		    {
		      DMSG("CNSHVoidSkList<..>::SurferWithMemory::checkIntegrity()> _jump[lv]->key() > _jump[lv-1]->key()\n");
		      return false;
		    };
		};
	    };
	};
      return true;
     };
    #endif    

   private:
    ulong _currMaxLevel;
    const Node* _jump[28]; // is there a list longer than 2^28 ?
    const Node* _currNode;
  }; // class SurferWithMemory  



 public:
  CNSHVoidSkList() 
   {
    CALL("constructor CNSHVoidSkList()"); 
    init();
   };

  ~CNSHVoidSkList() 
   {
    CALL("destructor ~CNSHVoidSkList()");
    destroy();
   }; // ~CNSHVoidSkList() 

  void init() 
  {
   CALL("init()");
   _currMaxLevel = 0;
   _sizeOfTopIndex = 1;
   #ifdef DEBUG_ALLOC_OBJ_TYPE
     ALWAYS(_headerClassDesc.registerAllocated(_sizeOfTopIndex * sizeof(Node*))); 
     _topIndex = (Node**)Alloc::allocate(_sizeOfTopIndex * sizeof(Node*),&_headerClassDesc); 
    #else
     _topIndex = (Node**)Alloc::allocate(_sizeOfTopIndex * sizeof(Node*)); 
    #endif
    _topIndex[0] = (Node*)0;
   _insertCount = 0;
   _size = 0;
   ASSERT(checkIntegrity());
  }; // void init() 

  void destroy();


  void* operator new(size_t) 
  {
   CALL("operator new(size_t)"); 
   #ifdef DEBUG_ALLOC_OBJ_TYPE
    ALWAYS(_classDesc.registerAllocated(sizeof(CNSHVoidSkList)));
    return Alloc::allocate(sizeof(CNSHVoidSkList),&_classDesc);
   #else
    return Alloc::allocate(sizeof(CNSHVoidSkList));
   #endif 
  };
  void operator delete(void* obj) 
  {
   CALL("operator delete(void* obj)"); 
   #ifdef DEBUG_ALLOC_OBJ_TYPE
    ALWAYS(_classDesc.registerDeallocated(sizeof(CNSHVoidSkList)));
    Alloc::deallocate(obj,sizeof(CNSHVoidSkList),&_classDesc);
   #else
    Alloc::deallocate(obj,sizeof(CNSHVoidSkList));
   #endif 
  };
  static ulong minSize() { return sizeof(CNSHVoidSkList); }; // has nothing to do with size()
  static ulong maxSize() { return sizeof(CNSHVoidSkList); }; // has nothing to do with size()
  static ulong headerMinSize() { return sizeof(Node*); }; // has nothing to do with size()
  static ulong headerMaxSize() { return ULONG_MAX; }; 


  void reset() 
  {
   CALL("reset()");
   destroy();
   init();
  };

  operator bool() const 
  {
   CALL("operator bool() const"); 
   return _topIndex[0] != 0; 
  }; 
  const Node* first() const 
  {
   CALL("first() const"); 
   return _topIndex[0]; 
  };
  Node* first() 
  {
   CALL("first()");
   return _topIndex[0]; 
  };

  ulong size() const { return _size; }; 

  ulong count() const 
  {   
   ulong res = 0; 
   for (const Node* n = first(); n; n = n->next()) 
    { 
     res++;
    };
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
   bool checkIntegrity() const
   {
    if (!_topIndex) return false; 
    
    #ifdef DEBUG_ALLOC
     bool res = true;
     ObjDesc* objDesc = GlobAlloc::hostObj(static_cast<void*>(_topIndex));          
     if (!objDesc) 
      { 
       DMSG("CNSHVoidSkList<..>::checkIntegrity()> Null pointer to object descriptor for *_topIndex.\n");  
       return false; 
      };     
     #ifdef DEBUG_ALLOC_OBJ_SIZE
      if (objDesc->size !=  _sizeOfTopIndex * sizeof(Node*)) 
       {
        DMSG("CNSHVoidSkList<..>::checkIntegrity()> Wrong size of *_topIndex.\n");
        res = false;
       };
     #endif
     #ifdef DEBUG_ALLOC_OBJ_STATUS
      if (objDesc->status != ObjDesc::Persistent) 
       {
        DMSG("CNSHVoidSkList<..>::checkIntegrity()> objDesc->status != ObjDesc::Persistent for *_topIndex.\n");
        res = false; 
       };
     #endif      
     #ifdef DEBUG_ALLOC_OBJ_TYPE
      if (objDesc->classDesc != &_headerClassDesc) 
       {
        DMSG("CNSHVoidSkList<..>::checkIntegrity()> Wrong objDesc->classDesc with *_topIndex.\n");
        res = false;
       };
     #endif
     if (!res) return false;
    #endif

     for (ulong lv = 1UL; lv <= _currMaxLevel; ++lv)
       {
	 if (Node::isHighestJump(_topIndex[lv]))
	   {
	     DMSG("CNSHVoidSkList<..>::checkIntegrity()> Node::isHighestJump(_topIndex[lv])\n");
	     return false;
	   };
	 if (!_topIndex[lv])
	   {
	     DMSG("CNSHVoidSkList<..>::checkIntegrity()> !_topIndex[lv]\n");
	     return false;
	   };
	 if (_topIndex[lv]->key() > _topIndex[lv-1]->key())
	   {
	     DMSG("CNSHVoidSkList<..>::checkIntegrity()> _topIndex[lv]->key() > _topIndex[lv-1]->key()\n");
	     return false;
	   };
       };
     if (count() != size())
       {
	 DMSG("CNSHVoidSkList<..>::checkIntegrity()> count() != size()\n");
	 return false;
       };
    return true; 
   }; // bool checkIntegrity() const
  #endif

  ostream& output(ostream& ostr) const
  {
   for (const Node* node = first(); node; node = node->next())
    {
     node->output(ostr,_currMaxLevel) << "\n";
    };
   return ostr;
  };
  ostream& outputKeys(ostream& ostr) const
  {
   for (const Node* node = first(); node; node = node->next())
    {
     ostr << node->key() << "\n";
    };
   return ostr;
  };

 private:
  CNSHVoidSkList(const CNSHVoidSkList& sl)
  {
   CALL("constructor CNSHVoidSkList(const CNSHVoidSkList& sl)");
   ICP("ICP0");
  };
  
  CNSHVoidSkList& operator=(const CNSHVoidSkList& sl)
  {
   CALL("operator=(const CNSHVoidSkList& sl)");
   ICP("ICP0");
   return *this;
  };

  ulong randomHeight() const
  { 
   ulong res = 0;
   ulong tmp = _insertCount;
   while (!(tmp % Base))
    { 
     res++;
     tmp = tmp/Base;
    };
   return res;
  }; // randomHeight() 
 private:
  ulong _sizeOfTopIndex;
  Node** _topIndex;
  ulong _currMaxLevel;
  ulong _insertCount;
  ulong _size;
  #ifdef DEBUG_ALLOC_OBJ_TYPE
   static ClassDesc _classDesc;
   static ClassDesc _headerClassDesc;
  #endif
 friend class Surfer; 
 friend class SurferWithMemory; 
}; // template <class Alloc,class KeyType,ulong Base,class InstanceId> class CNSHVoidSkList

}; // namespace BK


//============================================================================
#undef DEBUG_NAMESPACE
#ifdef DEBUG_COMPACT_NODE_SMALL_HEADER_SKIP_LIST
 #define DEBUG_NAMESPACE "CNSHVoidSkList<class Alloc,class KeyType,ulong Base,class InstanceId>"
#endif
#include "debugMacros.hpp"
//============================================================================

namespace BK 
{
template <class Alloc,class KeyType,ulong Base,class InstanceId>
inline
bool CNSHVoidSkList<Alloc,KeyType,Base,InstanceId>::insert(const KeyType& searchKey)  
{
 CALL("insert(const KeyType& searchKey)");
 ASSERT_IN(checkIntegrity(),"START");

 Node* update[32];

 Node* currNode = Node::node(_topIndex);      // tricky!
 ulong currLevel = _currMaxLevel;
 Node* nextNode = currNode->jump()[currLevel];  // tricky!
 ASSERT(!Node::isHighestJump(nextNode));

 KeyType nodeKey;
 check_next_node:
  if (nextNode)
   {
     ASSERT(nextNode->computeHeight() <= currLevel);
    nodeKey = nextNode->key();
    if (nodeKey > searchKey) 
     {
      currNode = nextNode;
      nextNode = Node::stripJump(nextNode->jump()[currLevel]);        
      goto check_next_node;
     };
    // nodeKey <= searchKey
    if (nodeKey == searchKey) 
      { 
	ASSERT(checkIntegrity()); 
	return false; 
      };
    // nodeKey < searchKey, "if (nextNode)" is not needed anymore
    update[currLevel] = currNode;
    while (currLevel) 
     { 
      currLevel--;
      nextNode = currNode->jump()[currLevel]; 

      ASSERT(!Node::isHighestJump(nextNode));
      nodeKey = nextNode->key();
      while (nodeKey > searchKey)
       {
	ASSERT(nextNode->computeHeight() == currLevel);
	currNode = nextNode;
        nextNode = Node::stripJump(nextNode->jump()[currLevel]);
        nodeKey = nextNode->key();
       };
      // nodeKey <= searchKey
      if (nodeKey == searchKey) { ASSERT(checkIntegrity()); return false; };
      // nodeKey < searchKey, "if (nextNode)" is not needed anymore
      update[currLevel] = currNode;
     };
    ASSERT(nextNode->key() < searchKey);
   }
  else // !nextNode
   {
    update[currLevel] = currNode;
    if (currLevel) 
     {
      currLevel--;
      nextNode = currNode->jump()[currLevel];
      ASSERT(!Node::isHighestJump(nextNode));
      goto check_next_node;
     };
   };
 
  ASSERT(!currLevel);    

  // insert new node
  ASSERT(_insertCount < ULONG_MAX);
  _insertCount++;  
  _size++;
  currLevel = randomHeight();

  currNode = new (currLevel) Node(searchKey); 
  Node** nodeJump = currNode->jump();  
  Node** jumpAddr; 
  Node* jump;

  if (currLevel > _currMaxLevel)
   {
    // _topIndex must be expanded 

    #ifdef DEBUG_ALLOC_OBJ_TYPE
     ALWAYS(_headerClassDesc.registerAllocated((currLevel + 1) * sizeof(Node*)));
     Node** newTopIndex = (Node**)Alloc::allocate((currLevel + 1) * sizeof(Node*),&_headerClassDesc);
    #else
     Node** newTopIndex = (Node**)Alloc::allocate((currLevel + 1) * sizeof(Node*));
    #endif
	
    ulong lev;
    for (lev = 0UL; lev <= _currMaxLevel; ++lev)
      newTopIndex[lev] = _topIndex[lev];
    
   
    #ifdef DEBUG_ALLOC_OBJ_TYPE
     ALWAYS(_headerClassDesc.registerDeallocated(_sizeOfTopIndex * sizeof(Node*)));
     Alloc::deallocate(_topIndex,_sizeOfTopIndex * sizeof(Node*),&_headerClassDesc);
    #else
     Alloc::deallocate(_topIndex,_sizeOfTopIndex * sizeof(Node*));   
    #endif
      

    Node* pseudonode = Node::node(_topIndex);      

    for (lev = 0UL; lev <= _currMaxLevel; ++lev)
      {
        ASSERT(!Node::isHighestJump(update[lev]));
	if (update[lev] == pseudonode)      
	  {
	    ASSERT((!newTopIndex[lev]) || (newTopIndex[lev]->key() < searchKey));
	    nodeJump[lev] = newTopIndex[lev];
	    newTopIndex[lev] = currNode;
	  }
	else
	  {
	    jumpAddr = update[lev]->jump() + lev;
	    jump = *jumpAddr;	
	    if (Node::isHighestJump(jump))
	      {
		ASSERT(update[lev]->computeHeight() == lev);
		*jumpAddr = Node::markAsHighestJump(currNode);	
		nodeJump[lev] = Node::stripJump(jump);
	      }
	    else
	      {
		ASSERT(update[lev]->computeHeight() > lev);
		*jumpAddr = currNode;	
		nodeJump[lev] = jump;
	      };
	  };
      }; // for (lev = 0UL;...
    
    _sizeOfTopIndex = currLevel + 1;
    _topIndex = newTopIndex;

    ++_currMaxLevel;
    while (_currMaxLevel < currLevel)
     {
      _topIndex[_currMaxLevel] = currNode;
      nodeJump[_currMaxLevel] = 0;
      ++_currMaxLevel;
     };
    _topIndex[_currMaxLevel] = currNode;
    nodeJump[_currMaxLevel] = Node::markAsHighestJump(static_cast<Node*>(0));
    ASSERT(currNode->computeHeight() == _currMaxLevel);
   }
  else // currLevel <= _currMaxLevel
   {       
     ulong lev;
     for (lev = 0UL; lev < currLevel; ++lev)
       {
	 ASSERT(!Node::isHighestJump(update[lev]));
	 jumpAddr = update[lev]->jump() + lev;
	 jump = *jumpAddr;		
	 if (Node::isHighestJump(jump))
	   {
	     ASSERT(update[lev]->computeHeight() == lev);
	     *jumpAddr = Node::markAsHighestJump(currNode);
	     nodeJump[lev] = Node::stripJump(jump);
	   }
	 else
	   {
	     ASSERT(update[lev]->computeHeight() > lev);
	     *jumpAddr = currNode;
	     nodeJump[lev] = jump;
	   };
       };
     
     ASSERT(!Node::isHighestJump(update[lev]));
     jumpAddr = update[lev]->jump() + lev;
     jump = *jumpAddr;		
     if (Node::isHighestJump(jump))
       {
	 ASSERT(update[lev]->computeHeight() == lev);
	 *jumpAddr = Node::markAsHighestJump(currNode);
	 nodeJump[lev] = jump;
       }
     else
       {
	 ASSERT(update[lev]->computeHeight() > lev);
	 *jumpAddr = currNode;
	 nodeJump[lev] = Node::markAsHighestJump(jump);
       };
     ASSERT(currNode->computeHeight() == currLevel);
   }; 
  ASSERT_IN(checkIntegrity(),"END");
  return true;
}; // Node* CNSHVoidSkList<..>::insert(const KeyType& searchKey) 



template <class Alloc,class KeyType,ulong Base,class InstanceId>
inline
bool CNSHVoidSkList<Alloc,KeyType,Base,InstanceId>::remove(const KeyType& searchKey) 
{
 CALL("remove(const KeyType& searchKey)");
 ASSERT(checkIntegrity());
 Node** jumpAddr = _topIndex;
 bool highest = false;
 ulong currLevel = _currMaxLevel; 
 Node* node;
 next_node:   
  node = jumpAddr[currLevel];
  ASSERT(highest == Node::isHighestJump(node));
  if (highest) node = Node::stripJump(node);
  
  if (node)
   {
    ASSERT(node->computeHeight() >= currLevel);  
    if (node->key() == searchKey)
     { 
       if (highest)
	 {
	   jumpAddr[currLevel] = Node::markAsHighestJump(node->jump()[currLevel]);
	 }
       else
	 {
	   jumpAddr[currLevel] = Node::stripJump(node->jump()[currLevel]);
	 };
      if (currLevel) 
	{ 
	  currLevel--;
	  highest = false;
	  goto next_node; 
	};

      while ((!(_topIndex[_currMaxLevel])) && _currMaxLevel)
	_currMaxLevel--; 


      delete node;
      _size--;
      ASSERT(checkIntegrity());
      ASSERT(!contains(searchKey));
      return true;
     };
    if (node->key() > searchKey) 
     {
      jumpAddr = node->jump();
      highest = true;
      goto next_node;
     };
    // node->key() < searchKey, try smaller level
    if (currLevel) 
      { 
	currLevel--;	
	highest = false;
	goto next_node; 
      }; 
    ASSERT(checkIntegrity());    
    ASSERT(!contains(searchKey)); 
    return false;
   };
    
  // !node
  if (currLevel) 
    { 
      currLevel--;      
      highest = false;
      goto next_node; 
    };
  ASSERT(checkIntegrity());
  ASSERT(!contains(searchKey));
  return false;
}; // bool CNSHVoidSkList<..>::remove(const KeyType& searchKey)  


template <class Alloc,class KeyType,ulong Base,class InstanceId>
void CNSHVoidSkList<Alloc,KeyType,Base,InstanceId>::destroy()
{
 CALL("destroy()");
 ASSERT(checkIntegrity());
 if (DestructionMode::isThorough())
   {
     Node* node = _topIndex[0];
     Node* tmp;
     while (node) 
       { 
	 tmp = node;   
	 node = node->next(); 
	 delete tmp;
       };  
#ifdef DEBUG_ALLOC_OBJ_TYPE
     ALWAYS(_headerClassDesc.registerDeallocated(_sizeOfTopIndex * sizeof(Node*)));
     Alloc::deallocate(_topIndex,_sizeOfTopIndex * sizeof(Node*),&_headerClassDesc);
     
#else 
     Alloc::deallocate(_topIndex,_sizeOfTopIndex * sizeof(Node*));
#endif
   };
}; // void CNSHVoidSkList<Alloc,KeyType,Base,InstanceId>::destroy()



}; // namespace BK

//======================================================================
#endif
