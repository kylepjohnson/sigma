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
//  file Term.hpp
//  defines classes Term and TermList
//
// 28/09/2002 Manchester, changed by adding NumericData
//

#ifndef __Term__
#define __Term__


class TermList;
class Substitution;


#include <string>


#include "Signature.hpp"
#include "Memory.hpp"
#include "Var.hpp"

#include "VampireKernel.hpp"


// ****************** class Term, definition *************************


class Term {
 private:
  class Data;
  class VarData;
  class CompoundData;
  class NumericData;

 public:
  enum Tag {
    VAR = 0,        // numbers required for normalization
    NUMERIC = 1,
    COMPOUND = 2 
  };

  // constructors/destructors
  Term ();
  Term (Signature::Fun* f, const TermList& args); // for compound terms
  Term (Signature::Fun* f);                       // for constants
  Term (const Term& t);
  ~Term ();
  explicit Term (Var v);
  explicit Term (double number);
  Term (const VampireKernel::Subterm* term, const VampireKernel& kernel);
  void operator= (const Term& rhs);

  // structure
  Tag tag () const;
  int var () const;
  double number () const;
  Signature::Fun* functor () const;
  const TermList& args () const;
  bool operator == (const Term& rhs) const;
  bool isvar () const;

  // declared but not defined, to prevent on-heap allocation
  void* operator new (size_t);

  // various
  bool occurs ( const Signature::Fun* ) const;
  bool occurs ( Var v ) const;
  bool equal (const Term& t) const;
  Compare compare (const Term& l) const; // comparison, used for normalisation
  bool equalUpTo (const Term& r, Var x, Var y) const; // this obtained from r by swapping x and y 
  bool defines (const Term& lhs) const;  // auxiliary function for atoms
  void apply (const Substitution& subst);
  void rectify (Substitution&, Var& last, VarList& freeVars);
  bool hasVarsNotIn (VarListList) const;
  bool isRenamingOf (Term t, Substitution& sbst) const;
  void occurring (bool* occurrences, Var max) const; // auxiliary for Formula::occurring

  // other
  static const char* Term::varPrefix; // for printing variables
 private:
  // structure
  Data* _data;
}; // class Term


class TermList 
  : public Lst<Term>
{
 public:
  // constructors
  TermList ();
  TermList (const TermList&);
  explicit TermList (const Term& t); // one-element list
  TermList (const Term& head, const TermList& tail);
  explicit TermList (LstData<Term>*);
  explicit TermList (const VampireKernel::Subterm* term);
  TermList (const VampireKernel::Subterm* term, const VampireKernel& kernel);

  // inherited functions
  const TermList& tail () const
    { return static_cast<const TermList&>(Lst<Term>::tail()); }

  // various
  bool occurs ( const Signature::Fun* ) const;
  bool occurs ( Var v ) const;
  bool equal (TermList t) const;
  Compare compare (TermList l) const; // comparison, used for normalisation
  bool equalUpTo (TermList rs, Var x, Var y) const; // this obtained from r by swapping x and y 
  bool hasVarsNotIn (VarListList) const;
  bool varsOnly () const; // list consists of variables only
  void apply (const Substitution& subst);
  void rectify (Substitution&, Var& last, VarList& freeVars);
  void buildFrom (VarList vs);
  bool isRenamingOf (TermList t, Substitution& sbst) const;
  void occurring (bool* occurrences, Var max) const; // auxiliary for Formula::occurring
}; // class TermList


class Term::Data 
{
 public:
  Data ();
  explicit Data (Tag tag);
  ~Data ();

  Term::Tag tag () const;

  void ref ();
  void deref ();
  void destroy ();

 protected:
  // structure
  int _counter;
  Term::Tag _tag;
}; // classTerm::Data


class Term::VarData 
: public Term::Data
#   if DEBUG_PREPRO
    , public Memory <CID_VTERM>
#   endif
{
 public:
  explicit VarData (Var v);
  Var var () const;

 private:
  // structure
  Var _var;
}; // Term::VarData


class Term::CompoundData 
: public Term::Data
#   if DEBUG_PREPRO
    , public Memory <CID_CTERM>
#   endif
{
 public:
  CompoundData (Signature::Fun* f, const TermList& args);
  explicit CompoundData (Signature::Fun* f);
  Signature::Fun* functor () const;
  const TermList& args () const;

 private:
  // structure
  Signature::Fun* _functor;
  TermList _args;
}; // Term::CompoundData


class Term::NumericData 
: public Term::Data
#   if DEBUG_PREPRO
    , public Memory <CID_NTERM>
#   endif
{
 public:
  explicit NumericData (double f);
  double number () const;

 private:
  // structure
  double _number;
}; // Term::NumericData


ostream& operator << ( ostream&, const Term );

// ******************* Term definitions ************************

inline
Term::Term () 
  : 
  _data (0) 
{
} // Term::Term


inline
Term::~Term () 
{
  if (_data) {
    _data->deref ();
  }
} // Term::~Term


// 25/08/2002 Torrevieja
inline
Term::Term (const Term& t)
  :
  _data (t._data)
{
  if (_data) {
    _data->ref ();
  }
} // Term::Term


// compound term constructor
// 25/08/2002 Torrevieja
inline
Term::Term (Signature::Fun* f, const TermList& args)
  :
  _data (new CompoundData (f,args))
{
} // Term (Signature::Fun* f, TermList args)


// compound term constructor
// 25/08/2002 Torrevieja
inline
Term::Term (Signature::Fun* f)
  :
  _data (new CompoundData (f))
{
} // Term (Signature::Fun* f, TermList args)


// variable term constructor
// 25/08/2002 Torrevieja
inline
Term::Term (Var v)
  :
  _data (new VarData (v))
{
} // Term (Var v)


// numeric term constructor
// 28/09/2002 Manchester
inline
Term::Term (double d)
  :
  _data (new NumericData (d))
{
} // Term (Var v)


inline
Term::Tag Term::tag () const 
{ 
  return _data->tag(); 
} // Term::tag ()


inline
bool Term::isvar () const 
{ 
  return _data->tag() == VAR; 
} // Term::isvar ()


inline
bool Term::operator == (const Term& rhs) const
{ 
  ASS (_data && rhs._data);

  return _data == rhs._data; 
} // Term::operator ==


// Var Term::var () - return the variable number of a variable term
// 25/08/2002 Torrevieja
inline
Var Term::var () const
{ 
  ASS (this && tag() == VAR);

  return (static_cast<Term::VarData*>(_data))->var();
} // Term::var


// return the functor of a compound term
// 25/08/2002 Torrevieja
inline
Signature::Fun* Term::functor () const
{ 
  ASS (this && tag() == COMPOUND);

  return (static_cast<Term::CompoundData*>(_data))->functor();
} // Term::functor


// return the arguments of a compound term
// 25/08/2002 Torrevieja
inline
const TermList& Term::args () const
{ 
  ASS (this && tag() == COMPOUND);

  return (static_cast<Term::CompoundData*>(_data))->args();
} // Term::args


// return the value of a numeric term
// 25/08/2002 Torrevieja
inline
double Term::number () const
{ 
  ASS (this && tag() == NUMERIC);

  return (static_cast<Term::NumericData*>(_data))->number();
} // Term::number


// **************** Term::Data definitions *********************

inline
Term::Data::Data (Term::Tag tag)
  : 
  _counter (1),
  _tag (tag)
{
} // Term::Data::Data (Tag tag)


inline 
Term::Data::~Data ()
{
  TRACER( "Term::Data::~Data" );

  ASS (_counter == 0);
} // Term::Data::~Data ()


inline
void Term::Data::ref () 
{ 
  ASS (this);

  _counter++;
} // Term::Data::ref ()


inline
Term::Tag Term::Data::tag () const 
{ 
  ASS (this);

  return _tag; 
} // Term::Data::tag ()


inline
void Term::Data::deref () 
{ 
  ASS (this);
  ASS (_counter > 0);
  _counter--;

  if (_counter == 0) {
    destroy ();
  }
} // Term::Data::deref ()


// **************** Term::VarData definitions *********************

inline
Term::VarData::VarData (Var v)
  : 
  Data (VAR), 
  _var (v) 
{
} // Term::VarData::VarData


inline
Var Term::VarData::var () const 
{ 
  ASS(_tag == VAR);

  return _var; 
} // Term::VarData::var ()


// **************** Term::NumericData definitions *********************

inline
Term::NumericData::NumericData (double value)
  : 
  Data (NUMERIC), 
  _number (value) 
{
} // Term::NumericData::NumericData


inline
double Term::NumericData::number () const 
{ 
  ASS(_tag == NUMERIC);

  return _number; 
} // Term::NumericData::number ()


// *************** Term::CompoundData definitions *****************

inline
Term::CompoundData::CompoundData (Signature::Fun* f, const TermList& args) 
  : 
  Data (COMPOUND), 
  _functor (f), 
  _args (args) 
{
} // Term::CompoundData::CompoundData


inline
Term::CompoundData::CompoundData (Signature::Fun* f) 
  : 
  Data (COMPOUND), 
  _functor (f) 
{
} // Term::CompoundData::CompoundData


inline
Signature::Fun* Term::CompoundData::functor () const 
{ 
  ASS (this && _tag == COMPOUND);

  return _functor; 
} // Term::CompoundData::functor ()


inline
const TermList& Term::CompoundData::args () const 
{ 
  ASS (this && _tag == COMPOUND);

  return _args; 
} // Term::CompoundData::args ()


// ******************* TermList definitions ************************

inline
TermList::TermList () 
  : 
  Lst<Term> ()
{
} // TermList::TermList


// copy constructor
// 25/08/2002 Torrevieja
// 07/09/2002 Manchester, changed
inline
TermList::TermList (const TermList& ts)
  :
  Lst<Term> (ts)
{
} // TermList::TermList


// almost a copy constructor
// 25/08/2002 Torrevieja
inline
TermList::TermList (LstData<Term>* d)
  :
  Lst<Term> (d)
{
} // TermList::TermList


// 'cons' list constructor
// 25/08/2002 Torrevieja
inline
TermList::TermList (const Term &hd, const TermList& tl)
  :
  Lst<Term> (hd,tl)
{
} // TermList::TermList


// 'cons' list constructor
// 25/08/2002 Torrevieja
inline
TermList::TermList (const Term &hd)
  :
  Lst<Term> (hd)
{
} // TermList::TermList


#endif // __Term__

