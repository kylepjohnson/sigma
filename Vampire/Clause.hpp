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
//  file Clause.hpp 
//  defines class Clause
//


#ifndef __Clause__
#define __Clause__


#include "Literal.hpp"


class Clause 
{
 private:
  class Data;

 public:
  // constructor
  Clause ();
  explicit Clause (const LiteralList& lits);
  Clause (const Clause& t);
  Clause (const VampireKernel::Clause*, const VampireKernel& kernel);
  ~Clause ();
  void operator= (const Clause& rhs);

  // structure access
  const LiteralList& literals () const;

  //miscellaneous
  bool isless (Clause l) const;       // comparison, used for normalisation
  Compare compare (Clause l) const; // comparison, used for normalisation
  void normalize ();
  // properties
  bool isEqualityAxiom () const;  // is one of the equality axioms
  bool isEmpty () const;
  bool isFunctionDefinition (Term& lhs, Term& rhs) const; 
  bool isRenamingOf (Clause c) const;
 private:
  // structure
  Data* _data;

  // equality axiom check
  bool isReflexivityAxiom () const;
  bool isFunctionReflexivityAxiom () const;
  bool isPredicateReflexivityAxiom () const;
  bool isSymmetryAxiom () const;
  bool isTransitivityAxiom () const;
}; // class Clause


class Clause::Data
#   if DEBUG_PREPRO
    : public Memory <CID_CLAUSE>
#   endif
{
 public:
  Data ();
  explicit Data (const LiteralList& literals);

  // structure access
  const LiteralList& literals () const;

  void ref ();
  void deref ();

 private:
  // structure
  int _counter;
  LiteralList _literals;
}; // class Clause::Data


class ClauseList 
: public Lst<Clause>
{
 public:
  // constructors
  ClauseList ();
  ClauseList (const ClauseList&);
  explicit ClauseList (const Clause& t); // one-element list
  ClauseList (const Clause& head, const ClauseList& tail);
  explicit ClauseList (LstData<Clause>*);

  // structure access
  const LiteralList& literals ();

  // inherited functions
  const ClauseList& tail () const
    { return static_cast<const ClauseList&>(Lst<Clause>::tail()); }
}; // class ClauseList


ostream& operator << (ostream&, Clause);


// ******************* class Clause, implementation *********************


inline
Clause::Clause () 
  : 
  _data (0) 
{
} // Clause::Clause


inline
Clause::~Clause () 
{
  if (_data) {
    _data->deref ();
  }
} // Clause::~Clause


inline
Clause::Clause (const Clause& t)
  :
  _data (t._data)
{
  if (_data) {
    _data->ref ();
  }
} // Clause::Clause


inline
Clause::Clause (const LiteralList& literals)
  :
  _data (new Data(literals))
{
} // Clause::Clause (const LiteralList&)


inline
const LiteralList& Clause::literals () const
{
  return _data->literals();
} // Clause::literals ()


inline
bool Clause::isEmpty () const
{ 
  return literals().isEmpty(); 
} // Clause::isEmpty ()


// true is this clause is a renaming of c
// 03/10/2002 Manchester
inline
bool Clause::isRenamingOf (Clause c) const
{
  return literals().isRenamingOf (c.literals());
} // Clause::isRenamingOf


// **************** class Clause::Data implementation ******************


inline
Clause::Data::Data (const LiteralList& literals) 
  : 
  _counter (1),
  _literals (literals)
{
} // Clause::Data::Data


inline
const LiteralList& Clause::Data::literals () const
{
  return _literals;
} // Clause::Data::literals ()


inline
void Clause::Data::ref () 
{ 
  ASS (this);

  _counter++;
} // Clause::Data::ref ()


inline
void Clause::Data::deref () 
{ 
  ASS (this);
  ASS (_counter > 0);
  _counter--;

  if (_counter == 0) {
    delete this;
  }
} // Clause::Data::deref ()


// ******************* ClauseList definitions ************************

inline
ClauseList::ClauseList () 
  : 
  Lst<Clause> ()
{
} // ClauseList::ClauseList


inline
ClauseList::ClauseList (const ClauseList& ts)
  :
  Lst<Clause> (ts)
{
} // ClauseList::ClauseList


inline
ClauseList::ClauseList (LstData<Clause>* d)
  :
  Lst<Clause> (d)
{
} // ClauseList::ClauseList


inline
ClauseList::ClauseList (const Clause &hd, const ClauseList& tl)
  :
  Lst<Clause> (hd,tl)
{
} // ClauseList::ClauseList


inline
ClauseList::ClauseList (const Clause &hd)
  :
  Lst<Clause> (hd)
{
} // ClauseList::ClauseList



#endif // __Clause__
