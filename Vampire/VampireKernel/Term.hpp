//
// File:         term.hpp
// Description:  Representation of stored terms.
// Created:      Oct 15, 1999.
// Author:       Alexandre Riazanov
// mail:         riazanov@cs.man.ac.uk
// Revised:      Dec 22, 2001.
//               32 bit long SubtermMask replaced by flexible 
//               typedef BitSet<GlobAlloc> LitNumMask.
//====================================================
#ifndef TERM_H
//=================================================
#define TERM_H
#include <cstddef>
#include <iostream>
#include "jargon.hpp"
#include "VampireKernelDebugFlags.hpp"
#include "VampireKernelConst.hpp"
#include "GlobAlloc.hpp"
#include "Comparison.hpp"
#include "Stack.hpp"
#include "ExpandingStack.hpp"
#include "BitSet.hpp"
#include "SmallHeaderSkipList.hpp"
#include "TermWeightType.hpp"
#include "Signature.hpp"
//=================================================

// Allignment of bits in the representation of symbols:

//       number    polarity splitting       arity        flag
//  [     16     ]   [1]       [1]       [    12     ]   [ 2 ] 

//================================================
#undef DEBUG_NAMESPACE
#ifdef DEBUG_TERM
#define DEBUG_NAMESPACE "TERM"
#endif
#include "debugMacros.hpp"
//=================================================

namespace VK
{
const ulong TermArityMask         = 0x00003FFC;
const ulong TermArityInvMask     = 0xFFFF9003;
const ulong TermSplittingMask     = 0x00004000;
const ulong TermSplittingInvMask = 0xFFFFBFFF; 
const ulong TermPolarityMask      = 0x00008000;
const ulong TermPolarityInvMask  = 0xFFFF7FFF;
const ulong TermVarMask           = 0xFFFF0000;
const ulong TermVarInvMask       = 0x0000FFFF;   
const ulong TermFunctorMask       = 0xFFFF0000;
const ulong TermFunctorInvMask   = 0x0000FFFF;
const ulong TermFlagMask          = 0x00000003;
const ulong TermFlagInvMask      = 0xFFFFFFFC;

enum TermFlag 
{ 
  TermRef      = 0x00000000, 
  TermVar      = 0x00000001, 
  TermComplex  = 0x00000002 
};

class TERM
{
 public:
  typedef ulong Arity;
  typedef ulong Functor;
  typedef ulong Polarity;
  static const Polarity PositivePolarity; // = 0UL;
  static const Polarity NegativePolarity; // = 1UL;
  inline Polarity inverse(const Polarity& pol) { return !pol; };

  class OpenCompact;
  class OpenCompactCautious;
  class Iterator;
  class IteratorWithDepth;
  class Traversal;
  class Arguments;
  class NonvariableWithInfo;
  class OpenUnshared;
  class FastOpenUnshared;
  

  typedef ulong ClauseNumber;
  typedef BK::BitSet<BK::GlobAlloc> LitNumMask;
  typedef BK::SHSkList<BK::GlobAlloc,LitNumMask*,ClauseNumber,2,TERM> OccList;

  class AssociatedInfo
  {
  public:
    AssociatedInfo() {};
    ~AssociatedInfo() {};
    OccList* occurences() { return _occurences; };
    const OccList* occurences() const { return _occurences; };
    void setOccurences(OccList* l) { _occurences = l; };

    /*********
    bool termIsGround() const { return _termIsGround; };
    void setTermIsGround(bool fl) { _termIsGround = fl; };
    const TermWeightType& minimalInstanceWeight() const { return _minimalInstanceWeight; };
    void setMinimalInstanceWeight(const TermWeightType& w)
    {
      _minimalInstanceWeight = w;
    };
    ***************/
  private:
    OccList* _occurences;
    //    bool _termIsGround;
    //    TermWeightType _minimalInstanceWeight;
  }; // class AssociatedInfo



  class InitStatic
    {
    public: 
      InitStatic() 
	{
	  if (!_count) { TERM::initStatic(); _count++; };    
	};
      ~InitStatic() 
	{
	};
    private:
      static long _count;
    };

 public: 
 
  //======= construction/initialisation/destruction

  TERM() {};
  explicit TERM(const ulong& cont) : content(cont) {};
  explicit TERM(TERM* const & r) : ref(r) {};
  TERM(const TERM& t) : content(t.content) {};
  TERM(TermFlag,ulong var) // variable
    : content(var) 
    {
      (content <<= 16) |= TermVar;
    };
 
  TERM(Arity a,Functor f,Polarity p) : content(f)
    { 
      CALL("costructor TERM(Arity a,Functor f,Polarity p)");

      ASSERT(a <= TERM::maxArity());
      ASSERT(((ulong)p) <= 1);
      (((((content <<= 1) |= p) <<= 13) |= a) <<= 2) |= TermComplex;  
    }; 

  ~TERM() {};
  
  void init() {};
  void init(Arity a,Functor f,Polarity p)
  {
    CALL("init(Arity a,Functor f,Polarity p)");
    ASSERT(a <= TERM::maxArity());
    ((((((content = f) <<= 1) |= p) <<= 13) |= a) <<= 2) |= TermComplex; 
  };
  void destroy() {};

  void Make(const ulong& c) { content = c; };
  void MakeVar(ulong var)
    {   
      ((content = var) <<= 16) |= TermVar;   
    };
  void MakeComplex(Arity a,Functor f,Polarity p)
    {
      CALL("MakeComplex(Arity a,Functor f,Polarity p)");
      ASSERT(a <= TERM::maxArity());
      ((((((content = f) <<= 1) |= p) <<= 13) |= a) <<= 2) |= TermComplex;  
    }; 
  void MakeReference(TERM* r) { ref = r; };
  void MakeHeader(ulong hdNum) // without arity!
    {
      ulong fun = hdNum/2;
      ulong pol = hdNum%2;
      MakeComplex((TERM::Arity)0,(TERM::Functor)fun,(Polarity)pol); 
    };

  void MakeSplitting() 
    {
      content |= TermSplittingMask;
    }; 

  //========== getting properties: ================

  TermFlag Flag() const { return TermFlag((content << 30) >> 30); };

  Arity arity() const { return (content << 18) >> 20; };
  static Arity maxArity() 
    {
      return VampireKernelConst::MaxSymbolArity; 
    };

  static Functor maxFunctor()
    {
      CALL("maxFunctor()");
      ASSERT(VampireKernelConst::MaxlSymbolNum <= 65535);
      return VampireKernelConst::MaxlSymbolNum;
    };
  
  static ulong maxHeaderNum()
    {
      CALL("maxHeaderNum()");
      ASSERT(VampireKernelConst::MaxlHeaderNum <= maxFunctor()*2 + 1);
      return maxFunctor()*2 + 1;
    };

  Polarity polarity() const { return (content << 16) >> 31; };
  bool Positive() const { return !polarity(); };
  bool Negative() const { return polarity() != 0UL; };
  bool IsSplitting() const 
    {
      return ((content << 17) >> 31) != 0UL;
    };
  Functor functor() const { return content >> 16; }; 
  ulong HeaderNum() const 
    {
      return functor()*2 + polarity();
    };
  static Polarity HeaderPolarity(ulong headerNum) 
    {
      return (headerNum % 2);
    };
  static bool HeaderPositive(ulong headerNum) 
    {
      return !(headerNum % 2);
    };
  static bool HeaderNegative(ulong headerNum) 
    {
      return (headerNum % 2) != 0UL;
    };
  ulong var() const { return content >> 16; }; 
  const ulong& Content() const { return content; };
  ulong& Content() { return content; };

  //=========== changing properties: ==================  
 
  void SetArity(ulong a)
    {
      CALL("SetArity(ulong a)");
      ASSERT(a <= TERM::maxArity());
      (content &= TermArityInvMask) |= (a <<= 2);    
    };
  void SetPolarity(Polarity p)
    {
      (content &= TermPolarityInvMask) |= (p <<= 15);   
    };   
  void SetVar(ulong v) // doesn't set the flag
    {
      (content &= TermVarInvMask) |= (v << 16);    
    };
  void makePositive() { SetPolarity(PositivePolarity); };
  void makeNegative() { SetPolarity(NegativePolarity); };
  void Inversepolarity() 
    { 
      if (Positive()) { SetPolarity(NegativePolarity); } else SetPolarity(PositivePolarity);
    };
  void SetFunctor(ulong f)
    {
      (content &= TermFunctorInvMask) |= (f <<= 16);
    };

  void SetFlag(TermFlag fl) 
    {
      CALL("SetFlag(TermFlag fl)"); 
      ASSERT(fl <= TermComplex);
      ((content >>= 2) <<= 2) |= fl;
    };   
  void MakeUnordered() { SetFunctor(VampireKernelConst::UnordEqNum); };
  void MakeOrdered() { SetFunctor(VampireKernelConst::OrdEqNum); };  
  
  //====== allocation: ===============================

  void* operator new(size_t,ulong arity) 
    {    
      CALL("operator new(size_t,ulong arity)");
      // arity can be 0 
      return BK::GlobAlloc::allocate((arity + 1)*sizeof(TERM) + sizeof(AssociatedInfo));
    };
  void operator delete(void* obj) 
    {
      CALL("operator delete(void* obj)");
      BK::GlobAlloc::deallocate(obj,((static_cast<TERM*>(obj))->arity() + 1)*sizeof(TERM) + sizeof(AssociatedInfo));
    }; 

#if (BK_COMPILER_FAMILY != BK_COMPILER_FAMILY_GCC_3)
  void operator delete(void* obj,ulong arity)
  {
    CALL("operator delete(void* obj,ulong arity)");
    BK::GlobAlloc::deallocate(obj,(arity + 1)*sizeof(TERM) + sizeof(AssociatedInfo));
  };
#endif  


  static TERM* AllocLit(const TERM& header)
    {
      TERM* res = 
	static_cast<TERM*>(BK::GlobAlloc::allocate((header.arity() + 1)*sizeof(TERM) + sizeof(void*)));
      res++;
      *res = header;
      return res;
    };  
  void DeleteLit() 
    { 
      BK::GlobAlloc::deallocate(this - 1,(arity() + 1)*sizeof(TERM) + sizeof(void*));
    };

  //===================== copying: ====================

  TERM& operator=(const TERM& t) { content = t.content; return *this; };
  TERM* CopyTop() const { return new(arity()) TERM(*this); };
  TERM* CopyLitTop() const { return AllocLit(*this); };

  //=============== comparison: =================================

  bool operator==(const TERM& t) const { return content == t.content; };
  bool operator!=(const TERM& t) const { return content != t.content; };
  bool operator<(const TERM& t) const { return content < t.content; };
  bool operator<=(const TERM& t) const { return content <= t.content; }; 
  bool operator>(const TERM& t) const { return content > t.content; };
  bool operator>=(const TERM& t) const { return content >= t.content; };

  bool hasGreaterPrecedenceThan(const TERM& f) const 
    {
      return 
	Signature::current()->symbolPrecedence(functor()) > 
	Signature::current()->symbolPrecedence(f.functor());
    };
  BK::Comparison comparePrecedence(const TERM& f) const
    {
      long diff = 
	Signature::current()->symbolPrecedence(functor()) - 
	Signature::current()->symbolPrecedence(f.functor());
      if (diff > 0L) return BK::Greater;
      if (diff < 0L) return BK::Less;
      return BK::Equal;
    };
  
  TermWeightType constantWeightPart() const
    {
      return Signature::current()->weightConstantPart(functor());
    };



  bool ComplementaryHeader(const TERM& hd) const 
    {
      return (functor() == hd.functor()) && (polarity() != hd.polarity());
    };
  static bool TERM::EqualComplexTerms(const TERM* t1,const TERM* t2);
  bool isSyntacticallyEqualTo(const TERM& t) const;

  BK::Comparison CompareLex(const TERM* t) const;

  
    
  //======= recognising: ========================

  bool IsReference() const { return !Flag(); };
  bool isVariable() const { return Flag() == TermVar; };
  bool isNotVariable() const { return Flag() != TermVar; };  
  bool isComplex() const { return Flag() == TermComplex; };
  bool IsConstant() const { return isComplex() && (!arity()); };
  bool InNonconstFunc() const { return isComplex() && arity(); };
  bool IsNull() const { return Flag() == TermComplex; };
  bool IsSymLitHeader() const 
    {
      //return false; 
      return IsEquality(); 
    };
  bool IsEquality() const { return VampireKernelConst::isEquality(functor()); };
  bool IsOrderedEq() const 
    {
      CALL("IsOrderedEq()");
      ASSERT(IsEquality());
      return VampireKernelConst::isOrderedEq(functor());
    };
  bool IsUnorderedEq() const 
    {
      CALL("IsUnorderedEq()");
      ASSERT(IsEquality());  
      return VampireKernelConst::isUnorderedEq(functor());
    };

  bool isBuiltInTruthValue() const
    {
      return VampireKernelConst::isBuiltInTrue(functor());
    };

  bool isBuiltInTrue() const
    {
      return *this == builtInTrue();
    };

  bool isBuiltInFalse() const
    {
      return *this == builtInFalse();
    };

  bool IsReservedHeader() const { return VampireKernelConst::isReservedPred(functor()); };

  bool IsDefHeader() const; // <=> "all arguments are different variables"

  bool isAnswerHeader() const
    {
      return Signature::current()->isAnswerPredicate(functor());
    };
  
  bool isAnswerLiteral() const
    {
      return isAnswerHeader();
    };
  
  static bool Symmetry(const TERM* t1,const TERM* t2);

  //========== surfing term structure: =================

  TERM* First() const { return ref; };   
  TERM* Args() { return this + arity(); };
  const TERM* Args() const { return this + arity(); };
  TERM* Next() { return this - 1; };
  const TERM* Next() const { return this - 1; };
  TERM* Previous() { return this + 1; };
  const TERM* Previous() const { return this + 1; };
  TERM* NthArg(ulong n) { return Args() - n; };   
  const TERM* NthArg(ulong n) const { return Args() - n; };
  TERM* Arg1() { return Args(); };
  const TERM* Arg1() const { return Args(); };
  TERM* Arg2() { return Args()->Next(); };
  const TERM* Arg2() const { return Args()->Next(); };

  //============= creating term structure: =========

  void SetFirst(TERM* t) { ref = t; };      

  //================ miscelanious: =============

  bool containsSymbol(const TERM& sym);

  static bool answerPredicatesExist() 
    { 
      return Signature::current()->answerPredicatesExist(); 
    };

  ulong size() const;
  ulong depth() const; 

  TERM unsharedCopy() const;
 
  TERM* unsharedLitCopy() const;

  void destroyUnsharedComplex();

  void destroyUnsharedLit();

  void Destroy() { Destroy(this); };
  void DestroyLit() { DestroyLit(this); };
  static void Destroy(TERM* t);
  static void DestroyLit(TERM* t)
    {
      for (TERM* arg = t->Args(); !t->IsNull(); t = t->Next())
	if (arg->IsReference()) Destroy(arg->First());
      t->DeleteLit();
    }; 


  AssociatedInfo* associatedInfo() const 
  { 
    return (AssociatedInfo*)(this + arity() + 1); 
  };
 
  OccList* Occurences() const
    {
      return associatedInfo()->occurences();
    }; 
  void InitOccurences() const { associatedInfo()->setOccurences((OccList*)0); };

  void DestroyOccurences() const
    {
      CALL("DestroyOccurences() const");               
      ASSERT(isComplex());
      OccList* occList = Occurences();
      if (occList) 
	{
	  OccList::Destruction destruction(*occList);
	  LitNumMask* litNumMask;
	  while (destruction.next(litNumMask))
	    {
	      ASSERT(litNumMask);
	      delete litNumMask;
	    };
	  delete occList;
	  InitOccurences();
	};
    }; // void DestroyOccurences() const 

  void RegisterOccurence(ulong clauseNum,ulong litNum) const
    {
      CALL("RegisterOccurence(ulong clauseNum,ulong litNum) const");
      ASSERT(isComplex());
      OccList* occList = Occurences();
      if (!occList) 
	{ 
	  occList = new OccList(); 
	  associatedInfo()->setOccurences(occList);
	};
      bool newNode;
      OccList::Node* node = occList->insert(clauseNum,newNode);

      if (newNode) node->value() = new LitNumMask();
      if (!node->value()->write(litNum))
	{
	  LitNumMask* newLitNumMask = node->value()->add(litNum);
	  delete node->value();
	  node->value() = newLitNumMask;
	};
    }; // void RegisterOccurence(ulong clauseNum,ulong litNum) const 

  void RemoveOccurences(ulong clauseNum) const
    {
      CALL("RemoveOccurences(ulong clauseNum) const");
      ASSERT(isComplex());
      OccList* occList = Occurences();
      if (!occList) return;
      LitNumMask* litNumMask;

      if (occList->remove(clauseNum,litNumMask))
	{
	  ASSERT(litNumMask);
	  delete litNumMask;
	};
      ASSERT(!occList->contains(clauseNum));
      if (!(*occList)) { delete occList; InitOccurences(); };
    }; // void RemoveOccurences(ulong clauseNum) const



  void** InfoField() const { return (void**)(this - 1); }; // for literals 
  static ulong UnordHeaderNum(ulong header_num)
    {
      CALL("UnordHeaderNum(ulong header_num)");
      ASSERT((header_num/2 == VampireKernelConst::UnordEqNum) || (header_num/2 == VampireKernelConst::OrdEqNum));
      return VampireKernelConst::UnordEqNum*2 + header_num%2; 
    };

  //================ output: ==================

  //const char* functorName() const;


  static ostream& output(ostream& str,const TERM& t) 
    {
      return output(str,t,Signature::current());
    };

  static ostream& output(ostream& str,
                         const TERM& t,
                         Signature* sig);

  static ostream& output(ostream& str,const TERM* t) 
    { 
      return output(str,t,Signature::current());
    };

  static ostream& output(ostream& str,const TERM* t,Signature* sig);
 
  ostream& output(ostream& str) const { return output(str,this); };
  ostream& output(ostream& str,Signature* sig) const 
    {
      return output(str,sig);
    };
  
  ostream& outputInTPTPFormat(ostream& str) const;
  ostream& outputAsPrologTerm(ostream& str) const;
  static const TERM& dummyPositiveHeader0() { return _dummyPositiveHeader0; };
  static const TERM& dummyPositiveHeader1() { return _dummyPositiveHeader1; };
  static const TERM& positiveOrdEqHeader() { return _positiveOrdEqHeader; };
  static const TERM& negativeOrdEqHeader() { return _negativeOrdEqHeader; };
  static const TERM& positiveUnordEqHeader() { return _positiveUnordEqHeader; };
  static const TERM& negativeUnordEqHeader() { return _negativeUnordEqHeader; };
  static const TERM& builtInTrue() { return _builtInTrue; };
  static const TERM& builtInFalse() { return _builtInFalse; };
  


 private:
  static void initStatic();
  TERM(const bool&) { CALL("constructor TERM(const bool&)"); ICP("ICP0"); };
  TERM& operator=(const bool&) 
  {
    CALL("operator=(const bool&)");
    ICP("ICP0");
    return *this;
  };
  TERM(const long&) { CALL("constructor TERM(const long&)"); ICP("ICP0"); };
  TERM& operator=(const long&) 
  {
    CALL("operator=(const long&)");
    ICP("ICP0");
    return *this;
  };

 private:
  union 
  {
    ulong content;
    TERM* ref;
  };


  static TERM _dummyPositiveHeader0;
  static TERM _dummyPositiveHeader1;
  static TERM _positiveOrdEqHeader;
  static TERM _negativeOrdEqHeader;
  static TERM _positiveUnordEqHeader;
  static TERM _negativeUnordEqHeader;  
  static TERM _builtInTrue;
  static TERM _builtInFalse;
  friend class InitStatic;
}; // class TERM

}; // namespace VK
//=================================================================
#undef DEBUG_NAMESPACE   
#ifdef DEBUG_TERM
#define DEBUG_NAMESPACE "TERM::Traversal"
#endif 
#include "debugMacros.hpp"
//=================================================

namespace VK
{
class TERM::Traversal 
{
 public:
  enum State { Func, Var, End };
 public:
  Traversal() 
    {
      CALLM("?","constructor Traversal()");
      DOP(_debugName = "??");
      wrapper[0] = TERM::dummyPositiveHeader1();   
      //wrapper[0].SetFlag(TermComplex);
      ASSERT_IN(wrapper->IsNull(),"end");
    }; 

  Traversal(const char* name)
    {
      CALLM(name,"constructor Traversal()");
      DOP(_debugName = name);
      wrapper[0] = TERM::dummyPositiveHeader1();  
      //wrapper[0].SetFlag(TermComplex);  
      ASSERT_IN(wrapper->IsNull(),"end");
    }; 
  
  Traversal(const TERM& t)
    {
      CALLM("?","constructor Traversal(const TERM& t)");
      DOP(_debugName = "???");
      wrapper[0] = TERM::dummyPositiveHeader1();  
      //wrapper[0].SetFlag(TermComplex);

      wrapper[1] = t;
      trm = wrapper + 1;
      if (t.isVariable()) 
	{
	  currState = Var;
	  sym = trm;
	}
      else // t is reference to a complex term
	{
	  currState = Func;
	  sym = trm->First();
	}; 
      ASSERT_IN(wrapper->IsNull(),"end");
    };

  Traversal(const char* name,const TERM& t)
    {
      CALLM(name,"constructor Traversal(const char* name,const TERM& t)");
      DOP(_debugName = name);
      wrapper[0] = TERM::dummyPositiveHeader1();  
      //wrapper[0].SetFlag(TermComplex);

      wrapper[1] = t;
      trm = wrapper + 1;
      if (t.isVariable()) 
	{
	  currState = Var;
	  sym = trm;
	}
      else // t is reference to a complex term
	{
	  currState = Func;
	  sym = trm->First();
	}; 
      ASSERT_IN(wrapper->IsNull(),"end");
    };

  Traversal(const TERM* t) 
    {
      CALLM("?","constructor Traversal(const TERM* t)");
      DOP(_debugName = "???");
      ASSERT(t->isComplex());
      wrapper[0] = TERM::dummyPositiveHeader1();  
      //wrapper[0].SetFlag(TermComplex);

      wrapper[1].SetFirst((TERM*)t);
      trm = wrapper + 1;
      currState = Func;
      sym = trm->First();
      ASSERT_IN(wrapper->IsNull(),"end");
    };
  Traversal(const char* name,const TERM* t)
    {
      CALLM(name,"constructor Traversal(const char* name,const TERM* t)");
      DOP(_debugName = name);
      ASSERT(t->isComplex());
      wrapper[0] = TERM::dummyPositiveHeader1();  
      //wrapper[0].SetFlag(TermComplex);

      wrapper[1].SetFirst((TERM*)t);
      trm = wrapper + 1;
      currState = Func;
      sym = trm->First();
      ASSERT_IN(wrapper->IsNull(),"end");
    };

  
  void init() 
    {
      CALLM("?","init()");
      subterms.init();
      DOP(_debugName = "??");
      wrapper[0] = TERM::dummyPositiveHeader1(); 
      ASSERT_IN(wrapper->IsNull(),"end");
    }; 
  
  void init(const char* name)
  {
    CALLM(name,"init(const char* name)");    
    subterms.init();
    DOP(_debugName = name);
    wrapper[0] = TERM::dummyPositiveHeader1(); 
    ASSERT_IN(wrapper->IsNull(),"end");
  };

  void destroy()
  {
    CALLM(_debugName,"destroy()");
    subterms.destroy();
    BK_CORRUPT(*this);
  }; 


  void reset(const TERM& t)
    {
      CALLM(_debugName,"reset(const TERM& t)");
      ASSERT(wrapper->IsNull());
      subterms.reset();
      wrapper[1] = t;
      trm = wrapper + 1;
      if (t.isVariable()) 
	{
	  currState = Var;
	  sym = trm;
	}
      else
	{
	  currState = Func;
	  sym = trm->First();
	}; 
      ASSERT_IN(wrapper->IsNull(),"end");
    };

  void reset(const TERM* t)
    {
      CALLM(_debugName,"reset(const TERM& t)");
      ASSERT(wrapper->IsNull());
      ASSERT(t->isComplex());
      subterms.reset();
      wrapper[1].SetFirst((TERM*)t);
      trm = wrapper + 1;
      currState = Func;
      sym = trm->First();
      ASSERT_IN(wrapper->IsNull(),"end");
    };

  const State& state() const { return currState; };
  operator bool() const { return currState != End; };

  void next()
    {
      CALLM(_debugName,"next()");
      ASSERT(wrapper->IsNull());
      const TERM* nextTrm = trm->Next();
      if ((currState == Func) && (sym->arity()))
	{
	  if (!nextTrm->IsNull()) subterms.push(nextTrm);
	  trm = sym->Args();
	}
      else // variable or constant
	{
	  if (nextTrm->IsNull())
	    { 
	      if (subterms.empty()) { currState = End; ASSERT(wrapper->IsNull()); return; }
	      else 
		{          
		  trm = subterms.pop();
		};
	    }
	  else 
	    {
	      trm = nextTrm;
	    };
	};
      if (trm->isVariable())
	{
	  currState = Var;
	  sym = trm;
	}
      else // reference to a nonvariable term
	{
	  currState = Func;
	  sym = trm->First();
	};
      ASSERT(wrapper->IsNull());
    };
  
  void after()
    {
      CALLM(_debugName,"after()");
      ASSERT(wrapper->IsNull());
      trm = trm->Next();
      if (trm->IsNull()) 
	if (subterms.empty()) { currState = End; ASSERT(wrapper->IsNull()); return; } 
	else trm = subterms.pop();
      if (trm->isVariable())
	{
	  currState = Var;
	  sym = trm;
	}
      else // reference to a nonvariable term
	{
	  currState = Func;
	  sym = trm->First();
	};   
      ASSERT(wrapper->IsNull());
    };

  const TERM& symbol() const { return *sym; }; 
  const TERM& term() const { return *trm; };

#ifndef NO_DEBUG
  bool checkIntegrity() 
    {
      return wrapper->IsNull();
    };
#endif

 private:
  TERM wrapper[2];
  State currState;
  BK::Stack<const TERM*,VampireKernelConst::MaxTermSize> subterms;
  const TERM* trm;
  const TERM* sym;
#ifndef NO_DEBUG
  const char* _debugName;
#endif
}; // class TERM::Traversal

}; // namespace VK


//=================================================
#undef DEBUG_NAMESPACE  
#ifdef DEBUG_TERM
#define DEBUG_NAMESPACE "TERM::Arguments"
#endif
#include "debugMacros.hpp"
//=================================================
namespace VK
{

class TERM::Arguments
{
 public:
  Arguments() {};
  Arguments(const TERM& t) : _args((TERM*)(t.First()->Args())) {};
  Arguments(const TERM* t) : _args((TERM*)(t->Args())) {};
  Arguments(const Arguments& args) : _args(args._args) {};
  Arguments& operator=(const Arguments& args) { _args = args._args; return *this; };
  void reset(const TERM& t) 
    {
      CALL(" reset(const TERM& t)");
      ASSERT(t.IsReference());
      ASSERT(t.First()->arity());
      _args = (TERM*)(t.First()->Args()); 
    };
  void reset(const TERM* t)
    {
      CALL("reset(const TERM* t)");
      ASSERT(t->isComplex());
      ASSERT(t->arity());
      _args = (TERM*)(t->Args());
    };
  TERM* nth(ulong n) { return _args - n; };
  const TERM* nth(ulong n) const { return _args - n; };
 private:
  TERM* _args;
}; // class TERM::Arguments


}; // namespace VK


//=================================================
#undef DEBUG_NAMESPACE  
#ifdef DEBUG_TERM
#define DEBUG_NAMESPACE "TERM::Iterator"
#endif
#include "debugMacros.hpp"
//=================================================
namespace VK
{
class TERM::Iterator // for complex terms only!
{
 private:
  BK::Stack<const TERM*,VampireKernelConst::MaxTermSize> subterms;   
  const TERM* sym;
  bool isVar;  
 public:
  Iterator() {};   
  Iterator(const TERM* t) 
    { 
      sym = t;
      isVar = false;
    };     
  
  void init() { subterms.init(); };  
  void destroy() { subterms.destroy(); };
  
  void Reset(const TERM* t) 
    { 
      subterms.reset(); 
      sym = t;
      isVar = false;
    };                   
  TERM Symbol() const { return *sym; };
  const TERM& symbolRef() const { return *sym; };
  const TERM* CurrPos() const { return sym; };
  bool CurrentSymIsVar() const { return isVar; };
 public:
  bool Next()
    {
      if (isVar)
	{
	  sym = sym->Next();
	}
      else
	{
	  sym = sym->Args();  
	};
      if (sym->IsNull()) 
	{
	  if (subterms.empty()) { return false; };
	  sym = subterms.pop();
	};
      if (!((isVar = sym->isVariable()))) // reference
	{
	  if (!(sym->Next()->IsNull())) { subterms.push(sym->Next()); };
	  sym = sym->First(); 
	};
      return true;
    };
  bool NextVar()
    {
      while (Next())
	{
	  if (isVar) { return true; };
	};
      return false;
    };
  bool SkipTerm()
    {
      if (isVar)
	{
	  sym = sym->Next();
	  if (sym->IsNull()) 
	    {
	      if (subterms.empty()) { return false; };
	      sym = subterms.pop();
	    };
	}
      else
	{
	  if (subterms.empty()) { return false; };
	  sym = subterms.pop();
	}; 
      if (!((isVar = sym->isVariable()))) // reference
	{
	  if (!(sym->Next()->IsNull())) { subterms.push(sym->Next()); };
	  sym = sym->First(); 
	};
      return true; 
    }; 
}; // class TERM::Iterator
}; // namespace VK
 
//=================================================================
#undef DEBUG_NAMESPACE  
#ifdef DEBUG_TERM
#define DEBUG_NAMESPACE "BracketCounter"
#endif
#include "debugMacros.hpp"
//=================================================
namespace VK
{
template <ulong MaxDepth>
class BracketCounter
{
 private:
  int holes[MaxDepth];
  int* next_holes;
  int curr_holes;  
 public:
  BracketCounter() : curr_holes(0) {  next_holes = holes; };
  ~BracketCounter() {};
  void init() { curr_holes = 0; next_holes = holes; };  
  void destroy() {};
  void Reset() { curr_holes = 0; next_holes = holes; };
  bool IsOpen() const { return curr_holes != 0; };
  bool AllClosed() const { return !(curr_holes); };
  void Open(int arity) 
    {
      CALL("Open(int arity)");  
      ASSERT(arity > 0); 
      ASSERT(next_holes < (holes + MaxDepth));
      *next_holes = curr_holes-1;
      next_holes++;
      curr_holes = arity;
    };
  unsigned long Close()
    {
      CALL("Close()");
      curr_holes--;
      if (curr_holes) return 0; // curr_holes can be -1 ! 
      unsigned closed = 1;
    pop_holes:
      if (next_holes == (holes + 1)) return closed;
      next_holes--;
      ASSERT(next_holes >= holes); 
      curr_holes = (*next_holes);
      if (curr_holes) return closed;
      closed++;
      goto pop_holes;
    };
}; // class BracketCounter<ulong MaxDepth>


}; // namespace VK
//=================================================
#undef DEBUG_NAMESPACE  
#ifdef DEBUG_TERM
#define DEBUG_NAMESPACE "ExpandingBracketCounter"
#endif
#include "debugMacros.hpp"
//=================================================
namespace VK
{

class ExpandingBracketCounter
{
 public:
  ExpandingBracketCounter()
    : _allHoles(DOP("ExpandingBracketCounter::_allHoles"))
    {
    };
  ~ExpandingBracketCounter() {};
  void init() 
  {
    CALL("init()");
    _allHoles.init(DOP("ExpandingBracketCounter::_allHoles"));
  };
  void destroy() 
  {
    CALL("destroy()");
    _allHoles.destroy();
  };
  void reset() { _currentNumOfHoles = 0; _allHoles.reset(); };
  bool isOpen() const { return _currentNumOfHoles != 0L; };
  bool everythingIsClosed() const { return !isOpen(); };
  void openSafe(ulong arity)
    {
      CALL("open(ulong arity)");
      ASSERT(arity > 0); 
      ASSERT(_currentNumOfHoles >= -1);
      _allHoles.pushSafe(_currentNumOfHoles - 1);
      _currentNumOfHoles = (long)arity;
    };
  
  ulong close() 
    {
      CALL("close()");
      _currentNumOfHoles--;
      if (_currentNumOfHoles) return 0UL; // curr_holes can be -1 !  
      ulong res = 1UL; // at least one closing bracket
    pop_holes: 
      if (_allHoles.empty()) return res;
      _currentNumOfHoles = _allHoles.pop();
      if (_currentNumOfHoles) return res;
      res++;         // one more closing bracket
      goto pop_holes;
    }; 

 private: 
  BK::ExpandingStack<BK::GlobAlloc,long,128UL,ExpandingBracketCounter> _allHoles; 
  long _currentNumOfHoles;
}; // class ExpandingBracketCounter

}; // namespace VK





//=================================================
#undef DEBUG_NAMESPACE
#ifdef DEBUG_TERM
#define DEBUG_NAMESPACE "DepthCounter"
#endif
#include "debugMacros.hpp"
//=================================================
namespace VK
{
class DepthCounter
{
 private:
  ulong depth;
 public: 
  DepthCounter() {};
  void init() {};
  void destroy() {};
  void Reset(ulong d) 
    {
      CALL("Reset(ulong d)");
      ASSERT(d);
      depth = d; 
    };
  ulong Depth() const { return depth; };
  void Down() 
    {
      depth++; 
    };
  void Up(ulong n) 
    { 
      CALL("Up(ulong n)");
      depth -= n; 
      RARE(!depth);
    };
};
}; // namespace VK
//=================================================
#undef DEBUG_NAMESPACE
#ifdef DEBUG_TERM
#define DEBUG_NAMESPACE "TERM::IteratorWithDepth"
#endif
#include "debugMacros.hpp"
//=================================================

namespace VK
{
class TERM::IteratorWithDepth  // for complex terms only!
: private Iterator,
	    private BracketCounter<VampireKernelConst::MaxTermDepth>,
	    private DepthCounter 
{
 public:
  IteratorWithDepth() {};
  void init()
  {
    CALL("init()");
    Iterator::init();
    BracketCounter<VampireKernelConst::MaxTermDepth>::init();
    DepthCounter::init();
  };
  void destroy()
  {
    CALL("destroy()");
    DepthCounter::destroy();
    BracketCounter<VampireKernelConst::MaxTermDepth>::destroy();
    Iterator::destroy();
  };
  void Reset(const TERM* t,ulong d) 
    {
      CALL("Reset(const TERM* t,ulong d)");
      ASSERT(d);
      Iterator::Reset(t);
      BracketCounter<VampireKernelConst::MaxTermDepth>::Reset();
      DepthCounter::Reset(d);
    };
  TERM Symbol() const 
    { 
      return Iterator::Symbol();  
    };
  const TERM& symbolRef() const { return Iterator::symbolRef(); };
  bool CurrentSymIsVar() const { return Iterator::CurrentSymIsVar(); };
  bool Next()
    {
      if (Symbol().arity())
	{
	  BracketCounter<VampireKernelConst::MaxTermDepth>::Open(Symbol().arity());
	  DepthCounter::Down();
	}
      else DepthCounter::Up(BracketCounter<VampireKernelConst::MaxTermDepth>::Close());
      return Iterator::Next();
    };
  ulong Depth() const { return DepthCounter::Depth(); }; 
}; // class TERM::IteratorWithDepth
}; // namespace VK

//================================================
#undef DEBUG_NAMESPACE
#ifdef DEBUG_TERM
#define DEBUG_NAMESPACE "SmartDepthCounter<ulong MaxDepth>"
#endif
#include "debugMacros.hpp"
//=================================================
namespace VK
{
template <ulong MaxDepth>
class SmartDepthCounter 
: private BracketCounter<MaxDepth>, private DepthCounter
{
 public:  
  SmartDepthCounter() {};
  void init()
  {
    CALL("init()");
    BracketCounter<MaxDepth>::init();
    DepthCounter::init();
  };
  void destroy()
  {
    CALL("destroy()");
    DepthCounter::destroy();
    BracketCounter<MaxDepth>::destroy();
  };
  void Reset(const unsigned long d)
    {
      CALL("Reset(const unsigned long d)");
      BracketCounter<MaxDepth>::Reset();
      DepthCounter::Reset(d);
    };
  void NonconstantFunctor(int arity) 
    {
      CALL("NonconstantFunctor(int arity)");
      ASSERT(arity > 0);
      ASSERT(DepthCounter::Depth() < VampireKernelConst::MaxTermDepth);
      BracketCounter<MaxDepth>::Open(arity);
      DepthCounter::Down();
    };
  void VarOrConst() { DepthCounter::Up(BracketCounter<MaxDepth>::Close()); };
  void NextSym(const TERM& sym)
    {
      if (sym.arity()) { NonconstantFunctor(sym.arity()); }
      else VarOrConst();       
    }; 
  unsigned long Depth() { return DepthCounter::Depth(); };
}; // class SmartDepthCounter<ulong MaxDepth>
}; // namespace VK

//=================================================================
#undef DEBUG_NAMESPACE   
#ifdef DEBUG_TERM
#define DEBUG_NAMESPACE "TERM::OpenCompact"
#endif 
#include "debugMacros.hpp"
//=================================================

namespace VK
{
class TERM::OpenCompact
{
  // Don't forget to give it enough memory!
  // VampireKernelConst::MaxTermSize may not be enough because every complex subterm
  // takes one additional word for the pointer to it. 
  // 2*VampireKernelConst::MaxTermSize will suffice.
 public:
  OpenCompact() 
    : _holes("_holes"), 
    _numOfHoles("_numOfHoles")
#ifdef DEBUG_NAMESPACE
    , _debugDepthCounter()
#endif
    {
    };
  OpenCompact(TERM* mem) 
    : _holes("_holes"), 
    _numOfHoles("_numOfHoles")
#ifdef DEBUG_NAMESPACE
    , _debugDepthCounter()
#endif
    {   
      _currHole = &_term;
      _currNumOfHoles = 1;
      _freeMem = mem;
      DOP(_debugDepthCounter.Reset(1));
      DOP(_debugSizeCounter = 1);
    };
  void reset(TERM* mem)
    {
      _holes.reset();
      _numOfHoles.reset();
      _currHole = &_term;
      _currNumOfHoles = 1;
      _freeMem = mem;
      DOP(_debugDepthCounter.Reset(1));
      DOP(_debugSizeCounter = 1);
    };
  void pushVar(ulong var) 
    {
      CALL("pushVar(ulong var)");
      DOP(_debugDepthCounter.VarOrConst());
      ASSERT(_debugDepthCounter.Depth() && (_debugDepthCounter.Depth() <= VampireKernelConst::MaxTermDepth));
      ASSERT(_currHole);
      _currHole->MakeVar(var);
      _currNumOfHoles--;
      if (_currNumOfHoles) 
	{
	  _currHole = _currHole->Next();
	}
      else
	if (_holes.nonempty())
	  {
	    _currHole = _holes.pop();  
	    _currNumOfHoles = _numOfHoles.pop();
	    ASSERT(_currNumOfHoles);
	  }
	else
	  {
	    DOP(_currHole = (TERM*)0);     
	  };
    };
  void pushVar(const TERM& var)
    {
      CALL("pushVar(const TERM& var)");
      ASSERT(var.isVariable());
      DOP(_debugDepthCounter.VarOrConst());
      ASSERT(_debugDepthCounter.Depth() && (_debugDepthCounter.Depth() <= VampireKernelConst::MaxTermDepth));
      ASSERT(_currHole);
      *_currHole = var;
      _currNumOfHoles--;
      if (_currNumOfHoles) 
	{
	  _currHole = _currHole->Next();
	}
      else
	if (_holes.nonempty())
	  {
	    _currHole = _holes.pop();  
	    _currNumOfHoles = _numOfHoles.pop();
	    ASSERT(_currNumOfHoles);
	  }
	else
	  {
	    DOP(_currHole = (TERM*)0);
	  };
    };
  void pushConst(const TERM& c) 
    {
      CALL("pushConst(const TERM& c)");
      ASSERT(c.isComplex());
      ASSERT(!c.arity());
      DOP(_debugDepthCounter.VarOrConst());
      ASSERT(_debugDepthCounter.Depth() && (_debugDepthCounter.Depth() <= VampireKernelConst::MaxTermDepth));
      ASSERT(_currHole);
      _currHole->SetFirst(_freeMem);   
      _currNumOfHoles--;
      *_freeMem = c;
      _freeMem++;
      if (_currNumOfHoles) 
	{
	  _currHole = _currHole->Next();
	}
      else
	if (_holes.nonempty())
	  {
	    _currHole = _holes.pop();  
	    _currNumOfHoles = _numOfHoles.pop();
	    ASSERT(_currNumOfHoles);
	  }
	else
	  {
	    DOP(_currHole = (TERM*)0);
	  };    
    };
  void pushNonconstFun(const TERM& f)
    {
      CALL("pushNonconstFun(const TERM& f)");
      ASSERT(f.isComplex());
      ASSERT(f.arity());
      DOP(_debugDepthCounter.NonconstantFunctor(f.arity()));
      ASSERT(_debugDepthCounter.Depth() && (_debugDepthCounter.Depth() <= VampireKernelConst::MaxTermDepth));
      DOP(_debugSizeCounter += f.arity());
      ASSERT(_debugSizeCounter <= VampireKernelConst::MaxTermSize);
      ASSERT(_currHole);
      _currHole->SetFirst(_freeMem);
      _currNumOfHoles--;
      if (_currNumOfHoles) 
	{
	  _holes.push(_currHole->Next());
	  _numOfHoles.push(_currNumOfHoles);
	};
      *_freeMem = f;
      _currHole = _freeMem->Args();
      _freeMem += (f.arity() + 1);
      _currNumOfHoles = f.arity();
    };
  void push(const TERM& sym)
    {
      CALL("push(const TERM& sym)");
      if (sym.isVariable()) { pushVar(sym); }
      else 
	if (sym.arity()) { pushNonconstFun(sym); } 
	else pushConst(sym);
    };
  TERM* freeMemory() const { return _freeMem; };
 private:
  BK::Stack<TERM*,VampireKernelConst::MaxTermDepth> _holes;
  BK::Stack<ulong,VampireKernelConst::MaxTermDepth> _numOfHoles;
  TERM* _currHole;
  ulong _currNumOfHoles;
  TERM _term;
  TERM* _freeMem;
#ifdef DEBUG_NAMESPACE
  SmartDepthCounter<VampireKernelConst::MaxTermDepth + 1> _debugDepthCounter;
  ulong _debugSizeCounter;
#endif
}; // class TERM::OpenCompact

}; // namespace VK

//=================================================================
#undef DEBUG_NAMESPACE   
#ifdef DEBUG_TERM
#define DEBUG_NAMESPACE "TERM::OpenCompactCautious"
#endif 
#include "debugMacros.hpp"
//=================================================

namespace VK
{
class TERM::OpenCompactCautious
{
  // Don't forget to give it enough memory when calling
  // OpenCompactCautious(TERM* mem,ulong maxSize) or reset(TERM* mem,ulong maxSize).
  // maxSize may not be enough because every complex subterm
  // takes one additional word for the pointer to it. 
  // 2*maxSize will suffice.
 public:
  OpenCompactCautious() 
    : _holes("_holes"), 
    _numOfHoles("_numOfHoles"),
    _depthCounter()
    {
    };
  OpenCompactCautious(TERM* mem,ulong maxSize) 
    : _maxSize(maxSize),
    _holes("_holes"), 
    _numOfHoles("_numOfHoles"),
    _depthCounter()
    {   
      _currHole = &_term;
      _currNumOfHoles = 1;
      _freeMem = mem;
      _depthCounter.Reset(1);
      _sizeCounter = 1;
    };

  void reset(TERM* mem,ulong maxSize)
    {
      _maxSize = maxSize;
      _holes.reset();
      _numOfHoles.reset();
      _currHole = &_term;
      _currNumOfHoles = 1;
      _freeMem = mem;
      _depthCounter.Reset(1);
      _sizeCounter = 1;
    };
  bool pushVar(ulong var) 
    {
      CALL("pushVar(ulong var)");
      _depthCounter.VarOrConst();
      if (_depthCounter.Depth() == 0UL) return false;
      if (!_currHole) return false;
      _currHole->MakeVar(var);
      _currNumOfHoles--;
      if (_currNumOfHoles) 
	{
	  _currHole = _currHole->Next();
	}
      else
	if (_holes.nonempty())
	  {
	    _currHole = _holes.pop();  
	    _currNumOfHoles = _numOfHoles.pop();
	    ASSERT(_currNumOfHoles);
	  }
	else
	  {
	    _currHole = (TERM*)0;     
	  };
      return true;
    }; // bool pushVar(ulong var) 

  bool pushVar(const TERM& var)
    {
      CALL("pushVar(const TERM& var)");
      ASSERT(var.isVariable());
      _depthCounter.VarOrConst();
      if (_depthCounter.Depth() == 0UL) return false;
      if (!_currHole) return false;
      *_currHole = var;
      _currNumOfHoles--;
      if (_currNumOfHoles) 
	{
	  _currHole = _currHole->Next();
	}
      else
	if (_holes.nonempty())
	  {
	    _currHole = _holes.pop();  
	    _currNumOfHoles = _numOfHoles.pop();
	    ASSERT(_currNumOfHoles);
	  }
	else
	  {
	    _currHole = (TERM*)0;
	  };
      return true;
    }; // bool pushVar(const TERM& var)

  bool pushConst(const TERM& c) 
    {
      CALL("pushConst(const TERM& c)");
      ASSERT(c.isComplex());
      ASSERT(!c.arity());
      _depthCounter.VarOrConst();
      if (_depthCounter.Depth() == 0) return false;
      if (!_currHole) return false;
      _currHole->SetFirst(_freeMem);   
      _currNumOfHoles--;
      *_freeMem = c;
      _freeMem++;
      if (_currNumOfHoles) 
	{
	  _currHole = _currHole->Next();
	}
      else
	if (_holes.nonempty())
	  {
	    _currHole = _holes.pop();  
	    _currNumOfHoles = _numOfHoles.pop();
	    ASSERT(_currNumOfHoles);
	  }
	else
	  {
	    _currHole = (TERM*)0;
	  };
      return true;    
    }; // bool pushConst(const TERM& c) 

  bool pushNonconstFun(const TERM& f)
    {
      CALL("pushNonconstFun(const TERM& f)");
      ASSERT(f.isComplex());
      ASSERT(f.arity());
      if (_depthCounter.Depth() >= VampireKernelConst::MaxTermDepth) return false;
      _depthCounter.NonconstantFunctor(f.arity());
      _sizeCounter += f.arity();
      if ((_sizeCounter > VampireKernelConst::MaxTermSize)
	  || (_sizeCounter > _maxSize))
	return false;
      if (!_currHole) return false;
      _currHole->SetFirst(_freeMem);
      _currNumOfHoles--;
      if (_currNumOfHoles) 
	{
	  _holes.push(_currHole->Next());
	  _numOfHoles.push(_currNumOfHoles);
	};
      *_freeMem = f;
      _currHole = _freeMem->Args();
      _freeMem += (f.arity() + 1);
      _currNumOfHoles = f.arity();
      return true; 
    }; // bool pushNonconstFun(const TERM& f)

  bool push(const TERM& sym)
    {
      CALL("push(const TERM& sym)");
      if (sym.isVariable()) { return pushVar(sym); }
      else 
	if (sym.arity()) { return pushNonconstFun(sym); } 
	else return pushConst(sym);
    };
  bool endOfTerm()
    {
      return (!_currHole) && (_depthCounter.Depth() == 1UL); 
    };
  TERM* freeMemory() const { return _freeMem; };
 private:
  ulong _maxSize;
  BK::Stack<TERM*,VampireKernelConst::MaxTermDepth> _holes;
  BK::Stack<ulong,VampireKernelConst::MaxTermDepth> _numOfHoles;
  TERM* _currHole;
  ulong _currNumOfHoles;
  TERM _term;
  TERM* _freeMem;
  SmartDepthCounter<VampireKernelConst::MaxTermDepth + 1> _depthCounter;
  ulong _sizeCounter;
}; // class TERM::OpenCompactCautious
}; // namespace VK

//=================================================
#undef DEBUG_NAMESPACE   
#ifdef DEBUG_TERM
#define DEBUG_NAMESPACE "TERM::OpenUnshared"
#endif 
#include "debugMacros.hpp"
//=================================================

namespace VK
{
class TERM::OpenUnshared
{
 public:
  OpenUnshared() 
    { 
      CALL("constructor OpenUnshared()");
      reset(); 
    };
  ~OpenUnshared() {};

  void init()
  {
    CALL("init()");
    _depthCounter.init();
    _holes.init();
    _freeHoles.init();
    _allocated.init();
    reset();
  };

  void destroy()
  {
    CALL("destroy()");
    _allocated.destroy();
    _freeHoles.destroy();
    _holes.destroy();
    _depthCounter.destroy();
  };

  void reset() 
    { 
      _currHole = &_res; 
      _numOfFreeHoles = 1UL;
      _size = 1UL; 
      _depthCounter.Reset(1UL);
      _holes.reset();
      _freeHoles.reset(); 
      _allocatedLit = (TERM*)0;
      _allocated.reset();
    };
  bool litHeader(const TERM& hd)
    {
      CALL("litHeader(const TERM& hd)"); 
      ulong arity = hd.arity();

      _currHole->SetFirst(TERM::AllocLit(hd));
      _allocatedLit = _currHole->First();
      if (arity) 
	{ 
	  _depthCounter.NonconstantFunctor(arity);
	  _numOfFreeHoles = arity;         
	  _currHole = _currHole->First()->Args();
	};
      return true;
    };
  bool functor(const TERM& f)
    {
      CALL("functor(const TERM& f)");
      ASSERT(_numOfFreeHoles);
      ASSERT(_numOfFreeHoles <= TERM::maxArity());
      ulong arity = f.arity();
      _size += arity;
      if (_size > VampireKernelConst::MaxTermSize) 
	{
	  recycle(); 
	  return false; 
	};
      _numOfFreeHoles--;
      if (arity) 
	{
	  _currHole->SetFirst(new (arity) TERM(f)); 
	  _allocated.push(_currHole->First()); 
	  if (_depthCounter.Depth() >= VampireKernelConst::MaxTermDepth) 
	    {
	      recycle(); 
	      return false; 
	    };
	  _depthCounter.NonconstantFunctor(arity);        
	  if (_numOfFreeHoles)
	    { 
	      _holes.push(_currHole->Next());
	      _freeHoles.push(_numOfFreeHoles);    
	    };
	  _currHole = _currHole->First()->Args();
	  _numOfFreeHoles = arity; 
	}
      else // constant
	{
	  _currHole->SetFirst(new (arity) TERM(f));
	  _depthCounter.VarOrConst(); 
	  if (_numOfFreeHoles) { _currHole = _currHole->Next(); }
	  else // backtrack
	    if (_holes)
	      {
		_currHole = _holes.pop();
		_numOfFreeHoles = _freeHoles.pop();
		ASSERT(_numOfFreeHoles);
		ASSERT(_numOfFreeHoles <= TERM::maxArity());
	      };     
	};
      return true;
    }; // bool functor(const TERM& f)

  void variable(const TERM& v)
    {
      CALL("variable(const TERM& v)"); 
      ASSERT(_numOfFreeHoles);
      ASSERT(_numOfFreeHoles <= TERM::maxArity());
      _depthCounter.VarOrConst(); 
      _numOfFreeHoles--;
      *_currHole = v;
      if (_numOfFreeHoles) { _currHole = _currHole->Next(); }
      else // backtrack   
	if (_holes)
	  {
	    _currHole = _holes.pop();
	    _numOfFreeHoles = _freeHoles.pop();
	    ASSERT(_numOfFreeHoles);
	    ASSERT(_numOfFreeHoles <= TERM::maxArity());
	  };  
    }; // void variable(const TERM& v)

  bool symbol(const TERM& s)
    { 
      CALL("symbol(const TERM& s)");
      // can not be applied to a predicate header!
      if (s.isVariable()) { variable(s); return true; } else return functor(s);
    };  

  const TERM& result() const { return _res; };
  void recycle()
    {
      CALL("recycle()");
      if (_allocatedLit) { _allocatedLit->DeleteLit(); _allocatedLit = (TERM*)0; };
      while (_allocated) delete _allocated.pop();    
    }; 

 private: 
  TERM _res;
  TERM* _currHole;
  ulong _size;
  ulong _numOfFreeHoles;
  SmartDepthCounter<VampireKernelConst::MaxTermDepth> _depthCounter;
  BK::Stack<TERM*,VampireKernelConst::MaxTermDepth> _holes;
  BK::Stack<ulong,VampireKernelConst::MaxTermDepth> _freeHoles;
  TERM* _allocatedLit;
  BK::Stack<TERM*,VampireKernelConst::MaxTermSize> _allocated;
}; // class TERM::OpenUnshared

}; // namespace VK

//=================================================
#undef DEBUG_NAMESPACE   
#ifdef DEBUG_TERM
#define DEBUG_NAMESPACE "TERM::FastOpenUnshared"
#endif 
#include "debugMacros.hpp"
//=================================================

namespace VK
{
class TERM::FastOpenUnshared
{
 public:
  FastOpenUnshared() : _holes(), _freeHoles()
    { 
      reset(); 
    };
  ~FastOpenUnshared() {};
  void init()
  {
    CALL("init()");
    _holes.init(); 
    _freeHoles.init();
    reset(); 
  };
  
  void destroy()
  {
    CALL("destroy()");
    _freeHoles.destroy();
    _holes.destroy();
  };

  void reset() 
    { 
      _currHole = &_res; 
      _numOfFreeHoles = 1UL;
      _holes.reset();
      _freeHoles.reset(); 
    };

  void litHeader(const TERM& hd)
    {
      CALL("litHeader(const TERM& hd)"); 
      ulong arity = hd.arity();

      _currHole->SetFirst(TERM::AllocLit(hd));
      if (arity) 
	{ 
	  _numOfFreeHoles = arity;         
	  _currHole = _currHole->First()->Args();
	};
    };

  void functor(const TERM& f)
    {
      CALL("functor(const TERM& f)");
      ASSERT(f.isComplex());
      ASSERT(_numOfFreeHoles);
      ASSERT(_numOfFreeHoles <= TERM::maxArity());
      ulong arity = f.arity();
      _numOfFreeHoles--;
      if (arity) 
	{       
	  _currHole->SetFirst(new (arity) TERM(f)); 
	  if (_numOfFreeHoles)
	    { 
	      _holes.push(_currHole->Next());
	      _freeHoles.push(_numOfFreeHoles);    
	    };
	  _currHole = _currHole->First()->Args();
	  _numOfFreeHoles = arity; 
	}
      else // constant
	{
	  _currHole->SetFirst(new (arity) TERM(f));
	  if (_numOfFreeHoles) { _currHole = _currHole->Next(); }
	  else // backtrack
	    if (_holes)
	      {
		_currHole = _holes.pop();
		_numOfFreeHoles = _freeHoles.pop();
		ASSERT(_numOfFreeHoles);
		ASSERT(_numOfFreeHoles <= TERM::maxArity());
	      };     
	};
    }; // void functor(const TERM& f)

  void variable(const TERM& v)
    {
      CALL("variable(const TERM& v)"); 
      ASSERT(v.isVariable());
      ASSERT(_numOfFreeHoles);
      ASSERT(_numOfFreeHoles <= TERM::maxArity());
      _numOfFreeHoles--;
      *_currHole = v;
      if (_numOfFreeHoles) { _currHole = _currHole->Next(); }
      else // backtrack   
	if (_holes)
	  {
	    _currHole = _holes.pop();
	    _numOfFreeHoles = _freeHoles.pop();
	    ASSERT(_numOfFreeHoles);
	    ASSERT(_numOfFreeHoles <= TERM::maxArity());
	  };  
    }; // void variable(const TERM& v)


  void symbol(const TERM& s)
    { 
      CALL("symbol(const TERM& s)");
      // can not be applied to a predicate header!
      if (s.isVariable()) { variable(s);  } else functor(s);
    };  

  const TERM& result() const { return _res; };

 private: 
  TERM _res;
  TERM* _currHole;
  ulong _numOfFreeHoles;
  BK::Stack<TERM*,VampireKernelConst::MaxTermDepth> _holes;
  BK::Stack<ulong,VampireKernelConst::MaxTermDepth> _freeHoles;
}; // class TERM::FastOpenUnshared

}; // namespace VK

//================================================
#undef DEBUG_NAMESPACE
#ifdef DEBUG_TERM
#define DEBUG_NAMESPACE "TERM"
#endif
#include "debugMacros.hpp"
//=================================================

namespace VK
{
inline bool TERM::EqualComplexTerms(const TERM* t1,const TERM* t2)
{
  if (*t1 == *t2)
    {
      Iterator iter1;
      Iterator iter2;
      iter1.Reset(t1);
      iter2.Reset(t2);
      while (iter1.Next())
	{
	  iter2.Next();
	  if (iter1.Symbol() != iter2.Symbol()) { return false; };
	};
      return true;
    }
  else { return false; };
}; // bool TERM::EqualComplexTerms(const TERM* t1,const TERM* t2)

inline bool TERM::isSyntacticallyEqualTo(const TERM& t) const
{
  CALL("isSyntacticallyEqualTo(const TERM& t) const");
  // Here we assume that there is no sharing of term structures at all.
  if (isVariable()) return (*this) == t;
  if (t.isVariable()) return false;
  ASSERT(IsReference() && t.IsReference());
  return EqualComplexTerms(First(),t.First());
}; // bool TERM::isSyntacticallyEqualTo(const TERM& t) const


inline BK::Comparison TERM::CompareLex(const TERM* t) const
{
  CALL("CompareLex(const TERM* t) const");
  ASSERT(isComplex());
  ASSERT(t->isComplex());
  Iterator iter1(this);
  Iterator iter2(t);
  do
    {
      if (iter1.symbolRef() != iter2.symbolRef())
	{ 
	  if (iter1.CurrentSymIsVar() || iter2.CurrentSymIsVar()) return BK::Incomparable;    
	  return iter1.symbolRef().comparePrecedence(iter2.symbolRef());
	};  
      iter2.Next();
    }
  while (iter1.Next());
  return BK::Equal;
}; // BK::Comparison CompareLex(const TERM* t) const




inline bool TERM::containsSymbol(const TERM& sym)
{
  CALL("containsSymbol(const TERM& sym)");
  ASSERT(isComplex());
  Iterator iter(this);
  do
    {
      if (iter.symbolRef() == sym) return true;
    }
  while (iter.Next());
  return false;
}; // bool TERM::containsSymbol(const TERM& sym)


}; // namespace VK

namespace std
{
inline ostream& operator<<(ostream& str,const VK::TERM& t)
{
  return VK::TERM::output(str,t);
}; 

inline ostream& operator<<(ostream& str,const VK::TERM* t)
{
  return VK::TERM::output(str,t);
}; 

#ifndef NO_DEBUG_VIS
inline ostream& operator<<(ostream& str,const VK::TERM::OccList* occ) 
{
  str << '{';
  if (!occ) return str << '}';
  
  for (const VK::TERM::OccList::Node* node = occ->first(); node; node = node->next())
    {
      str << '[' << node->key() << ':';
      str << node->value() << "] ";
    };   
  return str << '}';  
};
#endif

}; // namespace std

//================================================
#undef DEBUG_NAMESPACE
#ifdef DEBUG_TERM
#define DEBUG_NAMESPACE "TERM::NonvariableWithInfo"
#endif
#include "debugMacros.hpp"
//=================================================


namespace VK
{

class TERM::NonvariableWithInfo
{
 public:   
  template <typename Info> 
    static Info& info(TERM* trm)
    {
      return *(((Info*)trm) - 1);
    };  
  template <typename Info> 
    static const Info& info(const TERM* trm)
    {
      return *(((const Info*)trm) - 1);
    };  
  template <typename Info> 
    static Info* infoAddr(TERM* trm)
    {
      return ((Info*)trm) - 1;
    };  
  template <typename Info> 
    static const Info* infoAddr(const TERM* trm)
    {
      return ((const Info*)trm) - 1;
    };  
  template <typename Info> 
    static ulong sizeWithInfo(Arity arity) 
    {
      return sizeof(Info) + sizeof(TERM)*(arity + 1);
    };
  template <typename Info> 
    static TERM* newTopLevel(Arity arity) 
    {
      return (TERM*)(((Info*)BK::GlobAlloc::allocate(sizeWithInfo<Info>(arity))) + 1);
    };
  template <typename Info> 
    static void deleteTopLevel(TERM* trm)
    {
      BK::GlobAlloc::deallocate(infoAddr<Info>(trm),sizeWithInfo<Info>(trm->arity()));
    }; 
}; // class TERM::NonvariableWithInfo


static TERM::InitStatic termInitStatic;

}; // namespace VK


//==================================================
#endif
