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
//  file Renaming.hpp
//  defines class Renaming
//  completely changed using maps 02/10/2002, Manchester
//

#ifndef __renaming__
#define __renaming__


#include "Var.hpp"
#include "Map.hpp"


class Renaming
  : public Map<Var,Var>
{
 public:
  void push (Var v1, Var v2);
  Var get (Var v1); // -1 if not found
}; // class Renaming


// ************** class Renaming, implementation ********************

inline
void Renaming::push (Var v1, Var v2)
{
  insert (v1, v2);
} // Renaming::push 


inline
Var Renaming::get (Var v)
{
  Var result;
  if (find (v, result)) {
    return result;
  }

  return -1;
} // Renaming::get


#endif // __renaming__
