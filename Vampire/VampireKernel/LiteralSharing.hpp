//
// File:         LiteralSharing.hpp
// Description:  Implements literal sharing.
// Created:      Dec 13, 2001, Alexandre Riazanov, riazanov@cs.man.ac.uk
// Note:         Was LITERAL_INDEX in clause_index.h.
// Revised:      Dec 25, 2001, Alexandre Riazanov, riazanov@cs.man.ac.uk
//               Freed from HellConst::MaxNumOfFunctors.
//============================================================================
#ifndef LITERAL_SHARING_H
//=============================================================================
#define LITERAL_SHARING_H 
#include "jargon.hpp"
#include "VampireKernelDebugFlags.hpp"
#include "GlobAlloc.hpp"
#include "Array.hpp"
#include "Term.hpp"
//============================================================================
#undef DEBUG_NAMESPACE
#ifdef DEBUG_LITERAL_SHARING
 #define DEBUG_NAMESPACE "LiteralSharing"
#endif
#include "debugMacros.hpp"
//============================================================================
namespace VK
{
class Flatterm;
class DTREE_NODE;
class SHARING_INDEX;
class LiteralSharing
{
 public:
  LiteralSharing(SHARING_INDEX* si);
  ~LiteralSharing();
  void init(SHARING_INDEX* si);
  void destroy(); 
  void reset();

  DTREE_NODE** Tree(const TERM& header)
  {
   CALL("Tree(const TERM& header)");
   DOP(_discTree.unfreeze());
   DTREE_NODE** res = &(_discTree.sub(header.HeaderNum()));
   DOP(_discTree.freeze());
   return res;
  };

  DTREE_NODE* Integrate(TERM* lit); // returns the leaf
 
  DTREE_NODE* Integrate(const Flatterm* lit); // returns the leaf 
  DTREE_NODE* IntegrateEq(const TERM& header,
                          const Flatterm* arg1,
                          const Flatterm* arg2); // returns the leaf 


  DTREE_NODE* FindLeaf(TERM* lit) // returns the leaf
  {
   // temporary version
   return Integrate(lit); 
  };

  void InfoForDeletion(TERM* lit,
                       DTREE_NODE**& branch_to_destroy,
                       DTREE_NODE*& leaf,
                       DTREE_NODE*& last_fork); // for nonpropositional literals


 private:
 
  static void DestroyLeafContent(DTREE_NODE* leaf);
  TERM* ShareSplittingBranchName(const TERM& prop);
  TERM* ShareProp(const TERM& prop);

 private:

  BK::Array<BK::GlobAlloc,TERM*,128UL,LiteralSharing> _propositional;  

  SHARING_INDEX* _sharingIndex;
  BK::Array<BK::GlobAlloc,DTREE_NODE*,128UL,LiteralSharing> _discTree;
}; // class LiteralSharing

}; // namespace VK
//============================================================================
#endif
