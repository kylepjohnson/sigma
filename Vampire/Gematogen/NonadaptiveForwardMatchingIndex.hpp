//
// File:         NonadaptiveForwardMatchingIndex.hpp
// Description:  Interface for the code trees for matching.
// Created:      Mar 25, 2003, Alexandre Riazanov, riazanov@cs.man.ac.uk
// Note:         This is a part of the Gematogen library.
// Revised:      Apr 11, 2003, Alexandre Riazanov, riazanov@cs.man.ac.uk
//               Adjusted to the new interface of instruction representation.
//====================================================
#ifndef NONADAPTIVE_FORWARD_MATCHING_INDEX_H
//==================================================================
#define NONADAPTIVE_FORWARD_MATCHING_INDEX_H
#ifndef NO_DEBUG_VIS
#include <iostream>
#endif
#include "jargon.hpp"
#include "GematogenDebugFlags.hpp" 
#include "MultisetOfVariables.hpp"
#include "Array.hpp"
#include "DestructionMode.hpp"
#include "ForwardMatchingIndexTree.hpp"
#include "NonadaptiveForwardMatchingIndexTree.hpp"
//================================================
#undef DEBUG_NAMESPACE
#ifdef DEBUG_FORWARD_MATCHING_INDEX
#define DEBUG_NAMESPACE "NonadaptiveForwardMatchingIndex<MaxTermSize,MaxNumOfVariables,Alloc,Symbol,Flatterm,LeafInfo,InstanceId>"
#endif 
#include "debugMacros.hpp"
//=================================================

namespace Gem
{

template <ulong MaxTermSize,ulong MaxNumOfVariables,class Alloc,class Symbol,class Flatterm,class LeafInfo,class InstanceId>
class NonadaptiveForwardMatchingIndex
{ 
 public: 
  typedef Gem::FMCodeTree<MaxTermSize,MaxNumOfVariables,Alloc,Symbol,Flatterm,LeafInfo,InstanceId>::Instruction Instruction;
  typedef Gem::FMCodeTree<MaxTermSize,MaxNumOfVariables,Alloc,Symbol,Flatterm,LeafInfo,InstanceId>::Leaf Leaf;
 public:
  NonadaptiveForwardMatchingIndex() 
    : 
#ifdef NO_DEBUG 
    tree(0,0),
#else
    tree(0,"NonadaptiveForwardMatchingIndex<..>::tree"),
#endif
    integrator(subst), 
    removal(subst), 
    retrieval(), 
    _output()
    {
      CALL("constructor NonadaptiveForwardMatchingIndex()");
      DOP(tree.freeze());
      _output.SetSubst(subst);     
    };
  ~NonadaptiveForwardMatchingIndex() 
    { 
      CALL("destructor ~NonadaptiveForwardMatchingIndex()"); 
      if (BK::DestructionMode::isThorough()) 
	{
	  for (ulong f = 0; f < tree.size(); f++) 
	    {
	      if (tree[f])
		{ 
		  FMCodeTree<MaxTermSize,MaxNumOfVariables,Alloc,Symbol,Flatterm,LeafInfo,InstanceId>::Destroy(tree[f]);   
		};
	    };	  
	};
    }; 
   
  void init()
  {
    CALL("init()");
    
#ifdef NO_DEBUG 
    tree.init(0,0);
#else
    tree.init(0,"NonadaptiveForwardMatchingIndex<..>::tree");
#endif
    integrator.init(subst);
    removal.init(subst); 
    retrieval.init();
    _output.init();
    DOP(tree.freeze());
    _output.SetSubst(subst);    
  }; // void init()

  void destroy()       
    {
      CALL("destroy()");
      if (BK::DestructionMode::isThorough()) 
	{
	  for (ulong f = 0; f < tree.size(); f++) 
	    {
	      if (tree[f])
		{ 
		  FMCodeTree<MaxTermSize,MaxNumOfVariables,Alloc,Symbol,Flatterm,LeafInfo,InstanceId>::Destroy(tree[f]);   
		};
	    };
	};
      _output.destroy();
      retrieval.destroy();
      removal.destroy();
      integrator.destroy();
      tree.destroy();
      BK_CORRUPT(*this);
    }; // void destroy()    
  


  inline void integrationReset(const Symbol& fun)
    {
      CALL("integrationReset(const Symbol& fun)");
      ASSERT(fun.isComplex());
      DOP(tree.unfreeze());
      integrator.Reset(&(tree.sub(fun.functor())));
      DOP(tree.freeze());
    };    
  inline void integrationVar(const Symbol& var) 
    {
      CALL("integrationVar(const Symbol& var)");
      ASSERT(var.isVariable());
      integrator.PushVar(var);
    }; 
  inline void integrationFunc(const Symbol& fun)
    {
      CALL("integrationFunc(const Symbol& fun)");
      ASSERT(fun.isComplex());
      integrator.PushFunc(fun);
    };
  Leaf* integrationGetLeaf(bool& newLeaf)  
    {
      CALL("integrationGetLeaf()");
      return integrator.GetLeaf(newLeaf);
    };   
  

  BK::MultisetOfVariables<MaxNumOfVariables>& integrationGetVariables() { return integrator.Vars(); };
  const Flatterm** integrationMapVariable(ulong v) { return integrator.MapVar(v); };

  bool removalReset(const Symbol& fun)
    {
      CALL("removalReset(const Symbol& fun)");
      ASSERT(fun.isComplex());
      if (fun.functor() >= tree.size()) return false;
      Instruction** tree_addr = &(tree[fun.functor()]);  
      if (!(*tree_addr)) return false;
      removal.Reset(tree_addr);
      return true;
    };
   
  bool removalVar(const Symbol& var) 
    {
      CALL("removalVar(const Symbol& var)");
      ASSERT(var.isVariable());
      return removal.SkipVar(var);
    };
   
  bool removalFunc(const Symbol& fun) 
    {
      CALL("removalFunc(const Symbol& fun)");
      ASSERT(fun.isComplex());
      return removal.SkipFunc(fun);
    };
 
  bool removalFinish(Leaf*& leaf,Instruction**& branch_to_destroy)
    {
      CALL("removalGetLeaf(Leaf*& leaf,Instruction**& branch_to_destroy)");
      if (!removal.SkipComps()) return false;
      leaf = removal.GetLeaf();
      ASSERT(leaf->isLeaf());   
      branch_to_destroy = removal.BranchToDestroy();
      return true;
    };

  Leaf* Match(const Flatterm* query);
  Leaf* MatchAgain() 
    { 
      CALL("MatchAgain()"); 
      return retrieval.MatchAgain(); 
    };

 
#ifndef NO_DEBUG
  bool clean() const
    {
      CALL("clean() const");
      for (ulong f = 0; f < tree.size(); f++)  
	if (tree[f]) return false;
      return true; 
    };
  const FMCodeTreeNonadaptiveIntegrator<MaxTermSize,MaxNumOfVariables,Alloc,Symbol,Flatterm,LeafInfo,InstanceId>& debugIntegrator() { return integrator; };
#endif
#ifndef NO_DEBUG_VIS
  ostream& output(ostream& str) const;
#endif


 private:

  BK::Array<Alloc,Instruction*,128UL,NonadaptiveForwardMatchingIndex> tree;


  const Flatterm* subst[MaxTermSize];
  FMCodeTreeNonadaptiveIntegrator<MaxTermSize,MaxNumOfVariables,Alloc,Symbol,Flatterm,LeafInfo,InstanceId> integrator;
  FMCodeTreeNonadaptiveRemoval<MaxTermSize,MaxNumOfVariables,Alloc,Symbol,Flatterm,LeafInfo,InstanceId> removal;
  FMCodeTreeNonadaptiveRetrieval<MaxTermSize,MaxNumOfVariables,Alloc,Symbol,Flatterm,LeafInfo,InstanceId> retrieval;
  FMCodeTree<MaxTermSize,MaxNumOfVariables,Alloc,Symbol,Flatterm,LeafInfo,InstanceId>::Output _output;

}; // template <ulong MaxTermSize,ulong MaxNumOfVariables,class Alloc,class Symbol,class Flatterm,class LeafInfo,class InstanceId> class NonadaptiveForwardMatchingIndex

template <ulong MaxTermSize,ulong MaxNumOfVariables,class Alloc,class Symbol,class Flatterm,class LeafInfo,class InstanceId>
inline FMCodeTree<MaxTermSize,MaxNumOfVariables,Alloc,Symbol,Flatterm,LeafInfo,InstanceId>::Leaf* NonadaptiveForwardMatchingIndex<MaxTermSize,MaxNumOfVariables,Alloc,Symbol,Flatterm,LeafInfo,InstanceId>::Match(const Flatterm* query)
{
  CALL("Match(const Flatterm* query)");
  ASSERT(query->symbolRef().isComplex());
  ulong funcNum = query->symbolRef().functor();
  if (funcNum >= tree.size()) 
    return 0;
  return retrieval.Match(query + 1,tree[funcNum]);    
}; // Leaf* NonadaptiveForwardMatchingIndex<MaxTermSize,MaxNumOfVariables,Alloc,Symbol,Flatterm,LeafInfo,InstanceId>::Match(const Flatterm* query) 


#ifndef NO_DEBUG_VIS
template <ulong MaxTermSize,ulong MaxNumOfVariables,class Alloc,class Symbol,class Flatterm,class LeafInfo,class InstanceId>
ostream& NonadaptiveForwardMatchingIndex<MaxTermSize,MaxNumOfVariables,Alloc,Symbol,Flatterm,LeafInfo,InstanceId>::output(ostream& str) const
{
  for (ulong f = 0; f < tree.size(); f++)
    {
      const Instruction* t = tree[f];
      if (t)
	{
	  Symbol functor(0UL,f,Symbol::PositivePolarity);
	  str << "HEADER " << functor << '\n';
	  _output.Tree(str,t) << "\nEND OF TREE FOR " << functor << "\n\n";
	};  
    };  
  return str;
}; // ostream& NonadaptiveForwardMatchingIndex<MaxTermSize,MaxNumOfVariables,Alloc,Symbol,Flatterm,LeafInfo,InstanceId>::output(ostream& str) const
#endif

}; // namespace Gem

//==================================================================
#endif





















