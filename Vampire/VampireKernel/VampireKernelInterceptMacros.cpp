//
// File:         VampireKernelInterceptMacros.cpp
// Description:  Some objects used in VampireKernelInterceptMacros.h .
// Created:      Dec 05, 2000, 14:00
// Author:       Alexandre Riazanov
// mail:         riazanov@cs.man.ac.uk
//==================================================== 
#if (defined INTERCEPT_FORWARD_MATCHING_INDEXING) || \
    (defined INTERCEPT_BACKWARD_SUBSUMPTION_INDEXING) || \
    (defined INTERCEPT_BACKWARD_MATCHING_INDEXING) || \
    (defined INTERCEPT_INDEXED_SEARCH_FOR_GENERALISATIONS) || \
    (defined INTERCEPT_BACKWARD_SUPERPOSITION_INDEXING)
#  include "Logging.hpp"
#endif

#ifdef MEASURE_BACKWARD_CONSTR_CHECK
#  include "Timer.hpp"
#endif

#ifdef MEASURE_FORWARD_CONSTR_CHECK
#  include "Timer.hpp"
#endif
//====================================================  

#ifdef INTERCEPT_FORWARD_MATCHING_INDEXING
VK::Logging FMILog(VK::Logging::ForwardMatching);
const VK::Flatterm* interceptedForwardMatchingQuery = 0;
long maxSizeOfFMILogFile = LONG_MAX; 
#endif

#ifdef INTERCEPT_BACKWARD_SUBSUMPTION_INDEXING
VK::Logging BSILog(VK::Logging::BackwardSubsumption);
const VK::Clause* interceptedBackwardSubsumptionQuery = 0;
ulong interceptedBackwardSubsumptionResultTotal = 0UL;
long maxSizeOfBSILogFile = LONG_MAX;
ulong interceptedBackwardSubsumptionNextSymbolToDeclare = 0UL;
#endif

#ifdef INTERCEPT_BACKWARD_MATCHING_INDEXING
VK::Logging BMILog(VK::Logging::BackwardMatching);
const VK::TERM* interceptedBackwardMatchingQuery = 0;
ulong interceptedBackwardMatchingResultTotal = 0UL;
long maxSizeOfBMILogFile = LONG_MAX;
#endif

#ifdef INTERCEPT_INDEXED_SEARCH_FOR_GENERALISATIONS
VK::Logging SFGILog(VK::Logging::SearchForGeneralisations);
const VK::Flatterm* interceptedSearchForGeneralisationsQuery = 0;
ulong interceptedSearchForGeneralisationsResultTotal = 0UL;
long maxSizeOfSFGILogFile = LONG_MAX;
#endif

#ifdef INTERCEPT_BACKWARD_SUPERPOSITION_INDEXING
VK::Logging BSIULog(VK::Logging::BackwardSuperposition);
const VK::PrefixSym* interceptedBackwardSuperpositionQuery = 0;
ulong interceptedBackwardSuperpositionResultTotal = 0UL;
long maxSizeOfBSIULogFile = LONG_MAX;
#endif

#ifdef MEASURE_BACKWARD_CONSTR_CHECK
BK::Timer bccTimer1;
BK::Timer bccTimer2;
long bccNumOfConstrChecks; // implicitely initialised by 0L
long bccNumOfCompilationCalls; // implicitely initialised by 0L
#endif


#ifdef MEASURE_FORWARD_CONSTR_CHECK
BK::Timer fccTimer1;
BK::Timer fccTimer2;
long fccNumOfConstrChecks; // implicitely initialised by 0L
long fccNumOfSpecialisationCalls; // implicitely initialised by 0L
long fccCurrentNumOfConstrainedEq; // implicitely initialised by 0L
long fccMaxNumOfConstrainedEq; // implicitely initialised by 0L
long fccNumOfConclusiveShallowChecks; // implicitely initialised by 0L
long fccMemoryBefore;
long fccCurrentMemory; // implicitely initialised by 0L
long fccMaxMemory; // implicitely initialised by 0L
#endif
//=====================================================
