//==================================================================-*-C++-*-=
// File:        KIF.g
// Description: The parser of KTF (Knowledge Interchange Format).
//              See http://suo.ieee.org/suo-kif.html
//              The grammar is written for ANTLR parser generator.
// Part of:     
// Status:      
// Created:     Jul 09, 2002, Andrey Chaltsev, chaltsev@cs.man.ac.uk
// Doc:         http://suo.ieee.org/suo-kif.html
// Compliance:
// Reviewed:    
//              
// Revised:     1000 times, Andrei Voronkov
//              
// Note:        
//============================================================================


header 
{
  // head of lexer file
#include <iostream>
#include "Formula.hpp"
}


options { language="Cpp"; }


class KIFLexer extends Lexer;
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
  'N' | 'O' | 'P' | 'Q' | 'R' | 'S' | 'T' | 'U' | 'V' | 'W' | 'X' | 'Y' | 'Z'
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
SPECIAL  :
  '!' | '$' | '%' | '&' | '*' | '+' | '-' | '.' | '/' | '<' | '=' | '>' | '?' |
  '@' | '_' | '~' 
;


protected
WHITE  :
   ' '| '\t' | '\r' | ('\n' {newline();}) | '\f'
; // the same in skip-rule --- check it

protected
INITIALCHAR  :     UPPER | LOWER
;

protected
WORDCHAR  :        UPPER | LOWER | DIGIT | SPECIAL
;

protected
CHARACTER  :       UPPER | LOWER | DIGIT | SPECIAL | WHITE
;

protected
TICK  :
  '`'
;


// lexemes

WORD :            INITIALCHAR (WORDCHAR)*
;

STRING :          '"' ( (~('"' | '\\'| '\n')) 
                      | ( '\n' {newline();} )
                      | ('\\' . ) 
                      )* 
                  '"'                             // ?????
;

VARIABLE : '?' WORD | '@' WORD
;

protected
FIGURE :  DIGIT (FIGURE)?
;

NUMBER :          ('-')? FIGURE ( '.' FIGURE )? (EXPONENT)?
;

protected
EXPONENT :        'e'  ('-')? FIGURE
;

QUOTE  : TICK
;


// skip white characters.
SKIP_RULE :	  ( ' ' | '\t' | '\r' | ( '\n'{newline();}) | '\f' )
                          { _ttype = ANTLR_USE_NAMESPACE(antlr)Token::SKIP; }
; // I am not sure about newline and '\r'

//commentaries are skipped as well. Commentary is a string beginning with ';'.
COMMENT_LINE:     ';'   (~('\n'))*
                           { _ttype = ANTLR_USE_NAMESPACE(antlr)Token::SKIP; }
	;

// special character string (almost keywords)

LPAR:          '(';
RPAR:          ')';
LESS:          '<';
MORE:          '>';
LESSEQ:        "<=";
GEQ:           ">=";
IMPLY:         "=>";
EQUIVALENCE:   "<=>";




//++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++

//++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++

{
  // head of parser file
#include "IntNameTable.hpp"
#include "Signature.hpp"
#include "Problem.hpp"

}


class KIFParser extends Parser;
options 
{
  k=2;
  defaultErrorHandler=false;
}

// it is added into body of class KIFParser
{
}


// first rule to be invoked
start [FormulaList& formulas] 
        {
	  TRACER ("KIFParser::start");
	  Formula f;
	  FormulaList fs;
	}
  :
  sentence [f]
  (
    start [fs]
    |
  )
        {
	  formulas = FormulaList (f, fs);
	}
;

term [Term& t]
        {
	  TRACER ("KIFParser::term");
	  Var v;
	}
  :
    v = variable // variable
        {
	  t = Term (v);
	}
  | 
    c : WORD  // constant
        {
	  t = Term (sig->createFun (c->getText().c_str(), 0));
	}
  | 
    s:STRING // string constant
        { 
 	  t = Term ( sig->createFun (s->getText().c_str(), 0));
        }
  | 
    funterm [t]
  | 
    n:NUMBER                 
        {
	  char* endptr = 0;
	  const char* string = n->getText().c_str();
	  double d = strtod (string, &endptr);

	  if (*endptr) { // error returned by strtol
	    throw MyException ("Incorrect floating point number");
	  }
	  t = Term (d);
	}
  | 
    quoted_term [t]
  |
    // all logical connectives
    IMPLY
        {
	  t = Term (sig->createFun ("=>", 0));
	}
  |
    EQUIVALENCE
        {
	  t = Term (sig->createFun ("<=>", 0));
	}
  |
    kw_and
        {
	  t = Term (sig->createFun ("and", 0));
	}
  |
    kw_or
        {
	  t = Term (sig->createFun ("or", 0));
	}
  |
    kw_not
        {
	  t = Term (sig->createFun ("not", 0));
	}
  |
    kw_forall
        {
	  t = Term (sig->createFun ("forall", 0));
	}
  |
    kw_exists
        {
	  t = Term (sig->createFun ("exists", 0));
	}
  |
    kw_equal
        {
	  t = Term (sig->createFun ("equal", 0));
	}
;


funterm [Term& t]
        {
	  TRACER ("KIFParser::funterm");
	  TermList ts;
	}
  :
  LPAR
  w : WORD
  term_plus [ts]
        {
	  t = Term ( sig->createFun (w->getText().c_str(), ts.length()),
                     ts );
	}
  RPAR     
;


binary_connective 
  returns [Formula::Connective c]
:
  IMPLY
        { 
	  c = Formula::IMP;
	}
  |
  EQUIVALENCE
        { 
	  c = Formula::IFF;
	}
;


binary_connective_fun
  returns [Signature::Fun* f]
:
  IMPLY
        { 
	  f = sig->createFun ("=>", 2);
	}
  |
  EQUIVALENCE
        { 
	  f = sig->createFun ("<=>", 2);
	}
;


junction
  returns [Formula::Connective c]
:
  kw_and
        { 
	  c = Formula::AND;
	}
  |
  kw_or
        { 
	  c = Formula::OR;
	}
;


quantifier
  returns [Formula::Connective c]
:
  kw_forall
        { 
	  c = Formula::FORALL;
	}
  |
  kw_exists
        { 
	  c = Formula::EXISTS;
	}
;


quantifier_fun
  returns [Signature::Fun* f]
:
  kw_forall
        { 
	  f = sig->createFun ("forall",2);
	}
  |
  kw_exists
        { 
	  f = sig->createFun ("exists",2);
	}
;


quoted_term [Term& t]
        {
	  TRACER ("KIFParser::quoted_term");
	}
  :
  QUOTE
  term_sentence [t]
        {
	  t = Term (sig->createFun("$`",1), TermList(t));
	}
;


sentence [Formula& f]
        {
	  TRACER ("KIFParser::sentence");
	  Atom a;
	}
  :
    c : WORD
        {
          f = Formula (Atom ( sig->createPred (c->getText().c_str(), 0)));
	}
  | 
    equation [a]
        {
          f = Formula (a);
	}
  | 
    inequality [a]
        {
          f = Formula (a);
	}
  | 
    relsent [a]
        {
          f = Formula (a);
	}
  | 
    LPAR  
    kw_not
    sentence [f]
    RPAR
        {
	  f = Formula (Formula::NOT, f);
	}
  | 
        {
	  FormulaList fs;
	  Formula::Connective c;
	}
    LPAR  
    c = junction
    sentence_plus [fs]
    RPAR
        {
	  f = Formula (c, fs);
	}
  | 
        {
	  Formula g;
	  Formula::Connective c;
	}
    LPAR  
    c = binary_connective
    sentence [f]
    sentence [g]
    RPAR
        {
	  f = Formula (c, f, g);
	}

  | 
        {
	  VarList vs;
	  Formula::Connective q;
	}
    LPAR  
    q = quantifier  
    LPAR
    variable_plus [vs]
    RPAR
    sentence [f]
    RPAR
        {
	  f = Formula (q, vs, f);
	}
;


term_sentence [Term& t]
        {
	  TRACER ("KIFParser::term_sentence");
	}
  :
    term_equation [t]
  | 
    term_inequality [t]
  | 
    LPAR  
    kw_not
    term_sentence [t]
    RPAR
        {
	  t = Term (sig->createFun("not",1), TermList(t));
	}
  | 
        {
	  TermList ts;
	  Formula::Connective c;
	}
   LPAR  
   c = junction
   term_sentence_plus [ts]
   RPAR
        {
	  t = Term (sig->createFun(c == Formula::AND? "and" : "or", ts.length()), ts);
	}
  | 
        {
	  Term s;
	  Signature::Fun* f;
	}
    LPAR  
    f = binary_connective_fun
    term_sentence [s]
    term_sentence [t]
    RPAR
        {
	  t = Term (f, 
		    TermList(s, 
			     TermList(t)));
	}
  | 
        {
	  VarList vs;
	  TermList ts;
	  Signature::Fun* q;
	}
    LPAR  
    q = quantifier_fun
    LPAR
    variable_plus [vs]
    RPAR
    term_sentence [t]
    RPAR
        {
	  Signature::Fun* v = sig->createFun ("$qvars",vs.length());
	  ts.buildFrom (vs);
	  t = Term (q, TermList(Term(v,ts),TermList(t)));
	}
  |
    term [t]
;


equation [Atom& a]
        {
	  TRACER ("KIFParser::equation");
	  Term t1;
	  Term t2;
	}
  :
  LPAR  
  kw_equal
  term [t1]
  term [t2]
        {
	  a = Atom (t1, t2);
	}
  RPAR
;


term_equation [Term& t]
        {
	  TRACER ("KIFParser::term_equation");
	  Term t1;
	  Term t2;
	}
  :
  LPAR  
  kw_equal
  term [t1]
  term [t2]
        {
	  t = Term (sig->createFun ("equal", 2),
		    TermList (t1, 
			      TermList(t2)));
	}
  RPAR
;


binary_comparison
  returns [Signature::Pred* p]
        {
	  TRACER ("KIFParser::binary_comparison");
	}
:
  MORE
        {
	  p = sig->createPred (">", 2);
	}
  |
  LESS
        {
	  p = sig->createPred ("<", 2);
	}
  |
  GEQ
        {
	  p = sig->createPred (">=", 2);
	}
  |
  LESSEQ
        {
	  p = sig->createPred ("<=", 2);
	}
;


binary_comparison_fun
  returns [Signature::Fun* f]
        {
	  TRACER ("KIFParser::binary_comparison_fun");
	}
:
  MORE
        {
	  f = sig->createFun (">", 2);
	}
  |
  LESS
        {
	  f = sig->createFun ("<", 2);
	}
  |
  GEQ
        {
	  f = sig->createFun (">=", 2);
	}
  |
  LESSEQ
        {
	  f = sig->createFun ("<=", 2);
	}
;


inequality [Atom& a]
        {
	  TRACER ("KIFParser::inequality");
	  Signature::Pred* p;
	  Term t1;
	  Term t2;
	}
  :
    LPAR
    p = binary_comparison
    term [t1]
    term [t2]
    RPAR
        {
	  a = Atom (p, TermList (t1, 
		       TermList (t2)));
	}
;


term_inequality [Term& t]
        {
	  TRACER ("KIFParser::term_inequality");
	  Signature::Fun* f;
	  Term t1;
	  Term t2;
	}
  :
    LPAR
    f = binary_comparison_fun
    term [t1]
    term [t2]
    RPAR
        {
	  t = Term (f, TermList (t1, 
		       TermList (t2)));
	}
;


relsent [Atom& a]
        {
	  TRACER ("KIFParser::relsent");
	  TermList ts;
	}
  :
  LPAR
  w : WORD
  term_plus [ts]
        {
	  a = Atom ( sig->createPred (w->getText().c_str(), ts.length()),
                     ts );
	}
  RPAR     
;


sentence_plus [FormulaList& fs]
        {
	  TRACER ("KIFParser::sentence_plus");
          FormulaList rest;
          Formula f;
        }
  :
  sentence [f]
  ( 
    sentence_plus [rest]
    |
  )
        {
	  fs = FormulaList (f, rest);
	}
;


term_sentence_plus [TermList& ts]
        {
	  TRACER ("KIFParser::term_sentence_plus");
          TermList rest;
          Term t;
        }
  :
  term_sentence [t]
  ( 
    term_sentence_plus [rest]
    |
  )
        {
	  ts = TermList (t, rest);
	}
;


term_plus [TermList& ts]
        {
	  TRACER ("KIFParser::term_plus");
          TermList rest;
          Term t;
        }
  :
  term [t]
  ( 
    term_plus [rest]
    |
  )
        {
	  ts = TermList (t, rest);
	}
;

variable_plus [VarList& ws]
        {
	  TRACER ("KIFParser::variable_plus");
          VarList vs;
          Var vv;
        }
  :
  vv = variable
  ( 
    variable_plus [vs]
    | 
  )
        {
	  ws = VarList (vv, vs);
	}
;

variable 
  returns [Var v]
        {
	  TRACER ("KIFParser::variable");
	}
  :
  w : VARIABLE
        { 
          v = IntNameTable::vars->insert(w->getText().c_str());
        } 
;


// I do not know why but ANTLR does not recognise literal (such as "not")
// in lexer rules

//------------------   KEYWORDS  ---------------------------------------

// kw_not       returns [int line]	: l: "not"      {line=l->getLine();};
// kw_and       returns [int line]	: l: "and"      {line=l->getLine();};
// kw_or        returns [int line]	: l: "or"       {line=l->getLine();};
// kw_forall    returns [int line]	: l: "forall"   {line=l->getLine();};
// kw_exists    returns [int line]	: l: "exists"   {line=l->getLine();};


kw_equal     : "equal";
kw_not       : "not";
kw_and       : "and";
kw_or        : "or";
kw_forall    : "forall";
kw_exists    : "exists";

