//==================================================================-*-C++-*-=
// File:       TPTP.g
// Description: The parser of TPTP
//              The grammar is written for the ANTLR parser generator.
// Part of:     
// Status:      
// Created:     16/08/2002 by Andrei Voronkov
// Doc:         http://suo.ieee.org/suo-kif.html
// Compliance:
// Reviewed:    
//              
// Revised:     
//              
// Note:        
//============================================================================


header 
{
#include <iostream>
#include "Unit.hpp"
}


options { language="Cpp"; }


class TPTPLexer extends Lexer;
options
{ 
	k=3;
	charVocabulary = '\u0000'..'\u00FF'; // 8 bits character
  defaultErrorHandler=false;
}

// characters
 
protected
UPPER  : 
  'A' | 'B' | 'C' | 'D' | 'E' | 'F' | 'G' | 'H' | 'I' | 'J' | 'K' | 'L' | 'M' |
  'N' | 'O' | 'P' | 'Q' | 'R' | 'S' | 'T' | 'U' | 'V' | 'W' | 'X' | 'Y' | 'Z' |
  '_'
;

protected
LOWER  :
  'a' | 'b' | 'c' | 'd' | 'e' | 'f' | 'g' | 'h' | 'i' | 'j' | 'k' | 'l' | 'm' |
  'n' | 'o' | 'p' | 'q' | 'r' | 's' | 't' | 'u' | 'v' | 'w' | 'x' | 'y' | 'z'
;

protected
DIGIT  :
  '0' | '1' | '2' | '3' | '4' | '5' | '6' | '7' | '8' | '9'
;

protected
WHITE  :
   ' '| '\t' | '\r' | ('\n' {newline();}) | '\f'
; // the same in skip-rule --- check it

protected
NUMBER :
  (DIGIT)+
; 

protected
WORDCHAR  :        UPPER | LOWER | DIGIT
;

protected
CHARACTER  :       WORDCHAR | WHITE
;

// skip whitespace characters.
SKIP_RULE :	  ( ' ' | '\t' | '\r' | ( '\n'{newline();}) | '\f' )
                          { _ttype = ANTLR_USE_NAMESPACE(antlr)Token::SKIP; }
; // I am not sure about newline and '\r'

//commentaries are skipped as well. Commentary is a string beginning with ';'.
COMMENT_LINE:     '%'   (~('\n'))*
                          { _ttype = ANTLR_USE_NAMESPACE(antlr)Token::SKIP; }
	;

protected
QUOTE:
  '\''
;

FILE_NAME: QUOTE (~('\''))* QUOTE
;

// special character string (almost keywords)

LPAR:          '(';
RPAR:          ')';
LBRA:          '[';
RBRA:          ']';
COMMA:         ',';
COLON:         ':';
DOT:           '.';

AND:           '&';
NOT:           '~';
OR:            '|';
IFF:           "<=>";
IMP:           "=>";
XOR:           "<~>";
FORALL:        '!';
EXISTS:        '?';
PP:            "++";
MM:            "--";

// identtiffiers
NAME:          LOWER (WORDCHAR)*;
VAR:           UPPER (WORDCHAR)*;

//++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++

//++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++

{
  // header of the parser file
#include <fstream>

#include "IntNameTable.hpp"
#include "antlr/TokenStreamSelector.hpp"
#include "TPTPLexer.hpp"
#include "Options.hpp"


extern antlr::TokenStreamSelector* selector;
}


class TPTPParser extends Parser;
options 
{
  k=2;
  defaultErrorHandler=false;
}

// to bbe added into the body of TPTPParser
{
}


// first rule to be invoked
units [UnitList& us]
        {
	  Unit first;
	  UnitList vs;
	}
  :
 (
  include [vs]
  |
  unit [first]
 )
 (
   units [us]
   |
 )
        {
	  if (vs.isEmpty()) {
	    us = UnitList (first, us);
	  }
	  else {
	    vs.append (us);
	    us = vs;
	  }
	}
;

include [UnitList& us]
:
  kw_include
  LPAR
  nm : FILE_NAME
  RPAR
  DOT
        {
	  char* inputFileName = const_cast<char*>(nm->getText().c_str());
	  string absoluteName = options->includeFileName (inputFileName);
	  std::ifstream inFile (absoluteName.c_str());
	  if(!inFile) {
	    string err = "cannot open include file: '";
	    err += absoluteName + "'";
	    throw MyException (err);
	  };
	  TPTPLexer lexer(inFile); 
	  TPTPParser parser (lexer);
	  parser.units (us);
	}
;

unit [Unit& unit]
        {
	  InputType t;
	  char* nameCopy;
	  const char* name;
	}
  :
  kw_formula      // input_formula
  LPAR            //              (
  nm1 : NAME      //              name
  COMMA           //                   ,
  t = type        //                    axiom
  COMMA           //                         ,
        {
	  Formula f;
	}
  formula [f]     //                           a <=> b
  RPAR            //                                   )
  DOT             //                                    .
        {
	  name = nm1->getText().c_str();
          nameCopy = new char [ strlen(name) + 1 ];
          strcpy ( nameCopy, name );
	  unit = Unit ( nameCopy, t, f );
	}
  |
  kw_clause    // input_clause
  LPAR         //              (
  nm2 : NAME   //              name
  COMMA        //                   ,
  t = type     //                    axiom
  COMMA        //                         ,
        {
	  Clause c;
	}
  clause [c]   //                           [++ a, --b]
  RPAR         //                                     )
  DOT          //                                      .
        {
	  name = nm2->getText().c_str();
	  nameCopy = new char [ strlen(name) + 1 ];
	  strcpy ( nameCopy, name );
	  unit = Unit ( nameCopy, t, c );
	}
;


type 
  returns [InputType b]
:
  tp : NAME   //              name
        {
	  if ( ! strcmp (tp->getText().c_str(), "axiom") )
	    b = AXIOM;
	  else if ( ! strcmp (tp->getText().c_str(), "conjecture") )
	    b = CONJECTURE;
	  else if ( ! strcmp (tp->getText().c_str(), "hypothesis") )
	    b = HYPOTHESIS;
	}
;


formula [Formula& f]
:
  xorformula [f]
  (
    IFF
        {
	  Formula rhs;
	}
    formula [rhs]
        {
	  f = Formula ( Formula::IFF, f, rhs );
	}
    |
  )
;

clause [Clause& c]
  :
        {
	  LiteralList ls;
	}
  LBRA
  literals [ls]
  RBRA
        {
	  c = Clause (ls);
	}
;


xorformula [Formula& f]
  :
  impformula [f]
  (
    XOR
        {
	  Formula rhs;
	}
    xorformula [rhs]
        {
	  f = Formula ( Formula::XOR, f, rhs );
	}
    |
  )
;


literals [LiteralList& ls]
:
  |
        {
	  Literal first;
	}
  literal [first]
  (
    COMMA
    literals [ls]
    |
  )
        {
	  ls = LiteralList (first, ls);
	}
;

impformula [Formula& f]
  :
  orformula [f]
  (
    IMP
        {
	  Formula rhs;
	}
    impformula [rhs]
        {
	  f = Formula (Formula::IMP, f, rhs);
	}
    |
  )
;


sign
  returns [bool s]
:
  PP
        {
	  s = true;
	}
  |
  MM
        {
	  s = false;
	}
;


literal [Literal& l]
        {
	  bool s;
	  Atom a;
	}
:
  s = sign
  atom [a]
        {
	  l = Literal (s, a);
	}
;


orformula [Formula& f]
  :
  andformula [f]
  (
    OR
        {
	  Formula rhs;
	}
    orformula [rhs]
        {
	  f.makeJunction (Formula::OR, f, rhs);
	}
    |
  )
;


atom [Atom& a]
  :
        {
	  TermList ts;
	}
  fun : NAME
  (
    LPAR
    terms [ts]
    RPAR
    |
  )
        {
	  a = Atom ( sig->createPred (fun->getText().c_str(),ts.length()),
		     ts );
	}
;


andformula [Formula& f]
  :
        {
	  Formula rhs;
	}
  simple_formula [f]
  (
    AND
    andformula [rhs]
        {
	  f.makeJunction (Formula::AND, f, rhs);
	}
    |
  )
;


terms [TermList& ts]
  :
        {
	  Term t;
	}
  term [t]
  (
    COMMA
    terms [ts]
    |
  )
        {
	  ts = TermList (t, ts);
	}
;

simple_formula [Formula& f]
        {
	  VarList vs;
	  Atom a;
	}
  :
  NOT
  simple_formula [f]
        {
	  f = Formula ( Formula::NOT, f );
	}
  |
  FORALL
  varlist [vs]
  COLON
  simple_formula [f]
        {
	  f = Formula ( Formula::FORALL, vs, f );
	}
  |
  EXISTS
  varlist [vs]
  COLON
  simple_formula [f]
        {
	  f = Formula ( Formula::EXISTS, vs, f );
	}
  |
  LPAR
  formula [f]
  RPAR
  |
  atom [a]
        {
          f = Formula (a);
	}
;

term [Term& t]
  :
  fun : NAME
        {
	  TermList ts;
	}
  (
    LPAR
    terms [ts]
    RPAR
    |
  )
        {
	  t = Term ( sig->createFun (fun->getText().c_str(),ts.length()),
		     ts );
	}
  |
  var : VAR
        {
	  Var v = IntNameTable::vars->insert (var->getText().c_str());
	  t = Term (v);
	}
;


varlist [VarList& vs]
  :
  LBRA
  vars [vs]
  RBRA
;

vars [VarList& vs]
  :
        {
	  Var var;
	}
  v : VAR
  (
    COMMA
    vars [vs]
    |
  )
        {
	  var = IntNameTable::vars->insert (v->getText().c_str());
	  vs = VarList (var, vs);
	}
;

// TPTP keywords
kw_formula:        "input_formula";
kw_clause:         "input_clause";
kw_include:        "include";
