//
// File:         ForwardMatchingWithOriginalCodeTrees.hpp
// Description:  Interface for the original code trees 
//               for forward matching (search for generalisations).
// Created:      Apr 07, 2003, Alexandre Riazanov, riazanov@cs.man.ac.uk
// Note:         This is a part of the Gematogen library.
//====================================================
#ifndef FORWARD_MATCHING_WITH_ORIGINAL_CODE_TREES_H
//==================================================================
#define FORWARD_MATCHING_WITH_ORIGINAL_CODE_TREES_H
#ifndef NO_DEBUG_VIS
#include <iostream>
#endif
#include "jargon.hpp"
#include "GematogenDebugFlags.hpp" 
#include "MultisetOfVariables.hpp"
#include "Array.hpp"
#include "DestructionMode.hpp"
#include "OriginalFMCodeTree.hpp"
//================================================
#undef DEBUG_NAMESPACE
#ifdef DEBUG_FORWARD_MATCHING_INDEX
#define DEBUG_NAMESPACE "ForwardMatchingWithOriginalCodeTrees<MaxTermSize,MaxNumOfVariables,Alloc,Symbol,Term,SuccessInfo,InstanceId>"
#endif 
#include "debugMacros.hpp"
//=================================================

namespace Gem
{
  template <ulong MaxTermSize,ulong MaxNumOfVariables,class Alloc,class Symbol,class Term,class SuccessInfo,class InstanceId>
  class ForwardMatchingWithOriginalCodeTrees
  { 
  public:
    typedef OriginalFMCodeTree<MaxTermSize,MaxNumOfVariables,Alloc,Symbol,Term,SuccessInfo,InstanceId> CodeTree;
  public:
    ForwardMatchingWithOriginalCodeTrees() 
      : 
#ifdef NO_DEBUG 
      tree(0,0),
#else
      tree(0,"ForwardMatchingWithOriginalCodeTrees<..>::tree"),
#endif
      integrator(subst), 
      removal(subst)
    {
      CALL("constructor ForwardMatchingWithOriginalCodeTrees()");
      DOP(tree.freeze());
#ifndef NO_DEBUG_VIS
      _output.setSubst(subst);
#endif     
    };
    ~ForwardMatchingWithOriginalCodeTrees() 
    { 
      CALL("destructor ~ForwardMatchingWithOriginalCodeTrees()"); 
      if (BK::DestructionMode::isThorough()) 
	{
	  for (ulong f = 0; f < tree.size(); f++) 
	    {
	      if (tree[f])
		CodeTree::destroyTree(tree[f]);
	    };	  
	};
    }; 
   


    inline void integrationReset(const Symbol& fun)
    {
      CALL("integrationReset(const Symbol& fun)");
      ASSERT(fun.isComplex());
      DOP(tree.unfreeze());
      integrator.reset(fun,&(tree.sub(fun.functor())));
      DOP(tree.freeze());
    };    
    inline void integrationVar(const Symbol& var) 
    {
      CALL("integrationVar(const Symbol& var)");
      ASSERT(var.isVariable());
      integrator.variable(var.var());
    }; 
    inline void integrationFunc(const Symbol& fun)
    {
      CALL("integrationFunc(const Symbol& fun)");
      ASSERT(fun.isComplex());
      integrator.function(fun);
    };

    CodeTree::Success* integrationGetSucc(bool& newSuccInstr)  
    {
      CALL("integrationGetSucc(bool& newSuccInstr)");
      return integrator.successInstr(newSuccInstr);
    };   
  

    BK::MultisetOfVariables<MaxNumOfVariables>& integrationGetVariables() { return integrator.variables(); };
    const Term** integrationMapVariable(ulong v) { return integrator.mapVariable(v); };

    bool removalReset(const Symbol& fun)
    {
      CALL("removalReset(const Symbol& fun)");
      ASSERT(fun.isComplex());
      if (fun.functor() >= tree.size()) return false;
      CodeTree::Instruction** tree_addr = &(tree[fun.functor()]);  
      if (!(*tree_addr)) return false;
      removal.reset(fun,tree_addr);
      return true;
    };
   
    bool removalVar(const Symbol& var) 
    {
      CALL("removalVar(const Symbol& var)");
      ASSERT(var.isVariable());
      return removal.variable(var.var());
    };
   
    bool removalFunc(const Symbol& fun) 
    {
      CALL("removalFunc(const Symbol& fun)");
      ASSERT(fun.isComplex());
      return removal.function(fun);
    };
 
    bool removalFinish(CodeTree::Success*& leaf)
    {
      CALL("removalGetLeaf(CodeTree::Success*& leaf)");
      if (!removal.finalise()) return false;
      leaf = removal.successInstr();
      return true;
    };
    
    void removalDestroyBranch()
    {
      CALL("removalDestroyBranch()");
      removal.destroyBranch();
    };

    CodeTree::Success* match(const Term* query)
    {
      CALL("match(const Term* query)");
      ASSERT(query->isComplex());
      if (query->functor() >= tree.size()) return 0;
      CodeTree::Instruction* tr = tree[query->functor()];
      if (!tr) return 0;
      return retrieval.match(query,tr);
    };

    CodeTree::Success* matchAgain() 
    { 
      CALL("matchAgain()"); 
      return retrieval.matchAgain(); 
    };
 
#ifndef NO_DEBUG_VIS
    ostream& output(ostream& str) const;
#endif


  private:

    BK::Array<Alloc,CodeTree::Instruction*,128UL,ForwardMatchingWithOriginalCodeTrees> tree;


    const Term* subst[MaxTermSize];
    CodeTree::Integrator integrator;
    CodeTree::Removal removal;
    CodeTree::Retrieval retrieval;

#ifndef NO_DEBUG_VIS
    CodeTree::Output _output;
#endif
  }; // template <ulong MaxTermSize,ulong MaxNumOfVariables,class Alloc,class Symbol,class Term,class SuccessInfo,class InstanceId> class ForwardMatchingWithOriginalCodeTrees


#ifndef NO_DEBUG_VIS
  template <ulong MaxTermSize,ulong MaxNumOfVariables,class Alloc,class Symbol,class Term,class SuccessInfo,class InstanceId>
  ostream& ForwardMatchingWithOriginalCodeTrees<MaxTermSize,MaxNumOfVariables,Alloc,Symbol,Term,SuccessInfo,InstanceId>::output(ostream& str) const
  {
    for (ulong f = 0; f < tree.size(); f++)
      {
	const CodeTree::Instruction* t = tree[f];
	if (t)
	  {
	    Symbol functor(0UL,f,Symbol::PositivePolarity);
	    str << "HEADER " << functor << '\n';
	    _output.tree(str,t) << "\nEND OF TREE FOR " << functor << "\n\n";
	  };  
      };  
    return str;
  }; // ostream& ForwardMatchingWithOriginalCodeTrees<MaxTermSize,MaxNumOfVariables,Alloc,Symbol,Term,SuccessInfo,InstanceId>::output(ostream& str) const
#endif

}; // namespace Gem

//==================================================================
#endif

