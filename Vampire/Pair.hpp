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
//  Class Pair<C,D> for implementing pairs
//
//  14/04/2000 Breckenridge
//  02/06/2000 Manchester, == added
//
// *************************************************************

#ifndef __pair__
#  define __pair__


#include "Memory.hpp"


// *******************Class Pair*******************************


template <class C, class D, ClassID cid>
class Pair
#   if DEBUG_PREPRO
    : public Memory <cid>
#   endif
{
 public:

  // constructors
  inline
  Pair ( C left, D right )
    :
    _left ( left ),
    _right ( right ) {}

  inline
  Pair () {}

  // structure quering
  inline C left () const { return _left; }
  inline D right () const { return _right; }

  // change left/right elements
  inline void left ( C lft ) { _left = lft; }
  inline void right ( D rht ) { _right = rht; }

  // equality
  inline 
  bool operator == ( Pair& rhs ) const 
    { return left() == rhs.left() &&
             right() == rhs.right(); }

 protected:  // structure
  C _left;
  D _right;
};  // class Pair


// ******************* Class Pair, definitions ****************************


#endif


