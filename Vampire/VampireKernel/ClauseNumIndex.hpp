//
// File:         ClauseNumIndex.hpp
// Description:  Searching clauses by numbers.
// Created:      Apr 08, 2000, 22:00
// Author:       Alexandre Riazanov
// mail:         riazanov@cs.man.ac.uk
//============================================================================
#ifndef CLAUSE_NUM_INDEX_H
//============================================================================
#define CLAUSE_NUM_INDEX_H
#include "jargon.hpp"
#include "VampireKernelDebugFlags.hpp"    
#include "SkipList.hpp"
#include "GlobAlloc.hpp"
#include "DestructionMode.hpp"
namespace VK
{
class Clause;
}; // namespace VK
//============================================================================
#undef DEBUG_NAMESPACE
#ifdef DEBUG_CLAUSE_NUM_INDEX
 #define DEBUG_NAMESPACE "ClauseNumIndex"
#endif
#include "debugMacros.hpp"
//============================================================================
namespace VK
{
class ClauseNumIndex
{  
 public:
  // List cannot be made private since it has a static data member
  typedef BK::SkipList<BK::GlobAlloc,Clause*,ulong,2,28,ClauseNumIndex> List;
 public:
  ClauseNumIndex() 
  {
   CALL("constructor ClauseNumIndex()");
  };
  ~ClauseNumIndex() 
    { 
      CALL("destructor ~ClauseNumIndex()");  
    };
  void init()
  {
    CALL("init()");
    _list.init();
    _surfer.init();
  };

  void destroy()
  {
    CALL("destroy()");
    
    _surfer.destroy();
    _list.destroy();
  };
  void reset()
  {
   _list.reset();  
  };

  void insert(Clause* cl);
  void remove(Clause* cl);
  Clause* find(ulong num);
  #ifndef NO_DEBUG
   bool clean() const { return !_list; };
  #endif
 private:
  List _list;
  List::Surfer _surfer;
}; // class ClauseNumIndex

}; // namespace VK

//======================================================================
#undef DEBUG_NAMESPACE
//======================================================================
#endif
