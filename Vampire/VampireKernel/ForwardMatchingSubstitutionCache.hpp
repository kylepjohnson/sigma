//
// File:         ForwardMatchingSubstitutionCache.hpp
// Description:  Fast mapping of clause variables
//               into their instances from a substitution
//               collected by forward matching.
// Created:      Apr 28, 2002, Alexandre Riazanov, riazanov@cs.man.ac.uk
//===================================================================
#ifndef FORWARD_MATCHING_SUBSTITUTION_CACHE_H
#define FORWARD_MATCHING_SUBSTITUTION_CACHE_H
//===================================================================
#ifndef NO_DEBUG_VIS
#  include <iostream>
#endif
#include "jargon.hpp"
#include "VampireKernelDebugFlags.hpp"
#include "VampireKernelConst.hpp"
#include "Flatterm.hpp"
#include "Term.hpp"

#ifndef NO_DEBUG
#  include "LinearCombination.hpp"
#endif
//===================================================================
#undef DEBUG_NAMESPACE
#ifdef DEBUG_FORWARD_MATCHING_SUBSTITUTION_CACHE
 #define DEBUG_NAMESPACE "ForwardMatchingSubstitutionCache"
#endif
#include "debugMacros.hpp"
//===================================================================

namespace VK
{
class ForwardMatchingSubstitutionCache
{
 public: 
  class Instance;
 public:
  ForwardMatchingSubstitutionCache() {};
  ~ForwardMatchingSubstitutionCache() {};
  void init()
  {
#if (defined DEBUG_NAMESPACE) || (!(defined NO_DEBUG_VIS))
    _debugVariables.init();
#endif
  };
  void destroy()
  {
#if (defined DEBUG_NAMESPACE) || (!(defined NO_DEBUG_VIS))
    _debugVariables.destroy();
#endif
  };
  void reset() 
    {
      DOP(_debugVariables.reset());
    };
  void associate(ulong var,const Flatterm* term)
    {
      CALL("associate(ulong var,const Flatterm* term)");
      ASSERT(!_debugVariables.coefficient(var));
      _subst[var] = term;
      DOP(_debugVariables.add1(var));
    };
  void endOfCollection() {};
  const Flatterm* map(ulong var) const 
    {
      CALL("map(ulong var) const");
      ASSERT(_debugVariables.coefficient(var));
      return _subst[var]; 
    };

#ifndef NO_DEBUG_VIS
  ostream& output(ostream& str) const
    {
      for (const ulong* v = _debugVariables.begin(); v < _debugVariables.end(); ++v)
	str << "X" << *v << " ---> " << map(*v) << "\n";
      return str;
    };
#endif

 private:
  const Flatterm* _subst[VampireKernelConst::MaxNumOfVariables];
#if (defined DEBUG_NAMESPACE) || (!(defined NO_DEBUG_VIS))
  BK::LinearCombination<long,VampireKernelConst::MaxNumOfVariables> _debugVariables;
#endif
}; // class ForwardMatchingSubstitutionCache

}; // namespace VK

#ifndef NO_DEBUG_VIS
namespace std
{
inline
ostream& operator<<(ostream& str,const VK::ForwardMatchingSubstitutionCache* subst)
{
  return subst->output(str);
};
};
#endif

//===================================================================
#undef DEBUG_NAMESPACE
#ifdef DEBUG_FORWARD_MATCHING_SUBSTITUTION_CACHE
 #define DEBUG_NAMESPACE "ForwardMatchingSubstitutionCache::Instance"
#endif
#include "debugMacros.hpp"
//===================================================================

namespace VK
{
class ForwardMatchingSubstitutionCache::Instance
{
 public:
  Instance() : 
    _dummyTerm((TERM::Arity)0,(TERM::Functor)0,TERM::PositivePolarity)  
    {
    };
  ~Instance() {};
  void init()
  {
    _substIter.init();
    _dummyTerm.init((TERM::Arity)0,(TERM::Functor)0,TERM::PositivePolarity);
  };
  void reset(const TERM& t,const ForwardMatchingSubstitutionCache* subst);
  void reset(const TERM* t,const ForwardMatchingSubstitutionCache* subst);
  const TERM& symbol() const;
  bool next();
  bool after();
 private:
  const ForwardMatchingSubstitutionCache* _subst;
  TERM::Iterator _topLevel;
  Flatterm::ConstIterator _substIter;
  bool _onTop;
  TERM _dummyTerm;
}; // class ForwardMatchingSubstitutionCache::Instance

}; // namespace VK
//===================================================================
#undef DEBUG_NAMESPACE
#ifdef DEBUG_FORWARD_MATCHING_SUBSTITUTION_CACHE
 #define DEBUG_NAMESPACE "ForwardMatchingSubstitutionCache::Instance"
#endif
#include "debugMacros.hpp"
//===================================================================

namespace VK
{
inline
void 
ForwardMatchingSubstitutionCache::Instance::reset(const TERM& t,
						  const ForwardMatchingSubstitutionCache* subst)
{
  CALL("reset(const TERM& t,const ForwardMatchingSubstitutionCache* subst)");
  if (t.isVariable())
    {
      _subst = subst;
      _substIter.Reset(subst->map(t.var()));
      _topLevel.Reset(&_dummyTerm);
      NEVER(_topLevel.Next());
      _onTop = false;
    }
  else
    {
      ASSERT(t.IsReference());
      reset(t.First(),subst);
    };
}; // void ForwardMatchingSubstitutionCache::Instance::reset(const TERM& t,const ForwardMatchingSubstitutionCache* subst)

inline
void 
ForwardMatchingSubstitutionCache::Instance::reset(const TERM* t,
						  const ForwardMatchingSubstitutionCache* subst)
{
  CALL("reset(const TERM* t,const ForwardMatchingSubstitutionCache* subst)");
  ASSERT(t->isComplex());
  _subst = subst;
  _topLevel.Reset(t);
  _onTop = true;
}; // void ForwardMatchingSubstitutionCache::Instance::reset(const TERM* t,const ForwardMatchingSubstitutionCache* subst)


inline const TERM& ForwardMatchingSubstitutionCache::Instance::symbol() const
{
  CALL("symbol() const");
  if (_onTop) return _topLevel.symbolRef();
  ASSERT(!_substIter.EndOfTerm());
  return _substIter.CurrPos()->symbolRef();
}; // const TERM& ForwardMatchingSubstitutionCache::Instance::symbol() const

inline bool ForwardMatchingSubstitutionCache::Instance::next()
{
  CALL("next()");
  if (_onTop)
    {
    next_on_top:
      if (!_topLevel.Next()) return false;
      if (_topLevel.CurrentSymIsVar())
	{
	  _onTop = false;
	  _substIter.Reset(_subst->map(_topLevel.symbolRef().var()));
	};
      return true;
    }
  else // inside a substitution term
    {
      _substIter.NextSym();
      if (!_substIter.EndOfTerm()) return true;
      _onTop = true;
      goto next_on_top;
    };
}; // bool ForwardMatchingSubstitutionCache::Instance::next()



inline bool ForwardMatchingSubstitutionCache::Instance::after()
{
  CALL("after()");
  if (_onTop)
    {
    next_on_top:
      if (!_topLevel.SkipTerm()) return false;
      if (_topLevel.CurrentSymIsVar())
	{
	  _onTop = false;
	  _substIter.Reset(_subst->map(_topLevel.symbolRef().var()));
	};
      return true;
    }
  else // inside a substitution term
    {
      _substIter.after();
      if (!_substIter.EndOfTerm()) return true;
      _onTop = true;
      goto next_on_top;
    };
}; // bool ForwardMatchingSubstitutionCache::Instance::after()


}; // namespace VK



//===================================================================
#endif

