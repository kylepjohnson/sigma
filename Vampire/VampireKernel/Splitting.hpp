//
// File:         Splitting.hpp
// Description:  Core of splitting without backtracking.
//               Additionally implements negative equality splitting.
// Created:      Dec 12, 2001 
// Author:       Alexandre Riazanov
// mail:         riazanov@cs.man.ac.uk
// Revised:      Dec 17, 2001.
//               Array ulong _litNumMap[HellConst::MaxNumOfLiterals]
//               is replace by 
//               TmpLiteral* _numLitMap[HellConst::MaxNumOfLiterals] 
//============================================================================
#ifndef SPLITTING_H
//============================================================================
#define SPLITTING_H 
#include "jargon.hpp"
#include "RuntimeError.hpp"
#include "VampireKernelDebugFlags.hpp"
#include "VampireKernelConst.hpp"
#include "Array.hpp"
#include "ExpandingMultiset.hpp"
#include "GlobAlloc.hpp"
#include "DivIntoComp.hpp"
#include "ClauseQueue.hpp"
#include "TmpLitList.hpp"
#include "OpenNewClause.hpp"
//============================================================================
#undef DEBUG_NAMESPACE
#ifdef DEBUG_SPLITTING
 #define DEBUG_NAMESPACE "Splitting"
#endif
#include "debugMacros.hpp"
//============================================================================

namespace VK
{
class ComponentName;
class ForwardSubsumptionFrontEnd;
class Clause;
class PagedPassiveClauseStorage;
class ClauseAssembler;

class Splitting
{
 public:
  class Statistics
  {
   public:
    Statistics() { reset(); };
    ~Statistics() {};
    void init() { reset(); };
    void destroy() {};
    void reset() 
    {
     forwardSubsumed = 0;
     simplifiedByFSR = 0;
     differentComponents = 0;     
     split = 0;
     components = 0;
    };
   
    ulong forwardSubsumed;
    ulong simplifiedByFSR;
    ulong differentComponents;
    ulong split;
    ulong components;
  }; // class Statistics

 public:
  Splitting(OpenClauseBackground* ancestors);
  ~Splitting();
  
  void init(OpenClauseBackground* ancestors);
  void destroy();
  
  void setStyle(long num) { _splittingStyle = num; };
  void setSplittingWithNamingFlag(bool fl)
  {
   _splittingWithNamingFlag = fl;
  };  
  void setForwardSubsumptionFlag(bool fl)
  {
   _forwardSubsumptionFlag = fl;
  };
  void setForwardSubsumptionResolutionFlag(bool fl)
  {
   _forwardSubsumptionResolutionFlag = fl;
  };
  void setNegEqSplittingMinEligibleTermSize(ulong sz)
  {
   _negEqSplittingMinEligibleTermSize = sz;
  };
  void setForwardSubsumption(ForwardSubsumptionFrontEnd* fs)
  {
   _forwardSubsumption = fs;
  };
  void setComponentNamingService(ComponentName* compNaming)
  {
   _compName = compNaming;
  };   
  void setClauseAssembler(ClauseAssembler* clauseAssembler)
  {
   _clauseAssembler = clauseAssembler;
  };

  void adjustSizes(ulong newNumOfLits);


  // getting statistics 

  ulong statForwardSubsumed() const { return _statistics.forwardSubsumed; };
  ulong statSimplifiedByFSR() const { return _statistics.simplifiedByFSR; };
  ulong statSplit() const { return _statistics.split; };
  ulong statDifferentComponents() const { return _statistics.differentComponents; };
  ulong statComponents() const { return _statistics.components; };
  
  // keeping statistics
  void registerAnotherForwardSubsumed() { _statistics.forwardSubsumed++; };  
  void registerAnotherSimplifiedByFSR() { _statistics.simplifiedByFSR++; };
  void registerAnotherSplit(ulong numOfComponents) 
  { 
   _statistics.split++; 
   _statistics.components += numOfComponents;
  };
  void registerAnotherComponent() { _statistics.differentComponents++; };

  bool Split(TmpLitList& lits,ClauseQueue& newClauses);
  bool Split3(TmpLitList& lits,
              Clause*& answerClause,
              PagedPassiveClauseStorage& storage,
              ulong& numOfNewClauses,
              ulong& numOfRejectedNewClauses,
	      Clause::NewNumber* clauseNumberGenerator,
              bool showNewClauses);

  void splitNegativeEqualities(TmpLitList& lits,ClauseQueue& newClauses);

 private:

  bool NonblockingSplitting(TmpLitList& lits,ClauseQueue& newClauses);
  bool NonblockingSplitting3(TmpLitList& lits,  
                             Clause*& answerClause, 
                             PagedPassiveClauseStorage& storage,
                             ulong& numOfNewClauses,
                             ulong& numOfRejectedNewClauses,
                             Clause::NewNumber* clauseNumberGenerator,
                             bool showNewClauses);
   


  bool BlockingSplitting(TmpLitList& lits,ClauseQueue& newClauses);
  bool BlockingSplitting3(TmpLitList& lits,                            
                          Clause*& answerClause,
                          PagedPassiveClauseStorage& storage,
                          ulong& numOfNewClauses,
                          ulong& numOfRejectedNewClauses,
                          Clause::NewNumber* clauseNumberGenerator,
                          bool showNewClauses);

  void collectRegisteredComponents(TmpLitList& lits)
    {
      CALL("collectRegisteredComponents(TmpLitList& lits)");
      for (ulong n = 0; n < _registeredCompNums.numOfDiffElements(); n++) 
	{
	  lits.collectFrom(_components[_registeredCompNums.nth(n)]);
	};
    };


 private:
  long _splittingStyle;
  bool _splittingWithNamingFlag;
  bool _forwardSubsumptionFlag;
  bool _forwardSubsumptionResolutionFlag;
  long _negEqSplittingMinEligibleTermSize;
  OpenClauseBackground* _ancestors;
  Statistics _statistics;  
  ulong _maxNumOfLitsSoFar;
  DivIntoComp<VampireKernelConst::MaxNumOfVariables> _divisionIntoComponents;


  BK::Array<BK::GlobAlloc,TmpLiteral*,32UL,Splitting> _numLitMap;

  OpenNewClause _basicComponent;
  ComponentName* _compName;
  ClauseAssembler* _clauseAssembler;
  ForwardSubsumptionFrontEnd* _forwardSubsumption;

  BK::Array<BK::GlobAlloc,TmpLitList,32UL,Splitting> _components;


  BK::ExpandingMultiset<BK::GlobAlloc,ulong,32UL,Splitting> _registeredCompNums;


  BK::Array<BK::GlobAlloc,ulong,32UL,Splitting> _componentName;    

 
  BK::Array<BK::GlobAlloc,bool,32UL,Splitting> _newCompName;  

}; // class Splitting

}; // namespace VK

//============================================================================
#undef DEBUG_NAMESPACE
#ifdef DEBUG_SPLITTING
 #define DEBUG_NAMESPACE "Splitting"
#endif
#include "debugMacros.hpp"
//============================================================================


namespace VK
{
inline void Splitting::adjustSizes(ulong newNumOfLits)
{
 CALL("adjustSizes(ulong newNumOfLits)");
 if (newNumOfLits > _maxNumOfLitsSoFar)
  {
   _divisionIntoComponents.adjustSizes(newNumOfLits);
   
   DOP(_numLitMap.unfreeze());
   _numLitMap.expand(newNumOfLits);
   DOP(_numLitMap.freeze());

   ulong prevSize = _components.size();  
   DOP(_components.unfreeze());  
   _components.expand(newNumOfLits);
   DOP(_components.freeze());
   for (ulong i = prevSize; i < _components.size(); i++)
     _components[i].init();


   DOP(_registeredCompNums.unfreeze());
   _registeredCompNums.expand(newNumOfLits);
   DOP(_registeredCompNums.freeze());


   DOP(_componentName.unfreeze());
   _componentName.expand(newNumOfLits);
   DOP(_componentName.freeze());   


   DOP(_newCompName.unfreeze());
   _newCompName.expand(newNumOfLits);
   DOP(_newCompName.freeze());

   _maxNumOfLitsSoFar = newNumOfLits;
  }; 
}; // void Splitting::adjustSizes(ulong newNumOfLits) 

inline bool Splitting::Split(TmpLitList& lits,ClauseQueue& newClauses)
{
 CALL("Split(TmpLitList& lits,ClauseQueue& newClauses)");

 adjustSizes(lits.numOfAllLiterals()); 

 switch (_splittingStyle)
  {
   case 1L: return NonblockingSplitting(lits,newClauses);
   case 2L: return BlockingSplitting(lits,newClauses);
   default: 
    BK::RuntimeError::report("Wrong splitting style number."); 
    return false; 
  };
}; //  bool Splitting::Split(TmpLitList& lits,ClauseQueue& newClauses)

inline bool Splitting::Split3(TmpLitList& lits,                            
                              Clause*& answerClause,
                              PagedPassiveClauseStorage& storage,
                              ulong& numOfNewClauses,
                              ulong& numOfRejectedNewClauses,
                              Clause::NewNumber* clauseNumberGenerator,
                              bool showNewClauses)
{
  CALL("Split3(TmpLitList& lits,...)"); 

  adjustSizes(lits.numOfAllLiterals()); 

  switch (_splittingStyle)
  {
   case 1L: return NonblockingSplitting3(lits,
                                         answerClause,
                                         storage,
                                         numOfNewClauses,
                                         numOfRejectedNewClauses,
                                         clauseNumberGenerator,
                                         showNewClauses);
   case 2L: return BlockingSplitting3(lits,
                                      answerClause,
                                      storage,
                                      numOfNewClauses,
                                      numOfRejectedNewClauses,
                                      clauseNumberGenerator,
                                      showNewClauses);
   default: 
    BK::RuntimeError::report("Wrong splitting style number."); 
    return false; 
  };

}; // bool Splitting::Split3(TmpLitList& lits,PagedPassiveClauseStorage& storage,...



}; // namespace VK
//======================================================================
#endif
