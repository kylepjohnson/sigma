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
 *
 * Also add information on how to contact you by electronic and paper mail.
 */

/* $ANTLR 2.7.1: "KIF.g" -> "KIFParser.cpp"$ */
#include "KIFParser.hpp"
#include "antlr/NoViableAltException.hpp"
#include "antlr/SemanticException.hpp"
#line 145 "KIF.g"

// head of parser file
#include "IntNameTable.hpp"
#include "Signature.hpp"
#include "Problem.hpp"


#line 14 "KIFParser.cpp"
KIFParser::KIFParser(ANTLR_USE_NAMESPACE(antlr)TokenBuffer& tokenBuf, int k)
: ANTLR_USE_NAMESPACE(antlr)LLkParser(tokenBuf,k)
{
	setTokenNames(_tokenNames);
}

KIFParser::KIFParser(ANTLR_USE_NAMESPACE(antlr)TokenBuffer& tokenBuf)
: ANTLR_USE_NAMESPACE(antlr)LLkParser(tokenBuf,2)
{
	setTokenNames(_tokenNames);
}

KIFParser::KIFParser(ANTLR_USE_NAMESPACE(antlr)TokenStream& lexer, int k)
: ANTLR_USE_NAMESPACE(antlr)LLkParser(lexer,k)
{
	setTokenNames(_tokenNames);
}

KIFParser::KIFParser(ANTLR_USE_NAMESPACE(antlr)TokenStream& lexer)
: ANTLR_USE_NAMESPACE(antlr)LLkParser(lexer,2)
{
	setTokenNames(_tokenNames);
}

KIFParser::KIFParser(const ANTLR_USE_NAMESPACE(antlr)ParserSharedInputState& state)
: ANTLR_USE_NAMESPACE(antlr)LLkParser(state,2)
{
	setTokenNames(_tokenNames);
}

void KIFParser::start(
	FormulaList& formulas
) {
#line 167 "KIF.g"
	
		  TRACER ("KIFParser::start");
		  Formula f;
		  FormulaList fs;
		
#line 54 "KIFParser.cpp"
	
	sentence(f);
	{
	switch ( LA(1)) {
	case WORD:
	case LPAR:
	{
		start(fs);
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
#line 179 "KIF.g"
	
		  formulas = FormulaList (f, fs);
		
#line 79 "KIFParser.cpp"
}

void KIFParser::sentence(
	Formula& f
) {
	ANTLR_USE_NAMESPACE(antlr)RefToken  c = ANTLR_USE_NAMESPACE(antlr)nullToken;
#line 369 "KIF.g"
	
		  TRACER ("KIFParser::sentence");
		  Atom a;
		
#line 91 "KIFParser.cpp"
	
	if ((LA(1)==WORD)) {
		c = LT(1);
		match(WORD);
#line 376 "KIF.g"
		
		f = Formula (Atom ( sig->createPred (c->getText().c_str(), 0)));
			
#line 100 "KIFParser.cpp"
	}
	else if ((LA(1)==LPAR) && (LA(2)==LITERAL_equal)) {
		equation(a);
#line 381 "KIF.g"
		
		f = Formula (a);
			
#line 108 "KIFParser.cpp"
	}
	else if ((LA(1)==LPAR) && ((LA(2) >= LESS && LA(2) <= GEQ))) {
		inequality(a);
#line 386 "KIF.g"
		
		f = Formula (a);
			
#line 116 "KIFParser.cpp"
	}
	else if ((LA(1)==LPAR) && (LA(2)==WORD)) {
		relsent(a);
#line 391 "KIF.g"
		
		f = Formula (a);
			
#line 124 "KIFParser.cpp"
	}
	else if ((LA(1)==LPAR) && (LA(2)==LITERAL_not)) {
		match(LPAR);
		kw_not();
		sentence(f);
		match(RPAR);
#line 399 "KIF.g"
		
			  f = Formula (Formula::NOT, f);
			
#line 135 "KIFParser.cpp"
	}
	else if ((LA(1)==LPAR) && (LA(2)==LITERAL_and||LA(2)==LITERAL_or)) {
#line 403 "KIF.g"
		
			  FormulaList fs;
			  Formula::Connective c;
			
#line 143 "KIFParser.cpp"
		match(LPAR);
		c=junction();
		sentence_plus(fs);
		match(RPAR);
#line 411 "KIF.g"
		
			  f = Formula (c, fs);
			
#line 152 "KIFParser.cpp"
	}
	else if ((LA(1)==LPAR) && (LA(2)==IMPLY||LA(2)==EQUIVALENCE)) {
#line 415 "KIF.g"
		
			  Formula g;
			  Formula::Connective c;
			
#line 160 "KIFParser.cpp"
		match(LPAR);
		c=binary_connective();
		sentence(f);
		sentence(g);
		match(RPAR);
#line 424 "KIF.g"
		
			  f = Formula (c, f, g);
			
#line 170 "KIFParser.cpp"
	}
	else if ((LA(1)==LPAR) && (LA(2)==LITERAL_forall||LA(2)==LITERAL_exists)) {
#line 429 "KIF.g"
		
			  VarList vs;
			  Formula::Connective q;
			
#line 178 "KIFParser.cpp"
		match(LPAR);
		q=quantifier();
		match(LPAR);
		variable_plus(vs);
		match(RPAR);
		sentence(f);
		match(RPAR);
#line 440 "KIF.g"
		
			  f = Formula (q, vs, f);
			
#line 190 "KIFParser.cpp"
	}
	else {
		throw ANTLR_USE_NAMESPACE(antlr)NoViableAltException(LT(1), getFilename());
	}
	
}

void KIFParser::term(
	Term& t
) {
	ANTLR_USE_NAMESPACE(antlr)RefToken  c = ANTLR_USE_NAMESPACE(antlr)nullToken;
	ANTLR_USE_NAMESPACE(antlr)RefToken  s = ANTLR_USE_NAMESPACE(antlr)nullToken;
	ANTLR_USE_NAMESPACE(antlr)RefToken  n = ANTLR_USE_NAMESPACE(antlr)nullToken;
#line 184 "KIF.g"
	
		  TRACER ("KIFParser::term");
		  Var v;
		
#line 209 "KIFParser.cpp"
	
	switch ( LA(1)) {
	case VARIABLE:
	{
		v=variable();
#line 191 "KIF.g"
		
			  t = Term (v);
			
#line 219 "KIFParser.cpp"
		break;
	}
	case WORD:
	{
		c = LT(1);
		match(WORD);
#line 196 "KIF.g"
		
			  t = Term (sig->createFun (c->getText().c_str(), 0));
			
#line 230 "KIFParser.cpp"
		break;
	}
	case STRING:
	{
		s = LT(1);
		match(STRING);
#line 201 "KIF.g"
		
			  t = Term ( sig->createFun (s->getText().c_str(), 0));
		
#line 241 "KIFParser.cpp"
		break;
	}
	case LPAR:
	{
		funterm(t);
		break;
	}
	case NUMBER:
	{
		n = LT(1);
		match(NUMBER);
#line 208 "KIF.g"
		
			  char* endptr = 0;
			  const char* string = n->getText().c_str();
			  double d = strtod (string, &endptr);
		
			  if (*endptr) { // error returned by strtol
			    throw MyException ("Incorrect floating point number");
			  }
			  t = Term (d);
			
#line 264 "KIFParser.cpp"
		break;
	}
	case QUOTE:
	{
		quoted_term(t);
		break;
	}
	case IMPLY:
	{
		match(IMPLY);
#line 223 "KIF.g"
		
			  t = Term (sig->createFun ("=>", 0));
			
#line 279 "KIFParser.cpp"
		break;
	}
	case EQUIVALENCE:
	{
		match(EQUIVALENCE);
#line 228 "KIF.g"
		
			  t = Term (sig->createFun ("<=>", 0));
			
#line 289 "KIFParser.cpp"
		break;
	}
	case LITERAL_and:
	{
		kw_and();
#line 233 "KIF.g"
		
			  t = Term (sig->createFun ("and", 0));
			
#line 299 "KIFParser.cpp"
		break;
	}
	case LITERAL_or:
	{
		kw_or();
#line 238 "KIF.g"
		
			  t = Term (sig->createFun ("or", 0));
			
#line 309 "KIFParser.cpp"
		break;
	}
	case LITERAL_not:
	{
		kw_not();
#line 243 "KIF.g"
		
			  t = Term (sig->createFun ("not", 0));
			
#line 319 "KIFParser.cpp"
		break;
	}
	case LITERAL_forall:
	{
		kw_forall();
#line 248 "KIF.g"
		
			  t = Term (sig->createFun ("forall", 0));
			
#line 329 "KIFParser.cpp"
		break;
	}
	case LITERAL_exists:
	{
		kw_exists();
#line 253 "KIF.g"
		
			  t = Term (sig->createFun ("exists", 0));
			
#line 339 "KIFParser.cpp"
		break;
	}
	case LITERAL_equal:
	{
		kw_equal();
#line 258 "KIF.g"
		
			  t = Term (sig->createFun ("equal", 0));
			
#line 349 "KIFParser.cpp"
		break;
	}
	default:
	{
		throw ANTLR_USE_NAMESPACE(antlr)NoViableAltException(LT(1), getFilename());
	}
	}
}

Var  KIFParser::variable() {
#line 733 "KIF.g"
	Var v;
#line 362 "KIFParser.cpp"
	ANTLR_USE_NAMESPACE(antlr)RefToken  w = ANTLR_USE_NAMESPACE(antlr)nullToken;
#line 733 "KIF.g"
	
		  TRACER ("KIFParser::variable");
		
#line 368 "KIFParser.cpp"
	
	w = LT(1);
	match(VARIABLE);
#line 740 "KIF.g"
	
	v = IntNameTable::vars->insert(w->getText().c_str());
	
#line 376 "KIFParser.cpp"
	return v;
}

void KIFParser::funterm(
	Term& t
) {
	ANTLR_USE_NAMESPACE(antlr)RefToken  w = ANTLR_USE_NAMESPACE(antlr)nullToken;
#line 264 "KIF.g"
	
		  TRACER ("KIFParser::funterm");
		  TermList ts;
		
#line 389 "KIFParser.cpp"
	
	match(LPAR);
	w = LT(1);
	match(WORD);
	term_plus(ts);
#line 273 "KIF.g"
	
		  t = Term ( sig->createFun (w->getText().c_str(), ts.length()),
	ts );
		
#line 400 "KIFParser.cpp"
	match(RPAR);
}

void KIFParser::quoted_term(
	Term& t
) {
#line 356 "KIF.g"
	
		  TRACER ("KIFParser::quoted_term");
		
#line 411 "KIFParser.cpp"
	
	match(QUOTE);
	term_sentence(t);
#line 363 "KIF.g"
	
		  t = Term (sig->createFun("$`",1), TermList(t));
		
#line 419 "KIFParser.cpp"
}

void KIFParser::kw_and() {
	
	match(LITERAL_and);
}

void KIFParser::kw_or() {
	
	match(LITERAL_or);
}

void KIFParser::kw_not() {
	
	match(LITERAL_not);
}

void KIFParser::kw_forall() {
	
	match(LITERAL_forall);
}

void KIFParser::kw_exists() {
	
	match(LITERAL_exists);
}

void KIFParser::kw_equal() {
	
	match(LITERAL_equal);
}

void KIFParser::term_plus(
	TermList& ts
) {
#line 699 "KIF.g"
	
		  TRACER ("KIFParser::term_plus");
	TermList rest;
	Term t;
	
#line 461 "KIFParser.cpp"
	
	term(t);
	{
	switch ( LA(1)) {
	case WORD:
	case STRING:
	case VARIABLE:
	case NUMBER:
	case QUOTE:
	case LPAR:
	case IMPLY:
	case EQUIVALENCE:
	case LITERAL_equal:
	case LITERAL_not:
	case LITERAL_and:
	case LITERAL_or:
	case LITERAL_forall:
	case LITERAL_exists:
	{
		term_plus(rest);
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
#line 711 "KIF.g"
	
		  ts = TermList (t, rest);
		
#line 498 "KIFParser.cpp"
}

Formula::Connective  KIFParser::binary_connective() {
#line 281 "KIF.g"
	Formula::Connective c;
#line 504 "KIFParser.cpp"
	
	switch ( LA(1)) {
	case IMPLY:
	{
		match(IMPLY);
#line 285 "KIF.g"
		
			  c = Formula::IMP;
			
#line 514 "KIFParser.cpp"
		break;
	}
	case EQUIVALENCE:
	{
		match(EQUIVALENCE);
#line 290 "KIF.g"
		
			  c = Formula::IFF;
			
#line 524 "KIFParser.cpp"
		break;
	}
	default:
	{
		throw ANTLR_USE_NAMESPACE(antlr)NoViableAltException(LT(1), getFilename());
	}
	}
	return c;
}

Signature::Fun*  KIFParser::binary_connective_fun() {
#line 296 "KIF.g"
	Signature::Fun* f;
#line 538 "KIFParser.cpp"
	
	switch ( LA(1)) {
	case IMPLY:
	{
		match(IMPLY);
#line 300 "KIF.g"
		
			  f = sig->createFun ("=>", 2);
			
#line 548 "KIFParser.cpp"
		break;
	}
	case EQUIVALENCE:
	{
		match(EQUIVALENCE);
#line 305 "KIF.g"
		
			  f = sig->createFun ("<=>", 2);
			
#line 558 "KIFParser.cpp"
		break;
	}
	default:
	{
		throw ANTLR_USE_NAMESPACE(antlr)NoViableAltException(LT(1), getFilename());
	}
	}
	return f;
}

Formula::Connective  KIFParser::junction() {
#line 311 "KIF.g"
	Formula::Connective c;
#line 572 "KIFParser.cpp"
	
	switch ( LA(1)) {
	case LITERAL_and:
	{
		kw_and();
#line 315 "KIF.g"
		
			  c = Formula::AND;
			
#line 582 "KIFParser.cpp"
		break;
	}
	case LITERAL_or:
	{
		kw_or();
#line 320 "KIF.g"
		
			  c = Formula::OR;
			
#line 592 "KIFParser.cpp"
		break;
	}
	default:
	{
		throw ANTLR_USE_NAMESPACE(antlr)NoViableAltException(LT(1), getFilename());
	}
	}
	return c;
}

Formula::Connective  KIFParser::quantifier() {
#line 326 "KIF.g"
	Formula::Connective c;
#line 606 "KIFParser.cpp"
	
	switch ( LA(1)) {
	case LITERAL_forall:
	{
		kw_forall();
#line 330 "KIF.g"
		
			  c = Formula::FORALL;
			
#line 616 "KIFParser.cpp"
		break;
	}
	case LITERAL_exists:
	{
		kw_exists();
#line 335 "KIF.g"
		
			  c = Formula::EXISTS;
			
#line 626 "KIFParser.cpp"
		break;
	}
	default:
	{
		throw ANTLR_USE_NAMESPACE(antlr)NoViableAltException(LT(1), getFilename());
	}
	}
	return c;
}

Signature::Fun*  KIFParser::quantifier_fun() {
#line 341 "KIF.g"
	Signature::Fun* f;
#line 640 "KIFParser.cpp"
	
	switch ( LA(1)) {
	case LITERAL_forall:
	{
		kw_forall();
#line 345 "KIF.g"
		
			  f = sig->createFun ("forall",2);
			
#line 650 "KIFParser.cpp"
		break;
	}
	case LITERAL_exists:
	{
		kw_exists();
#line 350 "KIF.g"
		
			  f = sig->createFun ("exists",2);
			
#line 660 "KIFParser.cpp"
		break;
	}
	default:
	{
		throw ANTLR_USE_NAMESPACE(antlr)NoViableAltException(LT(1), getFilename());
	}
	}
	return f;
}

void KIFParser::term_sentence(
	Term& t
) {
#line 446 "KIF.g"
	
		  TRACER ("KIFParser::term_sentence");
		
#line 678 "KIFParser.cpp"
	
	if ((LA(1)==LPAR) && (LA(2)==LITERAL_equal)) {
		term_equation(t);
	}
	else if ((LA(1)==LPAR) && ((LA(2) >= LESS && LA(2) <= GEQ))) {
		term_inequality(t);
	}
	else if ((LA(1)==LPAR) && (LA(2)==LITERAL_not)) {
		match(LPAR);
		kw_not();
		term_sentence(t);
		match(RPAR);
#line 459 "KIF.g"
		
			  t = Term (sig->createFun("not",1), TermList(t));
			
#line 695 "KIFParser.cpp"
	}
	else if ((LA(1)==LPAR) && (LA(2)==LITERAL_and||LA(2)==LITERAL_or)) {
#line 463 "KIF.g"
		
			  TermList ts;
			  Formula::Connective c;
			
#line 703 "KIFParser.cpp"
		match(LPAR);
		c=junction();
		term_sentence_plus(ts);
		match(RPAR);
#line 471 "KIF.g"
		
			  t = Term (sig->createFun(c == Formula::AND? "and" : "or", ts.length()), ts);
			
#line 712 "KIFParser.cpp"
	}
	else if ((LA(1)==LPAR) && (LA(2)==IMPLY||LA(2)==EQUIVALENCE)) {
#line 475 "KIF.g"
		
			  Term s;
			  Signature::Fun* f;
			
#line 720 "KIFParser.cpp"
		match(LPAR);
		f=binary_connective_fun();
		term_sentence(s);
		term_sentence(t);
		match(RPAR);
#line 484 "KIF.g"
		
			  t = Term (f, 
				    TermList(s, 
					     TermList(t)));
			
#line 732 "KIFParser.cpp"
	}
	else if ((LA(1)==LPAR) && (LA(2)==LITERAL_forall||LA(2)==LITERAL_exists)) {
#line 490 "KIF.g"
		
			  VarList vs;
			  TermList ts;
			  Signature::Fun* q;
			
#line 741 "KIFParser.cpp"
		match(LPAR);
		q=quantifier_fun();
		match(LPAR);
		variable_plus(vs);
		match(RPAR);
		term_sentence(t);
		match(RPAR);
#line 502 "KIF.g"
		
			  Signature::Fun* v = sig->createFun ("$qvars",vs.length());
			  ts.buildFrom (vs);
			  t = Term (q, TermList(Term(v,ts),TermList(t)));
			
#line 755 "KIFParser.cpp"
	}
	else if ((_tokenSet_0.member(LA(1))) && (_tokenSet_1.member(LA(2)))) {
		term(t);
	}
	else {
		throw ANTLR_USE_NAMESPACE(antlr)NoViableAltException(LT(1), getFilename());
	}
	
}

void KIFParser::equation(
	Atom& a
) {
#line 512 "KIF.g"
	
		  TRACER ("KIFParser::equation");
		  Term t1;
		  Term t2;
		
#line 775 "KIFParser.cpp"
	
	match(LPAR);
	kw_equal();
	term(t1);
	term(t2);
#line 523 "KIF.g"
	
		  a = Atom (t1, t2);
		
#line 785 "KIFParser.cpp"
	match(RPAR);
}

void KIFParser::inequality(
	Atom& a
) {
#line 606 "KIF.g"
	
		  TRACER ("KIFParser::inequality");
		  Signature::Pred* p;
		  Term t1;
		  Term t2;
		
#line 799 "KIFParser.cpp"
	
	match(LPAR);
	p=binary_comparison();
	term(t1);
	term(t2);
	match(RPAR);
#line 619 "KIF.g"
	
		  a = Atom (p, TermList (t1, 
			       TermList (t2)));
		
#line 811 "KIFParser.cpp"
}

void KIFParser::relsent(
	Atom& a
) {
	ANTLR_USE_NAMESPACE(antlr)RefToken  w = ANTLR_USE_NAMESPACE(antlr)nullToken;
#line 646 "KIF.g"
	
		  TRACER ("KIFParser::relsent");
		  TermList ts;
		
#line 823 "KIFParser.cpp"
	
	match(LPAR);
	w = LT(1);
	match(WORD);
	term_plus(ts);
#line 655 "KIF.g"
	
		  a = Atom ( sig->createPred (w->getText().c_str(), ts.length()),
	ts );
		
#line 834 "KIFParser.cpp"
	match(RPAR);
}

void KIFParser::sentence_plus(
	FormulaList& fs
) {
#line 663 "KIF.g"
	
		  TRACER ("KIFParser::sentence_plus");
	FormulaList rest;
	Formula f;
	
#line 847 "KIFParser.cpp"
	
	sentence(f);
	{
	switch ( LA(1)) {
	case WORD:
	case LPAR:
	{
		sentence_plus(rest);
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
#line 675 "KIF.g"
	
		  fs = FormulaList (f, rest);
		
#line 872 "KIFParser.cpp"
}

void KIFParser::variable_plus(
	VarList& ws
) {
#line 716 "KIF.g"
	
		  TRACER ("KIFParser::variable_plus");
	VarList vs;
	Var vv;
	
#line 884 "KIFParser.cpp"
	
	vv=variable();
	{
	switch ( LA(1)) {
	case VARIABLE:
	{
		variable_plus(vs);
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
#line 728 "KIF.g"
	
		  ws = VarList (vv, vs);
		
#line 908 "KIFParser.cpp"
}

void KIFParser::term_equation(
	Term& t
) {
#line 530 "KIF.g"
	
		  TRACER ("KIFParser::term_equation");
		  Term t1;
		  Term t2;
		
#line 920 "KIFParser.cpp"
	
	match(LPAR);
	kw_equal();
	term(t1);
	term(t2);
#line 541 "KIF.g"
	
		  t = Term (sig->createFun ("equal", 2),
			    TermList (t1, 
				      TermList(t2)));
		
#line 932 "KIFParser.cpp"
	match(RPAR);
}

void KIFParser::term_inequality(
	Term& t
) {
#line 626 "KIF.g"
	
		  TRACER ("KIFParser::term_inequality");
		  Signature::Fun* f;
		  Term t1;
		  Term t2;
		
#line 946 "KIFParser.cpp"
	
	match(LPAR);
	f=binary_comparison_fun();
	term(t1);
	term(t2);
	match(RPAR);
#line 639 "KIF.g"
	
		  t = Term (f, TermList (t1, 
			       TermList (t2)));
		
#line 958 "KIFParser.cpp"
}

void KIFParser::term_sentence_plus(
	TermList& ts
) {
#line 681 "KIF.g"
	
		  TRACER ("KIFParser::term_sentence_plus");
	TermList rest;
	Term t;
	
#line 970 "KIFParser.cpp"
	
	term_sentence(t);
	{
	switch ( LA(1)) {
	case WORD:
	case STRING:
	case VARIABLE:
	case NUMBER:
	case QUOTE:
	case LPAR:
	case IMPLY:
	case EQUIVALENCE:
	case LITERAL_equal:
	case LITERAL_not:
	case LITERAL_and:
	case LITERAL_or:
	case LITERAL_forall:
	case LITERAL_exists:
	{
		term_sentence_plus(rest);
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
#line 693 "KIF.g"
	
		  ts = TermList (t, rest);
		
#line 1007 "KIFParser.cpp"
}

Signature::Pred*  KIFParser::binary_comparison() {
#line 550 "KIF.g"
	Signature::Pred* p;
#line 1013 "KIFParser.cpp"
#line 550 "KIF.g"
	
		  TRACER ("KIFParser::binary_comparison");
		
#line 1018 "KIFParser.cpp"
	
	switch ( LA(1)) {
	case MORE:
	{
		match(MORE);
#line 557 "KIF.g"
		
			  p = sig->createPred (">", 2);
			
#line 1028 "KIFParser.cpp"
		break;
	}
	case LESS:
	{
		match(LESS);
#line 562 "KIF.g"
		
			  p = sig->createPred ("<", 2);
			
#line 1038 "KIFParser.cpp"
		break;
	}
	case GEQ:
	{
		match(GEQ);
#line 567 "KIF.g"
		
			  p = sig->createPred (">=", 2);
			
#line 1048 "KIFParser.cpp"
		break;
	}
	case LESSEQ:
	{
		match(LESSEQ);
#line 572 "KIF.g"
		
			  p = sig->createPred ("<=", 2);
			
#line 1058 "KIFParser.cpp"
		break;
	}
	default:
	{
		throw ANTLR_USE_NAMESPACE(antlr)NoViableAltException(LT(1), getFilename());
	}
	}
	return p;
}

Signature::Fun*  KIFParser::binary_comparison_fun() {
#line 578 "KIF.g"
	Signature::Fun* f;
#line 1072 "KIFParser.cpp"
#line 578 "KIF.g"
	
		  TRACER ("KIFParser::binary_comparison_fun");
		
#line 1077 "KIFParser.cpp"
	
	switch ( LA(1)) {
	case MORE:
	{
		match(MORE);
#line 585 "KIF.g"
		
			  f = sig->createFun (">", 2);
			
#line 1087 "KIFParser.cpp"
		break;
	}
	case LESS:
	{
		match(LESS);
#line 590 "KIF.g"
		
			  f = sig->createFun ("<", 2);
			
#line 1097 "KIFParser.cpp"
		break;
	}
	case GEQ:
	{
		match(GEQ);
#line 595 "KIF.g"
		
			  f = sig->createFun (">=", 2);
			
#line 1107 "KIFParser.cpp"
		break;
	}
	case LESSEQ:
	{
		match(LESSEQ);
#line 600 "KIF.g"
		
			  f = sig->createFun ("<=", 2);
			
#line 1117 "KIFParser.cpp"
		break;
	}
	default:
	{
		throw ANTLR_USE_NAMESPACE(antlr)NoViableAltException(LT(1), getFilename());
	}
	}
	return f;
}

const char* KIFParser::_tokenNames[] = {
	"<0>",
	"EOF",
	"<2>",
	"NULL_TREE_LOOKAHEAD",
	"UPPER",
	"LOWER",
	"DIGIT",
	"SPECIAL",
	"WHITE",
	"INITIALCHAR",
	"WORDCHAR",
	"CHARACTER",
	"TICK",
	"WORD",
	"STRING",
	"VARIABLE",
	"FIGURE",
	"NUMBER",
	"EXPONENT",
	"QUOTE",
	"SKIP_RULE",
	"COMMENT_LINE",
	"LPAR",
	"RPAR",
	"LESS",
	"MORE",
	"LESSEQ",
	"GEQ",
	"IMPLY",
	"EQUIVALENCE",
	"\"equal\"",
	"\"not\"",
	"\"and\"",
	"\"or\"",
	"\"forall\"",
	"\"exists\"",
	0
};

const unsigned long KIFParser::_tokenSet_0_data_[] = { 4031438848UL, 15UL, 0UL, 0UL };
// WORD STRING VARIABLE NUMBER QUOTE LPAR IMPLY EQUIVALENCE "equal" "not" 
// "and" "or" "forall" "exists" 
const ANTLR_USE_NAMESPACE(antlr)BitSet KIFParser::_tokenSet_0(_tokenSet_0_data_,4);
const unsigned long KIFParser::_tokenSet_1_data_[] = { 4039827456UL, 15UL, 0UL, 0UL };
// WORD STRING VARIABLE NUMBER QUOTE LPAR RPAR IMPLY EQUIVALENCE "equal" 
// "not" "and" "or" "forall" "exists" 
const ANTLR_USE_NAMESPACE(antlr)BitSet KIFParser::_tokenSet_1(_tokenSet_1_data_,4);


