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
//  file sym_counter.cpp
//  implements class SymCounter counting occurrences of function and predicate symbols
//
//  01/05/2002, Manchester
//


#include "SymCounter.hpp"
#include "Problem.hpp"


SymCounter* SymCounter::norm = 0;


//  01/05/2002, Manchester
SymCounter::SymCounter (Signature& sig)
  :
  _noOfPreds (sig.noOfPreds()),
  _noOfFuns (sig.noOfFuns()),
  _preds ( new Pred[_noOfPreds] ),
  _funs ( new Fun[_noOfFuns] )
{
  Signature::Iterator ps (sig._pbuckets);

  while ( ps.more() ) {
    Signature::Pred* p = static_cast<Signature::Pred*>(ps.next());
    _preds[p->number()].pred(p);
  }

  Signature::Iterator fs (sig._fbuckets);
  while ( fs.more() ) {
    Signature::Fun* f = static_cast<Signature::Fun*>(fs.next());
    _funs[f->number()].fun(f);
  }
} // SymCounter::SymCounter


//  01/05/2002, Manchester
SymCounter::~SymCounter ()
{
  delete [] _preds;
  delete [] _funs;
} // SymCounter::~SymCounter


// this function is for debugging only
//  01/05/2002, Manchester
// 6/6/2003 Manchester, changed to a method of SymCounter
void SymCounter::output (ostream& str) const
{
  str << "Predicates\n";

  for (int i = 0; i < _noOfPreds; i++ ) {
    Pred& p = _preds[i];
    str << "  " << p.pred() << "(" 
        << p.nocc() << ',' 
        << p.docc() << ',' 
        << p.pocc() << ")\n";
  }

  str << "Functions\n";

  for (int j = 0; j < _noOfFuns; j++ ) {
    Fun& p = _funs[j];
    str << "  " << p.fun() << "(" 
        << p.occ() << ")\n";
  }
} // SymCounter::output (ostream& str) const


// c must be 1 or -1, 1 means add number of occurrences for each symbol, -1 means subtract
//  01/05/2002, Manchester
void SymCounter::count (Problem& p,int c)
{
  UnitChain::Iterator us ( p.units() );
  while ( us.more() ) {
    count (us.next(),c);
  }
} // SymCounter::count (Problem& p,int c)


//  01/05/2002, Manchester
void SymCounter::count (Unit u,int c)
{
  switch ( u.unitType () ) {
    case FORMULA:
      count (u.formula(), 1, c);
      return;

    case CLAUSE:
      count (u.clause(), c);
      return;
  }
} // SymCounter::count (Unit* u,int c)


// 01/05/2002, Manchester
// 11/09/2002 Manchester, changed
void SymCounter::count (Clause u,int add)
{
  Iterator<Literal> ls (u.literals());
  while ( ls.more() ) {
    count (ls.next(), add);
  }
} // SymCounter::count (Clause* u,int c)


//  01/05/2002, Manchester
void SymCounter::count (Formula f,int polarity,int add)
{
  switch ( f.connective() ) {
    case Formula::ATOM:
      count ( f.atom(), polarity, add );
      return;

    case Formula::AND:
    case Formula::OR: {
      Iterator<Formula> fs (f.args());
      while ( fs.more() ) {
        count ( fs.next(), polarity, add );
      }
      return;
    }

    case Formula::IMP:
      count ( f.left(), -polarity, add );
      count ( f.right(), polarity, add );
      return;

    case Formula::NOT:
      count ( f.uarg(), -polarity, add );
      return;

    case Formula::IFF:
    case Formula::XOR:
      count ( f.left(), 0, add );
      count ( f.right(), 0, add );
      return;

    case Formula::FORALL:
    case Formula::EXISTS:
      count ( f.qarg(), polarity, add );
      return;

    default:
      ASS(false);
      return;
  }
} // SymCounter::count (Formula* f,...)


//  01/05/2002, Manchester
void SymCounter::count (Literal l,int add)
{
  count (l.atom(), l.positive() ? 1 : -1, add);
} // SymCounter::count (Literal* l,int c)


//  01/05/2002, Manchester
void SymCounter::count (Atom a,int polarity,int add)
{
  get(a.functor())->add (polarity,add);

  Iterator<Term> ts (a.args());
  while ( ts.more() ) {
    count (ts.next(), add);
  }
} // SymCounter::count (Atom* f, ...)


//  01/05/2002, Manchester
// 28/09/2002 Manchester, changed to include numeric data
void SymCounter::count (Term t,int add)
{
  switch (t.tag())
    {
    case Term::VAR:
    case Term::NUMERIC:
      return;

    case Term::COMPOUND:
      {
	get(t.functor())->add (add);

	Iterator<Term> ts (t.args());
	while ( ts.more() ) {
	  count (ts.next(), add);
	}
      }
    }
} // SymCounter::count (Term* f, ...)


void SymCounter::Pred::add (int polarity, int add)
{
  switch (polarity) {
    case -1:
      _nocc += add;
      return;

    case 0:
      _docc += add;
      return;

    case 1:
      _pocc += add;
      return;

    default:
      ASS(false);
      return;
  }
} // SymCounter::Pred::add
