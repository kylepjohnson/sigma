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

#ifndef INC_XMLLexer_hpp_
#define INC_XMLLexer_hpp_

#include "antlr/config.hpp"
/* $ANTLR 2.7.1: "XML.g" -> "XMLLexer.hpp"$ */
#include "antlr/CommonToken.hpp"
#include "antlr/InputBuffer.hpp"
#include "antlr/BitSet.hpp"
#include "XMLLexerTokenTypes.hpp"
#include "antlr/CharScanner.hpp"
#line 20 "XML.g"

  // head of lexer file
#include <iostream>
#include "Query.hpp"

#line 18 "XMLLexer.hpp"
class XMLLexer : public ANTLR_USE_NAMESPACE(antlr)CharScanner, public XMLLexerTokenTypes
 {
#line 1 "XML.g"
#line 22 "XMLLexer.hpp"
private:
	void initLiterals();
public:
	bool getCaseSensitiveLiterals() const;
public:
	XMLLexer(ANTLR_USE_NAMESPACE(std)istream& in);
	XMLLexer(ANTLR_USE_NAMESPACE(antlr)InputBuffer& ib);
	XMLLexer(const ANTLR_USE_NAMESPACE(antlr)LexerSharedInputState& state);
	ANTLR_USE_NAMESPACE(antlr)RefToken nextToken();
	protected: void mLETTER(bool _createToken);
	protected: void mDIGIT(bool _createToken);
	public: void mNAME(bool _createToken);
	public: void mSTART_TAG(bool _createToken);
	public: void mSTART_OF_END_TAG(bool _createToken);
	public: void mEND_OF_TAG(bool _createToken);
	public: void mEND_OF_EMPTY_TAG(bool _createToken);
	public: void mEQ(bool _createToken);
	public: void mSTRING(bool _createToken);
	public: void mCOMMENT(bool _createToken);
	public: void mWHITE(bool _createToken);
private:
	
	static const unsigned long _tokenSet_0_data_[];
	static const ANTLR_USE_NAMESPACE(antlr)BitSet _tokenSet_0;
	static const unsigned long _tokenSet_1_data_[];
	static const ANTLR_USE_NAMESPACE(antlr)BitSet _tokenSet_1;
	static const unsigned long _tokenSet_2_data_[];
	static const ANTLR_USE_NAMESPACE(antlr)BitSet _tokenSet_2;
};

#endif /*INC_XMLLexer_hpp_*/
