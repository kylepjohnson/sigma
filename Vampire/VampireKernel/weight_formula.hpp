#ifndef WEIGHT_FORMULA_H
//===============================================================
#define WEIGHT_FORMULA_H
#ifndef NO_DEBUG_VIS
#  include <iostream>
#endif
#include "jargon.hpp"
#include "Comparison.hpp"
#include "VampireKernelDebugFlags.hpp"
#include "prefix.hpp"
#include "variables.hpp"
//=================================================================
#undef DEBUG_NAMESPACE  
#ifdef DEBUG_WEIGHT_FORMULA
 #define DEBUG_NAMESPACE "LARGE_WEIGHT_FORMULA"
#endif
#include "debugMacros.hpp"
//=================================================
namespace VK
{
class LARGE_WEIGHT_FORMULA
 {
  // this class has heavy constructor, objects must be static!
  private:
   LARGE_VAR_COUNTER var_counter;
   long const_part;
  public:
   LARGE_WEIGHT_FORMULA();
   ~LARGE_WEIGHT_FORMULA() { CALL("destructor ~LARGE_WEIGHT_FORMULA()"); };
   inline void Reset() { var_counter.Reset(); const_part = 0L; };      
   inline void RegisterVar(long coefficient,const Unifier::Variable* v) { var_counter.Register(coefficient,v); };
   inline void RegisterFunc(long coefficient) { const_part += coefficient; };

   inline void MultiplyBy(long coefficient) 
    {
     const_part = coefficient*const_part;
     var_counter.MultiplyBy(coefficient); 
    };  
   
   inline void InverseSign() { MultiplyBy(-1L); };
 
   inline void CompileSimple(long coefficient,PrefixSym* t) // without substitution
    {
     if (t->Head().isVariable()) 
      {
       RegisterVar(coefficient,t->Head().var());
       return;             
      };

     Prefix::SimpleIter iter;
     iter.Reset(t);
     do
      {
       PrefSym sym = iter.CurrPos()->Head();
       if (sym.isVariable())
        {
         RegisterVar(coefficient,sym.var());
        }
       else RegisterFunc(coefficient); 
       iter.SkipSym();
      }
     while (!iter.Empty());
    }; // void CompileSimple(long coefficient,PrefixSym* t)

 
   inline void Compile(long coefficient,const PrefixSym* t)
    {
     if (t->Head().isVariable()) 
      {
       bool free_var;
       const void* bind = t->Head().var()->Unref(free_var);
       if (free_var)
        {
         RegisterVar(coefficient,static_cast<const Unifier::Variable*>(bind));
         return;
        }
       else { t = static_cast<const PrefixSym*>(bind); }; 
      };

     Prefix::ConstIter iter;
     iter.Reset(t);
     do
      {
       if (iter.CurrVar())
        {
         RegisterVar(coefficient,iter.CurrVar());
        }
       else RegisterFunc(coefficient);  
       iter.SkipSym();
      }
     while (!iter.Empty());
    }; // void Compile(long coefficient,const PrefixSym* t)


   inline void CompileSubst(long coefficient,const Unifier::Variable* var)
    {
     bool free_var;
     const void* bind = var->Unref(free_var);
     if (free_var)
      {
       RegisterVar(coefficient,static_cast<const Unifier::Variable*>(bind));
      }
     else Compile(coefficient,static_cast<const PrefixSym*>(bind));
    };   

   inline void Substitute(LARGE_WEIGHT_FORMULA& result)
    {
     result.Reset();
     result.RegisterFunc(const_part);
     var_counter.ResetIteration();
     for (const Unifier::Variable* var = var_counter.NextRegistered(); var; var = var_counter.NextRegistered())
      {
       result.CompileSubst(var_counter.Score(var),var); 
      };
    }; // void Substitute(LARGE_WEIGHT_FORMULA& result)
   
   inline bool Zero() const { return (!const_part) && (!var_counter.NumOfRegistered()); };
   inline bool Negative() 
    {
     if (!var_counter.NumOfRegistered()) return (const_part < 0);
     if (const_part > 0)
      {
       long rest_of_const_part = const_part;
       long score;
       
       var_counter.ResetIteration(); 
       for (const Unifier::Variable* var = var_counter.NextRegistered(); var; var = var_counter.NextRegistered())
        {
         score = var_counter.Score(var);
         if (score > 0) return false; 
         rest_of_const_part+=score;
        };
       return (rest_of_const_part < 0);
      };
     // here const_part <= 0
     var_counter.ResetIteration();
     for (const Unifier::Variable* var = var_counter.NextRegistered(); var; var = var_counter.NextRegistered())
      if (var_counter.Score(var) > 0) return false;
     return true;
    };

   inline bool Positive() 
    {
     if (!var_counter.NumOfRegistered()) return (const_part > 0);
     if (const_part < 0)
      {
       long rest_of_const_part = const_part;
       long score;
       
       var_counter.ResetIteration(); 
       for (const Unifier::Variable* var = var_counter.NextRegistered(); var; var = var_counter.NextRegistered())
        {
         score = var_counter.Score(var);
         if (score < 0) return false; 
         rest_of_const_part+=score;
        };
       return (rest_of_const_part > 0);
      };
     // here const_part >= 0
     var_counter.ResetIteration();
     for (const Unifier::Variable* var = var_counter.NextRegistered(); var; var = var_counter.NextRegistered())
      if (var_counter.Score(var) < 0) return false;
     return true;
    }; // bool Positive() 

  public: // for debugging  
   #ifndef NO_DEBUG_VIS
    ostream& output(ostream& str);
    ostream& outputWithSubst(ostream& str);
   #endif 
 }; // class LARGE_WEIGHT_FORMULA

}; // namespace VK
//=================================================================
#undef DEBUG_NAMESPACE  
#ifdef DEBUG_WEIGHT_FORMULA
 #define DEBUG_NAMESPACE "ORD_FORMULA"
#endif
#include "debugMacros.hpp"
//=================================================
namespace VK
{

class ORD_FORMULA
 {
  // this class has heavy constructor, objects must be static!
  private:
   LARGE_WEIGHT_FORMULA difference;
   LARGE_WEIGHT_FORMULA subst_diff;
   PrefixSym* term1;
   PrefixSym* term2;
  public:
   ORD_FORMULA() : difference(), subst_diff() {};
   ~ORD_FORMULA() { CALL("destructor ~ORD_FORMULA()"); };
   inline void Reset() { difference.Reset(); };
   inline void Compile(PrefixSym* t1,PrefixSym* t2)
    {
     // t1 and t2 must be complex here, this is used in lexicographic comparison 
     term1 = t1;
     term2 = t2;
     difference.CompileSimple(1L,t1);
     difference.CompileSimple(-1L,t2);
    };
 
   inline void Inverse() 
    {
     PrefixSym* tmp = term1;
     term1 = term2;
     term2 = tmp;
     difference.InverseSign(); 
    };
  private:
   inline bool LexLessOrEq() 
    {
     Prefix::Iter iter1;
     Prefix::Iter iter2;
     iter1.Reset(term1);
     iter2.Reset(term2);
     TERM func1;
     TERM func2;
     do
      {
       if (iter1.CurrVar())
        {
         if (iter2.CurrVar())
          {
           if (iter1.CurrVar() != iter2.CurrVar()) return false;
          }
         else return false;
        } 
       else
        if (iter2.CurrVar())
         {
          return false;
         }
        else // both are functors
         {
          func1 = iter1.CurrSubterm()->Head().Func();     
          func2 = iter2.CurrSubterm()->Head().Func(); 
          if (func1 != func2) return (func1.comparePrecedence(func2) == BK::Less); 
         }; 
       iter1.SkipSym();
       iter2.SkipSym();
      }      
     while (!iter1.Empty());
     return true; // equal
    }; // bool LexLessOrEq() 

   inline bool LexGreater() 
    {
     Prefix::Iter iter1;
     Prefix::Iter iter2;
     iter1.Reset(term1);
     iter2.Reset(term2);
     TERM func1;
     TERM func2;
     do
      {
       if (iter1.CurrVar())
        {
         if (iter2.CurrVar())
          {
           if (iter1.CurrVar() != iter2.CurrVar()) return false; // incomparable
          }
         else return false; // incomparable
        } 
       else
        if (iter2.CurrVar())
         {
          return false; // incomparable
         }
        else // both are functors
         {
          func1 = iter1.CurrSubterm()->Head().Func();     
          func2 = iter2.CurrSubterm()->Head().Func(); 
          if (func1 != func2) return (func1.comparePrecedence(func2) == BK::Greater); 
         }; 
       iter1.SkipSym();
       iter2.SkipSym();
      }      
     while (!iter1.Empty());
     return false; // equal
    }; // bool LexGreater() 

  public:
   inline bool greater()
    {
     difference.Substitute(subst_diff); 
     if (subst_diff.Zero()) return LexGreater();
     return subst_diff.Positive();
    };
   inline bool LessOrEqual() 
    {
     difference.Substitute(subst_diff); 
     if (subst_diff.Zero()) return LexLessOrEq();
     return subst_diff.Negative();
    }; 
   
  public: // for debugging
   #ifndef NO_DEBUG_VIS
    ostream& output(ostream& str);
    ostream& outputWithSubst(ostream& str);
   #endif
 }; // class ORD_FORMULA

}; // namespace VK
//=================================================
#undef DEBUG_NAMESPACE  
#ifdef DEBUG_WEIGHT_FORMULA
 #define DEBUG_NAMESPACE "WEIGHT_FORMULA"
#endif
#include "debugMacros.hpp"
//=================================================

namespace VK
{
class WEIGHT_FORMULA
 {
  // this class has heavy constructor, objects must be static!
  private:
   LARGE_VAR_COUNTER var_counter;
   long const_part;
   unsigned long index;
  public:
   WEIGHT_FORMULA() 
    : const_part(0),
      index(0)
   {
     CALL("constructor WEIGHT_FORMULA()");
   };
   ~WEIGHT_FORMULA() { CALL("destructor ~WEIGHT_FORMULA()"); };
   void init()
   {
     CALL("init()");
     var_counter.init();
     const_part = 0L;
     index = 0UL;
   };
   
   void destroy()
   {
     CALL("destroy()");
     var_counter.destroy();
   };

   inline void SetIndex(unsigned long ind) 
     { 
       CALL("SetIndex(unsigned long ind)");
       index = ind; 
       //var_counter.SetIndex(ind); 
     };
   inline void Reset()
    {
     var_counter.Reset();
     const_part = 0;
    };  
   inline void RegVar(const Unifier::Variable* v) { var_counter.Register(v); };
   inline void RegFunc() { const_part++; }; 
   inline long Compute() 
    {
     long res = const_part;
     var_counter.ResetIteration();
     for (const Unifier::Variable* var = var_counter.NextRegistered();var;var = var_counter.NextRegistered())
      {
       bool free_var;
       const void* bind = var->Unref(free_var);
       long w = (free_var)? 1 : Prefix::Weight(static_cast<const PrefixSym*>(bind)); 
       res += var_counter.Score(var)*w;
      };
     return res; 
    };
   inline bool GreaterThan(long x)
    {
     x -= const_part;
     if (x < 0) return true;
     var_counter.ResetIteration();
     for (const Unifier::Variable* var = var_counter.NextRegistered();var;var = var_counter.NextRegistered())
      {
       bool free_var;
       const void* bind = var->Unref(free_var);
       long w = (free_var)? 1 : Prefix::Weight(static_cast<const PrefixSym*>(bind)); 
       x -= var_counter.Score(var)*w;
       if (x < 0) return true;
      };
     return false; 
    }; 
   inline void Compile(TERM* t)
    {
     // t must be complex here
     TERM::Iterator iter;
     iter.Reset(t);
     do
      {
       if (iter.CurrentSymIsVar())
        {
         RegVar(Unifier::current()->variable(iter.Symbol().var(),index));
        }
       else RegFunc();   
      }
     while (iter.Next()); 
    }; 
   inline void Compile(TERM t) 
    {
     if (t.isVariable())
      {
       RegVar(Unifier::current()->variable(t.var(),index));
      }
     else Compile(t.First()); 
    };
   inline void Compile(PrefixSym* t)
    {
     CALL("Compile(PrefixSym* t)");
     if (t->Head().isVariable()) 
      {
       ASSERT(t->Head().var()->Index() == index);
       RegVar(t->Head().var()); 
       return; 
      };
     Prefix::SimpleIter iter;
     iter.Reset(t);
     do
      {
       PrefSym sym = iter.CurrPos()->Head();
       if (sym.isVariable())
        {
         ASSERT(sym.var()->Index() == index);
         RegVar(sym.var());
        }
       else RegFunc(); 
       iter.SkipSym();
      }
     while (!iter.Empty()); 
    }; 
  public: // for debugging  
   #ifndef NO_DEBUG_VIS
    ostream& output(ostream& str);
   #endif
 }; // class WEIGHT_FORMULA
}; // namespace VK

//===============================================================
#endif
