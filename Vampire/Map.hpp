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
//  file Map.hpp 
//  defines class Map<Key,Val> of arbitrary maps
//    Key can be a pointer or integral value: anything that can be cast
//    to integer and compared using ==
//

#ifndef __Map__
#define __Map__


#include "assert.hpp"


extern int _mapLengths[];


template <typename Key, typename Val>
class Map 
{
 public:
  Map ();
  ~Map ();
  bool find (Key key, Val& value);
  void insert (Key key, Val value);

 private:
  class Entry
  {
  public:
    Entry ()
      : _occupied (false)
    {
    } // Map::Entry::Entry

    void fill (Key key, Val value)
    {
      TRACER ("Map::Entry::fill");

      _occupied = true;
      _key = key;
      _value = value;
    } // Map::Entry::fill

    bool occupied () const
    {
      return _occupied;
    } // Map::Entry::occupied

    Key key () const { return _key; }
    Val value () const { return _value; }

  private:
    bool _occupied;
    Key _key;
    Val _value;
  }; // class Map::Entry

  // structure
  int* _lengthPointer;
  int _length;
  int _noOfEntries;
  Entry* _entries;
  Entry* _afterLast; // entry after the last one
  int _maxEntries;

  void expand ();
  Entry* findEntry (Key); // find entry for the key
}; // class Map




// ******************* Map definitions ************************

template <class Key, class Val>
Map<Key,Val>::Map ()
  : _lengthPointer (_mapLengths),
    _noOfEntries (0),
    _entries (0)
{
  TRACER ("Map::Map");

  expand ();
} // Map::Map


// expand the map to the next available size
// 29/09/2002 Manchester
template <class Key, class Val>
void Map<Key, Val>::expand ()
{
  TRACER ("Map::expand");

  if (_lengthPointer[1] == 0) {
    // no way to expand the array itself
    throw MyException ("cannot expand a map");
  }

  _lengthPointer ++;
  _length = *_lengthPointer;

  Entry* oldEntries = _entries;
  _entries = new Entry [_length];
  if (! _entries) {
    throw MyException ("insufficient memory when allocating a map");
  }

  _afterLast = _entries + _length;
  _maxEntries = (int)(_length * 0.7);
  // experiments using random numbers: 38,000,000 allocations 
  // and 30,000,000 allocations (+ 100,000,000 chechs in both cases)
  // 0.5 : 32.21, 28.01
  // 0.6 : 33.61, 29.61
  // 0.7 : 35.51, 31.31
  // 0.8 : 37.71, 29,91* one less allocation
  // 0.9 : 41.11, 31,61* 
  // copy old entries
  Entry* current = oldEntries;
  int remaining = _noOfEntries;
  _noOfEntries = 0;
  while (remaining != 0) {
    // find first occupied entry
    while (! current->occupied()) {
      current ++;
    }
    // now current is occupied
    insert (current->key(), current->value());
    current ++;
    remaining --;
  }

  delete [] oldEntries;
} // Map::expand


template <class Key, class Val>
inline
Map<Key,Val>::~Map ()
{
  TRACER ("Map::~Map");

  delete [] _entries;
} // Map::~Map


// find value by the key. The result is true if a pair with this key
// is in the map. If such a pair is found then its value is
// returned in found
// 29/09/2002 Manchester
template <class Key, class Val>
bool Map<Key,Val>::find (Key key, Val& found)
{
  TRACER( "Map::find" );
  Entry* entry = findEntry (key);

  if (entry->occupied()) {
    found = entry->value();
    return true;
  }

  return false;
} // Map::find


// add pair (key,value) to the map. 
// WARNING: a value under this key must not be in the table!
// 29/09/2002 Manchester
template <class Key, class Val>
void Map<Key,Val>::insert (Key key, Val value)
{
  TRACER( "Map::insert" );

  if (_noOfEntries > _maxEntries) { // too many entries
    expand ();
  }

  Entry* entry = findEntry (key);

  ASS (! entry->occupied());

  entry->fill (key, value);
  _noOfEntries ++;
} // Map::insert


// compute hash value of a key and return the entry 
// (occupied or not) for this key
// 29/09/2002 Manchester
template <class Key, class Val>
Map<Key, Val>::Entry* Map<Key, Val>::findEntry (Key key)
{
  TRACER( "Map::findEntry" );

  div_t d = div((int)key, _length);
  Entry* entry = _entries + d.rem;
  while ( entry->occupied() ) {
    if (entry->key() == key) {
      return entry;
    }

    entry ++;
    // check if the entry is a valid one
    if (entry ==_afterLast) {
      entry =_entries;
    }
  }

  // found non-occupied entry
  return entry;
} // Map::findEntry




// **************** Map::Entry definitions *********************

/*
template <class Key, class Val>
inline
Map<Key,Val>::Entry::Entry ()
  : _occupied (false)
{
} // Map::Entry::Entry


template <class Key, class Val>
inline
bool Map<Key,Val>::Entry::occupied () const
{
  return _occupied;
} // Map::Entry::occupied


template <class Key, class Val>
inline
Key Map<Key,Val>::Entry::key () const
{
  return _key;
} // Map::Entry::key


template <class Key, class Val>
inline
Val Map<Key,Val>::Entry::value () const
{
  return _value;
} // Map::Entry::value


template <class Key, class Val>
void Map<Key,Val>::Entry::fill (Key key, Val value)
{
  TRACER ("Map::Entry::fill");

  _occupied = true;
  _key = key;
  _value = value;
} // Map::Entry::fill
*/

#endif // __lst__

