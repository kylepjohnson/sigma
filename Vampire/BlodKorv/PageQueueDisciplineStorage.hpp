//
// File:         PageQueueDisciplineStorage.hpp
// Description:  Storage of objects of arbitrary size organised
//               as a queue of pages.
// Created:      Apr 27, 2001, 20:30 
// Author:       Alexandre Riazanov
// mail:         riazanov@cs.man.ac.uk
// Note:         All sizes are in words (void*) rather than in bytes.
// Note:         This module is a draft.
//=============================================================================
#ifndef PAGE_QUEUE_DISCIPLINE_STORAGE_H
//=============================================================================
#define PAGE_QUEUE_DISCIPLINE_STORAGE_H 
#ifndef NO_DEBUG_VIS
 #include <iostream>
#endif
#include <cstddef>
#include "jargon.hpp"
#include "BlodKorvDebugFlags.hpp"
#include "AllocationFailureHandling.hpp"
//============================================================================
#undef DEBUG_NAMESPACE
#ifdef DEBUG_PAGE_QUEUE_DISCIPLINE_STORAGE
 #define DEBUG_NAMESPACE "PageQueueDisciplineStorage<class PageAlloc,ulong PageSize,class InstanceId>"
#endif
#include "debugMacros.hpp"
//============================================================================

namespace BK 
{
template <class PageAlloc,ulong PageSize,class InstanceId>
class PageQueueDisciplineStorage
{
 public:

  class FreePages
  {
   public:
    FreePages() 
    {
     CALL("FreePages::constructor FreePages()");
     init();
    };
    ~FreePages()
    {     
     CALL("FreePages::destructor ~FreePages()");
     destroy();
    };
    void init() { _pages = 0; }; 
    void destroy() 
    {
     CALL("FreePages::destroy()");
     if (DestructionMode::isThorough())
       while (_pages)
	 {
	   void** pageToRecycle = _pages;
	   _pages = PageQueueDisciplineStorage::nextPage(_pages);
	   PageAlloc::deallocate(pageToRecycle,sizeInBytes(PageSize));
	 };
     BK_CORRUPT(*this);
    };
    void reset() { destroy(); init(); }; 
    bool clean() { return !_pages; };
    void** givePage() 
    {
     CALL("FreePages::givePage()");
     if (_pages)
      {
       void** res = _pages;
       _pages = PageQueueDisciplineStorage::nextPage(_pages);
       return res;
      };     
     return 0;
    }; 
    void takePage(void** newFreePage)     
    {
     CALL("FreePages::takePage(void** newFreePage)");
     ASSERT(newFreePage);
     PageQueueDisciplineStorage::nextPage(newFreePage) = _pages;
     _pages = newFreePage;
    };
    bool recyclePage()
    {
     CALL("FreePages::recyclePage()");
     if (_pages)
      {
       void** pageToRecycle = _pages;
       _pages = PageQueueDisciplineStorage::nextPage(_pages);
       PageAlloc::deallocate(pageToRecycle,sizeInBytes(PageSize));
       return true;
      };
     return false;
    };
   private: 
    void** _pages;
  }; // class FreePages
 
 public:
  PageQueueDisciplineStorage() 
  {
   CALL("constructor PageQueueDisciplineStorage()");
   _freePages = 0;
   _headPage = 0;
   _headObject = 0;
   _lastPage = 0;
   _lastObject = 0;
   _localAllocFailureHandler.inheritUnrecoverableAllocationFailureHandler() = true;
  };
  PageQueueDisciplineStorage(FreePages* freePages)
  {
   CALL("constructor PageQueueDisciplineStorage()");
   _freePages = freePages;
   _headPage = 0;
   _headObject = 0;
   _lastPage = 0;
   _lastObject = 0;
   _localAllocFailureHandler.inheritUnrecoverableAllocationFailureHandler() = true;
  };

  ~PageQueueDisciplineStorage() 
  {
   CALL("destructor ~PageQueueDisciplineStorage()");
   if (DestructionMode::isThorough())
     {
       void** pageToRecycle;
       while (_headPage) 
	 { 
	   pageToRecycle = _headPage;
	   _headPage = nextPage(_headPage);
	   PageAlloc::deallocate(pageToRecycle,sizeInBytes(PageSize)); 
	 };
     };
   BK_CORRUPT(_headPage);
   BK_CORRUPT(_lastPage);  
  };

  void init(FreePages* freePages)
  {
   CALL("init(FreePages* freePages)");
   _localAllocFailureHandler.init();
   _freePages = freePages;
   _headPage = 0;
   _headObject = 0;
   _lastPage = 0;
   _lastObject = 0;
   _localAllocFailureHandler.inheritUnrecoverableAllocationFailureHandler() = true;
  };

  void destroy()
  {
   CALL("destroy()");
   if (DestructionMode::isThorough())
     {
       void** pageToRecycle;
       while (_headPage) 
	 { 
	   pageToRecycle = _headPage;
	   _headPage = nextPage(_headPage);
	   PageAlloc::deallocate(pageToRecycle,sizeInBytes(PageSize)); 
	 };
     };
   BK_CORRUPT(_headPage);
   BK_CORRUPT(_lastPage);  
   _localAllocFailureHandler.destroy();
  }; // void destroy()

  void reset(FreePages* freePages)
  {
   CALL("reset(FreePages* freePages)");
   destroy();
   init();
  }; // void reset(FreePages* freePages)

  // Writing (pushing) an object.  
  void* tryToAllocate(ulong size)
  {
   CALL("void* tryToAllocate(ulong size)");
   typename PageAlloc::AllocationFailureHandling::Activator allocHandlerActivator(&_localAllocFailureHandler);
   if (size + OneObjectStorageOverhead > PageSize - OnePageOverhead) return 0; 
   if (!_headPage)
    {
     _headPage = _freePages->givePage();
     if (!_headPage) 
       _headPage = static_cast<void**>(PageAlloc::tryToAllocate(sizeInBytes(PageSize)));
     if (!_headPage) return 0;
     _headObject = workingMemory(_headPage);
     nextPage(_headPage) = 0;
     previousPage(_headPage) = 0;
     _lastPage = _headPage;
     _lastObject = _headObject; 
     _endOfLastObject = _lastObject + size + OneObjectStorageOverhead; 
     nextObject(_lastObject) = 0;
     previousObject(_lastObject) = 0;
     return static_cast<void*>(_lastObject);
    };
   
   // try to allocate in the last page 
   ASSERT(nonempty());
   ASSERT(_endOfLastObject <= _lastPage + PageSize);
   ulong freeInLastPage = (_lastPage + PageSize) - _endOfLastObject;
   void** newObject; 
   if (size  + OneObjectStorageOverhead > freeInLastPage) 
    {
     void** newPage = _freePages->givePage();
     if (!newPage) newPage = static_cast<void**>(PageAlloc::tryToAllocate(sizeInBytes(PageSize)));
     if (!newPage) return 0;
     nextPage(newPage) = 0;
     nextPage(_lastPage) = newPage;
     previousPage(newPage) = _lastPage;
     _lastPage = newPage;  
     newObject = workingMemory(_lastPage);
    }
   else // may be allocated in _lastPage
    {
     newObject = _endOfLastObject;
    };
 
   ASSERT(!nextObject(_lastObject));  
   nextObject(_lastObject) = newObject; 
   previousObject(newObject) = _lastObject;
   nextObject(newObject) = 0;
   _lastObject = newObject;
   _endOfLastObject = _lastObject + size + OneObjectStorageOverhead;
   ASSERT(_lastObject && _lastPage);
   ASSERT(inPage(_lastObject,_lastPage));
   ASSERT(_endOfLastObject <= _lastPage + PageSize);
   return static_cast<void*>(_lastObject);      
  }; // void* tryToAllocate(ulong size)

  // Reading (popping) objects  
  bool empty() const { return !_headObject; };
  bool nonempty() const { return _headObject != 0; };
     
  void* head()
  {
   CALL("void* head()");
   return static_cast<void*>(_headObject);
  }; // void* head()

  const void* head() const
  {
   CALL("head() const");
   return static_cast<const void*>(_headObject);
  }; // void* head()
 
  static void* nextObject(void* object)
  {
   CALL("nextObject(void* object)"); 
   ASSERT(object);
   return static_cast<void*>(nextObject(static_cast<void**>(object)));
  }; // void* nextObject(void* object)

  static const void* nextObject(const void* object)
  {
   CALL("nextObject(const void* object)"); 
   ASSERT(object);
   return static_cast<const void*>(nextObject(static_cast<void* const *>(object)));
  }; // void* nextObject(void* object)


  static void* previousObject(void* object)
  {
   CALL("previousObject(void* object)"); 
   ASSERT(object);
   return static_cast<void*>(previousObject(static_cast<void**>(object)));
  }; // void* previousObject(void* object)
  
  static void* objectContent(void* object) 
  {
   CALL("objectContent(void* object)");
   ASSERT(object);   
   return static_cast<void*>((static_cast<void**>(object)) + OneObjectStorageOverhead);
  }; // void* objectContent(void* object) 

  static const void* objectContent(const void* object) 
  {
   CALL("objectContent(void* object)");
   ASSERT(object);   
   return static_cast<const void*>((static_cast<void* const *>(object)) + OneObjectStorageOverhead);
  }; // const void* objectContent(const void* object) 

  void pop()
  {
   CALL("pop()");

   ASSERT(nonempty());
   _headObject = nextObject(_headObject);
   if (!_headObject) // everything is free 
    {
     ASSERT(_lastPage == _headPage);
     _freePages->takePage(_headPage);
     _lastPage = _headPage = 0;
     _lastObject = _headObject = 0;
     return;          
    };
   // _headObject is nonnull
   previousObject(_headObject) = 0;
   if (!inPage(_headObject,_headPage))
    {
     // _headPage is free now
     void** oldHeadPage = _headPage;
     _headPage = nextPage(_headPage);
     ASSERT(_headPage);    
     previousPage(_headPage) = 0;
     _freePages->takePage(oldHeadPage);
    };
   ASSERT((!_lastPage) || inPage(_lastObject,_lastPage));
   ASSERT((!_lastPage) || _endOfLastObject <= _lastPage + PageSize);
  }; // void pop()

  // Reading and cancelling objects at the end of the queue
  
  void* endOfQueue() 
  {
   CALL("endOfQueue()");
   ASSERT(nonempty());
   return _lastObject;  
  }; // void* endOfQueue() 

  const void* endOfQueue() const
  {
   CALL("endOfQueue()");
   ASSERT(nonempty());
   return _lastObject;  
  }; // const void* endOfQueue() 

  void cancelLastObject()
  {
   CALL("cancelLastObject()");


   ASSERT(nonempty()); 
   if (_lastObject == _headObject) 
    {
     // everything will be free now
     ASSERT(_lastPage == _headPage);
     _freePages->takePage(_headPage);
     _lastPage = _headPage = 0;
     _lastObject = _headObject = 0;
     return; 
    };     
   
   void** oldLastObject = _lastObject; 
   _lastObject = previousObject(_lastObject);
   ASSERT(_lastObject);
   nextObject(_lastObject) = 0;
   // check whether _lastPage is free now
   if (!inPage(_lastObject,_lastPage))
    {
     void** oldLastPage = _lastPage;
     _lastPage = previousPage(_lastPage);
     _endOfLastObject = _lastPage + PageSize;   
     ASSERT(_lastPage);
     ASSERT(inPage(_lastObject,_lastPage));
     nextPage(_lastPage) = 0;
     _freePages->takePage(oldLastPage);     
    }
   else
    {
     _endOfLastObject = oldLastObject;   
    };
   ASSERT((!_lastPage) || inPage(_lastObject,_lastPage));
   ASSERT((!_lastPage) || _endOfLastObject <= _lastPage + PageSize);
  }; // void cancelLastObject() 


 private:
  static void**& nextPage(void** page) 
    {
      CALL("nextPage(void** page)");
      return (static_cast<void***>(static_cast<void*>(page)))[0]; 
    }; 
  static void**& previousPage(void** page) 
  { 
    return (static_cast<void***>(static_cast<void*>(page)))[1]; 
  };    
  static void** workingMemory(void** page) { return page + OnePageOverhead; };
  enum 
  {
	  OnePageOverhead = 2UL // next page and previous page pointers
  };
  static ulong sizeOfWorkingMemory() { return PageSize - OnePageOverhead; }; 
  static ulong sizeInBytes(ulong sizeInWords)
  {
   return sizeInWords*sizeof(void*);
  };
  enum 
  {
	  OneObjectStorageOverhead = 2UL // next object and  previous object pointers
  };
  static void**& nextObject(void** object) 
  { 
    return (static_cast<void***>(static_cast<void*>(object)))[0];
  };
  static void**& previousObject(void** object) { return (static_cast<void***>(static_cast<void*>(object)))[1]; };
  static bool inPage(void** ptr,void** page)  
  {
   return (ptr >= page) && (ptr < page + PageSize);
  };

 private:
  FreePages* _freePages; 
  void** _headPage;  
  void** _headObject;  
  void** _lastPage;
  void** _lastObject;
  void** _endOfLastObject;
  typename PageAlloc::AllocationFailureHandling _localAllocFailureHandler;
 friend class FreePages;
}; // template <class PageAlloc,ulong PageSize,class InstanceId> class PageQueueDisciplineStorage


}; // namespace BK

//============================================================================
#endif
