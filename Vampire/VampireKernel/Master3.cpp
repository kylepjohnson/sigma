//
// File:         Master3.cpp
// Description:  Main loop implementing main_alg 3
// Created:      Apr 29, 2001, Alexandre Riazanov, riazanov@cs.man.ac.uk
// Revised:      May 20, 2002, Alexandre Riazanov, riazanov@cs.man.ac.uk
//               Too strong an assertion removed in processCollectedInput3().
//====================================================
#include "Master.hpp"

#include "GlobalStopFlag.hpp"
//====================================================
#undef DEBUG_NAMESPACE
#ifdef DEBUG_MASTER
 #define DEBUG_NAMESPACE "Master"
#endif                  
#include "debugMacros.hpp"
//====================================================

  using namespace BK;
  using namespace Gem;
  using namespace VK;

  void Master::inputClause3(Clause* cl)
  {
    CALL("inputClause3(Clause* cl)");
    if (cl->isSupportClause())
      {
	inputSupportClause3(cl);
      }
    else
      inputQueryClause3(cl);
  }; // void Master::inputClause3(Clause* cl)

  void Master::inputQueryClause3(Clause* cl)
  {
    CALL("inputQueryClause3(Clause* cl)");  
    ASSERT(!cl->isSupportClause());
    switch (_continuation)
      {
      case Continuation_Init:
	break;
      case Continuation_InputQueryClause3_ProcessCollectedInput:
	cancelTermination();
	goto process_collected_input;
      default:
	ICP("ICP0");
	RuntimeError::report("Bad _continuation in VK::Master::inputQueryClause3(Clause* cl)");
      };


    ASSERT(!_terminated);
    ASSERT(!_proof);
    _timer.reset();
    _timer.start();
    _timerIsOn = true;

    if (_freshClause.loadClause(cl,ClauseBackground::Prepro))
      {
	_freshClause.PushAncestor(cl);    
	_freshClause.UsedRule(ClauseBackground::Prepro);
	_pagedPassive.maximalPriorityOn();
      process_collected_input:
	processCollectedInputQuery3();
	if (_proof) 
	  { 
	    terminate(Proof);
	    ASSERT(_retainedNewClauses.empty());    
	    disposeObsolete(); 
	    _timer.stop();
	    _timerIsOn = false; 
	    _continuation = Continuation_InputQueryClause3_ProcessCollectedInput;
	    return; 
	  };
	_continuation = Continuation_Init;
	_pagedPassive.maximalPriorityOff();
      };

  }; // void Master::inputQueryClause3(Clause* cl)



  void Master::inputSupportClause3(Clause* cl)
  {
    CALL("inputSupportClause3(Clause* cl)");   
    ASSERT(cl->isSupportClause());
    switch (_continuation)
      {
      case Continuation_Init:
	break;
      case Continuation_InputSupportClause3_ProcessCollectedInput:
	cancelTermination();
	goto process_collected_input;
      default:
	ICP("ICP0");
	RuntimeError::report("Bad _continuation in VK::Master::inputSupportClause3(Clause* cl)");
      };


    ASSERT(!_terminated);
    ASSERT(!_proof);
    _timer.reset();
    _timer.start();
    _timerIsOn = true;

    if (_freshClause.loadClause(cl,ClauseBackground::Prepro))
      {
	_freshClause.PushAncestor(cl);    
	_freshClause.UsedRule(ClauseBackground::Prepro);
	_freshClause.markAsSupport();
	_pagedPassive.maximalPriorityOn();
      process_collected_input:
	processCollectedInputSupport3();
	if (_proof) 
	  { 
	    terminate(Proof);
	    ASSERT(_retainedNewClauses.empty());    
	    disposeObsolete(); 
	    _timer.stop();
	    _timerIsOn = false; 
	    _continuation = Continuation_InputSupportClause3_ProcessCollectedInput;
	    return; 
	  };
	_continuation = Continuation_Init;
	_pagedPassive.maximalPriorityOff();
      };

  }; // void Master::inputSupportClause3(Clause* cl)



  void Master::processCollectedInputQuery3()
  {
    CALL("processCollectedInputQuery3()");
    Clause* newClause;  
  
    switch (_continuation)
      {
      case Continuation_Init:
	break;
      case Continuation_InputQueryClause3_ProcessCollectedInput:
	goto process_next_retained_clause;
      default:
	ICP("ICP0");
	RuntimeError::report("Bad _continuation in VK::Master::processCollectedInputQuery3()");
      };



    if (_parameters.outputStreamForGenerated) _freshClause.output(*_parameters.outputStreamForGenerated << "% GEN " << statNumOfGeneratedCl() << ' ') << '\n';

    _freshClause.ProcessCollected2(_retainedNewClauses);


  process_next_retained_clause:
    while (_retainedNewClauses.dequeue(newClause))
      { 
	ASSERT(newClause->mainSet() == Clause::MainSetNew);
	newClause->SetNumber(_clauseNumberGenerator->generate());

	INTERCEPT_BACKWARD_SUBSUMPTION_INDEXING_INTRODUCE_CLAUSE_ID(newClause);  

	if (_parameters.showPassive) _parameters.showPassive(newClause);    
 
	if (newClause->isAnswer()) 
	  {
	    rememberAnswer3(newClause);
	    _proof = newClause;
	    return;
	  };
	_continuation = Continuation_Init;
      
	newClause->setMainSet(Clause::MainSetFree);  
      
	if (!insertIntoPassive3(newClause)) 
	  ++_statClausesRefusedAllocation;
      
	disposeClause(newClause);   
      };
    ASSERT(_retainedNewClauses.empty());
  }; // void Master::processCollectedInputQuery3()


  void Master::processCollectedInputSupport3()
  {
    CALL("processCollectedInputSupport3()");
    Clause* newClause;  

    switch (_continuation)
      {
      case Continuation_Init:
	break;
      case Continuation_InputSupportClause3_ProcessCollectedInput:
	goto process_next_retained_clause;
      default:
	ICP("ICP0");
	RuntimeError::report("Bad _continuation in VK::Master::processCollectedInputSupport3()");
      };


    if (_parameters.outputStreamForGenerated) 
      _freshClause.output(*_parameters.outputStreamForGenerated << "% GEN " 
			  << statNumOfGeneratedCl() << ' ') << '\n';
  
    _freshClause.ProcessCollectedSupport(_retainedNewClauses);

  process_next_retained_clause:
    while (_retainedNewClauses.dequeue(newClause))
      { 
	ASSERT(newClause->mainSet() == Clause::MainSetNew);

	if (newClause->isAnswer()) 
	  {
	    rememberAnswer0(newClause);
	    _proof = newClause;
	    return;
	  };
	_continuation = Continuation_Init;

	newClause->SetNumber(_clauseNumberGenerator->generate());
	INTERCEPT_BACKWARD_SUBSUMPTION_INDEXING_INTRODUCE_CLAUSE_ID(newClause);
	if (_parameters.useClauseNumIndex) _clauseNumIndex.insert(newClause);
	if (_parameters.showPassive) _parameters.showPassive(newClause);
	integratePassive(newClause);
	makeActive0(newClause);
      };
    ASSERT(_retainedNewClauses.empty());
  }; // void Master::processCollectedInputSupport3()



  bool Master::insertIntoPassive3(Clause* cl)
  {
    CALL("Master::insertIntoPassive3(Clause* cl)");
    ASSERT(_parameters.main_alg == 3);
    ulong max_weight = _freshClause.MaxWeight();
    _freshClause.SetMaxWeight(VampireKernelConst::MaxClauseSize); 
 
    ALWAYS(_freshClause.loadClause(cl,ClauseBackground::Prepro));
    _freshClause.setClauseNumber(cl->Number());
      
    for (ClauseBackground::ClauseList* anc = cl->background().ancestors(); anc; anc = anc->tl())
      _freshClause.PushAncestor(anc->hd()); 

    _freshClause.InverseAncestorOrder();

    _freshClause.UsedRules(cl->background().properties());

    if (_parameters.outputStreamForGenerated) _freshClause.output(*_parameters.outputStreamForGenerated << "% GEN " << statNumOfGeneratedCl() << ' ') << '\n';
  
 
    bool res = 
		_freshClause.transferToPagedPassive(_pagedPassive,(_parameters.showPassive != 0));  
 
    _freshClause.SetMaxWeight(max_weight);

    return res;
  }; // bool Master::insertIntoPassive3(Clause* cl)



  void Master::mainLoop3()
  {
    CALL("mainLoop3()");
    PagedPassiveClauseStorage::StoredClause selectedClause;
    Clause* recentlyReanimatedClause;

    switch (_continuation)
      {
      case Continuation_Init:
	break;
      case Continuation_MainLoop3_SelectCurrent:
	cancelTermination();
	_continuation = Continuation_Init;
	goto select_current;
      case Continuation_MainLoop3_BackSimplify:
	cancelTermination();
	_continuation = Continuation_Init;
	goto back_simplify;
      case Continuation_MainLoop3_ProcessNewInference:       
	cancelTermination();
	_continuation = Continuation_Init;
	goto after_process_new_inference;
      default:
	ICP("ICP0");
	RuntimeError::report("Bad _continuation in VK::Master::mainLoop3()");
      };

    ASSERT(!_terminated);
    ASSERT(!_proof);


    _timer.reset();
    _timer.start();
    _timerIsOn = true;
    _startMemory = (float)GlobAlloc::occupiedByObjects(); 

    endOfInput();
    ASSERT(!_terminated);

  select_current: 
    if (!_pagedPassive.select(selectedClause)) 
      { 
	terminate(EmptyPassive); 
	_timer.stop();
	_timerIsOn = false; 
	return; 
      };
    if (_active.Total() >= _parameters.maxActive) 
      { 
	terminate(MaxActive); 
	_timer.stop();
	_timerIsOn = false; 
	return; 
      };


    if ((_parameters.orphan_murder)
	&& (!selectedClause.isVIP())
	&& (selectedClause.isOrphan()))
      {      
	_statPassiveOrphansMurdered++;
	_pagedPassive.popSelected();
	goto select_current;
      };


    _reanimatedClause = reanimate3(selectedClause);
  
    // _pagedPassive.popSelected() is done in reanimate3(selectedClause)

    if (!_reanimatedClause) { GlobalStopFlag::check(); goto select_current; };
    if (_reanimatedClause->isAnswer()) 
      { 
	rememberAnswer3(_reanimatedClause);
	_proof = _reanimatedClause;
	_reanimatedClause = 0;
	terminate(Proof);  
	_timer.stop();
	_timerIsOn = false; 
	_continuation = Continuation_MainLoop3_SelectCurrent;
	return; 
      };

    if (_parameters.backward_subsumption) 
      if (backSubsumeBy(_reanimatedClause)) 
	if (_parameters.orphan_murder) killActiveOrphans3();      
    
    disposeObsolete();
  

  back_simplify:
    backSimplifyBy3(_reanimatedClause);
    if (_proof) 
      {
	terminate(Proof);  
	_timer.stop();
	_timerIsOn = false; 
	_continuation = Continuation_MainLoop3_BackSimplify;
	return; 
      };    
    disposeObsolete();  
    makeActive3(_reanimatedClause);
    recentlyReanimatedClause = _reanimatedClause;
    _reanimatedClause = 0; // not to be destroyed as reanimated
    _inferenceDispatcherSemaphore = true;  
    _infDispatcher.newQuery(recentlyReanimatedClause);
  
  next_inference:
    if (_infDispatcher.nextInference())
      {    
	processCollected3();
	if (_proof) 
	  { 
	    terminate(Proof); 
	    _inferenceDispatcherSemaphore = false;  
	    _timer.stop();
	    _timerIsOn = false; 
	    _continuation = Continuation_MainLoop3_ProcessNewInference;
	    return; 
	  };
     
      after_process_new_inference: 
  
	if (_pagedPassive.totalNumberOfClauses() >= _parameters.maxPassive) 
	  { 
	    terminate(MaxPassive); 
	    _inferenceDispatcherSemaphore = false; 
	    _timer.stop();
	    _timerIsOn = false; 
	    return; 
	  };

	GlobalStopFlag::check();
	goto next_inference;
      };
    _inferenceDispatcherSemaphore = false;
 

    GlobalStopFlag::check();

    // destroy obsolete clauses
    ASSERT(_retainedNewClauses.empty());
    ASSERT(_queueForBackSimplification.empty());
    ASSERT(_queueForDisposal.empty());
    GlobalStopFlag::check();
    goto select_current; 

  };// void Master::mainLoop3()


  void Master::backSimplifyBy3(Clause* simplifier)
  {
    CALL("backSimplifyBy3(Clause* simplifier)");
    _backwardDemodulationSemaphore = true; 
    Clause* backSimplified; 
    bool somethingSimplified = false;
    if (backDemodulationQuery(simplifier))
      { 
	while (nextBackDemodulated())
	  {         
	    _statBackSimplified++;
         
	    if (collectBackSimplified())
	      { 
		backSimplified = _backDemodulation.simplified();
		ASSERT(backSimplified->mainSet() == Clause::MainSetActive);       
		enqueueForDisposal(backSimplified);        
		somethingSimplified = true;
		processCollected3(); // not before enqueueForDisposal3(backSimplified)!

		if (_proof) 
		  { 
		    terminate(Proof); 
		    _backwardDemodulationSemaphore = false; 
		    return; 
		  };
		if (_pagedPassive.totalNumberOfClauses() >= _parameters.maxPassive) 
		  { 
		    terminate(MaxPassive); 
		    _backwardDemodulationSemaphore = false; 
		    return; 
		  }; 
	      }
	    else
	      if (_freshClause.WasTautology())
		{
		  backSimplified = _backDemodulation.simplified();
		  enqueueForDisposal(backSimplified);
		};
	    GlobalStopFlag::check();
	  };  
      }; 
    if (_parameters.orphan_murder && somethingSimplified) killActiveOrphans3();
    _backwardDemodulationSemaphore = false;
  }; // void Master::backSimplifyBy3(Clause* simplifier)

  Clause* Master::reanimate3(PagedPassiveClauseStorage::StoredClause& cl)
  {
    CALL("reanimate3(PagedPassiveClauseStorage::StoredClause& cl)");
    ASSERT(_parameters.main_alg == 3); 

    ulong max_weight = _freshClause.MaxWeight();
    _freshClause.SetMaxWeight(VampireKernelConst::MaxClauseSize); 
 
    _freshClause.loadClause(cl.literals(),ClauseBackground::Reanimation);
 
    cl.resetAncestors();
    Clause* ancestor;
    while (cl.nextAncestor(ancestor)) _freshClause.PushAncestor(ancestor);
    _freshClause.InverseAncestorOrder();

    _freshClause.UsedRules(cl.rules());
    _freshClause.UsedRule(ClauseBackground::Reanimation);
    if (_parameters.outputStreamForGenerated) _freshClause.output(*_parameters.outputStreamForGenerated << "% GEN " << statNumOfGeneratedCl() << ' ') << '\n';

    _pagedPassive.popSelected();

    Clause* res = _freshClause.Reanimate();
    if (res)
      {
	res->SetNumber(_clauseNumberGenerator->generate());
	res->setMainSet(Clause::MainSetFree); 
	if (_parameters.useClauseNumIndex) _clauseNumIndex.insert(res);
	if (_parameters.showPassive) _parameters.showPassive(res);
      }; 
    _freshClause.SetMaxWeight(max_weight);

    return res;
  }; // Clause* Master::reanimate3(PagedPassiveClauseStorage::StoredClause& cl) 

  void Master::processCollected3()
  {
    CALL("processCollected3()");

    if (_parameters.outputStreamForGenerated) 
      _freshClause.output(*_parameters.outputStreamForGenerated << "% GEN " << statNumOfGeneratedCl() << ' ') << '\n';         
    Clause* answerClause;
    ulong numOfNewClauses;
    ulong numOfRejectedNewClauses;
    _freshClause.ProcessCollected3(answerClause,
				   _pagedPassive,
				   numOfNewClauses,
				   numOfRejectedNewClauses,
				   _clauseNumberGenerator,
				   (_parameters.showPassive != 0));
    _statClausesRefusedAllocation += numOfRejectedNewClauses;
    if (answerClause)  
      {
	rememberAnswer3(answerClause);
	_proof = answerClause;
	return; 
      };
  }; // void Master::processCollected3()



  void Master::makeActive3(Clause* cl)
  {
    CALL("makeActive3(Clause* cl)");
    cl->setMainSet(Clause::MainSetActive);
    cl->setFlag(Clause::FlagHasChildren); // too memory inefficient, must be changed 
    _active.Insert(cl);
    _infDispatcher.makeActive(cl);
    if (_parameters.forward_subsumption) integrateIntoForwardSubsumptionIndex(cl);
    if (_parameters.backward_subsumption) 
      { 
	integrateIntoBackwardSubsumptionIndex(cl);
      };
    if (_parameters.forward_demodulation > 0) integrateIntoForwardDemodulationIndex(cl);
    if (_parameters.backward_demodulation > 0) integrateIntoBackDemodulationIndex(cl);
    if (_parameters.showActive) _parameters.showActive(cl);
  }; // void Master::makeActive3(Clause* cl)


  void Master::rememberAnswer3(Clause* cl)
  {
    CALL("rememberAnswer3(Clause* cl)");  
    cl->setMainSet(Clause::MainSetAnswers);
    _answers.Insert(cl);
  }; // void Master::rememberAnswer3(Clause* cl)

  void Master::destroyPassive3()
  {
    CALL("destroyPassive3()");
    ASSERT(_passive.isEmpty());
    _pagedPassive.destroy();
    _pagedPassive.init();  
  }; // void Master::destroyPassive3()


  bool Master::releaseMemory3(ulong size)
  {
    CALL("releaseMemory3(ulong size)");
    if (size > PagedPassiveClauseStorage::pageSize()) 
      return false;
  try_free_page:
    if (_pagedPassive.recyclePage()) return true;
    if (_pagedPassive.cancelWorstClause()) goto try_free_page; 
    return false;
  }; // bool Master::releaseMemory3(ulong size) 


  void Master::killActiveOrphans3()
  {
    CALL("killActiveOrphans3()");
    _active.ResetIterator();
    for (Clause* cl = _active.Next(); cl; cl = _active.Next())
      if ((cl->Active())
	  && (!cl->IsVIP())
	  && (cl->IsOrphan()))
	{
	  enqueueForDisposal(cl);
	  _statActiveOrphansMurdered++;
	};
  }; // void Master::killActiveOrphans3()


//====================================================




