//
// File:         PathIndexingWithJoinsForInstanceRetrieval.hpp
// Description:  Indexing for instance retrieval
//               based on path-indexing + database joins. 
// Created:      Sep 12, 2001
// Author:       Alexandre Riazanov
// mail:         riazanov@cs.man.ac.uk
// Note:         This file is a part of the Gematogen library.
//============================================================================
#ifndef PATH_INDEXING_WITH_JOINS_FOR_INSTANCE_RETRIEVAL_H
//=============================================================================      
       // Uncomment any of the definitions to switch off
       // the corresponding optimisations

#define INSTANCE_RETRIEVAL_INDEX_NO_LOAD_SUBST_FROM_TERM
        
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
#define PATH_INDEXING_WITH_JOINS_FOR_INSTANCE_RETRIEVAL_H 
#ifndef NO_DEBUG_VIS
 #include <iostream>
#endif 
#include "jargon.hpp"
#include "Gematogen.hpp" 
#include "GematogenDebugFlags.hpp" 
#include "RuntimeError.hpp"
#include "Stack.hpp"
#include "PathIndex.hpp"
#include "CompactNodeSmallHeaderSkipList.hpp"
#include "GIncCountingSort.hpp"
//============================================================================
#undef DEBUG_NAMESPACE
#ifdef DEBUG_PATH_INDEXING_WITH_JOINS_FOR_INSTANCE_RETRIEVAL
 #define DEBUG_NAMESPACE "PathIndexingWithJoinsForInstanceRetrieval<Alloc,Term,MaxTermDepth,MaxTermSize,MaxNumOfVariables>"
#endif
#include "debugMacros.hpp"
//============================================================================ 

namespace Gem
{
template <class Alloc,class Term,ulong MaxTermDepth,ulong MaxTermSize,ulong MaxNumOfVariables>
class PathIndexingWithJoinsForInstanceRetrieval
{ 
 public:
  typedef typename Term::Symbol Symbol;
  typedef typename Term::Id TermId;
  typedef typename Term::Traversal TermTraversal;
  typedef typename Term::Arguments TermArguments;
  #ifdef INSTANCE_RETRIEVAL_INDEX_USE_SIGNATURE_FILTERS
   typedef typename Term::SignatureFilter SignatureFilter;
  #endif
  // These are made public since they have static data members.
  typedef BK::CNSHSkList<Alloc,TermArguments,TermId,2,PathIndexingWithJoinsForInstanceRetrieval> NonmaxPathRelation;
  typedef BK::CNSHVoidSkList<Alloc,TermId,2,PathIndexingWithJoinsForInstanceRetrieval> MaxPathRelation; 
  typedef PathIndex<Alloc,Symbol,NonmaxPathRelation,MaxPathRelation,MaxTermDepth,MaxTermSize> PathIndexForInstanceRetrieval;
 public:
  PathIndexingWithJoinsForInstanceRetrieval() 
   : _intermediateQueryRepresentation(&_abstractMachine)
  {
  };

  ~PathIndexingWithJoinsForInstanceRetrieval() {}; 
  bool integrate(const TermId& integratedTermId);
  bool remove(const TermId& removedTermId); 
  bool compileQuery(const TermId& query);
  bool nextMatch(TermId& matchId);
  const TermId& subst(ulong var) const { return _abstractMachine.substitution[var]; };
  static const char* about() 
  { 
   return GEMATOGEN_VERSION 
          ", optimised inst. ret. based on path-ind. and DB joins " 
          #ifdef INSTANCE_RETRIEVAL_INDEX_USE_WEIGHT_FILTERS
           " + weight filters " 
          #endif 
          #ifdef INSTANCE_RETRIEVAL_INDEX_USE_SIGNATURE_FILTERS  
           " + sig. filters " 
          #endif
          #ifdef INSTANCE_RETRIEVAL_INDEX_NO_LOAD_SUBST_FROM_TERM
           " - loading subst. from candidates " 
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
     GetToTermId,
     LoadSubst,
     #ifndef INSTANCE_RETRIEVAL_INDEX_NO_LOAD_SUBST_FROM_TERM
      LoadSubstFromTerm, 
     #endif
     Compare,  
     #ifndef INSTANCE_RETRIEVAL_INDEX_NO_LOAD_SUBST_FROM_TERM
      CompareFromTerm,
     #endif
     Success,

     // Combined and specialised actions
     GetToTermIdAndLoadSubst,
     GetToTermIdAndCompare  
     #ifndef INSTANCE_RETRIEVAL_INDEX_NO_LOAD_SUBST_FROM_TERM
      , LoadArgumentsAndLoadSubstFromTerm,
      LoadArgumentsAndCompareFromTerm
     #endif   
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
       NonmaxPathRelation::SurferWithMemory* nonmaxPathRelationSurfer; 
       MaxPathRelation::SurferWithMemory* maxPathRelationSurfer;
      };  
      const NonmaxPathRelation::Node* const * nonmaxPathCurrNode; // to get the term arguments
      ulong argNum;
      TermId* subst;
    }; // class Instruction
   public: 
    AbstractMachine() {};
    ~AbstractMachine() {};
    bool nextMatch(TermId& matchId);
    #ifndef NO_DEBUG_VIS
     ostream& outputCode(ostream& str) const; 
    #endif
   public:
     // Code
    Instruction code[MaxTermSize + 1];
    // Registers
    TermId substitution[MaxTermSize];
    NonmaxPathRelation::SurferWithMemory nonmaxPathRelationSurfers[MaxTermSize];
    MaxPathRelation::SurferWithMemory maxPathRelationSurfers[MaxTermSize];
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
     ostream& output(ostream& str) const;
    #endif

   private:
    enum PositionTag { Start,Compound,Constant,Variable,End }; 
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
      ulong variable;
      ulong numOfVarArgs;
      bool hasNonvarArg;
      const ulong* numInSorting;
      AbstractMachine::Instruction* mainInstruction;
    }; // class Position 
   private: 
    Position _positions[MaxTermSize + 2];
    Position* _nextPosition;
    ulong _currentHoles;
    Position* _currentNest;
    BK::Stack<ulong,MaxTermDepth> _backtrackHoles;
    BK::Stack<Position*,MaxTermDepth> _backtrackNonmaxPositions;
    BK::GSimpleIncCountingSort<ulong,MaxTermSize> _primaryNestSorting; 
    AbstractMachine* _abstractMachine;
    bool _substLoaded[MaxNumOfVariables];
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
}; // class PathIndexingWithJoinsForInstanceRetrieval<..>
}; // namespace Gem

//============================================================================
#undef DEBUG_NAMESPACE
#ifdef DEBUG_PATH_INDEXING_WITH_JOINS_FOR_INSTANCE_RETRIEVAL
 #define DEBUG_NAMESPACE "PathIndexingWithJoinsForInstanceRetrieval<Alloc,Term,MaxTermDepth,MaxTermSize,MaxNumOfVariables>"
#endif
#include "debugMacros.hpp"
//============================================================================

namespace Gem
{

template <class Alloc,class Term,ulong MaxTermDepth,ulong MaxTermSize,ulong MaxNumOfVariables>
inline bool PathIndexingWithJoinsForInstanceRetrieval<Alloc,Term,MaxTermDepth,MaxTermSize,MaxNumOfVariables>::integrate(const TermId& integratedTermId)
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
       BK::RuntimeError::report("Wrong term traversal state in PathIndexingWithJoinsForInstanceRetrieval<..>::integrate(..)");
       return false;
     #endif
    #endif       
   };
} // bool PathIndexingWithJoinsForInstanceRetrieval<..>::integrate(const TermId& integratedTermId)



template <class Alloc,class Term,ulong MaxTermDepth,ulong MaxTermSize,ulong MaxNumOfVariables>
inline bool PathIndexingWithJoinsForInstanceRetrieval<Alloc,Term,MaxTermDepth,MaxTermSize,MaxNumOfVariables>::remove(const TermId& removedTermId)
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
       BK::RuntimeError::report("Wrong term traversal state in PathIndexingWithJoinsForInstanceRetrieval<..>::remove(..)");
       return false;
     #endif
    #endif       
   };

} // bool PathIndexingWithJoinsForInstanceRetrieval<..>::remove(const TermId& removedTermId)



template <class Alloc,class Term,ulong MaxTermDepth,ulong MaxTermSize,ulong MaxNumOfVariables>
inline bool PathIndexingWithJoinsForInstanceRetrieval<Alloc,Term,MaxTermDepth,MaxTermSize,MaxNumOfVariables>::compileQuery(const TermId& query)
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
       BK::RuntimeError::report("Wrong term traversal state in PathIndexingWithJoinsForInstanceRetrieval<..>::compileQuery(..)");
       return false;
     #endif
    #endif    
  };
 
} // bool PathIndexingWithJoinsForInstanceRetrieval<..>::compileQuery(const TermId& query)

template <class Alloc,class Term,ulong MaxTermDepth,ulong MaxTermSize,ulong MaxNumOfVariables>
inline bool PathIndexingWithJoinsForInstanceRetrieval<Alloc,Term,MaxTermDepth,MaxTermSize,MaxNumOfVariables>::nextMatch(TermId& matchId)
{
 CALL("nextMatch(TermId& matchId)");
 return _abstractMachine.nextMatch(matchId);
}

}; // namespace Gem

//============================================================================
#undef DEBUG_NAMESPACE
#ifdef DEBUG_PATH_INDEXING_WITH_JOINS_FOR_INSTANCE_RETRIEVAL
 #define DEBUG_NAMESPACE "PathIndexingWithJoinsForInstanceRetrieval<Alloc,Term,MaxTermDepth,MaxTermSize,MaxNumOfVariables>::IntermediateQueryRepresentation"
#endif
#include "debugMacros.hpp"
//============================================================================

namespace Gem
{
template <class Alloc,class Term,ulong MaxTermDepth,ulong MaxTermSize,ulong MaxNumOfVariables>
inline void PathIndexingWithJoinsForInstanceRetrieval<Alloc,Term,MaxTermDepth,MaxTermSize,MaxNumOfVariables>::IntermediateQueryRepresentation::reset()
{
 CALL("reset()");
 _nextPosition = _positions + 1;
 _currentHoles = 1UL;
 _currentNest = _positions;
 _backtrackHoles.reset();
 _backtrackNonmaxPositions.reset();
 _primaryNestSorting.reset();
} // void PathIndexingWithJoinsForInstanceRetrieval<..>::IntermediateQueryRepresentation::reset()

template <class Alloc,class Term,ulong MaxTermDepth,ulong MaxTermSize,ulong MaxNumOfVariables>   
inline void PathIndexingWithJoinsForInstanceRetrieval<Alloc,Term,MaxTermDepth,MaxTermSize,MaxNumOfVariables>::IntermediateQueryRepresentation::compound(const TermId& subterm,ulong arity,const NonmaxPathRelation* pathRelation)
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

} // void PathIndexingWithJoinsForInstanceRetrieval<..>::IntermediateQueryRepresentation::compound(const TermId& subterm,ulong arity,const NonmaxPathRelation* pathRelation)

template <class Alloc,class Term,ulong MaxTermDepth,ulong MaxTermSize,ulong MaxNumOfVariables>    
inline void PathIndexingWithJoinsForInstanceRetrieval<Alloc,Term,MaxTermDepth,MaxTermSize,MaxNumOfVariables>::IntermediateQueryRepresentation::constant(const TermId& subterm,const MaxPathRelation* pathRelation)
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
 _nextPosition->numInSorting = _primaryNestSorting.num() + _primaryNestSorting.size();
 _primaryNestSorting.push(_nextPosition->maxPathRelation->size());

 _nextPosition++;
} // void PathIndexingWithJoinsForInstanceRetrieval<..>::IntermediateQueryRepresentation::constant(const TermId& subterm,const MaxPathRelation* pathRelation)

template <class Alloc,class Term,ulong MaxTermDepth,ulong MaxTermSize,ulong MaxNumOfVariables>    
inline void PathIndexingWithJoinsForInstanceRetrieval<Alloc,Term,MaxTermDepth,MaxTermSize,MaxNumOfVariables>::IntermediateQueryRepresentation::variable(ulong var)
{
 CALL("variable(ulong var)");

 _substLoaded[var] = false;
 _nextPosition->tag = Variable;
 _nextPosition->nestPosition = _currentNest;
 _nextPosition->variable = var;

 _currentNest->numOfVarArgs++;

 ASSERT(_currentHoles);
 _nextPosition->argNum = _currentNest->arity - _currentHoles; 
 _currentHoles--;
 if (!_currentHoles)
  {
   if (!_currentNest->hasNonvarArg) // _currentNest is a primary nest
    {
     _currentNest->numInSorting = _primaryNestSorting.num() + _primaryNestSorting.size();
     _primaryNestSorting.push(_currentNest->nonmaxPathRelation->size());
    }; 
   if (_backtrackHoles)
    {
     _currentHoles = _backtrackHoles.pop();
     ASSERT(_currentHoles);
     _currentNest = _backtrackNonmaxPositions.pop();
    };
  };

 _nextPosition++;

} // void PathIndexingWithJoinsForInstanceRetrieval<..>::IntermediateQueryRepresentation::variable(ulong var)

template <class Alloc,class Term,ulong MaxTermDepth,ulong MaxTermSize,ulong MaxNumOfVariables>    
inline void PathIndexingWithJoinsForInstanceRetrieval<Alloc,Term,MaxTermDepth,MaxTermSize,MaxNumOfVariables>::IntermediateQueryRepresentation::endOfQuery()
{
 CALL("endOfQuery()");
 ASSERT(!_currentHoles);
 ASSERT(!_backtrackHoles);
 _nextPosition->tag = End;

 //DF; output(cout << "\n\n") << "\n";
 //DF; _primaryNestSorting.output(cout << "\n") << "\n";


} // void PathIndexingWithJoinsForInstanceRetrieval<..>::IntermediateQueryRepresentation::endOfQuery()


template <class Alloc,class Term,ulong MaxTermDepth,ulong MaxTermSize,ulong MaxNumOfVariables>    
inline void PathIndexingWithJoinsForInstanceRetrieval<Alloc,Term,MaxTermDepth,MaxTermSize,MaxNumOfVariables>::IntermediateQueryRepresentation::generateCode()
{
 CALL("generateCode()");
  
 ulong primaryCodeSize = _primaryNestSorting.size();
 AbstractMachine::Instruction* nextSecondaryInstr = _abstractMachine->code + primaryCodeSize;
 AbstractMachine::Instruction* primaryInstr;
 Position* pos;
 Position* nest;
 NonmaxPathRelation::SurferWithMemory* nextNonmaxPathRelationSurfer; 
 MaxPathRelation::SurferWithMemory* nextMaxPathRelationSurfer;
 
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
	  primaryInstr = _abstractMachine->code + (*(pos->numInSorting));
          pos->mainInstruction = primaryInstr;
          primaryInstr->tag = AbstractMachine::NonmaxEqualiseTermId;
          primaryInstr->nonmaxPathRelationSurfer = nextNonmaxPathRelationSurfer;
         };
       }; 
      break;

     case Constant: 
      primaryInstr = _abstractMachine->code + (*(pos->numInSorting));
      pos->mainInstruction = primaryInstr; 
      primaryInstr->tag = AbstractMachine::MaxEqualiseTermId;
      nextMaxPathRelationSurfer = _abstractMachine->maxPathRelationSurfers + (pos - _positions);
      nextMaxPathRelationSurfer->reset(*(pos->maxPathRelation));
      primaryInstr->maxPathRelationSurfer = nextMaxPathRelationSurfer;
      break;

     case Variable:  
      nest = pos->nestPosition;
      ASSERT(nest->numOfVarArgs);
      if ((nest->hasNonvarArg)
          && (!nest->mainInstruction))
       {
	 // compile the variable occurence together with the nest
        #ifdef INSTANCE_RETRIEVAL_INDEX_NO_LOAD_SUBST_FROM_TERM      
         nextNonmaxPathRelationSurfer = _abstractMachine->nonmaxPathRelationSurfers + (nest - _positions);
         nextNonmaxPathRelationSurfer->reset(*(nest->nonmaxPathRelation));
         nextSecondaryInstr->nonmaxPathRelationSurfer = nextNonmaxPathRelationSurfer;
        #else
         if (nest != _positions + 1) // not the top level nest
	  {
           nextNonmaxPathRelationSurfer = _abstractMachine->nonmaxPathRelationSurfers + (nest - _positions);
           nextNonmaxPathRelationSurfer->reset(*(nest->nonmaxPathRelation));
           nextSecondaryInstr->nonmaxPathRelationSurfer = nextNonmaxPathRelationSurfer;
          };
        #endif

        nest->mainInstruction = nextSecondaryInstr;

        if (_substLoaded[pos->variable])
         {
          #ifdef INSTANCE_RETRIEVAL_INDEX_NO_LOAD_SUBST_FROM_TERM 
  	   nextSecondaryInstr->tag = AbstractMachine::GetToTermIdAndCompare;
           nextSecondaryInstr->nonmaxPathCurrNode = 
	    _abstractMachine->nonmaxPathRelationSurfers[nest - _positions].currNodeAddr();
          #else
           if (nest == _positions + 1) // the top level nest
            {
             nextSecondaryInstr->tag = AbstractMachine::LoadArgumentsAndCompareFromTerm;
            }
           else
	    {
             nextSecondaryInstr->tag = AbstractMachine::GetToTermIdAndCompare;
             nextSecondaryInstr->nonmaxPathCurrNode = 
	      _abstractMachine->nonmaxPathRelationSurfers[nest - _positions].currNodeAddr();
            };
          #endif

          nextSecondaryInstr->argNum = pos->argNum;
          nextSecondaryInstr->subst = _abstractMachine->substitution + pos->variable; 
          nextSecondaryInstr++;
         }
        else // first occurence 
         { 
	  _substLoaded[pos->variable] = true;
          #ifdef INSTANCE_RETRIEVAL_INDEX_NO_LOAD_SUBST_FROM_TERM 
           nextSecondaryInstr->tag = AbstractMachine::GetToTermIdAndLoadSubst;
           nextSecondaryInstr->nonmaxPathCurrNode = 
	    _abstractMachine->nonmaxPathRelationSurfers[nest - _positions].currNodeAddr();
          #else
           if (nest == _positions + 1) // the top level nest
            {
             nextSecondaryInstr->tag = AbstractMachine::LoadArgumentsAndLoadSubstFromTerm;
            }
           else
	    {
             nextSecondaryInstr->tag = AbstractMachine::GetToTermIdAndLoadSubst;
             nextSecondaryInstr->nonmaxPathCurrNode = 
	      _abstractMachine->nonmaxPathRelationSurfers[nest - _positions].currNodeAddr();
            };
          #endif
          nextSecondaryInstr->argNum = pos->argNum;
          nextSecondaryInstr->subst = _abstractMachine->substitution + pos->variable; 
          nextSecondaryInstr++;
         };
       }
      else 
       {
        // the variable occurence belongs to a primary nest
	// or the nest has been already compiled
        if (_substLoaded[pos->variable])
         {
          #ifdef INSTANCE_RETRIEVAL_INDEX_NO_LOAD_SUBST_FROM_TERM 
  	   nextSecondaryInstr->tag = AbstractMachine::Compare;
           nextSecondaryInstr->nonmaxPathCurrNode = 
	    _abstractMachine->nonmaxPathRelationSurfers[nest - _positions].currNodeAddr();
          #else
           if ((nest->hasNonvarArg) && (nest == _positions + 1)) // the top level secondary nest
            {
             nextSecondaryInstr->tag = AbstractMachine::CompareFromTerm;
            }
           else
	    {
             nextSecondaryInstr->tag = AbstractMachine::Compare;
             nextSecondaryInstr->nonmaxPathCurrNode = 
	      _abstractMachine->nonmaxPathRelationSurfers[nest - _positions].currNodeAddr();
            };
          #endif
          nextSecondaryInstr->argNum = pos->argNum;
          nextSecondaryInstr->subst = _abstractMachine->substitution + pos->variable; 
          nextSecondaryInstr++;
         }
        else // first occurence 
         { 
	  _substLoaded[pos->variable] = true;
          #ifdef INSTANCE_RETRIEVAL_INDEX_NO_LOAD_SUBST_FROM_TERM 
           nextSecondaryInstr->tag = AbstractMachine::LoadSubst;
           nextSecondaryInstr->nonmaxPathCurrNode = 
	    _abstractMachine->nonmaxPathRelationSurfers[nest - _positions].currNodeAddr();
          #else
           if ((nest->hasNonvarArg) && (nest == _positions + 1)) // the top secondary level nest
            {
             nextSecondaryInstr->tag = AbstractMachine::LoadSubstFromTerm;
            }
           else
	    {
             nextSecondaryInstr->tag = AbstractMachine::LoadSubst;
             nextSecondaryInstr->nonmaxPathCurrNode = 
	      _abstractMachine->nonmaxPathRelationSurfers[nest - _positions].currNodeAddr();
            };
          #endif
          nextSecondaryInstr->argNum = pos->argNum;
          nextSecondaryInstr->subst = _abstractMachine->substitution + pos->variable; 
          nextSecondaryInstr++;
         };
       }; 
      break;

     #ifdef DEBUG_NAMESPACE 
      default: ICP("ICP0"); return;
     #else
      #ifdef _SUPPRESS_WARNINGS_
       default: 
        BK::RuntimeError::report("Wrong tag in PathIndexingWithJoinsForInstanceRetrieval<..>::IntermediateQueryRepresentation::generateCode()"); 
        return;
      #endif   
     #endif
    };
  };
 
 ASSERT(pos->tag == End);
 nextSecondaryInstr->tag = AbstractMachine::Success;
 _abstractMachine->firstRetrievalCall = true;

 //DF; _abstractMachine->outputCode(cout << "\n\n") << "\n";

} // void PathIndexingWithJoinsForInstanceRetrieval<..>::IntermediateQueryRepresentation::generateCode()


#ifndef NO_DEBUG_VIS
 template <class Alloc,class Term,ulong MaxTermDepth,ulong MaxTermSize,ulong MaxNumOfVariables>    
 inline ostream& PathIndexingWithJoinsForInstanceRetrieval<Alloc,Term,MaxTermDepth,MaxTermSize,MaxNumOfVariables>::IntermediateQueryRepresentation::output(ostream& str) const
 {
  for (const Position* p = _positions; p < _nextPosition; p++)
   p->output(str,_positions) << "\n";
  return str;
 } // ostream& PathIndexingWithJoinsForInstanceRetrieval<..>::IntermediateQueryRepresentation::output(ostream& str) const
#endif



}; // namespace Gem

//============================================================================
#undef DEBUG_NAMESPACE
#ifdef DEBUG_PATH_INDEXING_WITH_JOINS_FOR_INSTANCE_RETRIEVAL
 #define DEBUG_NAMESPACE "PathIndexingWithJoinsForInstanceRetrieval<Alloc,Term,MaxTermDepth,MaxTermSize,MaxNumOfVariables>::IntermediateQueryRepresentation::Position"
#endif
#include "debugMacros.hpp"
//============================================================================


namespace Gem
{

#ifndef NO_DEBUG_VIS
 template <class Alloc,class Term,ulong MaxTermDepth,ulong MaxTermSize,ulong MaxNumOfVariables>    
 inline ostream& PathIndexingWithJoinsForInstanceRetrieval<Alloc,Term,MaxTermDepth,MaxTermSize,MaxNumOfVariables>::IntermediateQueryRepresentation::Position::output(ostream& str,const Position* base) const
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
 } // ostream& PathIndexingWithJoinsForInstanceRetrieval<..>::IntermediateQueryRepresentation::Position::output(ostream& str,const Position* base) const
#endif

}; // namespace Gem

//============================================================================
#undef DEBUG_NAMESPACE
#ifdef DEBUG_PATH_INDEXING_WITH_JOINS_FOR_INSTANCE_RETRIEVAL
 #define DEBUG_NAMESPACE "PathIndexingWithJoinsForInstanceRetrieval<Alloc,Term,MaxTermDepth,MaxTermSize,MaxNumOfVariables>::AbstractMachine"
#endif
#include "debugMacros.hpp"
//============================================================================

namespace Gem
{

template <class Alloc,class Term,ulong MaxTermDepth,ulong MaxTermSize,ulong MaxNumOfVariables>  
inline bool PathIndexingWithJoinsForInstanceRetrieval<Alloc,Term,MaxTermDepth,MaxTermSize,MaxNumOfVariables>::AbstractMachine::nextMatch(TermId& matchId)
{
 CALL("nextMatch(TermId& matchId)");
 Instruction* instr = code;
 TermId termId;
 #ifndef INSTANCE_RETRIEVAL_INDEX_NO_LOAD_SUBST_FROM_TERM
  TermArguments arguments;
 #endif

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
      BK::RuntimeError::report("Wrong tag in PathIndexingWithJoinsForInstanceRetrieval<..>::AbstractMachine::nextMatch(TermId& matchId)"); 
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

     case GetToTermId: 
      instr->nonmaxPathRelationSurfer->getTo(termId);
      goto next_instr;

     case LoadSubst:
      *(instr->subst) = *((*(instr->nonmaxPathCurrNode))->value().nth(instr->argNum));    
      goto next_instr;
          
     #ifndef INSTANCE_RETRIEVAL_INDEX_NO_LOAD_SUBST_FROM_TERM
      case  LoadSubstFromTerm:
       *(instr->subst) = *(arguments.nth(instr->argNum));    
       goto next_instr;
     #endif    

     case Compare: 
      if ((*(instr->subst)) != (*(*(instr->nonmaxPathCurrNode))->value().nth(instr->argNum))) 
       goto backtrack;
      goto next_instr;

     #ifndef INSTANCE_RETRIEVAL_INDEX_NO_LOAD_SUBST_FROM_TERM  
      case CompareFromTerm: 
       if ((*(instr->subst)) != (*(arguments.nth(instr->argNum))))
        goto backtrack;
       goto next_instr;
     #endif

     case Success:
      matchId = termId;     
      
      //DF; cout << "Success\n";

      return true;

     case GetToTermIdAndLoadSubst:
      instr->nonmaxPathRelationSurfer->getTo(termId);      
      *(instr->subst) = *((*(instr->nonmaxPathCurrNode))->value().nth(instr->argNum));    
      goto next_instr;

     case GetToTermIdAndCompare:
      instr->nonmaxPathRelationSurfer->getTo(termId);      
      if ((*(instr->subst)) != (*(*(instr->nonmaxPathCurrNode))->value().nth(instr->argNum))) 
       goto backtrack;
      goto next_instr;

     #ifndef INSTANCE_RETRIEVAL_INDEX_NO_LOAD_SUBST_FROM_TERM

      case LoadArgumentsAndLoadSubstFromTerm:       
       arguments.reset(termId);
       *(instr->subst) = *(arguments.nth(instr->argNum));    
       goto next_instr;

      case LoadArgumentsAndCompareFromTerm:
       arguments.reset(termId);
       if ((*(instr->subst)) != (*(arguments.nth(instr->argNum))))
        goto backtrack;
       goto next_instr; 
 
     #endif     

     #ifdef DEBUG_NAMESPACE 
      default: ICP("ICP1"); return false; 
     #else
      #ifdef _SUPPRESS_WARNINGS_
       default: 
        BK::RuntimeError::report("Wrong tag in PathIndexingWithJoinsForInstanceRetrieval<..>::AbstractMachine::nextMatch(TermId& matchId)"); 
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
       BK::RuntimeError::report("Wrong tag in PathIndexingWithJoinsForInstanceRetrieval<..>::AbstractMachine::nextMatch(TermId& matchId)"); 
       return false;
     #endif   
    #endif
   };

} // bool PathIndexingWithJoinsForInstanceRetrieval<..>::AbstractMachine::nextMatch(TermId& matchId)


#ifndef NO_DEBUG_VIS
       
 template <class Alloc,class Term,ulong MaxTermDepth,ulong MaxTermSize,ulong MaxNumOfVariables>  
 inline ostream& PathIndexingWithJoinsForInstanceRetrieval<Alloc,Term,MaxTermDepth,MaxTermSize,MaxNumOfVariables>::AbstractMachine::outputCode(ostream& str) const
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
 } //  PathIndexingWithJoinsForInstanceRetrieval<..>::AbstractMachine::outputCode(ostream& str) const  

#endif


}; // namespace Gem

//============================================================================
#undef DEBUG_NAMESPACE
#ifdef DEBUG_PATH_INDEXING_WITH_JOINS_FOR_INSTANCE_RETRIEVAL
 #define DEBUG_NAMESPACE "PathIndexingWithJoinsForInstanceRetrieval<Alloc,Term,MaxTermDepth,MaxTermSize,MaxNumOfVariables>::AbstractMachine::Instruction"
#endif
#include "debugMacros.hpp"
//============================================================================

namespace Gem
{

#ifndef NO_DEBUG_VIS
       
 template <class Alloc,class Term,ulong MaxTermDepth,ulong MaxTermSize,ulong MaxNumOfVariables>  
 inline ostream& PathIndexingWithJoinsForInstanceRetrieval<Alloc,Term,MaxTermDepth,MaxTermSize,MaxNumOfVariables>::AbstractMachine::Instruction::output(ostream& str,const Instruction* base,const TermId* substitution) const
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
    case GetToTermId:
     str << "GetToTermId";
     break;
    case LoadSubst:
     str << "LoadSubst"
         << " arg " << argNum
         << " var " << (subst - substitution);
     break;
    #ifndef INSTANCE_RETRIEVAL_INDEX_NO_LOAD_SUBST_FROM_TERM
     case  LoadSubstFromTerm:
      str << "LoadSubstFromTerm"
          << " arg " << argNum
          << " var " << (subst - substitution);
     break;
    #endif
    case Compare: 
     str << "Compare"
         << " arg " << argNum
         << " var " << (subst - substitution);
     break;
    #ifndef INSTANCE_RETRIEVAL_INDEX_NO_LOAD_SUBST_FROM_TERM
     case CompareFromTerm: 
      str << "CompareFromTerm"
          << " arg " << argNum
          << " var " << (subst - substitution);
     break;
    #endif
    case Success:
     str << "Success";
     break;
    
    case GetToTermIdAndLoadSubst:
     str << "GetToTermIdAndLoadSubst"
         << " arg " << argNum
         << " var " << (subst - substitution);
     break;
    case GetToTermIdAndCompare:
     str << "GetToTermIdAndCompare"
         << " arg " << argNum
         << " var " << (subst - substitution);
     break;
    #ifndef INSTANCE_RETRIEVAL_INDEX_NO_LOAD_SUBST_FROM_TERM
     case LoadArgumentsAndLoadSubstFromTerm: 
      str << "LoadArgumentsAndLoadSubstFromTerm"
         << " arg " << argNum
         << " var " << (subst - substitution);
      break;
     case LoadArgumentsAndCompareFromTerm:
      str << "LoadArgumentsAndCompareFromTerm"
          << " arg " << argNum
          << " var " << (subst - substitution);
      break;
      
    #endif  
    default: str << "UnknownInstructionType"; 
   }; 
  return str;
 } //  PathIndexingWithJoinsForInstanceRetrieval<..>::AbstractMachine::Instruction::output(ostream& str,const Instruction* base,const TermId* substitution) const  

#endif

}; // namespace Gem




//======================================================================
#undef DEBUG_NAMESPACE
//======================================================================
#endif
