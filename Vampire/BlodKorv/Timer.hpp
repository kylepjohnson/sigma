//
// File:         Timer.hpp
// Description:  Interface for "clock".
// Created:      Feb 16, 2000, 15:20
// Author:       Alexandre Riazanov
// mail:         riazanov@cs.man.ac.uk
//============================================================================
#ifndef TIMER_H
//=============================================================================
#define TIMER_H 
#include <ctime>
#include "jargon.hpp"
#include "BlodKorvDebugFlags.hpp"
//============================================================================
#undef DEBUG_NAMESPACE
#ifdef DEBUG_TIMER
 #define DEBUG_NAMESPACE "Timer"
#endif
#include "debugMacros.hpp"
//============================================================================
 
namespace BK 
{

class Timer
{
 public:
  Timer() 
  {
   CALL("constructor Timer()"); 
   reset(); 
  };
  ~Timer() 
  {
    CALL("destructor ~Timer()");
    BK_CORRUPT(*this);
  }; 
  void reset()
  {
   stopped = true;
   elapsedSec = 0;
  };
  void start()
  {
   if (stopped)   
    {
     stopped = false;
     startTime = clock();     
    };
  };
  void stop()
  {
   if (!stopped)
    {
     stopped = true;
     clock_t currTime = clock();
     if (currTime > startTime)      
       elapsedSec += (((float)(currTime - startTime))/CLOCKS_PER_SEC);
    };
  };

    
  float elapsed()
  {
   if (stopped) return elapsedSec; 
   clock_t currTime = clock();     
   if (currTime > startTime) 
    return elapsedSec + (((float)(currTime - startTime))/CLOCKS_PER_SEC); 
   return elapsedSec;
  };
 private:   
  bool stopped;
  float elapsedSec;
  clock_t startTime;
}; // class Timer

}; // namespace BK
//======================================================================
#endif
