//
// File:         sharing.hpp
// Description:  Agressive sharing.
// Created:      Feb 15, 2000, 17:00
// Author:       Alexandre Riazanov
// mail:         riazanov@cs.man.ac.uk
// Revised:      Dec 25, 2001.
//               Freed from HellConst::MaxNumOfFunctors.
//====================================================
#ifndef SHARING_H
//===========================================================
#define SHARING_H
#include <climits>
#include <iostream>
#include "jargon.hpp"
#include "VampireKernelDebugFlags.hpp"
#include "VampireKernelConst.hpp"
#include "VampireKernelInterceptMacros.hpp"
#include "GlobAlloc.hpp"
#include "Array.hpp"                 
#include "Term.hpp"
#include "Stack.hpp"
#ifdef DEBUG_ALLOC_OBJ_TYPE
#  include "ClassDesc.hpp"
#endif
//================================================
#undef DEBUG_NAMESPACE
#ifdef DEBUG_SHARING
#define DEBUG_NAMESPACE "SHARING_INDEX"
#endif
#include "debugMacros.hpp"
//=================================================
namespace VK
{
class Flatterm;
class SHARING_TREE_NODE;
class SHARING_INDEX
{  

     
 public:
  class SURFER; 
 public:
  SHARING_INDEX();
  ~SHARING_INDEX();
  void init();
  void destroy();
  void reset();


  SHARING_TREE_NODE** Tree(const TERM& fun)
    {
      CALL("Tree(const TERM& fun)");
      ASSERT(fun.arity());
      DOP(_discTree.unfreeze());
      SHARING_TREE_NODE** res = &(_discTree.sub(fun.functor()));
      DOP(_discTree.unfreeze());
      return res;
    };

  TERM* Const(const TERM& c)
    { 
      CALL("Const(const TERM& c)");
      DOP(_sharedConst.unfreeze());
      TERM* res = _sharedConst.sub(c.functor());
      DOP(_sharedConst.freeze()); 
      if (!res)
	{
	  res = new(0) TERM(c);
	  INTERCEPT_BACKWARD_SUBSUMPTION_INDEXING_INTRODUCE_TERM_ID(res);
	  res->InitOccurences();
	  _sharedConst[c.functor()] = res;
	};
      return res;
    };
    

  TERM* IntegrateComplex(TERM* t);

  TERM* IntegrateFlatComplex(const Flatterm* t);

  TERM* CopyLiteral(TERM* source);
  TERM* CopyFlatLiteral(const Flatterm* source);
  TERM* CopyFlatEq(const TERM& header,const Flatterm* arg1,const Flatterm* arg2);
  void DestroyLiteralCopy(TERM* t);
  TERM* CopyComplex(TERM* source);
  void RemoveComplex(TERM* t);
  void RemoveComplexNonconst(TERM* t);
   
#ifndef NO_DEBUG
  void ScanIndex(ostream& str); // for debugging    
  void ScanTree(ostream& str,SHARING_TREE_NODE* tree,ulong& numOfNodes); // for debugging 
#endif 
private:
  typedef BK::Stack<TERM*,VampireKernelConst::MaxTermSize> PosStack;

 private:
  SHARING_TREE_NODE* CreateBranch(TERM* whole);
  TERM* IntegrateComplexNonconst(TERM* t);
  TERM* IntegrateFlatComplexNonconst(const Flatterm* t);
  static void DestroyTree(SHARING_TREE_NODE* tree);


private: 
  TERM::Iterator t_iter;
  PosStack subterms;
  BK::Array<BK::GlobAlloc,SHARING_TREE_NODE*,128UL,SHARING_INDEX> _discTree;
  BK::Array<BK::GlobAlloc,TERM*,128UL,SHARING_INDEX> _sharedConst;  
}; // class SHARING_INDEX

}; // namespace VK
//================================================
#undef DEBUG_NAMESPACE
#ifdef DEBUG_SHARING
#define DEBUG_NAMESPACE "SHARING_TREE_NODE"
#endif
#include "debugMacros.hpp"
//================================================= 
namespace VK
{

class SHARING_TREE_NODE
{
  
 private: // structure
  TERM tag;
  union
  {
    SHARING_TREE_NODE* below;
    long num;
  };
  union
  {
    SHARING_TREE_NODE* alternative;
    long dummy;
  };
   
#ifdef DEBUG_ALLOC_OBJ_TYPE
  static BK::ClassDesc _classDesc;
#endif  
    
 public: // construction and destruction
  SHARING_TREE_NODE(const TERM& t) : tag(t) // for making intermediate nodes
    {
      CALL("SHARING_TREE_NODE(const TERM& t)");
      below = 0;
      alternative = 0;
    };
  SHARING_TREE_NODE(TERM* t) : tag(t) // for making leaves
    {
      CALL("SHARING_TREE_NODE(TERM* t)");
      num = 1;
    };
  ~SHARING_TREE_NODE() {};
  void* operator new(size_t)
    {
      CALL("operator new(size_t)");
      //_allocated++;

#ifdef DEBUG_ALLOC_OBJ_TYPE
      ALWAYS(_classDesc.registerAllocated(sizeof(SHARING_TREE_NODE)));
      return BK::GlobAlloc::allocate(sizeof(SHARING_TREE_NODE),&_classDesc);
#else
      return BK::GlobAlloc::allocate(sizeof(SHARING_TREE_NODE));
#endif
    };
  void operator delete(void* obj)
    {
      CALL("operator delete(void* obj)");
      //_deallocated++;

#ifdef DEBUG_ALLOC_OBJ_TYPE 
      ALWAYS(_classDesc.registerDeallocated(sizeof(SHARING_TREE_NODE)));
      BK::GlobAlloc::deallocate(obj,sizeof(SHARING_TREE_NODE),&_classDesc);
#else
      BK::GlobAlloc::deallocate(obj,sizeof(SHARING_TREE_NODE));
#endif
    };
 public:
  static ulong minSize() { return sizeof(SHARING_TREE_NODE); };
  static ulong maxSize() { return sizeof(SHARING_TREE_NODE); };   
  bool IsLeaf() { return tag.IsReference(); };
  TERM& Tag() { return tag; };
  const TERM& Tag() const { return tag; };
  // methods for leaves
  TERM* Term() { return tag.First(); }; 
  long Num() { return num; };
  void IncNum() { if (num < LONG_MAX) { num++; }; };
  void DecNum() { if (num < LONG_MAX) { num--; }; };
  // methods for intermediate nodes
  SHARING_TREE_NODE* Below() { return below; };
  SHARING_TREE_NODE** BelowAddr() { return &below; };
  SHARING_TREE_NODE* Alternative() { return alternative; };
  SHARING_TREE_NODE** AlternativeAddr() { return &alternative; };
  void SetBelow(SHARING_TREE_NODE* n) { below = n; };  
  void SetAlternative(SHARING_TREE_NODE* n) { alternative = n; }; 
}; // class SHARING_TREE_NODE  

}; // namespace VK
//================================================
#undef DEBUG_NAMESPACE
#ifdef DEBUG_SHARING
#define DEBUG_NAMESPACE "SHARING_INDEX::SURFER"
#endif
#include "debugMacros.hpp"
//================================================= 
namespace VK
{
class SHARING_INDEX::SURFER
{
 private:
  SHARING_INDEX* index;
  SHARING_TREE_NODE** node_addr;
 public:
  SURFER(SHARING_INDEX* si) : index(si) {};
  bool Reset(const TERM& fun)
    {
      CALL("Reset(const TERM& fun)");
      node_addr = index->Tree(fun);
      return (*node_addr) != 0;
    };
  bool Read(const TERM& sym)
    {
    check_tag:
      if ((*node_addr)->Tag() == sym)
	{
	  node_addr = (*node_addr)->BelowAddr();
	  return true;
	}
      else 
	{
	  node_addr = (*node_addr)->AlternativeAddr();
	  if (*node_addr) { goto check_tag; }
	  else return false;
	};
    }; // bool Read(const TERM& sym) 
  void Write(const TERM& sym)
    {
      *node_addr = new SHARING_TREE_NODE(sym);
      node_addr = (*node_addr)->BelowAddr();
    }; // void Write(const TERM& sym)
  void CreateLeaf(TERM* t) { *node_addr = new SHARING_TREE_NODE(t); };
  SHARING_TREE_NODE* Leaf() { return (*node_addr); }; 
}; // class SHARING_INDEX::SURFER
}; // namespace VK

//===========================================================
#endif








