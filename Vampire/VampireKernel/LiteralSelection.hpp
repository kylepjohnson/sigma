//
// File:         LiteralSelection.hpp
// Description:  Defines various literal selection functions
// Created:      Dec 12, 2001
// Author:       Alexandre Riazanov
// mail:         riazanov@cs.man.ac.uk
// Note:         Was a part of NewClause.
// Revised:      Dec 15, 2001. Bug fix in setSplittingFlag.
//============================================================================
#ifndef LITERAL_SELECTION_H
//=============================================================================

#define LITERAL_SELECTION_H 
#include "jargon.hpp"
#include "RuntimeError.hpp"
#include "VampireKernelDebugFlags.hpp"
#include "TmpLitList.hpp"
#include "LiteralOrdering.hpp"
class SymbolInfo;
//============================================================================
#undef DEBUG_NAMESPACE
#ifdef DEBUG_LITERAL_SELECTION
#define DEBUG_NAMESPACE "LiteralSelection"
#endif
#include "debugMacros.hpp"
//============================================================================

namespace VK
{
class LiteralSelection
{
 public:
  LiteralSelection() 
    {
      init();
    };
  
  ~LiteralSelection() {};
  void init()
    { 
      _literalComparisonMode = 0L;
      _literalOrdering = 0;
      _inversePolarity = false;
      _basicSelFun = function1; 
      _splittingIsEnabled = false;
      _splittingStyle = 0L;
    };
  void destroy() {};
  void setFunNum(long num);
  void setLiteralComparisonMode(long mode) { _literalComparisonMode = mode; };
  void setLiteralOrdering(LiteralOrdering* ord) { _literalOrdering = ord; };

  void setSplittingFlag(bool fl) 
    { 
      _splittingIsEnabled = (_splittingIsEnabled || fl);
      //  _splittingIsEnabled can only be changed from false to true.
    };
  void setSplittingStyle(long splittingStyle) { _splittingStyle = splittingStyle; };


#ifdef VKERNEL_FOR_STEP_RESOLUTION
  void setStepResolutionLiteralSelection(bool fl)
  {
    _stepResolutionLiteralSelectionFlag = fl;    
  };
#endif



  inline 
    bool 
    isPositiveForSelection(const TERM& header) const;
  
  inline
    bool 
    isNegativeForSelection(const TERM& header) const;


    void 
      select(TmpLitList& lits,TmpLitList& selectedLits,
	     TmpLitList& nonselectedLits,bool& negSelectionUsed) const;
    
    void 
      selectForSupport(TmpLitList& lits,
		       TmpLitList& selectedLits,
		       TmpLitList& nonselectedLits) const;
    
  bool 
    selectForElimination(TmpLitList& lits,TmpLitList& selectedLits,
			 TmpLitList& nonselectedLits,  
			 const SymbolInfo* eliminationTable) const;

 private:

  void 
  unselectLiteralsFromNonmaximalLayers(TmpLitList& lits,
				       TmpLitList& nonselectedLits) const;


  void 
    selectWithNonblockingSplitting(TmpLitList& lits,TmpLitList& selectedLits,
				   TmpLitList& nonselectedLits,bool& negSelectionUsed,
	                           const TmpLitList::Statistics& stat) const;

  void 
    selectWithBlockingSplitting(TmpLitList& lits,TmpLitList& selectedLits,
				TmpLitList& nonselectedLits,bool& negSelectionUsed,
	                        const TmpLitList::Statistics& stat) const;


  static 
    void 
    function1(TmpLitList& lits,TmpLitList& selectedLits,
	      TmpLitList& nonselectedLits,bool& negSelectionUsed,
	      const TmpLitList::Statistics& stat);

  static 
    void 
    function2(TmpLitList& lits,TmpLitList& selectedLits,
	      TmpLitList& nonselectedLits,bool& negSelectionUsed,
	      const TmpLitList::Statistics& stat);

  static 
    void 
    function3(TmpLitList& lits,TmpLitList& selectedLits,
	      TmpLitList& nonselectedLits,bool& negSelectionUsed,
	      const TmpLitList::Statistics& stat);

  static 
    void 
    function4(TmpLitList& lits,TmpLitList& selectedLits,
	      TmpLitList& nonselectedLits,bool& negSelectionUsed,
	      const TmpLitList::Statistics& stat); 

  static 
    void 
    function5(TmpLitList& lits,TmpLitList& selectedLits,
	      TmpLitList& nonselectedLits,bool& negSelectionUsed,
	      const TmpLitList::Statistics& stat); 

  static 
    void 
    function6(TmpLitList& lits,TmpLitList& selectedLits,
	      TmpLitList& nonselectedLits,bool& negSelectionUsed,
	      const TmpLitList::Statistics& stat);  
  
  static 
    void 
    function7(TmpLitList& lits,TmpLitList& selectedLits,
	      TmpLitList& nonselectedLits,bool& negSelectionUsed,
	      const TmpLitList::Statistics& stat);


  static 
    void 
    function1002(TmpLitList& lits,TmpLitList& selectedLits,
		 TmpLitList& nonselectedLits,bool& negSelectionUsed,
		 const TmpLitList::Statistics& stat);

  static 
    void 
    function1003(TmpLitList& lits,TmpLitList& selectedLits,
		 TmpLitList& nonselectedLits,bool& negSelectionUsed,
		 const TmpLitList::Statistics& stat);

  static 
    void 
    function1004(TmpLitList& lits,TmpLitList& selectedLits,
		 TmpLitList& nonselectedLits,bool& negSelectionUsed,
		 const TmpLitList::Statistics& stat);

  static 
    void 
    function1005(TmpLitList& lits,TmpLitList& selectedLits,
		 TmpLitList& nonselectedLits,bool& negSelectionUsed,
		 const TmpLitList::Statistics& stat);

  static 
    void 
    function1006(TmpLitList& lits,TmpLitList& selectedLits,
		 TmpLitList& nonselectedLits,bool& negSelectionUsed,
		 const TmpLitList::Statistics& stat);

  static 
    void 
    function1007(TmpLitList& lits,TmpLitList& selectedLits,
		 TmpLitList& nonselectedLits,bool& negSelectionUsed,
		 const TmpLitList::Statistics& stat);

  static 
    void 
    function1008(TmpLitList& lits,TmpLitList& selectedLits,
		 TmpLitList& nonselectedLits,bool& negSelectionUsed,
		 const TmpLitList::Statistics& stat);

  static 
    void 
    function1009(TmpLitList& lits,TmpLitList& selectedLits,
		 TmpLitList& nonselectedLits,bool& negSelectionUsed,
		 const TmpLitList::Statistics& stat);  


  inline
    static
    void 
    selectInherentlySelected(TmpLitList& lits,ulong numOfSelected,TmpLitList& selectedLits,TmpLitList& nonselectedLits);


  inline
    bool 
    satisfactorySelection1(TmpLitList& selectedLits,TmpLitList& nonselectedLits) const;

  inline
    bool 
    satisfactorySelection2(TmpLitList& selectedLits,TmpLitList& nonselectedLits) const;
  
  inline
    bool
    satisfactorySelection3(TmpLitList& selectedLits,TmpLitList& nonselectedLits) const;
   

  inline
    void 
    selectNegativeWithMaximalWeight(TmpLitList& lits,TmpLitList& selectedLits,
				    TmpLitList& nonselectedLits) const;

  inline
    void 
    selectNegativeLeastProlific(TmpLitList& lits,TmpLitList& selectedLits,
				TmpLitList& nonselectedLits) const;  
  
  inline
    void 
    selectNegativeMaximallyNongroundWithMaximalWeight(TmpLitList& lits,
						      TmpLitList& selectedLits,
						      TmpLitList& nonselectedLits) const;

  inline 
    void 
    unselectAnswerLiterals(TmpLitList& lits,
			   TmpLitList& selectedLits,
			   TmpLitList& nonselectedLits,
                           ulong numOfAnswerLits) const;

#ifdef VKERNEL_FOR_STEP_RESOLUTION
  static 
  inline
  bool 
  isNextStepLiteral(const TmpLiteral* lit);

  static
  inline
  bool 
  isCurrentStepLiteral(const TmpLiteral* lit);

  void 
  inline
  unselectCurrentStateLiterals(TmpLitList& lits,
			       TmpLitList& selectedLits,
			       TmpLitList& nonselectedLits) const;
#endif



 private:
  static const LiteralSelection* _currentObject; 
  // To communicate the current object to the static selection functions.
  // They are made static to be represented by pointers.   
  void (*_basicSelFun)(TmpLitList&,TmpLitList&,TmpLitList&,bool&,const TmpLitList::Statistics&);
  long _literalComparisonMode;
  LiteralOrdering* _literalOrdering;
  bool _inversePolarity;
  bool _splittingIsEnabled;
  long _splittingStyle;
  
#ifdef VKERNEL_FOR_STEP_RESOLUTION
  bool _stepResolutionLiteralSelectionFlag;
#endif
}; // class LiteralSelection

}; // namespace VK

//============================================================================
#undef DEBUG_NAMESPACE
#ifdef DEBUG_LITERAL_SELECTION
#define DEBUG_NAMESPACE "LiteralSelection"
#endif
#include "debugMacros.hpp"
//============================================================================


namespace VK
{
inline 
void 
LiteralSelection::setFunNum(long num)
{
  _inversePolarity = (num < 0);
  switch (num) 
    {
    case 1L : 
      _basicSelFun = function1; 
      return;
    case 2L : 
      _basicSelFun = function2;     
      return;
    case 3L : 
      _basicSelFun = function3; 
      return;
    case 4L : 
      _basicSelFun = function4; 
      return;
    case 5L : 
      _basicSelFun = function5;
      return;
    case 6L : 
      _basicSelFun = function6; 
      return;
    case 7L : 
      _basicSelFun = function7; 
      return;


    case 1002L :
      _basicSelFun = function1002; 
      return;
    case 1003L :
      _basicSelFun = function1003; 
      return;
    case 1004L :
      _basicSelFun = function1004; 
      return;
    case 1005L :
      _basicSelFun = function1005; 
      return;
    case 1006L :
      _basicSelFun = function1006; 
      return;
    case 1007L :
      _basicSelFun = function1007; 
      return;
    case 1008L :
      _basicSelFun = function1008; 
      return;    
    case 1009L :
      _basicSelFun = function1009; 
      return;          
     

    case -2L : 
      _basicSelFun = function2;
      return;
    case -3L : 
      _basicSelFun = function3;
      return;
    case -4L : 
      _basicSelFun = function4;
      return;
    case -5L : 
      _basicSelFun = function5;
      return;
    case -6L : 
      _basicSelFun = function6;
      return;
    case -7L : 
      _basicSelFun = function7;
      return;

    case -1005L :
      _basicSelFun = function1005; 
      return;
    case -1009L :
      _basicSelFun = function1009; 
      return;

    default: BK::RuntimeError::report("Wrong selection number.");
    };
}; // void LiteralSelection::setFunNum(long num)





inline 
bool 
LiteralSelection::isPositiveForSelection(const TERM& header) const
{
  if (header.IsEquality() || header.IsSplitting() || (!_inversePolarity)) return header.Positive();
  return header.Negative();
}; 


inline 
bool 
LiteralSelection::isNegativeForSelection(const TERM& header) const
{
  if (header.IsEquality() || header.IsSplitting()  || (!_inversePolarity)) return header.Negative();
  return header.Positive();
}; 


}; // namespace VK


//======================================================================
#endif
