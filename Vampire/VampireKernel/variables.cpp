
//
// File:         variables.cpp
// Description:  Various classes for dealing with variables and substitutions.
// Created:      Feb 15, 2000, 21:10
// Author:       Alexandre Riazanov
// mail:         riazanov@cs.man.ac.uk
//====================================================
#include "variables.hpp"
//================================================
#undef DEBUG_NAMESPACE
#ifdef DEBUG_VARIABLES
 #define DEBUG_NAMESPACE "Variable"
#endif
#include "debugMacros.hpp"
//=================================================

using namespace VK;


long Unifier::InitStatic::_count; // implicitely initialised to 0L
Unifier* Unifier::_current;
ulong Unifier::_offsetForIndex[VampireKernelConst::MaxNumOfIndices];

ostream& Unifier::Variable::output(ostream& str) const
{
  ulong num;
  ulong index;
  NumAndIndex(num,index);    
  return str << 'X' << num << '{' << index << '}';
}; //  ostream& Unifier::Variable::output(ostream& str) const
    
#ifndef NO_DEBUG_VIS
ostream& Unifier::Variable::outputBinding(ostream& str,ostream& (*outputTerm)(ostream& str,const PrefixSym* t)) const
{ 
  output(str) << " := ";
  if (!Binding()) return str << "NULL"; 
  if (Unifier::current()->isVariable(Binding()))
    return (static_cast<const Unifier::Variable*>(Binding()))->output(str);
  return outputTerm(str,static_cast<const PrefixSym*>(Binding()));
}; // ostream& Unifier::Variable::outputBinding(ostream& str,ostream& (*outputTerm)(ostream& str,const PrefixSym* t)) const
#endif

//================================================
#undef DEBUG_NAMESPACE
#ifdef DEBUG_VARIABLES
 #define DEBUG_NAMESPACE "Unifier"
#endif
#include "debugMacros.hpp"
//=================================================



Unifier::Unifier() 
{
  CALL("constructor Unifier()");
  
  next = bound;
  next_section = section;
	       
  for (ulong v = 0UL; 
       v < VampireKernelConst::MaxNumOfIndices*VampireKernelConst::MaxNumOfVariables;
       ++v)
    {
      _variableBank[v].init();
    };
  _endOfVariableBank = 
    _variableBank + 
    VampireKernelConst::MaxNumOfIndices*VampireKernelConst::MaxNumOfVariables;
}; // Unifier::Unifier() 


Unifier::~Unifier() { CALL("destructor ~Unifier()"); };

void Unifier::init() 
{
  CALL("init()");
  next = bound;
  next_section = section;
	       
  for (ulong v = 0UL; 
       v < VampireKernelConst::MaxNumOfIndices*VampireKernelConst::MaxNumOfVariables;
       ++v)
    {
      _variableBank[v].init();
    };
  _endOfVariableBank = 
    _variableBank + 
    VampireKernelConst::MaxNumOfIndices*VampireKernelConst::MaxNumOfVariables;
}; // void Unifier::init() 


void Unifier::destroy()
{
  CALL("destroy()");
  for (ulong v = 0UL; 
       v < VampireKernelConst::MaxNumOfIndices*VampireKernelConst::MaxNumOfVariables;
       ++v)
    {
      _variableBank[v].destroy();
    };
}; // void Unifier::destroy()



#ifndef NO_DEBUG_VIS
ostream& Unifier::output(ostream& str,ostream& (*outputTerm)(ostream& str,const PrefixSym* t)) const
{ 
  for(const Unifier::Variable* const * p = bound; p<next; p++)
    (*p)->outputBinding(str,outputTerm) << '\n'; 
  return str;        
}; // ostream& Unifier::output(ostream& str,ostream& (*outputTerm)(ostream& str,const PrefixSym* t)) const
#endif


//================================================
#undef DEBUG_NAMESPACE
#ifdef DEBUG_VARIABLES
 #define DEBUG_NAMESPACE "VAR_TO_INT_RENAMING"
#endif
#include "debugMacros.hpp"
//=================================================


#ifndef NO_DEBUG_VIS
ostream& VAR_TO_INT_RENAMING::output(ostream& str) const
{
  str << "VAR_TO_INT_RENAMING:\n";
  for (const Unifier::Variable* const * r = renamed; r < next; r++)
    (*r)->output(str) << " -> " << (ulong)(*(renaming.Entry(*r))) << '\n';
  return str << "END OF VAR_TO_INT_RENAMING.\n";
}; // ostream& VAR_TO_INT_RENAMING::output(ostream& str) const
    
#endif


//================================================
#undef DEBUG_NAMESPACE
#ifdef DEBUG_VARIABLES
 #define DEBUG_NAMESPACE "INT_VAR_RENAMING"
#endif
#include "debugMacros.hpp"
//=================================================



#ifndef NO_DEBUG_VIS
ostream& INT_VAR_RENAMING::output(ostream& str) const
{
  str << "VARIABLE RENAMING:\n";
  for (const ulong* r = renamed; r < next; r++)
    str << 'X' << *r << " -> " << 'X' << renaming[*r] << '\n';
  return str << "END OF VARIABLE RENAMING.\n";
}; // ostream& INT_VAR_RENAMING::output(ostream& str) const
#endif




//====================================================
