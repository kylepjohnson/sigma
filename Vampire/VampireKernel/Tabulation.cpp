//
// File:         Tabulation.cpp
// Description:  
// Created:      Sep 09, 2002, Alexandre Riazanov, riazanov@cs.man.ac.uk
//=======================================================
#include "Clause.hpp"
#include "Tabulation.hpp"
//=======================================================
using namespace VK;

void Tabulation::proofClause(const Clause* cl) 
{
  *_str << "vproof("; 
  quoted_atom(_jobId); 
  comma(); 
  //number(_kernelSessionId);
  //comma(); 
  cl->outputAsPrologTerm(*_str); 
  *_str << ").\n"; 
  flush(); 
}; // void Tabulation::proofClause(const Clause* cl) 


//=======================================================

