//
// File:         SharedLinearPolynomial.hpp
// Description:  Representation of shared linear polynomials with integer coefficients.
// Created:      May 5, 2000, Alexandre Riazanov, riazanov@cs.man.ac.uk
// Revised:      Apr 11, 2002, Alexandre Riazanov, riazanov@cs.man.ac.uk
//               LinComb renamed into SharedLinearPolynomial.
//============================================================================
#ifndef SHARED_LINEAR_POLYNOMIAL_H
//============================================================================
#define SHARED_LINEAR_POLYNOMIAL_H
#ifndef NO_DEBUG_VIS
 #include <iostream>
#endif
#include <climits>
#include "jargon.hpp"
#include "BlodKorvDebugFlags.hpp"
#ifdef DEBUG_ALLOC_OBJ_TYPE
 #include "ClassDesc.hpp"       
#endif
#ifndef NO_DEBUG
 #include "ObjDesc.hpp"
#endif  
#include "GDiscTree.hpp"
#include "Stack.hpp"
#include "DestructionMode.hpp"
//============================================================================
#undef DEBUG_NAMESPACE
#ifdef DEBUG_SHARED_LINEAR_POLYNOMIAL
 #define DEBUG_NAMESPACE "SharedLinearPolynomial<class Alloc,class CoeffType,ulong MaxSize>"
#endif
#include "debugMacros.hpp"
//============================================================================

namespace BK 
{

  template <class Alloc,class CoeffType,ulong MaxSize>
    class SharedLinearPolynomial
    {
    public:
      class Member
	{
	public:
	  Member() {};
	  Member(const CoeffType& constValue) :
	    _coefficient(constValue),
	    _variableNumber(ULONG_MAX)
	    {	      
	    };
	  Member(const CoeffType& coeff,ulong var) :
	    _coefficient(coeff),
	    _variableNumber(var)
	    {	      
	      CALL("Member::constructor Member(const CoeffType& coeff,ulong var)");
	      ASSERT(var != ULONG_MAX);
	    };
	  ~Member() {};

	  void makeConst(const CoeffType& constValue)
	    {	      
	      _coefficient = constValue;
	      _variableNumber = ULONG_MAX;
	    };
	  void makeVar(const CoeffType& coeff,ulong var)
	    {	      
	      CALL("makeVar(const CoeffType& coeff,ulong var)");
	      ASSERT(var != ULONG_MAX);
	      _coefficient = coeff;
	      _variableNumber = var;
	    };

	  const CoeffType& coeff() const 
	    {
	      CALL("Member::coeff() const");
	      ASSERT(isVariable());
	      return _coefficient;
	    };
	  ulong var() const 
	    {
	      CALL("Member::var() const");
	      ASSERT(isVariable());
	      return _variableNumber; 
	    };
	  const CoeffType& constant() const 
	    {
	      CALL("Member::constant() const");
	      ASSERT(isConstant());
	      return _coefficient; 
	    };

	  bool isVariable() const { return _variableNumber != ULONG_MAX; };
	  bool isConstant() const { return (_variableNumber == ULONG_MAX); };   
	  bool operator>(const Member& m) 
	    {
	      if (_variableNumber == m._variableNumber)
		{
		  return _coefficient > m._coefficient;
		}
	      else
		return _variableNumber > m._variableNumber;
	    };  
	  bool operator==(const Member& m) const
	  {
	    return (_variableNumber == m._variableNumber) &&
	      (_coefficient == m._coefficient);
	  };
	  bool operator>=(const Member& m) const
	    {
	      if (_variableNumber == m._variableNumber)
		{
		  return _coefficient >= m._coefficient;
		}
	      else
		return _variableNumber > m._variableNumber;
	    };
	  bool operator<(const Member& m) const
	    {
	      if (_variableNumber == m._variableNumber)
		{
		  return _coefficient < m._coefficient;
		}
	      else
		return _variableNumber < m._variableNumber;
	    };  
	  bool operator<=(const Member& m) const
	    {
	      if (_variableNumber == m._variableNumber)
		{
		  return _coefficient <= m._coefficient;
		}
	      else
		return _variableNumber < m._variableNumber;
	    };
	  

#ifndef NO_DEBUG_VIS
	  ostream& output(ostream& str) const 
	    {
	      if (isVariable())
		{
		  return str << coeff() << 'X' << var();
		}
	      else
		{
		  return str << constant();
		};
	    }; 
#endif
	private:     
	  CoeffType _coefficient;
	  ulong _variableNumber;    
	}; // class Member 
 
	
    public:
      SharedLinearPolynomial(const Member& m) : _member(m), _refCounter(0L), _tail(0) 
	{
	  CALL("constructor SharedLinearPolynomial(const Member& m)");
	};
      SharedLinearPolynomial(const Member& m,SharedLinearPolynomial* t) : _member(m), _refCounter(0L), _tail(t) 
	{
	  CALL("SharedLinearPolynomial(const Member& m,SharedLinearPolynomial* t)");
	  if (t) t->incReferenceCounter();

	}; 
      ~SharedLinearPolynomial() 
	{
	  CALL("destructor ~SharedLinearPolynomial()");
	  ASSERT(!referenceCounter());
	  if (DestructionMode::isThorough())
	    if (_tail) 
	      _tail->decReferenceCounter();
	};

      static ulong minSize() { return sizeof(SharedLinearPolynomial); };
      static ulong maxSize() { return sizeof(SharedLinearPolynomial); };
      void* operator new(size_t) 
	{
#ifdef DEBUG_ALLOC_OBJ_TYPE 
	  ALWAYS(_classDesc.registerAllocated(sizeof(SharedLinearPolynomial)));
	  return Alloc::allocate(sizeof(SharedLinearPolynomial),&_classDesc);
#else
	  return Alloc::allocate(sizeof(SharedLinearPolynomial));
#endif 
	};
      void  operator delete(void* obj) 
	{ 
#ifdef DEBUG_ALLOC_OBJ_TYPE 
	  ALWAYS(_classDesc.registerDeallocated(sizeof(SharedLinearPolynomial)));
	  Alloc::deallocate(obj,sizeof(SharedLinearPolynomial),&_classDesc); 
#else
	  Alloc::deallocate(obj,sizeof(SharedLinearPolynomial)); 
#endif 
	};
  

      const Member& hd() const 
	{
	  CALL("hd() const");
	  ASSERT(checkObj());
	  return _member; 
	};  
      const SharedLinearPolynomial* const & tl() const 
	{
	  CALL("tl() const");
	  ASSERT(checkObj());
          // Temporary res is introduced to avoid false gcc 3.0.4 warnings
	  const SharedLinearPolynomial* const & res = _tail;
	  return res; 
	};
      SharedLinearPolynomial*& tl() 
	{ 
	  CALL("tl()");
	  ASSERT(checkObj()); 
	  return _tail;
	};

      long referenceCounter() const 
	{
	  CALL("referenceCounter() const");
	  ASSERT(checkObj());  
	  return _refCounter; 
	};
      void incReferenceCounter() 
	{
	  CALL("incReferenceCounter()"); 
	  ++_refCounter;
	};
      void decReferenceCounter() 
	{
	  CALL("decReferenceCounter()");
	  --_refCounter;
	};
 
#ifndef NO_DEBUG 
      bool checkObj() const
	{
#ifndef DEBUG_ALLOC
	  return true;
#else
	  ObjDesc* objDesc = Alloc::hostObj(const_cast<void*>(static_cast<const void*>(this)));
	  if (!objDesc) 
	    { 
	      DMSG("SharedLinearPolynomial<..>::checkObj()> Null pointer to object descriptor.\n");  
	      return false; 
	    };  
	  bool res = true;
#ifdef DEBUG_ALLOC_OBJ_SIZE
	  if (objDesc->size !=  sizeof(SharedLinearPolynomial)) 
	    {
	      DMSG("SharedLinearPolynomial<..>::checkObj()> objDesc->size !=  sizeof(SharedLinearPolynomial<..>)\n");
	      res = false;
	    };
#endif
#ifdef DEBUG_ALLOC_OBJ_STATUS
	  if (objDesc->status != ObjDesc::Persistent) 
	    {	 
	      DMSG("SharedLinearPolynomial<..>::checkObj()> objDesc->status != ObjDesc::Persistent\n");
	      res = false;
	    };
#endif
#ifdef DEBUG_ALLOC_OBJ_TYPE
	  if (objDesc->classDesc != &_classDesc) 
	    {
	      DMSG("SharedLinearPolynomial<..>::checkObj()> objDesc->classDesc != &_classDesc\n");
	      res = false;
	    };
#endif
	  if (!res) DOP(DMSG("SharedLinearPolynomial<..>::checkObj()> host object ") << *objDesc << "\n"); 
	  return res;
#endif
	};
#endif

    private:
      Member _member;
      long _refCounter;
      SharedLinearPolynomial* _tail;
#ifdef DEBUG_ALLOC_OBJ_TYPE
      static ClassDesc _classDesc;
#endif
    }; // template <class Alloc,class CoeffType,ulong MaxSize> class SharedLinearPolynomial


}; // namespace BK

#ifndef NO_DEBUG_VIS

namespace std
{
template <class Alloc,class CoeffType,ulong MaxSize>
ostream& operator<<(ostream& str,const BK::SharedLinearPolynomial<Alloc,CoeffType,MaxSize>::Member& mem) 
{
  return mem.output(str);
};

template <class Alloc,class CoeffType,ulong MaxSize>
ostream& operator<<(ostream& str,const BK::SharedLinearPolynomial<Alloc,CoeffType,MaxSize>* lc) 
{
  if (!lc) return str << '0';
  while (lc)
    {
      lc->hd().output(str);
      lc = lc->tl();
    };
  return str;
};
}; // namespace std
#endif

//============================================================================
#undef DEBUG_NAMESPACE
#ifdef DEBUG_SHARED_LINEAR_POLYNOMIAL
#define DEBUG_NAMESPACE "LinearPolynomialSharing<class Alloc,class CoeffType,ulong MaxSharedLinearPolynomialSize>"
#endif
#include "debugMacros.hpp"
//============================================================================

namespace BK 
{
  template <class Alloc,class CoeffType,ulong MaxSharedLinearPolynomialSize>
    class LinearPolynomialSharing
    {
    public:
      // DiscTree cannot be made private since it has nested classes 
      // with static data members
      typedef GDiscTree<Alloc,SharedLinearPolynomial<Alloc,CoeffType,MaxSharedLinearPolynomialSize>::Member,SharedLinearPolynomial<Alloc,CoeffType,MaxSharedLinearPolynomialSize> > DiscTree;
      

    public:
      LinearPolynomialSharing() 
	: _members("_members"), 
	_linPolynomials("_linPolynomials")
	{
	  CALL("constructor LinearPolynomialSharing()");
	};
      ~LinearPolynomialSharing() 
	{ 
	  CALL("destructor ~LinearPolynomialSharing()");
	};

      void init()
      {
	CALL("init()");
	_discTree.init();
	_members.init("_members");
	_linPolynomials.init("_linPolynomials");
      };

      void destroy()
	{
	  CALL("destroyThoroughly()");
	  _linPolynomials.destroy();
	  _members.destroy();
	  _discTree.destroy();
	};
  
      void resetIntegration() 
	{
	  _members.reset();
	};

      void integrConst(const CoeffType& constValue)
	{
	  _members.next().makeConst(constValue);
	  _members.push();
	};

      void integrVar(const CoeffType& coeff,ulong var)
	{
	  _members.next().makeVar(coeff,var);
	  _members.push();
	};


      SharedLinearPolynomial<Alloc,CoeffType,MaxSharedLinearPolynomialSize>* integrate() 
	{
	  CALL("integrate()");
	  if (!_members) return 0;
	  SharedLinearPolynomial<Alloc,CoeffType,MaxSharedLinearPolynomialSize>** indObj;
	  const SharedLinearPolynomial<Alloc,CoeffType,MaxSharedLinearPolynomialSize>::Member* first;
	  SharedLinearPolynomial<Alloc,CoeffType,MaxSharedLinearPolynomialSize>* linPolynomial;
	  _linPolynomials.reset(); 
	  for (first = _members.begin(); first != _members.end(); first++)
	    {
	      indObj = integrateIntoIndex(first);
	      linPolynomial = *indObj;     
	      if (linPolynomial) 
		{ 
		  goto link_created_nodes; 
		};
	      linPolynomial = new SharedLinearPolynomial<Alloc,CoeffType,MaxSharedLinearPolynomialSize>(*first);   
	      *indObj = linPolynomial;
	      linPolynomial->incReferenceCounter();
	      ASSERT(linPolynomial->referenceCounter() == 1L); // only in the index 
	      _linPolynomials.push(linPolynomial);
	    };     

	  linPolynomial = _linPolynomials.pop();
	  ASSERT(linPolynomial->referenceCounter() == 1L); // only in the index 
	  ASSERT(!linPolynomial->tl());

	link_created_nodes:
	  SharedLinearPolynomial<Alloc,CoeffType,MaxSharedLinearPolynomialSize>* tmp = linPolynomial;    
	  while (_linPolynomials)
	    {
	      linPolynomial = _linPolynomials.pop();
	      linPolynomial->tl() = tmp;
	      tmp->incReferenceCounter();
	      tmp = linPolynomial;
	    };
	  return linPolynomial;
	}; // SharedLinearPolynomial* integrate() 

      void remove(SharedLinearPolynomial<Alloc,CoeffType,MaxSharedLinearPolynomialSize>* linPolynomial)
	{
	  CALL("remove(SharedLinearPolynomial<Alloc,CoeffType,MaxSharedLinearPolynomialSize>* linPolynomial)");
	  ASSERT(linPolynomial->referenceCounter() == 1L); // only in the index
	next:
	  removeFromIndex(linPolynomial);
	  ASSERT(!linPolynomial->referenceCounter());
	  SharedLinearPolynomial<Alloc,CoeffType,MaxSharedLinearPolynomialSize>* tmp = linPolynomial->tl();
	  delete linPolynomial; // decrements tmp->referenceCounter()!
	  linPolynomial = tmp;
	  if (linPolynomial && (linPolynomial->referenceCounter() == 1L))
	    goto next;      
	}; // void remove(SharedLinearPolynomial<Alloc,CoeffType,MaxSharedLinearPolynomialSize>* linPolynomial) 

    private: 
      SharedLinearPolynomial<Alloc,CoeffType,MaxSharedLinearPolynomialSize>** integrateIntoIndex(const SharedLinearPolynomial<Alloc,CoeffType,MaxSharedLinearPolynomialSize>::Member* first)
	{
	  CALL("integrateIntoIndex(const SharedLinearPolynomial<Alloc,CoeffType,MaxSharedLinearPolynomialSize>::Member* first)");
	  _discTree.integrator().reset(); 
	  while (first != _members.end())
	    {
	      _discTree.integrator().skipOrInsert(*first);
	      first++;
	    };
	  return _discTree.integrator().indexedObjAddr();
	}; // SharedLinearPolynomial<Alloc,CoeffType,MaxSharedLinearPolynomialSize>** integrateIntoIndex(const SharedLinearPolynomial<Alloc,CoeffType,MaxSharedLinearPolynomialSize>::Member* first)


      bool removeFromIndex(SharedLinearPolynomial<Alloc,CoeffType,MaxSharedLinearPolynomialSize>* const linPolynomial)
	{
	  CALL("removeFromIndex(SharedLinearPolynomial<Alloc,CoeffType,MaxSharedLinearPolynomialSize>* linPolynomial)");
	  ASSERT(linPolynomial->referenceCounter() == 1L); // only in the index
	  _discTree.remover().reset();
	  SharedLinearPolynomial<Alloc,CoeffType,MaxSharedLinearPolynomialSize>* tmp = linPolynomial;
	  while (tmp) 
	    {
	      if (!_discTree.remover().skip(tmp->hd())) return false;
	      tmp = tmp->tl();
	    };
	  SharedLinearPolynomial<Alloc,CoeffType,MaxSharedLinearPolynomialSize>** indObj = _discTree.remover().indexedObjAddr();
	  if ((!indObj) || (*indObj != linPolynomial)) return false;
	  _discTree.remover().destroyBranch();
	  linPolynomial->decReferenceCounter();
	  return true;
	}; // bool removeFromIndex(SharedLinearPolynomial<Alloc,CoeffType,MaxSharedLinearPolynomialSize>* linPolynomial)

    private:      
      DiscTree _discTree;
      Stack<SharedLinearPolynomial<Alloc,CoeffType,MaxSharedLinearPolynomialSize>::Member,MaxSharedLinearPolynomialSize> _members;
      Stack<SharedLinearPolynomial<Alloc,CoeffType,MaxSharedLinearPolynomialSize>*,MaxSharedLinearPolynomialSize> _linPolynomials; 
    }; // class LinearPolynomialSharing


}; // namespace BK





//============================================================================
#endif
