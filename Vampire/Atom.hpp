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
//  file Atom.hpp
//  defines class Atom
//

#ifndef __Atom__
#define __Atom__

class AtomList;

#include "Term.hpp"


// ****************** class Atom, definition *************************


class Atom {
 private:
  class Data;

 public:
  // constructors/destructors
  Atom ();
  Atom (Signature::Pred* p, const TermList& args); // for atoms
  Atom (Signature::Pred* p);                       // for propositional atoms
  Atom (const Term& l, const Term& r);             // for equality 
  Atom (const Atom& a);
  ~Atom ();
  void operator = (const Atom& rhs);

  // structure
  Signature::Pred* functor () const;
  const TermList& args () const;
  bool operator == (const Atom& rhs) const;
  bool isEquality () const;

  // declared but not defined, to prevent on-heap allocation
  void* operator new (size_t);

  // various
  bool occurs ( const Signature::Pred* ) const;
  bool occurs ( Var v ) const;
  bool equal (Atom t) const;
  Compare compare (Atom l) const; // comparison, used for normalisation
  /*  bool defines (Atom lhs) const;  // auxiliary function for atoms */
  void apply (const Substitution& subst);
  void rectify (Substitution&, Var& last, VarList& freeVars);
  bool hasVarsNotIn (VarListList) const;
  bool isDefinition ( Term& lhs, Term& rhs ) const;
  void normalize ();
  bool isFlat () const; // atom has the form P (x1,...,xn) where xi are variables

  // functions for checking equality axioms
  bool isTautology () const;
  static bool transitivity (Atom a1, Atom a2, Atom a3);
  static bool functionMonotonicity (Atom a1, Atom a2);
  static bool predicateMonotonicity (Atom a1, Atom a2, Atom a3);
  bool swap (Atom a) const; // true if this is t1 = t2 and a is t2 = t1 */
  bool isRenamingOf (Atom a, Substitution& sbst) const;
  void occurring (bool* occurrences, Var max) const; // auxiliary for Formula::occurring

 private:
  // structure
  Data* _data;
}; // class Atom


class AtomList 
: public Lst<Atom>
{
 public:
  // constructors
  AtomList ();
  AtomList (const AtomList&);
  explicit AtomList (const Atom& t); // one-element list
  AtomList (const Atom& head, const AtomList& tail);
  explicit AtomList (LstData<Atom>*);

  // inherited functions
  const AtomList& tail () const
    { return static_cast<const AtomList&>(Lst<Atom>::tail()); }

  // miscellaneous
  /*
  bool occurs ( const Signature::Fun* ) const;
  bool occurs ( Var v ) const;
  bool equal (AtomList t) const;
  Compare compare (AtomList l) const; // comparison, used for normalisation
  bool varsOnly () const; // list consists of variables only
  int length () const;
  void apply (const Substitution& subst);
  void rectify (Substitution&, Var& last, VarList& freeVars);
  */
}; // class AtomList


class Atom::Data 
#   if DEBUG_PREPRO
//    : public Memory <CID_ATOM>
#   endif
{
 public:
  Data ();
  ~Data ();
  Data (Signature::Pred* f, const TermList& args);
  explicit Data (Signature::Pred* f);
  Data (const Term& l, const Term& r);             // for equality 
  void ref ();
  void deref ();

  Signature::Pred* functor () const;
  const TermList& args () const;

 protected:
  // structure
  int _counter;
  Signature::Pred* _functor;
  TermList _args;
}; // Atom::Data


// ******************* Atom definitions ************************

inline
Atom::Atom () 
  : 
  _data (0) 
{
} // Atom::Atom


inline
Atom::~Atom () 
{
  if (_data) {
    _data->deref ();
  }
} // Atom::~Atom


// 25/08/2002 Torrevieja
inline
Atom::Atom (const Atom& t)
  :
  _data (t._data)
{
  if (_data) {
    _data->ref ();
  }
} // Atom::Atom


// atom constructor
// 25/08/2002 Torrevieja
inline
Atom::Atom (Signature::Pred* p, const TermList& args)
  :
  _data (new Data (p,args))
{
} // Atom (Signature::Pred* f, TermList& args)


// equality atom constructor
// 25/08/2002 Torrevieja
inline
Atom::Atom (const Term& lhs, const Term& rhs)
  :
  _data (new Data (lhs,rhs))
{
} // Atom (const Term& lhs, const Term& rhs)


// propositional atom constructor
// 25/08/2002 Torrevieja
inline
Atom::Atom (Signature::Pred* p)
  :
  _data (new Data (p))
{
} // Atom (Signature::Pred* p)


inline
bool Atom::operator == (const Atom& rhs) const
{ 
  ASS (_data && rhs._data);

  return _data == rhs._data; 
} // Atom::operator ==


inline
Signature::Pred* Atom::functor () const
{
  return _data->functor ();
} // Atom::functor ()


inline
const TermList& Atom::args () const
{
  return _data->args ();
} // Atom::args ()


inline
bool Atom::isEquality () const
{ 
  return functor()->isEquality (); 
} // Atom::isEquality ()


// true if the atom has free variables not occurring in vs
// 06/05/2002 Manchester
inline
bool Atom::hasVarsNotIn (VarListList vs) const
{
  return args().hasVarsNotIn (vs);
} // Atom::hasVarsNotIn


// true if the atom is p(x1,...,xn) where all of xi's are variables
inline 
bool Atom::isFlat () const
{
  return args().varsOnly();
} // Atom::isFlat


// **************** Atom::Data definitions *********************

inline 
Atom::Data::~Data ()
{
  TRACER( "Atom::Data::~Data" );

  ASS (_counter == 0);
} // Atom::Data::~Data ()


inline
void Atom::Data::ref () 
{ 
  ASS (this);

  _counter++;
} // Atom::Data::ref ()


inline
void Atom::Data::deref () 
{ 
  ASS (this);
  ASS (_counter > 0);
  _counter--;

  if (_counter == 0) {
    delete this;
  }
} // Atom::Data::deref ()


inline
Atom::Data::Data (Signature::Pred* f, const TermList& args) 
  : 
  _counter (1),
  _functor (f), 
  _args (args) 
{
} // Atom::Data::Data


inline
Atom::Data::Data (Signature::Pred* f) 
  : 
  _counter (1),
  _functor (f) 
{
} // Atom::Data::Data


inline
Signature::Pred* Atom::Data::functor () const 
{ 
  ASS (this);

  return _functor; 
} // Atom::Data::functor ()


inline
const TermList& Atom::Data::args () const 
{ 
  ASS (this);

  return _args; 
} // Atom::Data::args ()


// ******************* AtomList definitions ************************

inline
AtomList::AtomList () 
  : 
  Lst<Atom> ()
{
} // AtomList::AtomList


// copy constructor
// 25/08/2002 Torrevieja
inline
AtomList::AtomList (const AtomList& ts)
  :
  Lst<Atom> (ts)
{
} // AtomList::AtomList


// almost a copy constructor
// 25/08/2002 Torrevieja
inline
AtomList::AtomList (LstData<Atom>* d)
  :
  Lst<Atom> (d)
{
} // AtomList::AtomList


// 'cons' list constructor
// 25/08/2002 Torrevieja
inline
AtomList::AtomList (const Atom &hd, const AtomList& tl)
  :
  Lst<Atom> (hd,tl)
{
} // AtomList::AtomList


// 'cons' list constructor
// 25/08/2002 Torrevieja
inline
AtomList::AtomList (const Atom &hd)
  :
  Lst<Atom> (hd)
{
} // AtomList::AtomList


ostream& operator << (ostream& str, Atom);


#endif // __atom__
