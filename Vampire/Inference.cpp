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
//  file Inference.cpp
//  implements class Inference
//  started 17/04/2003, flight Barcelona-Manchester
//


#include "Inference.hpp"
#include "Position.hpp"
#include "Formula.hpp"


// ********************* Inference::DFormula *******************
//   Represents any inference whose premise is a formula
// *************************************************************

class Inference::DFormula
  : public Data
{
 public:
  DFormula (Rule rule, const Formula& premise);
  const Formula& premise () const;

 protected:
  // structure
  Formula _premise;
}; // class Inference::DFormula


inline
Inference::DFormula::DFormula (Rule r, const Formula& premise)
  :
  Data (r),
  _premise (premise)
{
} // Inference::DFormula::DFomrula


// ***************** Inference::ForallOrMiniscope ****************
//   Represents an inference
//
//             (! X Y) (A1 \/ ... \/ Ak) 
//          ----------------------------------------------------------
//           (! X) (A1 \/ ... \/ (! Y) (Ai \/ ... \/ Aj) \/ ... \/ Ak)
//
//   or a similar inference with ! replaced by ? and \/ by &
//   
//   Stores the following information:
//     _position is the position of this subformula
//     _toppledVarPositions are the positions of variables in Y
//       in the quantifier at the premise
//     _toppledSubformulaPositions is the list [i ... j]
//     _toppledSubformulaIndex is the index of (! Y) (Ai \/ ... \/ Aj)
//       in the disjunction at the conclusion
// ***************************************************************

class Inference::ForallOrMiniscope
  : public DFormula
#   if DEBUG_PREPRO
    , public Memory <CID_FORALL_OR_MINISCOPE>
#   endif
{
 public:
  ForallOrMiniscope (Rule r, 
		     const Formula& premise,
		     const Position& p,
		     IntList toppledVarPositions,
		     IntList toppledSubformulaPositions,
		     int toppledSubformulaIndex);
  const Position& position () const;

 private:
  // structure
  Position _position;
  IntList _toppledVarPositions;
  IntList _toppledSubformulaPositions;
  int toppledSubformulaIndex;
}; // Inference::ForallOrMiniscope


Inference::ForallOrMiniscope::ForallOrMiniscope (Rule r,
						 const Formula& premise,
						 const Position& p,
						 IntList toppledVarPositions,
						 IntList toppledSubformulaPositions,
						 int toppledSubformulaIndex)
  :
  DFormula (r, premise),
  _position (p),
  _toppledVarPositions (toppledVarPositions),
  _toppledSubformulaPositions (toppledSubformulaPositions)
{
  ASS (r == FORALL_OR_MINISCOPE);
} // Inference::ForallOrMiniscope::ForallOrMiniscope


// ************** Inference::DummyQuantifierRemoval **************
//   Represents an inference
//
//             (! X Y) A
//            -----------
//              (! X) A
//
//   or a similar inference with ! replaced by ?
//   
//   Stores the following information:
//     _position is the position of this subformula
//     _removedVars is the list of variables Y
// ***************************************************************

class Inference::DummyQuantifierRemoval
: public DFormula
#   if DEBUG_PREPRO
    , public Memory <CID_DUMMY_QUANTIFIER_REMOVAL>
#   endif
{
 public:
  DummyQuantifierRemoval (Rule r, 
			  const Formula& premise,
			  const Position& p, 
			  const VarList& removedVars);
  const Position& position () const;

 private:
  // structure
  Position _position;
  VarList _removedVars;
}; // Inference::DummyQuantifierRemoval


// 03/05/2003 Manchester
Inference::DummyQuantifierRemoval::DummyQuantifierRemoval (Rule r, 
							   const Formula& premise,
							   const Position& p, 
							   const VarList& removedVars)
  : 
  DFormula (r, premise),
  _removedVars (removedVars)
{
  ASS (r == DUMMY_QUANTIFIER_REMOVAL);
} // Inference::DummyQuantifierRemoval::DummyQuantifierRemoval


// ********************* Inference::Flatten *********************
//   Represents one of the following inferences
//
//             (! X)(! Y) A 
//            --------------
//              (! X) A
//
//             A0 & ... & Ak-1 & (B1 & ... & Bm) & A_k & ... & An
//            ----------------------------------------------------
//              A0 & ... & Ak-1 & B1 & ... & Bm & A_k & ... & An
//
//   or similar inferences with ! or & replaced by ? or \/
//   
//   Stores the following information:
//     _position is the position of this subformula
//     _subformulaIndex = k (0 if the first kind of inference)
// ***************************************************************

class Inference::Flatten
: public DFormula
#   if DEBUG_PREPRO
    , public Memory <CID_FLATTEN>
#   endif
{
 public:
  Flatten (Rule r, 
	   const Formula& premise,
	   const Position& p,
	   int subformulaIndex);
  const Position& position () const;

 private:
  // structure
  Position _position;
  int _subformulaIndex;
}; // Inference::Flatten


// 03/05/2003 Manchester
Inference::Flatten::Flatten (Rule r, 
			     const Formula& premise,
			     const Position& p,
			     int subformulaIndex)
  :
  DFormula (r, premise),
  _position (p),
  _subformulaIndex (subformulaIndex)
{
  ASS (r == FLATTEN);
} // Flatten::Flatten


// ********************** Inference::Type1 **********************
//   Represents the following types of inferences
//  
//     (a) Swap:
//
//             (! X)(! Y) A 
//            --------------
//             (! Y)(! X) A
//
//         or similar inferences with ! replaced by ? 
//
//     (b) Forall-And-Miniscope
//
//             (! X) (A1 & ... & Ak) 
//          --------------------------
//           (! X) A1 & ... & (! X) Ak 
//
//         or a similar inference with ! replaced by ? and & by \/
//   
//   Stores the following information:

//   
//   Stores the following information:
//     _position is the position of this subformula
// ***************************************************************


class Inference::Type1
: public DFormula
#   if DEBUG_PREPRO
    , public Memory <CID_INFERENCE_TYPE1>
#   endif
{
 public:
  Type1 (Rule r, 
	const Formula& premise,
	const Position& p);
  const Position& position () const;

 private:
  // structure
  Position _position;
}; // Inference::Type1


Inference::Type1::Type1 (Rule r, const Formula& premise, const Position& p)
  :
  DFormula (r, premise),
  _position (p)
{
  ASS (r == FORALL_AND_MINISCOPE || r == SWAP);
} // Inference::Type1::Type1


// ****************** Inference::Data ********************************

// 17/04/2003, flight Barcelona-Manchester
// 03/05/2003 Manchester changed
void Inference::Data::destroy ()
{ 
  TRACER ("Inference::Data::destroy");

  ASS (this);
  ASS (_counter == 0);

  switch (rule()) 
    {
    case FORALL_AND_MINISCOPE:
    case SWAP:
      delete static_cast<Type1*>(this);
      return;

    case FORALL_OR_MINISCOPE:
      delete static_cast<ForallOrMiniscope*>(this);
      return;

    case DUMMY_QUANTIFIER_REMOVAL:
      delete static_cast<DummyQuantifierRemoval*>(this);
      return;

    case FLATTEN:
      delete static_cast<Flatten*>(this);
      return;

    default:
      ASS (false);
    }
} // Inference::Data::destroy ()


// ******************* Class Inference ************************

// 03/05/2003 Manchester
Inference::Inference (Rule r, 
		      const Formula& premise, 
		      const Position& p,
		      const VarList& removedVars)
  :
  _data (new DummyQuantifierRemoval (r, premise, p, removedVars))
{
  ASS (r == DUMMY_QUANTIFIER_REMOVAL);
} // Inference::Inference


// 03/05/2003 Manchester
Inference::Inference (Rule r, 
		      const Formula& premise, 
		      const Position& p)
  :
  _data (new Type1 (r, premise, p))
{
  ASS (r == FORALL_AND_MINISCOPE || r == SWAP);
} // Inference::Inference


// 03/05/2003 Manchester
Inference::Inference (Rule r, 
		      const Formula& premise,
		      const Position& p,
		      const IntList& toppledVarPositions,
		      const IntList& toppledSubformulaPositions,
		      int toppledSubformulaIndex)
  :
  _data (new ForallOrMiniscope (r, premise, p, 
				toppledVarPositions,
				toppledSubformulaPositions,
				toppledSubformulaIndex))
{
  ASS (r == FORALL_OR_MINISCOPE);
} // Inference::Inference


// 03/05/2003 Manchester
Inference::Inference (Rule r, 
		      const Formula& premise,
		      const Position& p,
		      int subformulaIndex)
  :
  _data (new Flatten (r, premise, p, subformulaIndex))
{
  ASS (r == FLATTEN);
} // Inference::Inference


