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
//  file Substitution.hpp
//  defines classes Binding and Substitution
//  not a very fast implementation
//

#ifndef __Substitution__
#define __Substitution__


#include "Term.hpp"
#include "Lst.hpp"


class Binding {
 public:
  Binding (Var v, const Term& t);
  Binding (const Binding&);
  ~Binding ();
  void operator= (const Binding& rhs);

  Var var () const;
  const Term& term () const;
 private:
  class Data;

  // structure
  Data* _data;
}; // class Binding


class Substitution
: public Lst<Binding>
{
 public:
  Substitution ();
  Substitution (const Substitution& sbst);
  void operator= (const Lst<Binding>& rhs);

  void bind (Var v, Term t);
  bool bound (Var v, Term& t) const;
  void undoBinding (Var v);
  void domain (VarList& vs) const;
  void range (TermList& ts) const;
  bool inRange (Term t) const;

 private:
  void domain (VarList& vs, Lst<Binding>) const;
  void range (TermList& ts, Lst<Binding>) const;
  Lst<Binding> undoBinding (Var v, Lst<Binding>);
}; // class Substitution


class Binding::Data {
 public:
  Data (Var v, const Term& t);

  Var var () const;
  const Term& term () const;

  void ref ();
  void deref ();

 private:
  int _counter;
  Var _var;
  Term _term;
}; // class Binding::Data


// ****************** class Binding, implementation ***********************


inline
Binding::Binding (Var v, const Term& t) 
  : _data (new Data (v,t)) 
{
} // Binding::Binding


inline
Binding::~Binding () 
{
  if (_data) {
    _data->deref ();
  }
} // Binding::~Binding


inline
Binding::Binding (const Binding& t)
  :
  _data (t._data)
{
  if (_data) {
    _data->ref ();
  }
} // Binding::Binding


inline
Var Binding::var () const
{
  return _data->var();
} // Binding::var


inline
const Term& Binding::term () const
{
  return _data->term();
} // Binding::term


// ****************** class Binding::Data implementation ******************


inline
Binding::Data::Data (Var v, const Term& term) 
  : 
  _counter (1),
  _var (v), 
  _term (term)
{
} // Binding::Data::Data


inline
Var Binding::Data::var () const
{
  return _var;
} // Binding::Data::var


inline
const Term& Binding::Data::term () const
{
  return _term;
} // Binding::Data::term


inline
void Binding::Data::ref () 
{ 
  ASS (this);

  _counter++;
} // Binding::Data::ref ()


inline
void Binding::Data::deref () 
{ 
  ASS (this);
  ASS (_counter > 0);
  _counter--;

  if (_counter == 0) {
    delete this;
  }
} // Binding::Data::deref ()


// ****************** class Substitution implementation ******************


inline
Substitution::Substitution ()
{
} // Substitution::Substitution ()


inline
Substitution::Substitution (const Substitution& sbst)
  : Lst<Binding> (sbst)
{
} // Substitution::Substitution ()


// assignment operator
// 03/10/2002 Manchester, copied from Literal::operator=
inline
void Substitution::operator = (const Lst<Binding>& b)
{
  Lst<Binding>::operator= (b);
} // Substitution::operator =


inline
void Substitution::bind (Var v, Term t)
{
  push (Binding(v,t));
} // Binding::bind 


inline
void Substitution::domain (VarList& vs) const
{
  domain (vs, *this);
} // Substitution::domain


inline
void Substitution::range (TermList& ts) const
{
  range (ts, *this);
} // Substitution::range


#endif // __Substitution__

