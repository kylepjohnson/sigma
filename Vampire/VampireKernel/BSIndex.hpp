//
// File:         BSIndex.hpp
// Description:  Path index for backward subsumption.
// Created:      Feb 26, 2000.
// Author:       Alexandre Riazanov
// mail:         riazanov@cs.man.ac.uk
// Revised:      Dec 23, 2001. 
//               1) The array BSIndex::_propLitInd replaced by
//               extendable array BSIndex::_propLitOccurences
//               2) Freed from HellConst::MaxNumOfDifferentHeaders.
//============================================================================
#ifndef BS_INDEX_H
//=============================================================================

//#define BS_USE_ORD_LISTS

#define BS_INDEX_H 
#include "jargon.hpp"
#include "VampireKernelDebugFlags.hpp"
#include "VampireKernelConst.hpp"
#include "Term.hpp"
#include "GlobAlloc.hpp"
#include "Array.hpp"
#ifdef BS_USE_ORD_LISTS
#include "OrdList.hpp"
#else
#include "SkipList.hpp"
#endif
#include "GList.hpp"
#include "Tuple.hpp"
#include "PathIndexWithOpenPaths.hpp"

//============================================================================
#undef DEBUG_NAMESPACE
#ifdef DEBUG_BS_INDEX
#define DEBUG_NAMESPACE "BSIndex"
#endif
#include "debugMacros.hpp"
//============================================================================
namespace VK
{
  class BSIndex
  {
  public:
    class Integrator;
    class Retrieval;
    class Removal;
    typedef BK::Tuple2<ulong,TERM> LTPair;
    typedef BK::Tuple2<ulong,ulong> LAPair;
    typedef BK::Tuple3<ulong,ulong,TERM> LATTriple;
    typedef BK::GList<BK::GlobAlloc,ulong,BSIndex> LList; // lit. num.
    typedef BK::GList<BK::GlobAlloc,LTPair,BSIndex> LTList; // lit. num. + term
    typedef BK::GList<BK::GlobAlloc,LAPair,BSIndex> LAList; // lit.num. + arg.num.
    typedef BK::GList<BK::GlobAlloc,LATTriple,BSIndex> LATList; // lit.num. + arg.num. + term

#ifndef BS_USE_ORD_LISTS
    typedef BK::VoidSkipList<BK::GlobAlloc,ulong,2,28,BSIndex> CSkipList; // for prop. literals
    typedef BK::SkipList<BK::GlobAlloc,LList*,ulong,2,28,BSIndex> CLSkipList; // for constants in ordinary literals
    typedef BK::SkipList<BK::GlobAlloc,LTList*,ulong,2,28,BSIndex> CLTSkipList; // for all in ordinary literals
    typedef BK::SkipList<BK::GlobAlloc,LAList*,ulong,2,28,BSIndex> CLASkipList; // for constants in symmetric literals
    typedef BK::SkipList<BK::GlobAlloc,LATList*,ulong,2,28,BSIndex> CLATSkipList; // for all in symmetric literals 
#else
    typedef BK::VoidOrdList<BK::GlobAlloc,ulong,BSIndex> CSkipList; // for prop. literals
    typedef BK::OrdList<BK::GlobAlloc,LList*,ulong,BSIndex> CLSkipList; // for constants in ordinary literals
    typedef BK::OrdList<BK::GlobAlloc,LTList*,ulong,BSIndex> CLTSkipList; // for all in ordinary literals
    typedef BK::OrdList<BK::GlobAlloc,LAList*,ulong,BSIndex> CLASkipList; // for constants in symmetric literals
    typedef BK::OrdList<BK::GlobAlloc,LATList*,ulong,BSIndex> CLATSkipList; // for all in symmetric literals  
#endif
   
    typedef Gem::PathIndexWithOpenPaths<BK::GlobAlloc,TERM,CLTSkipList,CLSkipList,
                                        VampireKernelConst::MaxTermDepth,
			                VampireKernelConst::MaxTermSize> 
    OrdLitPITree;
	
    typedef Gem::PathIndexWithOpenPaths<BK::GlobAlloc,TERM,CLATSkipList,CLASkipList,
                                   VampireKernelConst::MaxTermDepth,
			           VampireKernelConst::MaxTermSize> 
    SymLitPITree;  
    
    
    typedef Gem::PathIndexWithOpenPathsDestruction<BK::GlobAlloc,TERM,CLTSkipList,CLSkipList,
      VampireKernelConst::MaxTermDepth,
			  VampireKernelConst::MaxTermSize> 
    OrdLitPITreeDestruction;
	
    typedef Gem::PathIndexWithOpenPathsDestruction<BK::GlobAlloc,TERM,CLATSkipList,CLASkipList,
      VampireKernelConst::MaxTermDepth,
			  VampireKernelConst::MaxTermSize> 
    SymLitPITreeDestruction;  
    

    class Sizes 
    {
    public:
      Sizes() 
	: _maxlNumOfNonproplLits(0UL)
      {
      };
      ~Sizes() {}; 
      void init()
      {
	_maxlNumOfNonproplLits = 0UL;
      };
      void destroy() {};
      ulong maxlNumOfNonproplLits() const { return _maxlNumOfNonproplLits; };
    private:
      ulong _maxlNumOfNonproplLits;
      friend class BSIndex;
      friend class Integrator; 
    }; // class Sizes

  public:
    BSIndex();
    ~BSIndex();
    void init();
    void destroy();
    const Sizes* sizes() const { return &_sizes; };

#ifndef NO_DEBUG

    static bool greater(const LAPair& p1,const LAPair& p2) 
    {
      if (p1.el1 == p2.el1) return p1.el2 > p2.el2;
      return p1.el1 > p2.el1;
    };

    static bool greater(const LATTriple& p1,const LATTriple& p2) 
    {
      if (p1.el1 == p2.el1) return p1.el2 > p2.el2;
      return p1.el1 > p2.el1;
    };
#endif 


  private: // methods
    static void remove(const ulong& clNum,CLSkipList& lst)
    {
      LList* llist;
      if (lst.remove(clNum,llist)) LList::destroyList(llist); 
    };
    static void remove(const ulong& clNum,CLTSkipList& lst)
    {
      LTList* ltlist;
      if (lst.remove(clNum,ltlist)) LTList::destroyList(ltlist); 
    }; 
    static void remove(const ulong& clNum,CLASkipList& lst)
    {
      LAList* lalist;
      if (lst.remove(clNum,lalist)) LAList::destroyList(lalist); 
    };  
    static void remove(const ulong& clNum,CLATSkipList& lst)
    {
      LATList* latlist;
      if (lst.remove(clNum,latlist)) LATList::destroyList(latlist); 
    };
 
    static void recycle(CLSkipList& lst)
    { 
      LList* llist;
      CLSkipList::Destruction destr(lst);
      while (destr.next(llist)) LList::destroyList(llist);      
    };

    static void recycle(CLTSkipList& lst)
    { 
      LTList* ltlist;
      CLTSkipList::Destruction destr(lst);
      while (destr.next(ltlist)) LTList::destroyList(ltlist);      
    }; 
  
    static void recycle(CLASkipList& lst)
    { 
      LAList* lalist;
      CLASkipList::Destruction destr(lst);
      while (destr.next(lalist)) LAList::destroyList(lalist);      
    };
 
    static void recycle(CLATSkipList& lst)
    { 
      LATList* latlist;
      CLATSkipList::Destruction destr(lst);
      while (destr.next(latlist)) LATList::destroyList(latlist);      
    };
 
    void insertPropLit(const TERM& lit,ulong clauseNum);

    void removePropLit(const TERM& lit,ulong clauseNum);

    const CSkipList* propLitOccurences(const TERM& lit)
    {
      CALL("propLitOccurences(const TERM& lit)");
      DOP(_propLitOccurences.unfreeze());
      CSkipList* occList = _propLitOccurences.sub(lit.HeaderNum());
      DOP(_propLitOccurences.freeze());
      if (!occList) 
	{
	  // This is done to simulate the old version functionality
	  occList = new CSkipList();
	  _propLitOccurences[lit.HeaderNum()] = occList;
	};
      return occList;
    };

  private: // data
   
    BK::Array<BK::GlobAlloc,CSkipList*,128UL,BSIndex> _propLitOccurences;
  
    OrdLitPITree::Trie _ordLitInd[1];
    SymLitPITree::Trie _symLitInd[1];
    Sizes _sizes;
    friend class Integrator;
    friend class Retrieval;
    friend class Removal; 
  }; // class BSIndex; 

}; // namespace VK


//============================================================================
#undef DEBUG_NAMESPACE
#ifdef DEBUG_BS_INDEX
#define DEBUG_NAMESPACE "BSIndex::Integrator"
#endif
#include "debugMacros.hpp"
//============================================================================
namespace VK
{
  class BSIndex::Integrator
  { 
  public:
    // VC++ 6.0 breaks down if I defin a constructor or destructor
    // for BSIndex::Integrator
    /************

    Integrator(BSIndex* index) : _index(index)
    {
      CALL("constructor Integrator(BSIndex* index)");
    };
    ~Integrator()
    {
      CALL("destructor ~Integrator()");
    };
    ***************/

    void init(BSIndex* index)
    {
      CALL("init(BSIndex* index)");
      _index = index;
      _ordLitIntegrator.init();
      _symLitIntegrator.init();
    };

    void destroy()
    {
      CALL("destroy()");
      _symLitIntegrator.destroy();
      _ordLitIntegrator.destroy();
      BK_CORRUPT(*this);
    };
    void reset(BSIndex* index) { _index = index; }; 
    void clause(const ulong& clNum) { _clauseNum = clNum; _litNum = 0; };
    void endOfClause()
    {
      if (_litNum > _index->_sizes._maxlNumOfNonproplLits)
	_index->_sizes._maxlNumOfNonproplLits = _litNum;
    };
    void propLit(const TERM& lit);
    void litHeader(const TERM& hd)
    {
      _ordLitIntegrator.reset(_index->_ordLitInd[0]);
      _ordLitIntegrator.nonconstFunc(hd);   
      _litNum++;
    };
    void subterm(const TERM& t);
    void symLitHeader(const TERM& hd)
    {
      _symLitIntegrator.reset(_index->_symLitInd[0]);
      _symLitIntegrator.nonconstFunc(hd);   
      _litNum++;
      _symLitArgNum = 0;
      _symLitHeader = hd;
    };
    void symLitSecondArg() 
    { 
      _symLitIntegrator.reset(_index->_symLitInd[0]);
      _symLitIntegrator.nonconstFunc(_symLitHeader);   
      _symLitArgNum = 1; 
    };

    void symLitSubterm(const TERM& t);
	

  private:
    BSIndex* _index;
    
    OrdLitPITree::Integrator _ordLitIntegrator;
    SymLitPITree::Integrator _symLitIntegrator;
    
    ulong _clauseNum;
    ulong _litNum;
    ulong _symLitArgNum;
    TERM _symLitHeader;
  }; // class BSIndex::Integrator

}; // namespace VK




//============================================================================
#undef DEBUG_NAMESPACE
#ifdef DEBUG_BS_INDEX
#define DEBUG_NAMESPACE "BSIndex::Retrieval"
#endif
#include "debugMacros.hpp"
//============================================================================

namespace VK
{
  class BSIndex::Retrieval
  {
  public:

    // VC++ 6.0 breaks down if I defin a constructor or destructor
    // for BSIndex::Retrieval

    /***********
    Retrieval(BSIndex* index) : _index(index)
    { 
      CALL("constructor Retrieval(BSIndex* index)");
    };
    
    ~Retrieval()
    {
      CALL("destructor ~Retrieval()");
    };
    ************/

    void init(BSIndex* index)
    {
      CALL("init(BSIndex* index)");
      _index = index;
      _ordLitRetrieval.init();
      _symLitRetrieval.init();
    };

    void destroy()
    {
      CALL("destroy()");
      _symLitRetrieval.destroy();
      _ordLitRetrieval.destroy();
      BK_CORRUPT(*this);
    };

    void reset(BSIndex* index) { _index = index; }; 
    bool propLit(const TERM& lit,const CSkipList*& occList)
    {
      CALL("propLit(const TERM& lit,const CSkipList*& occList)");
      occList = _index->propLitOccurences(lit);
      //occList = _index->_propLitInd + lit.HeaderNum();
      return (bool)(*occList);
    };
    bool litHeader(const TERM& hd)
    {
      return ((_ordLitRetrieval.reset(_index->_ordLitInd[0])) && (_ordLitRetrieval.nonconstFunc(hd)));
    };
    bool nonconstFunc(const TERM& f)
    {
      return _ordLitRetrieval.nonconstFunc(f); 
    };
    bool constant(const TERM& c,const CLSkipList*& occList)
    {
      if (_ordLitRetrieval.constant(c))
	{
	  occList = &(_ordLitRetrieval.constIndObj());
	  return (bool)(*occList);
	}; 
      return false;
    };
    bool variable(const CLTSkipList*& occList)
    {
      occList = &(_ordLitRetrieval.indexedObj());  
      _ordLitRetrieval.after();
      return (bool)(*occList);
    };

    bool symLitHeader(const TERM& hd)
    {
      _symLitHeader = hd;
      return ((_symLitRetrieval.reset(_index->_symLitInd[0])) && (_symLitRetrieval.nonconstFunc(hd)));
    };
 
    bool symLitSecondArg()
    {
      return ((_symLitRetrieval.reset(_index->_symLitInd[0])) && (_symLitRetrieval.nonconstFunc(_symLitHeader)));   
    };

    bool symLitNonconstFunc(const TERM& f)
    {
      return _symLitRetrieval.nonconstFunc(f); 
    };
    bool symLitConstant(const TERM& c,const CLASkipList*& occList)
    {
      if (_symLitRetrieval.constant(c))
	{
	  occList = &(_symLitRetrieval.constIndObj());
	  return (bool)(*occList);
	}; 
      return false;
    };
    bool symLitVariable(const CLATSkipList*& occList)
    {
      occList = &(_symLitRetrieval.indexedObj());  
      _symLitRetrieval.after();
      return (bool)(*occList);
    };

  private:
    BSIndex* _index;
    OrdLitPITree::Retrieval _ordLitRetrieval;
    SymLitPITree::Retrieval _symLitRetrieval;
    TERM _symLitHeader;
  }; // class BSIndex::Retrieval

}; // namespace VK
//============================================================================
#undef DEBUG_NAMESPACE
#ifdef DEBUG_BS_INDEX
#define DEBUG_NAMESPACE "BSIndex::Removal"
#endif
#include "debugMacros.hpp"
//============================================================================
namespace VK
{
  class BSIndex::Removal
  {
  public:


    // VC++ 6.0 breaks down if I defin a constructor or destructor
    // for BSIndex::Removal

    /***************
    Removal(BSIndex* index) : _index(index)
    {
     CALL("constructor Removal(BSIndex* index)");
    };
    
    ~Removal()
    {
      CALL("destructor ~Removal()");
    };
    ****************/

    void init(BSIndex* index)
    {
      CALL("init(BSIndex* index)");
      _index = index;
      _ordLitRemoval.init();
      _symLitRemoval.init();
    };

    void destroy()
    {
      CALL("destroy()");
      _symLitRemoval.destroy();
      _ordLitRemoval.destroy();
      BK_CORRUPT(*this);
    };

    void reset(BSIndex* index) { _index = index; }; 
    void clause(const ulong& clNum) { _clauseNum = clNum; };
    void propLit(const TERM& lit);

    bool litHeader(const TERM& hd)
    {
      CALL("litHeader(const TERM& hd)");
      //_ordLitRemoval.reset();
      _ordLitRemoval.reset(_index->_ordLitInd[0]);
      return _ordLitRemoval.nonconstFunc(hd);
    };

    bool nonconstFunc(const TERM& f)
    {
      CALL("nonconstFunc(const TERM& f)");
      if (_ordLitRemoval.occurences()) BSIndex::remove(_clauseNum,_ordLitRemoval.indexedObj());
      return _ordLitRemoval.nonconstFunc(f);
    };
 
    bool constant(const TERM& c)
    {
      CALL("constant(const TERM& c)");

      if (_ordLitRemoval.occurences()) BSIndex::remove(_clauseNum,_ordLitRemoval.indexedObj());
      if (_ordLitRemoval.constant(c))
	{
	  BSIndex::remove(_clauseNum,_ordLitRemoval.constIndObj());
	  return true;
	}; 
      return false;
    };
 
    void variable()
    {
      CALL("variable()");
      if (_ordLitRemoval.occurences()) BSIndex::remove(_clauseNum,_ordLitRemoval.indexedObj());
      _ordLitRemoval.after();
    };
  
    void after() { _ordLitRemoval.after(); };

    bool symbol(const TERM& s) 
    {
      CALL("symbol(const TERM& s)");
      if (s.isVariable()) { variable(); return true; };
      if (s.arity()) return nonconstFunc(s); 
      return constant(s);
    };
 
    void endOfLit() { CALL("endOfLit()"); _ordLitRemoval.recycleObsoleteNodes(); };

    bool symLitHeader(const TERM& hd)
    {
      CALL("symLitHeader(const TERM& hd)");
      //_symLitRemoval.reset();
      _symLitHeader = hd;
      _symLitRemoval.reset(_index->_symLitInd[0]);
      return _symLitRemoval.nonconstFunc(hd);
    };
  
    bool symLitSecondArg()
    {
      CALL("symLitSecondArg()");
      _symLitRemoval.recycleObsoleteNodes();
      _symLitRemoval.reset(_index->_symLitInd[0]);
      return _symLitRemoval.nonconstFunc(_symLitHeader);  
    };
 
    bool symLitNonconstFunc(const TERM& f)
    {
      CALL("symLitNonconstFunc(const TERM& f)");
      if (_symLitRemoval.occurences()) BSIndex::remove(_clauseNum,_symLitRemoval.indexedObj());
      return _symLitRemoval.nonconstFunc(f);
    };
 
    bool symLitConstant(const TERM& c)
    {
      CALL("symLitConstant(const TERM& c)");
      if (_symLitRemoval.occurences()) BSIndex::remove(_clauseNum,_symLitRemoval.indexedObj());
      if (_symLitRemoval.constant(c))
	{
	  BSIndex::remove(_clauseNum,_symLitRemoval.constIndObj());
	  return true;
	}; 
      return false;
    };
 
    void symLitVariable()
    {
      CALL("symLitVariable()");
      if (_symLitRemoval.occurences()) BSIndex::remove(_clauseNum,_symLitRemoval.indexedObj());
      _symLitRemoval.after();
    };

    void symLitAfter() { _symLitRemoval.after(); };
    bool symLitSymbol(const TERM& s) 
    {
      CALL("symLitSymbol(const TERM& s)");
      if (s.isVariable()) { symLitVariable(); return true; };
      if (s.arity()) return symLitNonconstFunc(s); 
      return symLitConstant(s);
    };

    void endOfSymLit() { CALL("endOfSymLit()"); _symLitRemoval.recycleObsoleteNodes(); };

  private:
    BSIndex* _index;
    OrdLitPITree::Removal _ordLitRemoval;
    SymLitPITree::Removal _symLitRemoval;
    TERM _symLitHeader;
    ulong _clauseNum;
  }; // class BSIndex::Removal
}; // namespace VK


//======================================================================
#endif
