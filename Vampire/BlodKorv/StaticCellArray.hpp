//
// File:         StaticCellArray.hpp
// Description:  Expanding arrays for storing elements 
//               which we do not want to move in memory. 
// Created:      Nov 05, 2001
// Author:       Alexandre Riazanov
// mail:         riazanov@cs.man.ac.uk
//============================================================================
#ifndef STATIC_CELL_ARRAY_H
//=============================================================================
#define STATIC_CELL_ARRAY_H 
#include "jargon.hpp"
#include "BlodKorvDebugFlags.hpp"
#include "Array.hpp"
#include "DestructionMode.hpp"
//============================================================================
#undef DEBUG_NAMESPACE
#ifdef DEBUG_STATIC_CELL_ARRAY
 #define DEBUG_NAMESPACE "StaticCellArray<class Alloc,class ElType,ulong MinIncrementSize,class InstanceId>"
#endif
#include "debugMacros.hpp"
//============================================================================

namespace BK 
{
template <class Alloc,class ElType,ulong MinIncrementSize,class InstanceId>
class StaticCellArray
{
 private:
  class WrappedElement;
 public: 
  class ReadWriteIterator
  {
   public:
    ReadWriteIterator() {};
    ~ReadWriteIterator() {};
    operator bool() const { return (bool)_ptr; };
    ElType& operator*() const { return _ptr->content; };
    ReadWriteIterator operator+(int i) const { return ReadWriteIterator(_ptr + i); };
    ReadWriteIterator operator+(long i) const { return ReadWriteIterator(_ptr + i); };
    ReadWriteIterator operator+(ulong i) const { return ReadWriteIterator(_ptr + i); };
    ReadWriteIterator operator-(int i) const { return ReadWriteIterator(_ptr - i); };
    ReadWriteIterator operator-(long i) const { return ReadWriteIterator(_ptr - i); };
    ReadWriteIterator operator-(ulong i) const { return ReadWriteIterator(_ptr - i); };
    ReadWriteIterator& operator++(int) { _ptr = _ptr->next; return *this; };
    ReadWriteIterator& operator++() { _ptr = _ptr->next; return *this; };
    ReadWriteIterator& operator--(int) { _ptr = _ptr->previous; return *this; };
    ReadWriteIterator& operator--() { _ptr = _ptr->previous; return *this; };
    ReadWriteIterator& operator+=(int i) 
    {
     while (i > 0) { i--; _ptr = _ptr->next; };
     return *this; 
    };
    ReadWriteIterator& operator+=(long i) 
    {
     while (i > 0) { i--; _ptr = _ptr->next; };
     return *this; 
    };
    ReadWriteIterator& operator+=(ulong i) 
    {
     while (i > 0) { i--; _ptr = _ptr->next; };
     return *this; 
    };
    ReadWriteIterator& operator-=(int i) 
    {
     while (i > 0) { i--; _ptr = _ptr->previous; };
     return *this; 
    };
    ReadWriteIterator& operator-=(long i) 
    {
     while (i > 0) { i--; _ptr = _ptr->previous; };
     return *this; 
    };
    ReadWriteIterator& operator-=(ulong i) 
    {
     while (i > 0) { i--; _ptr = _ptr->previous; };
     return *this; 
    };
   private: 
    ReadWriteIterator(WrappedElement* ptr) : _ptr(ptr) {}; 
   private: 
    WrappedElement* _ptr;
   friend class StaticCellArray;
  }; // class ReadWriteIterator  

  class ReadOnlyIterator
  {
   public:
    ReadOnlyIterator() {};
    ~ReadOnlyIterator() {};
    operator bool() const { return (bool)_ptr; };
    const ElType& operator*() const { return _ptr->content; };
    ReadOnlyIterator operator+(int i) const { return ReadOnlyIterator(_ptr + i); };
    ReadOnlyIterator operator+(long i) const { return ReadOnlyIterator(_ptr + i); };
    ReadOnlyIterator operator+(ulong i) const { return ReadOnlyIterator(_ptr + i); };
    ReadOnlyIterator operator-(int i) const { return ReadOnlyIterator(_ptr - i); };
    ReadOnlyIterator operator-(long i) const { return ReadOnlyIterator(_ptr - i); };
    ReadOnlyIterator operator-(ulong i) const { return ReadOnlyIterator(_ptr - i); };
    ReadOnlyIterator& operator++(int) { _ptr = _ptr->next; return *this; };
    ReadOnlyIterator& operator++() { _ptr = _ptr->next; return *this; };
    ReadOnlyIterator& operator--(int) { _ptr = _ptr->previous; return *this; };
    ReadOnlyIterator& operator--() { _ptr = _ptr->previous; return *this; };
    ReadOnlyIterator& operator+=(int i) 
    {
     while (i > 0) { i--; _ptr = _ptr->next; };
     return *this; 
    };
    ReadOnlyIterator& operator+=(long i) 
    {
     while (i > 0) { i--; _ptr = _ptr->next; };
     return *this; 
    };
    ReadOnlyIterator& operator+=(ulong i) 
    {
     while (i > 0) { i--; _ptr = _ptr->next; };
     return *this; 
    };
    ReadOnlyIterator& operator-=(int i) 
    {
     while (i > 0) { i--; _ptr = _ptr->previous; };
     return *this; 
    };
    ReadOnlyIterator& operator-=(long i) 
    {
     while (i > 0) { i--; _ptr = _ptr->previous; };
     return *this; 
    };
    ReadOnlyIterator& operator-=(ulong i) 
    {
     while (i > 0) { i--; _ptr = _ptr->previous; };
     return *this; 
    };
   private: 
    ReadOnlyIterator(WrappedElement* ptr) : _ptr(ptr) {}; 
   private: 
    WrappedElement* _ptr;
   friend class StaticCellArray;
  }; // class ReadOnlyIterator
 public:
  StaticCellArray() 
   : _redirectTable(DOP("StaticCellArray::_redirectTable"))
  {
   #ifdef DEBUG_NAMESPACE
    _name = "?";
   #endif
  }; 
  StaticCellArray(const char* nm) 
   : _redirectTable(DOP("StaticCellArray::_redirectTable")) 
  {
   #ifdef DEBUG_NAMESPACE
    _name = nm;
   #endif
  }; 

  ~StaticCellArray() 
    { 
      CALLM(_name,"destructor ~StaticCellArray()"); 
      if (DestructionMode::isThorough())
	for (ulong i = 0; i < size(); i++)
	  delete (_redirectTable[i]);
    };

  void init() 
  {
   CALL("init()");
   _redirectTable.init(DOP("StaticCellArray::_redirectTable")); 
   #ifdef DEBUG_NAMESPACE
    _name = "?";
   #endif
  };
  void init(const char* nm)
  {
   CALLM(nm,"init(const char* nm)");
   _redirectTable.init(DOP("StaticCellArray::_redirectTable")); 
   #ifdef DEBUG_NAMESPACE
    _name = nm;
   #endif
  };

  void destroy() 
  {
    CALLM(_name,"destroy()");  
    if (DestructionMode::isThorough())
      {
	for (ulong i = 0; i < size(); i++)
	  delete (_redirectTable[i]);
      };
    _redirectTable.destroy();
  };
  
  void* operator new(size_t) 
  {
   CALL("operator new (size_t)");
   return Alloc::allocate(sizeof(StaticCellArray));
  };
  void operator delete(void* obj) 
  {
   CALL("operator delete(void* obj)");
   Alloc::deallocate(obj,sizeof(StaticCellArray)); 
  };

  ReadWriteIterator begin()
  { 
   if (size()) return ReadWriteIterator(_redirectTable[0]);
   return ReadWriteIterator(0);
  };
  ReadWriteIterator end()
  { 
   return ReadWriteIterator(0);
  };
 
  ReadOnlyIterator begin() const
  { 
   if (size()) return ReadOnlyIterator(_redirectTable[0]);
   return ReadOnlyIterator(0);
  };
  ReadOnlyIterator end() const
  { 
   return ReadOnlyIterator(0);
  }; 

  ElType& operator[](ulong i) // unsafe but efficient
  {
   CALLM(_name,"operator[](ulong i)");
   ASSERT(i < size());  
   return _redirectTable[i]->content;
  };

  ElType& operator[](ulong i) const // unsafe but efficient
  {
   CALLM(_name,"operator[](ulong i) const"); 
   ASSERT(i < size());  
   return _redirectTable[i]->content;
  }; 

  const ElType& sub(ulong i) const // safe but inefficient
  {
   CALLM(_name,"const ElType& sub(ulong i) const");
   if (i >= size()) expand(i);
   return _redirectTable[i]->content;
  };
  
  ElType& sub(ulong i) // safe but inefficient
  {
   CALLM(_name,"ElType& sub(ulong i)");
   if (i >= size()) expand(i);
   return _redirectTable[i]->content;
  };

  void expand(ulong i)
  {
   CALLM(_name,"expand(ulong i)");
   ulong oldSize = size(); 
   _redirectTable.expand(i);
   for (ulong j = oldSize; j < size(); j++)
    initCell(j); 
  }; // void expand(ulong i)

  void expand()
  {
   CALLM(_name,"expand()");
   ulong oldSize = size();
   _redirectTable.expand();  
   ASSERT(size() > oldSize);
   for (ulong i = oldSize; i < size(); i++)
    initCell(i);        
  }; // void expand()

  ulong size() const { return _redirectTable.size(); };

  #ifndef NO_DEBUG
   void freeze() { _redirectTable.freeze(); };
   void unfreeze() { _redirectTable.unfreeze(); };
  #endif  

 private: 
  class WrappedElement 
  {
   public:
    WrappedElement() {};
    ~WrappedElement() {};
    void* operator new(size_t) 
    {
     CALL("WrappedElement::operator new(size_t)");
     return Alloc::allocate(sizeof(WrappedElement));
    };
    void operator delete(void* obj)
    {
     CALL("WrappedElement::operator delete(void* obj)");
     Alloc::deallocate(obj,sizeof(WrappedElement));
    };
   public:
    ElType content;
    WrappedElement* previous;
    WrappedElement* next;
  };

 private: 
  StaticCellArray(const StaticCellArray& a) 
  {
   CALL("constructor StaticCellArray(const StaticCellArray& a)");
   ICP("ICP0");
  };
  StaticCellArray& operator=(const StaticCellArray& a)  
  {
   CALL("operator=(const StaticCellArray& a)");
   ICP("ICP0");
   return *this;
  };

  void initCell(ulong i) 
  {
   CALL("initCell(ulong i)");
   _redirectTable[i] = new WrappedElement(); 
   if (i)
    {
     _redirectTable[i]->previous = _redirectTable[i-1]; 
     _redirectTable[i]->previous->next = _redirectTable[i];
    }
   else 
     _redirectTable[i]->previous = 0;
   _redirectTable[i]->next = 0;
  };
 private:
  Array<Alloc,WrappedElement*,MinIncrementSize,StaticCellArray> _redirectTable;
  #ifdef DEBUG_NAMESPACE
   const char* _name;
  #endif
 friend class ReadWriteIterator;
 friend class ReadOnlyIterator;
}; // template <class Alloc,class ElType,ulong MinIncrementSize,class InstanceId> class StaticCellArray
}; // namespace BK

//======================================================================
#endif
