#include "forward_subsumption.hpp"
#include "fs_command.hpp"
#include "GlobalStopFlag.hpp"
using namespace BK;
//================================================
#undef DEBUG_NAMESPACE
#ifdef DEBUG_FORWARD_SUBSUMPTION
#define DEBUG_NAMESPACE "ForwardSubsumption"
#endif 
#include "debugMacros.hpp"
//=================================================
using namespace VK;


ForwardSubsumption::ForwardSubsumption() 
  : FS_CODE(subst), 
  setMode(false), // default
  backtrack_lit(DOP("ForwardSubsumption::backtrack_lit")),
  backtrack_instr(DOP("ForwardSubsumption::backtrack_instr")),
  backtrack_cursor(DOP("ForwardSubsumption::backtrack_cursor"))
{
  CALL("constructor ForwardSubsumption()");
  DOP(backtrack_lit.freeze());
};

ForwardSubsumption::~ForwardSubsumption() {};
  
void ForwardSubsumption::init()
{
  CALL("init()");
  FS_CODE::init(subst);
  FS_QUERY::init();
  setMode = false; // default
  backtrack_lit.init(DOP("ForwardSubsumption::backtrack_lit"));
  backtrack_instr.init(DOP("ForwardSubsumption::backtrack_instr"));
  backtrack_cursor.init(DOP("ForwardSubsumption::backtrack_cursor"));
  DOP(backtrack_lit.freeze());
}; // void ForwardSubsumption::init()

void ForwardSubsumption::destroy()
{
  CALL("destroy()");
  backtrack_cursor.destroy();
  backtrack_instr.destroy();
  backtrack_lit.destroy();
  FS_QUERY::destroy();
  FS_CODE::destroy();
}; // void ForwardSubsumption::destroy()

void ForwardSubsumption::reset()
{
  CALL("reset()");
  FS_CODE::destroy();
  FS_CODE::init(subst);
}; // void ForwardSubsumption::reset()

Clause* ForwardSubsumption::SubsumeInSetMode()
{ 
  CALL("SubsumeInSetMode()");  
#ifdef FS_STAT
  anotherCall();
#endif 

  FS_COMMAND* curr_instr = Tree();
  if (!(curr_instr)) return 0; 
  
  EndOfQuery();
  
  const Flatterm* cursor = 0;
  
  TERM curr_sym;
  LIT* curr_lit;
  LIT* next_lit;
  // stacks
  backtrack_lit.reset();
  //LIT* backtrack_lit[VampireKernelConst::MaxNumOfLiterals];
  //LIT** next_backtrack_lit = backtrack_lit;

  backtrack_instr.reset();
  //FS_COMMAND* backtrack_instr[MAX_FS_CODE_DEPTH];
  //FS_COMMAND** next_backtrack_instr = backtrack_instr;
  
  backtrack_cursor.reset();
  //const Flatterm* backtrack_cursor[MAX_FS_CODE_DEPTH];
  //const Flatterm** next_backtrack_cursor = backtrack_cursor;
  
  Clause* subsuming_cl;
 check_tag:
  COP("check_tag");
#ifdef FS_STAT
  anotherInstr();
#endif 
  COP("switch (curr_instr->Tag())");
  switch (curr_instr->Tag())
    {
    case FS_COMMAND::LIT_HEADER:
      COP("LIT_HEADER");
      curr_lit = FirstLit(curr_instr->HeaderNum());
      if (curr_lit)
	{
	  next_lit = curr_lit->Next(); 
	  if (next_lit || curr_instr->Fork())
	    {
	      backtrack_lit.push(next_lit);
	      //*next_backtrack_lit = next_lit;
	      //next_backtrack_lit++;

	      backtrack_instr.push(curr_instr);
	      //*next_backtrack_instr = curr_instr;
	      //next_backtrack_instr++;
	    }; 
	  cursor = curr_lit->Args();
	  curr_instr = curr_instr->Next();
	  goto check_tag;
	}
      else // no literal with this header
	goto try_fork;
     
    case FS_COMMAND::ORD_EQ:
      COP("ORD_EQ");
      curr_lit = FirstLit(curr_instr->HeaderNum());
      if (curr_lit)
	{
	  next_lit = curr_lit->Next(); 
	  if (next_lit || curr_instr->Fork())
	    {
	      backtrack_lit.push(next_lit);
	      //*next_backtrack_lit = next_lit;
	      //next_backtrack_lit++;

	      backtrack_instr.push(curr_instr);
	      //*next_backtrack_instr = curr_instr;
	      //next_backtrack_instr++;
	    };
	  cursor = curr_lit->Arg1();
	  curr_instr = curr_instr->Next();
	  goto check_tag;
	}
      else // no ordered equations with this polarity
	goto try_fork;
       
       
    case FS_COMMAND::UNORD_EQ :
      COP("UNORD_EQ"); 
      curr_lit = FirstLit(curr_instr->HeaderNum());
      if (curr_lit)
	{
	  backtrack_lit.push(curr_lit);
	  //*next_backtrack_lit = curr_lit; 
	  //next_backtrack_lit++;
        
	  curr_instr->SwapOn();

	  backtrack_instr.push(curr_instr);
	  //*next_backtrack_instr = curr_instr;
	  //next_backtrack_instr++;
        
	  cursor = curr_lit->Arg1();
        
	  curr_instr = curr_instr->Next();
        
	  goto check_tag;
	}
      else // no equations with this polarity (neither ordered nor unordered)
	goto try_fork; 
       
    case FS_COMMAND::PROP_LIT :
      COP("PROP_LIT");
      if (FirstLit(curr_instr->HeaderNum())) // the query contains such literal
	{
	  if (curr_instr->Fork()) 
	    {

	      backtrack_instr.push(curr_instr);
	      //*next_backtrack_instr = curr_instr;
	      //next_backtrack_instr++;
	    }; 
	  curr_instr = curr_instr->Next(); 
	  goto check_tag;
	}
      else // no such literal in the query, try fork
	goto try_fork; 
       
       
    case FS_COMMAND::SECOND_EQ_ARG :
      COP("SECOND_EQ_ARG");
     
      if (cursor->IsBackJump()) 
	{
	  cursor = cursor->after();
	}; 
      
      curr_instr = curr_instr->Next(); 
      goto check_tag; 
       
    case FS_COMMAND::FUNC :
      COP("FUNC");
      curr_sym = cursor->Symbol();
      if (curr_sym.isVariable()) goto backtrack;
      
    compare_func:
      if (curr_sym == curr_instr->Func()) 
        {
	  cursor++;
	  curr_instr = curr_instr->Next();
	  goto check_tag; 
        };
      if (curr_sym < curr_instr->Func()) goto backtrack;
      curr_instr = curr_instr->Fork();
      if (!curr_instr) goto backtrack;
       
      if (curr_instr->IsFunc()) 
	{
#ifdef FS_STAT
	  anotherInstr();
#endif
          goto compare_func;
	};
                
      goto check_tag;
         

    case FS_COMMAND::VAR :
      COP("VAR");
      *(curr_instr->Var()) = cursor; 
      
      
      if ((curr_instr->Fork()) && (cursor->Symbol().isNotVariable()))
	{
	  backtrack_instr.push(curr_instr);
	  //*next_backtrack_instr = curr_instr;
	  //next_backtrack_instr++;

	  backtrack_cursor.push(cursor);
	  //*next_backtrack_cursor = cursor;
	  //next_backtrack_cursor++;
	};
       
      cursor = cursor->after();
      curr_instr = curr_instr->Next();
      goto check_tag;

    case FS_COMMAND::COMPARE_NF :
      COP("COMPARE_NF");
#ifdef FS_STAT
      anotherCompInstr();
#endif
      if ((*(curr_instr->Var1()))->equal(*(curr_instr->Var2())))
	{
#ifdef FS_STAT
	  anotherCompSucc();
#endif
	  curr_instr = curr_instr->Next(); 
	  goto check_tag;     
	}
      else goto backtrack;
 
      
    case FS_COMMAND::COMPARE :
      COP("COMPARE"); 
      
#ifdef FS_STAT
      anotherCompInstr();
#endif
     
      if ((*(curr_instr->Var1()))->equal(*(curr_instr->Var2())))
	{
#ifdef FS_STAT
	  anotherCompSucc();
#endif

	  if (curr_instr->Fork()) 
	    {
	      backtrack_instr.push(curr_instr);
	      //*next_backtrack_instr = curr_instr;
	      //next_backtrack_instr++;
   
	      backtrack_cursor.push(cursor);
	      //*next_backtrack_cursor = cursor;
	      //next_backtrack_cursor++;
	    };
	  curr_instr = curr_instr->Next(); 
	  goto check_tag; 
	}
      else goto try_fork;

    case FS_COMMAND::SIG_FILTER :
      COP("SIG_FILTER");
      if (curr_instr->Functors()->subset(*(Signature())))
	{
	  curr_instr = curr_instr->Next();       
	  goto check_tag;
	};
      goto backtrack;

    case FS_COMMAND::SUCCESS : 
      COP("SUCCESS");
      subsuming_cl = curr_instr->TheClause();
      if (FS_QUERY::SubsumptionAllowedInSetMode(subsuming_cl)) return subsuming_cl;
      goto backtrack; 
      
#ifdef DEBUG_NAMESPACE  
    default: ICP("ICP1"); return 0; 
#else
#ifdef _SUPPRESS_WARNINGS_
    default: return 0; 
#endif
#endif  
    }; 
   
 try_fork:
  COP("try_fork"); 
  curr_instr = curr_instr->Fork();
  if (curr_instr) goto check_tag;
  
  // no fork, try to backtrack
 backtrack:   
  COP("backtrack"); 

  if (backtrack_instr.empty())
    //if (next_backtrack_instr == backtrack_instr) 
    {
      return 0;
    }; 
   
  curr_instr = backtrack_instr.pop();
  //next_backtrack_instr--;
  //curr_instr = *next_backtrack_instr;
   
  COP("backtrack switch (curr_instr->Tag())");

  switch (curr_instr->Tag())
    {
    case FS_COMMAND::LIT_HEADER :
      COP("backtrack LIT_HEADER");
      
      GlobalStopFlag::check();
      curr_lit = backtrack_lit.pop();
      //next_backtrack_lit--;
      //curr_lit = *next_backtrack_lit;

      if (!curr_lit) goto try_fork;
      next_lit = curr_lit->Next(); 
      if (next_lit || curr_instr->Fork())
	{
	  backtrack_lit.push(next_lit);
	  //*next_backtrack_lit = next_lit;
	  //next_backtrack_lit++;

	  backtrack_instr.push(curr_instr);
	  //*next_backtrack_instr = curr_instr;
	  //next_backtrack_instr++;
	}; 
      cursor = curr_lit->Args();
      curr_instr = curr_instr->Next();
      goto check_tag;
     
    case FS_COMMAND::ORD_EQ :
      COP("backtrack ORD_EQ");

      GlobalStopFlag::check();
      curr_lit = backtrack_lit.pop();
      //next_backtrack_lit--;
      //curr_lit = *next_backtrack_lit;

      if (!curr_lit) goto try_fork;
      
      next_lit = curr_lit->Next(); 
      if (next_lit || curr_instr->Fork())
	{

	  backtrack_lit.push(next_lit);
	  //*next_backtrack_lit = next_lit;
	  //next_backtrack_lit++;

	  backtrack_instr.push(curr_instr);
	  //*next_backtrack_instr = curr_instr;
	  //next_backtrack_instr++;
	}; 
      cursor = curr_lit->Arg1();
      curr_instr = curr_instr->Next();
      goto check_tag;
      
    case FS_COMMAND::UNORD_EQ :
      COP("backtrack UNORD_EQ");
 
      GlobalStopFlag::check();
      curr_lit = backtrack_lit.pop();
      //next_backtrack_lit--;
      //curr_lit = *next_backtrack_lit;
 
      if (!curr_lit) goto try_fork;
      
      if (curr_instr->SwapFlag()) 
	{        
	  next_lit = curr_lit->Next();
	  if (next_lit || curr_instr->Fork())
	    {

	      backtrack_lit.push(next_lit);
	      //*next_backtrack_lit = next_lit;
	      //next_backtrack_lit++;

	      curr_instr->SwapOff();

	      backtrack_instr.push(curr_instr);
	      //*next_backtrack_instr = curr_instr;
	      //next_backtrack_instr++;
	    };
         
	  cursor = curr_lit->Arg2();
	}
      else
	{       
	  curr_instr->SwapOn();

	  backtrack_lit.push(); // the literal is already there
	  //next_backtrack_lit++;

	  backtrack_instr.push(); // the instruction is already there
	  //next_backtrack_instr++;
        
	  cursor = curr_lit->Arg1();
	}; 
      curr_instr = curr_instr->Next();
      goto check_tag; 
       
    case FS_COMMAND::PROP_LIT :
      COP("backtrack PROP_LIT");
      curr_instr = curr_instr->Fork();
      goto check_tag; 
     
    case FS_COMMAND::VAR :
      COP("backtrack VAR");
      // same as for COMPARE
      
    case FS_COMMAND::COMPARE :
      COP("backtrack COMPARE");
      curr_instr = curr_instr->Fork();

      cursor = backtrack_cursor.pop();
      //next_backtrack_cursor--;
      //cursor = *next_backtrack_cursor;
      goto check_tag;
    
#ifdef DEBUG_NAMESPACE  
    default: ICP("ICP2"); return 0; 
#else
#ifdef _SUPPRESS_WARNINGS_
    default: return 0; 
#endif
#endif 
    };
}; // Clause* FS::SubsumeInSetMode()



Clause* ForwardSubsumption::SubsumeInMultisetMode()
{ 
  CALL("ForwardSubsumption::SubsumeInMultisetMode()");  

#ifdef FS_STAT
  anotherCall();
#endif 

  FS_COMMAND* curr_instr = Tree();
  if (!(curr_instr)) return 0; 
  
  EndOfQuery();
  
  const Flatterm* cursor = 0;
  
  TERM curr_sym;
  LIT* curr_lit;
  LIT* next_lit;

  // stacks
  backtrack_lit.reset();
  //LIT* backtrack_lit[VampireKernelConst::MaxNumOfLiterals];
  //LIT** next_backtrack_lit = backtrack_lit;

  backtrack_instr.reset();
  //FS_COMMAND* backtrack_instr[MAX_FS_CODE_DEPTH];
  //FS_COMMAND** next_backtrack_instr = backtrack_instr;
  
  backtrack_cursor.reset();
  //const Flatterm* backtrack_cursor[MAX_FS_CODE_DEPTH];
  //const Flatterm** next_backtrack_cursor = backtrack_cursor;
  
  Clause* subsuming_cl;
  
 check_tag:
#ifdef FS_STAT
  anotherInstr();
#endif 
  
  switch (curr_instr->Tag())
    {
    case FS_COMMAND::LIT_HEADER :
      curr_lit = FirstLit(curr_instr->HeaderNum());
    check_lit:
      if (curr_lit)
        {
	  if (curr_lit->Captured())
	    {
	      curr_lit = curr_lit->Next();  
	      goto check_lit;
	    }
	  else
	    { 
	      curr_lit->Capture();
             
	      backtrack_lit.push(curr_lit); // to be released on backtracking
	      //*next_backtrack_lit = curr_lit; // to be released on backtracking
	      //next_backtrack_lit++; 

	      backtrack_instr.push(curr_instr);
	      //*next_backtrack_instr = curr_instr;
	      //next_backtrack_instr++;

	      cursor = curr_lit->Args();
	      curr_instr = curr_instr->Next();
	      goto check_tag;
	    } ;
        }
      else // no literal with this header
        goto try_fork;
     
    case FS_COMMAND::ORD_EQ :
      curr_lit = FirstLit(curr_instr->HeaderNum());
    check_ord_eq_lit:
      if (curr_lit)
        { 
	  if (curr_lit->Captured())
	    {
	      curr_lit = curr_lit->Next();  
	      goto check_ord_eq_lit;
	    }
	  else
	    { 
	      curr_lit->Capture();

	      backtrack_lit.push(curr_lit); // to be released on backtracking
	      //*next_backtrack_lit = curr_lit; // to be released on backtracking
	      //next_backtrack_lit++; 

	      backtrack_instr.push(curr_instr);
	      //*next_backtrack_instr = curr_instr;
	      //next_backtrack_instr++;

	      cursor = curr_lit->Arg1();
	      curr_instr = curr_instr->Next();
	      goto check_tag;
	    };
        }
      else // no ordered equations with this polarity
        goto try_fork;
       
       
    case FS_COMMAND::UNORD_EQ : 
      curr_lit = FirstLit(curr_instr->HeaderNum());
    check_unord_eq_lit:
      if (curr_lit)
        {
	  if (curr_lit->Captured())
	    {
	      curr_lit = curr_lit->Next();  
	      goto check_unord_eq_lit;
	    }
	  else
	    {
	      curr_lit->Capture(); 
           
	      backtrack_lit.push(curr_lit);
	      //*next_backtrack_lit = curr_lit; 
	      //next_backtrack_lit++;
        
	      curr_instr->SwapOn();
        
	      backtrack_instr.push(curr_instr);
	      //*next_backtrack_instr = curr_instr;
	      //next_backtrack_instr++;
        
	      cursor = curr_lit->Arg1();
        
	      curr_instr = curr_instr->Next();
        
	      goto check_tag;
	    };
        }
      else // no equations with this polarity (neither ordered nor unordered)
        goto try_fork; 
       
    case FS_COMMAND::PROP_LIT :
      if (FirstLit(curr_instr->HeaderNum())) // the query contains such literal
	{
	  if (curr_instr->Fork()) 
	    {
           
	      backtrack_instr.push(curr_instr);
	      //*next_backtrack_instr = curr_instr;
	      //next_backtrack_instr++;
	    }; 
	  curr_instr = curr_instr->Next(); 
	  goto check_tag;
	}
      else // no such literal in the query, try fork
	goto try_fork; 
       
       
    case FS_COMMAND::SECOND_EQ_ARG :
     
      if (cursor->IsBackJump()) 
	{
	  cursor = cursor->after();
	}; 
      
      curr_instr = curr_instr->Next(); 
      goto check_tag; 
       
    case FS_COMMAND::FUNC :
      curr_sym = cursor->Symbol();
      if (curr_sym.isVariable()) goto backtrack;
      
    compare_func:
      if (curr_sym == curr_instr->Func()) 
        {
	  cursor++;
	  curr_instr = curr_instr->Next();
	  goto check_tag; 
        };
      if (curr_sym < curr_instr->Func()) goto backtrack;
      curr_instr = curr_instr->Fork();
      if (!curr_instr) goto backtrack;
       
      if (curr_instr->IsFunc()) 
	{
#ifdef FS_STAT
	  anotherInstr();
#endif
          goto compare_func;
	};
                
      goto check_tag;
         

    case FS_COMMAND::VAR :
      *(curr_instr->Var()) = cursor; 
      
      
      if ((curr_instr->Fork()) && (cursor->Symbol().isNotVariable()))
	{

	  backtrack_instr.push(curr_instr);
	  //*next_backtrack_instr = curr_instr;
	  //next_backtrack_instr++;

	  backtrack_cursor.push(cursor);
	  //*next_backtrack_cursor = cursor;
	  //next_backtrack_cursor++;
	};
       
      cursor = cursor->after();
      curr_instr = curr_instr->Next();
      goto check_tag;

    case FS_COMMAND::COMPARE_NF :
#ifdef FS_STAT
      anotherCompInstr();
#endif
      if ((*(curr_instr->Var1()))->equal(*(curr_instr->Var2())))
	{
#ifdef FS_STAT
	  anotherCompSucc();
#endif
	  curr_instr = curr_instr->Next(); 
	  goto check_tag;     
	}
      else goto backtrack;
 
      
    case FS_COMMAND::COMPARE : 
      
#ifdef FS_STAT
      anotherCompInstr();
#endif
     
      if ((*(curr_instr->Var1()))->equal(*(curr_instr->Var2())))
	{
#ifdef FS_STAT
	  anotherCompSucc();
#endif

	  if (curr_instr->Fork()) 
	    {

	      backtrack_instr.push(curr_instr);
	      //*next_backtrack_instr = curr_instr;
	      //next_backtrack_instr++;

	      backtrack_cursor.push(cursor);
	      //*next_backtrack_cursor = cursor;
	      //next_backtrack_cursor++;
	    };
	  curr_instr = curr_instr->Next(); 
	  goto check_tag; 
	}
      else goto try_fork;

    case FS_COMMAND::SIG_FILTER :
      if (curr_instr->Functors()->subset(*(Signature())))
	{
	  curr_instr = curr_instr->Next();       
	  goto check_tag;
	};
      goto backtrack;

    case FS_COMMAND::SUCCESS : 
      subsuming_cl = curr_instr->TheClause();
      if (FS_QUERY::SubsumptionAllowedInMultisetMode(subsuming_cl)) return subsuming_cl;
      goto backtrack; 
      
#ifdef DEBUG_NAMESPACE  
    default: ICP("ICP1"); return 0; 
#else
#ifdef _SUPPRESS_WARNINGS_
    default: return 0; 
#endif
#endif  
    }; 
   
 try_fork:
  curr_instr = curr_instr->Fork();
  if (curr_instr) goto check_tag;
  
  // no fork, try to backtrack
 backtrack: 
  if (backtrack_instr.empty())
    //if (next_backtrack_instr == backtrack_instr) 
    {
      return 0;
    }; 
   
  curr_instr = backtrack_instr.pop();
  //next_backtrack_instr--;
  //curr_instr = *next_backtrack_instr;
   
  switch (curr_instr->Tag())
    {
    case FS_COMMAND::LIT_HEADER :
 
      GlobalStopFlag::check();
      curr_lit = backtrack_lit.pop();
      //next_backtrack_lit--;
      //curr_lit = *next_backtrack_lit;
 
      ASSERT(curr_lit); 
      ASSERT(curr_lit->Captured());
      curr_lit->Release();
      curr_lit = curr_lit->Next();       
      goto check_lit;
     
    case FS_COMMAND::ORD_EQ :
 
      GlobalStopFlag::check();
      curr_lit = backtrack_lit.pop();
      //next_backtrack_lit--;
      //curr_lit = *next_backtrack_lit;

      ASSERT(curr_lit); 
      ASSERT(curr_lit->Captured());
      curr_lit->Release();
      curr_lit = curr_lit->Next();       
      goto check_ord_eq_lit;      

    case FS_COMMAND::UNORD_EQ :
 
      GlobalStopFlag::check();
      curr_lit = backtrack_lit.pop();
      //next_backtrack_lit--;
      //curr_lit = *next_backtrack_lit;

      ASSERT(curr_lit);
      ASSERT(curr_lit->Captured());
      if (curr_instr->SwapFlag()) 
	{        
	  // curr_lit remains captured, so there is no curr_lit->Release() here 
	  next_lit = curr_lit->Next();

	  backtrack_lit.push(curr_lit);
	  //*next_backtrack_lit = curr_lit; 
	  //next_backtrack_lit++;

	  curr_instr->SwapOff();

	  backtrack_instr.push(curr_instr);
	  //*next_backtrack_instr = curr_instr;
	  //next_backtrack_instr++;
         
	  cursor = curr_lit->Arg2();
	  curr_instr = curr_instr->Next();
	  goto check_tag;
	}
      else
	{       
	  curr_lit->Release();        
	  curr_lit = curr_lit->Next(); 
	  goto check_unord_eq_lit;
	}; 
       
    case FS_COMMAND::PROP_LIT :
      curr_instr = curr_instr->Fork();
      goto check_tag; 
     
    case FS_COMMAND::VAR :
      // same as for COMPARE
      
    case FS_COMMAND::COMPARE :
      curr_instr = curr_instr->Fork();

      cursor = backtrack_cursor.pop();
      //next_backtrack_cursor--;
      //cursor = *next_backtrack_cursor;

      goto check_tag;
    
#ifdef DEBUG_NAMESPACE  
    default: ICP("ICP2"); return 0; 
#else
#ifdef _SUPPRESS_WARNINGS_
    default: return 0; 
#endif
#endif 
    };
}; // Clause* ForwardSubsumptionS::SubsumeInMultisetMode()




Clause* ForwardSubsumption::SubsumeInOptimisedMultisetMode()
{ 
  CALL("ForwardSubsumption::SubsumeInOptimisedMultisetMode()");  

#ifdef FS_STAT
  anotherCall();
#endif 

  FS_COMMAND* curr_instr = Tree();
  if (!(curr_instr)) return 0; 
  
  EndOfQuery();
  
  const Flatterm* cursor = 0;
  
  TERM curr_sym;
  LIT* curr_lit;
  LIT* next_lit;
  // stacks
  backtrack_lit.reset();
  //LIT* backtrack_lit[VampireKernelConst::MaxNumOfLiterals];
  //LIT** next_backtrack_lit = backtrack_lit;

  backtrack_instr.reset();
  //FS_COMMAND* backtrack_instr[MAX_FS_CODE_DEPTH];
  //FS_COMMAND** next_backtrack_instr = backtrack_instr;
  
  backtrack_cursor.reset();
  //const Flatterm* backtrack_cursor[MAX_FS_CODE_DEPTH];
  //const Flatterm** next_backtrack_cursor = backtrack_cursor;
  
  Clause* subsuming_cl;
  
  long numOfFactorings = 0L;

 check_tag:
#ifdef FS_STAT
  anotherInstr();
#endif 
  
  switch (curr_instr->Tag())
    {
    case FS_COMMAND::LIT_HEADER :
      curr_lit = FirstLit(curr_instr->HeaderNum());
    check_lit:
      if (curr_lit)
        {
	  ASSERT(numOfFactorings >= 0);
	  ASSERT(curr_lit->Captured() >= 0);
	  numOfFactorings += curr_lit->Captured();

	  curr_lit->Capture();
             
	  backtrack_lit.push(curr_lit); // to be released on backtracking
	  //*next_backtrack_lit = curr_lit; // to be released on backtracking
	  //next_backtrack_lit++; 

	  backtrack_instr.push(curr_instr);
	  //*next_backtrack_instr = curr_instr;
	  //next_backtrack_instr++;

	  cursor = curr_lit->Args();
	  curr_instr = curr_instr->Next();
	  goto check_tag;

        }
      else // no literal with this header
        goto try_fork;
     
    case FS_COMMAND::ORD_EQ :
      curr_lit = FirstLit(curr_instr->HeaderNum());
    check_ord_eq_lit:
      if (curr_lit)
        { 
	  ASSERT(numOfFactorings >= 0);
	  ASSERT(curr_lit->Captured() >= 0);
	  //if (curr_lit->Captured() && (TERM::HeaderPolarity(curr_instr->HeaderNum()) == TERM::PositivePolarity))
	  if (curr_lit->Captured() && (TERM::HeaderPositive(curr_instr->HeaderNum())))
	    {
	      curr_lit = curr_lit->Next();  
	      goto check_ord_eq_lit;
	    }
	  else
	    { 
	      numOfFactorings += curr_lit->Captured();
        
	      curr_lit->Capture();

	      backtrack_lit.push(curr_lit); // to be released on backtracking
	      //*next_backtrack_lit = curr_lit; // to be released on backtracking
	      //next_backtrack_lit++; 

	      backtrack_instr.push(curr_instr);
	      //*next_backtrack_instr = curr_instr;
	      //next_backtrack_instr++;

	      cursor = curr_lit->Arg1();
	      curr_instr = curr_instr->Next();
	      goto check_tag;
	    };
        }
      else // no ordered equations with this polarity
        goto try_fork;
       
       
    case FS_COMMAND::UNORD_EQ : 
      curr_lit = FirstLit(curr_instr->HeaderNum());
    check_unord_eq_lit:
      if (curr_lit)
        {
	  ASSERT(numOfFactorings >= 0);
	  ASSERT(curr_lit->Captured() >= 0);
	  //if (curr_lit->Captured() && (TERM::HeaderPolarity(curr_instr->HeaderNum()) == TERM::PositivePolarity))
	  if (curr_lit->Captured() && (TERM::HeaderPositive(curr_instr->HeaderNum())))
	    {
	      curr_lit = curr_lit->Next();  
	      goto check_unord_eq_lit;
	    }
	  else
	    { 
	      numOfFactorings += curr_lit->Captured();
	      curr_lit->Capture(); 
           
	      backtrack_lit.push(curr_lit);
	      //*next_backtrack_lit = curr_lit; 
	      //next_backtrack_lit++;
        
	      curr_instr->SwapOn();
        
	      backtrack_instr.push(curr_instr);
	      //*next_backtrack_instr = curr_instr;
	      //next_backtrack_instr++;
        
	      cursor = curr_lit->Arg1();
        
	      curr_instr = curr_instr->Next();
        
	      goto check_tag;
	    };
        }
      else // no equations with this polarity (neither ordered nor unordered)
        goto try_fork; 
       
    case FS_COMMAND::PROP_LIT :
      if (FirstLit(curr_instr->HeaderNum())) // the query contains such literal
	{
	  if (curr_instr->Fork()) 
	    {
	      backtrack_instr.push(curr_instr);
	      //*next_backtrack_instr = curr_instr;
	      //next_backtrack_instr++;
	    }; 
	  curr_instr = curr_instr->Next(); 
	  goto check_tag;
	}
      else // no such literal in the query, try fork
	goto try_fork; 
       
       
    case FS_COMMAND::SECOND_EQ_ARG :
     
      if (cursor->IsBackJump()) 
	{
	  cursor = cursor->after();
	}; 
      
      curr_instr = curr_instr->Next(); 
      goto check_tag; 
       
    case FS_COMMAND::FUNC :
      curr_sym = cursor->Symbol();
      if (curr_sym.isVariable()) goto backtrack;
      
    compare_func:
      if (curr_sym == curr_instr->Func()) 
        {
	  cursor++;
	  curr_instr = curr_instr->Next();
	  goto check_tag; 
        };
      if (curr_sym < curr_instr->Func()) goto backtrack;
      curr_instr = curr_instr->Fork();
      if (!curr_instr) goto backtrack;
       
      if (curr_instr->IsFunc()) 
	{
#ifdef FS_STAT
	  anotherInstr();
#endif
          goto compare_func;
	};
                
      goto check_tag;
         

    case FS_COMMAND::VAR :
      *(curr_instr->Var()) = cursor; 
      
      
      if ((curr_instr->Fork()) && (cursor->Symbol().isNotVariable()))
	{
	  backtrack_instr.push(curr_instr);
	  //*next_backtrack_instr = curr_instr;
	  //next_backtrack_instr++;

	  backtrack_cursor.push(cursor);
	  //*next_backtrack_cursor = cursor;
	  //next_backtrack_cursor++;
	};
       
      cursor = cursor->after();
      curr_instr = curr_instr->Next();
      goto check_tag;

    case FS_COMMAND::COMPARE_NF :
#ifdef FS_STAT
      anotherCompInstr();
#endif
      if ((*(curr_instr->Var1()))->equal(*(curr_instr->Var2())))
	{
#ifdef FS_STAT
	  anotherCompSucc();
#endif
	  curr_instr = curr_instr->Next(); 
	  goto check_tag;     
	}
      else goto backtrack;
 
      
    case FS_COMMAND::COMPARE : 
      
#ifdef FS_STAT
      anotherCompInstr();
#endif
     
      if ((*(curr_instr->Var1()))->equal(*(curr_instr->Var2())))
	{
#ifdef FS_STAT
	  anotherCompSucc();
#endif

	  if (curr_instr->Fork()) 
	    {
 
	      backtrack_instr.push(curr_instr);
	      //*next_backtrack_instr = curr_instr;
	      //next_backtrack_instr++;
 
	      backtrack_cursor.push(cursor);
	      //*next_backtrack_cursor = cursor;
	      //next_backtrack_cursor++;
	    };
	  curr_instr = curr_instr->Next(); 
	  goto check_tag; 
	}
      else goto try_fork;

    case FS_COMMAND::SIG_FILTER :
      if (curr_instr->Functors()->subset(*(Signature())))
	{
	  curr_instr = curr_instr->Next();       
	  goto check_tag;
	};
      goto backtrack;

    case FS_COMMAND::SUCCESS :
      ASSERT(numOfFactorings >= 0);
      subsuming_cl = curr_instr->TheClause();
      if (FS_QUERY::SubsumptionAllowedInOptimisedMultisetMode(subsuming_cl,numOfFactorings)) return subsuming_cl;
      goto backtrack; 
      
#ifdef DEBUG_NAMESPACE  
    default: ICP("ICP1"); return 0; 
#else
#ifdef _SUPPRESS_WARNINGS_
    default: return 0; 
#endif
#endif  
    }; 
   
 try_fork:
  curr_instr = curr_instr->Fork();
  if (curr_instr) goto check_tag;
  
  // no fork, try to backtrack
 backtrack: 
  if (backtrack_instr.empty())
    //if (next_backtrack_instr == backtrack_instr) 
    {
      return 0;
    }; 
   
  curr_instr = backtrack_instr.pop();
  //next_backtrack_instr--;
  //curr_instr = *next_backtrack_instr;
   
  switch (curr_instr->Tag())
    {
    case FS_COMMAND::LIT_HEADER :
 
      GlobalStopFlag::check();
      curr_lit = backtrack_lit.pop();
      //next_backtrack_lit--;
      //curr_lit = *next_backtrack_lit;


      ASSERT(curr_lit); 
      ASSERT(curr_lit->Captured());
      curr_lit->Release();
      numOfFactorings -= curr_lit->Captured();
      ASSERT(numOfFactorings >= 0);
      ASSERT(curr_lit->Captured() >= 0);
      curr_lit = curr_lit->Next();       
      goto check_lit;
     
    case FS_COMMAND::ORD_EQ :
 
      GlobalStopFlag::check();
      curr_lit = backtrack_lit.pop();
      //next_backtrack_lit--;
      //curr_lit = *next_backtrack_lit;

      ASSERT(curr_lit); 
      ASSERT(curr_lit->Captured());
      curr_lit->Release();
      numOfFactorings -= curr_lit->Captured();
      ASSERT(numOfFactorings >= 0);
      ASSERT(curr_lit->Captured() >= 0);
      curr_lit = curr_lit->Next();       
      goto check_ord_eq_lit;      

    case FS_COMMAND::UNORD_EQ :
      
              
      GlobalStopFlag::check();
      curr_lit = backtrack_lit.pop();
      //next_backtrack_lit--;
      //curr_lit = *next_backtrack_lit;

      ASSERT(curr_lit);
      ASSERT(curr_lit->Captured());
      if (curr_instr->SwapFlag()) 
	{        
	  // curr_lit remains captured, so there is no curr_lit->Release() here 
	  next_lit = curr_lit->Next();

	  backtrack_lit.push(curr_lit);  
	  //*next_backtrack_lit = curr_lit; 
	  //next_backtrack_lit++;

	  curr_instr->SwapOff();

	  backtrack_instr.push(curr_instr);
	  //*next_backtrack_instr = curr_instr;
	  //next_backtrack_instr++;
         
	  cursor = curr_lit->Arg2();
	  curr_instr = curr_instr->Next();
	  goto check_tag;
	}
      else
	{       
	  curr_lit->Release();
	  numOfFactorings -= curr_lit->Captured(); 
	  ASSERT(numOfFactorings >= 0);
	  ASSERT(curr_lit->Captured() >= 0);     
	  curr_lit = curr_lit->Next(); 
	  goto check_unord_eq_lit;
	}; 
       
    case FS_COMMAND::PROP_LIT :
      curr_instr = curr_instr->Fork();
      goto check_tag; 
     
    case FS_COMMAND::VAR :
      // same as for COMPARE
      
    case FS_COMMAND::COMPARE :
      curr_instr = curr_instr->Fork();
      
      cursor = backtrack_cursor.pop();
      //next_backtrack_cursor--;
      //cursor = *next_backtrack_cursor;

      goto check_tag;
    
#ifdef DEBUG_NAMESPACE  
    default: ICP("ICP2"); return 0; 
#else
#ifdef _SUPPRESS_WARNINGS_
    default: return 0; 
#endif
#endif 
    };
}; // Clause* ForwardSubsumption::SubsumeInOptimisedMultisetMode()


//================================================
#undef DEBUG_NAMESPACE
#ifdef DEBUG_FORWARD_SUBSUMPTION
#define DEBUG_NAMESPACE "FS_QUERY"
#endif 
#include "debugMacros.hpp"
//=================================================


FS_QUERY::FS_QUERY() 
  : 

#ifdef NO_DEBUG 
  _litsWithHeader(static_cast<LIT*>(0),0),
#else
  _litsWithHeader(static_cast<LIT*>(0),"CN_QUERY::_litsWithHeader"),
#endif

  _posOrdEqHeaderNum(TERM::positiveOrdEqHeader().HeaderNum()),
  _negOrdEqHeaderNum(TERM::negativeOrdEqHeader().HeaderNum()),

  _presentHeaders(DOP("FS_QUERY::_presentHeaders")), 
  collect_signature(true) // default
{
  CALL("constructor FS_QUERY()");
  DOP(_litsWithHeader.freeze());
  signature = 0; 
}; // FS_QUERY::FS_QUERY()

FS_QUERY::~FS_QUERY() { CALL("destructor ~FS_QUERY()"); };

void FS_QUERY::init()
{
  CALL("init()");
  _litPool.init();
#ifdef NO_DEBUG 
  _litsWithHeader.init(static_cast<LIT*>(0),0);
#else
  _litsWithHeader.init(static_cast<LIT*>(0),"CN_QUERY::_litsWithHeader");
#endif

  _posOrdEqHeaderNum = TERM::positiveOrdEqHeader().HeaderNum();
  _negOrdEqHeaderNum = TERM::negativeOrdEqHeader().HeaderNum();

  _presentHeaders.init(DOP("FS_QUERY::_presentHeaders"));
  collect_signature = true; // default
  DOP(_litsWithHeader.freeze());
  signature = 0; 
}; // FS_QUERY::init()


void FS_QUERY::destroy()
{
  CALL("destroy()");
  _presentHeaders.destroy();
  _litsWithHeader.destroy();
  _litPool.destroy();
}; // void FS_QUERY::destroy()


#ifndef NO_DEBUG_VIS
ostream& FS_QUERY::output(ostream& str) const
{
  for (ulong p = 0; p < _presentHeaders.size(); p++)
    for (const LIT* lit = _litsWithHeader[_presentHeaders.nth(p)]; lit; lit = lit->Next())
      lit->output(str) << '\n';

  return str;
}; // ostream& FS_QUERY::output(ostream& str) const
#endif

//=======================================================================
