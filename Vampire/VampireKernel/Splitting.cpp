
//
// File:         Splitting.cpp
// Description:  Core of splitting without backtracking
//               for the OTTER loop (--main_alg 0).
//               Additionally implements negative equality splitting.
// Created:      Dec 15, 2001, Alexandre Riazanov, riazanov@cs.man.ac.uk
// Revised:      Dec 17, 2001, Alexandre Riazanov, riazanov@cs.man.ac.uk
//               Array ulong _litNumMap[HellConst::MaxNumOfLiterals]
//               is replace by 
//               TmpLiteral* _numLitMap[HellConst::MaxNumOfLiterals] 
// Revised:      Dec 26, 2001, Alexandre Riazanov, riazanov@cs.man.ac.uk
//               Freed from HellConst::MaxNumOfFunctors.
// Revised:      Apr 19, 2002, Alexandre Riazanov, riazanov@cs.man.ac.uk
//               Old "pragmatic" version gone.
// Revised:      May 16, 2002, Alexandre Riazanov, riazanov@cs.man.ac.uk
//               Splitting2.cpp and Splitting3.cpp separated. 
//============================================================================
#include "Splitting.hpp"
#include "ForwardSubsumptionFrontEnd.hpp"
#include "Term.hpp"
#include "ComponentName.hpp"
#include "Clause.hpp"
#include "PagedPassiveClauseStorage.hpp"
#include "ClauseAssembler.hpp"
#include "Signature.hpp"
using namespace BK;
using namespace VK;
//============================================================================
#undef DEBUG_NAMESPACE
#ifdef DEBUG_SPLITTING
 #define DEBUG_NAMESPACE "Splitting"
#endif
#include "debugMacros.hpp"
//============================================================================



Splitting::Splitting(OpenClauseBackground* ancestors) 
  : _splittingStyle(1L),
    _splittingWithNamingFlag(false),
    _forwardSubsumptionFlag(false), 
    _forwardSubsumptionResolutionFlag(false),
    _negEqSplittingMinEligibleTermSize(LONG_MAX),
    _ancestors(ancestors), 
    _statistics(), 
    _maxNumOfLitsSoFar(0UL),
    _divisionIntoComponents(),
    _numLitMap(DOP("Splitting::_numLitMap")),
    _basicComponent(),
    _forwardSubsumption(0),
    _components(DOP("Splitting::_components")),
    _registeredCompNums(DOP("Splitting::_registeredCompNums")),
    _componentName(DOP("Splitting::_componentName")),
    _newCompName(DOP("Splitting::_newCompName"))
{
  CALL("constructor Splitting()");
  DOP(_numLitMap.freeze());
  DOP(_components.freeze());
  DOP(_registeredCompNums.freeze());
  DOP(_componentName.freeze());
  DOP(_newCompName.freeze());   
}; // Splitting::Splitting(OpenClauseBackground* ancestors) 


Splitting::~Splitting() 
{ 
  CALL("destructor ~Splitting()"); 
  if (DestructionMode::isThorough())
    {
      for (ulong i = 0; i < _components.size(); ++i)
	_components[i].destroy();
    };
}; // Splitting::~Splitting() 



void Splitting::init(OpenClauseBackground* ancestors)
{
  CALL("init(OpenClauseBackground* ancestors)");
  
  _splittingStyle = 1L;
  _splittingWithNamingFlag = false;
  _forwardSubsumptionFlag = false; 
  _forwardSubsumptionResolutionFlag = false;
  _negEqSplittingMinEligibleTermSize = LONG_MAX;
  _ancestors = ancestors;
  _statistics.init(); 
  _maxNumOfLitsSoFar = 0UL;
  _divisionIntoComponents.init();
  _numLitMap.init(DOP("Splitting::_numLitMap"));
  _basicComponent.init();
  _forwardSubsumption = 0;
  _components.init(DOP("Splitting::_components"));
  _registeredCompNums.init(DOP("Splitting::_registeredCompNums"));
  _componentName.init(DOP("Splitting::_componentName"));
  _newCompName.init(DOP("Splitting::_newCompName"));
  DOP(_numLitMap.freeze());
  DOP(_components.freeze());
  DOP(_registeredCompNums.freeze());
  DOP(_componentName.freeze());
  DOP(_newCompName.freeze());  
}; // void Splitting::init(OpenClauseBackground* ancestors)


void Splitting::destroy()
{
  CALL("destroy()");
  if (DestructionMode::isThorough())
    {
      for (ulong i = 0; i < _components.size(); ++i)
	_components[i].destroy();
    };
  _newCompName.destroy();
  _componentName.destroy();
  _registeredCompNums.destroy();
  _components.destroy();
  _basicComponent.destroy();
  _numLitMap.destroy();
  _divisionIntoComponents.destroy();
  _statistics.destroy();
}; // void Splitting::destroy()




bool Splitting::BlockingSplitting(TmpLitList& lits,ClauseQueue& newClauses)
{
 CALL("BlockingSplitting(TmpLitList& lits,ClauseQueue& newClauses)");

 ASSERT(lits.numOfAllLiterals()); 
 DOP(ulong debugNumOfLits = lits.numOfAllLiterals());
 _divisionIntoComponents.setGroundCompOptimalWeight(ULONG_MAX);
 _divisionIntoComponents.reset();
 _basicComponent.Reset();
 TERM::Polarity basicCompPol = TERM::NegativePolarity;
 TERM::Polarity nonBasicCompPol = TERM::PositivePolarity;
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
     if (el->header().polarity() == basicCompPol) 
       return false; // the clause is blocked
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
       // discarded (tautology or too heavy)  
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
       { 
        _basicComponent.AllLiterals().push(_components[compn].pop());
       };     
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
    };

   _ancestors->UsedRule(ClauseBackground::Split);
   Clause* cl = _clauseAssembler->assembleClause(_basicComponent.AllLiterals(),*_ancestors);
   cl->setMainSet(Clause::MainSetNew);
   newClauses.enqueue(cl); 

   registerAnotherSplit(numOfNamedComponents); 

   return true;   
  };

 // !namedComponentExists here


 if ((!Signature::current()->canCreateNSkolemSymbols(1)) || 
      (Signature::current()->nextSymbolNumber() > TERM::maxFunctor())) 
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

 //GlobalHeaderWeighting.MakeMinimal(header);
 Signature::current()->assignMinimalPrecedence(header.polarity(),header.functor());

 ALWAYS(_basicComponent.PushPropLit(header,false));


 header.MakeComplex((TERM::Arity)0,(TERM::Functor)(_componentName[openComponentNum]),nonBasicCompPol);
 header.MakeSplitting();

 //GlobalHeaderWeighting.MakeMinimal(header);
 Signature::current()->assignMinimalPrecedence(header.polarity(),header.functor());

 TmpLiteral* compNameLit = TmpLiteral::freshLiteral();
 compNameLit->reset();
 
 compNameLit->pushPropLit(header);

 _components[openComponentNum].push(compNameLit);
 Clause* cl = _clauseAssembler->assembleClause(_components[openComponentNum],*_ancestors); // the definition clause
 cl->MarkAsName();
 cl->setMainSet(Clause::MainSetNew);
 newClauses.enqueue(cl);    
 registerAnotherComponent(); 
 

 //_basicComponent.CollectLiterals(); // to make _basicComponent.AllLiterals() available

 // collect other (blocked)  components in _basicComponent  
 for (n = 0; n < _registeredCompNums.numOfDiffElements(); n++) 
  {
   compn = _registeredCompNums.nth(n);  
   ASSERT(_newCompName[compn]);
   if (compn != openComponentNum)
    while (_components[compn].nonempty())
     { 
      _basicComponent.AllLiterals().push(_components[compn].pop());
     };     
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
  };

 cl = _clauseAssembler->assembleClause(_basicComponent.AllLiterals(),*_ancestors);
 cl->setMainSet(Clause::MainSetNew);
 newClauses.enqueue(cl); 
 registerAnotherSplit(2);
 return true;   

}; // bool Splitting::BlockingSplitting(TmpLitList& lits,ClauseQueue& newClauses)


bool Splitting::NonblockingSplitting(TmpLitList& lits,ClauseQueue& newClauses)
{
 CALL("NonblockingSplitting(TmpLitList& lits)");

 ASSERT(lits.numOfAllLiterals()); 
 DOP(ulong debugNumOfLits = lits.numOfAllLiterals());
 _divisionIntoComponents.setGroundCompOptimalWeight(ULONG_MAX);
 _divisionIntoComponents.reset();
 _basicComponent.Reset();
 //TmpLiteral* lit; 
 bool groundLit;
 TERM::Polarity basicCompPol = TERM::NegativePolarity;
 TERM::Polarity nonBasicCompPol = TERM::PositivePolarity;

 
 ulong litNum = 0;
 for (TmpLiteral* el = lits.first(); el; el = el->next())
  {
   if (el->header().IsSplitting())
    {
     // this literal represents a previously made splitting branch
     ASSERT(Signature::current()->isSkolem(el->header().functor()));
     ASSERT(!(el->numOfDifferentVariables()));
     ASSERT(!el->header().arity());
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
    };
  };


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

       Signature::current()->assignMinimalPrecedence(header.polarity(),header.functor());


       ALWAYS(_basicComponent.PushPropLit(header,false));

       header.MakeComplex((TERM::Arity)0,(TERM::Functor)(_componentName[compn]),nonBasicCompPol);
       header.MakeSplitting();

       Signature::current()->assignMinimalPrecedence(header.polarity(),header.functor());

       TmpLiteral* compNameLit = TmpLiteral::freshLiteral();
       compNameLit->reset();
       compNameLit->pushPropLit(header);
       _components[compn].push(compNameLit);
       Clause* cl = _clauseAssembler->assembleClause(_components[compn],*_ancestors); // the definition clause
       cl->MarkAsName();
       cl->setMainSet(Clause::MainSetNew);
       newClauses.enqueue(cl);    
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

 if (basicComponentChosen)
  {
   // add all the literals from _components[chosenBasicComponent] to the basic component

   while (_components[chosenBasicComponent].nonempty()) 
    _basicComponent.AllLiterals().push(_components[chosenBasicComponent].pop());
  };

 if (_forwardSubsumptionFlag && (_forwardSubsumption->subsume(_basicComponent.AllLiterals())))
  { 
   registerAnotherForwardSubsumed();
   return true; 
  };

 if ((_forwardSubsumptionResolutionFlag) && (_forwardSubsumption->simplifyBySubsumptionResolution(_basicComponent.AllLiterals(),*_ancestors)))
  {
   _ancestors->MakeVIP();
   registerAnotherSimplifiedByFSR();
  };

 Clause* cl = _clauseAssembler->assembleClause(_basicComponent.AllLiterals(),*_ancestors);
 cl->setMainSet(Clause::MainSetNew);
 newClauses.enqueue(cl); 

 registerAnotherSplit(_divisionIntoComponents.numOfComponents());

 return true;
}; // bool Splitting::NonblockingSplitting(TmpLitList& lits,ClauseQueue& newClauses)




void Splitting::splitNegativeEqualities(TmpLitList& lits,ClauseQueue& newClauses)
{
 CALL("splitNegativeEqualities(TmpLitList& lits,ClauseQueue& newClauses)");

 adjustSizes(lits.numOfAllLiterals()); 

 bool firstArgEligible;
 bool secondArgEligible;
 bool firstNameExists;
 bool secondNameExists;
 ulong firstName;
 ulong secondName;
 bool newName; 
 TmpLiteral* spareLiteral = TmpLiteral::freshLiteral();

 for (TmpLiteral* el = lits.first(); el; el = el->next())
  {
   if (el->isEquality() && el->isNegative())
    {
      firstArgEligible = el->arg1IsGround() &&
	(el->arg1()->size() >= _negEqSplittingMinEligibleTermSize);
     secondArgEligible = el->arg2IsGround() && 
       (el->arg2()->size() >= _negEqSplittingMinEligibleTermSize);

     if (firstArgEligible)
      {
       if (secondArgEligible)
        {
	 // both ground and heavy enough        
	 firstNameExists = _compName->findName(el->arg1(),firstName);
         secondNameExists = _compName->findName(el->arg2(),secondName);
         if (firstNameExists)
	  {
           if (secondNameExists)
	    { 
	     // both arguments have names
	     ASSERT(firstName != secondName);
	     if (el->arg1()->size() > el->arg2()->size())
	      {            
               TERM negHeader((TERM::Arity)1,(TERM::Functor)firstName,TERM::NegativePolarity);   
	 
               spareLiteral->reset();
               spareLiteral->pushNonpropHeader(negHeader);
               spareLiteral->pushTerm(el->arg2());
	       spareLiteral->endOfLiteral();
              }          
             else // el->arg1()->size() <= el->arg2()->size()
	      {
               TERM negHeader((TERM::Arity)1,(TERM::Functor)secondName,TERM::NegativePolarity);       
               spareLiteral->reset();
               spareLiteral->pushNonpropHeader(negHeader);
               spareLiteral->pushTerm(el->arg1());
	       spareLiteral->endOfLiteral();
              };
            }
           else // firstNameExists && !secondNameExits
	    {
             TERM negHeader((TERM::Arity)1,(TERM::Functor)firstName,TERM::NegativePolarity);    
             spareLiteral->reset();
             spareLiteral->pushNonpropHeader(negHeader);
             spareLiteral->pushTerm(el->arg2()); 
	     spareLiteral->endOfLiteral();
            };
	   
	   
	   TmpLiteral* tmp = spareLiteral;
	   lits.replaceBy(el,spareLiteral);
	   spareLiteral = el;
	   el = tmp;


           _ancestors->UsedRule(ClauseBackground::Split);
          }
         else // !firstNameExists 
	  if (secondNameExists)
	   { 
            TERM negHeader((TERM::Arity)1,(TERM::Functor)secondName,TERM::NegativePolarity);               
            spareLiteral->reset();
            spareLiteral->pushNonpropHeader(negHeader);
            spareLiteral->pushTerm(el->arg1());
	    spareLiteral->endOfLiteral();

	    TmpLiteral* tmp = spareLiteral;
	    lits.replaceBy(el,spareLiteral);
	    spareLiteral = el;
	    el = tmp;

            _ancestors->UsedRule(ClauseBackground::Split);
           }
          else // !firstNameExists && !secondNameExits
	   {
	    // a new name must be created            
            if (el->arg1()->size() > el->arg2()->size())
	     {
	      if (_compName->nameFor(el->arg1(),firstName,newName))
	       {
		ASSERT_IN(newName,"COP100");
                TERM posHeader((TERM::Arity)1,(TERM::Functor)firstName,TERM::PositivePolarity); 
                TERM negHeader((TERM::Arity)1,(TERM::Functor)firstName,TERM::NegativePolarity);


		Signature::current()->assignMaximalPrecedence(posHeader.polarity(),posHeader.functor());

		Signature::current()->assignMaximalPrecedence(negHeader.polarity(),negHeader.functor());


                spareLiteral->reset();
                spareLiteral->pushNonpropHeader(negHeader);
                spareLiteral->pushTerm(el->arg2());
		spareLiteral->endOfLiteral();

		TmpLiteral* tmp = spareLiteral;
		lits.replaceBy(el,spareLiteral);
		spareLiteral = el;
		el = tmp;
 
                
		TmpLiteral* positiveName = TmpLiteral::freshLiteral();
                positiveName->reset();
                positiveName->pushNonpropHeader(posHeader);
                positiveName->pushTerm(spareLiteral->arg1());
		positiveName->endOfLiteral();

                _components[0].reset();
                _components[0].push(positiveName);
                _ancestors->UsedRule(ClauseBackground::Split);
                Clause* cl = _clauseAssembler->assembleClause(_components[0],*_ancestors);
                cl->setMainSet(Clause::MainSetNew);
                newClauses.enqueue(cl);                  
               };
             } 
            else // el->arg1()->size() <= el->arg2()->size() 
	     {
              if (_compName->nameFor(el->arg2(),secondName,newName))
	       {
		ASSERT_IN(newName,"COP200");
                TERM posHeader((TERM::Arity)1,(TERM::Functor)secondName,TERM::PositivePolarity); 
                TERM negHeader((TERM::Arity)1,(TERM::Functor)secondName,TERM::NegativePolarity);

		Signature::current()->assignMaximalPrecedence(posHeader.polarity(),posHeader.functor());

		Signature::current()->assignMaximalPrecedence(negHeader.polarity(),negHeader.functor());

                spareLiteral->reset();
                spareLiteral->pushNonpropHeader(negHeader);
                spareLiteral->pushTerm(el->arg1());
		spareLiteral->endOfLiteral();

		TmpLiteral* tmp = spareLiteral;
		lits.replaceBy(el,spareLiteral);
		spareLiteral = el;
		el = tmp;
 
                
		TmpLiteral* positiveName = TmpLiteral::freshLiteral();
                positiveName->reset();
                positiveName->pushNonpropHeader(posHeader);
                positiveName->pushTerm(spareLiteral->arg2());
		positiveName->endOfLiteral();

                _components[0].reset();
                _components[0].push(positiveName);
                _ancestors->UsedRule(ClauseBackground::Split);
                Clause* cl = _clauseAssembler->assembleClause(_components[0],*_ancestors);
                cl->setMainSet(Clause::MainSetNew);
                newClauses.enqueue(cl);                   
               };
             };
           };
        }
       else // only firstArgEligible
        {
         firstNameExists = _compName->findName(el->arg1(),firstName);
         if (firstNameExists) // the old name can be used
	  {
           TERM negHeader((TERM::Arity)1,(TERM::Functor)firstName,TERM::NegativePolarity);               
	   spareLiteral->reset();
           spareLiteral->pushNonpropHeader(negHeader);
           spareLiteral->pushTerm(el->arg2());
	   spareLiteral->endOfLiteral();

	   TmpLiteral* tmp = spareLiteral;
	   lits.replaceBy(el,spareLiteral);
	   spareLiteral = el;
	   el = tmp;

           _ancestors->UsedRule(ClauseBackground::Split);
          }
         else // a new name must be introduced
	  {
	   if ((el->arg2()->size() > 1) 
               && (_compName->nameFor(el->arg1(),firstName,newName)))
	    {
	     ASSERT_IN(newName,"COP300");
             TERM posHeader((TERM::Arity)1,(TERM::Functor)firstName,TERM::PositivePolarity); 
             TERM negHeader((TERM::Arity)1,(TERM::Functor)firstName,TERM::NegativePolarity);

	     Signature::current()->assignMaximalPrecedence(posHeader.polarity(),posHeader.functor());

	     Signature::current()->assignMaximalPrecedence(negHeader.polarity(),negHeader.functor());

             spareLiteral->reset();
             spareLiteral->pushNonpropHeader(negHeader);
             spareLiteral->pushTerm(el->arg2());
	     spareLiteral->endOfLiteral();


	     TmpLiteral* tmp = spareLiteral;
	     lits.replaceBy(el,spareLiteral);
	     spareLiteral = el;
	     el = tmp;    
                
	     TmpLiteral* positiveName = TmpLiteral::freshLiteral();
             positiveName->reset();
             positiveName->pushNonpropHeader(posHeader);
             positiveName->pushTerm(spareLiteral->arg1());
	     positiveName->endOfLiteral();

             _components[0].reset();
             _components[0].push(positiveName);
             _ancestors->UsedRule(ClauseBackground::Split);  
             Clause* cl = _clauseAssembler->assembleClause(_components[0],*_ancestors);
             cl->setMainSet(Clause::MainSetNew);
             newClauses.enqueue(cl);                    
            };
          };
        };
      }
     else // !firstArgEligible
      if (secondArgEligible)
       {
	// only secondArgEligible            
        secondNameExists = _compName->findName(el->arg2(),secondName);     
        if (secondNameExists) // the old name can be used
	 {
          TERM negHeader((TERM::Arity)1,(TERM::Functor)secondName,TERM::NegativePolarity);               
	  spareLiteral->reset();
          spareLiteral->pushNonpropHeader(negHeader);
          spareLiteral->pushTerm(el->arg1());
	  spareLiteral->endOfLiteral();

	  TmpLiteral* tmp = spareLiteral;
	  lits.replaceBy(el,spareLiteral);
	  spareLiteral = el;
	  el = tmp;

          _ancestors->UsedRule(ClauseBackground::Split);         
         }
        else // a new name must be introduced
	 {
          if ((el->arg1()->size() > 1)
              && (_compName->nameFor(el->arg2(),secondName,newName)))
	   {
            ASSERT_IN(newName,"COP400");
            TERM posHeader((TERM::Arity)1,(TERM::Functor)secondName,TERM::PositivePolarity); 
            TERM negHeader((TERM::Arity)1,(TERM::Functor)secondName,TERM::NegativePolarity);

	    Signature::current()->assignMaximalPrecedence(posHeader.polarity(),posHeader.functor());
            
	    Signature::current()->assignMaximalPrecedence(negHeader.polarity(),negHeader.functor());

            spareLiteral->reset();
            spareLiteral->pushNonpropHeader(negHeader);
            spareLiteral->pushTerm(el->arg1());
	    spareLiteral->endOfLiteral();


	    TmpLiteral* tmp = spareLiteral;
	    lits.replaceBy(el,spareLiteral);
	    spareLiteral = el;
	    el = tmp;

                
	    TmpLiteral* positiveName = TmpLiteral::freshLiteral();
            positiveName->reset();
            positiveName->pushNonpropHeader(posHeader);
            positiveName->pushTerm(spareLiteral->arg2());
	    positiveName->endOfLiteral();


            _components[0].reset();
            _components[0].push(positiveName);
            _ancestors->UsedRule(ClauseBackground::Split);   
            Clause* cl = _clauseAssembler->assembleClause(_components[0],*_ancestors);
            cl->setMainSet(Clause::MainSetNew);
            newClauses.enqueue(cl); 
	   };            
         };
       };

   }; // if (el->IsEquality() && el->Negative()) ...
 }; // for ...

 spareLiteral->recycle();
  
}; // void Splitting::splitNegativeEqualities(TmpLitList& lits,ClauseQueue& newClauses)




//=================================================
