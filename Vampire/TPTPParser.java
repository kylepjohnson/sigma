// $ANTLR 2.7.1: "TPTP.g" -> "TPTPParser.java"$

  // head of lexer file
#include <iostream>

  //#include "term.h"
  //#include "literal.h"
  //#include "clause.h"
  //#include "formula.h"
  //#include "int_name_table.h"
  #include "unit.h"
  //#include "signature.h"
  //#include "problem.h"

  //class Atom;
  //class Formula;

import antlr.TokenBuffer;
import antlr.TokenStreamException;
import antlr.TokenStreamIOException;
import antlr.ANTLRException;
import antlr.LLkParser;
import antlr.Token;
import antlr.TokenStream;
import antlr.RecognitionException;
import antlr.NoViableAltException;
import antlr.MismatchedTokenException;
import antlr.SemanticException;
import antlr.ParserSharedInputState;
import antlr.collections.impl.BitSet;
import antlr.collections.AST;
import antlr.ASTPair;
import antlr.collections.impl.ASTArray;

  // head of parser file
#include "literal.h"
#include "term.h"
#include "clause.h"
#include "formula.h"
#include "int_name_table.h"
#include "signature.h"
#include "problem.h"
#include "atom.h"

public class TPTPParser extends antlr.LLkParser
       implements TPTPLexerTokenTypes
 {


protected TPTPParser(TokenBuffer tokenBuf, int k) {
  super(tokenBuf,k);
  tokenNames = _tokenNames;
}

public TPTPParser(TokenBuffer tokenBuf) {
  this(tokenBuf,2);
}

protected TPTPParser(TokenStream lexer, int k) {
  super(lexer,k);
  tokenNames = _tokenNames;
}

public TPTPParser(TokenStream lexer) {
  this(lexer,2);
}

public TPTPParser(ParserSharedInputState state) {
  super(state,2);
  tokenNames = _tokenNames;
}

	public final Unit::List*  start() throws RecognitionException, TokenStreamException {
		Unit::List* units;
		
		
			  Unit* first;
			  Unit::List* rest = Unit::List::empty ();
			
		
		first=unit();
		{
		switch ( LA(1)) {
		case LITERAL_input_formula:
		case LITERAL_input_clause:
		{
			rest=start();
			break;
		}
		case EOF:
		{
			
				  rest = Unit::List::empty ();
				
			break;
		}
		default:
		{
			throw new NoViableAltException(LT(1), getFilename());
		}
		}
		}
		
			  units = new Unit::List (first, rest);
			
		return units;
	}
	
	public final Unit*  unit() throws RecognitionException, TokenStreamException {
		Unit* u;
		
		Token  nm1 = null;
		Token  nm2 = null;
		
			  InputType t;
			  char* nameCopy;
			  const char* name;
			
		
		switch ( LA(1)) {
		case LITERAL_input_formula:
		{
			kw_formula();
			match(LPAR);
			nm1 = LT(1);
			match(NAME);
			match(COMMA);
			t=type();
			match(COMMA);
			match(LPAR);
			
				  Formula* f;
				
			f=formula();
			match(RPAR);
			match(RPAR);
			match(DOT);
			
				  name = nm1->getText().c_str();
			nameCopy = new char [ strlen(name) + 1 ];
			strcpy ( nameCopy, name );
				  u = new Unit ( nameCopy, t, f );
				
			break;
		}
		case LITERAL_input_clause:
		{
			kw_clause();
			match(LPAR);
			nm2 = LT(1);
			match(NAME);
			match(COMMA);
			t=type();
			match(COMMA);
			
				  Clause* c;
				
			c=clause();
			match(RPAR);
			match(DOT);
			
				  name = nm2->getText().c_str();
				  nameCopy = new char [ strlen(name) + 1 ];
				  strcpy ( nameCopy, name );
				  u = new Unit ( nameCopy, t, c );
				
			break;
		}
		default:
		{
			throw new NoViableAltException(LT(1), getFilename());
		}
		}
		return u;
	}
	
	public final void kw_formula() throws RecognitionException, TokenStreamException {
		
		
		match(LITERAL_input_formula);
	}
	
	public final InputType  type() throws RecognitionException, TokenStreamException {
		InputType b;
		
		Token  tp = null;
		
		tp = LT(1);
		match(NAME);
		
			  if ( ! strcmp (tp->getText().c_str(), "axiom") )
			    b = AXIOM;
			  else if ( ! strcmp (tp->getText().c_str(), "conjecture") )
			    b = CONJECTURE;
			  else if ( ! strcmp (tp->getText().c_str(), "hypothesis") )
			    b = HYPOTHESIS;
			
		return b;
	}
	
	public final Formula*  formula() throws RecognitionException, TokenStreamException {
		Formula* f;
		
		
		f=xorformula();
		{
		switch ( LA(1)) {
		case IFF:
		{
			match(IFF);
			
				  Formula* rhs;
				
			rhs=formula();
			
				  f = new Formula ( Formula::IFF, f, rhs );
				
			break;
		}
		case RPAR:
		{
			break;
		}
		default:
		{
			throw new NoViableAltException(LT(1), getFilename());
		}
		}
		}
		return f;
	}
	
	public final void kw_clause() throws RecognitionException, TokenStreamException {
		
		
		match(LITERAL_input_clause);
	}
	
	public final Clause*  clause() throws RecognitionException, TokenStreamException {
		Clause* c;
		
		
		
			  Literal::List* l;
			
		match(LBRA);
		l=literals();
		match(RBRA);
		
			  c = new Clause ( l );
			
		return c;
	}
	
	public final Formula*  xorformula() throws RecognitionException, TokenStreamException {
		Formula* f;
		
		
		f=impformula();
		{
		switch ( LA(1)) {
		case XOR:
		{
			match(XOR);
			
				  Formula* rhs;
				
			rhs=xorformula();
			
				  f = new Formula ( Formula::XOR, f, rhs );
				
			break;
		}
		case RPAR:
		case IFF:
		{
			break;
		}
		default:
		{
			throw new NoViableAltException(LT(1), getFilename());
		}
		}
		}
		return f;
	}
	
	public final Literal::List*  literals() throws RecognitionException, TokenStreamException {
		Literal::List* ls;
		
		
		switch ( LA(1)) {
		case RBRA:
		{
			
			ls = Literal::List::empty ();
				
			break;
		}
		case PP:
		case MM:
		{
			
				  Literal* first;
				  Literal::List* rest;
				
			first=literal();
			{
			switch ( LA(1)) {
			case COMMA:
			{
				match(COMMA);
				rest=literals();
				break;
			}
			case RBRA:
			{
				
					  rest = Literal::List::empty ();
					
				break;
			}
			default:
			{
				throw new NoViableAltException(LT(1), getFilename());
			}
			}
			}
			
				  ls = new Literal::List (first, rest);
				
			break;
		}
		default:
		{
			throw new NoViableAltException(LT(1), getFilename());
		}
		}
		return ls;
	}
	
	public final Formula*  impformula() throws RecognitionException, TokenStreamException {
		Formula* f;
		
		
		f=orformula();
		{
		switch ( LA(1)) {
		case T_IMP:
		{
			match(T_IMP);
			
				  Formula* rhs;
				
			rhs=impformula();
			
				  f = new Formula (Formula::IMP, f, rhs);
				
			break;
		}
		case RPAR:
		case IFF:
		case XOR:
		{
			break;
		}
		default:
		{
			throw new NoViableAltException(LT(1), getFilename());
		}
		}
		}
		return f;
	}
	
	public final Literal*  literal() throws RecognitionException, TokenStreamException {
		Literal* l;
		
		
			  bool s;
			  Atom* a;
			
		
		s=sign();
		a=atom();
		
			  l = new Literal (s, a);
			
		return l;
	}
	
	public final Formula*  orformula() throws RecognitionException, TokenStreamException {
		Formula* f;
		
		
		f=andformula();
		{
		switch ( LA(1)) {
		case OR:
		{
			match(OR);
			
				  Formula* rhs;
				
			rhs=orformula();
			
				  f = Formula::parseJunction ( Formula::OR, f, rhs );
				
			break;
		}
		case RPAR:
		case IFF:
		case XOR:
		case T_IMP:
		{
			break;
		}
		default:
		{
			throw new NoViableAltException(LT(1), getFilename());
		}
		}
		}
		return f;
	}
	
	public final bool  sign() throws RecognitionException, TokenStreamException {
		bool s;
		
		
		switch ( LA(1)) {
		case PP:
		{
			match(PP);
			
				  s = true;
				
			break;
		}
		case MM:
		{
			match(MM);
			
				  s = false;
				
			break;
		}
		default:
		{
			throw new NoViableAltException(LT(1), getFilename());
		}
		}
		return s;
	}
	
	public final Atom*  atom() throws RecognitionException, TokenStreamException {
		Atom* f;
		
		Token  fun = null;
		
		
			  Term::List* ts;
			
		fun = LT(1);
		match(NAME);
		{
		switch ( LA(1)) {
		case LPAR:
		{
			match(LPAR);
			ts=terms();
			match(RPAR);
			break;
		}
		case RPAR:
		case RBRA:
		case COMMA:
		case AND:
		case OR:
		case IFF:
		case XOR:
		case T_IMP:
		{
			
				  ts = 0;
				
			break;
		}
		default:
		{
			throw new NoViableAltException(LT(1), getFilename());
		}
		}
		}
		
			  f = new Atom ( sig->createPred (fun->getText().c_str(),ts->length()),
					 ts );
			
		return f;
	}
	
	public final Formula*  andformula() throws RecognitionException, TokenStreamException {
		Formula* f;
		
		
		
			  Formula* lft;
			  Formula* rht = 0;
			
		lft=simple_formula();
		{
		switch ( LA(1)) {
		case AND:
		{
			match(AND);
			rht=andformula();
			break;
		}
		case RPAR:
		case OR:
		case IFF:
		case XOR:
		case T_IMP:
		{
			break;
		}
		default:
		{
			throw new NoViableAltException(LT(1), getFilename());
		}
		}
		}
		
			  if ( rht ) {
			    f = Formula::parseJunction ( Formula::AND, lft, rht );
			  }
			  else {
			    f = lft;
			  }
			
		return f;
	}
	
	public final Term::List*  terms() throws RecognitionException, TokenStreamException {
		Term::List* ts;
		
		
		
			  Term::List* rest = 0;
			  Term* t;
			
		t=term();
		{
		match(COMMA);
		rest=terms();
		}
		
			  ts = new Term::List ( t, rest );
			
		return ts;
	}
	
	public final Formula*  simple_formula() throws RecognitionException, TokenStreamException {
		Formula* f;
		
		
			  Formula* arg;
			  VarList* vs;
			  Atom* a;
			
		
		switch ( LA(1)) {
		case NOT:
		{
			match(NOT);
			arg=simple_formula();
			
				  f = new Formula ( Formula::NOT, arg );
				
			break;
		}
		case FORALL:
		{
			match(FORALL);
			vs=varlist();
			match(COLON);
			arg=simple_formula();
			
				  f = new Formula ( Formula::FORALL, vs, arg );
				
			break;
		}
		case EXISTS:
		{
			match(EXISTS);
			vs=varlist();
			match(COLON);
			arg=simple_formula();
			
				  f = new Formula ( Formula::EXISTS, vs, arg );
				
			break;
		}
		case LPAR:
		{
			match(LPAR);
			f=formula();
			match(RPAR);
			break;
		}
		case NAME:
		{
			a=atom();
			
			f = new Formula ( a );
				
			break;
		}
		default:
		{
			throw new NoViableAltException(LT(1), getFilename());
		}
		}
		return f;
	}
	
	public final Term*  term() throws RecognitionException, TokenStreamException {
		Term* t;
		
		Token  fun = null;
		Token  var = null;
		
		switch ( LA(1)) {
		case NAME:
		{
			fun = LT(1);
			match(NAME);
			
				  Term::List* ts;
				
			{
			switch ( LA(1)) {
			case LPAR:
			{
				match(LPAR);
				ts=terms();
				match(RPAR);
				break;
			}
			case COMMA:
			{
				
					  ts = 0;
					
				break;
			}
			default:
			{
				throw new NoViableAltException(LT(1), getFilename());
			}
			}
			}
			
				  t = new Term ( sig->createFun (fun->getText().c_str(),ts->length()),
						 ts );
				
			break;
		}
		case VAR:
		{
			var = LT(1);
			match(VAR);
			
				  Var v = IntNameTable::vars->insert (var->getText().c_str());
				  t = new Term (v);
				
			break;
		}
		default:
		{
			throw new NoViableAltException(LT(1), getFilename());
		}
		}
		return t;
	}
	
	public final VarList*  varlist() throws RecognitionException, TokenStreamException {
		VarList* vs;
		
		
		match(LBRA);
		vs=vars();
		match(RBRA);
		return vs;
	}
	
	public final VarList*  vars() throws RecognitionException, TokenStreamException {
		VarList* vs;
		
		Token  v = null;
		
		
			  VarList* rest = 0;
			  Var var;
			
		v = LT(1);
		match(VAR);
		{
		switch ( LA(1)) {
		case COMMA:
		{
			match(COMMA);
			rest=vars();
			break;
		}
		case RBRA:
		{
			break;
		}
		default:
		{
			throw new NoViableAltException(LT(1), getFilename());
		}
		}
		}
		
			  var = IntNameTable::vars->insert (v->getText().c_str());
			  vs = new VarList (var, rest);
			
		return vs;
	}
	
	
	public static final String[] _tokenNames = {
		"<0>",
		"EOF",
		"<2>",
		"NULL_TREE_LOOKAHEAD",
		"UPPER",
		"LOWER",
		"DIGIT",
		"WHITE",
		"NUMBER",
		"WORDCHAR",
		"CHARACTER",
		"SKIP_RULE",
		"COMMENT_LINE",
		"LPAR",
		"RPAR",
		"LBRA",
		"RBRA",
		"COMMA",
		"COLON",
		"DOT",
		"AND",
		"NOT",
		"OR",
		"IFF",
		"IMP",
		"XOR",
		"FORALL",
		"EXISTS",
		"PP",
		"MM",
		"NAME",
		"VAR",
		"T_IMP",
		"\"input_formula\"",
		"\"input_clause\""
	};
	
	
	}
