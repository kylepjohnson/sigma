//
// File:         Splitting3.cpp
// Description:  Core of splitting without backtracking
//               For DISCOUNT loop (--main_alg 3).
// Created:      May 16, 2002, Alexandre Riazanov, riazanov@cs.man.ac.uk
// Note:         Was part of Splitting.cpp.
//============================================================================
#include "Splitting.hpp"
#include "ForwardSubsumptionFrontEnd.hpp"
#include "Term.hpp"
#include "ComponentName.hpp"
#include "Clause.hpp"
#include "PagedPassiveClauseStorage.hpp"
#include "ClauseAssembler.hpp"
#include "Signature.hpp"
//============================================================================
#undef DEBUG_NAMESPACE
#ifdef DEBUG_SPLITTING
 #define DEBUG_NAMESPACE "Splitting"
#endif
#include "debugMacros.hpp"
//============================================================================
using namespace VK;


bool Splitting::BlockingSplitting3(TmpLitList& lits,                            
                                   Clause*& answerClause,
                                   PagedPassiveClauseStorage& storage,
                                   ulong& numOfNewClauses,
                                   ulong& numOfRejectedNewClauses,
                                   Clause::NewNumber* clauseNumberGenerator,
                                   bool showNewClauses)
{
 CALL("BlockingSplitting3(TmpLitList& lits,Clause*& answerClause,PagedPassiveClauseStorage& storage,..)");

 ASSERT(lits.numOfAllLiterals()); 
 DOP(ulong debugNumOfLits = lits.numOfAllLiterals());
 _divisionIntoComponents.setGroundCompOptimalWeight(ULONG_MAX);
 _divisionIntoComponents.reset();
 _basicComponent.Reset();
 TERM::Polarity basicCompPol = TERM::NegativePolarity;
 TERM::Polarity nonBasicCompPol = TERM::PositivePolarity;
 //TmpLiteral* lit;     
 bool groundLit;

 ulong litNum = 0;

 for (TmpLiteral* el = lits.first(); el; el = el->next())
  {
   if (el->header().IsSplitting())
    {
     // this literal represents a branch for previously made splitting 
     ASSERT(Signature::current()->isSkolem(el->header().functor()));
     ASSERT(!(el->numOfDifferentVariables()));
     ASSERT(!el->header().arity());
     if (el->header().polarity() == basicCompPol) return false; // the clause is blocked
     ALWAYS(_basicComponent.PushPropLit(el->header(),false));
    }
   else
    {
     groundLit = !(el->numOfDifferentVariables());

     _numLitMap[_divisionIntoComponents.nextLit(groundLit,0UL)] = el; 
 
     const ulong* beginVar;
     const ulong* endVar;
     el->variables(beginVar,endVar);
     for (const ulong* v = beginVar; v != endVar; ++v)
       _divisionIntoComponents.nextVar(*v);

    };        
   litNum++;
  }; 
 
 if (_divisionIntoComponents.numOfComponents() <= 1) 
   // splitting makes no sense 
  { 
   return false; 
  };  

 // splitting makes sense
 _registeredCompNums.reset();    

 _divisionIntoComponents.startResultCollection();

 ulong compNum;

 while (_divisionIntoComponents.litCompPair(litNum,compNum))
  {
   if (!(_registeredCompNums.scoreOf(compNum)))
    {
     _registeredCompNums.add(compNum);
     _components[compNum].reset();    
    };

   lits.remove(_numLitMap[litNum]);
   _components[compNum].push(_numLitMap[litNum]);

  };


 ulong compn;
 TERM header;
 ulong numOfNewComps = 0;
 ulong n;
 ulong numOfNamedComponents = 0;

 // identify components that have already been given names

 for (n = 0; n < _registeredCompNums.numOfDiffElements(); n++) 
  {
   compn = _registeredCompNums.nth(n);
   _newCompName[compn] = (!_splittingWithNamingFlag) || (!_compName->findName(_components[compn],_componentName[compn]));
   if (_newCompName[compn])
    {
     numOfNewComps++;
     if ((!Signature::current()->canCreateNSkolemSymbols(numOfNewComps)) ||
         (Signature::current()->nextSymbolNumber() + numOfNewComps - 1) > TERM::maxFunctor()) 
       {
	 collectRegisteredComponents(lits);
	 ASSERT(debugNumOfLits == lits.numOfAllLiterals());
	 return false;
       };
    }
   else // this component has already been given name   
    {
     header.MakeComplex((TERM::Arity)0,(TERM::Functor)(_componentName[compn]),basicCompPol);
     header.MakeSplitting();
     if (!_basicComponent.PushPropLit(header,false))
      {
       // tautology
       return true;
      };
     numOfNamedComponents++;
    };
  };

 if (numOfNamedComponents) 
  {
   // collect other components in _basicComponent  
   for (n = 0; n < _registeredCompNums.numOfDiffElements(); n++) 
    {
     compn = _registeredCompNums.nth(n);  
     if (_newCompName[compn])
      while (_components[compn].nonempty()) 
       _basicComponent.AllLiterals().push(_components[compn].pop());     
    };

   // make a clause of _basicComponent.AllLiterals() 
   if (_forwardSubsumptionFlag && (_forwardSubsumption->subsume(_basicComponent.AllLiterals())))
    { 
     registerAnotherForwardSubsumed();
     return true; 
    };

   if ((_forwardSubsumptionResolutionFlag) && (_forwardSubsumption->simplifyBySubsumptionResolution(_basicComponent.AllLiterals(),*_ancestors)))
    {
     _ancestors->MakeVIP();
     registerAnotherSimplifiedByFSR();
     if (_basicComponent.AllLiterals().containsOnlyAnswerLiterals()) // happens!
      {
       answerClause = _clauseAssembler->assembleClause(_basicComponent.AllLiterals(),*_ancestors);
       answerClause->SetNumber(clauseNumberGenerator->generate());
       answerClause->setMainSet(Clause::MainSetNew);
       numOfNewClauses++;
       if (showNewClauses) cout << "% " << answerClause << "\n";
       return true;  
      };
    };

   _ancestors->UsedRule(ClauseBackground::Split);
  
   if (_clauseAssembler->assembleClauseInPagedPassiveClauseStorage(storage,_basicComponent.AllLiterals(),*_ancestors,clauseNumberGenerator->generate(),showNewClauses,false))
    {
     numOfNewClauses++;                  
    }
   else
    {
     numOfRejectedNewClauses++;
    };
   
   registerAnotherSplit(numOfNamedComponents); 

   return true;   
  };

 // !numOfNamedComponents    here

 if ((!Signature::current()->canCreateNSkolemSymbols(1)) || 
      (Signature::current()->nextSymbolNumber() > TERM::maxFunctor())) // can not create a new name
   {
     collectRegisteredComponents(lits);
     ASSERT(debugNumOfLits == lits.numOfAllLiterals());     
     return false;
   };
 
 _ancestors->UsedRule(ClauseBackground::Split);

 // now, using heuristics, select a component to become the open one 
 
 ulong openComponentNum = _registeredCompNums.nth(0); // temporary simple solution, most certainly not the best one
 
 // give a name to the open component

 ALWAYS(Signature::current()->createNewSkolemPredicateNumber(0UL,_componentName[openComponentNum]));
 ASSERT(_componentName[openComponentNum] <= TERM::maxFunctor());

 if (_splittingWithNamingFlag)
  _compName->integrate(_components[openComponentNum],_componentName[openComponentNum]);

 header.MakeComplex((TERM::Arity)0,(TERM::Functor)(_componentName[openComponentNum]),basicCompPol);
 header.MakeSplitting();

 Signature::current()->assignMinimalPrecedence(header.polarity(),header.functor());
 

 ALWAYS(_basicComponent.PushPropLit(header,false));
 header.MakeComplex((TERM::Arity)0,(TERM::Functor)(_componentName[openComponentNum]),nonBasicCompPol);
 header.MakeSplitting();

 Signature::current()->assignMinimalPrecedence(header.polarity(),header.functor());

 TmpLiteral* compNameLit = TmpLiteral::freshLiteral();
 compNameLit->reset();
 compNameLit->pushPropLit(header);
 _components[openComponentNum].push(compNameLit);

 if (_clauseAssembler->assembleClauseInPagedPassiveClauseStorage(storage,_components[openComponentNum],*_ancestors,clauseNumberGenerator->generate(),showNewClauses,true))
  {
   numOfNewClauses++;                  
  }
 else
  {
   numOfRejectedNewClauses++;
  };


 registerAnotherComponent(); 
 

 // collect other (blocked)  components in _basicComponent  
 for (n = 0; n < _registeredCompNums.numOfDiffElements(); n++) 
  {
   compn = _registeredCompNums.nth(n);  
   ASSERT(_newCompName[compn]);
   if (compn != openComponentNum)
    while (_components[compn].nonempty()) 
     _basicComponent.AllLiterals().push(_components[compn].pop());     
  };

 // make a clause of _basicComponent.AllLiterals() 

 if (_forwardSubsumptionFlag && (_forwardSubsumption->subsume(_basicComponent.AllLiterals())))
  { 
   registerAnotherForwardSubsumed();
   return true; 
  };

 if ((_forwardSubsumptionResolutionFlag) && (_forwardSubsumption->simplifyBySubsumptionResolution(_basicComponent.AllLiterals(),*_ancestors)))
  {
   _ancestors->MakeVIP();
   registerAnotherSimplifiedByFSR();
   ASSERT(!_basicComponent.AllLiterals().empty());
  };

 //COP("COP300"); 
 if (_clauseAssembler->assembleClauseInPagedPassiveClauseStorage(storage,_basicComponent.AllLiterals(),*_ancestors,clauseNumberGenerator->generate(),showNewClauses,false))
  {
   numOfNewClauses++;                  
  }
 else
  {
   numOfRejectedNewClauses++;
  };

 registerAnotherSplit(2);
 return true;   

}; // bool Splitting::BlockingSplitting3(TmpLitList& lits,Clause*& answerClause,PagedPassiveClauseStorage& storage,..)


bool Splitting::NonblockingSplitting3(TmpLitList& lits,
                                      Clause*& answerClause,
                                      PagedPassiveClauseStorage& storage,
                                      ulong& numOfNewClauses,
                                      ulong& numOfRejectedNewClauses,
                                      Clause::NewNumber* clauseNumberGenerator,
                                      bool showNewClauses)
{
 CALL("NonblockingSplitting3(TmpLitList& lits,Clause*& answerClause,PagedPassiveClauseStorage& storage,..)");
 ASSERT(lits.numOfAllLiterals()); 
 DOP(ulong debugNumOfLits = lits.numOfAllLiterals());
 _divisionIntoComponents.setGroundCompOptimalWeight(ULONG_MAX);
 _divisionIntoComponents.reset();
 _basicComponent.Reset();
 TmpLiteral* lit;     
 bool groundLit;
 TERM::Polarity basicCompPol = TERM::NegativePolarity;
 TERM::Polarity nonBasicCompPol = TERM::PositivePolarity;

 ulong litNum = 0;
 for (TmpLiteral* el = lits.first(); el; el = el->next())
  {
   lit = el;
    //lit = lits.Nth(litNum);
   if (lit->header().IsSplitting())
    {
     // this literal represents a previously made splitting branch
     ASSERT(Signature::current()->isSkolem(lit->header().functor()));
     ASSERT(!(lit->numOfDifferentVariables()));
     ASSERT(!lit->header().arity());

     ALWAYS(_basicComponent.PushPropLit(lit->header(),false));
 
    }
   else
    {
     groundLit = !(lit->numOfDifferentVariables());


     _numLitMap[_divisionIntoComponents.nextLit(groundLit,0UL)] = lit;


     const ulong* beginVar;
     const ulong* endVar;
     lit->variables(beginVar,endVar);
     for (const ulong* v = beginVar; v != endVar; ++v)
       _divisionIntoComponents.nextVar(*v);


    };
   litNum++;        
  }; 
 
 if (_divisionIntoComponents.numOfComponents() <= 1) 
   // splitting makes no sense 
  return false; 

  
 // splitting makes sense
 _registeredCompNums.reset();    

 _divisionIntoComponents.startResultCollection();

 ulong compNum;

 while (_divisionIntoComponents.litCompPair(litNum,compNum))
  {
   if (!(_registeredCompNums.scoreOf(compNum)))
    {
     _registeredCompNums.add(compNum);
     _components[compNum].reset();    
    };

   lits.remove(_numLitMap[litNum]);
   _components[compNum].push(_numLitMap[litNum]);

  };


 ulong compn;
 TERM header;
 ulong numOfNewComps = 0;
 ulong n;

 // identify components that have already been given names
 for (n = 0; n < _registeredCompNums.numOfDiffElements(); n++) 
  {
   compn = _registeredCompNums.nth(n);
   _newCompName[compn] = (!_splittingWithNamingFlag) || (!_compName->findName(_components[compn],_componentName[compn]));
   if (_newCompName[compn])
    {

      //_components[compn].output(cout << "NEW COMP " << compn << " ") << "\n"; 
 
     numOfNewComps++;
     if ((!Signature::current()->canCreateNSkolemSymbols(numOfNewComps)) ||
         (Signature::current()->nextSymbolNumber() + numOfNewComps - 1) > TERM::maxFunctor()) 
       {
	 collectRegisteredComponents(lits);
	 ASSERT(debugNumOfLits == lits.numOfAllLiterals());
	 return false;
       };
    }
   else // this component has already been given name   
    {
     header.MakeComplex((TERM::Arity)0,(TERM::Functor)(_componentName[compn]),basicCompPol);
     header.MakeSplitting();
     if (!_basicComponent.PushPropLit(header,false))
      {
       // tautology
       return true;
      };
     //DF; cout << "PUSH " << header << "\n";

    };
  };

 //_basicComponent.CollectLiterals(); // to make _basicComponent.AllLiterals() available

 //_basicComponent.AllLiterals().output(cout << "*******BC1 ") << "\n"; // debug

 if (_forwardSubsumptionFlag && (_forwardSubsumption->subsume(_basicComponent.AllLiterals())))
  { 
   registerAnotherForwardSubsumed();
   return true; 
  };
 
 // process new components giving new names to them
 bool basicComponentChosen = false;
 ulong chosenBasicComponent;
 #ifdef _SUPPRESS_WARNINGS_ 
  chosenBasicComponent = 0;
 #endif 
  
 DOP(chosenBasicComponent = ULONG_MAX);
 _ancestors->UsedRule(ClauseBackground::Split);
 for (n = 0; n < _registeredCompNums.numOfDiffElements(); n++) 
  {
   compn = _registeredCompNums.nth(n);

   //cout << "COMPNUM " << compn << "\n"; // debug


   if (_newCompName[compn]) 
    {
      // this could change since we could have several components that are variants of each other, 
      // simply check this condition again to avoid duplicate names 
     _newCompName[compn] = 
      (!_splittingWithNamingFlag) 
      || (!_compName->findName(_components[compn],_componentName[compn]));
    };

   if (_newCompName[compn])
    {
     if (basicComponentChosen) 
      {
       ALWAYS(Signature::current()->createNewSkolemPredicateNumber(0UL,_componentName[compn]));
       ASSERT(_componentName[compn] <= TERM::maxFunctor());

       if (_splittingWithNamingFlag)
        _compName->integrate(_components[compn],_componentName[compn]);

       header.MakeComplex((TERM::Arity)0,(TERM::Functor)(_componentName[compn]),basicCompPol);
       header.MakeSplitting();

       //GlobalHeaderWeighting.MakeMinimal(header);
       Signature::current()->assignMinimalPrecedence(header.polarity(),header.functor());

       ALWAYS(_basicComponent.PushPropLit(header,false));
       header.MakeComplex((TERM::Arity)0,(TERM::Functor)(_componentName[compn]),nonBasicCompPol);
       header.MakeSplitting();

       //GlobalHeaderWeighting.MakeMinimal(header);
       Signature::current()->assignMinimalPrecedence(header.polarity(),header.functor());

       TmpLiteral* compNameLit = TmpLiteral::freshLiteral();
       compNameLit->reset();
       compNameLit->pushPropLit(header);
       _components[compn].push(compNameLit);
   
   
       if (_clauseAssembler->assembleClauseInPagedPassiveClauseStorage(storage,_components[compn],*_ancestors,clauseNumberGenerator->generate(),showNewClauses,true))
        {
         numOfNewClauses++;                  
        }
       else
        {
         numOfRejectedNewClauses++;
        };

       registerAnotherComponent();
      }
     else // this component is chosen to be the basic one
      { 
       basicComponentChosen = true;
       chosenBasicComponent = compn;
      };
    };
  };

 //_basicComponent.CollectLiterals(); // this is no problem as far as nothing has been pushed into _basicComponent.AllLiterals()


 //_basicComponent.AllLiterals().output(cout << "*******BC2 ") << "\n"; // debug


 if (basicComponentChosen)
  {
   // add all the literals from _components[chosenBasicComponent] to the basic component

   while (_components[chosenBasicComponent].nonempty()) 
    {
     TmpLiteral* bclit = _components[chosenBasicComponent].pop();

     //cout << "PUSH " << bclit << "\n";

     _basicComponent.AllLiterals().push(bclit);
    };
  }
 else
  {

    //cout << "!basicComponentChosen\n"; // debug

  };

 //_basicComponent.AllLiterals().output(cout << "*******BC3 ") << "\n"; // debug


 if (_forwardSubsumptionFlag && (_forwardSubsumption->subsume(_basicComponent.AllLiterals())))
  { 
   registerAnotherForwardSubsumed();
   return true; 
  };

 if ((_forwardSubsumptionResolutionFlag) && (_forwardSubsumption->simplifyBySubsumptionResolution(_basicComponent.AllLiterals(),*_ancestors)))
  {
   _ancestors->MakeVIP();
   registerAnotherSimplifiedByFSR();
   if (_basicComponent.AllLiterals().containsOnlyAnswerLiterals()) // happens!
    {
     answerClause = _clauseAssembler->assembleClause(_basicComponent.AllLiterals(),*_ancestors);
     answerClause->SetNumber(clauseNumberGenerator->generate());
     answerClause->setMainSet(Clause::MainSetNew);
     numOfNewClauses++;
     if (showNewClauses) cout << "% " << answerClause << "\n";
     return true;  
    };
  };

 if (_clauseAssembler->assembleClauseInPagedPassiveClauseStorage(storage,_basicComponent.AllLiterals(),*_ancestors,clauseNumberGenerator->generate(),showNewClauses,false))
  {
   numOfNewClauses++;                  
  }
 else
  {
   numOfRejectedNewClauses++;
  };

 registerAnotherSplit(_divisionIntoComponents.numOfComponents());

 return true;
}; // bool Splitting::NonblockingSplitting3(TmpLitList& lits,Clause*& answerClause,PagedPassiveClauseStorage& storage,..)







//=================================================
