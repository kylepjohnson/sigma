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

#ifndef INC_XMLParser_hpp_
#define INC_XMLParser_hpp_

#include "antlr/config.hpp"
/* $ANTLR 2.7.1: "XML.g" -> "XMLParser.hpp"$ */
#include "antlr/TokenStream.hpp"
#include "antlr/TokenBuffer.hpp"
#include "XMLLexerTokenTypes.hpp"
#include "antlr/LLkParser.hpp"

#line 20 "XML.g"

  // head of lexer file
#include <iostream>
#include "Query.hpp"

#line 18 "XMLParser.hpp"
class XMLParser : public ANTLR_USE_NAMESPACE(antlr)LLkParser, public XMLLexerTokenTypes
 {
#line 117 "XML.g"

#line 22 "XMLParser.hpp"
protected:
	XMLParser(ANTLR_USE_NAMESPACE(antlr)TokenBuffer& tokenBuf, int k);
public:
	XMLParser(ANTLR_USE_NAMESPACE(antlr)TokenBuffer& tokenBuf);
protected:
	XMLParser(ANTLR_USE_NAMESPACE(antlr)TokenStream& lexer, int k);
public:
	XMLParser(ANTLR_USE_NAMESPACE(antlr)TokenStream& lexer);
	XMLParser(const ANTLR_USE_NAMESPACE(antlr)ParserSharedInputState& state);
	public: Query*  start();
	public: Query*  query();
	public: Query*  assertion();
	public: void terminate();
	public: void kw_bye();
	public: void kw_query();
	public: Query::Attribute*  attributes();
	public: void kw_assertion();
	public: Query::Attribute*  attribute();
	public: void recover();
private:
	static const char* _tokenNames[];
	
};

#endif /*INC_XMLParser_hpp_*/
