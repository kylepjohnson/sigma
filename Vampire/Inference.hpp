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
//  file Inference.hpp
//  defines class Inference
//  started 17/04/2003, Barcelona Airport
//  
//  A detailed documentation of all inference rules is given in 
//  the file Inference.cpp
//

#ifndef __Inference__
#define __Inference__


#include "Lst.hpp"


class Term;
class Formula;
class Position;
class VarList;
class IntList;


class Inference
{
public:
  enum Rule {
    FORALL_AND_MINISCOPE,
    FORALL_OR_MINISCOPE,
    DUMMY_QUANTIFIER_REMOVAL,
    FLATTEN,
    SWAP
  };

  // constructor, destructor
  Inference (const Inference& inf);
  void operator= (const Term& rhs);
  ~Inference ();

  // various particular kinds of inferences
  Inference (Rule r,                            // dummy quantifier removal
	     const Formula& premise, const Position& p,
             const VarList& removedVars);
  Inference (Rule r,                            // forall_and_miniscope
	     const Formula& premise, 
	     const Position& p);
  Inference (Rule r,                            // forall_or_miniscope
	     const Formula& premise,
	     const Position& p,
	     const IntList& toppledVarPositions,
	     const IntList& toppledSubformulaPositions,
	     int toppledSubformulaIndex);
  Inference (Rule r,                            // flattening
	     const Formula& premise,
	     const Position& p,
	     int subformulaIndex);

  // structure
  Rule rule () const;

private:
  // data for various kinds of inferences
  class Data;
  class DFormula;    // conclusion is Formula
  class DFormula1;   // conclusion is Formula, one premise
  class DClause;     // conclusion is Clause
  class Type1;       // forall-and-miniscope, swap
  class ForallOrMiniscope;
  class DummyQuantifierRemoval;
  class Flatten;

  // declared but not defined, to prevent on-heap allocation
  //  void* operator new (size_t);

 private:
  // structure
  Data* _data;
}; // class Inference


class InferenceList
  : public Lst<Inference>
{
 public:
  // constructors
  InferenceList ();
  InferenceList (const InferenceList&);
  explicit InferenceList (const Inference& inf); // one-element list
  InferenceList (const Inference& inf, const InferenceList& tail);
  explicit InferenceList (LstData<Inference>*);

  // inherited functions
  const InferenceList& tail () const
    { return static_cast<const InferenceList&>(Lst<Inference>::tail()); }
}; // class InferenceList



class Inference::Data 
{
 public:
  Data ();
  explicit Data (Rule rule);
  ~Data ();
  Rule rule () const;

  void ref ();
  void deref ();
  void destroy ();

 protected:
  // structure
  int _counter;
  Rule _rule;
}; // class Inference::Data


// ******************* Inference definitions ************************

inline
Inference::~Inference () 
{
  if (_data) {
    _data->deref ();
  }
} // Inference::~Inference


inline
Inference::Inference (const Inference& t)
  :
  _data (t._data)
{
  if (_data) {
    _data->ref ();
  }
} // Inference::Inference


inline
Inference::Rule Inference::rule () const 
{ 
  return _data->rule(); 
} // Inference::rule ()


// **************** Inference::Data definitions *********************

inline
Inference::Data::Data (Inference::Rule rule)
  : 
  _counter (1),
  _rule (rule)
{
} // Inference::Data::Data (Rule rule)


inline 
Inference::Data::~Data ()
{
  TRACER( "Inference::Data::~Data" );

  ASS (_counter == 0);
} // Inference::Data::~Data ()


inline
void Inference::Data::ref () 
{ 
  ASS (this);

  _counter++;
} // Inference::Data::ref ()


inline
Inference::Rule Inference::Data::rule () const 
{ 
  ASS (this);

  return _rule; 
} // Inference::Data::rule ()


inline
void Inference::Data::deref () 
{ 
  ASS (this);
  ASS (_counter > 0);
  _counter--;

  if (_counter == 0) {
    destroy ();
  }
} // Inference::Data::deref ()


// ******************* InferenceList definitions ************************

inline
InferenceList::InferenceList () 
  : 
  Lst<Inference> ()
{
} // InferenceList::InferenceList


inline
InferenceList::InferenceList (const InferenceList& ts)
  :
  Lst<Inference> (ts)
{
} // InferenceList::InferenceList


inline
InferenceList::InferenceList (LstData<Inference>* d)
  :
  Lst<Inference> (d)
{
} // InferenceList::InferenceList


inline
InferenceList::InferenceList (const Inference& hd, const InferenceList& tl)
  :
  Lst<Inference> (hd,tl)
{
} // InferenceList::InferenceList


inline
InferenceList::InferenceList (const Inference& hd)
  :
  Lst<Inference> (hd)
{
} // InferenceList::InferenceList



#endif // __Inference__
