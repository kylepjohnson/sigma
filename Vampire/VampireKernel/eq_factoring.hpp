//
// File:         eq_factoring.hpp
// Description:  Classes dealing with equality factoring.
// Created:      Mar 12, 2000, Alexandre Riazanov, riazanov@cs.man.ac.uk
// Revised:      Dec 04, 2001, Alexandre Riazanov, riazanov@cs.man.ac.uk
//               1) The stack other_literals replaced by the extendable
//               stack _otherLiterals.
// Revised:      Dec 22, 2001, Alexandre Riazanov, riazanov@cs.man.ac.uk
//               In EQ_FACTORING it was implicitely assumed that 
//               <num of lits>*HellConst::MaxTermSize <= HellConst::MaxClauseSize
//               which is now wrong since HellConst::MaxClauseSize is
//               not defined via HellConst::MaxNumOfLiterals.
//====================================================
#ifndef EQ_FACTORING_H
//====================================================
#define EQ_FACTORING_H
#ifndef NO_DEBUG_VIS
#  include <iostream>
#endif
#include "jargon.hpp"
#include "VampireKernelDebugFlags.hpp"
#include "VampireKernelConst.hpp"
#include "NewClause.hpp"
#include "GlobAlloc.hpp"
#include "ExpandingStack.hpp"
//================================================
#undef DEBUG_NAMESPACE
#ifdef DEBUG_EQ_FACTORING
#  define DEBUG_NAMESPACE "EQ_FACTOR"
#endif
#include "debugMacros.hpp"
//================================================= 
namespace VK
{
  

class EQ_FACTOR
 {
  private:
   bool st_unordered;
   bool wrap_st;  
   PrefixSym* s;
   PrefixSym* t;
   bool wrap_s_t_;
   PrefixSym* s_;
   PrefixSym* t_;
   bool init_state;
   PrefixSym* s_t_;
   NewClause* _resolvent;
  public:
   EQ_FACTOR(NewClause* res) : _resolvent(res) {}; 
   ~EQ_FACTOR() { CALL("destructor ~EQ_FACTOR()"); };
   void init(NewClause* res) { _resolvent = res; };
   void destroy() {};
   void LoadST(PrefixSym* eq_lit) 
    {
     CALL("LoadST(PrefixSym* eq_lit)");
     ASSERT(Prefix::CorrectComplexTerm(eq_lit));
     st_unordered = eq_lit->Head().Func().IsUnorderedEq();
     s = eq_lit + 1;
     t = s->Bracket();
    };
   void LoadS_T_(PrefixSym* eq_lit) 
    {
     CALL("LoadS_T_(PrefixSym* eq_lit)");
     ASSERT(Prefix::CorrectComplexTerm(eq_lit));
     s_t_ = eq_lit;
     s_ = eq_lit + 1;
     t_ = s_->Bracket();
     init_state = true;
    };
  private: 
   bool UnifySS_()
    {
     return UNIFICATION::Unify(s,s_);
    };
   void WrapST()
    {
     PrefixSym* tmp = s;
     s = t;
     t = tmp;
    }; 
   void WrapS_T_()
    {
     PrefixSym* tmp = s_;
     s_ = t_;
     t_ = tmp;
    };
   
   bool FirstInf()
    {
     CALL("FirstInf()");
     wrap_st = st_unordered;
     wrap_s_t_ = true;
     
     if (UnifySS_()) return true;
     
     wrap_s_t_ = false;
     WrapS_T_();
     if (UnifySS_()) return true;
     
     if (!wrap_st) return false;
     
     wrap_st = false;     
     WrapST();
     if (UnifySS_()) { wrap_s_t_ = true; return true; };
    
     // last attempt
     WrapS_T_();
     ASSERT(!wrap_s_t_);
     ASSERT(!wrap_st);
     return UnifySS_();
    };
   
   bool NextInf()
    {
     CALL("NextInf()");
     if (wrap_s_t_)
      {
       wrap_s_t_ = false;
       WrapS_T_();
       if (UnifySS_()) return true;
      };
     if (wrap_st)
      {
       wrap_st = false; 
       WrapST();
       if (UnifySS_()) { wrap_s_t_ = true; return true; };
       
       // last attempt
       WrapS_T_();
       ASSERT(!wrap_s_t_);
       ASSERT(!wrap_st);
       return UnifySS_(); 
      }
     else return false;
    };
  
  public:
   bool NextInference()
    {
     CALL("NextInference()");
     if (init_state)
      {
       init_state = false;
       return FirstInf();
      };
     return NextInf(); 
    };   
    
   bool CollectResult()
    {
     CALL("CollectResult()");
  
     //DF; output(cout) << "\n";

     if (!_resolvent->PushPrefixLit(s_t_,false)) return false;
     return _resolvent->PushPrefixEqLit(TERM::negativeUnordEqHeader(),t,t_,false);
    };   
  
  public: // for debugging 
   #ifndef NO_DEBUG_VIS
    ostream& output(ostream& str) const
    {
     str << "S = "; Prefix::outputTerm(str,s) << '\n';  
     str << "T = "; Prefix::outputTerm(str,t) << '\n';
     str << "S_ = "; Prefix::outputTerm(str,s_) << '\n';  
     str << "T_ = "; Prefix::outputTerm(str,t_) << '\n';
     str << "SUBSTITUTION:\n";
     Unifier::current()->output(str,Prefix::outputWordWithSubst); 
     return str << '\n';
    }; 
   #endif
 }; // class EQ_FACTOR
}; // namespace VK


//================================================
#undef DEBUG_NAMESPACE
#ifdef DEBUG_EQ_FACTORING
 #define DEBUG_NAMESPACE "EQ_FACTORING"
#endif
#include "debugMacros.hpp"
//================================================= 

namespace VK
{
class EQ_FACTORING
 {
  private:
   EQ_FACTOR eq_factor; 
   NewClause* _resolvent;  

   PrefixSym pos_eq[VampireKernelConst::MaxClauseSize]; // memory

   PrefixSym* _freePosEqMemory;

   BK::ExpandingStack<BK::GlobAlloc,PrefixSym*,32UL,EQ_FACTORING> _positiveEqualities;

   long last_pos_eq;
   long last_max_pos_eq;
   
   BK::ExpandingStack<BK::GlobAlloc,TERM*,32UL,EQ_FACTORING> _otherLiterals;
   
   Clause* clause;
   long index;
   bool init_state;
   long curr_st;
   long curr_s_t_;
  public:
   EQ_FACTORING(NewClause* res,long ind)
    : eq_factor(res),
      _resolvent(res),
      _positiveEqualities(DOP("EQ_FACTORING::_positiveEqualities")),
      _otherLiterals(DOP("EQ_FACTORING::_otherLiterals")),
      index(ind)
   {
     CALL("constructor EQ_FACTORING(NewClause* res)");
    DOP(_positiveEqualities.freeze());
    DOP(_otherLiterals.freeze());
   };
   ~EQ_FACTORING() {};
   void init(NewClause* res,long ind) 
   {
     CALL("init(NewClause* res,long ind)");
     eq_factor.init(res);
     _resolvent = res;
     _positiveEqualities.init(DOP("EQ_FACTORING::_positiveEqualities"));
     _otherLiterals.init(DOP("EQ_FACTORING::_otherLiterals"));
     index = ind;
     DOP(_positiveEqualities.freeze());
     DOP(_otherLiterals.freeze());
   };
   void destroy() 
   {
     CALL("destroy()");
     _otherLiterals.destroy();
     _positiveEqualities.destroy();
     eq_factor.destroy(); 
   };
   void SetIndex(long ind) { index = ind; };  
  private:   
   void Reset()
    {
     _positiveEqualities.reset();
     _otherLiterals.reset();
     //other_literals.reset();
     _freePosEqMemory = pos_eq;
     last_pos_eq = -1;
     last_max_pos_eq = -1;
     init_state = true;
    };  
   Clause* TheClause() const { return clause; };

   PrefixSym* PosEq(long n) 
   { 
    CALL("PosEq(long n)");
    return _positiveEqualities.nth(n);
     //return pos_eq + (n*VampireKernelConst::MaxTermSize); 
   };

   void LoadMax(TERM* lit)
    {
     CALL("LoadMax(TERM* lit)");
     if (lit->Positive() && lit->IsEquality())
      {
       last_pos_eq++;
       last_max_pos_eq++;

       //PrefixSym* mem = PosEq(last_pos_eq);
       _positiveEqualities.push(_freePosEqMemory);
       DOP(PrefixSym* debugMem = _freePosEqMemory);
 
       StandardTermToPrefix(index,lit,_freePosEqMemory);   
       ASSERT(_freePosEqMemory > debugMem);
       ASSERT(_freePosEqMemory - debugMem <= (long)VampireKernelConst::MaxTermSize);
       ASSERT(_freePosEqMemory - pos_eq <= (long)VampireKernelConst::MaxClauseSize);  
       ASSERT(debugMem->Bracket() == _freePosEqMemory);
       ASSERT(Prefix::CorrectComplexTerm(PosEq(last_pos_eq))); 
       ASSERT(last_pos_eq >= 0);       
       ASSERT((!last_pos_eq) || (Prefix::CorrectComplexTerm(PosEq(last_pos_eq - 1))));
      }
     else 
      { 
       _otherLiterals.push(lit);
	//other_literals.push(lit); 
      }; 
    };
   void LoadNonmax(TERM* lit)
    {
     CALL("LoadNonmax(TERM* lit)");
     if (lit->Positive() && lit->IsEquality())
      {
       last_pos_eq++;
       ASSERT(last_pos_eq > 0); 

       //PrefixSym* mem = PosEq(last_pos_eq);
       _positiveEqualities.push(_freePosEqMemory);
       DOP(PrefixSym* debugMem = _freePosEqMemory);
       StandardTermToPrefix(index,lit,_freePosEqMemory);   
       ASSERT(_freePosEqMemory > debugMem);
       ASSERT(_freePosEqMemory - debugMem <= (long)VampireKernelConst::MaxTermSize);
       ASSERT(_freePosEqMemory - pos_eq <= (long)VampireKernelConst::MaxClauseSize);  
       ASSERT(debugMem->Bracket() == _freePosEqMemory);
       ASSERT(Prefix::CorrectComplexTerm(PosEq(last_pos_eq)));
       ASSERT(Prefix::CorrectComplexTerm(PosEq(last_pos_eq - 1)));
      }
     else 
      { 
       _otherLiterals.push(lit);
       //other_literals.push(lit); 
      };
    };
   
   bool FirstInf()
    {
     CALL("FirstInf()");
     ASSERT(last_max_pos_eq >= 0);
     ASSERT(last_pos_eq >= last_max_pos_eq);
     ASSERT(last_pos_eq > 0);
     curr_st = 0;
     curr_s_t_ = 1;
     COP("COP0");

     ASSERT(Prefix::CorrectComplexTerm(PosEq(curr_st)));
     eq_factor.LoadST(PosEq(curr_st));
     try_current_s_t_:
      COP("try_current_s_t_");
      eq_factor.LoadS_T_(PosEq(curr_s_t_)); 
      Unifier::current()->UnbindAll();
      if (eq_factor.NextInference()) return true;
      
      curr_s_t_++;
      if (curr_s_t_ == curr_st) curr_s_t_++;
      if (curr_s_t_ <= last_pos_eq) goto try_current_s_t_;
      
      curr_st++;
      if (curr_st > last_max_pos_eq) return false; 
      COP("COP10"); 
      eq_factor.LoadST(PosEq(curr_st));
      curr_s_t_ = 0; 
      goto try_current_s_t_;
    }; 
   
   bool NextInf()
    {
     CALL("NextInf()");
     Unifier::current()->UnbindAll();
     if (eq_factor.NextInference()) return true;
     try_next_s_t_:
      curr_s_t_++;
      if (curr_s_t_ == curr_st) curr_s_t_++;
      if (curr_s_t_ <= last_pos_eq)
       {
        try_curr_s_t_:
         eq_factor.LoadS_T_(PosEq(curr_s_t_)); 
         Unifier::current()->UnbindAll();
         if (eq_factor.NextInference()) return true;
         goto try_next_s_t_;
       };
       
      curr_st++;
      if (curr_st > last_max_pos_eq) return false;  
      eq_factor.LoadST(PosEq(curr_st));
      curr_s_t_ = 0;
      goto try_curr_s_t_; 
    }; 

   
   bool CollectOtherPosEq()
    {
     CALL("CollectOtherPosEq()");
     for (long ln = 0; ln <= last_pos_eq; ln++)
      if ((ln != curr_st) && (ln != curr_s_t_))
       if (!_resolvent->PushPrefixLit(PosEq(ln),false)) return false;
     return true; 
    };
   
   bool CollectOtherLiterals()
    {
     CALL("CollectOtherLiterals()");
     TERM** end = _otherLiterals.end();
     //TERM** end = other_literals.end();
     for (TERM** lit = _otherLiterals.begin(); lit < end; lit++)
      if (!_resolvent->PushStandardLit(index,*lit,false)) return false;
     return true; 
    };
    
   bool CollectResult()
    {
     CALL("CollectResult()");
     return clause->Active()
            && CollectOtherPosEq()  
            && eq_factor.CollectResult() 
            && CollectOtherLiterals();
    };
     
  public:
   bool Load(Clause* cl) 
    {
     CALL("Load(Clause* cl)");    
    
     DOP(_positiveEqualities.unfreeze());  
     _positiveEqualities.expand(cl->numOfAllLiterals());
     DOP(_positiveEqualities.freeze());   

     DOP(_otherLiterals.unfreeze());  
     _otherLiterals.expand(cl->numOfAllLiterals());
     DOP(_otherLiterals.freeze());     

     Reset();
     clause = cl;
     LiteralList* lits = cl->LitList();
     long num_of_max = lits->numOfSelectedLits();
     if (num_of_max == 1) return false;
     LiteralList::Iterator iter(lits);
     do
      {
       LoadMax(iter.currentLiteral());
       iter.next();       
       num_of_max--; 
      }
     while (num_of_max); 
   
     if (last_pos_eq == -1) return false;

     #ifdef DEBUG_NAMESPACE
      COP("max loaded"); 
	  long i;
      for (i = 0; i<= last_pos_eq; i++) 
       { 
        ASSERT(Prefix::CorrectComplexTerm(PosEq(i))); 
       }; 
     #endif 

     // nonselected literals
     while (iter.notEnd())
      {  
       ASSERT(!(iter.currentElement()->isInherentlySelected()));              
       LoadNonmax(iter.currentElement()->literal());
       iter.next();
      };
     
     #ifdef DEBUG_NAMESPACE
      for (i = 0; i<= last_pos_eq; i++) 
       { 
        COP("nonmax loaded"); 
        ASSERT(Prefix::CorrectComplexTerm(PosEq(i))); 
       }; 
     #endif

     return (last_pos_eq > 0);
    };
   
   bool NextInference()
    {
     CALL("NextInference()");
     
     if (init_state)
      {
       COP("init_state");
       init_state = false;
       #ifdef DEBUG_NAMESPACE
        for (long i = 0; i<= last_pos_eq; i++) 
         { 
          ASSERT(Prefix::CorrectComplexTerm(PosEq(i))); 
         }; 
       #endif       
       if (!FirstInf()) return false;
      }
     else // !init_state
      {
       try_next_inf:
        #ifdef DEBUG_NAMESPACE
         for (long i = 0; i<= last_pos_eq; i++) 
          { 
           ASSERT(Prefix::CorrectComplexTerm(PosEq(i))); 
          }; 
        #endif       
        if (!NextInf()) return false;
      };
     // try to collect the new clause
     _resolvent->openGenerated(ClauseBackground::EqFact);
     if (CollectResult())
      { 
       _resolvent->UsedRule(ClauseBackground::EqFact);
       _resolvent->PushAncestor(clause);
       return true;
       //_resolvent->ProcessCollected();
      };    
     goto try_next_inf;
    }; 

   //void Process(Clause* cl);
 }; // class EQ_FACTORING

}; // namespace VK

//====================================================
#endif
