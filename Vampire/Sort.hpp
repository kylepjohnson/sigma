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

// *************************************************************
//
//  Class Sort<C,n> for sorting
//  06/05/2002 
//
// *************************************************************


#ifndef __sort__
#  define __sort__


#include "Memory.hpp"


// ******************* Class CList *******************************


template <class C>
class Sort
{
 public:

  inline
  Sort (int length) 
    :
    _elems ( new C[length] ),
    _size (length),
    _length (0)
    {
      if (! _elems) {
        NO_MEMORY;
      }
    }

  ~Sort () 
    {
      delete [] _elems;
    }

  inline int length () const 
    { return _length; }

  // note that this operator can only return values
  inline C operator [] (int n) const
    { 
      ASS(n < _length);
      return _elems[n]; 
    } // Sort::operator []

  inline void add (C c)
    {
      ASS(_length < _size);

      _elems[_length++] = c;
    } // Sort::add

  // remove duplicates from sorted list
  void removeDuplicates ()
    {
      int cur = 0;

      for (int next = 1; next < _length; next++) {
        if ( _elems[cur] != _elems[next] ) {
          cur++;
        }
      }

      // reset length
      _length = cur + 1;
    } // Sort::removeDuplicates

  inline void sort ()
    { sort (0,_length-1); }

  // sort using a comparison function compare on C
  inline void sortF ()
    { sortF (0,_length-1); }

  // sort using a global comparison function compare on C
  // inline void sortGF ()
  //  { sortGF (0,_length-1); }

  // check membership in the sorted list
  inline bool member (const C c) const
    { return member (c,0,_length-1); }

 protected:  // structure

  C* _elems;
  int _size;
  int _length;

  // Quicksort, copied from Cormen et.al's "Introduction to Algorithms"
  void sort ( int p,int r ) 
    {
      ASS(r < _length);

      if (p < r) {
        int q = partition(p,r);
        sort(p,q);
        sort(q+1,r);    
      }
    } // Sort::sort

  // Quicksort, copied from Cormen et.al's "Introduction to Algorithms"
  void sortF ( int p,int r ) 
    {
      ASS(r < _length);

      if (p < r) {
        int q = partitionF(p,r);
        sortF(p,q);
        sortF(q+1,r);    
      }
    } // Sort::sort

  // Quicksort, copied from Cormen et.al's "Introduction to Algorithms"
  void sortGF ( int p,int r ) 
    {
      ASS(r < _length);

      if (p < r) {
        int q = partitionGF(p,r);
        sortGF(p,q);
        sortGF(q+1,r);    
      }
    } // Sort::sort

  int partition( int p,int r ) 
  {
    C x = _elems[p];
    int i = p-1;
    int j = r+1;

    for (;;) {
      do --j;
      while (x < _elems[j]);
      do ++i;
      while (_elems[i] < x);
      if (i < j) {
        // swap [i] and [j]
        C tmp = _elems[i];
        _elems[i] = _elems[j];
        _elems[j] = tmp;
      }
      else
        return j;
    }
  } // Sort::partitition

  int partitionF( int p,int r ) 
  {
    C x = _elems[p];
    int i = p-1;
    int j = r+1;

    for (;;) {
      do --j;
      while (x.isless(_elems[j]));

      do ++i;
      while (_elems[i].isless(x));

      if (i < j) {
        // swap [i] and [j]
        C tmp = _elems[i];
        _elems[i] = _elems[j];
        _elems[j] = tmp;
      }
      else
        return j;
    }
  } // Sort::partititionF

  int partitionGF( int p,int r ) 
  {
    C x = _elems[p];
    int i = p-1;
    int j = r+1;

    for (;;) {
      do --j;
      while ( isless(_elems[j],x) );

      do ++i;
      while ( isless(x,_elems[i]) );

      if (i < j) {
        // swap [i] and [j]
        C tmp = _elems[i];
        _elems[i] = _elems[j];
        _elems[j] = tmp;
      }
      else
        return j;
    }
  } // Sort::partititionGF

  bool member (const C c, int fst, int lst) const
    { 
      for (;;) {
        if (fst > lst) {
          return false;
        }

        int mid = (fst + lst) / 2;
      
        if (_elems[mid] == c) {
          return true;
        }

        if (_elems[mid] < c) {
          lst = mid-1;
        }
        else { // _elems[mid] > c
          fst = mid+1;
        }
      }
    } // Sort::member
};  // class Sort


#endif


