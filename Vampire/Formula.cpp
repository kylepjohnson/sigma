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
//  file Formula.cpp
//  implements class Formula
//

#include <string>


#include "Miniscope.hpp"
#include "Formula.hpp"
#include "Sort.hpp"
#include "Substitution.hpp"
#include "Inference.hpp"


// assignment operator
// 25/08/2002 Torrevieja
void Formula::operator = (const Formula& t)
{
  if (t._data) {
    t._data->ref ();
  }

  if (_data) {
    _data->deref ();
  }

  _data = t._data;
} // Formula::operator=


// nullifies the data
// 15/04/2003 Torrevieja
void Formula::makeNull ()
{
  if (_data) {
    _data->deref ();
    _data = 0;
  }
} // Formula::makeNull


inline
void Formula::Data::destroy () 
{ 
  TRACER ("Formula::Data::destroy");

  ASS (this);
  ASS (_counter == 0);

  switch ( connective() ) {
  case ATOM:
    delete static_cast<AtomicData*>(this);
    return;

  case AND:
  case OR:
    delete static_cast<JunctionData*>(this);
    return;

  case IMP:
  case IFF:
  case XOR:
    delete static_cast<BinaryData*>(this);
    return;

  case NOT:
    delete static_cast<UnaryData*>(this);
    return;

  case FORALL:
  case EXISTS:
    delete static_cast<QuantifiedData*>(this);
    return;

  default:
    ASS( false );
    return;
  }
} // Formula::Data::deref ()


// // size is the total number of occurrences of connectives, quantifiers, and atoms
// // in the formula
// int Formula::size () const
// {
//   switch ( connective () ) {
//     case ATOM: 
//       return 1;

//     case AND:
//     case OR: {
//       int sz = -1;
      
//       for ( List* fs = args(); fs->isNonEmpty (); fs = fs->tail() )
//         sz += fs->head()->size() + 1;
         
//       return sz;
//     }
        
//     case IMP:
//     case IFF:
//     case XOR:
//       return left()->size() + right()->size () + 1;

//     case NOT:
//       return arg()->size () + 1;

//     case FORALL:
//     case EXISTS:
//       return vars()->length() + arg()->size () + 1;

//     default:
//       ASS ( false );
//       return 0;
//   }
// } // Formula::size


// // the total number of symbols in the formula
// // a bug fixed 30/06/2001, flight Kopenhagen-Manchester
// int Formula::weight () const
// {
//   switch ( connective () ) {
//     case ATOM: 
//       return atom()->weight();

//     case AND:
//     case OR: {
//       int sz = -1;
      
//       for ( List* fs = args(); fs->isNonEmpty (); fs = fs->tail() )
//         sz += fs->head()->weight() + 1;
         
//       return sz;
//     }
        
//     case IMP:
//     case IFF:
//     case XOR:
//       return left()->weight() + 
//              right()->weight () + 1;

//     case NOT:
//       return arg()->weight () + 1;

//     case FORALL:
//     case EXISTS:
//       return vars()->length() + arg()->weight () + 1;

//     default:
//       ASS ( false );
//       return 0;
//   }
// } // Formula::weight


// rectify the formula 
// and universally quantify its free variables (added 14/07/2002)
// 27/06/2002 Manchester
// 30/08/2002 Torrevieja, changed
void Formula::rectify ()
{
  TRACER ("Formula::rectify");

  Substitution sbst;
  Var last = firstVar - 1;
  VarList free;
  
  rectify (sbst, last, free);
  if (free.isEmpty()) {
    return;
  }

  *this = Formula (FORALL, free, *this);
} // Formula::rectify


// rectify the formula 
// return in vs the list of free variables of vs 
// and in args the corresponding renamed list of terms
// 03/08/2002 Torrevieja
// 30/08/2002 Torrevieja, changed
void Formula::rectify (VarList& vs, TermList& args)
{
  TRACER ("Formula::rectify/2");

  Substitution sbst;
  Var last = firstVar - 1;
  VarList free;
  
  rectify (sbst, last, free);
  if (free.isEmpty()) {
    return;
  }

  sbst.domain (vs);
  sbst.range (args);

  *this = Formula (FORALL, free, *this);
} // Formula::rectify


// rectify the formula using previously collected substitution sbst
// and universally quantify its free variables (added 14/07/2002)
// 27/06/2002 Manchester
// 30/08/2002 Torrevieja, changed
void Formula::rectify ( Substitution& sbst, Var& last, VarList& freeVars )
{
  TRACER ("Formula::rectify/3");
 
  switch ( connective() ) {
  case ATOM: {
    Atom a (atom());
    a.rectify (sbst, last, freeVars);
    if (a == atom()) {
      return;
    }
    *this = Formula (a);
    return;
  }

  case AND: 
  case OR: {
    FormulaList newArgs (args());
    newArgs.rectify(sbst, last, freeVars);
    if (newArgs == args()) {
      return;
    }
    *this = Formula(connective(), newArgs);
    return;
  }

  case IMP: 
  case IFF: 
  case XOR: {
    Formula l (left());
    l.rectify (sbst, last, freeVars);
    Formula r (right());
    r.rectify (sbst, last, freeVars);
    if (l == left() && r == right()) {
      return;
    }
    *this = Formula (connective(), l, r);
    return;
  }

  case NOT: {
    Formula arg (uarg());
    arg.rectify (sbst, last, freeVars);
    if (uarg() == arg) {
      return;
    }
    *this = Formula (NOT, arg);
    return;
  }

  case FORALL: 
  case EXISTS: {
    VarList vs;
    vs.rectify (vars(), sbst, last);
    // this could be changed in the future by checking on equality
    // between vs and rectified vars
    Formula arg (qarg());
    arg.rectify (sbst, last, freeVars);
    Iterator<Var> ws (vars());
    while (ws.more()) {
      sbst.undoBinding (ws.next());
    }
    *this = Formula ( connective(), vs, arg);
    return;
  }

  default:
    ASS( false );
  }
} // Formula::rectify


// one of the auxiliary rectification functions
// 30/08/2002 Torrevieja, changed
void FormulaList::rectify (Substitution& subst, Var& last, VarList& freeVars)
{
  TRACER ("FormulaList::rectify/4");

  if (isEmpty()) {
    return;
  }
  Formula h (head());
  h.rectify (subst, last, freeVars);
  FormulaList t (tail());
  t.rectify (subst, last, freeVars);
  if (h == head() && t == tail()) {
    return;
  }

  *this = FormulaList (h,t);
} // FormulaList::rectify


// true if p occurs in this
// 04/09/2002 Bolzano, changed
bool Formula::occurs ( const Signature::Pred* p ) const
{
  switch ( connective () ) {
    case ATOM: 
      return p == atom().functor ();

    case AND:
    case OR: {
      Iterator<Formula> fs (args());
      while (fs.more()) {
        if (fs.next().occurs(p))
          return true;
      }
      return false;
    }

    case IMP:
    case IFF:
    case XOR:
      return left().occurs ( p )
          || right().occurs ( p );

    case NOT:
      return uarg().occurs ( p );

    case FORALL:
    case EXISTS:
      return qarg().occurs ( p );

    default:
      ASS ( false ) ;
      return false;
  }
} // Formula::occurs


// checks that the formula has the form forall x (P(x) <=> A[x]),
// where P is a predicate symbol and A[x] is a formula of x not containing P
// 04/05/2002, Manchester, bug fixed: previously it was not checked that x are all variables of A
// 04/09/2002 Bolzano, changed
bool Formula::isPredicateDefinition (Atom& lhs, Formula& rhs) const
{
  TRACER ("Formula::isPredicateDefinition");

  // first, collect the variables occurring in the outermost quantifiers in a list,
  // sort it and remove duplicates
  int noOfVars = 0;   // number of vars in the outermost quantifier prefix

  Formula f (*this);
  // skip all universal quantifiers in front of the formula
  while ( f.connective() == FORALL ) {
    noOfVars += f.vars().length ();
    f = f.qarg ();
  }

  // check that f has the form lft <=> rht
  if ( f.connective() != IFF ) 
    return 0;

  Formula lft (f.left());
  Formula rht (f.right());
  bool leftFlat;
  bool rightFlat;
  Atom la;
  Atom ra;
  if (lft.connective() == ATOM) {
    la = lft.atom();
    leftFlat = true;
  }
  else {
    leftFlat = false;
  }

  if (rht.connective() == ATOM) { 
    ra = rht.atom();
    rightFlat = true;
  }
  else {
    rightFlat = false;
  }

  // we are only interested in lhs (or rhs) which are non-flat non-equality atoms
  // whose predicate symbols do not occur in the other side
  if ( leftFlat && 
        ( la.isEquality() ||
          ! la.isFlat() || 
          rht.occurs (la.functor()) ) ) {
    leftFlat = false;
  }
  if ( rightFlat && 
        ( ra.isEquality() ||
          ! ra.isFlat() || 
          lft.occurs (ra.functor()) ) ) {
    rightFlat = false;
  }

  if (! (leftFlat || rightFlat)) {
    return false;
  }

  // now at least one of la, ra is flat
  // build an ordered list of the universally quantified variables of the formula prefix
  // with duplicates removed
  Sort<Var> univ (noOfVars);

  // add all variables to univ
  f = *this;
  // skip all universal quantifiers in front of the formula
  while ( f.connective() == FORALL ) {
    Iterator<Var> vs (f.vars());
    while ( vs.more() ) {
      univ.add ( vs.next() );
    }
    f = f.qarg ();
  }

  univ.sort ();
  univ.removeDuplicates ();
  int length = univ.length();

  // check that la (resp. ra) qualifies for the defined atom. This is done as follows:
  // the arguments of la are sorted. la qualifies iff the list of arguments coincides 
  // with the list univ 
  if (leftFlat) {
    if (la.functor()->arity() != length) {
      leftFlat = false;
    }
    else {
      Sort<Var> lvar (length);
      Iterator<Term> args (la.args());
      while (args.more()) {
        lvar.add (args.next().var());
      }
      lvar.sort ();
      for (int i = 0; i < length;i++) {
        if ( lvar[i] != univ[i] ) {
          leftFlat = false;
          break;
        }
      }
    }
  }
  if (leftFlat) {
    rhs = rht;
    lhs = la;
    return true;
  }

  if (rightFlat) {
    if (ra.functor()->arity() != length) {
      rightFlat = false;
    }
    else {
      Sort<Var> rvar (length);
      Iterator<Term> args (ra.args());
      while (args.more()) {
        rvar.add (args.next().var());
      }
      rvar.sort ();
      for (int i = 0; i < length;i++) {
        if ( rvar[i] != univ[i] ) {
          rightFlat = false;
          break;
        }
      }
    }
  }
  if (rightFlat) {
    rhs = lft;
    lhs = ra;
    return true;
  }

  return false;
} // Formula::isPredicateDefinition


// checks that the formula has the form forall x (f(x) = t[x]),
// where f is a function symbol and t[x] is a term of x
// WARNING: it is assumed that the formula contains no free variables!
// 09/05/2002, Manchester
// 04/09/2002 Bolzano, changed
bool Formula::isFunctionDefinition (Term& lhs, Term& rhs) const
{
  TRACER ( "Formula::isFunctionDefinition" );

  // first, collect the variables occurring in the outermost quantifiers in a list,
  // sort it and remove duplicates
  Formula f (*this);
  // skip all universal quantifiers in front of the formula
  while ( f.connective() == FORALL ) {
    f = f.qarg ();
  }

  // check that f is an atom
  if ( f.connective() != ATOM ) 
    return 0;

  return f.atom().isDefinition(lhs, rhs);
} // Formula::isFunctionDefinition


// bool Formula::isLinearAndFlat ()
// {
//   return ( connective () == ATOM ) 
//       && atom()->isLinearAndFlat ();      
// } // Formula::isFunctionDefinition


// int Formula::countOccurrences ( const FSymbol* f ) const
// {
//   switch ( connective() ) {
//     case ATOM:
//       return atom()->countOccurrences(f);

//     case AND:
//     case OR: {
//       int occ = 0;

//       for ( List* fs = args(); fs->isNonEmpty(); fs = fs->tail() )
//         occ += fs->head()->countOccurrences ( f );

//       return occ;
//     }

//     case IMP:
//     case IFF:
//     case XOR:
//       return left()->countOccurrences(f) + 
//              right()->countOccurrences(f);

//     case NOT:
//     case FORALL:
//     case EXISTS:
//       return arg()->countOccurrences(f);

//     default:
//       ASS ( false );
//       return 0;
//   }
// }


// int Formula::countOccurrences ( const PSymbol* p ) const
// {
//   switch ( connective() ) {
//     case ATOM:
//       return atom()->functor() == p ? 1 : 0;

//     case AND:
//     case OR: {
//       int occ = 0;

//       for ( List* fs = args(); fs->isNonEmpty(); fs = fs->tail() )
//         occ += fs->head()->countOccurrences ( p );

//       return occ;
//     }

//     case IMP:
//     case IFF:
//     case XOR:
//       return left()->countOccurrences(p) + 
//              right()->countOccurrences(p);

//     case NOT:
//     case FORALL:
//     case EXISTS:
//       return arg()->countOccurrences(p);

//     default:
//       ASS ( false );
//       return 0;
//   }
// }


// // 02/06/2001, copied from Clause
// void Formula::split ( Formula* l, List* spl, 
//                       List*& less, List*& equal, List*& greater )
// {
//   ASS ( debug_clause, spl->isNonEmpty() );

//   less = Formula::List::empty();
//   equal = Formula::List::empty();
//   greater = Formula::List::empty();

//   for (;;) {
//     List* tail = spl->tail ();

//     switch ( spl->head()->compare(l) ) {
//       case GREATER:
//         spl->tail ( greater );
//         greater = spl;
//         break;
//       case EQUAL:
//         spl->tail ( equal );
//         equal = spl;
//         break;
//       case LESS:
//         spl->tail ( less );
//         less = spl;
//         break;
//     }

//     if ( tail->isEmpty() )
//       return;
//     spl = tail;
//   }
// } // Formula::split


// 03/06/2001
// 29/04/2002, changed to use arbitrary universal quantifiers and call function on atoms
// 30/08/2002 Torrevieja, changed
bool Formula::isReflexivityAxiom () const
{
  TRACER("Formula::isReflexivityAxiom");

  Formula f (*this);
  while ( f.connective() == FORALL ) {
    f = f.qarg ();
  }

  // f must be t = t
  return f.connective() == ATOM &&
         f.atom().isTautology();
} // Formula::isReflexivityAxiom


// 03/06/2001
// 29/04/2002, changed to use arbitrary universal quantifiers and call function on atoms
// 30/08/2002 Torrevieja, changed
bool Formula::isSymmetryAxiom () const
{
  TRACER ("Formula::isSymmetryAxiom");

  Formula f (*this);
  while ( f.connective() == FORALL ) {
    f = f.qarg ();
  }

  // f must be x = y -> y = x
  if ( f.connective() != IMP ) {
    return false;
  }

  // f1 must be x = y 
  Formula f1 (f.left ());
  if ( f1.connective() != ATOM ) {
    return false;
  }
  // f2 must be y = x
  Formula f2 (f.right ());
  if ( f2.connective() != ATOM ) {
    return false;
  }

  return f1.atom().swap (f2.atom());
} // Formula::isSymmetryAxiom


// 03/06/2001
// 29/04/2002, changed to use arbitrary universal quantifiers and call function on atoms
// 30/08/2002 Torrevieja, changed
bool Formula::isTransitivityAxiom () const
{
  TRACER ("Formula::isTransitivityAxiom");

  Formula f (*this);
  while ( f.connective() == FORALL ) {
    f = f.qarg ();
  }

  // f must be x = y & y = z -> x = z
  if ( f.connective() != IMP ) {
    return false;
  }

  // c must be x = y & y = z
  Formula c (f.left ());
  if ( c.connective() != AND || c.args().length() != 2 ) {
    return false;
  }

  // c1,c2 must be x = y and y = z
  Formula c1 (c.args().head ());
  if ( c1.connective() != ATOM ) {
    return false;
  }
  Formula c2 (c.args().second());
  if ( c2.connective() != ATOM ) {
    return false;
  }
  // c3 must be x = z
  Formula c3 (f.right());
  if ( c3.connective() != ATOM ) {
    return false;
  }

  return Atom::transitivity (c1.atom(), c2.atom(), c3.atom());
} // Formula::isTransitivityAxiom


// 03/06/2001
// 29/04/2002, changed to use arbitrary universal quantifiers and call function on atoms
// 30/08/2002 Torrevieja, changed
bool Formula::isFunctionReflexivityAxiom () const
{
  TRACER("isFunctionReflexivityAxiom");

  Formula f (*this);
  while ( f.connective() == FORALL ) {
    f = f.qarg ();
  }

  // f must be x = y -> F(x)=F(y)
  if ( f.connective() != IMP ) {
    return false;
  }

  // both subformulas must be atoms
  if ( f.left().connective() != ATOM || 
       f.right().connective() != ATOM ) {
    return false;
  }
  
  return Atom::functionMonotonicity (f.left().atom(), f.right().atom());
} // Formula::isFunctionReflexivityAxiom


// 03/06/2001
// 29/04/2002, changed to use arbitrary universal quantifiers and call function on atoms
// 30/08/2002 Torrevieja, changed
bool Formula::isPredicateReflexivityAxiom () const
{
  TRACER("isPredicateReflexivityAxiom");
  Formula f (*this);
  while ( f.connective() == FORALL ) {
    f = f.qarg ();
  }

  // f must be x = y & P(x) -> P(y)
  if ( f.connective() != IMP ) {
    return false;
  }

  // c must be x = y & P(x) (but in unclear order)
  Formula c (f.left());
  if ( c.connective() != AND || c.args().length() != 2 ) {
    return false;
  }
  Formula c1 (c.args().head());
  if ( c1.connective() != ATOM ) {
    return false;
  }
  Formula c2 (c.args().second());
  if ( c2.connective() != ATOM ) {
    return false;
  }
  Formula c3 (f.right()); // P(y)
  if ( c3.connective() != ATOM ) {
    return false;
  }
  Atom a1 (c1.atom());
  Atom a2 (c2.atom());
  Atom a3 (c3.atom());

  return Atom::predicateMonotonicity (a1,a2,a3) ||
         Atom::predicateMonotonicity (a2,a1,a3);
} // Formula::isPredicateReflexivityAxiom


// 03/06/2001
bool Formula::isEqualityAxiom () const
{
  TRACER("Formula::isEqualityAxiom");

  return isReflexivityAxiom () || 
         isSymmetryAxiom () ||
         isTransitivityAxiom () ||
         isFunctionReflexivityAxiom () ||
         isPredicateReflexivityAxiom ();
} // Formula::isEqualityAxiom


// // true if v has a free occurrence in the formula
// // 13/06/2001 Manchester
// bool Formula::occurs (Var v) const
// {
//   switch ( connective() ) {
//     case ATOM:
//       return atom()->occurs (v);

//     case AND:
//     case OR: {
//       Formula::List::Iterator fs (args());
//       while (fs.more()) {
//         if ( fs.next()->occurs (v) ) 
//           return true;
//       }
//       return false;
//     }

//     case IFF:
//     case XOR:
//     case IMP:
//       return left()->occurs (v) ||
//              right()->occurs (v);

//     case FORALL:
//     case EXISTS:
//       if ( vars()->member(v) ) 
//         return false;
//       return arg()->occurs(v);

//     case NOT:
//       return arg()->occurs(v);

//     default:
//       ASS (debug_formula,false);
//       return 0;
//   }
// } // Formula::occurs (Var v)


// true if the formula has free variables
// 06/05/2002 Manchester
bool Formula::hasFreeVars () const
{
  VarListList empty;
  return hasFreeVars (empty);
} // Formula::hasFreeVars


// true if the formula has free variables not occurring in vs
// 06/05/2002 Manchester
bool Formula::hasFreeVars (VarListList vs) const
{
  TRACER( "Formula::hasFreeVars" );

  switch ( connective() ) {
    case ATOM:
      return atom().hasVarsNotIn (vs);

    case AND: {
    case OR:
      Iterator<Formula> arg (args());
      while ( arg.more() ) {
        if ( arg.next().hasFreeVars(vs) ) {
          return true;
        }
      }
      return false;
    }

    case IMP:
    case IFF:
    case XOR:
      return left().hasFreeVars (vs) ||
             right().hasFreeVars (vs);

    case NOT:
      return uarg().hasFreeVars (vs);

    case FORALL:
    case EXISTS: {
      VarListList ws (vars(),vs);
      return qarg().hasFreeVars (ws);
    }

    default:
      ASS (false);
      return false;
  }
} // Formula::hasFreeVars


// transform each formula into ennf
// 12/05/2002 Manchester
// 30/08/2002 Torrevieja, changed
void Formula::ennf (bool polarity)
{
  switch ( connective() ) {
    case ATOM:
      if (polarity) {
        return;
      }
      *this = Formula (NOT, *this);
      return;

    case AND: 
    case OR: {
      FormulaList fs (args());
      Connective con = (polarity == (connective() == AND)) ? AND : OR;
      fs.ennf (con, polarity);
      if (fs == args()) { 
        return;
      }
      *this = Formula (con, fs);
      return;
    }
      
    case IMP: {
      Formula l (left());
      l.ennf (!polarity);
      Formula r (right());
      r.ennf (polarity);

      FormulaList args; // arguments of the result

      bool lj = (polarity && l.connective() == OR) || // left is the right kind of junction
                (!polarity && l.connective() == AND);
      bool rj = (polarity && r.connective() == OR) || // right is the right kind of junction
                (!polarity && r.connective() == AND);


      if (lj) { // l1 & ... & ln
        args = l.args();
        if (rj) { // l1 & ... & ln, r1 & ... & rm
	  args.append(r.args());
        }
	else { // l1 & ... & ln, r0
	  args.append (FormulaList(r));
	}
      }
      // l0
      else if (rj) { // r1 & ... & rm
        args = FormulaList (l, r.args());
      }
      else { // l0, r0
	args = FormulaList (l, FormulaList(r));
      }
      *this = Formula (polarity ? OR : AND, args);
      return;
    }

    case IFF: 
    case XOR: {
      Formula l (left());
      l.ennf (true);
      Formula r (right());
      r.ennf (true);
      if (polarity && l == left() && r == right()) { // nothing has changed
        return;
      }
      *this = Formula (polarity == (connective() == IFF) ? IFF : XOR, l, r);
      return;
    }

    case NOT:
      if (polarity && uarg().connective() == ATOM) {
        return;
      }
      *this = uarg ();
      ennf (!polarity);
      return;

    case FORALL:
    case EXISTS: {
      Connective c = polarity == (connective() == FORALL) ? FORALL : EXISTS;
      Formula n (qarg());
      n.ennf(polarity);

      if (n.connective() == c) { // Ex Ey A
        // change into Exy A
	VarList vs (vars());
	vs.append (n.vars());
        *this = Formula (c, vs, n.qarg());
        return;
      }

      // n.connective != c
      if (n == qarg()) { // the formula has not changed
        ASS(polarity);
        return;
      }
      // n.connective != c, n != arg
      *this = Formula (c, vars(), n);
      return;
    }

    default:
      ASS( false );
      return;
  }
} // Formula::ennf


// transform a list of formulas into ennf
// 12/05/2002 Manchester
// 30/08/2002 Torrevieja, changed
void FormulaList::ennf (Formula::Connective c, bool polarity)
{
  ASS( c == Formula::AND || c == Formula::OR );

  if (isEmpty()) {
    return;
  }

  Formula hd (head());
  hd.ennf (polarity);

  FormulaList tl (tail());
  tl.ennf (c, polarity);

  // computing new arguments
  if (hd.connective() == c) { // h1 & ... & hn
    *this = hd.args();
    append (tl);
    return;
  }

  if (hd == head() && tl == tail()) {
    return;
  }

  *this = FormulaList (hd,tl);
} // FormulaList::ennf


// remove equivalence from ennf
// 14/05/2002 Manchester
// 30/08/2002 Torrevieja, changed
// 22/09/2002 Manchester, changed (polarity removed)
void Formula::removeIff ()
{
  switch ( connective() ) {
    case ATOM:
      return;

    case AND:
    case OR: {
      FormulaList fs (args());
      fs.removeIff ();
      if (fs == args()) { // nothing changed
        return;
      }
      *this = Formula (connective(), fs);
      return;
    }

    case IFF: 
    case XOR: {
      Formula l (left());
      l.removeIff();
      Formula r (right());
      r.removeIff();
      if (connective() == IFF) {
        // return (l => r) && (r => l)
	*this = Formula (AND, 
                         FormulaList (Formula (IMP, l, r),
                         FormulaList (Formula (IMP, r, l) ) ) );
      }
      else { // essentially, XOR,
	//  return (l => ~r) & (~l => r)
	*this = Formula (AND, 
			 FormulaList (Formula (IMP, l, Formula(NOT, r)),
				      FormulaList (Formula (IMP, Formula(NOT, l), r) ) ) );
      }
      return;
    }

    case NOT:
      ASS( uarg().connective() == ATOM );
      return;

    case FORALL:
    case EXISTS: {
      Formula n (qarg());
      n.removeIff();
      if (n == qarg()) {
        return;
      }
      *this = Formula (connective(), vars(), n);
      return;
    }

    case IMP:
    default:
      ASS( false );
      return;
  }
} // Formula::removeIff


// remove equivalence from ennf
// 14/05/2002 Manchester
// 30/08/2002 Torrevieja, changed
void FormulaList::removeIff ()
{
  if (isEmpty()) {
    return;
  }

  Formula nf (head());
  nf.removeIff ();
  FormulaList nfs (tail());
  nfs.removeIff ();
  if (nf == head() && nfs == tail()) {
    return;
  }

  *this = FormulaList (nf, nfs);
} // FormulaList::removeIff


// miniscope a flattened nnf
// 21/04/2003 flight Manchester-Frankfurt
void Formula::miniscope (InferenceList& inf)
{
  Position p;
  miniscope (inf,p);
} // Formula::miniscope


// miniscope a flattened nnf
//    pos is the position of the nnf in the original formula
//    inf is the list of inferences collected so far
// 25/05/2002 Manchester
// 15/04/2003 Torrevieja, changed
// 21/04/2003 flight Manchester-Frankfurt, changed to use position and inference
void Formula::miniscope (InferenceList& inf, const Position& pos)
{
  switch ( connective() ) 
    {
    case ATOM:
      return;

    case NOT:
      ASS(uarg().connective() == ATOM);
      return;

    case AND:
    case OR: 
      {
	FormulaList nas (args()); 
	nas.miniscope(connective(), inf, pos, 0, nas);
	if (nas == args()) { // miniscoping did not change arguments
	  return;
	}
        *this = Formula (connective(), nas);
	return;
      }

    case EXISTS: 
    case FORALL: 
      {
	Formula down (qarg());
        Position pdown (0, pos);
	down.miniscope (inf, pdown);
	VarList vs; 
        VarList removed;
	down.occurring (vars(), vs, removed);

        if (! removed.isEmpty()) { // at least one of the variables has been removed
	  // remove quantifier
	  // first, memorise the inference
	  inf.push (Inference (Inference::DUMMY_QUANTIFIER_REMOVAL,
			       *this, // premise
			       pos,
			       removed));
	  
	  if (vs.isEmpty()) { // none of the variables occurs in down
	    *this = down;
	    return;
	  }
	}

	down.topMiniscope(connective(), vs, inf, pos);

	if (! (down == qarg()) ) {  // top-miniscoping worked
	  *this = down;
	  return;
        }
	// top-miniscoping down did not work
        if (! removed.isEmpty()) { // but some dummy variables have been removed
	  // the inference have been recorded before, simply change the formula
	  *this = Formula (connective(), vs, down);
        }
        // variables have changed
        // 
	return;
      }

    case IMP:
    case IFF:
    case XOR:
    default:
      ASS( false );
      return;
  }
} // Formula::miniscope


// set out to the sublist of variables in in occurring in the formula
// 13/04/2003 Torrevieja
// WARNING: the formula must be rectified
void Formula::occurring (const VarList& in, 
			 VarList& out,
			 VarList& removedVars) const
{
  TRACER ("Formula::occurring (const VarList& in, VarList& out) const");
  ASS (in.isNonEmpty());

  // finding maximal variable
  Var max = in.max ();
  bool* occ = new bool [max+1]; // array to store occurrences info
  for (int i = max; i >= 0; i--) {
    occ [i] = false;
  }

  occurring (occ, max);
  splitVarList (in, out, removedVars, occ);

  delete [] occ;
} // Formula::occurring


// set to true in the array occs variables occurring in the formula
// 13/04/2003 Torrevieja
// WARNING: the formula must be rectified
void Formula::occurring (bool* occs, Var max) const
{
  TRACER("Formula::occurring (bool* occs, Var max) const");

  switch ( connective() ) 
    {
    case ATOM:
      atom().occurring (occs, max);
      return;

    case AND:
    case OR: {
      Iterator<Formula> fs (args());
      while (fs.more()) {
	fs.next().occurring(occs, max);
      }
      return;
    }  

    case IMP:
    case IFF:
    case XOR:
      left().occurring(occs, max);
      right().occurring(occs, max);
      return;

    case NOT:
      uarg().occurring(occs, max);
      return;

    case FORALL:
    case EXISTS:
      qarg().occurring(occs, max);
      return;

    default:
      ASS( false );
    }
} // Formula::occurring


// The input list is flattened nnf A1 & ... & Ak
// This list is part of a list B1 & ... & Bm & A1 & ... & Ak
// Miniscope each nnf in the list A1 ... Ak. If any formula has the
//     connective c, then flatten the list
// The arguments are as follows:
//   c is the connective (for example, &)
//   inf is a list of inferences collected so far
//   pos is the position of the subformula B1 & ... & Ak in the miniscoped
//       formula
//   argNumber is m+1
//   firstArg is B1 & ... & Ak
//
// 26/05/2002 Manchester
// 12/04/2003 Torrevieja, completely changed
// 21/04/2003 flight Manchester-Frankfurt, changed to add inference-related
//            arguments
void FormulaList::miniscope (Formula::Connective c, 
			     InferenceList& inf, 
			     const Position& pos, 
			     int argNumber,
			     const FormulaList& firstArg)
{
  if (isEmpty ()) {
    return;
  }

  Formula g (head());
  // the formula must be flattened before
  ASS (g.connective() != c);

  g.miniscope (inf, Position(argNumber,pos));
  FormulaList gs (tail()); 
  gs.miniscope(c, inf, pos, argNumber+1, firstArg);

  if (g == head() && gs == tail()) { // nothing has changed
    return;
  }

  if (g.connective() == c) { // g has the same connective
    // must be flattened

    // compute the premise


    FormulaList args;
    appendN (firstArg, *this, argNumber, args);
    
    inf.push (Inference (Inference::FLATTEN,
			 Formula (c, args), // premise
			 pos,
			 argNumber));
    *this = g.args();
    append(gs);
			 
    return;
  }

  // the connective is not c
  *this = FormulaList (g,gs);
} // FormulaList::miniscope/5


// the formula is the subformula F of (Q vs)F and is miniscoped
// this function tries to push the quantifier (Q vs) inside the formula
// pos is the position of (Q vs)F
// 26/05/2002 Manchester
// 13/04/2003 Torrevieja, changed considerably
// 28/04/2003 Manchester, inference information added
void Formula::topMiniscope (Connective Q, 
			    const VarList& vs, 
			    InferenceList& inf, 
			    const Position& pos)
{
  TRACER("Formula::topMiniscope");

  switch ( connective() ) 
    {
    case ATOM: // nothing to miniscope
    case NOT:
      return;
      
    case AND:
    case OR: 
      {
	TRACER("Formula::topMiniscope/AndOr");

	if ( (connective() == AND) == (Q == FORALL) ) { 
          TRACER("Formula::topMiniscope/ForallAnd");

	  // forall X (F1 & ... & Fn) or
	  // exists X (F1 \/ ... \/ Fn)
	  inf.push (Inference(Inference::FORALL_AND_MINISCOPE,
			      Formula(Q, vs, *this),
			      pos));
	  FormulaList as (args());
	  as.topMiniscope(Q, vs, inf, pos, 0);
	  *this = Formula (connective(), as);
	  return;
	}

	{TRACER("Formula::topMiniscope/ForallOr");
	// forall x1 ... xm (F1 \/ ... \/ Fn) or
	// exists x1 ... xm (F1 & ... & Fn)
	// create a miniscope class. It counts the number of occurrences of xi in Fj
	Miniscope cnt (Q, vs, *this, pos);
	if ( cnt.noMiniscoping() ) {
	  return;
	}

        cnt.pushOnes (inf);

	// no more variables with occurrences to just one formula
        while (cnt.pushMany (inf)) ;
	Formula res;
	cnt.result (res);
	*this = res;
	return;
	}}

    case FORALL:
    case EXISTS: 
      if (connective() == Q) { 
	TRACER("Formula::topMiniscope/ForallForall");
	// this is a rare case, it can only happen when in 
	// (forall X)(exists Y)(forall Z)F Y did not occur in (forall Z)F
	// or in (forall X)(forall Z)F the quantifier (forall X) was miniscoped

	Formula f (qarg());
	inf.push (Inference (Inference::SWAP, 
			     Formula(Q,vs,*this),
			     pos));
        Position pdown (0, pos);
        f.topMiniscope (Q,vs,inf,pdown);
	if (f == qarg()) { // top-miniscoping f did not work
	  TRACER("Formula::topMiniscope/f == qarg");
          const InferenceList i (inf.tail());
	  inf = i;
	  // inf.pop ();
	  {TRACER("Formula::topMiniscope/01");
	  Inference fl (Inference::FLATTEN, 
			Formula(Q,vs,*this),
			pos, 
			0);
	  {TRACER("Formula::topMiniscope/011");
	  inf.push(fl);
	  {TRACER("Formula::topMiniscope/02");
	  VarList newvars (vs);
	  newvars.append (vars());
	  {TRACER("Formula::topMiniscope/03");
	  *this = Formula (Q, newvars, f);
	  return;
	  }}}}}
	// *******************88
	// top-miniscoping f worked
	if (f.connective() == Q) {
	  TRACER("Formula::topMiniscope/f.connective == Q");
	  inf.push (Inference (Inference::FLATTEN, 
			       Formula(Q,vars(),*this),
			       pos, 
			       0));

	  VarList newvars (vars());
	  newvars.append (f.vars());
	  *this = Formula (Q, newvars, f.qarg());
	  return;
	}
	// miniscoping arg worked, f.connective != Q
	*this = Formula (Q, vars(), f);
	return;
      }
      // (forall X)(exists Y)F 
      return;

    case IMP:
    case IFF:
    case XOR:
    default:
      ASS( false );
      return;
    }
} // Formula::topMiniscope


// the input list a junction of miniscoped formulas, e.g. F1 & ... & Fn
// the list is set to the miniscoped version of (Q X)F1 & ... & (Q X)Fn
// 26/05/2002 Manchester
// 13/04/2003 Torrevieja, changed
// 29/04/2003 Manchester, inference-related arguments added
void FormulaList::topMiniscope (Formula::Connective Q, 
				const VarList& X,
				InferenceList& inf,
				const Position& pos,
				int index)
{
  TRACER("FormulaList::topMiniscope");

  if (isEmpty()) {
    return;
  }

  // the list is non-empty
  FormulaList gs (tail());
  gs.topMiniscope (Q, X, inf, pos, index+1);

  VarList V;
  VarList removed;
  head().occurring (X, V, removed);
  if (! removed.isEmpty()) {
    inf.push( Inference (Inference::DUMMY_QUANTIFIER_REMOVAL,
			 Formula (Q, X, head()),
			 Position (index, pos),
			 removed) );
  }

  // now V is the sublist of X consisting of variables occurring in head
  if (V.isEmpty()) { // V does not occur in F1, simply make the first formula F1
    *this = FormulaList (head(), gs);
    return;
  }

  // V is non-empty
  Formula f (head()); 
  f.topMiniscope(Q, V, inf, Position (index,pos));

  if (f == head()) { // top-miniscoping f did not work
    // simply make the first formula (Q V)F1
    Formula h (Q,V,f);
    *this = FormulaList (h, gs);
    return;
  }

  // top-miniscoping f worked
  *this = FormulaList (f,gs);
} // FormulaList::topMiniscope


// flatten the formula
// 30/08/2002 Torrevieja, return type changed to void
void Formula::flatten ()
{
  switch ( connective() ) {
  case ATOM:
    return;

  case AND:
  case OR: {
    FormulaList flattenedArgs (args());
    flattenedArgs.flatten (connective());
    if ( flattenedArgs == args() ) { 
      return;
    }
    *this = Formula (connective(), flattenedArgs);
    return;
    }

  case IMP:
  case IFF:
  case XOR: {
    Formula flattenedLeft (left());
    flattenedLeft.flatten();
    Formula flattenedRight (right());
    flattenedRight.flatten();
    if (flattenedLeft == left() && flattenedRight == right()) {
      return;
    }
    *this = Formula (connective(), flattenedLeft, flattenedRight);
    return;
    }
    
  case NOT: {
    Formula flattenedArg (uarg());
    flattenedArg.flatten();
    if (flattenedArg == uarg()) {
      return;
    }
    *this = Formula (NOT, flattenedArg);
    return;
  }
    
  case FORALL:
  case EXISTS: {
    Formula flattenedArg (qarg());
    flattenedArg.flatten();
    if (flattenedArg.connective() == connective()) {
      // needs more flattening, concatenate variables
      VarList vs (vars());
      vs.append (flattenedArg.vars());
      *this = Formula (connective(), vs, flattenedArg.qarg());
      return;
      }
    // no more flattening
    if (flattenedArg == qarg()) {
      return;
    }
    *this = Formula (connective(), vars(), flattenedArg);
    return;
  }

  default:
    ASS( false );
    return;
  }
} // Formula::flatten ()


// flatten the list of formulas (connected by c)
// 27/06/2002 Manchester
// 30/08/2002 Torrevieja, return type changed to void
void FormulaList::flatten (Formula::Connective c)
{
  if (isEmpty()) {
    return;
  }
  Formula flattenedHead (head()); 
  flattenedHead.flatten();
  FormulaList flattenedTail (tail());
  flattenedTail.flatten(c);

  if (flattenedHead.connective() == c) { // needs further flattening
    *this = flattenedHead.args();
    append(flattenedTail);
    return;
  }
  // needs no further flattening
  if (flattenedHead == head() && flattenedTail == tail()) {
    return;
  }
  *this = FormulaList (flattenedHead, flattenedTail);
} // FormulaList::flatten


// 28/06/2002 Manchester
// 05/09/2002 Trento, changed
void Formula::skolemizeNNF ()
{
  Substitution subst;

  VarList vs;
  skolemizeNNF (subst, vs);
} // Formula::skolemizeNNF


// 28/06/2002 Manchester
// 04/09/2002 Bolzano, changed
// 05/09/2002 Trento, changed
void Formula::skolemizeNNF (Substitution& subst, VarList vs)
{
  switch ( connective() ) {
  case ATOM: {
    Atom a (atom());
    a.apply (subst);
    if (a == atom()) {
      return;
    }
    *this = Formula(a);
    return;
  }

  case AND:
  case OR: {
    FormulaList fs (args());
    fs.skolemizeNNF(subst, vs);
    if (fs == args()) {
      return;
    }
    *this = Formula (connective(), fs);
    return;
  }

  case NOT: {
    ASS( uarg().connective() == ATOM);
    Atom a (uarg().atom());
    a.apply(subst);
    if (a == uarg().atom()) {
      return;
    }
    *this = Formula (NOT, Formula(a));
    return;
  }

  case FORALL: {
    VarList newVars (vars());
    newVars.append(vs);
    Formula f (qarg());
    f.skolemizeNNF (subst, newVars);
    *this = f;
    return;
    }

  case EXISTS: {
    // add skolem functions to the substitution
    TermList skolemArgs;
    skolemArgs.buildFrom (vs); 
    int noOfArgs = vs.length (); 
    Iterator<Var> newSubstVars (vars()); 
    while (newSubstVars.more()) { 
      subst.bind ( newSubstVars.next (), 
		   Term ( sig->newSkolemFunction (noOfArgs), 
			  skolemArgs ) ); 
    }
    // skolemize the argument 
    Formula f (qarg());
    f.skolemizeNNF (subst, vs);
    // restore the substitution
    Iterator<Var> ws (vars());
    while (ws.more()) {
      subst.undoBinding (ws.next());
    }
    *this = f;
    return;
    }

  case IMP:
  case IFF:
  case XOR:
  default:
    ASS( false );
    return;
  }
} // Formula::skolemizeNNF


// 28/06/2002 Manchester
// 04/09/2002 Bolzano, changed
void FormulaList::skolemizeNNF (Substitution& subst, VarList skolemArgs) 
{
  if (isEmpty()) {
    return;
  }

  Formula h (head());
  h.skolemizeNNF(subst, skolemArgs);

  FormulaList t (tail());
  t.skolemizeNNF(subst, skolemArgs);

  if (h == head() && t == tail()) {
    return;
  }

  *this = FormulaList(h,t);
} // Formula::skolemizeNNF


// count the number of universal outermost quantifers in the formula
int Formula::universalPrefixLength () const
{
  int result = 0;

  Formula f (*this);
  while (f.connective() == FORALL) {
    result += f.vars().length();
    f = f.qarg ();
  }

  return result;
} // Formula::universalPrefixLength ()


// A "constructor" for and-or formulas
// 25/09/2002 Manchester
void Formula::makeJunction (Connective c, Formula& lhs, Formula& rhs)
{
  if (lhs.connective() == c) {
    FormulaList largs (lhs.args());
    if (rhs.connective() == c) {
      largs.append (rhs.args());
    }
    else { // only lhs has c as the main connective
      largs.append (FormulaList(rhs));
    }
    *this = Formula (c, largs);
    return;
  }

  // lhs' connective is not c
  if (rhs.connective() == c) {
    *this = Formula (c, FormulaList(lhs,rhs.args()));
    return;
  }
  // both connectives are not c
  *this = Formula (c, 
		   FormulaList (lhs,
		   FormulaList (rhs)));
  return;
} // Formula::makeJunction


// make out into the sublist of variables v in in
// for which occurrences[v] = true
// 29/04/2003 Manchester, made from makeSublist
void Formula::splitVarList (const VarList& in, 
			    VarList& out, 
			    VarList& removed,
			    bool* occurrences)
{
  if (in.isEmpty()) {
    return;
  }

  // in is non-empty
  splitVarList (in.tail(), out, removed, occurrences);

  Var v = in.head();
  if (occurrences[v]) { // v must be included in out
    if (out == in.tail()) { // all variables so far must be included
      out = in;
      return;
    }
    // vs != in.tail
    out.push(v);
    return;
  }
  // v must not be included in out
  removed.push(v);
} // Formula::splitVarList


// result := the list obtained by appending the first N elements of
// fst to snd
// 04/05/2003 Manchester
void FormulaList::appendN (const FormulaList& fst, 
			   const FormulaList& snd,
			   int N,
			   FormulaList& result)
{
  ASS (N >= 0);

  if (N == 0) {
    result = snd;
    return;
  }

  // N > 0
  appendN (fst.tail(), snd, N-1, result);
  result.push (fst.head());
} // FormulaList::appendN


/*
  THIS IS USEFUL
  switch ( connective() ) 
    {
    case ATOM:
    case AND:
    case OR:
    case IMP:
    case IFF:
    case XOR:
    case NOT:
    case FORALL:
    case EXISTS:
    default:
      ASS( false );
    }

  switch ( ->connective() ) {
  case Formula::ATOM:
  case Formula::AND:
  case Formula::OR:
  case Formula::IMP:
  case Formula::IFF:
  case Formula::XOR:
  case Formula::NOT:
  case Formula::FORALL:
  case Formula::EXISTS:
  default:
    ASS (false);
  }
*/

