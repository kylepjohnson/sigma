//
// File:         Parser.hpp
// Description:  TPTP parser front end.
// Created:      Jun 04, 2002, Alexandre Riazanov, riazanov@cs.man.ac.uk
//======================================================================
#ifndef PARSER_H
#define PARSER_H
//======================================================================
#include <cstring>
#include "jargon.hpp"
#include "VampireKernelDebugFlags.hpp"
#include "DestructionMode.hpp"
#include "Input.hpp"
#include "pr_abstract_syntax.hpp"
#include "pr_parser.hpp"
//======================================================================
#undef DEBUG_NAMESPACE
#ifdef DEBUG_PARSER
 #define DEBUG_NAMESPACE "Parser"
#endif
#include "debugMacros.hpp"
//======================================================================

namespace VK
{
class Parser
{
 public:
  Parser() :
    _clauses(0)
    {
    };
  
  ~Parser() 
    {
      if (BK::DestructionMode::isThorough())
	{
	  Input::destroyClauseList(_clauses);
	};
    };
  
  bool parseFile(const char* tptpDir,const char* fileName);
  const Input::ClauseList* clauses() const { return _clauses; };
  Input::Signature& signature() { return _signature; };

 private:
  enum ItemSort
  {
    TPTPIncludeDirective,
    TPTPInputClause,
    TPTPInputFormula,
    TPTPExtensionSymbolPrecedence,
    TPTPExtensionSymbolWeight,
    TPTPExtensionSymbolInterpretation,
    TPTPExtensionAnswerPredicateDeclaration,
    UnknownItemSort
  };

  enum ItemType
  {
    TPTPAxiom,
    TPTPConjecture,
    TPTPHypothesis,
    UnknownItemType
  };
  
 private:    
  bool processInputItem(PR_LITERAL* item,
			const char* tptpDir,
			const char* fileName);

  static ItemSort itemSort(PR_LITERAL* item);
  static ItemType itemType(PR_TERM* it);
  bool readClause(PR_LITERAL* item);
  bool readLiterals(PR_TERM_LIST* lst,Input::LiteralList*& result);
  bool readLiteral(PR_TERM* lit,Input::Literal*& result);
  bool readAtom(PR_TERM* at,Input::Term*& result);
  bool readTerms(PR_TERM_LIST* terms,Input::TermList*& result);
  bool readTerm(PR_TERM* term,Input::Term*& result);
  bool readSymbolPrecedence(PR_LITERAL* item);
  bool readSymbolWeight(PR_LITERAL* item);
  bool readSymbolInterpretation(PR_LITERAL* item);
  bool readAnswerPredicateDeclaration(PR_LITERAL* item);
 private:
  Input::Signature _signature;
  Input::ClauseList* _clauses;
}; // class Parser

}; // namespace VK


//======================================================================
#endif
