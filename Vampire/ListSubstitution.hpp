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
//  file ListSubstitution.hpp
//  defines class ListSubstitution of substitutions which can be popped
//  and in which the order in which the bindings are inserted matters
//  not a very fast implementation
//

#ifndef __ListSubstitution__
#define __ListSubstitution__


#include "Term.hpp"


class ListSubstitution
{
 public:
  ListSubstitution ();
  ~ListSubstitution ();
  void push (Var v, Term t);
  bool get (Var v, Term& t) const;
  void pop (Var v);
  //void originalVars (VarList& vs) const 
  //  { return originalVars (vs, _bindings); }
  void terms (TermList& ts) const { terms (_bindings, ts); }

 private:
  class Binding;
  typedef List<Binding*, CID_BINDING_LIST> BindingList;

  // structure
  BindingList* _bindings;

  // auxiliary
  static void originalVars (VarList& vs, const BindingList*);
  static void terms (const BindingList*, TermList& ts);

 friend ostream& operator << (ostream& str, ListSubstitution&);
}; // class ListSubstitution


class ListSubstitution::Binding {
 public:
  Binding (Var v, Term t) 
    :
    _var (v),
    _term (t)
    {}
  Var var () const { return _var; }
  const Term& term () const { return _term; }
 private:
  // structure
  Var _var;
  const Term _term;
}; // class ListSubstitution::Binding


ostream& operator << (ostream& str, ListSubstitution&);


#endif // __ListSubstitution__

