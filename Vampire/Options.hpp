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
//  file Options.hpp
//  defines Vampire options
//


#ifndef __options__
#define __options__


#include <string>

#include "Output.hpp"


// class ostream;

class Opt {
 public:
  enum Tag {
    // general
    MODE,
    TAB,
    INPUT_FILE,
    RANDOM_SEED,

    // preprocessor options
    REMOVE_UNUSED_DEF,
    ARITY_CHECK,
    NO_FREE_VARS,
    MINISCOPE,
    OUTPUT_EQUALITY_INFIX,
    INCLUDE, // include files for TPTP

    // input/output options
    STATISTICS,
    PROOF,
    INPUT_SYNTAX,
    OUTPUT_SYNTAX,
    SILENT,
    SHOW_GEN,
    SHOW_PASSIVE,
    SHOW_ACTIVE,
    SHOW_PREPRO,
    SHOW_PROFILE,
    SHOW_INPUT,
    SHOW_OPTIONS,
    SHOW_WEIGHT_LIMIT_CHANGES,

    // Kernel options
    MAIN_ALG,
    SPLITTING,
    SPLITTING_STYLE,
    SPLITTING_WITH_NAMING,
    SELECTION,
    FORWARD_SUBSUMPTION,
    BACKWARD_SUBSUMPTION,
    FORWARD_SUBSUMPTION_RESOLUTION,
    AGE_WEIGHT_RATIO,
    FORWARD_DEMODULATION,
    BACKWARD_DEMODULATION,
    ORPHAN_MURDER,
    FD_ON_SPLITTING_BRANCHES,
    BD_ON_SPLITTING_BRANCHES,
    INHERITED_NEGATIVE_SELECTION,
    SOS,
    NONGOAL_WEIGHT_COEFFICIENT,
    SELECTED_EQUALITY_WEIGHT_COEFFICIENT,
    PARAMODULATION,
    ELIM_DEF,
    LRS_FIRST_TIME_CHECK,
    LRS_FIRST_MEMORY_CHECK,
    MAX_INFERENCE_DEPTH,
    TIME_LIMIT,
    MEMORY_LIMIT,
    INEQUALITY_SPLITTING,
    NO_INFERENCES_BETWEEN_RULES,
    ALLOCATION_BUFFER_SIZE,
    ALLOCATION_BUFFER_PRICE,
    MAX_ACTIVE,
    MAX_PASSIVE,
    MAX_WEIGHT,
    // **********************************
    LITERAL_COMPARISON_MODE,
    SYMBOL_PRECEDENCE,
    HEADER_PRECEDENCE_KINKY,

    MAX_SKOLEM,
    TEST_ID,
    TEST_NUMBER,
    SYMBOL_WEIGHT_BY_ARITY,
    TERM_WEIGHTING_SCHEME,
    SIMPLIFICATION_ORDERING,
  };
  
  Opt (Tag t,const char* nm,const char* srt) 
    : _tag (t), 
      _name (nm),
      _short (srt),
      _changed (false)
      {}
  virtual bool set (const char* val) = 0;
  virtual void toStream (ostream& str) const = 0;
  virtual bool quotes () const = 0; 
  const char* name () const { return _name; }
  bool changed () const { return _changed; }
  void output (ostream& str) const;
  void tabulate (ostream& str) const;

 protected:
  const Tag _tag;
  const char* _name;
  const char* _short;
  bool _changed;
}; // class Opt


template <class C>
class Option :
  public Opt {
 public:
  // constructor
  Option (Opt::Tag t, const C dfl, const char* name, const char* srt = 0 )
    : Opt (t, name, srt),
      _value (dfl),
      _default (dfl)
      {}
  virtual bool set (const char* val);
  virtual void toStream (ostream& str) const;
  virtual bool quotes () const; // whether quotes are required for Prolog output
  C value () const { return _value; }
  void value (C newVal) { _value = newVal; _changed = true; }

 private:
  C _value;
  const C _default;
}; // class Option


class Options {
 public:
  // possible values for various options
  enum Mode {
    VAMPIRE,
    KIF,
    CASC_18
  };

  enum Statistics {
    STATISTICS_OFF,
    STATISTICS_BRIEF,
    STATISTICS_DETAILED
  };

  enum MainAlg {
    LRS,
    OTTER,
    DISCOUNT
  };

  enum Splitting {
    SPLIT_OFF,
    SPLIT_PREPRO,
    SPLIT_ON
  };

  enum Subsumption {
    SUBSUMPTION_OFF,
    SUBSUMPTION_MULTISET,
    SUBSUMPTION_OPTIMIZED,
    SUBSUMPTION_SET
  };

  enum SplittingStyle {
    BLOCKING = 2,
    PARALLEL = 1
  };

  enum Selection {
    P_1 = 1,
    P_2 = 2,
    P_3 = 3,
    P_4 = 4,
    P_5 = 5,
    P_6 = 6,
    P_7 = 7,
    P_1002 = 1002,
    P_1003 = 1003,
    P_1004 = 1004,
    P_1005 = 1005,
    P_1006 = 1006,
    P_1007 = 1007,
    P_1008 = 1008,
    P_1009 = 1009,
    N_2 = -2,
    N_3 = -3,
    N_4 = -4,
    N_5 = -5,
    N_6 = -6,
    N_7 = -7,
    N_1005 = -1005,
    N_1009 = -1009
  };

  enum LiteralComparisonMode {
    NORMAL = 0,
    KINKY = 1,
    PREDICATE = 2
  };

  enum Demodulation {
    DEMODULATION_OFF = 0,
    DEMODULATION_PREORDERED = 1,
    DEMODULATION_ALL = 4
  };

  enum SymbolPrecedence {
    BY_OCCURRENCE = 0,
    BY_ARITY = 1,
    BY_REVERSE_ARITY = -1
  };

  enum SimplificationOrdering {
    KBO,
    KBO_NONREC
  };

  enum TermWeightingScheme {
    TWS_UNIFORM,
    TWS_NONUNIFORM_CONSTANT
  };

  enum InputSyntax {
    INPUT_TPTP,
    INPUT_KIF
  };

  enum Proof {
    PROOF_OFF,
    PROOF_CONDENSED,
    PROOF_FULL
  };

 public:
  Options ();
  // correct existing options using command line options
  void correct (int argc, char* argv []);
  void output (ostream&) const;
  void tabulate (ostream&) const;

  // general options

  Mode mode () const { return _mode.value(); }
  void mode (Mode m); // implementation in the cpp file

  const char* tab () const { return _tab.value(); }
  void tab (const char* newVal) { _tab.value(newVal); }

  const char* inputFile () const { return _inputFile.value(); }
  void inputFile (const char* newVal) { _inputFile.value(newVal); }

  int randomSeed () const { return _randomSeed.value(); }
  void randomSeed (int newVal) { _randomSeed.value(newVal); }

  // input/output options

  Statistics statistics () const { return _statistics.value(); }
  void statistics (Statistics m); // implementation in the cpp file

  Proof proof () const { return _proof.value(); }
  void proof (Proof newVal) { _proof.value(newVal); }

  InputSyntax inputSyntax () const { return _inputSyntax.value(); }
  void inputSyntax (InputSyntax newVal) { _inputSyntax.value(newVal); }

  OutputSyntax outputSyntax () const { return _outputSyntax.value(); }
  void outputSyntax (OutputSyntax newVal) { _outputSyntax.value(newVal); }

  bool silent () const { return _silent.value(); }
  void silent (bool newVal) { _silent.value(newVal); }

  bool showGen () const { return _showGen.value(); }
  void showGen (bool newVal) { _showGen.value(newVal); }

  bool showPassive () const { return _showPassive.value(); }
  void showPassive (bool newVal) { _showPassive.value(newVal); }

  bool showActive () const { return _showActive.value(); }
  void showActive (bool newVal) { _showActive.value(newVal); }

  bool showProfile () const { return _showProfile.value(); }
  void showProfile (bool newVal) { _showProfile.value(newVal); }

  bool showPrepro () const { return _showPrepro.value(); }
  void showPrepro (bool newVal) { _showPrepro.value(newVal); }

  bool showInput () const { return _showInput.value(); }
  void showInput (bool newVal) { _showInput.value(newVal); }

  bool showOptions () const { return _showOptions.value(); }
  void showOptions (bool newVal) { _showOptions.value(newVal); }

  bool showWeightLimitChanges () const { return _showWeightLimitChanges.value(); }
  void showWeightLimitChanges (bool newVal) { _showWeightLimitChanges.value(newVal); }

  // preprocessor options

  bool removeUnusedDefs () const { return _removeUnusedDefs.value (); }
  void removeUnusedDefs (bool newVal) { _removeUnusedDefs.value (newVal); }

  bool arityCheck () const { return _arityCheck.value (); }
  void arityCheck (bool newVal) { _arityCheck.value (newVal); }

  bool noFreeVars () const { return _noFreeVars.value (); }
  void noFreeVars (bool newVal) { _noFreeVars.value (newVal); }

  bool miniscope () const { return _miniscope.value (); }
  void miniscope (bool newVal) { _miniscope.value (newVal); }

  bool outputEqualityInfix () const { return _outputEqualityInfix.value (); }
  void outputEqualityInfix (bool newVal) { _outputEqualityInfix.value (newVal); }

  const char* include () const { return _include.value(); }
  void include (const char* newVal) { _include.value(newVal); }

  // kernel options
  MainAlg mainAlg () const { return _mainAlg.value(); }
  void mainAlg (MainAlg alg) { _mainAlg.value(alg); }

  Splitting splitting () const { return _splitting.value(); }
  void splitting (Splitting newVal) { _splitting.value(newVal); }

  SplittingStyle splittingStyle () const { return _splittingStyle.value(); }
  void splittingStyle (SplittingStyle newVal) { _splittingStyle.value(newVal); }

  bool splittingWithNaming () const { return _splittingWithNaming.value(); }
  void splittingWithNaming (bool newVal) { _splittingWithNaming.value(newVal); }

  Selection selection () const { return _selection.value(); }
  void selection (Selection s) { _selection.value (s); }

  LiteralComparisonMode literalComparisonMode () const { return _literalComparisonMode.value(); }
  void literalComparisonMode (LiteralComparisonMode lcm) { _literalComparisonMode.value(lcm); }

  bool inheritedNegativeSelection () const { return _inheritedNegativeSelection.value(); }
  void inheritedNegativeSelection (bool newVal) { _inheritedNegativeSelection.value(newVal); }

  bool forwardSubsumptionResolution () const { return _forwardSubsumptionResolution.value(); }
  void forwardSubsumptionResolution (bool newVal) { _forwardSubsumptionResolution.value(newVal); }

  Demodulation forwardDemodulation () const { return _forwardDemodulation.value(); }
  void forwardDemodulation (Demodulation newVal) { _forwardDemodulation.value(newVal); }

  bool fdOnSplittingBranches () const { return _fdOnSplittingBranches.value(); }
  void fdOnSplittingBranches (bool newVal) { _fdOnSplittingBranches.value(newVal); }

  Demodulation backwardDemodulation () const { return _backwardDemodulation.value(); }
  void backwardDemodulation (Demodulation newVal) { _backwardDemodulation.value(newVal); }

  bool bdOnSplittingBranches () const { return _bdOnSplittingBranches.value(); }
  void bdOnSplittingBranches (bool newVal) { _bdOnSplittingBranches.value(newVal); }

  Subsumption backwardSubsumption () const { return _backwardSubsumption.value(); }
  void backwardSubsumption (Subsumption newVal) { _backwardSubsumption.value(newVal); }

  Subsumption forwardSubsumption () const { return _forwardSubsumption.value(); }
  void forwardSubsumption (Subsumption newVal) { _forwardSubsumption.value(newVal); }

  bool orphanMurder () const { return _orphanMurder.value(); }
  void orphanMurder (bool newVal) { _orphanMurder.value(newVal); }

  int elimDef () const { return _elimDef.value(); }
  void elimDef (int newVal) { _elimDef.value(newVal); }

  int lrsFirstTimeCheck () const { return _lrsFirstTimeCheck.value(); }
  void lrsFirstTimeCheck (int newVal) { _lrsFirstTimeCheck.value(newVal); }

  int lrsFirstMemoryCheck () const { return _lrsFirstMemoryCheck.value(); }
  void lrsFirstMemoryCheck (int newVal) { _lrsFirstMemoryCheck.value(newVal); }

  int maxInferenceDepth () const { return _maxInferenceDepth.value(); }
  void maxInferenceDepth (int newVal) { _maxInferenceDepth.value(newVal); }

  SymbolPrecedence symbolPrecedence () const { return _symbolPrecedence.value(); }
  void symbolPrecedence (SymbolPrecedence newVal) { _symbolPrecedence.value(newVal); }

  bool headerPrecedenceKinky () const { return _headerPrecedenceKinky.value(); }
  void headerPrecedenceKinky (bool newVal) { _headerPrecedenceKinky.value(newVal); }

  bool noInferencesBetweenRules () const { return _noInferencesBetweenRules.value(); }
  void noInferencesBetweenRules (bool newVal) { _noInferencesBetweenRules.value(newVal); }

  long timeLimit () const { return _timeLimit.value(); }
  void timeLimit ( long newVal ) { _timeLimit.value (newVal); }

  int memoryLimit () const { return _memoryLimit.value(); }
  void memoryLimit ( int newVal ) { _memoryLimit.value (newVal); }

  int allocationBufferSize () const { return _allocationBufferSize.value(); }
  void allocationBufferSize ( int newVal ) { _allocationBufferSize.value (newVal); }

  float allocationBufferPrice () const { return _allocationBufferPrice.value(); }
  void allocationBufferPrice ( float newVal ) { _allocationBufferPrice.value (newVal); }

  int inequalitySplitting () const { return _inequalitySplitting.value(); }
  void inequalitySplitting ( int newVal ) { _inequalitySplitting.value (newVal); }

  int maxSkolem () const { return _maxSkolem.value(); }
  void maxSkolem ( int newVal ) { _maxSkolem.value (newVal); }

  long maxActive () const { return _maxActive.value(); }
  void maxActive (long newVal) { _maxActive.value(newVal); }

  long maxPassive () const { return _maxPassive.value(); }
  void maxPassive (long newVal) { _maxPassive.value(newVal); }

  long maxWeight () const { return _maxWeight.value(); }
  void maxWeight (long newVal) { _maxWeight.value(newVal); }

  int ageWeightRatio () const { return _ageWeightRatio.value(); }
  void ageWeightRatio (int newVal) { _ageWeightRatio.value(newVal); }

  const char* testId () const { return _testId.value(); }
  void testId (const char* newVal) { _testId.value(newVal); }

  int testNumber () const { return _testNumber.value(); }
  void testNumber (int newVal) { _testNumber.value(newVal); }

  bool paramodulation () const { return _paramodulation.value(); }
  void paramodulation (bool newVal) { _paramodulation.value(newVal); }

  int symbolWeightByArity () const { return _symbolWeightByArity.value(); }
  void symbolWeightByArity (int newVal) { _symbolWeightByArity.value(newVal); }

  TermWeightingScheme termWeightingScheme () const { return _termWeightingScheme.value (); }
  void termWeightingScheme (TermWeightingScheme newVal) { _termWeightingScheme.value (newVal); }

  SimplificationOrdering simplificationOrdering () const { return _simplificationOrdering.value(); }
  void simplificationOrdering (SimplificationOrdering newVal) { _simplificationOrdering.value(newVal); }

  float nongoalWeightCoefficient () const { return _nongoalWeightCoefficient.value(); }
  void nongoalWeightCoefficient (float newVal) { _nongoalWeightCoefficient.value(newVal); }

  float selectedEqualityWeightCoefficient () const { return _selectedEqualityWeightCoefficient.value(); }
  void selectedEqualityWeightCoefficient (float newVal) { _selectedEqualityWeightCoefficient.value(newVal); }

  bool sos () const { return _sos.value(); }
  void sos (bool newVal) { _sos.value(newVal); }

  // standard ways of creating options
  static Options* lrs (Selection selection);
  static Options* lrs (Selection selection, LiteralComparisonMode);

  static Options* discount (Selection selection);
  static Options* discount (Selection selection, LiteralComparisonMode);

  static Options* otter (Selection selection);

  // change to the standard KIF options
  void kif ();
  void fullSplitting ();

  // miscellaneous
  string includeFileName (char* relativeName); // absolute include file name
  bool testNumberChanged () const { return _testNumber.changed(); }

 private:
  // general options

  Option <Mode> _mode;
  Option <const char*> _tab;
  Option <const char*> _inputFile; // input file
  Option <int> _randomSeed;

  // input/output options

  Option <Statistics> _statistics;
  Option <InputSyntax> _inputSyntax;
  Option <OutputSyntax> _outputSyntax;
  Option <bool> _silent;
  Option <bool> _showGen;  
  Option <bool> _showPassive;  
  Option <bool> _showActive;
  Option <bool> _showPrepro;
  Option <bool> _showProfile;
  Option <bool> _showInput;
  Option <bool> _showOptions;
  Option <bool> _showWeightLimitChanges;  

  // preprocessor options

  Option <bool> _removeUnusedDefs;
  Option <bool> _arityCheck;
  Option <bool> _noFreeVars;
  Option <bool> _miniscope;
  Option <bool> _outputEqualityInfix;
  Option <const char*> _include;

  // Kernel's options

  Option <MainAlg> _mainAlg;
  Option <Splitting> _splitting;
  Option <SplittingStyle> _splittingStyle;
  Option <bool> _splittingWithNaming;
  Option <Selection> _selection;
  Option <LiteralComparisonMode> _literalComparisonMode;
  Option <bool> _inheritedNegativeSelection;
  Option <bool> _forwardSubsumptionResolution;
  Option <Demodulation> _forwardDemodulation;
  Option<bool> _fdOnSplittingBranches;
  Option <Demodulation> _backwardDemodulation;
  Option<bool> _bdOnSplittingBranches;
  Option <Subsumption> _forwardSubsumption;
  Option <Subsumption> _backwardSubsumption;
  Option <bool> _orphanMurder;
  Option <int> _elimDef;
  Option <int> _lrsFirstTimeCheck;
  Option <int> _lrsFirstMemoryCheck;
  Option <int> _maxInferenceDepth;
  Option <SymbolPrecedence> _symbolPrecedence;
  Option <bool> _headerPrecedenceKinky;
  Option <bool> _noInferencesBetweenRules;
  Option <long> _timeLimit;
  Option <int> _memoryLimit;
  Option <int> _allocationBufferSize;
  Option <float> _allocationBufferPrice;
  Option <int> _inequalitySplitting;
  Option <int> _maxSkolem;
  Option <long> _maxActive;
  Option <long> _maxPassive;
  Option <long> _maxWeight;
  Option <int> _ageWeightRatio;
  Option <Proof> _proof;
  Option <const char*> _testId;
  Option <int> _testNumber;
  Option <bool> _paramodulation;
  Option <int> _symbolWeightByArity;
  Option <TermWeightingScheme> _termWeightingScheme;
  Option <SimplificationOrdering> _simplificationOrdering;
  Option <float> _nongoalWeightCoefficient;
  Option <float> _selectedEqualityWeightCoefficient;
  Option <bool> _sos;

  Opt* find (const char*); // find option by name
  static const char* _names [];
}; // class Options


// a kind of kludge. Needed since some options are required for e.g. 
// output
extern Options* options;


inline
ostream& operator << (ostream& str, const Options& opts)
{
  opts.output(str);

  return str;
} // ostream& operator << (ostream& str, const Options& opts)


#endif

