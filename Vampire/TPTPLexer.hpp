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

#ifndef INC_TPTPLexer_hpp_
#define INC_TPTPLexer_hpp_

#include "antlr/config.hpp"
/* $ANTLR 2.7.1: "TPTP.g" -> "TPTPLexer.hpp"$ */
#include "antlr/CommonToken.hpp"
#include "antlr/InputBuffer.hpp"
#include "antlr/BitSet.hpp"
#include "TPTPLexerTokenTypes.hpp"
#include "antlr/CharScanner.hpp"
#line 19 "TPTP.g"

#include <iostream>
#include "Unit.hpp"

#line 17 "TPTPLexer.hpp"
class TPTPLexer : public ANTLR_USE_NAMESPACE(antlr)CharScanner, public TPTPLexerTokenTypes
 {
#line 1 "TPTP.g"
#line 21 "TPTPLexer.hpp"
private:
	void initLiterals();
public:
	bool getCaseSensitiveLiterals() const;
public:
	TPTPLexer(ANTLR_USE_NAMESPACE(std)istream& in);
	TPTPLexer(ANTLR_USE_NAMESPACE(antlr)InputBuffer& ib);
	TPTPLexer(const ANTLR_USE_NAMESPACE(antlr)LexerSharedInputState& state);
	ANTLR_USE_NAMESPACE(antlr)RefToken nextToken();
	protected: void mUPPER(bool _createToken);
	protected: void mLOWER(bool _createToken);
	protected: void mDIGIT(bool _createToken);
	protected: void mWHITE(bool _createToken);
	protected: void mNUMBER(bool _createToken);
	protected: void mWORDCHAR(bool _createToken);
	protected: void mCHARACTER(bool _createToken);
	public: void mSKIP_RULE(bool _createToken);
	public: void mCOMMENT_LINE(bool _createToken);
	protected: void mQUOTE(bool _createToken);
	public: void mFILE_NAME(bool _createToken);
	public: void mLPAR(bool _createToken);
	public: void mRPAR(bool _createToken);
	public: void mLBRA(bool _createToken);
	public: void mRBRA(bool _createToken);
	public: void mCOMMA(bool _createToken);
	public: void mCOLON(bool _createToken);
	public: void mDOT(bool _createToken);
	public: void mAND(bool _createToken);
	public: void mNOT(bool _createToken);
	public: void mOR(bool _createToken);
	public: void mIFF(bool _createToken);
	public: void mIMP(bool _createToken);
	public: void mXOR(bool _createToken);
	public: void mFORALL(bool _createToken);
	public: void mEXISTS(bool _createToken);
	public: void mPP(bool _createToken);
	public: void mMM(bool _createToken);
	public: void mNAME(bool _createToken);
	public: void mVAR(bool _createToken);
private:
	
	static const unsigned long _tokenSet_0_data_[];
	static const ANTLR_USE_NAMESPACE(antlr)BitSet _tokenSet_0;
	static const unsigned long _tokenSet_1_data_[];
	static const ANTLR_USE_NAMESPACE(antlr)BitSet _tokenSet_1;
	static const unsigned long _tokenSet_2_data_[];
	static const ANTLR_USE_NAMESPACE(antlr)BitSet _tokenSet_2;
};

#endif /*INC_TPTPLexer_hpp_*/
