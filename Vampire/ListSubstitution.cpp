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
//  file ListSubstitution.cpp
//  implements ListSubstitution
//  currently not a very fast implementation
//


#include "ListSubstitution.hpp"


// 27/06/2002 Manchester
ListSubstitution::ListSubstitution ()
  : _bindings (BindingList::empty())
{
  TRACER ("ListSubstitution::ListSubstitution");
} // ListSubstitution::ListSubstitution


// 27/06/2002 Manchester
ListSubstitution::~ListSubstitution ()
{
  TRACER ("ListSubstitution::~ListSubstitution");

  BindingList::Iterator bs (_bindings);
  while (bs.more()) {
    Binding* b = bs.next ();
    delete b;
  }
  _bindings->destroy ();
} // ListSubstitution::~ListSubstitution


// 27/06/2002 Manchester
void ListSubstitution::push (Var v, Term t)
{
  TRACER( "ListSubstitution::push" );

  List::push ( new Binding(v,t), _bindings );
} // ListSubstitution::push


// 27/06/2002 Manchester
bool ListSubstitution::get (Var v, Term& t) const
{
  TRACER( "ListSubstitution::get" );

  List::Iterator bs (_bindings);
  while (bs.more()) {
    Binding* b = bs.next ();
    if (b->var() == v) {
      t = b->term();
      return true;
    }
  }
  // not found
  return false;
} // ListSubstitution::get


// 27/06/2002 Manchester
// 16/09/2002 Manchester, changed (argument added)
void ListSubstitution::pop (Var v)
{
  TRACER( "ListSubstitution::pop" );

  List::DelIterator bs (_bindings);
  while (bs.more()) {
    Binding* b = bs.next();
    if (b->var() == v) { // found
      bs.del ();
      delete b;
      return;
    }
  }

  ASS(false);
} // ListSubstitution::pop


ostream& operator << (ostream& str, ListSubstitution& sbst)
{
  str << '[';

  ListSubstitution::List::Iterator bindings (sbst._bindings);
  while (bindings.more()) {
    ListSubstitution::Binding* bind = bindings.next();
    str << 'X' << bind->var() << '=' 
	<< bind->term();
    if (bindings.more()) {
      str << ',';
    }
  }

  str << ']';
  return str;
} // operator << (ostream& str, ListSubstitution& sbst)


// collect into VarList lhs of all bindings
// 03/08/2002 Torrevieja
// 05/09/2002 Trento, changed
void ListSubstitution::originalVars (VarList& lhs, const List* bindings)
{
  if (bindings->isEmpty()) {
    return;
  }

  VarList tl;
  originalVars (tl, bindings->tail());
  Var v = bindings->head()->var();

  lhs = VarList (v,tl);
} // ListSubstitution::originalVars


// collect into ts rhs of all bindings
// must be called with empty ts
// 25/08/2002 Torrevieja
void ListSubstitution::terms (const List* bindings, TermList& ts)
{
  if (bindings->isEmpty()) {
    return;
  }

  TermList rest;
  terms (bindings->tail(), rest);
  TermList result (bindings->head()->term(), rest);
  ts = result;
} // ListSubstitution::terms
