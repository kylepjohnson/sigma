//
// File:         PathProbeIndex.hpp
// Description:  Path-probe trees for forward matching.
// Created:      Nov 12, 2002, Alexandre Riazanov, riazanov@cs.man.ac.uk
// Note:         This file is a part of the Gematogen library.
//============================================================================
#ifndef PATH_PROBE_INDEX_H
#define PATH_PROBE_INDEX_H 
//=============================================================================
#ifndef NO_DEBUG_VIS
#  include <iostream>
#endif
#include "jargon.hpp"
#include "GematogenDebugFlags.hpp" 
#include "OptimisedPathIndex.hpp"
#include "ObjectPool.hpp"
#ifdef DEBUG_ALLOC_OBJ_TYPE 
#  include "ClassDesc.hpp"
#endif
//============================================================================
#undef DEBUG_NAMESPACE
#ifdef DEBUG_PATH_PROBE_INDEX
 #define DEBUG_NAMESPACE "PathProbeIndex<Alloc,Symbol,MaxTermDepth,MaxTermSize>"
#endif
#include "debugMacros.hpp"
//============================================================================

namespace Gem
{
  using namespace BK;
  template <class Alloc,class Symbol,ulong MaxTermDepth,ulong MaxTermSize>
// Requirements: 
  
  class PathProbeIndex
  {
  public:
    class Integrator;
  public:
    PathProbeIndex();
    ~PathProbeIndex();
  private:
    class PathInfo
    {
    public:
      PathInfo() { init(); };
      ~PathInfo() { destroy(); };
      void init() { _content = (void*)0; };
      void destroy() {};
      operator bool() const { return (bool)_content; };
      void* const & content() const { return _content; };
      void*& content() { return _content; };
    private:
      void* _content;
    }; // class PathInfo
    typedef 
    OptimisedPathIndex<Alloc,Symbol,PathInfo,PathInfo,MaxTermDepth,MaxTermSize>
    PathIndex;
    typedef PathIndex::Trie PathIndexTrie;
  private:
    PathIndexTrie _pathIndexTrie;
    friend class Integrator;
  }; // class PathProbeIndex
}; // namespace Gem

//============================================================================
#undef DEBUG_NAMESPACE
#ifdef DEBUG_PATH_PROBE_INDEX
 #define DEBUG_NAMESPACE "PathProbeIndex<Alloc,Symbol,MaxTermDepth,MaxTermSize>"
#endif
#include "debugMacros.hpp"
//============================================================================
namespace Gem
{
  template <class Alloc,class Symbol,ulong MaxTermDepth,ulong MaxTermSize>
  PathProbeIndex<Alloc,Symbol,MaxTermDepth,MaxTermSize>::PathProbeIndex()
  {
    CALL("constructor PathProbeIndex()");
  }; 

  template <class Alloc,class Symbol,ulong MaxTermDepth,ulong MaxTermSize>
  PathProbeIndex<Alloc,Symbol,MaxTermDepth,MaxTermSize>::~PathProbeIndex()
  {
    CALL("destructor ~PathProbeIndex()");
  }; 

}; // namespace Gem

//============================================================================
#undef DEBUG_NAMESPACE
#ifdef DEBUG_PATH_PROBE_INDEX
 #define DEBUG_NAMESPACE "PathProbeIndex<Alloc,Symbol,MaxTermDepth,MaxTermSize>::Integrator"
#endif
#include "debugMacros.hpp"
//============================================================================
namespace Gem
{
  template <class Alloc,class Symbol,ulong MaxTermDepth,ulong MaxTermSize>
  class PathProbeIndex<Alloc,Symbol,MaxTermDepth,MaxTermSize>::Integrator
  {
  public:
    Integrator() 
    {
      _subtermDescriptorPool.prepare(MaxTermSize);
    };
    Integrator(PathProbeIndex* index)
    { 
      _index = index;
      
    };
    ~Integrator() {};
    void reset();
    void function(const Symbol& f);
    void variable(const Symbol& v); 
    void endOfTerm();
    
#ifndef NO_DEBUG_VIS
    ostream& outputSubtermDescriptors(ostream& str) const;
#endif

  private:
    class SubtermDescriptor
    {
    public:
      SubtermDescriptor() {};
      ~SubtermDescriptor() {};
    private:

#ifndef NO_DEBUG_VIS
      ostream& output(ostream& str) const;
#endif
    private:
#ifndef NO_DEBUG
      ulong _debugNestNumber;
#endif
      SubtermDescriptor* _previous;
      SubtermDescriptor* _next;
      Symbol _topSymbol;
      SubtermDescriptor* _nest;
      ulong _numberInNest;
      ulong _nextArgumentNum;
      friend class Integrator;
    }; // class SubtermDescriptor
  private:
    PathProbeIndex* _index; 
    PathIndex::Integrator _pathIndexIntegrator;
    ObjectPool<Alloc,SubtermDescriptor> _subtermDescriptorPool;
#ifndef NO_DEBUG
    ulong _debugNextNestNumber;
#endif
    SubtermDescriptor* _allSubtermDescriptors;
    SubtermDescriptor* _lastSubtermDescriptor;
    SubtermDescriptor* _currentOpenNest;
    Stack<SubtermDescriptor*,MaxTermSize> _openNests;
 }; // class PathProbeIndex<Alloc,Symbol,MaxTermDepth,MaxTermSize>::Integrator
}; // namespace Gem

//============================================================================
#undef DEBUG_NAMESPACE
#ifdef DEBUG_PATH_PROBE_INDEX
 #define DEBUG_NAMESPACE "PathProbeIndex<Alloc,Symbol,MaxTermDepth,MaxTermSize>::Integrator"
#endif
#include "debugMacros.hpp"
//============================================================================

namespace Gem
{
  
  template <class Alloc,class Symbol,ulong MaxTermDepth,ulong MaxTermSize>
  inline
  void
  PathProbeIndex<Alloc,Symbol,MaxTermDepth,MaxTermSize>::Integrator::reset()
  {
    CALL("reset()");
    _subtermDescriptorPool.reset(); // reclaim all objects
    _allSubtermDescriptors = (SubtermDescriptor*)0;
    _lastSubtermDescriptor = (SubtermDescriptor*)0;  
    _pathIndexIntegrator.reset(_index->_pathIndexTrie);
    _currentOpenNest = (SubtermDescriptor*)0;
    _openNests.reset();
#ifndef NO_DEBUG
    _debugNextNestNumber = 0UL;
#endif
    // temporary
  }; // void PathProbeIndex<..>::Integrator::reset()

  template <class Alloc,class Symbol,ulong MaxTermDepth,ulong MaxTermSize>
  inline
  void 
  PathProbeIndex<Alloc,Symbol,MaxTermDepth,MaxTermSize>::Integrator::function(const Symbol& f)
  {
    CALL("function(const Symbol& f)");

    SubtermDescriptor* subtermDesc = _subtermDescriptorPool.reserveObject();
#ifndef NO_DEBUG
    subtermDesc->_debugNestNumber = _debugNextNestNumber;
    ++_debugNextNestNumber;
#endif
    if (!_allSubtermDescriptors)
      {
	_allSubtermDescriptors = subtermDesc;
      };
    subtermDesc->_previous = _lastSubtermDescriptor;
    subtermDesc->_next = (SubtermDescriptor*)0;  
    if (_lastSubtermDescriptor)
      {
	_lastSubtermDescriptor->_next = subtermDesc;
      };
    subtermDesc->_topSymbol = f;
    bool isNonConstant = (bool)f.arity();
    subtermDesc->_nest = _currentOpenNest;
    if (_currentOpenNest)
      {
	subtermDesc->_numberInNest = _currentOpenNest->_nextArgumentNum;
	++(_currentOpenNest->_nextArgumentNum);
	ASSERT(_currentOpenNest->_nextArgumentNum <= _currentOpenNest->_topSymbol.arity());
	if (_currentOpenNest->_nextArgumentNum < _currentOpenNest->_topSymbol.arity())
	  {
	    // the nest remains open 
	    if (isNonConstant)
	      {
	        _openNests.push(_currentOpenNest);
		_currentOpenNest = subtermDesc;
	      };
	  }
	else // the nest is closed
	  {	    
	    if (isNonConstant)
	      {
		_currentOpenNest = subtermDesc;
	      }
	    else
	      {
		_currentOpenNest = (_openNests)? _openNests.pop() : (SubtermDescriptor*)0;
	      };
	  };
      }
    else // !_currentOpenNest
      {
	subtermDesc->_numberInNest = 0UL;
	if (isNonConstant)
	  {
	    _currentOpenNest = subtermDesc;
	  };
      };

    subtermDesc->_nextArgumentNum = 0UL;

    // temporary

    _lastSubtermDescriptor = subtermDesc;
  }; // void PathProbeIndex<..>::Integrator::function(const Symbol& f)

  template <class Alloc,class Symbol,ulong MaxTermDepth,ulong MaxTermSize>
  inline
  void 
  PathProbeIndex<Alloc,Symbol,MaxTermDepth,MaxTermSize>::Integrator::variable(const Symbol& v)
  {
    CALL("variable(const Symbol& v)");


    SubtermDescriptor* subtermDesc = _subtermDescriptorPool.reserveObject();
#ifndef NO_DEBUG
    subtermDesc->_debugNestNumber = _debugNextNestNumber;
    ++_debugNextNestNumber;
#endif
    if (!_allSubtermDescriptors)
      {
	_allSubtermDescriptors = subtermDesc;
      };
    subtermDesc->_previous = _lastSubtermDescriptor;
    subtermDesc->_next = (SubtermDescriptor*)0;  
    if (_lastSubtermDescriptor)
      {
	_lastSubtermDescriptor->_next = subtermDesc;
      };
    subtermDesc->_topSymbol = v;

    subtermDesc->_nest = _currentOpenNest;

    if (_currentOpenNest)
      {
	subtermDesc->_numberInNest = _currentOpenNest->_nextArgumentNum;
	++(_currentOpenNest->_nextArgumentNum);
	ASSERT(_currentOpenNest->_nextArgumentNum <= _currentOpenNest->_topSymbol.arity());
	if (_currentOpenNest->_nextArgumentNum == _currentOpenNest->_topSymbol.arity())
	  {
	    // the nest is closed
	    _currentOpenNest = (_openNests)? _openNests.pop() : (SubtermDescriptor*)0;
	  };
      }
    else // !_currentOpenNest
      {
	subtermDesc->_numberInNest = 0UL;
      };

#ifndef NO_DEBUG
    subtermDesc->_nextArgumentNum = 0UL;
#endif

    // temporary

    _lastSubtermDescriptor = subtermDesc;

  }; // void PathProbeIndex<..>::Integrator::variable(const Symbol& v)

  template <class Alloc,class Symbol,ulong MaxTermDepth,ulong MaxTermSize>
  inline
  void 
  PathProbeIndex<Alloc,Symbol,MaxTermDepth,MaxTermSize>::Integrator::endOfTerm()
  {
    CALL("endOfTerm()");
    ASSERT(!_openNests);
    //DF; outputSubtermDescriptors(cout) << "\n";
    // temporary

  }; // void PathProbeIndex<..>::Integrator::endOfTerm()



#ifndef NO_DEBUG_VIS
  template <class Alloc,class Symbol,ulong MaxTermDepth,ulong MaxTermSize>
  inline
  ostream& 
  PathProbeIndex<Alloc,Symbol,MaxTermDepth,MaxTermSize>::Integrator::outputSubtermDescriptors(ostream& str) const
  {
    CALL("outputSubtermDescriptors(ostream& str) const");
    for (const SubtermDescriptor* d = _allSubtermDescriptors; 
	 d;
	 d = d->_next)
      {
	d->output(str) << "\n";
      };
    return str;
  }; // ostream& PathProbeIndex<..>::Integrator::outputSubtermDescriptors(ostream& str) const

  template <class Alloc,class Symbol,ulong MaxTermDepth,ulong MaxTermSize>
  inline
  ostream& 
  PathProbeIndex<Alloc,Symbol,MaxTermDepth,MaxTermSize>::Integrator::SubtermDescriptor::output(ostream& str) const
  {
    CALL("SubtermDescriptor::output(ostream& str) const");
    str << "#" << _debugNestNumber << " ";
    str << _numberInNest << "." << _topSymbol << " ";
    if (_nest) 
      {
	str << "in [" << _nest->_debugNestNumber << "] ";
      }
    else
      {
	str << "in [start] ";
      };
    str << "NA " << _nextArgumentNum << "(" << _topSymbol.arity() << ")";
    return str;
  }; // ostream& PathProbeIndex<..>::Integrator::SubtermDescriptor::output(ostream& str) const
#endif


}; // namespace Gem


//============================================================================
#endif
