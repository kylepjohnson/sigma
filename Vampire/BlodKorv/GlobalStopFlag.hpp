//
// File:         GlobalStopFlag.hpp
// Description:  Soft interrupt.
// Created:      Jun 12, 2002, Alexandre Riazanov, riazanov@cs.man.ac.uk 
//============================================================================
#ifndef GLOBAL_STOP_FLAG_H
#define GLOBAL_STOP_FLAG_H
//============================================================================
#include "jargon.hpp"
#include "BlodKorvDebugFlags.hpp"
#include "DestructionMode.hpp"
//============================================================================
#undef DEBUG_NAMESPACE
#ifdef DEBUG_GLOBAL_STOP_FLAG
 #define DEBUG_NAMESPACE "GlobalStopFlag"
#endif
#include "debugMacros.hpp"
//============================================================================

namespace BK 
{

class GlobalStopFlag
{
 public:
  class DefaultException 
    {
    public:
      DefaultException() {};
    };
  class HandlerActivator
    {
    public:
      HandlerActivator(void (*handler)())  : _handler(handler) 
	{
	  _previous = GlobalStopFlag::_currentHandlerActivator;
	  GlobalStopFlag::_currentHandlerActivator = this;
	};
      ~HandlerActivator()
	{
	  CALL("destructor ~HandlerActivator()");
	  if (DestructionMode::isThorough())
	    {
	      ASSERT(GlobalStopFlag::_currentHandlerActivator == this);
	      GlobalStopFlag::_currentHandlerActivator = _previous;
	    };
	};
    private:
      HandlerActivator() {};
      void handle();
    private:
      void (*_handler)();
      HandlerActivator* _previous;
      friend class GlobalStopFlag;
    };
 public:
  static void reset() { _stop = false; };
  static void raise() { _stop = true; };
  static void check()
    {
      CALL("check()");
      if (_stop)
	{
	  _currentHandlerActivator->handle();
	};
    };
 private:
  static bool _stop;
  static HandlerActivator* _currentHandlerActivator;
  friend class HandlerActivator;
}; // class GlobalStopFlag


}; // namespace BK


//============================================================================
#endif
