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

//
//  file query.h 
//  defines class Query of KIF XML queries
//


#ifndef __query__
#define __query__


#include <string>


#include "Formula.hpp"


class Query {
 public:
  class Attribute;
  enum QueryType {
    ASSERTION,
    GOAL,
    TERMINATE
  };

  // constructors
  Query (QueryType qtype, const Formula& formula, Attribute* attributes);
  Query () : _type (TERMINATE) {}

  // query structure
  QueryType type () const { return _type; }
  const Formula& formula () const { return _formula; }
  int timeLimit () const { return _timeLimit; }
  int bindingsLimit () const { return _bindingsLimit; }

 private:
  // structure
  QueryType _type; // a query can be a goal or an assertion
  Formula _formula;
  Attribute* _attributes;
  int _timeLimit;
  int _depthLimit;
  int _bindingsLimit;
}; // class Query


class Query::Attribute {
 public:
  // constructors
  Attribute (const string name, const string value) :
    _name (name),
    _value (value)
    {}

  // query the structure
  const string name () const { return _name; }
  const string value () const { return _value; }
  Attribute* next () const { return _next; }
  void next (Attribute* att) { _next = att; }
 private:
  const string _name;
  const string _value;
  Attribute* _next;
}; // class Query::Attribute


#endif // __query_
