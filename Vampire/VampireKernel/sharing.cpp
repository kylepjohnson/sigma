 
//
// File:         sharing.cpp 
// Description:  Agressive sharing.
// Created:      Apr 17, 2001, 17:30
// Author:       Alexandre Riazanov
// mail:         riazanov@cs.man.ac.uk
//====================================================
#include "sharing.hpp" 
#include "Flatterm.hpp"
using namespace BK;
using namespace VK;
//================================================
#undef DEBUG_NAMESPACE
#ifdef DEBUG_SHARING
#define DEBUG_NAMESPACE "SHARING_INDEX"
#endif
#include "debugMacros.hpp"
//================================================= 



#ifdef DEBUG_ALLOC_OBJ_TYPE
ClassDesc SHARING_TREE_NODE::_classDesc("SHARING_TREE_NODE",SHARING_TREE_NODE::minSize(),SHARING_TREE_NODE::maxSize());
#endif

SHARING_INDEX::SHARING_INDEX() 
  : t_iter(), 
  subterms("subterms"),
#ifdef NO_DEBUG
  _discTree(static_cast<SHARING_TREE_NODE*>(0),0),
  _sharedConst(static_cast<TERM*>(0),0)
#else
  _discTree(static_cast<SHARING_TREE_NODE*>(0),"SHARING_INDEX::_discTree"),
  _sharedConst(static_cast<TERM*>(0),"SHARING_INDEX::_sharedConst")
#endif
{
  CALL("constructor SHARING_INDEX()");

  DOP(_discTree.freeze());
  DOP(_sharedConst.freeze());
}; // SHARING_INDEX::SHARING_INDEX()

SHARING_INDEX::~SHARING_INDEX() 
{ 
  CALL("destructor ~SHARING_INDEX()"); 
  if (DestructionMode::isThorough())
    {    
      for (ulong f = 0; f < _discTree.size(); f++)
	{
	  DestroyTree(_discTree[f]);
	};
      
      for (ulong n = 0; n < _sharedConst.size(); n++)
	if (_sharedConst[n])
	  {
	    _sharedConst[n]->DestroyOccurences();
	    delete _sharedConst[n];
	  };
    };
}; // SHARING_INDEX::~SHARING_INDEX() 


void SHARING_INDEX::init()
{
  CALL("init()");
  t_iter.init();
  subterms.init("subterms");
#ifdef NO_DEBUG
  _discTree.init(static_cast<SHARING_TREE_NODE*>(0),0);
  _sharedConst.init(static_cast<TERM*>(0),0);
#else
  _discTree.init(static_cast<SHARING_TREE_NODE*>(0),"SHARING_INDEX::_discTree");
  _sharedConst.init(static_cast<TERM*>(0),"SHARING_INDEX::_sharedConst");
#endif
  DOP(_discTree.freeze());
  DOP(_sharedConst.freeze());
}; // void SHARING_INDEX::init()


void SHARING_INDEX::destroy()
{
  CALL("destroy()");
   
  if (DestructionMode::isThorough())
    {    
      for (ulong f = 0; f < _discTree.size(); f++)
	{
	  DestroyTree(_discTree[f]);
	};
      
      for (ulong n = 0; n < _sharedConst.size(); n++)
	if (_sharedConst[n])
	  {
	    _sharedConst[n]->DestroyOccurences();
	    delete _sharedConst[n];
	  };
    };

  _sharedConst.destroy();
  _discTree.destroy();
  subterms.destroy(); 
  t_iter.destroy();;
}; // void SHARING_INDEX::destroy()


void SHARING_INDEX::reset() 
{
  CALL("reset()");    
  for (ulong f = 0; f < _discTree.size(); f++)
    {
      DestroyTree(_discTree[f]);
      _discTree[f] = 0;
    };
   
  for (ulong n = 0; n < _sharedConst.size(); n++)
    if (_sharedConst[n])
      _sharedConst[n]->DestroyOccurences();
}; // void SHARING_INDEX::reset() 


inline SHARING_TREE_NODE* SHARING_INDEX::CreateBranch(TERM* whole)
{
  CALL("CreateBranch(TERM* whole)");
  SHARING_TREE_NODE* result = new SHARING_TREE_NODE(t_iter.Symbol());
  SHARING_TREE_NODE* node = result;
  while (t_iter.Next())
    {
      node->SetBelow(new SHARING_TREE_NODE(t_iter.Symbol()));
      node = node->Below();
    };
  node->SetBelow(new SHARING_TREE_NODE(whole));
  return result; 
}; // SHARING_TREE_NODE* SHARING_INDEX::CreateBranch(TERM* whole)
 
 


inline TERM* SHARING_INDEX::IntegrateComplexNonconst(TERM* t)
{
  CALL("IntegrateComplexNonconst(TERM* t)");
  ASSERT(t->arity());
  TERM::Iterator iter;
  iter.Reset(t);
  SURFER surf(this);
  SHARING_TREE_NODE* leaf;
  TERM sym; 
  
  if (surf.Reset(*t))
    {
    read_mode_next_sym:
      if (iter.Next())
	{
	  sym = iter.Symbol();
	  if (surf.Read(sym)) { goto read_mode_next_sym; }
	  else
	    {
	      surf.Write(sym);
	      goto write_mode_next_sym;
	    };
	}
      else // the term is in the index already
	{
	  leaf = surf.Leaf();
       
	  ASSERT(leaf);
	  ASSERT(leaf->IsLeaf());
  
	  leaf->IncNum();
	  return leaf->Term();
	}; 
    }
  else
    {
    write_mode_next_sym:
      if (iter.Next())
	{
	  surf.Write(iter.Symbol());
	  goto write_mode_next_sym;
	}
      else // branch created, leaf with a copy of the term must be created also          
	{
	  TERM* copy = t->CopyTop();
     

	  copy->InitOccurences();


	  TERM* src = t->Args();
	  TERM* targ = copy->Args();
	check_arg:
	  switch (src->Flag())
	    {
	    case TermVar:
	      *targ = *src;
	      src = src->Next();
	      targ = targ->Next();
	      goto check_arg;
	    case TermRef:
	      if (src->First()->arity())
		{
		  *targ = *src;
		  subterms.push(targ);
		}
	      else // constant
		{
		  targ->SetFirst(Const(*(src->First())));
		};
	      src = src->Next();
	      targ = targ->Next();
	      goto check_arg;
	    case TermComplex:
	      surf.CreateLeaf(copy);
	      INTERCEPT_BACKWARD_SUBSUMPTION_INDEXING_INTRODUCE_TERM_ID(copy);
	      return copy;
	    };
	};
    };
  
  return 0; // to surpress warnings 
}; // TERM* SHARING_INDEX::IntegrateComplexNonconst(TERM* t) 


TERM* SHARING_INDEX::IntegrateComplex(TERM* t)
{
  if (t->arity())
    {
      subterms.reset();
      TERM* result = IntegrateComplexNonconst(t);
      TERM* subt;
      while (!(subterms.empty()))
	{
	  subt = subterms.pop();
	  subt->SetFirst(IntegrateComplexNonconst(subt->First()));
	};
      return result;
    }
  else // constant
    {
      return Const(*t);
    };
}; // TERM* SHARING_INDEX::IntegrateComplex(TERM* t)

TERM* SHARING_INDEX::CopyLiteral(TERM* source)
{
  CALL("CopyLiteral(TERM* source)");
  TERM* target = source->CopyLitTop();
  source = source->Args();
  TERM* targ = target->Args();
 copy_arg:
  switch (source->Flag())
    {
    case TermVar:
      *targ = *source;
      source = source->Next();
      targ = targ->Next();
      goto copy_arg;
    case TermRef:
      targ->SetFirst(IntegrateComplex(source->First()));
      source = source->Next();
      targ = targ->Next();
      goto copy_arg;
    case TermComplex:
      INTERCEPT_BACKWARD_SUBSUMPTION_INDEXING_INTRODUCE_LITERAL_ID(target);
      return target;
    };
  return 0; // to surpress warnings
}; // TERM* SHARING_INDEX::CopyLiteral(TERM* source)

TERM* SHARING_INDEX::IntegrateFlatComplex(const Flatterm* t)
{
  CALL("IntegrateFlatComplex(const Flatterm* t)");
  if (t->Symbol().arity())
    {
      return IntegrateFlatComplexNonconst(t);
    }
  else // constant
    {
      return Const(t->Symbol());
    };
}; // TERM* SHARING_INDEX::IntegrateFlatComplex(const Flatterm* t)




inline TERM* SHARING_INDEX::IntegrateFlatComplexNonconst(const Flatterm* t)
{
  CALL("IntegrateFlatComplexNonconst(const Flatterm* t)");
  ASSERT(t->Symbol().arity());
  
  Flatterm::ConstIterator iter;
  iter.Reset(t);
  SURFER surf(this);
  SHARING_TREE_NODE* leaf;
  TERM sym;
  
  if (surf.Reset(t->Symbol()))
    {
    read_mode_next_sym:
      iter.NextSym();
      if (iter.EndOfTerm()) // the term is in the index already
	{
	  leaf = surf.Leaf();
       
	  ASSERT(leaf);
	  ASSERT(leaf->IsLeaf());
  
	  leaf->IncNum();
	  return leaf->Term();
	}; 
      sym = iter.CurrPos()->Symbol();
      if (surf.Read(sym)) { goto read_mode_next_sym; };
      surf.Write(sym);
      goto write_mode_next_sym;
    }
  else
    {
    write_mode_next_sym:
      iter.NextSym();
      if (iter.EndOfTerm()) // branch created, leaf with a copy of the term must be created also
	{
	  TERM* copy = new (t->Symbol().arity()) TERM(t->Symbol());

	  // copy->Info()->SetFirst(0);
	  copy->InitOccurences();

	  surf.CreateLeaf(copy);
	  TERM* targ = copy->Args();
	  iter.Reset(t);
	  iter.NextSym();
	copy_argument:
	  if (iter.CurrPos()->Symbol().isVariable())
	    {
	      *targ = iter.CurrPos()->Symbol();
	    }
	  else // complex term
	    {
	      if (iter.CurrPos()->Symbol().arity())
		{
		  targ->SetFirst(IntegrateFlatComplexNonconst(iter.CurrPos()));
		}
	      else // constant
		{
		  targ->SetFirst(Const(iter.CurrPos()->Symbol())); 
		}; 
	    }; 
	  iter.after();
	  if (iter.EndOfTerm())
	    {
	      INTERCEPT_BACKWARD_SUBSUMPTION_INDEXING_INTRODUCE_TERM_ID(copy);
	      return copy;
	    };
	  targ = targ->Next(); 
	  goto copy_argument;
       
	};
      surf.Write(iter.CurrPos()->Symbol());
      goto write_mode_next_sym;
    };
}; // TERM* SHARING_INDEX::IntegrateFlatComplexNonconst(const Flatterm* t)



TERM* SHARING_INDEX::CopyFlatLiteral(const Flatterm* source)
{
  CALL("CopyFlatLiteral(const Flatterm* source)");
  TERM* target = TERM::AllocLit(source->Symbol());
  TERM* targ = target->Args();
  Flatterm::ConstIterator iter;
  iter.Reset(source);
  iter.NextSym();
 copy_arg:
  if (iter.EndOfTerm()) 
    { 
      INTERCEPT_BACKWARD_SUBSUMPTION_INDEXING_INTRODUCE_LITERAL_ID(target);
      return target; 
    };
  if (iter.CurrPos()->Symbol().isVariable())
    {
      *targ = iter.CurrPos()->Symbol(); 
    }
  else // complex
    {
      targ->SetFirst(IntegrateFlatComplex(iter.CurrPos()));
    }; 
  iter.after();
  targ = targ->Next(); 
  goto copy_arg;
}; // TERM* SHARING_INDEX::CopyFlatLiteral(const Flatterm* source)

TERM* SHARING_INDEX::CopyFlatEq(const TERM& header,
				const Flatterm* arg1,
				const Flatterm* arg2)
{
  CALL("CopyFlatEq(const TERM& header,const Flatterm* arg1,const Flatterm* arg2)");
  TERM* result = TERM::AllocLit(header);
  TERM* rarg = result->Args();
  if (arg1->Symbol().isVariable()) { *rarg = arg1->Symbol(); }
  else rarg->SetFirst(IntegrateFlatComplex(arg1));
  rarg = rarg->Next();
  if (arg2->Symbol().isVariable()) { *rarg = arg2->Symbol(); }
  else rarg->SetFirst(IntegrateFlatComplex(arg2));
  INTERCEPT_BACKWARD_SUBSUMPTION_INDEXING_INTRODUCE_LITERAL_ID(result);
  return result;
}; // TERM* SHARING_INDEX::CopyFlatEq(const TERM& header,..  

 
void SHARING_INDEX::DestroyLiteralCopy(TERM* t)
{ 
  CALL("DestroyLiteralCopy(TERM* t)");
  INTERCEPT_BACKWARD_SUBSUMPTION_INDEXING_UNBIND_LITERAL_ID(t);
  for (TERM* arg = t->Args(); ; arg = arg->Next())
    {
      switch (arg->Flag())
	{
	case TermVar : break;
	case TermRef : 
	  RemoveComplex(arg->First());
	  break;
	case TermComplex :
	  t->DeleteLit();
	  //delete t;
	  return;
	};
    };
}; // void SHARING_INDEX::DestroyLiteralCopy(TERM* t)     
 
 
TERM* SHARING_INDEX::CopyComplex(TERM* source)
{
  CALL("CopyComplex(TERM* source)");
  TERM* target = source->CopyTop();

  target->InitOccurences();

  source = source->Args();
  TERM* targ = target->Args();
 copy_arg:
  switch (source->Flag())
    {
    case TermVar:
      *targ = *source;
      source = source->Next();
      targ = targ->Next();
      goto copy_arg;
    case TermRef:
      targ->SetFirst(IntegrateComplex(source->First()));
      source = source->Next();
      targ = targ->Next();
      goto copy_arg;
    case TermComplex:
      INTERCEPT_BACKWARD_SUBSUMPTION_INDEXING_INTRODUCE_TERM_ID(target);
      return target;
    };
  return 0; // to surpress warnings
}; // TERM* SHARING_INDEX::CopyComplex(TERM* source)

  
void SHARING_INDEX::RemoveComplex(TERM* t)
{
  CALL("RemoveComplex(TERM* t)");
  if (t->arity())
    {
      RemoveComplexNonconst(t);
    };
}; // void SHARING_INDEX::Remove(TERM* t)
  
void SHARING_INDEX::RemoveComplexNonconst(TERM* t)
{
  CALL("RemoveComplexNonconst(TERM* t)");
  ASSERT((t->Flag() == TermComplex) && (t->arity()));
  
  Stack<TERM*,VampireKernelConst::MaxTermSize> subterms("subterms(local)");
  TERM* subterm;
  SHARING_TREE_NODE* node;     
  SHARING_TREE_NODE** branch;
  SHARING_TREE_NODE** below_addr; 
  TERM::Iterator iter;
  SHARING_TREE_NODE* branch_to_delete;
  SHARING_TREE_NODE* node_to_delete;
 
 process_next_term: 
  
  ASSERT_IN((t->Flag() == TermComplex) && (t->arity()),"A2"); 
   
     
  branch = Tree(*t);
  node = *branch;
  //branch = &(nonconst_index[t->functor()]);
  //node = nonconst_index[t->functor()];  

  ASSERT_IN(node,"A3");
  below_addr = branch;
  iter.Reset(t);
  iter.Next();
  do
    {
           
      ASSERT_IN(!(node->IsLeaf()),"A4"); 
      if (node->Alternative())
	{
	  branch = below_addr;
        check_node:
	  if (node->Tag() == iter.Symbol())
	    {
	      below_addr = node->BelowAddr();
	      node = node->Below();
	    }
	  else // try alternative
	    {
	      branch = node->AlternativeAddr();
	      node = node->Alternative();
	      ASSERT(node);
	      goto check_node;
	    };
	}
      else
	{
	  ASSERT(node->Tag() == iter.Symbol());
	  below_addr = node->BelowAddr();
	  node = node->Below();
	};
    }
  while (iter.Next());
   
  // here node is a leaf
   
  node->DecNum();

  if (!(node->Num())) 
    {
      // last occurence of the term, top level of the term must be destroyed,
      // the branch must be deleted

      INTERCEPT_BACKWARD_SUBSUMPTION_INDEXING_UNBIND_TERM_ID(node->Term());

      subterm = node->Term()->Args();
      while (!(subterm->IsNull()))
	{
	  if ((subterm->IsReference()) && (subterm->First()->arity()))
	    {
	      subterms.push(subterm->First());
	    };
	  subterm = subterm->Next();
	}; 
     
      delete node->Term(); 
      
      branch_to_delete = *branch;
      *branch = (*branch)->Alternative(); 
      while (!(branch_to_delete->IsLeaf()))
	{
	  node_to_delete = branch_to_delete;
	  branch_to_delete = branch_to_delete->Below();
	  delete node_to_delete;
	};
      delete branch_to_delete; // leaf
    };

  if (!(subterms.empty()))    // was ommited in vampire 1.31 thus giving a serious memory leak
    {
      t = subterms.pop();
      goto process_next_term;
    };     
}; // void SHARING_INDEX::RemoveComplexNonconst(TERM* const t)

inline void SHARING_INDEX::DestroyTree(SHARING_TREE_NODE* tree)
{
  CALL("DestroyTree(SHARING_TREE_NODE* tree)");
  if (tree)
    {
      if (tree->IsLeaf()) 
	{
	  ASSERT(tree->Term()->isComplex());
	  tree->Term()->DestroyOccurences();
	  delete tree->Term();
	  delete tree; 
	}
      else
	{
	  if (tree->Alternative()) DestroyTree(tree->Alternative());
	  if (tree->Below()) DestroyTree(tree->Below());
	  delete tree;
	};
    };
}; // void SHARING_INDEX::DestroyTree(SHARING_TREE_NODE* tree)


#ifndef NO_DEBUG

void SHARING_INDEX::ScanIndex(ostream& str)
{
  CALL("ScanIndex()");
  ulong numOfNodes = 0;

  for (ulong f = 0; f < _discTree.size(); f++)
    {
      ScanTree(str,_discTree[f],numOfNodes); 
    };
  str << "NUMBER OF NODES: " << numOfNodes << "\n";
}; // void SHARING_INDEX::ScanIndex(ostream& str)


void SHARING_INDEX::ScanTree(ostream& str,SHARING_TREE_NODE* tree,ulong& numOfNodes)
{
  CALL("ScanTree(SHARING_TREE_NODE* tree,ulong& numOfNodes)");
  if (!tree) return;
  numOfNodes++;
  if (tree->IsLeaf()) 
    {
      str << tree->Num() << " x " << tree->Term() << "\n";
      return;
    }; 
  ScanTree(str,tree->Below(),numOfNodes);
  ScanTree(str,tree->Alternative(),numOfNodes);
}; // void SHARING_INDEX::ScanTree(ostream& str,SHARING_TREE_NODE* tree,ulong& numOfNodes)
#endif

//====================================================
