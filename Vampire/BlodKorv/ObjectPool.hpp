//
// File:         ObjectPool.hpp
// Description:  Pools of reusable objects. 
// Created:      May 22, 2002, Alexandre Riazanov
// mail:         riazanov@cs.man.ac.uk
//==========================================================================
#ifndef OBJECT_POOL_H
#define OBJECT_POOL_H
//==========================================================================
#include "jargon.hpp"
#include "BlodKorvDebugFlags.hpp"
#include "DestructionMode.hpp"
//==========================================================================
#undef DEBUG_NAMESPACE
#ifdef DEBUG_OBJECT_POOL
 #define DEBUG_NAMESPACE "ObjectPool<class Alloc,class C>"
#endif
#include "debugMacros.hpp"
//==========================================================================

namespace BK 
{

template <class Alloc,class C>
class ObjectPool
{
 public:
  class AutoObject
    {
    public:
      AutoObject(ObjectPool& pool) 
	{
	  CALL("AutoObject::constructor AutoObject(ObjectPool& pool)");
	  _pool = &pool;
	  _object = pool.reserveObject();
	};
      ~AutoObject() 
	{
	  CALL("AutoObject::destructor ~AutoObject()");
	  if (DestructionMode::isThorough()) 
	    _pool->releaseObject(_object);
	  BK_CORRUPT(_object);
	  BK_CORRUPT(_pool);
	}; 
      C& object() { return *_object; };
      const C& object() const { return *_object; };
    private:
      AutoObject() 
	{ 
	  CALL("AutoObject::constructor AutoObject()"); 
	  ICP("ICP0");
	};
    private:
      C* _object;
      ObjectPool* _pool;
    }; // class AutoObject
 public:
  ObjectPool() 
  {     
    _allObjects = 0;
    _lastReserved = 0;
    _lastObject = 0;  
  };
  ~ObjectPool() 
    {
      destroy();
    };
  void init() 
    { 
      _allObjects = 0;
      _lastReserved = 0;
      _lastObject = 0;      
    };
  
  void destroy();
  
  void reset();
  C* reserveObject();
  void releaseObject(C* obj);
  bool isEmpty(); 
  void prepare(ulong numberOfObjects);

 private:
  class Wrapper : public C
    {
    public:
      Wrapper() : 
#ifdef DEBUG_NAMESPACE
	debugIsReserved(false),
#endif
	_previous(0), 
	_next(0)
	{
	};
      ~Wrapper() {};
      Wrapper*& next() { return _next; };
      Wrapper*& previous() { return _previous; };
      void* operator new(size_t)
	{
	  return Alloc::allocate(sizeof(Wrapper));
	};
      void operator delete(void* obj)
	{
	  Alloc::deallocate(obj,sizeof(Wrapper));
	};
#ifdef DEBUG_NAMESPACE
    public:
      bool debugIsReserved;
#endif
    private:
      Wrapper* _previous;
      Wrapper* _next;
    };
  
 private:
  Wrapper* _allObjects;
  Wrapper* _lastReserved;
  Wrapper* _lastObject;
}; // class ObjectPool<class Alloc,class C>

}; // namespace BK

//==========================================================================
#undef DEBUG_NAMESPACE
#ifdef DEBUG_OBJECT_POOL
#define DEBUG_NAMESPACE "ObjectPool<class Alloc,class C>"
#endif
#include "debugMacros.hpp"
//==========================================================================

namespace BK 
{

template <class Alloc,class C>  
inline  
void ObjectPool<Alloc,C>::destroy()
{
  CALL("destroy()");
  if (DestructionMode::isThorough())
    while (_allObjects)
      {
	Wrapper* tmp = _allObjects;
	_allObjects = _allObjects->next();
	delete tmp;
      };
  BK_CORRUPT(*this);
};// void destroy()

template <class Alloc,class C>
inline
void ObjectPool<Alloc,C>::reset()
{
  CALL("reset()");
     
#ifdef DEBUG_NAMESPACE
  if (_lastReserved)
    {
      Wrapper* obj = _allObjects;
      while (obj != _lastReserved)
	{
	  ASSERT(obj->debugIsReserved);
	  obj->debugIsReserved = false;
	  obj = obj->next();
	};
      ASSERT(obj->debugIsReserved);
      obj->debugIsReserved = false;
    };
#endif
  _lastReserved = 0;
}; // void reset()
 
template <class Alloc,class C>
inline
C* ObjectPool<Alloc,C>::reserveObject()
{
  CALL("reserveObject()");
  Wrapper* res;
  if (_lastReserved)
    {
      res = _lastReserved->next();
      if (!res)
	{
	  res = new Wrapper();
	  res->previous() = _lastReserved;
	  _lastReserved->next() = res;
	  _lastObject = res;
	};
    }
  else
    {
      if (!_allObjects) 
	{
	  _allObjects = new Wrapper();
	  _lastObject = _allObjects;
	};
      res = _allObjects;
      ASSERT(!res->previous());
    };
  _lastReserved = res;
  DOP(res->debugIsReserved = true);
  return static_cast<C*>(res);
}; // C* reserveObject()
  

template <class Alloc,class C>
inline
void ObjectPool<Alloc,C>::releaseObject(C* obj)
{
  CALL("releaseObject(C* obj)");
  ASSERT((static_cast<Wrapper*>(obj))->debugIsReserved);
  DOP((static_cast<Wrapper*>(obj))->debugIsReserved = false);
  if (static_cast<Wrapper*>(obj) == _lastReserved)
    {
      _lastReserved = _lastReserved->previous();
      return;
    };
  
  Wrapper* prev = (static_cast<Wrapper*>(obj))->previous();
  Wrapper* nxt = (static_cast<Wrapper*>(obj))->next();
  ASSERT(nxt);
  (static_cast<Wrapper*>(obj))->previous() = _lastReserved;
  (static_cast<Wrapper*>(obj))->next() = _lastReserved->next();
  _lastReserved->next() = static_cast<Wrapper*>(obj);

  if ((static_cast<Wrapper*>(obj))->next())
  (static_cast<Wrapper*>(obj))->next()->previous() = static_cast<Wrapper*>(obj);   

  if (prev)
    {
      prev->next() = nxt;
    }
  else
    {
      ASSERT(_allObjects == (static_cast<Wrapper*>(obj)));
      _allObjects = nxt;
    };
  nxt->previous() = prev;
}; // void releaseObject(C* obj)

template <class Alloc,class C>
bool ObjectPool<Alloc,C>::isEmpty()
{
  CALL("isEmpty()");
  if (_lastReserved)    
    return !_lastReserved->next();
  return (bool)_allObjects;
}; // bool ObjectPool<Alloc,C>::isEmpty()

template <class Alloc,class C>
void ObjectPool<Alloc,C>::prepare(ulong numberOfObjects)
{
  CALL("prepare(ulong numberOfObjects)");
  if (!numberOfObjects) return;
  if (!_lastObject)// no objects yet
    {
      ASSERT(!_allObjects);
      _allObjects = new Wrapper();
      _lastObject = _allObjects;
      --numberOfObjects;
    };

  while (numberOfObjects)
    {
      _lastObject->next() = new Wrapper();
      _lastObject->next()->previous() = _lastObject;
      _lastObject = _lastObject->next();
      --numberOfObjects;
    };

}; // void ObjectPool<Alloc,C>::prepare(ulong numberOfObjects)



}; // namespace BK


//==========================================================================
#endif
