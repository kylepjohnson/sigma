//
// File:         CacheAllocator.hpp
// Description:  Light-weight stack-like allocation. 
// Created:      Mar 20, 2003, Alexandre Riazanov, riazanov@cs.man.ac.uk 
//=======================================================================
#ifndef CACHE_ALLOCATOR_H
//=======================================================================
#define CACHE_ALLOCATOR_H
#include "jargon.hpp"
#include "BlodKorvDebugFlags.hpp"
#include "DestructionMode.hpp"
//=====================================================================
#undef DEBUG_NAMESPACE
#ifdef DEBUG_CACHE_ALLOCATOR
 #define DEBUG_NAMESPACE "CacheAllocator<Alloc,RecommendedPageSize,InstanceId>"
#endif
#include "debugMacros.hpp"
//======================================================================
namespace BK 
{
template <class Alloc,size_t RecommendedPageSize,class InstanceId>
class CacheAllocator
{
public: 
  CacheAllocator() 
  {
    CALL("constructor CacheAllocator()");
    size_t normalisedRecommendedPageSize = normaliseSize(RecommendedPageSize);
    _currentPage = new(normalisedRecommendedPageSize) Page(normalisedRecommendedPageSize);
    _currentPage->next = 0;
    _allPages = _currentPage;
    _freeInCurrentPage = _currentPage->memory();
    _normalisedSizeOfFreeInCurrentPage = normalisedRecommendedPageSize;
  };

  ~CacheAllocator() 
  {
    CALL("destructor ~CacheAllocator()");
    if (DestructionMode::isThorough())
      {
	while (_allPages)
	  {
	    Page* tmp = _allPages;
	    _allPages = _allPages->next;
	    delete tmp;
	  };
      };
  };
  
  void* allocate(size_t size)
  {
    CALL("allocate(size_t size)");
    ASSERT(size >= 0);
    if (size > RecommendedPageSize)
      return allocateLarge(size);
    
    size_t normalisedSize = normaliseSize(size);

    if (normalisedSize <= _normalisedSizeOfFreeInCurrentPage)
      {
      allocate_in_current_page:
	ASSERT(normalisedSize <= _normalisedSizeOfFreeInCurrentPage);
	void* res = _freeInCurrentPage;
	_freeInCurrentPage += normalisedSize;
	_normalisedSizeOfFreeInCurrentPage -= normalisedSize;
	return res;
      };
    
    // normalisedSize > _normalisedSizeOfFreeInCurrentPage, try next page
    if (!_currentPage->next)
      {
	size_t normalisedRecommendedPageSize = normaliseSize(RecommendedPageSize);
	_currentPage->next = 
	  new(normalisedRecommendedPageSize) Page(normalisedRecommendedPageSize);
	_currentPage->next->next = 0;
      };

    _currentPage = _currentPage->next;
    _freeInCurrentPage = _currentPage->memory();
    _normalisedSizeOfFreeInCurrentPage = _currentPage->normalisedSize;
    goto allocate_in_current_page;
  }; // void* allocate(size_t size)

  void reclaimAllMemory()
  {
    CALL("reclaimAllMemory()");
    _currentPage = _allPages;
    _freeInCurrentPage = _currentPage->memory();
    _normalisedSizeOfFreeInCurrentPage = _currentPage->normalisedSize;
  }; // void reclaimAllMemory()

private:
  class Page
  {
  public:
    Page(size_t sz) : normalisedSize(sz) {};
    ~Page() {};
    void* operator new(size_t,size_t size)
    {
      CALL("operator new(size_t,size_t size)");
      return Alloc::allocate(sizeof(Page) + sizeInBytes(size));
    };
    void operator delete(void* obj)
    {
      CALL("operator delete(void* obj)");
      Alloc::deallocate(obj,
			sizeof(Page) + 
			sizeInBytes(static_cast<Page*>(obj)->normalisedSize));
    };
    void** memory()
    {
      return static_cast<void**>(static_cast<void*>(this + 1));
    };
  public:
    size_t normalisedSize;
    Page* next;
  }; // class Page

private:
  void* allocateLarge(size_t size)
  {
    CALL("allocateLarge(size_t size)");
    // Inefficient temporary solution
    size_t normalisedSize = normaliseSize(size);
    // check the current page
    if (normalisedSize <= _normalisedSizeOfFreeInCurrentPage)
      {
	void* res = _freeInCurrentPage;
	_freeInCurrentPage += normalisedSize;
	_normalisedSizeOfFreeInCurrentPage -= normalisedSize;
	return res;
      };
    
    // check subsequent pages 
    Page* prevPage = _currentPage;
    Page* nextPage = prevPage->next;
    while (nextPage)
      {
	if (normalisedSize <= nextPage->normalisedSize)
	  {
	    prevPage->next = nextPage->next;
	    nextPage->next = _allPages;
	    _allPages = nextPage;
	    return nextPage->memory();
	  }
	else
	  {
	    prevPage = nextPage;
	    nextPage = nextPage->next;
	  };
      };

    // request new page from Alloc
    Page* newPage = new (normalisedSize) Page(normalisedSize);
    newPage->next = _allPages;
    _allPages = newPage;
    return newPage->memory();
  }; // void* allocateLarge(size_t size)

  static size_t normaliseSize(size_t size)
  {
    size_t res = size/sizeof(void*);
    if (size % sizeof(void*)) ++res;
    return res;
  };
  static size_t sizeInBytes(size_t normalisedSize) 
  {
    return normalisedSize * sizeof(void*);
  };
private: 
  Page* _allPages;
  Page* _currentPage;
  void** _freeInCurrentPage;
  size_t _normalisedSizeOfFreeInCurrentPage;
}; // class CacheAllocator<Alloc,RecommendedPageSize,InstanceId>
}; // namespace BK

//=========================================================================
#endif
