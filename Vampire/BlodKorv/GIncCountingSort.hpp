//
// File:         GIncCountingSort.hpp
// Description:  Incremental version of sorting by counting comparisons.
// Created:      Feb 13, 2000, 16:30
// Author:       Alexandre Riazanov
// mail:         riazanov@cs.man.ac.uk
//============================================================================
#ifndef G_INC_COUNTING_SORT_H
//=============================================================================
#define G_INC_COUNTING_SORT_H
#ifndef NO_DEBUG_VIS
 #include <iostream>
#endif
#include "jargon.hpp"
#include "BlodKorvDebugFlags.hpp"
//============================================================================
#undef DEBUG_NAMESPACE
#ifdef DEBUG_G_INC_COUNTING_SORT
 #define DEBUG_NAMESPACE "GIncCountingSort<KeyType,ElType,MaxNumOfElements>"
#endif
#include "debugMacros.hpp"
//============================================================================
namespace BK 
{

template <class KeyType,class ElType,ulong MaxNumOfElements>
class GIncCountingSort
{
 public:
  GIncCountingSort() : _next(0UL)
  {    
  };
  ~GIncCountingSort() {};
  void init() 
  { 
    CALL("init()");
    for (ulong i = 0; i < MaxNumOfElements; ++i)
      _elem[i].init();
    _next = 0UL; 
  };
  void destroy()
  {
    CALL("destroy()");
    for (ulong i = 0; i < MaxNumOfElements; ++i)
      _elem[i].destroy();
    BK_CORRUPT(*this);
  };
  void reset() 
  {
   _next = 0UL;
  };
  ElType* next() { return _elem + _next; };
  void push()
  {
   CALL("push()");
   ASSERT(_next < MaxNumOfElements);
   KeyType key = _elem[_next].key();
   ulong num = 0;
   for (ulong i = 0; i < _next; i++)
    if (_elem[i].key() < key) { num++; }
    else _num[i]++;
   _num[_next] = num;
   _next++;     
  }; 
  void push(const ElType& el)
  {
   CALL("push(const KeyType& key)");
   ASSERT(_next < MaxNumOfElements);
   _elem[_next] = el;
   KeyType key = el.key();
   ulong num = 0;
   for (ulong i = 0; i < _next; i++)
    if (_elem[i].key() < key) { num++; }
    else _num[i]++;
   _num[_next] = num;
   _next++; 
  };
  const ElType* elem() const { return _elem; };
  const ulong* num() const { return _num; };
  ulong size() const { return _next; };
  #ifndef NO_DEBUG_VIS
   ostream& output(ostream& str) const 
   {
    for (ulong i = 0; i < _next; i++) str << _elem[i].key() << " [" << _num[i] << "] "; 
    return str << '\n';
   };
  #endif
 private:
  ElType _elem[MaxNumOfElements];
  ulong _num[MaxNumOfElements];
  ulong _next;
}; // template <class KeyType,class ElType,ulong MaxNumOfElements> class GIncCountingSort

}; // namespace BK

//============================================================================
#undef DEBUG_NAMESPACE
#ifdef DEBUG_G_INC_COUNTING_SORT
 #define DEBUG_NAMESPACE "GSimpleIncCountingSort<KeyType,MaxNumOfElements>"
#endif
#include "debugMacros.hpp"
//============================================================================
namespace BK 
{

template <class KeyType,ulong MaxNumOfElements>
class GSimpleIncCountingSort
{
 public:
  GSimpleIncCountingSort() : _next(0UL) {  };
  void init() 
  { 
    CALL("init()");
    _next = 0UL; 
    
  };
  void destroy() 
  {
    CALL("destroy()");
  };
  void reset() 
  {
   _next = 0UL;
  };
  KeyType* next() { return _key + _next; };
  void push()
  {   
   CALL("push()");
   ASSERT(_next < MaxNumOfElements);
   KeyType key = _key[_next];
   ulong num = 0;
   for (ulong i = 0; i < _next; i++)
    if (_key[i] < key) { num++; }
    else _num[i]++;
   _num[_next] = num;
   _next++;    
  }; 
  void push(const KeyType& key)
  {
   CALL("push(const KeyType& key)");
   ASSERT(_next < MaxNumOfElements);
   _key[_next] = key;
   ulong num = 0;
   for (ulong i = 0; i < _next; i++)
    if (_key[i] < key) { num++; }
    else _num[i]++;
   _num[_next] = num;
   _next++;   
  };
  const ulong* num() const { return _num; };
  ulong size() const { return _next; }; 
  #ifndef NO_DEBUG_VIS
   ostream& output(ostream& str) const 
   {
    for (ulong i = 0; i < _next; i++) str << _key[i] << " [" << _num[i] << "] "; 
    return str << '\n';
   };
  #endif
 private:
  KeyType _key[MaxNumOfElements];
  ulong _num[MaxNumOfElements];
  ulong _next;
}; // template <class KeyType,ulong MaxNumOfElements> class GSimpleIncCountingSort

}; // namespace BK
//============================================================================
#undef DEBUG_NAMESPACE
//============================================================================
#endif
