#ifndef UNIFICATION_H
//=====================================================================
#define UNIFICATION_H
#include "jargon.hpp"
#include "VampireKernelDebugFlags.hpp"
#include "VampireKernelConst.hpp"
#include "iu_command.hpp"
#include "variables.hpp"
//================================================
#undef DEBUG_NAMESPACE
#ifdef DEBUG_UNIFICATION
 #define DEBUG_NAMESPACE "UNIFICATION"
#endif
#include "debugMacros.hpp"
//=================================================

namespace VK
{
class UNIFICATION
 {
  public: 
   static inline bool Unify(PrefixSym* t1,PrefixSym* t2)
    {
     CALL("Unify(PrefixSym* t1,PrefixSym* t2)");
     // no bound variables in t1,t2!
     if (t1->Head().isVariable())
      {
       ASSERT(!(t1->Head().var()->Binding()));
       
       if (t2->Head().isVariable())
        {
         // both are variables
         ASSERT(!(t2->Head().var()->Binding()));
         Unifier::current()->MarkSection();
         if (t1->Head() != t2->Head())
          Unifier::current()->Bind(t1->Head().var(),static_cast<void*>(t2->Head().var()));
         return true;
        };
        
       // t1 is a variable, t2 is complex 
       if (OccurCheck::Occurs(t1->Head().var(),t2)) return false;
       Unifier::current()->MarkSection();
       Unifier::current()->Bind(t1->Head().var(),static_cast<void*>(t2));
       return true;
      }
     else
      {
       if (t2->Head().isVariable())
        {
         ASSERT(!(t2->Head().var()->Binding()));
         
         // t2 is a variable, t1 is complex 
         if (OccurCheck::Occurs(t2->Head().var(),t1)) return false;
         Unifier::current()->MarkSection();
         Unifier::current()->Bind(t2->Head().var(),static_cast<void*>(t1));
         return true; 
        }
       else // both complex
        return UnifyComplex(t1,t2);
      };
    }; // bool Unify(PrefixSym* t1,PrefixSym* t2)
       


   static inline bool UnifyArbitrary(PrefixSym* t1,PrefixSym* t2)
   {
     CALL("UnifyArbitrary(PrefixSym* t1,PrefixSym* t2)");
     Unifier::Variable* var1;
     Unifier::Variable* var2;
     PrefixSym* complexTerm1;
     PrefixSym* complexTerm2;
     
     if (t1->Head().isVariable())
       {
	 bool freeVar;
	 void* instance = t1->Head().var()->Unref(freeVar);
	 if (freeVar)
	   {
	     var1 = static_cast<Unifier::Variable*>(instance);
	   }
	 else
	   {
	     var1 = 0;
	     complexTerm1 = static_cast<PrefixSym*>(instance);
	   };
       }
     else
       {
	 var1 = 0;
	 complexTerm1 = t1;
       };
     
     if (t2->Head().isVariable())
       {
	 bool freeVar;
	 void* instance = t2->Head().var()->Unref(freeVar);
	 if (freeVar)
	   {
	     var2 = static_cast<Unifier::Variable*>(instance);
	   }
	 else
	   {
	     var2 = 0;
	     complexTerm2 = static_cast<PrefixSym*>(instance);
	   };
       }
     else
       {
	 var2 = 0;
	 complexTerm2 = t2;
       };
     
     if (var1)
       {	 
	 if (var2)
	   { 
	     Unifier::current()->MarkSection();
	     if (var1 != var2)
	       Unifier::current()->Bind(var1,static_cast<void*>(var2));
	     return true;
	   }
	 else // var1, !var2
	   {
	     if (OccurCheck::Occurs(var1,complexTerm2))
	       return false;
	     Unifier::current()->MarkSection();
	     Unifier::current()->Bind(var1,static_cast<void*>(complexTerm2));
	     return true;
	   };
       }
     else
       if (var2)
	 {
	   if (OccurCheck::Occurs(var2,complexTerm1))
	     return false;
	   Unifier::current()->MarkSection();
	   Unifier::current()->Bind(var2,static_cast<void*>(complexTerm1));
	   return true;
	 }
       else 
	 {
	   Prefix::CorrectBracket(complexTerm1);
	   Prefix::CorrectBracket(complexTerm2);
	   return UnifyComplex(complexTerm1,complexTerm2);  
	 };   
   }; // bool UnifyArbitrary(PrefixSym* t1,PrefixSym* t2)

   static inline bool UnifyComplex(PrefixSym* const word1,PrefixSym* const word2)
    {
     CALL("UnifyComplex(PrefixSym* const word1,PrefixSym* const word2)");
     // word1 and word2 must represent complex terms
     Unifier::current()->MarkSection();
     if (UnifyComplex_(word1,word2)) { return true; }
     else { Unifier::current()->UnbindLastSection(); return false; };
    }; // bool UnifyComplex(PrefixSym* const word1,PrefixSym* const word2)
    
   static inline bool UnifyComplex_(PrefixSym* const word1,PrefixSym* const word2)
    // word1 and word2 must represent complex terms
    {
     CALL("UnifyComplex_(PrefixSym* const word1,PrefixSym* const word2)");
     ASSERT(word1);
     ASSERT(word2);
     ASSERT(word1->Head().IsFunctor());
     ASSERT(word2->Head().IsFunctor());
     ASSERT(word1->Bracket());
     ASSERT(word2->Bracket());
     ASSERT(word1->Bracket() > word1);
     ASSERT(word2->Bracket() > word2);
     
     ASSERT(Prefix::CorrectComplexTerm(word1));
     ASSERT(Prefix::CorrectComplexTerm(word2));
     
     if (word1->Head() != word2->Head()) { return false; };

     Prefix::Iter iter1;
     Prefix::Iter iter2;
     iter1.Reset(word1);
     iter2.Reset(word2);
     iter1.SkipSym();
     iter2.SkipSym();
     
     next_sym:
      if (iter1.Empty())
       {
        ASSERT(iter2.Empty());
        ASSERT(Prefix::equal(word1,word2));
        return true;
       }     
      else
       {
        if (iter1.CurrVar())
         {
          if (iter2.CurrVar()) // both are variables
           {
            if (iter1.CurrVar() != iter2.CurrVar()) // different variables
             {
              Unifier::current()->Bind(iter1.CurrVar(),static_cast<void*>(iter2.CurrVar()));
             };
            iter1.SkipSym();
            iter2.SkipSym();
            goto next_sym;
           }
          else
           {
            Prefix::CorrectBracket(iter2.CurrSubterm());
            
            ASSERT(Prefix::CorrectComplexTerm(iter2.CurrSubterm()));
            
            if (OccurCheck::Occurs(iter1.CurrVar(),iter2.CurrSubterm()))
             {
              return false;
             }
            else
             {
              Unifier::current()->Bind(iter1.CurrVar(),static_cast<void*>(iter2.CurrSubterm()));
              iter1.SkipSym();
              iter2.SkipTerm();
              goto next_sym;
             };
           };
         }
        else // first is a complex term
         {
          if (iter2.CurrVar())
           {
            Prefix::CorrectBracket(iter1.CurrSubterm());
            
            ASSERT(Prefix::CorrectComplexTerm(iter1.CurrSubterm())); 
            
            if (OccurCheck::Occurs(iter2.CurrVar(),iter1.CurrSubterm()))
             {
              return false;
             }
            else
             {
              Unifier::current()->Bind(iter2.CurrVar(),static_cast<void*>(iter1.CurrSubterm()));
              iter1.SkipTerm();
              iter2.SkipSym();
              goto next_sym;
             };
           }
          else // both are nonvariable
           {
            if (iter1.CurrSubterm()->Head() == iter2.CurrSubterm()->Head())
             {
              iter1.SkipSym();
              iter2.SkipSym();
              goto next_sym;
             }
            else
             {
              return false;
             };
           };
         };
       };
    }; // bool UnifyComplex_(PrefixSym* word1,PrefixSym* word2)
   
   static inline bool UnifyPlain(PrefixSym* const word,PrefixSym* const pl_word)
    {
     CALL("UnifyPlain(PrefixSym* const word,PrefixSym* const pl_word)");
     // word must represent a complex term,
     // pl_word must represent a "plain" complex term,
     // no double variable occurences in pl_word
     // We also assume that all brackets are set inside pl_word.
     Unifier::current()->MarkSection();
     if (UnifyPlain_(word,pl_word)) { return true; }
     else { Unifier::current()->UnbindLastSection(); return false; };
     
    }; // bool UnifyPlain(PrefixSym* const word,PrefixSym* const pl_word)
   
   static inline bool UnifyPlain_(PrefixSym* const word,PrefixSym* const pl_word)
    // word must represent a complex term,
    // pl_word must represent a "plain" complex term,
    // We also assume that all brackets are set inside pl_word.
    {
     CALL("UnifyPlain_(PrefixSym* const word,PrefixSym* const pl_word)");   

     if (word->Head() != pl_word->Head()) { return false; };

     Prefix::Iter iter1;
     Prefix::SimpleIter iter2;
     iter1.Reset(word);
     iter2.Reset(pl_word);
     iter1.SkipSym();
     iter2.SkipSym();
     check_iter:
      if (iter2.Empty()) { return true; };
      if (iter1.CurrVar()) // free variable
       {
        if (iter2.CurrPos()->Head().isVariable())
         {
          Unifier::current()->Bind(iter1.CurrVar(),static_cast<void*>(iter2.CurrPos()->Head().var()));
          iter2.SkipSym();
         }
        else
         {
          Unifier::current()->Bind(iter1.CurrVar(),static_cast<void*>(iter2.CurrPos()));
          iter2.SkipTerm();
         };
        iter1.SkipSym();
        goto check_iter;
       }
      else // first is a complex term
       {
        if (iter2.CurrPos()->Head().isVariable())
         {
          Prefix::CorrectBracket(iter1.CurrSubterm());
          Unifier::current()->Bind(iter2.CurrPos()->Head().var(),iter1.CurrSubterm());
          iter1.SkipTerm();
          iter2.SkipSym();
          goto check_iter;
         }
        else // both are complex, compare headers
         {
          if (iter1.CurrSubterm()->Head() != iter2.CurrPos()->Head())
           {
            return false;
           };
          iter1.SkipSym();
          iter2.SkipSym();
          goto check_iter;
         };
       };
    }; // bool UnifyPlain_(PrefixSym* const word,PrefixSym* const pl_word)
    
   static inline bool MatchGroundPlain(PrefixSym* const word,PrefixSym* const gp_word)
    // word must represent a complex term,
    // gp_word must represent a ground and "plain" term,
    // "plain" means that there are no bound variables in the term.
    // We also assume that all brackets are set inside gp_word.
    {
     CALL("MatchGroundPlain(PrefixSym* const word,PrefixSym* const gp_word)");
     Unifier::current()->MarkSection();
     if (MatchGroundPlain_(word,gp_word)) { return true; }
     else { Unifier::current()->UnbindLastSection(); return false; };
    }; // bool MatchGroundPlain(PrefixSym* const word,PrefixSym* const gp_word)
   
   static inline bool MatchGroundPlain_(PrefixSym* const word,PrefixSym* const gp_word)
    // word must represent a complex term,
    // gp_word must represent a ground and "plain" term,
    // "plain" means that there are no bound variables in the term.
    // We also assume that all brackets are set inside gp_word.
    {
     CALL("MatchGroundPlain_(PrefixSym* const word,PrefixSym* const gp_word)");
     if (word->Head() != gp_word->Head()) { return false; };

     Unifier::Variable* var;
     Prefix::Iter iter1;
     Prefix::SimpleIter iter2;
     iter1.Reset(word);
     iter2.Reset(gp_word);
     iter1.SkipSym();
     iter2.SkipSym();
     next_sym:
      if (iter1.Empty())
       {
        return true;
       }
      else
       {
        var = iter1.CurrVar();
        if (var)
         {
          Unifier::current()->Bind(var,static_cast<void*>(iter2.CurrPos()));
          iter1.SkipSym();
          iter2.SkipTerm();
          goto next_sym;
         }
        else // complex
         {
          if (iter1.CurrSubterm()->Head() == iter2.CurrPos()->Head())
           {
            iter1.SkipSym();
            iter2.SkipSym();
            goto next_sym;
           }
          else
           {
            return false;
           };
         };
       }; 
    }; // bool MatchGroundPlain_(PrefixSym* word,PrefixSym* gp_word)
 }; // class UNIFICATION

 
//***************** Compiled unification: *******************************

inline bool Unify(IU_COMMAND* code,PrefixSym* word)
 {
  CALL("Unify(IU_COMMAND* code,PrefixSym* word)");
  
  if ((code->Symbol() != word->Head()) // different headers
      || ((!(code->AllVars())) // the code represents a ground term
           && (Prefix::Weight(code->Term()) < Prefix::Weight(word))))
   {
    return false; 
   };  
  
  void* bind;
  bool free_var;
  
  Unifier::current()->MarkSection(); 
   
  IU_COMMAND* current_command = code + 1;
  Prefix::Iter iter;
  iter.Reset(word);
  iter.SkipSym(); // skipping the header
  
  perform_command:
   
   switch (current_command->Tag())
    {
     case IU_COMMAND::START: break; // error
     case IU_COMMAND::VAR: 
      bind = current_command->Symbol().var()->Unref(free_var);
      if (free_var)
       {
        if (iter.CurrVar()) // both are free variables
         {
          if (static_cast<Unifier::Variable*>(bind) != iter.CurrVar())
           {
            Unifier::current()->Bind((static_cast<Unifier::Variable*>(bind)),static_cast<void*>(iter.CurrVar()));       
           };
          iter.SkipSym();
         }
        else // iter.CurrSubterm() is a complex term
         {
          if (OccurCheck::Occurs(static_cast<Unifier::Variable*>(bind),iter.CurrSubterm()))
           {
            // cycle
            goto failure;
           }
          else
           {
            Unifier::current()->Bind((static_cast<Unifier::Variable*>(bind)),static_cast<void*>(iter.CurrSubterm()));
            iter.SkipTerm();
           };
         };
       }
      else // bind is a complex term
       {
        if (iter.CurrVar()) 
         {
          if (OccurCheck::Occurs(iter.CurrVar(),static_cast<PrefixSym*>(bind)))
           {
            // cycle
            goto failure;
           };
          Unifier::current()->Bind(iter.CurrVar(),bind);
          iter.SkipSym();
         }
        else // both terms are complex, unify them
         {
          if (UNIFICATION::UnifyComplex_(iter.CurrSubterm(),static_cast<PrefixSym*>(bind)))
           {
            iter.SkipTerm();
           }
          else { goto failure; };
         };
       };
      current_command++;
      goto perform_command;

     case IU_COMMAND::VAR_FIRST_OCC:
      if (iter.CurrVar()) // both are free variables
       {
        Unifier::current()->Bind(current_command->Symbol().var(),static_cast<void*>(iter.CurrVar()));
        iter.SkipSym();
       }
      else // iter.CurrSubterm() is a complex term
       {
        Unifier::current()->Bind(current_command->Symbol().var(),static_cast<void*>(iter.CurrSubterm()));
        iter.SkipTerm();
       };
      current_command++;
      goto perform_command;
      
     case IU_COMMAND::FUNC:
      if (iter.CurrVar())
       {
        if (OccurCheck::Occurs(iter.CurrVar(),current_command->Term()))
         {
          // cycle
          goto failure;
         };
        Unifier::current()->Bind(iter.CurrVar(),static_cast<void*>(current_command->Term()));
        current_command = current_command->NextCommand();
       }
      else // iter.CurrSubterm() is a complex term, compare headers
       {
        if (current_command->Symbol() != iter.CurrSubterm()->Head())
         {
          // clash of functors
          goto failure;
         };
        current_command++; 
       };
      iter.SkipSym();
      goto perform_command; 

     case IU_COMMAND::FUNC_PLAIN:
      if (iter.CurrVar())
       {
        Unifier::current()->Bind(iter.CurrVar(),static_cast<void*>(current_command->Term()));
        current_command = current_command->NextCommand();
       }
      else
       {
        if (current_command->Symbol() != iter.CurrSubterm()->Head())
         {
          // clash of functors
          goto failure;
         };
        current_command++;
       };
      iter.SkipSym();
      goto perform_command;
      
     case IU_COMMAND::FUNC_GROUND:
     
      if (iter.CurrVar())
       {
        Unifier::current()->Bind(iter.CurrVar(),static_cast<void*>(current_command->Term()));
        current_command = current_command->NextCommand();
       }
      else
       {
        if ((Prefix::Weight(current_command->Term()) < Prefix::Weight(iter.CurrSubterm()))
            || (current_command->Symbol() != iter.CurrSubterm()->Head()))
         {
          goto failure;
         };
        current_command++;
       };
      iter.SkipSym(); 
      goto perform_command;
      
     case IU_COMMAND::CONST:
     
      if (iter.CurrVar())
       {
        Unifier::current()->Bind(iter.CurrVar(),static_cast<void*>(current_command->Term()));
       }
      else 
       {
        if (current_command->Symbol() != iter.CurrSubterm()->Head())
         {
          // clash of functors
          goto failure;
         };
       };
      
      iter.SkipSym(); 
      current_command++; 
      goto perform_command;
       
     case IU_COMMAND::END: return true;
      
    };

  ICP("ICP1");

  failure:
   Unifier::current()->UnbindLastSection();
   return false;
 }; // bool Unify(IU_COMMAND* code,PrefixSym* word)
}; // namespace VK
//=============================================================
#endif

 
