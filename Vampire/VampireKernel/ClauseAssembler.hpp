//
// File:         ClauseAssembler.hpp
// Description:  Implements clause sharing.
// Created:      Dec 13, 2001 
// Author:       Alexandre Riazanov
// mail:         riazanov@cs.man.ac.uk
// Note:         Was CLAUSE_INDEX in clause_index.h.
// Note:         Integrate and Remove methods need a revision,
//               they became very strange after several modifications.
//============================================================================
#ifndef CLAUSE_ASSEMBLER_H
//=============================================================================
#define CLAUSE_ASSEMBLER_H 
#include "jargon.hpp"
#include "VampireKernelDebugFlags.hpp"
#include "LiteralSharing.hpp"
#include "variables.hpp"
namespace VK
{

class SHARING_INDEX;
class DTREE_NODE;
class LiteralSelection;
class TmpLiteral;
class TmpLitList;
class Clause;
class OpenClauseBackground;
class PagedPassiveClauseStorage;
class SymbolInfo;
}; // namespace VK

//============================================================================
#undef DEBUG_NAMESPACE
#ifdef DEBUG_CLAUSE_ASSEMBLER
#define DEBUG_NAMESPACE "ClauseAssembler"
#endif
#include "debugMacros.hpp"
//============================================================================

namespace VK
{
class ClauseAssembler
{
 public:
  ClauseAssembler(SHARING_INDEX* si,const LiteralSelection* litSelection);
  ~ClauseAssembler() { CALL("destructor ~ClauseAssembler()"); };
  void init(SHARING_INDEX* si,const LiteralSelection* litSelection);
  void destroy();
  void reset();

  void setInheritNegativeSelection(bool flag) { _inheritNegativeSelection = flag; };
  void setEqualityIsPossibleFlag(bool flag) 
    {
      _equalityIsPossible = flag;
    };
  void setEliminatedPredicateTable(const SymbolInfo* tbl)
    {
      _eliminatedPredicateTable = tbl;
    };

  void setNongoalPenaltyCoefficient(float coeff)
    {
      _nongoalPenaltyCoefficient = coeff;
      if (_nongoalPenaltyCoefficient < 1.0) _nongoalPenaltyCoefficient = 1.0;
      _useNongoalPenaltyCoefficient = (_nongoalPenaltyCoefficient != 1.0);
    };

  Clause* assembleEmptyClause(OpenClauseBackground& ancestors) const;
  Clause* assembleUnitClause(TmpLiteral* lit,OpenClauseBackground& ancestors);
  Clause* assembleClauseWithEvthngSlctd(TmpLitList& lits,OpenClauseBackground& ancestors);
   
  Clause* assembleClause(TmpLitList& lits,OpenClauseBackground& ancestors);
 
  Clause* assembleUnsharedClause(TmpLitList& lits,OpenClauseBackground& ancestors);
 
  bool assembleClauseInPagedPassiveClauseStorage(PagedPassiveClauseStorage& storage,TmpLitList& lits,OpenClauseBackground& ancestors,long clauseNum,bool showNewClause,bool markAsName);
   

  //   void Integrate(Clause* cl);
  void removeFromSharing(Clause* cl);
   



 private:
   
  DTREE_NODE** Tree(const TERM& header) { return _litSharing.Tree(header); };
 

  TERM* shareTmpLiteral(const TmpLiteral* lit,Clause* cl);

 
  TERM* assembleUnsharedLit(const TmpLiteral& lit);

  void writeIntoPagedPassiveClauseStorage(PagedPassiveClauseStorage& storage,TmpLitList& lits);  

  void writeIntoPagedPassiveClauseStorage(PagedPassiveClauseStorage& storage,TmpLiteral* lit);
 
  TERM* Integrate(const Flatterm* lit,Clause* cl);
  TERM* IntegrateEq(const TERM& header,const Flatterm* arg1,const Flatterm* arg2,Clause* cl);

  void removeFromSharing(TERM* lit);

 private:
  SHARING_INDEX* _sharingIndex;
  LiteralSharing _litSharing;
  INT_VAR_RENAMING _intVarRenaming;
  VAR_WEIGHTING _varWeighting;
  TERM::FastOpenUnshared _openUnsharedTerm;
  const LiteralSelection* _litSelection;
  bool _inheritNegativeSelection;
  bool _equalityIsPossible;
  const SymbolInfo* _eliminatedPredicateTable;
  float _nongoalPenaltyCoefficient;
  bool _useNongoalPenaltyCoefficient;
}; // class ClauseAssembler

}; // namespace VK

//============================================================================
#endif
