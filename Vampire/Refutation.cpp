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
//  file Refutation.cpp
//  
//  implements class Refutation
//


#include "Refutation.hpp"
#include "Problem.hpp"
#include "IntNameTable.hpp"
#include "Unit.hpp"
#include "Map.hpp"


class Refutation::Map 
  : public ::Map <int, Unit>
{
}; // class Refutation::Map


Refutation::Refutation (const VampireKernel& kernel, const Problem& problem)
  : _kernel (kernel),
    _problem(problem)
{
  TRACER ("Refutation::Refutation");
 
  Map map;

  _goal = collect(kernel.refutation(), map);
} // Refutation::Refutation


// build the refutation rooting at the returned unit
// 29/09/2002 Manchester
// 04/10/2002 Manchester changed and simplified
Unit Refutation::collect (const VampireKernel::Clause* clause, Map& map)
{
  TRACER ("Refutation::collect (VampireKernel::Clause ...)");

  int number = clause->number();

  Unit unit;
  if ( map.find(number, unit) ) { // clause number is already in the map
    return unit;
  }

  // the number of the clause was NOT in the map
  if (clause->isInputClause()) { // the clause is an input clause 
    // the clause should not be added to the refutation but his parent unit should
    Unit parent (clause->inputClauseOrigin());
    return parent;
  }

  // the clause is not an input clause
  // collect all parents of the clause
  // and create the list of parent units
  UnitList parents = collect (clause->firstAncestor(), map);

  // create the unit itself
  Clause cls (clause, _kernel);
  Unit result (cls, parents);
  map.insert (number, result);

  return result;
} //  Refutation::collect (VampireKernel::Clause* ...)


// build a list of units representing a list of refutations refutations
// 04/10/2002 Manchester
UnitList Refutation::collect (const VampireKernel::Clause::Ancestor* parent, Map& map)
{
  TRACER ("Refutation::collect (VampireKernel::Clause::Ancestor* ...)");

  if (! parent) {
    return UnitList ();
  }

  // the list of parents is non-empty
  const VampireKernel::Clause* clause = parent->clause();
  const VampireKernel::Clause::Ancestor* rest = parent->next ();
  if (clause->isBuiltInTheoryFact()) {
    return collect (rest, map);
  }

  // not built-in theory fact
  Unit u (collect (clause, map));
  return UnitList (u, collect (rest, map));
} //  Refutation::collect (VampireKernel::Clause* ...)


// condense the refutation by removing all single-parent inferences
// 04/10/2002 Manchester
Unit Refutation::condense () const
{
  Map condensed;

  return condense (_goal, condensed);
} // Refutation::condense ()


// condense the refutation by removing all single-parent inferences
// 04/10/2002 Manchester
Unit Refutation::condense (Unit root, Map& prev) const
{
  if ( prev.find (root.number(), root) ) { // already condensed
    return root;
  }

  // not condensed yet
  UnitList condensedParents = condense (root.parents(), prev);
  if ( condensedParents.isNonEmpty() && 
       condensedParents.tail().isEmpty() ) { // one parent, case for condensing
    Unit parent ( condensedParents.head() ); // the single parent
    if ( parent.unitType() == FORMULA &&
         root.unitType() == CLAUSE ) { // clause obtained from formula
      // don't condense any more
      if ( condensedParents == root.parents() ) { // the parent was not in fact condensed
	prev.insert (root.number(), root);
	return root;
      }
      // the parent was condensed
      Unit result (root.clause(), condensedParents);
      prev.insert (root.number(), result);
      return result;      
    }
    // both the parent and the root have the same unit type, unconditionally condense
    // root to parent
    prev.insert (root.number(), parent);
    return parent;
  }

  // several parents, do not condense
  if ( condensedParents == root.parents() ) { // no change at all
    prev.insert (root.number(), root);
    return root;
  }

  // parents have change, so return a new unit
  if (root.unitType() == CLAUSE) {
    Unit result (root.clause(), condensedParents);
    prev.insert (root.number(), result);
    return result;      
  }
  // unit contains a formula
  Unit result (root.formula(), condensedParents);
  prev.insert (root.number(), result);
  return result;      
} // Refutation::condense (... Unit ...)


// condense a list of refutations by removing all single-parent inferences
// 04/10/2002 Manchester
UnitList Refutation::condense (UnitList units, Map& prev) const
{
  if (units.isEmpty()) {
    return units;
  }

  Unit first (units.head());
  UnitList rest (units.tail());
  Unit f ( condense (first, prev) );
  UnitList r ( condense (rest, prev) );

  if (f == first && r == rest) { // nothing has changed
    return units;
  }

  return UnitList (f, r);
} // Refutation::condense (... UnitList units ...)


/*
// BlodKorv      15,361
// Gematogen     10,559
// VampireKernel 72,429
// preprocessor  23,729
// --------------------
//             

*/
