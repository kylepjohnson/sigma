//
// File:         ClauseQueue.hpp
// Description:  Queues of clauses. 
// Created:      Apr 13, 2001, 16:00
// Author:       Alexandre Riazanov
// mail:         riazanov@cs.man.ac.uk
//============================================================================
#ifndef CLAUSE_QUEUE_H
//=============================================================================
#define CLAUSE_QUEUE_H
#include <iostream> 
#include "jargon.hpp"
#include "FIFO.hpp"
#include "GlobAlloc.hpp"
#include "Clause.hpp"
//======================================================================

namespace VK
{
typedef BK::FIFO<BK::GlobAlloc,Clause*,Clause> ClauseQueue;
typedef BK::FIFO<BK::GlobAlloc,const Clause*,Clause> ConstClauseQueue;
}; // namespace VK

namespace std
{
  inline
  ostream& operator<<(ostream& str,const VK::ClauseQueue& q)
  {
    for (const VK::ClauseQueue::Entry* e = q.begin();
	 e;
	 e = e->next())
      str << e->value() << "\n";
    return  str;
  };
  inline
  ostream& operator<<(ostream& str,const VK::ConstClauseQueue& q)
  {
    for (const VK::ConstClauseQueue::Entry* e = q.begin();
	 e;
	 e = e->next())
      str << e->value() << "\n";
    return  str;
  };

};

//======================================================================
#endif
