//
// File:         DivIntoComp.hpp
// Description:  Division into components in linear time.
// Created:      Ja 20, 2000, 14:20
// Author:       Alexandre Riazanov
// mail:         riazanov@cs.man.ac.uk
//
#ifndef DIV_INTO_COMP_H
#define DIV_INTO_COMP_H
//============================================================================
#include "jargon.hpp"
#include "Multiset.hpp"
#include "Array.hpp"
#include "GlobAlloc.hpp"
//============================================================================
#undef DEBUG_NAMESPACE
#ifdef DEBUG_DIV_INTO_COMP
 #define DEBUG_NAMESPACE "DivIntoComp"
#endif 
#include "debugMacros.hpp"
//=================================================

namespace VK
{

template <ulong MaxNumOfVars>
class DivIntoComp
{
 public:
  DivIntoComp() 
   : comp(DOP("DivIntoComp::comp")), 
     reg(), 
     groundCompOptimalWeight(ULONG_MAX) 
  {
   CALL("constructor DivIntoComp()"); 
   reset(); 
   DOP(comp.freeze());
  };

  ~DivIntoComp() {};

  void init()
  {
    CALL("init()");
    comp.init(DOP("DivIntoComp::comp"));
    reg.init(); 
    groundCompOptimalWeight = ULONG_MAX;
    reset(); 
    DOP(comp.freeze());
  };

  void destroy()
  {
    CALL("destroy()");
    reg.destroy();
    comp.destroy();
  };

  void adjustSizes(ulong newNumOfLits)
  {
   CALL("adjustSizes(ulong newNumOfLits)");
   DOP(comp.unfreeze());
   comp.expand(newNumOfLits); 
   DOP(comp.freeze());
  };
  void setGroundCompOptimalWeight(ulong gcow) { groundCompOptimalWeight = gcow; };
  void reset() 
  {
   CALL("reset()");
   currLit = -1; 
   reg.reset();  
   numOfComps = 0;       
   currentGroundComp = -1; 
   currGroundCompWeight = 0;
  };

  ulong nextLit(bool ground,ulong groundWeight)
  {
   CALL("nextLit(bool ground,ulong groundWeight)");
   currLit++;
   comp[currLit] = currLit; 
   if (ground)
    {     
     if (currentGroundComp < 0) // no ground components so far
      { 
       numOfComps++;
       currentGroundComp = currLit;
       currGroundCompWeight = groundWeight; 
      }
     else 
      {
       if (currGroundCompWeight + groundWeight <= groundCompOptimalWeight)
	{
         comp[currentGroundComp] = currLit;
         currentGroundComp = currLit;
         currGroundCompWeight += groundWeight; 
        }
       else // another ground component
        {
         numOfComps++;
         currentGroundComp = currLit;
         currGroundCompWeight = groundWeight;  
        };
      };
    } 
   else numOfComps++;
   return (ulong)currLit; 
  };


  void nextVar(const ulong& var)
  {
   CALL("nextVar(const ulong& var)");
   if (registeredVar(var)) 
    {
     long lt = lit[var];
     long tmp;
     while ((tmp = comp[lt]) != lt)
      {  
       comp[lt] = currLit;              
       lt = tmp;
      };
     if (comp[lt] != currLit)
      {
       comp[lt] = currLit;
       numOfComps--;
      };
     lit[var] = currLit; // nonnecessary but may be helpful   
    }
   else
    {
     lit[var] = currLit;
     registerVar(var);
    };
  };
  void startResultCollection() {};
  bool litCompPair(ulong& litNum,ulong& compNum)
  {
   CALL("litCompPair(ulong& litNum,ulong& compNum)");
   if (currLit < 0) return false;
   if (comp[currLit] == currLit) 
    { 
     litNum = currLit;
     compNum = currLit;
    }
   else 
    {
     comp[currLit] = comp[comp[currLit]];      
     litNum = currLit;
     compNum = comp[currLit];
    };
   currLit--; 
   return true;
  };
  ulong numOfComponents() const { return numOfComps; };
 private:
  bool registeredVar(const ulong& var) const { return reg.scoreOf(var) != 0L; };
  void registerVar(const ulong& var) { reg.add(var); };
 private:

	 BK::Array<BK::GlobAlloc,long,32UL,DivIntoComp<MaxNumOfVars> > comp;
  //long comp[MaxNumOfLits];

  long lit[MaxNumOfVars];
  BK::Multiset<ulong,MaxNumOfVars> reg; 
  long currLit; 
  ulong numOfComps;
  long currentGroundComp;
  ulong currGroundCompWeight;
  ulong groundCompOptimalWeight;
}; // template <ulong MaxNumOfVars> class DivIntoComp



}; // namespace VK


//=============================================================================
#endif
