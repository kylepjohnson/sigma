//
// File:         LiteralOrdering.hpp
// Description:  Extension of the simplification ordering on terms
//               to literals.  
// Created:      Nov 07, 2002, Alexandre Riazanov, riazanov@cs.man.ac.uk
//==================================================================
#include "LiteralOrdering.hpp"
//===================================================================
#undef DEBUG_NAMESPACE
#ifdef DEBUG_LITERAL_ORDERING
 #define DEBUG_NAMESPACE "LiteralOrdering"
#endif
#include "debugMacros.hpp"
//===================================================================

using namespace BK;
using namespace VK;


LiteralOrdering::LiteralOrdering() :
  _mode(0L)
{
};
   
LiteralOrdering::~LiteralOrdering()
{
};

void LiteralOrdering::init()
{
  CALL("init()");
  _mode = 0L;

}; // void LiteralOrdering::init()

void LiteralOrdering::destroy()
{
  CALL("destroy()");
}; // void LiteralOrdering::destroy()


Comparison LiteralOrdering::compare(TmpLiteral* lit1,
				    TmpLiteral* lit2) const
{
  CALL("compare(TmpLiteral* lit1,TmpLiteral* lit2) const");
  if (lit1->isEquality())
    {
      if (lit2->isEquality())
	{
	  return compareEq(lit1,lit2);
	}
      else
	return Less;
    }
  else
    if (lit2->isEquality())
      return Greater;
    
  // both are nonequalities

  if (_mode == 0L)
    {
      // compare atoms with the simplification ordering
    compare_atoms:
      Comparison acmp =  
	SimplificationOrdering::current()->compareAtoms(lit1->flatterm(),
							lit1->weight(),
							lit2->flatterm(),
							lit2->weight()); 
      if (acmp == Equal)
	{
	  if (lit1->polarity() == lit2->polarity()) return Equal;
	  return (lit1->isPositive()) ? Less : Equal;
	};
      return acmp;
    };

  if (_mode == 1L) 
    {
      // compare headers first
      Comparison hcmp = 
	Signature::current()->compareHeaderPrecedences(lit1->polarity(),
						       lit1->header().functor(),
						       lit2->polarity(),
						       lit2->header().functor());
      ASSERT(hcmp != Incomparable);
      if (hcmp == Equal)
	{
	  ASSERT(lit1->header() == lit2->header());

	  // compare atoms with the simplification ordering
	  return SimplificationOrdering::current()->compareAtoms(lit1->flatterm(),
								 lit1->weight(),
								 lit2->flatterm(),
								 lit2->weight()); 
	}
      else
	return hcmp;	
    }; // if (_mode == 1L)

  ASSERT(_mode == 2L);
  // compare predicates first
  if (lit1->header().functor() == lit2->header().functor())
    {
      goto compare_atoms;
    }
  else
    return 
      Signature::current()->compareHeaderPrecedences(lit1->polarity(),
						     lit1->header().functor(),
						     lit2->polarity(),
						     lit2->header().functor());

}; // Comparison LiteralOrdering::compare(TmpLiteral* lit1,TmpLiteral* lit2) const


inline
Comparison LiteralOrdering::compareEq(TmpLiteral* lit1,
				      TmpLiteral* lit2) const
{
  CALL("compareEq(TmpLiteral* lit1,TmpLiteral* lit2) const");
  ASSERT(lit1->isEquality());
  ASSERT(lit2->isEquality());
  
  // compare atoms first
  Comparison acmp;
  if (lit1->isOrderedEquality())
    {
      if (lit2->isOrderedEquality())
	{
	  // both are ordered
	  acmp = compareEqAtomsOO(lit1,lit2);
	}
      else
	{
	  acmp = compareEqAtomsOU(lit1,lit2);
	};
    }
  else
    if (lit2->isOrderedEquality())
      {
	acmp = inverse(compareEqAtomsOU(lit2,lit1));
      }
    else // both are unordered
      {
	acmp = compareEqAtomsUU(lit1,lit2);
      };
  
  if (acmp == Equal)
    {     
      if (lit1->polarity() == lit2->polarity()) return Equal;
      return (lit1->isPositive()) ? Less : Equal;
    };
  return acmp;
}; // Comparison LiteralOrdering::compareEq(TmpLiteral* lit1,TmpLiteral* lit2) const




Comparison LiteralOrdering::compareEqAtomsOO(TmpLiteral* lit1,
					     TmpLiteral* lit2) const

{
  CALL("compareEqAtomsOO(TmpLiteral* lit1,TmpLiteral* lit2) const");
  ASSERT(lit1->isOrderedEquality());
  ASSERT(lit2->isOrderedEquality());

  Comparison cmp11 = 
    SimplificationOrdering::current()->compare(lit1->arg1(),lit1->arg1Weight(),
					       lit2->arg1(),lit2->arg1Weight()); 
  if (cmp11 != Equal) return cmp11;
  return SimplificationOrdering::current()->compare(lit1->arg2(),lit1->arg2Weight(),
						    lit2->arg2(),lit2->arg2Weight()); 

}; // Comparison LiteralOrdering::compareEqAtomsOO(TmpLiteral* lit1,TmpLiteral* lit2) const

Comparison LiteralOrdering::compareEqAtomsOU(TmpLiteral* lit1,
					     TmpLiteral* lit2) const

{
  CALL("compareEqAtomsOU(TmpLiteral* lit1,TmpLiteral* lit2) const");
  ASSERT(lit1->isOrderedEquality());
  ASSERT(!lit2->isOrderedEquality());
  
  // lit1->arg1() > lit1->arg2()
  switch (SimplificationOrdering::current()->compare(lit1->arg1(),lit1->arg1Weight(),
						     lit2->arg1(),lit2->arg1Weight()))
    {
    case Less:
      // lit1->arg2() < lit1->arg1() < lit2->arg1()
      return Less;
	  
    case Equal: 
      // lit1->arg1() = lit2->arg1() * lit2->arg2(), compare lit1->arg2() with lit2->arg2()
      return SimplificationOrdering::current()->compare(lit1->arg2(),lit1->arg2Weight(),
							lit2->arg2(),lit2->arg2Weight());	
    case Greater: 
      // lit1->arg1() > lit2->arg1(), compare lit1->arg1() with lit2->arg2()
      return SimplificationOrdering::current()->compare(lit1->arg1(),lit1->arg1Weight(),
							lit2->arg2(),lit2->arg2Weight());
    case Incomparable:         
      // compare lit1->arg2() with lit2->arg1() and lit2->arg2()
      switch (SimplificationOrdering::current()->compare(lit1->arg2(),lit1->arg2Weight(),
							 lit2->arg1(),lit2->arg1Weight()))
	{
	case Less: 
	  if (SimplificationOrdering::current()->lessOrEqual(lit1->arg1(),lit1->arg1Weight(),
							     lit2->arg2(),lit2->arg2Weight()))
	    {
	      return Less;
	    }
	  else
	    return Incomparable;

	case Equal:
	  return SimplificationOrdering::current()->compare(lit1->arg1(),lit1->arg1Weight(),
							    lit2->arg2(),lit2->arg2Weight());

	case Greater:
	  // lit1->arg1() * lit2->arg1(), lit1->arg2() > lit2->arg1()
	  switch (SimplificationOrdering::current()->compare(lit1->arg1(),lit1->arg1Weight(),
							     lit2->arg2(),lit2->arg2Weight()))
	    {
	    case Less: return Less;
	    case Equal: return Greater;
	    case Greater: return Greater;
	    case Incomparable: 
	      if (SimplificationOrdering::current()->greaterOrEqual(lit1->arg2(),lit1->arg2Weight(),
								    lit2->arg2(),lit2->arg2Weight()))
		{
		  return Greater;
		}
	      else
		return Incomparable;
	    };

	case Incomparable:
	  // lit1->arg1() * lit2->arg1(), lit1->arg2() * lit2->arg1()
	  if (SimplificationOrdering::current()->less(lit1->arg1(),lit1->arg1Weight(),
						      lit2->arg2(),lit2->arg2Weight()))
	    {
	      return Less;
	    }
	  else
	    return Incomparable;
	};

    }; // switch

#ifdef DEBUG_NAMESPACE
  ICP("ICP0");
  return Incomparable;
#else
#  ifdef _SUPPRESS_WARNINGS_
  return Incomparable;
  
#  endif
#endif

}; // Comparison LiteralOrdering::compareEqAtomsOU(TmpLiteral* lit1,TmpLiteral* lit2) const


Comparison LiteralOrdering::compareEqAtomsUU(TmpLiteral* lit1,
					     TmpLiteral* lit2) const

{
  CALL("compareEqAtomsUU(TmpLiteral* lit1,TmpLiteral* lit2) const");
  ASSERT(!lit1->isOrderedEquality());
  ASSERT(!lit2->isOrderedEquality());

  Comparison cmp = SimplificationOrdering::current()->compare(lit1->arg1(),lit1->arg1Weight(),lit2->arg1(),lit2->arg1Weight());
  switch (cmp)
    {
    case Greater:
      // lit1->arg1() > lit2->arg1()
      cmp = SimplificationOrdering::current()->compare(lit1->arg1(),lit1->arg1Weight(),lit2->arg2(),lit2->arg2Weight()); 
      switch (cmp)
	{
	case Greater:
	  // lit1->arg1() > lit2->arg1(), lit1->arg1() > lit2->arg2()
	  return Greater;
        
	case Less:
	  // lit1->arg1() > lit2->arg1(), lit1->arg1() < lit2->arg2(). Not an error since v2.73
	  return SimplificationOrdering::current()->compare(lit1->arg2(),lit1->arg2Weight(),lit2->arg2(),lit2->arg2Weight()); 
       
	case Incomparable:
	  // lit1->arg1() > lit2->arg1(), lit1->arg1() * lit2->arg2()
	  cmp = SimplificationOrdering::current()->compare(lit1->arg2(),lit1->arg2Weight(),lit2->arg2(),lit2->arg2Weight()); 
	  switch (cmp)
	    {
	    case Greater:
	      // lit1->arg1() > lit2->arg1(), lit1->arg1() * lit2->arg2(), lit1->arg2() > lit2->arg2()
	      return Greater;
            
	    case Less:
	      // lit1->arg1() > lit2->arg1(), lit1->arg1() * lit2->arg2(), lit1->arg2() < lit2->arg2()
	      return Incomparable;
           
	    case Incomparable:
	      // lit1->arg1() > lit2->arg1(), lit1->arg1() * lit2->arg2(), lit1->arg2() * lit2->arg2()
	      return Incomparable;
           
	    case Equal:
	      // lit1->arg1() > lit2->arg1(), lit1->arg1() * lit2->arg2(), lit1->arg2() = lit2->arg2()
	      return Greater;
	    };
        
	case Equal:
	  // lit1->arg1() > lit2->arg1(), lit1->arg1() = lit2->arg2() : error because lit2->arg1() * lit2->arg2()
#ifdef DEBUG_NAMESPACE 
	  ICP("DE20"); 
#endif
	  return Incomparable;
	};

    case Less:
      // lit1->arg1() < lit2->arg1()
      cmp = SimplificationOrdering::current()->compare(lit1->arg2(),lit1->arg2Weight(),lit2->arg1(),lit2->arg1Weight()); 
      switch (cmp)
	{
	case Greater:
	  // lit1->arg1() < lit2->arg1(), lit1->arg2() > lit2->arg1(). Not an error since v2.73
	  return SimplificationOrdering::current()->compare(lit1->arg2(),lit1->arg2Weight(),lit2->arg2(),lit2->arg2Weight()); 
        
	case Less:
	  // lit1->arg1() < lit2->arg1(), lit1->arg2() < lit2->arg1()
	  return Less; // was Greater in the CASC-16 version (obvious incompleteness)
        
	case Incomparable:
	  // lit1->arg1() < lit2->arg1(), lit1->arg2() * lit2->arg1()
	  cmp = SimplificationOrdering::current()->compare(lit1->arg2(),lit1->arg2Weight(),lit2->arg2(),lit2->arg2Weight()); 
	  switch (cmp)
	    {
	    case Greater:
	      // lit1->arg1() < lit2->arg1(), lit1->arg2() * lit2->arg1(), lit1->arg2() > lit2->arg2()
	      return Incomparable;
           
	    case Less:
	      // lit1->arg1() < lit2->arg1(), lit1->arg2() * lit2->arg1(), lit1->arg2() < lit2->arg2()
	      return Less;
           
	    case Incomparable:
	      // lit1->arg1() < lit2->arg1(), lit1->arg2() * lit2->arg1(), lit1->arg2() * lit2->arg2()
	      return Incomparable;
           
	    case Equal:
	      // lit1->arg1() < lit2->arg1(), lit1->arg2() * lit2->arg1(), lit1->arg2() = lit2->arg2()
	      return Less; 
	    }; 
        
	case Equal:
	  // lit1->arg1() < lit2->arg1(), lit1->arg2() = lit2->arg1() : error because lit1->arg1() * lit1->arg2()
#ifdef DEBUG_NAMESPACE 
	  ICP("DE40");
#endif
	  return Incomparable;
	};
      
    case Incomparable:
      // lit1->arg1() * lit2->arg1()
      cmp = SimplificationOrdering::current()->compare(lit1->arg1(),lit1->arg1Weight(),lit2->arg2(),lit2->arg2Weight()); 
      switch (cmp)
	{
	case Greater:
	  // lit1->arg1() * lit2->arg1(), lit1->arg1() > lit2->arg2()
	  cmp = SimplificationOrdering::current()->compare(lit1->arg2(),lit1->arg2Weight(),lit2->arg1(),lit2->arg1Weight()); 
	  switch (cmp)
	    {
	    case Greater:
	      // lit1->arg1() * lit2->arg1(), lit1->arg1() > lit2->arg2(), lit1->arg2() > lit2->arg1()
	      return Greater; 
           
	    case Less:
	      // lit1->arg1() * lit2->arg1(), lit1->arg1() > lit2->arg2(), lit1->arg2() < lit2->arg1()
	      return Incomparable;
           
	    case Incomparable:
	      // lit1->arg1() * lit2->arg1(), lit1->arg1() > lit2->arg2(), lit1->arg2() * lit2->arg1()
	      return Incomparable;
           
	    case Equal:
	      // lit1->arg1() * lit2->arg1(), lit1->arg1() > lit2->arg2(), lit1->arg2() = lit2->arg1()
	      return Greater;
	    };
          
	case Less:
	  // lit1->arg1() * lit2->arg1(), lit1->arg1() < lit2->arg2()
	  cmp = SimplificationOrdering::current()->compare(lit1->arg2(),lit1->arg2Weight(),lit2->arg1(),lit2->arg1Weight()); 
	  switch (cmp)
	    {
	    case Greater:
	      // lit1->arg1() * lit2->arg1(), lit1->arg1() < lit2->arg2(), lit1->arg2() > lit2->arg1()
	      cmp = SimplificationOrdering::current()->compare(lit1->arg2(),lit1->arg2Weight(),lit2->arg2(),lit2->arg2Weight());
	      ASSERT(cmp != Equal);
	      return cmp;
          
	    case Less:
	      // lit1->arg1() * lit2->arg1(), lit1->arg1() < lit2->arg2(), lit1->arg2() < lit2->arg1()
	      return Less;
            
	    case Incomparable:
	      // lit1->arg1() * lit2->arg1(), lit1->arg1() < lit2->arg2(), lit1->arg2() * lit2->arg1()
	      cmp = SimplificationOrdering::current()->compare(lit1->arg2(),lit1->arg2Weight(),lit2->arg2(),lit2->arg2Weight()); 
	      if (cmp == Less) return Less;
	      return Incomparable;
            
	    case Equal:
	      // lit1->arg1() * lit2->arg1(), lit1->arg1() < lit2->arg2(), lit1->arg2() = lit2->arg1()
	      return Less; 
	    }; 
         

	case Incomparable:
	  // lit1->arg1() * lit2->arg1(), lit1->arg1() * lit2->arg2()
	  cmp = SimplificationOrdering::current()->compare(lit1->arg2(),lit1->arg2Weight(),lit2->arg1(),lit2->arg1Weight()); 
	  switch (cmp)
	    {
	    case Greater:
	      // lit1->arg1() * lit2->arg1(), lit1->arg1() * lit2->arg2(), lit1->arg2() > lit2->arg1()
	      cmp = SimplificationOrdering::current()->compare(lit1->arg2(),lit1->arg2Weight(),lit2->arg2(),lit2->arg2Weight()); 
	      if (cmp == Greater) return Greater;
	      ASSERT(cmp != Equal);
	      return Incomparable;
           
	    case Less:
	      // lit1->arg1() * lit2->arg1(), lit1->arg1() * lit2->arg2(), lit1->arg2() < lit2->arg1()
	      return Incomparable;
           
	    case Incomparable:
	      // lit1->arg1() * lit2->arg1(), lit1->arg1() * lit2->arg2(), lit1->arg2() * lit2->arg1()
	      return Incomparable;
           
	    case Equal:
	      // lit1->arg1() * lit2->arg1(), lit1->arg1() * lit2->arg2(), lit1->arg2() = lit2->arg1()
	      return Incomparable;
	    }; 
        
	case Equal:
	  // lit1->arg1() * lit2->arg1(), lit1->arg1() = lit2->arg2()
	  cmp = SimplificationOrdering::current()->compare(lit1->arg2(),lit1->arg2Weight(),lit2->arg1(),lit2->arg1Weight()); 
	  return cmp;
	};
      
    case Equal:
      // lit1->arg1() = lit2->arg1()
      cmp = SimplificationOrdering::current()->compare(lit1->arg2(),lit1->arg2Weight(),lit2->arg2(),lit2->arg2Weight()); 
      return cmp;
    };
#ifdef DEBUG_NAMESPACE 
  ICP("DE100");
  return Incomparable; 
#else
#ifdef _SUPPRESS_WARNINGS_
  return Incomparable; 
#endif  
#endif

}; // Comparison LiteralOrdering::compareEqAtomsUU(TmpLiteral* lit1,TmpLiteral* lit2) const




//=================================================================
