#ifndef BINARY_RESOLUTION_H
//======================================================
#define BINARY_RESOLUTION_H
#include "jargon.hpp"
#include "VampireKernelDebugFlags.hpp"
#include "premise_factoring.hpp"
#include "or_machine.hpp"
#include "weight_formula.hpp"
//================================================
#undef DEBUG_NAMESPACE
#ifdef DEBUG_BINARY_RESOLUTION
 #define DEBUG_NAMESPACE "BIN_ORD_RES"
#endif
#include "debugMacros.hpp"
//=================================================

namespace VK
{
class Clause;
class NewClause;

class BIN_ORD_RES
 {
  public:
   BIN_ORD_RES(NewClause* resolvent,OR_INDEX* resIndex);
   ~BIN_ORD_RES() { CALL("destructor ~BIN_ORD_RES()"); };
   void init(NewClause* resolvent,OR_INDEX* resIndex);
   void destroy();

   void setNoInfBetweenRulesFlag(bool fl)
   {
    _noInfBetweenRulesFlag = fl;
   };
   void Load(Clause* c);
   bool NextInference();  

   #ifndef NO_DEBUG 
    bool clean() const { return true; };
   #endif

  private:
   enum Continuation { ContFirstLit, ContFirstFactor, ContNextFactor }; 

  private: 
   void ResetFactoringState();
   bool NextFactor();
   bool MakeNewResolvent();
   void CollectWeightForm(const Clause* c);
   long MinResWeight() { return weight_form.Compute(); };
   Clause* FirstPremise() const { return premise1.TheClause(); };
   Clause* SecondPremise() const { return premise2.TheClause(); };
   inline bool CollectResult();

  private:
   OR_PREMISE_WITH_FACTORING premise1,premise2;
   ulong index1;
   ulong index2;
   ulong currentLit;
   OR_MACH ORMachine;
   WEIGHT_FORMULA weight_form;
   NewClause* _resolvent;
   Continuation _continuation;
   bool _noInfBetweenRulesFlag;
   bool _sideClauseCanNotBeRule;

 }; // class BIN_ORD_RES 
  
 
}; // namespace VK


//================================================
#undef DEBUG_NAMESPACE 
//======================================================
#endif
