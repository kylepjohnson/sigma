//
// File:         ForwardMatchingIndex.hpp
// Description:  Interface for the code trees for matching.
// Created:      ???
// Author:       Alexandre Riazanov
// mail:         riazanov@cs.man.ac.uk
// Note:         This is a part of the Gematogen library.
// Revised:      Dec 25, 2001. 
//               ForwardMatchingIndex<..> freed from MaxNumOfFunctors
//               by replacing the top level array by an extendable one.
// Revised:      Apr 11, 2003, Alexandre Riazanov, riazanov@cs.man.ac.uk
//               Adjusted to the new interface of instruction representation.
//====================================================
#ifndef FORWARD_MATCHING_INDEX_H
//==================================================================
#define FORWARD_MATCHING_INDEX_H
#ifndef NO_DEBUG_VIS
#include <iostream>
#endif
#include "jargon.hpp"
#include "GematogenDebugFlags.hpp" 
#include "MultisetOfVariables.hpp"
#include "Array.hpp"
#include "DestructionMode.hpp"
#include "ForwardMatchingIndexTree.hpp"
//================================================
#undef DEBUG_NAMESPACE
#ifdef DEBUG_FORWARD_MATCHING_INDEX
#define DEBUG_NAMESPACE "ForwardMatchingIndex<MaxTermSize,MaxNumOfVariables,Alloc,Symbol,Flatterm,LeafInfo,InstanceId>"
#endif 
#include "debugMacros.hpp"
//=================================================

namespace Gem
{

template <ulong MaxTermSize,ulong MaxNumOfVariables,class Alloc,class Symbol,class Flatterm,class LeafInfo,class InstanceId>
class ForwardMatchingIndex
{ 
 public: 
  typedef Gem::FMCodeTree<MaxTermSize,MaxNumOfVariables,Alloc,Symbol,Flatterm,LeafInfo,InstanceId>::Instruction Instruction;
  typedef Gem::FMCodeTree<MaxTermSize,MaxNumOfVariables,Alloc,Symbol,Flatterm,LeafInfo,InstanceId>::Leaf Leaf;
 public:
  ForwardMatchingIndex() 
    : 
#ifdef NO_DEBUG 
    tree(0,0),
#else
    tree(0,"ForwardMatchingIndex<..>::tree"),
#endif
    integrator(subst), 
    removal(subst), 
    retrieval(), 
    _output()
    {
      CALL("constructor ForwardMatchingIndex()");
      DOP(tree.freeze());
      _output.SetSubst(subst);     
    };
  ~ForwardMatchingIndex() 
    { 
      CALL("destructor ~ForwardMatchingIndex()"); 
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
    tree.init(0,"ForwardMatchingIndex<..>::tree");
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
  const FMCodeTree<MaxTermSize,MaxNumOfVariables,Alloc,Symbol,Flatterm,LeafInfo,InstanceId>::Integrator& debugIntegrator() { return integrator; };
#endif
#ifndef NO_DEBUG_VIS
  ostream& output(ostream& str) const;
#endif


 private:

  BK::Array<Alloc,Instruction*,128UL,ForwardMatchingIndex> tree;


  const Flatterm* subst[MaxTermSize];
  FMCodeTree<MaxTermSize,MaxNumOfVariables,Alloc,Symbol,Flatterm,LeafInfo,InstanceId>::Integrator integrator;
  FMCodeTree<MaxTermSize,MaxNumOfVariables,Alloc,Symbol,Flatterm,LeafInfo,InstanceId>::Removal removal;
  FMCodeTree<MaxTermSize,MaxNumOfVariables,Alloc,Symbol,Flatterm,LeafInfo,InstanceId>::Retrieval retrieval;
  FMCodeTree<MaxTermSize,MaxNumOfVariables,Alloc,Symbol,Flatterm,LeafInfo,InstanceId>::Output _output;

}; // template <ulong MaxTermSize,ulong MaxNumOfVariables,class Alloc,class Symbol,class Flatterm,class LeafInfo,class InstanceId> class ForwardMatchingIndex

template <ulong MaxTermSize,ulong MaxNumOfVariables,class Alloc,class Symbol,class Flatterm,class LeafInfo,class InstanceId>
inline FMCodeTree<MaxTermSize,MaxNumOfVariables,Alloc,Symbol,Flatterm,LeafInfo,InstanceId>::Leaf* ForwardMatchingIndex<MaxTermSize,MaxNumOfVariables,Alloc,Symbol,Flatterm,LeafInfo,InstanceId>::Match(const Flatterm* query)
{
  CALL("Match(const Flatterm* query)");
  ASSERT(query->symbolRef().isComplex());
  ulong funcNum = query->symbolRef().functor();
  if (funcNum >= tree.size()) 
    return 0;
  return retrieval.Match(query + 1,tree[funcNum]);    
}; // Leaf* ForwardMatchingIndex<MaxTermSize,MaxNumOfVariables,Alloc,Symbol,Flatterm,LeafInfo,InstanceId>::Match(const Flatterm* query) 


#ifndef NO_DEBUG_VIS
template <ulong MaxTermSize,ulong MaxNumOfVariables,class Alloc,class Symbol,class Flatterm,class LeafInfo,class InstanceId>
ostream& ForwardMatchingIndex<MaxTermSize,MaxNumOfVariables,Alloc,Symbol,Flatterm,LeafInfo,InstanceId>::output(ostream& str) const
{
  CALL("output(ostream& str) const");
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
}; // ostream& ForwardMatchingIndex<MaxTermSize,MaxNumOfVariables,Alloc,Symbol,Flatterm,LeafInfo,InstanceId>::output(ostream& str) const
#endif

}; // namespace Gem

//==================================================================
#endif





















