//
// File:         ClauseBackground.hpp
// Description:  Ancestors of a clause.
// Created:      Feb 12, 2000, 20:10
// Revised:      Feb 1, 2001, AR, 
//               representation changed from arrays to lists
// Author:       Alexandre Riazanov
// mail:         riazanov@cs.man.ac.uk 
//==================================================================
#ifndef  ClauseBackground_H
//==================================================================
#define ClauseBackground_H
#include <iostream>
#include "jargon.hpp"
#include "VampireKernelDebugFlags.hpp"
#ifdef DEBUG_ALLOC_OBJ_TYPE
#  include "ClassDesc.hpp"
#endif
#ifndef NO_DEBUG
#  include "ObjDesc.hpp"
#endif
#include "GlobAlloc.hpp"
#include "ExpandingStack.hpp"
#include "BitWord.hpp"
#include "GList.hpp"
#include "DestructionMode.hpp"
namespace VK
{
  class Clause;
}; // namespace VK
using namespace std;
//===================================================================
#undef DEBUG_NAMESPACE
#ifdef DEBUG_CLAUSE_BACKGROUND
#define DEBUG_NAMESPACE "ClauseBackground"
#endif
#include "debugMacros.hpp"
//===================================================================
namespace VK
{
  class ClauseBackground
    {
    public:
      enum Rule 
      { 
	Input = 0, 
	Prepro = 1, 
	BinRes = 2, 
	ForwSup = 3,
	BackSup = 4,
	EqRes = 5,
	EqFact = 6, 
	ForwDemod = 7,
	BackDemod = 8,
	EqResSimp = 9,
	ForwSubsRes = 10,
	BackSubsRes = 11,     
	Split = 12,
	Reanimation = 13,
	Name = 14,
	NegSel = 15,
	SimplificationByBuiltInTheories = 16,
	Support = 28,
	BuiltInTheoryFact = 29,
	Subgoal = 30,
	VIP = 31
      };
      typedef BK::BitWord Rules; 
      typedef BK::GList<BK::GlobAlloc,Clause*,ClauseBackground> ClauseList;
    public:
      ClauseBackground() : 
	_ancestors(0),
	_inferenceDepth(0L)
	{
	};
      ~ClauseBackground()
	{
	  CALL("desctructor ~ClauseBackground()");
	  if (BK::DestructionMode::isThorough()) 
	    {
	      if (!isInput()) ClauseList::destroyList(_ancestors);
	    };
	}; 

      void init()
      {	
	_rules.init();
	_ancestors = 0;
	_inferenceDepth = 0L;
      };
      
      void destroy()
      {
	CALL("destroy()");
	if (BK::DestructionMode::isThorough()) 
	  {
	    if (!isInput()) ClauseList::destroyList(_ancestors);
	  };
	_rules.destroy();
      };

      const Rules& properties() const { CALL("properties() const");  return _rules; };
      void SetRules(const Rules& r) { CALL("SetRules(Rules r");  _rules = r; };
      void MakeVIP() { CALL("MakeVIP()");  _rules.set(VIP); };
      bool IsVIP() const { CALL("MakeVIP()");  return _rules.bit(VIP); };
      bool isInput() const { return _rules.bit(Input); };
      void MarkAsName() { CALL("MarkAsName()");  _rules.set(Name); };
      bool isSubgoal() const { return _rules.bit(Subgoal); };
      void markAsSubgoal() { _rules.set(Subgoal); };
      bool isBuiltInTheoryFact() const { return _rules.bit(BuiltInTheoryFact); };
      void markAsBuiltInTheoryFact() { _rules.set(BuiltInTheoryFact); };
      bool isSupport() const { return _rules.bit(Support); };
      void markAsSupport() { _rules.set(Support); };

      bool NegativeSelection() const
	{ 
	  CALL("NegativeSelection() const"); 
	   
	  return _rules.bit(NegSel); 
	};
      void SetNegativeSelection(bool fl) 
	{
	  CALL("SetNegativeSelection(bool fl)");
	  
	  if (fl) { _rules.set(NegSel); } else _rules.clear(NegSel);
	}; 
      ClauseList* ancestors() 
	{
	  CALL("ancestors()");
	  ASSERT(!isInput());
	  return _ancestors; 
	};
      const ClauseList* ancestors() const 
	{
	  CALL("ancestors() const");
	  ASSERT(!isInput());
	  return _ancestors; 
	};
      void* inputClauseOrigin() const
	{
	  CALL("inputClauseOrigin() const");
	  ASSERT(isInput());
	  return _inputClauseOrigin; 
	};
      
      void setInputClauseOrigin(void* orig)
	{
	  CALL("setInputClauseOrigin()");
	  ASSERT(isInput());
	  _inputClauseOrigin = orig; 
	};
	
      long inferenceDepth() const { return _inferenceDepth; };
      void setInferenceDepth(long d) { _inferenceDepth = d; };

      void copyTo(ClauseBackground& copy) const;
      static ostream& outputRules(ostream& str,const BK::BitWord& _rules);
    private:     
      void init(const Rules& r,ClauseList* clauses,long inferenceDepth)
	{
	  _rules = r;
	  _ancestors = clauses;
	  _inferenceDepth = inferenceDepth;
	};
    private:
      BK::BitWord _rules;
      union
      {
	ClauseList* _ancestors;
	void* _inputClauseOrigin;
      };
      long _inferenceDepth;
#ifdef DEBUG_ALLOC_OBJ_TYPE
      static BK::ClassDesc _classDesc;
#endif
      friend class OpenClauseBackground;
    }; // class ClauseBackground
}; // namespace VK



//========================================
#ifdef DEBUG_CLAUSE_BACKGROUND
#undef DEBUG_NAMESPACE
#define DEBUG_NAMESPACE "OpenClauseBackground"
#endif
#include "debugMacros.hpp"
//========================================


namespace VK
{
  class OpenClauseBackground
    {
    public:
      OpenClauseBackground() 
	: _ancestors(DOP("OpenClauseBackground::_ancestors"))
	{
	  DOP(_ancestors.freeze());
	};
      ~OpenClauseBackground() {};
      void init()
      {
	_rules.init();
	_ancestors.init(DOP("OpenClauseBackground::_ancestors"));
      };

      void destroy()
      {
	_ancestors.destroy();
	_rules.destroy();
      };

      void Reset() 
	{ 
	  _rules.clear();
	  _ancestors.reset(); 
	  _inferenceDepth = 0L;
	};
      void MakeVIP() { _rules.set(ClauseBackground::VIP); };
      void UsedRule(ClauseBackground::Rule r) { _rules.set(r); };
      void markAsSubgoal() { _rules.set(ClauseBackground::Subgoal); };
      bool isSupport() const { return _rules.bit(ClauseBackground::Support); };
      void markAsSupport() { _rules.set(ClauseBackground::Support); };
      void CancelRule(ClauseBackground::Rule r) { _rules.clear(r); };
      void UsedRules(const ClauseBackground::Rules& rs) { _rules |= rs; };
      void pushAncestor(Clause* cl,long depth) 
	{
	  CALL("pushAncestor(Clause* cl)");
	  DOP(_ancestors.unfreeze());
	  _ancestors.pushSafe(cl);
	  DOP(_ancestors.freeze()); 
	  if (depth + 1 > _inferenceDepth) _inferenceDepth = depth + 1;
	};
      void InverseOrder();
      void writeNewBackgroundTo(ClauseBackground& b);
      const BK::BitWord& Rules() const { return _rules; };
      ulong numberOfAncestors() const { return _ancestors.size(); }; 

      Clause* const * begin() const { return _ancestors.begin(); };
      Clause* const * end() const { return _ancestors.end(); };
      Clause* const * last() const { return _ancestors.last(); };

      bool singletone() const { return _ancestors.size() == 1; }; 

      long inferenceDepth() const { return _inferenceDepth; };

    private:
      ClauseBackground::Rules _rules;
      BK::ExpandingStack<BK::GlobAlloc,Clause*,128UL,OpenClauseBackground> _ancestors;
      long _inferenceDepth;
    }; // OpenClauseBackground
}; // namespace VK
//=================================================================
#undef DEBUG_NAMESPACE   
//==========================================================================
#endif
