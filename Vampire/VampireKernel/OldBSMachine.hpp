//
// File:         OldBSMachine.hpp
// Description:  Core of backward subsumption based on discrimination trees. 
// Created:      Feb 26, 2000, 20:20
// Author:       Alexandre Riazanov
// mail:         riazanov@cs.man.ac.uk
//============================================================================
#ifndef OLD_BS_MACHINE_H  
//=============================================================================
#define OLD_BS_MACHINE_H 
#ifndef NO_DEBUG_VIS
 #include <iostream>
#endif
#include "jargon.hpp"
#include "VampireKernelDebugFlags.hpp"
#include "VampireKernelConst.hpp"
#include "Stack.hpp"
#include "OldBSIndex.hpp"
#include "Term.hpp"
#include "Clause.hpp"
//============================================================================
#undef DEBUG_NAMESPACE
#ifdef DEBUG_OLD_BS_MACHINE
 #define DEBUG_NAMESPACE "OldBSMachine"
#endif
#include "debugMacros.hpp"
//============================================================================
namespace VK
{
  using namespace BK;
  using namespace Gem;
class OldBSMachine
{
 public: 
  typedef OldBSIndex::CSkipList CSkipList;
 public:
  OldBSMachine() {};
  ~OldBSMachine() { CALL("destructor ~OldBSMachine()"); };
  bool subsumeNextSetMode(ulong& subsumedClNum);
  bool subsumeNextMultisetMode(ulong& subsumedClNum);
  void resetQuery(const Clause* cl) 
  { 
   _clause = cl;
  };
  bool endOfQuery() 
  {
   return false; // dummy 
  }; 
  bool queryPropLit(const CSkipList* occList) 
  {  
   return false; // dummy 
  }; 
  void queryNextLit() 
  { 
  }; 
  bool queryEndOfLit() 
  {      
    return false; // dummy 
  };

 private:
  const Clause* _clause;
}; // class OldBSMachine 

}; // namespace VK


//======================================================================
#undef DEBUG_NAMESPACE
//======================================================================
#endif
