//
// File:         WeightPolynomialSpecialisedComparisonCommand.cpp
// Description:  Definition of 
//               class WeightPolynomial::SpecialisedComparison::Command
// Created:      May 03, 2002, Alexandre Riazanov, riazanov@cs.man.ac.uk
// Revised:      May 14, 2002, Alexandre Riazanov, riazanov@cs.man.ac.uk
//               Some weight comparison commands changed. 
//===================================================================
#include "WeightPolynomialSpecialisedComparisonCommand.hpp"
//===================================================================
using namespace VK;

#ifndef NO_DEBUG_VIS
const WeightPolynomial::SpecialisedComparison::Command* 
WeightPolynomial::SpecialisedComparison::Command::_codeBase = 0;


ostream& operator<<(ostream& str,const WeightPolynomial::SpecialisedComparison::Command::Tag& tag)
{
  switch (tag)
    {
      
    case WeightPolynomial::SpecialisedComparison::Command::RetFailureToCompare:
      return str << "RetFailureToCompare";
    case WeightPolynomial::SpecialisedComparison::Command::RetAlwaysLess:
      return str << "RetAlwaysLess";
    case WeightPolynomial::SpecialisedComparison::Command::RetAlwaysEqual:
      return str << "RetAlwaysEqual";
    case WeightPolynomial::SpecialisedComparison::Command::RetAlwaysGreater:
      return str << "RetAlwaysGreater";
    case WeightPolynomial::SpecialisedComparison::Command::RetCanBeLessOrEqual:
      return str << "RetCanBeLessOrEqual";
    case WeightPolynomial::SpecialisedComparison::Command::RetCanBeGreaterOrEqual:
      return str << "RetCanBeGreaterOrEqual";
    case WeightPolynomial::SpecialisedComparison::Command::RetVolatileButNearlyAlwaysLess:
      return str << "RetVolatileButNearlyAlwaysLess";
    case WeightPolynomial::SpecialisedComparison::Command::RetVolatileButNearlyAlwaysGreater:
      return str << "RetVolatileButNearlyAlwaysGreater";
    case WeightPolynomial::SpecialisedComparison::Command::RetAbsolutelyVolatile:
      return str << "RetAbsolutelyVolatile";     
    

      //----------------------------------------

    case WeightPolynomial::SpecialisedComparison::Command::IfXGr1RetFail_IfGrndRetAlwEq_RetFail:
      return str << "IfXGr1RetFail_IfGrndRetAlwEq_RetFail";
    case WeightPolynomial::SpecialisedComparison::Command::IfXGr1RetFail_IfNotGrndRetFail:
      return str << "IfXGr1RetFail_IfNotGrndRetFail";

      //----------------------------------------
    case WeightPolynomial::SpecialisedComparison::Command::IfXGr1RetAlwGr_IfGrndRetAlwEq_RetCanBeGrOrEq:
      return str << "IfXGr1RetAlwGr_IfGrndRetAlwEq_RetCanBeGrOrEq";
    case WeightPolynomial::SpecialisedComparison::Command::IfXGr1RetAlwGr_GrndToGRND:
      return str << "IfXGr1RetAlwGr_GrndToGRND";
    case WeightPolynomial::SpecialisedComparison::Command::IfXGr1RetAlwGr_UpdateGRND:
      return str << "fXGr1RetAlwGr_UpdateGRND";
    case WeightPolynomial::SpecialisedComparison::Command::IfXGr1RetAlwGr_UpdateGRND_IfGRNDRetAlwEq_RetCanBeGrOrEq:
      return str << "IfXGr1RetAlwGr_UpdateGRND_IfGRNDRetAlwEq_RetCanBeGrOrEq";

      //----------------------------------------


    case WeightPolynomial::SpecialisedComparison::Command::IfNotGrndRetFail_IfXGrWRetFail_IfXLeWRetAlwGr_RetAlwEq:
      return str << "IfNotGrndRetFail_IfXGrWRetFail_IfXLeWRetAlwGr_RetAlwEq";
    case WeightPolynomial::SpecialisedComparison::Command::IfNotGrndRetFail_IfCXGrWRetFail_IfCXLeWRetAlwGr_RetAlwEq:
      return str << "IfNotGrndRetFail_IfCXGrWRetFail_IfCXLeWRetAlwGr_RetAlwEq";
    case WeightPolynomial::SpecialisedComparison::Command::IfNotGrndRetFail_WMinusXToACC_IfACCLe0RetFail:
      return str << "IfNotGrndRetFail_WMinusXToACC_IfACCLe0RetFail";
    case WeightPolynomial::SpecialisedComparison::Command::IfNotGrndRetFail_WMinusCXToACC_IfACCLe0RetFail:
      return str << "IfNotGrndRetFail_WMinusCXToACC_IfACCLe0RetFail";
    case WeightPolynomial::SpecialisedComparison::Command::IfNotGrndRetFail_ACCMinusXPlus1ToACC_IfACCLe0RetFail:
      return str << "IfNotGrndRetFail_ACCMinusXPlus1ToACC_IfACCLe0RetFail";
    case WeightPolynomial::SpecialisedComparison::Command::IfNotGrndRetFail_ACCMinusCXPlusCToACC_IfACCLe0RetFail:
      return str << "IfNotGrndRetFail_ACCMinusCXPlusCToACC_IfACCLe0RetFail";
    case WeightPolynomial::SpecialisedComparison::Command::IfNotGrndRetFail_ACCMinusXPlus1ToACC_IfACCLe0RetFail_IfACCEq0RetAlwEq_RetAlwGr:
      return str << "IfNotGrndRetFail_ACCMinusXPlus1ToACC_IfACCLe0RetFail_IfACCEq0RetAlwEq_RetAlwGr";
    case WeightPolynomial::SpecialisedComparison::Command::IfNotGrndRetFail_ACCMinusCXPlusCToACC_IfACCLe0RetFail_IfACCEq0RetAlwEq_RetAlwGr:
      return str << "IfNotGrndRetFail_ACCMinusCXPlusCToACC_IfACCLe0RetFail_IfACCEq0RetAlwEq_RetAlwGr";

      //----------------------------------------



    case WeightPolynomial::SpecialisedComparison::Command::IfXGrWRetAlwGr_IfXLeWRetFail_IfGrndRetAlwEq_RetCanBeGrOrEq:
      return str << "IfXGrWRetAlwGr_IfXLeWRetFail_IfGrndRetAlwEq_RetCanBeGrOrEq";
    case WeightPolynomial::SpecialisedComparison::Command::IfCXGrWRetAlwGr_IfCXLeWRetFail_IfGrndRetAlwEq_RetCanBeGrOrEq:
      return str << "IfCXGrWRetAlwGr_IfCXLeWRetFail_IfGrndRetAlwEq_RetCanBeGrOrEq";
    case WeightPolynomial::SpecialisedComparison::Command::WMinusXToACC_IfACCLe0RetAlwGr_GrndToGRND:
      return str << "WMinusXToACC_IfACCLe0RetAlwGr_GrndToGRND";
    case WeightPolynomial::SpecialisedComparison::Command::WMinusCXToACC_IfACCLe0RetAlwGr_GrndToGRND:
      return str << "WMinusCXToACC_IfACCLe0RetAlwGr_GrndToGRND";
    case WeightPolynomial::SpecialisedComparison::Command::ACCMinusXPlus1ToACC_IfACCLe0RetAlwGr_UpdateGRND:
      return str << "ACCMinusXPlus1ToACC_IfACCLe0RetAlwGr_UpdateGRND";
    case WeightPolynomial::SpecialisedComparison::Command::ACCMinusCXPlusCToACC_IfACCLe0RetAlwGr_UpdateGRND:
      return str << "ACCMinusCXPlusCToACC_IfACCLe0RetAlwGr_UpdateGRND";
    case WeightPolynomial::SpecialisedComparison::Command::IfXGrACCPlus1RetAlwGr_IfXLeACCPlus1RetFail_UpdateGRND_IfGRNDRetAlwEq_RetCanBeGrOrEq:
      return str << "IfXGrACCPlus1RetAlwGr_IfXLeACCPlus1RetFail_UpdateGRND_IfGRNDRetAlwEq_RetCanBeGrOrEq";
    case WeightPolynomial::SpecialisedComparison::Command::IfCXGrACCPlusCRetAlwGr_IfCXLeACCPlusCRetFail_UpdateGRND_IfGRNDRetAlwEq_RetCanBeGrOrEq:
      return str << "IfCXGrACCPlusCRetAlwGr_IfCXLeACCPlusCRetFail_UpdateGRND_IfGRNDRetAlwEq_RetCanBeGrOrEq";

      //----------------------------------------

    case WeightPolynomial::SpecialisedComparison::Command::XPlusWToPOSINST_IfConstPOSINSAndPOSINSTLe0RetFail_IfConstPOSINSTGotoL:
      return str << "XPlusWToPOSINST_IfConstPOSINSAndPOSINSTLe0RetFail_IfConstPOSINSTGotoL";
    case WeightPolynomial::SpecialisedComparison::Command::CXPlusWToPOSINST_IfConstPOSINSAndPOSINSTLe0RetFail_IfConstPOSINSTGotoL:
      return str << "CXPlusWToPOSINST_IfConstPOSINSAndPOSINSTLe0RetFail_IfConstPOSINSTGotoL";
    case WeightPolynomial::SpecialisedComparison::Command::XPlusWToPOSINST:
      return str << "XPlusWToPOSINST";
    case WeightPolynomial::SpecialisedComparison::Command::CXPlusWToPOSINST:
      return str << "CXPlusWToPOSINST";
    case WeightPolynomial::SpecialisedComparison::Command::POSINSTPlusXToPOSINST:
      return str << "POSINSTPlusXToPOSINST";
    case WeightPolynomial::SpecialisedComparison::Command::POSINSTPlusCXToPOSINST:
      return str << "POSINSTPlusCXToPOSINST";
    case WeightPolynomial::SpecialisedComparison::Command::POSINSTPlusXToPOSINST_IfConstPOSINSTAndPOSINSTLe0RetFail_IfConstPOSINSTGotoL:
      return str << "POSINSTPlusXToPOSINST_IfConstPOSINSTAndPOSINSTLe0RetFail_IfConstPOSINSTGotoL";
    case WeightPolynomial::SpecialisedComparison::Command::POSINSTPlusCXToPOSINST_IfConstPOSINSTAndPOSINSTLe0RetFail_IfConstPOSINSTGotoL:
      return str << "POSINSTPlusCXToPOSINST_IfConstPOSINSTAndPOSINSTLe0RetFail_IfConstPOSINSTGotoL";
    case WeightPolynomial::SpecialisedComparison::Command::XMinus1ToNEGINSTRetCompForGrOrEqPOSINSTvNEGINST:
      return str << "XMinus1ToNEGINSTRetCompForGrOrEqPOSINSTvNEGINST";
    case WeightPolynomial::SpecialisedComparison::Command::CXMinusCToNEGINSTRetCompForGrOrEqPOSINSTvNEGINST:
      return str << "CXMinusCToNEGINSTRetCompForGrOrEqPOSINSTvNEGINST";
    case WeightPolynomial::SpecialisedComparison::Command::XMinus1ToNEGINST_IfCompForGrOrEqPOSINSTvNEGINSTEqFailRetFail:
      return str << "XMinus1ToNEGINST_IfCompForGrOrEqPOSINSTvNEGINSTEqFailRetFail";
    case WeightPolynomial::SpecialisedComparison::Command::CXMinusCToNEGINST_IfCompForGrOrEqPOSINSTvNEGINSTEqFailRetFail:
      return str << "CXMinusCToNEGINST_IfCompForGrOrEqPOSINSTvNEGINSTEqFailRetFail";
    case WeightPolynomial::SpecialisedComparison::Command::NEGINSTPlusXMinus1ToNEGINST_IfCompForGrOrEqPOSINSTvNEGINSTEqFailRetFail:
      return str << "NEGINSTPlusXMinus1ToNEGINST_IfCompForGrOrEqPOSINSTvNEGINSTEqFailRetFail";
    case WeightPolynomial::SpecialisedComparison::Command::NEGINSTPlusCXMinusCToNEGINST_IfCompForGrOrEqPOSINSTvNEGINSTEqFailRetFail:
      return str << "NEGINSTPlusCXMinusCToNEGINST_IfCompForGrOrEqPOSINSTvNEGINSTEqFailRetFail";
    case WeightPolynomial::SpecialisedComparison::Command::NEGINSTPlusXMinus1ToNEGINST_RetCompForGrOrEqPOSINSTvNEGINST:
      return str << "NEGINSTPlusXMinus1ToNEGINST_RetCompForGrOrEqPOSINSTvNEGINST";
    case WeightPolynomial::SpecialisedComparison::Command::NEGINSTPlusCXMinusCToNEGINST_RetCompForGrOrEqPOSINSTvNEGINST:
      return str << "NEGINSTPlusCXMinusCToNEGINST_RetCompForGrOrEqPOSINSTvNEGINST";
    case WeightPolynomial::SpecialisedComparison::Command::IfNotGrndRetFail_IfXMinus1GrPOSINSTRetFail_IfXMinus1LePOSINSTRetAlwGr_RetAlwEq:
      return str << "IfNotGrndRetFail_IfXMinus1GrPOSINSTRetFail_IfXMinus1LePOSINSTRetAlwGr_RetAlwEq";
    case WeightPolynomial::SpecialisedComparison::Command::IfNotGrndRetFail_IfCXMinusCGrPOSINSTRetFail_IfCXMinusCLePOSINSTRetAlwGr_RetAlwEq:
      return str << "IfNotGrndRetFail_IfCXPlusCGrPOSINSTRetFail_IfCXPlusCLePOSINSTRetAlwGr_RetAlwE";
    case WeightPolynomial::SpecialisedComparison::Command::IfNotGrndRetFail_POSINSTPlus1MinusXToACC_IfACCLe0RetFail:
      return str << "IfNotGrndRetFail_POSINSTPlus1MinusXToACC_IfACCLe0RetFail";
    case WeightPolynomial::SpecialisedComparison::Command::IfNotGrndRetFail_POSINSTPlusCMinusCXToACC_IfACCLe0RetFail:
      return str << "IfNotGrndRetFail_POSINSTPlusCMinusCXToACC_IfACCLe0RetFail";
    case WeightPolynomial::SpecialisedComparison::Command::XPlusWToNEGINST_IfConstNEGINSTAndNEGINSTLe0RetAlwGr_IfConstNEGINSTGotoL:
      return str << "XPlusWToNEGINST_IfConstNEGINSTAndNEGINSTLe0RetAlwGr_IfConstNEGINSTGotoL";
    case WeightPolynomial::SpecialisedComparison::Command::CXPlusWToNEGINST_IfConstNEGINSTAndNEGINSTLe0RetAlwGr_IfConstNEGINSTGotoL:
      return str << "CXPlusWToNEGINST_IfConstNEGINSTAndNEGINSTLe0RetAlwGr_IfConstNEGINSTGotoL";
    case WeightPolynomial::SpecialisedComparison::Command::XPlusWToNEGINST:
      return str << "XPlusWToNEGINST";
    case WeightPolynomial::SpecialisedComparison::Command::CXPlusWToNEGINST:
      return str << "CXPlusWToNEGINST";
    case WeightPolynomial::SpecialisedComparison::Command::NEGINSTPlusXToNEGINST:
      return str << "NEGINSTPlusXToNEGINST";
    case WeightPolynomial::SpecialisedComparison::Command::NEGINSTPlusCXToNEGINST:
      return str << "NEGINSTPlusCXToNEGINST";
    case WeightPolynomial::SpecialisedComparison::Command::NEGINSTPlusXToNEGINST_IfConstNEGINSTAndNEGINSTLe0RetAlwGr_IfConstNEGINSTGotoL:
      return str << "NEGINSTPlusXToNEGINST_IfConstNEGINSTAndNEGINSTLe0RetAlwGr_IfConstNEGINSTGotoL";
    case WeightPolynomial::SpecialisedComparison::Command::NEGINSTPlusCXToNEGINST_IfConstNEGINSTAndNEGINSTLe0RetAlwGr_IfConstNEGINSTGotoL:
      return str << "NEGINSTPlusCXToNEGINST_IfConstNEGINSTAndNEGINSTLe0RetAlwGr_IfConstNEGINSTGoto";
    case WeightPolynomial::SpecialisedComparison::Command::XMinus1ToPOSINST_IfCompForGrOrEqPOSINSTvNEGINSTEqAlwGrRetAlwGr:
      return str << "XMinus1ToPOSINST_IfCompForGrOrEqPOSINSTvNEGINSTEqAlwGrRetAlwGr";
    case WeightPolynomial::SpecialisedComparison::Command::CXMinusCToPOSINST_IfCompForGrOrEqPOSINSTvNEGINSTEqAlwGrRetAlwGr:
      return str << "CXMinusCToPOSINST_IfCompForGrOrEqPOSINSTvNEGINSTEqAlwGrRetAlwGr";
    case WeightPolynomial::SpecialisedComparison::Command::POSINSTPlusXMinus1ToPOSINST_IfCompForGrOrEqPOSINSTvNEGINSTEqAlwGrRetAlwGr:
      return str << "POSINSTPlusXMinus1ToPOSINST_IfCompForGrOrEqPOSINSTvNEGINSTEqAlwGrRetAlwGr";
    case WeightPolynomial::SpecialisedComparison::Command::POSINSTPlusCXMinusCToPOSINST_IfCompForGrOrEqPOSINSTvNEGINSTEqAlwGrRetAlwGr:
      return str << "POSINSTPlusCXMinusCToPOSINST_IfCompForGrOrEqPOSINSTvNEGINSTEqAlwGrRetAlwGr";
    case WeightPolynomial::SpecialisedComparison::Command::POSINSTPlusXMinus1ToPOSINST_RetCompForGrOrEqPOSINSTvNEGINST:
      return str << "POSINSTPlusXMinus1ToPOSINST_RetCompForGrOrEqPOSINSTvNEGINST";
    case WeightPolynomial::SpecialisedComparison::Command::POSINSTPlusCXMinusCToPOSINST_RetCompForGrOrEqPOSINSTvNEGINST:
      return str << "POSINSTPlusCXPlusCToPOSINST_RetCompForGrOrEqPOSINSTvNEGINS";
    case WeightPolynomial::SpecialisedComparison::Command::NEGINSTPlus1MinusXToACC_IfACCLe0RetAlwGr_GrndToGRND:
      return str << "NEGINSTPlus1MinusXToACC_IfACCLe0RetAlwGr_GrndToGRND";
    case WeightPolynomial::SpecialisedComparison::Command::NEGINSTPlusCMinusCXToACC_IfACCLe0RetAlwGr_GrndToGRND:
      return str << "NEGINSTPlusCMinusCXToACC_IfACCLe0RetAlwGr_GrndToGRND";

    default: return str << "?????????";
    };
}; // ostream& operator<<(ostream& str,const WeightPolynomial::SpecialisedComparison::Command::Tag& tag)



ostream& 
WeightPolynomial::SpecialisedComparison::Command::output(ostream& str,const char* indent) const
{
  switch (tag())
    {
    case RetFailureToCompare: return outputShort(str << indent) << "\n";
    case RetAlwaysLess: return outputShort(str << indent) << "\n"; 
    case RetAlwaysEqual: return outputShort(str << indent) << "\n";
    case RetAlwaysGreater: return outputShort(str << indent) << "\n";
    case RetCanBeLessOrEqual: return outputShort(str << indent) << "\n"; 
    case RetCanBeGreaterOrEqual: return outputShort(str << indent) << "\n"; 
    case RetVolatileButNearlyAlwaysLess: return outputShort(str << indent) << "\n";
    case RetVolatileButNearlyAlwaysGreater: return outputShort(str << indent) << "\n"; 
    case RetAbsolutelyVolatile: return outputShort(str << indent) << "\n";     
   

      //----------------------------------------

    case IfXGr1RetFail_IfGrndRetAlwEq_RetFail:
      return outputShort(str << indent) << "\n" << indent << "  {\n" << indent   
			      << "     if min|X" << var() << "@| > 1 return FailureToCompare\n" << indent 
			      << "     if ground(X" << var() << "@) return AlwaysEqual\n" << indent 
			      << "     return FailureToCompare\n" << indent << "  }\n";
     

    case IfXGr1RetFail_IfNotGrndRetFail:
      return outputShort(str << indent) << "\n" << indent << "  {\n" << indent   
			      << "     if min|X" << var() << "@| > 1 return FailureToCompare\n" << indent 
			      << "     if !ground(X" << var() << "@) return FailureToCompare\n" << indent << "  }\n";
    



      //----------------------------------------



    case IfXGr1RetAlwGr_IfGrndRetAlwEq_RetCanBeGrOrEq:
      return outputShort(str << indent) << "\n" << indent << "  {\n" << indent   
			      << "     if min|X" << var() << "@| > 1 return AlwaysGreater\n" << indent 
			      << "     if ground(X" << var() << "@) return AlwaysEqual\n" << indent 
			      << "     return CanBeGreaterOrEqual\n" << indent << "  }\n";
     

    case IfXGr1RetAlwGr_GrndToGRND:
      return outputShort(str << indent) << "\n" << indent << "  {\n" << indent   
			      << "     if min|X" << var() << "@| > 1 return AlwaysGreater\n" << indent 
			      << "     GRND := ground(X" << var() << "@)\n" << indent << "  }\n";
     

    case IfXGr1RetAlwGr_UpdateGRND:
      return outputShort(str << indent) << "\n" << indent << "  {\n" << indent   
			      << "     if min|X" << var() << "@| > 1 return AlwaysGreater\n" << indent 
			      << "     GRND := GRND && ground(X" << var() << "@)\n" << indent << "  }\n";
    

    case IfXGr1RetAlwGr_UpdateGRND_IfGRNDRetAlwEq_RetCanBeGrOrEq:
      return outputShort(str << indent) << "\n" << indent << "  {\n" << indent   
			      << "     if min|X" << var() << "@| > 1 return AlwaysGreater\n" << indent 
			      << "     GRND := GRND && ground(X" << var() << "@)\n" << indent 
			      << "     if GRND return AlwaysEqual\n" << indent 
			      << "     return CanBeGreaterOrEqual\n" << indent << "  }\n";
    



      //----------------------------------------


    case IfNotGrndRetFail_IfXGrWRetFail_IfXLeWRetAlwGr_RetAlwEq:
      return outputShort(str << indent) << "\n" << indent << "  {\n" << indent   
			      << "     if !ground(X" << var() << "@) return FailureToCompare\n" << indent 
			      << "     if min|X" << var() << "@| > " << weight() << " return FailureToCompare\n" << indent 
			      << "     if min|X" << var() << "@| < " << weight() << " return AlwaysGreater\n" << indent 
			      << "     return AlwaysEqual\n" << indent << "  }\n";
    


    case IfNotGrndRetFail_IfCXGrWRetFail_IfCXLeWRetAlwGr_RetAlwEq:
      return outputShort(str << indent) << "\n" << indent << "  {\n" << indent   
			      << "     if !ground(X" << var() << "@) return FailureToCompare\n" << indent 
			      << "     if " << coefficient() << "*min|X" << var() << "@| > " << weight() << " return FailureToCompare\n" << indent 
			      << "     if " << coefficient() << "*min|X" << var() << "@| < " << weight() << " return AlwaysGreater\n" << indent 
			      << "     return AlwaysEqual\n" << indent << "  }\n";
    



    case IfNotGrndRetFail_WMinusXToACC_IfACCLe0RetFail:
      return outputShort(str << indent) << "\n" << indent << "  {\n" << indent   
			      << "     if !ground(X" << var() << "@) return FailureToCompare\n" << indent 
			      << "     ACC := " << weight() << " - min|X" << var() << "@|\n" << indent 
			      << "     if ACC < 0 return FailureToCompare\n" << indent << "  }\n";     
    



    case IfNotGrndRetFail_WMinusCXToACC_IfACCLe0RetFail:
      return outputShort(str << indent) << "\n" << indent << "  {\n" << indent   
			      << "     if !ground(X" << var() << "@) return FailureToCompare\n" << indent 
			      << "     ACC := " << weight() << " - " << coefficient() << "*min|X" << var() << "@|\n" << indent 
			      << "     if ACC < 0 return FailureToCompare\n" << indent << "  }\n";      
    


    case IfNotGrndRetFail_ACCMinusXPlus1ToACC_IfACCLe0RetFail:
      return outputShort(str << indent) << "\n" << indent << "  {\n" << indent   
			      << "     if !ground(X" << var() << "@) return FailureToCompare\n" << indent 
			      << "     ACC := ACC - min|X" << var() << "@| + 1\n" << indent 
			      << "     if ACC < 0 return FailureToCompare\n" << indent << "  }\n";
        
    

    case IfNotGrndRetFail_ACCMinusCXPlusCToACC_IfACCLe0RetFail:
      return outputShort(str << indent) << "\n" << indent << "  {\n" << indent   
			      << "     if !ground(X" << var() << "@) return FailureToCompare\n" << indent 
			      << "     ACC := ACC - " << coefficient() << "*min|X" << var() << "@| + " << coefficient() << "\n" << indent 
			      << "     if ACC < 0 return FailureToCompare\n" << indent << "  }\n"; 
    


    case IfNotGrndRetFail_ACCMinusXPlus1ToACC_IfACCLe0RetFail_IfACCEq0RetAlwEq_RetAlwGr:
      return outputShort(str << indent) << "\n" << indent << "  {\n" << indent   
			      << "     if !ground(X" << var() << "@) return FailureToCompare\n" << indent 
			      << "     ACC := ACC - min|X" << var() << "@| + 1\n" << indent 
			      << "     if ACC < 0 return FailureToCompare\n" << indent  
			      << "     if ACC = 0 return AlwaysEqual\n" << indent 
			      << "     return AlwaysGreater\n" << indent << "  }\n";
        


    case IfNotGrndRetFail_ACCMinusCXPlusCToACC_IfACCLe0RetFail_IfACCEq0RetAlwEq_RetAlwGr:
      return outputShort(str << indent) << "\n" << indent << "  {\n" << indent   
			      << "     if !ground(X" << var() << "@) return FailureToCompare\n" << indent 
			      << "     ACC := ACC - " << coefficient() << "*min|X" << var() << "@| + " << coefficient() << "\n" << indent 
			      << "     if ACC < 0 return FailureToCompare\n" << indent  
			      << "     if ACC = 0 return AlwaysEqual\n" << indent 
			      << "     return AlwaysGreater\n" << indent << "  }\n";
    


      //----------------------------------------



    case IfXGrWRetAlwGr_IfXLeWRetFail_IfGrndRetAlwEq_RetCanBeGrOrEq:
      return outputShort(str << indent) << "\n" << indent << "  {\n" << indent   
	                      << "     ASSERT(" << weight() << " > 0)\n" << indent 
			      << "     if min|X" << var() << "@| > " << weight() << " return AlwaysGreater\n" << indent 
			      << "     if min|X" << var() << "@| < " << weight() << " return FailureToCompare\n" << indent 
			      << "     if ground(X" << var() << "@) return AlwaysEqual\n" << indent 
			      << "     return CanBeGreaterOrEqual\n" << indent << "  }\n";
    
    
    case IfCXGrWRetAlwGr_IfCXLeWRetFail_IfGrndRetAlwEq_RetCanBeGrOrEq:
      return outputShort(str << indent) << "\n" << indent << "  {\n" << indent   
	                      << "     ASSERT(" << weight() << " > 0)\n" << indent 
			      << "     if " << coefficient() << "*min|X" << var() << "@| > " << weight() << " return AlwaysGreater\n" << indent 
			      << "     if " << coefficient() << "*min|X" << var() << "@| < " << weight() << " return FailureToCompare\n" << indent 
			      << "     if ground(X" << var() << "@) return AlwaysEqual\n" << indent 
			      << "     return CanBeGreaterOrEqual\n" << indent << "  }\n";
    


    case WMinusXToACC_IfACCLe0RetAlwGr_GrndToGRND:
      return outputShort(str << indent) << "\n" << indent << "  {\n" << indent   
			      << "     ACC := " << weight() << " - min|X" << var() << "@|\n" << indent 
			      << "     if ACC < 0 return AlwaysGreater\n" << indent 
			      << "     GRND := ground(X" << var() << "@)\n" << indent << "  }\n";
    


    case WMinusCXToACC_IfACCLe0RetAlwGr_GrndToGRND:
      return outputShort(str << indent) << "\n" << indent << "  {\n" << indent   
			      << "     ACC := " << weight() << " - " << coefficient() << "*min|X" << var() << "@|\n" << indent 
			      << "     if ACC < 0 return AlwaysGreater\n" << indent 
			      << "     GRND := ground(X" << var() << "@)\n" << indent << "  }\n";
    
    
    case ACCMinusXPlus1ToACC_IfACCLe0RetAlwGr_UpdateGRND:
      return outputShort(str << indent) << "\n" << indent << "  {\n" << indent   
			      << "     ACC := ACC - min|X" << var() << "@| + 1\n" << indent 
			      << "     if ACC < 0 return AlwaysGreater\n" << indent 
			      << "     GRND := GRND && ground(X" << var() << "@)\n" << indent << "  }\n";
         
    
    case ACCMinusCXPlusCToACC_IfACCLe0RetAlwGr_UpdateGRND:
      return outputShort(str << indent) << "\n" << indent << "  {\n" << indent   
			      << "     ACC := ACC - " << coefficient() << "*min|X" << var() << "@| + " << coefficient() << "\n" << indent 
			      << "     if ACC < 0 return AlwaysGreater\n" << indent 
			      << "     GRND := GRND && ground(X" << var() << "@)\n" << indent << "  }\n";
     


    case IfXGrACCPlus1RetAlwGr_IfXLeACCPlus1RetFail_UpdateGRND_IfGRNDRetAlwEq_RetCanBeGrOrEq:
      return outputShort(str << indent) << "\n" << indent << "  {\n" << indent   
			      << "     if min|X" << var() << "@| > ACC + 1 return AlwaysGreater\n" << indent 
			      << "     if min|X" << var() << "@| < ACC + 1 return FailureToCompare\n" << indent       
			      << "     GRND := GRND && ground(X" << var() << "@)\n" << indent 
			      << "     if GRND return AlwaysEqual\n" << indent 
			      << "     return CanBeGreaterOrEqual\n" << indent << "  }\n";
       
    
    case IfCXGrACCPlusCRetAlwGr_IfCXLeACCPlusCRetFail_UpdateGRND_IfGRNDRetAlwEq_RetCanBeGrOrEq:
      return outputShort(str << indent) << "\n" << indent << "  {\n" << indent   
			      << "     if " << coefficient() << "*min|X" << var() << "@| > ACC + " << coefficient() << " return AlwaysGreater\n" << indent 
			      << "     if " << coefficient() << "*min|X" << var() << "@| < ACC + " << coefficient() << " return FailureToCompare\n" << indent       
			      << "     GRND := GRND && ground(X" << var() << "@)\n" << indent 
			      << "     if GRND return AlwaysEqual\n" << indent 
			      << "     return CanBeGreaterOrEqual\n" << indent << "  }\n";
    
    
      //----------------------------------------

    case XPlusWToPOSINST_IfConstPOSINSAndPOSINSTLe0RetFail_IfConstPOSINSTGotoL:
      return outputShort(str << indent) << "\n" << indent << "  {\n" << indent   
			      << "     POSINST := |X" << var() << "@| + " << weight() << "\n" << indent 
			      << "     if constant(POSINST) && POSINST < 0 return FailureToCompare\n" << indent  
			      << "     if constant(POSINST) goto [" << label() - _codeBase << "]\n" << indent << "  }\n"; 
    

    

    case CXPlusWToPOSINST_IfConstPOSINSAndPOSINSTLe0RetFail_IfConstPOSINSTGotoL:
      return outputShort(str << indent) << "\n" << indent << "  {\n" << indent   
			      << "     POSINST := " << coefficient() << "*|X" << var() << "@| + " << weight() << "\n" << indent 
			      << "     if constant(POSINST) && POSINST < 0 return FailureToCompare\n" << indent  
			      << "     if constant(POSINST) goto [" << label() - _codeBase << "]\n" << indent << "  }\n";
    


    case XPlusWToPOSINST:
      return outputShort(str << indent) << "\n" << indent << "  {\n" << indent   
			      << "     POSINST := |X" << var() << "@| + " << weight() << "\n" << indent << "  }\n";
    

    
    case CXPlusWToPOSINST:
      return outputShort(str << indent) << "\n" << indent << "  {\n" << indent   
			      << "     POSINST := " << coefficient() << "*|X" << var() << "@| + " << weight() << "\n" << indent << "  }\n";
    

    case POSINSTPlusXToPOSINST:
      return outputShort(str << indent) << "\n" << indent << "  {\n" << indent   
			      << "     POSINST := POSINST + |X" << var() << "@|\n" << indent << "  }\n";
    
    
    case POSINSTPlusCXToPOSINST:
      return outputShort(str << indent) << "\n" << indent << "  {\n" << indent   
			      << "     POSINST := POSINST + " << coefficient() << "*|X" << var() << "@|\n" << indent << "  }\n";
    

    
    case POSINSTPlusXToPOSINST_IfConstPOSINSTAndPOSINSTLe0RetFail_IfConstPOSINSTGotoL:
      return outputShort(str << indent) << "\n" << indent << "  {\n" << indent   
			      << "     POSINST := POSINST + |X" << var() << "@|\n" << indent 
			      << "     if constant(POSINST) && POSINST < 0 return FailureToCompare\n" << indent  
			      << "     if constant(POSINST) goto [" << label() - _codeBase << "]\n" << indent << "  }\n";
    

    case POSINSTPlusCXToPOSINST_IfConstPOSINSTAndPOSINSTLe0RetFail_IfConstPOSINSTGotoL:
      return outputShort(str << indent) << "\n" << indent << "  {\n" << indent   
			      << "     POSINST := POSINST + " << coefficient() << "*|X" << var() << "@|\n" << indent 
			      << "     if constant(POSINST) && POSINST < 0 return FailureToCompare\n" << indent  
			      << "     if constant(POSINST) goto [" << label() - _codeBase << "]\n" << indent << "  }\n";
    


    case XMinus1ToNEGINSTRetCompForGrOrEqPOSINSTvNEGINST:
      return outputShort(str << indent) << "\n" << indent << "  {\n" << indent   
			      << "     NEGINST := |X" << var() << "@| - 1\n" << indent 
			      << "     return compareForGreaterOrEqual(POSINST,NEGINST)\n" << indent << "  }\n";
    
    
    case CXMinusCToNEGINSTRetCompForGrOrEqPOSINSTvNEGINST:
      return outputShort(str << indent) << "\n" << indent << "  {\n" << indent   
			      << "     NEGINST := " << coefficient() << "*|X" << var() << "@| - " << coefficient() << "\n" << indent 
			      << "     return compareForGreaterOrEqual(POSINST,NEGINST)\n" << indent << "  }\n";
    

    case XMinus1ToNEGINST_IfCompForGrOrEqPOSINSTvNEGINSTEqFailRetFail:
      return outputShort(str << indent) << "\n" << indent << "  {\n" << indent   
			      << "     NEGINST := |X" << var() << "@| - 1\n" << indent 
			      << "     if compareForGreaterOrEqual(POSINST,NEGINST) == FailureToCompare return FailureToCompare\n" << indent << "  }\n";
    

    case CXMinusCToNEGINST_IfCompForGrOrEqPOSINSTvNEGINSTEqFailRetFail:
      return outputShort(str << indent) << "\n" << indent << "  {\n" << indent   
			      << "     NEGINST := " << coefficient() << "*|X" << var() << "@| - " << coefficient() << "\n" << indent 
			      << "     if compareForGreaterOrEqual(POSINST,NEGINST) == FailureToCompare return FailureToCompare\n" << indent << "  }\n";
    

    case NEGINSTPlusXMinus1ToNEGINST_IfCompForGrOrEqPOSINSTvNEGINSTEqFailRetFail:
      return outputShort(str << indent) << "\n" << indent << "  {\n" << indent   
			      << "     NEGINST := NEGINST + |X" << var() << "@| - 1\n" << indent 
			      << "     if compareForGreaterOrEqual(POSINST,NEGINST) == FailureToCompare return FailureToCompare\n" << indent << "  }\n";
    

    case NEGINSTPlusCXMinusCToNEGINST_IfCompForGrOrEqPOSINSTvNEGINSTEqFailRetFail:
      return outputShort(str << indent) << "\n" << indent << "  {\n" << indent   
			      << "     NEGINST := NEGINST + " << coefficient() << "*|X" << var() << "@| - " << coefficient() << "\n" << indent 
			      << "     if compareForGreaterOrEqual(POSINST,NEGINST) == FailureToCompare return FailureToCompare\n" << indent << "  }\n";
    

    case NEGINSTPlusXMinus1ToNEGINST_RetCompForGrOrEqPOSINSTvNEGINST:
      return outputShort(str << indent) << "\n" << indent << "  {\n" << indent   
			      << "     NEGINST := NEGINST + |X" << var() << "@| - 1\n" << indent 
			      << "     return compareForGreaterOrEqual(POSINST,NEGINST)\n" << indent << "  }\n";
    

    case NEGINSTPlusCXMinusCToNEGINST_RetCompForGrOrEqPOSINSTvNEGINST:
      return outputShort(str << indent) << "\n" << indent << "  {\n" << indent   
			      << "     NEGINST := NEGINST + " << coefficient() << "*|X" << var() << "@| - " << coefficient() << "\n" << indent 
			      << "     return compareForGreaterOrEqual(POSINST,NEGINST)\n" << indent << "  }\n";
    


    case IfNotGrndRetFail_IfXMinus1GrPOSINSTRetFail_IfXMinus1LePOSINSTRetAlwGr_RetAlwEq:
      return outputShort(str << indent) << "\n" << indent << "  {\n" << indent   
			      << "     ASSERT(constant(POSINST))\n" << indent 
			      << "     ASSERT(POSINST >= 0)\n" << indent 
			      << "     if !ground(X" << var() << "@) return FailureToCompare\n" << indent 
			      << "     if min|X" << var() << "@| - 1 > POSINST return FailureToCompare\n" << indent 
			      << "     if min|X" << var() << "@| - 1 < POSINST return AlwaysGreater\n" << indent 
			      << "     return AlwaysEqual\n" << indent << "  }\n";
    

    case IfNotGrndRetFail_IfCXMinusCGrPOSINSTRetFail_IfCXMinusCLePOSINSTRetAlwGr_RetAlwEq:
      return outputShort(str << indent) << "\n" << indent << "  {\n" << indent   
			      << "     ASSERT(constant(POSINST))\n" << indent 
			      << "     ASSERT(POSINST >= 0)\n" << indent 
			      << "     if !ground(X" << var() << "@) return FailureToCompare\n" << indent 
			      << "     if " << coefficient() << "*min|X" << var() << "@| - " << coefficient() << " > POSINST return FailureToCompare\n" << indent 
			      << "     if " << coefficient() << "*min|X" << var() << "@| - " << coefficient() << " < POSINST return AlwaysGreater\n" << indent 
			      << "     return AlwaysEqual\n" << indent << "  }\n";
    

    case IfNotGrndRetFail_POSINSTPlus1MinusXToACC_IfACCLe0RetFail:
      return outputShort(str << indent) << "\n" << indent << "  {\n" << indent   
			      << "     ASSERT(constant(POSINST))\n" << indent 
			      << "     ASSERT(POSINST >= 0)\n" << indent 
			      << "     if !ground(X" << var() << "@) return FailureToCompare\n" << indent 
			      << "     ACC := POSINST + 1 - min|X" << var() << "@|\n" << indent       
			      << "     if ACC < 0 return FailureToCompare\n" ;
    

    case IfNotGrndRetFail_POSINSTPlusCMinusCXToACC_IfACCLe0RetFail:
      return outputShort(str << indent) << "\n" << indent << "  {\n" << indent   
			      << "     ASSERT(constant(POSINST))\n" << indent 
			      << "     ASSERT(POSINST >= 0)\n" << indent 
			      << "     if !ground(X" << var() << "@) return FailureToCompare\n" << indent 
			      << "     ACC := POSINST + " << coefficient() << " - " << coefficient() << "*min|X" << var() << "@|\n" << indent       
			      << "     if ACC < 0 return FailureToCompare\n" << indent << "  }\n";
    

    case XPlusWToNEGINST_IfConstNEGINSTAndNEGINSTLe0RetAlwGr_IfConstNEGINSTGotoL:
      return outputShort(str << indent) << "\n" << indent << "  {\n" << indent   
			      << "     NEGINST := |X" << var() << "@| + " << weight() << "\n" << indent 
			      << "     if constant(NEGINST) && NEGINST < 0 return AlwaysGreater\n" << indent 
			      << "     if constant(NEGINST) goto [" << label() - _codeBase << "]\n" << indent << "  }\n";
    


    case CXPlusWToNEGINST_IfConstNEGINSTAndNEGINSTLe0RetAlwGr_IfConstNEGINSTGotoL:
      return outputShort(str << indent) << "\n" << indent << "  {\n" << indent   
			      << "     NEGINST := " << coefficient() << "*|X" << var() << "@| + " << weight() << "\n" << indent 
			      << "     if constant(NEGINST) && NEGINST < 0 return AlwaysGreater\n" << indent 
			      << "     if constant(NEGINST) goto [" << label() - _codeBase << "]\n" << indent << "  }\n";
    

    

    case XPlusWToNEGINST:
      return outputShort(str << indent) << "\n" << indent << "  {\n" << indent   
			      << "     NEGINST := |X" << var() << "@| + " << weight() << "\n" << indent << "  }\n";
    

    
    
    case CXPlusWToNEGINST:
      return outputShort(str << indent) << "\n" << indent << "  {\n" << indent   
			      << "     NEGINST := " << coefficient() << "*|X" << var() << "@| + " << weight() << "\n" << indent << "  }\n";
    

    
        
    case NEGINSTPlusXToNEGINST:
      return outputShort(str << indent) << "\n" << indent << "  {\n" << indent   
			      << "     NEGINST := NEGINST + |X" << var() << "@|\n" << indent << "  }\n";
    

    case NEGINSTPlusCXToNEGINST:
      return outputShort(str << indent) << "\n" << indent << "  {\n" << indent   
			      << "     NEGINST := NEGINST + " << coefficient() << "*|X" << var() << "@|\n" << indent << "  }\n";
    
    
    case NEGINSTPlusXToNEGINST_IfConstNEGINSTAndNEGINSTLe0RetAlwGr_IfConstNEGINSTGotoL:
      return outputShort(str << indent) << "\n" << indent << "  {\n" << indent   
			      << "     NEGINST := NEGINST + |X" << var() << "@|\n" << indent 
			      << "     if constant(NEGINST) && NEGINST < 0 return AlwaysGreater\n" << indent 
			      << "     if constant(NEGINST) goto [" << label() - _codeBase << "]\n" << indent << "  }\n";
    
        
    case NEGINSTPlusCXToNEGINST_IfConstNEGINSTAndNEGINSTLe0RetAlwGr_IfConstNEGINSTGotoL:
      return outputShort(str << indent) << "\n" << indent << "  {\n" << indent   
			      << "     NEGINST := NEGINST + " << coefficient() << "*|X" << var() << "@|\n" << indent 
			      << "     if constant(NEGINST) && NEGINST < 0 return AlwaysGreater\n" << indent 
			      << "     if constant(NEGINST) goto [" << label() - _codeBase << "]\n" << indent << "  }\n";
    
    

    case XMinus1ToPOSINST_IfCompForGrOrEqPOSINSTvNEGINSTEqAlwGrRetAlwGr:
      return outputShort(str << indent) << "\n" << indent << "  {\n" << indent   
			      << "     POSINST := |X" << var() << "@| - 1\n" << indent 
			      << "     if compareForGreaterOrEqual(POSINST,NEGINST) == AlwaysGreater return AlwaysGreater\n" << indent << "  }\n";
    

    case CXMinusCToPOSINST_IfCompForGrOrEqPOSINSTvNEGINSTEqAlwGrRetAlwGr:
      return outputShort(str << indent) << "\n" << indent << "  {\n" << indent   
			      << "     POSINST := " << coefficient() << "*|X" << var() << "@| - " << coefficient() << "\n" << indent 
			      << "     if compareForGreaterOrEqual(POSINST,NEGINST) == AlwaysGreater return AlwaysGreater\n" << indent << "  }\n";
    

    case POSINSTPlusXMinus1ToPOSINST_IfCompForGrOrEqPOSINSTvNEGINSTEqAlwGrRetAlwGr:
      return outputShort(str << indent) << "\n" << indent << "  {\n" << indent   
			      << "     POSINST := POSINST + |X" << var() << "@| - 1\n" << indent 
			      << "     if compareForGreaterOrEqual(POSINST,NEGINST) == AlwaysGreater return AlwaysGreater\n" << indent << "  }\n";
    

    case POSINSTPlusCXMinusCToPOSINST_IfCompForGrOrEqPOSINSTvNEGINSTEqAlwGrRetAlwGr:
      return outputShort(str << indent) << "\n" << indent << "  {\n" << indent   
			      << "     POSINST := POSINST + " << coefficient() << "*|X" << var() << "@| - " << coefficient() << "\n" << indent 
			      << "     if compareForGreaterOrEqual(POSINST,NEGINST) == AlwaysGreater return AlwaysGreater\n" << indent << "  }\n";
    

    case POSINSTPlusXMinus1ToPOSINST_RetCompForGrOrEqPOSINSTvNEGINST:
      return outputShort(str << indent) << "\n" << indent << "  {\n" << indent   
			      << "     POSINST := POSINST + |X" << var() << "@| - 1\n" << indent 
			      << "     return compareForGreaterOrEqual(POSINST,NEGINST)\n" << indent << "  }\n";
    

    case POSINSTPlusCXMinusCToPOSINST_RetCompForGrOrEqPOSINSTvNEGINST:
      return outputShort(str << indent) << "\n" << indent << "  {\n" << indent   
			      << "     POSINST := POSINST + " << coefficient() << "*|X" << var() << "@| - " << coefficient() << "\n" << indent 
			      << "     return compareForGreaterOrEqual(POSINST,NEGINST)\n" << indent << "  }\n";
    


    case NEGINSTPlus1MinusXToACC_IfACCLe0RetAlwGr_GrndToGRND:
      return outputShort(str << indent) << "\n" << indent << "  {\n" << indent   
			      << "     ASSERT(constant(NEGINST))\n" << indent 
			      << "     ASSERT(NEGINST >= 0)\n" << indent 
			      << "     ACC := NEGINST + 1 - min|X" << var() << "@|\n" << indent 
			      << "     if ACC < 0 return AlwaysGreater\n" << indent 
			      << "     GRND := ground(X" << var() << "@)\n" << indent << "  }\n";
        

    case NEGINSTPlusCMinusCXToACC_IfACCLe0RetAlwGr_GrndToGRND:
      return outputShort(str << indent) << "\n" << indent << "  {\n" << indent   
			      << "     ASSERT(constant(NEGINST))\n" << indent 
			      << "     ASSERT(NEGINST >= 0)\n" << indent 
			      << "     ACC := NEGINST + " << coefficient() << " - " << coefficient() << "*min|X" << var() << "@|\n" << indent 
			      << "     if ACC < 0 return AlwaysGreater\n" << indent 
			      << "     GRND := ground(X" << var() << "@)\n" << indent << "  }\n";
   
    default:
      return str << "????????\n";
    };
}; // ostream& WeightPolynomial::SpecialisedComparison::Command::output(ostream& str,const char* indent) const



ostream& 
WeightPolynomial::SpecialisedComparison::Command::outputShort(ostream& str) const
{
  CALL("outputShort(ostream& str) const");
  str << "[" << (this - _codeBase) << "] ";
  str << tag() << " ";
  if (requiresVar()) str << "(X" << var() << ")";
  if (requiresCoefficient()) str << "(C=" << coefficient() << ")";
  if (requiresWeight()) str << "(W=" << weight() << ")";
  if (requiresLabel()) str << "(L=" << "[" << label() - _codeBase << "])";
  return str;
}; // ostream& WeightPolynomial::SpecialisedComparison::Command::outputShort(ostream& str) const


ostream& WeightPolynomial::SpecialisedComparison::Command::outputCode(ostream& str,const char* indent,const Command* code)
{
  const Command* longestJump = code;
  setCodeBase(code);
 next_command:
  code->output(str,indent);
  if (code->requiresLabel() && (code->label() > longestJump))
    longestJump = code->label();
  if (code->isTerminal() && (code >= longestJump))
    return str;
  ++code;
  goto next_command;
}; // ostream& WeightPolynomial::SpecialisedComparison::Command::outputCode(ostream& str,const char* indent,const Command* code)


#endif  
//===================================================================
