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
//  file Position.hpp
//  defines class Position of poositions in formulas
//  started 21/04/2003, flight Manchester-Frankfurt
//

#ifndef __Position__
#define __Position__


#include "Lists.hpp"


class Position
  : public IntList
{
 public:
  // constructors
  Position ();
  Position (const Position&);
  explicit Position (int p); // one-element list
  Position (int head, const Position& tail);

  // inherited functions
  const Position& tail () const
    { return static_cast<const Position&>(IntList::tail()); }
}; // class Position


// ******************* Position definitions ************************

inline
Position::Position () 
  : 
  IntList ()
{
} // Position::Position


inline
Position::Position (const Position& ts)
  :
  IntList (ts)
{
} // Position::Position


inline
Position::Position (int hd, const Position& tl)
  :
  IntList (hd,tl)
{
} // Position::Position


inline
Position::Position (int hd)
  :
  IntList (hd)
{
} // Position::Position


#endif // __Position__
