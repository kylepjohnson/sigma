//
// File:         DefinedHeaderTable.hpp
// Description:  Set of defined headers for stratified resolution.
// Created:      Dec 24, 2001.
// Author:       Alexandre Riazanov
// mail:         riazanov@cs.man.ac.uk
//============================================================================
#ifndef DEFINED_HEADER_TABLE_H
//============================================================================

#define DEFINED_HEADER_TABLE_H
#include "jargon.hpp"
#include "VampireKernelDebugFlags.hpp"
#include "GlobAlloc.hpp"
#include "Array.hpp"
//============================================================================
#undef DEBUG_NAMESPACE
#ifdef DEBUG_DEFINED_HEADER_TABLE
 #define DEBUG_NAMESPACE "DefinedHeaderTable"
#endif
#include "debugMacros.hpp"
//============================================================================
namespace VK
{
class DefinedHeaderTable
{
 public:
  DefinedHeaderTable() :
#ifdef NO_DEBUG 
    _defined(false,0)
#else
    _defined(false,"DefinedHeaderTable::_defined")
#endif   
  {
   CALL("DefinedHeaderTable()");
   DOP(_defined.freeze());
  };

  ~DefinedHeaderTable() 
  {
    CALL("destructor ~DefinedHeaderTable()");    
  };

  void init()
  {
   CALL("init()");
    #ifdef NO_DEBUG 
     _defined.init(false,0);
    #else
     _defined.init(false,"DefinedHeaderTable::_defined");
    #endif
   DOP(_defined.freeze());
  };

  void destroy()
  {
    CALL("destroy()");
    _defined.destroy();
  };

  void registerDefined(ulong headerNum) 
  {
   CALL("registerDefined(ulong headerNum)");
   DOP(_defined.unfreeze());
   _defined.sub(headerNum) = true;
   DOP(_defined.freeze());
  };

  bool isDefined(ulong headerNum) const 
  {
   CALL("isDefined(ulong headerNum) const");
   if (headerNum >= _defined.size()) return false;
   return _defined[headerNum];
  };  

 private:
  BK::Array<BK::GlobAlloc,bool,128UL,DefinedHeaderTable> _defined;
}; // class DefinedHeaderTable

}; // namespace VK
//======================================================================
#endif
