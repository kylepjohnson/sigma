//
// File:         WeightPolynomialSpecialisedComparisonCommand.hpp
// Description:  Definition of 
//               class WeightPolynomial::SpecialisedComparison::Command
// Created:      Apr 29, 2002, Alexandre Riazanov, riazanov@cs.man.ac.uk
// Revised:      May 14, 2002, Alexandre Riazanov, riazanov@cs.man.ac.uk
//               Some weight comparison commands changed. 
//===================================================================
#ifndef WEIGHT_POLYNOMIAL_SPECIALISED_COMPARISON_COMMAND_H
#define WEIGHT_POLYNOMIAL_SPECIALISED_COMPARISON_COMMAND_H
//===================================================================
#ifndef NO_DEBUG_VIS
#include <iostream>
#endif
#include "jargon.hpp"
#include "VampireKernelDebugFlags.hpp"
#include "VampireKernelConst.hpp"
#include "WeightPolynomial.hpp"
#include "TermWeightType.hpp"
//===================================================================
#undef DEBUG_NAMESPACE
#ifdef DEBUG_WEIGHT_POLYNOMIAL
 #define DEBUG_NAMESPACE "WeightPolynomial::SpecialisedComparison::Command"
#endif
#include "debugMacros.hpp"
//===================================================================

namespace VK
{
class WeightPolynomial::SpecialisedComparison::Command
{
 public:
  enum Tag
  {
    RetFailureToCompare,
    RetAlwaysLess,
    RetAlwaysEqual,
    RetAlwaysGreater,
    RetCanBeLessOrEqual,
    RetCanBeGreaterOrEqual,
    RetVolatileButNearlyAlwaysLess,
    RetVolatileButNearlyAlwaysGreater,
    RetAbsolutelyVolatile,     
    

    //----------------------------------------

    IfXGr1RetFail_IfGrndRetAlwEq_RetFail,
    /*
      if min|X@| > 1 return FailureToCompare;
      if ground(X@) return AlwaysEqual;
      return FailureToCompare;
     */

    IfXGr1RetFail_IfNotGrndRetFail,
    /*
      if min|X@| > 1 return FailureToCompare;
      if !ground(X@) return FailureToCompare;
    */

 

    //----------------------------------------



    IfXGr1RetAlwGr_IfGrndRetAlwEq_RetCanBeGrOrEq,
    /*
      if min|X@| > 1 return AlwaysGreater;
      if ground(X@) return AlwaysEqual;
      return CanBeGreaterOrEqual;
     */

    IfXGr1RetAlwGr_GrndToGRND,
    /*
      if min|X@| > 1 return AlwaysGreater;
      GRND := ground(X@);
     */

    IfXGr1RetAlwGr_UpdateGRND,
    /*
      if min|X@| > 1 return AlwaysGreater;
      GRND := GRND && ground(X@);
    */

    IfXGr1RetAlwGr_UpdateGRND_IfGRNDRetAlwEq_RetCanBeGrOrEq,
    /*
      if min|X@| > 1 return AlwaysGreater;
      GRND := GRND && ground(X@);
      if GRND return AlwaysEqual;
      return CanBeGreaterOrEqual;
    */



    //----------------------------------------


    IfNotGrndRetFail_IfXGrWRetFail_IfXLeWRetAlwGr_RetAlwEq,
    /*
      if !ground(X@) return FailureToCompare;
      if min|X@| > W return FailureToCompare;
      if min|X@| < W return AlwaysGreater;
      return AlwaysEqual;
    */


    IfNotGrndRetFail_IfCXGrWRetFail_IfCXLeWRetAlwGr_RetAlwEq,
    /*
      if !ground(X@) return FailureToCompare;
      if C*min|X@| > W return FailureToCompare;
      if C*min|X@| < W return AlwaysGreater;
      return AlwaysEqual;
    */



    IfNotGrndRetFail_WMinusXToACC_IfACCLe0RetFail,
    /*
      if !ground(X@) return FailureToCompare;
      ACC := W - min|X@|;
      if ACC < 0 return FailureToCompare;      
    */



    IfNotGrndRetFail_WMinusCXToACC_IfACCLe0RetFail,
    /*
      if !ground(X@) return FailureToCompare;
      ACC := W - C*min|X@|;
      if ACC < 0 return FailureToCompare;      
    */


    IfNotGrndRetFail_ACCMinusXPlus1ToACC_IfACCLe0RetFail,
    /*
      if !ground(X@) return FailureToCompare;
      ACC := ACC - min|X@| + 1;
      if ACC < 0 return FailureToCompare;
    */    
    

    IfNotGrndRetFail_ACCMinusCXPlusCToACC_IfACCLe0RetFail,
    /*
      if !ground(X@) return FailureToCompare;
      ACC := ACC - C*min|X@| + C;
      if ACC < 0 return FailureToCompare; 
    */


    IfNotGrndRetFail_ACCMinusXPlus1ToACC_IfACCLe0RetFail_IfACCEq0RetAlwEq_RetAlwGr,
    /*
      if !ground(X@) return FailureToCompare;
      ACC := ACC - min|X@| + 1;
      if ACC < 0 return FailureToCompare; 
      if ACC = 0 return AlwaysEqual;
      return AlwaysGreater;
    */    


    IfNotGrndRetFail_ACCMinusCXPlusCToACC_IfACCLe0RetFail_IfACCEq0RetAlwEq_RetAlwGr,
    /*
      if !ground(X@) return FailureToCompare;
      ACC := ACC - C*min|X@| + C;
      if ACC < 0 return FailureToCompare; 
      if ACC = 0 return AlwaysEqual;
      return AlwaysGreater;
    */


    //----------------------------------------



    IfXGrWRetAlwGr_IfXLeWRetFail_IfGrndRetAlwEq_RetCanBeGrOrEq,
    /*
      ASSERT(W > 0L);
      if min|X@| > W return AlwaysGreater;
      if min|X@| < W return FailureToCompare;
      if ground(X@) return AlwaysEqual;
      return CanBeGreaterOrEqual;
    */
    
    IfCXGrWRetAlwGr_IfCXLeWRetFail_IfGrndRetAlwEq_RetCanBeGrOrEq,
    /*
      ASSERT(W > 0L);
      if C*min|X@| > W return AlwaysGreater;
      if C*min|X@| < W return FailureToCompare;
      if ground(X@) return AlwaysEqual;
      return CanBeGreaterOrEqual;
    */


    WMinusXToACC_IfACCLe0RetAlwGr_GrndToGRND,
    /*
      ACC := W - min|X@|;
      if ACC < 0 return AlwaysGreater;
      GRND := ground(X@);
    */


    WMinusCXToACC_IfACCLe0RetAlwGr_GrndToGRND,
    /*
      ACC := W - C*min|X@|;
      if ACC < 0 return AlwaysGreater;
      GRND := ground(X@);
    */
    
    ACCMinusXPlus1ToACC_IfACCLe0RetAlwGr_UpdateGRND,
    /*
      ACC := ACC - min|X@| + 1;
      if ACC < 0 return AlwaysGreater;
      GRND := GRND && ground(X@);
     */    
    
    ACCMinusCXPlusCToACC_IfACCLe0RetAlwGr_UpdateGRND,
    /*
      ACC := ACC - C*min|X@| + C;
      if ACC < 0 return AlwaysGreater;
      GRND := GRND && ground(X@);
     */


    IfXGrACCPlus1RetAlwGr_IfXLeACCPlus1RetFail_UpdateGRND_IfGRNDRetAlwEq_RetCanBeGrOrEq,
    /*
      if min|X@| > ACC + 1 return AlwaysGreater;
      if min|X@| < ACC + 1 return FailureToCompare;      
      GRND := GRND && ground(X@);
      if GRND return AlwaysEqual;
      return CanBeGreaterOrEqual;
    */   
    
    IfCXGrACCPlusCRetAlwGr_IfCXLeACCPlusCRetFail_UpdateGRND_IfGRNDRetAlwEq_RetCanBeGrOrEq,
    /*
      if C*min|X@| > ACC + C return AlwaysGreater;
      if C*min|X@| < ACC + C return FailureToCompare;      
      GRND := GRND && ground(X@);
      if GRND return AlwaysEqual;
      return CanBeGreaterOrEqual;
    */
    
    //----------------------------------------

    XPlusWToPOSINST_IfConstPOSINSAndPOSINSTLe0RetFail_IfConstPOSINSTGotoL,
    /*
      POSINST := |X@| + W;
      if constant(POSINST) && POSINST < 0 return FailureToCompare; 
      if constant(POSINST) goto L; 
    */

    CXPlusWToPOSINST_IfConstPOSINSAndPOSINSTLe0RetFail_IfConstPOSINSTGotoL,
    /*
      POSINST := C*|X@| + W;
      if constant(POSINST) && POSINST < 0 return FailureToCompare; 
      if constant(POSINST) goto L;
    */


    XPlusWToPOSINST,
    /*
      POSINST := |X@| + W;
    */



    CXPlusWToPOSINST,
    /*
      POSINST := C*|X@| + W;
    */


    POSINSTPlusXToPOSINST,
    /*
      POSINST := POSINST + |X@|;
    */
    
    POSINSTPlusCXToPOSINST,
    /*
      POSINST := POSINST + C*|X@|;
    */

    
    POSINSTPlusXToPOSINST_IfConstPOSINSTAndPOSINSTLe0RetFail_IfConstPOSINSTGotoL,
    /*
      POSINST := POSINST + |X@|;
      if constant(POSINST) && POSINST < 0 return FailureToCompare; 
      if constant(POSINST) goto L;
    */

    POSINSTPlusCXToPOSINST_IfConstPOSINSTAndPOSINSTLe0RetFail_IfConstPOSINSTGotoL,
    /*
      POSINST := POSINST + C*|X@|;
      if constant(POSINST) && POSINST < 0 return FailureToCompare; 
      if constant(POSINST) goto L;
    */

    XMinus1ToNEGINSTRetCompForGrOrEqPOSINSTvNEGINST,
    /*
      NEGINST := |X@| - 1;
      return compareForGreaterOrEqual(POSINST,NEGINST);
    */
    
    CXMinusCToNEGINSTRetCompForGrOrEqPOSINSTvNEGINST,
    /*
      NEGINST := C*|X@| - C;
      return compareForGreaterOrEqual(POSINST,NEGINST);
    */

    XMinus1ToNEGINST_IfCompForGrOrEqPOSINSTvNEGINSTEqFailRetFail,
    /*
      NEGINST := |X@| - 1;
      if compareForGreaterOrEqual(POSINST,NEGINST) == FailureToCompare return FailureToCompare;
    */

    CXMinusCToNEGINST_IfCompForGrOrEqPOSINSTvNEGINSTEqFailRetFail,
    /*
      NEGINST := C*|X@| - C;
      if compareForGreaterOrEqual(POSINST,NEGINST) == FailureToCompare return FailureToCompare;
    */

    NEGINSTPlusXMinus1ToNEGINST_IfCompForGrOrEqPOSINSTvNEGINSTEqFailRetFail,
    /*
      NEGINST := NEGINST + |X@| - 1;
      if compareForGreaterOrEqual(POSINST,NEGINST) == FailureToCompare return FailureToCompare;
    */
    
    NEGINSTPlusCXMinusCToNEGINST_IfCompForGrOrEqPOSINSTvNEGINSTEqFailRetFail,
    /*
      NEGINST := NEGINST + C*|X@| - C;
      if compareForGreaterOrEqual(POSINST,NEGINST) == FailureToCompare return FailureToCompare;
    */


    NEGINSTPlusXMinus1ToNEGINST_RetCompForGrOrEqPOSINSTvNEGINST,
    /*
      NEGINST := NEGINST + |X@| - 1;
      return compareForGreaterOrEqual(POSINST,NEGINST);
    */

    NEGINSTPlusCXMinusCToNEGINST_RetCompForGrOrEqPOSINSTvNEGINST,
    /*
      NEGINST := NEGINST + C*|X@| - C;
      return compareForGreaterOrEqual(POSINST,NEGINST);
    */


    IfNotGrndRetFail_IfXMinus1GrPOSINSTRetFail_IfXMinus1LePOSINSTRetAlwGr_RetAlwEq,
    /*
      ASSERT(constant(POSINST));
      ASSERT(POSINST >= 0);
      if !ground(X@) return FailureToCompare;
      if min|X@| - 1 > POSINST return FailureToCompare;
      if min|X@| - 1 < POSINST return AlwaysGreater;
      return AlwaysEqual;
    */

    IfNotGrndRetFail_IfCXMinusCGrPOSINSTRetFail_IfCXMinusCLePOSINSTRetAlwGr_RetAlwEq,
    /*
      ASSERT(constant(POSINST));
      ASSERT(POSINST >= 0);
      if !ground(X@) return FailureToCompare;
      if C*min|X@| - C > POSINST return FailureToCompare;
      if C*min|X@| - C < POSINST return AlwaysGreater;
      return AlwaysEqual;
    */


    IfNotGrndRetFail_POSINSTPlus1MinusXToACC_IfACCLe0RetFail,
    /*
      ASSERT(constant(POSINST));
      ASSERT(POSINST >= 0);
      if !ground(X@) return FailureToCompare;
      ACC := POSINST + 1 - min|X@|;      
      if ACC < 0 return FailureToCompare; 
    */

    IfNotGrndRetFail_POSINSTPlusCMinusCXToACC_IfACCLe0RetFail,
    /*
      ASSERT(constant(POSINST));
      ASSERT(POSINST >= 0);
      if !ground(X@) return FailureToCompare;
      ACC := POSINST + C - C*min|X@|;      
      if ACC < 0 return FailureToCompare; 
    */




    XPlusWToNEGINST_IfConstNEGINSTAndNEGINSTLe0RetAlwGr_IfConstNEGINSTGotoL,
    /*
      NEGINST := |X@| + W;
      if constant(NEGINST) && NEGINST < 0 return AlwaysGreater;
      if constant(NEGINST) goto L;
    */



    CXPlusWToNEGINST_IfConstNEGINSTAndNEGINSTLe0RetAlwGr_IfConstNEGINSTGotoL,
    /*
      NEGINST := C*|X@| + W;
      if constant(NEGINST) && NEGINST < 0 return AlwaysGreater;
      if constant(NEGINST) goto L;
    */

    XPlusWToNEGINST,
    /*
      NEGINST := |X@| + W;
    */
    
    CXPlusWToNEGINST,
    /*
      NEGINST := C*|X@| + W;
    */

    NEGINSTPlusXToNEGINST,
    /*
      NEGINST := NEGINST + |X@|;
    */

    NEGINSTPlusCXToNEGINST,
    /*
      NEGINST := NEGINST + C*|X@|;
    */
    
    NEGINSTPlusXToNEGINST_IfConstNEGINSTAndNEGINSTLe0RetAlwGr_IfConstNEGINSTGotoL,
    /*
      NEGINST := NEGINST + |X@|;
      if constant(NEGINST) && NEGINST < 0 return AlwaysGreater;
      if constant(NEGINST) goto L;
    */
        
    NEGINSTPlusCXToNEGINST_IfConstNEGINSTAndNEGINSTLe0RetAlwGr_IfConstNEGINSTGotoL,
    /*
      NEGINST := NEGINST + C*|X@|;
      if constant(NEGINST) && NEGINST < 0 return AlwaysGreater;
      if constant(NEGINST) goto L;
    */
    

    XMinus1ToPOSINST_IfCompForGrOrEqPOSINSTvNEGINSTEqAlwGrRetAlwGr,
    /*
      POSINST := |X@| - 1;
      if compareForGreaterOrEqual(POSINST,NEGINST) == AlwaysGreater return AlwaysGreater;
    */

    CXMinusCToPOSINST_IfCompForGrOrEqPOSINSTvNEGINSTEqAlwGrRetAlwGr,
    /*
      POSINST := C*|X@| - C;
      if compareForGreaterOrEqual(POSINST,NEGINST) == AlwaysGreater return AlwaysGreater;
    */

    POSINSTPlusXMinus1ToPOSINST_IfCompForGrOrEqPOSINSTvNEGINSTEqAlwGrRetAlwGr,
    /*
      POSINST := POSINST + |X@| - 1;
      if compareForGreaterOrEqual(POSINST,NEGINST) == AlwaysGreater return AlwaysGreater;
    */

    POSINSTPlusCXMinusCToPOSINST_IfCompForGrOrEqPOSINSTvNEGINSTEqAlwGrRetAlwGr,
    /*
      POSINST := POSINST + C*|X@| - C;
      if compareForGreaterOrEqual(POSINST,NEGINST) == AlwaysGreater return AlwaysGreater;
    */

    POSINSTPlusXMinus1ToPOSINST_RetCompForGrOrEqPOSINSTvNEGINST,
    /*
      POSINST := POSINST + |X@| - 1;
      return compareForGreaterOrEqual(POSINST,NEGINST);
    */

    POSINSTPlusCXMinusCToPOSINST_RetCompForGrOrEqPOSINSTvNEGINST,
    /*
      POSINST := POSINST + C*|X@| - C;
      return compareForGreaterOrEqual(POSINST,NEGINST);
    */



    NEGINSTPlus1MinusXToACC_IfACCLe0RetAlwGr_GrndToGRND,
    /*
      ASSERT(constant(NEGINST));
      ASSERT(NEGINST >= 0);
      ACC := NEGINST + 1 - min|X@|;
      if ACC < 0 return AlwaysGreater;
      GRND := ground(X@);
    */    



    NEGINSTPlusCMinusCXToACC_IfACCLe0RetAlwGr_GrndToGRND
    /*
      ASSERT(constant(NEGINST));
      ASSERT(NEGINST >= 0);
      ACC := NEGINST + C - C*min|X@|;
      if ACC < 0 return AlwaysGreater;
      GRND := ground(X@);
    */


  }; // enum Tag

 public:
  Command() {};
  ~Command() {};
  const Tag& tag() const { return _tag; };
  void setTag(const Tag& tag) 
    { 
      _tag = tag;
      DOP(_var = ULONG_MAX);
      DOP(_nativeVar = 0);
      DOP(_label = 0);
    };
  const TermWeightType& coefficient() const 
    {
      CALL("coefficient() const");
      ASSERT(requiresCoefficient());
      return _coefficient; 
    };
  void setCoefficient(const TermWeightType& coeff) 
    {
      CALL("setCoefficient(const TermWeightType& coeff)");
      ASSERT(requiresCoefficient());
      ASSERT(coeff > (TermWeightType)0);
      ASSERT(coeff != (TermWeightType)1);
      _coefficient = coeff; 
    };
  ulong var() const 
    {
      CALL("var() const");
      ASSERT(requiresVar());
      return _var; 
    };
  void setVar(ulong var) 
    {
      CALL("setVar(ulong var)");
      ASSERT(requiresVar());
      _var = var; 
    };
  const void* nativeVar() const
    {
      CALL("nativeVar() const");
      ASSERT(requiresVar());
      return _nativeVar;
    };
  void setNativeVar(const void* v)
    {
      CALL("setNativeVar(const void* v)");
      ASSERT(requiresVar());
      _nativeVar = v;
    };
  const TermWeightType& weight() const 
    {
      CALL("weight() const");
      ASSERT(requiresWeight());
      return _weight; 
    };
  void setWeight(const TermWeightType& w) 
    {
      CALL("setWeight(const TermWeightType& w)");
      ASSERT(requiresWeight());
      _weight = w; 
    };
  const Command* label() const 
    {
      CALL("label() const");
      ASSERT(requiresLabel());
      return _label; 
    };
  void setLabel(Command* l) 
    {
      CALL("setLabel(Command* l)");
      ASSERT(requiresLabel());
      ASSERT(l > this);
      _label = l; 
    };
  static bool isTerminal(const Tag& tag);
  bool isTerminal() const { return isTerminal(tag()); };

  bool requiresVar() const;
  bool requiresCoefficient() const;
  bool requiresWeight() const;
  bool requiresLabel() const;

#ifndef NO_DEBUG_VIS
  static void setCodeBase(const Command* b) { _codeBase = b; };
  ostream& outputShort(ostream& str) const;
  ostream& output(ostream& str) const 
  {
    return output(str,"");
  };
  ostream& output(ostream& str,const char* indent) const;
  static ostream& outputCode(ostream& str,const Command* code)
  {
    return outputCode(str,"",code);
  };
  static ostream& outputCode(ostream& str,const char* indent,const Command* code);
#endif    

 private:
  Tag _tag;
  TermWeightType _coefficient;
  ulong _var;
  const void* _nativeVar;
  TermWeightType _weight;
  Command* _label;
#ifndef NO_DEBUG_VIS
  static const Command* _codeBase;
#endif    
}; // class WeightPolynomial::SpecialisedComparison::Command


}; // namespace VK

//===================================================================
#undef DEBUG_NAMESPACE
#ifdef DEBUG_WEIGHT_POLYNOMIAL
 #define DEBUG_NAMESPACE "WeightPolynomial::SpecialisedComparison::Command"
#endif
#include "debugMacros.hpp"
//===================================================================
namespace VK
{

inline
bool WeightPolynomial::SpecialisedComparison::Command::isTerminal(const Tag& tag)
{
  CALL("isTerminal(const Tag& tag)");
  switch (tag)
    {
    case RetFailureToCompare:
    case RetAlwaysLess:
    case RetAlwaysEqual:
    case RetAlwaysGreater:
    case RetCanBeLessOrEqual:
    case RetCanBeGreaterOrEqual:
    case RetVolatileButNearlyAlwaysLess:
    case RetVolatileButNearlyAlwaysGreater:
    case RetAbsolutelyVolatile:
 
    //----------------------------------------

    case IfXGr1RetFail_IfGrndRetAlwEq_RetFail:

    //----------------------------------------

    case IfXGr1RetAlwGr_IfGrndRetAlwEq_RetCanBeGrOrEq:
    case IfXGr1RetAlwGr_UpdateGRND_IfGRNDRetAlwEq_RetCanBeGrOrEq:

    //----------------------------------------

    case IfNotGrndRetFail_IfXGrWRetFail_IfXLeWRetAlwGr_RetAlwEq:
    case IfNotGrndRetFail_IfCXGrWRetFail_IfCXLeWRetAlwGr_RetAlwEq:
    case IfNotGrndRetFail_ACCMinusXPlus1ToACC_IfACCLe0RetFail_IfACCEq0RetAlwEq_RetAlwGr:
    case IfNotGrndRetFail_ACCMinusCXPlusCToACC_IfACCLe0RetFail_IfACCEq0RetAlwEq_RetAlwGr:

    //----------------------------------------

    case IfXGrWRetAlwGr_IfXLeWRetFail_IfGrndRetAlwEq_RetCanBeGrOrEq:
    case IfCXGrWRetAlwGr_IfCXLeWRetFail_IfGrndRetAlwEq_RetCanBeGrOrEq:
    case IfXGrACCPlus1RetAlwGr_IfXLeACCPlus1RetFail_UpdateGRND_IfGRNDRetAlwEq_RetCanBeGrOrEq:
    case IfCXGrACCPlusCRetAlwGr_IfCXLeACCPlusCRetFail_UpdateGRND_IfGRNDRetAlwEq_RetCanBeGrOrEq:

    //----------------------------------------
    
    case XMinus1ToNEGINSTRetCompForGrOrEqPOSINSTvNEGINST:
    case CXMinusCToNEGINSTRetCompForGrOrEqPOSINSTvNEGINST:
    case NEGINSTPlusXMinus1ToNEGINST_RetCompForGrOrEqPOSINSTvNEGINST:
    case NEGINSTPlusCXMinusCToNEGINST_RetCompForGrOrEqPOSINSTvNEGINST:
    case IfNotGrndRetFail_IfXMinus1GrPOSINSTRetFail_IfXMinus1LePOSINSTRetAlwGr_RetAlwEq:
    case IfNotGrndRetFail_IfCXMinusCGrPOSINSTRetFail_IfCXMinusCLePOSINSTRetAlwGr_RetAlwEq:
    case POSINSTPlusXMinus1ToPOSINST_RetCompForGrOrEqPOSINSTvNEGINST:
    case POSINSTPlusCXMinusCToPOSINST_RetCompForGrOrEqPOSINSTvNEGINST:
      return true;
    default: return false;
    };
}; // bool WeightPolynomial::SpecialisedComparison::Command::isTerminal(const Tag& tag)




inline bool WeightPolynomial::SpecialisedComparison::Command::requiresVar() const
{
  CALL("requiresVar() const");
  switch (tag())
    {
    case RetFailureToCompare:
    case RetAlwaysLess:
    case RetAlwaysEqual:
    case RetAlwaysGreater:
    case RetCanBeLessOrEqual:
    case RetCanBeGreaterOrEqual:
    case RetVolatileButNearlyAlwaysLess:
    case RetVolatileButNearlyAlwaysGreater:
    case RetAbsolutelyVolatile:  
      return false;
    default: return true;
    };
}; // bool WeightPolynomial::SpecialisedComparison::Command::requiresVar() const

inline bool WeightPolynomial::SpecialisedComparison::Command::requiresCoefficient() const
{
  CALL("requiresCoefficient() const");
  switch (tag())
    {
    case IfNotGrndRetFail_IfCXGrWRetFail_IfCXLeWRetAlwGr_RetAlwEq:
    case IfNotGrndRetFail_WMinusCXToACC_IfACCLe0RetFail:
    case IfNotGrndRetFail_ACCMinusCXPlusCToACC_IfACCLe0RetFail:
    case IfNotGrndRetFail_ACCMinusCXPlusCToACC_IfACCLe0RetFail_IfACCEq0RetAlwEq_RetAlwGr:

    //----------------------------------------
    
    case IfCXGrWRetAlwGr_IfCXLeWRetFail_IfGrndRetAlwEq_RetCanBeGrOrEq:
    case WMinusCXToACC_IfACCLe0RetAlwGr_GrndToGRND:
    case ACCMinusCXPlusCToACC_IfACCLe0RetAlwGr_UpdateGRND:
    case IfCXGrACCPlusCRetAlwGr_IfCXLeACCPlusCRetFail_UpdateGRND_IfGRNDRetAlwEq_RetCanBeGrOrEq:

    //----------------------------------------


    case CXPlusWToPOSINST_IfConstPOSINSAndPOSINSTLe0RetFail_IfConstPOSINSTGotoL:
    case CXPlusWToPOSINST:
    case POSINSTPlusCXToPOSINST:
    case POSINSTPlusCXToPOSINST_IfConstPOSINSTAndPOSINSTLe0RetFail_IfConstPOSINSTGotoL:
    case CXMinusCToNEGINSTRetCompForGrOrEqPOSINSTvNEGINST:
    case CXMinusCToNEGINST_IfCompForGrOrEqPOSINSTvNEGINSTEqFailRetFail:
    case NEGINSTPlusCXMinusCToNEGINST_IfCompForGrOrEqPOSINSTvNEGINSTEqFailRetFail:
    case NEGINSTPlusCXMinusCToNEGINST_RetCompForGrOrEqPOSINSTvNEGINST:
    case IfNotGrndRetFail_IfCXMinusCGrPOSINSTRetFail_IfCXMinusCLePOSINSTRetAlwGr_RetAlwEq:
    case IfNotGrndRetFail_POSINSTPlusCMinusCXToACC_IfACCLe0RetFail:
    case CXPlusWToNEGINST_IfConstNEGINSTAndNEGINSTLe0RetAlwGr_IfConstNEGINSTGotoL:
    case CXPlusWToNEGINST:
    case NEGINSTPlusCXToNEGINST:
    case NEGINSTPlusCXToNEGINST_IfConstNEGINSTAndNEGINSTLe0RetAlwGr_IfConstNEGINSTGotoL:
    case CXMinusCToPOSINST_IfCompForGrOrEqPOSINSTvNEGINSTEqAlwGrRetAlwGr:
    case POSINSTPlusCXMinusCToPOSINST_IfCompForGrOrEqPOSINSTvNEGINSTEqAlwGrRetAlwGr:
    case POSINSTPlusCXMinusCToPOSINST_RetCompForGrOrEqPOSINSTvNEGINST:
    case NEGINSTPlusCMinusCXToACC_IfACCLe0RetAlwGr_GrndToGRND:
      return true;
      
    default: return false;
    };
}; // bool WeightPolynomial::SpecialisedComparison::Command::requiresCoefficient() const


inline bool WeightPolynomial::SpecialisedComparison::Command::requiresWeight() const
{
  CALL("requiresWeight() const");
  switch (tag())
    {
    case IfNotGrndRetFail_IfXGrWRetFail_IfXLeWRetAlwGr_RetAlwEq:
    case IfNotGrndRetFail_IfCXGrWRetFail_IfCXLeWRetAlwGr_RetAlwEq:
    case IfNotGrndRetFail_WMinusXToACC_IfACCLe0RetFail:
    case IfNotGrndRetFail_WMinusCXToACC_IfACCLe0RetFail:

    //----------------------------------------

    case IfXGrWRetAlwGr_IfXLeWRetFail_IfGrndRetAlwEq_RetCanBeGrOrEq:
    case IfCXGrWRetAlwGr_IfCXLeWRetFail_IfGrndRetAlwEq_RetCanBeGrOrEq:
    case WMinusXToACC_IfACCLe0RetAlwGr_GrndToGRND:
    case WMinusCXToACC_IfACCLe0RetAlwGr_GrndToGRND:

    //----------------------------------------

    case XPlusWToPOSINST_IfConstPOSINSAndPOSINSTLe0RetFail_IfConstPOSINSTGotoL:
    case CXPlusWToPOSINST_IfConstPOSINSAndPOSINSTLe0RetFail_IfConstPOSINSTGotoL:
    case XPlusWToPOSINST:
    case CXPlusWToPOSINST:
    case XPlusWToNEGINST_IfConstNEGINSTAndNEGINSTLe0RetAlwGr_IfConstNEGINSTGotoL:
    case CXPlusWToNEGINST_IfConstNEGINSTAndNEGINSTLe0RetAlwGr_IfConstNEGINSTGotoL:
    case XPlusWToNEGINST:
    case CXPlusWToNEGINST:

      return true;
    default: return false;
    };
}; // bool WeightPolynomial::SpecialisedComparison::Command::requiresWeight() const




inline bool WeightPolynomial::SpecialisedComparison::Command::requiresLabel() const
{
  CALL("requiresLabel() const");
  switch (tag())
    {
    case XPlusWToPOSINST_IfConstPOSINSAndPOSINSTLe0RetFail_IfConstPOSINSTGotoL:
    case CXPlusWToPOSINST_IfConstPOSINSAndPOSINSTLe0RetFail_IfConstPOSINSTGotoL:
    case POSINSTPlusXToPOSINST_IfConstPOSINSTAndPOSINSTLe0RetFail_IfConstPOSINSTGotoL:
    case POSINSTPlusCXToPOSINST_IfConstPOSINSTAndPOSINSTLe0RetFail_IfConstPOSINSTGotoL:
    case XPlusWToNEGINST_IfConstNEGINSTAndNEGINSTLe0RetAlwGr_IfConstNEGINSTGotoL:
    case CXPlusWToNEGINST_IfConstNEGINSTAndNEGINSTLe0RetAlwGr_IfConstNEGINSTGotoL:
    case NEGINSTPlusXToNEGINST_IfConstNEGINSTAndNEGINSTLe0RetAlwGr_IfConstNEGINSTGotoL:
    case NEGINSTPlusCXToNEGINST_IfConstNEGINSTAndNEGINSTLe0RetAlwGr_IfConstNEGINSTGotoL:
      return true;
    default: return false;
    };
}; // bool WeightPolynomial::SpecialisedComparison::Command::requiresLabel() const

}; // namespace VK

//===================================================================
#endif
