//
// File:         OldBSIndex.hpp
// Description:  Simple index for backward subsumption based on discrimination trees.
// Created:      Jan 14, 2001, 15:40
// Author:       Alexandre Riazanov
// mail:         riazanov@cs.man.ac.uk
//============================================================================
#ifndef OLD_BS_INDEX_H
//=============================================================================

//#define BS_USE_ORD_LISTS

#define OLD_BS_INDEX_H 
#include "jargon.hpp"
#include "VampireKernelDebugFlags.hpp"
#include "VampireKernelConst.hpp"
#include "DestructionMode.hpp"
#include "Term.hpp"
#include "GlobAlloc.hpp"
#ifdef BS_USE_ORD_LISTS
 #include "OrdList.hpp"
#else
 #include "SkipList.hpp"
#endif
#include "GList.hpp"
#include "Tuple.hpp"
#include "DiscTree.hpp"
//============================================================================
#undef DEBUG_NAMESPACE
#ifdef DEBUG_OLD_BS_INDEX
 #define DEBUG_NAMESPACE "OldBSIndex"
#endif
#include "debugMacros.hpp"
//============================================================================
namespace VK
{
  using namespace BK;
  using namespace Gem;
class OldBSIndex
{
 public:
  class Integrator;
  class Retrieval;
  class Removal;

  #ifndef BS_USE_ORD_LISTS
   typedef VoidSkipList<GlobAlloc,ulong,2,28,OldBSIndex> CSkipList; 
   typedef SkipList<GlobAlloc,ulong,ulong,2,28,OldBSIndex> CLSkipList; 
  #else
   typedef VoidOrdList<GlobAlloc,ulong,OldBSIndex> CSkipList; 
   typedef OrdList<GlobAlloc,ulong,ulong,OldBSIndex> CLSkipList; 
  #endif

  typedef DiscTree<GlobAlloc,CLSkipList> DiscTree;

 public:
  OldBSIndex()
  {
   for (ulong i = 0; i < VampireKernelConst::MaxNumOfDifferentHeaders; i++) 
    _ordLitInd[i] = 0;
  }; 
  ~OldBSIndex() 
   { 
    CALL("destructor ~OldBSIndex()"); 
    if (DestructionMode::isThorough()) destroy();
   }; 
 
  void destroy();

  #ifndef NO_DEBUG
   bool clean() const 
   {
    for (ulong i = 0; i < VampireKernelConst::MaxNumOfDifferentHeaders; i++) 
     if (_ordLitInd[i] != 0) return false;
    for (ulong i = 0; i < VampireKernelConst::MaxNumOfDifferentHeaders; i++) 
     if (_propLitInd[i]) return false;
    return true; 
   }; 
  #endif 

 private: // types

 private: // methods

 private: // data
  CSkipList _propLitInd[VampireKernelConst::MaxNumOfDifferentHeaders];
  DiscTree::Node* _ordLitInd[VampireKernelConst::MaxNumOfDifferentHeaders];  
 friend class Integrator;
 friend class Retrieval;
 friend class Removal; 
}; // class OldBSIndex; 
}; // namespace VK


//============================================================================
#undef DEBUG_NAMESPACE
#ifdef DEBUG_BS_INDEX
 #define DEBUG_NAMESPACE "OldBSIndex::Integrator"
#endif
#include "debugMacros.hpp"
//============================================================================
namespace VK
{
  using namespace BK;
  using namespace Gem;
class OldBSIndex::Integrator
{
 public:
  Integrator() {}; 
  Integrator(OldBSIndex* index)
  { 
   reset(index); 
  };
  ~Integrator() {}; 
  
  void reset(OldBSIndex* index) { _index = index; }; 
  void clause(const ulong& clNum) { _clauseNum = clNum; };
  void propLit(const TERM* lit)
  {
   _index->_propLitInd[lit->HeaderNum()].insert(_clauseNum); 
  };

  void literal(const TERM* lit,ulong num)
  {
   CALL("literal(const TERM* lit,ulong num)");
   ASSERT(lit->arity());
   _integrator.reset(_index->_ordLitInd + lit->HeaderNum());
   TERM::Iterator iter(lit); 
   while (iter.Next()) 
    { 
     _integrator.push(iter.symbolRef()); 
    };
   bool newLeaf; 
   DiscTree::Node* leaf = _integrator.getLeaf(newLeaf);
   if (newLeaf) leaf->leafInfo().fastReset(); // initialisation

   //DF; cout << _clauseNum << "   " << lit << "\n";

   ASSERT(!leaf->leafInfo().contains(_clauseNum));
   bool newListNode;  
   CLSkipList::Node* listNode = leaf->leafInfo().insert(_clauseNum,newListNode);
   ASSERT(newListNode);
   listNode->value() = num; 
  };
 
 private:
  DiscTree::Integrator _integrator;
  OldBSIndex* _index;
  ulong _clauseNum;
}; // class OldBSIndex::Integrator

}; // namespace VK


//============================================================================
#undef DEBUG_NAMESPACE
#ifdef DEBUG_BS_INDEX
 #define DEBUG_NAMESPACE "OldBSIndex::Retrieval"
#endif
#include "debugMacros.hpp"
//============================================================================

namespace VK
{
  using namespace BK;
  using namespace Gem;
class OldBSIndex::Retrieval
{
 public:
  Retrieval() 
  {
  }; 
  Retrieval(OldBSIndex* index)
  { 
   reset(index); 
  };
  ~Retrieval() {}
  void reset(OldBSIndex* index) { _index = index; }; 
  bool propLit(const TERM& lit,const CSkipList*& occList)
  {
   occList = _index->_propLitInd + lit.HeaderNum();
   return (bool)(*occList);
  };

 private:
  OldBSIndex* _index;
}; // class OldBSIndex::Retrieval
}; // namespace VK

//============================================================================
#undef DEBUG_NAMESPACE
#ifdef DEBUG_BS_INDEX
 #define DEBUG_NAMESPACE "OldBSIndex::Removal"
#endif
#include "debugMacros.hpp"
//============================================================================
namespace VK
{
  using namespace BK;
  using namespace Gem;
class OldBSIndex::Removal
{
 public:
  Removal() 
  {
  }; 
  Removal(OldBSIndex* index)
  { 
   reset(index); 
  };
  ~Removal() {};
  void reset(OldBSIndex* index) { _index = index; }; 
  void clause(const ulong& clNum) { _clauseNum = clNum; };
  void propLit(const TERM& lit)
  {
   _index->_propLitInd[lit.HeaderNum()].remove(_clauseNum); 
  };

  bool litHeader(const TERM& hd)
  {
   CALL("litHeader(const TERM& hd)");
   return false; // dummy
  };

  bool nonconstFunc(const TERM& f)
  {
   CALL("nonconstFunc(const TERM& f)");
   return false; // dummy
  };
 
  bool constant(const TERM& c)
  {
   CALL("constant(const TERM& c)");
   return false; // dummy
  };
 
  void variable()
  {
   CALL("variable()");    
  };
  
  void after() {};

  bool symbol(const TERM& s) 
  {
   CALL("symbol(const TERM& s)");
   if (s.isVariable()) { variable(); return true; };
   if (s.arity()) return nonconstFunc(s); 
   return constant(s);
  };
 
  void endOfLit() { CALL("endOfLit()"); };

  bool symLitHeader(const TERM& hd)
  {
   CALL("symLitHeader(const TERM& hd)");
   return false; // dummy
  };
  
  bool symLitSecondArg()
  {
   CALL("symLitSecondArg()");
   return false; // dummy  
  };
 
  bool symLitNonconstFunc(const TERM& f)
  {
   CALL("symLitNonconstFunc(const TERM& f)");
   return false; // dummy
  };
 
  bool symLitConstant(const TERM& c)
  {
   CALL("symLitConstant(const TERM& c)");
   return false; // dummy
  };
 
  void symLitVariable()
  {
   CALL("symLitVariable()");
  };

  void symLitAfter() { };
  bool symLitSymbol(const TERM& s) 
  {
   CALL("symLitSymbol(const TERM& s)");
   if (s.isVariable()) { symLitVariable(); return true; };
   if (s.arity()) return symLitNonconstFunc(s); 
   return symLitConstant(s);
  };

  void endOfSymLit() { CALL("endOfSymLit()"); };

 private:
  OldBSIndex* _index;  
  ulong _clauseNum;
}; // class OldBSIndex::Removal

}; // namespace VK

//======================================================================
#undef DEBUG_NAMESPACE
//======================================================================
#endif
