//
// File:         Array.hpp
// Description:  Expanding arrays for relocatable data.
// Created:      Jan 30, 2000, Alexandre Riazanov, riazanov@cs.man.ac.uk
// Revised:      Dec 17, 2001,
//               Semantics of the copy constructor and assignment changed.  
// Note:         Semantics of many operations is different from
//               the one for conventional arrays. 
//               The elements are not destroyed upon destruction of an array
//               or upon assignment.   
//============================================================================
#ifndef ARRAY_H
//=============================================================================
#define ARRAY_H 
#include <cstdlib>
#include "jargon.hpp"
#include "BlodKorvDebugFlags.hpp"
#ifdef DEBUG_ALLOC
 #include "ObjDesc.hpp"
#endif
#include "DestructionMode.hpp"
//============================================================================
#undef DEBUG_NAMESPACE
#ifdef DEBUG_ARRAY
#  define DEBUG_NAMESPACE "Array<class Alloc,class ElType,ulong MinIncrementSize,class InstanceId>"
#endif
#include "debugMacros.hpp"
//============================================================================

namespace BK 
{

template <class Alloc,class ElType,ulong MinIncrementSize,class InstanceId>
class Array
{
 public:
  Array() { init(); };
  Array(const char* nm) { init(nm); };
  Array(const ElType& initElemVal,int /* dummy */) { init(initElemVal,0); };
  Array(const ElType& initElemVal,const char* nm) { init(initElemVal,nm); };

  Array(const Array& a) { init(a); };

  Array& operator=(const Array& a)
  {
   CALLM(_name,"operator=(const Array& a)");
   destroy();
   _currSize = a._currSize;   
   _initialElementValue = a._initialElementValue; 
   #ifdef DEBUG_NAMESPACE
    _name = a._name;
   #endif
   if (_currSize)
    {
     _memory = 
       static_cast<ElType*>(Alloc::allocate(sizeof(ElType)*_currSize));
     for (ulong j = 0; j < _currSize; j++) _memory[j] = a._memory[j]; 
    }
   else 
    _memory = 0;
   #ifndef NO_DEBUG
    _debugFrozen = a._debugFrozen;
   #endif
   return *this;
  };
  
  ~Array() 
    { 
      CALLM(_name,"destructor ~Array()"); 
      destroy();
    };

  void init() 
  {
   CALLM(_name,"init()");
   _memory = 0;
   _currSize = 0UL;
   #ifdef DEBUG_NAMESPACE
    _name = "?";
   #endif
   #ifndef NO_DEBUG
    _debugFrozen = false;
   #endif
  };
  void init(const char* nm)
  {
   CALLM(nm,"init(const char* nm)");
   _memory = 0;
   _currSize = 0UL;
   #ifdef DEBUG_NAMESPACE
    _name = nm;
   #endif
   #ifndef NO_DEBUG
    _debugFrozen = false;
   #endif
  };
  void init(const ElType& initElemVal,int /* dummy */)
  {
   CALLM("?","init(const ElType& initElemVal,int /* dummy */)");
   _initialElementValue = initElemVal;
   _memory = 0;
   _currSize = 0UL;
   #ifdef DEBUG_NAMESPACE
    _name = "?";
   #endif
   #ifndef NO_DEBUG
    _debugFrozen = false;
   #endif
  };  
  void init(const ElType& initElemVal,const char* nm)
  {
   CALLM(nm,"init(const ElType& initElemVal,const char* nm)");
   _initialElementValue = initElemVal;
   _memory = 0;
   _currSize = 0UL;
   #ifdef DEBUG_NAMESPACE
    _name = nm;
   #endif
   #ifndef NO_DEBUG
    _debugFrozen = false;
   #endif
  };     

  void init(const Array& a) 
  {
   CALLM("?","init(const Array& a)");
   _currSize = a._currSize;
   _initialElementValue = a._initialElementValue;
   #ifdef DEBUG_NAMESPACE
    _name = a._name;
   #endif
   if (_currSize)
    {
     _memory = 
       static_cast<ElType*>(Alloc::allocate(sizeof(ElType)*_currSize));
     for (ulong j = 0; j < _currSize; j++) _memory[j] = a._memory[j]; 
    }
   else 
    _memory = 0;
   #ifndef NO_DEBUG
    _debugFrozen = false;
   #endif
  };

  void destroy()
  {
    CALLM(_name,"destroy()"); 
    if (DestructionMode::isThorough())
      if (_currSize) 
	Alloc::deallocate(static_cast<void*>(_memory),sizeof(ElType)*_currSize);
    BK_CORRUPT(*this);
  };


  void* operator new(size_t) 
  {
   CALL("operator new (size_t)");
   return Alloc::allocate(sizeof(Array));
  };

  void operator delete(void* obj) 
  {
   CALL("operator delete(void* obj)");
   Alloc::deallocate(obj,sizeof(Array)); 
  };

  const ElType* memory() const { return _memory; };
  ElType* memory() { return _memory; };
  ElType* endOfMemory() { return _memory + _currSize; };

  ElType& operator[](ulong i) // unsafe but efficient
  {
   CALLM(_name,"operator[](ulong i)");  
   ASSERT(i < _currSize);
   COP("COP10");
   return _memory[i];
  };

  ElType& operator[](ulong i) const // unsafe but efficient
  {
   CALLM(_name,"operator[](ulong i) const");  
   ASSERT(i < _currSize);
   COP("COP10");
   return _memory[i];
  }; 

  
  ElType& sub(ulong i) // safe but inefficient
  {
   CALLM(_name,"ElType& sub(ulong i)");
   #ifndef NO_DEBUG
    if (_debugFrozen) Debug::error("sub(..) called on frozen array.");
   #endif   
   if (i >= _currSize) expand(i);
   ASSERT(i < _currSize);
   return _memory[i];
  };

  void expand(ulong i)
  {
   CALLM(_name,"expand(ulong i)");

   #ifndef NO_DEBUG
    if (_debugFrozen) Debug::error("Frozen array expanded.");
   #endif   

   if (i < _currSize) return;

   ulong newSize = (i/MinIncrementSize)*MinIncrementSize;
   if (i >= newSize) newSize += MinIncrementSize; 
   ASSERT(i < newSize);
   ElType* newMemory = 
     static_cast<ElType*>(Alloc::allocate(sizeof(ElType)*newSize));
   if (_currSize)
    {
     for (ulong j = 0; j < _currSize; j++) newMemory[j] = _memory[j];
    };
   for (ulong k = _currSize; k < newSize; k++) newMemory[k] = _initialElementValue;
   if (_currSize) Alloc::deallocate(static_cast<void*>(_memory),sizeof(ElType)*_currSize);
   _currSize = newSize;  
   _memory = newMemory;
  }; // void expand(ulong i)

  void expand()
  {
   CALLM(_name,"expand()");
   #ifndef NO_DEBUG
    if (_debugFrozen) Debug::error("Frozen array expanded.");
   #endif   
   ulong newSize = _currSize + MinIncrementSize;
   ElType* newMemory = 
     static_cast<ElType*>(Alloc::allocate(sizeof(ElType)*newSize));
   if (_currSize)
    {
     for (ulong j = 0; j < _currSize; j++) newMemory[j] = _memory[j];
    };
   for (ulong k = _currSize; k < newSize; k++) newMemory[k] = _initialElementValue;
   if (_currSize) Alloc::deallocate(static_cast<void*>(_memory),sizeof(ElType)*_currSize);
   _currSize = newSize;  
   _memory = newMemory;
  }; // void expand()

  ulong size() const { return _currSize; };

  #ifndef NO_DEBUG
   void freeze() { _debugFrozen = true; };
   void unfreeze() { _debugFrozen = false; };
   bool checkObj()
   {
    #ifdef DEBUG_ALLOC 
     if (_currSize)
      {
       ObjDesc* objDesc = Alloc::hostObj(static_cast<void*>(_memory));
       if (!objDesc) return false;
       #ifdef DEBUG_ALLOC_OBJ_SIZE
        if (objDesc->size <= 0) return false; 
       #endif
       #ifdef DEBUG_ALLOC_OBJ_STATUS
        if (objDesc->status != ObjDesc::Persistent) return false;
       #endif
      };
     #endif
    return true;
   };
  #endif
  
 private:
  ElType* _memory;
  ulong _currSize;   
  ElType _initialElementValue; 
  #ifdef DEBUG_NAMESPACE
   const char* _name;
  #endif
  #ifndef NO_DEBUG
   bool _debugFrozen;
  #endif
}; // template <class Alloc,class ElType,ulong MinIncrementSize,class InstanceId> class Array

}; // namespace BK

//======================================================================
#undef DEBUG_NAMESPACE
//======================================================================
#endif
