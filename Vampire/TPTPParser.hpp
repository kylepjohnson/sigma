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

#ifndef INC_TPTPParser_hpp_
#define INC_TPTPParser_hpp_

#include "antlr/config.hpp"
/* $ANTLR 2.7.1: "TPTP.g" -> "TPTPParser.hpp"$ */
#include "antlr/TokenStream.hpp"
#include "antlr/TokenBuffer.hpp"
#include "TPTPLexerTokenTypes.hpp"
#include "antlr/LLkParser.hpp"

#line 19 "TPTP.g"

#include <iostream>
#include "Unit.hpp"

#line 17 "TPTPParser.hpp"
class TPTPParser : public ANTLR_USE_NAMESPACE(antlr)LLkParser, public TPTPLexerTokenTypes
 {
#line 143 "TPTP.g"

#line 21 "TPTPParser.hpp"
protected:
	TPTPParser(ANTLR_USE_NAMESPACE(antlr)TokenBuffer& tokenBuf, int k);
public:
	TPTPParser(ANTLR_USE_NAMESPACE(antlr)TokenBuffer& tokenBuf);
protected:
	TPTPParser(ANTLR_USE_NAMESPACE(antlr)TokenStream& lexer, int k);
public:
	TPTPParser(ANTLR_USE_NAMESPACE(antlr)TokenStream& lexer);
	TPTPParser(const ANTLR_USE_NAMESPACE(antlr)ParserSharedInputState& state);
	public: void units(
		UnitList& us
	);
	public: void include(
		UnitList& us
	);
	public: void unit(
		Unit& unit
	);
	public: void kw_include();
	public: void kw_formula();
	public: InputType  type();
	public: void formula(
		Formula& f
	);
	public: void kw_clause();
	public: void clause(
		Clause& c
	);
	public: void xorformula(
		Formula& f
	);
	public: void literals(
		LiteralList& ls
	);
	public: void impformula(
		Formula& f
	);
	public: void literal(
		Literal& l
	);
	public: void orformula(
		Formula& f
	);
	public: bool  sign();
	public: void atom(
		Atom& a
	);
	public: void andformula(
		Formula& f
	);
	public: void terms(
		TermList& ts
	);
	public: void simple_formula(
		Formula& f
	);
	public: void term(
		Term& t
	);
	public: void varlist(
		VarList& vs
	);
	public: void vars(
		VarList& vs
	);
private:
	static const char* _tokenNames[];
	
};

#endif /*INC_TPTPParser_hpp_*/
