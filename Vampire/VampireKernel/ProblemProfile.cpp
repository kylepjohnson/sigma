//
// File:         ProblemProfile.cpp
// Description:  Analysing simple problem characteristics.
// Created:      Apr 15, 2000, 19:00
// Author:       Alexandre Riazanov                        
// mail:         riazanov@cs.man.ac.uk
// Revised:      Dec 26, 2001.
//               Freed from HellConst::MaxNumOfFunctors.
// Revised:      Dec 27, 2001. Slightly reimplemented.
//====================================================
#include "ProblemProfile.hpp"
//================================================
#undef DEBUG_NAMESPACE
#ifdef DEBUG_PROBLEM_PROFILE
#define DEBUG_NAMESPACE "ProblemProfile"
#endif
#include "debugMacros.hpp"
//=================================================
using namespace VK;

ProblemProfile::ProblemProfile() 
  :
#ifdef NO_DEBUG
  _predInfoTable(PredInfo(),(int)0),
  _funcInfoTable(FuncInfo(),(int)0)
#else
  _predInfoTable(PredInfo(),"ProblemProfile::_predInfoTable"),
  _funcInfoTable(FuncInfo(), "ProblemProfile::_funcInfoTable")
#endif
{
  CALL("constructor ProblemProfile()");
  DOP(_predInfoTable.freeze());
  DOP(_funcInfoTable.freeze());
  reset(); 
};

ProblemProfile::~ProblemProfile() { CALL("destructor ~ProblemProfile()"); };

void ProblemProfile::init()
{
  CALL("init()");
  _clauses.init();
    
#ifdef NO_DEBUG
  _predInfoTable.init(PredInfo(),(int)0);
  _funcInfoTable.init(FuncInfo(),(int)0);
#else
  _predInfoTable.init(PredInfo(),"ProblemProfile::_predInfoTable");
  _funcInfoTable.init(FuncInfo(), "ProblemProfile::_funcInfoTable");
#endif
  _symbolInfoTable.init();

  DOP(_predInfoTable.freeze());
  DOP(_funcInfoTable.freeze());
  reset(); 
}; // void ProblemProfile::init()


void ProblemProfile::destroy()
{
  CALL("destroy()");
  _symbolInfoTable.destroy();
  _funcInfoTable.destroy();
  _predInfoTable.destroy();
  _clauses.destroy();
}; // void ProblemProfile::destroy()


void ProblemProfile::reset()
{
  CALL("reset()");
  _clauses.reset();
 
  _numOfClauses = 0;
  _numOfNonhorn = 0;
  _numOfUnit = 0;
  _numOfLiterals = 0;
  _numOfEqLiterals = 0;
  _numOfPred = 0;
  _totalPred = 0;
  _numOfFuncs = 0;
 
  for (ulong f = 0; f < _funcInfoTable.size(); f++) _funcInfoTable[f].init();
  for (ulong p = 0; p < _predInfoTable.size(); p++) _predInfoTable[p].init();
  _symbolInfoTable.reset();
}; // void ProblemProfile::reset()
 
void ProblemProfile::load(const Clause* cl) 
{
  CALL("load(const Clause* cl)"); 


  _clauses.enqueue(cl);
 
  // general statistics
  _numOfClauses++;
  if (!cl->HornClause()) _numOfNonhorn++;
  if (cl->Unit()) _numOfUnit++;
  _numOfLiterals += cl->numOfAllLiterals();
  _numOfEqLiterals += cl->NumOfEqLits();   

  // collecting information on predicates 

  if (cl->SymmetryLaw())
    {
      _symbolInfoTable.registerSymmetry(cl->LitList()->firstLit()->functor(),cl);
      predInfo(cl->LitList()->firstLit()->functor())._symmetric = true;
    }
  else
    if (cl->TotalityLaw())
      {
	predInfo(cl->LitList()->firstLit()->functor())._total = true;
      };

  for(LiteralList::Iterator iter1(cl->LitList());iter1.notEnd();iter1.next())
    {
      const TERM* lit1 = iter1.currentLiteral();
      ulong pred1 = lit1->functor(); 
      predInfo(pred1).regPred(*lit1);

      if (predInfo(pred1)._status == PredInfo::CandidateForElimination) 
	{
	  if (lit1->arity() && (!lit1->IsEquality()) && (!lit1->isAnswerLiteral()))    
	    {
	      if (lit1->IsDefHeader())
		{
		  if (!predInfo(pred1).isNondef(*lit1))
		    {
		      // might be header of a definition
    
		      for(LiteralList::Iterator iter2(cl->LitList());iter2.notEnd();iter2.next())
			if ((iter1.currentElement() != iter2.currentElement()) && (pred1 == iter2.currentLiteral()->functor())) 
			  {
			    predInfo(pred1).nondef(*lit1);
			    predInfo(pred1).nondef(*(iter2.currentLiteral()));
			  };            
		    };  
		}
	      else // not a definition head
		{
		  predInfo(pred1).nondef(*lit1);                     
		  for(LiteralList::Iterator iter2(cl->LitList());iter2.notEnd();iter2.next())
		    if ((iter1.currentElement() != iter2.currentElement()) && 
			(pred1 == iter2.currentLiteral()->functor())) 
		      {
			predInfo(pred1)._status = PredInfo::CanNotBeEliminated; 
		      };
		};     
	    }
	  else // propositional or equality, or answer literal
	    {
	      predInfo(pred1)._status = PredInfo::CanNotBeEliminated; 
	    };       
	}
      else // some status established, must be CanNotBeEliminated
	{
	  ASSERT(predInfo(pred1)._status == PredInfo::CanNotBeEliminated);
	};
    };               


  // collecting information on functors
  ulong commFunc;
  if (cl->CommutativityLaw(commFunc))
    {
      _symbolInfoTable.registerCommutativity(commFunc,cl);
      funcInfo(commFunc)._commutative = true;
    };
  TERM::Iterator iter;
  for(LiteralList::Iterator litIter(cl->LitList());litIter.notEnd();litIter.next())
    {
      if (litIter.currentLiteral()->arity())
	{
	  iter.Reset(litIter.currentLiteral());
	  ALWAYS(iter.Next());
	  do
	    {     
	      if (!iter.CurrentSymIsVar()) 
		{
		  funcInfo(iter.symbolRef().functor())._registered++; 
		  funcInfo(iter.symbolRef().functor())._symbol = iter.symbolRef();
		};
	    }
	  while (iter.Next());
	};
    };


}; // void ProblemProfile::load(const Clause* cl) 

void ProblemProfile::summarise()
{
  CALL("summarise()");
  for (const ConstClauseQueue::Entry* qe = _clauses.begin(); qe; qe = qe->next())
    removeEliminationClashes(qe->value());

  for (ulong p = 0; p < _predInfoTable.size(); p++) 
    {                
      if ((predInfo(p)._registered) && (!predInfo(p)._symbol.IsEquality()))
	{
	  _numOfPred++;
	  if (predInfo(p)._status == PredInfo::CandidateForElimination)
	    { 
	      _symbolInfoTable.registerEliminated(p);
	    };
	  if (predInfo(p)._total) _totalPred++;
	};
    };  
}; // void ProblemProfile::summarise()
   
void ProblemProfile::removeEliminationClashes(const Clause* cl)
{
  CALL("removeEliminationClashes(const Clause* cl)");

  for(LiteralList::Iterator iter1(cl->LitList()); iter1.notEnd(); iter1.next())
    {
      ulong pred1 = iter1.currentLiteral()->functor();
      if (predInfo(pred1)._status == PredInfo::CandidateForElimination)
	{
	  for(LiteralList::Iterator iter2(cl->LitList(),iter1.currentElement()->next()); 
	      iter2.notEnd(); 
	      iter2.next())    
	    {
	      ulong pred2 = iter2.currentLiteral()->functor();
	      if (predInfo(pred2)._status == PredInfo::CandidateForElimination) 
		{
		  // elimination clash must be resolved
		  if (predInfo(pred1)._registered >= predInfo(pred2)._registered)
		    {
		      // clash resolved in favour of pred1
		      predInfo(pred2)._status = PredInfo::CanNotBeEliminated;
		    }
		  else
		    {
		      // clash resolved in favour of pred2
		      predInfo(pred1)._status = PredInfo::CanNotBeEliminated;
		      pred1 = pred2;
		    };
		};
	    };
	  return;
	};
    };

}; // void ProblemProfile::removeEliminationClashes(const Clause* cl)



ostream& ProblemProfile::output(ostream& str) const
{
  str << "% Category: " << spell(category()) << '\n';
  str << "% Number of non-equality predicates: " << _numOfPred << '\n';

  if (_symbolInfoTable.numOfSymmetric()) 
    str << "% Number of symmetric predicates: " 
	<< _symbolInfoTable.numOfSymmetric()  << '\n';
  if (_totalPred) str << "% Number of total predicates: " << _totalPred  << '\n';  
  if (_symbolInfoTable.numOfEliminated()) 
    str << "% Number of eliminated predicates: " 
	<< _symbolInfoTable.numOfEliminated() << '\n'; 
       
  str << "% Predicates: {";           
  for (ulong p = 0; p < _predInfoTable.size(); p++)
    if ((_predInfoTable[p]._registered) && (!_predInfoTable[p]._symbol.IsEquality()))
      _predInfoTable[p].output(str << '[') << "] ";   
  str << "}\n"; 

  str << "% Number of functors: " << _numOfFuncs << '\n';
  if (_symbolInfoTable.numOfCommutative())
    str << "% Number of commutative functors: " 
	<< _symbolInfoTable.numOfCommutative() << '\n';

  str << "% Functors: {";
  for (ulong f = 0; f < _funcInfoTable.size(); f++)
    if (_funcInfoTable[f]._registered) _funcInfoTable[f].output(str << '[') << "] ";
  str << "}\n"; 
  return str;
}; // ostream& ProblemProfile::output(ostream& str) const



//=================================================
