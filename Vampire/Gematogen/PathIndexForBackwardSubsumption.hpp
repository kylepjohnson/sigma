//
// File:         PathIndexForBackwardSubsumption.hpp
// Description:  Maintaining path indexing with joins 
//               for backward subsumption.
// Created:      Apr 10, 2002, Alexandre Riazanov, riazanov@cs.man.ac.uk
// Note:         This file is a part of the Gematogen library.
//============================================================================
#ifndef PATH_INDEX_FOR_BACKWARD_SUBSUMPTION_H
#define PATH_INDEX_FOR_BACKWARD_SUBSUMPTION_H
//============================================================================
#ifndef NO_DEBUG_VIS
 #include <iostream>
#endif 
#include "jargon.hpp"
#include "Gematogen.hpp"
#include "GematogenDebugFlags.hpp" 
#include "PathIndex.hpp"
#include "SmallHeaderSkipList.hpp"
#include "GList.hpp"
//============================================================================
#undef DEBUG_NAMESPACE
#ifdef DEBUG_PATH_INDEX_FOR_BACKWARD_SUBSUMPTION
#define DEBUG_NAMESPACE "PathIndexForBackwardSubsumption<..>"
#endif
#include "debugMacros.hpp"
//============================================================================ 

namespace Gem
{
template <class Alloc,class Term,class ClauseId,ulong MaxTermDepth,ulong MaxTermSize>
class PathIndexForBackwardSubsumption
{
 public:
  typedef typename Term::Symbol Symbol;
  typedef typename Term::Traversal TermTraversal;
  typedef typename Term::Arguments TermArguments;

 public:
  PathIndexForBackwardSubsumption() : 
    _trieForOrdinaryLits(),
    _trieForSymmetricLits()
    {
    };
  ~PathIndexForBackwardSubsumption() {};
  // Integration 

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

  // Removal

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

 private:
  typedef long LiteralNumber;
  class LiteralNumberAndSubterms
    {
    public:
      LiteralNumberAndSubterms(long ln,const TermArguments& sbts) : 
	literalNumber(ln),
	subterms(sbts)
	{
	};
      ~LiteralNumberAndSubterms() {};
    public:
      long literalNumber;
      TermArguments subterms;
    };
  class LitSubtListId {};
  typedef GList<Alloc,LiteralNumberAndSubterms,LitSubtListId> LitSubtList;
  class OrdLitNonmaxPathRelationId {};
  class OrdLitMaxPathRelationId {};
  class SymLitNonmaxPathRelationId {};
  class SymLitMaxPathRelationId {};
  typedef SHSkList<Alloc,LitSubtList*,ClauseId,2UL,OrdLitNonmaxPathRelationId> OrdLitNonmaxPathRelation;
  typedef SHSkList<Alloc,LiteralNumber,ClauseId,2UL,OrdLitMaxPathRelationId> OrdLitMaxPathRelation;
  typedef SHSkList<Alloc,LitSubtList*,ClauseId,2UL,SymLitNonmaxPathRelationId> SymLitNonmaxPathRelation;
  typedef SHSkList<Alloc,LiteralNumber,ClauseId,2UL,SymLitMaxPathRelationId> SymLitMaxPathRelation;

  typedef PathIndex<Alloc,Symbol,OrdLitNonmaxPathRelation,OrdLitMaxPathRelation,MaxTermDepth,MaxTermSize> 
    PathIndexForOrdinaryLits;
  typedef PathIndex<Alloc,Symbol,SymLitNonmaxPathRelation,SymLitMaxPathRelation,MaxTermDepth,MaxTermSize> 
    PathIndexForSymmetricLits; 

 private:

  PathIndexForOrdinaryLits::Trie _trieForOrdinaryLits;
  PathIndexForSymmetricLits::Trie _trieForSymmetricLits;
}; // class PathIndexForBackwardSubsumption<..>

}; // namespace Gem

//============================================================================
#undef DEBUG_NAMESPACE
#ifdef DEBUG_PATH_INDEX_FOR_BACKWARD_SUBSUMPTION
#define DEBUG_NAMESPACE "PathIndexForBackwardSubsumption<..>"
#endif
#include "debugMacros.hpp"
//============================================================================ 


namespace Gem
{

template <class Alloc,class Term,class ClauseId,ulong MaxTermDepth,ulong MaxTermSize>
bool PathIndexForBackwardSubsumption<Alloc,Term,ClauseId,MaxTermDepth,MaxTermSize>::integrationReset(const ClauseId& clauseId)
{
  CALL("integrationReset(const ClauseId& clauseId)");
  return true; // temporary
} // bool PathIndexForBackwardSubsumption<..>::integrationReset(const ClauseId& clauseId)


template <class Alloc,class Term,class ClauseId,ulong MaxTermDepth,ulong MaxTermSize>
bool PathIndexForBackwardSubsumption<Alloc,Term,ClauseId,MaxTermDepth,MaxTermSize>::integrationOrdinaryNonpropLit(const Symbol& header)
{
  CALL("integrationOrdinaryNonpropLit(const Symbol& header)");
  return true; // temporary
} // bool PathIndexForBackwardSubsumption<..>::integrationOrdinaryNonpropLit(const Symbol& header)


template <class Alloc,class Term,class ClauseId,ulong MaxTermDepth,ulong MaxTermSize>
bool PathIndexForBackwardSubsumption<Alloc,Term,ClauseId,MaxTermDepth,MaxTermSize>::integrationPropLit(const Symbol& lit)
{
  CALL("integrationPropLit(const Symbol& lit)");
  return true; // temporary
} // bool PathIndexForBackwardSubsumption<..>::integrationPropLit(const Symbol& lit)



template <class Alloc,class Term,class ClauseId,ulong MaxTermDepth,ulong MaxTermSize>
bool PathIndexForBackwardSubsumption<Alloc,Term,ClauseId,MaxTermDepth,MaxTermSize>::integrationOrdLitNonconstFunc(const Symbol& sym)
{
  CALL("integrationOrdLitNonconstFunc(const Symbol& sym)");
  return true; // temporary
} // bool PathIndexForBackwardSubsumption<..>::integrationOrdLitNonconstFunc(const Symbol& sym)

template <class Alloc,class Term,class ClauseId,ulong MaxTermDepth,ulong MaxTermSize>
bool PathIndexForBackwardSubsumption<Alloc,Term,ClauseId,MaxTermDepth,MaxTermSize>::integrationOrdLitConst(const Symbol& sym)
{
  CALL("integrationOrdLitConst(const Symbol& sym)");
  return true; // temporary
} // bool PathIndexForBackwardSubsumption<..>::integrationOrdLitConst(const Symbol& sym)


template <class Alloc,class Term,class ClauseId,ulong MaxTermDepth,ulong MaxTermSize>
bool PathIndexForBackwardSubsumption<Alloc,Term,ClauseId,MaxTermDepth,MaxTermSize>::integrationOrdLitVar(const Symbol& sym)
{
  CALL("integrationOrdLitVar(const Symbol& sym)");
  return true; // temporary
} // bool PathIndexForBackwardSubsumption<..>::integrationOrdLitVar(const Symbol& sym)



template <class Alloc,class Term,class ClauseId,ulong MaxTermDepth,ulong MaxTermSize>
bool PathIndexForBackwardSubsumption<Alloc,Term,ClauseId,MaxTermDepth,MaxTermSize>::integrationSymmetricLit(const Symbol& header)
{
  CALL("integrationSymmetricLit(const Symbol& header)");
  return true; // temporary
} // bool PathIndexForBackwardSubsumption<..>::integrationSymmetricLit(const Symbol& header)

template <class Alloc,class Term,class ClauseId,ulong MaxTermDepth,ulong MaxTermSize>
bool PathIndexForBackwardSubsumption<Alloc,Term,ClauseId,MaxTermDepth,MaxTermSize>::integrationSymLitSecondArg()
{
  CALL("integrationSymLitSecondArg()");
  return true; // temporary
} // bool PathIndexForBackwardSubsumption<..>::integrationSymLitSecondArg()

template <class Alloc,class Term,class ClauseId,ulong MaxTermDepth,ulong MaxTermSize>
bool PathIndexForBackwardSubsumption<Alloc,Term,ClauseId,MaxTermDepth,MaxTermSize>::integrationSymLitNonconstFunc(const Symbol& sym)
{
  CALL("integrationSymLitNonconstFunc(const Symbol& sym)");
  return true; // temporary
} // bool PathIndexForBackwardSubsumption<..>::integrationSymLitNonconstFunc(const Symbol& sym)

template <class Alloc,class Term,class ClauseId,ulong MaxTermDepth,ulong MaxTermSize>
bool PathIndexForBackwardSubsumption<Alloc,Term,ClauseId,MaxTermDepth,MaxTermSize>::integrationSymLitConst(const Symbol& sym)
{
  CALL("integrationSymLitConst(const Symbol& sym)");
  return true; // temporary
} // bool PathIndexForBackwardSubsumption<..>::integrationSymLitConst(const Symbol& sym)


template <class Alloc,class Term,class ClauseId,ulong MaxTermDepth,ulong MaxTermSize>
bool PathIndexForBackwardSubsumption<Alloc,Term,ClauseId,MaxTermDepth,MaxTermSize>::integrationSymLitVar(const Symbol& sym)
{
  CALL("integrationSymLitVar(const Symbol& sym)");
  return true; // temporary
} // bool PathIndexForBackwardSubsumption<..>::integrationSymLitVar(const Symbol& sym)


template <class Alloc,class Term,class ClauseId,ulong MaxTermDepth,ulong MaxTermSize>
bool PathIndexForBackwardSubsumption<Alloc,Term,ClauseId,MaxTermDepth,MaxTermSize>::integrationEndOfClause()
{
  CALL("integrationEndOfClause()");
  return true; // temporary
} // bool PathIndexForBackwardSubsumption<..>::integrationEndOfClause()




template <class Alloc,class Term,class ClauseId,ulong MaxTermDepth,ulong MaxTermSize>
bool PathIndexForBackwardSubsumption<Alloc,Term,ClauseId,MaxTermDepth,MaxTermSize>::removalReset(const ClauseId& clauseId)
{
  CALL("removalReset(const ClauseId& clauseId)");
  return true; // temporary
} // bool PathIndexForBackwardSubsumption<..>::removalReset(const ClauseId& clauseId)



template <class Alloc,class Term,class ClauseId,ulong MaxTermDepth,ulong MaxTermSize>
bool PathIndexForBackwardSubsumption<Alloc,Term,ClauseId,MaxTermDepth,MaxTermSize>::removalOrdinaryNonpropLit(const Symbol& header)
{
  CALL("removalOrdinaryNonpropLit(const Symbol& header)");
  return true; // temporary
} // bool PathIndexForBackwardSubsumption<..>::removalOrdinaryNonpropLit(const Symbol& header)


template <class Alloc,class Term,class ClauseId,ulong MaxTermDepth,ulong MaxTermSize>
bool PathIndexForBackwardSubsumption<Alloc,Term,ClauseId,MaxTermDepth,MaxTermSize>::removalPropLit(const Symbol& lit)
{
  CALL("removalPropLit(const Symbol& lit)");
  return true; // temporary
} // bool PathIndexForBackwardSubsumption<..>::removalPropLit(const Symbol& lit)



template <class Alloc,class Term,class ClauseId,ulong MaxTermDepth,ulong MaxTermSize>
bool PathIndexForBackwardSubsumption<Alloc,Term,ClauseId,MaxTermDepth,MaxTermSize>::removalOrdLitNonconstFunc(const Symbol& sym)
{
  CALL("removalOrdLitNonconstFunc(const Symbol& sym)");
  return true; // temporary
} // bool PathIndexForBackwardSubsumption<..>::removalOrdLitNonconstFunc(const Symbol& sym)

template <class Alloc,class Term,class ClauseId,ulong MaxTermDepth,ulong MaxTermSize>
bool PathIndexForBackwardSubsumption<Alloc,Term,ClauseId,MaxTermDepth,MaxTermSize>::removalOrdLitConst(const Symbol& sym)
{
  CALL("removalOrdLitConst(const Symbol& sym)");
  return true; // temporary
} // bool PathIndexForBackwardSubsumption<..>::removalOrdLitConst(const Symbol& sym)


template <class Alloc,class Term,class ClauseId,ulong MaxTermDepth,ulong MaxTermSize>
bool PathIndexForBackwardSubsumption<Alloc,Term,ClauseId,MaxTermDepth,MaxTermSize>::removalOrdLitVar(const Symbol& sym)
{
  CALL("removalOrdLitVar(const Symbol& sym)");
  return true; // temporary
} // bool PathIndexForBackwardSubsumption<..>::removalOrdLitVar(const Symbol& sym)



template <class Alloc,class Term,class ClauseId,ulong MaxTermDepth,ulong MaxTermSize>
bool PathIndexForBackwardSubsumption<Alloc,Term,ClauseId,MaxTermDepth,MaxTermSize>::removalSymmetricLit(const Symbol& header)
{
  CALL("removalSymmetricLit(const Symbol& header)");
  return true; // temporary
} // bool PathIndexForBackwardSubsumption<..>::removalSymmetricLit(const Symbol& header)

template <class Alloc,class Term,class ClauseId,ulong MaxTermDepth,ulong MaxTermSize>
bool PathIndexForBackwardSubsumption<Alloc,Term,ClauseId,MaxTermDepth,MaxTermSize>::removalSymLitSecondArg()
{
  CALL("removalSymLitSecondArg()");
  return true; // temporary
} // bool PathIndexForBackwardSubsumption<..>::removalSymLitSecondArg()

template <class Alloc,class Term,class ClauseId,ulong MaxTermDepth,ulong MaxTermSize>
bool PathIndexForBackwardSubsumption<Alloc,Term,ClauseId,MaxTermDepth,MaxTermSize>::removalSymLitNonconstFunc(const Symbol& sym)
{
  CALL("removalSymLitNonconstFunc(const Symbol& sym)");
  return true; // temporary
} // bool PathIndexForBackwardSubsumption<..>::removalSymLitNonconstFunc(const Symbol& sym)

template <class Alloc,class Term,class ClauseId,ulong MaxTermDepth,ulong MaxTermSize>
bool PathIndexForBackwardSubsumption<Alloc,Term,ClauseId,MaxTermDepth,MaxTermSize>::removalSymLitConst(const Symbol& sym)
{
  CALL("removalSymLitConst(const Symbol& sym)");
  return true; // temporary
} // bool PathIndexForBackwardSubsumption<..>::removalSymLitConst(const Symbol& sym)


template <class Alloc,class Term,class ClauseId,ulong MaxTermDepth,ulong MaxTermSize>
bool PathIndexForBackwardSubsumption<Alloc,Term,ClauseId,MaxTermDepth,MaxTermSize>::removalSymLitVar(const Symbol& sym)
{
  CALL("removalSymLitVar(const Symbol& sym)");
  return true; // temporary
} // bool PathIndexForBackwardSubsumption<..>::removalSymLitVar(const Symbol& sym)

template <class Alloc,class Term,class ClauseId,ulong MaxTermDepth,ulong MaxTermSize>
bool PathIndexForBackwardSubsumption<Alloc,Term,ClauseId,MaxTermDepth,MaxTermSize>::removalEndOfClause()
{
  CALL("removalEndOfClause()");
  return true; // temporary
} // bool PathIndexForBackwardSubsumption<..>::removalEndOfClause()


}; // namespace Gem






//============================================================================
#endif
