//
// File:         BackParaCandidates.hpp
// Description:  Indexed objects for back paramodulation.
// Created:      May 25, 2000, 14:20
// Author:       Alexandre Riazanov
// mail:         riazanov@cs.man.ac.uk
//============================================================================
#ifndef BACK_PARA_CANDIDATES_H
//=============================================================================
#define BACK_PARA_CANDIDATES_H
#include "jargon.hpp"
#include "VampireKernelDebugFlags.hpp"
#include "VampireKernelConst.hpp"
#include "GlobAlloc.hpp"
#include "SkipList.hpp"
#include "GList.hpp"
#include "Tuple.hpp"
#include "SimplificationOrdering.hpp"
#ifdef DEBUG_ALLOC_OBJ_TYPE
#  include "ClassDesc.hpp"
#endif
#include "DestructionMode.hpp"
namespace VK
{
class TERM;
class Clause;
}; // namespace VK
//=============================================================================
#undef DEBUG_NAMESPACE
#ifdef DEBUG_BACK_PARA_CANDIDATES
 #define DEBUG_NAMESPACE "BackParaCandidates"
#endif
#include "debugMacros.hpp"
//============================================================================

namespace VK
{
class BackParaCandidates
{
 public: 
  class LitRedexPair
  {
   public:
    LitRedexPair(ulong ln,const TERM* red,SimplificationOrdering::StoredConstraint* cns,LitRedexPair* nxt) : 
      _litNum(ln),
      _redex(red),
      _next(nxt)
    {
      CALL("LitRedexPair::constructor LitRedexPair(ulong ln,TERM* red,SimplificationOrdering::StoredConstraint* cns,LitRedexPair* nxt)");
     if (cns)
       {
	 _constraint = SimplificationOrdering::current()->copy(cns); 
       }
     else
       _constraint = 0;
    };  
    LitRedexPair(ulong ln,const TERM* red,SimplificationOrdering::StoredConstraint* cns) :      
      _litNum(ln),
      _redex(red),
      _next(0)
    {
      CALL("LitRedexPair::constructor LitRedexPair(ulong ln,TERM* red,SimplificationOrdering::StoredConstraint* cns)");
     if (cns)
       {
	 _constraint = SimplificationOrdering::current()->copy(cns); 
       }
     else
       _constraint = 0;
    };
    ~LitRedexPair() 
    {
      CALL("destructor ~LitRedexPair()");
      if (BK::DestructionMode::isThorough())
	{
	  if (_constraint)
	    {
	      SimplificationOrdering::current()->releaseConstraint(_constraint);
	    };
	};
    };
    void* operator new(size_t) 
    { 
     CALL("LitRedexPair::operator new(size_t)");
     #ifdef DEBUG_ALLOC_OBJ_TYPE
      ALWAYS(_classDesc.registerAllocated(sizeof(LitRedexPair)));
      return BK::GlobAlloc::allocate(sizeof(LitRedexPair),&_classDesc); 
     #else
      return BK::GlobAlloc::allocate(sizeof(LitRedexPair)); 
     #endif 
    };
    void  operator delete(void* obj) 
    {
     CALL("LitRedexPair::operator delete(void* obj)"); 
     #ifdef DEBUG_ALLOC_OBJ_TYPE
      ALWAYS(_classDesc.registerDeallocated(sizeof(LitRedexPair)));
      BK::GlobAlloc::deallocate(obj,sizeof(LitRedexPair),&_classDesc);
     #else
      BK::GlobAlloc::deallocate(obj,sizeof(LitRedexPair));  
     #endif 
    };
        
    static ulong minSize() { return sizeof(LitRedexPair); };
    static ulong maxSize() { return sizeof(LitRedexPair); };

    ulong litNum() const { return _litNum; };
    const TERM* redex() const { return _redex; };
    SimplificationOrdering::StoredConstraint* constraint() const 
      {
	CALL("constraint() const");
	return _constraint; 
      };
    LitRedexPair* next() const { return _next; }; 
    static void destroy(LitRedexPair* lst) 
    {
     CALL("destroy(LitRedexPair* lst)");
     LitRedexPair* rest; 
     while (lst) { rest = lst->next(); delete lst; lst = rest; };
    };
   private:
    ulong _litNum;
    const TERM* _redex;
    SimplificationOrdering::StoredConstraint* _constraint;
    LitRedexPair* _next;

    #ifdef DEBUG_ALLOC_OBJ_TYPE 
     static BK::ClassDesc _classDesc;
    #endif
  }; // class LitRedexPair

  // SkList cannot be made private since it has static data members
  typedef BK::SkipList<BK::GlobAlloc,LitRedexPair*,Clause*,2,28,BackParaCandidates> SkList;

 public:      

  class Iterator     
  {
   public:
    Iterator() {};
    Iterator(const BackParaCandidates& lst) { reset(lst); };
    ~Iterator() {};
    void init() {};
    void destroy() {};
    void reset(const BackParaCandidates& lst) 
    { 
     _currNode = lst._skList.first(); 
     if (_currNode) { _currLRPair = _currNode->value(); }
     else _currLRPair = 0;
    };

    bool next(Clause*& cl,LitRedexPair*& lrlist)
    {
     CALL("next(Clause*& cl,LitRedexPair*& lrlist)");
     if (_currLRPair) 
      { 
       cl = _currNode->key();       
       lrlist = _currLRPair;
       _currLRPair = _currLRPair->next();
       return true;
      };

     // !_currLRPair, try next node

     if (_currNode) 
      {
       next_node:
        _currNode = _currNode->next();
        if (_currNode)
 	 {
          lrlist = _currNode->value();
          if (lrlist) 
           {
            cl = _currNode->key();  
            _currLRPair = lrlist->next();
            return true;
           }
          else goto next_node;
         };
      };
     return false;
    }; // bool next(Clause*& cl,LitRedexPair*& lrlist)    

   private:
    const SkList::Node* _currNode;
    LitRedexPair* _currLRPair; 
  }; // class Iterator

 public:
  BackParaCandidates() : _skList() {};
  ~BackParaCandidates() 
  {
    CALL("destructor ~BackParaCandidates()");
    if (BK::DestructionMode::isThorough()) 
      {
	SkList::Destruction destr(_skList); 
	LitRedexPair* lrlist;
	while (destr.next(lrlist)) LitRedexPair::destroy(lrlist); 
      };
  };
  void* operator new(size_t) 
  { 
   return BK::GlobAlloc::allocate(sizeof(BackParaCandidates)); 
  };
  void operator delete(void* obj) 
  { 
   BK::GlobAlloc::deallocate(obj,sizeof(BackParaCandidates)); 
  };
  void add(Clause* cl,ulong ln,const TERM* red,SimplificationOrdering::StoredConstraint* cns);
  void remove(Clause* cl);
  bool empty() const { return !_skList; };
  #ifndef NO_DEBUG
   bool contains(Clause* cl) const { return _skList.contains(cl); }; 
   ulong size() const { return _skList.size(); };
   Clause* first() const { return _skList.first()->key(); };
   Clause* second() const { return _skList.first()->next()->key(); };
  #endif
 private:
  SkList _skList;
 friend class Iterator;
}; // class BackParaCandidates


inline void BackParaCandidates::add(Clause* cl,ulong ln,const TERM* red,SimplificationOrdering::StoredConstraint* cns)
{
 CALL("add(Clause* cl,ulong ln,const TERM* red,SimplificationOrdering::StoredConstraint* cns)");

 bool newNode;
 SkList::Node* node = _skList.insert(cl,newNode);
 if (newNode) 
  { 
   node->value() = new LitRedexPair(ln,red,cns);
  }
 else
  { 
   LitRedexPair* lrlist = node->value();
   ASSERT(lrlist);
   if ((lrlist->litNum() != ln) || (lrlist->redex() != red))
    {
     node->value() = new LitRedexPair(ln,red,cns,lrlist);     
    };
  };

 ASSERT(_skList.contains(cl));
}; // void BackParaCandidates::add(Clause* cl,ulong ln,const TERM* red,SimplificationOrdering::StoredConstraint* cns)

inline void BackParaCandidates::remove(Clause* cl)
{
 CALL("remove(Clause* cl)");
 LitRedexPair* lrlist; 
 if (_skList.remove(cl,lrlist)) LitRedexPair::destroy(lrlist); 
}; // void BackParaCandidates::remove(Clause* cl)


}; // namespace VK

//=============================================================================
#endif
