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

/*
//
//  file Renaming.cpp
//  implements Renaming
//  currently not a very fast implementation
//


#include "Renaming.hpp"


// 27/06/2002 Manchester
Renaming::Renaming ()
  : _bindings (List::empty())
{
} // Renaming::Renaming


// 27/06/2002 Manchester
Renaming::~Renaming ()
{
  List::Iterator bs (_bindings);
  while (bs.more()) {
    delete bs.next ();
  }
  _bindings->destroy ();
} // Renaming::~Renaming


// 27/06/2002 Manchester
void Renaming::push ( Var v, Var t)
{
  List::push ( new Binding(v,t), _bindings );
} // Renaming::push


// 27/06/2002 Manchester
Var Renaming::get (Var v) const
{
  List::Iterator bs (_bindings);
  while (bs.more()) {
    Binding* b = bs.next ();
    if (b->left() == v) {
      return b->right();
    }
  }
  // not found
  return -1;
} // Renaming::get


// 27/06/2002 Manchester
void Renaming::pop ()
{
  ASS(! _bindings->isEmpty());

  delete List::pop (_bindings);
} // Renaming::pop


*/
