//
// File:         StandardPathIndexingForInstanceRetrieval.hpp
// Description:  Indexing for instance retrieval
//               based on standard path-indexing + immediate post-check. 
// Created:      Feb 01, 2002
// Author:       Alexandre Riazanov
// mail:         riazanov@cs.man.ac.uk
// Note:         This file is a part of the Gematogen library.
//============================================================================
#ifndef STANDARD_PATH_INDEXING_FOR_INSTANCE_RETRIEVAL_H
//=============================================================================      

       // Uncomment any of the definitions to switch off
       // the corresponding optimisations

//#define INSTANCE_RETRIEVAL_INDEX_NO_SKIP_LISTS
//#define INSTANCE_RETRIEVAL_INDEX_NO_PATH_RELATION_SORTING


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
#define STANDARD_PATH_INDEXING_FOR_INSTANCE_RETRIEVAL_H 
#ifndef NO_DEBUG_VIS
 #include <iostream>
#endif 
#include "jargon.hpp"
#include "Gematogen.hpp" 
#include "GematogenDebugFlags.hpp" 
#include "RuntimeError.hpp"
#include "Stack.hpp"
#include "PathIndex.hpp"
#ifdef INSTANCE_RETRIEVAL_INDEX_NO_SKIP_LISTS
 #include "OrdList.hpp"
#else 
 #include "CompactNodeSmallHeaderSkipList.hpp"
#endif
#include "GIncCountingSort.hpp"
//============================================================================
#undef DEBUG_NAMESPACE
#ifdef DEBUG_STANDARD_PATH_INDEXING_FOR_INSTANCE_RETRIEVAL
 #define DEBUG_NAMESPACE "StandardPathIndexingForInstanceRetrieval<Alloc,Term,MaxTermDepth,MaxTermSize,MaxNumOfVariables>"
#endif
#include "debugMacros.hpp"
//============================================================================ 

namespace Gem
{
template <class Alloc,class Term,ulong MaxTermDepth,ulong MaxTermSize,ulong MaxNumOfVariables>
class StandardPathIndexingForInstanceRetrieval
{ 
 public:
  typedef typename Term::Symbol Symbol;
  typedef typename Term::Id TermId;
  typedef typename Term::Traversal TermTraversal;
  typedef typename Term::Arguments TermArguments;
  #ifdef INSTANCE_RETRIEVAL_INDEX_USE_SIGNATURE_FILTERS
   typedef typename Term::SignatureFilter SignatureFilter;
  #endif
  // These are made public because of the need
  // to initialise some static data members.
   class StandardPathIndexingForInstanceRetrievalNonmaxPathRelationId {};
   class StandardPathIndexingForInstanceRetrievalMaxPathRelationId {};
  #ifndef INSTANCE_RETRIEVAL_INDEX_NO_SKIP_LISTS
   typedef BK::CNSHVoidSkList<Alloc,TermId,2,StandardPathIndexingForInstanceRetrievalNonmaxPathRelationId> NonmaxPathRelation; 
   typedef BK::CNSHVoidSkList<Alloc,TermId,2,StandardPathIndexingForInstanceRetrievalMaxPathRelationId> MaxPathRelation; 
   typedef NonmaxPathRelation::SurferWithMemory NonmaxPathRelationSurfer;
   typedef MaxPathRelation::SurferWithMemory MaxPathRelationSurfer;
  #else
   typedef BK::VoidOrdList<Alloc,TermId,StandardPathIndexingForInstanceRetrievalNonmaxPathRelationId> NonmaxPathRelation; 
   typedef BK::VoidOrdList<Alloc,TermId,StandardPathIndexingForInstanceRetrievalMaxPathRelationId> MaxPathRelation; 
   typedef NonmaxPathRelation::Surfer NonmaxPathRelationSurfer;
   typedef MaxPathRelation::Surfer MaxPathRelationSurfer;
  #endif

  typedef PathIndex<Alloc,Symbol,NonmaxPathRelation,MaxPathRelation,MaxTermDepth,MaxTermSize> PathIndexForInstanceRetrieval;
 public:
  StandardPathIndexingForInstanceRetrieval() 
   : _trie(),

     _integrator(),
     _integratedTermTraversal(),
 
     _removal(),
     _removedTermTraversal(),

     //_destruction(),

    _queryTraversal(),
    _pathRelationRetrieval(),
    _intermediateQueryRepresentation(&_abstractMachine),

    _abstractMachine()
  {
  };
  ~StandardPathIndexingForInstanceRetrieval() 
    {
      CALL("destructor ~StandardPathIndexingForInstanceRetrieval()");
    };
  bool integrate(const TermId& integratedTermId);
  bool remove(const TermId& removedTermId);  
  bool compileQuery(const TermId& query);
  bool nextMatch(TermId& matchId);
  const TermId& subst(ulong var) const { return _abstractMachine.substitution[var]; };
  static const char* about() 
  { 
   return GEMATOGEN_VERSION 
          ", inst. ret. based on standard path-ind. and immediate post-check " 
          #ifdef INSTANCE_RETRIEVAL_INDEX_USE_WEIGHT_FILTERS
           " + weight filters " 
          #endif 
          #ifdef INSTANCE_RETRIEVAL_INDEX_USE_SIGNATURE_FILTERS  
           " + sig. filters " 
          #endif
          #ifdef INSTANCE_RETRIEVAL_INDEX_NO_SKIP_LISTS
           " - skip-lists (ordinary sorted lists are used instead)" 
          #endif
          #ifdef INSTANCE_RETRIEVAL_INDEX_NO_PATH_RELATION_SORTING
           " - sorting of path-relations" 
          #endif
          ;
  };
 private: 
  class AbstractMachine
  {
   public: 
    enum InstructionTag
    {
      // Basic atomary actions
     NonmaxEqualiseTermId,
     MaxEqualiseTermId,
     Match,
     ExtractSubstitution,
     Success
    };
    class Instruction
    {
     public:
      Instruction() {}; 
      ~Instruction() {}; 
      #ifndef NO_DEBUG_VIS
       ostream& output(ostream& str,const Instruction* base,const TermId* substitution) const;
      #endif
     public:
      InstructionTag tag;
      union
      {
       NonmaxPathRelationSurfer* nonmaxPathRelationSurfer; 
       MaxPathRelationSurfer* maxPathRelationSurfer;
      };  
      const NonmaxPathRelation::Node* const * nonmaxPathCurrNode; // to get the term arguments
      ulong argNum;
      TermId* subst;
    }; // class Instruction
   public: 
    AbstractMachine() 
     : queryTraversal(), 
       candidateTraversal(),
       allInstantiatedVars()
     {
       for (ulong i = 0; i < MaxNumOfVariables; i++)
	varInstantiated[i] = false;
     };
    ~AbstractMachine() {}; 
    bool nextMatch(TermId& matchId);
    bool match(const TermId& candidate);
    void extractSubstitution(const TermId& candidate);
    #ifndef NO_DEBUG_VIS
     ostream& outputCode(ostream& str) const; 
    #endif
   public:
     // Code
    Instruction code[MaxTermSize + 1];
    // Registers
    TermId substitution[MaxTermSize];
    NonmaxPathRelationSurfer nonmaxPathRelationSurfers[MaxTermSize];
    MaxPathRelationSurfer maxPathRelationSurfers[MaxTermSize];
    bool firstRetrievalCall;
    TermId query;
    #ifdef INSTANCE_RETRIEVAL_INDEX_USE_WEIGHT_FILTERS
     ulong queryWeight;
    #endif
    #ifdef INSTANCE_RETRIEVAL_INDEX_USE_SIGNATURE_FILTERS
     SignatureFilter querySignatureFilter;
    #endif
    TermTraversal queryTraversal;
    TermTraversal candidateTraversal;
    bool varInstantiated[MaxNumOfVariables];  
    BK::Stack<ulong,MaxTermSize> allInstantiatedVars; 
  }; //  class AbstractMachine 

  class IntermediateQueryRepresentation
  {
   public:
    IntermediateQueryRepresentation(AbstractMachine* abstractMachine) 
     : _backtrackHoles(),
       _backtrackNonmaxPositions(),
       #ifndef INSTANCE_RETRIEVAL_INDEX_NO_PATH_RELATION_SORTING
        _primaryNestSorting(),
       #endif
       _abstractMachine(abstractMachine),
       _allVariablesOccured()
    {
     _positions[0].tag = Start;
     _positions[0].arity = 1UL;
     for (ulong v = 0; v < MaxNumOfVariables; v++)
       _varOccured[v] = false;
    };
    ~IntermediateQueryRepresentation() {};
    void reset();
    void compound(const TermId& subterm,ulong arity,const NonmaxPathRelation* pathRelation);
    void constant(const TermId& subterm,const MaxPathRelation* pathRelation);
    void variable(ulong var);
    void endOfQuery();
    void generateCode();    

    #ifndef NO_DEBUG_VIS
     ostream& output(ostream& str) const;
    #endif

   private:
    enum PositionTag { Start,Compound,Constant,/* Variable, */ End }; 
    class Position
    {
     public:
      Position() {};
      ~Position() {};
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
      //ulong variable;
      ulong numOfVarArgs;
      bool hasNonvarArg;
      #ifndef INSTANCE_RETRIEVAL_INDEX_NO_PATH_RELATION_SORTING
        const ulong* numInSorting;
      #endif
      AbstractMachine::Instruction* mainInstruction;
    }; // class Position 
   private: 
    Position _positions[MaxTermSize + 2];
    Position* _nextPosition;
    ulong _currentHoles;
    Position* _currentNest;
    BK::Stack<ulong,MaxTermDepth> _backtrackHoles;
    BK::Stack<Position*,MaxTermDepth> _backtrackNonmaxPositions;
    #ifndef INSTANCE_RETRIEVAL_INDEX_NO_PATH_RELATION_SORTING
     BK::GSimpleIncCountingSort<ulong,MaxTermSize> _primaryNestSorting; 
    #else
     ulong _numOfPrimaryNests;
    #endif
    AbstractMachine* _abstractMachine;
    //bool _substLoaded[MaxNumOfVariables];
    bool _varOccured[MaxNumOfVariables];
    BK::Stack<ulong,MaxTermSize> _allVariablesOccured;
    bool _queryIsGround;
    bool _queryIsLinear;
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
  //PathIndexForInstanceRetrieval::Destruction _destruction;

    // Query compilation
  
  TermTraversal _queryTraversal;
  PathIndexForInstanceRetrieval::Retrieval _pathRelationRetrieval;
  IntermediateQueryRepresentation _intermediateQueryRepresentation; 

    // Abstract machine
  
  AbstractMachine _abstractMachine;
}; // class StandardPathIndexingForInstanceRetrieval<..>
}; // namespace Gem

//============================================================================
#undef DEBUG_NAMESPACE
#ifdef DEBUG_STANDARD_PATH_INDEXING_FOR_INSTANCE_RETRIEVAL
 #define DEBUG_NAMESPACE "StandardPathIndexingForInstanceRetrieval<Alloc,Term,MaxTermDepth,MaxTermSize,MaxNumOfVariables>"
#endif
#include "debugMacros.hpp"
//============================================================================

namespace Gem
{
template <class Alloc,class Term,ulong MaxTermDepth,ulong MaxTermSize,ulong MaxNumOfVariables>
inline bool StandardPathIndexingForInstanceRetrieval<Alloc,Term,MaxTermDepth,MaxTermSize,MaxNumOfVariables>::integrate(const TermId& integratedTermId)
{
 CALL("integrate(const TermId& integratedTermId)");
 _integratedTermTraversal.reset(integratedTermId);
 _integrator.reset(_trie);
 ulong arity;
 check_state:
  switch (_integratedTermTraversal.state())
   { 
    case TermTraversal::Func:
     arity = _integratedTermTraversal.symbol().arity();
     if (arity)
      {
       _integrator.nonconstFunc(_integratedTermTraversal.symbol());
       if (!_integrator.indexedObj().insert(integratedTermId)) return false;      
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
       BK::RuntimeError::report("Wrong term traversal state in StandardPathIndexingForInstanceRetrieval<..>::integrate(..)");
       return false;
     #endif
    #endif       
   };
} // bool StandardPathIndexingForInstanceRetrieval<..>::integrate(const TermId& integratedTermId)



template <class Alloc,class Term,ulong MaxTermDepth,ulong MaxTermSize,ulong MaxNumOfVariables>
inline bool StandardPathIndexingForInstanceRetrieval<Alloc,Term,MaxTermDepth,MaxTermSize,MaxNumOfVariables>::remove(const TermId& removedTermId)
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
       BK::RuntimeError::report("Wrong term traversal state in StandardPathIndexingForInstanceRetrieval<..>::remove(..)");
       return false;
     #endif
    #endif       
   };

} // bool StandardPathIndexingForInstanceRetrieval<..>::remove(const TermId& removedTermId)



template <class Alloc,class Term,ulong MaxTermDepth,ulong MaxTermSize,ulong MaxNumOfVariables>
inline bool StandardPathIndexingForInstanceRetrieval<Alloc,Term,MaxTermDepth,MaxTermSize,MaxNumOfVariables>::compileQuery(const TermId& query)
{
 CALL("compileQuery(const TermId& query)");
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
     _intermediateQueryRepresentation.variable(_queryTraversal.symbol().var());
     _pathRelationRetrieval.after();
     _queryTraversal.next(); 
     goto check_state;

    case TermTraversal::End: 
     _intermediateQueryRepresentation.endOfQuery();
     _intermediateQueryRepresentation.generateCode();
     _abstractMachine.query = query;
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
       BK::RuntimeError::report("Wrong term traversal state in StandardPathIndexingForInstanceRetrieval<..>::compileQuery(..)");
       return false;
     #endif
    #endif    
  };
 
} // bool StandardPathIndexingForInstanceRetrieval<..>::compileQuery(const TermId& query)

template <class Alloc,class Term,ulong MaxTermDepth,ulong MaxTermSize,ulong MaxNumOfVariables>
inline bool StandardPathIndexingForInstanceRetrieval<Alloc,Term,MaxTermDepth,MaxTermSize,MaxNumOfVariables>::nextMatch(TermId& matchId)
{
 CALL("nextMatch(TermId& matchId)");
 return _abstractMachine.nextMatch(matchId);
}

}; // namespace Gem

//============================================================================
#undef DEBUG_NAMESPACE
#ifdef DEBUG_STANDARD_PATH_INDEXING_FOR_INSTANCE_RETRIEVAL
 #define DEBUG_NAMESPACE "StandardPathIndexingForInstanceRetrieval<Alloc,Term,MaxTermDepth,MaxTermSize,MaxNumOfVariables>::IntermediateQueryRepresentation"
#endif
#include "debugMacros.hpp"
//============================================================================

namespace Gem
{

template <class Alloc,class Term,ulong MaxTermDepth,ulong MaxTermSize,ulong MaxNumOfVariables>
inline void StandardPathIndexingForInstanceRetrieval<Alloc,Term,MaxTermDepth,MaxTermSize,MaxNumOfVariables>::IntermediateQueryRepresentation::reset()
{
 CALL("reset()");
 _nextPosition = _positions + 1;
 _currentHoles = 1UL;
 _currentNest = _positions;
 _backtrackHoles.reset();
 _backtrackNonmaxPositions.reset();

 #ifndef INSTANCE_RETRIEVAL_INDEX_NO_PATH_RELATION_SORTING
  _primaryNestSorting.reset();
 #else
  _numOfPrimaryNests = 0UL;
 #endif 

 _queryIsGround = true;
 _queryIsLinear = false;
 while (_allVariablesOccured.nonempty())
  _varOccured[_allVariablesOccured.pop()] = false;
} // void StandardPathIndexingForInstanceRetrieval<..>::IntermediateQueryRepresentation::reset()

template <class Alloc,class Term,ulong MaxTermDepth,ulong MaxTermSize,ulong MaxNumOfVariables>   
inline void StandardPathIndexingForInstanceRetrieval<Alloc,Term,MaxTermDepth,MaxTermSize,MaxNumOfVariables>::IntermediateQueryRepresentation::compound(const TermId& subterm,ulong arity,const NonmaxPathRelation* pathRelation)
{
 CALL("compound(const TermId& subterm,ulong arity,const NonmaxPathRelation* pathRelation)");
 _nextPosition->tag = Compound;
 _nextPosition->nonmaxPathRelation = pathRelation;
 _nextPosition->nestPosition = _currentNest;
 _nextPosition->arity = arity;
 _nextPosition->numOfVarArgs = 0UL;
 _nextPosition->hasNonvarArg = false;

 _currentNest->hasNonvarArg = true;

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

} // void StandardPathIndexingForInstanceRetrieval<..>::IntermediateQueryRepresentation::compound(const TermId& subterm,ulong arity,const NonmaxPathRelation* pathRelation)

template <class Alloc,class Term,ulong MaxTermDepth,ulong MaxTermSize,ulong MaxNumOfVariables>    
inline void StandardPathIndexingForInstanceRetrieval<Alloc,Term,MaxTermDepth,MaxTermSize,MaxNumOfVariables>::IntermediateQueryRepresentation::constant(const TermId& subterm,const MaxPathRelation* pathRelation)
{
 CALL("constant(const TermId& subterm,const MaxPathRelation* pathRelation)");
 _nextPosition->tag = Constant;
 _nextPosition->maxPathRelation = pathRelation;
 _nextPosition->nestPosition = _currentNest; 

 _currentNest->hasNonvarArg = true;

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
 #ifndef INSTANCE_RETRIEVAL_INDEX_NO_PATH_RELATION_SORTING
  _nextPosition->numInSorting = _primaryNestSorting.num() + _primaryNestSorting.size();
  _primaryNestSorting.push(_nextPosition->maxPathRelation->size());
 #else
  _numOfPrimaryNests++;
 #endif

 _nextPosition++;
} // void StandardPathIndexingForInstanceRetrieval<..>::IntermediateQueryRepresentation::constant(const TermId& subterm,const MaxPathRelation* pathRelation)

template <class Alloc,class Term,ulong MaxTermDepth,ulong MaxTermSize,ulong MaxNumOfVariables>    
inline void StandardPathIndexingForInstanceRetrieval<Alloc,Term,MaxTermDepth,MaxTermSize,MaxNumOfVariables>::IntermediateQueryRepresentation::variable(ulong var)
{
 CALL("variable(ulong var)");
 _queryIsGround = false;
 if (_queryIsLinear)
  {
   if (_varOccured[var])
    {
     _queryIsLinear = false;
    }
   else
    {
     _varOccured[var] = true;
     _allVariablesOccured.push(var);
    };
  };
 //_substLoaded[var] = false;
 //_nextPosition->tag = Variable;
 //_nextPosition->nestPosition = _currentNest;
 //_nextPosition->variable = var;

 _currentNest->numOfVarArgs++;

 ASSERT(_currentHoles);
 //_nextPosition->argNum = _currentNest->arity - _currentHoles; 
 _currentHoles--;
 if (!_currentHoles)
  {
    if (!_currentNest->hasNonvarArg) // _currentNest is a primary nest
     {
      #ifndef INSTANCE_RETRIEVAL_INDEX_NO_PATH_RELATION_SORTING
       _currentNest->numInSorting = _primaryNestSorting.num() + _primaryNestSorting.size();
       _primaryNestSorting.push(_currentNest->nonmaxPathRelation->size());
      #else
       _numOfPrimaryNests++;
      #endif
     };
 
   if (_backtrackHoles)
    {
     _currentHoles = _backtrackHoles.pop();
     ASSERT(_currentHoles);
     _currentNest = _backtrackNonmaxPositions.pop();
    };
  };

 //_nextPosition++;

} // void StandardPathIndexingForInstanceRetrieval<..>::IntermediateQueryRepresentation::variable(ulong var)

template <class Alloc,class Term,ulong MaxTermDepth,ulong MaxTermSize,ulong MaxNumOfVariables>    
inline void StandardPathIndexingForInstanceRetrieval<Alloc,Term,MaxTermDepth,MaxTermSize,MaxNumOfVariables>::IntermediateQueryRepresentation::endOfQuery()
{
 CALL("endOfQuery()");
 ASSERT(!_currentHoles);
 ASSERT(!_backtrackHoles);
 _nextPosition->tag = End;

 //DF; output(cout << "\n\n") << "\n";
//DF; _primaryNestSorting.output(cout << "\n") << "\n";

} // void StandardPathIndexingForInstanceRetrieval<..>::IntermediateQueryRepresentation::endOfQuery()


template <class Alloc,class Term,ulong MaxTermDepth,ulong MaxTermSize,ulong MaxNumOfVariables>    
inline void StandardPathIndexingForInstanceRetrieval<Alloc,Term,MaxTermDepth,MaxTermSize,MaxNumOfVariables>::IntermediateQueryRepresentation::generateCode()
{
 CALL("generateCode()");
 #ifndef INSTANCE_RETRIEVAL_INDEX_NO_PATH_RELATION_SORTING 
  ulong primaryCodeSize = _primaryNestSorting.size();
 #else
  ulong primaryCodeSize = _numOfPrimaryNests;
  ulong currentPrimNestNum = 0UL; 
 #endif
 AbstractMachine::Instruction* nextSecondaryInstr = _abstractMachine->code + primaryCodeSize;
 AbstractMachine::Instruction* primaryInstr;
 Position* pos;
 NonmaxPathRelationSurfer* nextNonmaxPathRelationSurfer; 
 MaxPathRelationSurfer* nextMaxPathRelationSurfer;

  


 for (pos = _positions + 1; pos->tag != End; pos++)
  { 
   switch (pos->tag)
    { 
     case Compound:
      if (pos->numOfVarArgs)
       {
	if (pos->hasNonvarArg) // secondary  
	 {
	  pos->mainInstruction = (AbstractMachine::Instruction*)0;
         }
        else // primary 
	 {    
          nextNonmaxPathRelationSurfer = _abstractMachine->nonmaxPathRelationSurfers + (pos - _positions);
          nextNonmaxPathRelationSurfer->reset(*(pos->nonmaxPathRelation)); 
           
          #ifndef INSTANCE_RETRIEVAL_INDEX_NO_PATH_RELATION_SORTING
	   primaryInstr = _abstractMachine->code + (*(pos->numInSorting));
          #else
           primaryInstr = _abstractMachine->code + currentPrimNestNum;
           currentPrimNestNum++;
          #endif

          pos->mainInstruction = primaryInstr;
          primaryInstr->tag = AbstractMachine::NonmaxEqualiseTermId;
          primaryInstr->nonmaxPathRelationSurfer = nextNonmaxPathRelationSurfer;
         };
       }; 
      break;

     case Constant: 
      #ifndef INSTANCE_RETRIEVAL_INDEX_NO_PATH_RELATION_SORTING
       primaryInstr = _abstractMachine->code + (*(pos->numInSorting));
      #else
       primaryInstr = _abstractMachine->code + currentPrimNestNum;
       currentPrimNestNum++;
      #endif

      pos->mainInstruction = primaryInstr; 
      primaryInstr->tag = AbstractMachine::MaxEqualiseTermId;
      nextMaxPathRelationSurfer = _abstractMachine->maxPathRelationSurfers + (pos - _positions);
      nextMaxPathRelationSurfer->reset(*(pos->maxPathRelation));
      primaryInstr->maxPathRelationSurfer = nextMaxPathRelationSurfer;
      break;

      /*
     case Variable:  
      // nothing to do here
      break;
      */

     #ifdef DEBUG_NAMESPACE 
      default: ICP("ICP0"); return;
     #else
      #ifdef _SUPPRESS_WARNINGS_
       default: 
        BK::RuntimeError::report("Wrong tag in StandardPathIndexingForInstanceRetrieval<..>::IntermediateQueryRepresentation::generateCode()"); 
        return;
      #endif   
     #endif
    };
  };
 
 ASSERT(pos->tag == End);
 if (!_queryIsGround)
  {
   if (_queryIsLinear)
    {
     nextSecondaryInstr->tag = AbstractMachine::ExtractSubstitution;
    }
   else
    {
     nextSecondaryInstr->tag = AbstractMachine::Match;
    };
   ++nextSecondaryInstr;
  };
 nextSecondaryInstr->tag = AbstractMachine::Success;
 _abstractMachine->firstRetrievalCall = true;

 //DF; _abstractMachine->outputCode(cout << "\n\n") << "\n";

} // void StandardPathIndexingForInstanceRetrieval<..>::IntermediateQueryRepresentation::generateCode()


#ifndef NO_DEBUG_VIS
 template <class Alloc,class Term,ulong MaxTermDepth,ulong MaxTermSize,ulong MaxNumOfVariables>    
 inline ostream& StandardPathIndexingForInstanceRetrieval<Alloc,Term,MaxTermDepth,MaxTermSize,MaxNumOfVariables>::IntermediateQueryRepresentation::output(ostream& str) const
 {
  for (const Position* p = _positions; p < _nextPosition; p++)
   p->output(str,_positions) << "\n";
  return str;
 } // ostream& StandardPathIndexingForInstanceRetrieval<..>::IntermediateQueryRepresentation::output(ostream& str) const
#endif


}; // namespace Gem


//============================================================================
#undef DEBUG_NAMESPACE
#ifdef DEBUG_STANDARD_PATH_INDEXING_FOR_INSTANCE_RETRIEVAL
 #define DEBUG_NAMESPACE "StandardPathIndexingForInstanceRetrieval<Alloc,Term,MaxTermDepth,MaxTermSize,MaxNumOfVariables>::IntermediateQueryRepresentation::Position"
#endif
#include "debugMacros.hpp"
//============================================================================

namespace Gem
{

#ifndef NO_DEBUG_VIS
 template <class Alloc,class Term,ulong MaxTermDepth,ulong MaxTermSize,ulong MaxNumOfVariables>    
 inline ostream& StandardPathIndexingForInstanceRetrieval<Alloc,Term,MaxTermDepth,MaxTermSize,MaxNumOfVariables>::IntermediateQueryRepresentation::Position::output(ostream& str,const Position* base) const
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
         << " numOfVarArgs(" << numOfVarArgs << ")";  
     if (hasNonvarArg) str << " hasNonvarArg"; 
     break;   
    case Constant: 
     str << "Constant" 
         << " arg(" << argNum << ")"
         << " nest(" << (nestPosition - base) << ")"
         << " pr(" << (ulong)maxPathRelation << ")"; 
     break;  

     /*
    case Variable: 
     str << "Variable" 
         << " arg(" << argNum << ")"
         << " nest(" << (nestPosition - base) << ")"
         << " #" << variable;   
     break;
     */


    case End: str << "End"; break;
    default: str << "Unknown position tag"; break;
   };
  return str;
 } // ostream& StandardPathIndexingForInstanceRetrieval<..>::IntermediateQueryRepresentation::Position::output(ostream& str,const Position* base) const
#endif

}; // namespace Gem


//============================================================================
#undef DEBUG_NAMESPACE
#ifdef DEBUG_STANDARD_PATH_INDEXING_FOR_INSTANCE_RETRIEVAL
 #define DEBUG_NAMESPACE "StandardPathIndexingForInstanceRetrieval<Alloc,Term,MaxTermDepth,MaxTermSize,MaxNumOfVariables>::AbstractMachine"
#endif
#include "debugMacros.hpp"
//============================================================================

namespace Gem
{
template <class Alloc,class Term,ulong MaxTermDepth,ulong MaxTermSize,ulong MaxNumOfVariables>  
inline bool StandardPathIndexingForInstanceRetrieval<Alloc,Term,MaxTermDepth,MaxTermSize,MaxNumOfVariables>::AbstractMachine::nextMatch(TermId& matchId)
{
 CALL("nextMatch(TermId& matchId)");
 Instruction* instr = code;
 TermId termId;

 // Initialise termId

  //DF; instr->output(cout << "IN ",code,substitution) << "\n"; 

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
      BK::RuntimeError::report("Wrong tag in StandardPathIndexingForInstanceRetrieval<..>::AbstractMachine::nextMatch(TermId& matchId)"); 
      return false;
    #endif   
   #endif
  };
 
 next_instr:
  instr++;
  perform_instr:

  //DF; instr->output(cout << "FW ",code,substitution) << "\n"; 

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

     case Match: 
      if (!match(termId)) goto backtrack;
      goto next_instr;

     case ExtractSubstitution: 
      extractSubstitution(termId);
      goto next_instr;

     case Success:
      matchId = termId;     
      
      //DF; cout << "Success\n";

      return true;

     #ifdef DEBUG_NAMESPACE 
      default: ICP("ICP1"); return false; 
     #else
      #ifdef _SUPPRESS_WARNINGS_
       default: 
        BK::RuntimeError::report("Wrong tag in StandardPathIndexingForInstanceRetrieval<..>::AbstractMachine::nextMatch(TermId& matchId)"); 
        return false;
      #endif   
     #endif
    };

 ICP("ICP2");
 backtrack:
  instr = code;

  //DF; instr->output(cout << "BK ",code,substitution) << "\n"; 

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
       BK::RuntimeError::report("Wrong tag in StandardPathIndexingForInstanceRetrieval<..>::AbstractMachine::nextMatch(TermId& matchId)"); 
       return false;
     #endif   
    #endif
   };

} // bool StandardPathIndexingForInstanceRetrieval<..>::AbstractMachine::nextMatch(TermId& matchId)


template <class Alloc,class Term,ulong MaxTermDepth,ulong MaxTermSize,ulong MaxNumOfVariables>  
inline bool StandardPathIndexingForInstanceRetrieval<Alloc,Term,MaxTermDepth,MaxTermSize,MaxNumOfVariables>::AbstractMachine::match(const TermId& candidate)
{
 CALL("match(const TermId& candidate)");
 while (allInstantiatedVars.nonempty()) 
  {
   varInstantiated[allInstantiatedVars.pop()] = false;
  };
 queryTraversal.reset(query);
 candidateTraversal.reset(candidate);
 ulong varNum;
 check_state: 
  switch (queryTraversal.state())
   {
    case TermTraversal::Func:
     queryTraversal.next(); 
     candidateTraversal.next();
     goto check_state;

    case TermTraversal::Var:
     varNum = queryTraversal.symbol().var();
     if (varInstantiated[varNum])
       {
	if (candidateTraversal.term() != substitution[varNum])
	  return false;
       }
     else
       {
	allInstantiatedVars.push(varNum);
        varInstantiated[varNum] = true;
        substitution[varNum] = candidateTraversal.term();
       };
     queryTraversal.next(); 
     candidateTraversal.after();
     goto check_state;

    case TermTraversal::End:
     return true;

    #ifdef DEBUG_NAMESPACE 
     default: ICP("ICP0"); return false;
    #else
     #ifdef _SUPPRESS_WARNINGS_
      default: 
       BK::RuntimeError::report("Wrong term traversal state in StandardPathIndexingForInstanceRetrieval<..>::AbstractMachine::match(..)");
       return false;
     #endif
    #endif    
  };
}; // bool StandardPathIndexingForInstanceRetrieval<..>::AbstractMachine::match(const TermId& candidate)


template <class Alloc,class Term,ulong MaxTermDepth,ulong MaxTermSize,ulong MaxNumOfVariables>  
inline void StandardPathIndexingForInstanceRetrieval<Alloc,Term,MaxTermDepth,MaxTermSize,MaxNumOfVariables>::AbstractMachine::extractSubstitution(const TermId& candidate)
{
 CALL("extractSubstitution(const TermId& candidate)");
 queryTraversal.reset(query);
 candidateTraversal.reset(candidate);
 check_state: 
  switch (queryTraversal.state())
   {
    case TermTraversal::Func:
     queryTraversal.next(); 
     candidateTraversal.next();
     goto check_state;

    case TermTraversal::Var:
     substitution[queryTraversal.symbol().var()] = candidateTraversal.term();
     queryTraversal.next(); 
     candidateTraversal.after();
     goto check_state;

    case TermTraversal::End:
     return;

    #ifdef DEBUG_NAMESPACE 
     default: ICP("ICP0"); 
    #else
     #ifdef _SUPPRESS_WARNINGS_
      default: 
       BK::RuntimeError::report("Wrong term traversal state in StandardPathIndexingForInstanceRetrieval<..>::AbstractMachine::extractSubstitution(..)");
     #endif
    #endif    
  };
}; // void StandardPathIndexingForInstanceRetrieval<..>::AbstractMachine::extractSubstitution(const TermId& candidate)


#ifndef NO_DEBUG_VIS
       
 template <class Alloc,class Term,ulong MaxTermDepth,ulong MaxTermSize,ulong MaxNumOfVariables>  
 inline ostream& StandardPathIndexingForInstanceRetrieval<Alloc,Term,MaxTermDepth,MaxTermSize,MaxNumOfVariables>::AbstractMachine::outputCode(ostream& str) const
 {
  CALL("outputCode(ostream& str) const");
  const Instruction* instr = code;
  while (instr->tag != Success) 
   {
    instr->output(str,code,substitution) << "\n";
    instr++;
   }; 
  instr->output(str,code,substitution) << "\n"; 
  return str;
 } //  StandardPathIndexingForInstanceRetrieval<..>::AbstractMachine::outputCode(ostream& str) const  

#endif

}; // namespace Gem




//============================================================================
#undef DEBUG_NAMESPACE
#ifdef DEBUG_STANDARD_PATH_INDEXING_FOR_INSTANCE_RETRIEVAL
 #define DEBUG_NAMESPACE "StandardPathIndexingForInstanceRetrieval<Alloc,Term,MaxTermDepth,MaxTermSize,MaxNumOfVariables>::AbstractMachine::Instruction"
#endif
#include "debugMacros.hpp"
//============================================================================

namespace Gem
{

#ifndef NO_DEBUG_VIS
       
 template <class Alloc,class Term,ulong MaxTermDepth,ulong MaxTermSize,ulong MaxNumOfVariables>  
 inline ostream& StandardPathIndexingForInstanceRetrieval<Alloc,Term,MaxTermDepth,MaxTermSize,MaxNumOfVariables>::AbstractMachine::Instruction::output(ostream& str,const Instruction* base,const TermId* substitution) const
 {
  CALL("output(ostream& str,const Instruction* base,const TermId* substitution) const");
  str << (this - base) << " ";
  switch (tag) 
   {
    case NonmaxEqualiseTermId:
     str << "NonmaxEqualiseTermId";
     break;
    case MaxEqualiseTermId:
     str << "MaxEqualiseTermId";
     break;
    case Match:
     str << "Match";
     break;
    case ExtractSubstitution:
     str << "ExtractSubstitution";
     break;
    case Success:
     str << "Success";
     break;    
    default: str << "UnknownInstructionType"; 
   }; 
  return str;
 } //  StandardPathIndexingForInstanceRetrieval<..>::AbstractMachine::Instruction::output(ostream& str,const Instruction* base,const TermId* substitution) const  

#endif

}; // namespace Gem



//======================================================================
#undef DEBUG_NAMESPACE
//======================================================================
#endif
