//
// File:         OldBSIndex.cpp
// Description:  Simple index for backward subsumption based on discrimination trees.
// Created:      Jan 15, 2001, 14:30
// Author:       Alexandre Riazanov
// mail:         riazanov@cs.man.ac.uk
//============================================================================
#include "OldBSIndex.hpp"
//============================================================================
#undef DEBUG_NAMESPACE
#ifdef DEBUG_OLD_BS_INDEX
 #define DEBUG_NAMESPACE "OldBSIndex"
#endif
#include "debugMacros.hpp"
//============================================================================
 
  using namespace BK;
  using namespace Gem;
using namespace VK;


#ifdef DEBUG_ALLOC_OBJ_TYPE
 #include "ClassDesc.hpp"
 ClassDesc OldBSIndex::DiscTree::Node::_nonleafNodeClassDesc("OldBSIndex::DiscTree::Node");
 ClassDesc OldBSIndex::DiscTree::Node::_leafClassDesc("OldBSIndex::DiscTree::Node");
 ClassDesc OldBSIndex::CLSkipList::_classDesc("OldBSIndex::CLSkipList");
 ClassDesc OldBSIndex::CLSkipList::Node::_classDesc("OldBSIndex::CLSkipList::Node");
 ClassDesc OldBSIndex::CSkipList::_classDesc("OldBSIndex::CSkipList");
 ClassDesc OldBSIndex::CSkipList::Node::_classDesc("OldBSIndex::CSkipList::Node");
#endif 

void OldBSIndex::destroy()
{
 CALL("destroy()");
 for (ulong i = 0; i < VampireKernelConst::MaxNumOfDifferentHeaders; i++) _propLitInd[i].destroy();
 
}; // void OldBSIndex::destroy()



//======================================================================
#undef DEBUG_NAMESPACE
//============================================================================
