// Revised:    Jan 16, 2002
//             Bug fix in IU_TRANSLATOR::CompilePseudoLiteral(..).
//             The bug was introduced in v2.64.
//===========================================================
#ifndef IU_TRANSLATOR_H
//===========================================================
#define IU_TRANSLATOR_H
#include "jargon.hpp"
#include "VampireKernelDebugFlags.hpp"
#include "iu_command.hpp"
#include "GlobAlloc.hpp"
#include "ExpandingStack.hpp"
//================================================
#undef DEBUG_NAMESPACE
#ifdef DEBUG_FORWARD_SUPERPOSITION
 #define DEBUG_NAMESPACE "IU_TRANSLATOR"
#endif
#include "debugMacros.hpp"
//=================================================
namespace VK
{

class IU_TRANSLATOR
 {
  public: 
   IU_TRANSLATOR() : var_counter() 
   {
    CALL("constructor IU_TRANSLATOR()");
   };
   void init() 
   {
    CALL("init()");
    var_counter.init();
    _openBrackets.init();
    _bracketCounter.init();
   };
   ~IU_TRANSLATOR() { CALL("destructor ~IU_TRANSLATOR()"); };
   void destroy()
   {
     CALL("destroy()");
     _bracketCounter.destroy();
     _openBrackets.destroy();
     var_counter.destroy();
   };
   inline bool CompileLiteral(PrefixSym* word,
                              IU_COMMAND*& code_mem,
                              PrefixSym*& data_mem,
                              ulong max_code_size);
   inline bool CompilePseudoLiteral(TERM header,
                                    PrefixSym* word,
                                    IU_COMMAND*& code_mem,
                                    PrefixSym*& data_mem,
                                    ulong max_code_size);   
  private:
   LARGE_VAR_COUNTER var_counter;
   BK::ExpandingStack<BK::GlobAlloc,IU_COMMAND*,128UL,IU_TRANSLATOR> _openBrackets;
   ExpandingBracketCounter _bracketCounter;
 }; // class IU_TRANSLATOR

//******************* Definitions for IU_TRANSLATOR: *********************

inline bool IU_TRANSLATOR::CompileLiteral(PrefixSym* word,
                                          IU_COMMAND*& code_mem,
                                          PrefixSym*& data_mem,
                                          ulong max_code_size)
 {
  CALL("CompileLiteral(PrefixSym*,IU_COMMAND*&,PrefixSym*&,ulong)");
  
 
  if (!max_code_size) return false;
  _openBrackets.reset(); 

  Prefix::Iter iter;

  _bracketCounter.reset();

  iter.Reset(word);
     
  IU_COMMAND* var_list = 0; 
  IU_COMMAND* start = code_mem;
  PrefixSym* data = data_mem;
  if (iter.CurrSubterm()->Head().Func().arity())
   {
    _bracketCounter.openSafe(iter.CurrSubterm()->Head().Func().arity());
   };

  _openBrackets.pushSafe(code_mem);

  code_mem->Tag() = IU_COMMAND::START;
  code_mem->Symbol() = word->Head();
  data_mem->Head() = word->Head();
  //code_mem->Symbol().Func().Inversepolarity();
  code_mem->Term() = data_mem;
  code_mem++;
  data_mem++;
  max_code_size--;  
  iter.SkipSym(); // skipping header
     
  var_counter.Reset();
     
  Unifier::Variable* var;
  IU_COMMAND* last_var = code_mem-1;
  IU_COMMAND* last_var_not_fo = code_mem-1;
  ulong closed = 0UL;
     
  check_iter:
   if (!max_code_size) return false; 
   // ^ this check is enough since at most one instruction is written 
   // per iteration cycle
   
   if (iter.Empty()) 
    {
     code_mem->Tag() = IU_COMMAND::END;
     code_mem++;
     max_code_size--;
     data->SetBracket(data_mem); // needed only when the literal is propositional
     start->AllVars() = var_list;
     return true;
    };

   var = iter.CurrVar();
   if (var)
    { 
     if (var_counter.Register(var))
      {
       code_mem->Tag() = IU_COMMAND::VAR;
       last_var_not_fo = code_mem;
      }
     else // first occurence of the variable
      {
       code_mem->Tag() = IU_COMMAND::VAR_FIRST_OCC;
       code_mem->NextVar() = var_list;
       var_list = code_mem;
      };
     code_mem->Symbol().MkVar(var);
        
     last_var = code_mem;
        
     data_mem->Head().MkVar(var);
     data_mem->SetBracket(data_mem + 1);
     closed = _bracketCounter.close();
     //closed = bracket_counter.Close();
    }
   else // functor
    {
        
     ulong arity = iter.CurrSubterm()->Head().Func().arity();
     if (arity)
      {
       code_mem->Tag() = IU_COMMAND::FUNC;
       
       _openBrackets.pushSafe(code_mem);
       
       _bracketCounter.openSafe(arity);

      }
     else // constant
      {
       code_mem->Tag() = IU_COMMAND::CONST;
       closed = _bracketCounter.close();

       data_mem->SetBracket(data_mem + 1);
      };
     code_mem->Symbol() = iter.CurrSubterm()->Head();
     code_mem->Term() = data_mem;
     data_mem->Head() = iter.CurrSubterm()->Head();
    };
       
   iter.SkipSym();
   code_mem++; 
   data_mem++;
   max_code_size--;  

   while (closed)
    {
     IU_COMMAND* currentOpenBracket = _openBrackets.pop(); 
     currentOpenBracket->NextCommand() = code_mem;    
     unsigned length = code_mem - currentOpenBracket;
     currentOpenBracket->Term()->SetBracket(currentOpenBracket->Term() + length);

     if (last_var < currentOpenBracket) // the term is ground
      {
       currentOpenBracket->Tag() = IU_COMMAND::FUNC_GROUND;
      }
     else
      {
       if (last_var_not_fo < currentOpenBracket)
        {
         currentOpenBracket->Tag() = IU_COMMAND::FUNC_PLAIN;
        };
      };

     closed--;
    };
      
   goto check_iter;
 }; // bool IU_TRANSLATOR::CompileLiteral(PrefixSym* word,IU_COMMAND*& code_mem,PrefixSym*& data_mem,ulong max_code_size)


inline bool IU_TRANSLATOR::CompilePseudoLiteral(TERM header,
                                                PrefixSym* word,
                                                IU_COMMAND*& code_mem,
                                                PrefixSym*& data_mem,
                                                ulong max_code_size)
 {
   CALL("CompilePseudoLiteral(TERM,PrefixSym*,IU_COMMAND*&,PrefixSym*&)");
  // word must represent a complex term

  if (!max_code_size) return false;
  _openBrackets.reset(); 


  Prefix::Iter iter;

  _bracketCounter.reset();

  iter.Reset(word);
     
  IU_COMMAND* var_list = 0; 
  IU_COMMAND* start = code_mem;
  PrefixSym* data = data_mem;
  if (iter.CurrSubterm()->Head().Func().arity())
    {
     _bracketCounter.openSafe(iter.CurrSubterm()->Head().Func().arity());
    };

  _openBrackets.pushSafe(code_mem);

  code_mem->Tag() = IU_COMMAND::START;
  
  
  PrefSym pr_header(header);     
  code_mem->Symbol() = pr_header; 
  data_mem->Head() = pr_header;        
  code_mem->Symbol().Func().Inversepolarity(); 
 
  code_mem->Term() = data_mem;
  code_mem++;
  data_mem++;
  max_code_size--;
     
  var_counter.Reset();
     
  Unifier::Variable* var;
  IU_COMMAND* last_var = code_mem-1;
  IU_COMMAND* last_var_not_fo = code_mem-1;
  ulong closed = 0UL;
     
  check_iter:

   if (!max_code_size) return false; 
   // ^ this check is enough since at most one instruction is written 
   // per iteration cycle

   if (iter.Empty()) 
    {

     code_mem->Tag() = IU_COMMAND::END;
     code_mem++;

     data->SetBracket(data_mem); // needed only when the literal is propositional

     start->AllVars() = var_list;
     return true;
    };
   var = iter.CurrVar();
   if (var)
    {
     if (var_counter.Register(var))
      {
       code_mem->Tag() = IU_COMMAND::VAR;
       last_var_not_fo = code_mem;
      }
     else // first occurence of the variable
      {
       code_mem->Tag() = IU_COMMAND::VAR_FIRST_OCC;
       code_mem->NextVar() = var_list;
       var_list = code_mem;
      };
     code_mem->Symbol().MkVar(var);
        
     last_var = code_mem;
        
     data_mem->Head().MkVar(var);
     data_mem->SetBracket(data_mem + 1);
     closed = _bracketCounter.close();
     //closed = bracket_counter.Close();
    }
   else // functor
    {
        
     ulong arity = iter.CurrSubterm()->Head().Func().arity();
     if (arity)
      {
       code_mem->Tag() = IU_COMMAND::FUNC;

       _openBrackets.pushSafe(code_mem);

       _bracketCounter.openSafe(arity);
      }
     else // constant
      {
       code_mem->Tag() = IU_COMMAND::CONST;

       closed = _bracketCounter.close();
       data_mem->SetBracket(data_mem + 1);
      };
     code_mem->Symbol() = iter.CurrSubterm()->Head();
     code_mem->Term() = data_mem;
     data_mem->Head() = iter.CurrSubterm()->Head();
    };
       
   iter.SkipSym();
   code_mem++; 
   data_mem++;
   max_code_size--;

   while (closed)
    {
     IU_COMMAND* currentOpenBracket = _openBrackets.pop(); 
     currentOpenBracket->NextCommand() = code_mem;    
     unsigned length = code_mem - currentOpenBracket;
     currentOpenBracket->Term()->SetBracket(currentOpenBracket->Term() + length);

     if (last_var < currentOpenBracket) // the term is ground
      {
       currentOpenBracket->Tag() = IU_COMMAND::FUNC_GROUND;
      }
     else
      {
       if (last_var_not_fo < currentOpenBracket)
        {
         currentOpenBracket->Tag() = IU_COMMAND::FUNC_PLAIN;
        };
      };

     closed--;
    };
      
   goto check_iter;
 }; // void IU_TRANSLATOR::CompilePseudoLiteral(TERM header,PrefixSym* word,IU_COMMAND*& code_mem,PrefixSym*& data_mem)
}; // namespace VK
//===========================================================
#endif
