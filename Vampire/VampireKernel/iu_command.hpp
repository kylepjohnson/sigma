#ifndef IU_COMMAND_H
//===========================================================
#define IU_COMMAND_H
#ifndef NO_DEBUG_VIS
 #include <iostream>
#endif
#include "jargon.hpp"
#include "VampireKernelDebugFlags.hpp"
#include "prefix.hpp"
//#include "variables.hpp"
//================================================
#undef DEBUG_NAMESPACE
#ifdef DEBUG_IU_COMMAND
 #define DEBUG_NAMESPACE "IU_COMMAND"
#endif
#include "debugMacros.hpp"
//=================================================
namespace VK
{
class IU_COMMAND
 {
  public: 
   enum TAG
    {
     START,
     VAR,
     VAR_FIRST_OCC,
     FUNC,
     FUNC_PLAIN,
     FUNC_GROUND,
     CONST,
     END
    };
  private: // structure of a command
   TAG tag;
   PrefSym sym;
   PrefixSym* term;
   IU_COMMAND* nextCommand;
  public:
   IU_COMMAND() {};
   ~IU_COMMAND() {};
   TAG& Tag() { return tag; };
   const TAG& Tag() const { return tag; };
   const PrefSym& Symbol() const { return sym; };
   PrefSym& Symbol() { return sym; };
   PrefixSym*& Term() { return term; };
   const PrefixSym* const & Term() const 
   { 
     // Temporary res is introduced to avoid false gcc 3.0.4 warnings
     const PrefixSym* const & res = term;
     return res;
   };
   IU_COMMAND*& NextCommand() { return nextCommand; };
   const IU_COMMAND* const & NextCommand() const 
   { 
     // Temporary res is introduced to avoid false gcc 3.0.4 warnings
     const IU_COMMAND* const & res = nextCommand;
     return res;
   };
   IU_COMMAND*& AllVars() 
    {
     CALL("AllVars()");
     ASSERT(Tag() == START);
     return nextCommand; 
    };
   const IU_COMMAND* const & AllVars() const 
    {
     CALL("AllVars()");
     ASSERT(Tag() == START);
     return NextCommand();
    };
   IU_COMMAND*& NextVar() 
    {
     CALL("NextVar()");
     ASSERT(Tag() == VAR_FIRST_OCC);
     return nextCommand; 
    };
   const IU_COMMAND* const & NextVar() const
    {
     CALL("NextVar()");
     ASSERT(Tag() == VAR_FIRST_OCC);
     return NextCommand();    
    }; 
   void Inversepolarity() // applicable to a START command only
    {
     CALL("Inversepolarity()");
     ASSERT(Tag() == START);
     sym.Inversepolarity();
    };
  public: // for debugging 
   #ifndef NO_DEBUG_VIS
    ostream& output(ostream& str,const IU_COMMAND* start) const;
    ostream& outputCode(ostream& str) const; 
   #endif
 }; // class IU_COMMAND

}; // namespace VK

//===========================================================
#endif
