//
// File:         Parser.cpp
// Description:  TPTP parser front end.
// Created:      Jun 04, 2002, Alexandre Riazanov, riazanov@cs.man.ac.uk
//======================================================================
#include <cstdlib>
#include <iostream>
#include "Malloc.hpp"
#include "Parser.hpp"
using namespace std;
using namespace BK;
using namespace VK;
//======================================================================
#undef DEBUG_NAMESPACE
#ifdef DEBUG_PARSER
 #define DEBUG_NAMESPACE "Parser"
#endif
#include "debugMacros.hpp"
//======================================================================


bool Parser::parseFile(const char* tptpDir,const char* fileName)
{
  CALL("parseFile(const char* tptpDir,const char* fileName)");
  PR_FILE_PARS_RESULT* parsRes = new PR_FILE_PARS_RESULT();  
  ::ParseFile(fileName,parsRes);
  if (parsRes->Error())
    {      
      cout << "Error in " << fileName << " ";
      cout << "[" << parsRes->ErrorLineNum() << "/" << parsRes->ErrorCharNum() << "] : ";
      cout << parsRes->ErrorMessage() << "\n";
      return false;
    };


 for (PR_LITERAL_LIST* lst = parsRes->Literals(); lst; lst = lst->Next())
  if (!(processInputItem(lst->Literal(),tptpDir,fileName))) return false;
  
 delete parsRes;
 return true;   
}; // bool Parser::parseFile(const char* tptpDir,const char* fileName)
  

bool Parser::processInputItem(PR_LITERAL* item,
			      const char* tptpDir,
			      const char* fileName)
{
  CALL("processInputItem(PR_LITERAL* item,..)");
  switch (itemSort(item))
    {
    case TPTPIncludeDirective:
      {
	if (PR_TERM_LIST::Length(item->Arguments()) != 1)
	  {
	    cout << "Error in " << fileName << " "
		 << "[" << item->Header()->LineNum() << "/" 
		 << item->Header()->CharNum() << "] : "
		 << "Wrong arity. " << "\n";
	    return false; 
	  };
	if (item->Arguments()->Term()->Tag() != PR_TERM::ATOM_STRING)
	  {
	    cout << "Error in " << fileName << " "
		 << "[" << item->Header()->LineNum() << "/" 
		 << item->Header()->CharNum() << "] : "
		 << "String constant expected. " << "\n";
	    return false;
	  };
      
	const char* includedFileName = item->Arguments()->Term()->Symbol()->Symbol();
	char* fullFileName = 
	  static_cast<char*>(Malloc::allocate(strlen(tptpDir) + strlen(includedFileName) + 2));
	strcpy(fullFileName,tptpDir);
	fullFileName[strlen(tptpDir)] = '/';
	strcpy(fullFileName + strlen(tptpDir) + 1,includedFileName);
	fullFileName[strlen(tptpDir)  + strlen(includedFileName) + 1] = (char)0;
	bool result = parseFile(tptpDir,fullFileName);
	Malloc::deallocate(static_cast<void*>(fullFileName),strlen(tptpDir) + strlen(includedFileName) + 2);
	return result;
      };

    case TPTPInputClause:      
      if (!readClause(item)) 
	{	  
	  cout << "Error in " << fileName << "\n"
	       << "[" << item->Header()->LineNum() << "/" 
	       << item->Header()->CharNum() << "] : "
	       << "Can not read an input clause. " << "\n";
	  return false;
	};
      return true;

    case TPTPInputFormula:
      cout << "Error in " << fileName << " "
	   << "[" << item->Header()->LineNum() << "/" 
	   << item->Header()->CharNum() << "] : "
	   << "Full first-order logic syntax is not supported.\n" 
	   << "Use a clausifier.\n";
      return false;


    case TPTPExtensionSymbolPrecedence:
      if (!readSymbolPrecedence(item))
	{
	  cout << "Error in " << fileName << " "
	       << "[" << item->Header()->LineNum() << "/" 
	       << item->Header()->CharNum() << "] : "
	       << "Can not read symbol precedence. " << "\n";
	  return false;
	};
      return true;

    case TPTPExtensionSymbolWeight:
      if (!readSymbolWeight(item))
	{
	  cout << "Error in " << fileName << " "
	       << "[" << item->Header()->LineNum() << "/" 
	       << item->Header()->CharNum() << "] : "
	       << "Can not read symbol weight. " << "\n";
	  return false;
	};
      return true;


    case TPTPExtensionSymbolInterpretation:
      if (!readSymbolInterpretation(item))
	{
	  cout << "Error in " << fileName << " "
	       << "[" << item->Header()->LineNum() << "/" 
	       << item->Header()->CharNum() << "] : "
	       << "Can not read symbol interpretation. " << "\n";
	  return false;
	};
      return true;


      
      
    case TPTPExtensionAnswerPredicateDeclaration:
      if (!readAnswerPredicateDeclaration(item))
	{
	  cout << "Error in " << fileName << " "
	       << "[" << item->Header()->LineNum() << "/" 
	       << item->Header()->CharNum() << "] : "
	       << "Can not read answer predicate declaration. " << "\n";
	  return false;
	};
      return true;

    default:      
      cout << "Error in " << fileName << " "
	   << "[" << item->Header()->LineNum() << "/" 
	   << item->Header()->CharNum() << "] : "
	   << "Unknown input item type. " << "\n";
      return false;
   };


}; // bool Parser::processInputItem(PR_LITERAL* item,..)


Parser::ItemSort Parser::itemSort(PR_LITERAL* item)
{
  CALL("itemSort(PR_LITERAL* item)");
  const char* top = item->Header()->Symbol();
  if (!(strcmp(top,"include"))) return TPTPIncludeDirective;
  if (!(strcmp(top,"input_clause"))) return TPTPInputClause;
  if (!(strcmp(top,"input_formula"))) return TPTPInputFormula;
  if (!(strcmp(top,"symbol_precedence"))) return TPTPExtensionSymbolPrecedence;
  if (!(strcmp(top,"symbol_weight"))) return TPTPExtensionSymbolWeight;
  if (!(strcmp(top,"interpret"))) return TPTPExtensionSymbolInterpretation;
  if (!(strcmp(top,"answer"))) return TPTPExtensionAnswerPredicateDeclaration;
  return UnknownItemSort;
}; // Parser::ItemSort Parser::itemSort(PR_LITERAL* item)

Parser::ItemType Parser::itemType(PR_TERM* it)
{
  CALL("itemType(PR_TERM* it)");  
  if (it->Tag() != PR_TERM::ATOM_FUNCTOR) return UnknownItemType;
  const char* str = it->Symbol()->Symbol(); 
  if (!(strcmp(str,"axiom"))) return TPTPAxiom;
  if (!(strcmp(str,"conjecture"))) return TPTPConjecture;
  if (!(strcmp(str,"hypothesis"))) return TPTPHypothesis;
  return UnknownItemType;
}; //  Parser::ItemType Parser::itemType(PR_TERM* it)



bool Parser::readClause(PR_LITERAL* item)
{
  CALL("readClause(PR_LITERAL* item)");
  ASSERT(itemSort(item) == TPTPInputClause);
  if (PR_TERM_LIST::Length(item->Arguments()) != 3) return false; 
  if (PR_TERM_LIST::Nth(item->Arguments(),0)->Tag() != PR_TERM::LIST) return false;
  if ((PR_TERM_LIST::Nth(item->Arguments(),2)->Tag() != PR_TERM::ATOM_FUNCTOR) && 
      (PR_TERM_LIST::Nth(item->Arguments(),2)->Tag() != PR_TERM::ATOM_NUMBER))
    return false;
      
  const char* clauseName = 
    PR_TERM_LIST::Nth(item->Arguments(),2)->Symbol()->Symbol();
    
  ItemType clauseType = itemType(PR_TERM_LIST::Nth(item->Arguments(),1));
  if (clauseType == UnknownItemType) return false;
  
  _signature.resetVariables();
  Input::LiteralList* literals;
  if (!readLiterals(PR_TERM_LIST::Nth(item->Arguments(),0)->Arguments(),literals)) 
    return false;

  Input::Clause* clause = new Input::Clause(clauseName,literals);
  switch (clauseType)
    {
    case TPTPAxiom: clause->setCategory(Input::Clause::Axiom); break;
    case TPTPConjecture: clause->setCategory(Input::Clause::Conjecture); break;
    case TPTPHypothesis: clause->setCategory(Input::Clause::Hypothesis); break;
    case UnknownItemType: clause->setCategory(Input::Clause::Unclassified); break;
    };
  
  _clauses = new Input::ClauseList(clause,_clauses);
  return true;
}; // bool Parser::readClause(PR_LITERAL* item)

bool Parser::readLiterals(PR_TERM_LIST* lst,Input::LiteralList*& result)
{
  CALL("readLiterals(PR_TERM_LIST* lst,Input::LiteralList*& result)");
  if (!lst) { result = 0; return true; };
  Input::LiteralList* tail;
  if (!readLiterals(lst->Next(),tail)) return false; 
  
  Input::Literal* lit;
  if (!readLiteral(lst->Term(),lit))
    {
      Input::destroyLiteralList(tail);
      return false;
    };
  result = new Input::LiteralList(lit,tail);
  return true;
}; // bool Parser::readLiterals(PR_TERM_LIST* lst,Input::LiteralList*& result)

bool Parser::readLiteral(PR_TERM* lit,Input::Literal*& result)
{
  CALL("readLiteral(PR_TERM* lit,Input::Literal*& result)");
  bool isPositive = (strcmp(lit->Symbol()->Symbol(),"++") == 0);
  Input::Term* atom;
  if (!readAtom(lit->Arguments()->Term(),atom)) return false;
  result = new Input::Literal(isPositive,atom);
  return true;
}; // bool Parser::readLiteral(PR_TERM* lit,Input::Literal*& result)

bool Parser::readAtom(PR_TERM* at,Input::Term*& result)
{
  CALL("readAtom(PR_TERM* at,Input::Term*& result)");
  PR_TERM_LIST* args = at->Arguments();
  ulong arity = PR_TERM_LIST::Length(args);
  const Input::Symbol* pred = _signature.registerPredicate(at->Symbol()->Symbol(),
							   arity);
  if (!pred) return false;
  Input::TermList* terms;
  if (!readTerms(args,terms)) return false;
  result = new Input::Term(pred,terms);
  return true;
}; // bool Parser::readAtom(PR_TERM* at,Input::Term*& result)


bool Parser::readTerms(PR_TERM_LIST* terms,Input::TermList*& result)
{
  CALL("readTerms(PR_TERM_LIST* terms,Input::TermList*& result)");
  if (!terms) { result = 0; return true; };
  Input::TermList* tail;
  if (!readTerms(terms->Next(),tail)) return false;
  Input::Term* head;
  if (!readTerm(terms->Term(),head))
    {
      Input::destroyTermList(tail);
      return false;
    };
  result = new Input::TermList(head,tail);
  return true;
}; // bool Parser::readTerms(PR_TERM_LIST* terms,Input::TermList*& result)


bool Parser::readTerm(PR_TERM* term,Input::Term*& result)
{
  CALL("readTerm(PR_TERM* term,Input::Term*& result)");
  const Input::Symbol* top;
  switch (term->Tag())
    {
    case PR_TERM::ATOM_FUNCTOR: // constant, no break here
    case PR_TERM::ATOM_NUMBER: // constant
      top = _signature.registerFunction(term->Symbol()->Symbol(),0UL);
      if (!top) return false;
      result = new Input::Term(top);
      return true;

    case PR_TERM::VAR:
      top = _signature.registerVariable(term->Symbol()->Symbol());
      if (!top) return false;
      result = new Input::Term(top);
      return true;

    case PR_TERM::COMPLEX:
      {
	PR_TERM_LIST* args = term->Arguments();
	ulong arity = PR_TERM_LIST::Length(args);
	top = _signature.registerFunction(term->Symbol()->Symbol(),arity);
	if (!top) return false;
	Input::TermList* terms;
	if (!readTerms(args,terms)) return false;
	result = new Input::Term(top,terms);
	return true;
      };
    default: return false;
    };

}; // bool Parser::readTerm(PR_TERM* term,Input::Term*& result)

bool Parser::readSymbolPrecedence(PR_LITERAL* item)
{
  CALL("readSymbolPrecedence(PR_LITERAL* item)");
  if (PR_TERM_LIST::Length(item->Arguments()) != 4) 
    {
      cout << "Exactly 4 arguments expected.\n";
      return false;
    };

  PR_TERM* arg1 = PR_TERM_LIST::Nth(item->Arguments(),3);
  PR_TERM* arg2 = PR_TERM_LIST::Nth(item->Arguments(),2);
  PR_TERM* arg3 = PR_TERM_LIST::Nth(item->Arguments(),1);
  PR_TERM* arg4 = PR_TERM_LIST::Nth(item->Arguments(),0);
  
  if ((arg1->Tag() != PR_TERM::ATOM_FUNCTOR) ||
      (strcmp(arg1->Symbol()->Symbol(),"predicate") &&       
       strcmp(arg1->Symbol()->Symbol(),"function")))
    {
      cout << "1st argument must be either 'predicate' or 'function'\n";
      return false;
    };
  
  if ((arg2->Tag() != PR_TERM::ATOM_FUNCTOR) &&
      (arg2->Tag() != PR_TERM::ATOM_NUMBER))
    {
      cout << "Symbolic atom or numeric constant expected as the 2nd argument.\n";
      return false;
    };
  if (arg3->Tag() != PR_TERM::ATOM_NUMBER) 
    {
      cout << "Integer expected as the 3rd argument.\n";
      return false;
    };

  if (arg4->Tag() != PR_TERM::ATOM_NUMBER) 
    {
      cout << "Integer expected as the 4th argument.\n";
      return false;
    };

  bool predicate = !strcmp(arg1->Symbol()->Symbol(),"predicate");
  long arity = strtol(arg3->Symbol()->Symbol(),0,10);
  long precedence = strtol(arg4->Symbol()->Symbol(),0,10);
  bool res = _signature.registerPrecedence(predicate,
					   arg2->Symbol()->Symbol(),
					   arity,
					   precedence);
  if (!res)
    {
      cout << "Inconsistent with some other settings.\n";
    };
  return res;
}; // bool Parser::readSymbolPrecedence(PR_LITERAL* item)

bool Parser::readSymbolWeight(PR_LITERAL* item)
{
  CALL("readSymbolWeight(PR_LITERAL* item)");

  if (PR_TERM_LIST::Length(item->Arguments()) != 4) 
    {
      cout << "Exactly 4 arguments expected.\n";
      return false;
    };

  PR_TERM* arg1 = PR_TERM_LIST::Nth(item->Arguments(),3);
  PR_TERM* arg2 = PR_TERM_LIST::Nth(item->Arguments(),2);
  PR_TERM* arg3 = PR_TERM_LIST::Nth(item->Arguments(),1);
  PR_TERM* arg4 = PR_TERM_LIST::Nth(item->Arguments(),0);
  
   if ((arg1->Tag() != PR_TERM::ATOM_FUNCTOR) ||
      (strcmp(arg1->Symbol()->Symbol(),"predicate") &&       
       strcmp(arg1->Symbol()->Symbol(),"function")))
    {
      cout << "1st argument must be either 'predicate' or 'function'\n";
      return false;
    };

  if ((arg2->Tag() != PR_TERM::ATOM_FUNCTOR) && 
      (arg2->Tag() != PR_TERM::ATOM_NUMBER))
    {
      cout << "Function symbol expected as the 2nd argument.\n";
      return false;
    };   

  if (arg3->Tag() != PR_TERM::ATOM_NUMBER)
    {
      cout << "Integer expected as the 3rd argument.\n";
      return false;
    };  
  
  if (arg4->Tag() != PR_TERM::ATOM_NUMBER)
    {
      cout << "Integer expected as the 4th argument.\n";
      return false;
    };  
  
  bool predicate = !strcmp(arg1->Symbol()->Symbol(),"predicate");

  long arity = strtol(arg3->Symbol()->Symbol(),0,10);

  Input::Symbol* symbol = _signature.registerSymbol(predicate,arg2->Symbol()->Symbol(),arity);

  VampireKernel::TermWeightType weightConstPart = 
    strtol(arg4->Symbol()->Symbol(),0,10);

  if (weightConstPart < (VampireKernel::TermWeightType)0)
    {
      cout << "Symbol weight < 0.\n";
      return false;
    };

  symbol->setWeightConstPart(weightConstPart);
  
  return true;
}; // bool Parser::readSymbolWeight(PR_LITERAL* item)



bool Parser::readSymbolInterpretation(PR_LITERAL* item)
{
  CALL("readSymbolInterpretation(PR_LITERAL* item)");
  
  if (PR_TERM_LIST::Length(item->Arguments()) != 5) 
    {
      cout << "5 arguments expected: input symbol type ('predicate' or 'function'), print name and arity, theory name and native symbol.\n";
      return false;
    };

  PR_TERM* arg1 = PR_TERM_LIST::Nth(item->Arguments(),4);  
  PR_TERM* arg2 = PR_TERM_LIST::Nth(item->Arguments(),3);
  PR_TERM* arg3 = PR_TERM_LIST::Nth(item->Arguments(),2);
  PR_TERM* arg4 = PR_TERM_LIST::Nth(item->Arguments(),1);
  PR_TERM* arg5 = PR_TERM_LIST::Nth(item->Arguments(),0);
  

   if ((arg1->Tag() != PR_TERM::ATOM_FUNCTOR) ||
      (strcmp(arg1->Symbol()->Symbol(),"predicate") &&       
       strcmp(arg1->Symbol()->Symbol(),"function")))
    {
      cout << "1st argument must be either 'predicate' or 'function'\n";
      return false;
    };


  if ((arg2->Tag() != PR_TERM::ATOM_FUNCTOR) ||
      (arg3->Tag() != PR_TERM::ATOM_NUMBER) ||
      (arg4->Tag() != PR_TERM::ATOM_FUNCTOR) ||
      (arg5->Tag() != PR_TERM::ATOM_FUNCTOR))
    {
      cout << "The 2nd,4th and 5th arguments are expected to be symbolic atoms. The 3rd argument must be integer.\n";
      return false;
    };

  bool predicate = !strcmp(arg1->Symbol()->Symbol(),"predicate");

  long arity = strtol(arg3->Symbol()->Symbol(),0,10);

  Input::Symbol* symbol = _signature.registerSymbol(predicate,
						    arg2->Symbol()->Symbol(),
						    arity);
  symbol->addInterpretation(arg4->Symbol()->Symbol(),
			    arg5->Symbol()->Symbol());

  return true;
}; // bool Parser::readSymbolInterpretation(PR_LITERAL* item)


bool Parser::readAnswerPredicateDeclaration(PR_LITERAL* item)
{
  CALL("readAnswerPredicateDeclaration(PR_LITERAL* item)");
  if (PR_TERM_LIST::Length(item->Arguments()) != 2) 
    {
      cout << "2 arguments expected: predicate print name and arity.\n";
      return false;
    };
  PR_TERM* arg1 = PR_TERM_LIST::Nth(item->Arguments(),1);
  PR_TERM* arg2 = PR_TERM_LIST::Nth(item->Arguments(),0);
  if (arg1->Tag() != PR_TERM::ATOM_FUNCTOR) return false;  
  if (arg2->Tag() != PR_TERM::ATOM_NUMBER) return false;
  long arity = strtol(arg2->Symbol()->Symbol(),0,10);
  return _signature.registerAnswerPredicate(arg1->Symbol()->Symbol(),arity);
}; // bool Parser::readAnswerPredicateDeclaration(PR_LITERAL* item)

//======================================================================
