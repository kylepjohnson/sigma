//
// File:         Logging.cpp
// Description:  Various utilities for logging data.
// Created:      Mar 26, 2002, Alexandre Riazanov, riazanov@cs.man.ac.uk
//======================================================================
#include "Logging.hpp"
//======================================================================
using namespace VK;
const long* Logging::Integer32::_basePointer = 0;
bool Logging::Integer32::_basePointerSet = false;

//======================================================================
