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
//  file List.hpp
//  defines several instances of Lst classes 
//
// 29/04/2003 Manchester
//

#ifndef __Lists__
#define __Lists__


#include "Lst.hpp"


class IntList 
  : public Lst<int>
{
 public:
  // constructors
  IntList ();
  IntList (const IntList&);
  explicit IntList (int); // one-element list
  IntList (int head, const IntList& tail);

  // inherited functions
  const IntList& tail () const
    { return static_cast<const IntList&>(Lst<int>::tail()); }
}; // class IntList


// ******************* IntList definitions ************************

inline
IntList::IntList () 
  : 
  Lst<int> ()
{
} // IntList::IntList


// copy constructor
inline
IntList::IntList (const IntList& ts)
  :
  Lst<int> (ts)
{
} // IntList::IntList


// 'cons' list constructor
inline
IntList::IntList (int hd, const IntList& tl)
  :
  Lst<int> (hd,tl)
{
} // IntList::IntList


// one-element list constructor
inline
IntList::IntList (int hd)
  :
  Lst<int> (hd)
{
} // IntList::IntList


#endif // __Lists__

