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
// File int.h
//
// Implements several functions on integers
//
//  990806 Uppsala
//  990928 Uppsala, min added
//  19/02/2000 Manchester, slightly reimplemented
//  09/06/2000 Manchester, toString added and int.cpp created
//


#ifndef __INT__
#define __INT__


#include <climits>
#include "List.hpp"


using namespace std;


enum Compare 
{
  LESS = -1,
  EQUAL = 0,
  GREATER = 1
};


#ifdef _MSC_VER // VC++
#  undef max
#  undef min
#endif


class Int 
{
 public:

  static int max ( int i1, int i2 );
  static int min ( int i1, int i2 );
  static Compare compare ( int i1, int i2 );
  static const int max ();
  static const int min ();
  static void toString ( int i, char* str );
  static char* toString (int i);

  typedef List<int,CID_INTLIST> List;
};


inline 
int Int::max ( int i1, int i2 ) 
{
  return  i1 > i2 ? i1 : i2 ;
}


inline 
int Int::min ( int i1, int i2 ) 
{
  return  i1 < i2 ? i1 : i2 ;
}


inline 
Compare Int::compare ( int i1, int i2 ) 
{
  if ( i1 > i2 )
    return GREATER;

  if ( i1 == i2 )
    return EQUAL;

  return LESS;
} // Int::compare


inline
const int Int::max ()
{
  return INT_MAX;
}


inline
const int Int::min ()
{
  return INT_MIN;
}


#endif  // __INT__

