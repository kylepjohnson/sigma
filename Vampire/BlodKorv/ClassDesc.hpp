//
// File:         ClassDesc.hpp
// Description:  Structure for class descriptors used in dynamic type control.
// Created:      Mar 2, 2000, 16:20
// Author:       Alexandre Riazanov
// mail:         riazanov@cs.man.ac.uk
//====================================================
#ifndef NO_DEBUG
#ifndef CLASS_DESC_H
//====================================================
#define CLASS_DESC_H
#include <cstdlib>
#include <climits>
#include <iostream>
#include "jargon.hpp"
#include "Debug.hpp"
//====================================================
namespace BK 
{

class ClassDesc
{
 public: 
  class SizeAllocDeallocList
  {
   public: 
    ulong size() const { return _size; };
    long numOfAllocated() const { return _numOfAllocated; };
    long numOfDeallocated() const { return _numOfDeallocated; };
    long numOfPersistent() const { return _numOfAllocated - _numOfDeallocated; };
    const SizeAllocDeallocList* next() const { return _next; };
   private: 
    SizeAllocDeallocList(ulong size,SizeAllocDeallocList* nxt) 
     : _size(size), 
      _numOfAllocated(0),
      _numOfDeallocated(0),
      _next(nxt)
    {
    };
    void* operator new(size_t) 
    {
     void* res = ::malloc(sizeof(SizeAllocDeallocList));  
     if (!res) 
      Debug::debugFeatureInternalFault("!ClassDesc::SizeAllocDeallocList::operator new(size_t)::res");
     return res; 
    };
   private: 
    ulong _size;
    long _numOfAllocated; 
    long _numOfDeallocated;
    SizeAllocDeallocList* _next;
   friend class ClassDesc;
  };

  class List
  {
   public:
    const ClassDesc* hd() const { return _classDesc; };
    const List* next() const { return _next; }; 
   private:
    List(ClassDesc* cd,List* nxt) : _classDesc(cd), _next(nxt) {};
    void* operator new(size_t) 
    { 
     void* res = malloc(sizeof(List));  
     if (!res) 
      Debug::debugFeatureInternalFault("!ClassDesc::List::operator new(size_t)::res");
     return res; 
    };
    ClassDesc* _classDesc;
    List* _next;
   friend class ClassDesc;
  };  

 public: 
  ClassDesc(const char* nm) 
   : _name(nm), 
     _registeredObjects(0),
     _minSize(0UL),
     _maxSize(ULONG_MAX)
  {
   _registered = new List(this,_registered);
  }; 
  ClassDesc(const char* nm,ulong minSize,ulong maxSize) 
   : _name(nm), 
     _registeredObjects(0),
     _minSize(minSize),
     _maxSize(maxSize)
  {
   _registered = new List(this,_registered);
  }; 
  const char* name() const { return _name; };
  ulong minSize() const { return _minSize; };
  ulong maxSize() const { return _maxSize; };
  static ClassDesc* universalClassDesc() { return &_universal; };
  bool registerAllocated(ulong size);
  bool registerDeallocated(ulong size);
  const SizeAllocDeallocList* registeredObjects() const { return _registeredObjects; };
  static const List* registeredClasses() { return _registered; };
  static std::ostream& outputAll(std::ostream& str,bool objectStatistics); 
 private:
  const char* _name;
  SizeAllocDeallocList* _registeredObjects;
  ulong _minSize;
  ulong _maxSize;  
  static List* _registered;
  static ClassDesc _universal;
 friend class InitStatic;
}; // class ClassDesc

inline bool ClassDesc::registerAllocated(ulong size)
{
 SizeAllocDeallocList** nextAddr = &_registeredObjects;
 while (*nextAddr)
  {
   if ((*nextAddr)->size() < size) 
    {
     nextAddr = &((*nextAddr)->_next);
    }
   else 
    if ((*nextAddr)->size() == size)
     {
      (*nextAddr)->_numOfAllocated++; 
      return (size >= _minSize) && (size <= _maxSize);
     }
    else // (*nextAddr)->size() > size
     {
      goto insert_new; 
     };
  };
 insert_new: 
  *nextAddr = new SizeAllocDeallocList(size,*nextAddr);
  (*nextAddr)->_numOfAllocated++;
  return (size >= _minSize) && (size <= _maxSize);
}; // bool ClassDesc::registerAllocated(ulong size)
 
inline bool ClassDesc::registerDeallocated(ulong size)
{
 // returns false if the number of deallocated exceeds the number of allocated
 SizeAllocDeallocList** nextAddr = &_registeredObjects;
 while (*nextAddr)
  {
   if ((*nextAddr)->size() < size) 
    {
     nextAddr = &((*nextAddr)->_next);
    }
   else 
    if ((*nextAddr)->size() == size)
     {
      (*nextAddr)->_numOfDeallocated++;
      return ((*nextAddr)->numOfAllocated() >= (*nextAddr)->numOfDeallocated());

     }
    else // (*nextAddr)->size() > size
     {
      goto insert_new; 
     };
  };
 insert_new: 
  *nextAddr = new SizeAllocDeallocList(size,*nextAddr);
  (*nextAddr)->_numOfDeallocated++;
  return false;
}; // bool ClassDesc::registerDeallocated(ulong size)

}; // namespace BK

namespace std
{
inline ostream& operator<<(ostream& str,const BK::ClassDesc& cd)
{
 return str << cd.name();
};
};

//====================================================
#endif
#endif
