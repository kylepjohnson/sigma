//
// File:         term.cpp
// Description:  Representation of stored terms.
// Created:      Oct 15, 1999, 15:00
// Author:       Alexandre RiazanovStdTerm(const TERM* term,const Signature* sig) : _term(term), _sig(sig) {}; 
// mail:         riazanov@cs.man.ac.uk
//====================================================
#include "Term.hpp"
#include "Signature.hpp"
using namespace BK;
using namespace VK;
//================================================
#undef DEBUG_NAMESPACE
#ifdef DEBUG_TERM
#define DEBUG_NAMESPACE "TERM"
#endif
#include "debugMacros.hpp"
//=================================================


long TERM::InitStatic::_count; // Implicitely initialised to 0L

const TERM::Polarity TERM::PositivePolarity = 0UL;
const TERM::Polarity TERM::NegativePolarity = 1UL; 

#ifdef DEBUG_ALLOC_OBJ_TYPE
ClassDesc TERM::OccList::_classDesc("TERM::OccList",
				    TERM::OccList::minSize(),
				    TERM::OccList::maxSize());
 
ClassDesc TERM::OccList::_headerClassDesc("TERM::OccList(header)",
					  TERM::OccList::headerMinSize(),
					  TERM::OccList::headerMaxSize());

ClassDesc TERM::OccList::Node::_classDesc("TERM::OccList::Node",
					  TERM::OccList::Node::minSize(),
					  TERM::OccList::Node::maxSize());
#endif




TERM TERM::_dummyPositiveHeader0;
TERM TERM::_dummyPositiveHeader1;
TERM TERM::_positiveOrdEqHeader;
TERM TERM::_negativeOrdEqHeader;
TERM TERM::_positiveUnordEqHeader;
TERM TERM::_negativeUnordEqHeader;
TERM TERM::_builtInTrue;
TERM TERM::_builtInFalse;


bool TERM::IsDefHeader() const // all arguments are different variables
{
  CALL("IsDefHeader()");
  ASSERT(isComplex());
  for (const TERM* ar1 = Args(); !ar1->IsNull(); ar1 = ar1->Next())
    {
      if (!ar1->isVariable()) return false; 
      for (const TERM* ar2 = Args(); ar2 != ar1; ar2 = ar2->Next())     
	if (*ar2 == *ar1) return false;
    };
  return true;
}; // bool TERM::IsDefHeader() const
  
bool TERM::Symmetry(const TERM* t1,const TERM* t2)
{
  CALL("Symmetry(const TERM* t1,const TERM* t2)");
  ASSERT(t1->isComplex() && t2->isComplex());
  if (t1->functor() != t2->functor()) return false;    
  if (t1->arity() != 2) return false;
  if ((!t1->IsDefHeader()) || (!t2->IsDefHeader())) return false;
  if ((*(t1->Arg1()) != *(t2->Arg2()))
      || (*(t1->Arg2()) != *(t2->Arg1()))) return false; 
  return true;
}; // bool TERM::Symmetry(const TERM* t1,const TERM* t2)


ulong TERM::size() const
{
  CALL("size() const");
  ulong res = 1;
  if (!isVariable())
    {
      Iterator iter(this);
      while (iter.Next()) res++;
    };
  return res;
}; // ulong TERM::size() const

ulong TERM::depth() const
{
  CALL("depth() const");
  ulong res = 1UL;
  IteratorWithDepth iter;
  iter.Reset(this,1UL);
  while (iter.Next()) 
    if (iter.Depth() > res) res = iter.Depth();
  return res;
}; // ulong TERM::depth() const


TERM TERM::unsharedCopy() const
{
  CALL("unsharedCopy()"); 
  if (isVariable()) return (*this);
  ASSERT(IsReference());
  const TERM* t = First();
  ulong arity = t->arity();

  TERM* nt = new (arity) TERM((TERM::Arity)arity,(TERM::Functor)t->functor(),t->polarity()); 

  for (ulong i = 0; i < arity; i++)
    {

      *(nt->NthArg(i)) = t->NthArg(i)->unsharedCopy();
    };
  
  return TERM(nt);
}; // TERM TERM::unsharedCopy() const
 
TERM* TERM::unsharedLitCopy() const
{
  CALL("unsharedLitCopy() const");
  TERM* res = AllocLit(*this);   
  for (ulong i = 0; i < arity(); i++)
    {
      *(res->NthArg(i)) = NthArg(i)->unsharedCopy();
    };
  ASSERT(EqualComplexTerms(res,this));
  
  return res;
}; // TERM* TERM::unsharedLitCopy() const     

void TERM::destroyUnsharedComplex()
{
  CALL("destroyUnsharedComplex()");

  ASSERT(isComplex());
  for (TERM* argRef = Args(); !argRef->IsNull(); argRef = argRef->Next())
    if (argRef->IsReference()) 
      argRef->First()->destroyUnsharedComplex();
  delete this;
}; //void TERM::destroyUnsharedComplex()

void TERM::destroyUnsharedLit()
{
  CALL("destroyUnsharedLit()");

  ASSERT(isComplex());      
  for (TERM* argRef = Args(); !argRef->IsNull(); argRef = argRef->Next())
    {
      if (argRef->IsReference()) argRef->First()->destroyUnsharedComplex();
    };
  DeleteLit();
}; // void TERM::destroyUnsharedLit()


void TERM::Destroy(TERM* t)
{
  if (t->arity())
    {
      for (TERM* arg = t->Args(); !t->IsNull(); t = t->Next())
	if (arg->IsReference()) Destroy(arg->First());
      delete t;
    };
}; // void TERM::Destroy(TERM* t)



ostream& TERM::output(ostream& str,const TERM& t,Signature* sig) 
{ 
  CALL("output(ostream& str,const TERM& t,Signature* sig)");      
  if (t.isVariable()) { return str << 'X' << t.var(); }
  else
    if (t.isComplex())
      {
        bool isAnswerPred = sig->isAnswerPredicate(t.functor()); 
	if (t.IsSplitting()) str << "[";
        if (isAnswerPred) str << "<<";
	if (t.Negative()) str << ((t.IsEquality()) ? '!' : '~');
	if (t.IsEquality()) 
	  if (t.IsOrderedEq()) { return str << "=="; }    
	  else return str << '=';
	// not equality  
	if (t.isBuiltInTruthValue())
	  {
	    return str << "**true**";
	  };
	sig->outputSymbol(str,t.functor());
        if (isAnswerPred) str << ">>";
	if (t.IsSplitting()) str << "]";
	return str;
      }
    else // t.IsReference()
      { 
	return output(str,t.First(),sig);
      };         
}; // ostream& TERM::output(ostream& str,const TERM& t,Signature* sig) 


ostream& TERM::output(ostream& str,const TERM* t,Signature* sig) 
{   
  CALL("output(ostream& str,const TERM* t,Signature* sig)");
  if (t->IsEquality())
    { 
      output(str,*(t->Args()),sig);
      output(str,*t,sig);
      return output(str,*(t->Args()->Next()),sig);   
    };
  // !t->IsEquality()
  output(str,*t,sig);


  if (!t->arity()) return str; 
  str << '(';
  for (const TERM* arg = t->Args(); !arg->IsNull(); arg = arg->Next())
    { 
      output(str,*arg,sig);
      if (!arg->Next()->IsNull()) str << ',';
    };
  return str << ')';
}; //ostream& TERM::output(ostream& str,const TERM* t,Signature* sig)  



ostream& TERM::outputInTPTPFormat(ostream& str) const
{
  CALL("outputInTPTPFormat(ostream& str) const");
  if (isVariable()) { return str << 'X' << var(); }
  else
    if (isComplex())
      {
	if (IsEquality()) 
	  {
	    str << "equal";
	  }
	else 
	  if (isBuiltInTruthValue())
	    {
	      str << "vampire_kernel_built_in_true";
	    }
	  else
	    {
	      Signature::current()->outputSymbol(str,functor());
	    };
	if (!arity()) return str;
	str << '(';
	for (const TERM* arg = Args(); !arg->IsNull(); arg = arg->Next())
	  { 
	    arg->outputInTPTPFormat(str);
	    if (!arg->Next()->IsNull()) str << ',';
	  };
	return str << ')';
      }
    else // IsReference() 
      return First()->outputInTPTPFormat(str);      
}; // ostream& TERM::outputInTPTPFormat(ostream& str) const

ostream& TERM::outputAsPrologTerm(ostream& str) const
{
  CALL("TERM::outputAsPrologTerm(ostream& str) const");
  if (isVariable()) { return str << "\'X" << var() << '\''; }
  else 
    if (isComplex())
      {
	if (IsEquality()) 
	  { 
	    str << '('; 
	    Arg1()->outputAsPrologTerm(str);
	    if (IsOrderedEq())
	      {
		str << " => ";
	      }
	    else str << " = "; 
	    Arg2()->outputAsPrologTerm(str);
	    str << ')';
	    return str;
	  }
	else 
	  if (isBuiltInTruthValue())
	    {
	      str << "vampire_kernel_built_in_true";
	    }
	  else
	    {
	      Signature::current()->outputSymbol(str,functor());
	    };
	if (!arity()) return str;
	str << '(';
	for (const TERM* arg = Args(); !arg->IsNull(); arg = arg->Next())
	  { 
	    arg->outputAsPrologTerm(str);
	    if (!arg->Next()->IsNull()) str << ',';
	  };
	return str << ')';
      }
    else // IsReference() 
      return First()->outputAsPrologTerm(str);  
}; // ostream& TERM::outputAsPrologTerm(ostream& str) const

void TERM::initStatic()
{    
  CALL("initStatic()");
  _dummyPositiveHeader0.MakeComplex((TERM::Arity)0,(TERM::Functor)VampireKernelConst::ReservedPred0,TERM::PositivePolarity);
  _dummyPositiveHeader1.MakeComplex((TERM::Arity)1,(TERM::Functor)VampireKernelConst::ReservedPred1,TERM::PositivePolarity);
  _positiveOrdEqHeader.MakeComplex((TERM::Arity)2,(TERM::Functor)VampireKernelConst::OrdEqNum,TERM::PositivePolarity);
  _negativeOrdEqHeader.MakeComplex((TERM::Arity)2,(TERM::Functor)VampireKernelConst::OrdEqNum,TERM::NegativePolarity);
  _positiveUnordEqHeader.MakeComplex((TERM::Arity)2,(TERM::Functor)VampireKernelConst::UnordEqNum,TERM::PositivePolarity);
  _negativeUnordEqHeader.MakeComplex((TERM::Arity)2,(TERM::Functor)VampireKernelConst::UnordEqNum,TERM::NegativePolarity);
  _builtInTrue.MakeComplex((TERM::Arity)0,(TERM::Functor)VampireKernelConst::BuiltInTrueNum,TERM::PositivePolarity);
  _builtInFalse.MakeComplex((TERM::Arity)0,(TERM::Functor)VampireKernelConst::BuiltInTrueNum,TERM::NegativePolarity);
  
}; // void TERM::initStatic()


//====================================================
