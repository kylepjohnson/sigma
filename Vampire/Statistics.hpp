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
//  file Statistics.hpp
//  defines class Statistics
//


#ifndef __Statistics__
#define __Statistics__


// class ostream;

#include "VampireKernel.hpp"


using namespace std;


// ******************* class Statistics, definition *********************


class Statistics {
 public:
  // constructor
  Statistics (const VampireKernel &);
  void output (ostream&) const;
  void tabulate (ostream&) const;

 private:
  // general
  const float _versionNumber;
  const char* _versionDescription;
  const VampireKernel::TerminationReason _terminationReason;
  const float _time;   // seconds
  const long _memory;  // bytes
  // number of various categories of clauses
  const long _generated;
  const long _retained;
  const long _used;
  const long _currentlyActive;
  const long _currentlyPassive;
  // generated inferences
  const long _resolutions;
  const long _forwardSuperpositions;
  const long _backwardSuperpositions;
  const long _equalityResolutions;
  const long _equalityFactorings;
  const long _preprocessed; 
  const long _reanimated; 
  bool anyGeneratingInference () const;
  // simplifying inferences
  const long _forwardSubsumptionResolutions;
  const long _forwardDemodulated;
  const long _backwardDemodulated; 
  const long _theoryInferences;
  bool anySimplifyingInference () const;
  // redundant
  const long _forwardSubsumed;
  const long _backwardSubsumed;
  const long _discardedPropositionalTautologies;
  const long _discardedEquationalTautologies;
  const long _murderedPassiveOrphans;
  const long _murderedActiveOrphans;
  bool anyRedundantClause () const;
  // splitting
  const long _splittingInferences;
  const long _differentSplitComponents;
  const float _averageLiteralsPerComponent;
  const long _forwardDemodulatedOnBranches;
  const long _backwardDemodulatedOnBranches;
  bool anySplitting () const;
  // discarded
  const long _discardedWithTooHeavyLiterals;
  const long _discardedTooDeep;
  const long _discardedTooHeavy;
  const long _discardedWithTooManyVariables;
  const long _newClausesDiscardedDueToLackOfMemory;
  const long _recycledDueToLackOfResources;
  bool anyDiscarded () const;

  // tabulation
  static void tabulate (ostream&, const char* field, const char* value);
  static void tabulate (ostream&, const char* field, int value);
  static void tabulate (ostream&, const char* field, long value);
  static void tabulate (ostream&, const char* field, float value);
  static void startTab (ostream&, const char* field);
  static void endTab (ostream&);
  void outputTerminationReason (ostream& str) const;
}; // class Statistics


// ******************* class Statistics, implementation *********************

inline 
ostream& operator << (ostream& str, const Statistics& stat)
{
  stat.output (str);
  return str;
} // ostream& operator << (ostream& str, const Statistics& stat)


#endif // __Statistics__
