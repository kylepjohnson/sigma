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
//  file clause.cpp 
//  implements class Clause
//


#include "Clause.hpp"
#include "Sort.hpp"
#include "Renaming.hpp"


// build clause from the kernel clause
// 29/09/2002 Manchester
Clause::Clause (const VampireKernel::Clause* clause, 
		const VampireKernel& kernel)
  : _data (new Data (LiteralList (clause->literals(), kernel)))
{
} // Clause::Clause (const VampireKernel::Clause* clause)


// assignment operator
// 11/09/2002 Manchester
void Clause::operator = (const Clause& t)
{
  if (t._data) {
    t._data->ref ();
  }

  if (_data) {
    _data->deref ();
  }

  _data = t._data;
} // Clause::operator =


// clause is a definition of a function 
// the result is a lhs of the definition, ie. f(x1,...,xn)
// r is set to the rhs
// 10/05/2002 Manchester, changed 
// 11/09/2002 Manchester, changed
bool Clause::isFunctionDefinition (Term& lhs, Term& rhs) const
{
  if ( isEmpty() ) // empty clause
    return 0;
  if ( literals().tail().isNonEmpty() ) // more than one literal
    return 0;

  // now the clause has one literal
  Literal l (literals().head());
  
  if ( l.negative() ) // negative literal
    return 0;

  return l.atom().isDefinition (lhs, rhs);
} // Clause::isFunctionDefinition


// normalize the clause by sorting its literals
// in equality literals arguments can be swapped
// 26/06/2002 Manchester
void Clause::normalize ()
{
  TRACER ( "Clause::normalize" );

  int length = literals().length();
  
  Sort <Literal> srt (length);
  Iterator<Literal> lits (literals());
  while ( lits.more() ) {
    Literal l (lits.next ());
    l.normalize ();
    srt.add ( l );
  }
  srt.sortF ();

  LiteralList sorted;
  // sort is now sorted, create the resulting sorted list of literals
  for (int i = length-1; i >=0; i--) {
    sorted.push (srt[i]);
  }

  *this = Clause (sorted);
} // Clause::normalize


// used to compare clauses. The clause literals must be normalised before
// 11/09/2002 Manchester, changed
bool Clause::isless (Clause cls2) const
{
  LiteralList lits1 (literals());
  LiteralList lits2 (cls2.literals()); 

  for (;;) {
    if ( lits1.isEmpty() ) {
      return ! lits2.isEmpty();
    }

    // lits1 is non-empty
    if ( lits2.isEmpty() ) {
      return false;
    }

    // compare the first literals
    switch ( lits1.head().compare(lits2.head()) ) {
    case LESS:  
      return true;

    case EQUAL:
      break;

    case GREATER:
      return false;
    }

    lits1.pop();
    lits2.pop();
  }
} // Clause::isless ()


/*
// clause is a linear definition of a function. A definition f(X) = t 
// is linear if each variable in X occurs in t only once
// the result is a lhs of the definition, ie. f(x1,...,xn)
// r is set to the rhs
Term* Clause::isLinearDefinition ( Term*& rhs ) const
{
  Term* lhs = isDefinition (rhs);
  if ( ! lhs ) {
    return 0;
  }

  // check that every variable of lhs occurs in rhs at most once
  Term::List::Iterator args ( lhs->args() );
  while ( args.more() ) {
    if ( rhs->countOccurrences (args.next()->var()) > 1 ) {
      return 0;
    }
  }

  return lhs;
} // Clause::isLinearDefinition


int Clause::countOccurrences ( const PSymbol* p ) const
{
  int occ = 0;

  for ( Literal::List* ls = _literals; ls->isNonEmpty(); ls = ls->tail() ) {
    if ( ls->head()->atom()->functor() == p )
      occ ++;
  }

  return occ;
} // Clause::countOccurrences


int Clause::countOccurrences ( const FSymbol* f ) const
{
  int occ = 0;

  for ( Literal::List* ls = _literals; ls->isNonEmpty(); ls = ls->tail() ) 
    occ += ls->head()->countOccurrences (f);

  return occ;
} // Clause::countOccurrences


bool Clause::occurs ( const FSymbol* f ) const
{
  for ( Literal::List* ls = _literals; ls->isNonEmpty(); ls = ls->tail() ) {
    if ( ls->head()->occurs (f) )
      return true;
  }

  return false;
} // Clause::occurs


int Clause::countOccurrences ( const Var v ) const
{
  int occ = 0;

  for ( Literal::List* ls = _literals; ls->isNonEmpty(); ls = ls->tail() ) 
    occ += ls->head()->countOccurrences (v);

  return occ;
} // Clause::countOccurrences


Var Clause::maxVar () const
{
  Var result = firstVar - 1;

  for ( Literal::List* ls = _literals; ls->isNonEmpty(); ls = ls->tail() ) {
    Var m = ls->head()->maxVar ();
    if ( m > result )
      result = m;
  }

  return result;
} // Clause::maxVar


int Clause::maxLitWeight ()
{
  ASSERT( debug_clause, _literals->isNonEmpty() );

  int max = INT_MIN;

  for ( Literal::List* ls = _literals; ls->isNonEmpty(); ls = ls->tail() ) {
    int weight = ls->head()->weight();

    if ( weight > max )
      max = weight;
  }

  return max;
} // Clause::maxLitWeight


Clause* Clause::copy ()
{
  return new Clause ( Literal::copy(literals()) );
} // Clause::copy


// 06/06/2001 Manchester, rewritten from a more complex one
bool Clause::isHorn () const
{
  bool pfound = false;

  Literal::List::Iterator lits ( _literals );
  while ( lits.more() ) {
    if ( lits.next()->positive() ) {
      if ( pfound ) { // there was a positive literal already
        return false;
      }
      pfound = true;
    }
  }

  return true;
} // Clause::isHorn


// 06/06/2001 rewritten with Iterators
bool Clause::hasEquality () const
{
  Literal::List::Iterator lits ( _literals );
  while ( lits.more() ) {
    if ( lits.next()->atom()->isEquality() ) {
      return true;
    }
  }
  return false;
} // Clause::hasEquality


// 06/06/2001 rewritten with Iterators from pureEquality
bool Clause::hasNonEquality () const
{
  Literal::List::Iterator lits ( _literals );
  while ( lits.more() ) {
    if ( ! lits.next()->atom()->isEquality() ) {
      return true;
    }
  }

  return false;
} // Clause::hasNonEquality


bool Clause::hasFunctionSymbols () const
{
  for ( Literal::List* ls = _literals; ls->isNonEmpty(); ls = ls->tail() ) {
    for ( Term::List* ts = ls->head()->atom()->args(); ts->isNonEmpty(); ts = ts->tail() ) {
      Term* t = ts->head ();
      if ( ! (t->isVar() || t->args()->isEmpty()) )
        return true;
    }
  }
  return false;
} // Clause::hasFunctionSymbols


// 06/06/2001 Manchester, rewritten using iterators
bool Clause::isPropositional () const
{
  Literal::List::Iterator lits ( _literals );
  while ( lits.more() ) {
    if ( lits.next()->atom()->args()->isNonEmpty() )
      return false;
  }

  return true;
} // Clause::isPropositional


int Clause::length () const
{
  return literals()->length ();
} // Clause::length


bool Clause::isUnit () const
{
  return literals()->isNonEmpty() &&
         literals()->tail()->isEmpty();
} // Clause::isUnit


// the clause is f(U,x,Z) = f(U,y,Z) -> x = y
// and argNo is initialized to the number of the x argument
bool Clause::isCancellationAxiom ( FSymbol*& fun, int& argNo ) const
{ 
  Literal::List* ls = literals();

  if ( ls->isEmpty() ) // empty clause
    return false;      
  if ( ls->tail()->isEmpty() ) // unit clause
    return false;
  if ( ls->tail()->tail()->isNonEmpty() ) // umore than two literals
    return false;

  // now the clause has exactly two literals
  Atom* eq;
  Atom* neq;
  if ( ls->head()->positive() ) {
    eq = ls->head()->atom();
    if ( ls->tail()->head()->positive() ) {
      return false;
    }
    neq = ls->tail()->head()->atom();
  }
  else {
    neq = ls->head()->atom();
    if ( ls->tail()->head()->negative() ) {
      return false;
    }
    eq = ls->tail()->head()->atom();
  }
  // now eq is a positive atom, neq is a negative atom

  if ( ! eq->isEquality() || ! neq->isEquality() ) 
    return false;
  // now eq is a positive equality, neq is a negative equality

  Term::List* ts = eq->args ();
  // check that both arguments to the equality are variables
  if ( ! (ts->head()->isVar() && ts->second()->isVar()) ) 
    return false;

  Var x = ts->head()->var();
  Var y = ts->second()->var();
  if ( x == y ) 
    return false;

  // x = y found 
  // check that both arguments to the negative equality are non-variables and that 
  // they have the same function symbol
  ts = neq->args ();
  Term* t1 = ts->head();
  Term* t2 = ts->second();
  if ( t1->isVar() || t2->isVar() ) 
    return false;

  // finding function symbols and checking they are equal
  if ( t1->functor() != t2->functor() )
    return false;
  fun = t1->functor();

  // now the clause is t1 = t2 -> x = y
  Term::List* ts1;
  Term::List* ts2;
  // check that all arguments are variables
  for( ts1 = t1->args (); ts1 ->isNonEmpty(); ts1 = ts1->tail() ) {
    if ( ! ts1->head()->isVar() )
      return false;
  }
  for( ts2 = t2->args (); ts2 ->isNonEmpty(); ts2 = ts2->tail() ) {
    if ( ! ts2->head()->isVar() )
      return false;
  }

  // t1 = f(x1,..,xn) and t2 = f(y1,..,yn)
  // it remains to check that the variables are OK

  ts1 = t1->args();
  ts2 = t2->args();

  int currentArgNo = 0;
  bool xyFound = false;

  for (;;) {
    if ( ts1->isEmpty() ) 
      return xyFound;
    
    // ts1 is nonempty (and so is ts2, provided that each functor has unique arity)
    Var v1 = ts1->head()->var ();
    Var v2 = ts2->head()->var ();

    if ( v1 == x ) {
      if ( xyFound || v2 != y )
        return false;
      xyFound = true;
      argNo = currentArgNo;
    }
    else if ( v1 == y ) {
      if ( xyFound || v2 != x )
        return false;
      xyFound = true;
      argNo = currentArgNo;
    }
    // v1 is neither x nor y
    else if ( v1 != v2 ) 
      return false;
    else { // v1 == v2 
      // check that they have unique occurrences in the rest of the arguments
      Term::List* ss;
      for ( ss = ts1->tail() ; ss->isNonEmpty(); ss = ss->tail() ) {
        if ( ss->head()->var() == v1 )
          return false;
      }
      for ( ss = ts2->tail() ; ss->isNonEmpty(); ss = ss->tail() ) {
        if ( ss->head()->var() == v1 )
          return false;
      }
    }

    // else do nothing
    ts1 = ts1->tail();
    ts2 = ts2->tail();
    currentArgNo ++;
  }
} // Clause::isCancellationAxiom ()


// replace the cancellation axiom 
//   f(X,y,Z) = f(X,u,Z) -> y = u
// by 
//   i(f(X,y,Z),X,Z) = y
void Clause::replaceCancellation ( int argNo,FSymbol* inv )
{
  Literal::List* yu;
  Literal::List* ff;
  if ( literals()->head()->positive() ) { // y = u is the first literal
    yu = literals ();
    ff = yu->tail();
  }
  else {
    ff = literals ();
    yu = ff->tail();
  }
  yu->head()->destroy();
  delete yu;

  ff->tail (Literal::List::empty());
  ff->head()->introduceInverse (argNo,inv);
  _literals = ff;
} // Unit::replaceCancellation


// the clause is P(Z,x,U) & P(Z,y,U) -> x = y, P is returned to pred
// and argNo is initialized to the number of the x argument
bool Clause::isUniquenessAxiom ( PSymbol*& pred, int& argNo ) const
{ 
  Literal::List* ls = literals();

  for (;;) {
    if ( ls->isEmpty() )
      return false;
    if ( ls->head()->positive() ) 
      break;
    ls = ls->tail ();
  }

  // now the literal is positive
  Atom* atom = ls->head()->atom();
  // check that it is also an equality
  if ( ! atom->isEquality() ) 
    return false;

  // atom is equality
  Term::List* ts = atom->args ();
  // check that both arguments to the equality are variables
  if ( ! (ts->head()->isVar() && ts->second()->isVar()) ) 
    return false;

  Var x = ts->head()->var();
  Var y = ts->second()->var();
  if ( x == y ) 
    return false;

  // x = y found 
  // check that the clause has only two remaining literals, both negative
  ls = literals();
  // if the first literal is x = y, skip it
  if ( ls->head()->atom() == atom ) 
    ls = ls->tail ();
  if ( ls->isEmpty() || ls->head()->positive() ) // no next literal or positive literal
    return false;
  // now head of ls is a negative literal, remember it
  Atom* atom1 = ls->head()->atom();

  // proceed to the next literal
  ls = ls->tail();
  if ( ls->isEmpty() ) // no next literal
    return false;

  // if the first literal is x = y, skip it
  if ( ls->head()->atom() == atom ) {
    ls = ls->tail ();
    if ( ls->isEmpty() ) // no next literal
      return false;
  }

  if ( ls->head()->positive() ) 
    return false;

  // now head of ls is a negative literal, remember it
  Atom* atom2 = ls->head()->atom();

  // finding predicates and checking they are equal
  if ( atom1->functor() != atom2->functor() )
    return false;
  pred = atom1->functor();

  ls = ls->tail ();
  if ( ls->isNonEmpty() ) {
    if ( ls->head()->atom() != atom ) // the clause has one more literal
      return false;
    // the head of ls is x = y
    if ( ls->tail()->isNonEmpty() ) 
      return false;
  }

  // now the clause has exactly 3 literals, ~atom1, ~atom2 and x = y
  Term::List* ts1;
  Term::List* ts2;
  // check that all arguments are variables
  for( ts1 = atom1->args (); ts1 ->isNonEmpty(); ts1 = ts1->tail() ) {
    if ( ! ts1->head()->isVar() )
      return false;
  }
  for( ts2 = atom2->args (); ts2 ->isNonEmpty(); ts2 = ts2->tail() ) {
    if ( ! ts2->head()->isVar() )
      return false;
  }

  // atom1 = P(x1,..,xn) and atom2 = P(y1,..,yn)
  // it remains to check that the variables are OK

  ts1 = atom1->args();
  ts2 = atom2->args();

  int currentArgNo = 0;
  bool xyFound = false;

  for (;;) {
    if ( ts1->isEmpty() ) 
      return xyFound;
    
    // ts1 is nonempty (and so is ts2, provided that each functor has unique arity)
    Var v1 = ts1->head()->var ();
    Var v2 = ts2->head()->var ();

    if ( v1 == x ) {
      if ( xyFound || v2 != y )
        return false;
      xyFound = true;
      argNo = currentArgNo;
    }
    else if ( v1 == y ) {
      if ( xyFound || v2 != x )
        return false;
      xyFound = true;
      argNo = currentArgNo;
    }
    // v1 is neither x nor y
    else if ( v1 != v2 ) 
      return false;
    else { // v1 == v2 
      // check that they have unique occurrences in the rest of the arguments
      Term::List* ss;
      for ( ss = ts1->tail() ; ss->isNonEmpty(); ss = ss->tail() ) {
        if ( ss->head()->var() == v1 )
          return false;
      }
      for ( ss = ts2->tail() ; ss->isNonEmpty(); ss = ss->tail() ) {
        if ( ss->head()->var() == v1 )
          return false;
      }
    }

    // else do nothing
    ts1 = ts1->tail();
    ts2 = ts2->tail();
    currentArgNo ++;
  }
} // Clause::isUniquenessAxiom

// the clause if P(xi1,...,xin,f(x1,...,xn))
// where argNo is the number of the f(...) argument
// the result is the array [i1,...,in]
int* Clause::isFunctionalityAxiom ( const PSymbol* pred, FSymbol*& fun, int argNo, Atom*& atm ) const
{
  if ( literals()->isEmpty() || 
       literals()->tail()->isNonEmpty() || 
       literals()->head()->negative() )
    return 0;

  // the clause is a positive unit
  Atom* atom = literals()->head()->atom();
  if ( atom->functor() != pred )
    return 0;

  int len = atom->args()->length ();
  
  ASSERT( debug_problem, len <= 100 );
  Var pargs [100];
  Var targs [99];

  // put all arguments to atom in pargs
  int currentArg = 0;
  Term* t = 0; // to store argNo, initializing is unimportant
  Term::List* ts;
  for ( ts = atom->args(); ts->isNonEmpty(); ts = ts->tail() ) {
    if ( currentArg != argNo ) {
      if ( ! ts->head()->isVar() )
        return 0;
      pargs [currentArg] = ts->head()->var();
    }
    else 
      t = ts->head();
    currentArg ++;
  }
  // now all args to atom except argNo are variables, and argNo's is t
  if ( t->isVar() )
    return 0;

  if ( t->args()->length() != len - 1 )
    return 0;

  // put all arguments to t in targs
  currentArg = 0;
  for ( ts = t->args(); ts->isNonEmpty(); ts = ts->tail() ) {
    if ( ! ts->head()->isVar() )
      return 0;

    targs [currentArg] = ts->head()->var();
    // check that this argument is unique
    for ( int i = currentArg - 1; i >= 0; i -- ) {
      if (targs[i] == targs[currentArg])
        return 0;
    }
    currentArg ++;
  }

  // check that all arguments to t are among the arguments to atom
  // and remember their indexes

  int* result = new int [len-1];
  if (! result) {
    NO_MEMORY;
  }

  for ( int i = len - 2; i >= 0; i-- ) {
    int j;
    for ( j = len - 1; j >= 0; j-- ) {
      if ( j != argNo && targs[i] == pargs[j] ) {
        result[i] = j;
        break;
      }
    }
    if ( j < 0 ) { // variable was not found among atom's arguments
      delete [] result;
      return 0;
    }
  }

  fun = t->functor();
  atm = atom;
  return result;
} // Clause::isFunctionalityAxiom


bool Clause::replaceRelationByFunction ( PSymbol* pred, FSymbol* fun, int argNo, int* vs )
{
  bool replaced = false;

  for ( Literal::List* ls = _literals; ls->isNonEmpty(); ls = ls->tail() ) {
    Literal* l = ls->head();
    Atom* newAtom = l->atom()->replaceRelationByFunction (pred, fun, argNo, vs);
    if ( newAtom ) {
      l->atom ( newAtom );
      replaced = true;
    }
  }

  return replaced;
} // Clause::replaceRelationByFunction


// remove x != t and t != t
int Clause::removeVarEquality () 
{
  int removed = 0;

  Literal::List* ls = _literals;

  while ( ls->isNonEmpty() ) {
    Literal* l = ls->head ();
    if ( ls->head()->positive () ) {
      ls = ls->tail ();
      continue;
    }

    // next literal is negative
    Atom* t = ls->head()->atom();
    ls = ls->tail (); // since it can be deleted after

    if ( ! t->isEquality() ) 
      continue;

    Term::List* ts = t->args();
    Term* lhs = ts->head ();
    Term* rhs = ts->second ();

    if ( lhs->equal(rhs) ) {
      removed ++;
      _literals = _literals->remove ( l );
      t->destroy ();
      delete l;
      continue;
    }

    if ( lhs->isVar() ) {
      if ( rhs->occurs(lhs->var()) ) 
        continue;

      // now the literal is x != t, where x does not occur in t
      removed ++;
      _literals = _literals->remove ( l );
      delete l;
      for ( Literal::List* ms = _literals; ms->isNonEmpty(); ms = ms->tail() ) {
        ms->head()->atom()->replace ( lhs->var(), rhs );
      }
      t->destroy ();
      continue;
    }
    // lhs is not var
    if ( rhs->isVar() ) {
      if ( lhs->occurs(rhs->var()) ) 
        continue;

      // now the literal is x != t, where x does not occur in t
      removed ++;
      _literals = _literals->remove ( l );
      delete l;
      for ( Literal::List* ms = _literals; ms->isNonEmpty(); ms = ms->tail() ) {
        ms->head()->atom()->replace ( rhs->var(), lhs );
      }
      t->destroy ();
    }
    // both lhs and rhs are non-variables, do nothing
  }

  return removed;
} // Clause::removeVarEquality


bool Clause::isTautology () const
{
  if ( literals()->isEmpty() )
    return false;

  return isEquationalTautology () ||
         isNonEquationalTautology ();
} // Clause::isTaulology


bool Clause::isEquationalTautology () const
{
  ASSERT( debug_clause, literals()->isNonEmpty() );

  if ( literals()->tail()->isNonEmpty() ||
       literals()->head()->negative() ) 
    return false;

  // clause is unit and positive
  Atom* atom = literals()->head()->atom();
  if ( ! atom->isEquality () )
    return false;

  Term::List* args = atom->args ();
  return args->head()->equal ( args->second() );
} // Clause::isTaulology


bool Clause::isNonEquationalTautology () const
{
  ASSERT( debug_clause, literals()->isNonEmpty() );

  Literal::List* ls = _literals; 
  for (;;) {
    Literal::List* ms = ls->tail();
    if ( ms->isEmpty () )
      return false;
    do {
      if ( ls->head()->complementary ( ms->head() ) ) 
        return true;

      ms = ms->tail ();
    }
    while ( ms->isNonEmpty() );

    ls = ls->tail();
  }
} // Clause::isTaulology


bool Clause::becomesUnitEq ( PSymbol*& p, bool& sign )
{
  if ( literals()->length() != 2 )
    return false;

  Literal* lit1 = literals()->head();
  Literal* lit2 = literals()->second();

  if ( lit1->atom()->isEquality() ) {
    if ( (! lit2->atom()->isEquality() ) && lit2->atom()->isLinearAndFlat() ) {
      p = lit2->atom()->functor();
      sign = lit2->sign();
      return true;
    }
  }
  // lit1 is not equality
  else if ( lit2->atom()->isEquality() ) {
    if ( lit1->atom()->isLinearAndFlat() ) {
      p = lit1->atom()->functor();
      sign = lit1->sign();
      return true;
    }
    return false;
  }

  return false;
} // Clause::becomesUnitEq


// return true if 
//  (1) clause contains at most one occurrence of pred
//  (2) if this occurrence is of polarity sign, the literal must be linear and flat
bool Clause::canRemovePredicateDefinition ( PSymbol* pred, bool sign )
{
  bool found = false; // pred-literal found

  for ( Literal::List* ls = _literals; ls->isNonEmpty(); ls = ls->tail() ) {
    if ( ls->head()->atom()->functor() == pred ) {
      if ( found ) // two pred-literals
        return false;
      
      if ( ls->head()->sign() == sign && 
           ! ls->head()->atom()->isLinearAndFlat() )
        return false;

      found = true;
    }
  }

  return true;
} // Clause::canRemovePredicateDefinition


// true if predicate pred occurs, 
// sign = polarity of pred's first occurrence
bool Clause::occurs ( const PSymbol* pred, bool& sign ) const
{
  for ( Literal::List* ls = _literals; ls->isNonEmpty(); ls = ls->tail() ) {
    if ( ls->head()->atom()->functor() == pred ) {
      sign = ls->head()->sign();
      return true;
    }
  }

  return false;
} // Clause::occurs


Clause* Clause::resolveOn ( const PSymbol* pred, Clause* cl )
{
  // finding the pred-literal in this
  Literal::List* ls;
  Literal::List* ms;

  for ( ls = literals(); ls->head()->atom()->functor() != pred; ls = ls->tail() )
    ;

  // finding the pred-literal in cl
  for ( ms = cl->literals(); ms->head()->atom()->functor() != pred; ms = ms->tail() )
    ;

  Subst subst; // will be use to match ls against ms

  Term::List* ts1 = ls->head()->atom()->args();
  Term::List* ts2 = ms->head()->atom()->args();

  while ( ts1->isNonEmpty() ) {
    ASSERT( debug_clause, ts1->head()->isVar() );
    ASSERT( debug_clause, ts2->isNonEmpty() );

    subst.bind ( ts1->head()->var(), ts2->head() );
    ts1 = ts1->tail ();
    ts2 = ts2->tail ();
  }

  int lastVar = cl->maxVar();

  VarSubst vsubst1;

  Literal::List* ns;
  Literal::List* lits = Literal::List::empty ();

  // collect into lits all remaining literals of the clauses
  for ( ns = literals(); ns->isNonEmpty(); ns = ns->tail() ) {
    if ( ns != ls ) 
      // push the head of ns into lits with the substitution
      Literal::List::push ( ns->head()->apply(subst,vsubst1,lastVar),lits);
  }
  for ( ns = cl->literals(); ns->isNonEmpty(); ns = ns->tail() ) {
    if ( ns != ms ) 
      // push the head of ns into lits 
      Literal::List::push ( ns->head()->copy(),lits);
  }

  return new Clause ( lits );
} // Clause::resolveOn


int Clause::weight () const
{
  int wht = 0;
  Literal::List::Iterator ls ( _literals );
  while ( ls.more() ) {
    wht += ls.next()->weight();
  }

  return wht;
} // Clause::weight


Comparison Clause::compare ( const Clause* c ) const
{
  // check that the clauses are normalized
  ASSERT( debug_clause, _weight != -1 && c->_weight != -1 );

  switch ( Int::compare (_weight, c->_weight) ) {
    case LESS:
      return LESS;
    case EQUAL:
      break;
    case GREATER:
      return GREATER;
  }

  switch ( Int::compare (length(), c->length()) ) {
    case LESS:
      return LESS;
    case EQUAL:
      break;
    case GREATER:
      return GREATER;
  }

  // compare literals lexicographically
  Literal::List::Iterator ls1 ( _literals );
  Literal::List::Iterator ls2 ( c->_literals );
  while ( ls1.more() ) {
    // ls2 must be nonempty, otherwise comparison would fail before
    ASSERT( debug_clause, ls2.more() );

    switch ( ls1.next()->compare(ls2.next()) ) {
      case LESS:
        return LESS;
      case EQUAL:
        break;
      case GREATER:
        return GREATER;
    }
  }

  return EQUAL;
} // Clause::compare


// true if predicate pred occurs, 
bool Clause::occurs ( const PSymbol* pred ) const
{
  Literal::List::Iterator ls ( _literals );
  while ( ls.more() ) {
    if ( ls.next()->atom()->functor() == pred ) {
      return true;
    }
  }

  return false;
} // Clause::occurs


void Clause::splitLargeArity (const PSymbol* p, FSymbol::List* newFs )
{
  Literal::List::Iterator ls ( _literals );
  while ( ls.more() ) {
    ls.next()->splitLargeArity (p, newFs);
  }
} // Clause::splitLargeArity


void Clause::splitLargeArity (const FSymbol* f, FSymbol::List* newFs )
{
  Literal::List::Iterator ls ( _literals );
  while ( ls.more() ) {
    ls.next()->splitLargeArity (f, newFs);
  }
} // Clause::splitLargeArity


// no time to tab the split now
Clause::List* Clause::negativeEqualitySplit ( int thresholdWeight )
{
  List* result = Clause::List::empty ();

  Literal::List::Iterator ls ( _literals );
  while ( ls.more() ) {
    Literal* l = ls.next()->negativeEqualitySplit (thresholdWeight);
    if (l) {
      Clause::List::push (new Clause (new Literal::List (l)), result);
    }
  }

  return result;
} // Clause::negativeEqualitySplit


// 04/06/2001
void Clause::polarities ( const PSymbol* p, int& pol ) const
{
  Literal::List::Iterator ls ( _literals );
  while ( ls.more() ) {
    Literal* l = ls.next();
    if ( l->atom()->functor() == p ) {
      if ( l->positive() ) {
        pol |= 1;
      }
      else { // negative occurrence
        pol |= 2;
      }
      // check if both polarities have been found, to avoid extra work
      if ( pol == 3 )
        return;
    }
  }
} // Clause::polarities
*/


// true if the clause is one of the equality axioms
// 29/04/2002, Manchester, copied from Formula
bool Clause::isEqualityAxiom () const
{
  TRACER("Clause::isEqualityAxiom");

  return isReflexivityAxiom () || 
         isSymmetryAxiom () ||
         isTransitivityAxiom () ||
         isFunctionReflexivityAxiom () ||
         isPredicateReflexivityAxiom ();
} // Clause::isEqualityAxiom


// true if the clause is t=t
// 29/04/2002, Manchester, copied from Formula
// 10/09/2002 Manchester, changed
bool Clause::isReflexivityAxiom () const
{
  TRACER("Clause::isReflexivityAxiom");
  LiteralList ls (literals());
  if ( ls.isEmpty() || ls.tail().isNonEmpty()) { // length is not one
    return false;
  }
  Literal l (ls.head());
  return l.positive() && 
         l.atom().isTautology();
} // Clause::isReflexivityAxiom


// true if the clause is s!=t \/ t=s
// 29/04/2002, Manchester
// 10/09/2002 Manchester, changed
bool Clause::isSymmetryAxiom () const
{
  TRACER("Clause::isSymmetryAxiom");

  LiteralList ls (literals());
  if ( ls.isEmpty()) { 
    return false;
  }
  Literal l1 (ls.head());
  ls = ls.tail ();
  if ( ls.isEmpty()) { 
    return false;
  }
  Literal l2 (ls.head());
  ls = ls.tail ();
  if ( ls.isNonEmpty()) { 
    return false;
  }
  if (l1.sign() == l2.sign()) {
    return false;
  }
  return l1.atom().swap (l2.atom());
} // Clause::isSymmetryAxiom


// true if the clause is s!=t \/ r!=s \/ r=t
// 29/04/2002, Manchester
// 10/09/2002 Manchester, changed
bool Clause::isTransitivityAxiom () const
{
  TRACER("Clause::isTransitivityAxiom");

  LiteralList ls (literals());
  if ( ls.isEmpty()) { 
    return false;
  }
  Literal l1 (ls.head());
  ls = ls.tail ();
  if ( ls.isEmpty()) { 
    return false;
  }
  Literal l2 (ls.head());
  ls = ls.tail ();
  if ( ls.isEmpty()) { 
    return false;
  }
  Literal l3 (ls.head());
  ls = ls.tail();
  if ( ls.isNonEmpty()) { 
    return false;
  }
  Atom a1;
  Atom a2;
  Atom a3;
  if (l1.positive()) {
    if (l2.positive() || l3.positive()) {
      return false;
    }
    a3 = l1.atom ();
    a2 = l2.atom ();
    a1 = l3.atom ();
  }
  // l1 is negative
  else if (l2.positive()) {
    if (l3.positive()) {
      return false;
    }
    a3 = l2.atom ();
    a2 = l1.atom ();
    a1 = l3.atom ();
  }
  // l1,l2 are negative
  else if (l3.positive()) {
    a1 = l1.atom ();
    a2 = l2.atom ();
    a3 = l3.atom ();
  }
  // all three are negative
  else {
    return false;
  }

  return Atom::transitivity (a1, a2, a3);
} // Clause::isTransitivityAxiom


// true if the clause is x!=y \/ s=t, where x=y |- s=t
// 29/04/2002, Manchester
// 10/09/2002 Manchester, changed
bool Clause::isFunctionReflexivityAxiom () const
{
  TRACER("Clause::isFunctionReflexivityAxiom");

  LiteralList ls (literals());
  if ( ls.isEmpty()) { 
    return false;
  }
  Literal l1 (ls.head());
  ls = ls.tail ();
  if ( ls.isEmpty()) { 
    return false;
  }
  Literal l2 (ls.head());
  ls = ls.tail ();
  if ( ls.isNonEmpty()) { 
    return false;
  }
  if (l1.sign() == l2.sign()) {
    return false;
  }
  Atom a1;
  Atom a2;
  if (l1.positive()) {
    a1 = l2.atom ();
    a2 = l1.atom ();
  }
  else { // l1 is negative, hence l2 positive
    a1 = l1.atom ();
    a2 = l2.atom ();
  }
  return Atom::functionMonotonicity (a1,a2);
} // Clause::isFunctionReflexivityAxiom


// true if the clause is ~s=t \/ ~L[s] \/ L[t], where L is not an equality literal
// 29/04/2002, Manchester
// 10/09/2002 Manchester, changed
bool Clause::isPredicateReflexivityAxiom () const
{
  TRACER("Clause::isPredicateReflexivityAxiom");

  LiteralList ls (literals());
  if ( ls.isEmpty()) { 
    return false;
  }
  Literal l1 (ls.head());
  ls = ls.tail ();
  if ( ls.isEmpty()) { 
    return false;
  }
  Literal l2 (ls.head());
  ls = ls.tail ();
  if ( ls.isEmpty()) { 
    return false;
  }
  Literal l3 (ls.head());
  ls = ls.tail ();
  if ( ls.isNonEmpty()) { 
    return false;
  }
  Atom a1;
  Atom a2;
  Atom a3;
  if (l1.positive()) {
    if (l2.positive() || l3.positive()) {
      return false;
    }
    a3 = l1.atom ();
    a2 = l2.atom ();
    a1 = l3.atom ();
  }
  // l1 is negative
  else if (l2.positive()) {
    if (l3.positive()) {
      return false;
    }
    a3 = l2.atom ();
    a2 = l1.atom ();
    a1 = l3.atom ();
  }
  // l1,l2 are negative
  else if (l3.positive()) {
    a1 = l1.atom ();
    a2 = l2.atom ();
    a3 = l3.atom ();
  }
  // all three are negative
  else {
    return false;
  }

  return Atom::predicateMonotonicity (a1,a2,a3) ||
         Atom::predicateMonotonicity (a2,a1,a3);
} // Clause::isPredicateReflexivityAxiom


