//
// File:         InstanceRetrievalForDemodulation.hpp
// Description:  Indexing for matching in backward demodulation.
// Created:      May 09, 2002, Alexandre Riazanov, riazanov@cs.man.ac.uk
//====================================================
#ifndef INSTANCE_RETRIEVAL_FOR_DEMODULATION_H
#define INSTANCE_RETRIEVAL_FOR_DEMODULATION_H
//====================================================
#ifndef NO_DEBUG_VIS
#include <iostream>
#endif   
#include "jargon.hpp"
#include "VampireKernelConst.hpp"
#include "VampireKernelDebugFlags.hpp"
#include "Term.hpp"
#include "GlobAlloc.hpp"
#include "PathIndexingWithJoinsAndEarlyCleanUpForInstanceRetrieval.hpp"
//====================================================
#undef DEBUG_NAMESPACE
#ifdef DEBUG_INSTANCE_RETRIEVAL_FOR_DEMODULATION
 #define DEBUG_NAMESPACE "InstRet"
#endif
#include "debugMacros.hpp"
//====================================================

namespace VK
{
class InstRet
{ 
 private:
  class Term
    {
    public:
      typedef TERM Symbol;
      typedef TERM Id;
      typedef TERM::Traversal Traversal;
      typedef TERM::Arguments Arguments;
    };
public:
  // Cannot make it private since it has a static data member
  typedef Gem::PathIndexingJECFIR<BK::GlobAlloc,Term,VampireKernelConst::MaxTermDepth,VampireKernelConst::MaxTermSize,VampireKernelConst::MaxNumOfVariables> Index;
 public:
  class Substitution
    {
    public:
      class Instance;
    public:
      const TERM& map(ulong var) const 
	{ 
	  return ((const Index*)this)->subst(var);
	};
      const TERM* nativeVar(ulong varNum) const
	{ 
	  return (static_cast<const Index*>(static_cast<const void*>(this)))->nativeVar(varNum);
	};

#ifndef NO_DEBUG_VIS
      ostream& output(ostream& str) const
	{
	  return (static_cast<const Index*>(static_cast<const void*>(this)))->outputSubst(str);
	};
#endif      

    private:
      Substitution() {};
      ~Substitution() {};
      friend class InstRet;
    };
 public:
  InstRet() {};
  ~InstRet() {};
  void init()
  {
    _index.init();
  };
  void destroy()
  {
    CALL("destroy()");
    _index.destroy();
    BK_CORRUPT(*this);
  };
  void integrate(const TERM* complexTerm);
  void remove(const TERM* complexTerm);
  bool newQuery(const TERM* query);
  bool nextMatch(const TERM*& match);
  const Substitution* substitution() const
    {
      CALL("substitution() const");
      ASSERT(!_noMoreMatches);
      return static_cast<const Substitution*>(static_cast<const void*>(&_index));
    };


 private:
#ifdef DEBUG_NAMESPACE
  bool isValidMatch(const TERM* term) const;
#endif

 private:
  Index _index;
  bool _noMoreMatches;
  DOP(const TERM* _debugQuery);
}; // class InstRet
}; // namespace VK

#ifndef NO_DEBUG_VIS
namespace std
{
  inline ostream& operator<<(ostream& str,const VK::InstRet::Substitution* subst)
  {
    return subst->output(str);
  };
};
#endif 


//====================================================
#undef DEBUG_NAMESPACE
#ifdef DEBUG_INSTANCE_RETRIEVAL_FOR_DEMODULATION
 #define DEBUG_NAMESPACE "InstRet::Substitution::Instance"
#endif
#include "debugMacros.hpp"
//====================================================
namespace VK
{

class InstRet::Substitution::Instance
{
 public:
  Instance() : 
    _dummyTerm0((TERM::Arity)0,(TERM::Functor)0,TERM::PositivePolarity)  
    {
      _dummyTerm1[0].MakeComplex((TERM::Arity)1,(TERM::Functor)0,TERM::PositivePolarity);
    };
  ~Instance() {};
  
  void init()
  {
    CALL("init()");
    _topLevel.init();
    _substIter.init();
    _dummyTerm0.init((TERM::Arity)0,(TERM::Functor)0,TERM::PositivePolarity); 
    _dummyTerm1[0].MakeComplex((TERM::Arity)1,(TERM::Functor)0,TERM::PositivePolarity);
  };

  void destroy()
  {
    CALL("destroy()");
    _substIter.destroy();
    _topLevel.destroy();
  };

  void reset(const TERM& t,const InstRet::Substitution* subst);
  void reset(const TERM* t,const InstRet::Substitution* subst);
  const TERM& symbol() const;
  bool next();
 private:
  const InstRet::Substitution* _subst;
  TERM::Iterator _topLevel;
  TERM::Iterator _substIter;
  bool _onTop;
  TERM _dummyTerm0;
  TERM _dummyTerm1[2];
}; // class InstRet::Substitution::Instance

}; // namespace VK


//====================================================
#undef DEBUG_NAMESPACE
#ifdef DEBUG_INSTANCE_RETRIEVAL_FOR_DEMODULATION
 #define DEBUG_NAMESPACE "InstRet::Substitution::Instance"
#endif
#include "debugMacros.hpp"
//====================================================
 
namespace VK
{

inline
void 
InstRet::Substitution::Instance::reset(const TERM& t,
								const InstRet::Substitution* subst)
{
  CALL("reset(const TERM& t,const InstRet::Substitution* subst)");
  if (t.isVariable())
    {
      _subst = subst;
      if (subst->map(t.var()).isVariable())
	{
	  _dummyTerm1[1] = subst->map(t.var());
	  _substIter.Reset(_dummyTerm1);
	  ALWAYS(_substIter.Next());
	}
      else
	{
	  ASSERT(subst->map(t.var()).IsReference());
	  _substIter.Reset(subst->map(t.var()).First());
	};
      _topLevel.Reset(&_dummyTerm0);
      NEVER(_topLevel.Next());
      _onTop = false;
    }
  else
    {
      ASSERT(t.IsReference());
      reset(t.First(),subst);
    };
}; // void InstRet::Substitution::Instance::reset(const TERM& t,const InstRet::Substitution* subst)

inline
void 
InstRet::Substitution::Instance::reset(const TERM* t,
						  const InstRet::Substitution* subst)
{
  CALL("reset(const TERM* t,const InstRet::Substitution* subst)");
  ASSERT(t->isComplex());
  _subst = subst;
  _topLevel.Reset(t);
  _onTop = true;
}; // void InstRet::Substitution::Instance::reset(const TERM* t,const InstRet::Substitution* subst)


inline const TERM& InstRet::Substitution::Instance::symbol() const
{
  CALL("symbol() const");
  if (_onTop) return _topLevel.symbolRef();
  return _substIter.symbolRef();
}; // const TERM& InstRet::Substitution::Instance::symbol() const

inline bool InstRet::Substitution::Instance::next()
{
  CALL("next()");
  if (_onTop)
    {
    next_on_top:
      if (!_topLevel.Next()) 
	{
	  return false;
	};
      if (_topLevel.CurrentSymIsVar())
	{
	  _onTop = false;
	  if (_subst->map(_topLevel.symbolRef().var()).isVariable())
	    {
	      _dummyTerm1[1] = _subst->map(_topLevel.symbolRef().var());
	      _substIter.Reset(_dummyTerm1);
	      ALWAYS(_substIter.Next());
	    }
	  else
	    {
	      ASSERT(_subst->map(_topLevel.symbolRef().var()).IsReference());
	      _substIter.Reset(_subst->map(_topLevel.symbolRef().var()).First());
	    };
	};
      return true;
    }
  else // inside a substitution term
    {
      if (_substIter.Next())
	{
	  return true;
	};
      _onTop = true;
      goto next_on_top;
    };
}; // bool InstRet::Substitution::Instance::next()

}; // namespace VK








//====================================================
#endif
