//
// File:         Debug.hpp
// Description:  
// Created:      Oct 1, 1999, Alexandre Riazanov, riazanov@cs.man.ac.uk
// Revised:      Jan 17, 2002, Alexandre Riazanov, riazanov@cs.man.ac.uk
//               1) class InitStatic added instead of the old 
//               scheme for initilising the static part. 
//               2) nextEvent() made hookable from outside. 
//======================================================================
#ifndef NO_DEBUG
#ifndef DEBUG_H 
//======================================================================
#define DEBUG_H
#include <iostream>
#include "jargon.hpp"
#include "BlodKorvDebugFlags.hpp"
using namespace std;
//=====================================================================

namespace BK 
{
class Debug
{
 public:
  class InitStatic
    {
    public: 
      InitStatic();
      ~InitStatic();
    private:
      static long _count;
    }; // class InitStatic
  class Call;
 public:  
  static void assignJob(const char* jobId) { job = jobId; }; 
  static void setTracing(ostream* str,bool traceCOPs,bool traceAfterErr) 
    { 
      trace = str; 
      traceCP = traceCOPs; 
      traceAfterError = traceAfterErr;
    };
  static void tracingOff() { trace = 0; };
  static void setNextEventHook(void (*fun)(ulong))
    {
      nextEventHook = fun;
    };
  static void setErrorHook(void (*fun)())
    {
      errorHook = fun;
    };
  static void cp(const char* cop);
  static void error();
  static void error(const char* msg);
  static void debugFeatureInternalFault(const char* msg);
  static void reportCP();
  static void history();
  static ulong event() { return eventId; };                                 
  static ostream& msgStream() { return cout; };
  
 private:
  Debug() {};       
  ~Debug() {}; 
  static void nextEvent() 
    { 
      eventId++; 
      if (nextEventHook) nextEventHook(eventId); 
    }; 
  static void initSigHandlers();
  static void restoreSigHandlers();
  static void reportCP(const char* cn,const char* cp,const char* on); 
  static void handleSIGFPE(int);
  static void handleSIGILL(int);
  static void handleSIGSEGV(int);
  static void handleSIGBUS(int);
  static void handleSIGABRT(int);
  static void handleSIGTRAP(int);
 private:
  static const char* job;   
  static ulong eventId;
  static Call* currentCall;
  static Call* bottomCall;
  static long callDepth;
  static const char* errorDescription;
  static ostream* trace;
  static bool traceCP;
  static bool traceAfterError;
  static void (*nextEventHook)(ulong eventNum);
  static void (*errorHook)(); 
  friend class Call;
  friend class InitStatic;
}; // class Debug

class Debug::Call
{
public:
  Call(const char* callNm);
  Call(const char* callNm,const char* objName);
  ~Call();
private:
  Call() {};
private:
  const char* _callName;
  const char* _objName;
  const char* _controlPoint;
  Call* _outerCall;
  friend class Debug;
}; // class Debug::Call

//==================================================================


inline void Debug::cp(const char* cop) 
{    
  currentCall->_controlPoint = cop;
  nextEvent();
  if (trace && traceCP) 
    *trace << eventId << " COP " << cop << '\n' << flush; 
};

inline
Debug::Call::Call(const char* callNm)
{
 callDepth++;
 _callName = callNm;
 _objName = "?";
 _controlPoint = "start";
 _outerCall = Debug::currentCall;
 Debug::currentCall = this;

 nextEvent();
 if (trace)
  {
   *trace << eventId << " -> [" << callDepth << "] " << callNm 
          << " [?]\n" << flush;
  };
}; // Debug::Call::Call(const char* nameSp,const char* callNm)

inline
Debug::Call::Call(const char* callNm,const char* objName)
{
 callDepth++;
 _callName = callNm;
 _objName = objName;
 _controlPoint = "start";
 _outerCall = Debug::currentCall;
 Debug::currentCall = this;

 nextEvent();
 if (trace)
  {
   *trace << eventId << " -> [" << callDepth << "] " << callNm 
          << " [" << objName << "]\n" << flush;
  };
}; // Debug::Call::Call(const char* nameSp,const char* callNm)


inline
Debug::Call::~Call()
{
 nextEvent();
 if (trace)
  {
   *trace << eventId  << " <- [" << callDepth << "] " << _callName 
          << " [" << _objName << "] after " << _controlPoint << '\n' << flush; 
  };
 Debug::currentCall = _outerCall;
 callDepth--;   
}; // Debug::Call::~Call() 

//===================================================================

static Debug::InitStatic debugInitStatic;

}; // namespace BK


//===================================================================
#endif
#endif
