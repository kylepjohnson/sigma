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
//  file SymCounter.hpp
//  defines class SymCounter counting occurrences of function and predicate symbols
//
//  01/05/2002, Manchester
//

#ifndef __sym_counter__
#define __sym_counter__


class Problem;


#include "Signature.hpp"
#include "Unit.hpp"


using namespace std;


// ***************** class SymCounter, definition ****************************


class SymCounter { 
 public:

  class Fun {
    Signature::Fun* _fun;
    int _occ;
   public:
    Fun () : _occ(0) {}
    void fun (Signature::Fun* f) { _fun = f; }
    int occ () const { return _occ; }
    Signature::Fun* fun () { return _fun; }
    void add (int add) { _occ += add; }
  };

  class Pred {
    Signature::Pred* _pred;
    int _pocc;  // positive occurrences
    int _nocc;  // negative occurrences
    int _docc;  // double occurrences (under equivalence)
   public:
    Pred () 
      : _pocc (0), 
        _nocc (0),
        _docc (0) 
        {}
    void pred (Signature::Pred* p) { _pred = p; }
    int pocc () const { return _pocc; }
    int nocc () const { return _nocc; }
    int docc () const { return _docc; }
    Signature::Pred* pred () { return _pred; }
    void add (int polarity, int add);
  };

  explicit SymCounter (Signature&);
  ~SymCounter ();

  // counting functions
  void count (Problem&, int);
  void count (Unit, int);
  void count (Clause, int);
  void count (Formula, int polarity, int add);
  void count (Literal, int);
  void count (Atom a, int polarity, int add);
  void count (Term t, int add);

  // get 
  Pred* get (const Signature::Pred* p) { return _preds + p->number(); }
  Fun* get (const Signature::Fun* f) { return _funs + f->number(); }

  // output
  void output (ostream& str) const;

  static SymCounter* norm;    // used for normalisation

 private:

  // structure
  int _noOfPreds;
  int _noOfFuns;
  Pred* _preds;
  Fun* _funs;
}; // class SymCounter



inline 
ostream& operator << (ostream& str, const SymCounter& c)
{
  c.output(str);
  return str;
} // ostream& operator << (ostream& str, const SymCounter& c)


#endif // __sym_counter__
