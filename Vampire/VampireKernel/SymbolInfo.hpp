//
// File:         SymbolInfo.hpp
// Description:  Tabulated information about functors and predicates.
// Created:      Dec 26, 2001.
// Author:       Alexandre Riazanov
// mail:         riazanov@cs.man.ac.uk
//============================================================================
#ifndef SYMBOL_INFO_H
//=============================================================================
#define SYMBOL_INFO_H
#include "jargon.hpp"
#include "VampireKernelDebugFlags.hpp"
#include "GlobAlloc.hpp"
#include "Array.hpp"
//============================================================================
#undef DEBUG_NAMESPACE
#ifdef DEBUG_SYMBOL_INFO
 #define DEBUG_NAMESPACE "SymbolInfo"
#endif
#include "debugMacros.hpp"
//============================================================================

namespace VK
{
class Clause;

class SymbolInfo
{
 public: 
  SymbolInfo() :
   #ifdef NO_DEBUG
    _table(Entry(),(int)0)
   #else
    _table(Entry(),"SymbolInfo::_table")
   #endif    
  {
   CALL("constructor SymbolInfo()");
   DOP(_table.freeze());
   _numOfEliminated = 0UL;  
   _numOfCommutative = 0UL;
   _numOfSymmetric = 0UL;
  };

  ~SymbolInfo() {};

  void init() 
  {
   CALL("init()");
   #ifdef NO_DEBUG
    _table.init(Entry(),(int)0);
   #else
    _table.init(Entry(),"SymbolInfo::_table");
   #endif
   DOP(_table.freeze());
   _numOfEliminated = 0UL;  
   _numOfCommutative = 0UL;
   _numOfSymmetric = 0UL;
  };

  void destroy()
  {
    CALL("destroy()");
    _table.destroy();
  };
  void reset() 
  {
   CALL("reset()");
   for (ulong n = 0; n < _table.size(); n++)
     _table[n].init();
   _numOfEliminated = 0UL;  
   _numOfCommutative = 0UL;
   _numOfSymmetric = 0UL;
  };

  ulong numOfEliminated() const { return _numOfEliminated; };
  ulong numOfCommutative() const { return _numOfCommutative; };
  ulong numOfSymmetric() const { return _numOfSymmetric; }; 

  bool isToBeEliminated(ulong predNum) const
  {
   CALL("isToBeEliminated(ulong predNum) const");
   if (predNum >= _table.size()) return false;
   return _table[predNum].isToBeEliminated;
  };

  const Clause* commutativityLaw(ulong funNum) const
  {
   CALL("commutativityLaw(ulong funNum) const");
   if (funNum >= _table.size()) return 0;
   return _table[funNum].commutativityLaw;
  };
 
  const Clause* symmetryLaw(ulong predNum) const
  {
   CALL("symmetryLaw(ulong predNum) const");
   if (predNum >= _table.size()) return 0;
   return _table[predNum].symmetryLaw;
  };

  void registerEliminated(ulong predNum)
  {
   CALL("registerEliminated(ulong predNum)");
   DOP(_table.unfreeze());
   if (!_table.sub(predNum).isToBeEliminated) _numOfEliminated++;
   DOP(_table.freeze());
   _table[predNum].isToBeEliminated = true; 
  };

  void registerCommutativity(ulong funNum,const Clause* axiom)
  {
   CALL("registerCommutativity(ulong funNum,const Clause* axiom)");
   ASSERT(axiom);
   DOP(_table.unfreeze());
   if (!_table.sub(funNum).commutativityLaw) _numOfCommutative++;
   DOP(_table.freeze());
   _table[funNum].commutativityLaw = axiom;
  };  
 
  void registerSymmetry(ulong predNum,const Clause* axiom)
  {
   CALL("registerSymmetry(ulong predNum,const Clause* axiom)");
   ASSERT(axiom);
   DOP(_table.unfreeze());
   if (!_table.sub(predNum).symmetryLaw) _numOfCommutative++;
   DOP(_table.freeze());
   _table[predNum].symmetryLaw = axiom;
  };

 private: 

  class Entry
  {
   public:
    Entry() 
    {
     isToBeEliminated = false;
     commutativityLaw = 0; 
     symmetryLaw = 0;  
    };
    void init() 
    {
     isToBeEliminated = false;
     commutativityLaw = 0; 
     symmetryLaw = 0; 
    };  
   public:
    bool isToBeEliminated;
    const Clause* commutativityLaw;
    const Clause* symmetryLaw;
  }; // class Entry

 private:
  
  BK::Array<BK::GlobAlloc,Entry,128UL,SymbolInfo> _table;
  ulong _numOfEliminated;  
  ulong _numOfCommutative;
  ulong _numOfSymmetric;
}; // class SymbolInfo

}; // namespace VK
//======================================================================
#endif
