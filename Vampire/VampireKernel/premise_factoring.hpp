//
// File:         premise_factoring.hpp
// Description:  OR premises with factoring.
// Created:      Feb 15, 2000, Alexandre Riazanov, riazanov@cs.man.ac.uk
// Revised:      Dec 10, 2001 , Alexandre Riazanov, riazanov@cs.man.ac.uk
//               The arrays OR_PREMISE::{max_literal,mark} replaced by 
//               extending arrays OR_PREMISE::{_maxLiteral,_litIsMarked}
// Revised:      Dec 11, 2001, Alexandre Riazanov, riazanov@cs.man.ac.uk 
//               The stack OR_PREMISE_WITH_FACTORING::factored replaced by 
//               extending stack OR_PREMISE_WITH_FACTORING::_factoredLits
//====================================================
#ifndef PREMISE_FACTORING_H
//======================================================================
#define PREMISE_FACTORING_H
#include "jargon.hpp"
#include "VampireKernelDebugFlags.hpp"
#include "or_premise.hpp"
#include "ExpandingStack.hpp"
#include "GlobAlloc.hpp"
//================================================
#undef DEBUG_NAMESPACE
#ifdef DEBUG_PREMISE_FACTORING
 #define DEBUG_NAMESPACE "OR_PREMISE_WITH_FACTORING"
#endif
#include "debugMacros.hpp"
//=================================================
namespace VK
{
class NewClause;

class OR_PREMISE_WITH_FACTORING : public OR_PREMISE
 {
  private:
   BK::ExpandingStack<BK::GlobAlloc,ulong,32UL, OR_PREMISE_WITH_FACTORING> _factoredLits;
   
   PrefixSym* cut_literal; // the literal itself in prefix representation
   
   ulong first_factor;
   ulong last_factor;
   bool was_factored;
  public:
   OR_PREMISE_WITH_FACTORING();
   OR_PREMISE_WITH_FACTORING(NewClause* res);
   ~OR_PREMISE_WITH_FACTORING();
   void init();
   void init(NewClause* res);
   void destroy();

   void assignResolvent(NewClause* res) { OR_PREMISE::setResolvent(res); };
   
   void Load(ulong ind,Clause* c)
   {
    CALL("Load(ulong ind,Clause* c)");
    OR_PREMISE::Load(ind,c);

    DOP(_factoredLits.unfreeze());
    _factoredLits.expand(c->numOfAllLiterals());    
    DOP(_factoredLits.freeze());
   };
   void LoadOptimized(ulong ind,Clause* c)
   {
    CALL("LoadOptimized(ulong ind,Clause* c)");
    OR_PREMISE::LoadOptimized(ind,c);

    DOP(_factoredLits.unfreeze());
    _factoredLits.expand(c->numOfAllLiterals());    
    DOP(_factoredLits.freeze());
   };
   void Load(ulong ind,RES_PAIR_LIST* res_pair)
   {
    CALL("Load(ulong ind,RES_PAIR_LIST* res_pair)");
    OR_PREMISE::Load(ind,res_pair);

    DOP(_factoredLits.unfreeze());
    _factoredLits.expand(res_pair->TheClause()->numOfAllLiterals());    
    DOP(_factoredLits.freeze());
   };
   void LoadOptimized(ulong ind,RES_PAIR_LIST* res_pair)
   {
    CALL("LoadOptimized(ulong ind,RES_PAIR_LIST* res_pair)");
    OR_PREMISE::LoadOptimized(ind,res_pair);

    DOP(_factoredLits.unfreeze());
    _factoredLits.expand(res_pair->TheClause()->numOfAllLiterals());    
    DOP(_factoredLits.freeze());
   };

   void ResetFactoring()
    {
     CALL("ResetFactoring()");
     was_factored = false;
     if (FactoringPossible())
      {
       _factoredLits.reset();
       cut_literal = OR_PREMISE::_maxLiteral[cut_lit]; // max_literal[cut_lit];
      };
    };
   bool WasFactored() { return was_factored; };
   bool FactoringPossible() { return LastMax() != 0UL; };
   bool GetFirstFactor(ulong first_candidate);
   bool GetNextFactor();
  private: // aux. methods
   bool FindFirstFactor(ulong& candidate);
 }; // class OR_PREMISE_WITH_FACTORING

}; // namespace VK

//======================================================================
#endif
