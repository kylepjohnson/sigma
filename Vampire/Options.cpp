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
//  file options.cpp
//  implements Vampire options
//  06/06/2001 Manchester, completely rewritten
//


#include <string>
#include <cstdlib>
#include <iostream>
#include <climits>

#include "assert.hpp"
#include "Options.hpp"
#include "Random.hpp"


Options* options = 0;


// default options
Options::Options ()
  :
  _mode (Opt::MODE, KIF, "mode"),
  _tab (Opt::TAB, 0, "tab"),
  _inputFile (Opt::INPUT_FILE, 0, "input_file"),
  _randomSeed (Opt::RANDOM_SEED, Random::seed(), "random_seed"),
  
  // input/output options
  _statistics (Opt::STATISTICS, STATISTICS_DETAILED, "statistics"),
  _inputSyntax (Opt::INPUT_SYNTAX, INPUT_TPTP, "input_syntax"),
  _outputSyntax (Opt::OUTPUT_SYNTAX, NATIVE, "output_syntax"),
  _silent (Opt::SILENT, false, "silent"),
  _showGen (Opt::SHOW_GEN, false, "show_generated"),
  _showPassive (Opt::SHOW_PASSIVE, false, "show_passive"),
  _showActive (Opt::SHOW_ACTIVE, false, "show_active"),
  _showPrepro (Opt::SHOW_PREPRO, false, "show_prepro"),
  _showProfile (Opt::SHOW_PROFILE, false, "show_profile"),
  _showInput (Opt::SHOW_INPUT, false, "show_input"),
  _showOptions (Opt::SHOW_OPTIONS, false, "show_options"),
  _showWeightLimitChanges (Opt::SHOW_WEIGHT_LIMIT_CHANGES, false, "show_weight_limit_changes"),
  // preprocessor options
  _removeUnusedDefs (Opt::REMOVE_UNUSED_DEF, true, "remove_unused_defs"),
  _arityCheck (Opt::ARITY_CHECK, true, "arity_check"),
  _noFreeVars (Opt::NO_FREE_VARS, true, "no_free_vars"),
  _miniscope (Opt::MINISCOPE, true, "miniscope"),
  _outputEqualityInfix (Opt::OUTPUT_EQUALITY_INFIX, false, "output_equality_infix"),
  _include (Opt::INCLUDE, 0, "include"),
  // Kernel's options
  _mainAlg (Opt::MAIN_ALG, LRS, "man_alg"),
  _splitting (Opt::SPLITTING, SPLIT_OFF, "splitting"),
  _splittingStyle (Opt::SPLITTING_STYLE, PARALLEL, "splitting_style"),
  _splittingWithNaming (Opt::SPLITTING_WITH_NAMING, true, "splitting_with_naming"),
  _selection (Opt::SELECTION, P_4, "selection"),
  _literalComparisonMode (Opt::LITERAL_COMPARISON_MODE, NORMAL, "literal_comparison_mode"),
  _inheritedNegativeSelection (Opt::INHERITED_NEGATIVE_SELECTION, false, "inherited_negative_selection"),
  _forwardSubsumptionResolution (Opt::FORWARD_SUBSUMPTION_RESOLUTION, true, "forward_subsumption_resolution"),
  _forwardDemodulation (Opt::FORWARD_DEMODULATION, DEMODULATION_ALL, "forward_demodulation"),
  _fdOnSplittingBranches (Opt::FD_ON_SPLITTING_BRANCHES, true, "fd_on_splitting_branches"),
  _backwardDemodulation (Opt::BACKWARD_DEMODULATION, DEMODULATION_ALL, "backward_demodulation"),
  _bdOnSplittingBranches (Opt::BD_ON_SPLITTING_BRANCHES, true, "bd_on_splitting_branches"),
  _forwardSubsumption (Opt::FORWARD_SUBSUMPTION, SUBSUMPTION_MULTISET, "forward_subsumption"),
  _backwardSubsumption (Opt::BACKWARD_SUBSUMPTION, SUBSUMPTION_MULTISET, "backward_subsumption"),
  _orphanMurder (Opt::ORPHAN_MURDER, false, "orphan_murder"),
  _elimDef (Opt::ELIM_DEF, 0, "elim_def"),
  _lrsFirstTimeCheck (Opt::LRS_FIRST_TIME_CHECK, 5, "lrs_first_time_check"),
  _lrsFirstMemoryCheck (Opt::LRS_FIRST_MEMORY_CHECK, 100, "lrs_first_memory_check"),
  _maxInferenceDepth (Opt::MAX_INFERENCE_DEPTH, 2147483647, "max_inference_depth"),
  _symbolPrecedence (Opt::SYMBOL_PRECEDENCE, BY_ARITY, "symbol_precedence"),
  _headerPrecedenceKinky (Opt::HEADER_PRECEDENCE_KINKY, false, "header_precedence_kinky"),
  _noInferencesBetweenRules (Opt::NO_INFERENCES_BETWEEN_RULES, false, "no_inferences_between_rules"),
  _timeLimit (Opt::TIME_LIMIT, LONG_MAX, "time_limit"),
  _memoryLimit (Opt::MEMORY_LIMIT, 300, "memory_limit"),
  _allocationBufferSize (Opt::ALLOCATION_BUFFER_SIZE, 10, "allocation_buffer_size"),
  _allocationBufferPrice (Opt::ALLOCATION_BUFFER_PRICE, 2.0, "allocation_buffer_price"),
  _inequalitySplitting (Opt::INEQUALITY_SPLITTING, 0, "inequality_splitting"),
  _maxSkolem (Opt::MAX_SKOLEM, 4096, "max_skolem"),
  _maxActive (Opt::MAX_ACTIVE, LONG_MAX, "max_active"),
  _maxPassive (Opt::MAX_PASSIVE, LONG_MAX, "max_passive"),
  _maxWeight (Opt::MAX_WEIGHT, LONG_MAX, "max_weight"),
  _ageWeightRatio (Opt::AGE_WEIGHT_RATIO, 5, "age_weight_ratio"),
  _proof (Opt::PROOF, PROOF_FULL, "proof"),
  _testId (Opt::TEST_ID, "unspecified_test", "test_id"),
  _testNumber (Opt::TEST_NUMBER, Random::integer(), "test_number"),
  _paramodulation (Opt::PARAMODULATION, true, "paramodulation"),
  _symbolWeightByArity (Opt::SYMBOL_WEIGHT_BY_ARITY, 0, "symbol_weight_by_arity"),
  _termWeightingScheme (Opt::TERM_WEIGHTING_SCHEME, TWS_UNIFORM, "term_weighting_scheme"),
  _simplificationOrdering (Opt::SIMPLIFICATION_ORDERING, KBO, "simplification_ordering"),
  _nongoalWeightCoefficient (Opt::NONGOAL_WEIGHT_COEFFICIENT, 1.0, "nongoal_weight_coefficient" ),
  _selectedEqualityWeightCoefficient (Opt::SELECTED_EQUALITY_WEIGHT_COEFFICIENT, 1.0, "selected_equality_weight_coefficient" ),
  _sos (Opt::SOS, false, "sos")
{
} // Options::Options


// correct options using the command line arguments
void Options::correct (int argc, char* argv [])
{
  TRACER ( "Options::correct" );

  char** last = argv + argc;
  char** current = argv + 1;

  while (current != last) {
    Opt* opt = find (*current); // find option by name
    if (opt) { // found
      current++; // advance current, it should now point to the value of the option
      if (current == last) { // wrong, no value
        cerr << "Error in command line after " << current[-1] << "\n";
	throw MyException ("Terminated by error in command line");
      }
      // value found
      if (! opt->set (*current)) { // option not set successfully
        cerr << "Error in command line: a bad value ("
             << current[0] << ") for "
             << current[-1] << "\n";
	throw MyException ("Terminated by error in command line");
      }
      // now option is set successfully, go to the next option
      current++;
    }
    // option not found, should be the input file name
    else if (current+1 != last) { // there is something beyond the file name
      cerr << "Error in command line: " << *current << " is not a valid option\n";
      throw MyException ("Terminated by error in command line");
    }
    else { // the last option, should be the file name then
      inputFile (*current);

      const long maxTime = LONG_MAX/10;
      if (timeLimit() > maxTime) {
	timeLimit (maxTime);
      }

      // changing time to deciseconds
      timeLimit (timeLimit()*10);
      return;
    }
  }
  
  throw MyException ("input file name missing");
} // Options::correct


bool Option<bool>::set (const char* val)
{
  if (! strcmp (val,"on")) {
    _value = true;
  }
  else if (! strcmp (val,"off")) {
    _value = false;
  }
  else { // wrong value
    return false;
  }

  _changed = true;
  return true;
} // Option<bool>::set


// 27/05/2003 Manchester
void Opt::output (ostream& str) const
{
  str << _name << " = ";
  toStream (str);
  str << '\n';
} // Opts::output


// 27/05/2003 Manchester
void Opt::tabulate (ostream& str) const
{
  str << "option("
      << _name 
      << ',';
  if (quotes()) {
    str << '\'';
    toStream (str);
    str << '\'';
  }
  else {
    toStream (str);
  }
  str << ").\n";
} // Opts::tabulate


// 02/01/2003 Manchester
void Option<bool>::toStream (ostream& str) const
{
  str << (_value ? "on" : "off");
} // Option<bool>::toStream


// 27/05/2003 Manchester
bool Option<bool>::quotes () const
{
  return false;
} // Option<bool>::quotes


bool Option<const char*>::set (const char* val)
{
  _value = val;

  _changed = true;
  return true;
} // Option<const char*>::set


// 27/05/2003 Manchester
bool Option<const char*>::quotes () const
{
  return true;
} // Option<const char*>::quotes


// 02/01/2003 Manchester
void Option<const char*>::toStream (ostream& str) const
{
  str << _value;
} // Option<const char*>::toStream


bool Option<int>::set (const char* val)
{
  char* endptr = 0;
  _value = (int)(strtol (val, &endptr, 10));

  if (*endptr) { // error returned by strtol
    return false;
  }

  _changed = true;
  return true;
} // Option<int>::set


// 27/05/2003 Manchester
bool Option<int>::quotes () const
{
  return false;
} // Option<int>::quotes


// 02/01/2003 Manchester
void Option<int>::toStream (ostream& str) const
{
  str << _value;
} // Option<int>::toStream


bool Option<long>::set (const char* val)
{
  char* endptr = 0;
  _value = strtol (val, &endptr, 10);

  if (*endptr) { // error returned by strtol
    return false;
  }

  _changed = true;
  return true;
} // Option<long>::set


// 27/05/2003 Manchester
bool Option<long>::quotes () const
{
  return false;
} // Option<long>::quotes


// 02/01/2003 Manchester
void Option<long>::toStream (ostream& str) const
{
  str << _value;
} // Option<long>::toStream


bool Option<float>::set (const char* val)
{
  char* endptr = 0;
  _value = (float)(strtod (val, &endptr));

  if (*endptr) { // error returned by strtol
    return false;
  }

  _changed = true;
  return true;
} // Option<float>::set


// 27/05/2003 Manchester
bool Option<float>::quotes () const
{
  return false;
} // Option<float>::quotes


// 02/01/2003 Manchester
void Option<float>::toStream (ostream& str) const
{
  str << _value;
} // Option<float>::toStream


// Mode change can lead to change of other options
bool Option<Options::Mode>::set (const char* val)
{
  if (_changed) {
    throw MyException ("--mode cannot be set twice");
  }

  if (! strcmp (val,"vampire")) {
    ASS (_value == Options::VAMPIRE);
    // nothing to do since vampire is the default mode anyhow
  }
  else if (! strcmp (val,"kif")) {
    _value = Options::KIF;
    options->kif();
  }
  else if (! strcmp (val,"casc-18")) {
    throw MyException ("the casc-18 mode is not implemented yet");
    //_value = Options::CASC_18;
  }
  else { // wrong value
    return false;
  }

  _changed = true;
  return true;
} // Option<Mode>::set


// 27/05/2003 Manchester
bool Option<Options::Mode>::quotes () const
{
  return true;
} // Option<Options::Mode>::quotes


// 02/01/2003 Manchester
void Option<Options::Mode>::toStream (ostream& str) const
{
  switch (_value) 
    {
    case Options::VAMPIRE:
      str << "vampire";
      break;
    case Options::KIF:
      str << "kif";
      break;
    case Options::CASC_18:
      str << "casc_18";
      break;
    }
} // Option<Options::Mode>::toStream


// 02/01/2003, Manchester
bool Option<Options::Statistics>::set (const char* val)
{
  if (! strcmp (val,"off")) {
    _value = Options::STATISTICS_OFF;
  }
  else if (! strcmp (val,"brief")) {
    _value = Options::STATISTICS_BRIEF;
  }
  else if (! strcmp (val,"detailed")) {
    _value = Options::STATISTICS_DETAILED;
  }
  else { // wrong value
    return false;
  }

  _changed = true;
  return true;
} // Option<Statistics>::set


// 27/05/2003 Manchester
bool Option<Options::Statistics>::quotes () const
{
  return false;
} // Option<Options::Statistics>::quotes


// 02/01/2003 Manchester
void Option<Options::Statistics>::toStream (ostream& str) const
{
  switch (_value) 
    {
    case Options::STATISTICS_OFF:
      str << "off";
      break;
    case Options::STATISTICS_BRIEF:
      str << "brief";
      break;
    case Options::STATISTICS_DETAILED:
      str << "detailed";
      break;
    }
} // Option<Options::Statistics>::toStream


// 25/12/2003 Manchester, changed by integrating former LRS
bool Option<Options::MainAlg>::set (const char* val)
{
  if (! strcmp (val,"otter")) {
    _value = Options::OTTER;
  }
  else if (! strcmp (val,"discount")) {
    _value = Options::DISCOUNT;
  }
  else if (! strcmp (val,"lrs")) {
    _value = Options::LRS;
  }
  else { // wrong value
    return false;
  }

  _changed = true;
  return true;
} // Option<MainAlg>::set


// 27/05/2003 Manchester
bool Option<Options::MainAlg>::quotes () const
{
  return false;
} // Option<Options::MainAlg>::quotes


// 02/01/2003 Manchester
void Option<Options::MainAlg>::toStream (ostream& str) const
{
  switch (_value) 
    {
    case Options::OTTER:
      str << "otter";
      break;
    case Options::DISCOUNT:
      str << "discount";
      break;
    case Options::LRS:
      str << "lrs";
      break;
    }
} // Option<Options::MainAlg>::toStream


// 29/12/2003 Manchester
bool Option<Options::Subsumption>::set (const char* val)
{
  if (! strcmp (val,"off")) {
    _value = Options::SUBSUMPTION_OFF;
  }
  else if (! strcmp (val,"multiset")) {
    _value = Options::SUBSUMPTION_MULTISET;
  }
  else if (! strcmp (val,"optimized")) {
    _value = Options::SUBSUMPTION_OPTIMIZED;
  }
  else if (! strcmp (val,"set")) {
    _value = Options::SUBSUMPTION_SET;
  }
  else { // wrong value
    return false;
  }

  _changed = true;
  return true;
} // Option<Options::Subsumption>::set


// 27/05/2003 Manchester
bool Option<Options::Subsumption>::quotes () const
{
  return false;
} // Option<Options::Subsumption>::quotes


// 02/01/2003 Manchester
void Option<Options::Subsumption>::toStream (ostream& str) const
{
  switch (_value) 
    {
    case Options::SUBSUMPTION_OFF:
      str << "off";
      break;
    case Options::SUBSUMPTION_MULTISET:
      str << "multiset";
      break;
    case Options::SUBSUMPTION_OPTIMIZED:
      str << "optimized";
      break;
    case Options::SUBSUMPTION_SET:
      str << "set";
      break;
    }
} // Option<Options::Subsumption>::toStream


// 24/10/2002 Manchester
bool Option<Options::Proof>::set (const char* val)
{
  if (! strcmp (val,"off")) {
    _value = Options::PROOF_OFF;
  }
  else if (! strcmp (val,"condensed")) {
    _value = Options::PROOF_CONDENSED;
  }
  else if (! strcmp (val,"full")) {
    _value = Options::PROOF_FULL;
  }
  else { // wrong value
    return false;
  }

  _changed = true;
  return true;
} // Option<Options::Proof>::set


// 27/05/2003 Manchester
bool Option<Options::Proof>::quotes () const
{
  return false;
} // Option<Options::Proof>::quotes


// 02/01/2003 Manchester
void Option<Options::Proof>::toStream (ostream& str) const
{
  switch (_value) 
    {
    case Options::PROOF_OFF:
      str << "off";
      break;
    case Options::PROOF_CONDENSED:
      str << "condensed";
      break;
    case Options::PROOF_FULL:
      str << "full";
      break;
    }
} // Option<Options::Proof>::toStream


// 02/01/2003 Manchester symbolic names introduced
bool Option<Options::SymbolPrecedence>::set (const char* val)
{
  if (! strcmp (val,"occurrence")) {
    _value = Options::BY_OCCURRENCE;
  }
  else if (! strcmp (val,"arity")) {
    _value = Options::BY_ARITY;
  }
  else if (! strcmp (val,"reverse_arity")) {
    _value = Options::BY_REVERSE_ARITY;
  }
  else { // wrong value
    return false;
  }

  _changed = true;
  return true;
} // Option<Options::SymbolPrecedence>::set


// 27/05/2003 Manchester
bool Option<Options::SymbolPrecedence>::quotes () const
{
  return false;
} // Option<Options::SymbolPrecedence>::quotes


// 02/01/2003 Manchester
void Option<Options::SymbolPrecedence>::toStream (ostream& str) const
{
  switch (_value) 
    {
    case Options::BY_OCCURRENCE:
      str << "occurrence";
      break;
    case Options::BY_ARITY:
      str << "arity";
      break;
    case Options::BY_REVERSE_ARITY:
      str << "reverse_arity";
      break;
    }
} // Option<Options::SymbolPrecedence>::toStream


bool Option<Options::TermWeightingScheme>::set (const char* val)
{
  if (! strcmp (val,"uniform")) {
    _value = Options::TWS_UNIFORM;
  }
  else if (! strcmp (val,"nonuniform_constant")) {
    _value = Options::TWS_NONUNIFORM_CONSTANT;
  }
  else { // wrong value
    return false;
  }

  _changed = true;
  return true;
} // Option<Options::TermWeightingScheme>::set


// 27/05/2003 Manchester
bool Option<Options::TermWeightingScheme>::quotes () const
{
  return false;
} // Option<Options::TermWeightingScheme>::quotes


// 02/01/2003 Manchester
void Option<Options::TermWeightingScheme>::toStream (ostream& str) const
{
  switch (_value) 
    {
    case Options::TWS_UNIFORM:
      str << "uniform";
      break;
    case Options::TWS_NONUNIFORM_CONSTANT:
      str << "nonuniform_constant";
      break;
    }
} // Option<Options::TermWeightingScheme>::toStream


bool Option<Options::Demodulation>::set (const char* val)
{
  if (! strcmp (val,"off")) {
    _value = Options::DEMODULATION_OFF;
  }
  else if (! strcmp (val,"preordered")) {
    _value = Options::DEMODULATION_PREORDERED;
  }
  else if (! strcmp (val,"all")) {
    _value = Options::DEMODULATION_ALL;
  }
  else { // wrong value
    return false;
  }

  _changed = true;
  return true;
} // Option<Options::Demodulation>::set


// 27/05/2003 Manchester
bool Option<Options::Demodulation>::quotes () const
{
  return false;
} // Option<Options::Demodulation>::quotes


// 02/01/2003 Manchester
void Option<Options::Demodulation>::toStream (ostream& str) const
{
  switch (_value) 
    {
    case Options::DEMODULATION_OFF:
      str << "off";
      break;
    case Options::DEMODULATION_PREORDERED:
      str << "preordered";
      break;
    case Options::DEMODULATION_ALL:
      str << "all";
      break;
    }
} // Option<Options::Demodulation>::toStream


// 02/01/2003 Manchester, changed to symbolic names
bool Option<Options::LiteralComparisonMode>::set (const char* val)
{
  if (! strcmp (val,"normal")) {
    _value = Options::NORMAL;
  }
  else if (! strcmp (val,"kinky")) {
    _value = Options::KINKY;
  }
  else if (! strcmp (val,"predicate")) {
    _value = Options::PREDICATE;
  }
  else { // wrong value
    return false;
  }

  _changed = true;
  return true;
} // Option<LiteralComparisonMode>::set


// 27/05/2003 Manchester
bool Option<Options::LiteralComparisonMode>::quotes () const
{
  return false;
} // Option<Options::LiteralComparisonMode>::quotes


// 02/01/2003 Manchester
void Option<Options::LiteralComparisonMode>::toStream (ostream& str) const
{
  switch (_value) 
    {
    case Options::NORMAL:
      str << "normal";
      break;
    case Options::KINKY:
      str << "kinky";
      break;
    case Options::PREDICATE:
      str << "predicate";
      break;
    }
} // Option<Options::LiteralComparisonMode>::toStream


bool Option<Options::Selection>::set (const char* val)
{
  if (! strcmp (val,"1")) {
    _value = Options::P_1;
  }
  else if (! strcmp (val,"2")) {
    _value = Options::P_2;
  }
  else if (! strcmp (val,"3")) {
    _value = Options::P_3;
  }
  else if (! strcmp (val,"4")) {
    _value = Options::P_4;
  }
  else if (! strcmp (val,"5")) {
    _value = Options::P_5;
  }
  else if (! strcmp (val,"6")) {
    _value = Options::P_6;
  }
  else if (! strcmp (val,"7")) {
    _value = Options::P_7;
  }
  else if (! strcmp (val,"-2")) {
    _value = Options::N_2;
  }
  else if (! strcmp (val,"-3")) {
    _value = Options::N_3;
  }
  else if (! strcmp (val,"-4")) {
    _value = Options::N_4;
  }
  else if (! strcmp (val,"-5")) {
    _value = Options::N_5;
  }
  else if (! strcmp (val,"-6")) {
    _value = Options::N_6;
  }
  else if (! strcmp (val,"-7")) {
    _value = Options::N_7;
  }
  else if (! strcmp (val,"1002")) {
    _value = Options::P_1002;
  }
  else if (! strcmp (val,"1003")) {
    _value = Options::P_1003;
  }
  else if (! strcmp (val,"1004")) {
    _value = Options::P_1004;
  }
  else if (! strcmp (val,"1005")) {
    _value = Options::P_1005;
  }
  else if (! strcmp (val,"1006")) {
    _value = Options::P_1006;
  }
  else if (! strcmp (val,"1007")) {
    _value = Options::P_1007;
  }
  else if (! strcmp (val,"1008")) {
    _value = Options::P_1008;
  }
  else if (! strcmp (val,"1009")) {
    _value = Options::P_1009;
  }
  else if (! strcmp (val,"-1005")) {
    _value = Options::N_1005;
  }
  else if (! strcmp (val,"-1009")) {
    _value = Options::N_1009;
  }
  else { // wrong value
    return false;
  }

  _changed = true;
  return true;
} // Option<Selection>::set


// 27/05/2003 Manchester
bool Option<Options::Selection>::quotes () const
{
  return false;
} // Option<Options::Selection>::quotes


// 02/01/2003 Manchester
void Option<Options::Selection>::toStream (ostream& str) const
{
  switch (_value) 
    {
    case Options::P_1:
      str << "1";
      break;
    case Options::P_2:
      str << "2";
      break;
    case Options::P_3:
      str << "3";
      break;
    case Options::P_4:
      str << "4";
      break;
    case Options::P_5:
      str << "5";
      break;
    case Options::P_6:
      str << "6";
      break;
    case Options::P_7:
      str << "7";
      break;
    case Options::N_2:
      str << "-2";
      break;
    case Options::N_3:
      str << "-3";
      break;
    case Options::N_4:
      str << "-4";
      break;
    case Options::N_5:
      str << "-5";
      break;
    case Options::N_6:
      str << "-6";
      break;
    case Options::N_7:
      str << "-7";
      break;
    case Options::P_1002:
      str << "1002";
      break;
    case Options::P_1003:
      str << "1003";
      break;
    case Options::P_1004:
      str << "1004";
      break;
    case Options::P_1005:
      str << "1005";
      break;
    case Options::P_1006:
      str << "1006";
      break;
    case Options::P_1007:
      str << "1007";
      break;
    case Options::P_1008:
      str << "1008";
      break;
    case Options::P_1009:
      str << "1009";
      break;
    case Options::N_1005:
      str << "-1005";
      break;
    case Options::N_1009:
      str << "-1009";
      break;
    }
} // Option<Options::Selection>::toStream


// 25/12/2002 Manchester, changed
bool Option<Options::Splitting>::set (const char* val)
{
  if (! strcmp (val,"off")) {
    _value = Options::SPLIT_OFF;
  }
  else if (! strcmp (val,"prepro_only")) {
    _value = Options::SPLIT_PREPRO;
  }
  else if (! strcmp (val,"on")) {
    _value = Options::SPLIT_ON;
  }
  else { // wrong value
    return false;
  }

  _changed = true;
  return true;
} // Option<Splitting>::set


// 27/05/2003 Manchester
bool Option<Options::Splitting>::quotes () const
{
  return false;
} // Option<Options::Splitting>::quotes


// 02/01/2003 Manchester
void Option<Options::Splitting>::toStream (ostream& str) const
{
  switch (_value) 
    {
    case Options::SPLIT_OFF:
      str << "off";
      break;
    case Options::SPLIT_PREPRO:
      str << "prepro_only";
      break;
    case Options::SPLIT_ON:
      str << "on";
      break;
    }
} // Option<Options::Splitting>::toStream


// 25/12/2002 Manchester, names of the options changed 
bool Option<Options::SplittingStyle>::set (const char* val)
{
  if (! strcmp (val,"blocking")) {
    _value = Options::BLOCKING;
  }
  else if (! strcmp (val,"parallel")) {
    _value = Options::PARALLEL;
  }
  else { // wrong value
    return false;
  }

  _changed = true;
  return true;
} // Option<SplittingStyle>::set


// 27/05/2003 Manchester
bool Option<Options::SplittingStyle>::quotes () const
{
  return false;
} // Option<Options::SplittingStyle>::quotes


// 02/01/2003 Manchester
void Option<Options::SplittingStyle>::toStream (ostream& str) const
{
  switch (_value) 
    {
    case Options::BLOCKING:
      str << "blocking";
      break;
    case Options::PARALLEL:
      str << "parallel";
      break;
    }
} // Option<Options::SplittingStyle>::toStream


bool Option<Options::SimplificationOrdering>::set (const char* val)
{
  if (! strcmp (val,"standard_KBO")) {
    _value = Options::KBO;
  }
  else if (! strcmp (val,"nonrecursive_KBO")) {
    _value = Options::KBO_NONREC;
  }
  else { // wrong value
    return false;
  }

  _changed = true;
  return true;
} // Option<SimplificationOrdering>::set


// 27/05/2003 Manchester
bool Option<Options::SimplificationOrdering>::quotes () const
{
  return false;
} // Option<Options::SimplificationOrdering>::quotes


// 02/01/2003 Manchester
void Option<Options::SimplificationOrdering>::toStream (ostream& str) const
{
  switch (_value) 
    {
    case Options::KBO:
      str << "standard_KBO";
      break;
    case Options::KBO_NONREC:
      str << "nonrecursive_KBO";
      break;
    }
} // Option<Options::SimplificationOrdering>::toStream


bool Option<Options::InputSyntax>::set (const char* val)
{
  if (! strcmp (val,"tptp")) {
    _value = Options::INPUT_TPTP;
  }
  else if (! strcmp (val,"kif")) {
    _value = Options::INPUT_KIF;
  }
  else { // wrong value
    return false;
  }

  _changed = true;
  return true;
} // Option<InputSyntax>::set


// 27/05/2003 Manchester
bool Option<Options::InputSyntax>::quotes () const
{
  return false;
} // Option<Options::InputSyntax>::quotes


// 02/01/2003 Manchester
void Option<Options::InputSyntax>::toStream (ostream& str) const
{
  switch (_value) 
    {
    case Options::INPUT_TPTP:
      str << "tptp";
      break;
    case Options::INPUT_KIF:
      str << "kif";
      break;
    }
} // Option<Options::InputSyntax>::toStream


bool Option<OutputSyntax>::set (const char* val)
{
  if (! strcmp (val,"tptp")) {
    _value = TPTP;
  }
  else if (! strcmp (val,"kif")) {
    _value = KIF;
  }
  else if (! strcmp (val,"native")) {
    _value = NATIVE;
  }
  else if (! strcmp (val,"latex")) {
    _value = LATEX;
  }
  //  else if (! strcmp (val,"otter")) {
  //    _value = Options::SYNTAX_OTTER;
  //  }
  else { // wrong value
    return false;
  }

  _changed = true;
  return true;
} // Option<OutputSyntax>::set


// 27/05/2003 Manchester
bool Option<OutputSyntax>::quotes () const
{
  return false;
} // Option<OutputSyntax>::quotes


// 02/01/2003 Manchester
void Option<OutputSyntax>::toStream (ostream& str) const
{
  switch (_value) 
    {
    case TPTP:
      str << "tptp";
      break;
    case KIF:
      str << "kif";
      break;
    case NATIVE:
      str << "native";
      break;
    case LATEX:
      str << "latex";
      break;
    }
} // Option<OutputSyntax>::toStream


Opt* Options::find (const char* name) // find option by name
{
  if (*name != '-') {
    return 0;
  }
  name++;
  if (*name != '-') { // short name, like -t
    // now name is whatever follows '-'
    switch (*name) {
      case 'T': 
        if (! strcmp(name,"T") ) {
          return &_testId;
        }
        return 0;

      case 'm': 
        if (! strcmp(name,"m") ) {
          return &_memoryLimit;
        }
        return 0;

      case 'p': 
        if (! strcmp(name,"p") ) {
          return &_proof;
        }
        return 0;

      case 't': 
        if (! strcmp(name,"t") ) {
          return &_timeLimit;
        }
        return 0;

      case 'w': 
        if (! strcmp(name,"w") ) {
          return &_maxWeight;
        }
        return 0;

    default:
      // no short names so far
      return 0;
    }
  }

  name ++;
  // now name is whatever follows '--'

  switch (*name) {
    case 'a': 
      if (! strcmp(name,"age_weight_ratio") ) {
        return &_ageWeightRatio;
      }
      if (! strcmp(name,"arity_check") ) {
        return &_arityCheck;
      }
      if (! strcmp(name,"allocation_buffer_size") ) {
        return &_allocationBufferSize;
      }
      if (! strcmp(name,"allocation_buffer_price") ) {
        return &_allocationBufferPrice;
      }
      return 0;

    case 'b': 
      if (! strcmp(name,"backward_demodulation") ) {
        return &_backwardDemodulation;
      }
      if (! strcmp(name,"backward_subsumption") ) {
        return &_backwardSubsumption;
      }
      if (! strcmp(name,"bd_on_splitting_branches") ) {
        return &_bdOnSplittingBranches;
      }
      return 0;

    case 'c': 
      return 0;

    case 'd': 
      return 0;

    case 'e': 
      if (! strcmp(name,"elim_def") ) {
        return &_elimDef;
      }
      return 0;

    case 'f': 
      if (! strcmp(name,"fd_on_splitting_branches") ) {
        return &_fdOnSplittingBranches;
      }
      if (! strcmp(name,"forward_demodulation") ) {
        return &_forwardDemodulation;
      }
      if (! strcmp(name,"forward_subsumption") ) {
        return &_forwardSubsumption;
      }
      if (! strcmp(name,"forward_subsumption_resolution") ) {
        return &_forwardSubsumptionResolution;
      }
      return 0;

    case 'h': 
      if (! strcmp(name,"header_precedence_kinky") ) {
        return &_headerPrecedenceKinky;
      }
      return 0;

    case 'i': 
      if (! strcmp(name,"include") ) {
        return &_include;
      }
      if (! strcmp(name,"inherited_negative_selection") ) {
        return &_inheritedNegativeSelection;
      }
      if (! strcmp(name,"input_syntax") ) {
        return &_inputSyntax;
      }
      if (! strcmp(name,"inequality_splitting") ) {
	return &_inequalitySplitting;
      }
      if (! strcmp(name,"input_file") ) {
        return &_inputFile;
      }
      return 0;

    case 'k': 
      return 0;

    case 'l': 
      if (! strcmp(name,"literal_comparison_mode") ) {
        return &_literalComparisonMode;
      }
      if (! strcmp(name,"lrs_first_time_check") ) {
        return &_lrsFirstTimeCheck;
      }
      if (! strcmp(name,"lrs_first_memory_check") ) {
        return &_lrsFirstMemoryCheck;
      }
      return 0;

    case 'm':
      if (! strcmp(name,"main_alg") ) {
        return &_mainAlg;
      }
      if (! strcmp(name,"max_active") ) {
        return &_maxActive;
      }
      if (! strcmp(name,"max_passive") ) {
        return &_maxPassive;
      }
      if (! strcmp(name,"max_skolem") ) {
        return &_maxSkolem;
      }
      if (! strcmp(name,"max_weight") ) {
        return &_maxWeight;
      }
      if (! strcmp(name,"memory_limit") ) {
        return &_memoryLimit;
      }
      if (! strcmp(name,"miniscope") ) {
        return &_miniscope;
      }
      if (! strcmp(name,"mode") ) {
        return &_mode;
      }
      if (! strcmp(name,"max_inference_depth") ) {
        return &_maxInferenceDepth;
      }
      return 0;

    case 'n':
      if (! strcmp(name,"nongoal_weight_coefficient") ) {
        return &_nongoalWeightCoefficient;
      }
      if (! strcmp(name,"no_free_vars") ) {
        return &_noFreeVars;
      }
      if (! strcmp(name,"no_inferences_between_rules") ) {
        return &_noInferencesBetweenRules;
      }
      return 0;

    case 'o':
      if (! strcmp(name,"output_equality_infix") ) {
        return &_outputEqualityInfix;
      }
      if (! strcmp(name,"orphan_murder") ) {
        return &_orphanMurder;
      }
      if (! strcmp(name,"output_syntax") ) {
        return &_outputSyntax;
      }
      return 0;

    case 'p':
      if (! strcmp(name,"proof") ) {
        return &_proof;
      }
      if (! strcmp(name,"paramodulation") ) {
        return &_paramodulation;
      }
      return 0;

    case 'r':
      if (! strcmp(name,"random_seed") ) {
        return &_randomSeed;
      }
      if (! strcmp(name,"remove_unused_defs") ) {
        return &_removeUnusedDefs;
      }
      return 0;

    case 's':
      switch (name[1]) {
      case 'e':
	if (! strcmp(name,"selection") ) {
	  return &_selection;
	}
	if (! strcmp(name,"selected_equality_weight_coefficient") ) {
	  return &_selectedEqualityWeightCoefficient;
	}
	return 0;

      case 'h':
	if (! strcmp(name,"show_generated") ) {
	  return &_showGen;
	}
	if (! strcmp(name,"show_passive") ) {
	  return &_showPassive;
	}
	if (! strcmp(name,"show_active") ) {
	  return &_showActive;
	}
	if (! strcmp(name,"show_options") ) {
	  return &_showOptions;
	}
	if (! strcmp(name,"show_profile") ) {
	  return &_showProfile;
	}
	if (! strcmp(name,"show_prepro") ) {
	  return &_showPrepro;
	}
	if (! strcmp(name,"show_input") ) {
	  return &_showInput;
	}
	if (! strcmp(name,"show_weight_limit_changes") ) {
	  return &_showWeightLimitChanges;
	}
	return 0;

      case 'i':
	if (! strcmp(name,"silent") ) {
	  return &_silent;
	}
	if (! strcmp(name,"simplification_ordering") ) {
	  return &_simplificationOrdering;
	}
	return 0;

      case 'o':
	if (! strcmp(name,"sos") ) {
	  return &_sos;
	}
	return 0;

      case 'p':
        if (! strcmp(name,"splitting") ) {
          return &_splitting;
        }
	if (! strcmp(name,"splitting_with_naming") ) {
	  return &_splittingWithNaming;
	}
	if (! strcmp(name,"splitting_style") ) {
	  return &_splittingStyle;
	}
	return 0;

      case 't':
        if (! strcmp(name,"statistics") ) {
          return &_statistics;
	}
	return 0;

      case 'y':
	if (! strcmp(name,"symbol_precedence") ) {
	  return &_symbolPrecedence;
	}
	if (! strcmp(name,"symbol_weight_by_arity") ) {
	  return &_symbolWeightByArity;
	}
	return 0;

      default:
	return 0;
      }

    case 't':
      if (! strcmp(name,"tab") ) {
        return &_tab;
      }
      if (! strcmp(name,"term_weighting_scheme") ) {
        return &_termWeightingScheme;
      }
      if (! strcmp(name,"test_id") ) {
        return &_testId;
      }
      if (! strcmp(name,"test_number") ) {
        return &_testNumber;
      }
      if (! strcmp(name,"time_limit") ) {
        return &_timeLimit;
      }
      return 0;

    default:
      return 0;
  }
} // Options::find


// 25/12/2003 Manchester, changed passing MainAlg
Options* Options::lrs (Selection sel)
{
  Options* options = new Options;
  options->mainAlg (LRS);
  options->selection (sel);

  return options;
} // Options::lrs (Selection selection)


Options* Options::lrs (Selection sel, LiteralComparisonMode lcm)
{
  Options* options = lrs (sel);
  options->literalComparisonMode (lcm);

  if (lcm == KINKY) {
    options->headerPrecedenceKinky (true);
  }

  return options;
} // Options::lrs (Selection selection)


// 25/12/2003 Manchester, changed passing MainAlg
Options* Options::discount (Selection sel)
{
  Options* options = new Options;
  options->mainAlg (DISCOUNT);
  options->selection (sel);

  return options;
} // Options::discount (Selection selection)


// 25/12/2003 Manchester, changed passing MainAlg
Options* Options::discount (Selection sel, LiteralComparisonMode lcm)
{
  Options* options = discount (sel);
  options->literalComparisonMode (lcm);

  if (lcm == KINKY) {
    options->headerPrecedenceKinky (true);
  }

  return options;
} // Options::discount (Selection selection)


// 25/12/2003 Manchester, changed passing MainAlg
Options* Options::otter (Selection sel)
{
  Options* options = new Options;
  options->mainAlg (OTTER);
  options->selection (sel);

  return options;
} // Options::otter (Selection selection)


// KIF-specific options
// 13/09/2002
void Options::kif ()
{
  // preprocessing options
  removeUnusedDefs (false); // everything should be fast
  arityCheck (false); // arities in KIF are variable
  noFreeVars (false); // queries may contain free variables
  miniscope (false);

  // the only proof-search option
  nongoalWeightCoefficient (2.0);

  // KIF-specific
  sos (true);
  silent (true);
  backwardSubsumption (SUBSUMPTION_MULTISET);
  tab (0);
  inputSyntax (INPUT_KIF);
  outputSyntax (::KIF);
} // Options::kif ()


// 25/12/2002 Manchester, changed due to change in the splitting options
void Options::fullSplitting ()
{
  splitting (SPLIT_ON);
  splittingStyle (PARALLEL);
  splittingWithNaming (true);
  fdOnSplittingBranches (true);
  bdOnSplittingBranches (true);
} // Options::fullSplitting


// give the include file name using its relative name
// WARNING: the relative name must begin and terminate with "'"
// because of the TPTP syntax
string Options::includeFileName (char* relativeName)
{
  ASS (*relativeName == '\'');

  // skip the "'" character
  relativeName++; 

  // find the ending "'" character and remove it
  char* p = relativeName + 1;
  while (*p != '\'') {
    ASS (*p != 0);
    p++;
  }
  *p = 0;

  if (*relativeName == '/') { // absolute name
    return relativeName;
  }

  // relativeName is relative. 
  // Use the conventions of Vampire: 
  // (a) first search the value of "include"
  const char* dir = include ();
  if (! dir) { // include undefined
    // (b) search the value of the environment variable TPTP_DIR
    dir = getenv("TPTP_DIR");
    if (! dir) { // the environment variable is undefined as well
      dir = ".";
    }
  }
  // now dir is the directory to search

  string result (dir);
  result += "/";
  result += relativeName;

  return result;
} // Options::includeFileName (const char* name)


// 02/01/2003 Manchester
void Options::output (ostream& str) const
{
  if (! _showOptions.value()) {
    return;
  }

  str << "=========== Options ==========\n";

  // general options

  _mode.output(str);
  _tab.output(str);
  _inputFile.output(str);
  _randomSeed.output(str);

  // input/output options

  _statistics.output(str);
  _inputSyntax.output(str);
  _outputSyntax.output(str);
  _silent.output(str);
  _showGen.output(str);  
  _showPassive.output(str);  
  _showActive.output(str);
  _showPrepro.output(str);
  _showProfile.output(str);
  _showInput.output(str);
  _showOptions.output(str);
  _showWeightLimitChanges.output(str);  

  // preprocessor options

  _removeUnusedDefs.output(str);
  _arityCheck.output(str);
  _noFreeVars.output(str);
  _miniscope.output(str);
  _outputEqualityInfix.output(str);
  _include.output(str);

  // Kernel's options

  _mainAlg.output(str);
  _splitting.output(str);
  _splittingStyle.output(str);
  _splittingWithNaming.output(str);
  _selection.output(str);
  _literalComparisonMode.output(str);
  _inheritedNegativeSelection.output(str);
  _forwardSubsumptionResolution.output(str);
  _forwardDemodulation.output(str);
  _fdOnSplittingBranches.output(str);
  _backwardDemodulation.output(str);
  _bdOnSplittingBranches.output(str);
  _forwardSubsumption.output(str);
  _backwardSubsumption.output(str);
  _orphanMurder.output(str);
  _elimDef.output(str);
  _lrsFirstTimeCheck.output(str);
  _lrsFirstMemoryCheck.output(str);
  _maxInferenceDepth.output(str);
  _symbolPrecedence.output(str);
  _headerPrecedenceKinky.output(str);
  _noInferencesBetweenRules.output(str);
  _timeLimit.output(str);
  _memoryLimit.output(str);
  _allocationBufferSize.output(str);
  _allocationBufferPrice.output(str);
  _inequalitySplitting.output(str);
  _maxSkolem.output(str);
  _maxActive.output(str);
  _maxPassive.output(str);
  _maxWeight.output(str);
  _ageWeightRatio.output(str);
  _proof.output(str);
  _testId.output(str);
  _testNumber.output(str);
  _paramodulation.output(str);
  _symbolWeightByArity.output(str);
  _termWeightingScheme.output(str);
  _simplificationOrdering.output(str);
  _nongoalWeightCoefficient.output(str);
  _selectedEqualityWeightCoefficient.output(str);
  _sos.output(str);

  str << "======= End of options =======\n";
} // Options::output (ostream& str) const


// 27/05/2003 Manchester
void Options::tabulate (ostream& str) const
{
  // general options

  _mode.tabulate(str);
  _tab.tabulate(str);
  _inputFile.tabulate(str);
  _randomSeed.tabulate(str);

  // input/output options

  _statistics.tabulate(str);
  _inputSyntax.tabulate(str);
  _outputSyntax.tabulate(str);
  _silent.tabulate(str);
  _showGen.tabulate(str);  
  _showPassive.tabulate(str);  
  _showActive.tabulate(str);
  _showPrepro.tabulate(str);
  _showProfile.tabulate(str);
  _showInput.tabulate(str);
  _showOptions.tabulate(str);
  _showWeightLimitChanges.tabulate(str);  

  // preprocessor options

  _removeUnusedDefs.tabulate(str);
  _arityCheck.tabulate(str);
  _noFreeVars.tabulate(str);
  _miniscope.tabulate(str);
  _outputEqualityInfix.tabulate(str);
  _include.tabulate(str);

  // Kernel's options

  _mainAlg.tabulate(str);
  _splitting.tabulate(str);
  _splittingStyle.tabulate(str);
  _splittingWithNaming.tabulate(str);
  _selection.tabulate(str);
  _literalComparisonMode.tabulate(str);
  _inheritedNegativeSelection.tabulate(str);
  _forwardSubsumptionResolution.tabulate(str);
  _forwardDemodulation.tabulate(str);
  _fdOnSplittingBranches.tabulate(str);
  _backwardDemodulation.tabulate(str);
  _bdOnSplittingBranches.tabulate(str);
  _forwardSubsumption.tabulate(str);
  _backwardSubsumption.tabulate(str);
  _orphanMurder.tabulate(str);
  _elimDef.tabulate(str);
  _lrsFirstTimeCheck.tabulate(str);
  _lrsFirstMemoryCheck.tabulate(str);
  _maxInferenceDepth.tabulate(str);
  _symbolPrecedence.tabulate(str);
  _headerPrecedenceKinky.tabulate(str);
  _noInferencesBetweenRules.tabulate(str);
  _timeLimit.tabulate(str);
  _memoryLimit.tabulate(str);
  _allocationBufferSize.tabulate(str);
  _allocationBufferPrice.tabulate(str);
  _inequalitySplitting.tabulate(str);
  _maxSkolem.tabulate(str);
  _maxActive.tabulate(str);
  _maxPassive.tabulate(str);
  _maxWeight.tabulate(str);
  _ageWeightRatio.tabulate(str);
  _proof.tabulate(str);
  _testId.tabulate(str);
  _testNumber.tabulate(str);
  _paramodulation.tabulate(str);
  _symbolWeightByArity.tabulate(str);
  _termWeightingScheme.tabulate(str);
  _simplificationOrdering.tabulate(str);
  _nongoalWeightCoefficient.tabulate(str);
  _selectedEqualityWeightCoefficient.tabulate(str);
  _sos.tabulate(str);
} // Options::tabulate (ostream& str) const
