#include "GlobalStopFlag.hpp"
#include "iu_machine.hpp"
#include "unification.hpp"
using namespace BK;
using namespace VK;

//================================================
#undef DEBUG_NAMESPACE
#ifdef DEBUG_FORWARD_SUPERPOSITION
#define DEBUG_NAMESPACE "IU_MACH"
#endif
#include "debugMacros.hpp"
//=================================================


IU_MACH::IU_MACH() :
  _codeMemory(DOP("IU_MACH::_codeMemory")),
  _dataMemory(DOP("IU_MACH::_dataMemory")),
  _backtrackPoints(DOP("IU_MACH::_backtrackPoints")),
  _backtrackActions(DOP("IU_MACH::_backtrackActions"))
{ 
  DOP(_codeMemory.freeze());
  DOP(_dataMemory.freeze());
  DOP(_backtrackPoints.freeze());
  DOP(_backtrackActions.freeze());
  //ResetStatic();
};

IU_MACH::~IU_MACH() { CALL("destructor ~IU_MACH()"); };

void IU_MACH::init() 
{
  CALL("init()");
  compiler.init();
  proc.init();

  _codeMemory.init(DOP("IU_MACH::_codeMemory"));
  _dataMemory.init(DOP("IU_MACH::_dataMemory"));
  _backtrackPoints.init(DOP("IU_MACH::_backtrackPoints"));
  _backtrackActions.init(DOP("IU_MACH::_backtrackActions"));

  DOP(_codeMemory.freeze());
  DOP(_dataMemory.freeze());
  DOP(_backtrackPoints.freeze());
  DOP(_backtrackActions.freeze());
}; // void IU_MACH::init() 

void IU_MACH::destroy()
{
  CALL("destroy()");
  _backtrackActions.destroy();
  _backtrackPoints.destroy();
  _dataMemory.destroy();
  _codeMemory.destroy();
  proc.destroy();
  compiler.destroy();
}; // void IU_MACH::destroy()


inline bool IU_MACH::CompleteSearch()
{
  CALL("CompleteSearch()"); 
 
  IU_COMMAND* com = command;
  bool same_var;
  bool free_var;
  void* bind;
  
  
 check_tag:
  
  ASSERT((com > Code()) && (com < end_of_code));
  ASSERT((com->Tag() == IU_COMMAND::END) || (!(proc.CurrNode()->IsLeaf())));
  
   
  switch (com->Tag())
    {
    case IU_COMMAND::START : 
      ICP("R0");
      return false;
    case IU_COMMAND::VAR :
      bind = com->Symbol().var()->Unref(free_var);
      if (free_var)
	{
	  if (proc.BindWithOccCheck(static_cast<Unifier::Variable*>(bind),same_var))
	    {
              _backtrackPoints.push(com);

              if (same_var) 
               { 
		_backtrackActions.push(ALTER_BINDING_WITH_OC_SAME_VAR); 
               }
	      else
		_backtrackActions.push(ALTER_BINDING_WITH_OC); 

	      //PushBacktrackPoint(com);
              //if (same_var) { PushBacktrackAction(ALTER_BINDING_WITH_OC_SAME_VAR); }
              //else { PushBacktrackAction(ALTER_BINDING_WITH_OC); };

	      com++;
          
	      goto check_tag;  
	    }
	  else { goto backtrack; };
	}
      else
	{
       
	  ASSERT(Prefix::CorrectComplexTerm(static_cast<PrefixSym*>(bind)));
       
	  if (proc.ExtractUnifiable(static_cast<PrefixSym*>(bind)))
	    {
              _backtrackPoints.push(com);
              _backtrackActions.push(ALTER_UNIFIABLE);

	      //PushBacktrackPoint(com);
	      //PushBacktrackAction(ALTER_UNIFIABLE);

	      com++;
          
	      goto check_tag;
	    }
	  else 
	    {
	      goto backtrack; 
	    };
	};
      
    case IU_COMMAND::VAR_FIRST_OCC :
      proc.Bind(com->Symbol().var());

      _backtrackPoints.push(com);
      _backtrackActions.push(ALTER_BINDING);

      //PushBacktrackPoint(com);
      //PushBacktrackAction(ALTER_BINDING);

      com++;
      
      goto check_tag;  
   
    case IU_COMMAND::FUNC :
    func_check_node:
    switch (proc.CurrNode()->Tag())
      {
      case OR_INDEX_TREE_NODE::FUNC : 
	if (com->Symbol() == proc.CurrNode()->Symbol())
	  {

            _backtrackPoints.push(com);
            _backtrackActions.push(TRY_ALTERNATIVE_NODE);

            //PushBacktrackPoint(com);
            //PushBacktrackAction(TRY_ALTERNATIVE_NODE);

            com++;
            proc.Below();
            
            goto check_tag;
	  }
	else
	  {
	  func_try_alternative:
	    proc.Alternative();
	    if (proc.CurrNode()) { goto func_check_node;}
	    else { goto backtrack; };
	  };
      case OR_INDEX_TREE_NODE::CONST : goto func_try_alternative;
      case OR_INDEX_TREE_NODE::VAR :
	bind = (proc.Shift(proc.CurrNode()->Symbol().var()))->Unref(free_var);
	if (free_var)
	  {
            if (OccurCheck::Occurs(static_cast<Unifier::Variable*>(bind),com->Term()))
	      {
		goto func_try_alternative;
	      }
            else // assignment possible
	      {
		Unifier::current()->BindAlone(static_cast<Unifier::Variable*>(bind),com->Term());

                _backtrackPoints.push(com);
                _backtrackActions.push(UNBIND_LAST_TRY_ALTERNATIVE_NODE);

		//PushBacktrackPoint(com);
		//PushBacktrackAction(UNBIND_LAST_TRY_ALTERNATIVE_NODE);

		com = com->NextCommand();
		proc.Below();
              
		goto check_tag;
	      };
	  }
	else // complex term, unification needed
	  {
            if (UNIFICATION::UnifyComplex(static_cast<PrefixSym*>(bind),com->Term()))
	      {

                _backtrackPoints.push(com);
                _backtrackActions.push(UNBIND_SECTION_TRY_ALTERNATIVE_NODE);

		//PushBacktrackPoint(com);
		//PushBacktrackAction(UNBIND_SECTION_TRY_ALTERNATIVE_NODE);


		com = com->NextCommand();
		proc.Below();
              
		goto check_tag;
	      }
            else { goto func_try_alternative; };
	  };
      case OR_INDEX_TREE_NODE::VAR_FIRST_OCC :
	Unifier::current()->BindAlone(proc.Shift(proc.CurrNode()->Symbol().var()),com->Term());

        _backtrackPoints.push(com);
        _backtrackActions.push(UNBIND_LAST_TRY_ALTERNATIVE_NODE);

	//PushBacktrackPoint(com);
	//PushBacktrackAction(UNBIND_LAST_TRY_ALTERNATIVE_NODE);


	com = com->NextCommand();
	proc.Below();
          
	goto check_tag;
      case OR_INDEX_TREE_NODE::LEAF :
	ICP("R1");
	return false;
          
#ifdef DEBUG_NAMESPACE
      default : 
	ICP("A1");
	return false;    
#endif
      };
     
    case IU_COMMAND::FUNC_PLAIN :
    func_plain_check_node:
    switch (proc.CurrNode()->Tag())
      {
      case OR_INDEX_TREE_NODE::FUNC : 
	if (com->Symbol() == proc.CurrNode()->Symbol())
	  {

            _backtrackPoints.push(com);
            _backtrackActions.push(TRY_ALTERNATIVE_NODE);

            //PushBacktrackPoint(com);
            //PushBacktrackAction(TRY_ALTERNATIVE_NODE);


            com++;
            proc.Below();
            
            goto check_tag;
	  }
	else
	  {
	  func_plain_try_alternative:
	    proc.Alternative();
	    if (proc.CurrNode()) { goto func_plain_check_node;}
	    else { goto backtrack; };
	  };
      case OR_INDEX_TREE_NODE::CONST : goto func_plain_try_alternative;
      case OR_INDEX_TREE_NODE::VAR :
	bind = (proc.Shift(proc.CurrNode()->Symbol().var()))->Unref(free_var);
	if (free_var)
	  {
            Unifier::current()->BindAlone(static_cast<Unifier::Variable*>(bind),com->Term());

            _backtrackPoints.push(com);
            _backtrackActions.push(UNBIND_LAST_TRY_ALTERNATIVE_NODE);

            //PushBacktrackPoint(com);
            //PushBacktrackAction(UNBIND_LAST_TRY_ALTERNATIVE_NODE);

            com = com->NextCommand();
            proc.Below();
            
            goto check_tag;
	  }
	else // complex term
	  {
            if (UNIFICATION::UnifyPlain(static_cast<PrefixSym*>(bind),com->Term()))
	      {

                _backtrackPoints.push(com);
                _backtrackActions.push(UNBIND_SECTION_TRY_ALTERNATIVE_NODE);

		//PushBacktrackPoint(com);
		//PushBacktrackAction(UNBIND_SECTION_TRY_ALTERNATIVE_NODE);

		com = com->NextCommand();
		proc.Below();
		goto check_tag;
	      }
            else { goto func_plain_try_alternative; }; 
	  };
      case OR_INDEX_TREE_NODE::VAR_FIRST_OCC : 
	Unifier::current()->BindAlone(proc.Shift(proc.CurrNode()->Symbol().var()),com->Term());

        _backtrackPoints.push(com);
        _backtrackActions.push(UNBIND_LAST_TRY_ALTERNATIVE_NODE);

	//PushBacktrackPoint(com);
	//PushBacktrackAction(UNBIND_LAST_TRY_ALTERNATIVE_NODE);

	com = com->NextCommand();
	proc.Below();
          
	goto check_tag;
      case OR_INDEX_TREE_NODE::LEAF :
	ICP("S1");
	return false;
#ifdef DEBUG_NAMESPACE
      default : 
	ICP("L1");
	return false;    
#endif
      }; 
      
    /************/
      
    case IU_COMMAND::FUNC_GROUND :
    func_ground_check_node:
    switch (proc.CurrNode()->Tag())
      {
      case OR_INDEX_TREE_NODE::FUNC : 
	if (com->Symbol() == proc.CurrNode()->Symbol())
	  {

            _backtrackPoints.push(com);
            _backtrackActions.push(TRY_ALTERNATIVE_NODE);

            //PushBacktrackPoint(com);
            //PushBacktrackAction(TRY_ALTERNATIVE_NODE);

            com++;
            proc.Below();
            goto check_tag;
	  }
	else
	  {
	  func_ground_try_alternative:
	    proc.Alternative();
	    if (proc.CurrNode()) { goto func_ground_check_node;}
	    else { goto backtrack; };
	  };
      case OR_INDEX_TREE_NODE::CONST : goto func_ground_try_alternative;
      case OR_INDEX_TREE_NODE::VAR :
	bind = (proc.Shift(proc.CurrNode()->Symbol().var()))->Unref(free_var);
	if (free_var)
	  {
            Unifier::current()->BindAlone(static_cast<Unifier::Variable*>(bind),com->Term());

            _backtrackPoints.push(com);
            _backtrackActions.push(UNBIND_LAST_TRY_ALTERNATIVE_NODE);

            //PushBacktrackPoint(com);
            //PushBacktrackAction(UNBIND_LAST_TRY_ALTERNATIVE_NODE);


            com = com->NextCommand();
            proc.Below();
            goto check_tag;
	  }
	else // complex term
	  {
            if (UNIFICATION::MatchGroundPlain(static_cast<PrefixSym*>(bind),com->Term()))
	      {

                _backtrackPoints.push(com);
                _backtrackActions.push(UNBIND_SECTION_TRY_ALTERNATIVE_NODE);

		//PushBacktrackPoint(com);
		//PushBacktrackAction(UNBIND_SECTION_TRY_ALTERNATIVE_NODE);

		com = com->NextCommand();
		proc.Below();
		goto check_tag;
	      }
            else { goto func_ground_try_alternative; }; 
	  };
      case OR_INDEX_TREE_NODE::VAR_FIRST_OCC : 
	Unifier::current()->BindAlone(proc.Shift(proc.CurrNode()->Symbol().var()),com->Term());

        _backtrackPoints.push(com);
        _backtrackActions.push(UNBIND_LAST_TRY_ALTERNATIVE_NODE);

	//PushBacktrackPoint(com);
	//PushBacktrackAction(UNBIND_LAST_TRY_ALTERNATIVE_NODE);


	com = com->NextCommand();
	proc.Below();
	goto check_tag;
      case OR_INDEX_TREE_NODE::LEAF :
	ICP("R2");
	return false;
#ifdef DEBUG_NAMESPACE
      default : 
	ICP("A2");
	return false; 
#endif
      };
     
    case IU_COMMAND::CONST :
    const_check_node:
    switch (proc.CurrNode()->Tag())
      {
      case OR_INDEX_TREE_NODE::FUNC :
      const_try_alternative:
      proc.Alternative();
      if (proc.CurrNode()) { goto const_check_node;}
      else { goto backtrack; };
      case OR_INDEX_TREE_NODE::CONST : 
	if (com->Symbol() == proc.CurrNode()->Symbol())
	  {

            _backtrackPoints.push(com);
            _backtrackActions.push(TRY_ALTERNATIVE_NODE);

            //PushBacktrackPoint(com);
            //PushBacktrackAction(TRY_ALTERNATIVE_NODE);

            com++;
            proc.Below();
            goto check_tag;
	  }
	else { goto const_try_alternative; };
      case OR_INDEX_TREE_NODE::VAR : 
	bind = (proc.Shift(proc.CurrNode()->Symbol().var()))->Unref(free_var);
	if (free_var)
	  {
            Unifier::current()->BindAlone(static_cast<Unifier::Variable*>(bind),com->Term());


            _backtrackPoints.push(com);
            _backtrackActions.push(UNBIND_LAST_TRY_ALTERNATIVE_NODE);

            //PushBacktrackPoint(com);
            //PushBacktrackAction(UNBIND_LAST_TRY_ALTERNATIVE_NODE);

            com++;
            proc.Below();
            goto check_tag;
	  }
	else // complex term
	  {
            if (com->Symbol() == (static_cast<PrefixSym*>(bind))->Head())
	      {


                _backtrackPoints.push(com);
                _backtrackActions.push(TRY_ALTERNATIVE_NODE);

		//PushBacktrackPoint(com);
		//PushBacktrackAction(TRY_ALTERNATIVE_NODE);


		com++;
		proc.Below();
		goto check_tag;
	      }
            else { goto const_try_alternative; };
	  };
      case OR_INDEX_TREE_NODE::VAR_FIRST_OCC : 
	Unifier::current()->BindAlone(proc.Shift(proc.CurrNode()->Symbol().var()),com->Term());

        _backtrackPoints.push(com);
        _backtrackActions.push(UNBIND_LAST_TRY_ALTERNATIVE_NODE);

	//PushBacktrackPoint(com);
	//PushBacktrackAction(UNBIND_LAST_TRY_ALTERNATIVE_NODE);


	com++;
	proc.Below();
	goto check_tag;
      case OR_INDEX_TREE_NODE::LEAF :
	ICP("R3"); 
	return false;
#ifdef DEBUG_NAMESPACE
      default : 
	ICP("A3"); 
	return false;
#endif
      };
    case IU_COMMAND::END :
      ASSERT(proc.CurrNode()->IsLeaf());      
      return true;
#ifdef DEBUG_NAMESPACE
    default :
      ICP("A3"); 
      return false; 
#endif  
    };
 backtrack:
  command = com;
  if (Backtrack()) { com = command; goto check_tag; }
  else { return false; };
}; // bool IU_MACH::CompleteSearch() 


bool IU_MACH::FindFirstLeaf(OR_INDEX_TREE_NODE* tree)
{
  command = Code() + 1;
  if (tree)
    {
      _backtrackPoints.push(Code());
      _backtrackActions.push(FAILURE);
      //PushBacktrackPoint(Code());
      //PushBacktrackAction(FAILURE);

      proc.Start(tree);
      return CompleteSearch();
    }
  else 
    {
      return false; 
    };
}; // bool IU_MACH::FindFirstLeaf()

bool IU_MACH::FindNextLeaf()
{
  return ((Backtrack()) && (CompleteSearch()));
}; // inline bool IU_MACH::FindNextLeaf()


inline bool IU_MACH::Backtrack()
{
  CALL("Backtrack()");
 
  IU_COMMAND* com;
  bool same_var;
  Unifier::Variable* var;
  
  GlobalStopFlag::check();
  
 rollback:
  com = _backtrackPoints.pop();
   
  BACKTRACK_ACTION act = _backtrackActions.pop(); 
   
  // switch (PopBacktrackAction())
  switch (act) 
    {
    case FAILURE : return false;
    case ALTER_BINDING_WITH_OC : 
     
      ASSERT(com->Symbol().var()->Binding());
      if (proc.AlterBindingWithOccCheck(Unifier::current()->LastBound(),same_var))
	{

          _backtrackPoints.push(com);

	  //PushBacktrackPoint(com);
	  if (same_var)
	    {
	      _backtrackActions.push(ALTER_BINDING_WITH_OC_SAME_VAR);
	      //PushBacktrackAction(ALTER_BINDING_WITH_OC_SAME_VAR);
	    }
	  else
	    {
	      ASSERT(com->Symbol().var()->Binding());
              _backtrackActions.push(ALTER_BINDING_WITH_OC);
	      //PushBacktrackAction(ALTER_BINDING_WITH_OC);
	    };
	  command = com + 1;
	  return true;
	}
      else 
	{
	  goto rollback; 
	};

    case ALTER_BINDING_WITH_OC_SAME_VAR :
      
      var = static_cast<Unifier::Variable*>(com->Symbol().var()->Unref());
      ASSERT(Unifier::current()->isVariable(static_cast<void*>(var)));
      if (proc.AlterBindingWithOccCheck(var,same_var))
	{
          _backtrackPoints.push(com);
	  //PushBacktrackPoint(com);
	  if (same_var)
	    {
              _backtrackActions.push(ALTER_BINDING_WITH_OC_SAME_VAR);
	      //PushBacktrackAction(ALTER_BINDING_WITH_OC_SAME_VAR);
	    }
	  else
	    {
              _backtrackActions.push(ALTER_BINDING_WITH_OC);
	      //PushBacktrackAction(ALTER_BINDING_WITH_OC);
	    };
	  command = com + 1;
	  return true;
	}
      else 
	{
	  goto rollback; 
	};
      
    case ALTER_BINDING : 
     
      ASSERT(com->Tag() == IU_COMMAND::VAR_FIRST_OCC);
      ASSERT(com->Symbol().var()->Binding());
      
      if (proc.AlterBinding())
	{
	  ASSERT(com->Symbol().var()->Binding());

          _backtrackPoints.push(com);
          _backtrackActions.push(ALTER_BINDING);
	  //PushBacktrackPoint(com);
	  //PushBacktrackAction(ALTER_BINDING);


	  command = com + 1;
	  return true;
	}
      else 
	{
	  goto rollback; 
	};
      
    case ALTER_UNIFIABLE :
     
      if (proc.AlterUnifiable())
	{
          _backtrackPoints.push(com);
          _backtrackActions.push(ALTER_UNIFIABLE);
	  //PushBacktrackPoint(com);
	  //PushBacktrackAction(ALTER_UNIFIABLE);
	  command = com + 1;
	  return true;
	}
      else 
	{
	  goto rollback; 
	};
      
    case TRY_ALTERNATIVE_NODE :
      
      proc.RestoreCurrent();
      proc.Alternative();
      if (proc.CurrNode()) { command = com; return true; }
      else 
	{
	  goto rollback; 
	};
      
    case UNBIND_LAST_TRY_ALTERNATIVE_NODE :
     
      Unifier::current()->UnbindLast();
      proc.RestoreCurrent();
      proc.Alternative();
      if (proc.CurrNode()) { command = com; return true; }
      else 
	{
	  goto rollback; 
	};
      
    case UNBIND_SECTION_TRY_ALTERNATIVE_NODE :
     
      Unifier::current()->UnbindLastSection();
      proc.RestoreCurrent();
      proc.Alternative();
      if (proc.CurrNode()) { command = com; return true; }
      else 
	{
	  goto rollback; 
	};
    };   
  ICP("R1");
  return false;
}; // bool IU_MACH::Backtrack()


#ifndef NO_DEBUG_VIS
ostream& IU_MACH::output(ostream& str) const
{
  str << "CODE:\n";
  return outputCode(str) << "INDEX = " << GetIndex() << '\n';
}; // ostream& IU_MACH::output(ostream& str) const
 
ostream& IU_MACH::outputCode(ostream& str) const
{
  const IU_COMMAND* com;
  for (com = Code(); com->Tag() != IU_COMMAND::END; com++) com->output(str,Code()) << '\n';
  return com->output(str,Code()) << '\n';
}; // ostream& IU_MACH::outputCode(ostream& str) const 
#endif



//==============================================================
