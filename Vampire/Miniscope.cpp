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
//  file Miniscope.cpp
//  implements class Miniscope
//  Moved to a separate file from Formula.cpp 
//  14/04/2003 Torrevieja
//

#include "Inference.hpp"
#include "Formula.hpp"
#include "Miniscope.hpp"


// initialise:
// _occ such that _occ[i][j] is the numbeer of occurrences of vi in Fj
// _N such that _N[i] = number of j such that _occ[i,j] > 0
// _index such that _index[v] = occ[i], if v = v_i
//                  _index[v] = 0, otherwise
// 29/05/2002 Manchester
// 13/04/2003 Torrevieja, changed to new datastructures
// 15/04/2003 Torrevieja changed to new content
Miniscope::Miniscope (Formula::Connective Q, 
		      const VarList& V, 
		      const Formula& formula,
		      const Position& position)
  :
  _quant (Q),
  _formula (formula),
  _nfs (formula.args().length()),
  _realNfs (_nfs),
  _nvs (V.length()),
  _occ (new bool* [_nvs]),
  _mem (new bool [_nfs * _nvs]),
  _index (0),
  _vars (new Var[_nvs]),
  _N (new int [_nvs]),
  _formulas (new Formula [_nfs]),
  _position (position)
{
  TRACER ("Miniscope::Miniscope");

  ASS(_nfs > 1 && _nvs > 0);

  if (! (_occ && _mem && _vars && _N && _formulas) ) {
    NO_MEMORY;
  }
  // initialise elements of _mem
  for (int k = _nfs * _nvs - 1; k >= 0; k--) {
    _mem [k] = false;
  }

  // find a maximal variable in V
  _maxV = V.max();

  // allocate the index
  _index = new bool* [_maxV+1];
  if (! _index) {
    NO_MEMORY;
  }
  // initialise elements of the index to 0 first
  for (int v = _maxV; v >= 0; v--) {
    _index [v] = 0;
  }
  // then initialise the elements of _index, _occ _vars corresponding to variables in V
  // by their values
  bool* nxt = _mem;
  Var* nv = _vars;
  bool** nocc = _occ;
  Iterator<Var> vs (V);
  while (vs.more()) {
    Var v = vs.next();
    _index [v] = nxt;
    *nocc = nxt;
    nocc++;
    nxt += _nfs;
    *nv = v;
    nv++;
  }

  int j = 0; // formula number
  Iterator<Formula> fs (formula.args());
  while (fs.more()) {
    Formula f (fs.next());
    count (f, j);
    _formulas[j] = f;
    j++;
  }
  // now _index is not needed any more
  delete [] _index;
  _index = 0;

  // initialise elements of _N 
  // place in _occ corresponding to occurrences of vn in Fm
  bool* onm = _mem;
  int* Nn = _N; // same, but for variable vn in _N
  for (int n = 0; n < _nvs; n++) {
    Nn[0] = 0;
    for (int m = 0; m < _nfs; m++) {
      if (*onm) { // at least one occurrence
        Nn[0] ++;
      }
      onm ++;
    }
    Nn++;
  }
} // Miniscope::Miniscope


// 29/05/2002 Manchester
Miniscope::~Miniscope ()
{
  TRACER ("Miniscope::~Miniscope");
  delete [] _mem;
  delete [] _occ;
  delete [] _vars;
  delete [] _N;
  delete [] _formulas;
} // Miniscope::~Miniscope


// count the number of occurrences of variables in nnf Fj
// WARNING: assumes that the formula has no occurrences of quantifiers (Q x) in the 
// range of other quantifiers (Q x) bounding the same variable
// 29/05/2002 Manchester
// 13/04/2003 Torrevieja, changed to new datastructures
void Miniscope::count (const Formula& f, int j)
{
  TRACER ("Miniscope::count(const Formula& f, int j)");

  switch ( f.connective() ) 
    {
    case Formula::ATOM: 
      {
        Iterator<Term> ts (f.atom().args());
        while (ts.more()) {
          count(ts.next(), j);
        }
        return;
      }

    case Formula::AND:
    case Formula::OR: 
      {
	Iterator<Formula> fs (f.args());
	while (fs.more()) {
	  count(fs.next(), j);
	}
	return;
      }

    case Formula::NOT:
      count (f.uarg(), j);
      return;

    case Formula::FORALL:
    case Formula::EXISTS:
      count (f.qarg(), j);
      return;

    case Formula::IMP:
    case Formula::IFF:
    case Formula::XOR:
    default:
      ASS( false );
      return;
    }
} // Miniscope::count


// count the number of occurrences of variables in the term
// 29/05/2002 Manchester
// 13/04/2003 Torrevieja, changed to new term class
void Miniscope::count (const Term& t, int j)
{
  TRACER ("Miniscope::count(const Term& t, int j)");

  switch (t.tag()) 
    {
    case Term::VAR:
      {
	Var v = t.var ();
	if (v > _maxV) {
	  return;
	}
	
	bool* vi = _index[v];
	if (! vi) {
	  return;
	}

	vi[j] = true;
	return;
      }
    
    case Term::NUMERIC:
      return;

    case Term::COMPOUND: 
      {
	// the term is compound
	Iterator<Term> ts (t.args());
	while (ts.more()) {
	  count(ts.next(), j);
	}
	return;
      }
    }
} // Miniscope::count


// true if no miniscoping is possible
// 31/05/2002 Manchester
bool Miniscope::noMiniscoping () const
{
  TRACER("Miniscope::noMiniscoping");

  for (int i = _nvs-1; i >= 0; i--) {
    if ( _N[i] < _nfs ) {
      return false;
    }
  }

  return true;
} // Miniscope::noMiniscoping ()


// find variables having occurrences in exactly one formula in F
// and push them down using 
// (Q v)(F1 & ... & Fk & ... & Fn) => F1 & ... & (Q v)Fk & ... & Fn
// 14/04/2003 Torrevieja
// 28/04/2003 Manchester, inference information added
void Miniscope::pushOnes (InferenceList& inf)
{
  TRACER ("Miniscope::pushOnes");

  // searching for variables with a single occurrence
  for (int v = 0; v < _nvs; v++) {
    if (_N[v] == 1) {
      // find the formula k such that v occurs in Fk
      int k;
      for (k = _nfs-1; k >= 0; k--) {
	if (_occ[v][k]) { // found
	  pushOne (v, k, inf);
	  break;
	}
      }
      ASS (k >=0);
    }
  }
} // Miniscope::pushOnes


// Apply transformation 
//   (Q vs)(F0 & ... & Fk & ... & Fn) => F1 & ... & (Q vs)Fk & ... & Fn
// where vs are variables having occurrences in the same formulas as v
// that is, in Fk only
// 13/04/2003 Torrevieja
// 28/04/2003 Manchester, inference information added
void Miniscope::pushOne (int v, int k, InferenceList& inf)
{
  TRACER ("Miniscope::pushOne");

  // create (Q v)Fk
  // make the list of variables occurring in the same formulas as v
  IntList vs;
  occurInSameFormulas(v,vs);
  VarList ws;
  decodeVars(vs,ws);
  deleteVars (vs);
  Formula f (_formulas[k]);
  Formula g(f);
  IntList toppledIndexes (k);
  IntList toppled;
  decodeFormulas (toppledIndexes, toppled);
  inf.push( Inference( Inference::FORALL_OR_MINISCOPE,
		       g,                       // premise
		       _position,
		       vs,                      // toppled var positions
		       toppled,                 // toppled subformula positions
		       k ) );                   // toppled subformula index
  // miniscope (Q ws)Fk
  f.topMiniscope(_quant,ws,inf,Position(k,_position));
  _formulas[k] = f;
} // Miniscope::pushOne


// set vs to the list of variables after v having occurrences in exactly
// the same formulas as v (including v itself)
// 14/04/2003 Torrevieja
void Miniscope::occurInSameFormulas (int v, IntList& vs) const
{
  TRACER("Miniscope::occurInSameFormulas (Var v, VarList& vs)");

  IntList ws; // to store the result

  for (int w = _nvs-1; w > v; w--) {
    if (_N[w] == 0) { // w is already deleted
      continue;
    }
    if (occurInSameFormulas(v,w)) {
      ws.push (w);
    }
  }

  vs = IntList(v,ws);
} // Miniscope::occurInSameFormulas


// v and w occur in exactly the same formulas 
// 14/04/2003 Torrevieja
bool Miniscope::occurInSameFormulas (Var v, Var w) const
{
  TRACER("Miniscope::occurInSameFormulas (Var v, Var w)");

  bool* V = _occ[v];
  bool* W = _occ[w];

  for (int i = _nfs-1; i >= 0; i--) {
    if (*V != *W) {
      return false;
    }
    V++;
    W++;
  }

  return true;
} // Miniscope::occurInSameFormulas


// in a list of indexes of variables,
// out must be set to the list _vars[v], v in in
// 14/04/2003 Torrevieja
void Miniscope::decodeVars (const IntList& in, VarList& out) const
{
  TRACER ("Miniscope::decodeVars");

  if (in.isEmpty()) {
    return;
  }

  // in is non-empty
  VarList outTail;
  decodeVars (in.tail(), outTail);
  out = VarList (_vars[in.head()], outTail);
} // Miniscope::decodeVars


// delete variables from the list vs
// 14/04/2003 Torrevieja
void Miniscope::deleteVars (const IntList& vs)
{
  TRACER("Miniscope::deleteVars");

  Iterator<int> ws (vs);
  while (ws.more()) {
    _N[ws.next()] = 0;
  }
} // Miniscope::deleteVars


// (a) find a variable v having occurrences in the smallest
//     number of formulas in F
//     If this variable occurs in all formulas, return false
// (b) Find all variables vs having occurrences in the same formulas as v
// (c) create sublist G of F such that G1 ... Gm are exactly those formulas
//     in which v occurs
// (d) Replace G1 by (Q vs)(G1 & ... & Gm)
// (e) Delete G2 ... Gm
// (f) glue G1 ... Gm in G1 in the Miniscope object
// (g) delete vs from the index
// (h) return true
// 14/04/2003 Torrevieja
// 28/04/2003 Manchester, information on inferences added
bool Miniscope::pushMany (InferenceList& inf)
{
  TRACER ("Miniscope::pushMany");

  if (_realNfs < 3) { // cannot work when there is less than 3 formulas
    return false;
  }

  // (a) find a variable v having occurrences in the smallest
  //     number of formulas in F
  int v = 0; // initialization is not necessary
  int m = _realNfs; // minimal number of occurrences
  
  // searching for the minimal number of occurrences
  for (int w = 0; w < _nvs; w++) {
    if (_N[w] != 0 && _N[w] < m) {
      m = _N[w];
      v = w;
    }
  }
  //     If this variable occurs in all formulas, return
  if (m == _realNfs) { // all variables occur in all formulas
    return false;
  }
  // (b) Find all variables vs having occurrences in the same formulas as v
  IntList vs;
  occurInSameFormulas(v,vs);

  // (c) create sublist G of F such that G1 ... Gm are exactly those formulas
  //     in which v occurs
  FormulaList G;
  int lastDeleted = 0; // last formula to be deleted
  bool* occs = _occ[v]; // occurrences for variable v
  for (int i = _nfs-1; i >= 0; i--) {
    Formula f (_formulas[i]);
    if (f.isNull()) { // formula no. i is in fact deleted
      continue;
    }
    if (occs[i]) { // occurs
      G.push(f);
      if (lastDeleted != 0) {
	// (e) Delete G2 ... Gm
	_formulas[lastDeleted].makeNull();
        _realNfs --;
	// (f) glue G1 ... Gm in G1 in the Miniscope object
	for (int w = _nvs-1; w >= 0; w--) {
	  if (_N[w] != 0) { // w is not deleted
	    if (_occ[w][lastDeleted]) {
	      if (_occ[w][i]) { // w occurs in both i and lastDeleted
		_N[w] --;
		ASS (_N[w] > 0);
	      }
	      else { // w occurs in lastDeleted but not in i
		_occ[w][i] = true;
	      }
	    }
	    // else w does not occur in lastDeleted
	  }
	}
      }
      // mark i for deletion
      lastDeleted = i;
    }
  }

  // (g) delete vs from the index
  VarList ws;
  decodeVars(vs,ws);
  deleteVars (vs);

  // (d) Replace G1 by (Q vs)(G1 & ... & Gm)
  Formula F (_quant, ws, 
	     Formula( _formula.connective(), G));
  _formulas[lastDeleted] = F;

  // memorize the inference
  Formula conclusion (F);

  // (h) return true
  return true;
} // Miniscope::pushMany


// set res to result
// 15/04/2003 Torrevieja
void Miniscope::result (Formula& res) 
{
  TRACER("Miniscope::result");

  VarList ws;
  // collect into ws the remaining variables
  for (int v = _nvs-1; v >= 0; v--) {
    if (_N[v] != 0) { // this variable remains
      ws = VarList (_vars[v], ws);
    }
  }

  FormulaList fs;
  // collect into fs the remaining formulas
  for (int k = _nfs-1; k >= 0; k--) {
    Formula f (_formulas[k]);
    if (! f.isNull()) {
      fs = FormulaList (f,fs);
    }
  }
  ASS (fs.length() > 1);

  res = Formula (_formula.connective(),fs);
  if (ws.isNonEmpty()) {
    res = Formula (_quant,ws,res);
  }
} // Miniscope::result


// in is a list of indexes of formula in the array _formulas
// out is the list of their indices if the deleted formulas were removed
// 04/05/2003 Manchester
void Miniscope::decodeFormulas (const IntList& in, IntList& out) const
{
  TRACER("Miniscope::decodeFormulas/2");

  decodeFormulas (in, out, 0, 0);
} // Miniscope::decodeFormulas


// in is a list of indexes of formula in the array _formulas
// out is the list of their indices if the deleted formulas were removed
// inIndex is the index of the first formula in in
// outIndex is the index of the corresponding formula in out
// 04/05/2003 Manchester
void Miniscope::decodeFormulas (const IntList& in, IntList& out,
				int inIndex, int outIndex) const
{
  TRACER("Miniscope::decodeFormulas/4");

  if (in.isEmpty()) {
    return;
  }
  int i = in.head();
  while (inIndex != i) {
    if (! _formulas[i].isNull()) {
      outIndex ++;
    }
    inIndex++;
  }
  // inIndex == i
  ASS (! _formulas[i].isNull());
  out = IntList (outIndex);

  decodeFormulas (in.tail(), const_cast<IntList&>(out.tail()), 
		  inIndex+1, outIndex+1);
} // Miniscope::decodeFormulas

