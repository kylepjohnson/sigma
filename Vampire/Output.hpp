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
//  file Output.hpp
//  defines various output classes
//

#ifndef __Output__
#define __Output__


#include "Formula.hpp"


// class ostream;
class Literal;
class LiteralList;
class Clause;
class Unit;
class UnitList;
class Refutation;



// ****************** class Output, definition *************************


enum OutputSyntax {
  TPTP = 0,       // the numbers should not be changed
  NATIVE = 1,     // they are used in some analogues of
  KIF = 2,        // virtual tables
  LATEX = 3       // must be the last one
};


class Output {
 public:
  // create an output
  static Output* create (OutputSyntax, ostream&);

  // constructors/destructors
  Output (OutputSyntax syntax, ostream& str);

  virtual void pretty (const Refutation&, int answerNumber) = 0;
  virtual void formula (const Formula&);

 protected:
  class Map; // defined in Output.cpp
  virtual void symbol (const Signature::Symbol*);
  void refutation (const Unit& goal, int indent, Map& map);
  void refutations (const UnitList& goal, int indent, Map& map);
  virtual void inference (const Unit& conclusion, int indent) = 0;
  virtual void connective (Formula::Connective);
  virtual void var (Var v);
  virtual void term (const Term&);
  virtual void terms (const TermList&);
  virtual void atom (const Atom&);
  virtual void literal (const Literal&);
  virtual void clause (const Clause&) = 0;
  virtual void clause (const Clause&, int indent) = 0;
  virtual void formula (const Formula&, int indent) = 0;
  void subformula (const Formula&, int indent, Formula::Connective outer);
  void subformula (const Formula&, Formula::Connective outer);
  void content (const Unit& unit); // print clause or formula from unit
  virtual void content (const Unit& unit, int indent);
  bool fitsInOneLine (const Formula&) const;
  static bool fitsInOneLine (const Formula&, int& limit);
  static bool parenthesesRequired (Formula::Connective inner, 
				   Formula::Connective outer);
  void vars (const VarList&);

  // structure
  OutputSyntax _syntax;
  ostream& _stream;

  // printing various data types
  virtual void spaces (int times);
  void repeat (char character, int times);

  // virtual tables
  static int _oneLineLengthLimit [LATEX+1];
}; // class Output


class NativeOutput
  : public Output
{
 public:
  NativeOutput (ostream&);

  // output functions
  void pretty (const Refutation&, int answerNumber);

 public:
  void inference (const Unit& conclusion, int indent);
  void clause (const Clause&);
  void clause (const Clause&, int indent);
  void formula (const Formula&, int indent);
  void formula (const Formula&);
  virtual void connective (Formula::Connective);
};  // class NativeOutput


class TPTPOutput
  : public Output
{
 public:
  TPTPOutput (ostream&);

  // output functions
  void pretty (const Refutation&, int answerNumber);

 private:
  void inference (const Unit& conclusion, int indent);
  void clause (const Clause&);
  void clause (const Clause&, int indent);
  void formula (const Formula&);
  void formula (const Formula&, int indent);
};  // class TPTPOutput


class KIFOutput
  : public Output
{
 public:
  KIFOutput (ostream&);

  // output functions
  void pretty (const Refutation&, int answerNumber);

 private:
  void inference (const Unit& conclusion, int indent);
  void content (const Unit& unit, int indent);
  void clause (const Clause&);
  void clause (const Clause&, int indent);
  void formula (const Formula&);
  void formula (const Formula&, int indent);
  void literal (const Literal&);
  void atom (const Atom&);
  void connective (Formula::Connective);
  void var (Var v);
  void terms (const TermList&);
  void term (const Term&);

  static int _connectivePrintLength[];
};  // class KIFOutput


class LaTeXOutput
  : public Output
{
 public:
  LaTeXOutput (ostream&);

  // output functions
  void pretty (const Refutation&, int answerNumber);

 private:
  void symbol (const Signature::Symbol*);
  void inference (const Unit& conclusion, int indent);
  void clause (const Clause&);
  void literals (const LiteralList& lits);
  void literals (const LiteralList& lits, int indent);
  void clause (const Clause&, int indent);
  void formula (const Formula&, int indent);
  void formula (const Formula&);
  void connective (Formula::Connective);
  void var (Var v);
  void spaces (int times);
};  // class LaTeXOutput


// ***************** class Output, implementation ***************


// for debugging only
ostream& operator<< (ostream&, const Formula&);

#endif // __Output__

