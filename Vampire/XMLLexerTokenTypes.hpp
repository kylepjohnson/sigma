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

#ifndef INC_XMLLexerTokenTypes_hpp_
#define INC_XMLLexerTokenTypes_hpp_

/* $ANTLR 2.7.1: "XML.g" -> "XMLLexerTokenTypes.hpp"$ */
struct XMLLexerTokenTypes {
	enum {
		EOF_ = 1,
		UPPER = 4,
		LOWER = 5,
		DIGIT = 6,
		SPECIAL = 7,
		WHITE = 8,
		INITIALCHAR = 9,
		WORDCHAR = 10,
		CHARACTER = 11,
		TICK = 12,
		WORD = 13,
		STRING = 14,
		VARIABLE = 15,
		FIGURE = 16,
		NUMBER = 17,
		EXPONENT = 18,
		QUOTE = 19,
		SKIP_RULE = 20,
		COMMENT_LINE = 21,
		LPAR = 22,
		RPAR = 23,
		LESS = 24,
		MORE = 25,
		LESSEQ = 26,
		GEQ = 27,
		IMPLY = 28,
		EQUIVALENCE = 29,
		LITERAL_equal = 30,
		LITERAL_not = 31,
		LITERAL_and = 32,
		LITERAL_or = 33,
		LITERAL_forall = 34,
		LITERAL_exists = 35,
		LETTER = 36,
		NAME = 37,
		START_TAG = 38,
		START_OF_END_TAG = 39,
		END_OF_TAG = 40,
		END_OF_EMPTY_TAG = 41,
		EQ = 42,
		COMMENT = 43,
		LITERAL_query = 44,
		LITERAL_assertion = 45,
		LITERAL_bye = 46,
		NULL_TREE_LOOKAHEAD = 3
	};
};
#endif /*INC_XMLLexerTokenTypes_hpp_*/
