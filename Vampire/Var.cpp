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
//  file Var.cpp
//  
//  05/09/2002, Trento
//


#include "Var.hpp"
#include "Substitution.hpp"


// one of the auxiliary rectification functions
// 05/09/2002 Trento, changed from a function on formulas
void VarList::rectify (VarList vs, Substitution& subst, Var& last)
{
  TRACER ("Formula::rectifyVars");

  if (vs.isEmpty()) {
    return;
  }

  Var v = vs.head();
  Var newV = ++last;
  subst.bind (v, Term(newV));
  VarList tl;
  tl.rectify (vs.tail(), subst, last);
  *this = VarList (newV, tl);
} // VarList::rectify


// return maximal variable in a non-empty list
// 13/04/2003 Torrevieja
Var VarList::max () const
{
  TRACER("VarList::max");

  ASS (isNonEmpty());

  Var m = head ();

  Iterator<Var> vs (tail());
  while (vs.more()) {
    Var v = vs.next();
    if (v > m) {
      m = v;
    }
  }

  return m;
} // VarList::max ()

