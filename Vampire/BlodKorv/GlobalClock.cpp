//
// File:         GlobalClock.cpp
// Description:  A handy wrapper for the virtual process timer.
// Created:      Jan 30, 2000, Alexandre Riazanov, riazanov@cs.man.ac.uk 
//============================================================================
#include "jargon.hpp"
#if (BK_TARGET_OPERATIONAL_ENVIRONMENT_TYPE == BK_TARGET_OPERATIONAL_ENVIRONMENT_TYPE_UNIX)
#  include <sys/time.h>
#  include <csignal>
#elif (BK_TARGET_OPERATIONAL_ENVIRONMENT_TYPE == BK_TARGET_OPERATIONAL_ENVIRONMENT_TYPE_MSWIN32)
#  include <windows.h>
#endif
#include "GlobalClock.hpp"
#include "RuntimeError.hpp"
//============================================================================
#undef DEBUG_NAMESPACE
#ifdef DEBUG_ARRAY
#  define DEBUG_NAMESPACE "GlobalClock"
#endif
#include "debugMacros.hpp"
//============================================================================

using namespace BK;
long GlobalClock::InitStatic::_count; // implicitely initialised to 0L
GlobalClock::AlarmHandler* GlobalClock::_currentAlarmHandler; 
int GlobalClock::_elapsedDeciseconds;

#if (BK_TARGET_OPERATIONAL_ENVIRONMENT_TYPE == BK_TARGET_OPERATIONAL_ENVIRONMENT_TYPE_MSWIN32)
class GlobalClock::_Private
// to avoid including <windows.h> in GlobalClock.hpp
{
public:
  static VOID CALLBACK alarmHandler(HWND hwnd,UINT uMsg,UINT idEvent,DWORD dwTime);
  static DWORD WINAPI eventLoopFun(LPVOID lpParameter);
};
#endif

GlobalClock::InitStatic::InitStatic()
{
  if (!_count)
    {
      GlobalClock::_currentAlarmHandler = 0;
      GlobalClock::_elapsedDeciseconds = 0;

#if (BK_TARGET_OPERATIONAL_ENVIRONMENT_TYPE == BK_TARGET_OPERATIONAL_ENVIRONMENT_TYPE_UNIX)
      static struct itimerval timerSettings;
      timerSettings.it_interval.tv_sec = 0;
      timerSettings.it_interval.tv_usec = 100000; // 1 decisecond
      timerSettings.it_value.tv_sec = 0;
      timerSettings.it_value.tv_usec = 100000; // 1 decisecond
      signal(SIGVTALRM,GlobalClock::alarmSignalHandler);
      setitimer(ITIMER_VIRTUAL,&timerSettings,0);
#elif (BK_TARGET_OPERATIONAL_ENVIRONMENT_TYPE == BK_TARGET_OPERATIONAL_ENVIRONMENT_TYPE_MSWIN32)
	static DWORD eventLoopThreadId;
      HANDLE eventLoop = 
	CreateThread(NULL,
		     64000,
			 &GlobalClock::_Private::eventLoopFun,
		     NULL,
		     0,
             &eventLoopThreadId);
	if (eventLoop == NULL) 
	  {
	    RuntimeError::report("Can not spawn thread in GlobalClock::InitStatic::InitStatic().");
	  };

#endif
    };
  ++_count;
}; //GlobalClock::InitStatic::InitStatic()

GlobalClock::InitStatic::~InitStatic()
{
  --_count;
  if (!_count)
    {
		  
    };
}; // GlobalClock::InitStatic::~InitStatic()

#if (BK_TARGET_OPERATIONAL_ENVIRONMENT_TYPE == BK_TARGET_OPERATIONAL_ENVIRONMENT_TYPE_UNIX)

void GlobalClock::alarmSignalHandler(int)
{
  signal(SIGVTALRM,GlobalClock::alarmSignalHandler);
  ++_elapsedDeciseconds;
  if (_currentAlarmHandler)
    {
      if (_currentAlarmHandler->handlingFunction)
	{
	  _currentAlarmHandler->handlingFunction(_elapsedDeciseconds);
	};
    };
}; // void GlobalClock::alarmSignalHandler(int)

#elif (BK_TARGET_OPERATIONAL_ENVIRONMENT_TYPE == BK_TARGET_OPERATIONAL_ENVIRONMENT_TYPE_MSWIN32)

VOID CALLBACK GlobalClock::_Private::alarmHandler(HWND hwnd,UINT uMsg,UINT idEvent,DWORD dwTime) 
{
  ++_elapsedDeciseconds;
  if (_currentAlarmHandler)
    {
      if (_currentAlarmHandler->handlingFunction)
	{
	  _currentAlarmHandler->handlingFunction(_elapsedDeciseconds);
	};
    };
}; // VOID CALLBACK GlobalClock::_Private::alarmHandler(HWND hwnd,UINT uMsg,UINT idEvent,DWORD dwTime) 


DWORD WINAPI GlobalClock::_Private::eventLoopFun(LPVOID lpParameter)
{
  SetTimer(NULL,0,100U,&alarmHandler);
  MSG msg;
  while (true)
    {	  
      BOOL cont = 
	GetMessage(&msg,NULL,0,0);
      if (cont == 0) return 0;
      if (cont == -1)
	RuntimeError::report("GetMessage(&msg,NULL,0,0) = -1 in GlobalClock::eventLoopFun(LPVOID lpParameter).");
      TranslateMessage( &msg );
      // Use if (msg.message == WM_TIMER) ... here,
      // if some addition action is needed on WM_TIMER message
      DispatchMessage( &msg );
    };
  return 0;
}; // DWORD WINAPI GlobalClock::_Private::eventLoopFun(LPVOID lpParameter)
 


#endif
//============================================================================
