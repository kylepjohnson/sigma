//
// File:         SmallHeaderSkipList.hpp
// Description:  Generic skip lists with small (flexible size) headers.
// Created:      Apr 23, 2001, Alexandre Riazanov, riazanov@cs.man.ac.uk
// Note:         More memory efficient than skip lists with fixed size header,  
//               although some operations are slightly slower.  
// Note:         A specialised version for ValType = void is to be implemented. 
// Revised:      Jun 02, 2002, Alexandre Riazanov, riazanov@cs.man.ac.uk
//               _topIndex no longer shrinks on removal of an element.
// Revised:      Dec 12, 2002, Alexandre Riazanov, riazanov@cs.man.ac.uk
//               I had to slightly mess it up in order to compile by VC++ 6.0
//============================================================================
#ifndef SMALL_HEADER_SKIP_LIST_H
//=============================================================================
#define SMALL_HEADER_SKIP_LIST_H 
#include <climits>
#include "jargon.hpp"
#include "BlodKorvDebugFlags.hpp"
#ifdef DEBUG_ALLOC_OBJ_TYPE
 #include "ClassDesc.hpp"       
#endif
#include "DestructionMode.hpp"
//============================================================================
#undef DEBUG_NAMESPACE
#ifdef DEBUG_SMALL_HEADER_SKIP_LIST
 #define DEBUG_NAMESPACE "SHSkList<class Alloc,class ValType,class KeyType,ulong Base,class InstanceId>"
#endif
#include "debugMacros.hpp"
//============================================================================

namespace BK 
{
template <class Alloc,class ValType,class KeyType,ulong Base,class InstanceId>
class SHSkList
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
    Node* next() { return *(jump()); };
    const Node* next() const { return *(jump()); };    
    static ulong minSize() { return sizeof(Node) + sizeof(Node*); }; 
    static ulong maxSize() { return ULONG_MAX; }; 


   private:
    int level() const { return _level; };

   private: 
	// the following functions are public because Surfer and SHSkList use them,
	// "friend" can't be used due to VC++
    Node** jump() { return static_cast<Node**>(static_cast<void *>(this + 1)); }; 
    const Node* const * jump() const { return static_cast<const Node* const *>(static_cast<const void *>(this + 1)); };
    Node** jump(int lev) { return jump() + lev; }; 
    const Node* const * jump(int lev) const { return jump() + lev; };
   
    Node(int l,const KeyType& k) : _key(k), _level(l) {};
    Node* next(int lev) { return jump()[lev]; }; 
    const Node* next(int lev) const { return jump()[lev]; };
      
    static const Node* node(const Node * const * jmp)  // This is a hack! Must be removed.
    {
     return (static_cast<const Node*>(static_cast<const void*>(jmp))) - 1;
    };

    static Node* node(Node ** jmp)           // This is a hack! Must be removed.
    {
      return (static_cast<Node*>(static_cast<void*>(jmp))) - 1;
    };
  
    void* operator new(size_t size,int lv) 
    {    
     CALL("operator new(size_t size,int lv)");  
     #ifdef DEBUG_ALLOC_OBJ_TYPE
      ALWAYS(_classDesc.registerAllocated(size + (lv+1)*sizeof(Node*)));
      return Alloc::allocate(size + (lv+1)*sizeof(Node*),&_classDesc);
     #else
      return Alloc::allocate(size + (lv+1)*sizeof(Node*));
     #endif
    };
    void operator delete(void* obj)
    {
     CALL("operator delete(void* obj)");
     #ifdef DEBUG_ALLOC_OBJ_TYPE
      ALWAYS(_classDesc.registerDeallocated(sizeof(Node) + ((static_cast<Node*>(obj))->level() + 1)*sizeof(Node*)));
      Alloc::deallocate(obj,sizeof(Node) + ((static_cast<Node*>(obj))->level() + 1)*sizeof(Node*),&_classDesc);
     #else
      Alloc::deallocate(obj,sizeof(Node) + ((static_cast<Node*>(obj))->level() + 1)*sizeof(Node*));
     #endif
    };   
    
    void operator delete(void* obj,int lv)
    {
     CALL("operator delete(void* obj,int lv)");
     #ifdef DEBUG_ALLOC_OBJ_TYPE
      ALWAYS(_classDesc.registerDeallocated(sizeof(Node) + (lv + 1)*sizeof(Node*)));
      Alloc::deallocate(obj,sizeof(Node) + (lv + 1)*sizeof(Node*),&_classDesc);
     #else
      Alloc::deallocate(obj,sizeof(Node) + (lv + 1)*sizeof(Node*));
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
         DMSG("SHSkList<..>::Node::checkObj()> Null pointer to object descriptor.\n");  
         return false; 
        }; 
       bool res = true;   
       #ifdef DEBUG_ALLOC_OBJ_SIZE
        if (objDesc->size !=  (sizeof(Node) + (level() + 1)*sizeof(Node*)))
         {
          DMSG("SHSkList<..>::Node::checkObj()> objDesc->size != (sizeof(Node) + (level() + 1)*sizeof(Node*)) \n");
          res = false;
         };
       #endif
       #ifdef DEBUG_ALLOC_OBJ_STATUS
        if (objDesc->status != ObjDesc::Persistent) 
         {
          DMSG("SHSkList<..>::Node::checkObj()> objDesc->status != ObjDesc::Persistent\n");
          res = false;
         };
       #endif
       #ifdef DEBUG_ALLOC_OBJ_TYPE
        if (objDesc->classDesc != &_classDesc) 
         {
          DMSG("SHSkList<..>::Node::checkObj()> objDesc->classDesc != &_classDesc\n");
          res = false;
         };
       #endif
       if (!res) DOP(DMSG("SHSkList<..>::Node::checkObj()> host object ") << *objDesc << "\n"); 
       return res;
      #endif
     };
    #endif

   private: 
    KeyType _key;    
    int _level;
    ValType _value; 
    #ifdef DEBUG_ALLOC_OBJ_TYPE
     static ClassDesc _classDesc;
    #endif   
   friend class SHSkList<Alloc,ValType,KeyType,Base,InstanceId>;
   friend class Surfer;
   friend class SurferWithMemory;
   friend class Destruction;
  }; // class Node

  class Surfer
  {
   public:
    Surfer() {};
    Surfer(const SHSkList& sl) { reset(sl); };
    ~Surfer() {};
    void reset(const SHSkList& sl) 
    {
     CALL("Surfer::reset(const SHSkList& sl)");
     ASSERT(sl.checkIntegrity());
     _currMaxLevel = sl._currMaxLevel;
     _jump = sl._topIndex;
     _currNode = _jump[0];
    };
    const Node* currNode() const { return _currNode; };
    const Node* const * currNodeAddr() const { return &_currNode; }; 

    bool next()
    {
     CALL("next()");
     _currNode = _currNode->next();
     return (bool)_currNode;
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
      node = _jump[_currMaxLevel];   
      if (node) 
       {
    	ASSERT(node->level() == _currMaxLevel); 
	if (node->key() < searchKey)
	 {
          if (!_currMaxLevel) return false;
          int currLevel = _currMaxLevel - 1;
          const Node* const * jump_ = _jump;        
	  next_node:
	   node = jump_[currLevel];
           ASSERT(node->level() >= currLevel);
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
    int _currMaxLevel;
    const Node* const * _jump;
    const Node* _currNode;
  }; // class Surfer  
 

  class SurferWithMemory
  {
  public:
    SurferWithMemory() {};
    SurferWithMemory(const SHSkList& sl) { reset(sl); };
    ~SurferWithMemory() {};
    void reset(const SHSkList& sl) 
    {
      CALL("SurferWithMemory::reset(const SHSkList& sl)");
      ASSERT(sl.checkIntegrity());
      _currMaxLevel = sl._currMaxLevel;
      _currNode = sl._topIndex[0];
      if (_currNode)
	{
	  for (int lv = _currNode->level() + 1; lv <= _currMaxLevel; lv++)
	    _jump[lv] = sl._topIndex[lv]; 
	};
      ASSERT(checkIntegrity());
    };
    const Node* currNode() const { return _currNode; };
    const Node* const * currNodeAddr() const { return &_currNode; }; 

    bool next()
    {
      CALL("next()");
      ASSERT_IN(checkIntegrity(),"START");
      const Node* nextNode = _currNode->next();
      if (nextNode) 
	{
	  int lv = nextNode->level() + 1;
	  int currNodeLevel = _currNode->level();
	  if (lv <= currNodeLevel)
	    {
	      const Node* const * currNodeJump = _currNode->jump();      
	    next_level:
	      if (currNodeJump[lv])
		{
		  _jump[lv] = currNodeJump[lv];
		  lv++;
		  if (lv <= currNodeLevel) goto next_level;
		}
	      else
		{
		  _currMaxLevel = lv - 1;
		};
	    };
	  _currNode = nextNode;
	  ASSERT_IN(checkIntegrity(),"SUCCESS");
	  return true;
	} 
      else
	{
	  ASSERT_IN(checkIntegrity(),"FAILURE");
	  return false;
	};
    }; // bool next()

    bool find(const KeyType searchKey)
    {
      CALL("SurferWithMemory::find(const KeyType& searchKey)");
      ASSERT(checkIntegrity());
      if (_currNode->key() == searchKey) 
	{
	  return true;
	};
      const Node* node;
      int currLevel = _currMaxLevel;
      int currNodeLevel = _currNode->level();
      bool overJump = false;
      const Node* const * currJump;  
  
    check_level:
      ASSERT((!overJump) || (currLevel < _currMaxLevel));
      ASSERT(overJump || (currLevel == _currMaxLevel));
      if (currLevel == currNodeLevel)
	{  
	  if (_currNode->key() < searchKey) 
	    {
	      // _currNode is not changed
	      ASSERT(checkIntegrity());
	      return false;
	    };
	  currJump = _currNode->jump();
	  node = currJump[currLevel];
	  if (overJump) 
	    {
	      goto node_jump_with_overjump;
	    };
	  goto node_jump_no_overjump;
	};

      ASSERT(currLevel > currNodeLevel);

      node = _jump[currLevel];

      if (node->key() < searchKey) // too long jump
	{
	  overJump = true;
	  ASSERT(currLevel);
	  currLevel--;
	  goto check_level;
	};
      // node->key() >= searchKey
      if (node->key() == searchKey) 
	{
	  _currNode = node;
	  ASSERT(checkIntegrity());
	  return true;
	};
      // node->key() > searchKey
      ASSERT(node->key() < _currNode->key()); // closer to the goal than _currNode

      currJump = node->jump();
      node = currJump[currLevel];  
      if (overJump) 
	{
	  ASSERT_IN(node,"R200");
	  ASSERT(currLevel < _currMaxLevel);
	  goto node_jump_with_overjump;
	};
      ASSERT_IN(currLevel == _currMaxLevel,"E100");
      goto node_jump_no_overjump;

    node_jump_no_overjump:
      ASSERT_IN(currLevel == _currMaxLevel,"E200");
      if (node)
	{
	  if (node->key() < searchKey) // too long jump
	    { 
	      // this is an over-jump          
	      if (currLevel)
		{
		  _jump[currLevel] = node; // for the future calls  
		  currLevel--;
		  node = currJump[currLevel];
		  ASSERT(node);
		  goto node_jump_with_overjump;
		}
	      else 
		{ 
		  _currNode = node;     
		  ASSERT(checkIntegrity());      
		  return false; 
		};
	    };
	  // node->key() >= searchKey
	  if (node->key() == searchKey) 
	    {
	      _currNode = node;  
	      ASSERT(checkIntegrity()); 
	      return true;
	    };

	  // node->key() > searchKey, proceed with the same level 
	
	  currJump = node->jump();
	  node = currJump[currLevel];
	  ASSERT_IN(currLevel == _currMaxLevel,"E300");
	  ASSERT_IN((!node) || (node->level() == currLevel),"X150");
	  goto node_jump_no_overjump;
	}
      else // !node
	{
	  if (currLevel)
	    {
	      currLevel--;
	      _currMaxLevel = currLevel;
	      node = currJump[currLevel];
	      ASSERT_IN((!node) || (node->level() == currLevel),"X200");
	      goto node_jump_no_overjump;
	    }
	  else 
	    {
	      _currNode = (const Node*)0;
	      ASSERT(checkIntegrity());
	      return false;  
	    };
	};
      
    node_jump_with_overjump:
      if (node->key() < searchKey) // too long jump
	{
	  if (currLevel)
	    {
	      _jump[currLevel] = node; // for the future calls      
	      currLevel--;
	      node = currJump[currLevel];
	      goto node_jump_with_overjump;
	    }
	  else 
	    {
	      _currNode = node;
	      ASSERT(checkIntegrity());
	      return false; 
	    };
	};
      // node->key() >= searchKey
      if (node->key() == searchKey) 
	{
	  _currNode = node;
	  ASSERT(checkIntegrity());
	  return true;
	};
      // node->key() > searchKey proceed with the same level
      ASSERT_IN(node->level() == currLevel,"X300");
      currJump = node->jump();
      node = currJump[currLevel];
      ASSERT_IN(node->level() >= currLevel,"X350");
      goto node_jump_with_overjump;    
    }; // bool find(const KeyType searchKey)

    void getTo(const KeyType searchKey)
    {
      CALL("SurferWithMemory::getTo(const KeyType& searchKey)");
      ASSERT(checkIntegrity());
      if (_currNode->key() == searchKey) 
	{
	  ASSERT(checkIntegrity());
	  return;
	};
      const Node* node;
      int currLevel = _currMaxLevel;
      int currNodeLevel = _currNode->level();
      bool overJump = false;
      const Node* const * currJump;      
      
    check_level:
      ASSERT((!overJump) || (currLevel < _currMaxLevel));
      ASSERT(overJump || (currLevel == _currMaxLevel));
      if (currLevel == currNodeLevel)
	{  
	  ASSERT(_currNode->key() >= searchKey);
	  currJump = _currNode->jump();
	  node = currJump[currLevel];
	  if (overJump) 
	    {
	      goto node_jump_with_overjump;
	    };
	  goto node_jump_no_overjump;
	};

      ASSERT(currLevel > currNodeLevel);

      node = _jump[currLevel];  

      if (node->key() < searchKey) // too long jump
	{
	  overJump = true;
	  ASSERT(currLevel);
	  currLevel--;
	  goto check_level;
	};
      // node->key() >= searchKey
      if (node->key() == searchKey) 
	{
	  _currNode = node;
	  ASSERT(checkIntegrity());
	  return;
	};
      // node->key() > searchKey
      ASSERT(node->key() < _currNode->key()); // closer to the goal than _currNode

      currJump = node->jump();
      node = currJump[currLevel];  
      if (overJump) 
	{
	  ASSERT_IN(node,"R200");
	  ASSERT(currLevel < _currMaxLevel);
	  goto node_jump_with_overjump;
	};
      ASSERT_IN(currLevel == _currMaxLevel,"E100");
      goto node_jump_no_overjump;

    node_jump_no_overjump:
      ASSERT_IN(currLevel == _currMaxLevel,"E200");
      if (node)
	{
	  if (node->key() < searchKey) // too long jump
	    { 
	      // this is an over-jump  
	      ASSERT(currLevel);  
	      _jump[currLevel] = node; // for the future calls  
	      currLevel--;
	      node = currJump[currLevel];
	      ASSERT(node);
	      goto node_jump_with_overjump;
	    };
	  // node->key() >= searchKey
	  if (node->key() == searchKey) 
	    {
	      _currNode = node;  
	      ASSERT(checkIntegrity()); 
	      return;
	    };

	  // node->key() > searchKey, proceed with the same level 
	
	  currJump = node->jump();
	  node = currJump[currLevel];
	  ASSERT_IN(currLevel == _currMaxLevel,"E300");
	  ASSERT_IN((!node) || (node->level() == currLevel),"X150");
	  goto node_jump_no_overjump;

	}
      else // !node
	{
	  ASSERT(currLevel);
	  currLevel--;
	  _currMaxLevel = currLevel;
	  node = currJump[currLevel];
	  ASSERT_IN((!node) || (node->level() == currLevel),"X200");
	  goto node_jump_no_overjump;
	};
      
    node_jump_with_overjump:
      ASSERT_IN(node,"R300");
      if (node->key() < searchKey) // too long jump
	{
	  ASSERT(currLevel);
	  _jump[currLevel] = node; // for the future calls      
	  currLevel--;
	  node = currJump[currLevel];
	  ASSERT_IN(node,"R400");
	  goto node_jump_with_overjump;
	};
      // node->key() >= searchKey
      if (node->key() == searchKey) 
	{
	  _currNode = node;
	  ASSERT(checkIntegrity());
	  return;
	};
      // node->key() > searchKey proceed with the same level
      ASSERT_IN(node->level() == currLevel,"X300");
      currJump = node->jump();
      node = currJump[currLevel];
      ASSERT_IN(node->level() >= currLevel,"X350");
      goto node_jump_with_overjump;     
    }; // void getTo(const KeyType searchKey)



#ifdef DEBUG_NAMESPACE
     bool checkIntegrity()
     {
      if (!_currNode) return true;
      if (_currNode->level() > _currMaxLevel) 
       {
        DMSG("SHSkList<..>::SurferWithMemory::checkIntegrity()> _currNode->level() > _currMaxLevel\n");
        return false;  
       };
      int prevLev = -1; 
      for (int lv = _currNode->level() + 1; lv <= _currMaxLevel; lv++)
       {
	if (!_jump[lv])
	 { 
	  DMSG("SHSkList<..>::SurferWithMemory::checkIntegrity()> !_jump[lv], lv == ") << lv << "\n";
          return false;
         };
        if (_jump[lv]->key() >= _currNode->key())
	 {
          DMSG("SHSkList<..>::SurferWithMemory::checkIntegrity()> _jump[lv]->key() >= _currNode->key(), lv == ") << lv << "\n";
          return false;
         };        
        if ((prevLev != -1) && (_jump[prevLev]->key() < _jump[lv]->key()))
	 { 
          DMSG("SHSkList<..>::SurferWithMemory::checkIntegrity()> _jump[prevLev]->key() < _jump[lv]->key(), lv == ") 
           << lv << ", prevLev == " << prevLev << "\n";
          return false;
        
         };
        prevLev = lv; 
       };
      return true;
     };
    #endif    

   private:
    int _currMaxLevel;
    const Node* _jump[28]; // is there a list longer than 2^28 ?
    const Node* _currNode;
  }; // class SurferWithMemory  

  class Destruction
  {
   public:
    Destruction() {};
    Destruction(SHSkList& sl) { reset(sl); }; 
    ~Destruction() {};
    void reset(SHSkList& sl) 
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
    }; // void reset(SHSkList& sl) 

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
  SHSkList() 
   {
    CALL("constructor SHSkList()"); 
    init();
   };

  ~SHSkList() 
   {
    CALL("destructor ~SHSkList()");
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
    ALWAYS(_classDesc.registerAllocated(sizeof(SHSkList)));
    return Alloc::allocate(sizeof(SHSkList),&_classDesc);
   #else
    return Alloc::allocate(sizeof(SHSkList));
   #endif 
  };
  void operator delete(void* obj) 
  {
   CALL("operator delete(void* obj)"); 
   #ifdef DEBUG_ALLOC_OBJ_TYPE
    ALWAYS(_classDesc.registerDeallocated(sizeof(SHSkList)));
    Alloc::deallocate(obj,sizeof(SHSkList),&_classDesc);
   #else
    Alloc::deallocate(obj,sizeof(SHSkList));
   #endif 
  };
  static ulong minSize() { return sizeof(SHSkList); }; // has nothing to do with size()
  static ulong maxSize() { return sizeof(SHSkList); }; // has nothing to do with size()
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
       DMSG("SHSkList<..>::checkIntegrity()> Null pointer to object descriptor for *_topIndex.\n");  
       return false; 
      };     
     #ifdef DEBUG_ALLOC_OBJ_SIZE
      if (objDesc->size !=  _sizeOfTopIndex * sizeof(Node*)) 
       {
        DMSG("SHSkList<..>::checkIntegrity()> Wrong size of *_topIndex.\n");
        res = false;
       };
     #endif
     #ifdef DEBUG_ALLOC_OBJ_STATUS
      if (objDesc->status != ObjDesc::Persistent) 
       {
        DMSG("SHSkList<..>::checkIntegrity()> objDesc->status != ObjDesc::Persistent for *_topIndex.\n");
        res = false; 
       };
     #endif      
     #ifdef DEBUG_ALLOC_OBJ_TYPE
      if (objDesc->classDesc != &_headerClassDesc) 
       {
        DMSG("SHSkList<..>::checkIntegrity()> Wrong objDesc->classDesc with *_topIndex.\n");
        res = false;
       };
     #endif
     if (!res) return false;
    #endif

    return (count() == size()); 
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
  SHSkList(const SHSkList& sl)
  {
   CALL("constructor SHSkList(const SHSkList& sl)");
   ICP("ICP0");
  };
  
  SHSkList& operator=(const SHSkList& sl)
  {
   CALL("operator=(const SHSkList& sl)");
   ICP("ICP0");
   return *this;
  };

  int randomLevel() const
  { 
   int res = 0;
   ulong tmp = _insertCount;
   while (!(tmp % Base))
    { 
     res++;
     tmp = tmp/Base;
    };
   return res;
  }; // randomLevel() 
 private:
  ulong _sizeOfTopIndex;
  Node** _topIndex;
  int _currMaxLevel;
  ulong _insertCount;
  ulong _size;
  #ifdef DEBUG_ALLOC_OBJ_TYPE
   static ClassDesc _classDesc;
   static ClassDesc _headerClassDesc;
  #endif
 friend class Surfer; 
 friend class SurferWithMemory; 
 friend class Destruction;   
}; // template <class Alloc,class ValType,class KeyType,ulong Base,class InstanceId> class SHSkList


}; // namespace BK

//============================================================================
#undef DEBUG_NAMESPACE
#ifdef DEBUG_SMALL_HEADER_SKIP_LIST
 #define DEBUG_NAMESPACE "SHSkList<class Alloc,class ValType,class KeyType,ulong Base,class InstanceId>"
#endif
#include "debugMacros.hpp"
//============================================================================


namespace BK 
{

template <class Alloc,class ValType,class KeyType,ulong Base,class InstanceId>
inline
SHSkList<Alloc,ValType,KeyType,Base,InstanceId>::Node* SHSkList<Alloc,ValType,KeyType,Base,InstanceId>::insert(const KeyType& searchKey,bool& newNode)  
{
 CALL("insert(const KeyType& searchKey,bool& newNode)");
 ASSERT_IN(checkIntegrity(),"START");

 Node* update[32];

 Node* currNode = Node::node(_topIndex);      // tricky!
 int currLevel = _currMaxLevel;
 Node* nextNode = currNode->next(currLevel);  // tricky!

 KeyType nodeKey;
 check_next_node:
  if (nextNode)
   {
    ASSERT(nextNode->level() <= currLevel);
    nodeKey = nextNode->key();
    if (nodeKey > searchKey) 
     {
      currNode = nextNode;
      nextNode = nextNode->next(currLevel);        
      goto check_next_node;
     };
    // nodeKey <= searchKey
    if (nodeKey == searchKey) { newNode = false; ASSERT(checkIntegrity()); return nextNode; };
    // nodeKey < searchKey, "if (nextNode)" is not needed anymore
    update[currLevel] = currNode;
    while (currLevel) 
     { 
      currLevel--;
      nextNode = currNode->next(currLevel);
      nodeKey = nextNode->key();
      while (nodeKey > searchKey)
       {
        currNode = nextNode;
        nextNode = nextNode->next(currLevel); 
        nodeKey = nextNode->key();
       };
      // nodeKey <= searchKey
      if (nodeKey == searchKey) { newNode = false; ASSERT(checkIntegrity()); return nextNode; };
      // nodeKey < searchKey, "if (nextNode)" is not needed anymore
      update[currLevel] = currNode;
     };
    ASSERT(nextNode->key() < searchKey);
   }
  else // !next_node
   {
    update[currLevel] = currNode;
    if (currLevel) 
     {
      currLevel--;
      nextNode = currNode->next(currLevel);
      goto check_next_node;
     };
   };
 
  ASSERT(!currLevel);    

  // insert new node
  newNode = true;
  ASSERT(_insertCount < INT_MAX);
  _insertCount++;  
  _size++;
  currLevel = randomLevel();

  currNode = new (currLevel) Node(currLevel,searchKey); 
  Node** nodeJump = currNode->jump();  
  Node** jump; 

  if (currLevel > _currMaxLevel)
   {
    // _topIndex must be expanded 

    #ifdef DEBUG_ALLOC_OBJ_TYPE
     ALWAYS(_headerClassDesc.registerAllocated((currLevel + 1) * sizeof(Node*)));
     Node** newTopIndex = (Node**)Alloc::allocate((currLevel + 1) * sizeof(Node*),&_headerClassDesc);
    #else
     Node** newTopIndex = (Node**)Alloc::allocate((currLevel + 1) * sizeof(Node*));
    #endif

	 int lev;
    for (lev = _currMaxLevel; lev>=0; lev--)
     newTopIndex[lev] = _topIndex[lev];
     
    #ifdef DEBUG_ALLOC_OBJ_TYPE
     ALWAYS(_headerClassDesc.registerDeallocated(_sizeOfTopIndex * sizeof(Node*)));
     Alloc::deallocate(_topIndex,_sizeOfTopIndex * sizeof(Node*),&_headerClassDesc);
    #else
     Alloc::deallocate(_topIndex,_sizeOfTopIndex * sizeof(Node*));   
    #endif
      

    Node* pseudonode = Node::node(_topIndex);      
      
    for (lev = _currMaxLevel; lev>=0; lev--)
     { 
      if (update[lev] == pseudonode)      
       {
        jump = newTopIndex + lev;
       }
      else 
       jump = update[lev]->jump() + lev;
      ASSERT((*jump) ? ((*jump)->key() < searchKey) : true);
      nodeJump[lev] = *jump;       
      *jump = currNode;
     };

    _topIndex = newTopIndex;
    _sizeOfTopIndex = currLevel + 1;

    do
     {
      _currMaxLevel++;
      _topIndex[_currMaxLevel] = currNode;
      nodeJump[_currMaxLevel] = (Node*)0;
     }
    while (_currMaxLevel < currLevel);
   }
  else // currLevel <= _currMaxLevel
   {       
    for (int lev = currLevel; lev>=0; lev--)
     { 
      jump = update[lev]->jump() + lev;  
      ASSERT((*jump) ? ((*jump)->key() < searchKey) : true); 
      nodeJump[lev] = *jump;       
      *jump = currNode;
     };
   }; 
  ASSERT_IN(checkIntegrity(),"END");
  return currNode;
}; // Node* SHSkList<..>::insert(const KeyType& searchKey,bool& newNode) 



template <class Alloc,class ValType,class KeyType,ulong Base,class InstanceId>
inline
bool SHSkList<Alloc,ValType,KeyType,Base,InstanceId>::remove(const KeyType& searchKey,ValType& val) 
{
 CALL("remove(const KeyType& searchKey,ValType& val)");
 ASSERT(checkIntegrity());
 Node** jump = _topIndex;  
 int currLevel = _currMaxLevel; 
 Node* node;
 next_node:   
  node = jump[currLevel]; 
  if (node)
   {
    ASSERT(node->level() >= currLevel);  
    if (node->key() == searchKey)
     { 
      jump[currLevel] = node->jump()[currLevel]; 
      if (currLevel) { currLevel--; goto next_node; };
      val = node->value();

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
      jump = node->jump();        
      goto next_node;
     };
    // node->key() < searchKey, try smaller level
    if (currLevel) { currLevel--; goto next_node; }; 
    ASSERT(checkIntegrity());    
    ASSERT(!contains(searchKey)); 
    return false;
   };
    
  // !node
  if (currLevel) { currLevel--; goto next_node; };
  ASSERT(checkIntegrity());
  ASSERT(!contains(searchKey));
  return false;
}; // bool SHSkList<..>::remove(const KeyType& searchKey,ValType& val)  



template <class Alloc,class ValType,class KeyType,ulong Base,class InstanceId>
inline
bool SHSkList<Alloc,ValType,KeyType,Base,InstanceId>::remove(const KeyType& searchKey) 
{
 CALL("remove(const KeyType& searchKey)");
 ASSERT(checkIntegrity());
 Node** jump = _topIndex;  
 int currLevel = _currMaxLevel; 
 Node* node;
 next_node:   
  node = jump[currLevel]; 
  if (node)
   {
    ASSERT(node->level() >= currLevel);  
    if (node->key() == searchKey)
     { 
      jump[currLevel] = node->jump()[currLevel]; 
      if (currLevel) { currLevel--; goto next_node; };


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
      jump = node->jump();        
      goto next_node;
     };
    // node->key() < searchKey, try smaller level
    if (currLevel) { currLevel--; goto next_node; }; 
    ASSERT(checkIntegrity());    
    ASSERT(!contains(searchKey)); 
    return false;
   };
    
  // !node
  if (currLevel) { currLevel--; goto next_node; };
  ASSERT(checkIntegrity());
  ASSERT(!contains(searchKey));
  return false;
}; // bool SHSkList<..>::remove(const KeyType& searchKey)  



template <class Alloc,class ValType,class KeyType,ulong Base,class InstanceId>
void SHSkList<Alloc,ValType,KeyType,Base,InstanceId>::destroy()
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
}; // void SHSkList<Alloc,ValType,KeyType,Base,InstanceId>::destroy()

}; // namespace BK





//============ Specialised version for ValType = void
//============================================================================
#undef DEBUG_NAMESPACE
#ifdef DEBUG_SMALL_HEADER_SKIP_LIST
 #define DEBUG_NAMESPACE "SHVoidSkList<class Alloc,class KeyType,ulong Base,class InstanceId>"
#endif
#include "debugMacros.hpp"
//============================================================================

namespace BK 
{

template <class Alloc,class KeyType,ulong Base,class InstanceId>
class SHVoidSkList
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
    Node* next() { return *(jump()); };
    const Node* next() const { return *(jump()); };    
    static ulong minSize() { return sizeof(Node) + sizeof(Node*); }; 
    static ulong maxSize() { return ULONG_MAX; }; 


   private:
    int level() const { return _level; };
    Node** jump() { return (Node**)(this + 1); }; 
    const Node* const * jump() const { return (const Node* const *)(this + 1); };
    Node** jump(int lev) { return ((Node**)(this + 1)) + lev; }; 
    const Node* const * jump(int lev) const { return ((const Node* const *)(this + 1)) + lev; };
   
    Node(int l,const KeyType& k) : _key(k), _level(l) {};
    Node* next(int lev) { return ((Node**)(this + 1))[lev]; }; 
    const Node* next(int lev) const { return ((const Node* const *)(this + 1))[lev]; };
      
    static const Node* node(const Node * const * jmp)  // This is a hack! Must be removed.
    {
     return ((Node*)jmp) - 1;
    };

    static Node* node(Node ** jmp)           // This is a hack! Must be removed.
    {
     return ((Node*)jmp) - 1;
    };
  
    void* operator new(size_t size,int lv) 
    {    
     CALL("operator new(size_t size,int lv)");  
     #ifdef DEBUG_ALLOC_OBJ_TYPE
      ALWAYS(_classDesc.registerAllocated(size + (lv+1)*sizeof(Node*)));
      return Alloc::allocate(size + (lv+1)*sizeof(Node*),&_classDesc);
     #else
      return Alloc::allocate(size + (lv+1)*sizeof(Node*));
     #endif
    };
    void operator delete(void* obj)
    {
     CALL("operator delete(void* obj)");
     #ifdef DEBUG_ALLOC_OBJ_TYPE
      ALWAYS(_classDesc.registerDeallocated(sizeof(Node) + ((static_cast<Node*>(obj))->level() + 1)*sizeof(Node*)));
      Alloc::deallocate(obj,sizeof(Node) + ((static_cast<Node*>(obj))->level() + 1)*sizeof(Node*),&_classDesc);
     #else
      Alloc::deallocate(obj,sizeof(Node) + ((static_cast<Node*>(obj))->level() + 1)*sizeof(Node*));
     #endif
    };   

    void operator delete(void* obj,int lv)
    {
     CALL("operator delete(void* obj,int lv)");
     #ifdef DEBUG_ALLOC_OBJ_TYPE
      ALWAYS(_classDesc.registerDeallocated(sizeof(Node) + (lv + 1)*sizeof(Node*)));
      Alloc::deallocate(obj,sizeof(Node) + (lv + 1)*sizeof(Node*),&_classDesc);
     #else
      Alloc::deallocate(obj,sizeof(Node) + (lv + 1)*sizeof(Node*));
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
         DMSG("SHVoidSkList<..>::Node::checkObj()> Null pointer to object descriptor.\n");  
         return false; 
        }; 
       bool res = true;   
       #ifdef DEBUG_ALLOC_OBJ_SIZE
        if (objDesc->size !=  (sizeof(Node) + (level() + 1)*sizeof(Node*)))
         {
          DMSG("SHVoidSkList<..>::Node::checkObj()> objDesc->size != (sizeof(Node) + (level() + 1)*sizeof(Node*)) \n");
          res = false;
         };
       #endif
       #ifdef DEBUG_ALLOC_OBJ_STATUS
        if (objDesc->status != ObjDesc::Persistent) 
         {
          DMSG("SHVoidSkList<..>::Node::checkObj()> objDesc->status != ObjDesc::Persistent\n");
          res = false;
         };
       #endif
       #ifdef DEBUG_ALLOC_OBJ_TYPE
        if (objDesc->classDesc != &_classDesc) 
         {
          DMSG("SHVoidSkList<..>::Node::checkObj()> objDesc->classDesc != &_classDesc\n");
          res = false;
         };
       #endif
       if (!res) DOP(DMSG("SHVoidSkList<..>::Node::checkObj()> host object ") << *objDesc << "\n"); 
       return res;
      #endif
     };
    #endif

   private:  
    KeyType _key;
    int _level;  
    #ifdef DEBUG_ALLOC_OBJ_TYPE
     static ClassDesc _classDesc;
    #endif   
   friend class SHVoidSkList<Alloc,KeyType,Base,InstanceId>;
   friend class Surfer;
   friend class SurferWithMemory;
  }; // class Node

  class Surfer
  {
   public:
    Surfer() {};
    Surfer(const SHVoidSkList& sl) { reset(sl); };
    ~Surfer() {};
    void reset(const SHVoidSkList& sl) 
    {
     CALL("Surfer::reset(const SHVoidSkList& sl)");
     ASSERT(sl.checkIntegrity());
     _currMaxLevel = sl._currMaxLevel;
     _jump = sl._topIndex;
     _currNode = _jump[0];
    };
    const Node* currNode() const { return _currNode; };
    const Node* const * currNodeAddr() const { return &_currNode; }; 

    bool next()
    {
     CALL("next()"); 
     _currNode = _currNode->next();
     return (bool)_currNode;
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
      node = _jump[_currMaxLevel];   
      if (node) 
       {
    	ASSERT(node->level() == _currMaxLevel); 
        if (node->key() < searchKey)
	 {
          if (!_currMaxLevel) return false;
          int currLevel = _currMaxLevel - 1;
          const Node* const * jump_ = _jump;        
	  next_node:
	   node = jump_[currLevel];
           ASSERT(node->level() >= currLevel);
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
    int _currMaxLevel;
    const Node* const * _jump;
    const Node* _currNode;
  }; // class Surfer  


  class SurferWithMemory
  {
   public:
    SurferWithMemory() {};
    SurferWithMemory(const SHVoidSkList& sl) { reset(sl); };
    ~SurferWithMemory() {};
    void reset(const SHVoidSkList& sl) 
    {
     CALL("SurferWithMemory::reset(const SHVoidSkList& sl)");
     ASSERT(sl.checkIntegrity());
     _currMaxLevel = sl._currMaxLevel;
     _currNode = sl._topIndex[0];
     if (_currNode)
      {
       for (int lv = _currNode->level() + 1; lv <= _currMaxLevel; lv++)
        _jump[lv] = sl._topIndex[lv]; 
      };
     ASSERT(checkIntegrity());
    };
    const Node* currNode() const { return _currNode; };
    const Node* const * currNodeAddr() const { return &_currNode; }; 

    bool next()
    {
     CALL("next()");
     ASSERT_IN(checkIntegrity(),"START");
     const Node* nextNode = _currNode->next();
     if (nextNode) 
      {
       int lv = nextNode->level() + 1;
       int currNodeLevel = _currNode->level();
       if (lv <= currNodeLevel)
	{
	 const Node* const * currNodeJump = _currNode->jump();      
	 next_level:
	  if (currNodeJump[lv])
	   {
            _jump[lv] = currNodeJump[lv];
            lv++;
	    if (lv <= currNodeLevel) goto next_level;
           }
          else
	   {
	    _currMaxLevel = lv - 1;
           };
        };
       _currNode = nextNode;
       ASSERT_IN(checkIntegrity(),"SUCCESS");
       return true;
      } 
     else
       {
	 ASSERT_IN(checkIntegrity(),"FAILURE");
	 return false;
       };
    }; // bool next()

    bool find(const KeyType searchKey)
    {
      CALL("SurferWithMemory::find(const KeyType& searchKey)");
      ASSERT(checkIntegrity());
      if (_currNode->key() == searchKey) 
	{
	  ASSERT(checkIntegrity());
	  return true;
	};
      const Node* node;
      int currLevel = _currMaxLevel;
      int currNodeLevel = _currNode->level();
      bool overJump = false;
      const Node* const * currJump;  
  
    check_level:
      ASSERT((!overJump) || (currLevel < _currMaxLevel));
      ASSERT(overJump || (currLevel == _currMaxLevel));
      if (currLevel == currNodeLevel)
	{  
	  if (_currNode->key() < searchKey) 
	    {
	      // _currNode is not changed
	      ASSERT(checkIntegrity());
	      return false;
	    };
	  currJump = _currNode->jump();
	  node = currJump[currLevel];
	  if (overJump) 
	    {
	      goto node_jump_with_overjump;
	    };
	  goto node_jump_no_overjump;
	};

      ASSERT(currLevel > currNodeLevel);

      node = _jump[currLevel];

      if (node->key() < searchKey) // too long jump
	{
	  overJump = true;
	  ASSERT(currLevel);
	  currLevel--;
	  goto check_level;
	};
      // node->key() >= searchKey
      if (node->key() == searchKey) 
	{
	  _currNode = node;
	  ASSERT(checkIntegrity());
	  return true;
	};
      // node->key() > searchKey
      ASSERT(node->key() < _currNode->key()); // closer to the goal than _currNode

      currJump = node->jump();
      node = currJump[currLevel];  
      if (overJump) 
	{
	  ASSERT_IN(node,"R200");
	  ASSERT(currLevel < _currMaxLevel);
	  goto node_jump_with_overjump;
	};
      ASSERT_IN(currLevel == _currMaxLevel,"E100");
      goto node_jump_no_overjump;

    node_jump_no_overjump:
      ASSERT_IN(currLevel == _currMaxLevel,"E200");
      if (node)
	{
	  if (node->key() < searchKey) // too long jump
	    { 
	      // this is an over-jump          
	      if (currLevel)
		{
		  _jump[currLevel] = node; // for the future calls  
		  currLevel--;
		  node = currJump[currLevel];
		  ASSERT(node);
		  goto node_jump_with_overjump;
		}
	      else 
		{ 
		  _currNode = node;     
		  ASSERT(checkIntegrity());      
		  return false; 
		};
	    };
	  // node->key() >= searchKey
	  if (node->key() == searchKey) 
	    {
	      _currNode = node;  
	      ASSERT(checkIntegrity()); 
	      return true;
	    };

	  // node->key() > searchKey, proceed with the same level 
	
	  currJump = node->jump();
	  node = currJump[currLevel];
	  ASSERT_IN(currLevel == _currMaxLevel,"E300");
	  ASSERT_IN((!node) || (node->level() == currLevel),"X150");
	  goto node_jump_no_overjump;
	}
      else // !node
	{
	  if (currLevel)
	    {
	      currLevel--;
	      _currMaxLevel = currLevel;
	      node = currJump[currLevel];
	      ASSERT_IN((!node) || (node->level() == currLevel),"X200");
	      goto node_jump_no_overjump;
	    }
	  else 
	    {
	      _currNode = (const Node*)0;
	      ASSERT(checkIntegrity());
	      return false;  
	    };
	};
      
    node_jump_with_overjump:
      if (node->key() < searchKey) // too long jump
	{
	  if (currLevel)
	    {
	      _jump[currLevel] = node; // for the future calls      
	      currLevel--;
	      node = currJump[currLevel];
	      ASSERT_IN(node,"R400");
	      goto node_jump_with_overjump;
	    }
	  else 
	    {
	      _currNode = node;
	      ASSERT(checkIntegrity());
	      return false; 
	    };
	};
      // node->key() >= searchKey
      if (node->key() == searchKey) 
	{
	  _currNode = node;
	  ASSERT(checkIntegrity());
	  return true;
	};
      // node->key() > searchKey proceed with the same level
      ASSERT_IN(node->level() == currLevel,"X300");
      currJump = node->jump();
      node = currJump[currLevel];
      ASSERT_IN(node->level() >= currLevel,"X350");
      goto node_jump_with_overjump;  
    }; // bool find(const KeyType searchKey)

    void getTo(const KeyType searchKey)
    {
      CALL("SurferWithMemory::getTo(const KeyType& searchKey)");
      ASSERT(checkIntegrity());
      if (_currNode->key() == searchKey) 
	{
	  ASSERT(checkIntegrity());
	  return;
	};
      const Node* node;
      int currLevel = _currMaxLevel;
      int currNodeLevel = _currNode->level();
      bool overJump = false;
      const Node* const * currJump;      
      
    check_level:
      ASSERT((!overJump) || (currLevel < _currMaxLevel));
      ASSERT(overJump || (currLevel == _currMaxLevel));
      if (currLevel == currNodeLevel)
	{  
	  ASSERT(_currNode->key() >= searchKey);
	  currJump = _currNode->jump();
	  node = currJump[currLevel];
	  if (overJump) 
	    {
	      goto node_jump_with_overjump;
	    };
	  goto node_jump_no_overjump;
	};

      ASSERT(currLevel > currNodeLevel);

      node = _jump[currLevel]; 

      if (node->key() < searchKey) // too long jump
	{
	  overJump = true;
	  ASSERT(currLevel);
	  currLevel--;
	  goto check_level;
	};
      // node->key() >= searchKey
      if (node->key() == searchKey) 
	{
	  _currNode = node;
	  ASSERT(checkIntegrity());
	  return;
	};
      // node->key() > searchKey
      ASSERT(node->key() < _currNode->key()); // closer to the goal than _currNode

      currJump = node->jump();
      node = currJump[currLevel];  
      if (overJump) 
	{
	  ASSERT_IN(node,"R200");
	  ASSERT(currLevel < _currMaxLevel);
	  goto node_jump_with_overjump;
	};
      ASSERT_IN(currLevel == _currMaxLevel,"E100");
      goto node_jump_no_overjump;

    node_jump_no_overjump:
      ASSERT_IN(currLevel == _currMaxLevel,"E200");
      if (node)
	{
	  if (node->key() < searchKey) // too long jump
	    { 
	      // this is an over-jump  
	      ASSERT(currLevel);  
	      _jump[currLevel] = node; // for the future calls  
	      currLevel--;
	      node = currJump[currLevel];
	      ASSERT(node);
	      goto node_jump_with_overjump;
	    };
	  // node->key() >= searchKey
	  if (node->key() == searchKey) 
	    {
	      _currNode = node;  
	      ASSERT(checkIntegrity()); 
	      return;
	    };

	  // node->key() > searchKey, proceed with the same level 
	
	  currJump = node->jump();
	  node = currJump[currLevel];
	  ASSERT_IN(currLevel == _currMaxLevel,"E300");
	  ASSERT_IN((!node) || (node->level() == currLevel),"X150");
	  goto node_jump_no_overjump;

	}
      else // !node
	{
	  ASSERT(currLevel);
	  currLevel--;
	  _currMaxLevel = currLevel;
	  node = currJump[currLevel];
	  ASSERT_IN((!node) || (node->level() == currLevel),"X200");
	  goto node_jump_no_overjump;
	};
      
    node_jump_with_overjump:
      if (node->key() < searchKey) // too long jump
	{
	  ASSERT(currLevel);
	  _jump[currLevel] = node; // for the future calls      
	  currLevel--;
	  node = currJump[currLevel];
	  ASSERT_IN(node,"R400");
	  goto node_jump_with_overjump;
	};
      // node->key() >= searchKey
      if (node->key() == searchKey) 
	{
	  _currNode = node;
	  ASSERT(checkIntegrity());
	  return;
	};
      // node->key() > searchKey proceed with the same level
      ASSERT_IN(node->level() == currLevel,"X300");
      currJump = node->jump();
      node = currJump[currLevel];
      ASSERT_IN(node->level() >= currLevel,"X350");
      goto node_jump_with_overjump;     
    }; // void getTo(const KeyType searchKey)

    #ifdef DEBUG_NAMESPACE
     bool checkIntegrity()
     {
      if (!_currNode) return true;
      if (_currNode->level() > _currMaxLevel) 
       {
        DMSG("SHVoidSkList<..>::SurferWithMemory::checkIntegrity()> _currNode->level() > _currMaxLevel\n");
        return false;  
       };
      int prevLev = -1; 
      for (int lv = _currNode->level() + 1; lv <= _currMaxLevel; lv++)
       {
	if (!_jump[lv])
	 { 
	  DMSG("SHVoidSkList<..>::SurferWithMemory::checkIntegrity()> !_jump[lv], lv == ") << lv << "\n";
          return false;
         };
        if (_jump[lv]->key() >= _currNode->key())
	 {
          DMSG("SHVoidSkList<..>::SurferWithMemory::checkIntegrity()> _jump[lv]->key() >= _currNode->key(), lv == ") << lv << "\n";
          return false;
         };        
        if ((prevLev != -1) && (_jump[prevLev]->key() < _jump[lv]->key()))
	 { 
          DMSG("SHVoidSkList<..>::SurferWithMemory::checkIntegrity()> _jump[prevLev]->key() < _jump[lv]->key(), lv == ") 
           << lv << ", prevLev == " << prevLev << "\n";
          return false;
        
         };
        prevLev = lv; 
       };
      return true;
     };
    #endif    

   private:
    int _currMaxLevel;
    const Node* _jump[28]; // is there a list longer than 2^28 ?
    const Node* _currNode;
  }; // class SurferWithMemory  



 public:
  SHVoidSkList() 
   {
    CALL("constructor SHVoidSkList()"); 
    init();
   };

  ~SHVoidSkList() 
   {
    CALL("destructor ~SHVoidSkList()");
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
   }; // ~SHVoidSkList() 

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
    ALWAYS(_classDesc.registerAllocated(sizeof(SHVoidSkList)));
    return Alloc::allocate(sizeof(SHVoidSkList),&_classDesc);
   #else
    return Alloc::allocate(sizeof(SHVoidSkList));
   #endif 
  };
  void operator delete(void* obj) 
  {
   CALL("operator delete(void* obj)"); 
   #ifdef DEBUG_ALLOC_OBJ_TYPE
    ALWAYS(_classDesc.registerDeallocated(sizeof(SHVoidSkList)));
    Alloc::deallocate(obj,sizeof(SHVoidSkList),&_classDesc);
   #else
    Alloc::deallocate(obj,sizeof(SHVoidSkList));
   #endif 
  };
  static ulong minSize() { return sizeof(SHVoidSkList); }; // has nothing to do with size()
  static ulong maxSize() { return sizeof(SHVoidSkList); }; // has nothing to do with size()
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
       DMSG("SHVoidSkList<..>::checkIntegrity()> Null pointer to object descriptor for *_topIndex.\n");  
       return false; 
      };     
     #ifdef DEBUG_ALLOC_OBJ_SIZE
      if (objDesc->size !=  _sizeOfTopIndex * sizeof(Node*)) 
       {
        DMSG("SHVoidSkList<..>::checkIntegrity()> Wrong size of *_topIndex.\n");
        res = false;
       };
     #endif
     #ifdef DEBUG_ALLOC_OBJ_STATUS
      if (objDesc->status != ObjDesc::Persistent) 
       {
        DMSG("SHVoidSkList<..>::checkIntegrity()> objDesc->status != ObjDesc::Persistent for *_topIndex.\n");
        res = false; 
       };
     #endif      
     #ifdef DEBUG_ALLOC_OBJ_TYPE
      if (objDesc->classDesc != &_headerClassDesc) 
       {
        DMSG("SHVoidSkList<..>::checkIntegrity()> Wrong objDesc->classDesc with *_topIndex.\n");
        res = false;
       };
     #endif
     if (!res) return false;
    #endif

    return (count() == size()); 
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
  SHVoidSkList(const SHVoidSkList& sl)
  {
   CALL("constructor SHVoidSkList(const SHVoidSkList& sl)");
   ICP("ICP0");
  };
  
  SHVoidSkList& operator=(const SHVoidSkList& sl)
  {
   CALL("operator=(const SHVoidSkList& sl)");
   ICP("ICP0");
   return *this;
  };

  int randomLevel() const
  { 
   int res = 0;
   ulong tmp = _insertCount;
   while (!(tmp % Base))
    { 
     res++;
     tmp = tmp/Base;
    };
   return res;
  }; // randomLevel() 
 private:
  ulong _sizeOfTopIndex;
  Node** _topIndex;
  int _currMaxLevel;
  ulong _insertCount;
  ulong _size;
  #ifdef DEBUG_ALLOC_OBJ_TYPE
   static ClassDesc _classDesc;
   static ClassDesc _headerClassDesc;
  #endif
 friend class Surfer; 
 friend class SurferWithMemory; 
}; // template <class Alloc,class KeyType,ulong Base,class InstanceId> class SHVoidSkList

}; // namespace BK


//============================================================================
#undef DEBUG_NAMESPACE
#ifdef DEBUG_SMALL_HEADER_SKIP_LIST
 #define DEBUG_NAMESPACE "SHVoidSkList<class Alloc,class KeyType,ulong Base,class InstanceId>"
#endif
#include "debugMacros.hpp"
//============================================================================

namespace BK 
{
template <class Alloc,class KeyType,ulong Base,class InstanceId>
inline
bool SHVoidSkList<Alloc,KeyType,Base,InstanceId>::insert(const KeyType& searchKey)  
{
 CALL("insert(const KeyType& searchKey)");
 ASSERT_IN(checkIntegrity(),"START");

 Node* update[32];

 Node* currNode = Node::node(_topIndex);      // tricky!
 int currLevel = _currMaxLevel;
 Node* nextNode = currNode->next(currLevel);  // tricky!

 KeyType nodeKey;
 check_next_node:
  if (nextNode)
   {
    ASSERT(nextNode->level() <= currLevel);
    nodeKey = nextNode->key();
    if (nodeKey > searchKey) 
     {
      currNode = nextNode;
      nextNode = nextNode->next(currLevel);        
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
      nextNode = currNode->next(currLevel);
      nodeKey = nextNode->key();
      while (nodeKey > searchKey)
       {
	currNode = nextNode;
        nextNode = nextNode->next(currLevel); 
        nodeKey = nextNode->key();
       };
      // nodeKey <= searchKey
      if (nodeKey == searchKey) { ASSERT(checkIntegrity()); return false; };
      // nodeKey < searchKey, "if (nextNode)" is not needed anymore
      update[currLevel] = currNode;
     };
    ASSERT(nextNode->key() < searchKey);
   }
  else // !next_node
   {
    update[currLevel] = currNode;
    if (currLevel) 
     {
      currLevel--;
      nextNode = currNode->next(currLevel);
      goto check_next_node;
     };
   };
 
  ASSERT(!currLevel);    

  // insert new node
  ASSERT(_insertCount < INT_MAX);
  _insertCount++;  
  _size++;
  currLevel = randomLevel();

  currNode = new (currLevel) Node(currLevel,searchKey); 
  Node** nodeJump = currNode->jump();  
  Node** jump; 

  if (currLevel > _currMaxLevel)
   {
    // _topIndex must be expanded 

    #ifdef DEBUG_ALLOC_OBJ_TYPE
     ALWAYS(_headerClassDesc.registerAllocated((currLevel + 1) * sizeof(Node*)));
     Node** newTopIndex = (Node**)Alloc::allocate((currLevel + 1) * sizeof(Node*),&_headerClassDesc);
    #else
     Node** newTopIndex = (Node**)Alloc::allocate((currLevel + 1) * sizeof(Node*));
    #endif
	
    int lev;
    for (lev = _currMaxLevel; lev>=0; lev--)
     newTopIndex[lev] = _topIndex[lev];
   
    #ifdef DEBUG_ALLOC_OBJ_TYPE
     ALWAYS(_headerClassDesc.registerDeallocated(_sizeOfTopIndex * sizeof(Node*)));
     Alloc::deallocate(_topIndex,_sizeOfTopIndex * sizeof(Node*),&_headerClassDesc);
    #else
     Alloc::deallocate(_topIndex,_sizeOfTopIndex * sizeof(Node*));   
    #endif
      

    Node* pseudonode = Node::node(_topIndex);      
      
    for (lev = _currMaxLevel; lev>=0; lev--)
     { 
      if (update[lev] == pseudonode)      
       {
	jump = newTopIndex + lev;
       }
      else 
       jump = update[lev]->jump() + lev; 
      ASSERT((*jump) ? ((*jump)->key() < searchKey) : true);
      nodeJump[lev] = *jump;       
      *jump = currNode;
     };
    
    _sizeOfTopIndex = currLevel + 1;
    _topIndex = newTopIndex;

    do
     {
      _currMaxLevel++;
      _topIndex[_currMaxLevel] = currNode;
      nodeJump[_currMaxLevel] = (Node*)0;
     }
    while (_currMaxLevel < currLevel);
   }
  else // currLevel <= _currMaxLevel
   {       
    for (int lev = currLevel; lev>=0; lev--)
     { 
      jump = update[lev]->jump() + lev;  
      ASSERT((*jump) ? ((*jump)->key() < searchKey) : true); 
      nodeJump[lev] = *jump;       
      *jump = currNode;
     };
   }; 
  ASSERT_IN(checkIntegrity(),"END");
  return true;
}; // Node* SHVoidSkList<..>::insert(const KeyType& searchKey) 



template <class Alloc,class KeyType,ulong Base,class InstanceId>
inline
bool SHVoidSkList<Alloc,KeyType,Base,InstanceId>::remove(const KeyType& searchKey) 
{
 CALL("remove(const KeyType& searchKey)");
 ASSERT(checkIntegrity());
 Node** jump = _topIndex;  
 int currLevel = _currMaxLevel; 
 Node* node;
 next_node:   
  node = jump[currLevel]; 
  if (node)
   {
    ASSERT(node->level() >= currLevel);  
    if (node->key() == searchKey)
     { 
      jump[currLevel] = node->jump()[currLevel]; 
      if (currLevel) { currLevel--; goto next_node; };

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
      jump = node->jump();        
      goto next_node;
     };
    // node->key() < searchKey, try smaller level
    if (currLevel) { currLevel--; goto next_node; }; 
    ASSERT(checkIntegrity());    
    ASSERT(!contains(searchKey)); 
    return false;
   };
    
  // !node
  if (currLevel) { currLevel--; goto next_node; };
  ASSERT(checkIntegrity());
  ASSERT(!contains(searchKey));
  return false;
}; // bool SHVoidSkList<..>::remove(const KeyType& searchKey)  


template <class Alloc,class KeyType,ulong Base,class InstanceId>
void SHVoidSkList<Alloc,KeyType,Base,InstanceId>::destroy()
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
}; // void SHVoidSkList<Alloc,KeyType,Base,InstanceId>::destroy()



}; // namespace BK

//======================================================================
#endif
