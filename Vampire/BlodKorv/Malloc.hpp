//
// File:         Malloc.hpp
// Description:  Interface for "malloc" and "free".
// Created:      Feb 2, 2000, 17:10
// Author:       Alexandre Riazanov
// mail:         riazanov@cs.man.ac.uk
// Revised:      Mar 3, 2000, Alexandre Riazanov
//========================================================================
#ifndef MALLOC_H
//=============================================================================
#define MALLOC_H 
#include <cstdlib> 
#include "jargon.hpp"
#include "BlodKorvDebugFlags.hpp"
#include "ClassDesc.hpp"
#include "ObjDesc.hpp"
//================================================
#undef DEBUG_NAMESPACE
#ifdef DEBUG_MALLOC
 #define DEBUG_NAMESPACE "Malloc"
#endif 
#include "debugMacros.hpp"
//=================================================

namespace BK 
{
class Malloc
{
 public:
  static void* allocate(size_t size) 
  { 
   CALL("allocate(size_t size)");
   #ifndef DEBUG_ALLOC
    return malloc(size); 
   #else
    size_t nsize = size/sizeof(void*);
    if (size != nsize*sizeof(void*)) nsize++;
    ObjDesc* objDesc = static_cast<ObjDesc*>(malloc(nsize*sizeof(void*) + _prefSize));
    
    #ifdef DEBUG_ALLOC_OBJ_SIZE
     objDesc->size = size;
    #endif
    #ifdef DEBUG_ALLOC_OBJ_STATUS
     objDesc->status = ObjDesc::Persistent;       
    #endif
    #ifdef DEBUG_ALLOC_OBJ_TYPE
     objDesc->classDesc = ClassDesc::universalClassDesc();
    #endif
    #ifdef DEBUG_ALLOC_EVENT_ID
     objDesc->eventId = Debug::event();
    #endif
    return static_cast<void*>((static_cast<void**>(static_cast<void*>(objDesc))) + _nprefSize);
   #endif
  }; // void* allocate(size_t size) 
 
  static void deallocate(void* obj,size_t size) 
  {
   CALL("deallocate(void* obj,size_t size)");
   BK_CORRUPTMEM(obj,size);
   #ifndef DEBUG_ALLOC
    free(obj);
   #else
    ObjDesc* objDesc = hostObj(obj);
    #ifdef DEBUG_ALLOC_OBJ_SIZE
     ASSERT(objDesc->size == size);
    #endif
    #ifdef DEBUG_ALLOC_OBJ_STATUS
     ASSERT(objDesc->status == ObjDesc::Persistent);   
     objDesc->status = ObjDesc::Recycled;    
    #endif
    #ifdef DEBUG_ALLOC_OBJ_TYPE
     ASSERT(objDesc->classDesc == ClassDesc::universalClassDesc());
    #endif 
    #ifdef DEBUG_ALLOC_EVENT_ID
     objDesc->eventId = Debug::event();
    #endif
    #ifndef DEBUG_ALLOC_NO_REUSE
     free(static_cast<void*>(objDesc));
    #endif
   #endif
  }; //void deallocate(void* obj,size_t size)  


  #ifndef NO_DEBUG
   static void* allocate(size_t size,ClassDesc* classDesc) 
   { 
     CALL("allocate(size_t size,ClassDesc* classDesc)");
     void* res;
    #ifndef DEBUG_ALLOC
     res = malloc(size); 
    #else
     size_t nsize = size/sizeof(void*);
     if (size != nsize*sizeof(void*)) nsize++;
     ObjDesc* objDesc = 
       static_cast<ObjDesc*>(malloc(nsize*sizeof(void*) + _prefSize));        
     #ifdef DEBUG_ALLOC_OBJ_SIZE
      objDesc->size = size;
     #endif
     #ifdef DEBUG_ALLOC_OBJ_STATUS
      objDesc->status = ObjDesc::Persistent;       
     #endif
     #ifdef DEBUG_ALLOC_OBJ_TYPE
      objDesc->classDesc = classDesc;
     #endif
     #ifdef DEBUG_ALLOC_EVENT_ID
      objDesc->eventId = Debug::event();
     #endif 
     res = static_cast<void*>((static_cast<void**>(static_cast<void*>(objDesc))) + _nprefSize);
    #endif
    return res;
   }; // void* allocate(size_t size,ClassDesc* classDesc) 
 
   static void deallocate(void* obj,size_t size,ClassDesc* classDesc) 
   {
     CALL("deallocate(void* obj,size_t size,ClassDesc* classDesc");
     BK_CORRUPTMEM(obj,size);
    #ifndef DEBUG_ALLOC
     free(obj);             
    #else
     ObjDesc* objDesc = hostObj(obj);
     #ifdef DEBUG_ALLOC_OBJ_SIZE
      ASSERT(objDesc->size == size);
     #endif
     #ifdef DEBUG_ALLOC_OBJ_STATUS
      ASSERT(objDesc->status == ObjDesc::Persistent);   
      objDesc->status = ObjDesc::Recycled;    
     #endif
     #ifdef DEBUG_ALLOC_OBJ_TYPE
      ASSERT(objDesc->classDesc == classDesc);
     #endif
     #ifdef DEBUG_ALLOC_EVENT_ID
      objDesc->eventId = Debug::event();
     #endif 
     #ifndef DEBUG_ALLOC_NO_REUSE
      free((void*)objDesc);
     #endif
    #endif
   }; //void deallocate(void* obj,size_t size,ClassDesc* classDesc)
  #endif 

  #ifdef DEBUG_ALLOC
   static ObjDesc* hostObj(void* obj) 
   {  
    return static_cast<ObjDesc*>(static_cast<void*>(((void**)obj) - _nprefSize));
   };
  #endif
 private:
  #ifdef DEBUG_ALLOC
   static const size_t _prefSize; // = sizeof(ObjDesc);
   static const size_t _nprefSize; // = sizeof(ObjDesc)/sizeof(void*);
  #endif
}; // class Malloc



}; // namespace BK

//================================================
#undef DEBUG_NAMESPACE
//============================================================================
#endif
