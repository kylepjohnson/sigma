//
// File:         Master.cpp
// Description:  Main loop.
// Created:      Mar 5, 2000, 20:00
// Author:       Alexandre Riazanov
// mail:         riazanov@cs.man.ac.uk
//====================================================
#include "Master.hpp"
#include "Clause.hpp"
#include "ClauseBackground.hpp"
#include "BuiltInTheoryDispatcher.hpp"
//====================================================
#undef DEBUG_NAMESPACE
#ifdef DEBUG_MASTER
#  define DEBUG_NAMESPACE "Master"
#endif                  
#include "debugMacros.hpp"
//====================================================


using namespace BK;
using namespace Gem;
using namespace VK;

/**********
Master::Master() :
  _continuation(Continuation_Init),
  _terminated(false),
  _proof(0),
  _retainedNewClauses(DOP("_retainedNewClauses")),
  _queueForDisposal(DOP("_queueForDisposal")),
  _queueForBackSimplification(DOP("_queueForBackSimplification")),
  _queueForIntegrationIntoBackwardDemodIndex(DOP("_queueForIntegrationIntoBackwardDemodIndex")),
  _infDispatcher(&_freshClause),
  _backSubsumption(&_clauseNumIndex),
  _backDemodulation(&_freshClause,&_clauseNumIndex),
  _clauseAssembler(&_sharingIndex,&_litSelection),

  _inferenceDispatcherSemaphore(false),
  _backwardSubsumptionSemaphore(false),
  _backwardDemodulationSemaphore(false),
  _reanimatedClause(0)
{
  CALL("constructor Master()");  
  _freshClause.SetClauseIndex(&_clauseAssembler);
  _freshClause.SetComponentName(&_componentName);
  _freshClause.SetForwardSubsumption(&_forwardSubsumption);  
  _freshClause.SetSimplificationByUEq(&_forwardDemodulation);
  setStrategy();
}; // Master::Master()

Master::~Master()
{
  CALL("destructor ~Master()");
  if (DestructionMode::isThorough())
    {	
      // sets of persistent clauses
      switch (_parameters.main_alg)
	{
	case 0: destroyPassive0(); break;
	case 3: destroyPassive3(); break;
	default: ICP("ICP0"); break; 
	}; 

      Clause* cl;
      while (_reservedPassive.Nonempty())
	{
	  cl = _reservedPassive.Top();
	  enqueueForDisposal(cl);
	};
      _reservedPassive.Purge(); 

      while (_active.Nonempty())
	{
	  cl = _active.Top();
	  enqueueForDisposal(cl);
	};
      _active.Purge(); 

      while (_answers.Nonempty())
	{
	  cl = _answers.Top();
	  enqueueForDisposal(cl);
	};
      _answers.Purge(); 
  
      ASSERT(_answers.clean());

      if (_reanimatedClause)
	{
	  ASSERT(_reanimatedClause->mainSet() == Clause::MainSetFree);
	  enqueueForDisposal(_reanimatedClause);
	};

      // queues
      Clause* retainedNewClause;

      while (_retainedNewClauses.dequeue(retainedNewClause)) 
	{ 

	  ASSERT(retainedNewClause->mainSet() == Clause::MainSetNew);
	  retainedNewClause->setMainSet(Clause::MainSetFree);
	  disposeClause(retainedNewClause);  
	};

      // queue for disposal 
      // not after _history.DestroyAllClauses()! 
      disposeObsolete();
      _history.DestroyAllClauses();  // not before disposeObsolete()!
    };
}; // Master::~Master()
***********/

    
void Master::init()
{
  CALL("init()");

  options.init();
  _timer.init();

  _continuation = Continuation_Init;
  _terminated = false;
  _proof = 0;

  _parameters.init();
  _unifier.init();
  _passive.init();
  _pagedPassive.init();
  _reservedPassive.init();
  _active.init();
  _history.init();
  _answers.init();
  _retainedNewClauses.init(DOP("_retainedNewClauses"));
  _queueForDisposal.init(DOP("_queueForDisposal"));
  _queueForBackSimplification.init(DOP("_queueForBackSimplification"));
  _queueForIntegrationIntoBackwardDemodIndex.init(DOP("_queueForIntegrationIntoBackwardDemodIndex"));
  _infDispatcher.init(&_freshClause);
  _forwardSubsumption.init();
  _backSubsumption.init(&_clauseNumIndex);
  _forwardDemodulation.init();
  _backDemodulation.init(&_freshClause,&_clauseNumIndex);
  _componentName.init();
  _sharingIndex.init();
  _litSelection.init();
  _litOrdering.init();
  _clauseAssembler.init(&_sharingIndex,&_litSelection);

  _inferenceDispatcherSemaphore = false;
  _backwardSubsumptionSemaphore = false;
  _backwardDemodulationSemaphore = false;

  _freshClause.init();
  _clauseNumIndex.init();

  _reanimatedClause = 0;
  _freshClause.SetClauseIndex(&_clauseAssembler);
  _freshClause.SetComponentName(&_componentName);
  _freshClause.SetForwardSubsumption(&_forwardSubsumption);  
  _freshClause.SetSimplificationByUEq(&_forwardDemodulation); 
  setStrategy();
}; // void init()


void Master::destroy()
{
  CALL("destroy()");
  if (DestructionMode::isThorough())
    {	
      // sets of persistent clauses
      switch (_parameters.main_alg)
	{
	case 0: destroyPassive0(); break;
	case 3: destroyPassive3(); break;
	default: ICP("ICP0"); break; 
	}; 

      Clause* cl;
      while (_reservedPassive.Nonempty())
	{
	  cl = _reservedPassive.Top();
	  enqueueForDisposal(cl);
	};
      _reservedPassive.Purge(); 

      while (_active.Nonempty())
	{
	  cl = _active.Top();
	  enqueueForDisposal(cl);
	};
      _active.Purge(); 

      while (_answers.Nonempty())
	{
	  cl = _answers.Top();
	  enqueueForDisposal(cl);
	};
      _answers.Purge(); 
  
      ASSERT(_answers.clean());

      if (_reanimatedClause)
	{
	  ASSERT(_reanimatedClause->mainSet() == Clause::MainSetFree);
	  enqueueForDisposal(_reanimatedClause);
	};

      // queues
      Clause* retainedNewClause;

      while (_retainedNewClauses.dequeue(retainedNewClause)) 
	{ 

	  ASSERT(retainedNewClause->mainSet() == Clause::MainSetNew);
	  retainedNewClause->setMainSet(Clause::MainSetFree);
	  disposeClause(retainedNewClause);  
	};

      // queue for disposal 
      // not after _history.DestroyAllClauses()! 
      disposeObsolete();
      _history.DestroyAllClauses();  // not before disposeObsolete()!
    };
    
  _clauseNumIndex.destroy();
  _freshClause.destroy();
  _clauseAssembler.destroy();
  _litOrdering.destroy();
  _litSelection.destroy();
  _sharingIndex.destroy();
  _componentName.destroy();
  _backDemodulation.destroy();
  _forwardDemodulation.destroy();
  _backSubsumption.destroy();
  _forwardSubsumption.destroy();
  _infDispatcher.destroy();
  _queueForIntegrationIntoBackwardDemodIndex.destroy();
  _queueForBackSimplification.destroy();
  _queueForDisposal.destroy();
  _retainedNewClauses.destroy();
  _answers.destroy();
  _history.destroy();
  _active.destroy();
  _reservedPassive.destroy();
  _pagedPassive.destroy();
  _passive.destroy();
  _unifier.destroy();
  _parameters.destroy();
  _timer.destroy();
  options.destroy();
  BK_CORRUPT(*this);
}; // void Master::destroy()

void Master::reset() 
{
  CALL("reset()");
  _unifier.reset();
  Unifier::setCurrent(&_unifier);

  // sets of persistent clauses
  switch (_parameters.main_alg)
    {
    case 0: destroyPassive0(); break;
    case 3: destroyPassive3(); break;
    default: ICP("ICP0"); break; 
    }; 

  Clause* cl;
  while (_reservedPassive.Nonempty())
    {
      cl = _reservedPassive.Top();
      enqueueForDisposal(cl);
    };
  _reservedPassive.Purge(); // to make it clean()

  while (_active.Nonempty())
    {
      cl = _active.Top();
      enqueueForDisposal(cl);
    };
  _active.Purge(); // to make it clean()


  while (_answers.Nonempty())
    {
      cl = _answers.Top();
      enqueueForDisposal(cl);
    };
  _answers.Purge(); // to make it clean()
  
  ASSERT(_answers.clean());

  if (_reanimatedClause)
    {
      ASSERT(_reanimatedClause->mainSet() == Clause::MainSetFree);
      enqueueForDisposal(_reanimatedClause);
    };

  // queues
  Clause* retainedNewClause;

  while (_retainedNewClauses.dequeue(retainedNewClause)) 
    { 

      ASSERT(retainedNewClause->mainSet() == Clause::MainSetNew);
      retainedNewClause->setMainSet(Clause::MainSetFree);
      disposeClause(retainedNewClause);  
    };

  // queue for disposal 
  // not after _history.DestroyAllClauses()! 
  disposeObsolete();
  _history.DestroyAllClauses();  // not before disposeObsolete()!

  _queueForBackSimplification.reset();
  _queueForIntegrationIntoBackwardDemodIndex.reset();

  // indices and inference engines   


  _infDispatcher.reset();
  _forwardSubsumption.reset();
  _backSubsumption.reset();
  _forwardDemodulation.reset();
  _backDemodulation.reset();
  _sharingIndex.reset();
  _clauseAssembler.reset();    
  _clauseNumIndex.reset();

  options.reset(); 
  resetStatistics();
  _reanimatedClause = 0;
  setStrategy();
}; //  void Master::reset() 



void Master::endOfInput()
{
  CALL("endOfInput()");
  // destroy obsolete clauses
  ASSERT(_retainedNewClauses.empty());
  ASSERT(_queueForBackSimplification.empty());
  ASSERT(_queueForIntegrationIntoBackwardDemodIndex.empty());

  disposeObsolete(); 

  ASSERT(_queueForDisposal.empty());

}; // void Master::endOfInput()


void Master::mainLoop0()
{
  CALL("mainLoop0()");

  Clause* current;

  switch (_continuation)
    {
    case Continuation_Init:
      break;
    case Continuation_MainLoop0_ProcessNewInference:
      cancelTermination();
      _inferenceDispatcherSemaphore = true;
      goto process_new_inference;
    case Continuation_MainLoop0_BackSimplify:
      cancelTermination();
      _inferenceDispatcherSemaphore = true;
      goto back_simplify;
    default:
      ICP("ICP0");
      RuntimeError::report("Bad _continuation in VK::Master::mainLoop0()");
    };

  ASSERT(!_terminated);
  ASSERT(!_proof);

  _timer.reset(); 
  _timer.start();
  _timerIsOn = true;
  _externalTickFlag = false;
  _startMemory = (float)GlobAlloc::occupiedByObjects(); // mainly for LRS on memory


 select_current: 
  current = select0(); 
  if (!current) 
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
      && (!current->IsVIP())
      && (current->IsOrphan()))
    {      
      _statPassiveOrphansMurdered++;
      disposeClause(current);  
      estimateResources();
      goto select_current;
    }; 
  
  makeActive0(current); 

  eliminateAllocationBufferDeficit0();  

  _inferenceDispatcherSemaphore = true;
  _infDispatcher.newQuery(current);
 next_inference:
  if (_infDispatcher.nextInference())
    {
    process_new_inference:
      processCollected0();
      if (_proof) 
	{ 
	  terminate(Proof); 
	  _inferenceDispatcherSemaphore = false; 
	  _timer.stop();
	  _timerIsOn = false;
	  _continuation = Continuation_MainLoop0_ProcessNewInference;
	  return; 
	};
      _continuation = Continuation_Init;

      if (_passive.Total() >= _parameters.maxPassive) 
	{ 
	  terminate(MaxPassive);  
	  _inferenceDispatcherSemaphore = false; 
	  _timer.stop();
	  _timerIsOn = false;
	  return; 
	};

      internalTick0();

    back_simplify:
      backSimplify();
      if (_terminated) 
	{ 
	  _inferenceDispatcherSemaphore = false; 
	  _timer.stop();
	  _timerIsOn = false;
	  _continuation = Continuation_MainLoop0_BackSimplify; 
	  return; 
	};
      _continuation = Continuation_Init;
      integrateIntoBackDemIndexFromQueue();
      goto next_inference;
    };
  // no more inferences with current
  _inferenceDispatcherSemaphore = false;
  
  internalTick0(); 
    

  // destroy obsolete clauses
  ASSERT(_retainedNewClauses.empty());
  ASSERT(_queueForBackSimplification.empty());
  ASSERT(_queueForIntegrationIntoBackwardDemodIndex.empty());

  disposeObsolete(); 
  ASSERT(_queueForDisposal.empty());
  estimateResources();
  if ((_parameters.lrs == 2) || (_parameters.lrs == 3))
    {
      recycleUnreachablePassive0();
    };  
  goto select_current; 
}; // void Master::mainLoop0()

void Master::backSimplify()
{
  CALL("backSimplify()");  
  Clause* backSimplified;
  bool someActiveSimplified;
  switch (_continuation)
    {
    case Continuation_Init:
      break;
    case Continuation_InputQueryClause0_BackSimplify:
      // same as below
    case Continuation_EliminateDefinitions_BackSimplify:
      // same as below
    case Continuation_MainLoop0_BackSimplify:
      someActiveSimplified = true;
      goto process_new_inference;
    default:
      ICP("ICP0");
      RuntimeError::report("Bad _continuation in VK::Master::backSimplify()");
    };

  ASSERT((_parameters.backward_demodulation > 0) || 
	 _queueForBackSimplification.empty());
  _backwardDemodulationSemaphore = true;
  someActiveSimplified = false;
  if (_parameters.backward_demodulation > 0)
    {
    next_simplifier:
      if (_queueForBackSimplification.dequeue(_currentSimplifier))
	{ 
	  ASSERT(_currentSimplifier->flag(Clause::FlagInQueueForBackSimplification));
	  _currentSimplifier->clearFlag(Clause::FlagInQueueForBackSimplification);
	  if (backDemodulationQuery(_currentSimplifier))
	    { 
	    next_back_demodulated:
	      if (nextBackDemodulated())
		{         
		  _statBackSimplified++;
         
		  if (collectBackSimplified())
		    {   
		      _currentSimplifier->setFlag(Clause::FlagHasChildren);
		      backSimplified = _backDemodulation.simplified();
		      
		      someActiveSimplified = someActiveSimplified 
			|| (backSimplified->mainSet() == Clause::MainSetActive) 
			|| (backSimplified->mainSet() == Clause::MainSetDefinitions); 

		      backSimplified->setFlag(Clause::FlagHasChildren);

		      enqueueForDisposal(backSimplified);        

		    process_new_inference:
		      processCollected0(); // not before enqueueForDisposal(backSimplified)!

		      if (_proof) 
			{ 
			  terminate(Proof); 
			  return; 
			};
		            
		      _continuation = Continuation_Init;

		      if (_passive.Total() >= _parameters.maxPassive) 
			{ 
			  terminate(MaxPassive); 
			  _backwardDemodulationSemaphore = false; 
			  return; 
			}; 
		    }
		  else
		    {
		      if (_freshClause.WasTautology())
			{
			  backSimplified = _backDemodulation.simplified();
			  
			  enqueueForDisposal(backSimplified);
			};
		    };
		  internalTick0();
		  goto next_back_demodulated;
		}; // if (nextBackDemodulated()) 
	    };
	  
	  goto next_simplifier;
	}; // if (_queueForBackSimplification.dequeue(_currentSimplifier))
      if (_parameters.orphan_murder && someActiveSimplified) killActiveOrphans0();
    };
  _backwardDemodulationSemaphore = false;
}; // void Master::backSimplify()


void Master::eliminateDefinitions()
{
  CALL("Master::eliminateDefinitions()");
  ASSERT(!_parameters.main_alg);

  switch (_continuation)
    {
    case Continuation_Init:
      break;
    case Continuation_EliminateDefinitions_ProcessCollectedInput:
      cancelTermination();
      _inferenceDispatcherSemaphore = true;
      goto process_collected_input;
    case Continuation_EliminateDefinitions_BackSimplify:
      cancelTermination();
      _inferenceDispatcherSemaphore = true;
      goto back_simplify;

    default:
      ICP("ICP0");
      RuntimeError::report("Bad _continuation in VK::Master::eliminateDefinitions()");
    };

  _timer.reset();
  _timer.start();
  Clause* cl;
 select:
  _passive.ResetIterator();
 try_next:
  cl = _passive.IterNext();
  if (cl) 
    {
      if (cl->ContainsEliminated(_parameters.pred_elim_table))
	{
	  _passive.Remove(cl);
      
	  // perform resolutions
	  makeActive0(cl);
	  _inferenceDispatcherSemaphore = true;
	  _infDispatcher.newQuery(cl);
	check_next_inference:
	  if (_infDispatcher.nextInference())
	    {
	      _freshClause.UsedRule(ClauseBackground::Prepro);
	    process_collected_input:
	      processCollectedInputQuery0();
	      if (_proof) 
		{ 
		  terminate(Proof); 
		  _inferenceDispatcherSemaphore = false; 
		  _continuation = Continuation_EliminateDefinitions_ProcessCollectedInput;
		  _timer.stop();
		  _timerIsOn = false;
		  return;
		};
	      _continuation = Continuation_Init;

	    back_simplify:
	      backSimplify();
	      if (_terminated) 
		{ 
		  _inferenceDispatcherSemaphore = false; 
		  _continuation = Continuation_EliminateDefinitions_BackSimplify;    
		  _timer.stop();
		  _timerIsOn = false;
		  return;
		};
	      _continuation = Continuation_Init;
	      goto check_next_inference;
	    };
	  _inferenceDispatcherSemaphore = false;
	  // destroy obsolete clauses
	  ASSERT(_retainedNewClauses.empty());
	  ASSERT(_queueForBackSimplification.empty());
	  ASSERT(_queueForIntegrationIntoBackwardDemodIndex.empty());
	  disposeObsolete(); 

	  // use next
	  goto select;     
	}
      else goto try_next;      
    };

  // all done, collect results
  ASSERT(_retainedNewClauses.empty());
  ASSERT(!_proof); 
  ASSERT(_queueForBackSimplification.empty());
  ASSERT(_queueForIntegrationIntoBackwardDemodIndex.empty());
  ASSERT(_queueForBackSimplification.empty());
  ASSERT(_queueForIntegrationIntoBackwardDemodIndex.empty());

  // all _active are obsolete now, make them _history
  Clause* act;
  while (_active.Nonempty())
    {
      act = _active.Top();  
      enqueueForDisposal(act);
    };
  disposeObsolete();    
  _timer.stop();
  _timerIsOn = false;
}; // void Master::eliminateDefinitions()




void Master::inputClause0(Clause* cl)
{
  CALL("inputClause0(Clause* cl)");
  if (cl->isSupportClause())
    {
      inputSupportClause0(cl);
    }
  else
    inputQueryClause0(cl);
}; // void Master::inputClause0(Clause* cl)

void Master::inputSupportClause0(Clause* cl)
{
  CALL("inputSupportClause0(Clause* cl)");
  ASSERT(cl->isSupportClause());
  switch (_continuation)
    {
    case Continuation_Init:
      break;
    case Continuation_InputSupportClause0_ProcessCollectedInput:
      cancelTermination();
      goto process_collected_input;
    default:
      ICP("ICP0");
      RuntimeError::report("Bad _continuation in VK::Master::inputQueryClause0(Clause* cl)");
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
    process_collected_input:
      processCollectedInputSupport0();  
      if (_proof) 
	{  
	  terminate(Proof); 
	  ASSERT(_retainedNewClauses.empty());    
	  //disposeObsolete();  
	  _timer.stop();
	  _timerIsOn = false;
	  _continuation = Continuation_InputSupportClause0_ProcessCollectedInput;
	  return; 
	};  
      _continuation = Continuation_Init;
      integrateIntoBackDemIndexFromQueue(); 
    };
  _timer.stop();
  _timerIsOn = false;
}; // void Master::inputSupportClause0(Clause* cl)


void Master::inputQueryClause0(Clause* cl)
{
  CALL("inputQueryClause0(Clause* cl)");
  ASSERT(!cl->isSupportClause());
  switch (_continuation)
    {
    case Continuation_Init:
      break;
    case Continuation_InputQueryClause0_ProcessCollectedInput:
      cancelTermination();
      goto process_collected_input;
    case Continuation_InputQueryClause0_BackSimplify:
      cancelTermination();
      goto back_simplify;
    default:
      ICP("ICP0");
      RuntimeError::report("Bad _continuation in VK::Master::inputQueryClause0(Clause* cl)");
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
      _passive.MaxPriorityOn(); 
    process_collected_input:
      processCollectedInputQuery0(); 
      if (_proof) 
	{  
	  terminate(Proof); 
	  ASSERT(_retainedNewClauses.empty());    
	  //disposeObsolete();  
	  _timer.stop();
	  _timerIsOn = false;
	  _continuation = Continuation_InputQueryClause0_ProcessCollectedInput;
	  return; 
	};  
      _continuation = Continuation_Init;
      _passive.MaxPriorityOff(); 
      
    back_simplify:
      backSimplify();
      if (_terminated)
	{
	  _timer.stop();
	  _timerIsOn = false;
	  _continuation = Continuation_InputQueryClause0_BackSimplify;
	  return; 
	};
      ASSERT(_queueForBackSimplification.empty());
      _continuation = Continuation_Init;
      integrateIntoBackDemIndexFromQueue();
    };
  _timer.stop();
  _timerIsOn = false;
}; // void Master::inputQueryClause0(Clause* cl)

void Master::processCollected0()
{
  CALL("processCollected0()");
  Clause* newClause;
  switch (_continuation)
    {
    case Continuation_Init:
      break;
    case Continuation_InputQueryClause0_BackSimplify:
      // same as below
    case Continuation_EliminateDefinitions_BackSimplify:
      // same as below
    case Continuation_MainLoop0_ProcessNewInference:
      // same as below
    case Continuation_MainLoop0_BackSimplify:
      goto process_retained;
    default:
      ICP("ICP0");
      RuntimeError::report("Bad _continuation in VK::Master::processCollected0()");
    };

  if (_parameters.outputStreamForGenerated) _freshClause.output(*_parameters.outputStreamForGenerated << "% GEN " << statNumOfGeneratedCl() << ' ') << '\n';         
  _freshClause.ProcessCollected(_retainedNewClauses);


 process_retained:  
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

      makePassive0(newClause);
      if (newClause->weight() > _parameters.maxWeight)
	{        

	  enqueueForDisposal(newClause);
	  _statRecycled++;

	}
      else
	{
	  if (_parameters.backward_subsumption) 
	    if (backSubsumeBy(newClause))
	      if (_parameters.orphan_murder) 
		killActiveOrphans0();

	  if (_parameters.backward_demodulation > 0) 
	    {
	      ASSERT(!newClause->flag(Clause::FlagInQueueForBackSimplification));

	      _queueForBackSimplification.enqueue(newClause);

	      newClause->setFlag(Clause::FlagInQueueForBackSimplification);
	    };

	  integratePassive(newClause);

	  eliminateAllocationBufferDeficit0();
	};
    };
}; // void Master::processCollected0()

void Master::processCollectedInputSupport0()
{
  CALL("processCollectedInputSupport0()");
  Clause* newClause;  

  switch (_continuation)
    {
    case Continuation_Init:
      break;
    case Continuation_InputSupportClause0_ProcessCollectedInput:
      goto process_next_retained_clause;
    default:
      ICP("ICP0");
      RuntimeError::report("Bad _continuation in VK::Master::processCollectedInputSupport0()");
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
}; // void Master::processCollectedInputSupport0()

void Master::processCollectedInputQuery0()
{
  CALL("processCollectedInputQuery0()");
  Clause* newClause;  

  switch (_continuation)
    {
    case Continuation_Init:
      break;
    case Continuation_InputQueryClause0_ProcessCollectedInput:
      // same as below
    case Continuation_EliminateDefinitions_ProcessCollectedInput:
      goto process_next_retained_clause;
    default:
      ICP("ICP0");
      RuntimeError::report("Bad _continuation in VK::Master::processCollectedInputQuery0()");
    };


  if (_parameters.outputStreamForGenerated) 
    _freshClause.output(*_parameters.outputStreamForGenerated << "% GEN " 
			<< statNumOfGeneratedCl() << ' ') << '\n';
  _freshClause.ProcessCollected(_retainedNewClauses);

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

      makePassive0(newClause); 
      if (_parameters.backward_subsumption) backSubsumeBy(newClause);
      if (_parameters.backward_demodulation > 0) 
	{
	  ASSERT(!newClause->flag(Clause::FlagInQueueForBackSimplification));
	  _queueForBackSimplification.enqueue(newClause);
	  newClause->setFlag(Clause::FlagInQueueForBackSimplification);
	}; 
      integratePassive(newClause);
    };
  ASSERT(_retainedNewClauses.empty());
}; // void Master::processCollectedInputQuery0()



Clause* Master::select0()
{
  CALL("select0()");
  Clause* selected;
 try_next_passive:
  selected = _passive.Next();
  if (!selected)
    {// _passive is empty, try _reservedPassive 
      _parameters.lrs = 0;
      _parameters.maxWeight = VampireKernelConst::MaxClauseSize;  
      _freshClause.SetMaxWeight(_parameters.maxWeight);  
      if ((_parameters.showWeightChange) 
	  && (_freshClause.PreviousMaxWeight() != _freshClause.MaxWeight()))
	_parameters.showWeightChange(_freshClause.PreviousMaxWeight(),_freshClause.MaxWeight());    
      loadReservedClauses();    
      selected = _passive.Next();
      if (selected) 
	{
	  ASSERT(selected->mainSet() == Clause::MainSetPassive);
	  selected->setMainSet(Clause::MainSetFree);
	};  
      return selected;
    };
  if (selected->MinResWeight() > _parameters.maxWeight)
    { 
      makeReserved(selected); 
      goto try_next_passive;         
    };
  if (selected)     
    {
      ASSERT(selected->mainSet() == Clause::MainSetPassive);
      selected->setMainSet(Clause::MainSetFree);
    }; 
  return selected; 
}; // Clause* Master::select0()  

void Master::makeActive0(Clause* cl)
{
  CALL("makeActive0(Clause* cl)");
  //cl->SetStore(CS_USED);  
  cl->setMainSet(Clause::MainSetActive);
  cl->setFlag(Clause::FlagHasChildren); // too memory inefficient, must be changed 
  _active.Insert(cl);
  _infDispatcher.makeActive(cl);
  if (_parameters.showActive) _parameters.showActive(cl); 
}; // void Master::makeActive0(Clause* cl)

void Master::makePassive0(Clause* cl)
{
  CALL("makePassive0(Clause* cl)");
  cl->SetNumber(_clauseNumberGenerator->generate());
  INTERCEPT_BACKWARD_SUBSUMPTION_INDEXING_INTRODUCE_CLAUSE_ID(cl);
  cl->setMainSet(Clause::MainSetPassive);
  _passive.Insert(cl);
  if (_parameters.useClauseNumIndex) _clauseNumIndex.insert(cl);
  if (_parameters.showPassive) _parameters.showPassive(cl);
}; // void Master::makePassive0(Clause* cl)


void Master::rememberAnswer0(Clause* cl)
{
  CALL("rememberAnswer0(Clause* cl)");
  cl->SetNumber(_clauseNumberGenerator->generate());
  cl->setMainSet(Clause::MainSetAnswers);
  _answers.Insert(cl);
  if (_parameters.useClauseNumIndex) _clauseNumIndex.insert(cl);
  if (_parameters.showPassive) _parameters.showPassive(cl);
}; // void Master::rememberAnswer0(Clause* cl)

void Master::integratePassive(Clause* cl)
{
  CALL("integratePassive(Clause* cl)");
  // see also removeFromIndices(Clause* cl)

  if (_parameters.forward_subsumption) integrateIntoForwardSubsumptionIndex(cl);
  if (_parameters.backward_subsumption) integrateIntoBackwardSubsumptionIndex(cl);
  if (_parameters.forward_demodulation > 0) integrateIntoForwardDemodulationIndex(cl);
  if (_parameters.backward_demodulation > 0) 
    {
      if (_backwardDemodulationSemaphore)
	{
	  _queueForIntegrationIntoBackwardDemodIndex.enqueue(cl);
	  cl->setFlag(Clause::FlagInQueueForIntegrationIntoBackwardDemodIndex);
	}
      else 
	{
	  cl->clearFlag(Clause::FlagInQueueForIntegrationIntoBackwardDemodIndex);
	  integrateIntoBackDemodulationIndex(cl);
	};
    };
}; // void Master::integratePassive(Clause* cl) 


void Master::integrateIntoBackDemIndexFromQueue()
{
  CALL("integrateIntoBackDemIndexFromQueue()");
  ASSERT(!_backwardDemodulationSemaphore);
  Clause* cl;
  while (_queueForIntegrationIntoBackwardDemodIndex.dequeue(cl))
    { 
      ASSERT(cl->flag(Clause::FlagInQueueForIntegrationIntoBackwardDemodIndex));
      ASSERT(cl->mainSet() == Clause::MainSetPassive);
      cl->clearFlag(Clause::FlagInQueueForIntegrationIntoBackwardDemodIndex);
      integrateIntoBackDemodulationIndex(cl);
    };
}; // void Master::integrateIntoBackDemIndexFromQueue()






void Master::removeFromIndices(Clause* cl)
{
  CALL("removeFromIndices(Clause* cl)");
  // see also integratePassive(Clause* cl)
  if (cl->flag(Clause::FlagInBackDemodulationIndex)) removeFromBackDemodulationIndex(cl);
  if (cl->flag(Clause::FlagInForwardDemodulationIndex)) removeFromForwardDemodulationIndex(cl);
  if (cl->flag(Clause::FlagInBackwardSubsumptionIndex)) removeFromBackwardSubsumptionIndex(cl);
  if (cl->flag(Clause::FlagInForwardSubsumptionIndex)) removeFromForwardSubsumptionIndex(cl);
}; // void Master::removeFromIndices(Clause* cl)

void Master::makeReserved(Clause* cl)
{
  CALL("makeReserved(Clause* cl)");
  cl->setMainSet(Clause::MainSetReservedPassive);
  _reservedPassive.Insert(cl);
}; // void Master::makeReserved(Clause* cl)

void Master::enqueueForDisposal(Clause* cl)
{
  CALL("enqueueForDisposal(Clause* cl)"); 
  if (cl->mainSet() == Clause::MainSetTrash) return;
  switch (cl->mainSet()) 
    {
    case Clause::MainSetFree: break;
    case Clause::MainSetNew:
      ICP("ICP0");
      break;
    case Clause::MainSetPassive: _passive.Remove(cl); break;
    case Clause::MainSetReservedPassive: _reservedPassive.Remove(cl); break;    
    case Clause::MainSetActive: _active.Remove(cl); break;
    case Clause::MainSetAnswers: _answers.Remove(cl); break;
#ifdef DEBUG_NAMESPACE
    default: 
      ICP("ICP1"); 
      return;
#else
    default: 
      RuntimeError::report("Wrong clause affiliation.");
#endif
    };

 
  if (cl->flag(Clause::FlagInQueueForBackSimplification)) 
    {
      _queueForBackSimplification.removeFirst(cl); 
      cl->clearFlag(Clause::FlagInQueueForBackSimplification);
    };
  if (cl->flag(Clause::FlagInQueueForIntegrationIntoBackwardDemodIndex)) 
    {
      _queueForIntegrationIntoBackwardDemodIndex.removeFirst(cl); 
      cl->clearFlag(Clause::FlagInQueueForIntegrationIntoBackwardDemodIndex);
    };

  bool canBeDestroyed = true;
  if (!_inferenceDispatcherSemaphore)
    {
      if (cl->flag(Clause::FlagInInferenceDispatcherAsActive)) _infDispatcher.remove(cl);
      if (cl->flag(Clause::FlagInInferenceDispatcherAsDefinition)) _infDispatcher.removeDefinition(cl);
    }
  else
    {
      if ((cl->flag(Clause::FlagInInferenceDispatcherAsActive)) 
	  || (cl->flag(Clause::FlagInInferenceDispatcherAsDefinition)))
	canBeDestroyed = false;
    };

  if (!_backwardDemodulationSemaphore)
    {
      if (cl->flag(Clause::FlagInBackDemodulationIndex)) removeFromBackDemodulationIndex(cl); 
    }
  else
    if (cl->flag(Clause::FlagInBackDemodulationIndex)) canBeDestroyed = false;

  if (cl->flag(Clause::FlagInForwardDemodulationIndex)) removeFromForwardDemodulationIndex(cl);

  if (!_backwardSubsumptionSemaphore)
    {
      if (cl->flag(Clause::FlagInBackwardSubsumptionIndex)) removeFromBackwardSubsumptionIndex(cl);
    }
  else if (cl->flag(Clause::FlagInBackwardSubsumptionIndex)) canBeDestroyed = false;

  if (cl->flag(Clause::FlagInForwardSubsumptionIndex)) removeFromForwardSubsumptionIndex(cl);
 
  if (canBeDestroyed)
    {
      if (cl->flag(Clause::FlagHasChildren))
	{

	  // must be in history
	  if (cl->flag(Clause::FlagInClauseIndex)) 
	    {
	      //_clauseAssembler.removeFromSharing(cl,false);  
	      cl->clearFlag(Clause::FlagInClauseIndex);
	    };
	  cl->setMainSet(Clause::MainSetHistory); 
	  _history.Insert(cl); 
	}
      else // can be completely destroyed
	{
      

	  bool shared = cl->flag(Clause::FlagLiteralsShared);
	  if (cl->flag(Clause::FlagInClauseIndex)) 
	    {
	      if (shared) _clauseAssembler.removeFromSharing(cl);
	      cl->clearFlag(Clause::FlagInClauseIndex);
	      cl->clearFlag(Clause::FlagLiteralsShared);
	    };
	  if (cl->flag(Clause::FlagInClauseNumIndex)) _clauseNumIndex.remove(cl);
	  if (shared) { cl->Destroy(); } else cl->DestroyUnshared();
	};

    }
  else // must wait in the queue
    {
      cl->setMainSet(Clause::MainSetTrash);
      _queueForDisposal.enqueue(cl); 
    };
}; // void Master::enqueueForDisposal(Clause* cl)

void Master::disposeObsolete()
{
  CALL("disposeObsolete()"); 
  Clause* disposedClause;
  while (_queueForDisposal.dequeue(disposedClause))
    { 
      ASSERT(disposedClause->mainSet() == Clause::MainSetTrash);
      disposedClause->setMainSet(Clause::MainSetFree);
      disposeClause(disposedClause);
    };
}; // void Master::disposeObsolete()

void Master::disposeClause(Clause* cl)
{
  CALL("disposeClause(Clause* cl)");
  ASSERT(cl->mainSet() == Clause::MainSetFree);
  ASSERT((!_inferenceDispatcherSemaphore) || ((!cl->flag(Clause::FlagInInferenceDispatcherAsActive)) && (!cl->flag(Clause::FlagInInferenceDispatcherAsDefinition))));
  if (cl->flag(Clause::FlagInInferenceDispatcherAsActive)) _infDispatcher.remove(cl);
  if (cl->flag(Clause::FlagInInferenceDispatcherAsDefinition)) _infDispatcher.removeDefinition(cl);
  removeFromIndices(cl);
  ASSERT((_parameters.main_alg == 2) || (_parameters.main_alg == 3) || (cl->flag(Clause::FlagInClauseIndex)));
  ASSERT((_parameters.main_alg == 2) || (_parameters.main_alg == 3) || (cl->flag(Clause::FlagLiteralsShared)));


  ASSERT(!cl->flag(Clause::FlagInQueueForBackSimplification));
  ASSERT(!cl->flag(Clause::FlagInQueueForIntegrationIntoBackwardDemodIndex));

  if (cl->flag(Clause::FlagHasChildren))
    {
      // must be in history
      if (cl->flag(Clause::FlagInClauseIndex)) 
	{
	  //_clauseAssembler.removeFromSharing(cl,false);  
	  cl->clearFlag(Clause::FlagInClauseIndex);
	};
      cl->setMainSet(Clause::MainSetHistory); 
      _history.Insert(cl); 

    }
  else // can be completely destroyed
    {
      bool shared = cl->flag(Clause::FlagLiteralsShared);
      if (cl->flag(Clause::FlagInClauseIndex)) 
	{
	  if (shared) _clauseAssembler.removeFromSharing(cl);
	  cl->clearFlag(Clause::FlagInClauseIndex);
	  cl->clearFlag(Clause::FlagLiteralsShared);
	};
      if (cl->flag(Clause::FlagInClauseNumIndex)) _clauseNumIndex.remove(cl);
      if (shared) { cl->Destroy(); } else cl->DestroyUnshared();

    };

}; // void Master::disposeClause(Clause* cl)


void Master::loadReservedClauses()
{
  CALL("loadReservedClauses()");
  Clause* cl;
  _reservedPassive.ResetIterator();
  while (!_reservedPassive.Empty())
    {
      cl = _reservedPassive.Pop();
      ASSERT(cl->mainSet() == Clause::MainSetReservedPassive); 
      cl->setMainSet(Clause::MainSetPassive);
      _passive.Insert(cl); 
    };
}; // void Master::loadReservedClauses()

bool Master::collectBackSimplified()
{
  CALL("collectBackSimplified()");
  return _backDemodulation.collectInference();  
}; // bool Master::collectBackSimplified()


bool Master::backSubsumeBy(Clause* cl)
{
  CALL("backSubsumeBy(Clause* cl)");
  _backwardSubsumptionSemaphore = true;
  Clause* subsumedClause;
  bool usedSubsumed = false;
  if (_backSubsumption.query(cl))
    {
      while (_backSubsumption.subsumeNext())
	{ 
	  _statBackSubsumed++;
	  cl->MakeVIP();
	  subsumedClause = _backSubsumption.subsumed();
	  ASSERT(subsumedClause != cl);
	  ASSERT(subsumedClause->Active()); 
	  usedSubsumed = (usedSubsumed 
			  || (subsumedClause->mainSet() == Clause::MainSetActive) 
			  || (subsumedClause->mainSet() == Clause::MainSetDefinitions));     
	  enqueueForDisposal(subsumedClause);     
	};
    };
  _backwardSubsumptionSemaphore = false;
  return (bool)usedSubsumed;
}; // bool Master::backSubsumeBy(Clause* cl)

void Master::estimateResources1()
{
  CALL("estimateResources1()");
  ASSERT(_parameters.lrs);
  float used_memory = usedMemory();
  float used_time = _timer.elapsedDeciseconds();

  bool noTimeCheck = used_time < _parameters.timeLimit * _parameters.first_time_check;
  bool noMemoryCheck = used_memory < _parameters.memoryLimit * _parameters.first_mem_check;

  if (noTimeCheck && noMemoryCheck) return;

  long moved_from_passive = _passive.UsedAndRemoved();   
  long reachableByTime = (noTimeCheck)? 
    LONG_MAX 
    : 
    (long)(((_parameters.timeLimit - used_time)*moved_from_passive)/used_time);

  long reachableByMemory = (noMemoryCheck)? 
    LONG_MAX
    :
    (long)(((_parameters.memoryLimit - used_memory)*moved_from_passive)/used_memory);
  _parameters.reachable = (reachableByTime < reachableByMemory) ? reachableByTime : reachableByMemory;
  _passive.SetNumOfReachable1(_parameters.reachable);
  _parameters.maxWeight = _passive.MaxAcceptedWeight();
  if (_parameters.maxWeight > _parameters.maxMaxWeight) 
    {
      _parameters.maxWeight = _parameters.maxMaxWeight;
    };
  _freshClause.SetMaxWeight(_parameters.maxWeight);
  if ((_parameters.showWeightChange) 
      && (_freshClause.PreviousMaxWeight() != _freshClause.MaxWeight()))
    _parameters.showWeightChange(_freshClause.PreviousMaxWeight(),_freshClause.MaxWeight()); 

  if (_parameters.showUsagePace) _parameters.showUsagePace(moved_from_passive/used_time);
 
}; // void Master::estimateResources1()



void Master::estimateResources2()
{
  CALL("estimateResources2()");
  ASSERT(_parameters.lrs);
  float used_memory = usedMemory();
  float used_time = _timer.elapsedDeciseconds();

  bool noTimeCheck = used_time < _parameters.timeLimit * _parameters.first_time_check;
  bool noMemoryCheck = used_memory < _parameters.memoryLimit * _parameters.first_mem_check;

  if (noTimeCheck && noMemoryCheck) return;

  long moved_from_passive = _passive.UsedAndRemoved();   
  long reachableByTime = (noTimeCheck)? 
    LONG_MAX 
    : 
    (long)(((_parameters.timeLimit - used_time)*moved_from_passive)/used_time);

  long reachableByMemory = (noMemoryCheck)? 
    LONG_MAX
    :
    (long)(((_parameters.memoryLimit - used_memory)*moved_from_passive)/used_memory);
  _parameters.reachable = (reachableByTime < reachableByMemory) ? reachableByTime : reachableByMemory;
  _passive.SetNumOfReachable2(_parameters.reachable);
  _parameters.maxWeight = _passive.MaxAcceptedWeight();
  if (_parameters.maxWeight > _parameters.maxMaxWeight) 
    {
      _parameters.maxWeight = _parameters.maxMaxWeight;
    };
  _freshClause.SetMaxWeight(_parameters.maxWeight);
  if ((_parameters.showWeightChange) 
      && (_freshClause.PreviousMaxWeight() != _freshClause.MaxWeight()))
    _parameters.showWeightChange(_freshClause.PreviousMaxWeight(),_freshClause.MaxWeight()); 

  if (_parameters.showUsagePace) _parameters.showUsagePace(moved_from_passive/used_time);
 
}; // void Master::estimateResources2()


void Master::estimateResources3()
{
  CALL("estimateResources3()");
  ASSERT(_parameters.lrs);
  float used_memory = usedMemory();
  float used_time = _timer.elapsedDeciseconds();

  bool noTimeCheck = used_time < _parameters.timeLimit * _parameters.first_time_check;
  bool noMemoryCheck = used_memory < _parameters.memoryLimit * _parameters.first_mem_check;

  if (noTimeCheck && noMemoryCheck) return;

  long moved_from_passive = _passive.UsedAndRemoved();   
  long reachableByTime = (noTimeCheck)? 
    LONG_MAX 
    : 
    (long)(((_parameters.timeLimit - used_time)*moved_from_passive)/used_time);

  long reachableByMemory = (noMemoryCheck)? 
    LONG_MAX
    :
    (long)(((_parameters.memoryLimit - used_memory)*moved_from_passive)/used_memory);
  _parameters.reachable = (reachableByTime < reachableByMemory) ? reachableByTime : reachableByMemory;
  _passive.SetNumOfReachable3(_parameters.reachable);
  _parameters.maxWeight = _passive.MaxAcceptedWeight();
  if (_parameters.maxWeight > _parameters.maxMaxWeight) 
    {
      _parameters.maxWeight = _parameters.maxMaxWeight;
    };
  _freshClause.SetMaxWeight(_parameters.maxWeight);
  if ((_parameters.showWeightChange) 
      && (_freshClause.PreviousMaxWeight() != _freshClause.MaxWeight()))
    _parameters.showWeightChange(_freshClause.PreviousMaxWeight(),_freshClause.MaxWeight()); 

  if (_parameters.showUsagePace) _parameters.showUsagePace(moved_from_passive/used_time);
 
}; // void Master::estimateResources3()




void Master::estimateResources4()
{
  CALL("estimateResources4()");
  ASSERT(_parameters.lrs == 4);
  float used_memory = usedMemory();
  float used_time = _timer.elapsedDeciseconds();

  bool noTimeCheck = used_time < _parameters.timeLimit * _parameters.first_time_check;
  bool noMemoryCheck = used_memory < _parameters.memoryLimit * _parameters.first_mem_check;

  if (noTimeCheck && noMemoryCheck) return;

  long moved_from_passive = _passive.UsedAndRemoved();   
  long reachableByTime = (noTimeCheck)? 
    (_halfLongMax) 
    : 
    ((long)(((_parameters.timeLimit - used_time)*moved_from_passive)/used_time));

  long reachableByMemory = (noMemoryCheck)? 
    (_halfLongMax)
    :
    ((long)(((_parameters.memoryLimit - used_memory)*moved_from_passive)/used_memory));
  _parameters.reachable = (reachableByTime < reachableByMemory) ? reachableByTime : reachableByMemory;
  long currentlyUnreachable = _passive.Size() - _parameters.reachable;

  if (currentlyUnreachable <= 0)
    { 
      if ((currentlyUnreachable <= _parameters.unreachable) && (_parameters.maxWeight < ((long)VampireKernelConst::MaxClauseSize)))
	{ 
	  // weight limit increases 
	  _parameters.maxWeight++;
	  if (_parameters.maxWeight > _parameters.maxMaxWeight) 
	    {
	      _parameters.maxWeight = _parameters.maxMaxWeight;
	    };
	};
    }
  else // currentlyUnreachable > 0
    { 
      if (currentlyUnreachable >= _parameters.unreachable)
	{
	  // weight limit decreases 
	  if (_parameters.maxWeight > _passive.MaxPresentWeight()) 
	    {
	      _parameters.maxWeight = _passive.MaxPresentWeight(); 
	    }       
	  else 
	    if (_parameters.maxWeight > _passive.MinPresentWeight()) 
	      { 
		_parameters.maxWeight--;
	      };
	};
    };

  _parameters.unreachable = currentlyUnreachable;

  _freshClause.SetMaxWeight(_parameters.maxWeight);
  if ((_parameters.showWeightChange) 
      && (_freshClause.PreviousMaxWeight() != _freshClause.MaxWeight()))
    _parameters.showWeightChange(_freshClause.PreviousMaxWeight(),_freshClause.MaxWeight()); 
  if (_parameters.showUsagePace) _parameters.showUsagePace(moved_from_passive/used_time);
 
}; // void Master::estimateResources4()



void Master::estimateResources5()
{
  CALL("estimateResources5()");
  ASSERT(_parameters.lrs == 5);
  float used_memory = usedMemory();
  float used_time = _timer.elapsedDeciseconds();

  bool noTimeCheck = used_time < _parameters.timeLimit * _parameters.first_time_check;
  bool noMemoryCheck = used_memory < _parameters.memoryLimit * _parameters.first_mem_check;

  if (noTimeCheck && noMemoryCheck) return;

  long moved_from_passive = _passive.UsedAndRemoved();   
  long reachableByTime = (noTimeCheck)? 
    (_halfLongMax) 
    : 
    ((long)(((_parameters.timeLimit - used_time)*moved_from_passive)/used_time));

  long reachableByMemory = (noMemoryCheck)? 
    (_halfLongMax)
    :
    ((long)(((_parameters.memoryLimit - used_memory)*moved_from_passive)/used_memory));
  _parameters.reachable = (reachableByTime < reachableByMemory) ? reachableByTime : reachableByMemory;
  long currentlyUnreachable = _passive.Size() - _parameters.reachable;

  if (currentlyUnreachable <= 0)
    { 
      if ((currentlyUnreachable <= _parameters.unreachable) && (_parameters.maxWeight < ((long)VampireKernelConst::MaxClauseSize)))
	{ 
	  // weight limit increases 
	  _parameters.maxWeight++;
	  if (_parameters.maxWeight > _parameters.maxMaxWeight) 
	    {
	      _parameters.maxWeight = _parameters.maxMaxWeight;
	    };
	};
    }
  else // currentlyUnreachable > 0
    { 
      if (currentlyUnreachable >= _parameters.unreachable)
	{
	  // weight limit decreases 
	  if (_parameters.maxWeight > _passive.MaxPresentWeight()) 
	    {
	      _parameters.maxWeight = _passive.MaxPresentWeight(); 
	    }       
	  else 
	    if (_parameters.maxWeight >= _passive.MinPresentWeight())
	      { 
		_parameters.maxWeight--;
	      };
	};
    };

  _parameters.unreachable = currentlyUnreachable;

  _freshClause.SetMaxWeight(_parameters.maxWeight);
  if ((_parameters.showWeightChange) 
      && (_freshClause.PreviousMaxWeight() != _freshClause.MaxWeight()))
    _parameters.showWeightChange(_freshClause.PreviousMaxWeight(),_freshClause.MaxWeight()); 
  if (_parameters.showUsagePace) _parameters.showUsagePace(moved_from_passive/used_time);
 
}; // void Master::estimateResources5()




bool Master::completeStrategyUsed()
{
  return !(_parameters.incompleteAPriori
	   || statTooDeepInferences()
	   || statTooHeavy() 
	   || statTooHeavyLiterals() 
	   || statTooManyVariables()
	   || statTooLong()
	   || statTooDeep()
	   || statRecycled()
	   || statRefusedAllocation()
	   || (_parameters.selection >= 1000)
	   || (_parameters.selection <= -1000)
	   || BuiltInTheoryDispatcher::current());      
}; // bool Master::completeStrategyUsed()


void Master::recycleUnreachablePassive0()
{
  CALL("recycleUnreachablePassive0()");
  Clause* cl;
 recycle_next:
  if (_passive.UnreachableKept() <= (_passive.CurrentlyKept()/5)) 
    return; // nothing to recycle
  cl = _passive.TheMostUnreachableClause();    
  if (!cl) return;  
  ASSERT(cl->mainSet() == Clause::MainSetPassive);
  _passive.Cancel(cl);
  cl->setMainSet(Clause::MainSetFree);
  enqueueForDisposal(cl);
  _statRecycled++;
  goto recycle_next;
}; // void Master::recycleUnreachablePassive0()


void Master::killActiveOrphans0()
{
  CALL("killActiveOrphans0()");
  _active.ResetIterator();
  for (Clause* cl = _active.Next(); cl; cl = _active.Next())
    if ((cl->Active())
	&& (!cl->IsVIP())
	&& (cl->IsOrphan()))
      {
	enqueueForDisposal(cl);
	_statActiveOrphansMurdered++;
      };
}; // void Master::killActiveOrphans0()

void Master::destroyPassive0() 
{ 
  CALL("destroyPassive0()");
  _passive.DestroyAllClauses();
 
  _passive.ResetIterator();
  for (Clause* cl = _passive.IterNext(); cl; cl = _passive.IterNext())
    enqueueForDisposal(cl);
}; // void Master::destroyPassive0() 


//====================================================
