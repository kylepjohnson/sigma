//
// File:         debugMacros.hpp
// Description:  Macros for debugging.
// Created:      Sep 15, 1999, 20:00
// Author:       Alexandre Riazanov
// mail:         riazanov@cs.man.ac.uk
//=========================================================================
#include "Debug.hpp"
//=========================================================================
#ifdef DEBUG_NAMESPACE
 #undef CALL
 #ifdef DEBUG_NO_TRACING
  #define CALL(fname)
 #else
  #define CALL(fname) BK::Debug::Call debugFunctionCall(DEBUG_NAMESPACE "::" fname);
 #endif 

 #undef CALLM
 #ifdef DEBUG_NO_TRACING
  #define CALLM(objName,fname)
 #else
  #define CALLM(objName,fname) BK::Debug::Call debugMethodCall(DEBUG_NAMESPACE "::" fname,objName);
 #endif

 #undef COP
 #ifdef DEBUG_NO_TRACING
  #define COP(cop)
 #else
  #define COP(cop) BK::Debug::cp(cop);
 #endif

 #undef ASSERT
 #define ASSERT(cond) { if (!( cond )) BK::Debug::error("Assertion " #cond " violated."); }

 #undef ASSERT_IN
 #define ASSERT_IN(cond,cop) { if (!( cond )) { BK::Debug::cp(cop); BK::Debug::error("Assertion " #cond " violated."); }}

 #undef ALWAYS
 #define ALWAYS(cond)  { if (!( cond )) BK::Debug::error("Assertion ALWAYS(" #cond ") violated."); }

 #undef NEVER
 #define NEVER(cond)  { if ( cond ) BK::Debug::error("Assertion NEVER(" #cond ") violated."); }

 #undef ALMOST_ALWAYS
 #define ALMOST_ALWAYS(cond) { if (!(cond)) { BK::Debug::msgStream() << "Condition " #cond " , that must be almost always true, violated.\n"; BK::Debug::reportCP(); BK::Debug::history(); }; }


 #undef ICP
 #ifdef DEBUG_NO_TRACING
  #define ICP(cop) { BK::Debug::error("Illegal control point"); }
 #else
  #define ICP(cop) { BK::Debug::cp(cop); BK::Debug::error("Illegal control point"); }
 #endif


 #undef DOP
 #define DOP(e) e

 #undef BK_CORRUPT
 #define BK_CORRUPT(obj) \
   { \
     unsigned long* debugObjEnd = (unsigned long*)((&obj) + 1); \
     for (unsigned long* debugPtr = (unsigned long*)(&obj); \
          debugPtr + 1 <= debugObjEnd; \
	  ++debugPtr) \
       *debugPtr = 0x7B7B7B7B; \
   }

 #undef BK_CORRUPTMEM
 #define BK_CORRUPTMEM(ptr,size) \
   { \
     for (size_t debugIntVar = 0; debugIntVar < size/sizeof(unsigned long); ++debugIntVar) \
       ((unsigned long*)ptr)[debugIntVar] = 0x7B7B7B7B; \
   }

 #undef DMSG
 #define DMSG(m) BK::Debug::msgStream() << m

 #undef DWARN
 #define DWARN(m) BK::Debug::msgStream() << "BK::Debug warning:" << m << "\n"

 #undef REPCP
 #define REPCP BK::Debug::reportCP();

 #undef REPCPH
 #define REPCPH { BK::Debug::reportCP(); BK::Debug::history(); }

 #undef TRACE
 #define TRACE(traceCP,traceAterError) { BK::Debug::setTracing(&cout,traceCP,traceAterError); }


 #undef TRACINGOFF
 #define TRACINGOFF { BK::Debug::tracingOff(); }

 #undef PTROF
 #define PTROF(ptr,obj) ((((ulong*)ptr) >= ((ulong*)obj)) && (((ulong*)ptr) < ((ulong*)(obj + 1))))

 #undef MYPTR
 #define MYPTR(ptr) ((((ulong*)ptr) >= ((ulong*)this)) && (((ulong*)ptr) < ((ulong*)(this + 1))))


 #undef RARE
 #define RARE(cond) { if (cond) { BK::Debug::msgStream() << "Rare condition " #cond " detected.\n"; BK::Debug::reportCP(); BK::Debug::history(); }; }

#else

 #undef CALL
 #define CALL(fname)

 #undef CALLM
 #define CALLM(objName,fname)

 #undef COP
 #define COP(cop)

 #undef ASSERT
 #define ASSERT(cond)

 #undef ASSERT_IN
 #define ASSERT_IN(cond,cop)
 
 #undef ALWAYS
 #define ALWAYS(cond) cond

 #undef NEVER
 #define NEVER(cond) cond

 #undef ALMOST_ALWAYS
 #define ALMOST_ALWAYS(cond) cond

 #undef ICP
 #define ICP(cop)


 #undef DOP
 #define DOP(e) 
 
 #undef BK_CORRUPT
 #define BK_CORRUPT(obj)

 #undef BK_CORRUPTMEM
 #define BK_CORRUPTMEM(ptr,size)

 #undef DMSG
 #define DMSG(m)

 #undef DWARN
 #define DWARN(m)

 #undef REPCP
 #define REPCP

 #undef REPCPH
 #define REPCPH 

 #undef TRACE
 #define TRACE(traceCP,traceAterError)

 #undef TRACINGOFF
 #define TRACINGOFF

 #undef PTROF
 #define PTROF(ptr,obj) Debugging_feature_uncommented

 #undef MYPTR
 #define MYPTR(ptr) Debugging_feature_uncommented

 #undef RARE
 #define RARE(cond)
#endif

#ifndef NO_DEBUG 

 #undef SETDEBUGEVENTHOOK
 #define SETDEBUGEVENTHOOK(hookFun) { BK::Debug::setNextEventHook(hookFun); }

#else 

 #undef SETDEBUGEVENTHOOK
 #define SETDEBUGEVENTHOOK(hookFun) { Debugging_event_hook_uncommented }

#endif 


#ifndef NO_DEBUG 
 #undef MEMMAP 
 #define MEMMAP(objects) { ClassDesc::outputAll(cout,objects); } 
#else
 #undef MEMMAP 
 #define MEMMAP(objects)
#endif


#undef DF
#ifdef NO_DEBUG
 #define DF { Debugging_feature_uncommented } 
#else
 #define DF 
#endif



//===========================================================================

