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
//  file Signature.cpp
//  implements class Signature consisting of predicate and function symbols
//


#include <string.h>
#include <stdio.h>


#include "Signature.hpp"
#include "SymCounter.hpp"


Signature* sig = 0;


Signature::Signature () :
  _noOfPreds (0),
  _noOfFuns (0),
  _lastSkolem (-1),
  _answer (0)
{
  TRACER("Signature::Signature");

  // initialize all buckets to empty
  for ( int i = NO_OF_BUCKETS - 1; i >= 0; i-- ) {
    _fbuckets [i] = Signature::List::empty ();
    _pbuckets [i] = Signature::List::empty ();
  }

  // initialize equality
  _equality = createPred ( "equal", 2 );
} // Signature::Signature


// true if every symbol has only one arity and that no symbol 
// is both a function and a predicate symbol
// 04/05/2002
bool Signature::arityCheck () const 
{
  // iterate over function symbol buckets first
  for ( int i = NO_OF_BUCKETS - 1; i >= 0; i-- ) {
    // signals that cur currently ranges over the function bucket
    bool curIsFun = true;
    // iterate over function and predicate symbols in bucket i
    List* cur = _fbuckets [i];

    for (;;) { // this loop iterates cur
      if ( cur->isEmpty() ) {
        if (curIsFun) {
          cur = _pbuckets [i];
          if ( cur->isEmpty() ) {
            break; // go to the next bucket
          }
          curIsFun = false;
        }
        else {  // curIsFun is false and cur is empty, no more possibilities for cur
          break;
        }
      }
      // now cur is non-empty
      Symbol* curSym = cur->head ();

      List* rest = cur;
      bool restIsFun = curIsFun;

      for (;;) { // this loop iterates rest
        rest = rest->tail ();
        if ( rest->isEmpty() ) { 
          if (! restIsFun) { // no more possibilities for rest
            break;
          }
          // curIsFun is true, try to check pbuckets
          rest = _pbuckets [i];
          if ( rest->isEmpty() ) { // no more possibilities for rest
            break; 
          }
          restIsFun = false;
        }
        // now both cur and rest are set, check that the arity condition is 
        // not violated
        if (! strcmp (curSym->name(), rest->head()->name()) ) { // same names
          if (curIsFun) {
            if (restIsFun) { // both are functions
              cerr << "Function symbol " << curSym->name() 
                   << " is used with two different arities "
                   << curSym->arity() << " and " << rest->head()->arity() << "\n";
            }
            else {  // cur is a function, while rest is a predicate
              cerr << "Symbol " << curSym->name() 
                   << " is used both as a function symbol and a predicate symbol\n";
            }
            return false;
          }
          else { // cur is a predicate, and hence both are predicates
            cerr << "Predicate symbol " << curSym->name() 
                 << " is used with two different arities "
                 << curSym->arity() << " and " << rest->head()->arity() << "\n";
            return false;
          }
        }
      }
      cur = cur->tail ();
    }
  }

    // no more possibilities for true the arity condition is satisfied
  return true;
} // Signature::arityCheck


Signature::~Signature () 
{
  TRACER("Signature::~Signature");

  // destroy all buckets 
  for ( int i = NO_OF_BUCKETS - 1; i >= 0; i-- ) {
    Signature::List::Iterator fit (_fbuckets [i]);
    while (fit.more()) {
      delete fit.next();
    }
    _fbuckets [i]->destroy ();
    Signature::List::Iterator pit (_pbuckets [i]);
    while (pit.more()) {
      delete pit.next();
    }
    _pbuckets [i]->destroy ();
  }
} // Signature::~Signature


Signature::Symbol* Signature::create ( const char* str, 
				       int arity, 
				       List** buckets, 
				       bool isPred )
{
  TRACER("Signature::create");

  int bucket = hash ( str );
  for ( List* l = buckets [ bucket ]; ! l->isEmpty (); l = l->tail() ) {
    Symbol* sym = l->head();
    if ( arity == sym->arity() &&         // same arity and
         ! strcmp (str, sym->name()) ) {  // same name
      return l->head ();
    }
  }

  char* copy = new char [strlen (str) + 1];
  if (! copy) {
    NO_MEMORY;
  }

  strcpy ( copy, str );

  Symbol* newf; 
  if (isPred) {
    newf = new Pred (copy,arity,_noOfPreds);
  }
  else {
    newf = new Fun (copy,arity,_noOfFuns);
  }
  Signature::List::push ( newf, buckets [bucket] );

  return newf;
} // Signature::Symbol::create


// very simple 
int Signature::hash ( const char* str )
{
  TRACER("Signature::hash");

  int result = 0;

  for ( const char* s = str ; *s != 0; s++ ) 
    result = ( result * 10 + *s ) % NO_OF_BUCKETS;
  
  return result;
} // Signature::hash


// 01/05/2002, Manchester
ostream& operator << ( ostream& str, const Signature::Symbol* p )
{
  str << p->name ();

  return str;
} // operator << ( ostream& str, const Signature::Symbol* p )


/*
// return a new array sig of all symbols such that sig[i]
// is the symbol with the number i
PSymbol** PSymbol::signature () 
{
  PSymbol** sig = new PSymbol* [_noOfSymbols];
  if (! sig) {
    NO_MEMORY;
  }

  for ( int i = _noOfBuckets - 1; i >= 0; i-- ) {
    PSymbol::List::Iterator ps ( _buckets [i] );
    while ( ps.more() ) {
      PSymbol* p = ps.next ();
      sig [p->number()] = p;
    }
  }

  return sig;
} // PSymbol::signature


// symbol with the number n
PSymbol* PSymbol::nth (int n) 
{
  for ( int i = _noOfBuckets - 1; i >= 0; i-- ) {
    PSymbol::List::Iterator ps ( _buckets [i] );
    while ( ps.more() ) {
      PSymbol* p = ps.next ();
      if ( p->number() == n )
        return p;
    }
  }
  ASS( debug_psymbol, false );

  return 0;
} // PSymbol::nth


// make a new function symbol starting with prefix
PSymbol* PSymbol::createNew ( const char* prefix, int arity ) 
{
  int len = strlen (prefix);
  char* name = new char [len + 10]; // assume 10^9 is enough
  if (! name) {
    NO_MEMORY;
  }
  strcpy (name, prefix);
  char* postfix = name + len; // postfix, part of name coming after prefix

  int post = 0; // number used for postfix

  loop:
  // write post as the postfix
  Int::toString (post++, postfix);
  // searching for the name
  int bucket = hash ( name );

  for ( List* l = _buckets [ bucket ]; ! l->isEmpty (); l = l->tail() ) {
    if ( ! strcmp (name, l->head()->name() ) ) { // found
      goto loop;
    }
  }
  // not found, insert it
  char* copy = new char [strlen (name) + 1];
  if (! copy) {
    NO_MEMORY;
  }

  strcpy ( copy, name );
  delete [] name;

  PSymbol* newf = new PSymbol (copy,arity);
  PSymbol::List::push ( newf, _buckets [bucket] );

  return newf;
} // FSymbol::createNew



// 08/06/2001
void PSymbol::arities ( int max, int* ars )
{
  PSymbol** sig = signature ();

  for ( int i = _noOfSymbols - 1; i >= 0; i-- ) {
    PSymbol* f = sig[i];
    ars [Int::min(max,f->arity ())] += 1;
  }

  delete [] sig;
} // FSymbol::arities
*/


// 01/05/2002, Manchester
Signature::Iterator::Iterator (List** buckets)
  :
  _firstBucket (buckets),
  _currentBucket (buckets + NO_OF_BUCKETS - 1),
  _currentList (*_currentBucket)
{
  TRACER("Signature::Iterator::Iterator");
} // Signature::Iterator::Iterator
    

// 01/05/2002, Manchester
bool Signature::Iterator::more ()
{
  TRACER("Signature::Iterator::more");

  for (;;) {
    if ( _currentList->isNonEmpty() ) {
      return true;
    }
    // _currentList is empty
    if (_currentBucket == _firstBucket) {
      return false;
    }
    // current bucket is not first
    _currentBucket--;
    _currentList = *_currentBucket;
  }
} // Signature::Iterator::more
    

// 01/05/2002, Manchester
Signature::Symbol* Signature::Iterator::next ()
{
  TRACER("Signature::Iterator::next");
  ASS( _currentList->isNonEmpty() );

  Signature::Symbol* result = _currentList->head ();
  _currentList = _currentList->tail ();

  return result;
} // Signature::Iterator::next
    

// comparison of functors, needed to normalize
// they are compared first by arity and then by the number of occurrences
// 26/06/2002 Manchester
Compare Signature::Fun::compare (const Fun* f) const 
{
  switch ( Int::compare (arity(), f->arity()) ) {
    case LESS:
      return LESS;

    case GREATER:
      return GREATER;

    case EQUAL:
      return Int::compare (SymCounter::norm->get (this)->occ (),
                           SymCounter::norm->get (f)->occ () );
    #if DEBUG_PREPRO
    default:
      ASS(false);
    #endif
  }
} // Signature::Fun::compare


// comparison of functors, needed to normalize
// they are compared first by arity and then by the number of occurrences
// 26/06/2002 Manchester
Compare Signature::Pred::compare (const Pred* p) const 
{
  Compare comp = Int::compare (arity(), p->arity());
  if (comp != EQUAL) {
    return comp;
  }

  // arities are equal, use symbol counter
  SymCounter::Pred* p1 = SymCounter::norm->get (this);
  SymCounter::Pred* p2 = SymCounter::norm->get (p);

  comp = Int::compare (p1->pocc (), p2->pocc());
  if (comp != EQUAL) {
    return comp;
  }

  comp = Int::compare (p1->nocc (), p2->nocc());
  if (comp != EQUAL) {
    return comp;
  }

  return Int::compare (p1->docc (), p2->docc());
} // Signature::Pred::compare


// new skolem function
// 28/06/2002 Manchester
Signature::Fun* Signature::newSkolemFunction (int arity)
{
  char name [12] = {'s', 'k'}; // to output the name
  do {
    sprintf (name+2, "%d", ++_lastSkolem);
  }
  while (existsFun (name));

  // now the name does not exist
  return createFun (name, arity);
} // Signature::newSkolemFunction


// true if a function with the name name (and any arity)
// exists in the signature
// 28/06/2002 Manchester
bool Signature::existsFun (const char* name) const
{
  TRACER("Signature::existsFun");

  int bucket = hash (name);
  for ( List* l = _fbuckets [ bucket ]; ! l->isEmpty (); l = l->tail() ) {
    Symbol* sym = l->head();
    if ( ! strcmp (name, sym->name()) ) {  // same name
      return true;
    }
  }

  return false;
} // Signature::existsFun 


// add answer predicate to the signature
// 03/08/2002 Torrevieja
void Signature::createAnswerAtom (TermList ts, Atom& answer)
{
  _answer = createPred ( "$answer", ts.length ());
  answer = Atom (_answer, ts);
} // Signature::createAnswerAtom


// add arithmetic predicates to the signature
// 03/08/2002 Torrevieja
void Signature::addArithmetic ()
{
  createPred ( ">", 2);
  createPred ( "<", 2);
  createPred ( ">=", 2);
  createPred ( "<=", 2);

  createFun ( "+", 2);
  createFun ( "-", 2);
  createFun ( "-", 1);
  createFun ( "*", 2);
} // Signature::addArithmetic


