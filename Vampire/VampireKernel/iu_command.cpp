
#include "iu_command.hpp"
//===========================================================
using namespace std;
using namespace VK;

#ifndef NO_DEBUG_VIS
ostream& operator<<(ostream& str,IU_COMMAND::TAG tag)
{
  switch (tag)
    {
    case IU_COMMAND::START: return str << "START";
    case IU_COMMAND::VAR: return str << "VAR"; 
    case IU_COMMAND::VAR_FIRST_OCC: return str << "VAR_FIRST_OCC"; 
    case IU_COMMAND::FUNC: return str << "FUNC"; 
    case IU_COMMAND::FUNC_PLAIN: return str << "FUNC_PLAIN"; 
    case IU_COMMAND::FUNC_GROUND: return str << "FUNC_GROUND"; 
    case IU_COMMAND::CONST: return str << "CONST";
    case IU_COMMAND::END: return str << "END"; 
    default: return str << "UNKNOWN IU_COMMAND"; 
    };
};

ostream& IU_COMMAND::output(ostream& str,const IU_COMMAND* start) const
{
  str << '[' << this - start << "] " << Tag();
  if (Tag() == END) return str;
  str << ' ';
  Symbol().output(str); 

  if ((Tag() == FUNC) || (Tag() == FUNC_GROUND) || (Tag() == FUNC_PLAIN))
    {
      str << " next = [" << NextCommand() - start << "] ";
      Prefix::outputTerm(str,Term());
    };
  if (Tag() == START) 
    {
      str << " variables = [" << (AllVars() ? (AllVars() - start) : 0) << "] ";
      Prefix::outputTerm(str,Term());
    }; 
  if (Tag() == VAR_FIRST_OCC) 
    str << " next variable = [" << (NextVar() ? (NextVar() - start) : 0) << ']';
  return str;
}; // ostream& IU_COMMAND::output(ostream& str,const IU_COMMAND* start) const

ostream& IU_COMMAND::outputCode(ostream& str) const
{
  const IU_COMMAND* com;
  for (com = this; com->Tag() != END; com++)
    com->output(str,this) << '\n';
  return com->output(str,this) << '\n';
}; // ostream& IU_COMMAND::outputCode(ostream& str) const 
#endif

//===========================================================
