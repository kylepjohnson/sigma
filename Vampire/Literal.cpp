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
//  file Literal.cpp 
//  implements class Literal
//


#include "Literal.hpp"
#include "Substitution.hpp"


// build literal from a kernel literal
// 28/09/2002 Manchester
Literal::Literal (const VampireKernel::Literal* literal, 
		  const VampireKernel& kernel)
{
  TermList ts (literal->firstArgument (), kernel);
  ulong sym = literal->predicate();
  Atom a (sig->createPred (kernel.symbolPrintName (sym),
			   kernel.symbolArity(sym)),
	  ts);

  _data = new Data (literal->isPositive(), a);
} // Literal::Literal (const VampireKernel::Literal* lit, ...)


// build literal list from a kernel literal list
// 28/09/2002 Manchester
LiteralList::LiteralList (const VampireKernel::Literal* literal, 
		  const VampireKernel& kernel)
{
  if (! literal) {
    _data = 0;
    return;
  }

  _data = new LstData<Literal> ( Literal (literal, kernel),
		     LiteralList (literal->next(), kernel) );
} // Literal::Literal (const VampireKernel::Literal* lit, ...)


// assignment operator
// 25/08/2002 Torrevieja
void Literal::operator = (const Literal& t)
{
  if (t._data) {
    t._data->ref ();
  }

  if (_data) {
    _data->deref ();
  }

  _data = t._data;
} // Literal::operator =


// comparison of two literals, needed to normalize
// 26/06/2002 Manchester
Compare Literal::compare (Literal l) const  
{
  // negative literals are less than positive
  if ( negative() ) {
    if ( l.positive() )
      return LESS;
  }
  else { // positive
    if ( l.negative() )
      return GREATER;
  }

  // polarities are equal
  return atom().compare(l.atom());
} // Literal::compare


// comparison of two literals, needed to normalize
// 26/06/2002 Manchester
bool Literal::isless (Literal l) const  
{
  switch ( compare(l) ) {
  case LESS:
    return true;

  case EQUAL:
  case GREATER:
    return false;
  }

  ASS(false);
} // Literal::isless


// normalization
// 10/09/2002, Manchester
void Literal::normalize ()
{ 
  Atom a (atom());
  a.normalize();
  if (a == atom()) {
    return;
  }

  *this = Literal (sign(),a);
} // Literal::normalize ()


// the list is a renaming of lst (up to a permutation)
// 03/10/2002 Manchester
bool LiteralList::isRenamingOf (LiteralList lst) const
{
  // easy check first
  if (length() != lst.length()) {
    return false;
  }

  Substitution sbst;
  LiteralList empty;

  return isRenamingOf (empty, lst, sbst);
} // LiteralList::isRenamingOf


// the list is a renaming of (tried U remaining) up to a permutation
// but the first literal should match a literal in remaining
// sbst is a previously computed substitution
// 03/10/2002 Manchester
bool LiteralList::isRenamingOf (LiteralList tried, 
				LiteralList remaining, 
				Substitution sbst) const
{
  if (isEmpty()) {
    return tried.isEmpty() &&
           remaining.isEmpty();
  }

  while (remaining.isNonEmpty()) {
    // try to match the first literal
    // copy sbst, or else it may be spoiled by the first match
    Substitution s (sbst);
    if ( head().isRenamingOf(remaining.head(), s) ) {
      LiteralList all (tried);
      all.append (remaining.tail());
      LiteralList empty;
      if ( tail().isRenamingOf(empty, all, s) ) {
	return true;
      }
    }
    // the match of the first literal did not help, try the rest
    tried.push (remaining.head());
    remaining.pop();
  }

  // remaining is empty
  return false;
} // LiteralList::isRenamingOf


// the literal is a renaming of l
// sbst is a previously computed substitution
// 03/10/2002 Manchester
bool Literal::isRenamingOf (Literal l, Substitution& sbst) const
{
  return sign() == l.sign() &&
         atom().isRenamingOf(l.atom(), sbst);
} // Literal::isRenamingOf


/*
// replace the literal
//   f(X,y,Z) != f(X,u,Z)
// by 
//   i(f(X,y,Z),X,Z) = y
void Literal::introduceInverse ( int argNo, FSymbol* inv )
{
  // make literal into positive
  _sign = true;

  atom()->introduceInverse (argNo, inv);
} // Literal::introduceInverse


void Literal::splitLargeArity (const PSymbol* p, FSymbol::List* newFs )
{
  atom()->splitLargeArity (p, newFs);
} // Literal::splitLargeArity


void Literal::splitLargeArity (const FSymbol* f, FSymbol::List* newFs )
{
  atom()->splitLargeArity (f, newFs);
} // Literal::splitLargeArity


Literal* Literal::negativeEqualitySplit ( int thresholdWeight )
{
  if ( positive() ) // only negative equalities can be split
    return 0;

  Atom* a = atom()->negativeEqualitySplit (thresholdWeight);

  return a ? new Literal ( true, a ) : 0 ; 
} // Literal::negativeEqualitySplit
*/
