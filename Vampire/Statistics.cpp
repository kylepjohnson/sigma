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
//  file Statistics.cpp 
//  implements class Statistics
//


#include <iostream>


#include "Statistics.hpp"
#include "Options.hpp"


bool Statistics::anyGeneratingInference () const
{
  return _resolutions ||
         _forwardSuperpositions ||
         _backwardSuperpositions ||
         _equalityResolutions ||
        _equalityFactorings;
} // Statistics::anyGeneratingInference ()


bool Statistics::anySimplifyingInference () const
{
  return _forwardSubsumptionResolutions ||
         _forwardDemodulated ||
         _backwardDemodulated ||
         _theoryInferences;
} // Statistics::anySimplifyingInference ()


bool Statistics::anyRedundantClause () const
{
  return _forwardSubsumed ||
         _backwardSubsumed ||
         _discardedPropositionalTautologies ||
         _discardedEquationalTautologies ||
         _murderedPassiveOrphans ||
         _murderedActiveOrphans;
} // Statistics::anyRedundantClause ()


inline
bool Statistics::anySplitting () const
{
  return _splittingInferences > 0;
} // Statistics::anySplitting ()


inline
bool Statistics::anyDiscarded () const
{
  return _discardedWithTooHeavyLiterals ||
         _discardedTooDeep ||
         _discardedTooHeavy ||
         _discardedWithTooManyVariables ||
         _newClausesDiscardedDueToLackOfMemory ||
         _recycledDueToLackOfResources;
} // Statistics::anyDiscarded ()


// build statistics from the kernel
// 01/01/2003 Manchester
Statistics::Statistics (const VampireKernel& kernel)
  :
  // general
  _versionNumber (VampireKernel::versionNumber()),
  _versionDescription (VampireKernel::versionDescription()),
  _terminationReason (kernel.terminationReason()),
  _time (kernel.statTime()),
  _memory (kernel.statMemory()),
  // number of various categories of clauses
  _generated (kernel.statNumOfGeneratedCl()),
  _retained (kernel.statNumOfRetainedCl()),
  _used (kernel.statNumOfSelectedCl()),
  _currentlyActive (kernel.statNumOfCurrentlyActiveCl()),
  _currentlyPassive (kernel.statNumOfCurrentlyPassiveCl()),
  // generating inferences
  _resolutions (kernel.statNumOfClGeneratedByResolution()),
  _forwardSuperpositions (kernel.statNumOfClGeneratedByForwSup()),
  _backwardSuperpositions (kernel.statNumOfClGeneratedByBackSup()),
  _equalityResolutions (kernel.statNumOfClGeneratedByEqRes() +
                        kernel.statNumOfClSimplifiedByEqualityResol()),
  _equalityFactorings (kernel.statNumOfClGeneratedByEqFact()),
  _preprocessed (kernel.statNumOfClGeneratedByPrepro()),
  _reanimated (kernel.statNumOfClGeneratedByReanimation()),
  // simplifying inferences
  _forwardSubsumptionResolutions (kernel.statNumOfClSimplifiedByForwSubsResol()),
  _forwardDemodulated (kernel.statNumOfClSimplifiedByForwDemod()),
  _backwardDemodulated (kernel.statNumOfClSimplifiedByBackDemod()),
  _theoryInferences (kernel.statNumOfClSimplifiedByBuiltInTheories()),
  // redundant
  _forwardSubsumed (kernel.statNumOfForwSubsumedCl()),
  _backwardSubsumed (kernel.statNumOfBackSubsumedCl()),
  _discardedPropositionalTautologies (kernel.statNumOfDiscardedPropositionalTautologies()),
  _discardedEquationalTautologies (kernel.statNumOfDiscardedEquationalTautologies()),
  _murderedPassiveOrphans (kernel.statNumOfMurederedPassiveOrphans()),
  _murderedActiveOrphans (kernel.statNumOfMurederedActiveOrphans()),
  // splitting
  _splittingInferences (kernel.statNumOfSplitCl()),
  _differentSplitComponents (kernel.statNumOfDifferentSplitComponents()),
  _averageLiteralsPerComponent(kernel.statAverageNumOfSplitComponentsPerCl()),
  _forwardDemodulatedOnBranches (kernel.statNumOfClSimplifiedByForwDemodOnSplitBranches()),
  _backwardDemodulatedOnBranches (kernel.statNumOfClSimplifiedByBackDemodOnSplitBranches()),
  // discarded
  _discardedWithTooHeavyLiterals (kernel.statNumOfDiscardedClWithTooBigLiterals()),
  _discardedTooDeep (kernel.statNumOfDiscardedTooDeepCl()),
  _discardedTooHeavy (kernel.statNumOfDiscardedTooBigCl()),
  _discardedWithTooManyVariables (kernel.statNumOfDiscardedClWithTooManyVariables()),
  _newClausesDiscardedDueToLackOfMemory (kernel.statNumOfNewClDiscardedDueToLackOfMemory()),
  _recycledDueToLackOfResources (kernel.statNumOfClRecycledDueToLackOfResources())
{
} // Statistics::Statistics


// output statistics to a astream
// 02/01/2003 Manchester
void Statistics::output (ostream& str) const
{
  Options::Statistics level = options->statistics();

  if (level == Options::STATISTICS_OFF) {
    return;
  }

  str << "=========== Statistics ==========\n";

  if (level == Options::STATISTICS_DETAILED) {
    str << "version: " << _versionNumber << ' '
	<< _versionDescription << '\n'
	<< "=== General:\n";
  }

  str << "time: " << _time << "s\n"
      << "memory: " << ((float)(_memory/100000))/10 << "Mb\n"
      << "termination reason: ";

  outputTerminationReason(str);
  str << '\n';

  if (level == Options::STATISTICS_DETAILED) {
    // Statistics on clauses
    str << "=== Clauses:\n" 
	<< "generated: " << _generated << '\n'
        << "retained: " << _retained << '\n'
        << "total active: " << _used << '\n'
        << "currently active: " << _currentlyActive << '\n'
        << "currently passive: " << _currentlyPassive << '\n';

    // Generating inferences
    if ( anyGeneratingInference() ) {
      str << "=== Generating inferences:\n";
      if ( _resolutions) {
	str << "resolution: " << _resolutions << '\n';
      }
      if ( _forwardSuperpositions) {
	str << "forward superposition: " << _forwardSuperpositions << '\n';
      }
      if ( _backwardSuperpositions) {
	str << "backward superposition: " << _backwardSuperpositions << '\n';
      }
      if ( _equalityResolutions) {
	str << "equality resolution: " << _equalityResolutions << '\n';
      }
      if ( _equalityFactorings) {
	str << "equality factoring: " << _equalityFactorings << '\n';
      }
    }

    // Simplifying inferences
    if ( anySimplifyingInference() ) {
      str << "=== Simplifying inferences:\n";
      if (_forwardSubsumptionResolutions) {
	str << "forward subsumption resolution: " << _forwardSubsumptionResolutions << '\n';
      }
      if (_forwardDemodulated) {
	str << "forward demodulation: " << _forwardDemodulated << '\n';
      }
      if (_backwardDemodulated) {
	str << "backward demodulation: " << _backwardDemodulated << '\n';
      }
      if (_theoryInferences) {
	str << "theory inferences: " << _theoryInferences << '\n';
      }
    }

    if ( anyRedundantClause() ) {
      str << "=== Redundant clauses:\n";
      if ( _forwardSubsumed ) {
	str << "forward subsumed: " << _forwardSubsumed << '\n';
      }
      if ( _backwardSubsumed) {
	str << "backward subsumed: " << _backwardSubsumed << '\n';
      }
      if (_discardedPropositionalTautologies) {
	str << "propositional tautologies: " << _discardedPropositionalTautologies << '\n';
      }
      if (_discardedEquationalTautologies) {
	str << "equational tautologies: " << _discardedEquationalTautologies << '\n';
      }
      if (_murderedActiveOrphans) {
	str << "active orphans murdered: " << _murderedActiveOrphans << '\n';
      }
      if (_murderedPassiveOrphans) {
	str << "passive orphans murdered: " <<_murderedPassiveOrphans << '\n';
      }
    }

    // Splitting (if any)
    if ( anySplitting() ) {
      str << "=== Splitting:\n"
	  << "splitting inferences: " <<_splittingInferences << '\n'
	  << "distinct components: " << _differentSplitComponents << '\n'
	  << "average number of literals in a component: " << _averageLiteralsPerComponent << '\n';
      if (_forwardDemodulatedOnBranches) {
	str << "forward demodulation on branches: " << _forwardDemodulatedOnBranches << '\n';
      }
      if (_backwardDemodulatedOnBranches) {
	str << "backward demodulation on branches: " << _backwardDemodulatedOnBranches << '\n';
      }
    }

    if ( anyDiscarded() ) {
      str << "=== Discarded clauses:\n";
      if (_discardedTooHeavy) {
	str << "too heavy: " << _discardedTooHeavy << '\n';
      }
      if (_discardedTooDeep) {
	str << "too deep: " << _discardedTooDeep << '\n';
      }
      if (_discardedWithTooHeavyLiterals) {
	str << "having too heavy literals: " << _discardedWithTooHeavyLiterals << '\n';
      }
      if (_discardedWithTooManyVariables) {
	str << "having too many variables: " << _discardedWithTooManyVariables << '\n';
      }
      if (_newClausesDiscardedDueToLackOfMemory) {
	str << "due to lack of memory: " << _newClausesDiscardedDueToLackOfMemory << '\n';
      }
      if (_recycledDueToLackOfResources) {
	str << "recycled: " << _recycledDueToLackOfResources << '\n';
      }
    }
  }

  str << "======= End of statistics =======\n";
} // Statistics::output


// tabulate statistics to a stream in the Prolog format
// 27/05/2003 Manchester
void Statistics::tabulate (ostream& str) const
{
  tabulate(str, "time", _time);
  tabulate(str, "memory", (float)(_memory/100000)/10);

  // termination reason
  startTab(str, "termination_reason");
  str << '\'';
  outputTerminationReason(str);
  str << '\'';
  endTab(str);

  tabulate(str, "generated_clauses", _generated);
  tabulate(str, "retained_clauses", _retained);
  tabulate(str, "total_active", _used);
  tabulate(str, "currently_active", _currentlyActive);
  tabulate(str, "currently_passive", _currentlyPassive);

  // generating inferences
  if ( _resolutions) {
    tabulate(str, "resolution", _resolutions);
  }
  if ( _forwardSuperpositions) {
    tabulate(str, "forward_superposition", _forwardSuperpositions);
  }
  if ( _backwardSuperpositions) {
    tabulate(str, "backward_superposition", _backwardSuperpositions);
  }
  if ( _equalityResolutions) {
    tabulate(str, "equality_resolution", _equalityResolutions);
  }
  if ( _equalityFactorings) {
    tabulate(str, "equality_factoring", _equalityFactorings);
  }

  // simplifying inferences
  if (_forwardSubsumptionResolutions) {
    tabulate(str, "forward_subsumption_resolution", _forwardSubsumptionResolutions);
  }
  if (_forwardDemodulated) {
    tabulate(str, "forward_demodulation", _forwardDemodulated);
  }
  if (_backwardDemodulated) {
    tabulate(str, "backward_demodulation", _backwardDemodulated);
  }
  if (_theoryInferences) {
    tabulate(str, "theory_inferences", _theoryInferences);
  }
  // discarded clauses
  if ( _forwardSubsumed ) {
    tabulate(str, "forward_subsumed", _forwardSubsumed);
  }
  if ( _backwardSubsumed) {
    tabulate(str, "backward_subsumed", _backwardSubsumed);
  }
  if (_discardedPropositionalTautologies) {
    tabulate(str, "propositional_tautologies", _discardedPropositionalTautologies);
  }
  if (_discardedEquationalTautologies) {
    tabulate(str, "equational_tautologies", _discardedEquationalTautologies);
  }
  if (_murderedActiveOrphans) {
    tabulate(str, "murdered_active_orphans", _murderedActiveOrphans);
  }
  if (_murderedPassiveOrphans) {
    tabulate(str, "murdered_passive_orphans", _murderedPassiveOrphans);
  }

  // splitting
  if ( anySplitting() ) {
    tabulate(str, "splitting_inferences", _splittingInferences);
    tabulate(str, "distinct_components", _differentSplitComponents);
    tabulate(str, "average_number_of_literals_per_component", _averageLiteralsPerComponent);
    if (_forwardDemodulatedOnBranches) {
      tabulate(str, "forward_demodulation_on_branches", _forwardDemodulatedOnBranches);
    }
    if (_backwardDemodulatedOnBranches) {
      tabulate(str, "backward_demodulation_on_branches", 
	       _backwardDemodulatedOnBranches);
    }
  }

  // discarded
  if (_discardedTooHeavy) {
    tabulate(str, "too_heavy_clauses", _discardedTooHeavy);
  }
  if (_discardedTooDeep) {
    tabulate(str, "too_deep_clauses", _discardedTooDeep);
  }
  if (_discardedWithTooHeavyLiterals) {
    tabulate(str, "having_too_heavy_literals", _discardedWithTooHeavyLiterals);
  }
  if (_discardedWithTooManyVariables) {
    tabulate(str, "having_too_many_variables", _discardedWithTooManyVariables);
  }
  if (_newClausesDiscardedDueToLackOfMemory) {
    tabulate(str, "new_clausesdiscarded_due_to_lack_of_memory", 
	     _newClausesDiscardedDueToLackOfMemory);
  }
  if (_recycledDueToLackOfResources) {
    tabulate(str, "recycled", _recycledDueToLackOfResources);
  }
} // Statistics::tabulate


// 27/05/2003 Manchester
void Statistics::startTab (ostream& str, const char* field)
{
  str << "stat(" << field << ',';
} // Statistics::startTab


// 27/05/2003 Manchester
void Statistics::endTab (ostream& str)
{
  str << ").\n";
} // Statistics::endTab


// 27/05/2003 Manchester
void Statistics::tabulate (ostream& str, const char* field, const char* value)
{
  startTab(str, field);
  str << ",\'" << value;
  endTab(str);
  str << '\'';
} // Statistics::tabulate


// 27/05/2003 Manchester
void Statistics::tabulate (ostream& str, const char* field, float value)
{
  startTab(str, field);
  str << value;
  endTab(str);
} // Statistics::tabulate


// 27/05/2003 Manchester
void Statistics::tabulate (ostream& str, const char* field, long value)
{
  startTab(str, field);
  str << value;
  endTab(str);
} // Statistics::tabulate


// output statistics to a astream
// 02/01/2003 Manchester
void Statistics::outputTerminationReason (ostream& str) const
{
  switch (_terminationReason) 
    {
    case VampireKernel::UnknownTerminationReason:
      str << "unknown";
      break;
    case VampireKernel::RefutationFound:
      str << "refutation found";
      break;
    case VampireKernel::Satisfiable:
      str << "satisfiability detected";
      break;
    case VampireKernel::EmptyPassive:
      str << "no passive clauses left";
      break;
    case VampireKernel::MaxPassive:
      str << "max_passive exceeded";
      break;
    case VampireKernel::MaxActive:
      str << "max_active exceeded";
      break;
    case VampireKernel::TimeLimitExpired:
      str << "time limit expired";
      break;
    case VampireKernel::AbortedOnAllocationFailure:
      str << "allocation failure";
      break;
    case VampireKernel::AbortedOnExternalInterrupt:
      str << "external interrupt";
      break;
    case VampireKernel::AbortedOnErrorSignal:
      str << "error signal";
      break;
    case VampireKernel::AbortedOnError:
      str << "error";
      break;
    }
} // Statistics::outputTerminationReason 
