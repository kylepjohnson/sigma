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

#ifndef INC_KIFLexer_hpp_
#define INC_KIFLexer_hpp_

#include "antlr/config.hpp"
/* $ANTLR 2.7.1: "KIF.g" -> "KIFLexer.hpp"$ */
#include "antlr/CommonToken.hpp"
#include "antlr/InputBuffer.hpp"
#include "antlr/BitSet.hpp"
#include "KIFLexerTokenTypes.hpp"
#include "antlr/CharScanner.hpp"
#line 20 "KIF.g"

  // head of lexer file
#include <iostream>
#include "Formula.hpp"

#line 18 "KIFLexer.hpp"
class KIFLexer : public ANTLR_USE_NAMESPACE(antlr)CharScanner, public KIFLexerTokenTypes
 {
#line 1 "KIF.g"
#line 22 "KIFLexer.hpp"
private:
	void initLiterals();
public:
	bool getCaseSensitiveLiterals() const;
public:
	KIFLexer(ANTLR_USE_NAMESPACE(std)istream& in);
	KIFLexer(ANTLR_USE_NAMESPACE(antlr)InputBuffer& ib);
	KIFLexer(const ANTLR_USE_NAMESPACE(antlr)LexerSharedInputState& state);
	ANTLR_USE_NAMESPACE(antlr)RefToken nextToken();
	protected: void mUPPER(bool _createToken);
	protected: void mLOWER(bool _createToken);
	protected: void mDIGIT(bool _createToken);
	protected: void mSPECIAL(bool _createToken);
	protected: void mWHITE(bool _createToken);
	protected: void mINITIALCHAR(bool _createToken);
	protected: void mWORDCHAR(bool _createToken);
	protected: void mCHARACTER(bool _createToken);
	protected: void mTICK(bool _createToken);
	public: void mWORD(bool _createToken);
	public: void mSTRING(bool _createToken);
	public: void mVARIABLE(bool _createToken);
	protected: void mFIGURE(bool _createToken);
	public: void mNUMBER(bool _createToken);
	protected: void mEXPONENT(bool _createToken);
	public: void mQUOTE(bool _createToken);
	public: void mSKIP_RULE(bool _createToken);
	public: void mCOMMENT_LINE(bool _createToken);
	public: void mLPAR(bool _createToken);
	public: void mRPAR(bool _createToken);
	public: void mLESS(bool _createToken);
	public: void mMORE(bool _createToken);
	public: void mLESSEQ(bool _createToken);
	public: void mGEQ(bool _createToken);
	public: void mIMPLY(bool _createToken);
	public: void mEQUIVALENCE(bool _createToken);
private:
	
	static const unsigned long _tokenSet_0_data_[];
	static const ANTLR_USE_NAMESPACE(antlr)BitSet _tokenSet_0;
	static const unsigned long _tokenSet_1_data_[];
	static const ANTLR_USE_NAMESPACE(antlr)BitSet _tokenSet_1;
	static const unsigned long _tokenSet_2_data_[];
	static const ANTLR_USE_NAMESPACE(antlr)BitSet _tokenSet_2;
};

#endif /*INC_KIFLexer_hpp_*/
