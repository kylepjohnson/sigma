//
// File:         IUQuery.cpp
// Description:  Representation of queries for indexed unification.
// Created:      Mar 23, 2003, Alexandre Riazanov, riazanov@cs.man.ac.uk
// Status:       Sketch.
//============================================================================
#include "IUQuery.hpp"
using namespace VK;
//============================================================================
#ifdef DEBUG_IU_QUERY
#  undef DEBUG_NAMESPACE
#  define DEBUG_NAMESPACE "IUQuery::TermAllocator"
#endif
#include "debugMacros.hpp"
//============================================================================

IUQuery::TermAllocator::~TermAllocator()
{
  CALL("destructor ~TermAllocator()");
  
}; // IUQuery::TermAllocator::~TermAllocator()

void* IUQuery::TermAllocator::allocate(size_t size)
{
  CALL("allocate(size_t size)");
  return _memory.allocate(size);
}; // void* IUQuery::TermAllocator::allocate(size_t size)


//============================================================================

