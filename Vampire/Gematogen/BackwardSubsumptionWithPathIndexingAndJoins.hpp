//
// File:         BackwardSubsumptionWithPathIndexingAndJoins.hpp
// Description:  Front-end for backward subsumption implemented
//               with path indexing and database joins.
// Created:      Apr 10, 2002, Alexandre Riazanov, riazanov@cs.man.ac.uk
// Note:         This file is a part of the Gematogen library.
//============================================================================
#ifndef BACKWARD_SUBSUMPTION_WITH_PATH_INDEXING_AND_JOINS_H
#define BACKWARD_SUBSUMPTION_WITH_PATH_INDEXING_AND_JOINS_H
//============================================================================
#ifndef NO_DEBUG_VIS
 #include <iostream>
#endif 
#include "jargon.hpp"
#include "Gematogen.hpp"
#include "GematogenDebugFlags.hpp" 
#include "PathIndexForBackwardSubsumption.hpp"
//============================================================================
#undef DEBUG_NAMESPACE
#ifdef DEBUG_BACKWARD_SUBSUMPTION_WITH_PATH_INDEXING_AND_JOINS
#define DEBUG_NAMESPACE "BackwardSubsumptionWithPathIndexingAndJoins<..>"
#endif
#include "debugMacros.hpp"
//============================================================================ 

namespace Gem
{
template <class Alloc,class Term,class ClauseId,ulong MaxTermDepth,ulong MaxTermSize,ulong MaxNumOfVariables>
class BackwardSubsumptionWithPathIndexingAndJoins
{ 
 public:
  typedef typename Term::Symbol Symbol;
  typedef typename Term::Traversal TermTraversal;
  typedef typename Term::Arguments TermArguments;
 public:
  BackwardSubsumptionWithPathIndexingAndJoins() :
    _index()
    {
    };
  ~BackwardSubsumptionWithPathIndexingAndJoins()
    {
    };

  // Integration into index
  bool integrationReset(const ClauseId& clauseId);
  bool integrationOrdinaryNonpropLit(const Symbol& header);
  bool integrationPropLit(const Symbol& lit);
  bool integrationOrdLitNonconstFunc(const Symbol& sym);
  bool integrationOrdLitConst(const Symbol& sym);
  bool integrationOrdLitVar(const Symbol& sym);
  bool integrationSymmetricLit(const Symbol& header);
  bool integrationSymLitSecondArg();
  bool integrationSymLitNonconstFunc(const Symbol& sym);
  bool integrationSymLitConst(const Symbol& sym);
  bool integrationSymLitVar(const Symbol& sym);
  bool integrationEndOfClause();

  // Removal from index
  bool removalReset(const ClauseId& clauseId);
  bool removalOrdinaryNonpropLit(const Symbol& header);
  bool removalPropLit(const Symbol& lit);
  bool removalOrdLitNonconstFunc(const Symbol& sym);
  bool removalOrdLitConst(const Symbol& sym);
  bool removalOrdLitVar(const Symbol& sym);
  bool removalSymmetricLit(const Symbol& header);
  bool removalSymLitSecondArg();
  bool removalSymLitNonconstFunc(const Symbol& sym);
  bool removalSymLitConst(const Symbol& sym);
  bool removalSymLitVar(const Symbol& sym);
  bool removalEndOfClause();


  // Query

  bool queryReset();
  bool queryOrdinaryNonpropLit(const Symbol& header);
  bool queryPropLit(const Symbol& lit);
  bool queryOrdLitNonconstFunc(const Symbol& sym);
  bool queryOrdLitConst(const Symbol& sym);
  bool queryOrdLitVar(const Symbol& sym);
  bool querySymmetricLit(const Symbol& header);
  bool querySymLitSecondArg();
  bool querySymLitNonconstFunc(const Symbol& sym);
  bool querySymLitConst(const Symbol& sym);
  bool querySymLitVar(const Symbol& sym);
  bool queryEndOfClause();


 private:
  PathIndexForBackwardSubsumption<Alloc,Term,ClauseId,MaxTermDepth,MaxTermSize> _index;
}; // class BackwardSubsumptionWithPathIndexingAndJoins<..> 

}; // namespace Gem
//============================================================================
#undef DEBUG_NAMESPACE
#ifdef DEBUG_BACKWARD_SUBSUMPTION_WITH_PATH_INDEXING_AND_JOINS
#define DEBUG_NAMESPACE "BackwardSubsumptionWithPathIndexingAndJoins<..>"
#endif
#include "debugMacros.hpp"
//============================================================================ 


namespace Gem
{


template <class Alloc,class Term,class ClauseId,ulong MaxTermDepth,ulong MaxTermSize,ulong MaxNumOfVariables>
bool BackwardSubsumptionWithPathIndexingAndJoins<Alloc,Term,ClauseId,MaxTermDepth,MaxTermSize,MaxNumOfVariables>::integrationReset(const ClauseId& clauseId)
{
  CALL("integrationReset(const ClauseId& clauseId)");
  return _index.integrationReset(clauseId);
} // bool BackwardSubsumptionWithPathIndexingAndJoins<..>::integrationReset(const ClauseId& clauseId)

template <class Alloc,class Term,class ClauseId,ulong MaxTermDepth,ulong MaxTermSize,ulong MaxNumOfVariables>
bool BackwardSubsumptionWithPathIndexingAndJoins<Alloc,Term,ClauseId,MaxTermDepth,MaxTermSize,MaxNumOfVariables>::integrationOrdinaryNonpropLit(const Symbol& header)
{
  CALL("integrationOrdinaryNonpropLit(const Symbol& header)");
  return _index.integrationOrdinaryNonpropLit(header);
} // bool BackwardSubsumptionWithPathIndexingAndJoins<..>::integrationOrdinaryNonpropLit(const Symbol& header)


template <class Alloc,class Term,class ClauseId,ulong MaxTermDepth,ulong MaxTermSize,ulong MaxNumOfVariables>
bool BackwardSubsumptionWithPathIndexingAndJoins<Alloc,Term,ClauseId,MaxTermDepth,MaxTermSize,MaxNumOfVariables>::integrationPropLit(const Symbol& lit)
{
  CALL("integrationPropLit(const Symbol& lit)");
  return _index.integrationPropLit(lit);
} // bool BackwardSubsumptionWithPathIndexingAndJoins<..>::integrationPropLit(const Symbol& lit)



template <class Alloc,class Term,class ClauseId,ulong MaxTermDepth,ulong MaxTermSize,ulong MaxNumOfVariables>
bool BackwardSubsumptionWithPathIndexingAndJoins<Alloc,Term,ClauseId,MaxTermDepth,MaxTermSize,MaxNumOfVariables>::integrationOrdLitNonconstFunc(const Symbol& sym)
{
  CALL("integrationOrdLitNonconstFunc(const Symbol& sym)");
  return _index.integrationOrdLitNonconstFunc(sym);
} // bool BackwardSubsumptionWithPathIndexingAndJoins<..>::integrationOrdLitNonconstFunc(const Symbol& sym)

template <class Alloc,class Term,class ClauseId,ulong MaxTermDepth,ulong MaxTermSize,ulong MaxNumOfVariables>
bool BackwardSubsumptionWithPathIndexingAndJoins<Alloc,Term,ClauseId,MaxTermDepth,MaxTermSize,MaxNumOfVariables>::integrationOrdLitConst(const Symbol& sym)
{
  CALL("integrationOrdLitConst(const Symbol& sym)");
  return _index.integrationOrdLitConst(sym);
} // bool BackwardSubsumptionWithPathIndexingAndJoins<..>::integrationOrdLitConst(const Symbol& sym)


template <class Alloc,class Term,class ClauseId,ulong MaxTermDepth,ulong MaxTermSize,ulong MaxNumOfVariables>
bool BackwardSubsumptionWithPathIndexingAndJoins<Alloc,Term,ClauseId,MaxTermDepth,MaxTermSize,MaxNumOfVariables>::integrationOrdLitVar(const Symbol& sym)
{
  CALL("integrationOrdLitVar(const Symbol& sym)");
  return _index.integrationOrdLitVar(sym);
} // bool BackwardSubsumptionWithPathIndexingAndJoins<..>::integrationOrdLitVar(const Symbol& sym)



template <class Alloc,class Term,class ClauseId,ulong MaxTermDepth,ulong MaxTermSize,ulong MaxNumOfVariables>
bool BackwardSubsumptionWithPathIndexingAndJoins<Alloc,Term,ClauseId,MaxTermDepth,MaxTermSize,MaxNumOfVariables>::integrationSymmetricLit(const Symbol& header)
{
  CALL("integrationSymmetricLit(const Symbol& header)");
  return _index.integrationSymmetricLit(header);
} // bool BackwardSubsumptionWithPathIndexingAndJoins<..>::integrationSymmetricLit(const Symbol& header)

template <class Alloc,class Term,class ClauseId,ulong MaxTermDepth,ulong MaxTermSize,ulong MaxNumOfVariables>
bool BackwardSubsumptionWithPathIndexingAndJoins<Alloc,Term,ClauseId,MaxTermDepth,MaxTermSize,MaxNumOfVariables>::integrationSymLitSecondArg()
{
  CALL("integrationSymLitSecondArg()");
  return _index.integrationSymLitSecondArg();
} // bool BackwardSubsumptionWithPathIndexingAndJoins<..>::integrationSymLitSecondArg()

template <class Alloc,class Term,class ClauseId,ulong MaxTermDepth,ulong MaxTermSize,ulong MaxNumOfVariables>
bool BackwardSubsumptionWithPathIndexingAndJoins<Alloc,Term,ClauseId,MaxTermDepth,MaxTermSize,MaxNumOfVariables>::integrationSymLitNonconstFunc(const Symbol& sym)
{
  CALL("integrationSymLitNonconstFunc(const Symbol& sym)");
  return _index.integrationSymLitNonconstFunc(sym);
} // bool BackwardSubsumptionWithPathIndexingAndJoins<..>::integrationSymLitNonconstFunc(const Symbol& sym)

template <class Alloc,class Term,class ClauseId,ulong MaxTermDepth,ulong MaxTermSize,ulong MaxNumOfVariables>
bool BackwardSubsumptionWithPathIndexingAndJoins<Alloc,Term,ClauseId,MaxTermDepth,MaxTermSize,MaxNumOfVariables>::integrationSymLitConst(const Symbol& sym)
{
  CALL("integrationSymLitConst(const Symbol& sym)");
  return _index.integrationSymLitConst(sym);
} // bool BackwardSubsumptionWithPathIndexingAndJoins<..>::integrationSymLitConst(const Symbol& sym)


template <class Alloc,class Term,class ClauseId,ulong MaxTermDepth,ulong MaxTermSize,ulong MaxNumOfVariables>
bool BackwardSubsumptionWithPathIndexingAndJoins<Alloc,Term,ClauseId,MaxTermDepth,MaxTermSize,MaxNumOfVariables>::integrationSymLitVar(const Symbol& sym)
{
  CALL("integrationSymLitVar(const Symbol& sym)");
  return _index.integrationSymLitVar(sym);
} // bool BackwardSubsumptionWithPathIndexingAndJoins<..>::integrationSymLitVar(const Symbol& sym)


template <class Alloc,class Term,class ClauseId,ulong MaxTermDepth,ulong MaxTermSize,ulong MaxNumOfVariables>
bool BackwardSubsumptionWithPathIndexingAndJoins<Alloc,Term,ClauseId,MaxTermDepth,MaxTermSize,MaxNumOfVariables>::integrationEndOfClause()
{
  CALL("integrationEndOfClause()");
  return _index.integrationEndOfClause();
} // bool BackwardSubsumptionWithPathIndexingAndJoins<..>::integrationEndOfClause()


template <class Alloc,class Term,class ClauseId,ulong MaxTermDepth,ulong MaxTermSize,ulong MaxNumOfVariables>
bool BackwardSubsumptionWithPathIndexingAndJoins<Alloc,Term,ClauseId,MaxTermDepth,MaxTermSize,MaxNumOfVariables>::removalReset(const ClauseId& clauseId)
{
  CALL("removalReset(const ClauseId& clauseId)");
  return _index.removalReset(clauseId);
} // bool BackwardSubsumptionWithPathIndexingAndJoins<..>::removalReset(const ClauseId& clauseId)

template <class Alloc,class Term,class ClauseId,ulong MaxTermDepth,ulong MaxTermSize,ulong MaxNumOfVariables>
bool BackwardSubsumptionWithPathIndexingAndJoins<Alloc,Term,ClauseId,MaxTermDepth,MaxTermSize,MaxNumOfVariables>::removalOrdinaryNonpropLit(const Symbol& header)
{
  CALL("removalOrdinaryNonpropLit(const Symbol& header)");
  return _index.removalOrdinaryNonpropLit(header);
} // bool BackwardSubsumptionWithPathIndexingAndJoins<..>::removalOrdinaryNonpropLit(const Symbol& header)


template <class Alloc,class Term,class ClauseId,ulong MaxTermDepth,ulong MaxTermSize,ulong MaxNumOfVariables>
bool BackwardSubsumptionWithPathIndexingAndJoins<Alloc,Term,ClauseId,MaxTermDepth,MaxTermSize,MaxNumOfVariables>::removalPropLit(const Symbol& lit)
{
  CALL("removalPropLit(const Symbol& lit)");
  return _index.removalPropLit(lit);
} // bool BackwardSubsumptionWithPathIndexingAndJoins<..>::removalPropLit(const Symbol& lit)



template <class Alloc,class Term,class ClauseId,ulong MaxTermDepth,ulong MaxTermSize,ulong MaxNumOfVariables>
bool BackwardSubsumptionWithPathIndexingAndJoins<Alloc,Term,ClauseId,MaxTermDepth,MaxTermSize,MaxNumOfVariables>::removalOrdLitNonconstFunc(const Symbol& sym)
{
  CALL("removalOrdLitNonconstFunc(const Symbol& sym)");
  return _index.removalOrdLitNonconstFunc(sym);
} // bool BackwardSubsumptionWithPathIndexingAndJoins<..>::removalOrdLitNonconstFunc(const Symbol& sym)

template <class Alloc,class Term,class ClauseId,ulong MaxTermDepth,ulong MaxTermSize,ulong MaxNumOfVariables>
bool BackwardSubsumptionWithPathIndexingAndJoins<Alloc,Term,ClauseId,MaxTermDepth,MaxTermSize,MaxNumOfVariables>::removalOrdLitConst(const Symbol& sym)
{
  CALL("removalOrdLitConst(const Symbol& sym)");
  return _index.removalOrdLitConst(sym);
} // bool BackwardSubsumptionWithPathIndexingAndJoins<..>::removalOrdLitConst(const Symbol& sym)


template <class Alloc,class Term,class ClauseId,ulong MaxTermDepth,ulong MaxTermSize,ulong MaxNumOfVariables>
bool BackwardSubsumptionWithPathIndexingAndJoins<Alloc,Term,ClauseId,MaxTermDepth,MaxTermSize,MaxNumOfVariables>::removalOrdLitVar(const Symbol& sym)
{
  CALL("removalOrdLitVar(const Symbol& sym)");
  return _index.removalOrdLitVar(sym);
} // bool BackwardSubsumptionWithPathIndexingAndJoins<..>::removalOrdLitVar(const Symbol& sym)



template <class Alloc,class Term,class ClauseId,ulong MaxTermDepth,ulong MaxTermSize,ulong MaxNumOfVariables>
bool BackwardSubsumptionWithPathIndexingAndJoins<Alloc,Term,ClauseId,MaxTermDepth,MaxTermSize,MaxNumOfVariables>::removalSymmetricLit(const Symbol& header)
{
  CALL("removalSymmetricLit(const Symbol& header)");
  return _index.removalSymmetricLit(header);
} // bool BackwardSubsumptionWithPathIndexingAndJoins<..>::removalSymmetricLit(const Symbol& header)

template <class Alloc,class Term,class ClauseId,ulong MaxTermDepth,ulong MaxTermSize,ulong MaxNumOfVariables>
bool BackwardSubsumptionWithPathIndexingAndJoins<Alloc,Term,ClauseId,MaxTermDepth,MaxTermSize,MaxNumOfVariables>::removalSymLitSecondArg()
{
  CALL("removalSymLitSecondArg()");
  return _index.removalSymLitSecondArg();
} // bool BackwardSubsumptionWithPathIndexingAndJoins<..>::removalSymLitSecondArg()

template <class Alloc,class Term,class ClauseId,ulong MaxTermDepth,ulong MaxTermSize,ulong MaxNumOfVariables>
bool BackwardSubsumptionWithPathIndexingAndJoins<Alloc,Term,ClauseId,MaxTermDepth,MaxTermSize,MaxNumOfVariables>::removalSymLitNonconstFunc(const Symbol& sym)
{
  CALL("removalSymLitNonconstFunc(const Symbol& sym)");
  return _index.removalSymLitNonconstFunc(sym);
} // bool BackwardSubsumptionWithPathIndexingAndJoins<..>::removalSymLitNonconstFunc(const Symbol& sym)

template <class Alloc,class Term,class ClauseId,ulong MaxTermDepth,ulong MaxTermSize,ulong MaxNumOfVariables>
bool BackwardSubsumptionWithPathIndexingAndJoins<Alloc,Term,ClauseId,MaxTermDepth,MaxTermSize,MaxNumOfVariables>::removalSymLitConst(const Symbol& sym)
{
  CALL("removalSymLitConst(const Symbol& sym)");
  return _index.removalSymLitConst(sym);
} // bool BackwardSubsumptionWithPathIndexingAndJoins<..>::removalSymLitConst(const Symbol& sym)


template <class Alloc,class Term,class ClauseId,ulong MaxTermDepth,ulong MaxTermSize,ulong MaxNumOfVariables>
bool BackwardSubsumptionWithPathIndexingAndJoins<Alloc,Term,ClauseId,MaxTermDepth,MaxTermSize,MaxNumOfVariables>::removalSymLitVar(const Symbol& sym)
{
  CALL("removalSymLitVar(const Symbol& sym)");
  return _index.removalSymLitVar(sym);
} // bool BackwardSubsumptionWithPathIndexingAndJoins<..>::removalSymLitVar(const Symbol& sym)

template <class Alloc,class Term,class ClauseId,ulong MaxTermDepth,ulong MaxTermSize,ulong MaxNumOfVariables>
bool BackwardSubsumptionWithPathIndexingAndJoins<Alloc,Term,ClauseId,MaxTermDepth,MaxTermSize,MaxNumOfVariables>::removalEndOfClause()
{
  CALL("removalEndOfClause()");
  return _index.removalEndOfClause();
} // bool BackwardSubsumptionWithPathIndexingAndJoins<..>::removalEndOfClause()



template <class Alloc,class Term,class ClauseId,ulong MaxTermDepth,ulong MaxTermSize,ulong MaxNumOfVariables>
bool BackwardSubsumptionWithPathIndexingAndJoins<Alloc,Term,ClauseId,MaxTermDepth,MaxTermSize,MaxNumOfVariables>::queryReset()
{
  CALL("queryReset()");
  return true;
} // bool BackwardSubsumptionWithPathIndexingAndJoins<..>::queryReset()

template <class Alloc,class Term,class ClauseId,ulong MaxTermDepth,ulong MaxTermSize,ulong MaxNumOfVariables>
bool BackwardSubsumptionWithPathIndexingAndJoins<Alloc,Term,ClauseId,MaxTermDepth,MaxTermSize,MaxNumOfVariables>::queryOrdinaryNonpropLit(const Symbol& header)
{
  CALL("queryOrdinaryNonpropLit(const Symbol& header)");
  return true; // temporary
} // bool BackwardSubsumptionWithPathIndexingAndJoins<..>::queryOrdinaryNonpropLit(const Symbol& header)


template <class Alloc,class Term,class ClauseId,ulong MaxTermDepth,ulong MaxTermSize,ulong MaxNumOfVariables>
bool BackwardSubsumptionWithPathIndexingAndJoins<Alloc,Term,ClauseId,MaxTermDepth,MaxTermSize,MaxNumOfVariables>::queryPropLit(const Symbol& lit)
{
  CALL("queryPropLit(const Symbol& lit)");
  return true; // temporary
} // bool BackwardSubsumptionWithPathIndexingAndJoins<..>::queryPropLit(const Symbol& lit)



template <class Alloc,class Term,class ClauseId,ulong MaxTermDepth,ulong MaxTermSize,ulong MaxNumOfVariables>
bool BackwardSubsumptionWithPathIndexingAndJoins<Alloc,Term,ClauseId,MaxTermDepth,MaxTermSize,MaxNumOfVariables>::queryOrdLitNonconstFunc(const Symbol& sym)
{
  CALL("queryOrdLitNonconstFunc(const Symbol& sym)");
  return true; // temporary
} // bool BackwardSubsumptionWithPathIndexingAndJoins<..>::queryOrdLitNonconstFunc(const Symbol& sym)

template <class Alloc,class Term,class ClauseId,ulong MaxTermDepth,ulong MaxTermSize,ulong MaxNumOfVariables>
bool BackwardSubsumptionWithPathIndexingAndJoins<Alloc,Term,ClauseId,MaxTermDepth,MaxTermSize,MaxNumOfVariables>::queryOrdLitConst(const Symbol& sym)
{
  CALL("queryOrdLitConst(const Symbol& sym)");
  return true; // temporary
} // bool BackwardSubsumptionWithPathIndexingAndJoins<..>::queryOrdLitConst(const Symbol& sym)


template <class Alloc,class Term,class ClauseId,ulong MaxTermDepth,ulong MaxTermSize,ulong MaxNumOfVariables>
bool BackwardSubsumptionWithPathIndexingAndJoins<Alloc,Term,ClauseId,MaxTermDepth,MaxTermSize,MaxNumOfVariables>::queryOrdLitVar(const Symbol& sym)
{
  CALL("queryOrdLitVar(const Symbol& sym)");
  return true; // temporary
} // bool BackwardSubsumptionWithPathIndexingAndJoins<..>::queryOrdLitVar(const Symbol& sym)



template <class Alloc,class Term,class ClauseId,ulong MaxTermDepth,ulong MaxTermSize,ulong MaxNumOfVariables>
bool BackwardSubsumptionWithPathIndexingAndJoins<Alloc,Term,ClauseId,MaxTermDepth,MaxTermSize,MaxNumOfVariables>::querySymmetricLit(const Symbol& header)
{
  CALL("querySymmetricLit(const Symbol& header)");
  return true; // temporary
} // bool BackwardSubsumptionWithPathIndexingAndJoins<..>::querySymmetricLit(const Symbol& header)

template <class Alloc,class Term,class ClauseId,ulong MaxTermDepth,ulong MaxTermSize,ulong MaxNumOfVariables>
bool BackwardSubsumptionWithPathIndexingAndJoins<Alloc,Term,ClauseId,MaxTermDepth,MaxTermSize,MaxNumOfVariables>::querySymLitSecondArg()
{
  CALL("querySymLitSecondArg()");
  return true; // temporary
} // bool BackwardSubsumptionWithPathIndexingAndJoins<..>::querySymLitSecondArg()

template <class Alloc,class Term,class ClauseId,ulong MaxTermDepth,ulong MaxTermSize,ulong MaxNumOfVariables>
bool BackwardSubsumptionWithPathIndexingAndJoins<Alloc,Term,ClauseId,MaxTermDepth,MaxTermSize,MaxNumOfVariables>::querySymLitNonconstFunc(const Symbol& sym)
{
  CALL("querySymLitNonconstFunc(const Symbol& sym)");
  return true; // temporary
} // bool BackwardSubsumptionWithPathIndexingAndJoins<..>::querySymLitNonconstFunc(const Symbol& sym)

template <class Alloc,class Term,class ClauseId,ulong MaxTermDepth,ulong MaxTermSize,ulong MaxNumOfVariables>
bool BackwardSubsumptionWithPathIndexingAndJoins<Alloc,Term,ClauseId,MaxTermDepth,MaxTermSize,MaxNumOfVariables>::querySymLitConst(const Symbol& sym)
{
  CALL("querySymLitConst(const Symbol& sym)");
  return true; // temporary
} // bool BackwardSubsumptionWithPathIndexingAndJoins<..>::querySymLitConst(const Symbol& sym)


template <class Alloc,class Term,class ClauseId,ulong MaxTermDepth,ulong MaxTermSize,ulong MaxNumOfVariables>
bool BackwardSubsumptionWithPathIndexingAndJoins<Alloc,Term,ClauseId,MaxTermDepth,MaxTermSize,MaxNumOfVariables>::querySymLitVar(const Symbol& sym)
{
  CALL("querySymLitVar(const Symbol& sym)");
  return true; // temporary
} // bool BackwardSubsumptionWithPathIndexingAndJoins<..>::querySymLitVar(const Symbol& sym)

template <class Alloc,class Term,class ClauseId,ulong MaxTermDepth,ulong MaxTermSize,ulong MaxNumOfVariables>
bool BackwardSubsumptionWithPathIndexingAndJoins<Alloc,Term,ClauseId,MaxTermDepth,MaxTermSize,MaxNumOfVariables>::queryEndOfClause()
{
  CALL("queryEndOfClause()");
  return true; // temporary
} // bool BackwardSubsumptionWithPathIndexingAndJoins<..>::queryEndOfClause()




}; // namespace Gem



//============================================================================
#endif
