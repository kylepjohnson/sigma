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
//  file Output.cpp
//  implements various output classes
//


#include "Output.hpp"
#include "Unit.hpp"
#include "Refutation.hpp"
#include "Map.hpp"
#include "Problem.hpp"
#include "IntNameTable.hpp"


int Output::_oneLineLengthLimit [LATEX+1] = {5,  // TPTP
					     5,  // native
					     3,  // KIF
					     10}; // LaTeX

class Output::Map 
  : public ::Map <int, Unit>
{
}; // class Output::Map


// 05/10/2002 Manchester
Output::Output (OutputSyntax syntax, ostream& str)
  : _syntax (syntax),
    _stream (str)
{
} // Output::Output 


// 05/10/2002 Manchester
NativeOutput::NativeOutput (ostream& str)
  : Output (NATIVE, str)
{
} // NativeOutput::NativeOutput (ostream&)


// 05/10/2002 Manchester
TPTPOutput::TPTPOutput (ostream& str)
  : Output (TPTP, str)
{
} // TPTPOutput::TPTPOutput (ostream&)


// 05/10/2002 Manchester
KIFOutput::KIFOutput (ostream& str)
  : Output (KIF, str)
{
} // KIFOutput::KIFOutput (ostream&)


// 05/10/2002 Manchester
LaTeXOutput::LaTeXOutput (ostream& str)
  : Output (LATEX, str)
{
} // LaTeXOutput::LaTeXOutput (ostream&)


// output function or predicate symbol
// 06/10/2002 Manchester
void Output::symbol (const Signature::Symbol* sym)
{
  _stream << sym->name();
} // Output::symbol


// output character several times
// 05/10/2002 Manchester
void Output::repeat (char character, int times)
{
  ASS (times >= 0);

  while (times > 0) {
    times--;
    _stream << character;
  }
} // Output::repeat 


// create an Output in the right syntax
// 05/10/2002 Manchester
Output* Output::create (OutputSyntax syntax, ostream& str)
{
  switch (syntax) 
    {
    case TPTP:
      return new TPTPOutput (str);
    case NATIVE:
      return new NativeOutput (str);
    case KIF:
      return new KIFOutput (str);
    case LATEX:
      return new LaTeXOutput (str);
#if DEBUG_PREPRO
    default:
      ASS(false);
#endif
    }
} // Output::create (OutputSyntax, ostream&)


// pretty-print refutation in the native syntax
// 06/10/2002 Manchester
void NativeOutput::pretty (const Refutation& ref, int number)
{
  Unit goal (ref.condense ());

  _stream << "=========== Refutation ==========\n";

  Map map;
  refutation (goal, 0, map);

  _stream << "======= End of refutation =======\n";
} // Refutation::output


// output refutation rooted at the root
// 23/09/2002 Manchester
// 04/10/2002 Manchester, changed
void Output::refutation (const Unit& root, int indent, Map& map)
{
  Unit dummy;

  if ( map.find (root.number(), dummy) ) { // already output
    return;
  }
  map.insert (root.number(), root);

  // output refutation of parents
  refutations (root.parents(), indent, map);
  // output the inference used to obtain the root
  inference (root, indent);
} // Output::refutation


// output inference in the native syntax
// 06/10/2002 Manchester
void NativeOutput::inference (const Unit& conclusion, int indent) 
{
  spaces (indent);
  repeat ('*', 11);
  _stream << " [";
  Iterator<Unit> ps1 (conclusion.parents());
  while (ps1.more()) {
    _stream << ps1.next().number();
    if (ps1.more()) {
      _stream << ',';
    }
  }

  if (conclusion.parents().isNonEmpty()) {
    _stream << "->";
  }
  _stream << conclusion.number () 
	  << "] ";
  repeat ('*', 11);
  _stream << '\n';

  Iterator<Unit> ps (conclusion.parents());
  while (ps.more()) {
    spaces(indent);
    content (ps.next(), indent);
    _stream << '\n';
  }

  if (conclusion.parents().isNonEmpty()) {
    spaces (indent);
    repeat ('-', 29);
    _stream << '\n';
  }

  content (conclusion, indent);
  _stream << '\n';
} // NativeOutput::inference


// print clause or formula from unit
// 06/10/2002 Manchester
void Output::content (const Unit& unit)
{
  switch ( unit.unitType() ) 
    {
    case CLAUSE:
      clause (unit.clause());
      return;
    case FORMULA:
      formula (unit.formula());
      break;
    }
} // Output::content 


// print clause or formula from unit
// 06/10/2002 Manchester
void Output::content (const Unit& unit, int indent)
{
  switch ( unit.unitType() ) 
    {
    case CLAUSE:
      clause (unit.clause(), indent);
      return;
    case FORMULA:
      formula (unit.formula(), indent);
      break;
    }
} // Output::content 


// output list of refutations
// 06/10/2002 Manchester
void Output::refutations(const UnitList& units, int indent, Map& map)
{
  Iterator<Unit> us (units);

  while (us.more()) {
    refutation (us.next(), indent, map);
  }
} // Refutation::output (... UnitList ...)


// pretty-print clause in the native format
// 06/10/2002 Manchester
void NativeOutput::clause (const Clause& cls, int indent) 
{
  clause (cls);
//   if (cls.isEmpty()) {
//     _stream << '#';
//     return;
//   }

//   Iterator<Literal> lits (cls.literals());
//   while (lits.more()) {
//     literal (lits.next());
//     if (lits.more()) {
//       _stream << '\n';
//       spaces (indent);
//       connective (Formula::OR);
//       _stream << ' ';
//     }
//   }
} // NativeOutput::clause


// output clause in the native format
// 06/10/2002 Manchester
void NativeOutput::clause (const Clause& cls) 
{
  if (cls.isEmpty()) {
    _stream << '#';
    return;
  }

  Iterator<Literal> lits (cls.literals());
  while (lits.more()) {
    literal (lits.next());
    if (lits.more()) {
      _stream << ' ';
      connective (Formula::OR);
      _stream << ' ';
    }
  }
} // NativeOutput::clause


// output connective in the native syntax
void NativeOutput::connective (Formula::Connective con) 
{
  static const char* names [] =
    { "", "&", "\\/", "=>", "<=>", "<~>", "~", "!", "?"};

  _stream << names[(int)con];
} // NativeOutput::connective


// output literal (default for severl syntaxes)
// 06/10/2002 Manchester
void Output::literal (const Literal& lit)
{
  if (lit.negative()) {
    connective (Formula::NOT);
  }

  atom (lit.atom());
} // Output::literal


// output an atom (default for several syntaxes)
// 06/10/2002 Manchester
// 23/10/2002 Manchester changed slightly
void Output::atom (const Atom& atm)
{
  TermList args (atm.args());

  if (atm.isEquality()) {
    term (args.head());
    _stream << '=';
    term (args.second());
    return;
  }

  // TEMPORARY, for CHECKING INCONSISTENCY OF KIF
  if (! strcmp(atm.functor()->name(), "holds")) {
    term (args.head());
    _stream << '(';
    terms (args.tail());
    _stream << ')';
    return;
  }

  symbol (atm.functor());

  if (args.isEmpty()) {
    return;
  }

  _stream << '(';
  terms (args);
  _stream << ')';
} // Atom::output (...)


// output term (in TPTP and Native syntaxes)
// 21/09/2002 Manchester
// 28/09/2002 Manchester, changed to include numeric data
// 04/10/2002 Manchester, LaTeX output added
void Output::term (const Term& t)
{
  switch (t.tag()) 
    {
    case Term::VAR:
      var (t.var());
      return;

    case Term::NUMERIC:
      _stream << t.number();
      return;

    case Term::COMPOUND:
      symbol (t.functor());
      // non-variable term
      if (t.args().isEmpty()) {
	return;
      }

      _stream << '(';
      terms(t.args());
      _stream << ')';
      return;
    }
} // Output::term


// term list output
// 06/10/2002 Manchester
void Output::terms (const TermList& tms) 
{
  Iterator<Term> ts (tms);
  while (ts.more()) {
    term (ts.next());
    if (ts.more()) {
      // output separator
      _stream << ',';
    }
  }
} // Output::terms


// output a variable
// 06/10/2002 Manchester
void Output::var (Var v)
{
  _stream << 'X' << v;
} // Output::var


// pretty-print formula in the native syntax
// 11/10/2002 flight Manchester-Istambul
void NativeOutput::formula (const Formula& f, int indent)
{
  if ( fitsInOneLine(f) ) {
    formula (f);
    return;
  }

  Formula::Connective con (f.connective());

  switch (con) 
    {
    case Formula::ATOM:
      atom (f.atom());
      return;

    case Formula::AND:
    case Formula::OR: 
      {
	FormulaList args (f.args());
	ASS (f.args().length() >= 2);
	
	subformula (args.head(), indent+2, con);
	Iterator<Formula> arg (args.tail());
	while (arg.more()) {
	  _stream << ' ';
	  connective (con);
	  _stream << '\n';
	  spaces (indent);
	  subformula (arg.next(), indent+2, con);
	}
	return;
      }

  case Formula::IMP:
  case Formula::IFF:
  case Formula::XOR:
    subformula (f.left(), indent+2, con);
    _stream << ' ';
    connective (con);
    _stream << '\n';
    spaces (indent);
    subformula (f.right(), indent+2, con);
    return;

  case Formula::NOT:
    connective (con);
    subformula (f.uarg(), indent, con);
    return;

  case Formula::FORALL:
  case Formula::EXISTS:
    _stream << '(';
    connective (con);
    vars (f.vars());
    _stream << ")\n";
    spaces (indent+2);
    subformula (f.qarg(), indent+2, con);
    return;

  default:
    ASS (false);
  }
} // NativeOutput::formula (...)


// output formula in several syntaxes (works both for LaTeX and Native)
// 12/10/2002, Tbilisi
void Output::formula (const Formula& f)
{
  Formula::Connective con (f.connective());

  switch (con) 
    {
    case Formula::ATOM:
      atom (f.atom());
      return;

    case Formula::AND:
    case Formula::OR: 
      {
	FormulaList args (f.args());
	ASS (f.args().length() >= 2);
	
	subformula (args.head(), con);
	Iterator<Formula> arg (args.tail());
	while (arg.more()) {
	  _stream << ' ';
	  connective (con);
	  _stream << ' ';
	  subformula (arg.next(), con);
	}
	return;
      }

    case Formula::IMP:
    case Formula::IFF:
    case Formula::XOR:
      subformula (f.left(), con);
      _stream << ' ';
      connective (con);
      _stream << ' ';
      subformula (f.right(), con);
      return;

    case Formula::NOT:
      connective (con);
      subformula (f.uarg(), con);
      return;

    case Formula::FORALL:
    case Formula::EXISTS:
      _stream << '(';
      connective (con);
      vars (f.vars());
      _stream << ')';
      subformula (f.qarg(), con);
      return;

    default:
      ASS (false);
    }
} // Output::formula (...)


// true if the formula should be printed in one line
// 11/10/2002, flight Istambul-Tbilisi
bool Output::fitsInOneLine (const Formula& f) const
{
  // formulas of size > limit are printed in one line
  int limit = _oneLineLengthLimit [_syntax];

  return fitsInOneLine (f, limit);
} // Formula::printOneLine


// auxiliary function for fitsInOneLine/1
// 11/10/2002, flight Istambul-Tbilisi
bool Output::fitsInOneLine (const Formula& f, int& limit)
{
  TRACER("Formula::fitsInOneLine");

  if (--limit < 0) {
    return false;
  } 

  switch ( f.connective() ) 
    {
    case Formula::ATOM:
      return true;

    case Formula::OR:
    case Formula::AND: 
      {
	limit -= f.args().length() - 1;
	if (limit < 0) {
        return false;
	}
	Iterator<Formula> as (f.args());
	while ( as.more() ) {
	  if (! fitsInOneLine(as.next(), limit) ) {
	    return false;
	  }
	}
	return true;
      }

    case Formula::IMP: 
    case Formula::IFF:
    case Formula::XOR:
      if (--limit < 0) {
        return false;
      }
      return fitsInOneLine (f.left(), limit) &&
	fitsInOneLine (f.right(), limit);

    case Formula::NOT:
      if (--limit < 0) {
        return false;
      } 
      return fitsInOneLine (f.uarg(), limit);

    case Formula::FORALL:
    case Formula::EXISTS:
      limit -= f.vars().length() + 1;
      if (--limit < 0) {
        return false;
      } 
      return fitsInOneLine (f.qarg(), limit);

    default:
      ASS ( false );
      return false;
    }
} // Formula::fitsInOneLine


// pretty-print subformula in the range of the connective outer
// 11/10/2002, flight Istambul-Tbilisi
void Output::subformula (const Formula& f, 
			 int indent, 
			 Formula::Connective outer)
{
  if (parenthesesRequired (f.connective(), outer)) {
    _stream << '(';
    formula (f, indent);
    _stream << ')';
  }
  else {
    formula (f, indent);
  }
} // Output::subformula


// output subformula in the range of the connective outer
// 13/10/2002, Tbilisi
void Output::subformula (const Formula& f, Formula::Connective outer)
{
  if (parenthesesRequired (f.connective(), outer)) {
    _stream << '(';
    formula (f);
    _stream << ')';
  }
  else {
    formula (f);
  }
} // Output::subformula


// true if the formula needs parentheses around itself when in scope of outer
// 21/09/2002 Manchester
bool Output::parenthesesRequired(Formula::Connective inner,
				 Formula::Connective outer)
{
  switch ( inner ) 
    {
    case Formula::ATOM:
    case Formula::NOT:
    case Formula::FORALL:
    case Formula::EXISTS:
      return false;

    case Formula::OR:
    case Formula::AND:
      return outer != Formula::IMP &&
	     outer != Formula::IFF &&
             outer != Formula::XOR;

    case Formula::IMP:
      return outer != Formula::IFF &&
	     outer != Formula::XOR;

    case Formula::IFF:
    case Formula::XOR:
      return true;

    default:
      ASS( false );
      return false;
    }
} // Output::parenthesesRequired


// output a sequence of variables in a quantifier prefix
// 12/10/2002 Tbilisi
void Output::vars (const VarList& vs)
{
  Iterator<Var> ws (vs);
  while (ws.more()) {
    _stream << ' ';
    var (ws.next());
  }
} // Output::vars


// pretty-print inference in KIF format
// 18/10/2002 Tbilisi
void KIFOutput::inference (const Unit& u, int indent)
{
  spaces(indent);
  _stream << "<proofStep>\n";

  // output premises
  spaces(indent+2);
  _stream << "<premises>\n";
  Iterator<Unit> ps (u.parents());
  while (ps.more()) {
    spaces(indent+4);
    _stream << "<premise>\n";
    spaces(indent+6);
    content (ps.next(), indent+6);
    _stream << '\n';
    spaces(indent+4);
    _stream << "</premise>\n";
  }
  spaces(indent+2);
  _stream << "</premises>\n";
      
  // output conclusion
  spaces(indent+2);
  _stream << "<conclusion>\n";
  spaces(indent+4);
  content (u, indent+4);
  _stream << '\n';
  spaces(indent+2);
  _stream << "</conclusion>\n";

  spaces(indent);
  _stream << "</proofStep>\n";
} // KIFOutput::inference


// pretty-priintt output in the KIF format
// 20/10/2002 Tbilisi
void KIFOutput::pretty (const Refutation& ref, int answerNumber)
{
  int indent=2;
  spaces (indent);
  _stream << "<answer result='yes' number='" << answerNumber << "'>\n";

  if (ref.problem().answerLiteral().exists()) { // no answer literal - no binding
    const VampireKernel::Literal* goalLiterals = ref.kernel().refutation()->literals();
      
    spaces (indent+2);
    _stream << "<bindingSet type='";
    if (! goalLiterals) { // anything is the answer
      _stream << "inconsistent";
    }
    else if (! goalLiterals->next()) { // exactly one answer literal
      _stream << "definite";
    }
    else {
      _stream << "disjunctive";
    }
    _stream << "'>\n";
      
    // output each binding
    while (goalLiterals) {
      spaces (indent+4);
      _stream << "<binding>\n";
	
      const VampireKernel::Subterm* subterms = goalLiterals->firstArgument();
      Iterator<Var> vs (ref.problem().originalAnswerVariables());
      while (vs.more()) {
	ASS (subterms);
	spaces(indent+6);
	_stream << "<var name='" 
		<< (*IntNameTable::vars) [vs.next()]
		<< "' value='";
	Term t (subterms, ref.kernel());
	term (t);
	_stream << "'/>\n";
	subterms = subterms->next ();
      }
	
      spaces(indent+4);
      _stream << "</binding>\n";
      goalLiterals = goalLiterals->next();
    }
    spaces(indent+2);
    _stream << "</bindingSet>\n";
  }
  spaces(indent+2);
  _stream << "<proof>\n";

  //  output refutation itself
  Unit goal (ref.condense ());
  Map map;
  refutation (goal, indent+4, map);

  spaces(indent+2);
  _stream << "</proof>\n";
  spaces(indent);
  _stream << "</answer>\n";
} // KIFOutput::prretty


// pretty-print formula in the KIF format
// 21/10/2001 Tbilisi
void KIFOutput::formula (const Formula& f, int indent) 
{
  Formula::Connective con = f.connective();

  switch ( con ) 
    {
    case Formula::ATOM:
      atom (f.atom());
      return;

    case Formula::AND:
    case Formula::OR: 
      {
	_stream << '(';
	connective(con); 
	_stream << ' ';
	indent += _connectivePrintLength[con] + 2;
	Iterator<Formula> fs (f.args());
	while (fs.more()) {
	  formula (fs.next(), indent);
	  if (fs.more()) {
	    _stream << '\n';
	    spaces (indent);
	  }
	}
	_stream << ')';
	return;
      }

    case Formula::IMP:
    case Formula::IFF:
      _stream << '(';
      connective(con); 
      _stream << ' ';
      indent += _connectivePrintLength[con] + 2;
      formula (f.left(), indent);
      _stream << '\n';
      spaces (indent);
      formula (f.right(), indent);
      _stream << ')';
      return;

    case Formula::XOR: 
      // there is no XOR in the KIF syntax
      formula (Formula (Formula::NOT, 
			Formula (Formula::IFF, f.left(), f.right())));
      return;

    case Formula::NOT:
      _stream << '(';
      connective(con); 
      _stream << ' ';
      formula (f.uarg(), indent+_connectivePrintLength[con] + 2);
      _stream << ')';
      return;

    case Formula::FORALL:
    case Formula::EXISTS: 
      {
	_stream << '(';
	connective(con); 
	_stream << " (";
	indent += 2;
	Iterator<Var> vs (f.vars());
	while (vs.more()) {
	  _stream << ' ';
	  var (vs.next());
	}
	_stream << ")\n";
	spaces (indent);
	formula (f.qarg(), indent);
	_stream << ')';
	return;
      }
    
    default:
      ASS( false );
      return;
    }
} // KIFOutput::formula


// output connective in the KIF syntax
// 21/10/2002 Tbilisi
// POD 2004.08.02 ensure proper xml
void KIFOutput::connective (Formula::Connective con) 
{
  static const char* names [] =
    { "", "and", "or", "=&gt;", "&lt;=&gt;", "", "not", "forall", "exists"};

  _stream << names[(int)con];
} // KIFOutput::connective


// print lengths of connectives in the KIF syntax
// 21/10/2002 Tbilisi
int KIFOutput::_connectivePrintLength [] =
  { 0, 3, 2, 5, 9, 0, 3, 6, 6};

// output a variable in the KIF syntax
// 21/10/2002 Tbilisi
void KIFOutput::var (Var v)
{
  _stream << "?X" << v;
} // KIFOutput::var


// output atom in the KIF syntax
// 21/10/2002 Tbilisi
// POD 2004.08.02 ensure proper xml
void KIFOutput::atom (const Atom& a)
{
  const char* funct =  a.functor()->name();
  if      (!strcmp(funct,"<="))  _stream << "(&lt;= ";
  else if (!strcmp(funct,">="))  _stream << "(&gt;= ";
  else if (!strcmp(funct,"<"))   _stream << "(&lt; ";
  else if (!strcmp(funct,">"))   _stream << "(&gt; ";
  else _stream << '(' << a.functor() << ' ';
  terms (a.args());
  _stream << ')';
} // KIFOutput::atom 


// term list output in the KIF syntax
// 06/10/2002 Manchester
void KIFOutput::terms (const TermList& tms) 
{
  Iterator<Term> ts (tms);
  while (ts.more()) {
    term (ts.next());
    if (ts.more()) {
      // output separator
      _stream << ' ';
    }
  }
} // KIFOutput::terms


// output term in the KIF syntax
// 21/10/2002 Tbilisi
void KIFOutput::term (const Term& t)
{
  switch (t.tag()) 
    {
    case Term::VAR:
      var (t.var());
      return;

    case Term::NUMERIC:
      _stream << t.number();
      return;
      
    case Term::COMPOUND:
      // non-variable term
      if (t.args().isEmpty()) {
	_stream << t.functor();
	return;
      }
      break;
    }

  // compoundd term with more than one argument
  const char* name = t.functor()->name();
  if (!strcmp (name, "$`")) {
    _stream << '`';
    term(t.args().head());
    return;
  }

  _stream << '(';
  if (strcmp (name, "$qvars")) {
    _stream << t.functor() << ' ';
  }

  terms (t.args());
  _stream << ')';
} // KIFOutput::term


// print clause or formula from unit
// 06/10/2002 Manchester
void KIFOutput::content (const Unit& unit, int indent)
{
  switch ( unit.unitType() ) 
    {
    case CLAUSE:
      _stream << "<clause number='"
              << unit.number()
              << "'>\n";
      spaces (indent+2);
      clause (unit.clause(), indent+2);
      _stream << '\n';
      spaces (indent);
      _stream << "</clause>";
      return;
    case FORMULA:
      _stream << "<formula number='"
              << unit.number()
              << "'>\n";
      spaces (indent+2);
      formula (unit.formula(), indent+2);
      _stream << '\n';
      spaces (indent);
      _stream << "</formula>";
      break;
    }
} // KIFOutput::content 


// pretty-printing a clause in the KIF syntax
// 22/10/2002 Tbilisi
void KIFOutput::clause (const Clause& c, int indent)
{
  const LiteralList& ls = c.literals ();

  if (ls.isEmpty()) { // empty clause
    _stream << "FALSE";
    return;
  }
  
  // there is at least one literal
  if (ls.tail().isEmpty()) { // one-literal clause
    literal (ls.head());
    return;
  }

  _stream << "(or ";
  Iterator<Literal> lits (ls);
  while (lits.more()) {
    literal(lits.next());
    if (lits.more()) {
      _stream << '\n';
      spaces (indent+4);
    }
  }	
  _stream << ")";
} // KIFOutput::clause


// output literal in the KIF syntax
// 22/10/2002 Tbilisi
void KIFOutput::literal (const Literal& lit)
{
  if (lit.negative()) {
    _stream << '(';
    connective (Formula::NOT);
    _stream << ' ';
    atom (lit.atom());
    _stream << ')';
    return;
  }

  // positive literal
  atom (lit.atom());
} // Output::literal


// pretty-print refutation in the LaTeX format 
// 21/10/2002 Istambul airport
void LaTeXOutput::pretty (const Refutation& ref, int answerNumber) 
{
  Unit goal (ref.condense ());
  Map map;

  _stream << 
    "\\documentclass[fleqn]{article}\n"
    "\\usepackage{fullpage,latexsym}\n"
    "\\input{VampireProofMacros}\n"
    "\\begin{document}\n"
    "\\begin{VampireProof}\n";
  refutation (goal, 0, map);
  _stream << 
    "\\end{VampireProof}\n"
    "\\end{document}\n";
} // LaTeXOutput::pretty


// output inference in LaTeX
// 23/10/2002 Manchester
void LaTeXOutput::inference (const Unit& unit, int indent) 
{
  _stream << "\\noindent\\hrulefill~$";
  if (unit.parents().isNonEmpty()) {
    _stream << '(';
    Iterator<Unit> ps (unit.parents());
    while (ps.more()) {
      _stream << ps.next().number();
      if (ps.more()) {
	_stream << ',';
      }
    }
    _stream << ")\\rightarrow ";
  }
  _stream << unit.number()
	  << "$~\\noindent\\hrulefill"
	  << "\\[\n";

  // output premises
  _stream << "\\begin{VampireInference}\n";

  if (unit.parents().isNonEmpty()) {
    Iterator<Unit> ps (unit.parents());
    while (ps.more()) {
      _stream << "\\begin{VampirePremise}%\n";
      spaces (indent);
      content (ps.next(), indent+2);
      _stream << "\\end{VampirePremise}\n";
      if (ps.more()) {
	_stream << "\\VPremiseSeparator\n";
      }
    }
    _stream << "\\VConclusionSeparator\n";
  }

  // output conclusion
  _stream << "\\begin{VampireConclusion}\n";
  content (unit, indent+2);
  _stream << "\\end{VampireConclusion}"
    " \\end{VampireInference}\n"
    "\\]\n";
} // LaTeXOutput::inference (const Unit& unit, int indent) 


// output clause in LaTeX
// 23/10/2002 Manchester
void LaTeXOutput::clause (const Clause& cls, int indent)
{
  if (cls.isEmpty()) { // empty clause
    _stream << "\\VEmptyClause";
    return;
  }

  literals (cls.literals(), indent);
} // LaTeXOutput::clause (const Clause& cls, int indent)


// output clause in LaTeX
// 23/10/2002 Manchester
void LaTeXOutput::clause (const Clause& cls)
{
  if (cls.isEmpty()) { // empty clause
    _stream << "\\VEmptyClause";
    return;
  }

  literals (cls.literals());
} // LaTeXOutput::clause (const Clause& cls)


// output clause as list of literals in LaTeX
// 23/10/2002 Manchester
void LaTeXOutput::literals (const LiteralList& lits)
{
  ASS (lits.isNonEmpty());

  literal (lits.head());

  if (lits.tail().isEmpty()) {
    return;
  }

  connective (Formula::OR);
  literals (lits.tail());
} // LaTeXOutput::literals
  

// pretty-print clause as list of literals in LaTeX
// 23/10/2002 Manchester
void LaTeXOutput::literals (const LiteralList& lits, int indent)
{
  ASS (lits.isNonEmpty());

  literal (lits.head());

  if (lits.tail().isEmpty()) {
    return;
  }

  _stream << "\\\\\n";
  spaces (indent);
  connective (Formula::OR);
  literals (lits.tail(), indent);
} // LaTeXOutput::literals
  

// output connective in the LaTeX syntax
// 23/10/2002 Manchester
void LaTeXOutput::connective (Formula::Connective con) 
{
  static const char* names [] =
    { "", "\\Vand", "\\Vor", "\\Vimp", "\\Viff", "\\Vxor", "\\neg", "\\forall", "\\exists"};

  _stream << names[(int)con];
} // LaTeXOutput::connective


// output function or predicate symbol in LaTeX
// 05/10/2002 Manchester
// 23/10/2002 Manchester, changed to handle special KIF names
void LaTeXOutput::symbol (const Signature::Symbol* sym)
{
  if (! strcmp (sym->name(), "$`")) { // KIF's symbol
    _stream << '`';
    return;
  }
  if (! strcmp (sym->name(), "$qvars")) { // KIF's symbol
    return;
  }
  if (! strcmp (sym->name(), "$answer")) { // answer predicate
    _stream << "\\mathit{answer}=";
    return;
  }

  _stream << "\\mathit{" << sym->name () << '}';
} // LaTeXOutput::symbol


// output a variable in LaTeX
// 23/10/2002 Manchester
void LaTeXOutput::var (Var v)
{
  _stream << "x_{" << v << '}';
} // LaTeXOutput::var


void Output::spaces (int times)
{
  repeat (' ', times);
} // Output::spaces


// LaTeX indentation
// 23/10/2002 Manchester
void LaTeXOutput::spaces (int times)
{
  repeat ('~', times);
} // LaTeXOutput::spaces


// pretty-print formula in the LaTeX syntax
// 11/10/2002 flight Manchester-Istambul
void LaTeXOutput::formula (const Formula& f, int indent)
{
  if ( fitsInOneLine(f) ) {
    formula (f);
    return;
  }

  Formula::Connective con (f.connective());

  switch (con) 
    {
    case Formula::ATOM:
      atom (f.atom());
      return;

    case Formula::AND:
    case Formula::OR: 
      {
	FormulaList args (f.args());
	ASS (f.args().length() >= 2);
	
	subformula (args.head(), indent+2, con);
	Iterator<Formula> arg (args.tail());
	while (arg.more()) {
	  _stream << ' ';
	  connective (con);
	  _stream << "\\\\\n";
	  spaces (indent);
	  subformula (arg.next(), indent+2, con);
	}
	return;
      }

  case Formula::IMP:
  case Formula::IFF:
  case Formula::XOR:
    subformula (f.left(), indent+2, con);
    _stream << ' ';
    connective (con);
    _stream << "\\\\\n";
    spaces (indent);
    subformula (f.right(), indent+2, con);
    return;

  case Formula::NOT:
    connective (con);
    subformula (f.uarg(), indent, con);
    return;

  case Formula::FORALL:
  case Formula::EXISTS:
    _stream << '(';
    connective (con);
    vars (f.vars());
    _stream << ")\\\\\n";
    spaces (indent+2);
    subformula (f.qarg(), indent+2, con);
    return;

  default:
    ASS (false);
  }
} // LaTeXOutput::formula (...)


// print formula in the LaTeX syntax
// 23/10/2002 Manchester
void LaTeXOutput::formula (const Formula& f)
{
  Output::formula (f);
} // LaTeXOutput::formula (const Formula& f)


// print formula in the LaTeX syntax
// 23/10/2002 Manchester
void NativeOutput::formula (const Formula& f)
{
  Output::formula (f);
} // NativeOutput::formula (const Formula& f)


/*
// 05/10/2002 Manchester, changed
void Refutation::outputStep (Output& out, Unit unit, int indent) const
{
  switch ( out.syntax() ) 
    {
    case TPTP: 
      {
	Iterator<Unit> ps (unit.parents());
	while (ps.more()) {
	  Unit parent (ps.next());
	  if (parent.unitType() == CLAUSE) {
	    spaces(2);
	    _stream << parent.clause() << '\n';
	  }
	}
	_stream << unit << '\n';
	return;
      }
} // Refutation::outputStep (..., Unit, ...)


void Term::output (ostream& str, Options::OutputSyntax syntax) const
{
  switch (tag()) 
    {
    case VAR:
      switch (syntax) 
	{
	case Options::OUTPUT_LATEX:
	  _stream << "x_{" << var() << '}';
	  return;
	}

    case NUMERIC:
      _stream << number();
      return;

    case COMPOUND:
      // non-variable term
      if (args().isEmpty()) {
	functor()->output(str,syntax);
	return;
      }

      switch (syntax) 
	{	
	case Options::OUTPUT_LATEX:
	  functor()->output(str,syntax);
	  _stream << '(';
	  break;
	}

      args().output(str, syntax);
      _stream << ')';
      return;
    }
} // Term::output


// pretty-printing
// 21/09/2002 Manchester
void Formula::output (ostream& str, 
		      Options::OutputSyntax syntax, 
		      Connective outer,
		      int indent) const 
{
  if (syntax != Options::OUTPUT_KIF &&
      parenthesesRequired (outer)) {
    _stream << '(';
    output (str, syntax, indent);
    _stream << ')';
  }
  else {
    output (str, syntax, indent);
  }
} // Formula::output (...)


// one-line output
// 21/09/2002 Manchester
void Formula::output (ostream& str, 
		      Options::OutputSyntax syntax, 
		      Connective outer) const 
{
  if (syntax != Options::OUTPUT_KIF &&
      parenthesesRequired (outer)) {
    _stream << '(';
    output (str, syntax);
    _stream << ')';
  }
  else {
    output (str, syntax);
  }
} // Formula::output (...)


// pretty-printing
// 21/09/2002 Manchester
// 05/10/2002 Manchester, latex output added
void Formula::output (ostream& str, Options::OutputSyntax syntax, int indent) const 
{
  if (printOneLine(syntax)) {
    output (str, syntax);
    return;
  }

  const char* con;
  int moreIndent;

  switch (syntax) 
    {
    case Options::OUTPUT_TPTP:
    case Options::OUTPUT_NATIVE: 
      {
	switch ( connective() ) 
	  {
	  case ATOM:
	    atom().output(str, syntax);
	    return;

	  case AND:
	    con = "&";
	    goto junction;

	  case OR: {
	    con = "|";
	  junction:
	    FormulaList fs (args());
	    fs.head().output (str, syntax, connective(), indent);
	    fs.pop();
	    while (! fs.isEmpty()) {
	      _stream << '\n';
	      for (int s = indent; s > 0; s--) {
		_stream << ' ';
	      }
	      _stream << con << ' ';
	      fs.head().output (str, syntax, connective(), indent+2);
	      fs.pop();
	    }
	    return;
	  }

	  case IMP:
	    con = "=>";
	    moreIndent = 3;
	    goto binary;

	  case IFF:
	    con = "<=>";
	    moreIndent = 4;
	    goto binary;

	  case XOR: 
	    {
	      con = "<~>";
	      moreIndent = 4;
	    binary:
	      left().output(str, syntax, connective(), indent);
	      _stream << '\n';
	      for (int s = indent; s > 0; s--) {
		_stream << ' ';
	      }
	      _stream << con << ' ';
	      right().output(str, syntax, connective(), indent+moreIndent);
	      return;
	    }

	  case NOT:
	    _stream << "~";
	    uarg().output(str, syntax, connective(), indent+1);
	    return;

	  case FORALL:
	    con = "forall";
	    goto quantifier;

	  case EXISTS:
	    con = "exists";
	  quantifier:
	    _stream << con << " [";
	    vars().output(str, syntax);
	    _stream << "] :\n";
	    indent += 2;
	    for (int s = indent; s > 0; s--) {
	      _stream << ' ';
	    }
	    qarg().output (str, syntax, connective(), indent);
	    return;

	  default:
	    ASS( false );
	    return;
	  }
      }

    case Options::OUTPUT_LATEX: 
      {
	switch ( connective() ) 
	  {
	  case ATOM:
	    atom().output(str, syntax);
	    return;

	  case AND:
	    con = " \\Vand ";
	    goto latex_junction;

	  case OR: {
	    con = " \\Vor ";
	  latex_junction:
	    FormulaList fs (args());
	    fs.head().output (str, syntax, connective(), indent);
	    fs.pop();
	    while (! fs.isEmpty()) {
	      _stream << "\\\\\n";
	      for (int s = indent; s > 0; s--) {
		_stream << '~';
	      }
	      _stream << con;
	      fs.head().output (str, syntax, connective(), indent+2);
	      fs.pop();
	    }
	    return;
	  }

	  case IMP:
	    con = " \\Vimp ";
	    goto binary;

	  case IFF:
	    con = " \\Viff ";
	    goto latex_binary;

	  case XOR: 
	    {
	      con = " \\Vxor ";
	    latex_binary:
	      left().output(str, syntax, connective(), indent);
	      _stream << '\n';
	      for (int s = indent; s > 0; s--) {
		_stream << '~';
	      }
	      _stream << con;
	      right().output(str, syntax, connective(), indent+2);
	      return;
	    }

	  case NOT:
	    _stream << "\\neg ";
	    uarg().output(str, syntax, connective(), indent+1);
	    return;

	  case FORALL:
	    con = "\\forall";
	    goto latex_quantifier;

	  case EXISTS:
	    con = "\\exists";
	  latex_quantifier:
	    _stream << con << " (";
	    vars().output(str, syntax);
	    _stream << ")\\\\\n";
	    indent += 2;
	    for (int s = indent; s > 0; s--) {
	      _stream << '~';
	    }
	    qarg().output (str, syntax, connective(), indent);
	    return;

	  default:
	    ASS( false );
	    return;
	  }
      }

    }
} // Formula::output (ostream&, Options::Syntax, int indent)


// one-line output
// 21/09/2002 Manchester
void Formula::output (ostream& str, Options::OutputSyntax syntax) const
{
  const char* con;

  switch (syntax) 
    {
    case Options::OUTPUT_TPTP:
    case Options::OUTPUT_NATIVE: 
      {
	switch ( connective() ) 
	  {
	  case ATOM:
	    atom().output(str, syntax);
	    return;

	  case AND:
	    con = " & ";
	    goto junction;

	  case OR: {
	    con = " | ";
	  junction:
	    FormulaList fs (args());
	    fs.head().output (str, syntax, connective());
	    fs.pop();
	    while (! fs.isEmpty()) {
	      _stream << con;
	      fs.head().output (str, syntax, connective());
	      fs.pop();
	    }
	    return;
	  }

	  case IMP:
	    con = "=>";
	    goto binary;

	  case IFF:
	    con = "<=>";
	    goto binary;

	  case XOR: 
	    {
	      con = "<~>";
	    binary:
	      left().output(str, syntax, connective());
	      _stream << ' ' << con << ' ';
	      right().output(str, syntax, connective());
	      return;
	    }

	  case NOT:
	    _stream << "~";
	    uarg().output(str, syntax, connective());
	    return;

	  case FORALL:
	    con = "forall";
	    goto quantifier;

	  case EXISTS:
	    con = "exists";
	  quantifier:
	    _stream << con << " [";
	    vars().output(str, syntax);
	    _stream << "] : ";
	    qarg().output (str, syntax, connective());
	    return;

	  default:
	    ASS( false );
	    return;
	  }
      }

    case Options::OUTPUT_KIF: 
      {
	switch ( connective() ) 
	  {
	  case ATOM:
	    atom().output(str, syntax);
	    return;

	  case AND:
	    _stream << "(and ";
	    goto kif_junction;

	  case OR: 
	    {
	      _stream << "(or ";
	    kif_junction:
	      Iterator<Formula> fs (args());
	      while (fs.more()) {
		fs.next().output (str, syntax);
		if (fs.more()) {
		  _stream << ' ';
		}
	      }
	      _stream << ')';
	      return;
	    }

	  case IMP:
	    con = "=>";
	    goto kif_binary;

	  case IFF:
	    con = "<=>";
	    goto kif_binary;

	  case XOR: { // there is no XOR in the KIF syntax
	    con = "not (<=>";
	  kif_binary:
	    _stream << '(' << con << ' ';
	    left().output(str, syntax);
	    _stream << ' ';
	    right().output(str, syntax);
	    _stream << ')';
	    if (connective() == XOR) {
	      _stream << ')';
	    }
	    return;
	  }

	  case NOT:
	    _stream << "(not ";
	    uarg().output(str, syntax);
	    _stream << ')';
	    return;

	  case FORALL:
	    con = "forall";
	    goto kif_quantifier;

	  case EXISTS:
	    con = "exists";
	  kif_quantifier:
	    _stream << '(' << con << " (";
	    vars().output(str, syntax);
	    _stream << ") ";
	    qarg().output (str, syntax);
	    _stream << ')';
	    return;

	  default:
	    ASS( false );
	    return;
	  }
      }
    }
} // Formula::output (ostream&, Options::Syntax)


ostream& operator << (ostream& str, Formula f)
{
  f.output (str, options->outputSyntax());

  return str;
} // operator << (ostream&, Formula)


// output a sequence of variables
// 21/09/2002 Manchester
void VarList::output (ostream& str, Options::OutputSyntax syntax) const
{
  ASS (isNonEmpty());

  char* varPrefix;
  char separator;

  switch (syntax) {
  case Options::OUTPUT_KIF:
    varPrefix = "?X";
    separator = ' ';
    break;

  case Options::OUTPUT_TPTP:
  case Options::OUTPUT_NATIVE:
    varPrefix = "X";
    separator = ',';
    break;

  default:
    ASS(false);
  }

  _stream << varPrefix << head();
  VarList vs (tail());
  while (vs.isNonEmpty()) {
    _stream << separator << varPrefix << vs.head();
    vs.pop();
  }
} // VarList::output

// 27/09/2002 Manchester
ostream& operator << (ostream& str, Unit unit)
{
  unit.output(str, options->outputSyntax());

  return str;
} // operator << (ostream& str, Unit u)


// pretty-printing
// 21/09/2002 Manchester
void Unit::output (ostream& str, 
		   Options::OutputSyntax syntax, 
		   int indent) const
{
  bool isClause = unitType() == CLAUSE;

  switch (syntax) 
    {
    case Options::OUTPUT_KIF:
      _stream << "<" << (isClause ? "clause" : "formula")
	  << " number='" << number() << "'>\n";
      for (int s = indent+2; s > 0; s--) {
	_stream << ' ';
      }
      if (isClause) {
	clause().output (str, syntax, indent+2);
      }
      else {
	formula().output (str, syntax, indent+2);
      }
      _stream << "\n";
      for (int s = indent; s > 0; s--) {
	_stream << ' ';
      }
      _stream << "</" << (isClause ? "clause" : "formula") << '>';
      return;
    
    case Options::OUTPUT_TPTP:
    case Options::OUTPUT_NATIVE: 
      {
	_stream << number() << ". ";
	if (isClause) {
	  clause().output (str, syntax, indent+4);
	}
	else {
	  formula().output (str, syntax, indent+4);
	}
	// output parents
	_stream << " [";
	Iterator<Unit> ps (parents());
	while (ps.more()) {
	  _stream << ps.next().number();
	  if (ps.more()) {
	    _stream << ',';
	  }
	}
	_stream << ']';
	return;
      }

    case Options::OUTPUT_LATEX: 
      {
	for (int s = indent; s > 0; s--) {
	  _stream << ' ';
	}
	_stream << "\\VampireUnit{"
	    << number() << "}{";
	if (isClause) {
	  clause().output (str, syntax);
	}
	else {
	  formula().output (str, syntax, indent+4);
	}
	_stream << "}{";
	// output parents
	Iterator<Unit> ps (parents());
	while (ps.more()) {
	  _stream << ps.next().number();
	  if (ps.more()) {
	    _stream << ',';
	  }
	}
	_stream << '}';
	return;
      }
    }
} // Unit::output (...)


// one-line output
// 21/09/2002 Manchester
void Unit::output (ostream& str, Options::OutputSyntax syntax) const
{
  bool isClause = unitType() == CLAUSE;

  switch (syntax) {
  case Options::OUTPUT_KIF:
    _stream << "<" << (isClause ? "clause" : "formula")
	<< " number='" << number() << "'>";
    if (isClause) {
      clause().output (str, syntax);
    }
    else {
      formula().output (str, syntax);
    }
    _stream << '<' << (isClause ? "clause" : "formula") << '>';
    return;
    
  case Options::OUTPUT_TPTP:
  case Options::OUTPUT_NATIVE: {
    _stream << number() << ". ";
    if (isClause) {
      clause().output (str, syntax);
    }
    else {
      formula().output (str, syntax);
    }
    // output parents
    _stream << " [";
    Iterator<Unit> ps (parents());
    while (ps.more()) {
      _stream << ps.next().number();
      if (ps.more()) {
	_stream << ',';
      }
    }
    _stream << ']';
    return;
  }
}
} // Unit::output (...)


ostream& operator << ( ostream& str, UnitChain& units )
{
  UnitChain::Iterator us (units);

  while ( us.more() ) {
    Unit unit (us.next ());

    _stream << unit.unitType () 
        << '(';

    if (unit.rule() == IR_INPUT && unit.name()) {
      _stream << unit.name ();
    }
    else {
      _stream << "unit_" << unit.number();
    }

    _stream << ','
        << unit.inputType()
        << ",\n";

    switch ( unit.unitType() )  {
      case FORMULA:
        unit.formula().output (str, Options::OUTPUT_TPTP, 4);
        break;

      case CLAUSE:
        unit.clause().output (str, Options::OUTPUT_TPTP, 4);
        break;
    }
    _stream << " ).\n\n";
  }

  return str;
} // operator << ( ostream& str, Unit::chain& c )


// pretty-printing as a clause
// 21/09/2002 Manchester
void LiteralList::output (ostream& str, 
			  Options::OutputSyntax syntax, 
			  int indent) const
{
  switch (syntax) 
    {
    case Options::OUTPUT_TPTP: 
      {
	if (isEmpty ()) {
	  _stream << "[]";
	  return;
	}

	_stream << '[';
 
	LiteralList lits (*this);
	while (lits.isNonEmpty()) {
	  lits.head().output (str, syntax);
	  lits.pop();
	  if (lits.isEmpty()) {
	    _stream << "]";
	  }
	  else {
	    _stream << ",\n";
	    for (int s = indent; s >= 0; s--) {
	      _stream << ' ';
	    }
	  }
	}
	return;
      }

    case Options::OUTPUT_NATIVE: 
      {
	if (isEmpty()) {
	  _stream << '#';
	  return;
	}

	LiteralList lits (*this);
	while (lits.isNonEmpty()) {
	  lits.head().output (str, syntax);
	  lits.pop();
	  if (lits.isNonEmpty()) {
	    _stream << " |\n";
	    for (int s = indent; s >= 0; s--) {
	      _stream << ' ';
	    }
	  }
	}
	return;
      }

    case Options::OUTPUT_LATEX: 
      output (str, syntax);
      return;
    }
} // LiteralList::output


// pretty-printing as a clause
// 21/09/2002 Manchester
void LiteralList::output (ostream& str, Options::OutputSyntax syntax) const
{
  switch (syntax) 
    {
    case Options::OUTPUT_TPTP: 
      {
	if (isEmpty ()) {
	  _stream << "[]";
	  return;
	}

	_stream << '[';
 
	LiteralList lits (*this);
	while (lits.isNonEmpty()) {
	  lits.head().output (str, syntax);
	  lits.pop();
	  if (lits.isEmpty()) {
	    _stream << "]";
	  }
	  else {
	    _stream << ", ";
	  }
	}
	return;
      }

    case Options::OUTPUT_KIF: 
      {
	if (isEmpty()) { // empty clause
	  _stream << "FALSE";
	  return;
	}
	// there is at least one literal
	if (tail().isEmpty()) { // one-literal clause
	  head().output (str, syntax);
	  return;
	}

	_stream << "(or ";

	Iterator<Literal> lits (*this);
	while (lits.more()) {
	  lits.next().output (str, syntax);
	  if (lits.more()) {
	    _stream << ' ';
	  }
	}
	_stream << ")";
	return;
      }
    }
} // LiteralList::output


// one-line output
// 21/09/2002 Manchester
void Literal::output (ostream& str, Options::OutputSyntax syntax) const
{
  if (positive()) {
    atom().output(str, syntax);
    return;
  }

  switch (syntax) 
    {
    case Options::OUTPUT_KIF:
      _stream << "(not ";
      atom().output(str, syntax);
      _stream << ')';
      return;
} // Literal::output (...)


// pretty-printing
// 11/09/2002 Manchester, changed
// 21/09/2002 Manchester, changed
void Clause::output (ostream& str, 
		     Options::OutputSyntax syntax, 
		     int indent) const
{
  literals().output (str, syntax, indent);
} // Clause::output


// one-line output
// 21/09/2002 Manchester
void Clause::output (ostream& str, Options::OutputSyntax syntax) const
{
  literals().output (str, syntax);
} // Clause::output


// one-line output
// 11/09/2002 Manchester, changed
// 04/10/2002 Manchester, changed to use output (...)
ostream& operator << (ostream& str, Clause c) 
{
  c.output (str,options->outputSyntax());

  return str;
} // Clause::operator <<


// 01/05/2002, Manchester
ostream& operator << ( ostream& str, Problem& p )
{
  _stream << p.units ();

  return str;
} // operator << ( ostream& str, Problem* p )

*/

void TPTPOutput::pretty (const Refutation&, int answerNumber) {}

void TPTPOutput::inference (const Unit& unit, int indent) {}

void TPTPOutput::clause (const Clause& unit) {}
void KIFOutput::clause (const Clause& unit) {}

void TPTPOutput::clause (const Clause& unit, int indent) {}

void TPTPOutput::formula (const Formula& unit, int indent) {}

void TPTPOutput::formula (const Formula& unit) {}
void KIFOutput::formula (const Formula& unit) {}

void Output::connective (Formula::Connective) {}


// this function is for debugging only. It outputs f using the native
// syntax
ostream& operator<< (ostream& str, const Formula& f)
{
  TRACER ("ostream& operator<< (ostream& str, const Formula& f)");

  Output* out = Output::create (NATIVE, str);
  out->formula(f);
  delete out;

  return str;
} // ostream& operator<< (ostream& str, const Formula& f)
