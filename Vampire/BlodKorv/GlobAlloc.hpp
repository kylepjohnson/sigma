//
// File:         GlobAlloc.hpp
// Description:  Memory allocation front-end with debugging facilities.
// Created:      Mar 2, 2000, Alexandre Riazanov, riazanov@cs.man.ac.uk
// Revised:      Nov 28, 2001, Alexandre Riazanov, riazanov@cs.man.ac.uk
//               1) Some debugging stuff has been transferred to _allocator. 
//               2) allocateLarge and deallocateLarge are gone.
//====================================================
#ifndef GLOB_ALLOC_H
//====================================================
#define GLOB_ALLOC_H   
#include <cstdlib>
#include "jargon.hpp"
#include "BlodKorvDebugFlags.hpp"  
#include "ClassDesc.hpp"
#include "ObjDesc.hpp"
#include "AllocationFailureHandling.hpp"
#include "Allocator.hpp"
//================================================
#undef DEBUG_NAMESPACE
#ifdef DEBUG_ALLOC
#  define DEBUG_NAMESPACE "GlobAlloc"
#endif 
#include "debugMacros.hpp"
//=================================================

#define GLOB_ALLOC_CHUNK_SIZE 16000UL
#define GLOB_ALLOC_MAX_OBJECT_SIZE 1023UL
// ^ in machine words

namespace BK 
{

class GlobAlloc  
{
 public:
  class AllocationFailureHandlingId {};
  typedef AllocationFailureHandling<AllocationFailureHandlingId> AllocationFailureHandling;
 public:
  class InitStatic
  {
   public: 
    InitStatic();
    ~InitStatic();
   private:
    static long _count;
  };
 public:
  static void reset();
  static AllocationFailureHandling& basicAllocationFailureHandling() 
  { 
    static AllocationFailureHandling _basicAllocationFailureHandling;
    return _basicAllocationFailureHandling;
  }; 
  static void setHardMemoryLimit(long lim)
  { 
   CALL("setHardMemoryLimit(long lim)");
   allocator().setHardMemoryLimit(lim/(sizeof(void*)));
  };  
  static void setAllocationBufferSize(long abs) 
  { 
   allocator().setBufferSize(abs);  
  };
  static long occupiedByObjects() { return sizeof(void*)*allocator().occupiedByObjects(); };
  static long occupiedByChunks() { return sizeof(void*)*allocator().occupiedByChunks(); }; 
  static long freeInTheCurrentChunk() { return sizeof(void*)*allocator().freeInTheCurrentChunk(); };
  static long totalBufferDeficit() { return sizeof(void*)*allocator().totalBufferDeficit(); };
  static long activeBufferDeficit() { return sizeof(void*)*allocator().activeBufferDeficit(); };
  static void annulBufferDeficit() { allocator().annulBufferDeficit(); };

  static void* allocate(size_t size);
  static void* tryToAllocate(size_t size); 
  static void deallocate(void* obj,size_t size); 
#ifndef NO_DEBUG
  static void* allocate(size_t size,ClassDesc* classDesc); 
  static void* tryToAllocate(size_t size,ClassDesc* classDesc); 
  static void deallocate(void* obj,size_t size,ClassDesc* classDesc); 
#endif

  #ifdef DEBUG_ALLOC
   static ObjDesc* hostObj(const void* ptr) 
   { 
    return *(static_cast<ObjDesc**>(allocator().mirror(ptr)));
   };
  #endif

#ifndef NO_DEBUG
   static ostream& outputInternalStatistics(ostream& str)
     {
       return allocator().outputInternalStatistics(str);
     };
#endif

#ifdef DEBUG_ALLOC_OBJ_STATUS
  static ostream& outputPersistent(ostream& str,ulong maxNumOfObjects)
    {
      return allocator().outputPersistent(str,maxNumOfObjects);
    };
#endif
 
public: 
  typedef Allocator<AllocationFailureHandling,GLOB_ALLOC_MAX_OBJECT_SIZE,GLOB_ALLOC_CHUNK_SIZE> Alloc;

private:
  static Alloc& allocator()
  {
    //static Alloc _allocator;
    return _allocator;
  }; 
private:
  static Alloc _allocator;
  //static AllocationFailureHandling _basicAllocationFailureHandling; 
  friend class InitStatic;
}; // class GlobAlloc


static GlobAlloc::InitStatic globAllocInitStatic;


}; // namespace BK

//====================================================
#undef GLOB_ALLOC_CHUNK_SIZE
#undef GLOB_ALLOC_MAX_OBJECT_SIZE 
//====================================================
#endif
 
