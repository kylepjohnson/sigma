//
// File:         InstanceRetrievalForDemodulation.cpp
// Description:  Indexing for matching in backward demodulation.
// Created:      May 09, 2002, Alexandre Riazanov, riazanov@cs.man.ac.uk
//====================================================
#include "InstanceRetrievalForDemodulation.hpp"
//====================================================
#undef DEBUG_NAMESPACE
#ifdef DEBUG_INSTANCE_RETRIEVAL_FOR_DEMODULATION
 #define DEBUG_NAMESPACE "InstRet"
#endif
#include "debugMacros.hpp"
//====================================================
using namespace BK;
using namespace Gem;
using namespace VK;

#ifdef DEBUG_ALLOC_OBJ_TYPE

ClassDesc InstRet::Index::NonmaxPathRelation::_classDesc("InstRet::Index::NonmaxPathRelation",
										  InstRet::Index::NonmaxPathRelation::minSize(),
										  InstRet::Index::NonmaxPathRelation::maxSize());

ClassDesc InstRet::Index::NonmaxPathRelation::_headerClassDesc("InstRet::Index::NonmaxPathRelation(header)",
											InstRet::Index::NonmaxPathRelation::headerMinSize(),
											InstRet::Index::NonmaxPathRelation::headerMaxSize());



ClassDesc InstRet::Index::NonmaxPathRelation::Node::_classDesc("InstRet::Index::NonmaxPathRelation::Node",
											InstRet::Index::NonmaxPathRelation::Node::minSize(),
											InstRet::Index::NonmaxPathRelation::Node::maxSize());





ClassDesc InstRet::Index::MaxPathRelation::_classDesc("InstRet::Index::MaxPathRelation",
									       InstRet::Index::MaxPathRelation::minSize(),
									       InstRet::Index::MaxPathRelation::maxSize());

ClassDesc InstRet::Index::MaxPathRelation::_headerClassDesc("InstRet::Index::MaxPathRelation(header)",
										     InstRet::Index::MaxPathRelation::headerMinSize(),
										     InstRet::Index::MaxPathRelation::headerMaxSize());



ClassDesc InstRet::Index::MaxPathRelation::Node::_classDesc("InstRet::Index::MaxPathRelation::Node",
										     InstRet::Index::MaxPathRelation::Node::minSize(),
										     InstRet::Index::MaxPathRelation::Node::maxSize());





ClassDesc InstRet::Index::PathIndexForInstanceRetrieval::Node::_funcClassDesc(
												       "InstRet::Index::PathIndexForInstanceRetrieval::Node(func)",
												       InstRet::Index::PathIndexForInstanceRetrieval::Node::funcMinSize(),
												       InstRet::Index::PathIndexForInstanceRetrieval::Node::funcMaxSize());

ClassDesc InstRet::Index::PathIndexForInstanceRetrieval::Node::_constClassDesc(
													"InstRet::Index::PathIndexForInstanceRetrieval::Node(const)",
													InstRet::Index::PathIndexForInstanceRetrieval::Node::constMinSize(),
													InstRet::Index::PathIndexForInstanceRetrieval::Node::constMaxSize());


#endif



void InstRet::integrate(const TERM* complexTerm)
{
  CALL("integrate(const TERM* complexTerm)");
  _index.integrate(TERM(const_cast<TERM*>(complexTerm)));
}; 
void InstRet::remove(const TERM* complexTerm)
{
  CALL("remove(const TERM* complexTerm)");
  _index.remove(TERM(const_cast<TERM*>(complexTerm)));
}; 

bool InstRet::newQuery(const TERM* query)
{
  CALL("newQuery(const TERM* query)");
  _noMoreMatches = !_index.compileQuery(TERM(const_cast<TERM*>(query)));
  DOP(_debugQuery = query);
  return !_noMoreMatches;
}; 
bool InstRet::nextMatch(const TERM*& match)
{
  CALL("nextMatch(const TERM*& match)");
  if (_noMoreMatches) return false;
  TERM matchId;
  bool result = _index.nextMatch(matchId);
  if (result)
    {
      ASSERT(matchId.IsReference());
      match = matchId.First();
      ASSERT(isValidMatch(match));
      return true;
    };
  _noMoreMatches = true;
  return false;
}; 


#ifdef DEBUG_NAMESPACE
bool InstRet::isValidMatch(const TERM* term) const
{
  static Substitution::Instance queryInst;
  static TERM::Iterator matchIter; 
  queryInst.reset(_debugQuery,substitution());
  matchIter.Reset(term);
  do
    {
      if (queryInst.symbol() != matchIter.symbolRef()) return false;
      matchIter.Next();
    }
  while (queryInst.next());
  return true;
}; // bool InstRet::isValidMatch(const TERM* term) const

#endif

//====================================================
