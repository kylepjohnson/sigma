/** This software is an adaptation of the theorem prover Vampire for
 * working with large knowledge bases in the KIF format, see 
 * http://www.prover.info for publications on Vampire.
 *
 * Copyright (C) Andrei Voronkov and Alexandre Riazanov
 *
 * @author Alexandre Riazanov <riazanov@cs.man.ac.uk>
 * @author Andrei Voronkov <voronkov@cs.man.ac.uk>, <andrei@voronkov.com>
 *
 * @date 06/06/2003
 * 
 * This software is free software; you can redistribute it and/or
 * modify it under the terms of the GNU Lesser General Public
 * License as published by the Free Software Foundation; either
 * version 2.1 of the License, or (at your option) any later version.
 * 
 * This library is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
 * Lesser General Public License for more details.
 *
 * You should have received a copy of the GNU Lesser General Public
 * License along with this library; if not, write to the Free Software
 * Foundation, Inc., 59 Temple Place, Suite 330, Boston, MA  02111-1307  USA
 */

/* $ANTLR 2.7.1: "TPTP.g" -> "TPTPParser.cpp"$ */
#include "TPTPParser.hpp"
#include "antlr/NoViableAltException.hpp"
#include "antlr/SemanticException.hpp"
#line 121 "TPTP.g"

// header of the parser file
#include <fstream>

#include "IntNameTable.hpp"
#include "antlr/TokenStreamSelector.hpp"
#include "TPTPLexer.hpp"
#include "Options.hpp"


extern antlr::TokenStreamSelector* selector;

#line 19 "TPTPParser.cpp"
TPTPParser::TPTPParser(ANTLR_USE_NAMESPACE(antlr)TokenBuffer& tokenBuf, int k)
: ANTLR_USE_NAMESPACE(antlr)LLkParser(tokenBuf,k)
{
	setTokenNames(_tokenNames);
}

TPTPParser::TPTPParser(ANTLR_USE_NAMESPACE(antlr)TokenBuffer& tokenBuf)
: ANTLR_USE_NAMESPACE(antlr)LLkParser(tokenBuf,2)
{
	setTokenNames(_tokenNames);
}

TPTPParser::TPTPParser(ANTLR_USE_NAMESPACE(antlr)TokenStream& lexer, int k)
: ANTLR_USE_NAMESPACE(antlr)LLkParser(lexer,k)
{
	setTokenNames(_tokenNames);
}

TPTPParser::TPTPParser(ANTLR_USE_NAMESPACE(antlr)TokenStream& lexer)
: ANTLR_USE_NAMESPACE(antlr)LLkParser(lexer,2)
{
	setTokenNames(_tokenNames);
}

TPTPParser::TPTPParser(const ANTLR_USE_NAMESPACE(antlr)ParserSharedInputState& state)
: ANTLR_USE_NAMESPACE(antlr)LLkParser(state,2)
{
	setTokenNames(_tokenNames);
}

void TPTPParser::units(
	UnitList& us
) {
#line 148 "TPTP.g"
	
		  Unit first;
		  UnitList vs;
		
#line 58 "TPTPParser.cpp"
	
	{
	switch ( LA(1)) {
	case LITERAL_include:
	{
		include(vs);
		break;
	}
	case LITERAL_input_formula:
	case LITERAL_input_clause:
	{
		unit(first);
		break;
	}
	default:
	{
		throw ANTLR_USE_NAMESPACE(antlr)NoViableAltException(LT(1), getFilename());
	}
	}
	}
	{
	switch ( LA(1)) {
	case LITERAL_input_formula:
	case LITERAL_input_clause:
	case LITERAL_include:
	{
		units(us);
		break;
	}
	case ANTLR_USE_NAMESPACE(antlr)Token::EOF_TYPE:
	{
		break;
	}
	default:
	{
		throw ANTLR_USE_NAMESPACE(antlr)NoViableAltException(LT(1), getFilename());
	}
	}
	}
#line 163 "TPTP.g"
	
		  if (vs.isEmpty()) {
		    us = UnitList (first, us);
		  }
		  else {
		    vs.append (us);
		    us = vs;
		  }
		
#line 108 "TPTPParser.cpp"
}

void TPTPParser::include(
	UnitList& us
) {
	ANTLR_USE_NAMESPACE(antlr)RefToken  nm = ANTLR_USE_NAMESPACE(antlr)nullToken;
	
	kw_include();
	match(LPAR);
	nm = LT(1);
	match(FILE_NAME);
	match(RPAR);
	match(DOT);
#line 181 "TPTP.g"
	
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
		
#line 136 "TPTPParser.cpp"
}

void TPTPParser::unit(
	Unit& unit
) {
	ANTLR_USE_NAMESPACE(antlr)RefToken  nm1 = ANTLR_USE_NAMESPACE(antlr)nullToken;
	ANTLR_USE_NAMESPACE(antlr)RefToken  nm2 = ANTLR_USE_NAMESPACE(antlr)nullToken;
#line 196 "TPTP.g"
	
		  InputType t;
		  char* nameCopy;
		  const char* name;
		
#line 150 "TPTPParser.cpp"
	
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
#line 209 "TPTP.g"
		
			  Formula f;
			
#line 166 "TPTPParser.cpp"
		formula(f);
		match(RPAR);
		match(DOT);
#line 215 "TPTP.g"
		
			  name = nm1->getText().c_str();
		nameCopy = new char [ strlen(name) + 1 ];
		strcpy ( nameCopy, name );
			  unit = Unit ( nameCopy, t, f );
			
#line 177 "TPTPParser.cpp"
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
#line 228 "TPTP.g"
		
			  Clause c;
			
#line 193 "TPTPParser.cpp"
		clause(c);
		match(RPAR);
		match(DOT);
#line 234 "TPTP.g"
		
			  name = nm2->getText().c_str();
			  nameCopy = new char [ strlen(name) + 1 ];
			  strcpy ( nameCopy, name );
			  unit = Unit ( nameCopy, t, c );
			
#line 204 "TPTPParser.cpp"
		break;
	}
	default:
	{
		throw ANTLR_USE_NAMESPACE(antlr)NoViableAltException(LT(1), getFilename());
	}
	}
}

void TPTPParser::kw_include() {
	
	match(LITERAL_include);
}

void TPTPParser::kw_formula() {
	
	match(LITERAL_input_formula);
}

InputType  TPTPParser::type() {
#line 243 "TPTP.g"
	InputType b;
#line 227 "TPTPParser.cpp"
	ANTLR_USE_NAMESPACE(antlr)RefToken  tp = ANTLR_USE_NAMESPACE(antlr)nullToken;
	
	tp = LT(1);
	match(NAME);
#line 247 "TPTP.g"
	
		  if ( ! strcmp (tp->getText().c_str(), "axiom") )
		    b = AXIOM;
		  else if ( ! strcmp (tp->getText().c_str(), "conjecture") )
		    b = CONJECTURE;
		  else if ( ! strcmp (tp->getText().c_str(), "hypothesis") )
		    b = HYPOTHESIS;
		
#line 241 "TPTPParser.cpp"
	return b;
}

void TPTPParser::formula(
	Formula& f
) {
	
	xorformula(f);
	{
	switch ( LA(1)) {
	case IFF:
	{
		match(IFF);
#line 263 "TPTP.g"
		
			  Formula rhs;
			
#line 259 "TPTPParser.cpp"
		formula(rhs);
#line 267 "TPTP.g"
		
			  f = Formula ( Formula::IFF, f, rhs );
			
#line 265 "TPTPParser.cpp"
		break;
	}
	case RPAR:
	{
		break;
	}
	default:
	{
		throw ANTLR_USE_NAMESPACE(antlr)NoViableAltException(LT(1), getFilename());
	}
	}
	}
}

void TPTPParser::kw_clause() {
	
	match(LITERAL_input_clause);
}

void TPTPParser::clause(
	Clause& c
) {
	
#line 276 "TPTP.g"
	
		  LiteralList ls;
		
#line 293 "TPTPParser.cpp"
	match(LBRA);
	literals(ls);
	match(RBRA);
#line 282 "TPTP.g"
	
		  c = Clause (ls);
		
#line 301 "TPTPParser.cpp"
}

void TPTPParser::xorformula(
	Formula& f
) {
	
	impformula(f);
	{
	switch ( LA(1)) {
	case XOR:
	{
		match(XOR);
#line 293 "TPTP.g"
		
			  Formula rhs;
			
#line 318 "TPTPParser.cpp"
		xorformula(rhs);
#line 297 "TPTP.g"
		
			  f = Formula ( Formula::XOR, f, rhs );
			
#line 324 "TPTPParser.cpp"
		break;
	}
	case RPAR:
	case IFF:
	{
		break;
	}
	default:
	{
		throw ANTLR_USE_NAMESPACE(antlr)NoViableAltException(LT(1), getFilename());
	}
	}
	}
}

void TPTPParser::literals(
	LiteralList& ls
) {
	
	switch ( LA(1)) {
	case RBRA:
	{
		break;
	}
	case PP:
	case MM:
	{
#line 308 "TPTP.g"
		
			  Literal first;
			
#line 356 "TPTPParser.cpp"
		literal(first);
		{
		switch ( LA(1)) {
		case COMMA:
		{
			match(COMMA);
			literals(ls);
			break;
		}
		case RBRA:
		{
			break;
		}
		default:
		{
			throw ANTLR_USE_NAMESPACE(antlr)NoViableAltException(LT(1), getFilename());
		}
		}
		}
#line 317 "TPTP.g"
		
			  ls = LiteralList (first, ls);
			
#line 380 "TPTPParser.cpp"
		break;
	}
	default:
	{
		throw ANTLR_USE_NAMESPACE(antlr)NoViableAltException(LT(1), getFilename());
	}
	}
}

void TPTPParser::impformula(
	Formula& f
) {
	
	orformula(f);
	{
	switch ( LA(1)) {
	case IMP:
	{
		match(IMP);
#line 327 "TPTP.g"
		
			  Formula rhs;
			
#line 404 "TPTPParser.cpp"
		impformula(rhs);
#line 331 "TPTP.g"
		
			  f = Formula (Formula::IMP, f, rhs);
			
#line 410 "TPTPParser.cpp"
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
		throw ANTLR_USE_NAMESPACE(antlr)NoViableAltException(LT(1), getFilename());
	}
	}
	}
}

void TPTPParser::literal(
	Literal& l
) {
#line 354 "TPTP.g"
	
		  bool s;
		  Atom a;
		
#line 435 "TPTPParser.cpp"
	
	s=sign();
	atom(a);
#line 362 "TPTP.g"
	
		  l = Literal (s, a);
		
#line 443 "TPTPParser.cpp"
}

void TPTPParser::orformula(
	Formula& f
) {
	
	andformula(f);
	{
	switch ( LA(1)) {
	case OR:
	{
		match(OR);
#line 373 "TPTP.g"
		
			  Formula rhs;
			
#line 460 "TPTPParser.cpp"
		orformula(rhs);
#line 377 "TPTP.g"
		
			  f.makeJunction (Formula::OR, f, rhs);
			
#line 466 "TPTPParser.cpp"
		break;
	}
	case RPAR:
	case IFF:
	case IMP:
	case XOR:
	{
		break;
	}
	default:
	{
		throw ANTLR_USE_NAMESPACE(antlr)NoViableAltException(LT(1), getFilename());
	}
	}
	}
}

bool  TPTPParser::sign() {
#line 339 "TPTP.g"
	bool s;
#line 487 "TPTPParser.cpp"
	
	switch ( LA(1)) {
	case PP:
	{
		match(PP);
#line 343 "TPTP.g"
		
			  s = true;
			
#line 497 "TPTPParser.cpp"
		break;
	}
	case MM:
	{
		match(MM);
#line 348 "TPTP.g"
		
			  s = false;
			
#line 507 "TPTPParser.cpp"
		break;
	}
	default:
	{
		throw ANTLR_USE_NAMESPACE(antlr)NoViableAltException(LT(1), getFilename());
	}
	}
	return s;
}

void TPTPParser::atom(
	Atom& a
) {
	ANTLR_USE_NAMESPACE(antlr)RefToken  fun = ANTLR_USE_NAMESPACE(antlr)nullToken;
	
#line 387 "TPTP.g"
	
		  TermList ts;
		
#line 527 "TPTPParser.cpp"
	fun = LT(1);
	match(NAME);
	{
	switch ( LA(1)) {
	case LPAR:
	{
		match(LPAR);
		terms(ts);
		match(RPAR);
		break;
	}
	case RPAR:
	case RBRA:
	case COMMA:
	case AND:
	case OR:
	case IFF:
	case IMP:
	case XOR:
	{
		break;
	}
	default:
	{
		throw ANTLR_USE_NAMESPACE(antlr)NoViableAltException(LT(1), getFilename());
	}
	}
	}
#line 397 "TPTP.g"
	
		  a = Atom ( sig->createPred (fun->getText().c_str(),ts.length()),
			     ts );
		
#line 561 "TPTPParser.cpp"
}

void TPTPParser::andformula(
	Formula& f
) {
	
#line 406 "TPTP.g"
	
		  Formula rhs;
		
#line 572 "TPTPParser.cpp"
	simple_formula(f);
	{
	switch ( LA(1)) {
	case AND:
	{
		match(AND);
		andformula(rhs);
#line 413 "TPTP.g"
		
			  f.makeJunction (Formula::AND, f, rhs);
			
#line 584 "TPTPParser.cpp"
		break;
	}
	case RPAR:
	case OR:
	case IFF:
	case IMP:
	case XOR:
	{
		break;
	}
	default:
	{
		throw ANTLR_USE_NAMESPACE(antlr)NoViableAltException(LT(1), getFilename());
	}
	}
	}
}

void TPTPParser::terms(
	TermList& ts
) {
	
#line 423 "TPTP.g"
	
		  Term t;
		
#line 611 "TPTPParser.cpp"
	term(t);
	{
	switch ( LA(1)) {
	case COMMA:
	{
		match(COMMA);
		terms(ts);
		break;
	}
	case RPAR:
	{
		break;
	}
	default:
	{
		throw ANTLR_USE_NAMESPACE(antlr)NoViableAltException(LT(1), getFilename());
	}
	}
	}
#line 432 "TPTP.g"
	
		  ts = TermList (t, ts);
		
#line 635 "TPTPParser.cpp"
}

void TPTPParser::simple_formula(
	Formula& f
) {
#line 437 "TPTP.g"
	
		  VarList vs;
		  Atom a;
		
#line 646 "TPTPParser.cpp"
	
	switch ( LA(1)) {
	case NOT:
	{
		match(NOT);
		simple_formula(f);
#line 445 "TPTP.g"
		
			  f = Formula ( Formula::NOT, f );
			
#line 657 "TPTPParser.cpp"
		break;
	}
	case FORALL:
	{
		match(FORALL);
		varlist(vs);
		match(COLON);
		simple_formula(f);
#line 453 "TPTP.g"
		
			  f = Formula ( Formula::FORALL, vs, f );
			
#line 670 "TPTPParser.cpp"
		break;
	}
	case EXISTS:
	{
		match(EXISTS);
		varlist(vs);
		match(COLON);
		simple_formula(f);
#line 461 "TPTP.g"
		
			  f = Formula ( Formula::EXISTS, vs, f );
			
#line 683 "TPTPParser.cpp"
		break;
	}
	case LPAR:
	{
		match(LPAR);
		formula(f);
		match(RPAR);
		break;
	}
	case NAME:
	{
		atom(a);
#line 470 "TPTP.g"
		
		f = Formula (a);
			
#line 700 "TPTPParser.cpp"
		break;
	}
	default:
	{
		throw ANTLR_USE_NAMESPACE(antlr)NoViableAltException(LT(1), getFilename());
	}
	}
}

void TPTPParser::term(
	Term& t
) {
	ANTLR_USE_NAMESPACE(antlr)RefToken  fun = ANTLR_USE_NAMESPACE(antlr)nullToken;
	ANTLR_USE_NAMESPACE(antlr)RefToken  var = ANTLR_USE_NAMESPACE(antlr)nullToken;
	
	switch ( LA(1)) {
	case NAME:
	{
		fun = LT(1);
		match(NAME);
#line 478 "TPTP.g"
		
			  TermList ts;
			
#line 725 "TPTPParser.cpp"
		{
		switch ( LA(1)) {
		case LPAR:
		{
			match(LPAR);
			terms(ts);
			match(RPAR);
			break;
		}
		case RPAR:
		case COMMA:
		{
			break;
		}
		default:
		{
			throw ANTLR_USE_NAMESPACE(antlr)NoViableAltException(LT(1), getFilename());
		}
		}
		}
#line 487 "TPTP.g"
		
			  t = Term ( sig->createFun (fun->getText().c_str(),ts.length()),
				     ts );
			
#line 751 "TPTPParser.cpp"
		break;
	}
	case VAR:
	{
		var = LT(1);
		match(VAR);
#line 493 "TPTP.g"
		
			  Var v = IntNameTable::vars->insert (var->getText().c_str());
			  t = Term (v);
			
#line 763 "TPTPParser.cpp"
		break;
	}
	default:
	{
		throw ANTLR_USE_NAMESPACE(antlr)NoViableAltException(LT(1), getFilename());
	}
	}
}

void TPTPParser::varlist(
	VarList& vs
) {
	
	match(LBRA);
	vars(vs);
	match(RBRA);
}

void TPTPParser::vars(
	VarList& vs
) {
	ANTLR_USE_NAMESPACE(antlr)RefToken  v = ANTLR_USE_NAMESPACE(antlr)nullToken;
	
#line 509 "TPTP.g"
	
		  Var var;
		
#line 791 "TPTPParser.cpp"
	v = LT(1);
	match(VAR);
	{
	switch ( LA(1)) {
	case COMMA:
	{
		match(COMMA);
		vars(vs);
		break;
	}
	case RBRA:
	{
		break;
	}
	default:
	{
		throw ANTLR_USE_NAMESPACE(antlr)NoViableAltException(LT(1), getFilename());
	}
	}
	}
#line 518 "TPTP.g"
	
		  var = IntNameTable::vars->insert (v->getText().c_str());
		  vs = VarList (var, vs);
		
#line 817 "TPTPParser.cpp"
}

const char* TPTPParser::_tokenNames[] = {
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
	"QUOTE",
	"FILE_NAME",
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
	"\"input_formula\"",
	"\"input_clause\"",
	"\"include\"",
	0
};



