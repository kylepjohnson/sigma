#include "or_index.hpp"
#include "Clause.hpp"
#include "unification.hpp"
#include "SimplificationOrdering.hpp"
using namespace BK;
using namespace VK;
//================================================
#undef DEBUG_NAMESPACE
#ifdef DEBUG_OR_INDEX
#define DEBUG_NAMESPACE "OR_INDEX"
#endif 
#include "debugMacros.hpp"
//================================================= 


OR_INDEX::OR_INDEX() : 
#ifdef NO_DEBUG
  _literalIndex(static_cast<OR_INDEX_TREE_NODE*>(0),0),
#else
  _literalIndex(static_cast<OR_INDEX_TREE_NODE*>(0),"OR_INDEX::_literalIndex"),
#endif

  forward_para_from_var_tree(static_cast<OR_INDEX_TREE_NODE*>(0)),
  backward_para_tree(static_cast<OR_INDEX_TREE_NODE*>(0))
{
  CALL("constructor OR_INDEX()");
  DOP(_literalIndex.freeze());
  pseudo_literal[0] = TERM((TERM::Arity)1,(TERM::Functor)0,TERM::PositivePolarity);
}; // OR_INDEX::OR_INDEX()

OR_INDEX::~OR_INDEX() 
{ 
  CALL("destructor ~OR_INDEX()"); 
  if (DestructionMode::isThorough())
    {        
      for (ulong i = 0; i < _literalIndex.size(); i++)
	{
	  OR_INDEX_TREE_NODE::Destroy(_literalIndex[i]); 
	};
      OR_INDEX_TREE_NODE::Destroy(forward_para_from_var_tree); 
      OR_INDEX_TREE_NODE::DestroyBPI(backward_para_tree);
    };
}; // OR_INDEX::~OR_INDEX() 

void OR_INDEX::init()
{
  CALL("init()");
  l_iter.init();
  var_counter.init();
  _vars1.init();
  _vars2.init();

#ifdef NO_DEBUG
  _literalIndex.init(static_cast<OR_INDEX_TREE_NODE*>(0),0);
#else
  _literalIndex.init(static_cast<OR_INDEX_TREE_NODE*>(0),"OR_INDEX::_literalIndex");
#endif

  forward_para_from_var_tree = 0;
  backward_para_tree = 0;
  DOP(_literalIndex.freeze());
  pseudo_literal[0] = TERM((TERM::Arity)1,(TERM::Functor)0,TERM::PositivePolarity);
}; // void OR_INDEX::init()

void OR_INDEX::destroy()
{
  CALL("destroy()");  
  if (DestructionMode::isThorough())
    {        
      for (ulong i = 0; i < _literalIndex.size(); i++)
	{
	  OR_INDEX_TREE_NODE::Destroy(_literalIndex[i]); 
	};
      OR_INDEX_TREE_NODE::Destroy(forward_para_from_var_tree); 
      OR_INDEX_TREE_NODE::DestroyBPI(backward_para_tree);
    };

  _literalIndex.destroy();
  _vars2.destroy();
  _vars1.destroy();
  var_counter.destroy();
  l_iter.destroy();
}; // void OR_INDEX::destroy()


void OR_INDEX::reset()
{
  CALL("reset()");
  
  for (ulong i = 0; i < _literalIndex.size(); i++)
    {
      OR_INDEX_TREE_NODE::Destroy(_literalIndex[i]);  
      _literalIndex[i] = 0;
    };
 

  OR_INDEX_TREE_NODE::Destroy(forward_para_from_var_tree); 
  forward_para_from_var_tree = 0;


  OR_INDEX_TREE_NODE::DestroyBPI(backward_para_tree);
  backward_para_tree = 0;

}; // void OR_INDEX::reset()



void OR_INDEX::Integrate(Clause* cl)
{
  CALL("Integrate(Clause* cl)");
  LiteralList* lits = cl->LitList();
  LiteralList::Element* firstNonselectedLit = lits->nth(lits->numOfSelectedLits());
  ulong litNum = 0;


  for(LiteralList::Iterator iter(lits);iter.currentElement() != firstNonselectedLit;iter.next()) 
    {
      Integrate(iter.currentLiteral(),cl,litNum);
      litNum++;
    };
}; // void OR_INDEX::Integrate(Clause* const cl)

inline void OR_INDEX::Integrate(const TERM* lit,Clause* cl,unsigned lit_num)
{
  CALL("Integrate(const TERM* lit,Clause* cl,unsigned lit_num)");
  if (lit->arity())
    {
      if (paramodulation) integrateForBackwardSuperposition(lit,cl,lit_num);
      if (lit->IsEquality())
	{      
	  if (lit->Positive()) integrateForForwardSuperposition(lit,cl,lit_num);
	}
      else
	{
	  OR_INDEX_TREE_NODE* leaf = IntegrateNonpropLit(lit);
	  ASSERT(leaf->Term().IsReference());
	  ASSERT((!leaf->Term().First()) || (leaf->Term().First() == lit));
	  leaf->SetTerm(lit);
	  leaf->AddPair(lit_num,cl); 
	}; 
    }
  else
    {
      OR_INDEX_TREE_NODE* leaf = IntegratePropLit(*lit);    
      ASSERT(leaf->Term().IsReference());
      ASSERT((!leaf->Term().First()) || (leaf->Term().First() == lit));
      leaf->SetTerm(lit);
      leaf->AddPair(lit_num,cl); 
    };  
}; // void OR_INDEX::Integrate(const TERM* lit,Clause* cl,unsigned lit_num)

inline void OR_INDEX::integrateBackSupRedex(const TERM* redex,Clause* cl,unsigned lit_num,
					    SimplificationOrdering::StoredConstraint* constr)
{
  CALL("integrateBackSupRedex(const TERM* redex,Clause* cl,unsigned lit_num,SimplificationOrdering::StoredConstraint* constr)");
  ASSERT(redex->isComplex());

  INTERCEPT_BACKWARD_SUPERPOSITION_INDEXING_INTEGRATE(redex);

  OR_INDEX_TREE_NODE** tree = &backward_para_tree;
  OR_INDEX_TREE_NODE* leaf;
  pseudo_literal->Args()->SetFirst(const_cast<TERM*>(redex));
  l_iter.Reset(pseudo_literal,1UL);
  l_iter.Next();
  leaf = IntegrateArgs(tree);
  leaf->SetTerm(redex);
  leaf->addBackSupTriple(lit_num,cl,redex,constr);  
}; // void OR_INDEX::integrateBackSupRedex(const TERM* redex,Clause* cl,unsigned lit_num,SimplificationOrdering::StoredConstraint* constr)

inline void OR_INDEX::integrateForBackwardSuperposition(const TERM* lit,Clause* cl,unsigned lit_num)
{
  CALL("integrateForBackwardSuperposition(const TERM* lit,Clause* cl,unsigned lit_num)");
  ASSERT(lit->arity());
  TERM::Iterator iter;
  if (lit->IsEquality())
    {
      if (lit->IsOrderedEq())
	{
	  // only the first argument can be rewritten
	  ASSERT(lit->Arg1()->IsReference());
	  const TERM* arg = lit->Arg1()->First();
	  iter.Reset(arg);
	  do
	    {
	      if (!iter.CurrentSymIsVar()) 
		integrateBackSupRedex(iter.CurrPos(),cl,lit_num,0);
				      
	    }
	  while (iter.Next());
	}
      else // unordered equality
	{

	  const TERM* arg1ref = lit->Arg1();
	  const TERM* arg2ref = arg1ref->Next();
	  if (arg1ref->isVariable() || arg2ref->isVariable()) goto integrate_ordinary;

	  SimplificationOrdering::StoredConstraint* constraint;
	  SimplificationOrdering::SpecialisedComparison* specialisedArgComparison = 
	    SimplificationOrdering::current()->reserveSpecialisedComparison();
	  ASSERT(specialisedArgComparison);
	  specialisedArgComparison->load(*arg1ref,*arg2ref);
	  iter.Reset(arg1ref->First());

	  if ((specialisedArgComparison->lhsVarInstMayOrder()) &&
	      ((constraint = SimplificationOrdering::current()->storedConstraintLessOrEqual(specialisedArgComparison))))
	    {
	      do
		{
		  if (!iter.CurrentSymIsVar()) 
		    integrateBackSupRedex(iter.CurrPos(),cl,lit_num,constraint);
		}
	      while (iter.Next());
	      SimplificationOrdering::current()->releaseConstraint(constraint);
	    }
	  else
	    {
	      do
		{
		  if (!iter.CurrentSymIsVar()) 
		    integrateBackSupRedex(iter.CurrPos(),cl,lit_num,0);
		}
	      while (iter.Next());
	    };
      
	  iter.Reset(arg2ref->First());
	  if ((specialisedArgComparison->rhsVarInstMayOrder()) &&
	      ((constraint = SimplificationOrdering::current()->storedConstraintGreaterOrEqual(specialisedArgComparison))))
	    {
	      do
		{
		  if (!iter.CurrentSymIsVar()) 
		    integrateBackSupRedex(iter.CurrPos(),cl,lit_num,
					  constraint);
		}
	      while (iter.Next());
	      SimplificationOrdering::current()->releaseConstraint(constraint);
	    }
	  else
	    {
	      do
		{
		  if (!iter.CurrentSymIsVar()) 
		    integrateBackSupRedex(iter.CurrPos(),cl,lit_num,0);
		}
	      while (iter.Next());
	    };
	  SimplificationOrdering::current()->releaseSpecialisedComparison(specialisedArgComparison);
	};
    }
  else // ordinary literal
    {
    integrate_ordinary:
      iter.Reset(lit);
      while (iter.Next()) 
	if (!iter.CurrentSymIsVar()) 
	  {
	    integrateBackSupRedex(iter.CurrPos(),cl,lit_num,0); 
	  };    
    };
}; // void OR_INDEX::integrateForBackwardSuperposition(const TERM* lit,Clause* cl,unsigned lit_num)

inline void OR_INDEX::integrateForForwardSuperposition(const TERM* lit,Clause* cl,unsigned lit_num)
{
  CALL("integrateForForwardSuperposition(const TERM* lit,Clause* cl,unsigned lit_num)");
  ASSERT(lit->IsEquality());
  ASSERT(lit->Positive());

  if (lit->IsOrderedEq()) { integratePreorderedForForwardSuperposition(lit,cl,lit_num); return; };

  const TERM* arg1ref = lit->Args();
  const TERM* arg2ref = arg1ref->Next();
  if (arg1ref->IsReference() && arg2ref->IsReference())
    {

      SimplificationOrdering::StoredConstraint* constraint;
      SimplificationOrdering::SpecialisedComparison* specialisedArgComparison = 
	SimplificationOrdering::current()->reserveSpecialisedComparison();
      ASSERT(specialisedArgComparison);
      specialisedArgComparison->load(*arg1ref,*arg2ref);

      if (specialisedArgComparison->lhsVarInstMayOrder())
	{
	  constraint = SimplificationOrdering::current()->storedConstraintLessOrEqual(specialisedArgComparison);

	  integrateConstrainedForForwardSuperposition(*arg1ref,*arg2ref,cl,lit_num,constraint); 
	  if (constraint) SimplificationOrdering::current()->releaseConstraint(constraint);
	};
      
      if (specialisedArgComparison->rhsVarInstMayOrder())
	{
	  constraint = SimplificationOrdering::current()->storedConstraintGreaterOrEqual(specialisedArgComparison);   
	  integrateConstrainedForForwardSuperposition(*arg2ref,*arg1ref,cl,lit_num,constraint);
	  if (constraint) SimplificationOrdering::current()->releaseConstraint(constraint);
	};
      
      SimplificationOrdering::current()->releaseSpecialisedComparison(specialisedArgComparison);
      return;
    };

  OR_INDEX_TREE_NODE* leaf;
  // integrate the first argument
  const TERM* arg = lit->Args();
  TERM replacing_term = *(arg->Next());
  bool stop = false;
 integrate_arg: 
  if (arg->isVariable())
    {
      pseudo_literal[1] = *arg;
      l_iter.Reset(pseudo_literal,1UL);
      l_iter.Next();
      leaf = IntegrateArgs(&forward_para_from_var_tree);
      leaf->SetTerm(*arg);
      leaf->addForwardSupTriple(lit_num,cl,replacing_term,0); 
    }
  else // complex term, integrate as ordinary literal
    {
      if (arg->First()->arity())
	{
	  l_iter.Reset(arg->First(),1UL);
	  l_iter.Next();
	  leaf = IntegrateArgs(FParaTree(*(arg->First())));
	}
      else
	{
	  leaf = *(FParaTree(*(arg->First())));
	  if (!leaf) 
	    {
	      leaf = OR_INDEX_TREE_NODE::NewLeaf();
	      *(FParaTree(*(arg->First()))) = leaf;
	    };
	}; 
      leaf->SetTerm(arg->First());
      leaf->addForwardSupTriple(lit_num,cl,replacing_term,0);
    };    
   
  if (lit->IsOrderedEq() || stop) return; 
  // integrate the second argument 
  replacing_term = *arg;
  arg = arg->Next();
  stop = true;
  goto integrate_arg;
}; // void OR_INDEX::integrateForForwardSuperposition(const TERM* lit,Clause* cl,unsigned lit_num)



inline
void OR_INDEX::integrateConstrainedForForwardSuperposition(const TERM& lhs,const TERM& rhs,Clause* cl,
						  unsigned lit_num,SimplificationOrdering::StoredConstraint* constr)
{
  CALL("integrateConstrainedForForwardSuperposition(..)");
  ASSERT(lhs.IsReference());
  ASSERT(rhs.IsReference());  

  OR_INDEX_TREE_NODE* leaf;
  if (lhs.First()->arity())
    {
      l_iter.Reset(lhs.First(),1UL);
      l_iter.Next();
      leaf = IntegrateArgs(FParaTree(*(lhs.First())));
    }
  else
    {
      leaf = *(FParaTree(*(lhs.First())));
      if (!leaf) 
	{
	  leaf = OR_INDEX_TREE_NODE::NewLeaf();
	  *(FParaTree(*(lhs.First()))) = leaf;
	};
    }; 
  leaf->SetTerm(lhs.First());
  leaf->addForwardSupTriple(lit_num,cl,rhs,constr); 

}; // void integrateConstrainedForForwardSuperposition(const TERM& lhs,const TERM& rhs,Clause* cl,unsigned lit_num,SimplificationOrdering::StoredConstraint* constr)


inline void OR_INDEX::integratePreorderedForForwardSuperposition(const TERM* lit,Clause* cl,unsigned lit_num)
{
  CALL("integratePreorderedForForwardSuperposition(const TERM* lit,Clause* cl,unsigned lit_num)");
  ASSERT(lit->IsOrderedEq());
  OR_INDEX_TREE_NODE* leaf;
  // integrate only the first argument
  const TERM* arg = lit->Args();
  ASSERT(arg->IsReference());
  TERM replacing_term = *(arg->Next());   
  if (arg->First()->arity())
    {
      l_iter.Reset(arg->First(),1UL);
      l_iter.Next();
      leaf = IntegrateArgs(FParaTree(*(arg->First())));
    }
  else
    {
      leaf = *(FParaTree(*(arg->First())));
      if (!leaf) 
	{
	  leaf = OR_INDEX_TREE_NODE::NewLeaf();
	  *(FParaTree(*(arg->First()))) = leaf;
	};
    }; 
  leaf->SetTerm(arg->First());
  leaf->addForwardSupTriple(lit_num,cl,replacing_term,0);  
}; // void OR_INDEX::integratePreorderedForForwardSuperposition(const TERM* lit,Clause* cl,unsigned lit_num)





inline OR_INDEX_TREE_NODE* OR_INDEX::IntegratePropLit(TERM lit)
{
  CALL("IntegratePropLit(TERM lit)");
  OR_INDEX_TREE_NODE** tree = Tree(lit);
  if (!(*tree))
    {
      // no tree for this header yet, create it here
      *tree = OR_INDEX_TREE_NODE::NewLeaf();
    };
  return *tree;
}; // OR_INDEX_TREE_NODE* OR_INDEX::IntegratePropLit(TERM lit)

inline OR_INDEX_TREE_NODE* OR_INDEX::IntegrateNonpropLit(const TERM* lit)
{
  CALL("IntegrateNonpropLit(const TERM* lit)");
  OR_INDEX_TREE_NODE** tree = Tree(*lit);
  l_iter.Reset(lit,1UL);
  l_iter.Next();
  return IntegrateArgs(tree);
}; // OR_INDEX_TREE_NODE* OR_INDEX::IntegrateNonpropLit(const TERM* lit)
 
inline OR_INDEX_TREE_NODE* OR_INDEX::IntegrateArgs(OR_INDEX_TREE_NODE** tree)
{
  CALL("IntegrateArgs(OR_INDEX_TREE_NODE** tree)");
  var_counter.Reset();
  if (*tree)
    {
      OR_INDEX_TREE_NODE* node = *tree;
      PrefSym sym;
    take_next_sym:
      if (l_iter.CurrentSymIsVar())
	{
	  ulong var_num = l_iter.Symbol().var();
	  sym.MkVar(Unifier::current()->variableBase(var_num));
	  var_counter.Register(var_num);
	}
      else
	{
	  sym.MkFunc(l_iter.Symbol());
	};
    check_node:
      if (node->Symbol() == sym)
	{
	  node = node->Below();
	  if (l_iter.Next())
	    {
	      goto take_next_sym;
	    }
	  else // node must be a leaf
	    {
	      ASSERT(node->IsLeaf());
	      return node;
	    };
	}
      else 
	{
	  if (node->Alternative())
	    {
	      node = node->Alternative();
	      goto check_node;
	    }
	  else // no alternative, new branch must be constructed
	    {
	      OR_INDEX_TREE_NODE* leaf;
	      node->SetAlternative(CreateBranch(leaf));
	      return leaf;
	    };
	};      
    }
  else // empty tree, create branch
    {
      OR_INDEX_TREE_NODE* leaf;
      *tree = CreateBranch(leaf);
      return leaf;
    };
}; // OR_INDEX_TREE_NODE* OR_INDEX::IntegrateArgs(OR_INDEX_TREE_NODE** tree)



void OR_INDEX::Remove(Clause* cl)
{   
  LiteralList* allLiterals = cl->LitList();
  LiteralList::Element* firstNonselectedLit = allLiterals->nth(allLiterals->numOfSelectedLits());
  for(LiteralList::Iterator iter(allLiterals);iter.currentElement() != firstNonselectedLit;iter.next())
    Remove(iter.currentLiteral(),cl); 
}; // void OR_INDEX::Remove(Clause* c)

void OR_INDEX::RemovePropLit(const TERM* lit,Clause* cl)
{
  CALL("RemovePropLit(const TERM* lit,Clause* cl)");
  ASSERT(!lit->arity());
  OR_INDEX_TREE_NODE** tree = Tree(*lit);
  ASSERT(*tree);
  ASSERT((*tree)->IsLeaf());
  (*tree)->Remove(cl);
  if (!(*tree)->Clauses()) 
    {
      (*tree)->DeleteLeaf();
      *tree = 0;
    };
}; // void OR_INDEX::RemovePropLit(const TERM* lit,Clause* cl)
 
OR_INDEX_TREE_NODE* OR_INDEX::LeafAndBranch(const TERM* t,OR_INDEX_TREE_NODE** tree,OR_INDEX_TREE_NODE**& branch)
{
  CALL("LeafAndBranch(const TERM* t,OR_INDEX_TREE_NODE** tree,OR_INDEX_TREE_NODE**& branch)");
  ASSERT(tree);
  ASSERT(t->arity());
  branch = tree;
  OR_INDEX_TREE_NODE* node = *tree;
  
  if (!node) return 0; 
  
  OR_INDEX_TREE_NODE** node_addr = tree; 
  
  TERM::Iterator iter;  

  iter.Reset(t);

 next_sym:
  if (!iter.Next())
    {
      ASSERT(node->IsLeaf());
      return node;
    };
 check_node:
  if (node->SameSymbol(iter.Symbol()))
    {
      if (node->Alternative()) branch = node_addr;
      node_addr = node->BelowAddr();
      node = node->Below();
      ASSERT(node);
      goto next_sym;     
    }; 
  // try alternative
  if (node->Alternative())
    {
      node_addr = node->AlternativeAddr();
      branch = node_addr;
      node = node->Alternative();
      goto check_node;
    };
  // no appropriate alternative, search fails
  return 0;
}; // OR_INDEX_TREE_NODE* OR_INDEX::LeafAndBranch(const TERM* t,OR_INDEX_TREE_NODE** tree,OR_INDEX_TREE_NODE**& branch)



void OR_INDEX::Remove(const TERM* t,Clause* cl,OR_INDEX_TREE_NODE** tree)
{  
  CALL("Remove(const TERM* t,Clause* cl,OR_INDEX_TREE_NODE** tree)");
  
  ASSERT(t->arity());
  OR_INDEX_TREE_NODE** branch;
  OR_INDEX_TREE_NODE* branch_to_delete;
  OR_INDEX_TREE_NODE* node_to_delete;
  
  OR_INDEX_TREE_NODE* leaf = LeafAndBranch(t,tree,branch);

  if (!leaf) return;

  leaf->Remove(cl);

  if (!(leaf->Clauses())) 
    {
      // no more clauses in the leaf, the whole branch must be deleted
      // *branch can't be a leaf here
      branch_to_delete = *branch;
      *branch = (*branch)->Alternative(); 
      while (!(branch_to_delete->IsLeaf()))
	{
	  node_to_delete = branch_to_delete;
	  branch_to_delete = branch_to_delete->Below();
	  node_to_delete->DeleteNode();
	};
      branch_to_delete->DeleteLeaf(); // leaf
    };  
}; // void OR_INDEX::Remove(const TERM* t,Clause* cl,OR_INDEX_TREE_NODE** tree)

void OR_INDEX::RemoveBPCandidates(const TERM* t,Clause* cl,OR_INDEX_TREE_NODE** tree)
{  
  CALL("RemoveBPCandidates(const TERM* t,Clause* cl,OR_INDEX_TREE_NODE** tree)");
  
  ASSERT(t->arity());
  OR_INDEX_TREE_NODE** branch;
  OR_INDEX_TREE_NODE* branch_to_delete;
  OR_INDEX_TREE_NODE* node_to_delete;
  
  INTERCEPT_BACKWARD_SUPERPOSITION_INDEXING_PREPARE_FOR_REMOVAL(t->Args()->First());


  OR_INDEX_TREE_NODE* leaf = LeafAndBranch(t,tree,branch);

  if (!leaf) 
    {
      INTERCEPT_BACKWARD_SUPERPOSITION_INDEXING_REMOVAL_MODE('F');
      return;
    };

  leaf->RemoveBPCandidates(cl);

  if (!(leaf->BPCandidates())) 
    {
      // no more clauses in the leaf, the whole branch must be deleted
      // *branch can't be a leaf here
      INTERCEPT_BACKWARD_SUPERPOSITION_INDEXING_REMOVAL_MODE('R');
      branch_to_delete = *branch;
      *branch = (*branch)->Alternative(); 
      while (!(branch_to_delete->IsLeaf()))
	{
	  node_to_delete = branch_to_delete;
	  branch_to_delete = branch_to_delete->Below();
	  node_to_delete->DeleteNode();
	};
      branch_to_delete->DeleteLeaf(); // leaf
    }
  else
    {
      INTERCEPT_BACKWARD_SUPERPOSITION_INDEXING_REMOVAL_MODE('C');
    };
}; // void OR_INDEX::RemoveBPCandidates(const TERM* t,Clause* cl,OR_INDEX_TREE_NODE** tree)

void OR_INDEX::RemoveFromBPI(const TERM* lit,Clause* cl)
{
  TERM::Iterator iter;
  if (lit->IsEquality() && lit->IsOrderedEq())
    {
      const TERM* arg = lit->Args()->First();
      if (!arg->isVariable())
	{
	  iter.Reset(arg);
	  do
	    {
	      if (!iter.CurrentSymIsVar()) RemoveBPRedex(iter.CurrPos(),cl);
	    }
	  while (iter.Next());
	};
    }
  else // ordinary literal or unordered equality
    {
      iter.Reset(lit);
      while (iter.Next())
	if (!iter.CurrentSymIsVar()) RemoveBPRedex(iter.CurrPos(),cl);
    };
}; // void OR_INDEX::RemoveFromBPI(const TERM* lit,Clause* cl)

void OR_INDEX::RemoveBPRedex(const TERM* t,Clause* cl)
{
  CALL("RemoveBPRedex(const TERM* t,Clause* cl)");
  pseudo_literal->Args()->SetFirst(const_cast<TERM*>(t));
  RemoveBPCandidates(pseudo_literal,cl,&backward_para_tree);
}; // void OR_INDEX::RemoveBPRedex(const TERM* t,Clause* cl)

void OR_INDEX::RemoveFromFPI(const TERM* lit,Clause* cl)
{
  bool stop = false;
  const TERM* arg = lit->Args();
 remove_arg:
  if (arg->isVariable())
    {
      pseudo_literal[1] = *arg;
      Remove(pseudo_literal,cl,&forward_para_from_var_tree);
    }
  else // complex  
    if (arg->First()->arity())
      {
	Remove(arg->First(),cl,FParaTree(*(arg->First()))); 
      }
    else // constant 
      {
	OR_INDEX_TREE_NODE** tree = FParaTree(*(arg->First()));
       
	if (*tree)
	  {
	    (*tree)->Remove(cl);
	    if (!(*tree)->Clauses()) 
	      {
		(*tree)->DeleteLeaf();
		*tree = 0;
	      };
	  };
      };
  if (lit->IsOrderedEq() || stop) 
    { 
      return;
    }; 
  stop = true;
  arg = arg->Next();
  goto remove_arg;          
}; // void OR_INDEX::RemoveFromFPI(const TERM* lit,Clause* cl)


void OR_INDEX::Remove(const TERM* lit,Clause* cl)
{
  if (!lit->arity()) { RemovePropLit(lit,cl); return; };
  if (paramodulation) RemoveFromBPI(lit,cl);
  if (lit->IsEquality())
    {     
      if (lit->Positive()) 
	{
	  RemoveFromFPI(lit,cl);
	};
    }
  else 
    {
      Remove(lit,cl,Tree(*lit));
    };
}; // void OR_INDEX::Remove(const TERM* lit,Clause* cl)




inline OR_INDEX_TREE_NODE* OR_INDEX::CreateBranch(OR_INDEX_TREE_NODE*& leaf)
{
  PrefSym sym;
  OR_INDEX_TREE_NODE::TAG tag;
  if (l_iter.CurrentSymIsVar())
    {
      if (var_counter.Occurences(l_iter.Symbol().var()))
	{
	  tag = OR_INDEX_TREE_NODE::VAR;
	}
      else // first occurence of the variable
	{
	  var_counter.Register(l_iter.Symbol().var());
	  tag = OR_INDEX_TREE_NODE::VAR_FIRST_OCC;
	};
      sym.MkVar(Unifier::current()->variableBase(l_iter.Symbol().var()));
    }
  else
    {
      if (l_iter.Symbol().arity())
	{
	  tag = OR_INDEX_TREE_NODE::FUNC;
	}
      else
	{
	  tag = OR_INDEX_TREE_NODE::CONST;
	};
      sym.MkFunc(l_iter.Symbol());
    };
  OR_INDEX_TREE_NODE* result = OR_INDEX_TREE_NODE::NewNode(tag,sym,l_iter.Depth());
  OR_INDEX_TREE_NODE* node = result;
  while (l_iter.Next())
    {
      if (l_iter.CurrentSymIsVar())
	{
	  if (var_counter.Occurences(l_iter.Symbol().var()))
	    {
	      tag = OR_INDEX_TREE_NODE::VAR;
	    }
	  else // first occurence of the variable
	    {
	      var_counter.Register(l_iter.Symbol().var());
	      tag = OR_INDEX_TREE_NODE::VAR_FIRST_OCC;
	    };
	  sym.MkVar(Unifier::current()->variableBase(l_iter.Symbol().var()));
	}
      else
	{
	  if (l_iter.Symbol().arity())
	    {
	      tag = OR_INDEX_TREE_NODE::FUNC;
	    }
	  else
	    {
	      tag = OR_INDEX_TREE_NODE::CONST;
	    };
	  sym.MkFunc(l_iter.Symbol());
	};
      node->SetBelow(OR_INDEX_TREE_NODE::NewNode(tag,sym,l_iter.Depth()));
      node = node->Below();
    };
  
  leaf = OR_INDEX_TREE_NODE::NewLeaf();
  node->SetBelow(leaf);
  return result;
}; // OR_INDEX_TREE_NODE* OR_INDEX::CreateBranch(OR_INDEX_TREE_NODE*& leaf)



//================================================
#undef DEBUG_NAMESPACE
#ifdef DEBUG_OR_INDEX
#define DEBUG_NAMESPACE "OR_INDEX_TREE_NODE"
#endif 
#include "debugMacros.hpp"
//================================================= 

void OR_INDEX_TREE_NODE::Destroy(OR_INDEX_TREE_NODE* tree)
{
  CALL("Destroy(OR_INDEX_TREE_NODE* tree)");
  if (!tree) return;
  if (tree->IsLeaf())
    {
      tree->DeleteLeaf();
    }
  else
    {
      OR_INDEX_TREE_NODE* tree_below = tree->Below();
      OR_INDEX_TREE_NODE* tree_alt = tree->Alternative(); 
      tree->DeleteNode();
      Destroy(tree_below);
      if (tree_alt)
	{
	  Destroy(tree_alt);
	};   
    };
}; // void OR_INDEX_TREE_NODE::Destroy(OR_INDEX_TREE_NODE* tree)

void OR_INDEX_TREE_NODE::DestroyBPI(OR_INDEX_TREE_NODE* tree)
{
  CALL("DestroyBPI(OR_INDEX_TREE_NODE* tree)");
  if (!tree) return;
  if (tree->IsLeaf())
    {
      tree->DeleteBPILeaf();
    }
  else
    {
      OR_INDEX_TREE_NODE* tree_below = tree->Below();
      OR_INDEX_TREE_NODE* tree_alt = tree->Alternative(); 
      tree->DeleteNode();
      DestroyBPI(tree_below);
      if (tree_alt)
	{
	  DestroyBPI(tree_alt);
	};   
    };
}; // void OR_INDEX_TREE_NODE::DestroyBPI(OR_INDEX_TREE_NODE* tree)

#ifndef NO_DEBUG_VIS

 
ostream& OR_INDEX_TREE_NODE::output(ostream& str,unsigned var_offset) const
{
  str << '[' << (ulong)this << "] " << Tag() << ' ';
  switch (Tag())
    {
    case LEAF: str << ' ' << Term(); break;
    case FUNC: 
    case CONST:
    case VAR: 
    case VAR_FIRST_OCC:    
      Symbol().output(str,var_offset) << " [" << (ulong)Below() << "] ["
                                      << (ulong)Alternative() << "] "
                                      << Depth();
      break;
    };
  return str;
}; // ostream& OR_INDEX_TREE_NODE::output(ostream& str,unsigned var_offset) const
#endif


namespace std
{
ostream& operator<<(ostream& str,const OR_INDEX_TREE_NODE::TAG& tag)
{
  switch (tag)
    {
    case OR_INDEX_TREE_NODE::LEAF: return str << "LEAF";
    case OR_INDEX_TREE_NODE::FUNC: return str << "FUNC";
    case OR_INDEX_TREE_NODE::CONST: return str << "CONST"; 
    case OR_INDEX_TREE_NODE::VAR: return str << "VAR";
    case OR_INDEX_TREE_NODE::VAR_FIRST_OCC: return str << "VAR_FIRST_OCC"; 
    default: return str << "UNKNOWN TAG"; 
    };
}; // ostream& operator<<(ostream& str,const OR_INDEX_TREE_NODE::TAG& tag)
}; 

//================================================
#undef DEBUG_NAMESPACE
#ifdef DEBUG_OR_INDEX
#define DEBUG_NAMESPACE "OR_PROCESSOR"
#endif 
#include "debugMacros.hpp"
//================================================= 


inline bool OR_PROCESSOR::CompleteExtraction()
{
  CALL("CompleteExtraction()");
 check_iter:
  
  if (curr_iter->Empty()) 
    {
      return true; 
    };
   
 check_sym:
   
   
  Unifier::Variable* var = curr_iter->CurrVar();
  if (var) // free variable
    {
     
      ASSERT(!(var->Binding()));
      bool same_var;
      
      if (BindWithOccCheck(var,same_var))
	{
       
	  ASSERT(!(same_var && var->Binding()));
	  if (same_var) { *curr_sit = VAR_SAME_VAR; }
	  else { *curr_sit = VAR_ANYTHING; };
	  curr_sit++;
	  curr_iter->SkipSym();
	  goto check_iter;
	}
      else
	{
	  if (BacktrackExtraction()) { goto check_sym; }
	  else { return false; };
	};
    }
  else // functor
    {
     
    check_node:
      switch (OR_INDEX_PATH::CurrNode()->Tag())
        {
	case OR_INDEX_TREE_NODE::FUNC : 
          // currently the same as for OR_INDEX_TREE_NODE::CONST,
          // but can be optimized by jumping to check_sym instead of check_iter
          
	case OR_INDEX_TREE_NODE::CONST : 
         
          if (OR_INDEX_PATH::CurrNode()->Symbol() == curr_iter->CurrSubterm()->Head())
	    { // same functor
            
	      curr_iter->SkipSym();
	      OR_INDEX_PATH::Below();
	      *curr_sit = FUNCTOR_FUNCTOR;
	      curr_sit++;
	      goto check_iter;
	    }
          else // different functors, try alternative
	    {
	      OR_INDEX_PATH::Alternative();
	      if (OR_INDEX_PATH::CurrNode())
		{
		  goto check_node;
		}
	      else // backtrack needed
		{
		  if (BacktrackExtraction()) { goto check_sym; }
		  else { return false; };
		};
	    };
         
	case OR_INDEX_TREE_NODE::VAR :
          {
	    bool free_var;
	    void* bind = OR_INDEX_PATH::Shift(OR_INDEX_PATH::CurrNode()->Symbol().var())->Unref(free_var);
	    
	    Prefix::CorrectBracket(curr_iter->CurrSubterm());
           
	    ASSERT(Prefix::CorrectComplexTerm(curr_iter->CurrSubterm()));
           
	    if (free_var)
	      {
		if (OccurCheck::Occurs(static_cast<Unifier::Variable*>(bind),curr_iter->CurrSubterm()))
		  {
		    // try alternative
		    OR_INDEX_PATH::Alternative();
		    if (OR_INDEX_PATH::CurrNode())
		      {
			goto check_node;
		      }
		    else // backtrack needed
		      {
			if (BacktrackExtraction()) { goto check_sym; }
			else { return false; };
		      };
		  }
		else // the variable doesn't occur in the subterm, make binding
		  {
		    Unifier::current()->BindAlone(static_cast<Unifier::Variable*>(bind),curr_iter->CurrSubterm());
		    curr_iter->SkipTerm();
		    OR_INDEX_PATH::Below();
		    *curr_sit = FUNCTOR_FREE_VAR;
		    curr_sit++;
		    goto check_iter;
		  };
	      }
	    else // variable bound by a complex term, try to unify
	      {
		if (UNIFICATION::UnifyComplex(static_cast<PrefixSym*>(bind),curr_iter->CurrSubterm()))
		  {
		    curr_iter->SkipTerm();
		    OR_INDEX_PATH::Below();
		    *curr_sit = FUNCTOR_BOUND_VAR;
		    curr_sit++;
		    goto check_iter;
		  }
		else // try alternative
		  {
		    OR_INDEX_PATH::Alternative();
		    if (OR_INDEX_PATH::CurrNode())
		      {
			goto check_node;
		      }
		    else // backtrack needed
		      {
			if (BacktrackExtraction()) { goto check_sym; }
			else { return false; };
		      };
		  };
	      };
          };
	case OR_INDEX_TREE_NODE::VAR_FIRST_OCC :
          Prefix::CorrectBracket(curr_iter->CurrSubterm());
          
          ASSERT(Prefix::CorrectComplexTerm(curr_iter->CurrSubterm()));
          
          Unifier::current()->BindAlone(OR_INDEX_PATH::Shift(OR_INDEX_PATH::CurrNode()->Symbol().var()),
				curr_iter->CurrSubterm());
          
          curr_iter->SkipTerm();
          OR_INDEX_PATH::Below();
          *curr_sit = FUNCTOR_FREE_VAR;
          curr_sit++;
          goto check_iter;         
         
#ifdef DEBUG_NAMESPACE
	default : ICP("A2"); return false;
#elif defined _SUPPRESS_WARNINGS_
	default : return false;
#endif 
        }; // switch
      ICP("A3");
      return false;
    };  
}; // bool OR_PROCESSOR::CompleteExtraction()

inline bool OR_PROCESSOR::BacktrackExtraction()
{
  CALL("BacktrackExtraction()");
 rollback:
  ASSERT((!(OR_INDEX_PATH::CurrNode())) || OR_INDEX_PATH::CurrNode()->IsLeaf());
  
  curr_sit--;
   
  switch (*curr_sit)
    {
    case START_OF_EXTRACTION : 
      ForgetIter(); 
      return false;
    case VAR_SAME_VAR :
      {
	bool same_var;
	curr_iter->Backtrack();

	ASSERT(curr_iter->CurrVar());
	ASSERT(!(curr_iter->CurrVar()->Binding()));
       
	if (AlterBindingWithOccCheck(curr_iter->CurrVar(),same_var)) 
	  {
	    if (!(same_var)) { *curr_sit = VAR_ANYTHING; };
	    curr_sit++;
	    curr_iter->SkipSym();
	    return true; 
	  }
	else 
	  {
	    goto rollback; 
	  };
      };
    case VAR_ANYTHING : 
      {
	bool same_var;
	curr_iter->Backtrack();
       
	if (AlterBindingWithOccCheck(curr_iter->CurrVar(),same_var)) 
	  {
	    if (same_var) 
	      {
		*curr_sit = VAR_SAME_VAR; 
	      };
	    curr_sit++; 
	    curr_iter->SkipSym();
	    return true; 
	  }
	else 
	  {
	    goto rollback; 
	  };
      };

    case FUNCTOR_FUNCTOR : 
      curr_iter->Backtrack();
      
      
      ASSERT(curr_iter->CurrSubterm()->Head().IsFunctor());

      OR_INDEX_PATH::RestoreCurrent();
      OR_INDEX_PATH::Alternative();
      if (OR_INDEX_PATH::CurrNode()) { return true; }
      else 
	{
	  goto rollback; 
	};

    case FUNCTOR_FREE_VAR :
      Unifier::current()->UnbindLast();
      curr_iter->Backtrack();

      ASSERT(curr_iter->CurrSubterm()->Head().IsFunctor());
      OR_INDEX_PATH::RestoreCurrent();
      OR_INDEX_PATH::Alternative();
      if (OR_INDEX_PATH::CurrNode()) { return true; }
      else 
	{
	  goto rollback; 
	};

    case FUNCTOR_BOUND_VAR :
      
      Unifier::current()->UnbindLastSection();
      curr_iter->Backtrack();
      ASSERT(curr_iter->CurrSubterm()->Head().IsFunctor());
      OR_INDEX_PATH::RestoreCurrent();
      OR_INDEX_PATH::Alternative();
      if (OR_INDEX_PATH::CurrNode()) 
	{
	  return true; 
	}
      else 
	{
	  goto rollback; 
	};
#ifdef DEBUG_NAMESPACE
    default : ICP("ICP1"); return false;
#endif
    };  
  ICP("ICP2"); 
  return false; 
}; // bool OR_PROCESSOR::BacktrackExtraction()

bool OR_PROCESSOR::ExtractUnifiable(PrefixSym* word) 
  // word must represent a complex term
{
  NextIter();
  curr_iter->Reset(word);
  *curr_sit = START_OF_EXTRACTION;
  curr_sit++;
  return CompleteExtraction();
}; // bool OR_PROCESSOR::ExtractUnifiable(PrefixSym* word)
 
bool OR_PROCESSOR::AlterUnifiable()
{
  if (BacktrackExtraction()) 
    {
      return CompleteExtraction(); 
    }
  else 
    {
      return false; 
    };
}; // bool OR_PROCESSOR::AlterUnifiable()


#ifndef NO_DEBUG_VIS
namespace std
{
ostream& operator<<(ostream& str,const OR_PROCESSOR::SITUATION& sit) 
{
  switch (sit)
    {
    case OR_PROCESSOR::START_OF_EXTRACTION : str << "START_OF_EXTRACTION"; break;
    case OR_PROCESSOR::VAR_SAME_VAR : str << "VAR_SAME_VAR"; break;
    case OR_PROCESSOR::VAR_ANYTHING : str << "VAR_ANYTHING"; break;
    case OR_PROCESSOR::FUNCTOR_FUNCTOR : str << "FUNCTOR_FUNCTOR"; break;
    case OR_PROCESSOR::FUNCTOR_FREE_VAR : str << "FUNCTOR_FREE_VAR"; break;
    case OR_PROCESSOR::FUNCTOR_BOUND_VAR : str << "FUNCTOR_BOUND_VAR"; break;
    };
  return str;
}; //ostream& operator<<(ostream& str,const OR_PROCESSOR::SITUATION& sit) 
};
#endif


//================================================
#undef DEBUG_NAMESPACE
#ifdef DEBUG_OR_INDEX
#define DEBUG_NAMESPACE "OR_INDEX_PATH"
#endif 
#include "debugMacros.hpp"
//================================================= 

#ifndef NO_DEBUG_VIS
ostream& OR_INDEX_PATH::output(ostream& str) const
{
  for (const OR_INDEX_TREE_NODE* const * p = path; p<next; p++)    
    (*p)->output(str,var_offset) << '\n'; 
  str << "CURRENT NODE = ";
  if (curr_node) { curr_node->output(str,var_offset); }
  else str << "[0]";  
  return str << '\n'; 
}; // ostream& OR_INDEX_PATH::output(ostream& str) const
#endif




//=================================================
