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

/* $ANTLR 2.7.1: "XML.g" -> "XMLParser.cpp"$ */
#include "XMLParser.hpp"
#include "antlr/NoViableAltException.hpp"
#include "antlr/SemanticException.hpp"
#line 100 "XML.g"

// head of parser file
#include "KIFParser.hpp"

#include "antlr/TokenStreamSelector.hpp"
extern antlr::TokenStreamSelector* selector;


#line 15 "XMLParser.cpp"
XMLParser::XMLParser(ANTLR_USE_NAMESPACE(antlr)TokenBuffer& tokenBuf, int k)
: ANTLR_USE_NAMESPACE(antlr)LLkParser(tokenBuf,k)
{
	setTokenNames(_tokenNames);
}

XMLParser::XMLParser(ANTLR_USE_NAMESPACE(antlr)TokenBuffer& tokenBuf)
: ANTLR_USE_NAMESPACE(antlr)LLkParser(tokenBuf,2)
{
	setTokenNames(_tokenNames);
}

XMLParser::XMLParser(ANTLR_USE_NAMESPACE(antlr)TokenStream& lexer, int k)
: ANTLR_USE_NAMESPACE(antlr)LLkParser(lexer,k)
{
	setTokenNames(_tokenNames);
}

XMLParser::XMLParser(ANTLR_USE_NAMESPACE(antlr)TokenStream& lexer)
: ANTLR_USE_NAMESPACE(antlr)LLkParser(lexer,2)
{
	setTokenNames(_tokenNames);
}

XMLParser::XMLParser(const ANTLR_USE_NAMESPACE(antlr)ParserSharedInputState& state)
: ANTLR_USE_NAMESPACE(antlr)LLkParser(state,2)
{
	setTokenNames(_tokenNames);
}

Query*  XMLParser::start() {
#line 135 "XML.g"
	Query* q;
#line 49 "XMLParser.cpp"
	
	if ((LA(1)==START_TAG) && (LA(2)==LITERAL_query)) {
		q=query();
	}
	else if ((LA(1)==START_TAG) && (LA(2)==LITERAL_assertion)) {
		q=assertion();
	}
	else if ((LA(1)==ANTLR_USE_NAMESPACE(antlr)Token::EOF_TYPE||LA(1)==START_TAG) && (LA(2)==ANTLR_USE_NAMESPACE(antlr)Token::EOF_TYPE||LA(2)==LITERAL_bye)) {
		terminate();
#line 143 "XML.g"
		
			  q = new Query ();
			
#line 63 "XMLParser.cpp"
	}
	else {
		throw ANTLR_USE_NAMESPACE(antlr)NoViableAltException(LT(1), getFilename());
	}
	
	return q;
}

Query*  XMLParser::query() {
#line 158 "XML.g"
	Query* query;
#line 75 "XMLParser.cpp"
#line 158 "XML.g"
	
		  Query::Attribute* attrs;
		  Formula formula;
		
#line 81 "XMLParser.cpp"
	
	match(START_TAG);
	kw_query();
	attrs=attributes();
	match(END_OF_TAG);
#line 168 "XML.g"
	//  !!! KIF parser invocation !!!
	// switch lexers
	selector->push("KIFLexer");
	// create a parser to handle the KIF structures
	KIFParser kifParser (getInputState());
	kifParser.sentence(formula); // go parse KIF
	// switch lexers back
	selector->pop();
	query = new Query (Query::GOAL, formula, attrs);
	
#line 98 "XMLParser.cpp"
	match(START_OF_END_TAG);
	kw_query();
	match(END_OF_TAG);
	return query;
}

Query*  XMLParser::assertion() {
#line 181 "XML.g"
	Query* query;
#line 108 "XMLParser.cpp"
#line 181 "XML.g"
	
		  Query::Attribute* attrs;
		  Formula formula;
		
#line 114 "XMLParser.cpp"
	
	match(START_TAG);
	kw_assertion();
	attrs=attributes();
	match(END_OF_TAG);
#line 191 "XML.g"
	//  !!! KIF parser invocation !!!
	// switch lexers
	selector->push("KIFLexer");
	// create a parser to handle the KIF structures
	KIFParser kifParser (getInputState());
	kifParser.sentence(formula); // go parse KIF
	// switch lexers back
	selector->pop();
	query = new Query (Query::ASSERTION, formula, attrs);
	
#line 131 "XMLParser.cpp"
	match(START_OF_END_TAG);
	kw_assertion();
	match(END_OF_TAG);
	return query;
}

void XMLParser::terminate() {
	
	switch ( LA(1)) {
	case START_TAG:
	{
		match(START_TAG);
		kw_bye();
		match(END_OF_EMPTY_TAG);
		break;
	}
	case ANTLR_USE_NAMESPACE(antlr)Token::EOF_TYPE:
	{
		match(ANTLR_USE_NAMESPACE(antlr)Token::EOF_TYPE);
		break;
	}
	default:
	{
		throw ANTLR_USE_NAMESPACE(antlr)NoViableAltException(LT(1), getFilename());
	}
	}
}

void XMLParser::kw_bye() {
	
	match(LITERAL_bye);
}

void XMLParser::kw_query() {
	
	match(LITERAL_query);
}

Query::Attribute*  XMLParser::attributes() {
#line 206 "XML.g"
	Query::Attribute* attrs;
#line 173 "XMLParser.cpp"
#line 206 "XML.g"
	
		  Query::Attribute* rest = 0;
		
#line 178 "XMLParser.cpp"
	
	switch ( LA(1)) {
	case NAME:
	{
		attrs=attribute();
		rest=attributes();
#line 214 "XML.g"
		
			  attrs->next (rest);
			
#line 189 "XMLParser.cpp"
		break;
	}
	case END_OF_TAG:
	{
#line 218 "XML.g"
		
			  attrs = 0;
			
#line 198 "XMLParser.cpp"
		break;
	}
	default:
	{
		throw ANTLR_USE_NAMESPACE(antlr)NoViableAltException(LT(1), getFilename());
	}
	}
	return attrs;
}

void XMLParser::kw_assertion() {
	
	match(LITERAL_assertion);
}

Query::Attribute*  XMLParser::attribute() {
#line 224 "XML.g"
	Query::Attribute* attr;
#line 217 "XMLParser.cpp"
	ANTLR_USE_NAMESPACE(antlr)RefToken  n = ANTLR_USE_NAMESPACE(antlr)nullToken;
	ANTLR_USE_NAMESPACE(antlr)RefToken  st = ANTLR_USE_NAMESPACE(antlr)nullToken;
	
	n = LT(1);
	match(NAME);
	match(EQ);
	st = LT(1);
	match(STRING);
#line 230 "XML.g"
	
		  attr = new Query::Attribute ( n->getText(), st->getText());
		
#line 230 "XMLParser.cpp"
	return attr;
}

void XMLParser::recover() {
	
	{
	switch ( LA(1)) {
	case START_OF_END_TAG:
	{
		match(START_OF_END_TAG);
		break;
	}
	case LITERAL_query:
	case LITERAL_assertion:
	{
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
	case LITERAL_assertion:
	{
		kw_assertion();
		break;
	}
	case LITERAL_query:
	{
		kw_query();
		break;
	}
	default:
	{
		throw ANTLR_USE_NAMESPACE(antlr)NoViableAltException(LT(1), getFilename());
	}
	}
	}
	match(END_OF_TAG);
}

const char* XMLParser::_tokenNames[] = {
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
	"LETTER",
	"NAME",
	"<",
	"</",
	">",
	"/>",
	"=",
	"COMMENT",
	"\"query\"",
	"\"assertion\"",
	"\"bye\"",
	0
};



