//
// File:         variables.hpp
// Description:  Various classes for dealing with variables and substitutions.
// Created:      Feb 15, 2000, 19:40
// Author:       Alexandre Riazanov
// mail:         riazanov@cs.man.ac.uk
//====================================================
#ifndef VARIABLES_H
//===================================================================
#define VARIABLES_H
#ifndef NO_DEBUG_VIS
#include <iostream>
#endif
#include "jargon.hpp"
#include "VampireKernelDebugFlags.hpp"
#include "VampireKernelConst.hpp"
#include "MultisetOfVariables.hpp"
#include "Comparison.hpp"
#include "Stack.hpp"
#include "WeightPolynomial.hpp"
//================================================
#undef DEBUG_NAMESPACE
#ifdef DEBUG_VARIABLES
#  define DEBUG_NAMESPACE "Unifier"
#endif
#include "debugMacros.hpp"
//=================================================
namespace VK
{

  class PrefixSym;
  class Unifier
    {
    public:
  
      class Variable
	{

	private: // structure
	  void* binding;
	public:
	  Variable() : binding(0) {};  
	  ~Variable() {};
	  void init() { binding = 0; };	
	  void destroy() {};
	public:
	  void Bind(void* bind) { binding = bind; };
	  void Unbind() 
	    {
	      CALL("Variable::Unbind()");
	      binding = 0; 
	    };
	  const void* Binding() const { return binding; };
          void* Binding() { return binding; };
	  
	  Variable* Next() { return this+1; };
	  Variable* Previous() { return this-1; };

	  ulong Index() const 
	    { 
	      CALL("Variable::Index() const");
	      return (this - Unifier::current()->firstVariable())/VampireKernelConst::MaxNumOfVariables; 
	    }; 
	  void NumAndIndex(ulong& num,ulong& index) const
	    {
	      CALL("Variable::NumAndIndex(ulong& num,ulong& index) const");
	      ulong pos = this - Unifier::current()->firstVariable();
	      index = pos/VampireKernelConst::MaxNumOfVariables;
	      num = pos%VampireKernelConst::MaxNumOfVariables;
	    };
   
	  ulong absoluteVarNum() const
	    {
	      CALL("Variable::absoluteVarNum() const");
	      return static_cast<ulong>(this - Unifier::current()->firstVariable());
	    };
   

	  void* Unref() 
	    {
	      CALL("Variable::Unref()");
	      ASSERT(Unifier::current()->isVariable(static_cast<void*>(this)));
	      void* res = Binding();
	      if (res)
		{
		check_binding:
		  if (Unifier::current()->isVariable(res))
		    {
		      void* tmp = (static_cast<Variable*>(res))->Binding();
		      if (tmp)
			{
			  res = tmp;
			  goto check_binding;
			}
		      else
			{
			  return res; // free variable
			};
		    }
		  else // complex term 
		    {
		      return res;
		    };
		}
	      else
		{
		  return this;
		};
	    }; 

	  const void* Unref(bool& free_var) const
	    {
	      CALL("Variable::Unref(bool& free_var const)");
	      ASSERT(Unifier::current()->isVariable(static_cast<const void*>(this)));
	      const void* res = Binding();
	      if (res)
		{
		check_binding:
		  if (Unifier::current()->isVariable(res))
		    {
		      const void* tmp = (static_cast<const Variable*>(res))->Binding();
		      if (tmp)
			{
			  res = tmp;
			  goto check_binding;
			}
		      else
			{
			  free_var = true;
			  return res; // free variable
			};
		    }
		  else // complex term 
		    {
		      free_var = false;
		      return res;
		    };
		}
	      else
		{
		  free_var = true;
		  return static_cast<const void*>(this);
		};
	    }; // const void* Unref(bool& free_var) const

	  void* Unref(bool& free_var)
	    {
	      CALL("Variable::Unref(bool& free_var)");
	      ASSERT(Unifier::current()->isVariable(static_cast<void*>(this)));
	      void* res = Binding();
	      if (res)
		{
		check_binding:
		  if (Unifier::current()->isVariable(res))
		    {
		      void* tmp = (static_cast<Variable*>(res))->Binding();
		      if (tmp)
			{
			  res = tmp;
			  goto check_binding;
			}
		      else
			{
			  free_var = true;
			  return res; // free variable
			};
		    }
		  else // complex term 
		    {
		      free_var = false;
		      return res;
		    };
		}
	      else
		{
		  free_var = true;
		  return static_cast<void*>(this);
		};
	    }; // void* Unref(bool& free_var)

	public: // methods for debugging and logging
	  ostream& output(ostream& str) const;
#ifndef NO_DEBUG_VIS
	  ostream& outputBinding(ostream& str,ostream& (*outputTerm)(ostream& str,const PrefixSym* t)) const;
#endif
	}; // class Variable

    public:
      class InitStatic
	{
	public:
	  InitStatic() 
	    {
	      if (!_count)
		{
		  
		  for (ulong i = 0UL; i < VampireKernelConst::MaxNumOfIndices; ++i)
		    {
		      Unifier::_offsetForIndex[i] = i*VampireKernelConst::MaxNumOfVariables;
		    };

		};
	      ++_count;
	    };
	private:
	  static long _count;
	};
    public:
      Unifier();
      ~Unifier();
      void init();
      void destroy();
      void reset() 
	{
	  CALL("reset()");
	  UnbindAll(); 
	};

      const void* state() const 
      {
	return static_cast<const void*>(next);
      };

      static Unifier* current() 
	{

	  return _current; 
	};
      static void setCurrent(Unifier* subst) { _current = subst; };

      Variable* variableBase(ulong v) { return _variableBank + v; };
      static ulong variableOffset(ulong index) 
      {
	CALL("variableOffset(ulong index)");
	return _offsetForIndex[index]; 
      };     
      Variable* variable(ulong v,ulong index) 
	{ 
	  return variableBase(v) + variableOffset(index); 
	};
      Variable* variable(ulong absoluteVarNum)
	{
	  return _variableBank + absoluteVarNum;
	};
      Variable* firstVariable() { return _variableBank; };
      Variable* firstVariable(ulong index) { return firstVariable() + variableOffset(index); };
      Variable* lastVariable(ulong index) 
	{ 
	  return firstVariable(index) + VampireKernelConst::MaxNumOfVariables - 1;
	};

      static ulong variableIndex(ulong varOffset) 
	{
	  return (varOffset/VampireKernelConst::MaxNumOfVariables);
	};

      
      bool isVariable(const void* ptr) const 
	{ 
	  return ((ptr >= (static_cast<const void*>(_variableBank))) && 
		  (ptr < (static_cast<const void*>(_endOfVariableBank)))); 
	};

	    
      void UnbindAll() 
	{
	  CALL("UnbindAll()");

	  while (next != bound)
	    {
	      next--;
	      (*next)->Unbind();
	    };
	  next_section = section;
	};
    
#ifndef NO_DEBUG
      bool NoBoundVariables() const 
	{
	  return (next == bound) && (next_section == section);
	};
#endif   

      void Bind(Variable* v,void* val) 
	{
	  CALL("Bind(Variable* v,void* val)");
	  ASSERT(v != (static_cast<Variable*>(val)));
	  v->Bind(val);
	  *next = v;
	  next++;
	};
   
      void BindAlone(Variable* v,void* val)
	{
	  Bind(v,val);
	}; 
   
      void BindInSection(Variable* v,void* val)
	{
	  Bind(v,val);
	};
    
      Variable* LastBound() const { return *(next-1); };
      void UnbindLast()
	{
	  CALL("UnbindLast()");
	  next--;
	  (*next)->Unbind();
     
	  ASSERT(SectionsOK());
	};
      void MarkSection() 
	{
	  CALL("MarkSection()");
	  *next_section = next; 
	  next_section++;
	  ASSERT(SectionsOK());
	};
      Variable*** LastMark() const
	{
	  CALL("LastMark()");
	  ASSERT(next_section > section);
	  return next_section-1; 
	};
   
      void UnbindDownto(Variable*** mark)
	{
	  CALL("UnbindDownto(Variable*** mark)");
	  Variable** m = *mark;
	  next_section = mark;
	  while (next != m)
	    {
	      next--;
	      (*next)->Unbind();
	    };
	  ASSERT(SectionsOK());
	};

      void UnbindLastSection()
	{
	  CALL("UnbindLastSection()");
	  ASSERT(next_section > section);
	  next_section--;
	  Variable** section_mark = *next_section;
     
	  ASSERT(section_mark >= bound);
	  ASSERT(section_mark <= next);
     
	  while (next != section_mark)
	    {
	      next--;
	      (*next)->Unbind();
	    };
	  ASSERT(SectionsOK());
	};
      Variable*** MarkState()
	{
	  MarkSection();
	  return LastMark();
	};
 
    public:
      bool VariablesAffected(Variable* first,Variable* last)
	{
	  for (Variable** bv = bound; bv < next; bv++)
	    if (((*bv) >= first) && ((*bv) <= last)) return true;    
	  return false;
	};
      bool VariablesAffected(ulong index)
	{ 
	  return VariablesAffected(firstVariable(index),lastVariable(index));
	};

    public: // methods for debugging
#ifndef NO_DEBUG
      bool SectionsOK() const
	{
	  Variable*** sect = next_section - 1;
	  if (sect >= (static_cast<Variable** const *>(section)))
	    {
	      if ((*sect) > next)
		{
		  return false;
		};
	    };
	  return true;     
	};
      ulong NumOfBindings() const { return (next - static_cast<Variable* const *>(bound)); };
#endif

#ifndef NO_DEBUG_VIS
      ostream& output(ostream& str,ostream& (*outputTerm)(ostream& str,const PrefixSym* t)) const;
#endif
    private: 
      Variable _variableBank[VampireKernelConst::MaxNumOfIndices*VampireKernelConst::MaxNumOfVariables];
      static ulong _offsetForIndex[VampireKernelConst::MaxNumOfIndices];
      const Variable* _endOfVariableBank;

      Variable* bound[VampireKernelConst::MaxNumOfVariables*VampireKernelConst::MaxNumOfIndices];
      Variable** next;
      Variable** section[VampireKernelConst::MaxNumOfVariables*VampireKernelConst::MaxNumOfIndices];
      Variable*** next_section;
      static Unifier* _current;
      friend class InitStatic;
    }; // class Unifier

static Unifier::InitStatic unifierInitStatic;

}; // namespace VK


//================================================
#undef DEBUG_NAMESPACE
#ifdef DEBUG_VARIABLES
#define DEBUG_NAMESPACE "LARGE_VAR_TABLE"
#endif
#include "debugMacros.hpp"
//=================================================

namespace VK
{
  class LARGE_VAR_TABLE
    {
    public:
      LARGE_VAR_TABLE() 
	{ 
	};
      LARGE_VAR_TABLE(void* val) 
	{ 
	  for (ulong i = 0; 
	       i< VampireKernelConst::MaxNumOfIndices*VampireKernelConst::MaxNumOfVariables; 
	       i++)
	    {
	      table[i] = val;
	    };
	};
      ~LARGE_VAR_TABLE() {};
      void init() {};
      void init(void* val) 
	{ 
	  for (ulong i = 0; 
	       i< VampireKernelConst::MaxNumOfIndices*VampireKernelConst::MaxNumOfVariables; 
	       i++)
	    {
	      table[i] = val;
	    };
	};
      void destroy() {};
      void** Entry(const Unifier::Variable* v)
	{ 
	  return table + v->absoluteVarNum();
	};
      void* const * Entry(const Unifier::Variable* v) const
	{ 
	  return table + v->absoluteVarNum();
	};
      
    private:
      void* table[VampireKernelConst::MaxNumOfIndices*VampireKernelConst::MaxNumOfVariables];
    }; // class LARGE_VAR_TABLE

}; // namespace VK

//================================================
#undef DEBUG_NAMESPACE
#ifdef DEBUG_VARIABLES
#define DEBUG_NAMESPACE "LARGE_VAR_COUNTER"
#endif
#include "debugMacros.hpp"
//=================================================
 
namespace VK
{

  class LARGE_VAR_COUNTER
    {
    private:
      LARGE_VAR_TABLE counter;
      const Unifier::Variable* registered[VampireKernelConst::MaxNumOfVariables*VampireKernelConst::MaxNumOfIndices]; // stack
      const Unifier::Variable** next;
      const Unifier::Variable** iter;
    public:
      LARGE_VAR_COUNTER() : counter(static_cast<void*>(0))
	{ 
	  next = registered; 
	};
      ~LARGE_VAR_COUNTER() {};
      void init()
	{
	  CALL("init()");
	  counter.init(static_cast<void*>(0));
	  next = registered; 
	};
      void destroy()
      {
	CALL("destroy()");
	counter.destroy();
      };
 
      void Reset()
	{
	  while (next != registered)
	    {
	      next--; 
	      *(counter.Entry(*next)) = 0;
	    };
	};
    private:  
      void Unregister(const Unifier::Variable* v)
	{
	  CALL("Unregister(const Unifier::Variable* v)");
	  ASSERT(!Score(v));  
	  for (const Unifier::Variable** ptr = registered; ptr < next; ptr++)  
	    {
	      if (v == (*ptr)) { next--; *ptr = *next; return; };
	    };
	};

    public:
      long Register(const Unifier::Variable* v) 
	// returns previous value of the counter for the variable
	{
	  long res = reinterpret_cast<long>(*(counter.Entry(v)));
	  *(counter.Entry(v)) = reinterpret_cast<void*>(res + 1);
	  if (!res) { *next = v; next++; }
	  else 
	    if (!Score(v)) 
	      {
		// variable with zero score must be ungeristered
		Unregister(v);
	      };
	  return res;      
	};

      long Register(long coefficient,const Unifier::Variable* v) 
	// returns previous value of the counter for the variable
	{
	  long res = reinterpret_cast<long>(*(counter.Entry(v)));
	  *(counter.Entry(v)) = reinterpret_cast<void*>(res + coefficient);
	  if (!res) { *next = v; next++; }
	  else 
	    if (!Score(v)) 
	      {
		// variable with zero score must be ungeristered
		Unregister(v);
	      };
	  return res;      
	};

      void MultiplyBy(long coefficient)
	{
	  if (coefficient)
	    {
	      for (const Unifier::Variable** ptr = registered; ptr < next; ptr++)  
		{
		  *(counter.Entry(*ptr)) = reinterpret_cast<void*>(coefficient*Score(*ptr)); 
		};
	    }
	  else Reset();
	};
 
      long Score(const Unifier::Variable* const v) const
	{
	  return reinterpret_cast<long>(*(counter.Entry(v)));
	};

      ulong NumOfRegistered() const { return (next - registered); };

      void ResetIteration()
	{
	  iter = registered;
	};
      const Unifier::Variable* NextRegistered()
	{
	  if (iter == next) { return 0; };
	  const Unifier::Variable* res = *iter;
	  iter++;
	  return res;
	};
    }; // class LARGE_VAR_COUNTER

}; // namespace VK


//================================================
#undef DEBUG_NAMESPACE
#ifdef DEBUG_VARIABLES
#define DEBUG_NAMESPACE "VAR_TO_INT_RENAMING"
#endif
#include "debugMacros.hpp"
//=================================================
namespace VK
{

  class VAR_TO_INT_RENAMING
    {
    private:
      LARGE_VAR_TABLE renaming;
      ulong next_var;
      const Unifier::Variable* renamed[VampireKernelConst::MaxNumOfVariables*VampireKernelConst::MaxNumOfIndices]; // stack
      const Unifier::Variable** next;
    public: 
      VAR_TO_INT_RENAMING() : renaming(static_cast<void*>(0))
	{
	  next_var = 1;
	  next = renamed;
	};
      ~VAR_TO_INT_RENAMING() {};
      void init()      
      {
	CALL("init()");
	renaming.init(static_cast<void*>(0));
	next_var = 1;
	next = renamed;
      };
      void destroy()
      {
	CALL("destroy()");
	renaming.destroy();
      };
      
      void Reset()
	{
	  next_var = 1;
	  while (next != renamed)
	    {
	      next--; 
	      *(renaming.Entry(*next)) = static_cast<void*>(0);
	    };
	};
      ulong Rename(const Unifier::Variable* var)
	{
	  ulong res = reinterpret_cast<ulong>(*(renaming.Entry(var)));
	  if (!res) 
	    {
	      res = next_var;
	      *(renaming.Entry(var)) = reinterpret_cast<void*>(res);
	      next_var++;
	      *next = var;
	      next++;
	    };
	  return res;
	};
     
    public: // for debugging
#ifndef NO_DEBUG_VIS
      ostream& output(ostream& str) const;
#endif
    }; // class VAR_TO_INT_RENAMING
}; // namespace VK


//================================================
#undef DEBUG_NAMESPACE
#ifdef DEBUG_VARIABLES
#define DEBUG_NAMESPACE "INT_VAR_RENAMING"
#endif
#include "debugMacros.hpp"
//=================================================
namespace VK
{
  class INT_VAR_RENAMING
    {
    public: 
      INT_VAR_RENAMING() 
	{
	  for (ulong v = 0; v < VampireKernelConst::MaxNumOfVariables; v++)
	    {
	      renaming[v] = 0;
	    };
	  next_var = 1;
	  next = renamed;
	};
      ~INT_VAR_RENAMING() {};
      void init()
      {
	for (ulong v = 0; v < VampireKernelConst::MaxNumOfVariables; v++)
	  {
	    renaming[v] = 0;
	  };
	next_var = 1;
	next = renamed;
      };
      
      void destroy() {};

      void Reset()
	{
	  next_var = 1;
	  while (next != renamed)
	    {
	      next--; 
	      renaming[*next] = 0;
	    };
	};
      ulong Rename(ulong var)
	{
	  ulong res = renaming[var];
	  if (!res) 
	    {
	      res = next_var;
	      renaming[var] = res;
	      next_var++;
	      *next = var;
	      next++;
	    };
	  return res - 1;
	};
    public: // for debugging
#ifndef NO_DEBUG_VIS
      ostream& output(ostream& str) const;
#endif
    private:
      ulong renaming[VampireKernelConst::MaxNumOfVariables];
      ulong next_var;
      ulong renamed[VampireKernelConst::MaxNumOfVariables]; // stack
      ulong* next;
    }; // class INT_VAR_RENAMING
}; // namespace VK


//================================================
#undef DEBUG_NAMESPACE
#ifdef DEBUG_VARIABLES
#define DEBUG_NAMESPACE "DOUBLE_INT_VAR_RENAMING"
#endif
#include "debugMacros.hpp"
//=================================================

namespace VK
{
  class DOUBLE_INT_VAR_RENAMING
    {
    private:
      ulong renaming[2*VampireKernelConst::MaxNumOfVariables];
      ulong next_var;
      ulong renamed[2*VampireKernelConst::MaxNumOfVariables]; // stack
      ulong* next;
    public: 
      DOUBLE_INT_VAR_RENAMING() 
	{
	  for (ulong v = 0; v < 2*VampireKernelConst::MaxNumOfVariables; v++)
	    {
	      renaming[v] = 0;
	    };
	  next_var = 1;
	  next = renamed;
	};
      ~DOUBLE_INT_VAR_RENAMING() {};
      void Reset()
	{
	  next_var = 1;
	  while (next != renamed)
	    {
	      next--; 
	      renaming[*next] = 0;
	    };
	};
      ulong Rename(ulong var)
	{
	  ulong res = renaming[var];
	  if (!res) 
	    {
	      res = next_var;
	      renaming[var] = res;
	      next_var++;
	      *next = var;
	      next++;
	    };
	  return res - 1;
	};
    }; // class DOUBLE_INT_VAR_RENAMING

}; // namespace VK
//================================================
#undef DEBUG_NAMESPACE
#ifdef DEBUG_VARIABLES
#define DEBUG_NAMESPACE "VAR_WEIGHT"
#endif
#include "debugMacros.hpp"
//================================================= 
namespace VK
{
  class VAR_WEIGHT
    {
    private: // structure
      ulong num_of_occurences;
      ulong different_terms; 
    public:
      VAR_WEIGHT() : num_of_occurences(0), different_terms(0) {};
      ~VAR_WEIGHT() {};
      void init() { num_of_occurences = 0; different_terms = 0; };
      void destroy() {};
      void Reset() { num_of_occurences = 0; different_terms = 0; };
      bool Nonnull() { return (num_of_occurences || different_terms); };
      void MoreOccurences(ulong n) { num_of_occurences += n; };
      void AnotherTerm() { different_terms++; };
      BK::Comparison Compare(const VAR_WEIGHT& w) const
	{
	  if (num_of_occurences == w.num_of_occurences)
	    {
	      if (different_terms == w.different_terms) { return BK::Equal; }
	      else
		{ 
		  if (different_terms > w.different_terms) { return BK::Greater; }
		  else return BK::Less; 
		};
	    }
	  else
	    {
	      if (num_of_occurences > w.num_of_occurences) { return BK::Greater; }
	      else return BK::Less; 
	    }; 
	};
    }; // class VAR_WEIGHT

}; // namespace VK
//================================================
#undef DEBUG_NAMESPACE
#ifdef DEBUG_VARIABLES
#define DEBUG_NAMESPACE "VAR_WEIGHTING"
#endif
#include "debugMacros.hpp"
//================================================= 
namespace VK
{ 
  class VAR_WEIGHTING
    {
    public:
      VAR_WEIGHTING() : registered("VampireKernelConst::MaxNumOfVariables in VAR_WEIGHTING::registered")
	{ 
	};
      ~VAR_WEIGHTING() {};

      void init()
      {
	CALL("init()");
	registered.init("VampireKernelConst::MaxNumOfVariables in VAR_WEIGHTING::registered");
	for (ulong v = 0; v < VampireKernelConst::MaxNumOfVariables; v++) weight[v].init(); 
      };
      void destroy()
      {
	CALL("destroy()");
	for (ulong v = 0; v < VampireKernelConst::MaxNumOfVariables; v++) weight[v].destroy();
	registered.destroy();
      };

      void Reset()
	{
	  while (!(registered.empty())) { weight[registered.pop()].Reset(); };
	};
      void MoreOccurences(ulong var,ulong occ)
	{
	  if (!(weight[var].Nonnull())) { registered.push(var); };
	  weight[var].MoreOccurences(occ);
	};
      void AnotherTerm(ulong var)
	{
	  if (!(weight[var].Nonnull())) { registered.push(var); };
	  weight[var].AnotherTerm();
	};
      BK::Comparison Compare(ulong var1,ulong var2) const
	{
	  return weight[var1].Compare(weight[var2]);
	};
      void AnotherTermVariables(BK::MultisetOfVariables<VampireKernelConst::MaxNumOfVariables>& vars)
	{
	  vars.ResetIterator();
	  const ulong* next_var = vars.NextVar();
	  while (next_var)
	    {
	      AnotherTerm(*next_var);
	      MoreOccurences(*next_var,vars.Occurences(*next_var));
	      next_var = vars.NextVar();
	    };
	};
      void AnotherTermVariables(const WeightPolynomial::Monomials& vars)
	{
	  CALL("AnotherTermVariables(const WeightPolynomial::Monomials& vars)");
	  const ulong* end = vars.end();
	  for (const ulong* v = vars.begin(); v != end; ++v)
	    {
	      AnotherTerm(*v);
	      MoreOccurences(*v,vars.coefficient(*v));
	    };
	};

    private: 
      VAR_WEIGHT weight[VampireKernelConst::MaxNumOfVariables];
      BK::Stack<ulong,VampireKernelConst::MaxNumOfVariables> registered;
    }; // class VAR_WEIGHTING  
}; // namespace VK
//================================================
#undef DEBUG_NAMESPACE 
//===================================================================
#endif 





