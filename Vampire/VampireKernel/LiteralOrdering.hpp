//
// File:         LiteralOrdering.hpp
// Description:  Extension of the simplification ordering on terms
//               to literals.  
// Created:      Nov 07, 2002, Alexandre Riazanov, riazanov@cs.man.ac.uk
//==================================================================
#ifndef LITERAL_ORDERING_H
#define LITERAL_ORDERING_H
//==================================================================
#include "jargon.hpp"
#include "VampireKernelDebugFlags.hpp"
#include "Comparison.hpp"
#include "SimplificationOrdering.hpp"
#include "tmp_literal.hpp"
#include "Signature.hpp"
//===================================================================
#undef DEBUG_NAMESPACE
#ifdef DEBUG_LITERAL_ORDERING
 #define DEBUG_NAMESPACE "LiteralOrdering"
#endif
#include "debugMacros.hpp"
//===================================================================

namespace VK
{
  class LiteralOrdering
  {
  public:
    LiteralOrdering();    
    ~LiteralOrdering();
    void init();
    void destroy();
    void setMode(long m) { _mode = m; };
    bool isStratified() const { return (_mode != 0L); };
    BK::Comparison compare(TmpLiteral* lit1,
		       TmpLiteral* lit2) const;
    BK::Comparison compareLayers(const TmpLiteral* lit1,
			     const TmpLiteral* lit2) const;
  private:
    BK::Comparison compareEq(TmpLiteral* lit1,
			 TmpLiteral* lit2) const;
    BK::Comparison compareEqAtomsOO(TmpLiteral* lit1,
				TmpLiteral* lit2) const;
    BK::Comparison compareEqAtomsOU(TmpLiteral* lit1,
				TmpLiteral* lit2) const;
    BK::Comparison compareEqAtomsUU(TmpLiteral* lit1,
				TmpLiteral* lit2) const;
    
    
  private:
    long _mode;
  }; // class LiteralOrdering
}; // namespace VK

//===================================================================
#undef DEBUG_NAMESPACE
#ifdef DEBUG_LITERAL_ORDERING
 #define DEBUG_NAMESPACE "LiteralOrdering"
#endif
#include "debugMacros.hpp"
//===================================================================

namespace VK
{  
  inline
  BK::Comparison LiteralOrdering::compareLayers(const TmpLiteral* lit1,
					    const TmpLiteral* lit2) const
  {
    CALL("compareLayers(const TmpLiteral* lit1,const TmpLiteral* lit2) const");
    if (lit1->isEquality())
      {
	if (lit2->isEquality())
	  {
	    return BK::Equal;
	  }
	else
	  return BK::Less;
      }
    else
      if (lit2->isEquality())
	{
	  return BK::Greater;
	};
    // both are nonequalities
    
    if (_mode == 0L) return BK::Equal;
    if (_mode == 1L)
      {
	return 
	  Signature::current()->compareHeaderPrecedences(lit1->polarity(),
							 lit1->header().functor(),
							 lit2->polarity(),
							 lit2->header().functor());
      };

    ASSERT(_mode == 2L);
    
    if (lit1->header().functor() == lit2->header().functor())
      {
	return BK::Equal;
      }
    else
      return 
	Signature::current()->compareHeaderPrecedences(lit1->polarity(),
						       lit1->header().functor(),
						       lit2->polarity(),
						       lit2->header().functor());
  }; // BK::Comparison LiteralOrdering::compareLayers(const TmpLiteral* lit1,const TmpLiteral* lit2) const

}; // namespace VK

//=================================================================
#endif
