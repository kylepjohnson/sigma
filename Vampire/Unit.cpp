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
//  file Unit.cpp
//

#include <string.h>


#include "Unit.hpp"


long Unit::Data::_lastNumber = 0;


// build a list of units from a list of formulas using the input type tp
// 26/09/2002 Manchester
UnitList::UnitList (InputType tp, const FormulaList& formulas)
{
  FormulaList fs (formulas);
  while (fs.isNonEmpty()) {
    push (Unit (0, tp, fs.head()));
    fs.pop();
  }
} // UnitList::UnitList (InputType tp, const FormulaList& formulas)


// assignment operator
// 30/08/2002 Torrevieja
void Unit::operator = (const Unit& t)
{
  if (t._data) {
    t._data->ref ();
  }

  if (_data) {
    _data->deref ();
  }

  _data = t._data;
} // Unit::operator =


ostream& operator << ( ostream& str, InputType t )
{
  switch ( t ) {
    case AXIOM:
      str << "axiom";
      break;

    case CONJECTURE:
      str << "conjecture";
      break;

    case HYPOTHESIS:
      str << "hypothesis";
      break;
  }

  return str;
} // operator << ( ostream& str, InputType t )


ostream& operator << ( ostream& str, UnitType t )
{
  switch ( t ) {
    case CLAUSE:
      str << "input_clause";
      break;

    case FORMULA:
      str << "input_formula";
      break;
  }

  return str;
} // operator << ( ostream& str, UnitType t )


Unit::Data::Data (char* name, InputType intype, const Clause& c)
  :
  _counter (1),
  _untype (CLAUSE),
  _intype (intype),
  _rule (IR_INPUT),
  _number (++ _lastNumber),
  _name (name),
  _clause (c),
  _defType (DT_NONE)
{
} // Unit::Data::Data


Unit::Data::Data ( char* name, InputType intype, const Formula& f )
  :
  _counter (1),
  _untype (FORMULA),
  _intype (intype),
  _rule (IR_INPUT),
  _number (++ _lastNumber),
  _name (name),
  _formula (f),
  _defType (DT_NONE)
{
} // Unit::Unit


// unit obtained by an inference with one parent
// 07/05/2002 Manchester
// 30/08/2002 Torrevieja, changed
Unit::Data::Data (InferenceRule ir, const Formula& f, const Unit& parent)
  :
  _counter (1),
  _untype (FORMULA),
  _intype (parent.inputType()),
  _rule ( ir ),
  _number (++ _lastNumber),
  _parents (parent),
  _formula (f),
  _defType (DT_NONE)
{
} // Unit::Unit


// unit obtained by an inference with one parent
// 07/05/2002 Manchester
Unit::Data::Data (InferenceRule ir, const Clause& c, const Unit& parent)
  :
  _counter (1),
  _untype (CLAUSE),
  _intype (parent.inputType()),
  _rule (ir),
  _number (++ _lastNumber),
  _parents (parent),
  _clause (c),
  _defType (DT_NONE)
{
} // Unit::Unit


// unit obtained by an inference with several parent
// WARNING: temporary there is no inference rule associated
// 29/09/2002 Manchester
Unit::Data::Data (const Clause& c, const UnitList& parents)
  :
  _counter (1),
  _untype (CLAUSE),
  _intype (parents.head().inputType()), // temporary
  _rule (IR_UNKNOWN),                   // temporary
  _number (++ _lastNumber),
  _parents (parents),
  _clause (c),
  _defType (DT_NONE)
{
} // Unit::Unit


// unit obtained by an inference with several parent
// WARNING: temporary there is no inference rule associated
// 04/10/2002 Manchester
Unit::Data::Data (const Formula& f, const UnitList& parents)
  :
  _counter (1),
  _untype (FORMULA),
  _intype (parents.head().inputType()), // temporary
  _rule (IR_UNKNOWN),                   // temporary
  _number (++ _lastNumber),
  _parents (parents),
  _formula (f),
  _defType (DT_NONE)
{
} // Unit::Unit


// 11/09/2002 Manchester, changed
Unit::Data::~Data ()
{
  TRACER( "Unit::Data::~Data" );
  ASS (_counter == 0);

  if (_rule == IR_INPUT) {
    delete [] _name;
  }
} // Unit::~Unit


/*
// Warning: only resets an already existing formula
// 11/05/2002 Manchester
void Unit::formula ( Formula* f ) 
{
  ASS ( debug_problem, _formula && 
                          unitType() == FORMULA );

  f->inc();
  _formula->dec(); 
  _formula = f;
} // Unit::formula
*/

/*
int Unit::countOccurrences ( const PSymbol* p ) const
{
  switch ( unitType() ) {
    case CLAUSE:
      return clause()->countOccurrences (p);

    case FORMULA:
      return formula()->countOccurrences (p);

    default:
      ASS( false );
      return false;
  }
} // Unit::countOccurrences


int Unit::countOccurrences ( const FSymbol* p ) const
{
  switch ( unitType() ) {
    case CLAUSE:
      return clause()->countOccurrences (p);

    case FORMULA:
      return formula()->countOccurrences (p);

    default:
      ASS( false );
      return false;
  }
} // Unit::countOccurrences


bool Unit::occurs ( const FSymbol* p ) const
{
  ASS( unitType() == CLAUSE );
  
  return clause()->occurs (p);
} // Unit::occurs


// 03/06/2001
bool Unit::occurs ( const PSymbol* p ) const
{
  switch ( unitType() ) {
    case CLAUSE:
      return clause()->occurs (p);

    case FORMULA:
      return formula()->occurs (p);

    default:
      ASS( false );
      return false;
  }
} // Unit::occurs


Var* Unit::isFunctionalityAxiom ( const PSymbol* pred, FSymbol*& fun, int argNo, Atom*& atom ) const
{
  ASS( unitType() == CLAUSE );

  return clause()->isFunctionalityAxiom (pred,fun,argNo, atom);
} // Unit::isFunctionalityAxiom


// the unit is P(x,y) & P(x,z) -> y = z
bool Unit::isUniquenessAxiom ( PSymbol*& pred, int& argNo ) const
{
  ASS( unitType() == CLAUSE );

  return clause()->isUniquenessAxiom (pred, argNo);
} // Unit::isUniquenessAxiom


// the unit is f(X,y,Z) = f(X,u,Z) -> y = u
bool Unit::isCancellationAxiom ( FSymbol*& fun, int& argNo ) const
{
  ASS( unitType() == CLAUSE );

  return clause()->isCancellationAxiom (fun, argNo);
} // Unit::isCancellationAxiom


// replace the cancellation axiom 
//   f(X,y,Z) = f(X,u,Z) -> y = u
// by 
//   i(f(X,y,Z),X,Z) = y
//void Unit::replaceCancellation ( const FSymbol* fun,int argNo,FSymbol* inv, ostream& report )
//{
//  ASS( unitType() == CLAUSE );
//
//  clause()->replaceCancellation (argNo, inv);
//
//  report << "inference(" << clause() << "," << name() 
//         << "_rcbi,[" << name () << "],[rcbi]]).\n";
//  _name += "_rcbi"; // replace cancellation by inverse
//} // Unit::replaceCancellation


//void Unit::apply ( const ReplaceCode& code, const Unit* def, ostream& report ) 
//{
//  ASS( unitType() == CLAUSE );
//
//  if ( code.apply (clause()) ) {
//    report << "inference(" << clause() << "," << name() 
//           << "_dapp,[" << name () << "," << def->name () << "],[dapp]]).\n";
//    _name += "_dapp"; // replace cancellation by inverse
//  }
//} // Unit::apply


int Unit::maxLitWeight ( const ReplaceCode& code ) const
{
  ASS( unitType() == CLAUSE );

  return code.maxLitWeight (clause());
} // Unit::maxLitWeight


int Unit::weight () const 
{
  switch ( unitType() ) {
    case CLAUSE:
      return clause()->weight ();

    case FORMULA:
      return formula()->weight ();

    default:
      ASS( false );
      return false;
  }
} // Unit::weight


Term* Unit::isDefinition ( Term* & rhs ) const
{
  ASS( unitType() == CLAUSE );

  return clause()->isDefinition (rhs);
} // Unit::isDefinition
*/


/*
// 03/06/2001 Manchester
Atom* Unit::isPredicateDefinition ( Formula& rhs ) const
{
  TRACER ( "Unit::isPredicateDefinition" );

  switch ( unitType() ) {
    case CLAUSE:
      return 0;

    case FORMULA:
      return formula().isPredicateDefinition (rhs);

    default:
      ASS ( false );
      return 0;
  }
} // Unit::isPredicateDefinition
*/


/*
Term* Unit::isLinearDefinition ( Term* & rhs ) const
{
  ASS( unitType() == CLAUSE );

  return clause()->isLinearDefinition (rhs);
} // Unit::isLinearDefinition


bool Unit::hasFunctionSymbols () const
{
  ASS( unitType() == CLAUSE );

  return clause()->hasFunctionSymbols ();
} // Unit::hasFunctionSymbols


bool Unit::hasEquality () const
{
  ASS( unitType() == CLAUSE );

  return clause()->hasEquality ();
} // Unit::hasEquality


bool Unit::isHorn () const
{
  ASS( unitType() == CLAUSE );

  return clause()->isHorn ();
} // Unit::isHorn


bool Unit::isUnit () const
{
  ASS( unitType() == CLAUSE );

  return clause()->isUnit ();
} // Unit::isUnit


bool Unit::isPropositional () const
{
  ASS( unitType() == CLAUSE );

  return clause()->isPropositional ();
} // Unit::isPropositional


// 06/06/2001 made from pureEquality
bool Unit::hasNonEquality () const
{
  ASS( unitType() == CLAUSE );

  return clause()->hasNonEquality ();
} // Unit::hasNonEquality


int Unit::numberOfLiterals () const
{
  ASS( unitType() == CLAUSE );

  return clause()->length ();
} // Unit::numberOfLiterals


//void Unit::replaceRelationByFunction ( PSymbol* pred, FSymbol* fun, int argNo, int* vs, ostream& report )
//{
//  if ( clause()->replaceRelationByFunction (pred, fun, argNo, vs) ) {
//    report << "inference(" << clause() << "," << name() 
//           << "_rrbf,[" << name () << ",replace_" << pred << "_by_" << fun  << "],[rrbf]]).\n";
//    _name += "_rrbf";
//
//    removeVarEquality (report);
//  }
//} // Unit::replaceRelationByFunction


Unit* Unit::resolveOn ( const PSymbol* f, Unit* unit, ostream& report )
{
  static int res_number = 0;
  static const char prefix [] = "res_";
  static const int prefixLength = strlen (prefix);

  // making name for the new unit
  char* newName = new char [prefixLength + 10];
  strcpy ( newName, prefix );
  char* cursor = newName + prefixLength;
  Int::toString ( res_number, cursor );
  res_number++;

  Clause* c = clause()->resolveOn (f, unit->clause());
  Unit* result = new Unit ( newName, inputType(), c );
  report << "inference(" << c << "," << newName << ",[" 
         << name () <<"," << unit->name () << "],[br]).\n";
  
  return result;
} // Unit::resolveOn


Compare Unit::compare ( const Unit* u ) const
{
  // only units of the same type can be compared at this moment
  ASS ( _untype == u->_untype );

  switch ( _untype ) {
    case CLAUSE:
      return clause()->compare(u->clause());

    case FORMULA:
      return formula()->compare(u->formula());

    default:
      ASS ( false );
  }
} // Unit::compare


//bool Unit::removeVarEquality ( ostream& report )
//{
//  // only clauses can be treated at this moment
//  ASS ( _untype == CLAUSE );
//
//  if ( clause()->removeVarEquality() ) {
//    report << "inference(" << clause() << "," << name() << "_rve,[" << name () << "],[rve]).\n";
//    _name += "_rve";
//    return true;
//  }
//
//  return false;
//} // Unit::removeVarEquality


bool Unit::isTautology ( ostream& report ) 
{
  // only clauses can be treated at this moment
  ASS ( _untype == CLAUSE );

  if ( clause()->isTautology() ) {
    report << "prepro(removed_as_tautology," << name() << "," << clause() << ").\n";
    return true;
  }

  return false;
} // Unit::isTautology


int Unit::maxLitWeight ()
{
  // only clauses can be treated at this moment
  ASS ( _untype == CLAUSE );

  return clause()->maxLitWeight();
} // Unit::maxLitWeight


void Unit::splitLargeArity ( const PSymbol* p, FSymbol::List* newFs )
{
  // only clauses can be treated at this moment
  ASS ( _untype == CLAUSE );

  clause()->splitLargeArity (p, newFs);
} // Unit::splitLargeArity


void Unit::splitLargeArity ( const FSymbol* f, FSymbol::List* newFs )
{
  // only clauses can be treated at this moment
  ASS ( _untype == CLAUSE );

  clause()->splitLargeArity (f, newFs);
} // Unit::splitLargeArity


// no time to tab the split now
Unit::List* Unit::negativeEqualitySplit ( int thresholdWeight, ostream& tab )
{
  // only clauses can be treated at this moment
  ASS ( _untype == CLAUSE );

  Clause::List* cs = clause()->negativeEqualitySplit( thresholdWeight );

  List* newUnits = Unit::List::empty ();

  Clause::List::Iterator cls ( cs );
  while ( cls.more() ) {
    static int number = 0;
    static char name [] = "term_def_          ";
    Int::toString ( ++number, name+9 );
    char* nm = new char [strlen(name)+1];
    strcpy (nm, name);
    Unit::List::push ( new Unit (nm, AXIOM, cls.next()), newUnits );
  }

  cs->destroy ();

  return newUnits;
} // Unit::negativeEqualitySplit
*/

// the unit is one of the equality axioms
// 03/06/2001
// 29/04/2002 added check for clauses
// 29/08/2002 Torrevieja, changed
// 11/09/2002 Manchester, changed
bool Unit::isEqualityAxiom () const
{
  TRACER("Unit::isEqualityAxiom");

  switch ( unitType() ) {
    case CLAUSE:  
      return clause().isEqualityAxiom ();

    case FORMULA:
      return formula().isEqualityAxiom ();

    default:
      ASS ( false );
      return 0;
  }
} // Unit::isEqualityAxiom


/*
// 04/06/2001
// 11/05/2002 Manchester, changed completely
void Unit::skolemize (ClauseList&)
{
  switch ( unitType() ) {
    case CLAUSE:  
      return;

    case FORMULA: {
//      bool polarity = true;
//
//      switch ( inputType() ) {
//
//        case AXIOM:
//        case HYPOTHESIS:
//          break;
//
//        case CONJECTURE:
//          polarity = false;
//          break;
//      }
//      
//      // skolemize
//      Clause::List* newClauses = formula ()->skolemize (polarity);
//      // to generate names for the new units
//      int appendix = 0; // to append to the unit name
//      char app [10];  
//      app[0] = '_';
//      // create new units and append them to appendTo
//      Clause::List::Iterator cls (newClauses);
//      while ( cls.more() ) {
//        // make a new name by appending the current name with appendix
//        Int::toString (appendix++,app+1);
//        // create a new unit and append it
//        Unit::List::push ( new Unit (name(), app, inputType(), cls.next()),
//                           appendTo );
//      }
//      int result = newClauses->length ();
//      newClauses->destroy ();
//      return result;
    }  

    default:
      ASS ( false );
      return 0;
  }  
} // Unit::skolemize
*/


/*
// 04/06/2001
void Unit::polarities ( const PSymbol* p, int& pol ) const
{
  switch ( unitType() ) {
    case CLAUSE:  
      clause()->polarities (p,pol);
      return;

    case FORMULA:
      switch ( inputType() ) {
        case AXIOM:
        case HYPOTHESIS:
          formula()->polarities (p,true,pol);
          return;
        case CONJECTURE:
          formula()->polarities (p,false,pol);
          return;
      }

    default:
      ASS ( false );
      return;
  }
} // Unit::polarities


// 04/06/2001
// the unit is a definition (P(X) <=> rhs)
// is pos, then it should be changed into (P(X)->rhs), otherwise into
// (rhs->P(X)), i.e. pos is the polarity of rsh in the new formula
//void Unit::makeHalfDefinition ( bool positive, const Formula* rhs )
//{
//  ASS ( _untype == FORMULA );
//  ASS ( _intype != HYPOTHESIS );
//  
//  _name += positive ? "_neg" : "_pos";
//  formula()->makeHalfDefinition ( positive, rhs );
//} // Unit::makeHalfDefinition


// 06/06/2001
bool Unit::isGoal () const
{
  return inputType () == CONJECTURE;
} // Unit::isGoal


*/

// true if the unit is a formula with free variables
// 06/05/2002 Manchester
bool Unit::formulaWithFreeVars () const
{
  return unitType() == FORMULA &&
         formula().hasFreeVars ();
} // Unit::formulaWithFreeVars


// If the unit is not a definition
//    return false
// If the unit is a definition in the right form, return true
//    change the _defType tag of the unit and set def to the unit itself
// If the unit is a definition but e.g. with lhs and rhs swapped then return
//    true and set def to a new unit obtained by transforming the original one
// 07/05/2002 Manchester
// 29/08/2002 Torrevieja, changed
// 04/09/2002 Bolzano, changed
// 11/09/2002 Manchester, changed
bool Unit::makeDefinition (Unit& def)
{
  TRACER ( "Unit::makeDefinition" );

  switch ( unitType() ) {
    case CLAUSE: {
      Clause c (clause());
      Term lt;
      Term rt;
      if (! c.isFunctionDefinition (lt, rt)) {
        return false;
      }

      // now either lhs or lt is non-zero
      Unit current (*this);

      // now we have a definition
      ASS( ! c.isEmpty() );

      // check that the lt and rt are in the right order
      Atom a (c.literals().head().atom ());
      ASS( a.isEquality() );
      TermList ts (a.args());
      if (ts.head() == lt) {
        // the order is correct
        current.setDefType (DT_FUN);
	def = current;
        return true;
      }

      // the order is wrong, build a new unit
      Literal l (true, Atom (ts.second(), ts.head()));
      LiteralList lits(l);
      Clause c1 (lits);

      // increase counter since the unit will be replaced by a new one
      Unit result (IR_REORDER_EQ, c1, current);
      result.setDefType (DT_FUN);
      def = result;
      return true;
    }

    case FORMULA: {
      Formula f (formula());
      Atom lhs;
      Formula rhs;
      Term lt;
      Term rt;
      bool isPredDef = f.isPredicateDefinition (lhs,rhs);
      if (! isPredDef) {
        if (! f.isFunctionDefinition (lt, rt)) {
          return false;
        }
      }
      // now it is a definition
      Unit current (*this);

      // now we have a definition
      ASS( 
              f.connective() == Formula::IFF || 
              f.connective() == Formula::ATOM || 
              f.connective() == Formula::FORALL ) 
      // check if the unit needs flattening
      if ( f.connective() == Formula::FORALL &&
           f.qarg().connective() == Formula::FORALL ) {
        // the formula needs flattening, first collect outermost variables in vs
        VarList vs;
        Formula g (f);
        while (g.connective() == Formula::FORALL) {
          vs.append (g.vars());
          g = g.qarg ();
        }
        // build a new flattened formula
        f = Formula (Formula::FORALL, vs, g);
        current = Unit (IR_FLATTEN, f, current);
      }
      if (isPredDef) { // it was a predicate definition
        // now current is the (possibly new) unit with the flattened formula f 
        // check that the lhs and rhs of f are in the right order
        ASS( f.connective() == Formula::IFF || 
	                 f.connective() == Formula::FORALL );
        Formula g (f.connective() == Formula::IFF ? f : f.qarg());
        // g is the iff-part of f
        ASS( g.connective() == Formula::IFF);
        if (g.right() == rhs) {
          // the order is correct
          current.setDefType (DT_PRED);
	  def = current;
          return true;
        }
        // the order is wrong, build a new unit
        Formula g1 (Formula::IFF, g.right(), g.left());
        if (f.connective() == Formula::FORALL) {
          g1 = Formula (Formula::FORALL, f.vars(), g1);
        }

        current = Unit (IR_PERMUT, g1, current);
        current.setDefType (DT_PRED);
	def = current;
        return true;
      }
      // it is a function definition
      // now current is the (possibly new) unit with the flattened formula f
      // check that the lt and rt are in the right order
      Formula g (f.connective() == Formula::FORALL ? f.qarg() : f);
      ASS (g.connective() == Formula::ATOM);
      Atom a (g.atom ());
      ASS (a.isEquality());
      TermList ts (a.args());
      if (ts.head() == lt) {
        // the order is correct
        current.setDefType (DT_FUN);
        def = current;
	return true;
      }
      // the order is wrong, build a new unit
      Formula g1 (Atom (ts.second(), ts.head()));
      if (f.connective() == Formula::FORALL) {
        g1 = Formula (Formula::FORALL, f.vars(), g1);
      }

      // increase counter since the unit will be replaced by a new one
      current = Unit (IR_REORDER_EQ, g1, current);
      current.setDefType (DT_FUN);
      def = current;
      return true;
    }

    default:
      ASS ( false );
      return 0;
  }
} // Unit::makeDefinition


/*
// flatten the unit's formula
// 27/06/2002 Manchester
void Unit::flatten ()
{
  TRACER ( "Unit::flatten" );

  switch ( unitType () ) {
    case FORMULA:
      formula ( formula()->flatten() );
      return;

    case CLAUSE:
      return;
  }
} // Unit::flatten
*/


