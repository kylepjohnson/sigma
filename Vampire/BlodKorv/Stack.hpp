//
// File:         Stack.hpp
// Description:  Simple fixed-size stacks.
// Created:      Sep 24, 1999, 16:30
// Author:       Alexandre Riazanov
// mail:         riazanov@cs.man.ac.uk 
//=======================================================================
#ifndef STACK_H
//=======================================================================
#define STACK_H
#include "jargon.hpp"
#include "BlodKorvDebugFlags.hpp"
//============================================================================
#undef DEBUG_NAMESPACE
#ifdef DEBUG_STACK
 #define DEBUG_NAMESPACE "Stack<class ElementT,ulong maxSize>"
#endif
#include "debugMacros.hpp"
//======================================================================
namespace BK 
{


template <class ElementT,ulong maxSize>
class Stack
{
 public:
  Stack() : cursor(static_cast<ElementT*>(static_cast<void*>(memory))) 
  {
   #ifdef DEBUG_STACK
    name = "?";
   #endif
  };
  Stack(const char* nm) 
   : cursor((static_cast<ElementT*>(static_cast<void*>(memory)))) 
  {
   #ifdef DEBUG_STACK
    name = nm;
   #endif
  };
  Stack(const Stack& s) 
   : cursor((static_cast<ElementT*>(static_cast<void*>(memory))))
  {
   for (ElementT* p = static_cast<ElementT*>(static_cast<void*>(s.memory)); p < s.cursor; p++) push(*p); 
   #ifdef DEBUG_STACK
    name = "?";
   #endif        
  }; 
  Stack(const Stack& s,const char* nm) 
   : cursor((static_cast<ElementT*>(static_cast<void*>(memory))))
  {
   for (ElementT* p = static_cast<ElementT*>(static_cast<void*>(s.memory)); p < s.cursor; p++) push(*p); 
   #ifdef DEBUG_STACK
    name = nm;
   #endif        
  };
  ~Stack() 
  { 
    CALLM(name,"destructor ~Stack()");
    BK_CORRUPT(*this);
  };
  Stack& operator=(const Stack& s) { copy(s); return *this; };     
  
  void init() 
  {
   #ifdef DEBUG_STACK
    name = "?";
   #endif 
   cursor = (static_cast<ElementT*>(static_cast<void*>(memory))); 
  };
  void init(const char* nm) 
  { 
   #ifdef DEBUG_STACK
    name = nm;
   #endif   
   cursor = (static_cast<ElementT*>(static_cast<void*>(memory))); 
  };

  void destroy() 
  {
    CALLM(name,"destroy()");
    BK_CORRUPT(*this);
  };

  void reset() { cursor = (static_cast<ElementT*>(static_cast<void*>(memory))); };
  void copy(const Stack& s) 
  { 
   reset();
   for (ElementT* p = static_cast<ElementT*>(static_cast<void*>(s.memory)); p < s.cursor; p++) push(*p); 
  }; 

  operator bool() const { return cursor != (static_cast<const ElementT*>(static_cast<const void*>(memory))); };
  bool empty() const { return cursor ==  (static_cast<const ElementT*>(static_cast<const void*>(memory))); };
  bool nonempty() const { return cursor != (static_cast<const ElementT*>(static_cast<const void*>(memory))); };
  bool full() const { return size() == maxSize; };
  bool nonfull() const { return size() < maxSize; };

  
  bool roomFor(unsigned  long n) const 
  {
   return size() + n <= maxSize;   
  };
 

  ulong size() const { return (cursor - (static_cast<const ElementT*>(static_cast<const void*>(memory)))); }; 
  
  ElementT* begin() { return (static_cast<ElementT*>(static_cast<void*>(memory))); };
  ElementT* end() { return cursor; };
  ElementT* last() { return cursor - 1; };
  const ElementT* begin() const { return (static_cast<const ElementT*>(static_cast<const void*>(memory))); };
  const ElementT* end() const { return cursor; };
  const ElementT* last() const { return cursor - 1; };
  ElementT& top() 
  {
   CALLM(name,"(ElementT& top())"); 
   ASSERT(cursor > static_cast<ElementT*>(static_cast<void*>(memory)));
   return *(cursor-1); 
  };
  const ElementT& top() const 
  {
   CALLM(name,"(const ElementT& top())"); 
   ASSERT(cursor > static_cast<const ElementT*>(static_cast<const void*>(memory)));
   return *(cursor-1); 
  };
  ElementT& next() 
  {
   CALLM(name,"(ElementT& next())");
   ASSERT(nonfull());
   return *cursor; 
  };
  const ElementT& next() const 
  {
   CALLM(name,"(const ElementT& next())");
   ASSERT(nonfull()); 
   return *cursor; 
  };
  void push(const ElementT& el) 
  {
   CALLM(name,"push(const ElementT& el)");
   ASSERT(nonfull());
   *cursor = el; 
   cursor++; 
  };
  void push() 
  { 
   CALLM(name,"push()");
   ASSERT(nonfull());
   cursor++; 
  };
  const ElementT& pop() 
  {
   CALLM(name,"const ElementT& pop()");
   ASSERT(nonempty());
   cursor--; 
   return *cursor; 
  };
  void skipTop() 
  {
   CALLM(name,"skipTop()");
   ASSERT(nonempty());
   cursor--; 
  };
  const ElementT& nth(ulong n) const
  {
   return (static_cast<const ElementT*>(static_cast<const void*>(memory)))[n]; 
  };
  ElementT& nth(ulong n)
  {
   return (static_cast<ElementT*>(static_cast<void*>(memory)))[n]; 
  };   
 private:
  char memory[sizeof(ElementT)*maxSize];
  ElementT* cursor;  
  #ifdef DEBUG_NAMESPACE
   const char* name;
  #endif   
}; // template <class ElementT,ulong maxSize> class Stack
}; // namespace BK

//=========================================================================
#endif
