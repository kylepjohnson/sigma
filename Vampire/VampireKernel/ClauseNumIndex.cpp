//
// File:         ClauseNumIndex.cpp
// Description:  Searching clauses by numbers.
// Created:      Apr 08, 2000, 22:00
// Author:       Alexandre Riazanov
// mail:         riazanov@cs.man.ac.uk
//============================================================================
#include "ClauseNumIndex.hpp"
#include "ClassDesc.hpp"
#include "Clause.hpp"
//===========================================================================
#undef DEBUG_NAMESPACE
#ifdef DEBUG_CLAUSE_NUM_INDEX
 #define DEBUG_NAMESPACE "ClauseNumIndex"
#endif
#include "debugMacros.hpp"
//============================================================================

using namespace BK;
using namespace VK;

#ifdef DEBUG_ALLOC_OBJ_TYPE
 ClassDesc ClauseNumIndex::List::_classDesc("ClauseNumIndex::List",
                                             ClauseNumIndex::List::minSize(),
                                             ClauseNumIndex::List::maxSize());

 ClassDesc ClauseNumIndex::List::Node::_classDesc("ClauseNumIndex::List::Node",
                                                   ClauseNumIndex::List::Node::minSize(),
                                                   ClauseNumIndex::List::Node::maxSize());
#endif

void ClauseNumIndex::insert(Clause* cl)
{
 CALL("insert(Clause* cl)");
 bool newNode; 
 List::Node* node = _list.insert(cl->Number(),newNode);
 ASSERT(newNode);
 node->value() = cl;
 ASSERT(!cl->flag(Clause::FlagInClauseNumIndex));
 cl->setFlag(Clause::FlagInClauseNumIndex);
}; // void ClauseNumIndex::insert(Clause* cl)

void ClauseNumIndex::remove(Clause* cl)
{
 CALL("remove(Clause* cl)");
 ASSERT(cl->flag(Clause::FlagInClauseNumIndex));
 Clause* removedClause;
 ALWAYS(_list.remove(cl->Number(),removedClause));
 ASSERT(removedClause == cl);
 ASSERT(cl->flag(Clause::FlagInClauseNumIndex));
 cl->clearFlag(Clause::FlagInClauseNumIndex);
}; // void ClauseNumIndex::remove(Clause* cl)

Clause* ClauseNumIndex::find(ulong num) 
{
 CALL("find(ulong num)");
 _surfer.reset(_list);
 const List::Node* node;
 if (_surfer.find(num,node)) return node->value();
 return 0;
}; // Clause* ClauseNumIndex::find(ulong num) 

//============================================================================
           
