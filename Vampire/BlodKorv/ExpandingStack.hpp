//
// File:         ExpandingStack.hpp
// Description:  Stacks that can expand. 
// Created:      Nov 28, 2001 
// Author:       Alexandre Riazanov
// mail:         riazanov@cs.man.ac.uk 
//=======================================================================
#ifndef EXPANDING_STACK_H
//=======================================================================
#define EXPANDING_STACK_H
#include "jargon.hpp"
#include "BlodKorvDebugFlags.hpp"
#include "Array.hpp"
//============================================================================
#undef DEBUG_NAMESPACE
#ifdef DEBUG_EXPANDING_STACK
 #define DEBUG_NAMESPACE "ExpandingStack<Alloc,ElType,MinIncrementSize,InstanceId>"
#endif
#include "debugMacros.hpp"
//======================================================================
namespace BK 
{

template <class Alloc,class ElType,ulong MinIncrementSize,class InstanceId>
class ExpandingStack
{
 public: 
  ExpandingStack() 
   : _memory(DOP("ExpandingStack<..>::_memory"))
  {
   CALL("constructor ExpandingStack()"); 
   _cursor = _memory.memory();
   #ifdef DEBUG_NAMESPACE
    _name = "?";
   #endif
  };
  ExpandingStack(const char* name) 
   : _memory(DOP("ExpandingStack<..>::_memory"))
  {
   CALLM(name,"constructor ExpandingStack(const char* name)");
   _cursor = _memory.memory();
   #ifdef DEBUG_NAMESPACE
    _name = name;
   #endif
  };
  
  ExpandingStack(const ExpandingStack& s)
   : _memory(s._memory)
     #ifdef DEBUG_NAMESPACE
      , _name(s._name)
     #endif
  {
   CALLM(_name,"constructor ExpandingStack(const ExpandingStack& s)");
   _cursor = _memory.memory() + (s._cursor - s._memory.memory());
  };  

  ~ExpandingStack() 
  {
    CALLM(_name,"destructor ~ExpandingStack()");
    BK_CORRUPT(_cursor);
  };


  void init()
  {
   CALLM("?","init()");
   _memory.init(DOP("ExpandingStack<..>::_memory"));
   _cursor = _memory.memory();
   #ifdef DEBUG_NAMESPACE
    _name = "?";
   #endif
  };
  void init(const char* name)
  {
   CALLM(name,"init(const char* name)");
   _memory.init(DOP("ExpandingStack<..>::_memory"));
   _cursor = _memory.memory();
   #ifdef DEBUG_NAMESPACE
    _name = name;
   #endif
  };

  void init(const ExpandingStack& s)
  {
   CALLM(s._name,"init(const ExpandingStack& s)")
   _memory.init(s._memory);

   #ifdef DEBUG_NAMESPACE
    _name = s._name;
   #endif
   _cursor = _memory.memory() + (s._cursor - s._memory.memory());
  };  

  void destroy()
  {
    CALLM(_name,"destroy(");
    _memory.destroy();
  };

  void reset()
  {
   CALLM(_name,"reset()");
   _cursor = _memory.memory();
  }; // void reset()

  operator bool() const
  {
   CALLM(_name,"operator bool() const");
   return _cursor != _memory.memory();   
  };
  bool empty() const
  {
   CALLM(_name,"bool empty() const");
   return _cursor == _memory.memory();  
  };
  bool nonempty() const
  {
   CALLM(_name,"nonempty() const"); 
   return _cursor != _memory.memory(); 
  };

  ulong size() const
  {
   CALLM(_name,"size() const");
   return _cursor - _memory.memory();
  };
  
  ElType* begin() { return _memory.memory(); };
  ElType* end() { return _cursor; };
  ElType* last() { return _cursor - 1; };
  const ElType* begin() const { return _memory.memory(); }; 
  const ElType* end() const { return _cursor; }; 
  const ElType* last() const { return _cursor - 1; };
  ElType& top()
  {
   CALLM(_name,"ElType& top()");
   return *(_cursor - 1);
  }; 
  const ElType& top() const
  {
   CALLM(_name,"const ElType& top() const");
   return *(_cursor - 1);
  };
  ElType& next()      // unsafe
  {
   CALLM(_name,"ElType& next()");
   ASSERT(_cursor < _memory.endOfMemory());
   return *_cursor;
  };

  ElType& nextSafe()     
  {
   CALLM(_name,"ElType& next()");
   if (_cursor >= _memory.endOfMemory()) 
    {
     ulong sz = _cursor - _memory.memory(); 
     _memory.expand();
     _cursor = _memory.memory() + sz;
    };
   return *_cursor;
  };    

  const ElType& next() const       // unsafe
  {
   CALLM(_name,"const ElType& next() const");
   ASSERT(_cursor < _memory.endOfMemory());
   return *_cursor;
  };
  
  void push(const ElType& el)  // unsafe
  {
   CALLM(_name,"push(const ElType& el)");
   ASSERT(_cursor < _memory.endOfMemory());
   *_cursor = el;
   _cursor++;
  };

  void pushSafe(const ElType& el)
  {
   CALLM(_name,"pushSafe(const ElType& el)");
   if (_cursor >= _memory.endOfMemory()) 
    {
     ulong sz = _cursor - _memory.memory(); 
     _memory.expand();
     _cursor = _memory.memory() + sz;
    };
   ASSERT(_cursor < _memory.endOfMemory());
   *_cursor = el;
   _cursor++;
  };

  void push() // unsafe
  {
   CALLM(_name,"push()");
   ASSERT(_cursor < _memory.endOfMemory());
   _cursor++;
  };

  void pushSafe()
  {
   CALLM(_name,"pushSafe()");
   if (_cursor >= _memory.endOfMemory()) 
    {
     ulong sz = _cursor - _memory.memory();
     _memory.expand();
     _cursor = _memory.memory() + sz;
    };
   ASSERT(_cursor < _memory.endOfMemory());
   _cursor++;
  };
  const ElType& pop()
  {
   CALLM(_name,"pop()");
   ASSERT(nonempty());
   _cursor--;
   return *_cursor;
  };
  void skipTop()
  {
   CALLM(_name,"skipTop()");
   ASSERT(nonempty());
   _cursor--;
  };
  const ElType& nth(ulong n) const // unsafe
  {
   CALLM(_name,"nth(ulong n) const");
   ASSERT(n < _memory.size());
   return _memory[n];
  };
  ElType& nth(ulong n)  // unsafe
  {
   CALLM(_name,"nth(ulong n)");
   ASSERT(n < _memory.size());
   return _memory[n];
  };
  ElType& nthSafe(ulong n)
  {
   CALLM(_name,"nthSafe(ulong n)");
   if (n >= _memory.size()) 
    {
     ulong sz = _cursor - _memory.memory(); 
     _memory.expand(n);
     _cursor = _memory.memory() + sz;
    };
   return _memory[n];  
  };
  void expand(ulong n)
  {
   CALLM(_name,"expand(ulong n)");
   ulong sz = _cursor - _memory.memory(); 
   _memory.expand(n);
   _cursor = _memory.memory() + sz;
  };
  void expand()
  {
   CALLM(_name,"expand()");
   ulong sz = _cursor - _memory.memory(); 
   _memory.expand();
   _cursor = _memory.memory() + sz;
  };

  #ifndef NO_DEBUG
   void freeze() { _memory.freeze(); };
   void unfreeze() { _memory.unfreeze(); };
  #endif  

 private: 
  typedef Array<Alloc,ElType,MinIncrementSize,ExpandingStack> Array;

 private:
  Array _memory;
  ElType* _cursor;
  #ifdef DEBUG_NAMESPACE
   const char* _name;
  #endif
}; // class ExpandingStack<Alloc,ElType,MinIncrementSize,InstanceId>

}; // namespace BK


//=========================================================================
#undef DEBUG_NAMESPACE
//=========================================================================
#endif
