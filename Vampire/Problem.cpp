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
//  File problem.cpp
//
//  implements the class Problem
//
//  started 14/05/2000, Dallas
//

#include <string>
#include <climits>
#include <cstdlib>


#include "Problem.hpp"
#include "Options.hpp"
#include "SymCounter.hpp"
#include "Inference.hpp"


// 30/04/2002 Manchester, changed
// 11/09/2002 Manchester
Problem::Problem (const UnitList& lst)
  : 
  _hasFormulas (false)
{
  // transform list into chain
  Iterator<Unit> us (lst);

  while ( us.more() ) {
    Unit u (us.next());
    switch ( u.unitType() ) {
      case FORMULA:
        _hasFormulas = true;
        // negate conjectures formula (unpleasant feature of TPTP...)
        if ( u.inputType() == CONJECTURE ) {
          Formula f (Formula::NOT, u.formula());
          u.setFormula (f);
        }
        break;

      default:
        break;
    }
    _units.addLast (u);
  }
} // Problem::Problem


Problem::~Problem ()
{
  // destroy all input units in the chain
  while (! _units.isEmpty() ) {
    _units.delFirst ();
  }
} // Problem::~Problem


void Problem::preprocess (Options& opt)
{
  TRACER("Problem::preprocess");

  // options related to the input 
  if ( opt.arityCheck() && ! sig->arityCheck() ) { 
    throw MyException ("the same symbol is use either with two different arities or as both a function and a predicate symbol");
  }
  Formula f;
  if ( opt.noFreeVars() ) { 
    if (hasFreeVars(f)) {
      throw MyException ("formula with free variables occurs in the input");
    }
  }

  rectify ();
  flatten ();

  // option-independant processing
  // removed to speed up the KIF mode
  switch (options->mode()) 
    {
    case Options::KIF: 
      break;
      
    default:
      removeEqualityAxioms ();
      if (_hasFormulas) {
	findDefinitions ();
	if ( opt.removeUnusedDefs() ) {
	  removeUnusedDefinitions ();
	}
      }
    }

  // transform all formulas into ennf
  ennf ();

  // here should be naming
  removeIff ();

  ASS ( ! hasFreeVars(f));
  if ( opt.miniscope () ) {
    miniscope ();
  }

  skolemize ();
  clausify ();
} // Problem::preprocess


/*
// finds and removes all relational definitions from the problem
int Problem::removeAllRelationalFunctionDefinitions ( ostream& tab )
{  
  int removed = 0;

  while ( removeOneRelationalFunctionDefinition ( tab ) ) 
    removed ++;

  return removed;
} // Problem::removeAllRelationalFunctionDefinitions


// finds and removes all relational definitions from the problem
bool Problem::removeOneRelationalFunctionDefinition ( ostream& tab )
{  
  Unit::List::Iterator it ( _units );
  while ( it.more() ) {
    Unit* unit = it.next ();
    PSymbol* pred;
    int argNo;
    // check that the current unit is P(X,y) & P(X,z) -> y = z
    if ( unit->isUniquenessAxiom (pred, argNo) )  {
      FSymbol* fun;
      Var* vs;
      // check that the current clause is P(X,f(X))
      Atom* atom;
      Unit* funax;
      Unit::List::Iterator ulit ( _units );
      while ( ulit.more() ) {
        funax = ulit.next();
        vs = funax->isFunctionalityAxiom (pred, fun, argNo, atom);
        if ( vs ) {
          // output inference( P(X,y) --> y = f(X), replace_P_by_f, [ppp,fff] ).
          // note that atom is P(X,f(X))
          tab << "inference(" << pred << "(";
          TermList args (atom->args());
          int arg = 0;
          for (;;) {
            if ( arg == argNo ) 
              tab << "Y";
            else
              tab << args.head ();
            args.pop();
            if ( args->isEmpty() )
              break;
            // args is nonempty
            tab << ",";
            arg++;
          }
          tab << ") --> Y=" << atom->args()->nth(argNo) 
              << ",replace_" << pred << "_by_" << fun 
              << ",[" << unit->name() << "," << funax->name() << "]).\n";

          // remove the uniqueness axiom and the functionality axiom from the set
          _units = _units->remove ( unit );
          _units = _units->remove ( funax );

          // replace P(S,t) by t = f(S) in all units
          replaceRelationByFunction ( pred, fun, argNo, vs, tab );
          return true;
        }
      }
    }
  }

  return false;
} // Problem::removeOneRelationalFunctionDefinition


// find all cancellation axioms
//   f(X,y,Z) = f(X,u,Z) -> y = u
// and replace them by 
//   i(f(X,y,Z),X,Z) = y
int Problem::replaceCancellationAxioms ( ostream& tab )
{  
  Unit::List::DelIterator it ( _units );
  int found = 0;
  while ( it.more() ) {
    Unit* unit = it.next ();
    FSymbol* fun;
    int argNo;
    if ( unit->isCancellationAxiom (fun, argNo) )  {
      found ++;
      // create new function symbol of arity 2 * arity (f) - 1
      FSymbol* inv = FSymbol::createNew ( "inv", fun->arity() * 2 - 1);
      unit->replaceCancellation (fun,argNo,inv,tab);
    }
  }

  return found;
} // Problem::replaceCancellationAxioms


int Problem::countOccurrences ( const PSymbol* p ) const
{
  int occ = 0;

  Unit::List::Iterator us (_units);
  while ( us.more() ) {
    occ += us.next()->countOccurrences (p);
  }
    
  return occ;
} // Problem::countOccurrences


bool Problem::occurs ( const FSymbol* f ) const
{
  Unit::List::Iterator us (_units);
  while ( us.more() ) {
    if ( us.next()->occurs (f) )
      return true;
  }
    
  return false;
} // Problem::occurs


bool Problem::occurs ( const PSymbol* p ) const
{
  Unit::List::Iterator us (_units);
  while ( us.more() ) {
    if ( us.next()->occurs (p) )
      return true;
  }
    
  return false;
} // Problem::occurs


int Problem::countOccurrences ( const FSymbol* f ) const
{
  int occ = 0;

  Unit::List::Iterator us (_units);
  while ( us.more() ) {
    occ += us.next()->countOccurrences (f);
  }
    
  return occ;
} // Problem::countOccurrences


void Problem::printDefinitions ( ostream& str )
{
  Term lhs;
  Term rhs;
  Unit* unit;

  Unit::List::Iterator units (_units);
  while ( units.more() ) {
    unit = units.next();
    ASS( debug_problem, unit->unitType() == CLAUSE );

    lhs = unit->isDefinition(rhs);
    if ( lhs ) {
      str << "Unit " 
          << unit->name() 
          << " is a definition of the function "
          << lhs->functor() << '\n'
          << "  having "
          << countOccurrences ( lhs->functor() )
          << " occurrences in the problem\n";
    }
  }
} // Problem::printDefinitions


*/


// the result is the number of removed symbols
// 03/06/2001
// 02/05/2002 Manchester, changed considerably using 
// 10/05/2002 Manchester, changed completely
// 27/09/2002 Manchester, changed to new datastructures
// 16/04/2003 Torrevieja, bug fixed causing the resulting formula
//            have free variables
int Problem::removeUnusedDefinitions ()
{
  TRACER ( "Problem::removeUnusedDefinitions" );

  // first, count the number of symbols
  SymCounter cnt (*sig);
  cnt.count (*this,1);

  int removed = 0;
  bool changed = true;

  while (changed) {
    changed = false;
    UnitChain::DelIterator us (units());
    while ( us.more() ) {
      Unit u (us.next ());
      switch ( u.defType() ) 
	{
	case  DT_NONE:
          break;

        case DT_PRED: // finding lhs
	  {
	    Formula f (u.formula ());
	    if ( f.connective() == Formula::FORALL ) {
	      f = f.qarg ();
	    }
	    // now f is a definition P(x) <=> A[x]
	    ASS (f.connective() == Formula::IFF &&
		 f.left().connective() == Formula::ATOM );
	    Signature::Pred* p = f.left().atom().functor();
	    SymCounter::Pred* pcount = cnt.get (p);
	    if ( pcount->docc() > 1 ) { // more than 1 occurrence under <=>
	      // nothing can be done
	      break;
	    }
	    // there must be one occurrence in the definition itself!
	    ASS (pcount->docc() == 1); 
	    if ( pcount->pocc() > 0 ) { // there are positive occurrences
	      if ( pcount->nocc() > 0 ) { // there are also negative occurrences
		// nothing can be done
		break;
	      }
	      // positive occurrences only
	      // change P(x) <=> A[x] to P(x) => A[x]
	      Formula g (Formula::IMP, f.left(), f.right());
	      if (u.formula().connective() == Formula::FORALL) {
		g = Formula (Formula::FORALL, u.formula().vars(), g);
	      }
	      Unit v (IR_HALF_EQUIV, g, u);
	      
	      cnt.count (u,-1);
	      cnt.count (v,1);
	      us.replace (v);

	      removed++;
	      changed = true;
	      break;
	    }
	    // no positive occurrences
	    if ( pcount->nocc() > 0 ) { // negative occurrences only
	      // positive occurrences only
	      // change P(x) <=> A[x] to A[x] => P(x)
	      Formula g (Formula::IMP, f.right(), f.left());
	      if (u.formula().connective() == Formula::FORALL) {
		g = Formula (Formula::FORALL, u.formula().vars(), g);
	      }
	      Unit v (IR_HALF_EQUIV, g, u);

	      cnt.count (u,-1);
	      cnt.count (v,1);
	      us.replace (v);

	      removed++;
	      changed = true;
	      break;
	    }
	    // no occurrences at all, the unit can be deleted
	    // (1) reset the counter
	    cnt.count (u,-1);
	    // (2) delete the unit from the chain and from the memory
	    us.del ();
	    changed = true;
	    
	    removed ++;
	    break;
	  }

        case DT_FUN: 
	  {
	    Signature::Fun* f;
	    if (u.unitType() == CLAUSE) {
	      Clause c (u.clause ());
	      // now c is a definition f(x) = t[x]
	      f = c.literals().head().atom().args().head().functor();
	    }
	    else { // formula
	      Formula form (u.formula ());
	      while ( form.connective() == Formula::FORALL ) {
		form = form.qarg ();
	      }
	      // now form is a definition f(x) = t[x]
	      ASS (form.connective() == Formula::ATOM);
	      f = form.atom().args().head().functor();
	    }

	    SymCounter::Fun* fcount = cnt.get (f);
	    if ( fcount->occ() > 1 ) { // more than 1 occurrence 
	      // nothing can be done
	      break;
	    }
	    // there must be one occurrence in the definition itself!
	    ASS (fcount->occ() == 1); 

	    // no occurrences at all, the unit can be deleted
	    // (1) reset the counter
	    cnt.count (u,-1);
	    // (2) delete the unit from the chain and from the memory
	    us.del ();
	    changed = true;
          
	    removed ++;
	    break;
	  }
	}
    }
  }

  return removed;
} // Problem::removeUnusedDefinitions


/*
void Problem::applyFunDef ( const Term* lhs, const Term* rhs, const Unit* def, ostream& report ) 
{
  ReplaceCode code ( lhs, rhs );

  Unit::List::Iterator us ( _units );
  while ( us.more () ) {
    Unit* unit = us.next ();
    unit->apply (code,def,report);
  }
} // Problem::applyFunDef


bool Problem::hasEquality () const
{
  Unit::List::Iterator unit ( _units ); 
  while ( unit.more() ) {
    if ( unit.next()->hasEquality() )
      return true;
  }
  
  return false;
} // Problem::hasEquality


bool Problem::isHorn () const
{
  Unit::List::Iterator unit ( _units ); 
  while ( unit.more() ) {
    if ( ! unit.next()->isHorn() )
      return false;
  }
  
  return true;
} // Problem::isHorn


// 06/06/2001 made from pureEquality
bool Problem::hasNonEquality () const
{
  Unit::List::Iterator unit ( _units ); 
  while ( unit.more() ) {
    if ( unit.next()->hasNonEquality() )
      return true;
  }
  
  return false;
} // Problem::hasNonEquality


bool Problem::hasFunctionSymbols () const
{
  Unit::List::Iterator unit ( _units ); 
  while ( unit.more() ) {
    if ( unit.next()->hasFunctionSymbols() )
      return true;
  }
  
  return false;
} // Problem::hasFunctionSymbols


bool Problem::isPropositional () const
{
  Unit::List::Iterator unit ( _units ); 
  while ( unit.more() ) {
    if ( ! unit.next()->isPropositional() )
      return false;
  }
  
  return true;
} // Problem::isPropositional


int Problem::numberOfClauses () const
{
  int result = 0;

  Unit::List::Iterator unit ( _units ); 
  while ( unit.more() ) {
    if ( ! unit.next()->unitType() == CLAUSE )
      result++;
  }

  return result;
} // Problem::numberOfClauses


int Problem::numberOfLiterals () const
{
  int result = 0;

  Unit::List::Iterator unit ( _units ); 
  while ( unit.more() ) {
    result += unit.next()->numberOfLiterals();
      result++;
  }

  return result;
} // Problem::numberOfLiterals


bool Problem::isUnit () const
{
  Unit::List::Iterator unit ( _units ); 
  while ( unit.more() ) {
    if ( ! unit.next()->isUnit() )
      return false;
  }
  
  return true;
} // Problem::isUnit


// appends statistics to the file
void Problem::statistics ( ostream& str ) const
{ 
  str << "problem_info( '" 
      << name ()
      << "', [equality("
      << (isUnit() ? "unit" : ( hasEquality() ? ( hasNonEquality() ? "yes" : "pure" ) : "no" ))
      << "), horn(" 
      << (isHorn () ? "yes" : "no")
      << "), functions("
      << ( isPropositional() ? "prop" : ( hasFunctionSymbols () ? "yes" : "no") )
      << "), clauses("
      << numberOfClauses ()
      << "), literals("
      << numberOfLiterals ()
      << ")] ).\n";
} // Problem::statistics


void Problem::replaceRelationByFunction ( PSymbol* pred, FSymbol* fun, int argNo, int* vs, ostream& report )
{
  Unit::List::Iterator ii (_units);
  while ( ii.more() ) {
    Unit* unit = ii.next();
    ASS( debug_problem, unit->unitType() == CLAUSE );
    unit->replaceRelationByFunction (pred, fun, argNo, vs, report);
  }

  delete [] vs;
} // Problem::replaceRelationByFunction


int Problem::removeVarEquality ( ostream& tab )
{
  int removed = 0;

  Unit::List::Iterator inp ( _units );
  while ( inp.more () ) {
    if ( inp.next()->removeVarEquality (tab) ) 
      removed++;
  }

  return removed;
} // Problem::removeVarEquality


int Problem::removeTautologies ( ostream& tab )
{
  int removed = 0;

  Unit::List::DelIterator us (_units);
  while ( us.more() ) {
    if ( us.next()->isTautology (tab) ) {
      us.del ();
      removed++;
    }
  }

  return removed;
} // Problem::removeTautologies


bool Problem::splitLargeArity ( int maxArity )
{
  bool splitted = false;

  FSymbol** fsig = FSymbol::signature (); 
  for ( int j = FSymbol::noOfSymbols() - 1; j >= 0; j-- ) {
    FSymbol* f = fsig[j];
    int arity = f->arity ();
    if ( arity > maxArity ) {
      splitted = true;
      div_t qr = div ( arity, maxArity );
      int iterations = qr.quot;
      int restArity = qr.rem;
      // list storing new predicate symbols
      FSymbol::List* fs = FSymbol::List::empty ();
      if ( restArity ) {
        FSymbol::List::push ( FSymbol::createNew(f->name(),restArity), fs );
      }
      while ( iterations-- > 0 ) {
        FSymbol::List::push ( FSymbol::createNew(f->name(),maxArity), fs );
      }
      fs = fs->reverse ();

      Unit::List::Iterator us ( _units );
      while ( us.more() ) {
        us.next()->splitLargeArity (f, fs);
      }
      f->arity ( fs->length() );
      fs->destroy ();
    }
  }

  delete [] fsig;

  PSymbol** sig = PSymbol::signature (); 
  for ( int i = PSymbol::noOfSymbols() - 1; i >= 0; i-- ) {
    PSymbol* p = sig[i];
    int arity = p->arity ();
    if ( arity > maxArity ) {
      splitted = true;
      div_t qr = div ( arity, maxArity );
      int iterations = qr.quot;
      int restArity = qr.rem;
      // list storing new predicate symbols
      FSymbol::List* ps = FSymbol::List::empty ();
      if ( restArity ) {
        FSymbol::List::push ( FSymbol::createNew(p->name(),restArity), ps );
      }
      while ( iterations-- > 0 ) {
        FSymbol::List::push ( FSymbol::createNew(p->name(),maxArity), ps );
      }
      ps = ps->reverse ();

      Unit::List::Iterator us ( _units );
      while ( us.more() ) {
        us.next()->splitLargeArity (p, ps);
      }
      p->arity ( ps->length() );
      ps->destroy ();
    }
  }

  delete [] sig;

  return splitted;
} // Problem::splitLargeArity


int Problem::maxLitWeight ()
{
  int result = Int::min ();

  Unit::List::Iterator unit ( _units );
  while ( unit.more() ) {
    int mlcw = unit.next()->maxLitWeight(); 
    if ( mlcw > result )
      result = mlcw;
  }

  return result;
} // Problem::maxLitWeight


int Problem::cutTooLongClauses ( int maxLiterals )
{
  return 0;
}


void Problem::removeResolvable ( const PSymbol* pred, bool sign, ostream& report )
{
  Unit::List* pos = Unit::List::empty(); // clauses with a sign-occurrence of pred
  Unit::List* neg = Unit::List::empty(); // clauses with a not-sign-occurrence of pred

  Unit::List::DelIterator us (_units);
  while ( us.more() ) {
    Unit* u = us.next ();
    ASS( debug_problem, u->unitType() == CLAUSE );
    bool litSign;

    if ( u->clause()->occurs(pred,litSign) ) {
      us.del ();
      if ( sign == litSign ) { // move unit to pos
        Unit::List::push (u,pos);
      }
      else { // move it to neg
        Unit::List::push (u,neg);
      }
    }
  }

  ASS( debug_problem, pos->isNonEmpty() );
  ASS( debug_problem, neg->isNonEmpty() );
  ASS( debug_problem, pos->tail()->isEmpty() );

  // create all resolvents between pos and neg and move them to rest
  for ( Unit::List* p = pos; p->isNonEmpty(); p = p->tail() ) {
    for ( Unit::List* n = neg; n->isNonEmpty(); n = n->tail() ) {
      // create resolvent and add it to _units
      Unit::List::push ( p->head()->resolveOn (pred, n->head(),report), _units );
    }
  }

  Unit::List* first;
  // destroy pos and neg, and write report
  for ( first = pos; first->isNonEmpty(); first = first->tail() ) {
    Unit* u = first->head();
    report << "prepro(removed_as_resolved," << u->name() << u->clause() << "]).\n";
    u->destroy();
  }
  pos->destroy ();
  for ( first = neg; first->isNonEmpty(); first = first->tail() ) {
    Unit* u = first->head();
    report << "prepro(removed_as_resolved," << u->name() << "," << u->clause() << "]).\n";
    u->destroy();
  }
  neg->destroy ();
} // Problem::removeResolvable


int Problem::eliminateResolvable ( ostream& report )
{
  int symbols = PSymbol::noOfSymbols();
  if ( symbols == 0 ) {
    return 0;
  }

  int eliminated = 0;

  for (;;) {
    // starting next round 
    Resolvable res ( symbols );

    Unit::List::Iterator us ( _units );
    while ( us.more () ) {
      Unit* unit = us.next ();
      ASS( debug_problem, unit->unitType() == CLAUSE );

      if ( ! res.getClause(unit->clause()) ) {
        return eliminated;
      }
    }

    // round terminated
    bool sign;
    PSymbol* p = res.first (sign);
    if ( p ) {
      eliminated++;
      removeResolvable (p,sign,report);
    }
    else {
      return eliminated;
    }
  }
} // Problem::eliminateResolvable


int Problem::eliminatePure ( ostream& report )
{
  int symbols = PSymbol::noOfSymbols();
  if ( symbols == 0 ) {
    return 0;
  }

  // at least one symbol present
  int eliminated = 0;

  for (;;) {
    // one round of eliminating pure literals
    Pure pure ( symbols );

    Unit::List::Iterator us ( _units );
    while ( us.more () ) {
      Unit* unit = us.next ();
      ASS( debug_problem, unit->unitType() == CLAUSE );

      if ( ! pure.getClause(unit->clause()) ) {
        return eliminated;
      }
    }
    // round terminated, check if there are pure literals
    // and remove them
    int elim = 0;
    Pure::Iterator symb (pure);
    PSymbol* p;
    while ( (p = symb.next()) != 0 ) {
      elim++;
      removePredicate ( p, report );
    }
    if ( elim == 0 ) // no symbols eliminated this round
      return eliminated;
    eliminated += elim;
  } // for (;;)
} // Problem::eliminatePure


void Problem::removePredicate ( const PSymbol* p, ostream& report )
{
  Unit::List::DelIterator us ( _units );
  while ( us.more() ) {
    Unit* u = us.next ();
    if ( u->occurs (p) ) {
      u->destroy ();
      report << "prepro(removed_as_pure," << u->name() << "," 
             << p << ").\n";
      us.del ();
    }
  }
} // Problem::eliminatePure


int Problem::negativeEqualitySplit ( int thresholdWeight, ostream& tab )
{
  int changed = 0;

  Unit::List::Iterator us (_units);
  while ( us.more() ) {
    Unit::List* newUnits = us.next()->negativeEqualitySplit (thresholdWeight, tab);
    changed += newUnits->length();
    _units = Unit::List::concat (newUnits, _units);
  }

  return changed;
} // Problem::negativeEqualitySplit
*/


// 03/06/2001
int Problem::removeEqualityAxioms ()
{
  TRACER("Problem::removeEqualityAxioms");
  int removed = 0;

  UnitChain::DelIterator us (_units);
  while ( us.more() ) {
    Unit u (us.next());
    if ( u.isEqualityAxiom() ) {
      us.del();
      removed++;
    }
  }

  return removed;
} // Problem::removeEqualityAxioms


/*
// 06/06/2001
bool Problem::hasEqualityInGoal () const
{
  Unit::List::Iterator unit ( _units ); 
  while ( unit.more() ) {
    Unit* u = unit.next();
    if ( u->isGoal() && u->hasEquality() ) {
      return true;
    }
  }
  
  return false;
} // Problem::hasEquality


// 08/06/2001
int Problem::goalWeight ()
{
  int w = 0;
  Unit::List::Iterator unit ( _units ); 
  while ( unit.more() ) {
    Unit* u = unit.next();
    ASS( debug_problem, u->unitType() == CLAUSE );
    if ( u->isGoal() ) {
      w += u->clause ()->weight();
    }
  }
  
  return w;
} // Problem::goalWeight

*/


// if contains a formula with free variables return this formula
// 06/05/2002 Manchester
// 11/09/2002 Manchester, changed
bool Problem::hasFreeVars (Formula& f) const
{
  TRACER("Problem::hasFreeVars");

  UnitChain::Iterator unit (_units);
 
  while ( unit.more() ) {
    Unit u (unit.next());
    if ( u.formulaWithFreeVars() ) {
      f = u.formula();
      return true;
    }
  }
  
  return false;
} // Problem::hasFreeVars


// marks all units in the problem that are definitions
// returns the number of definitions found
// 09/05/2002 Manchester
int Problem::findDefinitions ()
{
  TRACER ( "Problem::findDefinitions" );

  int found = 0;

  UnitChain::DelIterator unit ( _units ); 
  while ( unit.more() ) {
    Unit u (unit.next ());
    Unit d;
    if (u.makeDefinition (d)) {
      found ++;
      if (! (u == d)) { // definition in a new unit
        unit.replace (d);
      }
    }
  }

  return found;
} // Problem::findDefinitions


// transform each formula into ennf
// 12/05/2002 Manchester
void Problem::ennf ()
{
  if (! _hasFormulas ) {
    return;
  }

  UnitChain::DelIterator unit ( _units ); 
  while ( unit.more() ) {
    Unit u (unit.next());
    switch ( u.unitType() ) {
      case CLAUSE:
        break;

      case FORMULA: {
        Formula f (u.formula());
        f.ennf (true);
	if (options->proof() && ! (f == u.formula())) { 
	  Unit v (IR_ENNF, f, u);
	  unit.replace (v);
	}
	else {
	  u.setFormula(f);
	}
      }
    }
  }
} // Problem::ennf


// remove equivalence and XOR from ennf
// 14/05/2002 Manchester
// 30/08/2002 Torrevieja, changed
int Problem::removeIff ()
{
  if (! _hasFormulas ) {
    return 0;
  }

  int removed = 0;

  UnitChain::DelIterator unit ( _units ); 
  while ( unit.more() ) {
    Unit u (unit.next());
    switch ( u.unitType() ) {
      case CLAUSE:
        break;

      case FORMULA: {
        Formula g (u.formula());
        g.removeIff ();
	Formula h;
	if (! (g == u.formula())) {
	  if (options->proof()) { 
	    Unit v (IR_RM_EQUIV, g, u);
	    g.ennf (true);
	    Unit w (IR_ENNF, g, v);
	    unit.replace (w);
	  }
	  else {
	    g.ennf (true);
	    u.setFormula(g);
	  }
          removed ++;
	}
      }
    }
  }

  return removed;
} // Problem::removeIff


// miniscope formulas
// 14/05/2002 Manchester
int Problem::miniscope ()
{
  TRACER("Problem::miniscope");

  if (! _hasFormulas ) {
    return 0;
  }

  int changed = 0;

  UnitChain::DelIterator unit ( _units ); 
  while ( unit.more() ) {
    Unit u (unit.next ());
    switch ( u.unitType() ) {
      case CLAUSE:
        break;

      case FORMULA: {
        Formula f (u.formula());
        InferenceList inf;
        f.miniscope (inf);
        if (!(u.formula() == f)) { 
	  f.rectify ();
          //cout << u.formula() << "\n" << flush;
          //cout << f << "\n\n" << flush;
          Unit v (IR_MINISCOPE, f, u);
          unit.replace (v);
          changed ++;
        }
      }
    }
  }

  return changed;
} // Problem::miniscope


// flatten each formula
// 27/06/2002 Manchester
// 30/08/2002 Torrevieja, changed
void Problem::flatten ()
{
  TRACER ( "Problem::flatten" );

  if ( ! _hasFormulas ) {
    return;
  }

  UnitChain::DelIterator unit ( _units ); 
  while ( unit.more() ) {
    Unit u (unit.next());
    switch ( u.unitType() ) {
      case CLAUSE:
        break;

      case FORMULA: {
        Formula f (u.formula());
        f.flatten ();
	if (options->proof() && ! (f == u.formula())) {
	  Unit v (IR_FLATTEN, f, u);
	  unit.replace (v);
	}
	else {
	  u.setFormula(f);
	}
      }
    }
  }
} // flatten


// rectify each formula
// 27/06/2002 Manchester
// 30/08/2002 Torrevieja, changed
void Problem::rectify ()
{
  TRACER ( "Problem::rectify" );
  if ( ! _hasFormulas ) {
    return;
  }

  UnitChain::DelIterator unit ( _units ); 
  while ( unit.more() ) {
    Unit u (unit.next ());
    switch ( u.unitType() ) {
      case CLAUSE:
        break;

      case FORMULA: {
        Formula f (u.formula());
	if (u.inputType() == CONJECTURE) {
	  // answer variables must be collected
	  TermList answerAtomArgs;
	  f.rectify (_originalAnswerVariables, answerAtomArgs);
	  if (_originalAnswerVariables.isNonEmpty()) {
	    // create answer atom
	    Atom answer;
	    sig->createAnswerAtom (answerAtomArgs, answer);
	    _answerLiteral = Literal (true, answer);
	  }
	}
	else {
	  f.rectify();
	}
	if (options->proof() && // proof output on
	    (u.formula().universalPrefixLength() != // f is NOT obtained
	     f.universalPrefixLength())) {           // by a simple renaming
	  Unit v (IR_RECTIFY, f, u);
	  unit.replace (v);
	}
	else {
	  u.setFormula (f);
	}
      }
    }
  }
} // rectify


// skolemize each formula
// 28/06/2002 Manchester
void Problem::skolemize ()
{
  TRACER ( "Problem::skolemize" );
  if ( ! _hasFormulas ) {
    return;
  }

  UnitChain::DelIterator unit ( _units ); 
  while ( unit.more() ) {
    Unit u (unit.next());
    switch ( u.unitType() ) {
      case CLAUSE:
        break;

      case FORMULA: {
        Formula f (u.formula());
        f.skolemizeNNF ();
	if (! (f == u.formula())) {
	  if (options->proof()) {
	    Unit v (IR_SKOLEMIZE, f, u);
	    unit.replace (v);
	  }
	  else {
	    u.setFormula(f);
	  }
	}
      }
    }
  }
} // skolemize


// transform every formula into CNF and replace it by the resulting set of clauses
// 04/06/2001
// 11/05/2002 Manchester, changed completely
// 28/06/2002 Manchester, changed completely again
// 30/08/2002 Torrevieja, changed
void Problem::clausify ()
{
  if (! _hasFormulas) {
    return;
  }

  UnitChain::DelIterator us (_units);
  while ( us.more() ) {
    Unit u (us.next());
    if ( u.unitType() == FORMULA ) {
      FormulaList fs (u.formula());
      LiteralList answers;
      if (u.inputType() == CONJECTURE && _answerLiteral.exists ()) {
	answers.push (_answerLiteral);
      }
      clausify (fs, answers, u);
      us.del ();
    }
  }
} // Problem::clausify


// clausify ([F1,...,Fn] [L1,...,Lm], p) - clausify the nnf formula
//    F1 \/ ... \/ Fn \/ L1 \/ ... \/ Lm
// 28/06/2002 Manchester
void Problem::clausify (FormulaList formulas, 
			LiteralList literals,
			Unit parent)
{
  if (formulas.isEmpty()) {
    Clause c (literals);
    _units.addFirst (Unit (IR_CLAUSIFY, c, parent));
    return;
  }

  // formulas is non-empty
  Formula f (formulas.head ());
  FormulaList fs (formulas.tail ());

  switch ( f.connective() ) {
  case Formula::ATOM: {
    literals.push (Literal(true, f.atom()));
    clausify (fs, literals, parent);
    return;
  }

  case Formula::NOT: {
    literals.push (Literal(false, f.uarg().atom()));
    clausify (fs, literals, parent);
    return;
  }

  case Formula::OR: {
    FormulaList newFs (f.args());
    newFs.append (fs);
    clausify (newFs, literals, parent);
    return;
  }

  case Formula::AND: {
    Iterator<Formula> args (f.args());
    while (args.more()) {
      FormulaList newFs (args.next(), fs);
      clausify (newFs, literals, parent);
    }
    return;
  }

  case Formula::IMP:
  case Formula::IFF:
  case Formula::XOR:
  case Formula::FORALL:
  case Formula::EXISTS:
  default:
    ASS(false);
  }
} // Problem::clausify


// remove all conjectures from the problem
// 02/08/2002 Torrevieja
void Problem::removeAllConjectures ()
{
  TRACER ( "Problem::removeAllConjectures" );

  UnitChain::DelIterator us (units());
  while ( us.more() ) {
    switch (us.next().inputType()) {
    case CONJECTURE:
      us.del ();
      break;

    default:
      break;
    }
  }

  _originalAnswerVariables.makeEmpty();
  Literal empty;
  _answerLiteral = empty; 
} // Problem::removeAllConjectures


// add unit to the problem
// 02/08/2002 Torrevieja
void Problem::addUnit (Unit u)
{
  TRACER ( "Problem::addUnit" );

  _units.addLast (u);
} // Problem::addUnit

