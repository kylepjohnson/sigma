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
//  file Refutation.hpp
//  implements output of refutations
//  


#ifndef __Refutation__
#  define __Refutation__


// class ostream;
class Output;


#include "List.hpp"
#include "Unit.hpp"
#include "VampireKernel.hpp"


class Refutation
{
 public:
  // constructors/destructors
  Refutation (const VampireKernel& kernel, 
	      const Problem& problem);
  Unit condense () const; // return refutation in condensed form
  const VampireKernel& kernel () const;
  const Problem& problem () const;

 private:
  class Map; // defined in Refutation.cpp

  const VampireKernel& _kernel;
  const Problem& _problem;
  Unit _goal;

  Unit collect (const VampireKernel::Clause*, Map& map);
  UnitList collect (const VampireKernel::Clause::Ancestor* ancestors, Map& map);
  Unit condense (Unit root, Map& condenced) const; 
  UnitList condense (UnitList units, Map& condensed) const; 
}; // class Refutation


// **************** class Refutation, implementation ***********************


inline
const VampireKernel& Refutation::kernel () const
{
  return _kernel;
} // Refutation::kernel


inline
const Problem& Refutation::problem () const
{
  return _problem;
} // Refutation::problem


#endif // __Refutation__
