//
// File:         Allocator.hpp
// Description:  Raw memory allocation.
// Created:      Mar 2, 2000, Alexandre Riazanov, riazanov@cs.man.ac.uk
// Revised:      Nov 28, 2001,  Alexandre Riazanov, riazanov@cs.man.ac.uk
//               Completely reimplemented. In particular, some 
//               debugging stuff dealing with ObjDesc now added.
// Revised:      Dec 01, 2001, Alexandre Riazanov, riazanov@cs.man.ac.uk
//               1) tryToAllocate() added instead of using
//               the flag FailureHandling::returnNullOnFailure().
//               2) Two bugs fixed in getBestFreeLarge(..)
// Revised:      Dec 03, 2001, Alexandre Riazanov, riazanov@cs.man.ac.uk 
//               Buffer statistics reimplemented.
// Revised:      Dec 10, 2001, Alexandre Riazanov, riazanov@cs.man.ac.uk
//               Another bug in getBestFreeLarge(..) fixed.
// Revised:      May 31, 2001, Alexandre Riazanov, riazanov@cs.man.ac.uk
//               Bufferisation reimplemented. 
//====================================================
#ifndef ALLOCATOR_H
//====================================================
#define ALLOCATOR_H
#include <cstdlib>
#include <climits> 
#include "jargon.hpp"
#include "RuntimeError.hpp"
#include "Exit.hpp"
#include "BlodKorvDebugFlags.hpp"
#include "Debug.hpp"
#ifdef DEBUG_ALLOC
#include "ObjDesc.hpp"
#endif
#include "BufferedAllocationStatistics.hpp"
using namespace std;
//================================================
#undef DEBUG_NAMESPACE
#ifdef DEBUG_ALLOCATOR
#define DEBUG_NAMESPACE "Allocator<FailureHandling,MaxSmallObjectSize,StandardChunkSize>"
#endif 
#include "debugMacros.hpp"
//=================================================

#ifdef DEBUG_ALLOC
#define DEBUG_ALLOC_TOTAL_MEMORY_SIZE_IN_WORDS  33554432
// 128 Mb  
// 8388608
// 32Mb
// 33554432
// 128 Mb
// 16777216
// 64Mb
#define DEBUG_ALLOC_TOTAL_MEMORY_SIZE_DECREMENT_IN_WORDS 262144
// 1Mb
#endif


namespace BK 
{
  
  template <class FailureHandling,ulong MaxSmallObjectSize,ulong StandardChunkSize>
  class Allocator
  {
  public:   
    Allocator() 
    {
      CALL("constructor Allocator()");
      ASSERT(MaxSmallObjectSize <= StandardChunkSize);
 
      _freeLarge = 0;  
      _allChunks = 0; 
      _availableChunks = 0; 
      _nextInCurrentChunk = 0;  
      _freeInCurrChunk = 0UL;
      _occupiedByChunks = 0L;
      _hardMemoryLimit = LONG_MAX;
      _unbufferedAllocationLimit = LONG_MAX;
      for (ulong i = 0; i <= MaxSmallObjectSize; i++) 
	{
	  _freeList[i] = 0;
	  DOP(_free[i] = 0L);
	  DOP(_persistent[i] = 0L);
	};
      DOP(_sizeOfFreeLarge = 0L);
      DOP(_sizeOfPersistentLarge = 0L);


#ifdef DEBUG_ALLOC
      _debugWithMirrorMemorySize = DEBUG_ALLOC_TOTAL_MEMORY_SIZE_IN_WORDS; 
    request_all_memory:   
      _debugWithMirrorFreeMemory = static_cast<void**>(malloc(_debugWithMirrorMemorySize*sizeof(void*)));
      if (!_debugWithMirrorFreeMemory) 
	{     
	  if (_debugWithMirrorMemorySize < DEBUG_ALLOC_TOTAL_MEMORY_SIZE_DECREMENT_IN_WORDS)
	    Debug::debugFeatureInternalFault("_debugWithMirrorMemorySize < DEBUG_ALLOC_TOTAL_MEMORY_SIZE_DECREMENT_IN_WORDS");

	  _debugWithMirrorMemorySize -= DEBUG_ALLOC_TOTAL_MEMORY_SIZE_DECREMENT_IN_WORDS; 
	  goto request_all_memory;
	};
      _debugWithMirrorHalfMemorySize = _debugWithMirrorMemorySize/2;
      _debugMirror = 
	static_cast<ObjDesc**>(static_cast<void*>(_debugWithMirrorFreeMemory));
      _debugMirrorEnd = _debugMirror + _debugWithMirrorHalfMemorySize;
      for (ulong j = 0; j < _debugWithMirrorHalfMemorySize; j++)
	{
	  _debugMirror[j] = 0;
	};
      _debugWithMirrorEndOfFreeMemory = _debugWithMirrorFreeMemory + _debugWithMirrorMemorySize;
      _debugWithMirrorFreeMemory += _debugWithMirrorHalfMemorySize; 
#endif
    }; // Allocator() 
 
    ~Allocator() 
    { 
      CALL("destructor ~Allocator()");
      BK_CORRUPT(_freeList);
    };
    

    void reset()
    {
      CALL("reset()");
      _freeLarge = 0;  
      _availableChunks = _allChunks;
      _nextInCurrentChunk = 0;  
      _freeInCurrChunk = 0UL;
      _occupiedByChunks = 0L;
      _hardMemoryLimit = LONG_MAX;
      _unbufferedAllocationLimit = LONG_MAX;
      for (ulong i = 0; i <= MaxSmallObjectSize; i++) 
	{
	  _freeList[i] = 0;
	  DOP(_free[i] = 0L);
	  DOP(_persistent[i] = 0L);
	};
      DOP(_sizeOfFreeLarge = 0L);
      DOP(_sizeOfPersistentLarge = 0L);
      _statistics.reset();

#ifdef DEBUG_ALLOC
      ulong j = 0;
      while (j < _debugWithMirrorHalfMemorySize)
	{
	  if (_debugMirror[j])
	    {
	      ObjDesc* p = _debugMirror[j];
	      delete p;
	      _debugMirror[j] = 0;
	      ++j;
	      while ((j < _debugWithMirrorHalfMemorySize) &&
		     (_debugMirror[j] == p))
		{
		  _debugMirror[j] = 0;
		  ++j;
		};

	    }
	  else
	    ++j;
	};
#endif
    }; // void reset()



    void setHardMemoryLimit(long lim)
    {
      CALL("setHardMemoryLimit(long lim)");
      _hardMemoryLimit = lim;
    }; 
    void setBufferSize(long percentage) 
    {
      CALL("setBufferSize(long percentage)");
      if (percentage > 0)    
	{
	  if (percentage < 100)
	    {
	      _unbufferedAllocationLimit = (long)(_hardMemoryLimit * (1 - (((float)percentage)/100)));       
	    }
	  else _unbufferedAllocationLimit = 0L;
	  _statistics.setSizeOfNonbufferMemory(_unbufferedAllocationLimit);
	}
      else 
	{
	  _unbufferedAllocationLimit = _hardMemoryLimit;     
	  _statistics.setSizeOfNonbufferMemory(LONG_MAX); // no buffer
	};
    };

    void* allocate(ulong size)
    {
      CALL("allocate(ulong size)");  

      void* res;
      if (size > MaxSmallObjectSize) 
	{
	  res = allocateLarge(size);
	  BK_CORRUPTMEM(res,size);
	  goto return_res;
	};   

      res = tryToAllocate(size);
      if (!res)
	{
	  FailureHandling::reportUnrecoverableAllocationFailure(size);
	  RuntimeError::report("unhandled unrecoverable allocation failure");
	  Exit::exit(1);
	};

    return_res: 
      // The only return in the function, easy to intercept.
      return res;
    }; // void* allocate(ulong size)


    void* tryToAllocate(ulong size)
    {
      CALL("tryToAllocate(ulong size)");  
      ASSERT(size);

      if (size > MaxSmallObjectSize) return tryToAllocateLarge(size);

      void* res; 

    take_from_free_list:
      res = allocateFromFreeList(size);
      if (res) 
	{
#ifdef DEBUG_ALLOC
	  ObjDesc* objDesc = *(static_cast<ObjDesc**>(mirror(res)));
	  ASSERT(objDesc);
#ifdef DEBUG_ALLOC_OBJ_PTR
	  ASSERT(objDesc->objPtr == res);
#endif
#ifdef DEBUG_ALLOC_OBJ_STATUS    
	  ASSERT(objDesc->status == ObjDesc::Recycled);
	  objDesc->status = ObjDesc::Persistent;
#endif
#ifdef DEBUG_ALLOC_EVENT_ID
	  objDesc->eventId = Debug::event();
#endif
#endif
	  DOP(++_persistent[size]);
	  ASSERT(_persistent[size] == _statistics.persistent(size));
	  ASSERT(_free[size] == _statistics.free(size));
	  BK_CORRUPTMEM(res,size);
	  return res;
	};

    take_from_current_chunk:
      res = allocateFromCurrentChunk(size);
      if (res) 
	{
	  DOP(++_persistent[size]);
	  ASSERT(_persistent[size] == _statistics.persistent(size));
	  BK_CORRUPTMEM(res,size);
	  return res;
	};

      if (createNewChunk())
	{
	  goto take_from_current_chunk;
	};
    
      if (useFreeLargeAsChunk()) 
	{
	  goto take_from_current_chunk; 
	};
    
#ifndef DEBUG_ALLOC_NO_REUSE
      if (useFreeAsChunk(size)) 
	{
	  goto take_from_current_chunk;  
	};
#endif
  
      if (FailureHandling::requestMemoryRelease(size)) goto take_from_free_list;
      
      return 0;
    }; // void* tryToAllocate(ulong size)



    void deallocate(void* obj,ulong size)
    {
      CALL("deallocate(void* obj,ulong size)");
      if (size > MaxSmallObjectSize) 
	{
	  deallocateLarge(obj,size);
	}
      else
	{ 
	  BK_CORRUPTMEM(obj,size);
	  ASSERT(size);
	  pushIntoFreeList(obj,size);

#ifdef DEBUG_ALLOC
	  ObjDesc* objDesc = *(static_cast<ObjDesc**>(mirror(obj)));
	  ASSERT(objDesc);
#ifdef DEBUG_ALLOC_OBJ_PTR
	  ASSERT(objDesc->objPtr == obj);
#endif
#ifdef DEBUG_ALLOC_OBJ_STATUS    
	  ASSERT(objDesc->status == ObjDesc::Persistent);
	  objDesc->status = ObjDesc::Recycled;
#endif
#ifdef DEBUG_ALLOC_EVENT_ID
	  objDesc->eventId = Debug::event();
#endif
#endif         
	  _statistics.registerDeallocationIntoFreeList(size);
	  DOP(--_persistent[size]);
	  ASSERT(_persistent[size] == _statistics.persistent(size));
	  ASSERT(_free[size] == _statistics.free(size));
	};
    }; // void deallocate(void* obj,ulong size)

    long occupiedByObjects() const 
    { 
      return _statistics.totalSizeOfPersistent();
    };
    long occupiedByChunks() const { return _occupiedByChunks; };
    long freeInTheCurrentChunk() const { return (long)_freeInCurrChunk; };
    long totalBufferDeficit() const 
    { 
      return _statistics.countTotalSizeOfBufferDeficit(); // heavy!
    };
    long activeBufferDeficit() const 
    {
      return _statistics.totalSizeOfActiveBufferDeficit();
    }; 
    void annulBufferDeficit() 
    {
      CALL("annulBufferDeficit()");
      _statistics.suspendActiveBufferDeficit();
    }; // void annulBufferDeficit() 

#ifndef NO_DEBUG
    void** chunk(void* obj) const
    {
      void** chunk = _allChunks;
      while (chunk)
	{
	  if (inChunk(obj,chunk)) return chunk;
	  chunk = static_cast<void**>(chunk[0]);
	}; 
      return 0;              
    };
#endif

#ifdef DEBUG_ALLOC
    void* mirror(const void* ptr)
    {
      return 
	static_cast<void*>
	((static_cast<void**>
	  (const_cast<void*>(ptr))) - _debugWithMirrorHalfMemorySize);
    }; 
#endif


#ifndef NO_DEBUG
    ostream& outputInternalStatistics(ostream& str) const 
    {
      str << "**** ALLOCATOR " << (long)this << " INTERNAL STATISTICS: ****\n";
      _statistics.output(str);
      str << "**** END OF ALLOCATOR " << (long)this << " INTERNAL STATISTICS: ****\n";
      return str;
    };

#endif

#ifdef DEBUG_ALLOC_OBJ_STATUS
    ostream& outputPersistent(ostream& str,ulong maxNumOfObjects)
    {
      CALL("outputPersistent(ostream& str,ulong maxNumOfObjects)");
      ObjDesc** p = _debugMirror;
      while (maxNumOfObjects && (p < _debugMirrorEnd))
	{	
	  ObjDesc* objDesc = *p;
	  if (objDesc)
	    {
	      if (objDesc->status == ObjDesc::Persistent)
		{
		  str << *objDesc << "\n";
		  --maxNumOfObjects;
		};
	      // skip the object 
	      do
		{
		  ++p;
		}
	      while ((p < _debugMirrorEnd) && ((*p) == objDesc));
	    }
	  else
	    ++p;  
	};
      return str;
    }; // ostream& outputPersistent(ostream& str,ulong maxNumOfObjects)
  
#endif



  private:
#ifdef DEBUG_ALLOC
    void* debugWithMirrorMalloc(size_t size)
    {
      CALL("debugWithMirrorMalloc(size_t size)");   
      if (_debugWithMirrorFreeMemory + size > _debugWithMirrorEndOfFreeMemory) return 0;
      void* res = _debugWithMirrorFreeMemory;
      _debugWithMirrorFreeMemory += size;
      return res; 
    };   
#endif

    void* allocateLarge(ulong size)
    {
      CALL("allocateLarge(ulong size)");
      ASSERT(size > MaxSmallObjectSize); 
      void* res = tryToAllocateLarge(size);
      if (res) return res;
      // Nothing to do here, give up
      FailureHandling::reportUnrecoverableAllocationFailure(size);
      RuntimeError::report("unhandled unrecoverable allocation failure");
      Exit::exit(1);
      return 0; // To suppress compiler warnings   
    }; // void* allocateLarge(ulong size)




    void* tryToAllocateLarge(ulong size)
    {
      CALL("tryToAllocateLarge(ulong size)");
      ASSERT(size > MaxSmallObjectSize); 
      // first try to find the best match among the freed large pieces
      void* res;
      ulong freeLargeSize;
    take_from_free_large: 
      res = getBestFreeLarge(size,freeLargeSize);
      if (res) 
	{	  
	  DOP(_sizeOfFreeLarge -= freeLargeSize);
	  ASSERT(freeLargeSize >= size); 
#ifdef DEBUG_ALLOC
	  ObjDesc* objDesc = *(static_cast<ObjDesc**>(mirror(res)));
	  ASSERT(objDesc);
#ifdef DEBUG_ALLOC_OBJ_PTR
	  ASSERT(objDesc->objPtr == res);
#endif
#ifdef DEBUG_ALLOC_OBJ_STATUS    
	  ASSERT(objDesc->status == ObjDesc::Recycled);
#endif
#endif
 
	  ulong sizeOfRest = freeLargeSize - size;
	  if (sizeOfRest)
	    {
	      void** rest = static_cast<void**>(res) + size;
	      if (sizeOfRest <= MaxSmallObjectSize)
		{
		  pushIntoFreeList(static_cast<void*>(rest),sizeOfRest);         
		}
	      else // the rest is still large
		{
		  rest[0] = static_cast<void*>(_freeLarge);
		  rest[1] = reinterpret_cast<void*>(sizeOfRest);
		  _freeLarge = rest;
		  DOP(_sizeOfFreeLarge += sizeOfRest);
		};

#ifdef DEBUG_ALLOC
	      ObjDesc* newObjDesc = new ObjDesc(); 
	      for (ulong n = 0; n < sizeOfRest; n++)
		(static_cast<ObjDesc**>(mirror(static_cast<void*>(rest))))[n] = newObjDesc;     
#ifdef DEBUG_ALLOC_OBJ_PTR
	      newObjDesc->objPtr = static_cast<void*>(rest);
#endif
#ifdef DEBUG_ALLOC_OBJ_STATUS
	      newObjDesc->status = ObjDesc::Recycled;
#endif
#ifdef DEBUG_ALLOC_EVENT_ID
	      newObjDesc->eventId = Debug::event();
#endif
#endif 
	    }; // if (sizeOfRest) ...

#ifdef DEBUG_ALLOC
#ifdef DEBUG_ALLOC_OBJ_STATUS    
	  objDesc->status = ObjDesc::Persistent;
#endif
#ifdef DEBUG_ALLOC_EVENT_ID
	  objDesc->eventId = Debug::event();
#endif
#endif
	  DOP(_sizeOfPersistentLarge += size);
	  _statistics.registerAllocationFromFreeLarge(size,sizeOfRest);
	  ASSERT((sizeOfRest > MaxSmallObjectSize) || (_free[sizeOfRest] == _statistics.free(sizeOfRest)));
	  ASSERT(_sizeOfFreeLarge == _statistics.sizeOfFreeLarge());
	  ASSERT(_sizeOfPersistentLarge == _statistics.sizeOfPersistentLarge());
	  return res;      
	};

      // No recycled large piece of appropriate size exists.
      // Try the current chunk.
    take_from_current_chunk:
      res = allocateFromCurrentChunk(size);
      if (res) 
	{
	  DOP(_sizeOfPersistentLarge += size);
	  ASSERT(_sizeOfPersistentLarge == _statistics.sizeOfPersistentLarge());
	  return res;
	};

      // Not enough space in the current chunk.
      // Try to request a new chunk from the system.

      if (createNewChunkNotSmallerThan(size))
	goto take_from_current_chunk;
      
      // Try to release some memory
      if (FailureHandling::requestMemoryRelease(size)) goto take_from_free_large;
       
    
      // Nothing to do here, give up
      return 0; // To suppress compiler warnings   
    }; // void* tryToAllocateLarge(ulong size)



    void deallocateLarge(void* obj,ulong size) 
    {
      CALL("deallocateLarge(void* obj,ulong size)");
      BK_CORRUPTMEM(obj,size);

      ASSERT(size > MaxSmallObjectSize);   

#ifdef DEBUG_ALLOC
      ObjDesc* objDesc = *(static_cast<ObjDesc**>(mirror(obj)));
      ASSERT(objDesc);
#ifdef DEBUG_ALLOC_OBJ_PTR
      ASSERT(objDesc->objPtr == obj);
#endif
#ifdef DEBUG_ALLOC_OBJ_STATUS    
      ASSERT(objDesc->status == ObjDesc::Persistent);
      objDesc->status = ObjDesc::Recycled;
#endif
#ifdef DEBUG_ALLOC_EVENT_ID
      objDesc->eventId = Debug::event();
#endif
#endif
      (static_cast<void**>(obj))[0] = static_cast<void*>(_freeLarge);
      (static_cast<void**>(obj))[1] = reinterpret_cast<void*>(size);
      _freeLarge = static_cast<void**>(obj);
      DOP(_sizeOfFreeLarge += size);
      _statistics.registerDeallocationOfLarge(size);
      ASSERT(_sizeOfFreeLarge == _statistics.sizeOfFreeLarge());
      DOP(_sizeOfPersistentLarge -= size);
      ASSERT(_sizeOfPersistentLarge == _statistics.sizeOfPersistentLarge());
    }; // void deallocateLarge(void* obj,ulong size) 

 

    void* allocateFromFreeList(ulong size)
    {
      CALL("allocateFromFreeList(ulong size)");

#ifndef DEBUG_ALLOC_NO_REUSE
      void** res = static_cast<void**>(popFromFreeList(size));
      if (res) 
	{     
	  _statistics.registerAllocationFromFreeList(size);
	}
      else
	{
	  ASSERT(_free[size] == 0L);
	};
      ASSERT(_free[size] == _statistics.free(size));
      return static_cast<void*>(res);
#else
      ASSERT(_free[size] == _statistics.free(size));
      return 0;
#endif
    }; // void* allocateFromFreeList(ulong size)




    void* getBestFreeLarge(ulong size,ulong& freeLargeSize)
    {
      CALL("getBestFreeLarge(ulong size,ulong& freeLargeSize)");   
      void*** bestMatch = 0;
      freeLargeSize = ULONG_MAX;
      void*** currentFreeLarge = &_freeLarge;
      while (*currentFreeLarge)
	{  
	  if (reinterpret_cast<ulong>((*currentFreeLarge)[1]) == size) // perfect match
	    {
	      freeLargeSize = size;  
	      void* res = static_cast<void*>(*currentFreeLarge);
	      *currentFreeLarge = static_cast<void**>((*currentFreeLarge)[0]); 
	      return res;
	    };  
	  if ((reinterpret_cast<ulong>((*currentFreeLarge)[1]) > size)
	      && (reinterpret_cast<ulong>((*currentFreeLarge)[1]) < freeLargeSize))
	    {
	      // best match so far
	      bestMatch = currentFreeLarge;  
	      freeLargeSize = reinterpret_cast<ulong>((*currentFreeLarge)[1]);
	    };  
	  currentFreeLarge = 
	    static_cast<void***>(static_cast<void*>(&((*currentFreeLarge)[0])));  
	};  
      if (bestMatch) 
	{
	  void* res = static_cast<void*>(*bestMatch);
	  *bestMatch = static_cast<void**>((*bestMatch)[0]);
	  return res;
	}
      else 
	return 0;
    }; // void* getBestFreeLarge(ulong size,ulong& freeLargeSize)

  
    void* allocateFromCurrentChunk(ulong size)
    {
      CALL("allocateFromCurrentChunk(ulong size)"); 
      if (size <= _freeInCurrChunk)
	{
	  _freeInCurrChunk -= size;
	  void* res = static_cast<void*>(_nextInCurrentChunk);
	  _nextInCurrentChunk += size; 
#ifdef DEBUG_ALLOC
	  ObjDesc* objDesc = *(static_cast<ObjDesc**>(mirror(res)));
	  ASSERT(!objDesc);
	  objDesc = new ObjDesc();
	  for (ulong n = 0; n < size; n++)
	    (static_cast<ObjDesc**>(mirror(res)))[n] = objDesc;  
#ifdef DEBUG_ALLOC_OBJ_PTR 
	  objDesc->objPtr = res;
#endif
#ifdef DEBUG_ALLOC_OBJ_STATUS
	  objDesc->status = ObjDesc::Persistent; 
#endif   
#ifdef DEBUG_ALLOC_EVENT_ID
	  objDesc->eventId = Debug::event();
#endif
#endif     
	  _statistics.registerAllocationFromCurrentChunk(size);
	  return static_cast<void*>(res); 
	}
      else
	return 0;
    }; // void* allocateFromCurrentChunk(ulong size)

    void* claimChunkNotSmallerThan(ulong recomendedSize,ulong& realSize)
    {
      CALL("claimChunkNotSmallerThan(ulong recomendedSize,ulong& realSize)");
      if (_availableChunks)
	{
	  void** previousChunk = 0;
	  for (void** currentChunk = _availableChunks;
	       currentChunk;
	       currentChunk = static_cast<void**>(currentChunk[0]))
	    {
	      ulong currentChunkSize = reinterpret_cast<ulong>(currentChunk[1]);
	      if (currentChunkSize >= recomendedSize)
		{
		  realSize = currentChunkSize;
		  void* res = static_cast<void*>(currentChunk + 2);
		  if (previousChunk)
		    {
		      previousChunk[0] = currentChunk[0];
		      currentChunk[0] = _allChunks;
		      _allChunks = currentChunk;
		    }
		  else
		    {
		      ASSERT(currentChunk == _availableChunks);
		      _availableChunks = static_cast<void**>(_availableChunks[0]);
		    };
		  return res;
		};
	      previousChunk = currentChunk;
	    };
	};
      
      if (_occupiedByChunks + ((long)recomendedSize) + 2 > _hardMemoryLimit)  
	return 0;
      
#ifdef DEBUG_ALLOC
      void** res = static_cast<void**>(debugWithMirrorMalloc(recomendedSize + 2));
#else
      void** res = static_cast<void**>(malloc((recomendedSize + 2)*sizeof(void*)));
#endif
  
      if (res)
	{
	  res[0] = static_cast<void*>(_allChunks);
	  _allChunks = res;
	  res[1] = reinterpret_cast<void*>(recomendedSize);      
	  res += 2;
	  _occupiedByChunks += (recomendedSize + 2); 
	  realSize = recomendedSize;
	};    
      return static_cast<void*>(res);
    }; // void* claimChunkNotSmallerThan(ulong recomendedSize,ulong& realSize)


    void* claimChunk(ulong recomendedSize,ulong& realSize)
    {
      CALL("claimChunk(ulong recomendedSize,ulong& realSize)");
      if (_availableChunks)
	{
	  void* res = static_cast<void*>(_availableChunks + 2);
	  realSize = reinterpret_cast<ulong>(_availableChunks[1]);
	  _availableChunks = static_cast<void**>(_availableChunks[0]);
	  return res;
	}
      else
	{
	  realSize = recomendedSize;
	  if (_occupiedByChunks + ((long)realSize) + 2 > _hardMemoryLimit)
	    {
	      long available = _hardMemoryLimit - (_occupiedByChunks + 2);
	      if (available <= 0L)
		return 0;
	      realSize = available;
	    };
  
#ifdef DEBUG_ALLOC
	  void** res = static_cast<void**>(debugWithMirrorMalloc(realSize + 2));
#else
	  void** res = static_cast<void**>(malloc((realSize + 2)*sizeof(void*)));
#endif
  
	  if (res)
	    {
	      res[0] = static_cast<void*>(_allChunks);
	      _allChunks = res;
	      res[1] = reinterpret_cast<void*>(realSize);      
	      res += 2;
	      _occupiedByChunks += (realSize + 2); 
	    };    
	  return static_cast<void*>(res);
	};
    }; // void* claimChunk(ulong recomendedSize,ulong& realSize)

    bool createNewChunk()
    {
      CALL("createNewChunk()");
      ulong realChunkSize;
      void** currentChunk = static_cast<void**>(claimChunk(StandardChunkSize,realChunkSize));
      if (!currentChunk) return false;
      utiliseRestOfCurrentChunk();
      _nextInCurrentChunk = currentChunk;  
      _freeInCurrChunk = realChunkSize;
      return true;
    }; // bool createNewChunk()


    bool createNewChunkNotSmallerThan(ulong size)
    {
      CALL("createNewChunkNotSmallerThan(ulong size)");
      ulong realChunkSize;
      void** currentChunk = 
	static_cast<void**>(claimChunkNotSmallerThan(size,realChunkSize));
      if (!currentChunk) return false;
      utiliseRestOfCurrentChunk();
      _nextInCurrentChunk = currentChunk;  
      _freeInCurrChunk = realChunkSize;
      return true;
    }; // bool createNewChunkNotSmallerThan(ulong size)


    bool useFreeLargeAsChunk()
    {
      CALL("useFreeLargeAsChunk()");
      if (!_freeLarge) return false;
      utiliseRestOfCurrentChunk(); 
      _nextInCurrentChunk = _freeLarge;  
      _freeInCurrChunk = reinterpret_cast<ulong>(_freeLarge[1]);
      _freeLarge = static_cast<void**>(_freeLarge[0]); 
      DOP(_sizeOfFreeLarge -= _freeInCurrChunk);

#ifdef DEBUG_ALLOC
      ObjDesc* objDesc = *(static_cast<ObjDesc**>(mirror(static_cast<void*>(_nextInCurrentChunk))));
      for (ulong n = 0; n < _freeInCurrChunk; n++)
	(static_cast<ObjDesc**>(mirror(static_cast<void*>(_nextInCurrentChunk))))[n] = 0;
      ASSERT(objDesc);
#ifdef DEBUG_ALLOC_OBJ_PTR
      ASSERT(objDesc->objPtr == static_cast<void*>(_nextInCurrentChunk));
#endif
      delete objDesc;
#endif

      _statistics.registerUseOfFreeLargeAsCurrentChunk(_freeInCurrChunk);
      ASSERT(_sizeOfFreeLarge == _statistics.sizeOfFreeLarge());
      return true;
    }; // bool useFreeLargeAsChunk()

    bool useFreeAsChunk(ulong size)
    {
      CALL("useFreeAsChunk(ulong size)");
      for (ulong n = MaxSmallObjectSize; n > size; n--)
	{
	  if (_freeList[n])
	    {
	      utiliseRestOfCurrentChunk();
	      void** currentChunk = static_cast<void**>(popFromFreeList(n));
	      
	      ASSERT(currentChunk);

	      _nextInCurrentChunk = currentChunk;  
	      _freeInCurrChunk = n; 
#ifdef DEBUG_ALLOC
	      ObjDesc* objDesc = *(static_cast<ObjDesc**>(mirror(static_cast<void*>(_nextInCurrentChunk))));
	      for (ulong i = 0; i < _freeInCurrChunk; i++)
		(static_cast<ObjDesc**>(mirror(static_cast<void*>(_nextInCurrentChunk))))[i] = 0;
	      ASSERT(objDesc);
#ifdef DEBUG_ALLOC_OBJ_PTR
	      ASSERT(objDesc->objPtr == static_cast<void*>(_nextInCurrentChunk));
#endif
	      delete objDesc;
#endif
	      _statistics.registerUseOfFreeAsCurrentChunk(_freeInCurrChunk);
	      ASSERT(_free[size] == _statistics.free(size));
	      return true;
	    };
	};       
      return false; 
    }; // bool useFreeAsChunk(ulong size)



    void utiliseRestOfCurrentChunk()
    {
      CALL("utiliseRestOfCurrentChunk()");
      if (!_freeInCurrChunk) return; // nothing to do at all
      if (_freeInCurrChunk <= MaxSmallObjectSize)
	{
	  pushIntoFreeList(static_cast<void*>(_nextInCurrentChunk),_freeInCurrChunk);
	}
      else // _freeInCurrChunk > MaxSmallObjectSize
	{ 
	  _nextInCurrentChunk[0] = static_cast<void*>(_freeLarge);
	  _nextInCurrentChunk[1] = reinterpret_cast<void*>(_freeInCurrChunk);
	  _freeLarge = _nextInCurrentChunk;
	  DOP(_sizeOfFreeLarge += _freeInCurrChunk);
	};

#ifdef DEBUG_ALLOC
      ObjDesc* objDesc = new ObjDesc(); 
      for (ulong n = 0; n < _freeInCurrChunk; n++)
	(static_cast<ObjDesc**>(mirror(static_cast<void*>(_nextInCurrentChunk))))[n] = objDesc;     
#ifdef DEBUG_ALLOC_OBJ_PTR
      objDesc->objPtr = static_cast<void*>(_nextInCurrentChunk);
#endif
#ifdef DEBUG_ALLOC_OBJ_STATUS
      objDesc->status = ObjDesc::Recycled;
#endif
#ifdef DEBUG_ALLOC_EVENT_ID
      objDesc->eventId = Debug::event();
#endif
#endif
      _statistics.registerUtilisationOfCurrentChunkReminder(_freeInCurrChunk);
      ASSERT((_freeInCurrChunk > MaxSmallObjectSize) || (_free[_freeInCurrChunk] == _statistics.free(_freeInCurrChunk)));
      DOP(_freeInCurrChunk = 0L);
      DOP(_nextInCurrentChunk = 0);     
    }; // void utiliseRestOfCurrentChunk()

    void pushIntoFreeList(void* piece,ulong size) 
    {
      CALL("pushIntoFreeList(void* piece,ulong size)");
      (static_cast<void**>(piece))[0] = _freeList[size];
      _freeList[size] = static_cast<void**>(piece);  
      DOP(++_free[size]);
    }; // void pushIntoFreeList(void* piece,ulong size) 

    void* popFromFreeList(ulong size) 
    {
      CALL("popFromFreeList(ulong size)"); 
      void** res = _freeList[size];
      if (res) 
	{
	  _freeList[size] = static_cast<void**>(res[0]); 
	  DOP(--_free[size]);
	}
      else
	{
	  ASSERT(_free[size] == 0L);
	  ASSERT(_statistics.free(size) == 0L);
	};
      return static_cast<void*>(res);
    }; // void* popFromFreeList(ulong size) 
 

#ifndef NO_DEBUG
    static bool inChunk(void* obj,void** chunk) 
    {
      return ((static_cast<void**>(obj)) >= (chunk + 2)) && (static_cast<ulong>((static_cast<void**>(obj)) - (chunk + 2)) < reinterpret_cast<ulong>(chunk[1]));
    }; 
#endif

#ifdef DEBUG_NAMESPACE
    long countFreeSmall(ulong size)
    {
      long res = 0L;
      for (void** p = _freeList[size]; p; p = static_cast<void**>(p[0]))
	++res;
      return res;
    };
#endif


  private:
  
    // Free lists
    void** _freeList[MaxSmallObjectSize+1];

    // Free list for recycled large objects
    void** _freeLarge;

    // Chunks

    void** _allChunks;
    void** _availableChunks;
    void** _nextInCurrentChunk;  
    ulong _freeInCurrChunk;


    // Statistics 
    BufferedAllocationStatistics<MaxSmallObjectSize> _statistics;

    long _occupiedByChunks;
    long _hardMemoryLimit;

    // Buffered allocation
    long _unbufferedAllocationLimit;
 
    // Debugging mode
#ifdef DEBUG_NAMESPACE
    long _free[MaxSmallObjectSize+1];
    long _persistent[MaxSmallObjectSize+1];
    long _sizeOfFreeLarge;
    long _sizeOfPersistentLarge;
#endif


#ifdef DEBUG_ALLOC
    ObjDesc** _debugMirror;
    ObjDesc** _debugMirrorEnd;
    ulong _debugWithMirrorMemorySize;
    ulong _debugWithMirrorHalfMemorySize;
    void** _debugWithMirrorFreeMemory;
    void** _debugWithMirrorEndOfFreeMemory;        
#endif
  }; //class Allocator<class FailureHandling,ulong MaxSmallObjectSize,ulong StandardChunkSize> 


}; // namespace BK

//================================================
#ifdef DEBUG_ALLOC
#  undef DEBUG_ALLOC_TOTAL_MEMORY_SIZE_IN_WORDS 
#  undef DEBUG_ALLOC_TOTAL_MEMORY_SIZE_DECREMENT_IN_WORDS 
#endif
//====================================================
#endif

