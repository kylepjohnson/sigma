//
// File:         Exit.cpp
// Description:  
// Created:      Apr 11, 2000, 16:00
// Author:       Alexandre Riazanov
// mail:         riazanov@cs.man.ac.uk
//====================================================
#include "Exit.hpp"
#include "DestructionMode.hpp"
//====================================================
using namespace BK;

void Exit::exit(int code)
{
  DestructionMode::makeFast(); // thorough would not be safe with ::exit
  ::exit(code);
}; 



//=================================================
