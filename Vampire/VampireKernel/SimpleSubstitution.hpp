//
// File:         SimpleSubstitution.hpp
// Description:  Simple representation of substitutions for variables without indexes.
// Created:      Apr 12, 2000, 21:20
// Author:       Alexandre Riazanov
// mail:         riazanov@cs.man.ac.uk
//================================================
#ifndef NO_DEBUG
#ifndef SIMPLE_SUBSTITUTION_H
//================================================
#define SIMPLE_SUBSTITUTION_H
#ifndef NO_DEBUG_VIS
 #include <iostream>
#endif
#include "jargon.hpp"
#include "VampireKernelDebugFlags.hpp"
#include "VampireKernelConst.hpp"
#include "Stack.hpp"
#include "Term.hpp"
//================================================
#undef DEBUG_NAMESPACE
#ifdef DEBUG_SIMPLE_SUBSTITUTION
 #define DEBUG_NAMESPACE "SimpleSubstitution"
#endif 
#include "debugMacros.hpp"
//================================================

namespace VK
{
class SimpleSubstitution
{
 public: 
  class TermTraversal;
 public:
  SimpleSubstitution() : _registered("_registered")
  {
   for (ulong v = 0; v < VampireKernelConst::MaxNumOfVariables; v++) _assigned[v] = false;
  };
  ~SimpleSubstitution() {};
  void init()
  {
    CALL("init()");
    _registered.init("_registered");
    for (ulong v = 0; v < VampireKernelConst::MaxNumOfVariables; v++) _assigned[v] = false;
  };
  void destroy()
  {
    CALL("destroy()");
    _registered.destroy();
  };

  bool assigned(ulong var) const { return _assigned[var]; }; 
  void reset()
  {
   CALL("reset()");
   while (_registered) _assigned[_registered.pop()] = false;
  };
  void nextPair(ulong var,const TERM& term)
  {
   CALL("nextPair(ulong var,const TERM& term)");
   ASSERT(!_assigned[var]);
   _assigned[var] = true; 
   _registered.push(var);
   _term[var] = term;
  };
  bool assignment(ulong var,TERM& term) const
  {
   term = _term[var];
   return _assigned[var];
  };

  #ifndef NO_DEBUG_VIS
   ostream& output(ostream& str) const
   {
    for (const ulong* v = _registered.begin(); v < _registered.end(); v++)
     {  
      str << 'X' << *v << " --> " << _term[*v] << "  ";
     };
    return str;
   };
  #endif

 private:
  TERM _term[VampireKernelConst::MaxNumOfVariables];
  bool _assigned[VampireKernelConst::MaxNumOfVariables];
  BK::Stack<ulong,VampireKernelConst::MaxNumOfVariables> _registered;       
}; // class SimpleSubstitution

}; // namespace VK
//================================================
#undef DEBUG_NAMESPACE
#ifdef DEBUG_SIMPLE_SUBSTITUTION
 #define DEBUG_NAMESPACE "SimpleSubstitution::TermTraversal"
#endif 
#include "debugMacros.hpp"
//================================================
namespace VK
{
class SimpleSubstitution::TermTraversal
{
 public: 
  typedef TERM::Traversal::State State;
 public:
  TermTraversal(SimpleSubstitution* s) 
   : _subst(s), 
     _topTraversal(DOP("SimpleSubstitution::TermTraversal::_topTraversal")), 
     _substTraversal(DOP("SimpleSubstitution::TermTraversal::_substTraversal")) 
   {
   };
  TermTraversal(SimpleSubstitution* s,const TERM& t) 
   : _subst(s), 
     _topTraversal(DOP("SimpleSubstitution::TermTraversal::_topTraversal")), 
     _substTraversal(DOP("SimpleSubstitution::TermTraversal::_substTraversal"))  
  {
   reset(t);
  };
  ~TermTraversal() {};
  void init(SimpleSubstitution* s) 
  {
    CALL("init(SimpleSubstitution* s)");
    _subst = s; 
    _topTraversal.init(DOP("SimpleSubstitution::TermTraversal::_topTraversal"));
    _substTraversal.init(DOP("SimpleSubstitution::TermTraversal::_substTraversal")); 
  };

  void init(SimpleSubstitution* s,const TERM& t) 
  {
    CALL("init(SimpleSubstitution* s,const TERM& t)");
    init(s);
    reset(t);
  };

  void destroy()
  {
    CALL("destroy()");
    _substTraversal.destroy();
    _topTraversal.destroy();
  };

  void setSubst(SimpleSubstitution* s) { _subst = s; };
  bool reset(const TERM& t) 
  {
   CALL("reset(const TERM& t)");
   _topTraversal.reset(t); 
   _top = !(t.isVariable());
   if (!_top)
    {
     TERM substTerm;
     if (!_subst->assignment(t.var(),substTerm)) return false;
     COP("COP10");
     _substTraversal.reset(substTerm);
    }; 
   return true;
  };
  const State& state() const 
  { 
   return ((_top) ? _topTraversal.state() : _substTraversal.state());
  };
  operator bool() const { return _topTraversal.state() != TERM::Traversal::End; };
  bool next() 
  {
   CALL("next()");
   if (_top)
    { 
     top_next:
      _topTraversal.next();    
      if (_topTraversal.state() == TERM::Traversal::Var)
       {
        _top = false;       
        TERM substTerm;
        if (!_subst->assignment(_topTraversal.symbol().var(),substTerm)) return false;
        _substTraversal.reset(substTerm);
       };     
    }
   else // inside a substitution term 
    {
     ASSERT(_topTraversal.state() == TERM::Traversal::Var);
     _substTraversal.next();    
     if (_substTraversal.state() == TERM::Traversal::End)
      { 
       _top = true;
       goto top_next;
      };
    };
   return true;
  }; // bool next()  

  bool after()
  {
   CALL("after()");
   if (_top)
    {
     top_after: 
      _topTraversal.after(); 
      if (_topTraversal.state() == TERM::Traversal::Var)
       {
        _top = false;       
        TERM substTerm;
        if (!_subst->assignment(_topTraversal.symbol().var(),substTerm)) return false;
        _substTraversal.reset(substTerm);
       };
    } 
   else
    {
     ASSERT(_topTraversal.state() == TERM::Traversal::Var);
     _substTraversal.after();     
     if (_substTraversal.state() == TERM::Traversal::End)
      { 
       _top = true;
       goto top_after;
      };
    };
   return true;
  }; // bool after() 
 
  const TERM& symbol() const { return ((_top) ? _topTraversal.symbol() : _substTraversal.symbol()); };
 
 private:
  SimpleSubstitution* _subst;
  TERM::Traversal _topTraversal;
  TERM::Traversal _substTraversal;
  bool _top;
}; // class SimpleSubstitution::TermTraversal

}; // namespace VK


//================================================
#undef DEBUG_NAMESPACE
//================================================
#endif
#endif
