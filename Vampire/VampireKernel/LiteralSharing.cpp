//
// File:         LiteralSharing.cpp
// Description:  Implements literal sharing.
// Created:      Dec 13, 2001 
// Author:       Alexandre Riazanov
// mail:         riazanov@cs.man.ac.uk
// Note:         Was LITERAL_INDEX in clause_index.{h,cpp}.
//============================================================================
#include "LiteralSharing.hpp"
#include "Flatterm.hpp"
#include "dtree.hpp"
#include "sharing.hpp"
//============================================================================
#undef DEBUG_NAMESPACE
#ifdef DEBUG_LITERAL_SHARING
 #define DEBUG_NAMESPACE "LiteralSharing"
#endif
#include "debugMacros.hpp"
//============================================================================

using namespace BK;
using namespace VK;



LiteralSharing::LiteralSharing(SHARING_INDEX* si) 
  : 
#ifdef NO_DEBUG
  _propositional(static_cast<TERM*>(0),0),
#else
  _propositional(static_cast<TERM*>(0),"LiteralSharing::_propositional"),  
#endif 

  _sharingIndex(si),

#ifdef NO_DEBUG
  _discTree(static_cast<DTREE_NODE*>(0),0)
#else
  _discTree(static_cast<DTREE_NODE*>(0),"LiteralSharing::_discTree")
#endif 

{
  CALL("constructor LiteralSharing(SHARING_INDEX* si)");
   
  DOP(_propositional.freeze());  
  DOP(_discTree.freeze()); 
}; // LiteralSharing::LiteralSharing(SHARING_INDEX* si) 

LiteralSharing::~LiteralSharing() 
{ 
  CALL("destructor ~LiteralSharing()"); 
  if (DestructionMode::isThorough())
    {
      ulong n;
      for (n = 0UL; n < _propositional.size(); n++)
	if (_propositional[n]) _propositional[n]->DeleteLit();
      for (n = 0UL; n < _discTree.size(); n++)
	{
	  DTREE::Destroy(_discTree[n],DestroyLeafContent);
	};
    };      
}; // LiteralSharing::~LiteralSharing() 


void LiteralSharing::init(SHARING_INDEX* si)
{
  CALL("init(SHARING_INDEX* si)");
#ifdef NO_DEBUG
  _propositional.init(static_cast<TERM*>(0),0);
#else
  _propositional.init(static_cast<TERM*>(0),"LiteralSharing::_propositional");
#endif 

  _sharingIndex = si;

#ifdef NO_DEBUG
  _discTree.init(static_cast<DTREE_NODE*>(0),0);
#else
  _discTree.init(static_cast<DTREE_NODE*>(0),"LiteralSharing::_discTree");
#endif 
  DOP(_propositional.freeze());  
  DOP(_discTree.freeze()); 
}; // void LiteralSharing::init(SHARING_INDEX* si)


void LiteralSharing::destroy()
{
  CALL("destroy()");  
  if (DestructionMode::isThorough())
    {
      ulong n;
      for (n = 0UL; n < _propositional.size(); n++)
	if (_propositional[n]) _propositional[n]->DeleteLit();
      for (n = 0UL; n < _discTree.size(); n++)
	{
	  DTREE::Destroy(_discTree[n],DestroyLeafContent);
	};
    };
  _discTree.destroy();
  _propositional.destroy();
  BK_CORRUPT(*this);
}; // void LiteralSharing::destroy()




void LiteralSharing::reset()
{
  CALL("reset()");
  ulong n;
  for (n = 0UL; n < _propositional.size(); n++)
    if (_propositional[n])
      *(_propositional[n]->InfoField()) = 0; 

  for (n = 0UL; n < _discTree.size(); n++)
    {
      DTREE::Destroy(_discTree[n],DestroyLeafContent);
      _discTree[n] = 0; 
    };
  
}; //void LiteralSharing::reset()  



DTREE_NODE* LiteralSharing::Integrate(TERM* lit) 
  // returns the leaf corresponding to the literal
{
  CALL("Integrate(TERM* lit)");
  TERM header = *lit;
  DTREE_NODE** tree = Tree(header);
  
  if (header.arity())
    {
      TERM::Iterator iter;
      iter.Reset(lit);
      DTREE::SURFER surf;
      surf.Reset(tree,1);
   
      if (*tree)
	{
	read_mode_next_sym:
	  if (iter.Next())
	    {
	      if (surf.Read(iter.Symbol()))
		{
		  goto read_mode_next_sym;
		}
	      else
		{
		  surf.Write(iter.Symbol());
		  goto write_mode_next_sym;
		}; 
	    }
	  else // the literal is in the index already
	    {
	      return surf.Leaf();
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
	  else // end of the literal, leaf must be created
	    {
	      surf.CreateLeaf(_sharingIndex->CopyLiteral(lit));
         
	      *(surf.Leaf()->Term()->InfoField()) = 0;
         
	      return surf.Leaf();
	    };
	}; 
    }
  else // propositional
    {
      if (!(*tree)) 
	{
	  *tree = new DTREE_NODE(ShareProp(header));
	};
    
      ASSERT((*tree)->IsLeaf());
    
      return *tree;
    };
}; // DTREE_NODE* LiteralSharing::Integrate(TERM* lit)


DTREE_NODE* LiteralSharing::Integrate(const Flatterm* lit) 
  // returns the leaf corresponding to the literal
{
  CALL("Integrate(const Flatterm* lit)");
  TERM header = lit->Symbol();
  DTREE_NODE** tree = Tree(header);
  
  if (header.arity())
    {
      Flatterm::ConstIterator iter;
      iter.Reset(lit);
      DTREE::SURFER surf;
      surf.Reset(tree,1);
      if (*tree)
	{
	read_mode_next_sym:
	  iter.NextSym();
	  if (iter.EndOfTerm()) return surf.Leaf(); // the literal is in the index already
	  if (surf.Read(iter.CurrPos()->Symbol())) { goto read_mode_next_sym; };
	  surf.Write(iter.CurrPos()->Symbol()); 
	  goto write_mode_next_sym;
	}
      else
	{
	write_mode_next_sym:
	  iter.NextSym();
	  if (iter.EndOfTerm()) 
	    {
	      surf.CreateLeaf(_sharingIndex->CopyFlatLiteral(lit));
         
	      *(surf.Leaf()->Term()->InfoField()) = 0;
         
	      return surf.Leaf();
	    };       
	  surf.Write(iter.CurrPos()->Symbol());
	  goto write_mode_next_sym;
	}; 
    }
  else // propositional
    {
      if (!(*tree)) 
	{
	  *tree = new DTREE_NODE(ShareProp(header));
	};
    

      ASSERT((*tree)->IsLeaf());
    
      return *tree;
    };
}; // DTREE_NODE* LiteralSharing::Integrate(const Flatterm* lit) 


DTREE_NODE* LiteralSharing::IntegrateEq(const TERM& header,const Flatterm* arg1,const Flatterm* arg2) 
  // returns the leaf corresponding to the literal
{
  CALL("IntegrateEq(const TERM&,const Flatterm*,const Flatterm*)");
  DTREE_NODE** tree = Tree(header);
  Flatterm::ConstIterator iter;
  iter.Reset(arg1);
  bool first_arg = true;
  DTREE::SURFER surf;
  surf.Reset(tree,1);
  if (*tree)
    {
    read_mode_next_sym:
      if (iter.EndOfTerm()) 
	{
	  if (first_arg)
	    {// the second argument must be processed
	      iter.Reset(arg2);
	      first_arg = false;
	      goto read_mode_next_sym;
	    };
	  return surf.Leaf(); // the literal is in the index already
	};
      if (surf.Read(iter.CurrPos()->Symbol())) 
	{
	  iter.NextSym();
	  goto read_mode_next_sym;
	};
      surf.Write(iter.CurrPos()->Symbol()); 
      iter.NextSym();
      goto write_mode_next_sym;
    }
  else
    {
    write_mode_next_sym:
      if (iter.EndOfTerm()) 
	{
	  if (first_arg)
	    {// the second argument must be processed
	      iter.Reset(arg2);
	      first_arg = false;
	      goto write_mode_next_sym;
	    };
	  surf.CreateLeaf(_sharingIndex->CopyFlatEq(header,arg1,arg2));
	  *(surf.Leaf()->Term()->InfoField()) = 0;
	  return surf.Leaf();
	};
       
      surf.Write(iter.CurrPos()->Symbol());
      iter.NextSym();
      goto write_mode_next_sym;
    }; 
}; // DTREE_NODE* LiteralSharing::IntegrateEq(const TERM& header,const Flatterm* arg1,const Flatterm* arg2) 



void LiteralSharing::InfoForDeletion(TERM* lit,
                                     DTREE_NODE**& branch_to_destroy,
                                     DTREE_NODE*& leaf,
                                     DTREE_NODE*& last_fork)
{
  CALL("InfoForDeletion(TERM*,DTREE_NODE**&,DTREE_NODE*&,DTREE_NODE*&)");
  // lit can't be propositional here
  ASSERT(lit->arity());
  
  DTREE_NODE** tree = Tree(*lit); // *tree must be nonnull and nonleaf here
    
  ASSERT(*tree);
  ASSERT(!((*tree)->IsLeaf()));
  
  TERM::Iterator iter;
  iter.Reset(lit);
  iter.Next(); // skipping the header
  
  DTREE::InfoForDeletion(iter,tree,branch_to_destroy,leaf,last_fork);
}; // void LiteralSharing::InfoForDeletion(TERM* lit,...              




void LiteralSharing::DestroyLeafContent(DTREE_NODE* leaf)
{  
  if (leaf->Term()->arity()) 
    leaf->Term()->DeleteLit();
}; // void LiteralSharing::DestroyLeafContent(DTREE_NODE* leaf)



inline TERM* LiteralSharing::ShareSplittingBranchName(const TERM& prop)
{
  CALL("ShareSplittingBranchName(const TERM& prop)");
  ASSERT(!(prop.arity()));
  ASSERT(prop.IsSplitting());
 
  ulong hdNum = prop.HeaderNum();
  DOP(_propositional.unfreeze());
  TERM* res = _propositional.sub(hdNum);
  DOP(_propositional.freeze());
  if (!res) 
    {
      res = TERM::AllocLit(prop);
      *(res->InfoField()) = 0;
      _propositional[hdNum] = res;
    };
  return res;
};


inline TERM* LiteralSharing::ShareProp(const TERM& prop)
{
  CALL("ShareProp(const TERM& prop)");
  ASSERT(!(prop.arity()));
  ulong hdNum = prop.HeaderNum();
  DOP(_propositional.unfreeze());
  TERM* res = _propositional.sub(hdNum);
  DOP(_propositional.freeze());
  if (!res) 
    {
      res = TERM::AllocLit(prop);
      *(res->InfoField()) = 0;
      _propositional[hdNum] = res;
    };
  return res;
};




//============================================================================

