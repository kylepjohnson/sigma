//
// File:         fs_command.cpp
// Description:  Command of the abstract machine for forward subsumption.
// Created:      Apr 12, 2001, 17:50
// Author:       Alexandre Riazanov
// mail:         riazanov@cs.man.ac.uk 
//====================================================
#include "fs_command.hpp" 
#include "Stack.hpp"
//================================================
#undef DEBUG_NAMESPACE
#ifdef DEBUG_FS_COMMAND
 #define DEBUG_NAMESPACE "FS_COMMAND"
#endif
#include "debugMacros.hpp"
//=================================================

using namespace BK;
using namespace VK;

#ifdef DEBUG_ALLOC_OBJ_TYPE
ClassDesc FunSet::BitStr::_classDesc("FunSet::BitStr",
										 FunSet::BitStr::minSize(),
										 FunSet::BitStr::maxSize());

ClassDesc FS_COMMAND::_classDesc("FS_COMMAND",FS_COMMAND::minSize(),
									 FS_COMMAND::maxSize());

#endif




void FS_COMMAND::DestroyTree(FS_COMMAND* tree) 
{
  CALL("DestroyTree(FS_COMMAND* tree)");
  ASSERT(tree);
  if (tree->Fork()) DestroyTree(tree->Fork());
  if (tree->IsSuccess()) 
    {
      delete tree; 
    }
  else
    {
      DestroyTree(tree->Next());
      delete tree; 
    };
}; // void FS_COMMAND::DestroyTree(FS_COMMAND* tree) 
   
void FS_COMMAND::DestroyTree(FS_COMMAND* tree,void (*destroyIndexedObj)(void*)) 
{
  CALL("DestroyTree(FS_COMMAND* tree,void (*destroyIndexedObj)(void*))"); 
  ASSERT(tree);
  if (tree->Fork()) DestroyTree(tree->Fork());
  if (tree->IsSuccess()) 
    {
      destroyIndexedObj(tree->IndexedObject());
      delete tree; 
    }
  else
    {
      DestroyTree(tree->Next());
      delete tree; 
    };
}; // void FS_COMMAND::DestroyTree(FS_COMMAND* tree,void (*destroyIndexedObj)(void*)) 



#ifndef NO_DEBUG_VIS
ostream& FS_COMMAND::outputTree(ostream& str,FS_COMMAND* tree,const Flatterm** subst)
{
  long fork_num = 0;
  if (!tree) { return str; };
  Stack<FS_COMMAND*,MAX_FS_CODE_DEPTH> forks("forks");
  Stack<long,MAX_FS_CODE_DEPTH> fork_nums("fork_nums");
 
  FS_COMMAND* node = tree;
 
 next_branch:
  while (node)
    {
      node->output(str,subst);
      if (node->Fork()) 
	{
	  forks.push(node); 
	  fork_nums.push(fork_num); 
	  str << " [" << fork_num << ']';
	  fork_num++;
	};
      str << '\n';
      node = node->Next();
    };
  if (forks.empty()) { return str; };
  node = forks.pop();
  str << "\nFORK [" << fork_nums.pop() << "]\n";
  node = node->Fork();
  goto next_branch;
}; // ostream& FS_COMMAND::outputTree(ostream& str,FS_COMMAND* tree,const Flatterm** subst)
 
ostream& FS_COMMAND::output(ostream& str,const FunSet& fs) 
{
  str << '{';
  ulong ct = fs.count();
  TERM header; 
  for (size_t el = 0; el < fs.upperBound(); el++)
    if (fs.contains(el))
      {
	header.MakeHeader(el);
	str << header;
	ct--;
	if (ct) str << ',';
      }; 
  return str << '}';
}; // ostream& FS_COMMAND::output(ostream& str,const FunSet& fs) 

ostream& FS_COMMAND::output(ostream& str,const Flatterm** vars) const
{
  str << '[' << (ulong)this << ']';
  TERM header;
  switch (Tag())
    {
    case LIT_HEADER: header.MakeHeader(HeaderNum()); str << "LIT_HEADER " << header; break;
    case PROP_LIT: header.MakeHeader(HeaderNum()); str << "PROP_LIT " << header; break;   
    case UNORD_EQ: header.MakeHeader(HeaderNum()); str << "UNORD_EQ " << header; break; 
    case ORD_EQ: header.MakeHeader(HeaderNum()); str << "ORD_EQ " << header; break; 
    case SECOND_EQ_ARG: str << "SECOND_EQ_ARG "; break;
    case FUNC: str << "FUNC " << Func(); break;
    case VAR: str << "VAR " << Var() - vars; break;
    case COMPARE: str << "COMPARE " << Var1() - vars << ' ' << Var2() - vars; break;
    case COMPARE_NF: str << "COMPARE_NF " << Var1() - vars << ' ' << Var2() - vars; break;
    case SIG_FILTER: str << "SIG_FILTER "; output(str,*(Functors())); break;
    case SUCCESS: str << "SUCCESS " << TheClause(); break;
    };
  return str;
}; // ostream& FS_COMMAND::output(ostream& str,const Flatterm** vars) const
#endif


//====================================================
