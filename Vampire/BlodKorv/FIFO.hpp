//
// File:         FIFO.hpp
// Description:  Queues of unrestricted capacity.
// Created:      Feb 16, 2000, 14:35
// Author:       Alexandre Riazanov
// mail:         riazanov@cs.man.ac.uk
// Revised:      Apr 17, 2001, a bug fixed in removeFirst, AR
//============================================================================
#ifndef FIFO_H
//=============================================================================
#define FIFO_H 
#include "jargon.hpp"
#include "BlodKorvDebugFlags.hpp"
#include "DestructionMode.hpp"
//============================================================================
#undef DEBUG_NAMESPACE
#ifdef DEBUG_FIFO
 #define DEBUG_NAMESPACE "FIFO<class Alloc,class ElType,class InstanceId>"
#endif
#include "debugMacros.hpp"
//============================================================================
namespace BK 
{

template <class Alloc,class ElType,class InstanceId>
class FIFO
{
 public:
  class Entry
  {
   public:    
    Entry(const ElType& v) : _value(v), _next(0) 
    {
    };
    ~Entry() {};
    const ElType& value() const { return _value; };
    ElType& value() { return _value; }; 
    const Entry* next() const { return const_cast<const Entry*>(_next); };
    Entry*& next() { return _next; };
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
  };
 public:
  FIFO() : 
    _first(0), 
    _last(0)
#ifdef DEBUG_NAMESPACE
    , _name("?")
#endif
  {
   CALLM(_name,"constructor FIFO()"); 
  }; 
  FIFO(const char* name) : 
    _first(0), 
    _last(0)
#ifdef DEBUG_NAMESPACE
    , _name(name)
#endif
  {
    CALLM(_name,"constructor FIFO(const char* name)");
  };
  ~FIFO() 
    { 
      CALLM(_name,"destructor ~FIFO()"); 
      destroy(); 
    };
  void init()
  {
    CALLM("?","init()");
    _first = 0; 
    _last = 0; 
  };
  void init(const char* name)
  {
    CALLM(name,"init()");
    _first = 0; 
    _last = 0;
#ifdef DEBUG_NAMESPACE
    _name = name;
#endif 
  };

  void destroy() 
  {
    CALLM(_name,"destroy()");
    if (DestructionMode::isThorough())
      {
	while (_first)
	  {
	    Entry* tmp = _first->next();
	    delete _first;
	    _first = tmp;
	  };
      }; 
    BK_CORRUPT(*this);
  };

  void reset() 
  { 
    CALLM(_name,"reset()");
#ifdef DEBUG_NAMESPACE
    const char* name = _name;
    destroy();
    init(name);
#else    
    destroy();
    init("?");
#endif

  };
  operator bool() const { return _last; };
  bool empty() const { return !_last; };
  bool nonempty() const { return _last; };
  ulong count() const
  {
   ulong res = 0;
   for (const Entry* e = begin(); e; e = e->next()) res++;
   return res;
  };

  bool contains(const ElType& x) const
    {
      CALLM(_name,"contains(const ElType& x) const");
      for (const Entry* e = begin(); e; e = e->next())
	if (e->value() == x) return true;      
      return false;
    };

  void enqueue(const ElType& x)
  {
   CALLM(_name,"enqueue(const ElType& x)");
   if (!_last) // empty queue
    {
     ASSERT(!_first);
     _first = (_last = new Entry(x));
     ASSERT(_first);
    }
   else // nonempty queue
    {
     ASSERT(_first);
     _last->next() = new Entry(x);
     _last = _last->next();
     ASSERT(_last);
    };
  }; // void enqueue(const ElType& x)

  bool dequeue(ElType& x)
  {
   CALLM(_name,"dequeue(ElType& x)");
   if (_first)
    { 
     ASSERT(_last);
     x = _first->value();
     Entry* tmp = _first->next();    
     delete _first;
     _first = tmp;
     if (!_first) _last = 0; // was only one element
     return true;
    };
   // empty queue
   ASSERT(!_last);
   return false;
  }; // bool dequeue(ElType& x)

  const Entry* begin() const { return const_cast<const Entry*>(_first); };
  Entry* begin() { return _first; };

  void removeFirst(const ElType& x)
  {
   CALLM(_name,"removeFirst(const ElType& x)");
   if (_first)
    {
     if (_first->value() == x) 
      {      
       Entry* tmp = _first->next(); 
       delete _first;
       _first = tmp;
       if (!_first) _last = 0;
      }
     else
      {
       Entry* prev = _first;
       Entry* curr = _first->next();
       while (curr) 
	{
	 if (curr->value() == x)
	  {
	   prev->next() = curr->next();
           if (_last == curr) _last = prev;
           delete curr;         
           return; 
          };
         prev = curr;
         curr = curr->next(); 
        };
      };
    };   
  }; // void removeFirst(const ElType& x)

  #ifndef NO_DEBUG
   bool checkIntegrity() const
   { 
     return (_first && _last) || ((!_first) && (!_last));  
   };
  #endif
 private:
  FIFO(const FIFO&) {}; 
 private:
  Entry* _first;
  Entry* _last;
#ifdef DEBUG_NAMESPACE
  const char* _name;
#endif
}; // template <class Alloc,class ElType,class InstanceId> class FIFO

}; // namespace BK


//======================================================================
#endif
