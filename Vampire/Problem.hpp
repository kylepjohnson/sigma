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
//  file problem.h 
//  defines class Problem
//


#ifndef __Problem__
#define __Problem__


#include "Unit.hpp"


class FSymbol;
class Options;


// ****************** class Problem, definition *******************


class Problem {
 public:
  // constructors & destructors
  explicit Problem ( const UnitList& );
  ~Problem ();
  void preprocess (Options&);

  // query the structure
  UnitChain& units () { return _units; }

  // properties
  bool hasFormulas () const { return _hasFormulas; }

  // miscellaneous
  int removeUnusedDefinitions (); // the result is the number of removed definitions
  int removeEqualityAxioms ();
  int split ();
  void ennf ();
  int removeIff ();
  int miniscope ();
  void flatten ();
  void rectify ();
  void skolemize (); 
  void clausify ();
  void addUnit (Unit u); // for incremental building
  void removeAllConjectures (); // for work with KIF
  const VarList& originalAnswerVariables () const
    { return _originalAnswerVariables; }
  const Literal& answerLiteral () const { return _answerLiteral; }

 private:

  // structure
  UnitChain _units;
  bool _hasFormulas;
  VarList _originalAnswerVariables;
  Literal _answerLiteral;

  // auxiliary functions
  bool hasFreeVars (Formula&) const;
  int findDefinitions ();
  void clausify (FormulaList formulas, 
		 LiteralList literals,
		 Unit parent);
};


#endif // __Problem__
