//
// File:         RuntimeError.cpp
// Description:  
// Created:      Oct 31, 2000, 18:10
// Author:       Alexandre Riazanov
// mail:         riazanov@cs.man.ac.uk
//
//=====================================================================
#include <iostream>
#include "RuntimeError.hpp"
#include "Exit.hpp"
//=====================================================================
using namespace std;
using namespace BK;
void (*RuntimeError::reportFun)(const char* msg) = RuntimeError::defaultReport;
void (*RuntimeError::reportFun2)(const char* msg1,const char* msg2) = RuntimeError::defaultReport2;
void (*RuntimeError::reportFun3)(const char* msg1,const char* msg2,const char* msg3) = RuntimeError::defaultReport3;
void (*RuntimeError::errorHook)() = 0;

void RuntimeError::defaultReport(const char* msg)
{
  cout << "Runtime error: " << msg << "\n";
  Exit::exit(1);
};

void RuntimeError::defaultReport2(const char* msg1,const char* msg2)
{
  cout << "Runtime error: " << msg1 << msg2 << "\n";
  Exit::exit(1);
};


void RuntimeError::defaultReport3(const char* msg1,const char* msg2,const char* msg3)
{
  cout << "Runtime error: " << msg1 << msg2 << msg3 << "\n";
  Exit::exit(1);
};



//=====================================================================
