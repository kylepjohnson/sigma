//
// File:         VK::BSIndex.cpp
// Description:  Path index for backward subsumption.
// Created:      Feb 26, 2000.
// Author:       Alexandre Riazanov
// mail:         riazanov@cs.man.ac.uk
// Revised:      Dec 23, 2001. 
//               1) The array VK::BSIndex::_propLitInd replaced by
//               extendable array VK::BSIndex::_propLitOccurences
//               2) Freed from HellConst::MaxNumOfDifferentHeaders.
//============================================================================
#include "BSIndex.hpp"

//============================================================================
#undef DEBUG_NAMESPACE
#ifdef DEBUG_BS_INDEX
#  define DEBUG_NAMESPACE "VK::BSIndex"
#endif
#include "debugMacros.hpp"
//============================================================================

using namespace BK;
using namespace VK;

#ifdef DEBUG_ALLOC_OBJ_TYPE
ClassDesc BSIndex::LList::_classDesc("BSIndex::LList",
                                      BSIndex::LList::minSize(),
                                      BSIndex::LList::maxSize());
ClassDesc BSIndex::LTList::_classDesc("BSIndex::LTList",
                                       BSIndex::LTList::minSize(),
                                       BSIndex::LTList::maxSize());
ClassDesc BSIndex::LAList::_classDesc("BSIndex::LAList",
                                       BSIndex::LAList::minSize(),
                                       BSIndex::LAList::maxSize());
ClassDesc BSIndex::LATList::_classDesc("BSIndex::LATList",
                                        BSIndex::LATList::minSize(),
                                        BSIndex::LATList::maxSize());

ClassDesc BSIndex::OrdLitPITree::FuncNode::_funcClassDesc("BSIndex::OrdLitPITree::FuncNode(nonconst)",
                                                           BSIndex::OrdLitPITree::FuncNode::funcMinSize(),
                                                           BSIndex::OrdLitPITree::FuncNode::funcMaxSize());
ClassDesc BSIndex::OrdLitPITree::FuncNode::_constClassDesc("BSIndex::OrdLitPITree::FuncNode(const)",
                                                          BSIndex::OrdLitPITree::FuncNode::constMinSize(),
                                                          BSIndex::OrdLitPITree::FuncNode::constMaxSize());

ClassDesc BSIndex::SymLitPITree::FuncNode::_funcClassDesc("BSIndex::SymLitPITree::FuncNode(nonconst)",
                                                          BSIndex::SymLitPITree::FuncNode::funcMinSize(),
                                                          BSIndex::SymLitPITree::FuncNode::funcMaxSize());
ClassDesc BSIndex::SymLitPITree::FuncNode::_constClassDesc("BSIndex::SymLitPITree::FuncNode(const)",
                                                          BSIndex::SymLitPITree::FuncNode::constMinSize(),
                                                          BSIndex::SymLitPITree::FuncNode::constMaxSize());


ClassDesc BSIndex::OrdLitPITree::OccNode::_classDesc("BSIndex::OrdLitPITree::OccNode",
                                                      BSIndex::OrdLitPITree::OccNode::minSize(),
                                                      BSIndex::OrdLitPITree::OccNode::maxSize());
ClassDesc BSIndex::SymLitPITree::OccNode::_classDesc("BSIndex::SymLitPITree::OccNode",
                                                      BSIndex::SymLitPITree::OccNode::minSize(),
                                                      BSIndex::SymLitPITree::OccNode::maxSize());
#endif

#ifdef DEBUG_ALLOC_OBJ_TYPE
 #ifndef BS_USE_ORD_LISTS
  ClassDesc BSIndex::CSkipList::_classDesc("BSIndex::CSkipList",
                                            BSIndex::CSkipList::minSize(),
                                            BSIndex::CSkipList::maxSize());
  ClassDesc BSIndex::CSkipList::Node::_classDesc("BSIndex::CSkipList::Node",
                                                 BSIndex::CSkipList::Node::minSize(),
                                                 BSIndex::CSkipList::Node::maxSize());
  ClassDesc BSIndex::CLSkipList::_classDesc("BSIndex::CLSkipList",
                                             BSIndex::CLSkipList::minSize(),
                                             BSIndex::CLSkipList::maxSize());
  ClassDesc BSIndex::CLSkipList::Node::_classDesc("BSIndex::CLSkipList::Node",
                                                  BSIndex::CLSkipList::Node::minSize(),
                                                  BSIndex::CLSkipList::Node::maxSize());
  ClassDesc BSIndex::CLTSkipList::_classDesc("BSIndex::CLTSkipList",
                                              BSIndex::CLTSkipList::minSize(),
                                              BSIndex::CLTSkipList::maxSize());
  ClassDesc BSIndex::CLTSkipList::Node::_classDesc("BSIndex::CLTSkipList::Node",
                                                   BSIndex::CLTSkipList::Node::minSize(),
                                                   BSIndex::CLTSkipList::Node::maxSize());
  ClassDesc BSIndex::CLASkipList::_classDesc("BSIndex::CLASkipList",
                                              BSIndex::CLASkipList::minSize(),
                                              BSIndex::CLASkipList::maxSize());
  ClassDesc BSIndex::CLASkipList::Node::_classDesc("BSIndex::CLASkipList::Node",
                                                   BSIndex::CLASkipList::Node::minSize(),
                                                   BSIndex::CLASkipList::Node::maxSize());
  ClassDesc BSIndex::CLATSkipList::_classDesc("BSIndex::CLATSkipList",
                                               BSIndex::CLATSkipList::minSize(),
                                               BSIndex::CLATSkipList::maxSize());
  ClassDesc BSIndex::CLATSkipList::Node::_classDesc("BSIndex::CLATSkipList::Node",
                                                    BSIndex::CLATSkipList::Node::minSize(),
                                                    BSIndex::CLATSkipList::Node::maxSize());
 #endif
#endif



BSIndex::BSIndex() 
  : 
#ifdef NO_DEBUG
  _propLitOccurences(static_cast<CSkipList*>(0),(int)0),
#else
  _propLitOccurences(static_cast<CSkipList*>(0),"BSIndex::_propLitOccurences"),    
#endif 

  _sizes()
{
  CALL("constructor BSIndex()");
    
  DOP(_propLitOccurences.freeze());
  _ordLitInd[0].init(); 
  _symLitInd[0].init(); 
}; // BSIndex::BSIndex() 

BSIndex::~BSIndex() 
{ 
  CALL("destructor ~BSIndex()"); 
  if (DestructionMode::isThorough()) 
    {
      for (ulong i = 0; i < _propLitOccurences.size(); i++) 
	if (_propLitOccurences[i])
	  delete (_propLitOccurences[i]);

      bool constant;

      OrdLitPITreeDestruction _ordLitIndDestruction;

      if (_ordLitIndDestruction.reset(_ordLitInd[0],constant))
	{ 
	  do 
	    {
	      if (constant) { recycle(_ordLitIndDestruction.constIndObj()); }
	      else recycle(_ordLitIndDestruction.indexedObj());
	    }
	  while (_ordLitIndDestruction.next(constant));
	  _ordLitIndDestruction.recycle(); 
	};

      SymLitPITreeDestruction _symLitIndDestruction;
      if (_symLitIndDestruction.reset(_symLitInd[0],constant))
	{ 
	  do 
	    {
	      if (constant) 
		{ 
		  recycle(_symLitIndDestruction.constIndObj()); 
		  ASSERT(!_symLitIndDestruction.constIndObj());
		}
	      else 
		{
		  recycle(_symLitIndDestruction.indexedObj());
		  ASSERT(!_symLitIndDestruction.indexedObj());
		};
	    }
	  while (_symLitIndDestruction.next(constant));
	  _symLitIndDestruction.recycle(); 
	};

    };
}; // BSIndex::~BSIndex() 

void BSIndex::init()
{
#ifdef NO_DEBUG
  _propLitOccurences.init(static_cast<CSkipList*>(0),(int)0);
#else
  _propLitOccurences.init(static_cast<CSkipList*>(0),"BSIndex::_propLitOccurences");   
#endif 

  DOP(_propLitOccurences.freeze());
  _ordLitInd[0].init(); 
  _symLitInd[0].init(); 
  _sizes.init();
}; // void BSIndex::init()


void BSIndex::destroy() 
{	
  CALL("destroy()");
  if (DestructionMode::isThorough()) 
    {
      for (ulong i = 0; i < _propLitOccurences.size(); i++) 
	if (_propLitOccurences[i])
	  delete (_propLitOccurences[i]);

      bool constant;

      OrdLitPITreeDestruction _ordLitIndDestruction;

      if (_ordLitIndDestruction.reset(_ordLitInd[0],constant))
	{ 
	  do 
	    {
	      if (constant) { recycle(_ordLitIndDestruction.constIndObj()); }
	      else recycle(_ordLitIndDestruction.indexedObj());
	    }
	  while (_ordLitIndDestruction.next(constant));
	  _ordLitIndDestruction.recycle(); 
	};

      SymLitPITreeDestruction _symLitIndDestruction;
      if (_symLitIndDestruction.reset(_symLitInd[0],constant))
	{ 
	  do 
	    {
	      if (constant) 
		{ 
		  recycle(_symLitIndDestruction.constIndObj()); 
		  ASSERT(!_symLitIndDestruction.constIndObj());
		}
	      else 
		{
		  recycle(_symLitIndDestruction.indexedObj());
		  ASSERT(!_symLitIndDestruction.indexedObj());
		};
	    }
	  while (_symLitIndDestruction.next(constant));
	  _symLitIndDestruction.recycle(); 
	};

    };

  _sizes.destroy();
  _symLitInd[0].destroy(); 
  _ordLitInd[0].destroy(); 
  _propLitOccurences.destroy();
  BK_CORRUPT(*this);
}; //  void BSIndex::destroy()  


inline
void BSIndex::insertPropLit(const TERM& lit,ulong clauseNum)
{
  CALL("insertPropLit(const TERM& lit,ulong clauseNum)");
  DOP(_propLitOccurences.unfreeze());
  CSkipList* occList = _propLitOccurences.sub(lit.HeaderNum());
  DOP(_propLitOccurences.freeze());
  if (!occList) 
    {
      occList = new CSkipList();
      _propLitOccurences[lit.HeaderNum()] = occList;
    };
  occList->insert(clauseNum);
  //_propLitInd[lit.HeaderNum()].insert(clauseNum);
};


inline
void BSIndex::removePropLit(const TERM& lit,ulong clauseNum)
{
  CALL("removePropLit(const TERM& lit,ulong clauseNum)");
  if (_propLitOccurences[lit.HeaderNum()])
    _propLitOccurences[lit.HeaderNum()]->remove(clauseNum);
};


//============================================================================
#undef DEBUG_NAMESPACE
#ifdef DEBUG_BS_INDEX
#define DEBUG_NAMESPACE "BSIndex::Integrator"
#endif
#include "debugMacros.hpp"
//============================================================================

void BSIndex::Integrator::propLit(const TERM& lit)
{
  CALL("propLit(const TERM& lit)");
  _index->insertPropLit(lit,_clauseNum);
};


void BSIndex::Integrator::subterm(const TERM& t)
{
  CALL("subterm(const TERM& t)");
  bool newNode;
  CLTSkipList::Node* node = _ordLitIntegrator.indexedObj().insert(_clauseNum,newNode);  
  if (newNode)
    {
      node->value() = new LTList(LTPair(_litNum,t)); 
    }
  else
    {
      ASSERT(node->value());
      ASSERT(_litNum > node->value()->hd().el1);
      node->value() = new LTList(LTPair(_litNum,t),node->value()); 
    };
  if (t.isVariable())
    {
      _ordLitIntegrator.variable();
    } 
  else // t is reference to a complex term 
    {
      ASSERT(t.IsReference());
      TERM sym = *(t.First());
      if (sym.arity())
	{
	  _ordLitIntegrator.nonconstFunc(sym);
	}
      else // constant
	{
	  _ordLitIntegrator.constant(sym);
	  CLSkipList::Node* cnode = _ordLitIntegrator.constIndObj().insert(_clauseNum,newNode); 
	  if (newNode) 
	    { 
	      cnode->value() = new LList(_litNum);
	    }    
	  else 
	    {
	      ASSERT(cnode->value());
	      ASSERT(_litNum > cnode->value()->hd());
	      cnode->value() = new LList(_litNum,cnode->value());
	    };
	};
    };
}; // void BSIndex::Integrator::subterm(const TERM& t)



void BSIndex::Integrator::symLitSubterm(const TERM& t)
{
  CALL("symLitSubterm(const TERM& t)");
  bool newNode;


  CLATSkipList::Node* node = _symLitIntegrator.indexedObj().insert(_clauseNum,newNode);
  ASSERT(newNode || greater(LATTriple(_litNum,_symLitArgNum,t),node->value()->hd()));
  if (newNode) node->value() = 0;  
  node->value() = new LATList(LATTriple(_litNum,_symLitArgNum,t),node->value());
  if (t.isVariable())
    {   
      _symLitIntegrator.variable();  
    } 
  else // t is reference to a complex term 
    {    
      ASSERT(t.IsReference()); 
      TERM sym = *(t.First());
      if (sym.arity())
	{  
	  _symLitIntegrator.nonconstFunc(sym);
	}
      else // constant
	{  
	  _symLitIntegrator.constant(sym); 
	  CLASkipList::Node* cnode = _symLitIntegrator.constIndObj().insert(_clauseNum,newNode);
	  ASSERT(newNode || greater(LAPair(_litNum,_symLitArgNum),cnode->value()->hd()));
	  if (newNode) cnode->value() = 0; 
	  cnode->value() = new LAList(LAPair(_litNum,_symLitArgNum),cnode->value());
	};
    };
  
}; // void BSIndex::Integrator::symLitSubterm(const TERM& t)



//============================================================================
#undef DEBUG_NAMESPACE
#ifdef DEBUG_BS_INDEX
#define DEBUG_NAMESPACE "BSIndex::Removal"
#endif
#include "debugMacros.hpp"
//============================================================================

void BSIndex::Removal::propLit(const TERM& lit)
{
  CALL("propLit(const TERM& lit)");
  _index->removePropLit(lit,_clauseNum); 
};

//============================================================================
