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
 */

//
//  file query.cpp
//  implements class Query of KIF XML queries
//

#include <cstdlib>

#include "Query.hpp"

const string timeLimitString = "timeLimit";
const string depthLimitString = "depthLimit";
const string bindingsLimitString = "bindingsLimit";


// build a query and read values from attributes
// 04/08/2002 TOrrevieja
Query::Query (QueryType qtype, const Formula& formula, Attribute* attributes)
  :
  _type (qtype),
  _formula (formula),
  _attributes (attributes),
  _timeLimit (5),
  _depthLimit (-1),
  _bindingsLimit (5)
{
  if (qtype == GOAL) { // query, attributes should be sent
    for (Attribute* attr = attributes; attr; attr = attr->next()) {
      if (attr->name() == timeLimitString) {
	// truncate quotes
	char* endptr; 
	_timeLimit = strtoul (attr->value().data(), &endptr, 10);
      }
      else if (attr->name() == depthLimitString) {
	// truncate quotes
	char* endptr;
	_depthLimit = strtoul (attr->value().data(), &endptr, 10);
      }
      else if (attr->name() == bindingsLimitString) {
	// truncate quotes
	char* endptr = 0; // 
	_bindingsLimit = strtoul (attr->value().data(), &endptr, 10);
      }
    }
  }
} // Query::Query


