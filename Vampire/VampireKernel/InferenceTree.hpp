//
// File:         InferenceTree.hpp
// Description:  Collecting inference trees.
// Created:      Feb 13, 2000, 18:00
// Author:       Alexandre Riazanov
// mail:         riazanov@cs.man.ac.uk
//============================================================================
#ifndef INFERENCE_TREE_H
//=============================================================================
#define INFERENCE_TREE_H 
#include "jargon.hpp"
#include "VampireKernelDebugFlags.hpp"
#include "GSortTree.hpp"
#include "Clause.hpp"
#include "GlobAlloc.hpp"
//============================================================================
#undef DEBUG_NAMESPACE
#ifdef DEBUG_INFERENCE_TREE
 #define DEBUG_NAMESPACE "InferenceTree"
#endif
#include "debugMacros.hpp"
//============================================================================
namespace VK
{
class InferenceTree
{
 public:
  InferenceTree();
  InferenceTree(const Clause* root);
  ~InferenceTree();
  void init();
  void destroy();
  void collect(const Clause* root);
  void resetIter() { iter.reset(collected); };
  const Clause* getNextClause() 
  {
   if (iter)
    {
     const Clause* res = iter.value().clause();
     iter.next();
     return res; 
    }; 
   return 0; 
  };
 private:
  class ClWrapper
  {
   public:
    ClWrapper(const Clause* c) : cl(c) {};
    ~ClWrapper() {};
    const Clause* clause() const { return cl; };
    bool operator==(const ClWrapper& cw) const
    {
     return cl->Number() == cw.clause()->Number();
    };
    bool operator<(const ClWrapper& cw) const
    {
     return cl->Number() < cw.clause()->Number();
    };
    bool operator>(const ClWrapper& cw) const
    {
     return cl->Number() > cw.clause()->Number();
    };
   private:
    ClWrapper() { ICP("Default constructor called"); };
   private:  
    const Clause* cl; 
  };
public:
  // ClBag cannot be made private since it has nested classes with static
  // data members
  typedef BK::GSortTree<BK::GlobAlloc,ClWrapper,InferenceTree> ClBag;
 private:
  void collectInf(const Clause* root);
 private:
  ClBag collected;
  ClBag::LeftRightIter iter;    
}; //class InferenceTree 

}; // namespace VK

//======================================================================
#endif
