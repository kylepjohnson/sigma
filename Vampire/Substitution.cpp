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
//  file Substitution.cpp
//  implements classes Binding and Substitution
//  currently not a very fast implementation
//


#include "Substitution.hpp"


// 03/10/2002 Manchester
bool Substitution::bound (Var v, Term& t) const
{
  TRACER( "Substitution::bound" );

  Iterator<Binding> bs (*this);
  while (bs.more()) {
    Binding b (bs.next ());
    if (b.var() == v) {
      t = b.term();
      return true;
    }
  }

  // not found
  return false;
} // Substitution::get


// collect into VarList lhs of all bindings
// 03/08/2002 Torrevieja
// 05/09/2002 Trento, changed
// 03/10/2002 Manchester, changed to new data structures
void Substitution::domain (VarList& vs, Lst<Binding> bs) const
{
  if (bs.isEmpty()) {
    return;
  }

  VarList tl;
  domain (tl, bs.tail());

  vs = VarList (bs.head().var(), tl);
} // Substitution::domain


// collect into VarList rhs of all bindings
// 03/10/2002 Manchester, copied from domain ()
void Substitution::range (TermList& ts, Lst<Binding> bs) const
{
  if (bs.isEmpty()) {
    return;
  }

  TermList tl;
  range (tl, bs.tail());

  ts = TermList (bs.head().term(), tl);
} // Substitution::range


// undo the last binding for v
// 03/10/2002 Manchester
void Substitution::undoBinding (Var v)
{
  *this = undoBinding (v, *this);
} // Substitution::undoBinding


// undo the last binding for v
// 03/10/2002 Manchester
Lst<Binding> Substitution::undoBinding (Var v, Lst<Binding> bs)
{
  ASS ( bs.isNonEmpty() );

  if (bs.head().var() == v) {
    return bs.tail();
  }

  return Lst<Binding> (bs.head(), undoBinding(v,bs.tail()));
} // Substitution::undoBinding


// t is in the range of the substitution
// WARNING: may not work correctly if the substitution contains
// multiple bindings of the same variable
bool Substitution::inRange (Term t) const
{
  Iterator<Binding> bs (*this);
  while ( bs.more() ) {
    if ( t.equal(bs.next().term()) ) {
      return true;
    }
  }

  return false;
} // Substitution::inRange
