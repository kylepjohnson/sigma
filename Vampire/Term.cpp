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
//  file Term.cpp
//  implements class Term
//

#include <string>

#include "Term.hpp"
#include "Sort.hpp"
#include "Substitution.hpp"


const char* Term::varPrefix = "X";
// const TermList::TermList _empty;


// build term from a kernel term
// 28/09/2002 Manchester
Term::Term (const VampireKernel::Subterm* term, const VampireKernel& kernel)
{
  
  if (term->isVariable()) {
    _data = new VarData (term->variableNumber());
    return;
  }

  // non-variable
  ulong symbol = term->topFunction();
  if (kernel.isNumericConstantId(symbol)) {
    _data = new NumericData (kernel.numericConstantValue(symbol));
    return;
  }

  const char* name = kernel.symbolPrintName (symbol);
  int arity = kernel.symbolArity (symbol);

  _data = new CompoundData (sig->createFun (name, arity),
			    TermList (term->firstArgument(), kernel));
} // Term::Term (const VampireKernel::Subterm* term)


// build term list from a kernel term
// 28/09/2002 Manchester
TermList::TermList (const VampireKernel::Subterm* terms, 
		    const VampireKernel& kernel)
{
  _data = (terms ?
	   new LstData<Term> (Term(terms, kernel),
		     TermList(terms->next(), kernel)) :
	   0);
} // TermList::TermList (const VampireKernel::Subterm* term)


// assignment operator
// 25/08/2002 Torrevieja
void Term::operator = (const Term& t)
{
  if (t._data) {
    t._data->ref ();
  }

  if (_data) {
    _data->deref ();
  }

  _data = t._data;
} // Term::operator=


// 28/09/2002 Manchester, changed to use numbers
inline
void Term::Data::destroy () 
{ 
  ASS (this);
  ASS (_counter == 0);

  switch (tag()) 
    {
    case VAR:
      delete static_cast<VarData*>(this);
      return;

    case COMPOUND:
      delete static_cast<CompoundData*>(this);
      return;

    case NUMERIC:
      delete static_cast<NumericData*>(this);
      return;
    }
} // Term::Data::destroy ()


/*
//
//  bool Term::isLinearAndFlat () const
//    the term has the form f(x1,...,xn), 
//      where xi are pairwise different variables
//
bool Term::isLinearAndFlat () const
{
  if ( isvar () )
    return false;

  // non-variable term
  return sequenceOfUniqueVariables (args());
} // Term::isLinearAndFlat


//
//  static bool Term::sequenceOfUniqueVariables ( const List* ts )
//    ts is a sequence of pairwise different variables (x1,...,xn) 
//
//  17/07/2001 Manchester, changed from a quadratic algorithm to a linear one
//
bool Term::sequenceOfUniqueVariables ( const List* terms )
{
  // initial size = 32, size increment by 8, initial value false
  typedef ExtendableArray <bool, 32, 8, false> Array; 

  // array used to denote occurrences of variables in the previously parsed arguments
  static Array occur;

  bool result = true;
  const List* ts; 

  // check that all terms are variables
  for ( ts = terms; ! ts->isEmpty (); ts = ts->tail () ) {
    // check that this argument is a variables
    Term* t = ts->head ();
    if ( ! t->isvar() ) { // t is not a variable
      result = false;
      goto reset_array;
    }

    // t is a variable
    Var v = t->var ();

    if ( occur[v] ) { // the variable has already occurred
      result = false;
      goto reset_array;      
    }
    // v is a new variable, mark occurs
    occur[v] = true;
  }

  reset_array:
  // reset values set to true before by false
  for ( const List* ns = terms; ns != ts; ns = ns->tail () ) {
    ASS ( occur[ns->head()->var()] );
    occur[ns->head()->var()] = false;
  }

  return result;
} // Term::sequenceOfUniqueVariables


//
//  void Term::destroy () -
//    destroy the whole term, going recursively to the arguments
//
//  17/07/2001 Manchester, changed to use Iterator
//
void Term::destroy ()
{
  if ( ! isvar() ) { // non-variable
    // destroy every argument
    Term::List::Iterator ts ( args() );

    while ( ts.more() ) {
      ts.next()->destroy ();
    }

    // release the memory occupied by the list
    args()->destroy();
  }

  // release the memory occupied by the term header
  delete this;
} // Term::destroy


//
//  int Term::weight () const -
//    return the number of function symbols and variables in the term
//
int Term::weight () const
{
  if ( isvar() )
    return 1;

return 0;
//  return 1 + weight ( args() );
} // Term::weight


//
//  Term::weight ( const List* ts ) -
//    return the number of function symbols and variables in the list of terms ts
//
int Term::weight ( const List* ts )
{
  int sz = 0;

  while ( ts->isNonEmpty() ) {
    sz += ts->head()->weight();
    ts = ts->tail ();
  }

  return sz;
} // Term::weight ( const List* )
*/

//  bool Term::occurs ( const Signature::Fun* f ) const -
//    true if function symbol f occurs in the term
// 28/09/2002 Manchester, changed to use numeric terms
bool Term::occurs ( const Signature::Fun* f ) const
{
  TRACER ("Term::occurs ( const Signature::Fun* f )");

  switch (tag()) 
    {
    case COMPOUND:
      return functor () == f ||
	     args().occurs(f);
    default:
      return false;
    }
} // Term::occurs


//  bool Term::occurs ( Var v ) const -
//    true if variable v occurs in the term
bool Term::occurs (Var v) const
{
  TRACER ("Term::occurs (Var v)");

  switch (tag()) 
    {
    case COMPOUND:
      return args().occurs(v);

    case VAR:
      return var() == v;

    case NUMERIC:
      return false;

    #if DEBUG_PREPRO
    default:
      ASS(false);
    #endif
    }
} // Term::occurs


//  bool TermList::occurs ( const Signature::Fun* f ) -
//    true if function symbol f occurs in the term list 
bool TermList::occurs (const Signature::Fun* f) const
{
  TRACER ("TermList::occurs (const Signature::Fun* f)");

  TermList ts (*this);

  while ( ts.isNonEmpty() ) {
    if ( ts.head().occurs(f) ) {
      return true;
    }

    ts.pop();
  }

  return false;
} // Term::occurs ( List* ts, Signature::Fun* f )


// true if variable v occurs in the term list ts
// 25/05/2002 Manchester, changed to use iterators
bool TermList::occurs (Var v) const
{
  TRACER ("TermList::occurs (Var v)");

  Iterator<Term> ss (*this);
  while ( ss.more() ) {
    if ( ss.next().occurs(v) ) {
      return true;
    }
  }

  return false;
} // TermList::occurs ( Var v )


/*
//
//  int Term::countOccurrences ( const FSymbol* f ) const -
//    the number of occurrences of function symbol f in the term
//
int Term::countOccurrences ( const FSymbol* f ) const
{
  if ( isvar () )
    return 0;

  int occ = functor() == f ? 1 : 0;

  Term::List::Iterator ts ( args() );
  while ( ts.more() ) {
    occ += ts.next()->countOccurrences (f);
  }

  return occ;
} // Term::countOccurrences


//
//  int Term::countOccurrences ( const Var v ) const -
//    the number of occurrences of variable v in the term
//
int Term::countOccurrences ( const Var v ) const
{
  if ( isvar () )
    return var() == v ? 1 : 0;

  int occ = 0;

  Term::List::Iterator ts ( args() );
  while ( ts.more() ) {
    occ += ts.next()->countOccurrences (v);
  }

  return occ;
} // Term::countOccurrences


//
//  void Term::maxVar ( Var& max ) -
//    sets max to the maximum of its old value and the maximal 
//    variable number in the term
//
void Term::maxVar ( Var& max )
{
  if ( isvar () ) {
    if ( var() > max )
      max = var ();
    return;
  }

  // not a variable
  Term::List::Iterator ts ( args() );

  while ( ts.more() ) {
    ts.next()->maxVar (max);
  }
} // Term::maxVar


//
//  void Term::replace ( Var v, Term* t ) -
//    replace in the term all occurrences of v by t
//
void Term::replace ( Var v, Term* t )
{
  if ( isvar() ) { // the term is a variable
    if ( var() == v ) { // the term coincides with v
      // replace the term by t
      if ( t->isvar() ) { // t is a variable
        // simply replace the variable numbet by that of v
        var ( t->var() );
        return;
      }
      // t is not variable, make a deep copy of t to this
      tag ( COMPOUND );
      functor ( t->functor() );
      args ( copy(t->args()) );
      return;
    }
    // t is a variable different from v, do nothing
    return;
  }

  // t is not a variable, replace v by t in all arguments
  Term::List::Iterator ts ( args() );

  while ( ts.more() ) {
    ts.next()->replace ( v, t );
  }
} // Term::replace
*/


//  bool Term::equal ( const Term* t ) const -
//    true if the term is equal to t
// 28/09/2002 Manchester, changed to include numeric data
bool Term::equal (const Term& t) const
{
  TRACER("Term::equal");

  if (tag() != t.tag()) {
    return false;
  }

  switch (tag()) 
    {
    case VAR:
      return var() == t.var();

    case COMPOUND:
      return functor() == t.functor() &&
             args().equal (t.args());

    case NUMERIC:
      return number() == t.number();

    #ifdef DEBUG_PREPRO
    default:
      ASS(false);
    #endif
    }
} // Term::equal



//
//  static bool Term::equal ( const List* ts1, const List* ts2 ) -
//    true if ts1 is equal to ts2
//
//  20/07/2001 Manchester, iterators used
//  29/04/2002 Manchester, bug in ASS fixed
bool TermList::equal (TermList ts) const
{
  TRACER ("TermList::equal");

  Iterator<Term> ss1(*this);
  Iterator<Term> ss2 (ts);
  while ( ss1.more() ) {
    ASS ( ss2.more() );
    
    if ( ! ss1.next().equal(ss2.next()) ) {
      return false;
    }
  }
  ASS ( ! ss2.more() );

  return true;
} // Term::equal( List* , List* )


//  Compare Term::compare ( const Term* t ) const -
//    compare terms this and t using an order described below
//    Used to normalize terms
//
//    t1 is less than t2 if
//      (1) t1 is variable and t2 is not, or else
//      (2) t1->functor < t2.functor, or else
//      (3) t1.args << t2.args,  where << is the lexicographic comparison
// 28/09/2002 Manchester, changed to use numerical terms
//
//    t1 is less than t2 if
//      (1) the tag of t1 is less than the tag of t2
//      (2) both are numeric and the value of t1 is less than the value of t2
//      (3) both are compound and
//          (3a) t1->functor < t2.functor, or else
//          (3b) t1.args << t2.args,  where << is the lexicographic comparison
Compare Term::compare (const Term& t) const
{
  switch (Int::compare (tag(), t.tag())) 
    {
    case GREATER:
      return GREATER;
    case LESS:
      return LESS;
    case EQUAL:
      break;
    }

  switch (tag()) /// note that tag() is equal to t.tag()
    {
    case VAR:
      return EQUAL;

    case NUMERIC:
      if (number() > t.number()) {
	return GREATER;
      }
      if (number() < t.number()) {
	return LESS;
      }
      return EQUAL;

    case COMPOUND:
      switch ( functor()->compare (t.functor()) ) 
	{
	case LESS:
	  return LESS;
	
	case GREATER:
	  return GREATER;

	case EQUAL:
	  // both are nonvariable and functors are equal
	  return args().compare(t.args());
	}

    #if DEBUG_PREPRO
    default:
      ASS(false);
    #endif
    }
} // Term::compare


//  Compare Termlist::compare (const List* ts2 ) -
//    compare terms lists ts1 and ts2 using the order above
//    Used to normalize terms
Compare TermList::compare (TermList ts2) const
{
  TermList ts1 (*this);

  for (;;) {
    if ( ts1.isEmpty() ) {
      if (ts2.isEmpty()) {
        return EQUAL;
      }
      return LESS;
    }
    // ts1 is non-empty
    if (ts2.isEmpty()) {
      return GREATER;
    }
    // both non-empty
    Compare cmp = ts1.head().compare(ts2.head());
    if (cmp != EQUAL) {
      return cmp;
    }

    ts1.pop();
    ts2.pop();
  }
} // Term::compare


/*
//
//  void Term::split ( FSymbol* f, int m, int i, int r, FSymbol* g, FSymbol* h )
//    replace all occurrences of f(t_1,...,t_mi+r) in the term by
//    f(g(t_1,...,t_m),...,g(t_m(i-1)+1,...,t_mi),h(t_mi+1,...,t_mi+r))
//
void Term::split ( FSymbol* tosplit, int maxArity, int iterations, int restArity, 
                   FSymbol* firstFun, FSymbol* restFun )
{
  if ( isvar() )
    return;

  // the term is a non-variable
  // first, make the replacement in all arguments
  Term::List::Iterator ts ( args() );
  
  while ( ts.more() ) {
    ts.next()->split ( tosplit, maxArity, iterations, restArity, firstFun, restFun );
  }

  // now make the replacement on the top level
  if ( functor() != tosplit ) // the functor is not f, terminate
    return;
    
  // the functor is f, replace
  args ( split(args(), maxArity, iterations, restArity, firstFun, restFun) );
} // Term::split


//
//  void Term::split ( List* ts, int m, int i, int r, FSymbol* g, FSymbol* h )
//    given the list ts = (t_1,...,t_mi+r), return the list
//    g(t_1,...,t_m),...,g(t_m(i-1)+1,...,t_mi),h(t_mi+1,...,t_mi+r)
//
Term::List* Term::split ( List* ts, int maxArity, int iterations, int restArity, 
                          FSymbol* firstFun, FSymbol* restFun )
{
  // note that the arguments are already splitted
  if ( iterations == 0 ) {  // no more iterations
    // return h(t_mi+1,...,t_mi+r), if r != 0 or 
    // the empty list if r = 0
    if ( restArity == 0 ) { // r = 0
      ASS( ts->isEmpty() );

      return Term::List::empty ();
    }

    // restArity != 0;
    return new List ( new Term (restFun, ts) );
  }

  // iterations != 0
  // chop from the head of ts a list of length maxArity
  List* last = ts;

  for ( int i = maxArity - 1; i > 0; i-- ) {
    last = last->tail ();
  } 
  
  // now last points to the last chain before the rest
  // rest will be used as the rest of the input list, without the first m members
  List* rest = last->tail();
  
  // make ts the list of the form (t_1,...,t_m)
  last->tail ( Term::List::empty() );

  return new List ( new Term (firstFun,ts),
                        split ( rest, maxArity, iterations - 1, restArity, firstFun, restFun ) );
    
} // Term::split


Term::List* Term::splitLargeArity ( Term::List* ts, FSymbol::List* newFs )
{
  if ( newFs->isEmpty() ) {
    ASS ( ts->isEmpty() );
    return Term::List::empty ();
  }

  // 
  List* rest;
  FSymbol* f = newFs->head();
  List* first = ts->split (f->arity(), rest);
  return new Term::List ( new Term (f, first),
                          splitLargeArity (rest, newFs->tail()) );

} // Term::splitLargeArity


void Term::splitLargeArity (const FSymbol* f, FSymbol::List* newFs )
{
  if ( isvar() )
    return;

  Term::List::Iterator ts ( args() );
  while ( ts.more() ) {
    ts.next()->splitLargeArity (f,newFs);
  }

  if ( functor() == f ) {
    args ( Term::splitLargeArity (args(),newFs) );
  }
} // Term::splitLargeArity


bool Term::isGround () const
{
  if ( isvar() )
    return false;

  Term::List::Iterator ts ( args() );
  while ( ts.more() ) {
    if ( ! ts.next()->isGround() ) {
      return false;
    }
  }

  return true;
} // Term::isGround
*/


// 03/06/2001
// ls can be obtained from rs by swapping x and y
// 30/04/2002 Manchester, changed
bool TermList::equalUpTo (TermList rs, Var x, Var y) const
{
  Iterator<Term> ts1 (*this);
  Iterator<Term> ts2 (rs);

  while ( ts1.more() ) {
    // ls and rs must be of the same length
    ASS ( ts2.more() );
    if ( ! ts1.next().equalUpTo(ts2.next(), x, y) ) {
      return false;
    }
  }
  ASS ( ! ts2.more() );

  return true;
} // Term::equalUpTo


// r can be obtained from this by swapping x and y
// 30/04/2002 Manchester
// 28/09/2002 Manchester, changed to include numerical data
bool Term::equalUpTo (const Term& r, Var x, Var y) const
{
  if (tag() != r.tag()) {
    return false;
  }

  switch (tag()) // note that both tags are equal
    {
    case VAR:
      if (var() == r.var()) {
	return true;
      }
      // this and r are different vars
      return (var() == x && r.var() == y) ||
	     (var() == y && r.var() == x);

    case NUMERIC:
      return number() == r.number();

    case COMPOUND:
      return functor() == r.functor() &&
             args().equalUpTo(r.args(),x,y);

    #if DEBUG_PREPRO
    default:
      ASS(false);
    #endif
    }
} // Term::equalUpTo


// given a sequence of variables v1,...,vn
// return the sequence of terms v1,...,vn
// 03/06/2001
// 25/08/2002 Torrevieja changed
// 09/07/2002 Manchester, changed
void TermList::buildFrom (VarList vs)
{
  if (vs.isEmpty()) {
    return;
  }

  *this = TermList(Term (vs.head()));
  const_cast<TermList&>(tail()).buildFrom (vs.tail());
} // TermList::buildFrom


// true if any of the terms has free variables not occurring in vs
// 06/05/2002 Manchester
// 11/09/2002 Manchester, changed
// 28/09/2002 Manchester, changed by adding call to Term::hasVarsNotIn
bool TermList::hasVarsNotIn (VarListList vs) const
{
  Iterator<Term> terms (*this);
  while ( terms.more() ) {
    if (terms.next().hasVarsNotIn (vs)) {
      return true;
    }
  }

  return false;
} // TermList::hasVarsNotIn (...)


// true if the term has free variables not occurring in vs
// 28/09/2002 Manchester
bool Term::hasVarsNotIn (VarListList vs) const
{
  switch (tag()) 
    {
    case VAR: {
      // check that the variable does not occur in any of the lists in vs
      Var v = var ();

      Iterator<VarList> vss (vs);
      while ( vss.more() ) {
	if ( vss.next().member(v) ) { // found
	  return false;
	}
      }
      // not found
      return true;
    }

    case NUMERIC:
      return false;

    case COMPOUND:
      return args().hasVarsNotIn (vs);

    #ifdef DEBUG_PREPRO
    default:
      ASS (false);
    #endif
    }
} // Term::hasVarsNotIn


// ts consists of variables only
// 07/05/2002 Manchester
bool TermList::varsOnly () const 
{
  TRACER ("TermList::varsOnly");

  Iterator<Term> t (*this);
  while ( t.more() ) {
    if (! t.next().isvar() )
      return false;
  }

  return true;
} // Term::varsOnly


// 28/09/2002 Manchester, changed to include numeric data
bool allIn (const Term t, Sort<Var>& s)
{
  switch (t.tag()) 
    {
    case Term::VAR:
      return s.member (t.var());

    case Term::NUMERIC:
      return true;

    case Term::COMPOUND: 
      {
	Iterator<Term> ts (t.args());
	while (ts.more()) {
	  if ( ! allIn (ts.next(),s) ) {
	    return false;
	  }
	}

	return true;
      }
    #ifdef DEBUG_PREPRO
    default:
      ASS(false);
    #endif
    }
} // allIn


// checks that this has the form t[x] and lhs has the form f(x)
// where f is a function symbol and t[x] is a term of x not containing f
// assumes both arguments are non-variables
// 09/05/2002, Manchester
// 24/10/2002 Manchester, bug fixed (when lhs was numeric there was a problem)
bool Term::defines (const Term& lhs) const  // auxiliary function for atoms
{
  TRACER( "Term::defines" );

  if (lhs.tag() == NUMERIC) {
    return false;
  }

  TermList ls (lhs.args());
  int ll = ls.length ();
  if ( ! ls.varsOnly() ||
       occurs( lhs.functor()) ) {
    return false;
  }

  // sorting variables in ls
  Sort<Var> lvar (ll);
  Iterator<Term> largs (ls);
  while (largs.more()) {
    lvar.add (largs.next().var());
  }
  lvar.sort ();
  lvar.removeDuplicates ();
  if (ll != lvar.length()) { // there was a duplicate removed, so lhs is not linear
    return false;
  }

  // now we have to check that every variable occuring in l 
  // also occurs in the sorted list
  return allIn (*this, lvar);
} // Term::defines


// Substitution subst is applied to term
// 25/08/2002 Torrevieja
// 28/09/2002 Manchester, changed to include numeric data
void Term::apply (const Substitution& subst) 
{
  TRACER ("Term::apply");

  switch (tag())
    {
    case VAR:
      {
	Var v = var();
	Term t;
	if (subst.bound (v,t)) { // variable is bound
	  *this = t;
	}
	return;
      }

    case NUMERIC:
      return;

    case COMPOUND:
      {
	TermList ts (args());
	ts.apply(subst);
	if (ts == args()) { // arguments did not change
	  return;
	}
	
	Term t (functor(),ts);
	*this = t;
      }
    }
} // Term::apply


// Substitution subst is applied to term list
// 25/08/2002 Torrevieja
void TermList::apply (const Substitution& subst) 
{
  TRACER ("TermList::apply");

  if (isEmpty()) {
    return;
  }
  // not empty
  Term t (head());
  t.apply(subst);
  TermList ts (tail());
  ts.apply (subst);

  if (t == head() && ts == tail()) { // no changes
    return;
  }

  TermList result (t,ts);
  *this = result;
} // TermList::apply


// rectify a term
// 25/08/2002 Torrevieja
// 28/09/2002 Manchester, changed to include numeric data
void Term::rectify (Substitution& subst, Var& last, VarList& freeVars) 
{
  TRACER ("Term::rectify");

  switch (tag())
    {
    case VAR:
      {
	Var v = var();
	Term t;
	if ( subst.bound(v,t) ) { // variable is bound
	  *this = t;
	  return;
	}
	// unbound variable
	Var newV = ++last;
	Term s (newV);
	subst.bind (v, s);
	freeVars.push (newV);
	*this = s;
	return;
      }

    case NUMERIC:
      return;

    case COMPOUND:
      {
	TermList ts (args());
	ts.rectify (subst, last, freeVars);

	if (ts == args()) { // args did not change
	  return;
	}

	Term t (functor(), ts);
	*this = t;
      }
    }
} // Term::rectify


// rectify a term list
// 25/08/2002 Torrevieja
void TermList::rectify (Substitution& subst, Var& last, VarList& freeVars)
{
  TRACER ("TermList::rectify");

  if (isEmpty()) {
    return;
  }
  // non-empty
  Term t (head());
  t.rectify (subst, last, freeVars);
  TermList ts (tail());
  ts.rectify (subst, last, freeVars);

  if (t == head() && ts == tail()) {
    return;
  }

  TermList result (t,ts);
  *this = result;
} // TermList::rectify


// the list is a renaming of ts
// sbst is a previously computed substitution
// 03/10/2002 Manchester
bool TermList::isRenamingOf (TermList ts, Substitution& sbst) const
{
  if ( isEmpty() ) {
    ASS (ts.isEmpty());
    return true;
  }

  return head().isRenamingOf(ts.head(), sbst) &&
         tail().isRenamingOf (ts.tail(), sbst);
} // TermList::isRenamingOf


// the term is a renaming of t
// sbst is a previously computed substitution
// 03/10/2002 Manchester
bool Term::isRenamingOf (Term t, Substitution& sbst) const
{
  if (tag() != t.tag()) {
    return false;
  }

  // tags are equal
  switch ( tag() ) 
    {
    case VAR: {
      Term s;
      if ( sbst.bound(var(),s) ) {
	return t.equal (s);
      }
      // the variable is not unbound
      if (sbst.inRange(t)) {
	return false;
      }
      // t is a variable not in range of sbst
      sbst.bind (var(), t);
      return true;
    }


    case NUMERIC:
      return number() == t.number();

    case COMPOUND:
      return functor() == t.functor() &&
	     args().isRenamingOf(t.args(), sbst);

    #ifdef DEBUG_PREPRO
    default:
      ASS( false );
    #endif
    }
} // Term::isRenamingOf


// set to true in the array occs variables occurring in the list
// 13/04/2003 Torrevieja
void TermList::occurring (bool* occs, Var max) const
{
  TRACER("TermList::occurring");

  Iterator<Term> ts (*this);
  while (ts.more()) {
    ts.next().occurring(occs,max);
  }
} // TermList::occurring


// set to true in the array occs variables occurring in the term
// 13/04/2003 Torrevieja
void Term::occurring (bool* occs, Var max) const
{
  TRACER("Term::occurring");

  switch (tag()) 
    {
    case VAR: {
      Var v = var ();
      if (v <= max) {
	occs[v] = true;
      }
      return;
    }

    case COMPOUND:
      args().occurring(occs,max);
      return;

    case NUMERIC:
      return;
    }
} // Term::occurring

/*
  this is helpful
  switch (tag()) 
    {
    case VAR:
    case COMPOUND:
    case NUMERIC:
    }
*/

