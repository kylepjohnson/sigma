//
// File:         VampireKernel.hpp
// Description:  Vampire kernel interface. 
// Created:      May 19, 2002, Alexandre Riazanov, riazanov@cs.man.ac.uk
//======================================================================
#ifndef VAMPIRE_KERNEL_H
#define VAMPIRE_KERNEL_H
//======================================================================
#include <iostream>
#include <fstream>
#include "jargon.hpp"
//======================================================================

class VampireKernel
{
 public:
  typedef long TermWeightType;
  class Options;
  class Emergency;
  enum TerminationReason
  {
    UnknownTerminationReason,
    RefutationFound,
    Satisfiable,
    EmptyPassive,
    MaxPassive,
    MaxActive,
    TimeLimitExpired,
    AbortedOnAllocationFailure,
    AbortedOnExternalInterrupt,
    AbortedOnErrorSignal,
    AbortedOnError
  };
  class Subterm;
  class Literal;
  class Clause;
  class ClauseSetIterator;
 public:
  // Initialisation and destruction
  VampireKernel();
  ~VampireKernel();
  void init(); // explicit constructor
  void destroy(); // explicit destructor, suitable for detecting memory leaks
  void openSession(const Options& opt);
  void closeSession();
  
  // Version information
  
  static float versionNumber();
  static const char* versionDescription();

  // Receiving signature information

  void beginInputSignature();  
  void endInputSignature();
  void registerInputFunction(const char* name,
			     ulong arity,
			     ulong& id); 

  void registerInputPredicate(const char* name,
			      ulong arity,
			      bool isAnswer,
			      ulong& id); 
  
  void setSymbolWeightConstPart(ulong symId,const TermWeightType& weight);
  void setSymbolPrecedence(ulong symId,long prec); // prec can not be 0
  void interpretFunction(const char* inputSymbol,
			 ulong arity,
			 const char* theory,
			 const char* nativeSymbol);
  void interpretPredicate(const char* inputSymbol,
			  ulong arity,
			  const char* theory,
			  const char* nativeSymbol);

  // Receiving input clauses
  
  void beginInputClauses();
  void endInputClauses();
  
  void beginInputClause(ulong clauseNumber,void* origin,bool goal,bool support);
  void endInputClause();
  
  void beginLiteral(bool positive,ulong predicateId);
  void endLiteral();
  
  void receiveFunction(ulong symId);
  void receiveNumericConstant(double value);
  void receiveVariable(ulong varNum);
  
  // Searching for a proof, includes preprocessing
  
  void saturate(ulong nextVacantClauseId,
		long timeLimit); // deciseconds 
  void lookForAnotherAnswer();
  // throws Emergency and intercepts SIGVTALRM
  static VampireKernel* currentSession();
  void suspend();
  void resume();
  void tabulate();
  void reportInterrupt(int signalNumber);
  void reportErrorSignal(int signalNumber);
  void reportError();

  // Communicating search results

  TerminationReason terminationReason() const;
  const Clause* refutation() const;
  ClauseSetIterator* activeClauses(); // saturation if everything is kosher
  void disposeOf(ClauseSetIterator* iterator); 

  // Internal signature interface

  bool isNumericConstantId(ulong symbolId) const;
  bool isInputSymbol(ulong symbolId) const; 
  bool isAnswerPredicateId(ulong symbolId) const;
  ulong symbolArity(ulong symbolId) const;
  const char* symbolPrintName(ulong symbolId) const; 
  // ^ does not work for numeric constants
  double numericConstantValue(ulong symbolId) const; 

  // Getting the session statistics
  
  float statTime() const;   // seconds
  long statMemory() const;  // bytes
  long statNumOfGeneratedCl() const
  {
    return 
      statNumOfClGeneratedByPrepro() +
      statNumOfClGeneratedByReanimation() +
      statNumOfClGeneratedByResolution() + 
      statNumOfClGeneratedByEqRules();
  };

  long statNumOfClGeneratedByPrepro() const; // often just copies of input clauses
  long statNumOfClGeneratedByReanimation() const;
  long statNumOfClGeneratedByResolution() const;
  long statNumOfClGeneratedByEqRules() const
  {
    return
      statNumOfClGeneratedByForwSup() +
      statNumOfClGeneratedByBackSup() +
      statNumOfClGeneratedByEqRes() +
      statNumOfClGeneratedByEqFact() +  
      statNumOfClGeneratedByBackDemod();
  };
  
  long statNumOfClGeneratedByForwSup() const;
  long statNumOfClGeneratedByBackSup() const;
  long statNumOfClGeneratedByEqRes() const;
  long statNumOfClGeneratedByEqFact() const;  
  long statNumOfClGeneratedByBackDemod() const;

  long statNumOfDiscardedEquationalTautologies() const;
  long statNumOfDiscardedPropositionalTautologies() const;
  long statNumOfForwSubsumedCl() const;
  long statNumOfClSimplifiedByForwSubsResol() const;
  long statNumOfClSimplifiedByForwDemod() const; // includes statNumOfClSimplifiedByForwDemodOnSplitBranches()
  long statNumOfClSimplifiedByForwDemodOnSplitBranches() const;
  long statNumOfClSimplifiedByBuiltInTheories() const;
  long statNumOfClSimplifiedByEqualityResol() const;
  long statNumOfSplitCl() const;
  long statNumOfDifferentSplitComponents() const;
  float statAverageNumOfSplitComponentsPerCl() const;
  long statNumOfDiscardedClWithTooBigLiterals() const;
  long statNumOfDiscardedTooDeepCl() const;
  long statNumOfDiscardedTooBigCl() const;
  long statNumOfDiscardedClWithTooManyVariables() const;
  long statNumOfNewClDiscardedDueToLackOfMemory() const;
  long statNumOfRetainedCl() const;
  long statNumOfSelectedCl() const;
  long statNumOfBackSubsumedCl() const;
  long statNumOfClSimplifiedByBackDemod() const; // includes statNumOfClSimplifiedByBackDemodOnSplitBranches()
  long statNumOfClSimplifiedByBackDemodOnSplitBranches() const;
  long statNumOfMurederedPassiveOrphans() const;
  long statNumOfMurederedActiveOrphans() const;
  long statNumOfClRecycledDueToLackOfResources() const;  
  long statNumOfCurrentlyPassiveCl() const;
  long statNumOfCurrentlyActiveCl() const;

 private:
  class _Implementation; 
  
 private:
  _Implementation* _implementation;
}; // class VampireKernel

//======================================================================

class VampireKernel::Options
{
 public:
  enum SimplificationOrdering
  {
    NonrecursiveKBOrdering,
    StandardKBOrdering
  };
  enum TermWeightingScheme
  {
    UniformTermWeighting,
    NonuniformConstantTermWeighting
  };
 public:
  Options() { setDefaultValues(); };
  ~Options() {};
  void setDefaultValues();
  std::ostream& output(std::ostream& str);
 public:
  
  // session attributes
  const char* job_id;
  

  // main algorithm
    
  long main_alg;

  // inference rules
  bool no_inferences_between_rules;
  bool static_splitting;
  bool dynamic_splitting;
  long splitting_style;
  bool splitting_with_naming;
  long static_neg_eq_splitting;
  bool paramodulation;
  long sim_back_sup;
  long selection;
  long literal_comparison_mode;
  bool inherited_negative_selection;
     
  // strategy 
  bool forward_subsumption;
  bool fs_set_mode;
  bool fs_optimised_multiset_mode;
  bool fs_use_sig_filters;
  bool fs_old_style;
  long simplify_by_fs;
  long forward_demodulation;
  bool fd_on_splitting_branches;
  bool normalize_by_commutativity;
  long backward_demodulation;
  bool bd_on_splitting_branches;
  bool simplify_by_eq_res;
  bool backward_subsumption;
  bool bs_set_mode;
  bool bs_optimised_multiset_mode;
  long orphan_murder;
  long lrs;
  long elim_def;
  SimplificationOrdering simplification_ordering;
  long symbol_precedence_by_arity;
  bool header_precedence_kinky;
  TermWeightingScheme term_weighting_scheme;
  long symbol_weight_by_arity;

  // numeric options 
  long memory_limit; // kilobytes
  long allocation_buffer_size;
  double allocation_buffer_price;
  long max_skolem; 
  long max_active;
  long max_passive;
  long max_weight;
  long max_inference_depth;
  long age_weight_ratio;
  float selected_pos_eq_penalty_coefficient;    
  float nongoal_penalty_coefficient;
  long first_time_check;   
  long first_mem_check;   
    
  // interface options    
  bool show_opt;
  bool show_input;
  bool show_prepro;
  bool show_gen;
  bool show_passive;
  bool show_active; 
  bool proof;
  bool show_profile;
  bool show_weight_limit_changes;  
  std::ofstream* tabulation;
  std::ostream* std_output;

#ifdef VKERNEL_FOR_STEP_RESOLUTION
  bool step_resolution_literal_selection;
#endif

}; // class VampireKernel::Options

namespace std
{
inline 
ostream& operator<<(ostream& str,
		    const VampireKernel::Options::SimplificationOrdering& so)
{
  switch (so)
    {
    case VampireKernel::Options::NonrecursiveKBOrdering: 
      str << "nonrecursive_KB";
      break;
    case VampireKernel::Options::StandardKBOrdering: 
      str << "standard_KB";
      break;
    };
  return str;
};

inline 
ostream& operator<<(ostream& str,
		    const VampireKernel::Options::TermWeightingScheme& tws)
{
  switch (tws)
    {
    case VampireKernel::Options::UniformTermWeighting:
      str << "uniform";
      break;
    case VampireKernel::Options::NonuniformConstantTermWeighting:
      str << "nonuniform_constant";
      break;
    };
  return str;
};
};

//======================================================================

class VampireKernel::Emergency
{
 public:
  enum Type
  {
    AllocationFailure,
    TimeLimit
  };
 public:
  Emergency(const Type& t) : type_(t) {}; 
  ~Emergency() {};
  const Type& type() const { return type_; };
 private:
  Emergency() {};
 private:
  Type type_;
}; // class VampireKernel::Emergency

//======================================================================

class VampireKernel::Subterm
{
 public:
  const Subterm* next() const; 
  // ^ returns 0 if this is the last subterm
  bool isVariable() const;
  ulong variableNumber() const; 
  ulong topFunction() const;
  const Subterm* firstArgument() const; 
  // ^ returns 0 if there are no arguments
 private:
  Subterm() {};
  ~Subterm() {};
  friend class VampireKernel;
}; // class VampireKernel::Subterm

class VampireKernel::Literal
{
 public:
  const Literal* next() const;
  // ^ return 0 if this is the last literal
  bool isPositive() const;
  ulong predicate() const;
  const Subterm* firstArgument() const; 
  // ^ returns 0 if there are no arguments
 private:
  Literal() {};
  ~Literal() {};
  friend class VampireKernel;
}; // class VampireKernel::Literal


class VampireKernel::Clause
{
 public:
  class Ancestor
    {
    public:
      const Clause* clause() const;
      const Ancestor* next() const;
    private:
      Ancestor() {};
      ~Ancestor() {};
      friend class Clause;
    };

  enum BackgroundFlag
    { 
      IsInputClause,
      IsObtainedDuringPreprocessing,
      BinaryResolutionUsed,
      ForwardSuperpositionUsed,
      BackwardSuperpositionUsed,
      EqualityResolutionUsed,
      EqualityFactoringUsed,
      ForwardDemodulationUsed,
      BackwardDemodulationUsed,
      SimplificationByEqualityResolutionUsed,
      ForwardSubsumptionResolutionUsed,
      BackwardSubsumptionResolutionUsed,
      SplittingUsed, 
      IsReanimatedPassive,
      IsPartOfNameIntroduction,
      SimplificationByBuiltInTheoriesUsed,
      IsBuiltInTheoryFact
    };

  class BackgroundFlagIterator
  {
  public:
    BackgroundFlagIterator();
    ~BackgroundFlagIterator();
    bool getCurrentFlag(BackgroundFlag& flag) const;
    void goToNextFlag(); 
  private:
    const void* _base;
    unsigned long _offset;
    friend class Clause;
  };

 public:
  ulong number() const;
  const Literal* literals() const;
  // ^ return 0 if the clause is empty
  bool isInputClause() const;
  void* inputClauseOrigin() const;
  bool isBuiltInTheoryFact() const;
  const Ancestor* firstAncestor() const;
  BackgroundFlagIterator backgroundFlagIterator() const;
  bool backgroundFlagIsSet(BackgroundFlag fl) const; 
 private:
  Clause() {};
  ~Clause() {};
  friend class VampireKernel;
}; // class VampireKernel::Clause

class VampireKernel::ClauseSetIterator
{
public:
  ClauseSetIterator() {};
  virtual ~ClauseSetIterator();
  operator bool() const { return !isEnd(); };
  virtual bool isEnd() const = 0;
  ClauseSetIterator& operator ++() { next(); return *this; };
  virtual void next() = 0;
  virtual const Clause* clause() const = 0;
}; // class VampireKernel::ClauseSetIterator

//======================================================================
#endif



