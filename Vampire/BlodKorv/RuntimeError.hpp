//
// File:         RuntimeError.hpp
// Description:  
// Created:      Oct 31, 2000, 18:05
// Author:       Alexandre Riazanov
// mail:         riazanov@cs.man.ac.uk
//
#ifndef RUNTIME_ERROR_H 
//======================================================================
#define RUNTIME_ERROR_H
#include "jargon.hpp"
//=====================================================================


namespace BK 
{

class RuntimeError
{
 public:
  static void report(const char* msg) 
    { 
      if (errorHook) 
	{
	  void (*tmpErrorHook)();
	  tmpErrorHook = errorHook;
	  errorHook = 0;
	  tmpErrorHook();
	};
      reportFun(msg); 
    };
  static void defaultReport(const char* msg);
  static void report(const char* msg1,const char* msg2) 
    {  
      if (errorHook) 
	{
	  void (*tmpErrorHook)();
	  tmpErrorHook = errorHook;
	  errorHook = 0;
	  tmpErrorHook();
	};
      reportFun2(msg1,msg2); 
    };
  static void defaultReport2(const char* msg1,const char* msg2);
  static void report(const char* msg1,const char* msg2,const char* msg3) 
    { 
      if (errorHook) 
	{
	  void (*tmpErrorHook)();
	  tmpErrorHook = errorHook;
	  errorHook = 0;
	  tmpErrorHook();
	};
      reportFun3(msg1,msg2,msg3); 
    };
  static void defaultReport3(const char* msg1,const char* msg2,const char* msg3);
  
  static void (*reportFun)(const char* msg);
  static void (*reportFun2)(const char* msg1,const char* msg2);
  static void (*reportFun3)(const char* msg1,const char* msg2,const char* msg3);
  static void (*errorHook)();
};

}; // namespace BK

//===================================================================
#endif
