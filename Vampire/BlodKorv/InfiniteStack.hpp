//
// File:         InfiniteStack.hpp
// Description:  Stacks of unlimited size implemented as double-linked lists. 
// Created:      Mar 12, 2003, Alexandre Riazanov, riazanov@cs.man.ac.uk 
//=======================================================================
#ifndef INFINITE_STACK_H
//=======================================================================
#define INFINITE_STACK_H
#include "jargon.hpp"
#include "BlodKorvDebugFlags.hpp"
#include "DestructionMode.hpp"
//=====================================================================
#undef DEBUG_NAMESPACE
#ifdef DEBUG_INFINITE_STACK
 #define DEBUG_NAMESPACE "InfiniteStack<Alloc,ElType,CacheSize,InstanceId>"
#endif
#include "debugMacros.hpp"
//======================================================================
namespace BK 
{

template <class Alloc,class ElType,ulong CacheSize,class InstanceId>
class InfiniteStack
{
public:
  class Entry
  {
  public:
    ElType& value() { return _value; };
    const ElType& value() const { return _value; };
    Entry* next() { return _next; };
    const Entry* next() const { return _next; };
    Entry* previous() { return _previous; };
    const Entry* previous() const { return _previous; };
  private:
    Entry() {};
    ~Entry() {};
    void* operator new(size_t)
    {
      CALL("Entry::operator new(size_t)");
      return Alloc::allocate(sizeof(Entry));
    };
    void operator delete(void* obj)
    {
      CALL("Entry::operator delete(void* obj)");
      Alloc::deallocate(obj,sizeof(Entry));
    };
  private:
    ElType _value;
    Entry* _next;
    Entry* _previous;
    friend class InfiniteStack;
  }; // class Entry

public:
  InfiniteStack();
  InfiniteStack(const char* name);
  ~InfiniteStack();
  void reset() { _top = _cache; };
  operator bool() const { return nonempty(); };
  bool empty() const { return _top == _cache; };
  bool nonempty() const { return _top != _cache; };

  // forward iteration 
  Entry* begin() { return _cache + 1; };
  const Entry* begin() const { return _cache + 1; };
  Entry* end() { return  _top->_next; };
  const Entry* end() const { return  _top->_next; };

  // backward iteration
  Entry* last() { return _top; };
  const Entry* last() const { return _top; };
  Entry* beforeBegin() { return _cache; };
  const Entry* beforeBegin() const { return _cache; };
  
  
 
  ElType& top()
  {
    CALLM(_name,"top()");
    ASSERT(nonempty());
    return _top->_value;
  };
  const ElType& top() const
  {
    CALLM(_name,"top() const");
    ASSERT(nonempty());
    return _top->_value;
  };
  ElType& next()
  {
    CALLM(_name,"next()");
    Entry* nxt = _top->_next;
    if (!nxt) 
      {
	nxt = new Entry();
	_top->_next = nxt;
	nxt->_next = 0;
	nxt->_previous = _top;
      };
    return nxt->_value;
  }; 

  void push(const ElType& el)
  {
    CALLM(_name,"push(const ElType& el)"); 
    Entry* nxt = _top->_next;
    if (!nxt) 
      {
	nxt = new Entry();
	_top->_next = nxt;
	nxt->_next = 0;
	nxt->_previous = _top;
      };
    nxt->_value = el;
    _top = nxt;
  };

  void push()
  {
    CALLM(_name,"push()"); 
    Entry* nxt = _top->_next;
    if (!nxt) 
      {
	nxt = new Entry();
	_top->_next = nxt;
	nxt->next = 0;
	nxt->_previous = _top;
      };
    _top = nxt;
  };

  const ElType& pop()
  {
    CALLM(_name,"pop()");
    ASSERT(nonempty());
    Entry* prevTop = _top;
    _top = _top->_previous;
    return prevTop->_value;
  };
  void skipTop()
  {
    CALLM(_name,"skipTop()");
    ASSERT(nonempty());
    _top = _top->_previous;
  };

private:
  Entry _cache[CacheSize + 1];
  Entry* _top;
  #ifdef DEBUG_NAMESPACE
   const char* _name;
  #endif
}; // class InfiniteStack<Alloc,ElType,CacheSize,InstanceId>
}; // namespace BK


//=====================================================================
#undef DEBUG_NAMESPACE
#ifdef DEBUG_INFINITE_STACK
 #define DEBUG_NAMESPACE "InfiniteStack<Alloc,ElType,CacheSize,InstanceId>"
#endif
#include "debugMacros.hpp"
//======================================================================

namespace BK 
{

template <class Alloc,class ElType,ulong CacheSize,class InstanceId>
InfiniteStack<Alloc,ElType,CacheSize,InstanceId>::InfiniteStack() :
  _top(_cache)
{
  CALLM("?","constructor InfiniteStack()");
#ifdef DEBUG_NAMESPACE
  _name = "?";
#endif
  _cache[0]._previous = 0;
  _cache[CacheSize]._next = 0;
  for (ulong i = 0; i < CacheSize; ++i)
    {
      _cache[i]._next = _cache + (i + 1);
      _cache[i+1]._previous = _cache + i;
    };
}; // InfiniteStack<Alloc,ElType,CacheSize,InstanceId>::InfiniteStack()


template <class Alloc,class ElType,ulong CacheSize,class InstanceId>
InfiniteStack<Alloc,ElType,CacheSize,InstanceId>::InfiniteStack(const char* name) :
  _top(_cache)
{
  CALLM(name,"constructor InfiniteStack(const char* name)");
#ifdef DEBUG_NAMESPACE
  _name = name;
#endif
  _cache[0]._previous = 0;
  _cache[CacheSize]._next = 0;
  for (ulong i = 0; i < CacheSize; ++i)
    {
      _cache[i]._next = _cache + (i + 1);
      _cache[i+1]._previous = _cache + i;
    };
}; // InfiniteStack<Alloc,ElType,CacheSize,InstanceId>::InfiniteStack(const char* name)


template <class Alloc,class ElType,ulong CacheSize,class InstanceId>
InfiniteStack<Alloc,ElType,CacheSize,InstanceId>::~InfiniteStack()
{
  CALLM(_name,"destructor ~InfiniteStack()");
  if (DestructionMode::isThorough())
    {
      Entry* tmp1 = _cache[CacheSize]._next;
      while (tmp1)
	{
	  Entry* tmp2 = tmp1->_next;
	  delete tmp1;
	  tmp1 = tmp2;
	};
    };
}; // InfiniteStack<Alloc,ElType,CacheSize,InstanceId>::~InfiniteStack()


}; // namespace BK
//=========================================================================
#endif
