//
// File:         PathIndexingWithJoinsAndEarlyAndCompiledCleanUpForInstanceRetrieval.hpp
// Description:  Optimised indexing for instance retrieval
//               based on path-indexing + database joins 
//               + some clean-up instructions
//               are moved upward in the code + remaining clean-up is compiled 
// Created:      Oct 11, 2001
// Author:       Alexandre Riazanov
// mail:         riazanov@cs.man.ac.uk
// Note:         This file is a part of the Gematogen library.
//============================================================================
#ifndef PATH_INDEXING_WITH_JOINS_AND_EARLY_AND_COMPILED_CLEAN_UP_FOR_INSTANCE_RETRIEVAL_H
//=============================================================================      
 
       // Uncomment any of the definitions to switch on
       // the corresponding optimisations

//#define INSTANCE_RETRIEVAL_INDEX_USE_WEIGHT_FILTERS
//#define INSTANCE_RETRIEVAL_INDEX_USE_SIGNATURE_FILTERS

//====================================================
      
     // Derived compilation parameters. Do not remove it!

#if ((defined INSTANCE_RETRIEVAL_INDEX_USE_WEIGHT_FILTERS) \
     || (defined INSTANCE_RETRIEVAL_INDEX_USE_SIGNATURE_FILTERS))
#define INSTANCE_RETRIEVAL_INDEX_USE_FILTERS
#ifdef INSTANCE_RETRIEVAL_INDEX_USE_WEIGHT_FILTERS
#ifdef INSTANCE_RETRIEVAL_INDEX_USE_SIGNATURE_FILTERS
// both weight and signature filters
#define INSTANCE_RETRIEVAL_INDEX_FILTER_OUT(termId) \
            (Term::weight(termId) < queryWeight) \
             || (!querySignatureFilter.subset(Term::signatureFilter(termId)))
#else
// only weight filters
#define INSTANCE_RETRIEVAL_INDEX_FILTER_OUT(termId) \
            Term::weight(termId) < queryWeight
#endif
#else
// only signature filters
#define INSTANCE_RETRIEVAL_INDEX_FILTER_OUT(termId) \
           !querySignatureFilter.subset(Term::signatureFilter(termId))
#endif  
#else
#undef INSTANCE_RETRIEVAL_INDEX_USE_FILTERS
#undef INSTANCE_RETRIEVAL_INDEX_FILTER_OUT
#endif


//============================================================================
#define PATH_INDEXING_WITH_JOINS_AND_EARLY_AND_COMPILED_CLEAN_UP_FOR_INSTANCE_RETRIEVAL_H 
#ifndef NO_DEBUG_VIS
#include <iostream>
#endif 
#include "jargon.hpp"
#include "GlobalStopFlag.hpp"
#include "Gematogen.hpp" 
#include "GematogenDebugFlags.hpp" 
#include "RuntimeError.hpp"
#include "Stack.hpp"
#include "PathIndex.hpp"
#include "CompactNodeSmallHeaderSkipList.hpp"
#include "GIncCountingSort.hpp"
#include "Multiset.hpp"
#include "LinearCombination.hpp"
#include "DestructionMode.hpp"
//============================================================================
#undef DEBUG_NAMESPACE
#ifdef DEBUG_PATH_INDEXING_WITH_JOINS_AND_EARLY_AND_COMPILED_CLEAN_UP_FOR_INSTANCE_RETRIEVAL
#define DEBUG_NAMESPACE "PathIndexingWithJoinsAndEarlyAndCompiledCleanUpForInstanceRetrieval<Alloc,Term,MaxTermDepth,MaxTermSize,MaxNumOfVariables>"
#endif
#include "debugMacros.hpp"
//============================================================================ 

namespace Gem
{

template <class Alloc,class Term,ulong MaxTermDepth,ulong MaxTermSize,ulong MaxNumOfVariables>
class PathIndexingWithJoinsAndEarlyAndCompiledCleanUpForInstanceRetrieval
{ 
 public:
  typedef typename Term::Symbol Symbol;
  typedef typename Term::Id TermId;
  typedef typename Term::Traversal TermTraversal;
  typedef typename Term::Arguments TermArguments;
#ifdef INSTANCE_RETRIEVAL_INDEX_USE_SIGNATURE_FILTERS
  typedef typename Term::SignatureFilter SignatureFilter;
#endif
  // Cannot make the following types private
  // since they have static data members 
  typedef BK::CNSHSkList<Alloc,TermArguments,TermId,2,PathIndexingWithJoinsAndEarlyAndCompiledCleanUpForInstanceRetrieval> NonmaxPathRelation;
  typedef BK::CNSHVoidSkList<Alloc,TermId,2,PathIndexingWithJoinsAndEarlyAndCompiledCleanUpForInstanceRetrieval> MaxPathRelation;
  typedef PathIndex<Alloc,Symbol,NonmaxPathRelation,MaxPathRelation,MaxTermDepth,MaxTermSize> PathIndexForInstanceRetrieval;
 public:
  PathIndexingWithJoinsAndEarlyAndCompiledCleanUpForInstanceRetrieval() 
    : 
    _intermediateQueryRepresentation(&_abstractMachine)
    {
    };
  ~PathIndexingWithJoinsAndEarlyAndCompiledCleanUpForInstanceRetrieval() 
    {
    };
  bool integrate(const TermId& integratedTermId);
  bool remove(const TermId& removedTermId); 
  bool compileQuery(const TermId& query);
  bool nextMatch(TermId& matchId);
  const TermId& subst(ulong var) const 
    { 
      CALL("subst(ulong var) const");
      ASSERT(_debugQueryVariables.coefficient(var));
      return _abstractMachine.substitution[var]; 
    };
  const TermId* nativeVar(ulong varNum) const
    { 
      CALL("nativeVar(ulong varNum) const");
      ASSERT(_debugQueryVariables.coefficient(varNum));
      return _abstractMachine.substitution + varNum;
    };
  static const char* about() 
    { 
      return GEMATOGEN_VERSION 
	", optimised inst. ret. based on path-ind. and DB joins"
	", features combined early and compiled clean-up"  
#ifdef INSTANCE_RETRIEVAL_INDEX_USE_WEIGHT_FILTERS
	" + weight filters " 
#endif 
#ifdef INSTANCE_RETRIEVAL_INDEX_USE_SIGNATURE_FILTERS  
	" + sig. filters " 
#endif
	;
    };

#ifndef NO_DEBUG_VIS
  ostream& outputSubst(ostream& str) const;
#endif

 

  class AbstractMachine
    {
    public: 
      enum InstructionTag
      {
	// Basic atomary actions
	NonmaxEqualiseTermId,
	MaxEqualiseTermId,
	LoadSubst,
	Compare,  
	Success,


	// Compiled clean-up 
	ArgsInit,
	ArgsFetch,  // fetch stored position
	ArgsMove,   // get from cache
	ArgsLoadSubst,
	ArgsCompare,
	
	// Combined and specialised actions
	
	ArgsInitAndStore,
	ArgsFetchAndStore,
	ArgsMoveAndStore
      };
      class Instruction
	{
	public:
	  Instruction() {}; 
	  ~Instruction() {}; 
#ifndef NO_DEBUG_VIS
	  ostream& output(ostream& str,const AbstractMachine* machine,const TermId* substitution) const;
#endif
	public:
	  InstructionTag tag;
	  union
	  {
	    NonmaxPathRelation::SurferWithMemory* nonmaxPathRelationSurfer; 
	    MaxPathRelation::SurferWithMemory* maxPathRelationSurfer;
	    TermArguments* storedArguments;
	  };  
	  const NonmaxPathRelation::Node* const * nonmaxPathCurrNode; // to get the term arguments
	  ulong argNum;
	  TermArguments* argumentsToFetch;
	  TermId* subst;
	}; // class Instruction
    public: 
      AbstractMachine() 
	{
	  DOP(debugEndOfCode = code + (2*MaxTermSize + 1));
	};
      ~AbstractMachine() {};
      bool nextMatch(TermId& matchId);
#ifndef NO_DEBUG_VIS
      ostream& outputCode(ostream& str) const; 
      ulong surferNum(const NonmaxPathRelation::Node* const * currNdAddr) const; 
      ulong surferNum(const NonmaxPathRelation::SurferWithMemory* surf) const;
      ulong surferNum(const MaxPathRelation::SurferWithMemory* surf) const;
#endif
    public:
      // Code
      Instruction code[2*MaxTermSize + 1];
#ifdef DEBUG_NAMESPACE
      Instruction* debugEndOfCode;
#endif
      // Registers
      TermId substitution[MaxTermSize];
      NonmaxPathRelation::SurferWithMemory nonmaxPathRelationSurfers[MaxTermSize];
      MaxPathRelation::SurferWithMemory maxPathRelationSurfers[MaxTermSize];
      TermArguments arguments[MaxTermSize];
      bool firstRetrievalCall;
#ifdef INSTANCE_RETRIEVAL_INDEX_USE_WEIGHT_FILTERS
      ulong queryWeight;
#endif
#ifdef INSTANCE_RETRIEVAL_INDEX_USE_SIGNATURE_FILTERS
      SignatureFilter querySignatureFilter;
#endif
    }; //  class AbstractMachine 

  class IntermediateQueryRepresentation
    {
    public:
      IntermediateQueryRepresentation(AbstractMachine* abstractMachine) 
	: _abstractMachine(abstractMachine)
	{
	  _positions[0].tag = Start;
	  _positions[0].arity = 1UL;
	  _positions[0].numOfVarArgs = 0UL;
	}; 
      ~IntermediateQueryRepresentation() {};
      void reset();
      void compound(const TermId& subterm,ulong arity,const NonmaxPathRelation* pathRelation);
      void constant(const TermId& subterm,const MaxPathRelation* pathRelation);
      void variable(ulong var);
      void endOfQuery();
      void generateCode(); 
#ifndef NO_DEBUG_VIS
      ostream& output(ostream& str);
#endif


    private:
      enum PositionTag { Start,Compound,Constant,Variable,End }; 
      class Position
	{
	public:
	  Position() {};
#ifndef NO_DEBUG_VIS
	  ostream& output(ostream& str,const Position* base) const;
#endif
	public: 
	  PositionTag tag;
	  union 
	  {
	    const NonmaxPathRelation* nonmaxPathRelation;
	    const MaxPathRelation* maxPathRelation;
	  };
	  ulong argNum;      
	  Position* nestPosition;
	  ulong arity;
	  ulong variable;
	  ulong numOfVarArgs;
	  Position* nextVarArg;
	  bool hasNonvarArg;
	  AbstractMachine::Instruction* mainInstruction;

	  // for compiled clean-up:
	  ulong numOfArgsWithSecondaryNests;
	  bool isSecondaryNest;
	}; // class Position 
      class PositionPtr
	{
	public:
	  PositionPtr() {};
	  PositionPtr(Position* p) : ptr(p) {};
	  ~PositionPtr() {};
	  ulong key() const // for sorting 
	    {
	      return (ptr->tag == Constant) 
                ? ptr->maxPathRelation->size() 
	        : ptr->nonmaxPathRelation->size();                     
	    };
	public:
	  Position* ptr;
	};   

    private: 
      void generatePrimaryCode();    
      void generateSecondaryCode(); 
      void generateCodeForSingletons();

      void writeNonmaxEqualiseTermId(Position* primaryNest);
      void writeMaxEqualiseTermId(Position* primaryNest);
      void writeCompare(Position* varPos);
      void writeLoadSubst(Position* varPos);

    private: 
      Position _positions[MaxTermSize + 2];
      AbstractMachine* _abstractMachine;

      // data used locally in constructing interm. rep.
      Position* _nextPosition;
      ulong _currentHoles;
      Position* _currentNest;
      BK::Stack<ulong,MaxTermDepth> _backtrackHoles;
      BK::Stack<Position*,MaxTermDepth> _backtrackNonmaxPositions;

      // data used locally in code generation 
      BK::GIncCountingSort<ulong,PositionPtr,MaxTermSize> _primaryNestSorting;
      Position* _primaryPositions[MaxTermSize]; 
      ulong _numOfPrimaryPositions;
      bool _substLoaded[MaxNumOfVariables];  
      Position* _availableVarPosition[MaxNumOfVariables];
      BK::Multiset<ulong,MaxNumOfVariables> _internalVars;   
      BK::MultisetWithRemoval<ulong,MaxNumOfVariables> _singletonVars;
      AbstractMachine::Instruction* _nextInstr;
      AbstractMachine::Instruction* _lastCompare;
    }; // class IntermediateQueryRepresentation

 private: 
  PathIndexForInstanceRetrieval::Trie _trie; 

  // Integration
  PathIndexForInstanceRetrieval::Integrator _integrator;
  TermTraversal _integratedTermTraversal;
   
  // Removal
  PathIndexForInstanceRetrieval::Removal _removal;
  TermTraversal _removedTermTraversal;

  // Destruction
  // PathIndexForInstanceRetrieval::Destruction _destruction;

  // Query compilation
  
  TermTraversal _queryTraversal;
  PathIndexForInstanceRetrieval::Retrieval _pathRelationRetrieval;
  IntermediateQueryRepresentation _intermediateQueryRepresentation; 
  
#ifndef NO_DEBUG
  BK::LinearCombination<long,MaxNumOfVariables> _debugQueryVariables;
#endif

  // Abstract machine
  
  AbstractMachine _abstractMachine;
}; // class PathIndexingWithJoinsAndEarlyAndCompiledCleanUpForInstanceRetrieval<..>
}; // namespace Gem

//============================================================================
#undef DEBUG_NAMESPACE
#ifdef DEBUG_PATH_INDEXING_WITH_JOINS_AND_EARLY_AND_COMPILED_CLEAN_UP_FOR_INSTANCE_RETRIEVAL
#define DEBUG_NAMESPACE "PathIndexingWithJoinsAndEarlyAndCompiledCleanUpForInstanceRetrieval<Alloc,Term,MaxTermDepth,MaxTermSize,MaxNumOfVariables>"
#endif
#include "debugMacros.hpp"
//============================================================================

namespace Gem
{

template <class Alloc,class Term,ulong MaxTermDepth,ulong MaxTermSize,ulong MaxNumOfVariables>
inline bool PathIndexingWithJoinsAndEarlyAndCompiledCleanUpForInstanceRetrieval<Alloc,Term,MaxTermDepth,MaxTermSize,MaxNumOfVariables>::integrate(const TermId& integratedTermId)
{
  CALL("integrate(const TermId& integratedTermId)");
  _integratedTermTraversal.reset(integratedTermId);
  _integrator.reset(_trie);
  bool newNode;
  ulong arity;
 check_state:
  switch (_integratedTermTraversal.state())
    { 
    case TermTraversal::Func:
      arity = _integratedTermTraversal.symbol().arity();
      if (arity)
	{
	  _integrator.nonconstFunc(_integratedTermTraversal.symbol());
	  NonmaxPathRelation::Node* node = _integrator.indexedObj().insert(integratedTermId,newNode);
	  if (!newNode) return false;
	  node->value().reset(_integratedTermTraversal.term());             
	}
      else // constant
	{
	  _integrator.constant(_integratedTermTraversal.symbol());
	  if (!_integrator.constIndObj().insert(integratedTermId)) return false;
	};
      _integratedTermTraversal.next();
      goto check_state;
 
    case TermTraversal::Var:
      _integrator.variable();
      _integratedTermTraversal.next();
      goto check_state;
     
    case TermTraversal::End: 
      return true; 

#ifdef DEBUG_NAMESPACE 
    default: ICP("ICP0"); return false;
#else
#ifdef _SUPPRESS_WARNINGS_
    default: 
      BK::RuntimeError::report("Wrong term traversal state in PathIndexingWithJoinsAndEarlyAndCompiledCleanUpForInstanceRetrieval<..>::integrate(..)");
      return false;
#endif
#endif       
    };
} // bool PathIndexingWithJoinsAndEarlyAndCompiledCleanUpForInstanceRetrieval<..>::integrate(const TermId& integratedTermId)



template <class Alloc,class Term,ulong MaxTermDepth,ulong MaxTermSize,ulong MaxNumOfVariables>
inline bool PathIndexingWithJoinsAndEarlyAndCompiledCleanUpForInstanceRetrieval<Alloc,Term,MaxTermDepth,MaxTermSize,MaxNumOfVariables>::remove(const TermId& removedTermId)
{
  CALL("remove(const TermId& removedTermId)");
  _removedTermTraversal.reset(removedTermId);
  _removal.reset(_trie);

  bool somethingRemoved = false;
  ulong arity;

 check_state:
  switch (_removedTermTraversal.state())
    { 
    case TermTraversal::Func:
      arity = _removedTermTraversal.symbol().arity();
      if (arity)
	{
	  if (_removal.nonconstFunc(_removedTermTraversal.symbol()))
	    {
	      somethingRemoved = _removal.indexedObj().remove(removedTermId) || somethingRemoved;
	      _removedTermTraversal.next();
	    }
	  else
	    {
	      _removal.after();
	      _removedTermTraversal.after();
	    };
	}
      else // constant
	{
	  if (_removal.constant(_removedTermTraversal.symbol()))
	    {
	      somethingRemoved = _removal.constIndObj().remove(removedTermId) || somethingRemoved;
	    }
	  else
	    {
	      _removal.after();
	    };
	  _removedTermTraversal.next();
	};
      goto check_state;
 
    case TermTraversal::Var:
      _removal.after();
      _removedTermTraversal.next();
      goto check_state;
     
    case TermTraversal::End: 
      _removal.recycleObsoleteNodes();     
      return somethingRemoved;

#ifdef DEBUG_NAMESPACE 
    default: ICP("ICP0"); return false;
#else
#ifdef _SUPPRESS_WARNINGS_
    default: 
      BK::RuntimeError::report("Wrong term traversal state in PathIndexingWithJoinsAndEarlyAndCompiledCleanUpForInstanceRetrieval<..>::remove(..)");
      return false;
#endif
#endif       
    };

} // bool PathIndexingWithJoinsAndEarlyAndCompiledCleanUpForInstanceRetrieval<..>::remove(const TermId& removedTermId)




template <class Alloc,class Term,ulong MaxTermDepth,ulong MaxTermSize,ulong MaxNumOfVariables>
inline bool PathIndexingWithJoinsAndEarlyAndCompiledCleanUpForInstanceRetrieval<Alloc,Term,MaxTermDepth,MaxTermSize,MaxNumOfVariables>::compileQuery(const TermId& query)
{
  CALL("compileQuery(const TermId& query)");

  //DF; cout << "QUERY " << query << "\n";

#ifndef NO_DEBUG
  _debugQueryVariables.reset();
#endif
  _queryTraversal.reset(query);
  _pathRelationRetrieval.reset(_trie);
  _intermediateQueryRepresentation.reset();
  ulong arity;
 check_state: 
  switch (_queryTraversal.state())
    {
    case TermTraversal::Func:
      arity = _queryTraversal.symbol().arity();
      if (arity) 
	{
	  if (!_pathRelationRetrieval.nonconstFunc(_queryTraversal.symbol())) return false;
	  _intermediateQueryRepresentation.compound(_queryTraversal.term(),arity,_pathRelationRetrieval.indObjAddr());
	}
      else // constant 
	{     
	  if (!_pathRelationRetrieval.constant(_queryTraversal.symbol())) return false;
	  _intermediateQueryRepresentation.constant(_queryTraversal.term(),_pathRelationRetrieval.constIndObjAddr());
	};
      _queryTraversal.next(); 
      goto check_state;

    case TermTraversal::Var:
#ifndef NO_DEBUG
      _debugQueryVariables.add1(_queryTraversal.symbol().var());
#endif
      _intermediateQueryRepresentation.variable(_queryTraversal.symbol().var());
      _pathRelationRetrieval.after();
      _queryTraversal.next(); 
      goto check_state;

    case TermTraversal::End: 
      _intermediateQueryRepresentation.endOfQuery();
      _intermediateQueryRepresentation.generateCode();
#ifdef INSTANCE_RETRIEVAL_INDEX_USE_WEIGHT_FILTERS
      _abstractMachine.queryWeight = Term::weight(query);
#endif
#ifdef INSTANCE_RETRIEVAL_INDEX_USE_SIGNATURE_FILTERS
      _abstractMachine.querySignatureFilter = Term::signatureFilter(query);
#endif
      return true;

#ifdef DEBUG_NAMESPACE 
    default: ICP("ICP0"); return false;
#else
#ifdef _SUPPRESS_WARNINGS_
    default: 
      BK::RuntimeError::report("Wrong term traversal state in PathIndexingWithJoinsAndEarlyAndCompiledCleanUpForInstanceRetrieval<..>::compileQuery(..)");
      return false;
#endif
#endif    
    };
 
} // bool PathIndexingWithJoinsAndEarlyAndCompiledCleanUpForInstanceRetrieval<..>::compileQuery(const TermId& query)

template <class Alloc,class Term,ulong MaxTermDepth,ulong MaxTermSize,ulong MaxNumOfVariables>
inline bool PathIndexingWithJoinsAndEarlyAndCompiledCleanUpForInstanceRetrieval<Alloc,Term,MaxTermDepth,MaxTermSize,MaxNumOfVariables>::nextMatch(TermId& matchId)
{
  CALL("nextMatch(TermId& matchId)");
  return _abstractMachine.nextMatch(matchId);
}

#ifndef NO_DEBUG_VIS

template <class Alloc,class Term,ulong MaxTermDepth,ulong MaxTermSize,ulong MaxNumOfVariables>
ostream& PathIndexingWithJoinsAndEarlyAndCompiledCleanUpForInstanceRetrieval<Alloc,Term,MaxTermDepth,MaxTermSize,MaxNumOfVariables>::outputSubst(ostream& str) const
{
  for (const ulong* v = _debugQueryVariables.begin();
       v != _debugQueryVariables.end();
       ++v)
    {
      str << "[X" << *v << " -> " << subst(*v) << "] "; 
    };
  return str;
} // ostream& PathIndexingWithJoinsAndEarlyAndCompiledCleanUpForInstanceRetrieval<Alloc,Term,MaxTermDepth,MaxTermSize,MaxNumOfVariables>::outputSubst(ostream& str) const
#endif

}; // namespace Gem




//============================================================================
#undef DEBUG_NAMESPACE
#ifdef DEBUG_PATH_INDEXING_WITH_JOINS_AND_EARLY_AND_COMPILED_CLEAN_UP_FOR_INSTANCE_RETRIEVAL
#define DEBUG_NAMESPACE "PathIndexingWithJoinsAndEarlyAndCompiledCleanUpForInstanceRetrieval<Alloc,Term,MaxTermDepth,MaxTermSize,MaxNumOfVariables>::IntermediateQueryRepresentation"
#endif
#include "debugMacros.hpp"
//============================================================================

namespace Gem
{
template <class Alloc,class Term,ulong MaxTermDepth,ulong MaxTermSize,ulong MaxNumOfVariables>
inline void PathIndexingWithJoinsAndEarlyAndCompiledCleanUpForInstanceRetrieval<Alloc,Term,MaxTermDepth,MaxTermSize,MaxNumOfVariables>::IntermediateQueryRepresentation::reset()
{
  CALL("reset()");
  _nextPosition = _positions + 1;
  _currentHoles = 1UL;
  _currentNest = _positions;
  _backtrackHoles.reset();
  _backtrackNonmaxPositions.reset();
  _primaryNestSorting.reset();
} // void PathIndexingWithJoinsAndEarlyAndCompiledCleanUpForInstanceRetrieval<..>::IntermediateQueryRepresentation::reset()

template <class Alloc,class Term,ulong MaxTermDepth,ulong MaxTermSize,ulong MaxNumOfVariables>   
inline void PathIndexingWithJoinsAndEarlyAndCompiledCleanUpForInstanceRetrieval<Alloc,Term,MaxTermDepth,MaxTermSize,MaxNumOfVariables>::IntermediateQueryRepresentation::compound(const TermId& subterm,ulong arity,const NonmaxPathRelation* pathRelation)
{
  CALL("compound(const TermId& subterm,ulong arity,const NonmaxPathRelation* pathRelation)");
  _nextPosition->tag = Compound;
  _nextPosition->nonmaxPathRelation = pathRelation;
  _nextPosition->nestPosition = _currentNest;
  _nextPosition->arity = arity;
  _nextPosition->numOfVarArgs = 0UL;
  _nextPosition->nextVarArg = (Position*)0;
  _nextPosition->hasNonvarArg = false;

  _nextPosition->numOfArgsWithSecondaryNests = 0UL;
  _nextPosition->isSecondaryNest = false;

  _currentNest->hasNonvarArg = true;

  if ((_currentNest != _positions) && 
      (!_currentNest->isSecondaryNest) && 
      (_currentNest->numOfVarArgs))
    {
      _currentNest->isSecondaryNest = true;
      Position* nest = _currentNest->nestPosition;
    check_nest:
      if (nest != _positions)
	{
	  ++(nest->numOfArgsWithSecondaryNests);
	  if ((!nest->isSecondaryNest) && 
	      (nest->numOfArgsWithSecondaryNests == 1UL))
	    {
	      nest = nest->nestPosition;
	      goto check_nest;
	    };

	};
    };


  ASSERT(_currentHoles);
  _nextPosition->argNum = _currentNest->arity - _currentHoles; 
  _currentHoles--;
 
  if (_currentHoles) 
    {
      _backtrackHoles.push(_currentHoles);
      _backtrackNonmaxPositions.push(_currentNest);   
    };
  _currentHoles = arity;
  _currentNest = _nextPosition;
  _nextPosition++;

} // void PathIndexingWithJoinsAndEarlyAndCompiledCleanUpForInstanceRetrieval<..>::IntermediateQueryRepresentation::compound(const TermId& subterm,ulong arity,const NonmaxPathRelation* pathRelation)

template <class Alloc,class Term,ulong MaxTermDepth,ulong MaxTermSize,ulong MaxNumOfVariables>    
inline void PathIndexingWithJoinsAndEarlyAndCompiledCleanUpForInstanceRetrieval<Alloc,Term,MaxTermDepth,MaxTermSize,MaxNumOfVariables>::IntermediateQueryRepresentation::constant(const TermId& subterm,const MaxPathRelation* pathRelation)
{
  CALL("constant(const TermId& subterm,const MaxPathRelation* pathRelation)");
  _nextPosition->tag = Constant;
  _nextPosition->maxPathRelation = pathRelation;
  _nextPosition->nestPosition = _currentNest; 

  _currentNest->hasNonvarArg = true;

  if ((_currentNest != _positions) && 
      (!_currentNest->isSecondaryNest) && 
      (_currentNest->numOfVarArgs))
    {
      _currentNest->isSecondaryNest = true;
      Position* nest = _currentNest->nestPosition;
    check_nest:
      if (nest != _positions)
	{
	  ++(nest->numOfArgsWithSecondaryNests);
	  if ((!nest->isSecondaryNest) && 
	      (nest->numOfArgsWithSecondaryNests == 1UL))
	    {
	      nest = nest->nestPosition;
	      goto check_nest;
	    };

	};
    };

  ASSERT(_currentHoles);
  _nextPosition->argNum = _currentNest->arity - _currentHoles;
  _currentHoles--;
  if (!_currentHoles)
    {
      if (_backtrackHoles)
	{
	  _currentHoles = _backtrackHoles.pop();
	  ASSERT(_currentHoles);
	  _currentNest = _backtrackNonmaxPositions.pop();
	};
    };

  // _nextPosition is a primary position

  _primaryNestSorting.push(PositionPtr(_nextPosition));

  _nextPosition++;
} // void PathIndexingWithJoinsAndEarlyAndCompiledCleanUpForInstanceRetrieval<..>::IntermediateQueryRepresentation::constant(const TermId& subterm,const MaxPathRelation* pathRelation)

template <class Alloc,class Term,ulong MaxTermDepth,ulong MaxTermSize,ulong MaxNumOfVariables>    
inline void PathIndexingWithJoinsAndEarlyAndCompiledCleanUpForInstanceRetrieval<Alloc,Term,MaxTermDepth,MaxTermSize,MaxNumOfVariables>::IntermediateQueryRepresentation::variable(ulong var)
{
  CALL("variable(ulong var)");

  _substLoaded[var] = false;
  _availableVarPosition[var] = (Position*)0;
  _nextPosition->tag = Variable;
  _nextPosition->nestPosition = _currentNest;
  _nextPosition->variable = var;
  _currentNest->numOfVarArgs++;
  _nextPosition->nextVarArg = _currentNest->nextVarArg; 
  _currentNest->nextVarArg = _nextPosition;


  if ((_currentNest != _positions) && 
      (!_currentNest->isSecondaryNest) && 
      (_currentNest->hasNonvarArg))
    {
      _currentNest->isSecondaryNest = true;
      Position* nest = _currentNest->nestPosition;
    check_nest:
      if (nest != _positions)
	{
	  ++(nest->numOfArgsWithSecondaryNests);
	  if ((!nest->isSecondaryNest) && 
	      (nest->numOfArgsWithSecondaryNests == 1UL))
	    {
	      nest = nest->nestPosition;
	      goto check_nest;
	    };

	};
    };

  ASSERT(_currentHoles);
  _nextPosition->argNum = _currentNest->arity - _currentHoles; 
  _currentHoles--;
  if (!_currentHoles)
    {
      if (!_currentNest->hasNonvarArg) // _currentNest is a primary nest
	{
	  ASSERT(!_currentNest->isSecondaryNest);
	  _primaryNestSorting.push(PositionPtr(_currentNest));
	}; 
      if (_backtrackHoles)
	{
	  _currentHoles = _backtrackHoles.pop();
	  ASSERT(_currentHoles);
	  _currentNest = _backtrackNonmaxPositions.pop();
	};
    };

  _nextPosition++;

} // void PathIndexingWithJoinsAndEarlyAndCompiledCleanUpForInstanceRetrieval<..>::IntermediateQueryRepresentation::variable(ulong var)

template <class Alloc,class Term,ulong MaxTermDepth,ulong MaxTermSize,ulong MaxNumOfVariables>    
inline void PathIndexingWithJoinsAndEarlyAndCompiledCleanUpForInstanceRetrieval<Alloc,Term,MaxTermDepth,MaxTermSize,MaxNumOfVariables>::IntermediateQueryRepresentation::endOfQuery()
{
  CALL("endOfQuery()");
  ASSERT(!_currentHoles);
  ASSERT(!_backtrackHoles);
  _nextPosition->tag = End;
  _numOfPrimaryPositions = _primaryNestSorting.size();
  for (ulong i = 0; i < _numOfPrimaryPositions; i++)
    _primaryPositions[_primaryNestSorting.num()[i]] = _primaryNestSorting.elem()[i].ptr;

  //DF; output(cout << "\n\n") << "\n";

} // void PathIndexingWithJoinsAndEarlyAndCompiledCleanUpForInstanceRetrieval<..>::IntermediateQueryRepresentation::endOfQuery()


template <class Alloc,class Term,ulong MaxTermDepth,ulong MaxTermSize,ulong MaxNumOfVariables>    
inline void PathIndexingWithJoinsAndEarlyAndCompiledCleanUpForInstanceRetrieval<Alloc,Term,MaxTermDepth,MaxTermSize,MaxNumOfVariables>::IntermediateQueryRepresentation::generateCode()
{
  CALL("generateCode()");
 
  _nextInstr = _abstractMachine->code; 
 
  // Compile primary nests
  generatePrimaryCode(); 

  // Compile remaining clean-up
  generateSecondaryCode(); 
  generateCodeForSingletons(); 
  
  _nextInstr->tag = AbstractMachine::Success;

  _abstractMachine->firstRetrievalCall = true;

  //DF; _abstractMachine->outputCode(cout << "\n\n") << "\n";

} // void PathIndexingWithJoinsAndEarlyAndCompiledCleanUpForInstanceRetrieval<..>::IntermediateQueryRepresentation::generateCode()






template <class Alloc,class Term,ulong MaxTermDepth,ulong MaxTermSize,ulong MaxNumOfVariables>    
inline void PathIndexingWithJoinsAndEarlyAndCompiledCleanUpForInstanceRetrieval<Alloc,Term,MaxTermDepth,MaxTermSize,MaxNumOfVariables>::IntermediateQueryRepresentation::generatePrimaryCode()
{
  CALL("generatePrimaryCode()");
  
  // Compile primary nests

  _singletonVars.reset();

  for (ulong i = 0; i < _numOfPrimaryPositions; i++)
    {
      Position* primaryNest = _primaryPositions[i];
      if (primaryNest->tag == Constant)
	{  
	  writeMaxEqualiseTermId(primaryNest);    
	}
      else
	{ 
	  writeNonmaxEqualiseTermId(primaryNest);

	  // Write all immediately available Compare instructions 
	  // for the variable arguments in primaryNest
	  for (Position* newVarPos = primaryNest->nextVarArg; newVarPos; newVarPos = newVarPos->nextVarArg)
	    {
	      ulong newVar = newVarPos->variable;        
	      if (_substLoaded[newVar])
		{
		  writeCompare(newVarPos);
		  ASSERT(newVarPos->mainInstruction);
		}
	      else
		{
		  newVarPos->mainInstruction = (AbstractMachine::Instruction*)0;
		};
	    };
           
	  // Write remaining Compare instructions 
	  // for the variable arguments in primaryNest
	  // using also the accumulated variable positions _availableVarPosition
	  for (Position* newVarPos = primaryNest->nextVarArg; newVarPos; newVarPos = newVarPos->nextVarArg)
	    {
	      if (!newVarPos->mainInstruction)
		{ 
		  ulong newVar = newVarPos->variable;
		  ASSERT(!_substLoaded[newVar]);
		  Position* availableVarPos = _availableVarPosition[newVar];
		  if (availableVarPos)
		    {
		      ASSERT(_singletonVars.scoreOf(newVar));
		      // load instantiation         
		      writeLoadSubst(availableVarPos);
		      
		      _substLoaded[newVar] = true;
		      _availableVarPosition[newVar] = (Position*)0;
		      _singletonVars.removeAll(newVar);

		      // write the Compare instruction
		      writeCompare(newVarPos);

		      // now some other Compare instructions
		      // may have become immediately available
		      for (Position* newVarPos1 = newVarPos->nextVarArg; newVarPos1; newVarPos1 = newVarPos1->nextVarArg)
			{
			  if (newVarPos1->variable == newVar)
			    {
			      writeCompare(newVarPos1);
			    };
			};
		    }
		  else
		    {
		      _availableVarPosition[newVar] = newVarPos;
		      _singletonVars.add(newVar);
		    };
		};
	    };
	};
   
    };

} // void PathIndexingWithJoinsAndEarlyAndCompiledCleanUpForInstanceRetrieval<..>::IntermediateQueryRepresentation::generatePrimaryCode()



template <class Alloc,class Term,ulong MaxTermDepth,ulong MaxTermSize,ulong MaxNumOfVariables>    
inline void PathIndexingWithJoinsAndEarlyAndCompiledCleanUpForInstanceRetrieval<Alloc,Term,MaxTermDepth,MaxTermSize,MaxNumOfVariables>::IntermediateQueryRepresentation::generateSecondaryCode()
{
  CALL("generateSecondaryCode()");

  Position* argsCache = (Position*)0;
  Position* pos = _positions + 1;
  

  if ((pos->numOfVarArgs && pos->hasNonvarArg) || 
      (pos->numOfArgsWithSecondaryNests))
    {
      // first position
      if (pos->numOfArgsWithSecondaryNests > 1UL)
	{
	  _nextInstr->tag = AbstractMachine::ArgsInitAndStore;
	  _nextInstr->storedArguments = _abstractMachine->arguments + (pos - _positions); 
	}
      else
	{
	  _nextInstr->tag = AbstractMachine::ArgsInit;
	};
      argsCache = pos;
      ++_nextInstr;
      
      for (Position* varArg = pos->nextVarArg; varArg; varArg = varArg->nextVarArg)
	{ 
	  ulong var = varArg->variable;
	  Position* availableVarPos = _availableVarPosition[var];
	  if (availableVarPos)
	    {
	      ASSERT(!_substLoaded[var]);
	      ASSERT(_singletonVars.scoreOf(var));
	      writeLoadSubst(availableVarPos);
	      _substLoaded[var] = true;
	      _availableVarPosition[var] = (Position*)0;
	      _singletonVars.removeAll(var);
	    };

	  if (_substLoaded[var])
	   {
	     _nextInstr->tag = AbstractMachine::ArgsCompare;
           }
          else
	   {
	     _nextInstr->tag = AbstractMachine::ArgsLoadSubst;
	    _substLoaded[var] = true;
           };	  
	  _nextInstr->argNum = varArg->argNum; 
          _nextInstr->subst = _abstractMachine->substitution + var; 
	  ++_nextInstr;
	 };
      
      // remaining positions
      for (++pos; pos < _nextPosition; ++pos)
	{
	  if ((pos->tag == Compound) &&
	      (((pos->numOfVarArgs && pos->hasNonvarArg) || 
		(pos->numOfArgsWithSecondaryNests))))
	    {
	      // relevant position
	      
	      // get the arguments
	      if (pos->nestPosition == argsCache)
		{
		  // arguments can be taken from cache
		  if (pos->numOfArgsWithSecondaryNests > 1UL)
		    {
		      _nextInstr->tag = AbstractMachine::ArgsMoveAndStore;
		      _nextInstr->storedArguments = _abstractMachine->arguments + (pos - _positions);
		    }
		  else
		    {
		      _nextInstr->tag = AbstractMachine::ArgsMove;
		    };
		  _nextInstr->argNum = pos->argNum; 
		}
	      else // arguments must be fetched
		{
		  if (pos->numOfArgsWithSecondaryNests > 1UL)
		    {
		      _nextInstr->tag = AbstractMachine::ArgsFetchAndStore; 
		      _nextInstr->storedArguments = _abstractMachine->arguments + (pos - _positions);
		      
		    }
		  else
		    {
		      _nextInstr->tag = AbstractMachine::ArgsFetch; 
		    };        
		  _nextInstr->argumentsToFetch =  _abstractMachine->arguments + (pos->nestPosition - _positions);
		  _nextInstr->argNum = pos->argNum;
		};
	      
	      argsCache = pos;
	      ++_nextInstr;

	      // compile variable arguments
	      for (Position* varArg = pos->nextVarArg; varArg; varArg = varArg->nextVarArg)
		{ 
		  ulong var = varArg->variable;
		  Position* availableVarPos = _availableVarPosition[var];
		  if (availableVarPos)
		    {
		      ASSERT(!_substLoaded[var]);
		      ASSERT(_singletonVars.scoreOf(var));
		      writeLoadSubst(availableVarPos);
		      _substLoaded[var] = true;
		      _availableVarPosition[var] = (Position*)0;
		      _singletonVars.removeAll(var);
		    };

		  if (_substLoaded[var])
		    {
		      _nextInstr->tag = AbstractMachine::ArgsCompare;
		    }
		  else
		    {
		      _nextInstr->tag = AbstractMachine::ArgsLoadSubst;
		      _substLoaded[var] = true;
		    };	  
		  _nextInstr->argNum = varArg->argNum; 
		  _nextInstr->subst = _abstractMachine->substitution + var; 
		  ++_nextInstr;
		};
	    };
	};
    };
} // void PathIndexingWithJoinsAndEarlyAndCompiledCleanUpForInstanceRetrieval<..>::IntermediateQueryRepresentation::generateSecondaryCode()



template <class Alloc,class Term,ulong MaxTermDepth,ulong MaxTermSize,ulong MaxNumOfVariables>    
inline void PathIndexingWithJoinsAndEarlyAndCompiledCleanUpForInstanceRetrieval<Alloc,Term,MaxTermDepth,MaxTermSize,MaxNumOfVariables>::IntermediateQueryRepresentation::generateCodeForSingletons()
{
  CALL("generateCodeForSingletons()");
  // Compile loading instances for singletone variables
  ulong numOfSingletonVars = _singletonVars.numOfDiffElements();
  for (ulong n = 0UL; n < numOfSingletonVars; n++) 
    {
      ulong var = _singletonVars.nth(n);
      Position* availableVarPos = _availableVarPosition[var];
      ASSERT(availableVarPos);
      ASSERT(availableVarPos->nestPosition->mainInstruction);
      ASSERT(!_substLoaded[var]);
      writeLoadSubst(availableVarPos);
    };
} // void PathIndexingWithJoinsAndEarlyAndCompiledCleanUpForInstanceRetrieval<..>::IntermediateQueryRepresentation::generateCodeForSingletons()




template <class Alloc,class Term,ulong MaxTermDepth,ulong MaxTermSize,ulong MaxNumOfVariables>    
inline void PathIndexingWithJoinsAndEarlyAndCompiledCleanUpForInstanceRetrieval<Alloc,Term,MaxTermDepth,MaxTermSize,MaxNumOfVariables>::IntermediateQueryRepresentation::writeNonmaxEqualiseTermId(Position* primaryNest)
{
  CALL("writeNonmaxEqualiseTermId(Position* primaryNest)");
  ASSERT(primaryNest->tag == Compound);
  ASSERT(!primaryNest->hasNonvarArg);
  _nextInstr->tag = AbstractMachine::NonmaxEqualiseTermId;    
  NonmaxPathRelation::SurferWithMemory* nextNonmaxPathRelationSurfer = 
    _abstractMachine->nonmaxPathRelationSurfers + (primaryNest - _positions);
  nextNonmaxPathRelationSurfer->reset(*(primaryNest->nonmaxPathRelation));  
  _nextInstr->nonmaxPathRelationSurfer = nextNonmaxPathRelationSurfer; 
  primaryNest->mainInstruction = _nextInstr;
  _nextInstr++;
} // void PathIndexingWithJoinsAndEarlyAndCompiledCleanUpForInstanceRetrieval<..>::IntermediateQueryRepresentation::writeNonmaxEqualiseTermId(Position* primaryNest)

template <class Alloc,class Term,ulong MaxTermDepth,ulong MaxTermSize,ulong MaxNumOfVariables>    
inline void PathIndexingWithJoinsAndEarlyAndCompiledCleanUpForInstanceRetrieval<Alloc,Term,MaxTermDepth,MaxTermSize,MaxNumOfVariables>::IntermediateQueryRepresentation::writeMaxEqualiseTermId(Position* primaryNest)
{
  CALL("writeMaxEqualiseTermId(Position* primaryNest)");
  ASSERT(primaryNest->tag == Constant);
  _nextInstr->tag = AbstractMachine::MaxEqualiseTermId;
  MaxPathRelation::SurferWithMemory* nextMaxPathRelationSurfer = 
    _abstractMachine->maxPathRelationSurfers + (primaryNest - _positions);
  nextMaxPathRelationSurfer->reset(*(primaryNest->maxPathRelation));
  _nextInstr->maxPathRelationSurfer = nextMaxPathRelationSurfer;
  _nextInstr++;
} // void PathIndexingWithJoinsAndEarlyAndCompiledCleanUpForInstanceRetrieval<..>::IntermediateQueryRepresentation::writeMaxEqualiseTermId(Position* primaryNest)


template <class Alloc,class Term,ulong MaxTermDepth,ulong MaxTermSize,ulong MaxNumOfVariables>    
inline void PathIndexingWithJoinsAndEarlyAndCompiledCleanUpForInstanceRetrieval<Alloc,Term,MaxTermDepth,MaxTermSize,MaxNumOfVariables>::IntermediateQueryRepresentation::writeCompare(Position* varPos)
{
  CALL("writeCompare(Position* varPos)");
  ASSERT(varPos->nestPosition->tag == Compound);
  ASSERT(varPos->tag == Variable); 
  _nextInstr->tag = AbstractMachine::Compare;
  varPos->mainInstruction = _nextInstr;
  _nextInstr->nonmaxPathCurrNode = 
    _abstractMachine->nonmaxPathRelationSurfers[varPos->nestPosition - _positions].currNodeAddr(); 
  _nextInstr->argNum = varPos->argNum;
  _nextInstr->subst = _abstractMachine->substitution + varPos->variable; 
  _nextInstr++;
} // void PathIndexingWithJoinsAndEarlyAndCompiledCleanUpForInstanceRetrieval<..>::IntermediateQueryRepresentation::writeCompare(Position* varPos)

template <class Alloc,class Term,ulong MaxTermDepth,ulong MaxTermSize,ulong MaxNumOfVariables>    
inline void PathIndexingWithJoinsAndEarlyAndCompiledCleanUpForInstanceRetrieval<Alloc,Term,MaxTermDepth,MaxTermSize,MaxNumOfVariables>::IntermediateQueryRepresentation::writeLoadSubst(Position* varPos)
{
  CALL("writeLoadSubst(Position* varPos)");
  ASSERT(varPos->nestPosition->tag == Compound);
  ASSERT(varPos->tag == Variable); 
  _nextInstr->tag = AbstractMachine::LoadSubst;
  varPos->mainInstruction = _nextInstr;
  _nextInstr->nonmaxPathCurrNode = 
    _abstractMachine->nonmaxPathRelationSurfers[varPos->nestPosition - _positions].currNodeAddr(); 
  _nextInstr->argNum = varPos->argNum;
  _nextInstr->subst = _abstractMachine->substitution + varPos->variable; 
  _nextInstr++;
} // void PathIndexingWithJoinsAndEarlyAndCompiledCleanUpForInstanceRetrieval<..>::IntermediateQueryRepresentation::writeLoadSubst(Position* varPos)



#ifndef NO_DEBUG_VIS
template <class Alloc,class Term,ulong MaxTermDepth,ulong MaxTermSize,ulong MaxNumOfVariables>    
inline ostream& PathIndexingWithJoinsAndEarlyAndCompiledCleanUpForInstanceRetrieval<Alloc,Term,MaxTermDepth,MaxTermSize,MaxNumOfVariables>::IntermediateQueryRepresentation::output(ostream& str)
{
  for (const Position* p = _positions; p < _nextPosition; p++)
    p->output(str,_positions) << "\n";

  str << "\nPRIM: { ";
  for (ulong i = 0; i < _numOfPrimaryPositions; i++)
    {
      str << (_primaryPositions[i] - _positions) 
	  << "(" << PositionPtr(_primaryPositions[i]).key() << ") ";
    };
  str << "}";
 

  return str << "\n";
} // ostream& PathIndexingWithJoinsAndEarlyAndCompiledCleanUpForInstanceRetrieval<..>::IntermediateQueryRepresentation::output(ostream& str)
#endif

}; // namespace Gem



//============================================================================
#undef DEBUG_NAMESPACE
#ifdef DEBUG_PATH_INDEXING_WITH_JOINS_AND_EARLY_AND_COMPILED_CLEAN_UP_FOR_INSTANCE_RETRIEVAL
#define DEBUG_NAMESPACE "PathIndexingWithJoinsAndEarlyAndCompiledCleanUpForInstanceRetrieval<Alloc,Term,MaxTermDepth,MaxTermSize,MaxNumOfVariables>::IntermediateQueryRepresentation::Position"
#endif
#include "debugMacros.hpp"
//============================================================================

namespace Gem
{

#ifndef NO_DEBUG_VIS
template <class Alloc,class Term,ulong MaxTermDepth,ulong MaxTermSize,ulong MaxNumOfVariables>    
inline ostream& PathIndexingWithJoinsAndEarlyAndCompiledCleanUpForInstanceRetrieval<Alloc,Term,MaxTermDepth,MaxTermSize,MaxNumOfVariables>::IntermediateQueryRepresentation::Position::output(ostream& str,const Position* base) const
{
  str << (this - base) << " ";
  switch (tag) 
    {
    case Start: str << "Start" << " arity(" << arity << ")"; break;
    case Compound: 
      str << "Compound" 
	  << " arg(" << argNum << ")"
	  << " nest(" << (nestPosition - base) << ")"
	  << " pr(" << (ulong)nonmaxPathRelation << ")"
	  << " arity(" << arity << ")"
	  << " numOfVarArgs(" << numOfVarArgs << ")"
	  << " numOfArgsWithSecondaryNests(" << numOfArgsWithSecondaryNests << ")"; 
      if (hasNonvarArg) { str << " hasNonvarArg"; } else str << "             ";
      if (nextVarArg)
	{
	  str << " varArgs{";
	  for (Position* pos = nextVarArg; pos; pos = pos->nextVarArg)
	    {
	      str << (pos - base);
	      if (pos->nextVarArg) str << ",";
	    };
	  str << "}";
	};
      break;   
    case Constant: 
      str << "Constant" 
	  << " arg(" << argNum << ")"
	  << " nest(" << (nestPosition - base) << ")"
	  << " pr(" << (ulong)maxPathRelation << ")"; 
      break;  
    case Variable: 
      str << "Variable" 
	  << " arg(" << argNum << ")"
	  << " nest(" << (nestPosition - base) << ")"
	  << " #" << variable;   
      break;
    case End: str << "End"; break;
    default: str << "Unknown position tag"; break;
    };
  return str;
} // ostream& PathIndexingWithJoinsAndEarlyAndCompiledCleanUpForInstanceRetrieval<..>::IntermediateQueryRepresentation::Position::output(ostream& str,const Position* base) const
#endif

}; // namespace Gem


//============================================================================
#undef DEBUG_NAMESPACE
#ifdef DEBUG_PATH_INDEXING_WITH_JOINS_AND_EARLY_AND_COMPILED_CLEAN_UP_FOR_INSTANCE_RETRIEVAL
#define DEBUG_NAMESPACE "PathIndexingWithJoinsAndEarlyAndCompiledCleanUpForInstanceRetrieval<Alloc,Term,MaxTermDepth,MaxTermSize,MaxNumOfVariables>::AbstractMachine"
#endif
#include "debugMacros.hpp"
//============================================================================

namespace Gem
{

template <class Alloc,class Term,ulong MaxTermDepth,ulong MaxTermSize,ulong MaxNumOfVariables>  
inline bool PathIndexingWithJoinsAndEarlyAndCompiledCleanUpForInstanceRetrieval<Alloc,Term,MaxTermDepth,MaxTermSize,MaxNumOfVariables>::AbstractMachine::nextMatch(TermId& matchId)
{
  CALL("nextMatch(TermId& matchId)");
  Instruction* instr = code;
  TermId termId;
  TermArguments cachedArguments;

  // Initialise termId

  //DF; instr->output(cout << "IN ",this,substitution) << "\n"; 


  switch (instr->tag) 
    { 
    case NonmaxEqualiseTermId:

      //DF; cout << "InitId PrimVar " << firstRetrievalCall << "\n";

      if (firstRetrievalCall) 
	{
	  firstRetrievalCall = false;
	}
      else  
	{
	  if (!instr->nonmaxPathRelationSurfer->next()) return false;
	};
      ASSERT_IN(instr->nonmaxPathRelationSurfer->currNode(),"A100");  
      termId = instr->nonmaxPathRelationSurfer->currNode()->key();

#ifdef INSTANCE_RETRIEVAL_INDEX_USE_FILTERS

      while (INSTANCE_RETRIEVAL_INDEX_FILTER_OUT(termId))
	{
	  if (!instr->nonmaxPathRelationSurfer->next()) return false;
	  termId = instr->nonmaxPathRelationSurfer->currNode()->key();
	};
#endif
      break;    

    case MaxEqualiseTermId:

      //DF; cout << "InitId PrimConst " << firstRetrievalCall << "\n";

      if (firstRetrievalCall) 
	{
	  firstRetrievalCall = false;
	}
      else  
	{
	  if (!instr->maxPathRelationSurfer->next()) return false;
	};
      ASSERT(instr->maxPathRelationSurfer->currNode());
      termId = instr->maxPathRelationSurfer->currNode()->key();
#ifdef INSTANCE_RETRIEVAL_INDEX_USE_FILTERS
    


      while (INSTANCE_RETRIEVAL_INDEX_FILTER_OUT(termId))
	{
	  if (!instr->maxPathRelationSurfer->next()) return false;
	  termId = instr->maxPathRelationSurfer->currNode()->key();
	};
#endif
      break;


#ifdef DEBUG_NAMESPACE 
    default: ICP("ICP0"); return false; 
#else
#ifdef _SUPPRESS_WARNINGS_
    default: 
      BK::RuntimeError::report("Wrong tag in PathIndexingWithJoinsAndEarlyAndCompiledCleanUpForInstanceRetrieval<..>::AbstractMachine::nextMatch(TermId& matchId)"); 
      return false;
#endif   
#endif
    };
 
 next_instr:
  instr++;
 perform_instr:
  ASSERT(instr < debugEndOfCode);

  //DF; instr->output(cout << "FW ",this,substitution) << "\n"; 

  switch (instr->tag)
    {
    case NonmaxEqualiseTermId:
       

      //DF; cout << "Forward PrimVar\n";

      if (instr->nonmaxPathRelationSurfer->find(termId)) goto next_instr; 
      if (instr->nonmaxPathRelationSurfer->currNode())
	{
	  termId = instr->nonmaxPathRelationSurfer->currNode()->key();
      
#ifdef INSTANCE_RETRIEVAL_INDEX_USE_FILTERS
	  while (INSTANCE_RETRIEVAL_INDEX_FILTER_OUT(termId))
	    {
	      if (!instr->nonmaxPathRelationSurfer->next()) return false;
	      termId = instr->nonmaxPathRelationSurfer->currNode()->key();

	    };
#endif

	  instr = code;
	
	  //DF; cout << "NotFound\n";

	  goto perform_instr;
	}
      else 
	{

	  //DF; cout << "Failure\n";     

	  return false; 
	};   

    case MaxEqualiseTermId:
      //DF; cout << "Forward PrimConst\n";

      if (instr->maxPathRelationSurfer->find(termId)) goto next_instr; 
      if (instr->maxPathRelationSurfer->currNode())
	{
	  termId = instr->maxPathRelationSurfer->currNode()->key();
#ifdef INSTANCE_RETRIEVAL_INDEX_USE_FILTERS
	  while (INSTANCE_RETRIEVAL_INDEX_FILTER_OUT(termId))
	    {
	      if (!instr->maxPathRelationSurfer->next()) return false;
	      termId = instr->maxPathRelationSurfer->currNode()->key();
	    };
#endif

	  instr = code;

	  //DF; cout << "NotFound\n";

	  goto perform_instr;
	}
      else 
	{

	  //DF; cout << "Failure\n";     

	  return false;
	};

    case LoadSubst:
      *(instr->subst) = *((*(instr->nonmaxPathCurrNode))->value().nth(instr->argNum));    
      goto next_instr;
          
    case Compare: 
      if ((*(instr->subst)) != (*(*(instr->nonmaxPathCurrNode))->value().nth(instr->argNum))) 
	goto backtrack;
      goto next_instr;

    case Success:
      matchId = termId;     
      
      //DF; cout << "Success\n";

      return true;

     case ArgsInit:

       //DF; cout << "CANDIDATE " << termId << "\n";
      cachedArguments.reset(termId);
      goto next_instr;

     case ArgsFetch:
      cachedArguments.reset(*(instr->argumentsToFetch->nth(instr->argNum)));
      goto next_instr;

     case ArgsMove:
      cachedArguments.reset(*(cachedArguments.nth(instr->argNum)));
      goto next_instr;    

     case ArgsLoadSubst:
      *(instr->subst) = *(cachedArguments.nth(instr->argNum));
      goto next_instr;    

     case ArgsCompare:
      if (*(instr->subst) != *(cachedArguments.nth(instr->argNum))) goto backtrack;
      goto next_instr; 

     case ArgsInitAndStore:

       //DF; cout << "CANDIDATE " << termId << "\n";

      cachedArguments.reset(termId);
      *(instr->storedArguments) = cachedArguments;
      goto next_instr;

     case ArgsFetchAndStore: 
      cachedArguments.reset(*(instr->argumentsToFetch->nth(instr->argNum)));
      *(instr->storedArguments) = cachedArguments;
      goto next_instr;
     
     case ArgsMoveAndStore: 
      cachedArguments.reset(*(cachedArguments.nth(instr->argNum))); 
      *(instr->storedArguments) = cachedArguments;
      goto next_instr;  

#ifdef DEBUG_NAMESPACE 
    default: ICP("ICP1"); return false; 
#else
#ifdef _SUPPRESS_WARNINGS_
    default: 
      BK::RuntimeError::report("Wrong tag in PathIndexingWithJoinsAndEarlyAndCompiledCleanUpForInstanceRetrieval<..>::AbstractMachine::nextMatch(TermId& matchId)"); 
      return false;
#endif   
#endif
    };

  ICP("ICP2");
 backtrack:
 
  BK::GlobalStopFlag::check();
  instr = code;

  //DF; instr->output(cout << "BK ",this,substitution) << "\n"; 

  switch (instr->tag)
    {
    case NonmaxEqualiseTermId:

      //DF; cout << "Backtrack PrimVar\n";

      if (instr->nonmaxPathRelationSurfer->next()) 
	{
	  ASSERT_IN(instr->nonmaxPathRelationSurfer->currNode(),"A200");
	  termId = instr->nonmaxPathRelationSurfer->currNode()->key();
#ifdef INSTANCE_RETRIEVAL_INDEX_USE_FILTERS
	  while (INSTANCE_RETRIEVAL_INDEX_FILTER_OUT(termId))
	    {
	      if (!instr->nonmaxPathRelationSurfer->next()) return false;
	      termId = instr->nonmaxPathRelationSurfer->currNode()->key();
	    };
#endif

	  goto next_instr;
	}
      else 
	{

	  //DF; cout << "Failure\n";     

	  return false;
	};

    case MaxEqualiseTermId:

      //DF; cout << "Backtrack PrimConst\n";

      if (instr->maxPathRelationSurfer->next())
	{
	  ASSERT(instr->maxPathRelationSurfer->currNode());
	  termId = instr->maxPathRelationSurfer->currNode()->key();
#ifdef INSTANCE_RETRIEVAL_INDEX_USE_FILTERS
	  while (INSTANCE_RETRIEVAL_INDEX_FILTER_OUT(termId))
	    {
	      if (!instr->maxPathRelationSurfer->next()) return false;
	      termId = instr->maxPathRelationSurfer->currNode()->key();
	    };
#endif
	  goto next_instr;
	}
      else 
	{

	  //DF; cout << "Failure\n";     

	  return false;
	};

#ifdef DEBUG_NAMESPACE 
    default: ICP("ICP3"); return false; 
#else
#ifdef _SUPPRESS_WARNINGS_
    default: 
      BK::RuntimeError::report("Wrong tag in PathIndexingWithJoinsAndEarlyAndCompiledCleanUpForInstanceRetrieval<..>::AbstractMachine::nextMatch(TermId& matchId)"); 
      return false;
#endif   
#endif
    };

} // bool PathIndexingWithJoinsAndEarlyAndCompiledCleanUpForInstanceRetrieval<..>::AbstractMachine::nextMatch(TermId& matchId)


#ifndef NO_DEBUG_VIS
       
template <class Alloc,class Term,ulong MaxTermDepth,ulong MaxTermSize,ulong MaxNumOfVariables>  
inline ostream& PathIndexingWithJoinsAndEarlyAndCompiledCleanUpForInstanceRetrieval<Alloc,Term,MaxTermDepth,MaxTermSize,MaxNumOfVariables>::AbstractMachine::outputCode(ostream& str) const
{
  CALL("outputCode(ostream& str) const");
  const Instruction* instr = code;
  while (instr->tag != Success) 
    {
      instr->output(str,this,substitution) << "\n";
      instr++;
    }; 
  instr->output(str,this,substitution) << "\n"; 
  return str;
} //  PathIndexingWithJoinsAndEarlyAndCompiledCleanUpForInstanceRetrieval<..>::AbstractMachine::outputCode(ostream& str) const  

#endif

}; // namespace Gem



//============================================================================
#undef DEBUG_NAMESPACE
#ifdef DEBUG_PATH_INDEXING_WITH_JOINS_AND_EARLY_AND_COMPILED_CLEAN_UP_FOR_INSTANCE_RETRIEVAL
#define DEBUG_NAMESPACE "PathIndexingWithJoinsAndEarlyAndCompiledCleanUpForInstanceRetrieval<Alloc,Term,MaxTermDepth,MaxTermSize,MaxNumOfVariables>::AbstractMachine::Instruction"
#endif
#include "debugMacros.hpp"
//============================================================================

namespace Gem
{

#ifndef NO_DEBUG_VIS
       
template <class Alloc,class Term,ulong MaxTermDepth,ulong MaxTermSize,ulong MaxNumOfVariables>  
inline ostream& PathIndexingWithJoinsAndEarlyAndCompiledCleanUpForInstanceRetrieval<Alloc,Term,MaxTermDepth,MaxTermSize,MaxNumOfVariables>::AbstractMachine::Instruction::output(ostream& str,const AbstractMachine* machine,const TermId* substitution) const
{
  CALL("output(ostream& str,const AbstractMachine* machine,const TermId* substitution) const");
  str << (this - machine->code) << " ";
  switch (tag) 
    {
    case NonmaxEqualiseTermId:
      str << "NonmaxEqualiseTermId"
	  << " surf " << machine->surferNum(nonmaxPathRelationSurfer);
      break;
    case MaxEqualiseTermId:
      str << "MaxEqualiseTermId"
	  << " surf " << machine->surferNum(maxPathRelationSurfer);
      break;
    case LoadSubst:
      str << "LoadSubst"
	  << " surf " << machine->surferNum(nonmaxPathCurrNode)
	  << " arg " << argNum
	  << " var " << (subst - substitution);
      break;
    case Compare: 
      str << "Compare"
	  << " surf " << machine->surferNum(nonmaxPathCurrNode)
	  << " arg " << argNum
	  << " var " << (subst - substitution);
      break;
    case Success:
      str << "Success";
      break;
    case ArgsInit:
      str << "ArgsInit";
      break;   
    case ArgsFetch:
      str << "ArgsFetch"
	  << " argNum(" << argNum << ")";
      break;  
    case ArgsMove:
      str << "ArgsMove"
	  << " argNum(" << argNum << ")";
      break;  
    case ArgsLoadSubst:
      str << "ArgsLoadSubst"
	  << " argNum(" << argNum << ")"
	  << " var(" << (this->subst - substitution) << ")";
      break;   
    case ArgsCompare:
      str << "ArgsCompare"
	  << " argNum(" << argNum << ")"
	  << " var(" << (this->subst - substitution) << ")";
      break; 
    case ArgsInitAndStore:
      str << "ArgsInitAndStore";
      break;   
    case ArgsFetchAndStore:
      str << "ArgsFetchAndStore";
      break;   
    case ArgsMoveAndStore:
      str << "ArgsMoveAndStore"
	  << " argNum(" << argNum << ")";
      break;   

    default: str << "UnknownInstructionType"; 
    }; 
  return str;
} //  PathIndexingWithJoinsAndEarlyAndCompiledCleanUpForInstanceRetrieval<..>::AbstractMachine::Instruction::output(ostream& str,const AbstractMachine* machine,const TermId* substitution) const  


  template <class Alloc,class Term,ulong MaxTermDepth,ulong MaxTermSize,ulong MaxNumOfVariables>  
inline ulong PathIndexingWithJoinsAndEarlyAndCompiledCleanUpForInstanceRetrieval<Alloc,Term,MaxTermDepth,MaxTermSize,MaxNumOfVariables>::AbstractMachine::surferNum(const NonmaxPathRelation::Node* const * currNdAddr) const
{
  for (ulong res = 0UL; res < MaxTermSize; res++)
    if (nonmaxPathRelationSurfers[res].currNodeAddr() == currNdAddr) return res;
  BK::RuntimeError::report("Surfer not found in  PathIndexingWithJoinsAndEarlyAndCompiledCleanUpForInstanceRetrieval<..>::AbstractMachine::surferNum(const NonmaxPathRelation::Node* const * currNdAddr) const");
  return 0UL; // dummy
} // ulong PathIndexingWithJoinsAndEarlyAndCompiledCleanUpForInstanceRetrieval<..>::AbstractMachine::surferNum(const NonmaxPathRelation::Node* const * currNdAddr) const

template <class Alloc,class Term,ulong MaxTermDepth,ulong MaxTermSize,ulong MaxNumOfVariables>  
inline ulong PathIndexingWithJoinsAndEarlyAndCompiledCleanUpForInstanceRetrieval<Alloc,Term,MaxTermDepth,MaxTermSize,MaxNumOfVariables>::AbstractMachine::surferNum(const NonmaxPathRelation::SurferWithMemory* surf) const
{
  return surf - nonmaxPathRelationSurfers;
} // ulong PathIndexingWithJoinsAndEarlyAndCompiledCleanUpForInstanceRetrieval<..>::AbstractMachine::surferNum(const NonmaxPathRelation::SurferWithMemory* surf) const

template <class Alloc,class Term,ulong MaxTermDepth,ulong MaxTermSize,ulong MaxNumOfVariables>  
inline ulong PathIndexingWithJoinsAndEarlyAndCompiledCleanUpForInstanceRetrieval<Alloc,Term,MaxTermDepth,MaxTermSize,MaxNumOfVariables>::AbstractMachine::surferNum(const MaxPathRelation::SurferWithMemory* surf) const
{
  return surf - maxPathRelationSurfers; 
} // ulong PathIndexingWithJoinsAndEarlyAndCompiledCleanUpForInstanceRetrieval<..>::AbstractMachine::surferNum(const MaxPathRelation::SurferWithMemory* surf) const

#endif


}; // namespace Gem


//======================================================================
#undef DEBUG_NAMESPACE
//======================================================================
#endif
