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
//  file Formula.hpp
//  defines class Formula
//

#ifndef __formula__
#define __formula__


#include "Atom.hpp"


class Position;
class Substitution;
class FormulaList;
class InferenceList;


class Formula 
{
 private:
  class Data;
  class AtomicData;
  class UnaryData;
  class BinaryData;
  class QuantifiedData;
  class JunctionData;

 public:
  enum Connective {
    ATOM = 0,     // WARNING: the order is important for
    AND = 1,      // normalize() to work and for printing 
    OR = 2,       // names of connectives
    IMP = 3,
    IFF = 4,
    XOR = 5,
    NOT = 6,
    FORALL = 7,
    EXISTS = 8
  }; // enum Formula::Connective

 public:
  // constructors/destructors
  Formula ();
  Formula (const Formula& f);
  ~Formula ();
  void operator= (const Formula& rhs);
  Formula ( Connective c, const Formula& l, const Formula& r); // for binary formulas
  Formula ( Connective c, const Formula& a );            // for unary formulas
  Formula ( Connective c, const FormulaList& fs );       // for and/or formulas
  Formula (Connective c, const VarList& vs, const Formula& a); // for quantified formulas
  explicit Formula ( const Atom& a );                    // for atomic formulas
  void makeJunction (Connective c, Formula& lhs, Formula& rhs); // for or-and formulas

  // structure
  Connective connective () const;
  const FormulaList& args () const;
  const Formula& left () const;
  const Formula& right () const;
  const Formula& qarg () const;
  const VarList& vars () const;
  const Formula& uarg () const;
  const Atom& atom () const;
  bool operator == (const Formula& rhs) const;
  bool isNull () const; 
  void makeNull (); 

  // declared but not defined, to prevent on-heap allocation
  void* operator new (size_t);

  // miscellaneous
  bool isEqualityAxiom () const;  // is one of the equality axioms
  bool hasFreeVars () const; 
  void flatten ();  // flatten the formula
  void ennf (bool polarity);
  void rectify (); // renames variables in the order of their occurrence
  void rectify (VarList& originalVars, TermList& answerAtom); // same
  void rectify (Substitution&, Var& last, VarList& freeVars); 
  void removeIff ();
  void skolemizeNNF ();
  void skolemizeNNF (Substitution& subst, VarList); 
  bool isPredicateDefinition (Atom& lhs, Formula& rhs ) const;
  bool isFunctionDefinition (Term& lhs, Term& rhs) const; 
  bool occurs (const Signature::Pred* p) const; // predicate symbol f occurs in the formula
  int universalPrefixLength () const; 

  // miniscoping
  void miniscope (InferenceList&);
  void miniscope (InferenceList&, const Position&);
  void occurring (const VarList& input, 
		  VarList& output, 
		  VarList& removedVars) const; 
  void topMiniscope (Connective, const VarList&, 
		     InferenceList&, const Position&);
 private:
  // structure
  Data* _data;

  // auxiliary functions
  bool hasFreeVars (VarListList) const; 
 
  // equality axiom check
  bool isReflexivityAxiom () const;
  bool isFunctionReflexivityAxiom () const;
  bool isPredicateReflexivityAxiom () const;
  bool isSymmetryAxiom () const;
  bool isTransitivityAxiom () const;

  // miniscoping
  void occurring (bool* occurrences, Var max) const;
  static void splitVarList (const VarList& in, 
			    VarList& out, 
			    VarList& removed,
			    bool* occurrences);
}; // class Formula


class FormulaList 
  : public Lst <Formula>
{
 public:
  // constructors
  FormulaList ();
  FormulaList (const FormulaList&);
  explicit FormulaList (const Formula& t); // one-element list
  FormulaList (const Formula& head, const FormulaList& tail);
  explicit FormulaList (LstData<Formula>*);

  // inherited functions
  const FormulaList& tail () const
    { return static_cast<const FormulaList&>(Lst<Formula>::tail()); }

  // miscellaneous
  void flatten (Formula::Connective);
  void ennf (Formula::Connective andOr, bool polarity);
  void rectify (Substitution&, Var& last, VarList& freeVars); 
  void removeIff ();
  void skolemizeNNF (Substitution& subst, VarList); 
  void miniscope (Formula::Connective con, 
		  InferenceList&, const Position&, 
		  int argNumber, 
		  const FormulaList& lst);
  void topMiniscope (Formula::Connective, 
		     const VarList& vars,
		     InferenceList& inf,
		     const Position& pos,
		     int index); // auxiliary for Formula::topMiniscope
  static void appendN (const FormulaList& fst, 
		       const FormulaList& snd,
		       int N,
		       FormulaList& result); // auxiliary for miniscope/5
}; // class FormulaList


class Formula::Data 
{
 public:
  Data (Connective c);
  ~Data ();

  void ref ();
  void deref ();

  Connective connective () const;

 protected:
  // structure
  int _counter;
  Connective _connective;

  // auxiliary functions
  void destroy ();
}; // class Formula::Data


class Formula::AtomicData :
  public Formula::Data
{
 public:
  explicit AtomicData (const Atom& atom);

  const Atom& atom () const;

 protected:
  // structure
  Atom _atom;
}; // class Formula::AtomicData


class Formula::UnaryData :
  public Formula::Data
{
 public:
  UnaryData (Connective c, const Formula& arg);

  const Formula& arg () const;

 protected:
  // structure
  Formula _arg;
}; // class Formula::UnaryData


class Formula::BinaryData :
  public Formula::Data
{
 public:
  BinaryData (Connective c, const Formula& lhs, const Formula& rhs);

  const Formula& left () const;
  const Formula& right () const;

 protected:
  // structure
  Formula _lhs;
  Formula _rhs;
}; // class Formula::BinaryData


class Formula::JunctionData :
  public Formula::Data
{
 public:
  JunctionData (Connective c, const FormulaList& lhs);

  const FormulaList& args () const;

 protected:
  // structure
  FormulaList _args;
}; // class Formula::JunctionData


class Formula::QuantifiedData :
  public Formula::Data
#   if DEBUG_PREPRO
    , public Memory <CID_QFORMULA>
#   endif
{
 public:
  QuantifiedData (Connective c, const VarList& vs, const Formula& arg);

  const Formula& arg () const;
  const VarList& vars () const;

 protected:
  // structure
  VarList _vars;
  Formula _arg;
}; // class Formula::QuantifiedData


// ******************* Formula definitions ************************

inline
Formula::Formula () 
  : 
  _data (0) 
{
} // Formula::Formula


// 15/04/2003 Torrevieja
inline
bool Formula::isNull () const
{
  return _data == 0;
} // Formula::isNull


// for quantified formulas
// 29/08/2002 Torrevieja
inline
Formula::Formula (Connective c, const VarList& vs, const Formula& a)
  :
  _data (new QuantifiedData(c,vs,a))
{
} // Formula::Formula (Connective c, const VarList& vs, const Formula& a)


// for atomic formulas
// 30/08/2002 Torrevieja
inline
Formula::Formula (const Atom& atom)
  :
  _data (new AtomicData(atom))
{
} // Formula::Formula (Connective c, const Atom& arg)


// for unary formulas
// 29/08/2002 Torrevieja
inline
Formula::Formula (Connective c, const Formula& arg)
  :
  _data (new UnaryData(c,arg))
{
} // Formula::Formula (Connective c, const Formula& arg)


// for binary formulas
// 29/08/2002 Torrevieja
inline
Formula::Formula (Connective c, const Formula& lhs, const Formula& rhs)
  :
  _data (new BinaryData(c,lhs,rhs))
{
} // Formula::Formula (Connective c, const Formula& lhs, const Formula& rhs)


// for multi-ary formulas
// 30/08/2002 Torrevieja
inline
Formula::Formula (Connective c, const FormulaList& args)
  :
  _data (new JunctionData(c,args))
{
} // Formula::Formula (Connective c, const FormulaList& args)


inline
Formula::~Formula () 
{
  if (_data) {
    _data->deref ();
  }
} // Formula::~Formula


// 25/08/2002 Torrevieja
inline
Formula::Formula (const Formula& t)
  :
  _data (t._data)
{
  if (_data) {
    _data->ref ();
  }
} // Formula::Formula


inline
bool Formula::operator == (const Formula& rhs) const
{ 
  ASS (_data && rhs._data);

  return _data == rhs._data; 
} // Formula::operator ==


inline
Formula::Connective Formula::connective () const
{
  return _data->connective();
} // Formula::args ()


inline
const Atom& Formula::atom () const
{
  ASS (connective() == ATOM);

  return (static_cast<AtomicData*>(_data))->atom();
} // Formula::atom ()


inline
const FormulaList& Formula::args () const
{
  ASS (connective() == AND || connective() == OR);

  return (static_cast<JunctionData*>(_data))->args();
} // Formula::args ()


inline
const Formula& Formula::left () const
{
  ASS (connective() == IFF || 
                      connective() == XOR || 
                      connective() == IMP);

  return (static_cast<BinaryData*>(_data))->left();
} // Formula::left ()


inline
const Formula& Formula::right () const
{
  ASS (connective() == IFF || 
                      connective() == XOR || 
                      connective() == IMP);

  return (static_cast<BinaryData*>(_data))->right();
} // Formula::right ()


inline
const Formula& Formula::uarg () const
{
  ASS (connective() == NOT);

  return (static_cast<UnaryData*>(_data))->arg();
} // Formula::uarg ()


inline
const Formula& Formula::qarg () const
{
  ASS (connective() == FORALL || connective() == EXISTS);

  return (static_cast<QuantifiedData*>(_data))->arg();
} // Formula::qarg ()


inline
const VarList& Formula::vars () const
{
  ASS (connective() == FORALL || connective() == EXISTS);

  return (static_cast<QuantifiedData*>(_data))->vars();
} // Formula::vars ()


// **************** Formula::Data definitions *********************

inline
Formula::Data::Data (Connective c)
  : 
  _counter (1),
  _connective (c)
{
} // Formula::Data::Data (bool isVar)


inline 
Formula::Data::~Data ()
{
  TRACER( "Formula::Data::~Data" );

  ASS (_counter == 0);
} // Formula::Data::~Data ()


inline
void Formula::Data::ref () 
{ 
  ASS (this);

  _counter++;
} // Formula::Data::ref ()


inline
void Formula::Data::deref () 
{ 
  ASS (this);
  ASS (_counter > 0);
  _counter--;

  if (_counter == 0) {
    destroy ();
  }
} // Formula::Data::deref ()


inline
Formula::Connective Formula::Data::connective () const
{ 
  ASS (this);

  return _connective;
} // Formula::Data::connective ()


// **************** Formula::AtomicData definitions *********************


inline
Formula::AtomicData::AtomicData (const Atom& atom) 
  :
  Data (ATOM),
  _atom (atom)
{ 
} // AtomicData::AtomicData


inline
const Atom& Formula::AtomicData::atom () const
{
  return _atom; 
} // Formula::AtomicData::atom ()


// **************** Formula::UnaryData definitions *********************


inline
Formula::UnaryData::UnaryData (Connective c, 
			       const Formula& arg) 
  :
  Data (c),
  _arg (arg)
{ 
  ASS ( c == NOT );
} // UnaryData::UnaryData


inline
const Formula& Formula::UnaryData::arg () const
{
  return _arg; 
} // Formula::UnaryData::arg ()


// **************** Formula::BinaryData definitions *********************


inline
Formula::BinaryData::BinaryData (Connective c, 
				 const Formula& lhs, 
				 const Formula& rhs)
  :
  Data (c),
  _lhs (lhs),
  _rhs (rhs)
{ 
  ASS ( c == IMP || c == IFF || c == XOR );
} // BinaryData::BinaryData


inline
const Formula& Formula::BinaryData::left () const
{
  return _lhs; 
} // Formula::BinaryData::left ()


inline
const Formula& Formula::BinaryData::right () const
{
  return _rhs; 
} // Formula::BinaryData::right ()


// **************** Formula::JunctionData definitions *********************


inline
Formula::JunctionData::JunctionData (Connective c, const FormulaList& args) 
  :
  Data (c),
  _args (args)
{ 
  ASS ( c == AND || c == OR );
} // JunctionData::JunctionData


inline
const FormulaList& Formula::JunctionData::args () const
{
  return _args; 
} // Formula::JunctionData::args ()


// **************** Formula::QuantifiedData definitions *********************


inline
Formula::QuantifiedData::QuantifiedData (Connective c, 
					 const VarList& vs, 
					 const Formula& arg)
  :
  Data (c),
  _vars (vs),
  _arg (arg)
{ 
  ASS ( c == FORALL || c == EXISTS );
} // QuantifiedData::QuantifiedData


inline
const Formula& Formula::QuantifiedData::arg () const
{
  return _arg; 
} // Formula::QuantifiedData::arg ()


inline
const VarList& Formula::QuantifiedData::vars () const
{
  return _vars; 
} // Formula::QuantifiedData::vars ()


// ******************* FormulaList definitions ************************

inline
FormulaList::FormulaList () 
  : 
  Lst<Formula> ()
{
} // FormulaList::FormulaList


// copy constructor
// 25/08/2002 Torrevieja
inline
FormulaList::FormulaList (const FormulaList& ts)
  :
  Lst<Formula> (ts)
{
} // FormulaList::FormulaList


// almost a copy constructor
// 25/08/2002 Torrevieja
inline
FormulaList::FormulaList (LstData<Formula>* d)
  :
  Lst<Formula> (d)
{
} // FormulaList::FormulaList


// 'cons' list constructor
// 25/08/2002 Torrevieja
inline
FormulaList::FormulaList (const Formula &hd, const FormulaList& tl)
  :
  Lst<Formula> (hd,tl)
{
} // FormulaList::FormulaList


// one-element list constructor
// 25/08/2002 Torrevieja
inline
FormulaList::FormulaList (const Formula &hd)
  :
  Lst<Formula> (hd)
{
} // FormulaList::FormulaList


#endif // __formula__
