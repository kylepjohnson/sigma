//
// File:         DestructionMode.cpp
// Description:  Global flag, regulating destruction. 
// Created:      May 23, 2002, Alexandre Riazanov, riazanov@cs.man.ac.uk
//====================================================
#include "DestructionMode.hpp"
//====================================================
using namespace BK;

long DestructionMode::InitStatic::_count; // implicitely initialised to 0L
bool DestructionMode::_isThorough;

//====================================================
