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
//  File IntNameTable.cpp
//
//  Implements the class IntNameTable for a table of names
//


#include <string>


#include "IntNameTable.hpp"


const int IntNameTable::_noOfBuckets = 2039;
IntNameTable* IntNameTable::vars = new IntNameTable;


IntNameTable::IntNameTable ()
  :
  _buckets ( new EntryList* [_noOfBuckets] ),
  _nextNumber ( 0 )
{
  if ( ! _buckets ) {
    NO_MEMORY;
  }

  // initialize all buckets to empty
  for ( int i = _noOfBuckets - 1; i >= 0; i-- ) 
    _buckets [i] = EntryList::empty ();
} // IntNameTable::IntNameTable


int IntNameTable::insert ( const char* str )
{
  int bucket = hash ( str );

  for ( EntryList* l = _buckets [ bucket ]; ! l->isEmpty (); l = l->tail() ) {
    if ( ! strcmp (str, l->head()->right()) ) { // found
      return l->head()->left();
    }
  }

  char* copy = new char [strlen (str) + 1];
  if (! copy) {
    NO_MEMORY;
  }

  strcpy ( copy, str );
  int result = _nextNumber;
  _nextNumber ++;

  Entry* newEntry = new Entry ( result, copy );

  EntryList::push ( newEntry, _buckets [bucket] );

  return result;
} // IntNameTable::insert


IntNameTable::~IntNameTable ()
{
  // destroy all buckets and strings in them
  for ( int i = _noOfBuckets - 1; i >= 0; i-- ) {
    // delete all strings
    for ( EntryList* lst = _buckets [i]; ! lst->isEmpty (); lst = lst->tail() ) {
      delete [] lst->head()->right();
      delete lst->head ();
    }

    
    // destroy the bucket itself
    _buckets [i] ->destroy ();
  }

  delete [] _buckets;
} // IntNameTable::~IntNameTable


// very simple 
int IntNameTable::hash ( const char* str )
{
  int result = 0;

  for ( const char* s = str ; *s != 0; s++ ) 
    result = ( result * 10 + *s ) % _noOfBuckets;
  
  return result;
} // IntNameTable::hash


// return name of the entry with the number num
// 03/08/2002 Torrevieja
const char* IntNameTable::operator[] ( int num ) const
{
  for ( int i = _noOfBuckets - 1; i >= 0; i-- ) {
    // delete all strings
    for ( EntryList* lst = _buckets [i]; ! lst->isEmpty (); lst = lst->tail() ) {
      if (lst->head()->left() == num) { // found
	return lst->head()->right();
      }
    }
  }
} // IntNameTable::operator[]
