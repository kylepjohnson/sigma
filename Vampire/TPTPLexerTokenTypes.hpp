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

#ifndef INC_TPTPLexerTokenTypes_hpp_
#define INC_TPTPLexerTokenTypes_hpp_

/* $ANTLR 2.7.1: "TPTP.g" -> "TPTPLexerTokenTypes.hpp"$ */
struct TPTPLexerTokenTypes {
	enum {
		EOF_ = 1,
		UPPER = 4,
		LOWER = 5,
		DIGIT = 6,
		WHITE = 7,
		NUMBER = 8,
		WORDCHAR = 9,
		CHARACTER = 10,
		SKIP_RULE = 11,
		COMMENT_LINE = 12,
		QUOTE = 13,
		FILE_NAME = 14,
		LPAR = 15,
		RPAR = 16,
		LBRA = 17,
		RBRA = 18,
		COMMA = 19,
		COLON = 20,
		DOT = 21,
		AND = 22,
		NOT = 23,
		OR = 24,
		IFF = 25,
		IMP = 26,
		XOR = 27,
		FORALL = 28,
		EXISTS = 29,
		PP = 30,
		MM = 31,
		NAME = 32,
		VAR = 33,
		LITERAL_input_formula = 34,
		LITERAL_input_clause = 35,
		LITERAL_include = 36,
		NULL_TREE_LOOKAHEAD = 3
	};
};
#endif /*INC_TPTPLexerTokenTypes_hpp_*/
