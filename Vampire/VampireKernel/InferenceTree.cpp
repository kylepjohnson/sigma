//
// File:         InferenceTree.cpp
// Description:  Collecting inference trees.
// Created:      Apr 13, 2001, 17:30
// Author:       Alexandre Riazanov
// mail:         riazanov@cs.man.ac.uk
//============================================================================
#include "InferenceTree.hpp"
//============================================================================
#undef DEBUG_NAMESPACE
#ifdef DEBUG_INFERENCE_TREE
 #define DEBUG_NAMESPACE "InferenceTree"
#endif
#include "debugMacros.hpp"
//============================================================================

using namespace BK;
using namespace VK;

#ifdef DEBUG_ALLOC_OBJ_TYPE
 #include "ClassDesc.hpp"
 ClassDesc InferenceTree::ClBag::LeftRightIter::NodeList::_classDesc("InferenceTree::ClBag::LeftRightIter::NodeList",
                                                                     InferenceTree::ClBag::LeftRightIter::NodeList::minSize(),
                                                                     InferenceTree::ClBag::LeftRightIter::NodeList::maxSize());


 ClassDesc InferenceTree::ClBag::RightLeftIter::NodeList::_classDesc("InferenceTree::ClBag::RightLeftIter::NodeList",
                                                                     InferenceTree::ClBag::RightLeftIter::NodeList::minSize(),
                                                                     InferenceTree::ClBag::RightLeftIter::NodeList::maxSize());

 ClassDesc InferenceTree::ClBag::Node::_classDesc("InferenceTree::ClBag::Node",
                                                  InferenceTree::ClBag::Node::minSize(),
                                                  InferenceTree::ClBag::Node::maxSize());
#endif


InferenceTree::InferenceTree() 
{
  CALL("constructor InferenceTree()");
};
InferenceTree::InferenceTree(const Clause* root) : collected(), iter()
{
  CALL("constructor InferenceTree(const Clause* root)");
  collect(root);
};

InferenceTree::~InferenceTree() { CALL("destructor ~InferenceTree()"); };

void InferenceTree::init()
{
  CALL("init()");
  collected.init();
  iter.init();
};
  
void InferenceTree::destroy()
{
  CALL("destroy()");
  iter.destroy();
  collected.destroy();
};

void InferenceTree::collect(const Clause* root)
{
  CALL("collect(const Clause* root)");
  collected.destroy();
  collected.init();
  collectInf(root);
};

void InferenceTree::collectInf(const Clause* root)
{
  CALL("collectInf(const Clause* root)");
  unsigned long oldScore;
  collected.insert(ClWrapper(root),oldScore);
  if (!oldScore)
    {

      if ((!root->isInputClause()) && 
	  (!root->isBuiltInTheoryFact()))
	{
	  for (const ClauseBackground::ClauseList* anc = root->background().ancestors(); 
	       anc; 
	       anc = anc->tl())
	    collectInf(anc->hd());
	};
    };
};




//============================================================================
