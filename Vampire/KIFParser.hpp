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

#ifndef INC_KIFParser_hpp_
#define INC_KIFParser_hpp_

#include "antlr/config.hpp"
/* $ANTLR 2.7.1: "KIF.g" -> "KIFParser.hpp"$ */
#include "antlr/TokenStream.hpp"
#include "antlr/TokenBuffer.hpp"
#include "KIFLexerTokenTypes.hpp"
#include "antlr/LLkParser.hpp"

#line 20 "KIF.g"

  // head of lexer file
#include <iostream>
#include "Formula.hpp"

#line 18 "KIFParser.hpp"
class KIFParser : public ANTLR_USE_NAMESPACE(antlr)LLkParser, public KIFLexerTokenTypes
 {
#line 162 "KIF.g"

#line 22 "KIFParser.hpp"
protected:
	KIFParser(ANTLR_USE_NAMESPACE(antlr)TokenBuffer& tokenBuf, int k);
public:
	KIFParser(ANTLR_USE_NAMESPACE(antlr)TokenBuffer& tokenBuf);
protected:
	KIFParser(ANTLR_USE_NAMESPACE(antlr)TokenStream& lexer, int k);
public:
	KIFParser(ANTLR_USE_NAMESPACE(antlr)TokenStream& lexer);
	KIFParser(const ANTLR_USE_NAMESPACE(antlr)ParserSharedInputState& state);
	public: void start(
		FormulaList& formulas
	);
	public: void sentence(
		Formula& f
	);
	public: void term(
		Term& t
	);
	public: Var  variable();
	public: void funterm(
		Term& t
	);
	public: void quoted_term(
		Term& t
	);
	public: void kw_and();
	public: void kw_or();
	public: void kw_not();
	public: void kw_forall();
	public: void kw_exists();
	public: void kw_equal();
	public: void term_plus(
		TermList& ts
	);
	public: Formula::Connective  binary_connective();
	public: Signature::Fun*  binary_connective_fun();
	public: Formula::Connective  junction();
	public: Formula::Connective  quantifier();
	public: Signature::Fun*  quantifier_fun();
	public: void term_sentence(
		Term& t
	);
	public: void equation(
		Atom& a
	);
	public: void inequality(
		Atom& a
	);
	public: void relsent(
		Atom& a
	);
	public: void sentence_plus(
		FormulaList& fs
	);
	public: void variable_plus(
		VarList& ws
	);
	public: void term_equation(
		Term& t
	);
	public: void term_inequality(
		Term& t
	);
	public: void term_sentence_plus(
		TermList& ts
	);
	public: Signature::Pred*  binary_comparison();
	public: Signature::Fun*  binary_comparison_fun();
private:
	static const char* _tokenNames[];
	
	static const unsigned long _tokenSet_0_data_[];
	static const ANTLR_USE_NAMESPACE(antlr)BitSet _tokenSet_0;
	static const unsigned long _tokenSet_1_data_[];
	static const ANTLR_USE_NAMESPACE(antlr)BitSet _tokenSet_1;
};

#endif /*INC_KIFParser_hpp_*/
