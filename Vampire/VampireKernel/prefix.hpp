#ifndef PREFIX_H
//==================================================================
#define PREFIX_H
#ifndef NO_DEBUG_VIS 
 #include <iostream>
#endif
#include "jargon.hpp"
#include "VampireKernelDebugFlags.hpp"
#include "VampireKernelConst.hpp"
#include "Term.hpp"
#include "variables.hpp"
#include "Stack.hpp"
//================================================
#undef DEBUG_NAMESPACE
#ifdef DEBUG_PREFIX
 #define DEBUG_NAMESPACE "PrefSym"
#endif
#include "debugMacros.hpp"
//=================================================
namespace VK
{

class PrefSym 
{
 private:
  TERM sym;
 public:
  PrefSym() {};
  PrefSym(const TERM& s) 
    {
      if (s.isVariable())
	{
	  MkVar(Unifier::current()->variableBase(s.var()));
	}
      else // functor
	{
	  sym = s;
	};
    };
  ~PrefSym() {};
  bool IsFunctor() const { return !isVariable(); };
  bool isVariable() const { return sym.Flag() == TermRef; };
  const TERM& Func() const { return sym; };
  TERM& Func() { return sym; };
  Unifier::Variable* var() const 
  { 
    return static_cast<Unifier::Variable*>(static_cast<void*>(sym.First())); 
  };
  void MkFunc(const TERM& f) { sym = f; };
  void Inversepolarity() { sym.Inversepolarity(); };
  void MkVar(const Unifier::Variable* v) 
  { 
    sym.SetFirst(const_cast<TERM*>(static_cast<const TERM*>(static_cast<const void*>(v)))); 
  };
  int operator==(const PrefSym& s) const 
  { return sym.Content() == s.Func().Content(); };
  int operator!=(const PrefSym& s) const 
  { return sym.Content() != s.Func().Content(); };
 public: // methods for debugging and logging
  ostream& output(ostream& str) const { return output(str,0); };
  ostream& output(ostream& str,unsigned var_offset) const
    {
      if (IsFunctor()) return str << Func();
      return (var() + var_offset)->output(str);
    };
}; // class PrefSym

}; // namespace VK
//================================================
#undef DEBUG_NAMESPACE
#ifdef DEBUG_PREFIX
#define DEBUG_NAMESPACE "PrefixSym"
#endif
#include "debugMacros.hpp"
//================================================= 
namespace VK
{
class PrefixSym
{
 private: // structure
  PrefSym head;
  PrefixSym* bracket;
 public: 
  PrefixSym() {};
  ~PrefixSym() {};
  PrefSym& Head() { return head; };
  const PrefSym& Head() const { return head; };
  void SetBracket(PrefixSym* br) { bracket = br; };
  void ResetBracket() { bracket = 0; };
  PrefixSym* Bracket() const { return bracket; };
  PrefixSym* Arg1() { return this + 1; };
  PrefixSym* Arg2() { return Arg1()->Bracket(); };
 public: // methods for debugging and logging
  ostream& output(ostream& str) const
    {
      return Head().output(str) << " [" << (ulong)Bracket() << ']'; 
    };
}; // class PrefixSym
}; // namespace VK

//================================================
#undef DEBUG_NAMESPACE
#ifdef DEBUG_PREFIX
#define DEBUG_NAMESPACE "Prefix"
#endif
#include "debugMacros.hpp"
//================================================= 

namespace VK
{
class Prefix
{
 public:
  static long Weight(const PrefixSym* word) { return (word->Bracket() - word); };
  static ulong WeightWithSubst(const PrefixSym* word) 
    {
      CALL("WeightWithSubst(const PrefixSym* word)");
      // word must represent a complex term here
      ASSERT(word->Head().IsFunctor());
      ulong res = 0; 
      Iter iter; 
      iter.Reset((PrefixSym*)word);
      do
	{ 
	  res++;
	  iter.SkipSym();
	}
      while (iter.NotEmpty());
      return res;
    };  

  static bool GroundnessAndWeight(const PrefixSym* word,ulong& weight)
    {
      CALL("GroundnessAndWeight(const PrefixSym* word,ulong& weight)");
      // word must represent a complex term here
      ASSERT(word->Head().IsFunctor());
      weight = 0;
      Iter iter; 
      iter.Reset((PrefixSym*)word);
      do
	{ 
	  if (iter.CurrVar()) return false;      
	  weight++;
	  iter.SkipSym();
	}
      while (iter.NotEmpty());
      return true;
    }; // bool GroundnessAndWeight(const PrefixSym* word,ulong& weight)


 public:
  class SimpleIter
    {
    private:
      PrefixSym* curr_pos;
      PrefixSym* end_of_word;
    public:
      SimpleIter() {};
      SimpleIter(PrefixSym*word) // word must represent a complex term here
	{
	  CALL("SimpleIter::constructor SimpleIter(PrefixSym* word)");
	  Reset(word);
	};
      void Reset(PrefixSym* word) // word must represent a complex term here
	{
	  CALL("SimpleIter::Reset(PrefixSym* word)");
	  ASSERT(Prefix::CorrectComplexTerm(word)); 
	  curr_pos = word;
	  end_of_word = word->Bracket();
	};
      void MemorizeState(SimpleIter& memo) const
	{
	  memo.curr_pos = curr_pos;
	  memo.end_of_word = end_of_word;
	};
      void RestoreState(const SimpleIter& memo)
	{
	  curr_pos = memo.curr_pos;
	  end_of_word = memo.end_of_word;
	}; 
      PrefixSym* CurrPos() const { return curr_pos; };
      bool Empty() const { return (curr_pos == end_of_word); };
      bool NotEmpty() const { return end_of_word != curr_pos; };
      void SkipSym()
	{
	  CALL("SimpleIter::SkipSym()");
	  curr_pos++; 
	  ASSERT(curr_pos <= end_of_word);
	};

      void SkipTerm()
	{
	  CALL("SimpleIter::SkipTerm()");
	  ASSERT((curr_pos < curr_pos->Bracket()) 
		 && (curr_pos->Bracket() - curr_pos <= (long)VampireKernelConst::MaxTermSize));
	  curr_pos = curr_pos->Bracket();
	  ASSERT(curr_pos);
	};
    }; // class Prefix::SimpleIter

  class SimpleConstIter
    {
    private:
      const PrefixSym* curr_pos;
      const PrefixSym* end_of_word;
    public:
      SimpleConstIter() {};
      SimpleConstIter(PrefixSym*word) // word must represent a complex term here
	{
	  CALL("SimpleConstIter::constructor SimpleConstIter(PrefixSym* word)");
	  Reset(word);
	};
      void Reset(const PrefixSym* word) // word must represent a complex term here
	{
	  CALL("SimpleConstIter::Reset(PrefixSym* word)");
	  ASSERT(Prefix::CorrectComplexTerm(word)); 
	  curr_pos = word;
	  end_of_word = word->Bracket();
	};
      void MemorizeState(SimpleConstIter& memo) const
	{
	  memo.curr_pos = curr_pos;
	  memo.end_of_word = end_of_word;
	};
      void RestoreState(const SimpleConstIter& memo)
	{
	  curr_pos = memo.curr_pos;
	  end_of_word = memo.end_of_word;
	}; 
      const PrefixSym* CurrPos() const { return curr_pos; };
      bool Empty() const { return (curr_pos == end_of_word); };
      bool NotEmpty() const { return end_of_word != curr_pos; };
      void SkipSym()
	{
	  CALL("SimpleConstIter::SkipSym()");
	  curr_pos++; 
	  ASSERT(curr_pos <= end_of_word);
	};

      void SkipTerm()
	{
	  CALL("SimpleConstIter::SkipTerm()");
	  ASSERT((curr_pos < curr_pos->Bracket()) 
		 && (curr_pos->Bracket() - curr_pos <= (long)VampireKernelConst::MaxTermSize));
	  curr_pos = curr_pos->Bracket();
	  ASSERT(curr_pos);
	};
    }; // class Prefix::SimpleConstIter


 public:
  class Iter : private SimpleIter
    {
    private:
      SimpleIter* next_continuation;
      SimpleIter continuation[VampireKernelConst::MaxTermDepth];
      Unifier::Variable* curr_var;
    private:
      void PopContinuation()
	{
	  next_continuation--;
	  SimpleIter::RestoreState(*next_continuation);
	};
      void PushContinuation()
	{
	  SimpleIter::MemorizeState(*next_continuation); 
	  next_continuation++;
	};
    public: 
      Iter() {};
      Iter(PrefixSym* word) { Reset(word); };
      void Reset(PrefixSym* word) // word must represent a complex term here
	{
	  CALL("Iter::Reset(PrefixSym* word)");
	  SimpleIter::Reset(word);
	  ASSERT(word);
	  ASSERT(word->Head().IsFunctor());
	  ASSERT(word->Bracket());
	  //continuation_num = 0;
	  next_continuation = continuation;
	  curr_var = (Unifier::Variable*)0;
	};
      bool Empty() const
	{
	  CALL("Iter::Empty() const"); 
#ifdef DEBUG_NAMESPACE
	  if (SimpleIter::Empty()) 
	    {
	      ASSERT(next_continuation == continuation);
	      return true;
	    }
	  else { return false; };
#else
	  return SimpleIter::Empty(); 
#endif
	};
      bool NotEmpty() const { return SimpleIter::NotEmpty(); };
      Unifier::Variable* CurrVar() const { return curr_var; };
      PrefixSym* CurrSubterm() const { return SimpleIter::CurrPos(); };
      void SkipSym() // can't be applied if the iterator is empty
	{
	  CALL("Iter::SkipSym()");
	  // SimpleIter can't be empty here
	  ASSERT(!(SimpleIter::Empty()));

	  SimpleIter::SkipSym();
	  if (SimpleIter::Empty()) 
	    {
	      if (next_continuation - continuation)
		{
		  PopContinuation();
		}
	      else
		{
		  return; // empty iterator now
		};
	    };
	  if (SimpleIter::CurrPos()->Head().isVariable())
	    {
	      curr_var = SimpleIter::CurrPos()->Head().var();
	      void* bind = curr_var->Binding();
	      if (bind) // bound variable
		{
		check_bind_type: 
		  if (Unifier::current()->isVariable(bind))
		    {
		      curr_var = (Unifier::Variable*)bind;
		      bind = curr_var->Binding();
		      if (bind) { goto check_bind_type; };
		    }
		  else // variable bound by a complex term
		    {
		      curr_var = (Unifier::Variable*)0;
		      SimpleIter::SkipSym();
		      if (SimpleIter::NotEmpty())
			{
			  PushContinuation();
			};
		      SimpleIter::Reset((PrefixSym*)bind);
		    };
		};
	    }
	  else
	    {
	      curr_var = (Unifier::Variable*)0;
	    };
	}; // void SkipSym()  

      void SkipTerm() // can't be applied if the iterator is empty
	{
	  CALL("Iter::SkipTerm()");
	  // SimpleIter can't be empty here
	  ASSERT(!(SimpleIter::Empty()));
        
	  SimpleIter::SkipTerm(); // the only difference with void SkipSym() 
	  if (SimpleIter::Empty()) 
	    {
	      if (next_continuation - continuation)
		{
		  PopContinuation();
		}
	      else
		{
		  return; // empty iterator now
		};
	    };
	  if (SimpleIter::CurrPos()->Head().isVariable())
	    {
	      curr_var = SimpleIter::CurrPos()->Head().var();
	      void* bind = curr_var->Binding();
	      if (bind) // bound variable
		{
		check_bind_type: 
		  if (Unifier::current()->isVariable(bind))
		    {
		      curr_var = (Unifier::Variable*)bind;
		      bind = curr_var->Binding();
		      if (bind) { goto check_bind_type; };
		    }
		  else // variable bound by a complex term
		    {
		      curr_var = (Unifier::Variable*)0;
		      SimpleIter::SkipSym();
		      if (SimpleIter::NotEmpty())
			{
			  PushContinuation();
			};
		      SimpleIter::Reset((PrefixSym*)bind);
		    };
		};
	    }
	  else
	    {
	      curr_var = (Unifier::Variable*)0;
	    };
	}; // void SkipTerm()
    }; // class Prefix::Iter 



  class ConstIter : private SimpleConstIter
    {
    private:
      SimpleConstIter* next_continuation;
      SimpleConstIter continuation[VampireKernelConst::MaxTermDepth];
      Unifier::Variable* curr_var;
    private:
      void PopContinuation()
	{
	  next_continuation--;
	  SimpleConstIter::RestoreState(*next_continuation);
	};
      void PushContinuation()
	{
	  SimpleConstIter::MemorizeState(*next_continuation); 
	  next_continuation++;
	};
    public: 
      ConstIter() {};
      ConstIter(const PrefixSym* word) { Reset(word); };
      void Reset(const PrefixSym* word) // word must represent a complex term here
	{
	  CALL("ConstIter::Reset(const PrefixSym* word)");
	  SimpleConstIter::Reset(word);
	  ASSERT(word);
	  ASSERT(word->Head().IsFunctor());
	  ASSERT(word->Bracket());
	  //continuation_num = 0;
	  next_continuation = continuation;
	  curr_var = (Unifier::Variable*)0;
	};
      bool Empty() const
	{
	  CALL("ConstIter::Empty() const"); 
#ifdef DEBUG_NAMESPACE
	  if (SimpleConstIter::Empty()) 
	    {
	      ASSERT(next_continuation == continuation);
	      return true;
	    }
	  else { return false; };
#else
	  return SimpleConstIter::Empty(); 
#endif
	};
      bool NotEmpty() const { return SimpleConstIter::NotEmpty(); };
      Unifier::Variable* CurrVar() const { return curr_var; };
      const PrefixSym* CurrSubterm() const { return SimpleConstIter::CurrPos(); };
      void SkipSym() // can't be applied if the iterator is empty
	{
	  CALL("ConstIter::SkipSym()");
	  // SimpleConstIter can't be empty here
	  ASSERT(!(SimpleConstIter::Empty()));

	  SimpleConstIter::SkipSym();
	  if (SimpleConstIter::Empty()) 
	    {
	      if (next_continuation - continuation)
		{
		  PopContinuation();
		}
	      else
		{
		  return; // empty iterator now
		};
	    };
	  if (SimpleConstIter::CurrPos()->Head().isVariable())
	    {
	      curr_var = SimpleConstIter::CurrPos()->Head().var();
	      void* bind = curr_var->Binding();
	      if (bind) // bound variable
		{
		check_bind_type: 
		  if (Unifier::current()->isVariable(bind))
		    {
		      curr_var = (Unifier::Variable*)bind;
		      bind = curr_var->Binding();
		      if (bind) { goto check_bind_type; };
		    }
		  else // variable bound by a complex term
		    {
		      curr_var = (Unifier::Variable*)0;
		      SimpleConstIter::SkipSym();
		      if (SimpleConstIter::NotEmpty())
			{
			  PushContinuation();
			};
		      SimpleConstIter::Reset((PrefixSym*)bind);
		    };
		};
	    }
	  else
	    {
	      curr_var = (Unifier::Variable*)0;
	    };
	}; // void SkipSym()  

      void SkipTerm() // can't be applied if the iterator is empty
	{
	  CALL("ConstIter::SkipTerm()");
	  // SimpleConstIter can't be empty here
	  ASSERT(!(SimpleConstIter::Empty()));
        
	  SimpleConstIter::SkipTerm(); // the only difference with void SkipSym() 
	  if (SimpleConstIter::Empty()) 
	    {
	      if (next_continuation - continuation)
		{
		  PopContinuation();
		}
	      else
		{
		  return; // empty iterator now
		};
	    };
	  if (SimpleConstIter::CurrPos()->Head().isVariable())
	    {
	      curr_var = SimpleConstIter::CurrPos()->Head().var();
	      void* bind = curr_var->Binding();
	      if (bind) // bound variable
		{
		check_bind_type: 
		  if (Unifier::current()->isVariable(bind))
		    {
		      curr_var = (Unifier::Variable*)bind;
		      bind = curr_var->Binding();
		      if (bind) { goto check_bind_type; };
		    }
		  else // variable bound by a complex term
		    {
		      curr_var = (Unifier::Variable*)0;
		      SimpleConstIter::SkipSym();
		      if (SimpleConstIter::NotEmpty())
			{
			  PushContinuation();
			};
		      SimpleConstIter::Reset((PrefixSym*)bind);
		    };
		};
	    }
	  else
	    {
	      curr_var = (Unifier::Variable*)0;
	    };
	}; // void SkipTerm()
    }; // class Prefix::ConstConstIter 


 public:
  class IterWithBacktracking : private SimpleIter
    {
    private:
      enum ActionToUndo
      {
        NOTHING = 0,
        POP = 1, // = NOTHING + POP
        PUSH = 2, // = NOTHING + PUSH
        POP_AND_PUSH = 3 // = POP + PUSH
      };
    private:
      SimpleIter* next_continuation;
      SimpleIter continuation[VampireKernelConst::MaxTermDepth];
      Unifier::Variable* current_var[VampireKernelConst::MaxTermSize];
      Unifier::Variable** next_curr_var;
      Unifier::Variable* curr_var;
    private:
#ifdef DEBUG_NAMESPACE
      PrefixSym* top_level;
#endif
       
    private:
      ActionToUndo undo[VampireKernelConst::MaxTermSize];
      unsigned undo_num;
      SimpleIter state[VampireKernelConst::MaxTermSize];
      SimpleIter used_continuation[VampireKernelConst::MaxTermSize];
    private:
      void PushCurrVar() 
	{
	  CALL("IterWithBacktracking::PushCurrVar()");
	  *next_curr_var = curr_var;
	  next_curr_var++;
	  ASSERT(next_curr_var - current_var <= (long)VampireKernelConst::MaxTermSize);
	};   
      void PopCurrVar()
	{
	  CALL("IterWithBacktracking::PopCurrVar()");
	  next_curr_var--;
        
	  ASSERT(next_curr_var >= current_var);
        
	  curr_var = *next_curr_var;
	};   
      void PopContinuation()
	{
	  next_continuation--;
	  SimpleIter::RestoreState(*next_continuation);
	};
      void PushContinuation()
	{
	  SimpleIter::MemorizeState(*next_continuation); 
	  next_continuation++;
	};
    public:
#ifdef DEBUG_NAMESPACE
      PrefixSym* TopLevel() { return top_level; };
#endif  
    public: 
      IterWithBacktracking() {};
      void Reset(PrefixSym* word) // word must represent a complex term here
	{
	  CALL("IterWithBacktracking::Reset(PrefixSym* word)");
	  ASSERT(word);
	  ASSERT(word->Head().IsFunctor());
	  ASSERT(word->Bracket());
        
	  SimpleIter::Reset(word);
        
#ifdef DEBUG_NAMESPACE
	  top_level = word;
#endif
        
	  next_continuation = continuation;
	  curr_var = (Unifier::Variable*)0;
	  next_curr_var = current_var;
	  undo_num = 0;
	};
      bool Empty() const { return SimpleIter::Empty(); };
      bool NotEmpty() const { return SimpleIter::NotEmpty(); };
      Unifier::Variable* CurrVar() const { return curr_var; };
      PrefixSym* CurrSubterm() const { return SimpleIter::CurrPos(); };
      void SkipSym() // can't be applied if the iterator is empty
	{
	  CALL("IterWithBacktracking::SkipSym()");
	  // SimpleIter can't be empty here
	  ASSERT(!(SimpleIter::Empty()));
        
	  SimpleIter::MemorizeState(state[undo_num]);
	  PushCurrVar();
        
	  unsigned undo_action = NOTHING;
	  SimpleIter::SkipSym();
	  if (SimpleIter::Empty()) 
	    {
	      if (next_continuation - continuation)
		{
		  undo_action += POP;
		  PopContinuation();
		  SimpleIter::MemorizeState(used_continuation[undo_num]);
		}
	      else
		{
		  undo[undo_num] = (ActionToUndo)undo_action;
		  undo_num++;
		  return; // empty iterator now
		};
	    };
	  if (SimpleIter::CurrPos()->Head().isVariable())
	    {
	      curr_var = SimpleIter::CurrPos()->Head().var();
	      void* bind = curr_var->Binding();
	      if (bind) // bound variable
		{
		check_bind_type: 
		  if (Unifier::current()->isVariable(bind))
		    {
		      curr_var = (Unifier::Variable*)bind;
		      bind = curr_var->Binding();
		      if (bind) { goto check_bind_type; };
		    }
		  else // variable bound by a complex term
		    {
		      curr_var = (Unifier::Variable*)0;
		      SimpleIter::SkipSym();
		      if (SimpleIter::NotEmpty())
			{
			  undo_action += PUSH;
			  PushContinuation();
			};
		      SimpleIter::Reset((PrefixSym*)bind);
		    };
		};
	    }
	  else
	    {
	      curr_var = (Unifier::Variable*)0;
	    };
	  undo[undo_num] = (ActionToUndo)undo_action;
	  undo_num++;
	}; // void SkipSym() 
       
      void SkipTerm() // can't be applied if the iterator is empty
	{
	  CALL("IterWithBacktracking::SkipTerm()");
	  // SimpleIter can't be empty here
	  ASSERT(!(SimpleIter::Empty()));
	  SimpleIter::MemorizeState(state[undo_num]);
	  PushCurrVar();
        
	  unsigned undo_action = NOTHING;
	  SimpleIter::SkipTerm(); // the only difference with void SkipSym() 
	  if (SimpleIter::Empty()) 
	    {
	      if (next_continuation - continuation)
		{
		  undo_action += POP;
		  PopContinuation();
		  SimpleIter::MemorizeState(used_continuation[undo_num]);
		}
	      else
		{
		  undo[undo_num] = (ActionToUndo)undo_action;
		  undo_num++;
		  return; // empty iterator now
		};
	    };
	  if (SimpleIter::CurrPos()->Head().isVariable())
	    {
	      curr_var = SimpleIter::CurrPos()->Head().var();
	      void* bind = curr_var->Binding();
	      if (bind) // bound variable
		{
		check_bind_type: 
		  if (Unifier::current()->isVariable(bind))
		    {
		      curr_var = (Unifier::Variable*)bind;
		      bind = curr_var->Binding();
		      if (bind) { goto check_bind_type; };
		    }
		  else // variable bound by a complex term
		    {
		      curr_var = (Unifier::Variable*)0;
		      SimpleIter::SkipSym();
		      if (SimpleIter::NotEmpty())
			{
			  undo_action += PUSH;
			  PushContinuation();
			};
		      SimpleIter::Reset((PrefixSym*)bind);
		    };
		};
	    }
	  else
	    {
	      curr_var = (Unifier::Variable*)0;
	    };
	  undo[undo_num] = (ActionToUndo)undo_action;
	  undo_num++;
	}; // void SkipTerm() 
     
      void Backtrack()
	{
	  CALL("IterWithBacktracking::Backtrack()");
	  ASSERT(undo_num);
	  undo_num--;
	  SimpleIter::RestoreState(state[undo_num]);
	  PopCurrVar();
	  switch (undo[undo_num])
	    {
	    case NOTHING: break;
	    case POP:
	      next_continuation->RestoreState(used_continuation[undo_num]);
	      next_continuation++;
	      break;
	    case PUSH:
	      next_continuation--;
	      break;
	    case POP_AND_PUSH: 
	      (next_continuation-1)->RestoreState(used_continuation[undo_num]);
	      break;
#ifdef DEBUG_NAMESPACE
	    default: ICP("default"); 
#endif 
	    };
	}; // void Backtrack()
    }; // class Prefix::IterWithBacktracking
 public: 
  static bool equal(PrefixSym* word1,PrefixSym* word2)
    {
      Iter iter1;
      Iter iter2;
      iter1.Reset(word1);
      iter2.Reset(word2);
      while (!(iter1.Empty()))
	{
	  if (iter1.CurrVar())
	    {
	      if (iter1.CurrVar() != iter2.CurrVar()) { return false; };
	    }
	  else
	    {
	      if ((iter2.CurrVar()) 
		  || (iter1.CurrSubterm()->Head() != iter2.CurrSubterm()->Head()))
		{ return false; };
	    };
       
	  iter1.SkipSym();
	  iter2.SkipSym();
	};
      return true;
    }; 
    
#ifndef NO_DEBUG
  static bool CorrectComplexTerm(const PrefixSym* word)
    {
      CALL("CorrectComplexTerm(const PrefixSym* word)");

      if (!word) 
	{
	  DMSG("Prefix::CorrectComplexTerm(word)> !word\n");
	  return false; 
	};
      if (!word->Head().IsFunctor())
	{
	  DMSG("Prefix::CorrectComplexTerm(word)> !word->Head().IsFunctor()\n");
	  return false;
	};
      if (!word->Bracket())
	{
	  DMSG("Prefix::CorrectComplexTerm(word)> !word->Bracket()\n");
	  return false;
	};
      if (word->Bracket() <= word) 
	{
	  DMSG("Prefix::CorrectComplexTerm(word)> word->Bracket() <= word\n");
	  return false;
	};

      
      PrefixSym* end = word->Bracket();
      long holes = word->Head().Func().arity();
      word++;
      while (word < end)
	{
	  holes--;
	  if (holes < 0) 
	    {
	      DMSG("Prefix::CorrectComplexTerm(word)> holes < 0\n"); 
	      return false; 
	    };
	  if (word->Head().IsFunctor())
	    {
	      holes += word->Head().Func().arity();
	    }
	  else
	    {
	      if (!Unifier::current()->isVariable(word->Head().var())) 
		{
		  DMSG("Prefix::CorrectComplexTerm(word)> !Unifier::current()->isVariable(word->Head().var())\n"); 
		  return false;
		};
	    };
	  word++;
	};
      if (holes) 
	{
	  DMSG("Prefix::CorrectComplexTerm(word)> holes\n"); 
	  return false;
	};
      return true;
    };
#endif

#ifndef NO_DEBUG_VIS 
  static ostream& outputWord(ostream& str,const PrefixSym* word);
  static ostream& outputTerm(ostream& str,const PrefixSym* word);
#endif
  static ostream& outputWordWithSubst(ostream& str,const PrefixSym* word);

 public:
  static void CorrectBracket(PrefixSym* word)
    {
      if (!(word->Bracket()))
	{
	  PrefixSym* curr_pos = word+1; 
	  BracketCounter<VampireKernelConst::MaxTermDepth> brack_counter;
	  brack_counter.Open(word->Head().Func().arity());
	  do
	    {
	      if (curr_pos->Head().isVariable() || (!(curr_pos->Head().Func().arity())))
		{
		  brack_counter.Close();
		}
	      else { brack_counter.Open(curr_pos->Head().Func().arity()); };
	      curr_pos++;
	    }
	  while (brack_counter.IsOpen());
	  word->SetBracket(curr_pos);
	};
    }; // void CorrectBracket(PrefixSym* word)
 public: 
  static void ResetCorruptedBrackets(PrefixSym* word,
				     PrefixSym* changed_suffix,
				     PrefixSym* old_end_of_word,
				     PrefixSym* new_end_of_word)
    {
      while (word < changed_suffix)
	{
	  if (word->Bracket() > changed_suffix)
	    {
	      if (word->Bracket() == old_end_of_word)
		{
		  word->SetBracket(new_end_of_word);
		}
	      else
		{
		  word->ResetBracket();
		};
	    };
	  word++;
	};
    }; // void ResetCorruptedBrackets(..........
}; // class Prefix

}; // namespace VK

//================================================
#undef DEBUG_NAMESPACE
#ifdef DEBUG_PREFIX
#define DEBUG_NAMESPACE "(Prefix)OccurCheck"
#endif
#include "debugMacros.hpp"
//=================================================  

namespace VK
{
class OccurCheck 
{
 public:
  static bool Occurs(Unifier::Variable* v,PrefixSym* word)
    {
     CALL("Occurs(Unifier::Variable* v,PrefixSym* word)");
     DOP(PrefixSym* debug_top_level_word = word);
     static LARGE_VAR_COUNTER link_counter;
     link_counter.Reset();
      
#ifdef NO_DEBUG
     static BK::Stack<void*,VampireKernelConst::MaxNumOfVariables * VampireKernelConst::MaxNumOfIndices > 
       _links;    

#else
     static BK::Stack<void*,VampireKernelConst::MaxNumOfVariables * VampireKernelConst::MaxNumOfIndices > 
       _links("OccurCheck::Occurs(..):_links");    
      
#endif

     _links.reset();

		     //void* links[VampireKernelConst::MaxTmpTermSize];
		     //void** next_link = links;

     Prefix::SimpleIter iter;
     void* link;
     
     check_top_level:
      iter.Reset(word);
      iter.SkipSym();
     
      while (!(iter.Empty()))
       {
        PrefSym sym  = iter.CurrPos()->Head();
        if (sym.isVariable())
         {
          link = sym.var()->Binding();
          if (link) // bound variable
           {
            if (!(link_counter.Register(sym.var())))
	      {
		_links.push(link);
		//*next_link = link;
		//next_link++;
	      };
           }
          else // free variable
           {
            if (sym.var() == v) 
             {
              ASSERT(OccursIneff(v,debug_top_level_word));
              return true; 
             };
           };
         };
        iter.SkipSym();
       }; 
     
      try_next_link:
       if (_links.empty())
       //if (next_link == links) 
        {
         ASSERT(!(OccursIneff(v,debug_top_level_word)));
         return false; 
        };
   
       link = _links.pop();
       //next_link--;
       //link = *next_link;
       check_link:
        if (Unifier::current()->isVariable(link))
         {
          if (((Unifier::Variable*)link)->Binding()) 
           {
            if (!(link_counter.Register((Unifier::Variable*)link)))
             {
              link = ((Unifier::Variable*)link)->Binding();
              goto check_link;
             }
            else
             {
              goto try_next_link;
             };
           }
          else // free var
           {
            if (((Unifier::Variable*)link) == v) 
             {
              ASSERT(OccursIneff(v,debug_top_level_word));
              return true; 
             };
            goto try_next_link;
           };
         }
        else // complex term
         {
          word = (PrefixSym*)link;
          goto check_top_level;
         };
    }; // bool Occurs(Unifier::Variable* v,PrefixSym* word)
    
   #ifndef NO_DEBUG
    // Inefficient but simple version: 
    static bool OccursIneff(Unifier::Variable* v,PrefixSym* word) 
     // word must represent a complex term, v must be a nonnull pointer
     {
      CALL("OccursIneff(Unifier::Variable* v,PrefixSym* word)");
      ASSERT(v);
      ASSERT(Unifier::current()->isVariable(v));
      ASSERT(Prefix::CorrectComplexTerm(word));
      
      Prefix::Iter iter;
      iter.Reset(word);
      next_sym:
       iter.SkipSym();
       if (iter.Empty())
        {
         return false;
        }
       else
        {
         if (iter.CurrVar() == v)
          {
           return true;
          }
         else
          {
           goto next_sym;
          };
        };
     }; // bool OccursIneff(Unifier::Variable* v,PrefixSym* word) 
   #endif
 }; // class OccurCheck

}; // namespace VK
//================================================
#undef DEBUG_NAMESPACE
#ifdef DEBUG_PREFIX
#define DEBUG_NAMESPACE "StdTermWithSubst"
#endif
#include "debugMacros.hpp"
//=================================================  
namespace VK
{


  class StdTermWithSubst
    {
    private:
      unsigned var_offset;
      Prefix::Iter word;
      TERM::Iterator top;
      bool top_level;
      bool empty;
      PrefSym curr_sym;
      TERM dummyTerm[2];
    public:
      StdTermWithSubst() 
	{
	  dummyTerm[0].MakeComplex((TERM::Arity)1,(TERM::Functor)VampireKernelConst::ReservedPred1,TERM::PositivePolarity);	  
	};
      ~StdTermWithSubst() {};
      void Reset(ulong index,const TERM* t) 
	// t must be a complex term here 
	{
	  CALL("Reset(ulong index,TERM* t)");
	  ASSERT(t->isComplex());
	  top.Reset(t);
	  var_offset = Unifier::variableOffset(index);
	  top_level = true;
	  empty = false;
	  curr_sym.MkFunc(top.Symbol());
	};
      void Reset(ulong index,const TERM& t)
	// t must be either variable or reference
	{
	  CALL("Reset(ulong index,TERM& t)");
	  ASSERT(t.isVariable() || t.IsReference());
	  dummyTerm[1] = t;
	  Reset(index,dummyTerm);
	  SkipSym();
	  ASSERT(!Empty());
	}; 
      bool Empty() const { return empty; };
      const PrefSym& CurrSym() const { return curr_sym; };
      const TERM* CurrPosInBase() const { return top.CurrPos(); }; 
   
      void SkipTerm()
	{
	  CALL("SkipTerm()");
	  if (top_level)
	    {
	      if ((empty = !top.SkipTerm())) return;
	    top_check_sym:
	      if (top.CurrentSymIsVar())
		{
		  bool free_var;
		  void* bind = (Unifier::current()->variableBase(top.Symbol().var()) + var_offset)->Unref(free_var);
		  if (free_var)
		    {
		      curr_sym.MkVar((Unifier::Variable*)bind);
		    }
		  else
		    {
		      top_level = false;
		      word.Reset((PrefixSym*)bind);
		      curr_sym = word.CurrSubterm()->Head();
		    };
		}
	      else // functor
		{
		  curr_sym.MkFunc(top.Symbol());
		};
	    }
	  else // in the substitution
	    {
	      word.SkipTerm();
	      if (word.Empty())
		{
		  top_level = true;
		  if (!top.Next()) return;
		  goto top_check_sym;
		}
	      else
		{
		  if (word.CurrVar())
		    {
		      curr_sym.MkVar(word.CurrVar());
		    }
		  else
		    {
		      curr_sym = word.CurrSubterm()->Head();
		    };
		};
	    };
	};
    
     
      void SkipSym()
	{
	  CALL("SkipSym()");
	  bool free_var;
	  void* bind;
	  if (top_level)
	    {
	    top_next_sym:
	      if (top.Next())
		{
		  if (top.CurrentSymIsVar())
		    {
		      bind = (Unifier::current()->variableBase(top.Symbol().var()) + var_offset)->Unref(free_var);
		      if (free_var)
			{
			  curr_sym.MkVar((Unifier::Variable*)bind);
			}
		      else
			{
			  top_level = false;
			  word.Reset((PrefixSym*)bind);
			  curr_sym = word.CurrSubterm()->Head();
			};
		    }
		  else // functor
		    {
		      curr_sym.MkFunc(top.Symbol());
		    };
		}
	      else { empty = true; };
	    }
	  else
	    {
	      word.SkipSym();
	      if (word.Empty())
		{
		  top_level = true;
		  goto top_next_sym;
		}
	      else
		{
		  if (word.CurrVar())
		    {
		      curr_sym.MkVar(word.CurrVar());
		    }
		  else
		    {
		      curr_sym = word.CurrSubterm()->Head();
		    };
		};
	    };
	};
 
      static bool equal(ulong index1,const TERM* t1,ulong index2,const TERM* t2)
	{
	  CALL("equal(ulong index1,const TERM* t1,ulong index2,const TERM* t2)");
	  ASSERT(t1->isComplex());
	  ASSERT(t2->isComplex());
	  StdTermWithSubst iter1;
	  StdTermWithSubst iter2;
	  iter1.Reset(index1,t1);
	  iter2.Reset(index2,t2);
	  do
	    {
	      if (iter1.CurrPosInBase() == iter2.CurrPosInBase()) { iter1.SkipTerm(); iter2.SkipTerm(); }
	      else  
		if (iter1.CurrSym() == iter2.CurrSym()) { iter1.SkipSym(); iter2.SkipSym(); } 
		else return false;             
	    }
	  while (!iter1.Empty());
	  return true;
	};

    }; // class StdTermWithSubst


  void StandardTermToPrefix(ulong index,const TERM* t,PrefixSym*& memory);

  inline void StandardTermToPrefix(ulong index,const TERM& t,PrefixSym*& memory) 
    {
      if (t.isVariable())
	{
	  memory->Head().MkVar(Unifier::current()->variable(t.var(),index));
	  memory->SetBracket(memory + 1);
	  memory++;
	}
      else StandardTermToPrefix(index,t.First(),memory); 
    }; // void StandardTermToPrefix(ulong index,TERM t,PrefixSym*& memory)


}; // namespace VK
  //==================================================================
#endif 
 
 
 
 
 
 
 
