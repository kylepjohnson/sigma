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
//  file Unit.hpp
//


#ifndef __unit__
#define __unit__


#include "Chain.hpp"
#include "Clause.hpp"
#include "Formula.hpp"


class Clause;
class Problem;
class UnitList;


enum InputType {
  AXIOM,
  CONJECTURE,
  HYPOTHESIS
};


enum UnitType {
  CLAUSE,
  FORMULA
};


enum DefType {
  DT_NONE,
  DT_PRED,
  DT_FUN
};


enum InferenceRule {
  IR_INPUT,         // input formula or clause
  IR_PROP,          // obtained by propositional manipulations, e.g., a -> b replaced by ~a \/ b
  IR_PERMUT,        // obtained by permutations, e.g. f <=> g replaced by g <=> f
  IR_FLATTEN,       // obtained by some form of flattening (quantifiers, conjunctions etc.)
  IR_REORDER_EQ,    // obtained by reordering equality
  IR_HALF_EQUIV,    // obtained by rewriting a positive equivalence into an implication
  IR_ENNF,          // clear
  IR_RM_EQUIV,      // remove equivalences
  IR_MINISCOPE,
  IR_SKOLEMIZE,
  IR_RECTIFY,
  IR_SWAP,          // obtained by using commutativity of connectives, eg &
  IR_CLAUSIFY,      // obtain clause from a formula
  IR_UNKNOWN        // temporary fix
}; 


// ****************** class Unit, definition *******************


class Unit {
 private:
  class Data;

 public:
  // constructors/destructors
  Unit ();
  Unit (const Unit& a);
  Unit (char* name, InputType untype, const Formula&);
  Unit (char* name, InputType untype, const Clause&);
  Unit (InferenceRule, const Formula&, const Unit& parent);
  Unit (InferenceRule, const Clause&, const Unit& parent);
  Unit (const Clause&, const UnitList& parents);
  Unit (const Formula&, const UnitList& parents);
  explicit Unit (void*); // kludge, to interface with the kernel
  ~Unit ();
  void operator = (const Unit& rhs);

  // structure access
  bool operator == (const Unit& rhs) const;
  UnitType unitType () const;
  InputType inputType () const;
  DefType defType () const;
  void setDefType (DefType);
  const Formula& formula () const;
  const Clause& clause () const;
  void setFormula (Formula f);
  char* name () const;
  InferenceRule rule () const;
  long number () const;
  const UnitList& parents () const;
  void* giveAway (); // to interface with the parser

  // declared but not defined, to prevent on-heap allocation
  void* operator new (size_t);

  // miscellaneous
  bool isEqualityAxiom () const;      // is one of the equality axioms
  bool formulaWithFreeVars () const;  // is a formula with free variables
  bool makeDefinition (Unit& def); 
 private:
  // structure
  Data* _data;
}; // class Unit


class UnitList 
: public Lst<Unit>
{
 public:
  // constructors
  UnitList ();
  UnitList (const UnitList&);
  explicit UnitList (const Unit& t); // one-element list
  UnitList (const Unit& head, const UnitList& tail);
  explicit UnitList (LstData<Unit>*);
  UnitList (InputType tp, const FormulaList& formulas);

  // inherited functions
  const UnitList& tail () const
    { return static_cast<const UnitList&>(Lst<Unit>::tail()); }
}; // class UnitList


class Unit::Data
#   if DEBUG_PREPRO
    : public Memory <CID_UNIT>
#   endif
{
 public:
  Data (char* name, InputType untype, const Formula&);
  Data (char* name, InputType untype, const Clause&);
  Data (InferenceRule, const Formula&, const Unit& parent);
  Data (InferenceRule, const Clause&, const Unit& parent);
  Data (const Clause&, const UnitList& parents);
  Data (const Formula&, const UnitList& parents);
  ~Data ();

  void ref ();
  void deref ();

  // structure access
  UnitType unitType () const;
  InputType inputType () const;
  DefType defType () const;
  void setDefType (DefType);
  const Formula& formula () const;
  const Clause& clause () const;
  void setFormula (Formula f);
  char* name () const;
  InferenceRule rule () const;
  long number () const;
  const UnitList& parents () const;
  InputType intype () const;

 private:
  // structure
  int _counter;
  UnitType _untype;
  InputType _intype;
  InferenceRule _rule;  
  long _number;
  UnitList _parents;
  char* _name;
  Clause _clause;
  Formula _formula;
  DefType _defType;

  static long _lastNumber;
}; // class Unit::Data


typedef Chain<Unit,CID_UNIT_LINK> UnitChain;


ostream& operator << (ostream&, InputType);
ostream& operator << (ostream&, UnitType);
ostream& operator << (ostream&, UnitChain&);
ostream& operator << (ostream&, Unit);


// ******************* Unit implementation ************************

inline
Unit::Unit () 
  : 
  _data (0) 
{
} // Unit::Unit


inline
Unit::~Unit () 
{
  if (_data) {
    _data->deref ();
  }
} // Unit::~Unit


// 25/08/2002 Torrevieja
inline
Unit::Unit (const Unit& t)
  :
  _data (t._data)
{
  if (_data) {
    _data->ref ();
  }
} // Unit::Unit


// rebuild unit from data disguised as void pointer 
// 11/09/2002 Manchester
inline
Unit::Unit (void* d)
  :
  _data (reinterpret_cast<Data*>(d))
{
  _data->ref();
} // Unit::Unit


inline
Unit::Unit (char* name, InputType untype, const Formula& formula) 
  : 
  _data (new Data(name, untype, formula))
{
} // Unit::Unit


inline
Unit::Unit (char* name, InputType untype, const Clause& clause) 
  : 
  _data (new Data(name, untype, clause)) 
{
} // Unit::Unit


inline
Unit::Unit (InferenceRule ir, const Formula& f, const Unit& parent) 
  : 
  _data (new Data(ir, f, parent))
{
} // Unit::Unit


inline
Unit::Unit (InferenceRule ir, const Clause& clause, const Unit& parent) 
  : 
  _data (new Data(ir, clause, parent)) 
{
} // Unit::Unit


inline
Unit::Unit (const Clause& clause, const UnitList& parents) 
  : 
  _data (new Data(clause, parents)) 
{
} // Unit::Unit


inline
Unit::Unit (const Formula& formula, const UnitList& parents) 
  : 
  _data (new Data(formula, parents)) 
{
} // Unit::Unit


inline
bool Unit::operator == (const Unit& rhs) const
{ 
  ASS (_data && rhs._data);

  return _data == rhs._data; 
} // Unit::operator ==


inline
UnitType Unit::unitType () const
{
  return _data->unitType ();
} // Unit::unitType ()


inline
const Formula& Unit::formula () const
{
  ASS (unitType() == FORMULA);

  return _data->formula();
} // Unit::formula ()


inline
void Unit::setFormula (Formula f)
{
  _data->setFormula(f);
} // Unit::setFormula ()


inline
void Unit::setDefType (DefType dt)
{
  _data->setDefType (dt);
} // Unit::setDefType ()


inline
const Clause& Unit::clause () const
{
  ASS (unitType() == CLAUSE);

  return _data->clause();
} // Unit::clause ()


inline
InputType Unit::inputType () const
{
  return _data->inputType ();
} // Unit::inputType


inline
DefType Unit::defType () const
{
  return _data->defType ();
} // Unit::defType


inline
long Unit::number () const
{
  return _data->number ();
} // Unit::number


inline
InferenceRule Unit::rule () const
{
  return _data->rule ();
} // Unit::rule


inline
const UnitList& Unit::parents () const
{
  return _data->parents ();
} // Unit::parents


inline
char* Unit::name () const
{
  return _data->name ();
} // Unit::name


// to interface with the parser
// should be used with extreme care since _data can be destroyed
// after giving away
inline
void* Unit::giveAway ()
{
  return _data;
} // Unit::giveAway ()


// **************** Unit::Data definitions *********************


inline
void Unit::Data::ref () 
{ 
  ASS (this);

  _counter++;
} // Unit::Data::ref ()


inline
void Unit::Data::deref () 
{ 
  ASS (this);
  ASS (_counter > 0);
  _counter--;

  if (_counter == 0) {
    delete this;
  }
} // Unit::Data::deref ()


inline
UnitType Unit::Data::unitType () const
{
  return _untype;
} // Unit::unitType ()


inline
const Formula& Unit::Data::formula () const
{
  return _formula;
} // Unit::Data::formula ()


inline
void Unit::Data::setFormula (Formula f)
{
  _formula = f;
} // Unit::Data::setFormula ()


inline
void Unit::Data::setDefType (DefType dt)
{
  _defType = dt;
} // Unit::Data::setDefType ()


inline
const Clause& Unit::Data::clause () const
{
  return _clause;
} // Unit::Data::clause ()


// return the parents of a unit
// 22/07/2002 Manchester
// 11/09/2002 Manchester, changed
inline
const UnitList& Unit::Data::parents () const
{
  return _parents;
} // Unit::Data::parents


inline
InputType Unit::Data::inputType () const
{
  return _intype;
} // Unit::Data::inputType


inline
DefType Unit::Data::defType () const
{
  return _defType;
} // Unit::Data::defType


inline
long Unit::Data::number () const
{
  return _number;
} // Unit::Data::number


inline
char* Unit::Data::name () const
{
  return _name;
} // Unit::Data::name


inline
InferenceRule Unit::Data::rule () const
{
  return _rule;
} // Unit::Data::rule


// ******************* UnitList definitions ************************

inline
UnitList::UnitList () 
  : 
  Lst<Unit> ()
{
} // UnitList::UnitList


// copy constructor
// 25/08/2002 Torrevieja
inline
UnitList::UnitList (const UnitList& ts)
  :
  Lst<Unit> (ts)
{
} // UnitList::UnitList


// almost a copy constructor
// 25/08/2002 Torrevieja
inline
UnitList::UnitList (LstData<Unit>* d)
  :
  Lst<Unit> (d)
{
} // UnitList::UnitList


// 'cons' list constructor
// 25/08/2002 Torrevieja
inline
UnitList::UnitList (const Unit &hd, const UnitList& tl)
  :
  Lst<Unit> (hd,tl)
{
} // UnitList::UnitList


// 'cons' list constructor
// 25/08/2002 Torrevieja
inline
UnitList::UnitList (const Unit &hd)
  :
  Lst<Unit> (hd)
{
} // UnitList::UnitList


#endif // __unit__
