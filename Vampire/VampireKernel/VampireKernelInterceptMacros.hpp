//
// File:         VampireKernelInterceptMacros.hpp
// Description:  Macros for intercepting some operations from the library.
// Created:      Dec 05, 2000, 14:00
// Author:       Alexandre Riazanov
// mail:         riazanov@cs.man.ac.uk
//====================================================
#ifndef HELL_INTERCEPT_MACROS_H  
//====================================================
#define HELL_INTERCEPT_MACROS_H 
#include <iostream> 
#include <cstring>
#include "Exit.hpp"
#include "Signature.hpp"

#if (defined INTERCEPT_FORWARD_MATCHING_INDEXING) || \
    (defined INTERCEPT_BACKWARD_SUBSUMPTION_INDEXING) || \
    (defined INTERCEPT_INDEXED_SEARCH_FOR_GENERALISATIONS) || \
    (defined INTERCEPT_BACKWARD_MATCHING_INDEXING) || \
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

// Indexing operations for forward matching:     

#ifdef INTERCEPT_FORWARD_MATCHING_INDEXING
                   
extern Logging FMILog; // declared in VampireKernelInterceptMacros.cpp
extern const Flatterm* interceptedForwardMatchingQuery;        
extern long maxSizeOfFMILogFile; 
inline void checkMaxSizeOfFMILogFile()
{
  if (FMILog.counter() >= maxSizeOfFMILogFile) 
    { 
      cout << "%    Forced termination: file specified by --fmilog is full.\n"; 
      Exit::exit(0); 
    }; 
};
#define INTERCEPT_FORWARD_MATCHING_INDEXING_INTEGRATE(term) \
  { \
   FMILog << "+" << Logging::NormalisedTerm(term) << "\n"; \
   FMILog.flush(); \
   checkMaxSizeOfFMILogFile(); \
  }

#define INTERCEPT_FORWARD_MATCHING_INDEXING_REMOVE(term) \
  { \
   FMILog << "-" << Logging::NormalisedTerm(term) << "\n"; \
   FMILog.flush(); \
   checkMaxSizeOfFMILogFile(); \
  }

#define INTERCEPT_FORWARD_MATCHING_INDEXING_RETRIEVAL_QUERY(term) \
  ( \
   (interceptedForwardMatchingQuery=term), \
   term \
  )

#define INTERCEPT_FORWARD_MATCHING_INDEXING_RETRIEVAL_RESULT(res)\
  ( \
     (\
      (interceptedForwardMatchingQuery) ? \
       ( \
        (res) ? \
	 ( \
          (FMILog << "!" << interceptedForwardMatchingQuery << "\n"), \
          interceptedForwardMatchingQuery = 0, \
          FMILog.flush(), \
          checkMaxSizeOfFMILogFile(), \
          res \
         ) \
	 : \
	 ( \
          (FMILog << "?" << interceptedForwardMatchingQuery << "\n"), \
          interceptedForwardMatchingQuery = 0, \
          FMILog.flush(), \
          checkMaxSizeOfFMILogFile(), \
          res \
         ) \
       ) \
       : \
       res \
     ) \
  )
#endif


// Undefining the macros in the case an efficient version of Vampire is built  

#ifndef INTERCEPT_FORWARD_MATCHING_INDEXING

#undef INTERCEPT_FORWARD_MATCHING_INDEXING_INTEGRATE
#define INTERCEPT_FORWARD_MATCHING_INDEXING_INTEGRATE(term) 
 
#undef INTERCEPT_FORWARD_MATCHING_INDEXING_REMOVE
#define INTERCEPT_FORWARD_MATCHING_INDEXING_REMOVE(term)

#undef INTERCEPT_FORWARD_MATCHING_INDEXING_RETRIEVAL_QUERY  
#define INTERCEPT_FORWARD_MATCHING_INDEXING_RETRIEVAL_QUERY(term) term

#undef INTERCEPT_FORWARD_MATCHING_INDEXING_RETRIEVAL_RESULT
#define INTERCEPT_FORWARD_MATCHING_INDEXING_RETRIEVAL_RESULT(res) res
 
#endif

// Indexing operations for backward subsumption:
    
#ifdef INTERCEPT_BACKWARD_SUBSUMPTION_INDEXING
                   

#ifdef INTERCEPT_BACKWARD_SUBSUMPTION_INDEXING_WITH_COMMENTS
#define INTERCEPT_BACKWARD_SUBSUMPTION_INDEXING_COMMENT(com) com
#else
#define INTERCEPT_BACKWARD_SUBSUMPTION_INDEXING_COMMENT(com) 
#endif
 
extern Logging BSILog; // defined in VampireKernelInterceptMacros.cpp
extern const Clause* interceptedBackwardSubsumptionQuery;
extern ulong interceptedBackwardSubsumptionResultTotal;       
extern long maxSizeOfBSILogFile; 
extern ulong interceptedBackwardSubsumptionNextSymbolToDeclare;
inline void checkMaxSizeOfBSILogFile()
{
  if (BSILog.counter() >= maxSizeOfBSILogFile) 
    { 
      cout << "%    Forced termination: file specified by --bsilog is full.\n"; 
      INTERCEPT_BACKWARD_SUBSUMPTION_INDEXING_COMMENT(BSILog << "// end of file\n");
      Exit::exit(0); 
    }; 
};


#define INTERCEPT_BACKWARD_SUBSUMPTION_INDEXING_SIGNATURE_UPDATE \
  { \
    BSILog.outputSignatureUpdate(*Signature::current(), \
                                 interceptedBackwardSubsumptionNextSymbolToDeclare); \
  }


#define INTERCEPT_BACKWARD_SUBSUMPTION_INDEXING_INTEGRATE(clause) \
  { \
   BSILog << '+' << '[' << Logging::Integer32(clause->Number()) << ']'; \
   INTERCEPT_BACKWARD_SUBSUMPTION_INDEXING_COMMENT(\
     BSILog << " // " << Logging::StdClause(clause)); \
   BSILog << '\n'; \
   BSILog.flush(); \
   checkMaxSizeOfBSILogFile(); \
  }

#define INTERCEPT_BACKWARD_SUBSUMPTION_INDEXING_REMOVE(clause) \
  { \
   BSILog << '-' << '[' << Logging::Integer32(clause->Number()) << ']'; \
   INTERCEPT_BACKWARD_SUBSUMPTION_INDEXING_COMMENT(\
     BSILog << " // " << Logging::StdClause(clause)); \
   BSILog << '\n'; \
   BSILog.flush(); \
   checkMaxSizeOfBSILogFile(); \
  }

#define INTERCEPT_BACKWARD_SUBSUMPTION_INDEXING_RETRIEVAL_QUERY(clause) \
  { \
   INTERCEPT_BACKWARD_SUBSUMPTION_INDEXING_SIGNATURE_UPDATE; \
   interceptedBackwardSubsumptionQuery=clause; \
   interceptedBackwardSubsumptionResultTotal = 0UL; \
   BSILog << '?' << '[' << Logging::Integer32(clause->Number()) << ']'; \
   INTERCEPT_BACKWARD_SUBSUMPTION_INDEXING_COMMENT(\
     BSILog << " // " << Logging::StdClause(clause)); \
   BSILog << '\n'; \
   BSILog.flush(); \
   checkMaxSizeOfBSILogFile(); \
  }

#define INTERCEPT_BACKWARD_SUBSUMPTION_INDEXING_RETRIEVAL_RESULT(clause) \
  { \
   interceptedBackwardSubsumptionResultTotal++; \
   BSILog << '!' << '[' << Logging::Integer32(clause->Number()) << ']'; \
   INTERCEPT_BACKWARD_SUBSUMPTION_INDEXING_COMMENT(\
     BSILog << "\n // " << Logging::StdClause(clause)); \
   BSILog << '\n'; \
   BSILog.flush(); \
   checkMaxSizeOfBSILogFile(); \
  }

#define INTERCEPT_BACKWARD_SUBSUMPTION_INDEXING_RETRIEVAL_TOTAL \
  { \
   BSILog << "$" << interceptedBackwardSubsumptionResultTotal << " \n"; \
   BSILog.flush(); \
   checkMaxSizeOfBSILogFile(); \
  } 

#define INTERCEPT_BACKWARD_SUBSUMPTION_INDEXING_INTRODUCE_TERM_ID(term) \
  { \
    INTERCEPT_BACKWARD_SUBSUMPTION_INDEXING_SIGNATURE_UPDATE; \
    if (term->arity()) \
     { \
       BSILog << "#[" << Logging::Integer32(term) << ']' << term; \
       INTERCEPT_BACKWARD_SUBSUMPTION_INDEXING_COMMENT(\
         BSILog << " // " \
                << Logging::StdTerm(term,*Signature::current())); \
       BSILog << '\n'; \
       BSILog.flush(); \
       checkMaxSizeOfBSILogFile(); \
     } \
  } 


#define INTERCEPT_BACKWARD_SUBSUMPTION_INDEXING_INTRODUCE_LITERAL_ID(literal) \
  { \
    INTERCEPT_BACKWARD_SUBSUMPTION_INDEXING_SIGNATURE_UPDATE; \
    if (literal->arity()) \
      { \
        BSILog << "*[" << Logging::Integer32(literal) << ']'\
               << (literal->Positive() ? "++" : "--") \
               << literal; \
        INTERCEPT_BACKWARD_SUBSUMPTION_INDEXING_COMMENT(\
          BSILog << " // " << Logging::StdTerm(literal,*Signature::current())); \
        BSILog << '\n'; \
        BSILog.flush(); \
        checkMaxSizeOfBSILogFile(); \
      } \
  } 


#define INTERCEPT_BACKWARD_SUBSUMPTION_INDEXING_INTRODUCE_CLAUSE_ID(clause) \
  { \
   INTERCEPT_BACKWARD_SUBSUMPTION_INDEXING_SIGNATURE_UPDATE; \
   BSILog << "|" \
          << '[' << Logging::Integer32(clause->Number()) << ']' \
          << clause; \
   INTERCEPT_BACKWARD_SUBSUMPTION_INDEXING_COMMENT(\
     BSILog << "\n // " << Logging::StdClause(clause)); \
   BSILog << '\n'; \
   BSILog.flush(); \
   checkMaxSizeOfBSILogFile(); \
  } 

#define INTERCEPT_BACKWARD_SUBSUMPTION_INDEXING_UNBIND_TERM_ID(term) \
  { \
    if (term->arity()) \
     { \
       BSILog << "~#[" << Logging::Integer32(term) << ']'; \
       INTERCEPT_BACKWARD_SUBSUMPTION_INDEXING_COMMENT(\
         BSILog << " // " << Logging::StdTerm(term,*Signature::current())); \
       BSILog << '\n'; \
       BSILog.flush(); \
       checkMaxSizeOfBSILogFile(); \
     } \
  } 


#define INTERCEPT_BACKWARD_SUBSUMPTION_INDEXING_UNBIND_LITERAL_ID(literal) \
  { \
    if (literal->arity()) \
      { \
        BSILog << "~*[" << Logging::Integer32(literal) << ']'; \
        INTERCEPT_BACKWARD_SUBSUMPTION_INDEXING_COMMENT(\
          BSILog << " // " << Logging::StdTerm(literal,*Signature::current())); \
        BSILog << '\n'; \
        BSILog.flush(); \
        checkMaxSizeOfBSILogFile(); \
      } \
  } 


#define INTERCEPT_BACKWARD_SUBSUMPTION_INDEXING_UNBIND_CLAUSE_ID(clause) \
  { \
   BSILog << "~|" \
          << '[' << Logging::Integer32(clause->Number()) << "]\n"; \
   INTERCEPT_BACKWARD_SUBSUMPTION_INDEXING_COMMENT(\
     BSILog << " // " << Logging::StdClause(clause)); \
   BSILog << '\n'; \
   BSILog.flush(); \
   checkMaxSizeOfBSILogFile(); \
  } 



#endif

// Undefining the macros in the case an efficient version of Vampire is built  

#ifndef INTERCEPT_BACKWARD_SUBSUMPTION_INDEXING

#undef INTERCEPT_BACKWARD_SUBSUMPTION_INDEXING_SIGNATURE_UPDATE
#define INTERCEPT_BACKWARD_SUBSUMPTION_INDEXING_SIGNATURE_UPDATE

#undef INTERCEPT_BACKWARD_SUBSUMPTION_INDEXING_INTEGRATE
#define INTERCEPT_BACKWARD_SUBSUMPTION_INDEXING_INTEGRATE(clause) 
 
#undef INTERCEPT_BACKWARD_SUBSUMPTION_INDEXING_REMOVE
#define INTERCEPT_BACKWARD_SUBSUMPTION_INDEXING_REMOVE(clause)

#undef INTERCEPT_BACKWARD_SUBSUMPTION_INDEXING_RETRIEVAL_QUERY  
#define INTERCEPT_BACKWARD_SUBSUMPTION_INDEXING_RETRIEVAL_QUERY(clause)

#undef INTERCEPT_BACKWARD_SUBSUMPTION_INDEXING_RETRIEVAL_RESULT
#define INTERCEPT_BACKWARD_SUBSUMPTION_INDEXING_RETRIEVAL_RESULT(res) 
  
#undef INTERCEPT_BACKWARD_SUBSUMPTION_INDEXING_RETRIEVAL_TOTAL 
#define INTERCEPT_BACKWARD_SUBSUMPTION_INDEXING_RETRIEVAL_TOTAL

#undef INTERCEPT_BACKWARD_SUBSUMPTION_INDEXING_INTRODUCE_TERM_ID
#define INTERCEPT_BACKWARD_SUBSUMPTION_INDEXING_INTRODUCE_TERM_ID(term)

#undef INTERCEPT_BACKWARD_SUBSUMPTION_INDEXING_INTRODUCE_LITERAL_ID
#define INTERCEPT_BACKWARD_SUBSUMPTION_INDEXING_INTRODUCE_LITERAL_ID(literal)

#undef INTERCEPT_BACKWARD_SUBSUMPTION_INDEXING_INTRODUCE_CLAUSE_ID
#define INTERCEPT_BACKWARD_SUBSUMPTION_INDEXING_INTRODUCE_CLAUSE_ID(clause)

#undef INTERCEPT_BACKWARD_SUBSUMPTION_INDEXING_UNBIND_TERM_ID
#define INTERCEPT_BACKWARD_SUBSUMPTION_INDEXING_UNBIND_TERM_ID(term)

#undef INTERCEPT_BACKWARD_SUBSUMPTION_INDEXING_UNBIND_LITERAL_ID
#define INTERCEPT_BACKWARD_SUBSUMPTION_INDEXING_UNBIND_LITERAL_ID(literal)

#undef INTERCEPT_BACKWARD_SUBSUMPTION_INDEXING_UNBIND_CLAUSE_ID
#define INTERCEPT_BACKWARD_SUBSUMPTION_INDEXING_UNBIND_CLAUSE_ID(clause)


#endif



// Indexing operations for backward matching:
    
#ifdef INTERCEPT_BACKWARD_MATCHING_INDEXING
                   

#ifdef INTERCEPT_BACKWARD_MATCHING_INDEXING_WITH_COMMENTS
#define INTERCEPT_BACKWARD_MATCHING_INDEXING_COMMENT(com) com
#else
#define INTERCEPT_BACKWARD_MATCHING_INDEXING_COMMENT(com) 
#endif
 
extern VK::Logging BMILog; // declared in VampireKernelInterceptMacros.cpp
extern const VK::TERM* interceptedBackwardMatchingQuery;
extern ulong interceptedBackwardMatchingResultTotal;       
extern long maxSizeOfBMILogFile; 
inline void checkMaxSizeOfBMILogFile()
{
  static long n = strlen("\\ end of file\n"); 
  if (BMILog.counter() >= maxSizeOfBMILogFile - n) 
    { 
      cout << "%    Forced termination: file specified by --bmilog is full.\n"; 
      INTERCEPT_BACKWARD_MATCHING_INDEXING_COMMENT(BMILog << "// end of file\n";)
      BK::Exit::exit(0); 
    }; 
};


#define INTERCEPT_BACKWARD_MATCHING_INDEXING_INTEGRATE(term) \
  { \
   if (term->arity()) \
    { \
     BMILog << "+" << term; \
     INTERCEPT_BACKWARD_MATCHING_INDEXING_COMMENT( \
      BMILog << "         // " << Logging::StdTerm(term,*Signature::current()); \
                                                 ) \
     BMILog << "\n"; \
     BMILog.flush(); \
     checkMaxSizeOfBMILogFile(); \
    }; \
  }

#define INTERCEPT_BACKWARD_MATCHING_INDEXING_REMOVE(term) \
  { \
   if (term->arity()) \
    { \
     BMILog << "-" << term; \
     INTERCEPT_BACKWARD_MATCHING_INDEXING_COMMENT( \
      BMILog << "         // " << Logging::StdTerm(term,*Signature::current()); \
                                                 ) \
     BMILog << "\n"; \
     BMILog.flush(); \
     checkMaxSizeOfBMILogFile(); \
    }; \
  }

#define INTERCEPT_BACKWARD_MATCHING_INDEXING_RETRIEVAL_QUERY(term) \
  { \
   interceptedBackwardMatchingQuery=term; \
   if (term->arity()) \
    { \
     interceptedBackwardMatchingResultTotal = 0UL; \
     BMILog << "?" << term; \
     INTERCEPT_BACKWARD_MATCHING_INDEXING_COMMENT( \
      BMILog << "         // " << Logging::StdTerm(term,*Signature::current()); \
                                                 ) \
     BMILog << "\n"; \
     BMILog.flush(); \
     checkMaxSizeOfBMILogFile(); \
    }; \
  }

#ifdef INTERCEPT_BACKWARD_MATCHING_INDEXING_SHOW_MATCHES
#define INTERCEPT_BACKWARD_MATCHING_INDEXING_RETRIEVAL_RESULT(res) \
   { \
    if (interceptedBackwardMatchingQuery->arity()) \
     { \
      interceptedBackwardMatchingResultTotal++; \
      BMILog << " !" << res; \
      INTERCEPT_BACKWARD_MATCHING_INDEXING_COMMENT( \
       BMILog << "         // " << Logging::StdTerm(res,*Signature::current()); \
                                                 ) \
      BMILog << "\n"; \
      BMILog.flush(); \
      checkMaxSizeOfBMILogFile(); \
     }; \
   }
#else 
#define INTERCEPT_BACKWARD_MATCHING_INDEXING_RETRIEVAL_RESULT(res) \
   { \
    if (interceptedBackwardMatchingQuery->arity()) \
     { \
      interceptedBackwardMatchingResultTotal++; \
     }; \
   }
#endif

#define INTERCEPT_BACKWARD_MATCHING_INDEXING_RETRIEVAL_TOTAL \
  { \
   if (interceptedBackwardMatchingQuery->arity()) \
    { \
     BMILog << "$" << interceptedBackwardMatchingResultTotal << " \n"; \
     BMILog.flush(); \
     checkMaxSizeOfBMILogFile(); \
    }; \
  } 

#define INTERCEPT_BACKWARD_MATCHING_INDEXING_RETRIEVAL_ABORT \
  { \
   if (interceptedBackwardMatchingQuery->arity()) \
    { \
     BMILog << "@" << interceptedBackwardMatchingResultTotal << " \n"; \
     BMILog.flush(); \
     checkMaxSizeOfBMILogFile(); \
    }; \
  } 

#endif

// Undefining the macros in the case an efficient version of Vampire is built  

#ifndef INTERCEPT_BACKWARD_MATCHING_INDEXING

#undef INTERCEPT_BACKWARD_MATCHING_INDEXING_INTEGRATE
#define INTERCEPT_BACKWARD_MATCHING_INDEXING_INTEGRATE(term) 
 
#undef INTERCEPT_BACKWARD_MATCHING_INDEXING_REMOVE
#define INTERCEPT_BACKWARD_MATCHING_INDEXING_REMOVE(term)

#undef INTERCEPT_BACKWARD_MATCHING_INDEXING_RETRIEVAL_QUERY  
#define INTERCEPT_BACKWARD_MATCHING_INDEXING_RETRIEVAL_QUERY(term)

#undef INTERCEPT_BACKWARD_MATCHING_INDEXING_RETRIEVAL_RESULT
#define INTERCEPT_BACKWARD_MATCHING_INDEXING_RETRIEVAL_RESULT(res) 

#undef INTERCEPT_BACKWARD_MATCHING_INDEXING_RETRIEVAL_ABORT
#define INTERCEPT_BACKWARD_MATCHING_INDEXING_RETRIEVAL_ABORT
  
#undef INTERCEPT_BACKWARD_MATCHING_INDEXING_RETRIEVAL_TOTAL 
#define INTERCEPT_BACKWARD_MATCHING_INDEXING_RETRIEVAL_TOTAL

#endif






// Indexing operations for forward matching with exhaustive retrieval:
    
#ifdef INTERCEPT_INDEXED_SEARCH_FOR_GENERALISATIONS
                   

#ifdef INTERCEPT_INDEXED_SEARCH_FOR_GENERALISATIONS_WITH_COMMENTS
#define INTERCEPT_INDEXED_SEARCH_FOR_GENERALISATIONS_COMMENT(com) com
#else
#define INTERCEPT_INDEXED_SEARCH_FOR_GENERALISATIONS_COMMENT(com) 
#endif
 
extern VK::Logging SFGILog; // declared in VampireKernelInterceptMacros.cpp
extern const VK::Flatterm* interceptedSearchForGeneralisationsQuery;
extern ulong interceptedSearchForGeneralisationsResultTotal;       
extern long maxSizeOfSFGILogFile; 
inline void checkMaxSizeOfSFGILogFile()
{
  static long n = strlen("\\ end of file\n"); 
  if (SFGILog.counter() >= maxSizeOfSFGILogFile - n) 
    { 
      cout << "%    Forced termination: file specified by --sfgilog is full.\n"; 
      INTERCEPT_INDEXED_SEARCH_FOR_GENERALISATIONS_COMMENT(SFGILog << "// end of file\n";)
      BK::Exit::exit(0); 
    }; 
};


#define INTERCEPT_INDEXED_SEARCH_FOR_GENERALISATIONS_INTEGRATE(term) \
  { \
   if (term->arity()) \
    { \
     SFGILog << "+" << term; \
     INTERCEPT_INDEXED_SEARCH_FOR_GENERALISATIONS_COMMENT( \
      SFGILog << "         // " << Logging::StdTerm(term,*Signature::current()); \
                                                 ) \
     SFGILog << "\n"; \
     SFGILog.flush(); \
     checkMaxSizeOfSFGILogFile(); \
    }; \
  }

#define INTERCEPT_INDEXED_SEARCH_FOR_GENERALISATIONS_REMOVE(term) \
  { \
   if (term->arity()) \
    { \
     SFGILog << "-" << term; \
     INTERCEPT_INDEXED_SEARCH_FOR_GENERALISATIONS_COMMENT( \
      SFGILog << "         // " << Logging::StdTerm(term,*Signature::current()); \
                                                 ) \
     SFGILog << "\n"; \
     SFGILog.flush(); \
     checkMaxSizeOfSFGILogFile(); \
    }; \
  }

#define INTERCEPT_INDEXED_SEARCH_FOR_GENERALISATIONS_RETRIEVAL_QUERY(term) \
  { \
   interceptedSearchForGeneralisationsQuery=term; \
   if (term->arity()) \
    { \
     interceptedSearchForGeneralisationsResultTotal = 0UL; \
     SFGILog << "?" << term; \
     INTERCEPT_INDEXED_SEARCH_FOR_GENERALISATIONS_COMMENT( \
      SFGILog << "         // " << Logging::StdTerm(term,*Signature::current()); \
                                                 ) \
     SFGILog << "\n"; \
     SFGILog.flush(); \
     checkMaxSizeOfSFGILogFile(); \
    }; \
  }

#ifdef INTERCEPT_INDEXED_SEARCH_FOR_GENERALISATIONS_SHOW_MATCHES
#define INTERCEPT_INDEXED_SEARCH_FOR_GENERALISATIONS_RETRIEVAL_RESULT(res) \
   { \
    if (interceptedSearchForGeneralisationsQuery->arity()) \
     { \
      interceptedSearchForGeneralisationsResultTotal++; \
      SFGILog << " !" << res; \
      INTERCEPT_INDEXED_SEARCH_FOR_GENERALISATIONS_COMMENT( \
       SFGILog << "         // " << Logging::StdTerm(res,*Signature::current()); \
                                                 ) \
      SFGILog << "\n"; \
      SFGILog.flush(); \
      checkMaxSizeOfSFGILogFile(); \
     }; \
   }
#else 
#define INTERCEPT_INDEXED_SEARCH_FOR_GENERALISATIONS_RETRIEVAL_RESULT(res) \
   { \
    if (interceptedSearchForGeneralisationsQuery->arity()) \
     { \
      interceptedSearchForGeneralisationsResultTotal++; \
     }; \
   }
#endif

#define INTERCEPT_INDEXED_SEARCH_FOR_GENERALISATIONS_RETRIEVAL_TOTAL \
  { \
   if (interceptedSearchForGeneralisationsQuery->arity()) \
    { \
     SFGILog << "$" << interceptedSearchForGeneralisationsResultTotal << " \n"; \
     SFGILog.flush(); \
     checkMaxSizeOfSFGILogFile(); \
    }; \
  } 

#define INTERCEPT_INDEXED_SEARCH_FOR_GENERALISATIONS_RETRIEVAL_ABORT \
  { \
   if (interceptedSearchForGeneralisationsQuery->arity()) \
    { \
     SFGILog << "@" << interceptedSearchForGeneralisationsResultTotal << " \n"; \
     SFGILog.flush(); \
     checkMaxSizeOfSFGILogFile(); \
    }; \
  } 

#endif

// Undefining the macros in the case an efficient version of Vampire is built  

#ifndef INTERCEPT_INDEXED_SEARCH_FOR_GENERALISATIONS

#undef INTERCEPT_INDEXED_SEARCH_FOR_GENERALISATIONS_INTEGRATE
#define INTERCEPT_INDEXED_SEARCH_FOR_GENERALISATIONS_INTEGRATE(term) 
 
#undef INTERCEPT_INDEXED_SEARCH_FOR_GENERALISATIONS_REMOVE
#define INTERCEPT_INDEXED_SEARCH_FOR_GENERALISATIONS_REMOVE(term)

#undef INTERCEPT_INDEXED_SEARCH_FOR_GENERALISATIONS_RETRIEVAL_QUERY  
#define INTERCEPT_INDEXED_SEARCH_FOR_GENERALISATIONS_RETRIEVAL_QUERY(term)

#undef INTERCEPT_INDEXED_SEARCH_FOR_GENERALISATIONS_RETRIEVAL_RESULT
#define INTERCEPT_INDEXED_SEARCH_FOR_GENERALISATIONS_RETRIEVAL_RESULT(res) 

#undef INTERCEPT_INDEXED_SEARCH_FOR_GENERALISATIONS_RETRIEVAL_ABORT
#define INTERCEPT_INDEXED_SEARCH_FOR_GENERALISATIONS_RETRIEVAL_ABORT
  
#undef INTERCEPT_INDEXED_SEARCH_FOR_GENERALISATIONS_RETRIEVAL_TOTAL 
#define INTERCEPT_INDEXED_SEARCH_FOR_GENERALISATIONS_RETRIEVAL_TOTAL

#endif









// Indexing operations for backward superposition:
    
#ifdef INTERCEPT_BACKWARD_SUPERPOSITION_INDEXING
                   

#ifdef INTERCEPT_BACKWARD_SUPERPOSITION_INDEXING_WITH_COMMENTS
#define INTERCEPT_BACKWARD_SUPERPOSITION_INDEXING_COMMENT(com) com
#else
#define INTERCEPT_BACKWARD_SUPERPOSITION_INDEXING_COMMENT(com) 
#endif
 
extern VK::Logging BSIULog; // declared in VampireKernelInterceptMacros.cpp
extern const VK::PrefixSym* interceptedBackwardSuperpositionQuery;
extern ulong interceptedBackwardSuperpositionResultTotal;       
extern long maxSizeOfBSIULogFile; 
inline void checkMaxSizeOfBSIULogFile()
{
  static long n = strlen("\\ end of file\n"); 
  if (BSIULog.counter() >= maxSizeOfBSIULogFile - n) 
    { 
      cout << "%    Forced termination: file specified by --bsiulog is full.\n"; 
      INTERCEPT_BACKWARD_SUPERPOSITION_INDEXING_COMMENT(BSIULog << "// end of file\n";)
      BK::Exit::exit(0); 
    }; 
};


#define INTERCEPT_BACKWARD_SUPERPOSITION_INDEXING_INTEGRATE(term) \
  { \
     BSIULog << "+" << term; \
     INTERCEPT_BACKWARD_SUPERPOSITION_INDEXING_COMMENT( \
	BSIULog << "         // " << Logging::StdTerm(term); \
	) \
     BSIULog << "\n"; \
     BSIULog.flush(); \
     checkMaxSizeOfBSIULogFile(); \
  }

#define INTERCEPT_BACKWARD_SUPERPOSITION_INDEXING_PREPARE_FOR_REMOVAL(term) \
  { \
    BSIULog << "-" << term; \
    INTERCEPT_BACKWARD_SUPERPOSITION_INDEXING_COMMENT( \
       BSIULog << "         // " << Logging::StdTerm(term); \
       ) \
    BSIULog << "\n"; \
    BSIULog.flush(); \
    checkMaxSizeOfBSIULogFile(); \
  }

#define INTERCEPT_BACKWARD_SUPERPOSITION_INDEXING_REMOVAL_MODE(c) \
  { \
    BSIULog << ";" << c; \
    INTERCEPT_BACKWARD_SUPERPOSITION_INDEXING_COMMENT( \
      if (c == 'F') BSIULog << "         // Failure"; \
      if (c == 'R') BSIULog << "         // Complete"; \
      if (c == 'C') BSIULog << "         // Cancel";) \
     BSIULog << "\n"; \
     BSIULog.flush(); \
     checkMaxSizeOfBSIULogFile(); \
  } 
           

#define INTERCEPT_BACKWARD_SUPERPOSITION_INDEXING_RETRIEVAL_QUERY(term) \
  { \
   interceptedBackwardSuperpositionQuery=term; \
   interceptedBackwardSuperpositionResultTotal = 0UL; \
   BSIULog << "?" << term; \
   INTERCEPT_BACKWARD_SUPERPOSITION_INDEXING_COMMENT( \
         BSIULog << "\n/*\n " << Logging::PrefixTerm(term) << "\n*/"; \
      ) \
   BSIULog << "\n"; \
   BSIULog.flush(); \
   checkMaxSizeOfBSIULogFile(); \
  }

#ifdef INTERCEPT_BACKWARD_SUPERPOSITION_INDEXING_SHOW_RETRIEVED
#define INTERCEPT_BACKWARD_SUPERPOSITION_INDEXING_RETRIEVAL_RESULT(res) \
 { \
   interceptedBackwardSuperpositionResultTotal++; \
   BSIULog << " !" << res; \
   INTERCEPT_BACKWARD_SUPERPOSITION_INDEXING_COMMENT( \
      BSIULog << "         // " << Logging::StdTerm(res); \
                                                 ) \
   BSIULog << "\n"; \
   BSIULog.flush(); \
   checkMaxSizeOfBSIULogFile(); \
 }
#else 
#define INTERCEPT_BACKWARD_SUPERPOSITION_INDEXING_RETRIEVAL_RESULT(res) \
   { \
      interceptedBackwardSuperpositionResultTotal++; \
   }
#endif

#define INTERCEPT_BACKWARD_SUPERPOSITION_INDEXING_RETRIEVAL_TOTAL \
  { \
    BSIULog << "$" << interceptedBackwardSuperpositionResultTotal << " \n"; \
    BSIULog.flush(); \
    checkMaxSizeOfBSIULogFile(); \
  } 

#define INTERCEPT_BACKWARD_SUPERPOSITION_INDEXING_RETRIEVAL_ABORT \
  { \
    BSIULog << "@" << interceptedBackwardSuperpositionResultTotal << " \n"; \
    BSIULog.flush(); \
    checkMaxSizeOfBSIULogFile(); \
  } 

#endif

// Undefining the macros in the case an efficient version of Vampire is built  

#ifndef INTERCEPT_BACKWARD_SUPERPOSITION_INDEXING

#undef INTERCEPT_BACKWARD_SUPERPOSITION_INDEXING_INTEGRATE
#define INTERCEPT_BACKWARD_SUPERPOSITION_INDEXING_INTEGRATE(term) 
 
#undef INTERCEPT_BACKWARD_SUPERPOSITION_INDEXING_PREPARE_FOR_REMOVAL
#define INTERCEPT_BACKWARD_SUPERPOSITION_INDEXING_PREPARE_FOR_REMOVAL(term)

#undef INTERCEPT_BACKWARD_SUPERPOSITION_INDEXING_REMOVAL_MODE
#define INTERCEPT_BACKWARD_SUPERPOSITION_INDEXING_REMOVAL_MODE(c)

#undef INTERCEPT_BACKWARD_SUPERPOSITION_INDEXING_RETRIEVAL_QUERY  
#define INTERCEPT_BACKWARD_SUPERPOSITION_INDEXING_RETRIEVAL_QUERY(term)

#undef INTERCEPT_BACKWARD_SUPERPOSITION_INDEXING_RETRIEVAL_RESULT
#define INTERCEPT_BACKWARD_SUPERPOSITION_INDEXING_RETRIEVAL_RESULT(res) 

#undef INTERCEPT_BACKWARD_SUPERPOSITION_INDEXING_RETRIEVAL_ABORT
#define INTERCEPT_BACKWARD_SUPERPOSITION_INDEXING_RETRIEVAL_ABORT
  
#undef INTERCEPT_BACKWARD_SUPERPOSITION_INDEXING_RETRIEVAL_TOTAL 
#define INTERCEPT_BACKWARD_SUPERPOSITION_INDEXING_RETRIEVAL_TOTAL

#endif
















// Measuring backward constraint checks:
   
#ifdef MEASURE_BACKWARD_CONSTR_CHECK

extern BK::Timer bccTimer1;
extern BK::Timer bccTimer2;
extern long bccNumOfConstrChecks;
extern long bccNumOfCompilationCalls;

#  define INTERCEPT_BACKWARD_CONSTR_CHECK_BEGIN1 \
       bccTimer1.start()

#  define INTERCEPT_BACKWARD_CONSTR_CHECK_END1 \
     bccTimer1.stop()         

#  define INTERCEPT_BACKWARD_CONSTR_CHECK_BEGIN2 \
     bccTimer2.start()

#  define INTERCEPT_BACKWARD_CONSTR_CHECK_END2 \
     bccTimer2.stop()         

#  define INTERCEPT_BACKWARD_CONSTR_CHECK_NEW_CHECK \
     ++bccNumOfConstrChecks

#  define INTERCEPT_BACKWARD_CONSTR_CHECK_COMPILATION_CALL \
     ++bccNumOfCompilationCalls

#  define INTERCEPT_BACKWARD_CONSTR_CHECK_SUSPEND \
     { \
       bccTimer1.stop(); \
       bccTimer2.stop(); \
     }

#  define INTERCEPT_BACKWARD_CONSTR_CHECK_RESUME \
     { \
       bccTimer1.start(); \
       bccTimer2.start(); \
     }



#  define TABULATE_BACKWARD_CONSTR_CHECK_STATISTICS(tabulation) \
     if (tabulation) \
      { \
        tabulation->open_unit(); \
        tabulation->simple_name("development_feature","backward_constr_check_statistics"); \
        tabulation->comma(); \
        tabulation->lpar(); \
        tabulation->quoted_atom("time_compiled"); \
        tabulation->lpar(); \
        tabulation->number(bccTimer1.elapsed()); \
        tabulation->rpar(); \
        tabulation->comma(); \
        tabulation->quoted_atom("time_straightforward"); \
        tabulation->lpar(); \
        tabulation->number(bccTimer2.elapsed()); \
        tabulation->rpar(); \
        tabulation->comma(); \
        tabulation->quoted_atom("num_of_constr_checks"); \
        tabulation->lpar(); \
        tabulation->number(bccNumOfConstrChecks); \
        tabulation->rpar(); \
        tabulation->comma(); \
        tabulation->quoted_atom("num_of_compilation_calls"); \
        tabulation->lpar(); \
        tabulation->number(bccNumOfCompilationCalls); \
        tabulation->rpar(); \
        tabulation->rpar(); \
        tabulation->close_unit(); \
        tabulation->flush(); \
      }

#endif

// Undefining the macros in the case an efficient version of Vampire is built  


#ifndef MEASURE_BACKWARD_CONSTR_CHECK

#  undef INTERCEPT_BACKWARD_CONSTR_CHECK_BEGIN1
#  define INTERCEPT_BACKWARD_CONSTR_CHECK_BEGIN1

#  undef INTERCEPT_BACKWARD_CONSTR_CHECK_END1
#  define INTERCEPT_BACKWARD_CONSTR_CHECK_END1

#  undef INTERCEPT_BACKWARD_CONSTR_CHECK_BEGIN2
#  define INTERCEPT_BACKWARD_CONSTR_CHECK_BEGIN2

#  undef INTERCEPT_BACKWARD_CONSTR_CHECK_END2
#  define INTERCEPT_BACKWARD_CONSTR_CHECK_END2

#  undef INTERCEPT_BACKWARD_CONSTR_CHECK_NEW_CHECK
#  define INTERCEPT_BACKWARD_CONSTR_CHECK_NEW_CHECK

#  undef INTERCEPT_BACKWARD_CONSTR_CHECK_COMPILATION_CALL
#  define INTERCEPT_BACKWARD_CONSTR_CHECK_COMPILATION_CALL

#  undef INTERCEPT_BACKWARD_CONSTR_CHECK_SUSPEND
#  define INTERCEPT_BACKWARD_CONSTR_CHECK_SUSPEND

#  undef INTERCEPT_BACKWARD_CONSTR_CHECK_RESUME
#  define INTERCEPT_BACKWARD_CONSTR_CHECK_RESUME

#  undef TABULATE_BACKWARD_CONSTR_CHECK_STATISTICS
#  define TABULATE_BACKWARD_CONSTR_CHECK_STATISTICS(tabulation)

#endif



// Measuring forward constraint checking:
   
#ifdef MEASURE_FORWARD_CONSTR_CHECK

extern BK::Timer fccTimer1;
extern BK::Timer fccTimer2;
extern long fccNumOfConstrChecks;
extern long fccNumOfSpecialisationCalls;
extern long fccCurrentNumOfConstrainedEq;
extern long fccMaxNumOfConstrainedEq;
extern long fccNumOfConclusiveShallowChecks;
extern long fccMemoryBefore;
extern long fccCurrentMemory;
extern long fccMaxMemory;


#  define INTERCEPT_FORWARD_CONSTR_CHECK_BEGIN1 \
       fccTimer1.start()

#  define INTERCEPT_FORWARD_CONSTR_CHECK_END1 \
     fccTimer1.stop()         

#  define INTERCEPT_FORWARD_CONSTR_CHECK_BEGIN2 \
     fccTimer2.start()

#  define INTERCEPT_FORWARD_CONSTR_CHECK_END2 \
     fccTimer2.stop()         

#  define INTERCEPT_FORWARD_CONSTR_CHECK_NEW_CHECK \
     ++fccNumOfConstrChecks

#  define INTERCEPT_FORWARD_CONSTR_CHECK_SPECIALISATION_CALL \
     ++fccNumOfSpecialisationCalls

#  define INTERCEPT_FORWARD_CONSTR_CHECK_NEW_CONSTRAINED_EQ \
     { \
       ++fccCurrentNumOfConstrainedEq; \
       if (fccCurrentNumOfConstrainedEq > fccMaxNumOfConstrainedEq) \
         fccMaxNumOfConstrainedEq = fccCurrentNumOfConstrainedEq; \
     }

#  define INTERCEPT_FORWARD_CONSTR_CHECK_REMOVE_CONSTRAINED_EQ \
     --fccCurrentNumOfConstrainedEq

#  define INTERCEPT_FORWARD_CONSTR_CHECK_CONCLUSIVE_SHALLOW_CHECK \
     ++fccNumOfConclusiveShallowChecks

#  define INTERCEPT_FORWARD_CONSTR_CHECK_MEMORY_COUNT_ON \
     fccMemoryBefore = GlobAlloc::occupiedByObjects()

#  define INTERCEPT_FORWARD_CONSTR_CHECK_MEMORY_COUNT_OFF \
     { \
       fccCurrentMemory += (GlobAlloc::occupiedByObjects() - fccMemoryBefore); \
       if (fccCurrentMemory > fccMaxMemory) \
         fccMaxMemory = fccCurrentMemory; \
     }

#  define INTERCEPT_FORWARD_CONSTR_CHECK_MEMORY_PLUS(n) \
     fccCurrentMemory += (n)


#  define INTERCEPT_FORWARD_CONSTR_CHECK_SUSPEND \
     { \
       fccTimer1.stop(); \
       fccTimer2.stop(); \
     }

#  define INTERCEPT_FORWARD_CONSTR_CHECK_RESUME \
     { \
       fccTimer1.start(); \
       fccTimer2.start(); \
     }



#  define TABULATE_FORWARD_CONSTR_CHECK_STATISTICS(tabulation) \
     if (tabulation) \
      { \
        tabulation->open_unit(); \
        tabulation->simple_name("development_feature","forward_constr_check_statistics"); \
        tabulation->comma(); \
        tabulation->lpar(); \
        tabulation->quoted_atom("time_optimised"); \
        tabulation->lpar(); \
        tabulation->number(fccTimer1.elapsed()); \
        tabulation->rpar(); \
        tabulation->comma(); \
        tabulation->quoted_atom("memory_optimised"); \
        tabulation->lpar(); \
        tabulation->number(fccMaxMemory); \
        tabulation->rpar(); \
        tabulation->comma(); \
        tabulation->quoted_atom("time_straightforward"); \
        tabulation->lpar(); \
        tabulation->number(fccTimer2.elapsed()); \
        tabulation->rpar(); \
        tabulation->comma(); \
        tabulation->quoted_atom("num_of_constr_checks"); \
        tabulation->lpar(); \
        tabulation->number(fccNumOfConstrChecks); \
        tabulation->rpar(); \
        tabulation->comma(); \
        tabulation->quoted_atom("num_of_specialisation_calls"); \
        tabulation->lpar(); \
        tabulation->number(fccNumOfSpecialisationCalls); \
        tabulation->rpar(); \
        tabulation->comma(); \
        tabulation->quoted_atom("max_num_of_constrained_eq"); \
        tabulation->lpar(); \
        tabulation->number(fccMaxNumOfConstrainedEq); \
        tabulation->rpar(); \
        tabulation->comma(); \
        tabulation->quoted_atom("num_of_conclusive_shallow_checks"); \
        tabulation->lpar(); \
        tabulation->number(fccNumOfConclusiveShallowChecks); \
        tabulation->rpar(); \
        tabulation->rpar(); \
        tabulation->close_unit(); \
        tabulation->flush(); \
      }

#endif

// Undefining the macros in the case an efficient version of Vampire is built  


#ifndef MEASURE_FORWARD_CONSTR_CHECK

#  undef INTERCEPT_FORWARD_CONSTR_CHECK_BEGIN1
#  define INTERCEPT_FORWARD_CONSTR_CHECK_BEGIN1

#  undef INTERCEPT_FORWARD_CONSTR_CHECK_END1
#  define INTERCEPT_FORWARD_CONSTR_CHECK_END1

#  undef INTERCEPT_FORWARD_CONSTR_CHECK_BEGIN2
#  define INTERCEPT_FORWARD_CONSTR_CHECK_BEGIN2

#  undef INTERCEPT_FORWARD_CONSTR_CHECK_END2
#  define INTERCEPT_FORWARD_CONSTR_CHECK_END2

#  undef INTERCEPT_FORWARD_CONSTR_CHECK_NEW_CHECK
#  define INTERCEPT_FORWARD_CONSTR_CHECK_NEW_CHECK

#  undef INTERCEPT_FORWARD_CONSTR_CHECK_SPECIALISATION_CALL
#  define INTERCEPT_FORWARD_CONSTR_CHECK_SPECIALISATION_CALL

#  undef INTERCEPT_FORWARD_CONSTR_CHECK_NEW_CONSTRAINED_EQ 
#  define INTERCEPT_FORWARD_CONSTR_CHECK_NEW_CONSTRAINED_EQ 

#  undef INTERCEPT_FORWARD_CONSTR_CHECK_REMOVE_CONSTRAINED_EQ
#  define INTERCEPT_FORWARD_CONSTR_CHECK_REMOVE_CONSTRAINED_EQ

#  undef INTERCEPT_FORWARD_CONSTR_CHECK_CONCLUSIVE_SHALLOW_CHECK
#  define INTERCEPT_FORWARD_CONSTR_CHECK_CONCLUSIVE_SHALLOW_CHECK

#  undef INTERCEPT_FORWARD_CONSTR_CHECK_MEMORY_COUNT_ON
#  define INTERCEPT_FORWARD_CONSTR_CHECK_MEMORY_COUNT_ON 

#  undef INTERCEPT_FORWARD_CONSTR_CHECK_MEMORY_COUNT_OFF
#  define INTERCEPT_FORWARD_CONSTR_CHECK_MEMORY_COUNT_OFF

#  undef INTERCEPT_FORWARD_CONSTR_CHECK_MEMORY_PLUS
#  define INTERCEPT_FORWARD_CONSTR_CHECK_MEMORY_PLUS(n)

#  undef INTERCEPT_FORWARD_CONSTR_CHECK_SUSPEND
#  define INTERCEPT_FORWARD_CONSTR_CHECK_SUSPEND

#  undef INTERCEPT_FORWARD_CONSTR_CHECK_RESUME
#  define INTERCEPT_FORWARD_CONSTR_CHECK_RESUME

#  undef TABULATE_FORWARD_CONSTR_CHECK_STATISTICS
#  define TABULATE_FORWARD_CONSTR_CHECK_STATISTICS(tabulation)

#endif






//====================================================    
#endif




