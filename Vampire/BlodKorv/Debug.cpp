//
// File:         Debug.cpp
// Description:   
// Created:      Oct 1, 1999, Alexandre Riazanov, riazanov@cs.man.ac.uk
// Revised:      Jan 17, 2002, Alexandre Riazanov, riazanov@cs.man.ac.uk
//               1) class InitStatic added instead of the old 
//               scheme for initilising the static part. 
//               2) nextEvent() made hookable from outside.  
//==============================================================================
#ifndef NO_DEBUG
//==============================================================================
#include <iostream>
#include <csignal>
#include <climits>
#include "Debug.hpp"
#include "Exit.hpp"
//==============================================================================
using  namespace std;
using namespace BK;

long Debug::InitStatic::_count; // Implicitely initialised to 0L
const char* Debug::job;  
ulong Debug::eventId;
Debug::Call* Debug::currentCall;
Debug::Call* Debug::bottomCall;
long Debug::callDepth;
const char* Debug::errorDescription; 
ostream* Debug::trace;
bool Debug::traceCP;
bool Debug::traceAfterError;
void (*Debug::nextEventHook)(ulong);
void (*Debug::errorHook)() = 0;


Debug::InitStatic::InitStatic() 
{
  if (!_count) 
    { 
#ifdef DEBUG_ERROR_SIGNALS
      Debug::initSigHandlers();
#endif
      Debug::callDepth = 0L;
      Debug::job = "unknown job";
      Debug::eventId = 0UL;
      Debug::errorDescription = "unknown";
      Debug::trace = 0;
      Debug::traceCP = false;   
      Debug::traceAfterError = false;
      Debug::nextEventHook = 0;
      Debug::errorHook = 0;
      Debug::bottomCall = new Debug::Call("<global>","<global>");
    };    
  ++_count;
}; // Debug::InitStatic::InitStatic() 


Debug::InitStatic::~InitStatic() 
{
  --_count;
  if (!_count)
    {      
      delete Debug::bottomCall;
    };
}; // Debug::InitStatic::~InitStatic() 


void Debug::reportCP(const char* cn,const char* cp,const char* on)
{
  cout << cn << '/' << cp << " [" << on << ']';
};

void Debug::error()
{
 cout << "Job " << job << " aborted.\n"; 
 cout << "Error : " << errorDescription << " in\n";
 history(); 
 cout << "The job was " << job << '\n';
 if (!traceAfterError) tracingOff();
 if (errorHook) 
   {
     void (*tmpErrorHook)();
     tmpErrorHook = errorHook;
     errorHook = 0;
     tmpErrorHook();
   };
 Exit::exit(1);
};

void Debug::error(const char* msg)
{
 errorDescription = msg;
 error();
};

void Debug::debugFeatureInternalFault(const char* msg)
{
 cout << "Job " << job << " aborted.\n"; 
 cout << "Error : debug feature internal fault : " << msg << " in \n";
 history(); 
 cout << "The job was " << job << '\n';
 if (!traceAfterError) tracingOff();
 Exit::exit(1);
};

void Debug::reportCP()
{
  cout << eventId << " ";
  reportCP(currentCall->_callName,currentCall->_controlPoint,currentCall->_objName); 
  cout << '\n';
};

void Debug::history()
{
  cout << eventId << " ";
  int indent = 0;
  Call* call = currentCall;
  while (call)
    {
      for (int i = 0; i < indent; i++) cout << ' ';
      if (indent) cout << "|_ ";
      ++indent;
      reportCP(call->_callName,call->_controlPoint,call->_objName); 
      cout << '\n';
      call = call->_outerCall;
    };
}; // void Debug::history()


void Debug::initSigHandlers()
{
#ifdef BK_TARGET_OPERATIONAL_ENVIRONMENT_TYPE
#  if (BK_TARGET_OPERATIONAL_ENVIRONMENT_TYPE == BK_TARGET_OPERATIONAL_ENVIRONMENT_TYPE_UNIX)
  signal(SIGFPE,handleSIGFPE);
  signal(SIGILL,handleSIGILL);
  signal(SIGSEGV,handleSIGSEGV);
  signal(SIGBUS,handleSIGBUS);  
  signal(SIGTRAP,handleSIGTRAP);
  signal(SIGABRT,handleSIGABRT);
#  elif (BK_TARGET_OPERATIONAL_ENVIRONMENT_TYPE == BK_TARGET_OPERATIONAL_ENVIRONMENT_TYPE_MSWIN32 )
  signal(SIGFPE,handleSIGFPE);
  signal(SIGILL,handleSIGILL);
  signal(SIGSEGV,handleSIGSEGV); 
#  else
#    error "Unsupported target operational environment type, check BK_TARGET_OPERATIONAL_ENVIRONMENT_TYPE"
#  endif
#else
#  error "Undefined macro BK_TARGET_OPERATIONAL_ENVIRONMENT_TYPE"
#endif
}; // void Debug::initSigHandlers()

void Debug::restoreSigHandlers()
{
  #ifdef BK_TARGET_OPERATIONAL_ENVIRONMENT_TYPE
#  if (BK_TARGET_OPERATIONAL_ENVIRONMENT_TYPE == BK_TARGET_OPERATIONAL_ENVIRONMENT_TYPE_UNIX)
  signal(SIGFPE,SIG_DFL);
  signal(SIGILL,SIG_DFL);
  signal(SIGSEGV,SIG_DFL);
  signal(SIGBUS,SIG_DFL);  
  signal(SIGTRAP,SIG_DFL);
  signal(SIGABRT,SIG_DFL);
#  elif (BK_TARGET_OPERATIONAL_ENVIRONMENT_TYPE == BK_TARGET_OPERATIONAL_ENVIRONMENT_TYPE_MSWIN32 )
  signal(SIGFPE,SIG_DFL);
  signal(SIGILL,SIG_DFL);
  signal(SIGSEGV,SIG_DFL); 
#  else
#    error "Unsupported target operational environment type, check BK_TARGET_OPERATIONAL_ENVIRONMENT_TYPE"
#  endif
#else
#  error "Undefined macro BK_TARGET_OPERATIONAL_ENVIRONMENT_TYPE"
#endif
}; // void Debug::restoreSigHandlers()
         
void Debug::handleSIGFPE(int)
{
 restoreSigHandlers();
 error("Program Error Signal SIGFPE (arithmetic error)");
};
void Debug::handleSIGILL(int)
{
 restoreSigHandlers();
 error("Program Error Signal SIGILL (illegal instruction)");
};
void Debug::handleSIGSEGV(int)
{
 restoreSigHandlers();
 error("Program Error Signal SIGSEGV (segmentation violation)");
};
void Debug::handleSIGBUS(int) 
{
 restoreSigHandlers();
 error("Program Error Signal SIGBUS (bus error)");
};
void Debug::handleSIGABRT(int)
{
 restoreSigHandlers();
 error("Program Error Signal SIGABRT");
};
void Debug::handleSIGTRAP(int)
{
 restoreSigHandlers();
 error("Program Error Signal SIGTRAP");
};


//=========================================================================
#endif
