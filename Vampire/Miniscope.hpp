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
//  file Miniscope.hpp
//  defines class Miniscope for miniscoping
//  Torrevieja, April 2003
//

#ifndef __Miniscope__
#define __Miniscope__


#include "Position.hpp"
#include "Formula.hpp"


class Miniscope 
{
 public:
  // constructor, destructor
  Miniscope (Formula::Connective Q, 
	     const VarList& vars, 
	     const Formula&,
	     const Position& pos);
  ~Miniscope ();

  bool noMiniscoping () const; // no miniscoping is possible
  void pushOnes (InferenceList&);
  bool pushMany (InferenceList&);
  void result (Formula& res); // set res to result

 private:
  Formula::Connective _quant; //
  const Formula& _formula;
  const int _nfs; // number of formulas
  int _realNfs;   // number of non-deleted formulas
  int _nvs;       // number of variables
  bool** _occ;    // array or arrays to allocate occurrences
  bool* _mem;     // memory for storing _occ[i]
  bool** _index;   // for every variable, give its allocation in the array, needed only in the constructor
  Var* _vars;     // array of variables
  int* _N;        // N[i] is the number of formulas among Fj in which vi occurs
  Var _maxV;      // maximal variable in vs
  Formula* _formulas; // formulas in the list, 0 if deleted
  Position _position;

  void count (const Formula&, int formNumber);
  void count (const Term&, int formNumber);
  void pushOne (Var v, int k, InferenceList&);
  void occurInSameFormulas (int v, IntList& vs) const; 
  bool occurInSameFormulas (int v, int w) const; 
  void decodeVars (const IntList& in, VarList& out) const;
  void decodeFormulas (const IntList& in, IntList& out) const;
  void decodeFormulas (const IntList& in, IntList& out, 
		       int inIndex, int outIndex) const;
  void deleteVars (const IntList& vs);
  void partition (const FormulaList& F, Var v, FormulaList& G, FormulaList& H);
}; // class Miniscope


#endif // __Miniscope__
