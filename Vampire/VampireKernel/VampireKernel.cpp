//
// File:         VampireKernel.cpp
// Description:  Implementation of Vampire kernel. 
// Created:      Jun 09, 2002, Alexandre Riazanov, riazanov@cs.man.ac.uk
// Revised:      Dec 03, 2002, Alexandre Riazanov, riazanov@cs.man.ac.uk
//               Had to mess the code up to please VC++ 6.0
//======================================================================
#include <climits>
#include <cstdio>
#include "GlobAlloc.hpp"
#include "GlobalClock.hpp"
#include "RuntimeError.hpp"
#include "GlobalStopFlag.hpp"
#include "VampireKernel.hpp"
#include "Signature.hpp"
#include "Clause.hpp"
#include "ClauseQueue.hpp"
#include "ProblemProfile.hpp"
#include "Tabulation.hpp"
#include "Master.hpp"
#include "ClauseNumIndex.hpp"
#include "TermWeighting.hpp"
#include "UniformTermWeighting.hpp"
#include "NonuniformConstantTermWeighting.hpp"
#include "NonrecursiveKBOrdering.hpp"
#include "StandardKBOrdering.hpp"
#include "InferenceTree.hpp"
#include "tmp_literal.hpp"
#include "WeightPolynomial.hpp"
#include "BuiltInTheoryDispatcher.hpp"
#include "clause_set.hpp"
using namespace BK;
using namespace VK;
//======================================================================
#undef DEBUG_NAMESPACE
#ifdef DEBUG_VAMPIRE_KERNEL
#  define DEBUG_NAMESPACE "VampireKernel::_Implementation"
#endif    
#include "debugMacros.hpp"
//======================================================================



class VampireKernel::_Implementation
{
public:

    //
    //   VC++ 6.0 (debug mode) breaks down if I try to define a constructor
    //   or destructor for _Implementation

  /***********
  _Implementation(const VampireKernel::Options& opt) :    
    _termWeightingWrapper(opt.term_weighting_scheme),
    _simplificationOrderingWrapper(opt.simplification_ordering),
    _options(opt),
    _saturationRunning(false),
    _mainLoopRunning(false),
    _incompleteProcedure(false),
    _tabulation(0),
    _builtInTheoryDispatcher(0),
    _terminationReason(VampireKernel::UnknownTerminationReason),
    _continuation(Continuation_Init),
    _lookingForMoreAnswers(false),
    _numberOfSupportClauses(0L)
  {
    CALL("constructor _Implementation(const VampireKernel::Options& opt)");
    
    Signature::setCurrent(&_signature);
    _signature.setMaxlNumOfSkolemSymbols(_options.max_skolem);
    if (_options.tabulation)
      {
	_tabulation = new Tabulation();
	_tabulation->assignStream(_options.tabulation);
	_tabulation->assignJob(_options.job_id);
	static ulong sessionId = 0UL;
	_tabulation->assignKernelSession(sessionId);
	++sessionId;
      }
    else
      _tabulation = 0;
    
    _master.setClauseNumberGenerator(&_clauseNumberGenerator);
    
    WeightPolynomial::setSharing(&_weightPolynomialSharing);

  }; // _Implementation(const VampireKernel::Options& opt)
  

  ~_Implementation()
  {
    CALL("destructor ~_Implementation()");
    if (DestructionMode::isThorough())
      {
	VK::Clause* clause;
	while (_inputSupportClauses.dequeue(clause))
	  clause->DestroyUnshared();
	while (_inputQueryClauses.dequeue(clause))
	  clause->DestroyUnshared();
	while (_preprocessingHistory.dequeue(clause))
	  clause->DestroyUnshared();
	if (_tabulation) delete _tabulation;
	if (_builtInTheoryDispatcher) delete _builtInTheoryDispatcher;
      };
  }; // ~_Implementation()

  ******************/

  void init(const VampireKernel::Options& opt) 
  {
    CALL("init(const VampireKernel::Options& opt)");
 
    _weightPolynomialSharing.init();

    _termWeightingWrapper.init(opt.term_weighting_scheme);
    _simplificationOrderingWrapper.init(opt.simplification_ordering);

    _options = opt;

    _timer.init();

    _saturationRunning = false;
    _mainLoopRunning = false;

    _signature.init();
    _newInputClause.init();

    _inputSupportClauses.init();
    _inputQueryClauses.init();

    _incompleteProcedure = false;
    
    _problemProfile.init();

    _tabulation = 0;

    _clauseNumberGenerator.init();

    _builtInTheoryDispatcher = 0;

    _master.init();
    _preprocessingHistory.init();
    _numberToClauseMapping.init();
    
    _terminationReason = VampireKernel::UnknownTerminationReason;

    _proofTree.init();

    _continuation = Continuation_Init;
    _lookingForMoreAnswers = false;
    _numberOfSupportClauses = 0L;

    Signature::setCurrent(&_signature);
    _signature.setMaxlNumOfSkolemSymbols(_options.max_skolem);
    if (_options.tabulation)
      {
	_tabulation = new Tabulation();
	_tabulation->assignStream(_options.tabulation);
	_tabulation->assignJob(_options.job_id);
	static ulong sessionId = 0UL;
	_tabulation->assignKernelSession(sessionId);
	++sessionId;
      }
    else
      _tabulation = 0;
    
    _master.setClauseNumberGenerator(&_clauseNumberGenerator);
    
    WeightPolynomial::setSharing(&_weightPolynomialSharing);

  }; // void init(const VampireKernel::Options& opt)



  void destroy() 
  { 
    CALL("destroy()");
   
    if (DestructionMode::isThorough())
      {
	VK::Clause* clause;
	while (_inputSupportClauses.dequeue(clause))
	  clause->DestroyUnshared();
	while (_inputQueryClauses.dequeue(clause))
	  clause->DestroyUnshared();
	while (_preprocessingHistory.dequeue(clause))
	  clause->DestroyUnshared();
	if (_tabulation) delete _tabulation;
	if (_builtInTheoryDispatcher) delete _builtInTheoryDispatcher;
      };

    _proofTree.destroy();
    _numberToClauseMapping.destroy();
    _preprocessingHistory.destroy();
    _master.destroy();
    _clauseNumberGenerator.destroy();
    _problemProfile.destroy();
    _inputQueryClauses.destroy();
    _inputSupportClauses.destroy();
    _newInputClause.destroy();
    _signature.destroy();
    _timer.destroy();	
    _simplificationOrderingWrapper.destroy();
    _termWeightingWrapper.destroy();
    _weightPolynomialSharing.destroy();
  }; // void destroy() 


  void openSession()
  {
    _currentSession = this;
  };

  static void closeSession()
  {
    _currentSession = 0;
  };
  
 
  void* operator new(size_t)
  {
    return GlobAlloc::allocate(sizeof(_Implementation));
  };
   
  void operator delete(void* obj)
  {
    GlobAlloc::deallocate(obj,sizeof(_Implementation));
  };

  static _Implementation* allocate()
  { 
    return 
      static_cast<_Implementation*>(GlobAlloc::allocate(sizeof(_Implementation)));
  };

  static void deallocate(_Implementation* obj)
  {
    GlobAlloc::deallocate(static_cast<void*>(obj),sizeof(_Implementation));
  };

  void beginInputSignature() 
  {
    _symbolWeightConstantPartInputIgnored = false;
    _unaryFunctionWithZeroWeightExists = false;
  };
  void endInputSignature() 
  {
    if (_symbolWeightConstantPartInputIgnored)
      {
	if (_options.std_output)
	  *_options.std_output << "% WARNING: input of some symbol weight constant parts ignored\n"
			       << "%          because of --term_weighting_scheme " 
			       << _options.term_weighting_scheme << "\n";
      };

    if (_options.selection < 0L)
      {
	_signature.adjustHeaderPrecedenceForPositiveSelection();
      };
    if (_options.header_precedence_kinky)
      {
	_signature.makeHeaderPrecedenceKinky();
      };   
    _signature.setDependenceOfPrecedenceOnArity(_options.symbol_precedence_by_arity);
    _signature.setDependenceOfWeightOnArity(_options.symbol_weight_by_arity);
    _signature.endOfInput();
  };
  
  void registerInputFunction(const char* name,
			     ulong arity,
			     ulong& id)
  {
    id = _signature.inputSymbolNumber(false,name,arity);
  };



  void registerInputPredicate(const char* name,
			      ulong arity,
			      bool isAnswer,
			      ulong& id)
  {
    id = _signature.inputSymbolNumber(true,name,arity);
    if (isAnswer) _signature.makeAnswerPredicate(id);
  };
  
  void setSymbolWeightConstPart(ulong symId,
				const VampireKernel::TermWeightType& weight)
  {
    if (_options.term_weighting_scheme == 
	VampireKernel::Options::UniformTermWeighting) 
      {
	_symbolWeightConstantPartInputIgnored = true;
	return;
      };
    
    VampireKernel::TermWeightType w = weight;
    if (w == (VampireKernel::TermWeightType)0)
      {
	ulong arity = _signature.arity(symId);
	if (arity == 0UL)
	  {
	    if (_options.std_output)
	      {
		*_options.std_output << "% WARNING: zero weight of the constant/propositional variable ";
		_signature.outputSymbol(*_options.std_output,symId) << " is reset to 1.\n";
	      };
	    w = (VampireKernel::TermWeightType)1;
	  }
	else
	  if ((arity == 1UL) &&
	      (!_signature.isPredicate(symId)))
	      
	    {
	      if (_unaryFunctionWithZeroWeightExists)
		{
		  if (_options.std_output)
		    {
		      *_options.std_output << "% WARNING: zero weight of the unary function ";
		      _signature.outputSymbol(*_options.std_output,symId) 
			<< " is reset to 1.\n"
			<< "           Zero weight was previously assigned to ";
		      _signature.outputSymbol(*_options.std_output,_unaryFunctionWithZeroWeight) << "\n";
		    };
		  w = (VampireKernel::TermWeightType)1;
		}
	      else
		{
		  _unaryFunctionWithZeroWeightExists = true;
		  _unaryFunctionWithZeroWeight = symId;
		  _signature.assignMaximalPrecedence(symId);
		};	      
	    };
      };
    _signature.setSymbolWeightConstantPart(symId,w);
  };

  void setSymbolPrecedence(ulong symId,long prec)
  {
    if (prec >= Signature::maximalPossibleSymbolPrecedence())
      {
	if (_options.std_output)
	  {
	    *_options.std_output << "% WARNING: precedence of ";
	    _signature.outputSymbol(*_options.std_output,symId) << " ignored: too high.\n";
	  };
	return;
      };
    if ((_unaryFunctionWithZeroWeightExists) && 
	(symId == _unaryFunctionWithZeroWeight))
      {
	if (_options.std_output)
	  {
	    *_options.std_output << "% WARNING: precedence of ";
	    _signature.outputSymbol(*_options.std_output,symId) 
	      << " ignored: maximal precedence assigned since the function has zero weight.\n";
	  };
	return;
      };
    
    _signature.setInputSymbolPrecedence(symId,prec);
  };

  void interpret(bool predicate,
		 const char* inputSymbol,
		 ulong arity,
		 const char* theory,
		 const char* nativeSymbol)
  {
    if (!_builtInTheoryDispatcher)
      {
	_builtInTheoryDispatcher = new BuiltInTheoryDispatcher(&_clauseNumberGenerator);
	BuiltInTheoryDispatcher::setCurrent(_builtInTheoryDispatcher);
      };
    ulong symId = _signature.inputSymbolNumber(predicate,inputSymbol,arity);
    if (!_builtInTheoryDispatcher->interpret(symId,theory,nativeSymbol))
      {
	if (_options.std_output)
	  *_options.std_output << "% WARNING: can not interpret " << inputSymbol << "/" << arity 
			       << " as " << theory << "/" << nativeSymbol << "\n";
      };
  };

  
  void beginInputClauses()
  {
    CALL("beginInputClauses()");
    ASSERT(_inputSupportClauses.empty());
    ASSERT(_inputQueryClauses.empty());
    _numberOfRejectedInputClauses = 0UL;
    _numberOfSupportClauses = 0L;
    _equalityInSupport = false;
  };

  void endInputClauses()
  {
    CALL("endInputClauses()");
    if (_numberOfSupportClauses && _options.std_output)
      {
	*_options.std_output << "% WARNING: " << _numberOfSupportClauses 
                             << " support clauses in the input.\n"
			     << "%          Preprocessing by definition elimination is blocked.\n";
	_options.elim_def = 0;
      };   
  };
  
  void beginInputClause(ulong clauseNumber,void* origin,bool goal,bool support)
  {
    CALL("beginInputClause(ulong clauseNumber,void* origin,bool goal,bool support)");
    _inputClauseRejected = false;
    _newInputClause.reset();
    _newInputClause.usedRule(ClauseBackground::Input);
    if (goal) _newInputClause.markAsSubgoal();
    if (support) 
      {
	_newInputClause.markAsSupport();
	//_incompleteProcedure = true;
        ++_numberOfSupportClauses;        
      };
    _currentInputClauseNumber = clauseNumber;
    _currentInputClauseOrigin = origin;
  };

  void endInputClause()
  {
    CALL("endInputClause()");
    if (_inputClauseRejected)
      {
	++_numberOfRejectedInputClauses;
      }
    else
      {
	VK::Clause* clause = _newInputClause.result();
	clause->SetNumber(_currentInputClauseNumber);
	clause->setInputClauseOrigin(_currentInputClauseOrigin);

	if (clause->isSupportClause())
	  {
	    if ((!_equalityInSupport) &&
                clause->containsEquality())
              _equalityInSupport = true;

	    _inputSupportClauses.enqueue(clause);
	  }
	else
	  _inputQueryClauses.enqueue(clause);

	if (clause->Empty())
	  {
	    if (_options.std_output)
	      *_options.std_output << "% WARNING: empty input clause.\n";
	  };
      };
  }; // void endInputClause()
  
  void beginLiteral(bool positive,ulong predicateId)
  {
    CALL("beginLiteral(bool positive,ulong predicateId)");
    if (_inputClauseRejected) return;
    TERM::Polarity polarity = 
      (positive)? TERM::PositivePolarity : TERM::NegativePolarity;
    TERM header((TERM::Arity)_signature.arity(predicateId),
		(TERM::Functor)predicateId,
		polarity);
    if (!_newInputClause.litHeader(header)) _inputClauseRejected = true;
  }; // void beginLiteral(bool positive,ulong predicateId)

  void endLiteral()
  {
  };
  
  void receiveFunction(ulong symId)
  {
    CALL("receiveFunction(ulong symId)");
    if (_inputClauseRejected) return;
    TERM symbol((TERM::Arity)_signature.arity(symId),
		(TERM::Functor)symId,
		TERM::PositivePolarity);
    if (!_newInputClause.functor(symbol)) _inputClauseRejected = true;
  }; // void receiveFunction(ulong symId)

  void receiveNumericConstant(const double& value)
  {
    CALL("receiveNumericConstant(const double& value)");
    ulong id = _signature.numericConstantNumber(value);
    receiveFunction(id);
  }; // void receiveNumericConstant(const double& value


  void receiveVariable(ulong varNum)
  {
    CALL("receiveVariable(ulong varNum)");
    if (_inputClauseRejected) return;
    _newInputClause.variable(TERM(TermVar,varNum));
  };

  void saturate(ulong nextVacantClauseId,long timeLimit)
  {
    CALL("saturate(ulong nextVacantClauseId,long timeLimit)");

    GlobalStopFlag::reset();
    _saturationRunning = true;
    _saturationTimeLimit = timeLimit;
    if (_saturationTimeLimit < 0L) _saturationTimeLimit = 0L;
    _mainLoopRunning = false;
    _timer.reset();
    _timer.start();
    GlobalStopFlag::HandlerActivator 
      globalStopFlagHandlerActivator(globalStopFlagHandler);
    GlobalClock::AlarmHandler alarmHandler(handleAlarm);

    _clauseNumberGenerator.reset(nextVacantClauseId);
    if (_options.std_output)
      *_options.std_output << "% New Vampire Kernel session, time limit: " 
			   << _saturationTimeLimit/10 << "." 
			   << _saturationTimeLimit%10 << " sec.\n";
    if (_numberOfRejectedInputClauses)
      {
	if (_options.std_output)
	  *_options.std_output << "% WARNING: " << _numberOfRejectedInputClauses 
			       << " input clauses rejected.\n";
	_incompleteProcedure = true;
      };
    
    if (_options.show_opt) 
      {	
	if (_options.std_output)
	  {
	    *_options.std_output << "%================== Options: =======================\n";
	    *_options.std_output<< "% time limit = " << ((float)_saturationTimeLimit)/10 << "\n";
	    _options.output(*_options.std_output);
	    *_options.std_output << "%========== End of options: ======================\n";
	  };
      };

    analyseOptions();

    if (_options.show_input && _options.std_output)
      {
	*_options.std_output << "%================== Input clauses (support + query): ===================\n";
	outputInputSupportClauses();
	outputInputQueryClauses();
	*_options.std_output << "%============== End of input clauses: ================\n";
      };
    VK::Clause::setSelectedPosEqPenaltyCoefficient(_options.selected_pos_eq_penalty_coefficient);
    VK::Clause::setNongoalPenaltyCoefficient(_options.nongoal_penalty_coefficient);
    if (_options.show_profile && _options.std_output)
      {
	_problemProfile.load(_inputSupportClauses);
	_problemProfile.load(_inputQueryClauses);
	_problemProfile.summarise();
        *_options.std_output << "%========= Profile of input (support + query):\n"; 
	_problemProfile.output(*_options.std_output);
	*_options.std_output << "%========= End of profile. ========\n";
	*_options.std_output << _signature;
      };


    GlobalStopFlag::check();
    preprocessAndRunMainLoop();
  }; // void saturate(ulong nextVacantClauseId,long timeLimit)

  void preprocessAndRunMainLoop()
  {
    CALL("preprocessAndRunMainLoop()");    
    long remainingTime;
    float remainingMemory;

    switch (_continuation)
      {
      case Continuation_Init:
	break;
      case Continuation_Prepro_Simplification:
	// same as below
      case Continuation_Prepro_DefElimination_LoadInput:
	// same as below
      case Continuation_Prepro_DefElimination_DefElimination:
	goto preprocess;
      case Continuation_LoadInputQuery:
	if (_options.std_output)
	  *_options.std_output << "% Resuming loading preprocessed clauses...\n";
	goto load_input_query;
      case Continuation_LoadInputSupport:
	if (_options.std_output)
	  *_options.std_output << "% Resuming loading preprocessed clauses...\n";
	goto load_input_support;
      case Continuation_MainLoop:
	if (_options.std_output)
	  *_options.std_output << "% Resuming main loop...\n";
	goto main_loop;
      default:
	ICP("ICP0");
	RuntimeError::report("Bad _continuation in VampireKernel::_Implementation::preprocessAndRunMainLoop()");
      };


  preprocess:
    preprocessInputQueryClauses();

    if (_master.terminated())
      {
	_solvedByPreprocessing = true;
	reportTermination();
	_timer.stop();
	_saturationRunning = false; 
	return;
      };
    _continuation = Continuation_Init;

    _solvedByPreprocessing = false;

    
    _signature.setCommutativity(_problemProfile.commutativityTable(),
				_problemProfile.symmetryTable());
    _signature.makeCommutative(VampireKernelConst::UnordEqNum);
    _signature.makeCommutative(VampireKernelConst::OrdEqNum);

#ifdef INTERCEPT_FORWARD_MATCHING_INDEXING
    FMILog << _signature;
#endif
#ifdef INTERCEPT_BACKWARD_SUBSUMPTION_INDEXING
    BSILog.unblock();
    BSILog << "MODE_DESCRIPTION\n";
    if (options.bs_set_mode)
      {
	BSILog << "  'set mode'\n";
      }
    else
      if (!options.bs_optimised_multiset_mode)
	{
	  BSILog << "  'multiset mode'\n";
	} 
      else 
	{
	  BSILog << "// Logging aborted. Incorrect subsumption mode. \n";
	  BSILog.flush();
	  RuntimeError::report("BSI logging aborted: incorrect subsumption mode."); 
	};
    BSILog << "END_OF_MODE_DESCRIPTION\n";
    BSILog.block();
#endif 

#ifdef INTERCEPT_BACKWARD_MATCHING_INDEXING
    BMILog.unblock();
    // Mode modifiers:
    BMILog << "$\n"; // end of mode description
    BMILog << _signature;
    INTERCEPT_BACKWARD_MATCHING_INDEXING_COMMENT(BMILog << "           // end of signature declaration");
    BMILog.block();
#endif


#ifdef INTERCEPT_INDEXED_SEARCH_FOR_GENERALISATIONS
    SFGILog.unblock();
    // Mode modifiers:
    SFGILog << "$\n"; // end of mode description
    SFGILog << _signature;
    INTERCEPT_INDEXED_SEARCH_FOR_GENERALISATIONS_COMMENT(SFGILog << "           // end of signature declaration");
    SFGILog.block();
#endif


#ifdef INTERCEPT_BACKWARD_SUPERPOSITION_INDEXING
    BSIULog.unblock();
    // Mode modifiers:
    BSIULog << "$\n"; // end of mode description
    BSIULog << _signature;
    INTERCEPT_BACKWARD_SUPERPOSITION_INDEXING_COMMENT(BSIULog << "           // end of signature declaration");
    BSIULog.block();
#endif


    if ((!_problemProfile.numOfEqLiterals()) &&
        (!_equalityInSupport))
      {
	switchBuiltinEqualityOff();
      };

    GlobalStopFlag::check();

    remainingTime = _saturationTimeLimit - _timer.elapsedDeciseconds();
    remainingMemory = 
      (((float)_options.memory_limit)*1024) - GlobAlloc::occupiedByObjects();
    if (remainingMemory < 0.0) remainingMemory = 0.0;
    prepareMasterForSaturation(remainingTime,remainingMemory);

  load_input_support:
    loadInputSupportClausesIntoMaster();

    if (_master.terminated())
      {
	reportTermination();
	_timer.stop();
	_saturationRunning = false; 
	_continuation = Continuation_LoadInputSupport;
	return;
      };
    _continuation = Continuation_Init;

  load_input_query:

    loadInputQueryClausesIntoMaster();
   
    if (_master.terminated())
      {
	reportTermination();
	_timer.stop();
	_saturationRunning = false; 
	_continuation = Continuation_LoadInputQuery;
	return;
      };
    _continuation = Continuation_Init;

    GlobalStopFlag::check();

  main_loop:
    _mainLoopRunning = true;
    _master.mainLoop();
    _mainLoopRunning = false;
    reportTermination();
    _timer.stop();
    _saturationRunning = false; 
    _continuation = Continuation_MainLoop;
  }; // void preprocessAndRunMainLoop()

  void lookForAnotherAnswer()
  {
    CALL("lookForAnotherAnswer()");
    ASSERT(refutation());
    ASSERT(!_timer.isTicking());
    ASSERT(!_saturationRunning);
    GlobalStopFlag::reset();
    _lookingForMoreAnswers = true;
    _timer.start();
    _saturationRunning = true;
    GlobalStopFlag::HandlerActivator 
      globalStopFlagHandlerActivator(globalStopFlagHandler);
    GlobalClock::AlarmHandler alarmHandler(handleAlarm);

    switch (_continuation)
      {
      case Continuation_Prepro_Simplification:
	// same as below
      case Continuation_Prepro_DefElimination_LoadInput:
	// same as below
      case Continuation_Prepro_DefElimination_DefElimination:
	// same as below
      case Continuation_LoadInputQuery:
	// same as below	
      case Continuation_LoadInputSupport:
	// same as below	
      case Continuation_MainLoop:
	if (_options.std_output)
	  *_options.std_output << "% Resuming saturation in attempt to find another answer...\n";
	preprocessAndRunMainLoop();
	break;
      default:
	ICP("ICP0");
	RuntimeError::report("Bad _continuation in VampireKernel::_Implementation::lookForAnotherAnswer()");
      };
  }; // void lookForAnotherAnswer()


  void suspend()
  {
    if (_saturationRunning)
      {
	_master.suspend();
	_timer.stop();
      };
        
    if (_options.std_output)
      *_options.std_output << "% Kernel session suspended.\n";   
    outputStatistics();
  };

  void resume()
  {
    if (_options.std_output)
      *_options.std_output << "% Kernel session resumed.\n";
    if (_saturationRunning)
      {
	_timer.start();
	_master.resume();
      };
  }; 


  void reportInterrupt(int signalNumber)
  {
    CALL("reportInterrupt(int signalNumber)");
    _terminationReason = VampireKernel::AbortedOnExternalInterrupt;
    
    if (_options.std_output)
      {
	outputStatistics();
	*_options.std_output << "% Proof not found. Aborted on external signal " 
			     << signalNumber << ".\n";
	_options.std_output->flush();
      };
  }; // void reportInterrupt(int signalNumber)

  void reportErrorSignal(int signalNumber)
  {
    CALL("reportErrorSignal(int signalNumber)");
    _terminationReason = VampireKernel::AbortedOnErrorSignal;
    if (_options.std_output)
      *_options.std_output << "% Proof not found. Kernel session aborted on error signal " 
			   << signalNumber << ".\n";
  }; // void reportErrorSignal(int signalNumber)

  
  void reportError()
  {
    CALL("reportError()");
    _terminationReason = VampireKernel::AbortedOnError;
    
    if (_options.std_output)
      *_options.std_output << "% Proof not found. Kernel session aborted due to an error.\n";
  }; // void reportError(const char* errorQualifier,const char* errorMessage)

  VampireKernel::TerminationReason terminationReason() const
  {
    return _terminationReason;
  };

  const VampireKernel::Clause* refutation() const
  {
    return static_cast<const VampireKernel::Clause*>(static_cast<const void*>(_master.proof()));
  };

  VampireKernel::ClauseSetIterator* activeClauses()
  {
    CALL("activeClauses()");
    return 
      static_cast<VampireKernel::ClauseSetIterator*>(new ClauseSetIterator(_master.activeClauses()));
  };

  void disposeOf(VampireKernel::ClauseSetIterator* iterator)
  {
    CALL("disposeOf(VampireKernel::ClauseSetIterator* iterator)");
    delete static_cast<ClauseSetIterator*>(iterator);
  };


  bool isNumericConstantId(ulong symbolId) const
  {
    return _signature.isNumericConstant(symbolId);
  };
  bool isInputSymbol(ulong symbolId) const
  {
    return !_signature.isSkolem(symbolId);
  }; 
  bool isAnswerPredicateId(ulong symbolId) const
  {
    return _signature.isAnswerPredicate(symbolId);
  };
  ulong symbolArity(ulong symbolId) const
  {
    return _signature.arity(symbolId);
  };
  const char* symbolPrintName(ulong symbolId) const
  {
    CALL("symbolPrintName(ulong symbolId) const");
    ASSERT(!isNumericConstantId(symbolId));
    return _signature.symbolName(symbolId);
  }; 
  const double& numericConstantValue(ulong symbolId) const
  {
    CALL("numericConstantValue(ulong symbolId) const");
    ASSERT(isNumericConstantId(symbolId));
    return _signature.numericConstantValue(symbolId);
  }; 

private:
  class ClauseSetIterator : public VampireKernel::ClauseSetIterator
  {
  public:
    ClauseSetIterator(const CLAUSE_SET& set) :
      _iter(set)
    {
    };
    ~ClauseSetIterator();
    void* operator new(size_t)
    {
      return GlobAlloc::allocate(sizeof(ClauseSetIterator));
    };
    void operator delete(void* obj)
    {
      GlobAlloc::deallocate(obj,sizeof(ClauseSetIterator));
    };
    bool isEnd() const;
    void next();
    const VampireKernel::Clause* clause() const;
    
  private:
    CLAUSE_SET::ConstIter _iter;
  }; // class ClauseSetIterator


private:
  static void handleAlarm(int)
  {
    if (_currentSession && _currentSession->_saturationRunning)
      {	
	if (_currentSession->_timer.elapsedDeciseconds() > _currentSession->_saturationTimeLimit)
	  {
	    GlobalStopFlag::raise();
	  }
	else
	  {
	    if (_currentSession->_mainLoopRunning)
	      {
		_currentSession->_master.externalTick();
	      };
	  };
      };
  };

  static void globalStopFlagHandler()
  {
    CALL("globalStopFlagHandler()");
    if (_currentSession) 
      { 
	_currentSession->reportTimeLimitExpired();
	DestructionMode::makeFast();
       
	/***************
	DF; cout << "THROW TL\n";
	DF; REPCPH;
	DF; cout << "\n\nTRACE:\n\n";
	DF; TRACE(false,false);
	****************/

	throw VampireKernel::Emergency(VampireKernel::Emergency::TimeLimit);
      };
  }; // void globalStopFlagHandler()

  void preprocessInputQueryClauses()
  {
    CALL("preprocessInputQueryClauses()"); 

    switch (_continuation)
      {
      case Continuation_Init:
	break;
      case Continuation_Prepro_Simplification:
	if (_options.std_output)
	  *_options.std_output << "% Resuming preprocessing by saturation "
			       << _preproIterationNumber << "\n";
	goto simplify;
      case Continuation_Prepro_DefElimination_LoadInput:
	// same as below
      case Continuation_Prepro_DefElimination_DefElimination:
	if (_options.std_output)
	  *_options.std_output << "% Resuming preprocessing by def. elimination "
			       << _preproIterationNumber << "\n";
	goto eliminate_definitions;
      default:
	ICP("ICP0");
	RuntimeError::report("Bad _continuation in VampireKernel::_Implementation::preprocessInputQueryClauses()");
      };



    _preproStatSplit = 0UL;
    _preproStatComponents = 0UL;
    _preproStatDifferentComponents = 0UL;
    _preproIterationNumber = 0UL;
    _preproEliminationCycles = 0L;

  simplify:
    preprocessQueryBySimplification(_preproIterationNumber);
    collectPreprocessingIterationStatistics();
    _problemProfile.reset();


    _problemProfile.load(_inputQueryClauses);
    _problemProfile.summarise();
    if (_options.show_profile && _options.std_output)
      {
	*_options.std_output << "%========= Profile of preprocessed query (" 
			     << _preproIterationNumber << "):\n"; 
	_problemProfile.output(*_options.std_output);
	*_options.std_output << "%========= End of profile for preprocessed query (" 
			     << _preproIterationNumber << ") ========\n";
      }; 
    if (_master.terminated()) 
      {
	_continuation = Continuation_Prepro_Simplification;
	return;
      };
    _continuation = Continuation_Init;

    if (_problemProfile.numOfEliminated() && 
	(_preproEliminationCycles < _options.elim_def))
      { 
      next_elimination_cycle:
	_preproIterationNumber++;
	_preproEliminationCycles++;

      eliminate_definitions:
	preprocessQueryByDefElimination(_preproIterationNumber);
	collectPreprocessingIterationStatistics();
	_problemProfile.reset(); 

	_problemProfile.load(_inputQueryClauses);
	_problemProfile.summarise();   
	if (_options.show_profile && _options.std_output)
	  {
	    *_options.std_output << "%========= Profile of preprocessed query (" 
				 << _preproIterationNumber << "):\n"; 
	    _problemProfile.output(*_options.std_output);
	    *_options.std_output << "%========= End of profile for preprocessed query (" 
				 << _preproIterationNumber << ") ========\n";
	  }; 
	if (_master.terminated()) 
	  {
	    return;
	  };
      
	_continuation = Continuation_Init;
	if (_problemProfile.numOfEliminated() && 
	    (_preproEliminationCycles < _options.elim_def))
	  goto next_elimination_cycle;
	goto simplify;
      };
    // end of story
    if (_options.std_output)
      *_options.std_output << "%======== Preprocessing has been done in " << _preproIterationNumber + 1 << " stages.\n";
    
  }; // void preprocessInputQueryClauses()

  void preprocessQueryBySimplification(ulong iterationNumber)
  {
    CALL("preprocessQueryBySimplification(ulong iterationNumber)");    
    switch (_continuation)
      {
      case Continuation_Init:
	break;
      case Continuation_Prepro_Simplification:
	goto load_input;
      default:
	ICP("ICP0");
	RuntimeError::report("Bad _continuation in VampireKernel::_Implementation::preprocessQueryBySimplification(ulong iterationNumber)");
      };
    
    if (_options.std_output)
      *_options.std_output << "%=============== Preprocessing by simplification " 
			   << iterationNumber << " ============\n";
    prepareMasterForSimplification();
    
  load_input:
    loadInputQueryClausesIntoMaster();
    if (_master.terminated()) return;
    _continuation = Continuation_Init;
    extractPreprocessedQueryClauses();
    if (_options.show_prepro && _options.std_output) 
      {
	
	*_options.std_output << "%=============== Preprocessing  by simplification " 
			     << iterationNumber << " statistics: ============\n";
	outputStatistics();
	*_options.std_output << "%=============== Query clauses preprocessed by simplification " 
			     << iterationNumber << " ================\n";
	outputInputQueryClauses();
	*_options.std_output << "%============== End of clauses preprocessed by " 
			     << iterationNumber << "  ==========\n";
      };
  }; // void preprocessQueryBySimplification(ulong iterationNumber)

  void preprocessQueryByDefElimination(ulong iterationNumber)
  {
    CALL("preprocessQueryByDefElimination(ulong iterationNumber)");    

    switch (_continuation)
      {
      case Continuation_Init:
	break;
      case Continuation_Prepro_DefElimination_LoadInput:
	goto load_input;
      case Continuation_Prepro_DefElimination_DefElimination:
	ASSERT_IN(_inputQueryClauses.empty(),"CONT WITH ELIM");
	goto eliminate_definitions;
      default:
	ICP("ICP0");
	RuntimeError::report("Bad _continuation in VampireKernel::_Implementation::preprocessQueryByDefElimination(ulong iterationNumber)");
      };


    if (_options.std_output) 
      *_options.std_output << "%=============== Preprocessing by def. elimination " 
			   << iterationNumber << " ============\n";
    prepareMasterForDefElimination();

  load_input:

    loadInputQueryClausesIntoMaster();
    if (_master.terminated()) 
      {
	_continuation = Continuation_Prepro_DefElimination_LoadInput;
	return;
      };  
    _continuation = Continuation_Init;

  eliminate_definitions:
    ASSERT_IN(_inputQueryClauses.empty(),"ELIMINATE");
  
    _master.eliminateDefinitions();
    if (_master.terminated())
      {
	ASSERT_IN(_inputQueryClauses.empty(),"PROVED BY DEF ELIM");
	_continuation = Continuation_Prepro_DefElimination_DefElimination;

	return;
      };
    _continuation = Continuation_Init;
    extractPreprocessedQueryClauses(); 

    if (_options.show_prepro && _options.std_output) 
      {
	*_options.std_output << "%=============== Preprocessing  by def. elimination " 
			     << iterationNumber << " statistics: ============\n";
	outputStatistics();
        *_options.std_output << "%=============== Query clauses preprocessed by def. elimination " 
			     << iterationNumber << " ================\n";
	outputInputQueryClauses();
	*_options.std_output << "%============== End of clauses preprocessed by " 
			     << iterationNumber << " ==========\n";
      };

  }; // void preprocessQueryByDefElimination(ulong iterationNumber)

  void prepareMasterForSimplification()
  {
    CALL("prepareMasterForSimplification()");
    _master.reset();
    _master.options.main_alg = 0;
    _master.options.binary_resolution = false; 
    _master.options.no_inferences_between_rules = false;
    _master.options.splitting = _options.static_splitting;
    _master.options.splitting_style = _options.splitting_style;
    _master.options.splitting_with_naming = _options.splitting_with_naming;  
    _master.options.neg_eq_splitting = _options.static_neg_eq_splitting;  
    _master.options.paramodulation = false;
    _master.options.sim_back_sup = 0;
    _master.options.selection = _options.selection;
    _master.options.literal_comparison_mode = _options.literal_comparison_mode;
    _master.options.inherited_negative_selection = _options.inherited_negative_selection;

    // strategy switches        
    _master.options.handle_equality = true;
    _master.options.forward_subsumption = true;
    _master.options.fs_set_mode = true; // not to worry about completeness here  
    _master.options.fs_optimised_multiset_mode = false;
    _master.options.fs_use_sig_filters = false;
    _master.options.fs_old_style = false;
    _master.options.simplify_by_fs = _options.simplify_by_fs;
    _master.options.forward_demodulation = 0; 
    _master.options.fd_on_splitting_branches = false;
    if (_options.normalize_by_commutativity)
      {
	_master.options.normalize_by_commutativity = _problemProfile.commutativityTable();
      }
    else 
      _master.options.normalize_by_commutativity = 0;
    _master.options.backward_demodulation = 0;
    _master.options.bd_on_splitting_branches = false;
    _master.options.simplify_by_eq_res = true;
    _master.options.backward_subsumption = true;
    _master.options.bs_set_mode = true; // not to worry about completeness here  
    _master.options.bs_optimised_multiset_mode = false;
    _master.options.orphan_murder = 0;
    _master.options.lrs = 0;
    _master.options.pred_elim_table = 0;
    // numeric options
    _master.options.age_weight_ratio = 0;
    _master.options.timeLimit = LONG_MAX/10;
    _master.options.memoryLimit = (float)LONG_MAX;
    _master.options.allocation_buffer_price = (float)_options.allocation_buffer_price;
    _master.options.maxActive = LONG_MAX;
    _master.options.maxPassive = LONG_MAX;
    _master.options.maxWeight = LONG_MAX;
    _master.options.first_time_check = 1.0; 
    _master.options.first_mem_check = 1.0;
    // interface options
    _master.options.outputStreamForGenerated = (_options.show_gen) ? _options.std_output : 0;
    _master.options.showPassive = (_options.show_passive) ? outputClause : 0;
    _master.options.showActive = (_options.show_active) ? outputClause : 0;
    _master.options.showWeightChange = (_options.show_weight_limit_changes) ? outputWeightChange : 0;

#ifdef VKERNEL_FOR_STEP_RESOLUTION
    _master.options.step_resolution_literal_selection = _options.step_resolution_literal_selection;
#endif

    _master.setStrategy();
  }; // void prepareMasterForSimplification()

  void prepareMasterForDefElimination()
  {
    CALL("prepareMasterForDefElimination()");
    _master.reset();

    _master.options.main_alg = 0;
    _master.options.binary_resolution = true; // for elimination of definitions
    _master.options.no_inferences_between_rules = false;
    _master.options.splitting = false;   
    _master.options.splitting_style = 1L;
    _master.options.splitting_with_naming = false; 
    _master.options.neg_eq_splitting = 0L; // off  
    _master.options.paramodulation = false;
    _master.options.sim_back_sup = 0;
    _master.options.selection = _options.selection;
    _master.options.literal_comparison_mode = _options.literal_comparison_mode;
    _master.options.inherited_negative_selection = _options.inherited_negative_selection;

    // strategy switches        
    _master.options.handle_equality = true;
    _master.options.forward_subsumption = true;
    _master.options.fs_set_mode = true; // not to worry about completeness here  
    _master.options.fs_optimised_multiset_mode = false;
    _master.options.fs_use_sig_filters = false;
    _master.options.fs_old_style = false;
    _master.options.simplify_by_fs = _options.simplify_by_fs;
    _master.options.forward_demodulation = 0;
    _master.options.fd_on_splitting_branches = false;
    if (_options.normalize_by_commutativity)
      {
	_master.options.normalize_by_commutativity = _problemProfile.commutativityTable();
      }
    else 
      _master.options.normalize_by_commutativity = 0;
    _master.options.backward_demodulation = 0;
    _master.options.bd_on_splitting_branches = false;
    _master.options.simplify_by_eq_res = true;
    _master.options.backward_subsumption = true;
    _master.options.bs_set_mode = true; // not to worry about completeness here  
    _master.options.bs_optimised_multiset_mode = false;
    _master.options.orphan_murder = 0;
    _master.options.lrs = 0;
    _master.options.pred_elim_table = _problemProfile.eliminationTable();
    // numeric options
    _master.options.age_weight_ratio = 0;
    _master.options.timeLimit = LONG_MAX/10;
    _master.options.memoryLimit = (float)LONG_MAX;
    _master.options.allocation_buffer_price = (float)_options.allocation_buffer_price;
    _master.options.maxActive = LONG_MAX;
    _master.options.maxPassive = LONG_MAX;
    _master.options.maxWeight = LONG_MAX;
    _master.options.first_time_check = 1.0;
    _master.options.first_mem_check = 1.0;
    // interface options
    _master.options.outputStreamForGenerated = (_options.show_gen) ? _options.std_output : 0;
    _master.options.showPassive = (_options.show_passive) ? outputClause : 0;
    _master.options.showActive = (_options.show_active) ? outputClause : 0;
    _master.options.showWeightChange = (_options.show_weight_limit_changes) ? outputWeightChange : 0;

#ifdef VKERNEL_FOR_STEP_RESOLUTION
    _master.options.step_resolution_literal_selection = _options.step_resolution_literal_selection;
#endif

    _master.setStrategy();
  }; // void prepareMasterForDefElimination()

  void loadInputQueryClausesIntoMaster()
  {
    CALL("loadInputQueryClausesIntoMaster()");

    switch (_continuation)
      {
      case Continuation_Init:
	break;
      case Continuation_Prepro_Simplification:
	// same as below
      case Continuation_Prepro_DefElimination_LoadInput:
	// same as below
      case Continuation_LoadInputQuery:
	goto load_clause;
      default:
	ICP("ICP0");
	RuntimeError::report("Bad _continuation in VampireKernel::_Implementation::loadInputQueryClausesIntoMaster()");
      };

  next_clause:
    if (_inputQueryClauses.dequeue(_currentInputQueryClause))
      {
	_preprocessingHistory.enqueue(_currentInputQueryClause);
      load_clause:
	_master.inputClause(_currentInputQueryClause);
	if (_master.terminated()) 
	  {
	    return;
	  };
	_continuation = Continuation_Init;	
	goto next_clause;
      };
    
    _master.endOfInput();
    ASSERT(_inputQueryClauses.empty());
  }; // void loadInputQueryClausesIntoMaster()



  void loadInputSupportClausesIntoMaster()
  {
    CALL("loadInputSupportClausesIntoMaster()");

    switch (_continuation)
      {
      case Continuation_Init:
	break;
      case Continuation_LoadInputSupport:
	goto load_clause;
      default:
	ICP("ICP0");
	RuntimeError::report("Bad _continuation in VampireKernel::_Implementation::loadInputSupportClausesIntoMaster()");
      };

  next_clause:
    if (_inputSupportClauses.dequeue(_currentInputSupportClause))
      {
	_preprocessingHistory.enqueue(_currentInputSupportClause);
      load_clause:
        _master.inputClause(_currentInputSupportClause);
	if (_master.terminated()) 
	  {
	    return;
	  };
	_continuation = Continuation_Init;
	goto next_clause;
      };
    _master.endOfInput();
    ASSERT(_inputSupportClauses.empty());
  }; // void loadInputSupportClausesIntoMaster()



  void extractPreprocessedQueryClauses()
  {
    CALL("extractPreprocessedQueryClauses()");

    ASSERT(_inputQueryClauses.empty());
    _master.passiveResetIter();
    _master.historyResetIter();
    VK::Clause* cl;
    for (cl = _master.nextPassive(); cl; cl = _master.nextPassive())
      {
	ASSERT(!cl->isBuiltInTheoryFact());

	VK::Clause* copy = cl->unsharedCopy();
	_numberToClauseMapping.insert(copy);
	_inputQueryClauses.enqueue(copy);
      };
    for (cl = _master.nextHistory(); cl; cl = _master.nextHistory())
      {
	VK::Clause* copy = cl->unsharedCopy();
	_numberToClauseMapping.insert(copy);
	_preprocessingHistory.enqueue(copy); 
      };

    const ClauseQueue::Entry* qe;
    for (qe = _inputQueryClauses.begin(); 
	 qe; 
	 qe = qe->next())
      {
	cl = qe->value();
	if ((!cl->isInputClause()) &&
	    (!cl->isBuiltInTheoryFact()))
	  {
	    for (ClauseBackground::ClauseList* ancestors = 
		   cl->background().ancestors(); 
		 ancestors; 
		 ancestors = ancestors->tl())
	      {
		VK::Clause* ancClause = ancestors->hd();
		VK::Clause* ancCopy = 
		  _numberToClauseMapping.find(ancClause->Number());
		if (ancCopy)  { ancestors->hd() = ancCopy; }
		else 
		  {
		    ASSERT(ancClause->isInputClause() || ancClause->isBuiltInTheoryFact());
		  }; 
	      };  
	  };

      };

    for (qe = _preprocessingHistory.begin(); 
	 qe; 
	 qe = qe->next())
      {
	cl = qe->value(); 
	if ((!cl->isInputClause()) &&
	    (!cl->isBuiltInTheoryFact()))
	  {
	    for (ClauseBackground::ClauseList* ancestors = 
		   cl->background().ancestors(); 
		 ancestors; 
		 ancestors = ancestors->tl())
	      {
		VK::Clause* ancClause = ancestors->hd();
		VK::Clause* ancCopy = _numberToClauseMapping.find(ancClause->Number());
		if (ancCopy)  { ancestors->hd() = ancCopy; }
		else 
		  {
		    ASSERT(ancClause->isInputClause() || ancClause->isBuiltInTheoryFact());
		  }; 
	      };
	  };
      };
  }; // void extractPreprocessedQueryClauses()

  void prepareMasterForSaturation(long timeLimit, // deciseconds
				  float memoryLimit) // bytes
  {
    _master.reset();
    _master.options.incompleteAPriori = _incompleteProcedure;
    _master.options.main_alg = _options.main_alg;
    _master.options.binary_resolution = true;
    _master.options.no_inferences_between_rules = _options.no_inferences_between_rules;
    _master.options.splitting = _options.dynamic_splitting;  
    _master.options.splitting_style = _options.splitting_style;
    _master.options.splitting_with_naming = _options.splitting_with_naming;
    _master.options.neg_eq_splitting = 0L; // off  
    _master.options.paramodulation = _options.paramodulation;
    _master.options.sim_back_sup = _options.sim_back_sup;
    _master.options.selection = _options.selection;
    _master.options.literal_comparison_mode = _options.literal_comparison_mode;
    _master.options.inherited_negative_selection = _options.inherited_negative_selection;

    // strategy switches
    _master.options.handle_equality = _options.paramodulation;
    _master.options.forward_subsumption = _options.forward_subsumption;
    _master.options.fs_set_mode = _options.fs_set_mode;
    _master.options.fs_optimised_multiset_mode = _options.fs_optimised_multiset_mode;
    _master.options.fs_use_sig_filters = _options.fs_use_sig_filters;
    _master.options.fs_old_style = _options.fs_old_style;
    _master.options.simplify_by_fs = _options.simplify_by_fs;
    _master.options.forward_demodulation = _options.forward_demodulation;
    _master.options.fd_on_splitting_branches = _options.fd_on_splitting_branches;
    if (_options.normalize_by_commutativity)
      {
	_master.options.normalize_by_commutativity = _problemProfile.commutativityTable();
      }
    else
      {
	_master.options.normalize_by_commutativity = 0;
      };
    _master.options.backward_demodulation = _options.backward_demodulation;
    _master.options.bd_on_splitting_branches = _options.bd_on_splitting_branches;
    _master.options.simplify_by_eq_res = _options.simplify_by_eq_res;
    _master.options.backward_subsumption = _options.backward_subsumption;
    _master.options.bs_set_mode = _options.bs_set_mode;
    _master.options.bs_optimised_multiset_mode = _options.bs_optimised_multiset_mode;
    _master.options.orphan_murder = _options.orphan_murder;
    _master.options.lrs = _options.lrs;

    _master.options.pred_elim_table = 0;

    // numeric options
    _master.options.age_weight_ratio = _options.age_weight_ratio;
    _master.options.timeLimit = timeLimit;
    _master.options.memoryLimit = memoryLimit;
    _master.options.allocation_buffer_price = (float)_options.allocation_buffer_price;
    _master.options.maxActive = _options.max_active;
    _master.options.maxPassive = _options.max_passive;
    _master.options.maxWeight = _options.max_weight;
    _master.options.maxInferenceDepth = _options.max_inference_depth;
    _master.options.first_time_check = ((float)_options.first_time_check)/100;
    _master.options.first_mem_check = ((float)_options.first_mem_check)/100;
    // interface options 
    _master.options.outputStreamForGenerated = (_options.show_gen) ? _options.std_output : 0;
    _master.options.showPassive = (_options.show_passive) ? outputClause : 0;
    _master.options.showActive = (_options.show_active) ? outputClause : 0;
    _master.options.showWeightChange = (_options.show_weight_limit_changes) ? outputWeightChange : 0;


#ifdef VKERNEL_FOR_STEP_RESOLUTION
    _master.options.step_resolution_literal_selection = _options.step_resolution_literal_selection;
#endif


    _master.setStrategy();
#ifdef INTERCEPT_BACKWARD_MATCHING_INDEXING
    BMILog.unblock();
#endif

#ifdef INTERCEPT_INDEXED_SEARCH_FOR_GENERALISATIONS
    SFGILog.unblock();
#endif

#ifdef INTERCEPT_BACKWARD_SUPERPOSITION_INDEXING
    BSIULog.unblock();
#endif

#ifdef INTERCEPT_BACKWARD_SUBSUMPTION_INDEXING
    BSILog.unblock();
#endif
  }; // void prepareMasterForSaturation(long timeLimit,float memoryLimit)


  void reportTermination()
  {
    CALL("reportTermination()");
    
    outputStatistics();
    switch (_master.terminationReason())
      {
      case Master::UnknownTR:
	ICP("ICP0");
	if (_options.std_output)
	  *_options.std_output << "% Proof not found. Internal error: unknown termination type.\n"; 
	_terminationReason = VampireKernel::UnknownTerminationReason;
	break;  
      case Master::Proof: 
	if (_options.std_output)
	  *_options.std_output << "% Proof found. Thanks to Tanya!\n"; 
	_terminationReason = VampireKernel::RefutationFound;
	if (_options.proof && _options.std_output)
	  {
	    _proofTree.collect(_master.proof());    
	    *_options.std_output << "%================== Proof: ======================\n"; 
	    _proofTree.resetIter();   
	    for (const VK::Clause* cl = _proofTree.getNextClause(); 
		 cl; 
		 cl = _proofTree.getNextClause())
	      *_options.std_output << "% " << cl << '\n';
	    
	    *_options.std_output << "%==============  End of proof. ==================\n"; 
	  }; 
	break; 
      case Master::EmptyPassive: 
	if (_master.completeStrategyUsed() && 
	    (!_lookingForMoreAnswers))
	  {
	    if (_options.std_output)
	      *_options.std_output << "% Unprovable.\n";  
	    _terminationReason = VampireKernel::Satisfiable;
	  }
	else 
	  { 
	    if (_options.std_output)
	      *_options.std_output << "% Proof not found. Empty passive.\n"; 
	    _terminationReason = VampireKernel::EmptyPassive;
	  };
	break;

      case Master::MaxPassive: 
	if (_options.std_output)
	  *_options.std_output << "% Proof not found. Max. num. of passive reached. \n";
	_terminationReason = VampireKernel::MaxPassive;
	break; 
      case Master::MaxActive: 
	if (_options.std_output)
	  *_options.std_output << "% Proof not found. Max. num. of active reached. \n"; 
	_terminationReason = VampireKernel::MaxActive;
	break; 
      }; 
    if (_options.std_output) _options.std_output->flush();
  }; // void reportTermination()  

  void reportTimeLimitExpired()
  {
    CALL("reportTimeLimitExpired()");  
    _terminationReason = VampireKernel::TimeLimitExpired;
    if (_saturationRunning)
      {
	outputStatistics();
      };
    if (_options.std_output)
      {
	*_options.std_output << "% Proof not found. Time limit expired.\n";
	_options.std_output->flush();
      };
  }; // void reportTimeLimitExpired()


  static void setHardMemoryLimit(float lim) // bytes
  {
    CALL("setHardMemoryLimit(float lim)");
    if (lim < 0.0) lim = 0.0;
    long truncated;
    if (lim > (LONG_MAX/2))
      {
	truncated = LONG_MAX/2;
      }
    else
      truncated = (long)lim;
    GlobAlloc::setHardMemoryLimit(truncated);
    GlobAlloc::basicAllocationFailureHandling().setMemoryReleaseRequest(releaseMemory);
    GlobAlloc::basicAllocationFailureHandling().setUnrecoverableAllocationFailureHandler(handleAllocationFailure);
  }; // setHardMemoryLimit(float lim

  static void setAllocationBufferSize(long percentage)
  {
    GlobAlloc::setAllocationBufferSize(percentage);
  }; 

  static bool releaseMemory(ulong size)
  {
    CALL("releaseMemory(ulong size)");
    if (_currentSession && (_currentSession->_mainLoopRunning))
      {
	return _currentSession->_master.releaseMemory(size);
      }
    else
      return false;
  }; // bool releaseMemory(ulong size)
  
  static void handleAllocationFailure(ulong size)
  {
    CALL("handleAllocationFailure(ulong size)");
    if (_currentSession) 
      {
	_currentSession->reportAllocationFailure(size);
	DestructionMode::makeFast();
	throw VampireKernel::Emergency(VampireKernel::Emergency::AllocationFailure);
      }
    else
      {
	cout << "% Allocation failure occured when no kernel session is running.\n";
	DestructionMode::makeFast();
	throw VampireKernel::Emergency(VampireKernel::Emergency::AllocationFailure);
      };
  }; // void handleAllocationFailure(ulong size)

  void reportAllocationFailure(ulong size)
  {
    CALL("reportAllocationFailure(ulong size)");
    _terminationReason = VampireKernel::AbortedOnAllocationFailure;
    if (_options.std_output)
      {
	*_options.std_output << "% ------------- ALLOCATION FAILURE: " 
			     << size << " bytes -----------------\n";
	outputStatistics();
	*_options.std_output << "% Proof not found. Allocation failure.\n";
	_options.std_output->flush();
      };
  }; // void reportAllocationFailure(ulong size)

  static void outputClause(const VK::Clause* cl)
  {
    if (_currentSession && (_currentSession->_options.std_output))
      *(_currentSession->_options.std_output) << "% " << cl << "\n";
  };
  
  static void outputWeightChange(long oldWL,long newWL)
  {
    if (_currentSession && (_currentSession->_options.std_output))
      *(_currentSession->_options.std_output) << "% WEIGHT LIMIT CHANGED: " << oldWL << " ------> " << newWL << "\n";
  };


private:

  enum Continuation
  {
    Continuation_Init,
    Continuation_Prepro_Simplification,
    Continuation_Prepro_DefElimination_LoadInput,
    Continuation_Prepro_DefElimination_DefElimination,
    Continuation_LoadInputQuery,
    Continuation_LoadInputSupport,
    Continuation_MainLoop
  };

  class TermWeightingWrapper
  {
  public:
    TermWeightingWrapper(const VampireKernel::Options::TermWeightingScheme& sc)
    {
      CALL("TermWeightingWrapper(const VampireKernel::Options::TermWeightingScheme& sc)");      
      switch (sc)
	{
	case VampireKernel::Options::UniformTermWeighting:
	  _uniformTermWeighting = new UniformTermWeighting();
	  TermWeighting::setCurrent(static_cast<TermWeighting*>(_uniformTermWeighting));
	  _nonuniformConstantTermWeighting = 0;
	  break;
	case VampireKernel::Options::NonuniformConstantTermWeighting:
	  _nonuniformConstantTermWeighting = new NonuniformConstantTermWeighting();
	  TermWeighting::setCurrent(static_cast<TermWeighting*>(_nonuniformConstantTermWeighting));
	  _uniformTermWeighting = 0;
	  break;
	};
    };
    ~TermWeightingWrapper()
    {
      if (DestructionMode::isThorough())
	{
	  if (_uniformTermWeighting) delete _uniformTermWeighting;
	  if (_nonuniformConstantTermWeighting) delete _nonuniformConstantTermWeighting;
	};
    };
  
    void init(const VampireKernel::Options::TermWeightingScheme& sc)
    {
      CALL("init(const VampireKernel::Options::TermWeightingScheme& sc)");
      switch (sc)
	{
	case VampireKernel::Options::UniformTermWeighting:
	  _uniformTermWeighting = new UniformTermWeighting();
	  TermWeighting::setCurrent(static_cast<TermWeighting*>(_uniformTermWeighting));
	  _nonuniformConstantTermWeighting = 0;
	  break;
	case VampireKernel::Options::NonuniformConstantTermWeighting:
	  _nonuniformConstantTermWeighting = new NonuniformConstantTermWeighting();
	  TermWeighting::setCurrent(static_cast<TermWeighting*>(_nonuniformConstantTermWeighting));
	  _uniformTermWeighting = 0;
	  break;
	};
    }; // void init(const VampireKernel::Options::TermWeightingScheme& sc)
  
    void destroy()
    {
      CALL("destroy()");    
      if (DestructionMode::isThorough())
	{
	  if (_uniformTermWeighting) delete _uniformTermWeighting;
	  if (_nonuniformConstantTermWeighting) delete _nonuniformConstantTermWeighting;
	};
    }; // void destroy()


  private:
    UniformTermWeighting* _uniformTermWeighting;
    NonuniformConstantTermWeighting* _nonuniformConstantTermWeighting;
  }; // class TermWeightingWrapper

  class SimplificationOrderingWrapper
  {
  public:
    SimplificationOrderingWrapper(const VampireKernel::Options::SimplificationOrdering& so)
    {
      switch (so)
	{ 
	case VampireKernel::Options::NonrecursiveKBOrdering:
	  _nonrecursiveKBOrdering = new NonrecursiveKBOrdering();
	  SimplificationOrdering::setCurrent(static_cast<SimplificationOrdering*>(_nonrecursiveKBOrdering));
	  _standardKBOrdering = 0;
	  break;
	case VampireKernel::Options::StandardKBOrdering:
	  _standardKBOrdering = new StandardKBOrdering();
	  SimplificationOrdering::setCurrent(static_cast<SimplificationOrdering*>(_standardKBOrdering));
	  _nonrecursiveKBOrdering = 0;
	  break;	
	};
    };
    ~SimplificationOrderingWrapper() 
    { 
      if (DestructionMode::isThorough())
	{
	  if (_nonrecursiveKBOrdering) delete _nonrecursiveKBOrdering;
	  if (_standardKBOrdering) delete _standardKBOrdering;
	};
    };
    void init(const VampireKernel::Options::SimplificationOrdering& so)
    {
      CALL("init(const VampireKernel::Options::SimplificationOrdering& so)");
      switch (so)
	{ 
	case VampireKernel::Options::NonrecursiveKBOrdering:
	  _nonrecursiveKBOrdering = new NonrecursiveKBOrdering();
	  SimplificationOrdering::setCurrent(static_cast<SimplificationOrdering*>(_nonrecursiveKBOrdering));
	  _standardKBOrdering = 0;
	  break;
	case VampireKernel::Options::StandardKBOrdering:
	  _standardKBOrdering = new StandardKBOrdering();
	  SimplificationOrdering::setCurrent(static_cast<SimplificationOrdering*>(_standardKBOrdering));
	  _nonrecursiveKBOrdering = 0;
	  break;	
	};
    }; // void init(const VampireKernel::Options::SimplificationOrdering& so)

    void destroy()
    {
      CALL("destroy()");     
      if (DestructionMode::isThorough())
	{
	  if (_nonrecursiveKBOrdering) delete _nonrecursiveKBOrdering;
	  if (_standardKBOrdering) delete _standardKBOrdering;
	};
    };

  private:
    NonrecursiveKBOrdering* _nonrecursiveKBOrdering;
    StandardKBOrdering* _standardKBOrdering;
  }; // class SimplificationOrderingWrapper




private:
  void outputInputSupportClauses()
  {
    if (!_options.std_output) return;
    for (const ClauseQueue::Entry* cl = _inputSupportClauses.begin();
	 cl;
	 cl = cl->next())
      {
	*_options.std_output << "% " << cl->value() << "\n";
      };
  };

  void outputInputQueryClauses()
  {
    if (!_options.std_output) return;
    for (const ClauseQueue::Entry* cl = _inputQueryClauses.begin();
	 cl;
	 cl = cl->next())
      {
	*_options.std_output << "% " << cl->value() << "\n";
      };
  };
  

  
  void outputStatistics()
  {
    if (!_options.std_output) return;
    *_options.std_output << "%================= Statistics: =======================\n";
    *_options.std_output << "% time " << _timer.elapsedSeconds() 
			 << " (" << _master.timer().elapsedSeconds() << " current loop)\n";
    *_options.std_output << "% memory " << (GlobAlloc::occupiedByObjects()/1024) 
			 << "Kb (" << (GlobAlloc::occupiedByChunks()/1024) << "Kb in chunks), " 
			 << "total buffer deficit " << (GlobAlloc::totalBufferDeficit()/1024) << "Kb\n"; 
    *_options.std_output << "%=== Generated clauses:\n";
    *_options.std_output << "%      total " << _master.statNumOfGeneratedCl() 
			 << " (" << _master.statNumOfClGeneratedByResolution() << " res + "
                         <<  _master.statNumOfClGeneratedByEqRules() << " eq + "
                         <<  _master.statNumOfGeneratedCl() - (_master.statNumOfClGeneratedByResolution() + _master.statNumOfClGeneratedByEqRules())
                         << " other)\n";
    *_options.std_output << "%      forward subsumed " << _master.statFSubsumed() << '\n';
    *_options.std_output << "%      eq.tautologies   " << _master.statEqTautologies() << '\n';
    *_options.std_output << "%      prop.tautologies " << _master.statPropTautologies() << '\n';
    *_options.std_output << "%      simplified " << _master.statFSimplified() << '\n';
    *_options.std_output << "%         by FSR          " << _master.statFSimplifiedByFSR() << '\n';
    *_options.std_output << "%         by demod.       " << _master.statFSimplifiedByDemod() << '\n';
    *_options.std_output << "%               on splitting branches " << _master.statFDOnSplittingBranches() << '\n';
    *_options.std_output << "%         by built-in th. " << _master.statSimplifiedByBuiltInTheories() << '\n'; 
    *_options.std_output << "%         by eq. res.     " << _master.statFSimplifiedByEqRes() << '\n';
    *_options.std_output << "%      split " << _master.statSplit() 
			 << "  avg.components " << _master.statAvgComponents() 
			 << "  diff.components " << _master.statDiffComponents() << '\n';
    *_options.std_output << "%      useless:\n";
    *_options.std_output << "%        too deep inferences " << _master.statTooDeepInferences() << '\n';
    *_options.std_output << "%        too heavy " << _master.statTooHeavy() << '\n'; 
    *_options.std_output << "%        too heavy literals " << _master.statTooHeavyLiterals() << '\n'; 
    *_options.std_output << "%        too long " << _master.statTooLong() << '\n';  
    *_options.std_output << "%        too deep " << _master.statTooDeep() << '\n';  
    *_options.std_output << "%        too many variables " << _master.statTooManyVariables() << '\n';  
    *_options.std_output << "%        refused allocation " << _master.statRefusedAllocation() << '\n';

    *_options.std_output << "%=== Retained clauses:\n";
    *_options.std_output << "%      total " << _master.statRetained() << '\n';
    *_options.std_output << "%      used " << _master.statUsed() << '\n';
    *_options.std_output << "%      back subsumed " << _master.statBSubsumed() << '\n';
    *_options.std_output << "%      back simplified " << _master.statBSimplified() << '\n';
    *_options.std_output << "%        on splitting branches " << _master.statBDOnSplittingBranches() << '\n';
    *_options.std_output << "%      rewritten by def. unfolding " << _master.statRewrittenByDefUnfolding() << '\n';
    *_options.std_output << "%      orphans murdered " << _master.statOrphansMurdered() << '\n';
    *_options.std_output << "%               passive " << _master.statPassiveOrphansMurdered() << '\n';
    *_options.std_output << "%               active  " << _master.statActiveOrphansMurdered() << '\n';
    *_options.std_output << "%      recycled " << _master.statRecycled() << '\n';
    *_options.std_output << "%      currently passive " << _master.statCurrentlyPassive()        
			 << " (" << _master.statPassiveReachable() << " reachable)\n";
    *_options.std_output << "%      reserved passive " << _master.statReservedPassive() << '\n';
    *_options.std_output << "%      currently active " << _master.statCurrentlyActive() << '\n'; 

    *_options.std_output << "%========== End of statistics. ======================\n";
    _options.std_output->flush();
  }; // void outputStatistics()

  void tabulateStatistics()
  {
    CALL("tabulateStatistics()");
    _tabulation->stat("time",_timer.elapsedSeconds());
    _tabulation->stat("memory",GlobAlloc::occupiedByObjects()/1024);
    _tabulation->stat("generated",_master.statNumOfGeneratedCl());
    _tabulation->stat("eq_tautologies",_master.statEqTautologies());
    _tabulation->stat("prop_tautologies",_master.statPropTautologies());
    _tabulation->stat("forward_subsumed",_master.statFSubsumed());
    _tabulation->stat("forward_simplified",_master.statFSimplified());
    _tabulation->stat("forward_demodulated_on_splitting_branches",_master.statFDOnSplittingBranches());
    _tabulation->stat("too_heavy_literals",_master.statTooHeavyLiterals());
    _tabulation->stat("too_deep",_master.statTooDeep());
    _tabulation->stat("too_heavy_clauses",_master.statTooHeavy());
    _tabulation->stat("too_long_clauses",_master.statTooLong());
    _tabulation->stat("too_many_variables",_master.statTooManyVariables());
    _tabulation->stat("backward_subsumed",_master.statBSubsumed());
    _tabulation->stat("backward_simplified",_master.statBSimplified());
    _tabulation->stat("backward_demodulated_on_splitting_branches",_master.statBDOnSplittingBranches());
    _tabulation->stat("rewritten_by_def_unfolding",_master.statRewrittenByDefUnfolding());
    _tabulation->stat("passive_orphans_murdered",_master.statPassiveOrphansMurdered()); 
    _tabulation->stat("active_orphans_murdered",_master.statActiveOrphansMurdered()); 
    _tabulation->stat("used",_master.statUsed());
    _tabulation->stat("kept_total",_master.statRetained());
    _tabulation->stat("solved_by_preprocessing",_solvedByPreprocessing);
    _tabulation->stat("split",_master.statSplit()); 
    _tabulation->stat("different_components",_master.statDiffComponents());
    _tabulation->stat("average_components",_master.statAvgComponents());
  }; // void tabulateStatistics()


  void tabulatePreprocessingStatistics()
  {
    CALL("tabulatePreprocessingStatistics()");
    _tabulation->stat("prepro_split",_preproStatSplit); 
    _tabulation->stat("prepro_different_components",
		      _preproStatDifferentComponents);
    _tabulation->stat("prepro_average_components",
		      (_preproStatSplit) ? 
		      (((float)_preproStatComponents)/_preproStatSplit) :
		      0.0);
  }; // void tabulatePreprocessingStatistics()

  const char* flagValue(bool flag)
  {
    if (flag) return "on";
    return "off";
  };

  void tabulateOptions()
  {
    CALL("tabulateOptions()");
    if (!_tabulation) return;

    // main algorithm
    _tabulation->option("main_alg",_options.main_alg);

    // inference rules
    _tabulation->option("no_inferences_between_rules",flagValue(_options.no_inferences_between_rules));
    _tabulation->option("static_splitting",flagValue(_options.static_splitting));
    _tabulation->option("dynamic_splitting",flagValue(_options.dynamic_splitting));
    _tabulation->option("splitting_style",_options.splitting_style);
    _tabulation->option("splitting_with_naming",flagValue(_options.splitting_with_naming));
    _tabulation->option("static_neg_eq_splitting",flagValue(_options.static_neg_eq_splitting != 0L));
    _tabulation->option("paramodulation",flagValue(_options.paramodulation));
    _tabulation->option("sim_back_sup",_options.sim_back_sup);
    _tabulation->option("selection",_options.selection);
    _tabulation->option("literal_comparison_mode",_options.literal_comparison_mode);
    _tabulation->option("inherited_negative_selection",flagValue(_options.inherited_negative_selection));
     
    // strategy 
    _tabulation->option("forward_subsumption",flagValue(_options.forward_subsumption));
    _tabulation->option("fs_set_mode",flagValue(_options.fs_set_mode));
    _tabulation->option("fs_optimised_multiset_mode",flagValue(_options.fs_optimised_multiset_mode));
    _tabulation->option("fs_use_sig_filters",flagValue(_options.fs_use_sig_filters));
    _tabulation->option("fs_old_style",flagValue(_options.fs_old_style));
    _tabulation->option("simplify_by_fs",_options.simplify_by_fs);
    _tabulation->option("forward_demodulation",_options.forward_demodulation);
    _tabulation->option("fd_on_splitting_branches",flagValue(_options.fd_on_splitting_branches));
    _tabulation->option("normalize_by_commutativity",flagValue(_options.normalize_by_commutativity));
    _tabulation->option("backward_demodulation",_options.backward_demodulation);
    _tabulation->option("bd_on_splitting_branches",flagValue(_options.bd_on_splitting_branches));
    _tabulation->option("simplify_by_eq_res",flagValue(_options.simplify_by_eq_res));
    _tabulation->option("backward_subsumption",_options.backward_subsumption);
    _tabulation->option("bs_set_mode",flagValue(_options.bs_set_mode));
    _tabulation->option("bs_optimised_multiset_mode",flagValue(_options.bs_optimised_multiset_mode));
    _tabulation->option("orphan_murder",flagValue(_options.orphan_murder != 0L));
    _tabulation->option("lrs",_options.lrs);
    _tabulation->option("elim_def",_options.elim_def);
    
    switch (_options.simplification_ordering)
      {
      case VampireKernel::Options::NonrecursiveKBOrdering:
	_tabulation->option("simplification_ordering","nonrecursive_KB");
	break;
      case VampireKernel::Options::StandardKBOrdering:
	_tabulation->option("simplification_ordering","standard_KB");
	break;
      default: 
	RuntimeError::report("Wrong value of --simplification_ordering.");
      };

    _tabulation->option("symbol_precedence_by_arity",_options.symbol_precedence_by_arity);
    _tabulation->option("header_precedence_kinky",flagValue(_options.header_precedence_kinky));
    switch (_options.term_weighting_scheme)
      {
      case VampireKernel::Options::UniformTermWeighting:
	_tabulation->option("term_weighting_scheme","uniform");
	break;
      case VampireKernel::Options::NonuniformConstantTermWeighting: 
	_tabulation->option("term_weighting_scheme","nonuniform_constant");
	break;
      default:
	RuntimeError::report("Wrong value of --term_weighting_scheme.");
      };
    _tabulation->option("symbol_weight_by_arity",_options.symbol_weight_by_arity);


    // numeric options 
    _tabulation->option("time_limit",((float)_saturationTimeLimit)/10);
    _tabulation->option("memory_limit",_options.memory_limit);
    _tabulation->option("allocation_buffer_size",_options.allocation_buffer_size); 
    _tabulation->option("allocation_buffer_price",_options.allocation_buffer_price); 
    _tabulation->option("max_skolem",_options.max_skolem);
    _tabulation->option("max_active",_options.max_active);
    _tabulation->option("max_passive",_options.max_passive);
    _tabulation->option("max_weight",_options.max_weight);
    _tabulation->option("max_inference_depth",_options.max_inference_depth);
    _tabulation->option("age_weight_ratio",_options.age_weight_ratio);
    _tabulation->option("selected_pos_eq_penalty_coefficient",_options.selected_pos_eq_penalty_coefficient);    
    _tabulation->option("nongoal_penalty_coefficient",_options.nongoal_penalty_coefficient);
    _tabulation->option("first_time_check",_options.first_time_check);   
    _tabulation->option("first_mem_check",_options.first_mem_check);   

    // interface options
    _tabulation->option("proof",flagValue(_options.proof));

#ifdef VKERNEL_FOR_STEP_RESOLUTION
    _tabulation->option("step_resolution_literal_selection",flagValue(_options.step_resolution_literal_selection));
#endif
  
  }; // void tabulateOptions()


  void tabulate()
  {
    CALL("tabulate()");
    if (!_tabulation) return;

    COP("COP100");

    tabulateOptions();
    COP("COP110");
    _signature.tabulate(_tabulation);
    COP("COP120");
    tabulatePreprocessingStatistics();
    COP("COP130");
    tabulateStatistics();
    COP("COP140");
    
    if ((_terminationReason == VampireKernel::RefutationFound) && 
	_options.proof)
      {
	// _proofTree is supposed to be collected here,
        // see reportTermination()
	COP("COP150");
	_proofTree.resetIter();   
	COP("COP160");
	for (const VK::Clause* cl = _proofTree.getNextClause(); 
	     cl; 
	     cl = _proofTree.getNextClause())
	  {
	    COP("COP170");
	    _tabulation->proofClause(cl);
	  };
      }; 
    COP("END");
  }; // void tabulate()


  void switchBuiltinEqualityOff()
  {
    CALL("switchBuiltinEqualityOff()");
    if  (_options.paramodulation 
	 || _options.inherited_negative_selection
	 || _options.forward_demodulation
	 || _options.fd_on_splitting_branches
	 || _options.backward_demodulation
	 || _options.bd_on_splitting_branches
	 || _options.simplify_by_eq_res)
      {
	if (_options.std_output)
	  *_options.std_output << "% WARNING: No equality. All equality related features are switched off!\n";   
	_options.paramodulation = false;
	_options.inherited_negative_selection = false;
	_options.forward_demodulation = false;
	_options.fd_on_splitting_branches = false;
	_options.backward_demodulation = false;
	_options.bd_on_splitting_branches = false;
	_options.simplify_by_eq_res = false;
      };
  }; // void switchBuiltinEqualityOff()

  void analyseOptions()
  {
    CALL("analyseOptions()");
    switch (_options.selection)
      { 
      case 1006L:
      case 1007L:
      case 1008L:
      case 1009L:
      case -1009L:
	_options.inherited_negative_selection = false;
	if (_options.std_output)
	  *_options.std_output << "% WARNING: bidirectional literal selection "
			       << _options.selection 
			       << " switches inherited_negative_selection off!\n";
      };
    
    if (_options.header_precedence_kinky)
      {
	_incompleteProcedure = true;
	// may be refined in the future 
      };
    
    if ((_options.term_weighting_scheme != VampireKernel::Options::UniformTermWeighting) &&
	(_options.term_weighting_scheme != VampireKernel::Options::NonuniformConstantTermWeighting))
      {
	_incompleteProcedure = true;
	// will be refined in the future 
      };


    if (_options.fs_set_mode || _options.bs_set_mode)
      {	
	_incompleteProcedure = true;
      };
    
  }; // void analyseOptions()


  void collectPreprocessingIterationStatistics()
  {   
    _preproStatSplit += _master.statSplit();
    _preproStatComponents += _master.statComponents();
    _preproStatDifferentComponents += _master.statDiffComponents();

    if (_master.statTooHeavy() || 
	_master.statTooHeavyLiterals() || 
	_master.statTooManyVariables() || 
	_master.statTooLong() || 
	_master.statTooDeep() || 
	_master.statRecycled() || 
	_master.statRefusedAllocation())
      {
	_incompleteProcedure = true;
      }; 

  }; // void collectPreprocessingIterationStatistics()



  static bool isRelevantBackgroundFlag(const VK::ClauseBackground::Rule& fl)
  {
    switch (fl)
      {
      case VK::ClauseBackground::Input:
      case VK::ClauseBackground::Prepro:
      case VK::ClauseBackground::BinRes:
      case VK::ClauseBackground::ForwSup: 
      case VK::ClauseBackground::BackSup:
      case VK::ClauseBackground::EqRes:
      case VK::ClauseBackground::EqFact:
      case VK::ClauseBackground::ForwDemod:
      case VK::ClauseBackground::BackDemod: 
      case VK::ClauseBackground::EqResSimp:
      case VK::ClauseBackground::ForwSubsRes:
      case VK::ClauseBackground::BackSubsRes:  
      case VK::ClauseBackground::Split:
      case VK::ClauseBackground::Reanimation:
      case VK::ClauseBackground::Name:
      case VK::ClauseBackground::SimplificationByBuiltInTheories:
      case VK::ClauseBackground::BuiltInTheoryFact:
        return true;
      default:
        return false;
      };
  }; // bool isRelevantBackgroundFlag(const VK::ClauseBackground::Rule& fl)

  static 
  VampireKernel::Clause::BackgroundFlag
  convertBackgroundFlag(const VK::ClauseBackground::Rule& fl)
  {
    switch (fl)
      {
	
      case VK::ClauseBackground::Input: 
	return VampireKernel::Clause::IsInputClause;
      case VK::ClauseBackground::Prepro: 
	return VampireKernel::Clause::IsObtainedDuringPreprocessing;
      case VK::ClauseBackground::BinRes: 
	return VampireKernel::Clause::BinaryResolutionUsed;
      case VK::ClauseBackground::ForwSup: 
	return VampireKernel::Clause::ForwardSuperpositionUsed; 
      case VK::ClauseBackground::BackSup: 
	return VampireKernel::Clause::BackwardSuperpositionUsed;
      case VK::ClauseBackground::EqRes: 
	return VampireKernel::Clause::EqualityResolutionUsed;
      case VK::ClauseBackground::EqFact: 
	return VampireKernel::Clause::EqualityFactoringUsed;
      case VK::ClauseBackground::ForwDemod: 
	return VampireKernel::Clause::ForwardDemodulationUsed;
      case VK::ClauseBackground::BackDemod: 
	return VampireKernel::Clause::BackwardDemodulationUsed; 
      case VK::ClauseBackground::EqResSimp: 
	return VampireKernel::Clause::SimplificationByEqualityResolutionUsed;
      case VK::ClauseBackground::ForwSubsRes: 
	return VampireKernel::Clause::ForwardSubsumptionResolutionUsed;
      case VK::ClauseBackground::BackSubsRes: 
	return VampireKernel::Clause::BackwardSubsumptionResolutionUsed;  
      case VK::ClauseBackground::Split: 
	return VampireKernel::Clause::SplittingUsed;
      case VK::ClauseBackground::Reanimation: 
	return VampireKernel::Clause::IsReanimatedPassive;
      case VK::ClauseBackground::Name: 
	return VampireKernel::Clause::IsPartOfNameIntroduction;
      case VK::ClauseBackground::SimplificationByBuiltInTheories: 
	return VampireKernel::Clause::SimplificationByBuiltInTheoriesUsed;
      case VK::ClauseBackground::BuiltInTheoryFact: 
	return VampireKernel::Clause::IsBuiltInTheoryFact;
	
      default:
	return VampireKernel::Clause::IsInputClause; // to avoid warnings
      };

  }; // VampireKernel::Clause::BackgroundFlag convertBackgroundFlag(const VK::ClauseBackground::Rule& fl)

  static 
  VK::ClauseBackground::Rule
  convertBackgroundFlag(const VampireKernel::Clause::BackgroundFlag& fl)
  {
    CALL("convertBackgroundFlag(const VampireKernel::Clause::BackgroundFlag& fl)");
    switch (fl)
      {
      case VampireKernel::Clause::IsInputClause:
	return VK::ClauseBackground::Input;
      case VampireKernel::Clause::IsObtainedDuringPreprocessing:
	return VK::ClauseBackground::Prepro;
      case VampireKernel::Clause::BinaryResolutionUsed:
	return VK::ClauseBackground::BinRes;
      case VampireKernel::Clause::ForwardSuperpositionUsed:
	return VK::ClauseBackground::ForwSup;
      case VampireKernel::Clause::BackwardSuperpositionUsed:
	return VK::ClauseBackground::BackSup;
      case VampireKernel::Clause::EqualityResolutionUsed:
	return VK::ClauseBackground::EqRes;
      case VampireKernel::Clause::EqualityFactoringUsed:
	return VK::ClauseBackground::EqFact;
      case VampireKernel::Clause::ForwardDemodulationUsed:
	return VK::ClauseBackground::ForwDemod;
      case VampireKernel::Clause::BackwardDemodulationUsed:
	return VK::ClauseBackground::BackDemod;
      case VampireKernel::Clause::SimplificationByEqualityResolutionUsed:
	return VK::ClauseBackground::EqResSimp;
      case VampireKernel::Clause::ForwardSubsumptionResolutionUsed:
	return VK::ClauseBackground::ForwSubsRes;
      case VampireKernel::Clause::BackwardSubsumptionResolutionUsed:
	return VK::ClauseBackground::BackSubsRes;
      case VampireKernel::Clause::SplittingUsed:
	return VK::ClauseBackground::Split; 
      case VampireKernel::Clause::IsReanimatedPassive:
	return VK::ClauseBackground::Reanimation;
      case VampireKernel::Clause::IsPartOfNameIntroduction:
	return VK::ClauseBackground::Name;
      case VampireKernel::Clause::SimplificationByBuiltInTheoriesUsed:
	return VK::ClauseBackground::SimplificationByBuiltInTheories;
      case VampireKernel::Clause::IsBuiltInTheoryFact:
	return VK::ClauseBackground::BuiltInTheoryFact;
      default:
	return VK::ClauseBackground::Input; // to avoid warnings
      };
    
  }; // VK::ClauseBackground::Rule convertBackgroundFlag(const VampireKernel::Clause::BackgroundFlag& fl)

private:  
  static VampireKernel::_Implementation* _currentSession;

  WeightPolynomial::Sharing _weightPolynomialSharing;

  TermWeightingWrapper _termWeightingWrapper;
  SimplificationOrderingWrapper _simplificationOrderingWrapper;

  VampireKernel* _wrapper;
  VampireKernel::Options _options;
  GlobalClock::Timer _timer;
  long _saturationTimeLimit;
  bool _saturationRunning;
  bool _mainLoopRunning;
  Signature _signature;
  VK::Clause::OpenUnshared _newInputClause;
  ulong _currentInputClauseNumber;
  void* _currentInputClauseOrigin;

  ClauseQueue _inputSupportClauses;
  ClauseQueue _inputQueryClauses;

  bool _inputClauseRejected;
  ulong _numberOfRejectedInputClauses;
  bool _incompleteProcedure;
  ProblemProfile _problemProfile;
  Tabulation* _tabulation;
  VK::Clause::NewNumber _clauseNumberGenerator;

  BuiltInTheoryDispatcher* _builtInTheoryDispatcher;
  Master _master;
  ClauseQueue _preprocessingHistory;
  ClauseNumIndex _numberToClauseMapping;

  bool _solvedByPreprocessing;
  VampireKernel::TerminationReason _terminationReason;
  InferenceTree _proofTree;  
  ulong _preproStatSplit;
  ulong _preproStatComponents;
  ulong _preproStatDifferentComponents;
  bool _symbolWeightConstantPartInputIgnored;
  bool _unaryFunctionWithZeroWeightExists;
  ulong _unaryFunctionWithZeroWeight;
  Continuation _continuation;
  ulong _preproIterationNumber;
  long _preproEliminationCycles;
  bool _lookingForMoreAnswers;
  long _numberOfSupportClauses;
  bool _equalityInSupport;
  VK::Clause* _currentInputQueryClause;
  VK::Clause* _currentInputSupportClause;
  friend class VampireKernel;
  friend class VampireKernel::Clause;
  friend class VampireKernel::Clause::BackgroundFlagIterator;
}; // class VampireKernel::_Implementation

VampireKernel::_Implementation* 
VampireKernel::_Implementation::_currentSession;

//======================================================================
#undef DEBUG_NAMESPACE
#ifdef DEBUG_VAMPIRE_KERNEL
#define DEBUG_NAMESPACE "VampireKernel::_Implementation::ClauseSetIterator"
#endif    
#include "debugMacros.hpp"
//======================================================================

VampireKernel::_Implementation::ClauseSetIterator::~ClauseSetIterator()
{
  CALL("destructor ~ClauseSetIterator()");
};


bool VampireKernel::_Implementation::ClauseSetIterator::isEnd() const
{
  return _iter.isEnd();
}; 

void VampireKernel::_Implementation::ClauseSetIterator::next()
{
  _iter.next();
};

const VampireKernel::Clause* VampireKernel::_Implementation::ClauseSetIterator::clause() const
{
  return 
    static_cast<const VampireKernel::Clause*>(static_cast<const void*>(_iter.clause()));
};
    

//======================================================================
#undef DEBUG_NAMESPACE
#ifdef DEBUG_VAMPIRE_KERNEL
#define DEBUG_NAMESPACE "VampireKernel"
#endif    
#include "debugMacros.hpp"
//======================================================================

VampireKernel::VampireKernel() 
{
  CALL("constructor VampireKernel()");
  _implementation = 0; 
};
VampireKernel::~VampireKernel() 
{
  CALL("destructor ~VampireKernel()");
  if (DestructionMode::isThorough())
    {
      if (_implementation)
	{
	  _Implementation::closeSession();

	  
	  _implementation->destroy();
	  _Implementation::deallocate(_implementation);	
	  
	  //delete _implementation;
	};
      TmpLiteral::destroyStatic(); // ugly
    };
};

void VampireKernel::init()
{
  CALL("init()");
  _implementation = 0;
};

void VampireKernel::destroy() 
{
  CALL("destroy()");
  if (DestructionMode::isThorough())
    {
      if (_implementation)
	{
	  _Implementation::closeSession();

	 
	  _implementation->destroy();
	  _Implementation::deallocate(_implementation);	
	  
	  // delete _implementation;
	};
      TmpLiteral::destroyStatic(); // ugly
    };
}; // void VampireKernel::destroy() 

void VampireKernel::openSession(const Options& opt) 
{
  CALL("openSession(const Options& opt)");
  ASSERT(!GlobAlloc::occupiedByObjects());
  GlobalStopFlag::reset();
  _Implementation::setHardMemoryLimit(((float)opt.memory_limit)*1024);
  _Implementation::setAllocationBufferSize(opt.allocation_buffer_size);
  //_implementation = new _Implementation(opt);
 
  _implementation = _Implementation::allocate();
  _implementation->init(opt);
  
  _implementation->openSession();
  _implementation->_wrapper = this;
}; // void VampireKernel::openSession(const Options& opt) 

void VampireKernel::closeSession()
{
  _Implementation::closeSession();
  _implementation = 0;
  TmpLiteral::resetStatic(); // ugly
}; // void VampireKernel::closeSession()


float VampireKernel::versionNumber()
{
  return 5.53f;
};

const char* VampireKernel::versionDescription() 
{
  return "Abacus (5.52 + VC++ port resurrected)";
};

void VampireKernel::beginInputSignature() 
{
  _implementation->beginInputSignature();
};  
void VampireKernel::endInputSignature() 
{
  _implementation->endInputSignature();
};


void VampireKernel::registerInputFunction(const char* name,
					  ulong arity,
					  ulong& id)
{
  _implementation->registerInputFunction(name,arity,id);
}; 
  
  

void VampireKernel::registerInputPredicate(const char* name,
					   ulong arity,
					   bool isAnswer,
					   ulong& id)
{
  _implementation->registerInputPredicate(name,arity,isAnswer,id);
};  

void VampireKernel::setSymbolWeightConstPart(ulong symId,const VampireKernel::TermWeightType& weight) 
{
  _implementation->setSymbolWeightConstPart(symId,weight);
};


void VampireKernel::setSymbolPrecedence(ulong symId,long prec) 
{
  _implementation->setSymbolPrecedence(symId,prec);
}; 


void VampireKernel::interpretFunction(const char* inputSymbol,
				      ulong arity,
				      const char* theory,
				      const char* nativeSymbol)
{
  _implementation->interpret(false,inputSymbol,arity,theory,nativeSymbol);
};

void VampireKernel::interpretPredicate(const char* inputSymbol,
				       ulong arity,
				       const char* theory,
				       const char* nativeSymbol)
{
  _implementation->interpret(true,inputSymbol,arity,theory,nativeSymbol);
};

  
void VampireKernel::beginInputClauses() 
{
  _implementation->beginInputClauses();
};

void VampireKernel::endInputClauses() 
{
  _implementation->endInputClauses();
};
  
void VampireKernel::beginInputClause(ulong clauseNumber,
				     void* origin,
				     bool goal,
				     bool support) 
{
  _implementation->beginInputClause(clauseNumber,origin,goal,support); 
};

void VampireKernel::endInputClause() 
{
  _implementation->endInputClause();
};
  
void VampireKernel::beginLiteral(bool positive,ulong predicateId) 
{
  _implementation->beginLiteral(positive,predicateId);
};
void VampireKernel::endLiteral() 
{
  _implementation->endLiteral();
};
  
void VampireKernel::receiveFunction(ulong symId) 
{
  _implementation->receiveFunction(symId);
};

void VampireKernel::receiveNumericConstant(double value)
{
  _implementation->receiveNumericConstant(value);
};

void VampireKernel::receiveVariable(ulong varNum) 
{
  _implementation->receiveVariable(varNum);
};
    
void VampireKernel::saturate(ulong nextVacantClauseId,long timeLimit) 
{
  _implementation->saturate(nextVacantClauseId,timeLimit);
};


void VampireKernel::lookForAnotherAnswer()
{
  _implementation->lookForAnotherAnswer();
};


VampireKernel* VampireKernel::currentSession()
{
  if (_Implementation::_currentSession)
    {
      return _Implementation::_currentSession->_wrapper;
    }
  else
    return 0;
};


void VampireKernel::suspend()
{
  _implementation->suspend();
};

void VampireKernel::resume()
{
  _implementation->resume();
}; 

void VampireKernel::tabulate()
{
  CALL("tabulate()");
  _implementation->tabulate();
};


void VampireKernel::reportInterrupt(int signalNumber)
{
  if (_implementation)
    {
      _implementation->reportInterrupt(signalNumber);
    };
};
  
void VampireKernel::reportErrorSignal(int signalNumber)
{
  if (_implementation)
    {
      _implementation->reportErrorSignal(signalNumber);
    };
};

void VampireKernel::reportError()
{
  if (_implementation)
    {
      _implementation->reportError();
    };
};

// Communicating search results

VampireKernel::TerminationReason VampireKernel::terminationReason() const
{
  return _implementation->terminationReason();
};
const VampireKernel::Clause* VampireKernel::refutation() const 
{
  return _implementation->refutation();
};

VampireKernel::ClauseSetIterator* VampireKernel::activeClauses()
{
  return _implementation->activeClauses();
};

void VampireKernel::disposeOf(VampireKernel::ClauseSetIterator* itr)
{
  _implementation->disposeOf(itr);
};

bool VampireKernel::isNumericConstantId(ulong symbolId) const
{
  return _implementation->isNumericConstantId(symbolId);
}; 
bool VampireKernel::isInputSymbol(ulong symbolId) const
{
  return _implementation->isInputSymbol(symbolId);
}; 

bool VampireKernel::isAnswerPredicateId(ulong symbolId) const
{
  return _implementation->isAnswerPredicateId(symbolId);
};

ulong VampireKernel::symbolArity(ulong symbolId) const
{
  return _implementation->symbolArity(symbolId);
};
const char* VampireKernel::symbolPrintName(ulong symbolId) const
{
  return _implementation->symbolPrintName(symbolId);
}; 
double VampireKernel::numericConstantValue(ulong symbolId) const
{
  return _implementation->numericConstantValue(symbolId);
}; 



// Getting the session statistics
 
  
float VampireKernel::statTime() const
{
  return _implementation->_master.timer().elapsedSeconds();
};

long VampireKernel::statMemory() const
{
  return GlobAlloc::occupiedByObjects();
};


long VampireKernel::statNumOfClGeneratedByPrepro() const
{
  return _implementation->_master.statNumOfClGeneratedByPrepro();
};
long VampireKernel::statNumOfClGeneratedByReanimation() const
{
  return _implementation->_master.statNumOfClGeneratedByReanimation();
};
long VampireKernel::statNumOfClGeneratedByResolution() const
{
  return _implementation->_master.statNumOfClGeneratedByResolution();
};

long VampireKernel::statNumOfClGeneratedByForwSup() const
{
  return _implementation->_master.statNumOfClGeneratedByForwSup();
};
long VampireKernel::statNumOfClGeneratedByBackSup() const
{
  return _implementation->_master.statNumOfClGeneratedByBackSup();
};
long VampireKernel::statNumOfClGeneratedByEqRes() const
{
  return _implementation->_master.statNumOfClGeneratedByEqRes();
};
long VampireKernel::statNumOfClGeneratedByEqFact() const
{
  return _implementation->_master.statNumOfClGeneratedByEqFact();
};  
long VampireKernel::statNumOfClGeneratedByBackDemod() const
{
  return _implementation->_master.statNumOfClGeneratedByBackDemod();
};


long VampireKernel::statNumOfDiscardedEquationalTautologies() const
{
  return _implementation->_master.statEqTautologies();
};

long VampireKernel::statNumOfDiscardedPropositionalTautologies() const
{
  return _implementation->_master.statPropTautologies();
};

long VampireKernel::statNumOfForwSubsumedCl() const
{
  return _implementation->_master.statFSubsumed();
};

long VampireKernel::statNumOfClSimplifiedByForwSubsResol() const
{
  return _implementation->_master.statFSimplifiedByFSR();
};

long VampireKernel::statNumOfClSimplifiedByForwDemod() const
{
  return _implementation->_master.statFSimplifiedByDemod();
};

long VampireKernel::statNumOfClSimplifiedByForwDemodOnSplitBranches() const
{
  return _implementation->_master.statFDOnSplittingBranches();
};

long VampireKernel::statNumOfClSimplifiedByBuiltInTheories() const
{
  return _implementation->_master.statSimplifiedByBuiltInTheories();
};

long VampireKernel::statNumOfClSimplifiedByEqualityResol() const
{
  return _implementation->_master.statFSimplifiedByEqRes();
};

long VampireKernel::statNumOfSplitCl() const
{
  return _implementation->_master.statSplit();
};

long VampireKernel::statNumOfDifferentSplitComponents() const
{
  return _implementation->_master.statDiffComponents();
};

float VampireKernel::statAverageNumOfSplitComponentsPerCl() const
{
  return _implementation->_master.statAvgComponents();
};

long VampireKernel::statNumOfDiscardedClWithTooBigLiterals() const
{
  return _implementation->_master.statTooHeavyLiterals();
};

long VampireKernel::statNumOfDiscardedTooDeepCl() const
{
  return _implementation->_master.statTooDeep();
};

long VampireKernel::statNumOfDiscardedTooBigCl() const
{
  return _implementation->_master.statTooHeavy();
};

long VampireKernel::statNumOfDiscardedClWithTooManyVariables() const
{
  return _implementation->_master.statTooManyVariables();
};

long VampireKernel::statNumOfNewClDiscardedDueToLackOfMemory() const
{
  return _implementation->_master.statRefusedAllocation();
};

long VampireKernel::statNumOfRetainedCl() const
{
  return _implementation->_master.statRetained();
};

long VampireKernel::statNumOfSelectedCl() const
{
  return _implementation->_master.statUsed();
};

long VampireKernel::statNumOfBackSubsumedCl() const
{
  return _implementation->_master.statBSubsumed();
};

long VampireKernel::statNumOfClSimplifiedByBackDemod() const
{
  return _implementation->_master.statBSimplified();
};

long VampireKernel::statNumOfClSimplifiedByBackDemodOnSplitBranches() const
{
  return _implementation->_master.statBDOnSplittingBranches();
};

long VampireKernel::statNumOfMurederedPassiveOrphans() const
{
  return _implementation->_master.statPassiveOrphansMurdered();
};

long VampireKernel::statNumOfMurederedActiveOrphans() const
{
  return _implementation->_master.statActiveOrphansMurdered();
};

long VampireKernel::statNumOfClRecycledDueToLackOfResources() const
{
  return _implementation->_master.statRecycled();
};
  
long VampireKernel::statNumOfCurrentlyPassiveCl() const
{
  return _implementation->_master.statCurrentlyPassive();
};

long VampireKernel::statNumOfCurrentlyActiveCl() const
{
  return _implementation->_master.statCurrentlyActive();
};

//======================================================================
#undef DEBUG_NAMESPACE
#ifdef DEBUG_VAMPIRE_KERNEL
#define DEBUG_NAMESPACE "VampireKernel::Options"
#endif    
#include "debugMacros.hpp"
//======================================================================

void VampireKernel::Options::setDefaultValues()
{
  // session attributes
  job_id = "default_job_id";
  
  // main algorithm
    
  main_alg = 0L;

  // inference rules
  no_inferences_between_rules = false;
  static_splitting = true;
  dynamic_splitting = false;
  splitting_style = 1L;
  splitting_with_naming = true;
  static_neg_eq_splitting = 3L;
  paramodulation = true;
  sim_back_sup = 0;
  selection = 4L;
  literal_comparison_mode = 0;
  inherited_negative_selection = false;
     
  // strategy 
  forward_subsumption = true;
  fs_set_mode = false;
  fs_optimised_multiset_mode = false;
  fs_use_sig_filters = false;
  fs_old_style = false;
  simplify_by_fs = 1L;
  forward_demodulation = 4L;
  fd_on_splitting_branches = true;
  normalize_by_commutativity = true;
  backward_demodulation = 4L;
  bd_on_splitting_branches = true;
  simplify_by_eq_res = true;
  backward_subsumption = true;
  bs_set_mode = false;
  bs_optimised_multiset_mode = false;
  orphan_murder = false;
  lrs = 1L;
  elim_def = 5L;
  simplification_ordering = NonrecursiveKBOrdering;
  symbol_precedence_by_arity = 1L;
  header_precedence_kinky = false;
  term_weighting_scheme = UniformTermWeighting;
  symbol_weight_by_arity = 0L;

  // numeric options 
  memory_limit = LONG_MAX; // kilobytes
  allocation_buffer_size = 10L;
  allocation_buffer_price = 2.0;
  max_skolem = 1024; 
  max_active = LONG_MAX;
  max_passive = LONG_MAX;
  max_weight = LONG_MAX;
  max_inference_depth = LONG_MAX;
  age_weight_ratio = 5L;
  selected_pos_eq_penalty_coefficient = 1.0;  
  nongoal_penalty_coefficient = 1.0;
  first_time_check = 5L;   
  first_mem_check = 200L;   
    
  // interface options    
  show_opt = false;
  show_input = false;
  show_prepro = false;
  show_gen = false;
  show_passive = false;
  show_active = false; 
  proof = true;
  show_profile = false;
  show_weight_limit_changes = false;  
  tabulation = 0;
  std_output = &cout;

#ifdef VKERNEL_FOR_STEP_RESOLUTION
  step_resolution_literal_selection = false;
#endif

}; // void VampireKernel::Options::setDefaultValues()


ostream& VampireKernel::Options::output(ostream& str)
{
  str << "% job_id = " << job_id << '\n';

  str << "% main_alg = " << main_alg << '\n';
  
  str << "% no_inferences_between_rules = " << no_inferences_between_rules << '\n';
  str << "% static_splitting = " << static_splitting << '\n';
  str << "% dynamic_splitting = " << dynamic_splitting << '\n';
  str << "% splitting_style = " << splitting_style << '\n'; 
  str << "% splitting_with_naming = " << splitting_with_naming << '\n'; 
  str << "% static_neg_eq_splitting = " << static_neg_eq_splitting << '\n';
  str << "% paramodulation = " << paramodulation << '\n'; 
  str << "% sim_back_sup = " << sim_back_sup << '\n';   
  str << "% selection = " << selection << '\n';
  str << "% literal_comparison_mode = " << literal_comparison_mode << '\n';
  str << "% inherited_negative_selection = " << inherited_negative_selection << '\n'; 
     
  str << "% forward_subsumption = " << forward_subsumption << '\n'; 
  str << "% fs_set_mode = " << fs_set_mode << '\n'; 
  str << "% fs_optimised_multiset_mode = " << fs_optimised_multiset_mode << '\n'; 
  str << "% fs_use_sig_filters = " << fs_use_sig_filters << '\n';
  str << "% fs_old_style = " << fs_old_style << '\n';  
  str << "% simplify_by_fs = " << simplify_by_fs << '\n';
  str << "% forward_demodulation = " << forward_demodulation << '\n';
  str << "% fd_on_splitting_branches = " << fd_on_splitting_branches << '\n';
  str << "% normalize_by_commutativity = " << normalize_by_commutativity << '\n';
  str << "% backward_demodulation = " << backward_demodulation << '\n';
  str << "% bd_on_splitting_branches = " << bd_on_splitting_branches << '\n';
  str << "% simplify_by_eq_res = " << simplify_by_eq_res << '\n'; 
  str << "% backward_subsumption = " << backward_subsumption << '\n'; 
  str << "% bs_set_mode = " << bs_set_mode << "\n";
  str << "% bs_optimised_multiset_mode = " << bs_optimised_multiset_mode << "\n";
  str << "% orphan_murder = " << orphan_murder << '\n'; 
  str << "% lrs = " << lrs << '\n';
  str << "% elim_def = " << elim_def << '\n';
  str << "% simplification_ordering = " << simplification_ordering << '\n';
  str << "% symbol_precedence_by_arity = " << symbol_precedence_by_arity << '\n';
  str << "% header_precedence_kinky = " << header_precedence_kinky << '\n';
  str << "% term_weighting_scheme = " << term_weighting_scheme << '\n';
  str << "% symbol_weight_by_arity = " << symbol_weight_by_arity <<  '\n';

  str << "% memory_limit = " << memory_limit << '\n';
  str << "% allocation_buffer_size = " << allocation_buffer_size << '\n';
  str << "% allocation_buffer_price = " << allocation_buffer_price << '\n';
  str << "% max_skolem = " << max_skolem << '\n';
  str << "% max_active = " << max_active << '\n'; 
  str << "% max_passive = " << max_passive << '\n';  
  str << "% max_weight = " << max_weight << '\n';    
  str << "% max_inference_depth = " << max_inference_depth << '\n';  
  str << "% age_weight_ratio = " << age_weight_ratio << '\n';
  str << "% selected_pos_eq_penalty_coefficient = " 
      << selected_pos_eq_penalty_coefficient << '\n';  
  str << "% nongoal_penalty_coefficient = "
      << nongoal_penalty_coefficient << '\n';
  str << "% first_time_check = " << first_time_check << '\n';   
  str << "% first_mem_check = " << first_mem_check << '\n';  
    
  str << "% tabulation = " << (tabulation != 0) << '\n';
  str.flush();
  return str;
}; // ostream& VampireKernel::Options::output(ostream& str)

//======================================================================
#undef DEBUG_NAMESPACE
#ifdef DEBUG_VAMPIRE_KERNEL
#define DEBUG_NAMESPACE "VampireKernel::Clause"
#endif    
#include "debugMacros.hpp"
//======================================================================

ulong VampireKernel::Clause::number() const
{
  CALL("number() const");
  return (static_cast<const VK::Clause*>(static_cast<const void*>(this)))->Number();
};

const VampireKernel::Literal* VampireKernel::Clause::literals() const
{
  CALL("literals() const");
  ASSERT(!isBuiltInTheoryFact());
  return 
    static_cast<const VampireKernel::Literal*>
    (static_cast<const void*>
     ((static_cast<const VK::Clause*>
       (static_cast<const void*>(this)))->LitList()->firstElem()));
};

bool VampireKernel::Clause::isInputClause() const
{
  CALL("isInputClause() const");  
  ASSERT(!isBuiltInTheoryFact());
  return (static_cast<const VK::Clause*>
	  (static_cast<const void*>(this)))->isInputClause();
};

bool VampireKernel::Clause::isBuiltInTheoryFact() const
{
  CALL("isBuiltInTheoryFact() const"); 
  return (static_cast<const VK::Clause*>
	  (static_cast<const void*>(this)))->isBuiltInTheoryFact();
};


void* VampireKernel::Clause::inputClauseOrigin() const
{
  CALL("inputClauseOrigin() const");
  ASSERT(!isBuiltInTheoryFact());
  ASSERT(isInputClause());
  return (static_cast<const VK::Clause*>
	  (static_cast<const void*>(this)))->inputClauseOrigin();
};

const VampireKernel::Clause::Ancestor* VampireKernel::Clause::firstAncestor() const
{
  CALL("firstAncestor() const");  
  ASSERT(!isBuiltInTheoryFact());
  if (isInputClause()) return 0;
  return 
    static_cast<const VampireKernel::Clause::Ancestor*>
    (static_cast<const void*>
     ((((static_cast<const VK::Clause*>
	 (static_cast<const void*>(this)))->background().ancestors()))));
}; 

VampireKernel::Clause::BackgroundFlagIterator 
VampireKernel::Clause::backgroundFlagIterator() const
{
  CALL("backgroundFlagIterator() const");
  BackgroundFlagIterator res;
  res._base = static_cast<const void*>(this);
  ulong fl = 0UL;
  while ((fl < 32UL) && 
	 (!(VampireKernel::_Implementation::isRelevantBackgroundFlag((VK::ClauseBackground::Rule)fl) &&
	    (static_cast<const VK::Clause*>
	     (static_cast<const void*>(this)))->background().properties().bit(fl))))
    {
      ++fl;
    };
    
  res._offset = fl;
  return res;
}; // VampireKernel::Clause::BackgroundFlagIterator VampireKernel::Clause::backgroundFlagIterator() const

bool VampireKernel::Clause::backgroundFlagIsSet(BackgroundFlag fl) const
{
  CALL("backgroundFlagIsSet(BackgroundFlag fl) const");
  ulong nativeFlagNum = 
    (ulong)VampireKernel::_Implementation::convertBackgroundFlag(fl);
  return 
    (static_cast<const VK::Clause*>
     (static_cast<const void*>(this)))->background().properties().bit((ulong)nativeFlagNum);
}; // bool VampireKernel::Clause::backgroundFlagIsSet(BackgroundFlag fl) const


//======================================================================
#undef DEBUG_NAMESPACE
#ifdef DEBUG_VAMPIRE_KERNEL
#define DEBUG_NAMESPACE "VampireKernel::Clause::Ancestor"
#endif    
#include "debugMacros.hpp"
//======================================================================

const VampireKernel::Clause* VampireKernel::Clause::Ancestor::clause() const
{
  CALL("clause() const");
  return 
    static_cast<const VampireKernel::Clause*>
    (static_cast<const void*>
     ((static_cast<const VK::ClauseBackground::ClauseList*>
       (static_cast<const void*>(this)))->hd()));
}; 

const VampireKernel::Clause::Ancestor* VampireKernel::Clause::Ancestor::next() const
{
  CALL("next() const");  
  return 
    static_cast<const VampireKernel::Clause::Ancestor*>
    (static_cast<const void*>
     ((static_cast<const VK::ClauseBackground::ClauseList*>
       (static_cast<const void*>(this)))->tl()));
};




//======================================================================
#undef DEBUG_NAMESPACE
#ifdef DEBUG_VAMPIRE_KERNEL
#define DEBUG_NAMESPACE "VampireKernel::Clause::BackgroundFlagIterator"
#endif    
#include "debugMacros.hpp"
//======================================================================


VampireKernel::Clause::BackgroundFlagIterator::BackgroundFlagIterator()
{
  _base = 0;
};

VampireKernel::Clause::BackgroundFlagIterator::~BackgroundFlagIterator()
{
};

bool VampireKernel::Clause::BackgroundFlagIterator::getCurrentFlag(BackgroundFlag& flag) const
{
  CALL("getCurrentFlag(BackgroundFlag& flag) const");
  if (_base && (_offset < 32UL)) 
    {
      flag = 
	VampireKernel::_Implementation::convertBackgroundFlag((const VK::ClauseBackground::Rule&)_offset);
      return true;
    }
  else
    return false;
};

void VampireKernel::Clause::BackgroundFlagIterator::goToNextFlag()
{
  CALL("goToNextFlag()");
  ++_offset;
  while ((_offset < 32UL) && 
	 (!(VampireKernel::_Implementation::isRelevantBackgroundFlag((VK::ClauseBackground::Rule)_offset) &&
	    (static_cast<const VK::Clause*>(_base))->background().properties().bit(_offset))))
      
    {
      ++_offset;
    };
};

//======================================================================
#undef DEBUG_NAMESPACE
#ifdef DEBUG_VAMPIRE_KERNEL
#define DEBUG_NAMESPACE "VampireKernel::Literal"
#endif    
#include "debugMacros.hpp"
//======================================================================


const VampireKernel::Literal* 
VampireKernel::Literal::next() const
{
  CALL("next() const");
  return
    static_cast<const VampireKernel::Literal*>
    (static_cast<const void*>    
     ((static_cast<const VK::LiteralList::Element*>
       (static_cast<const void*>(this)))->next()));
}; 

bool VampireKernel::Literal::isPositive() const 
{
  CALL("isPositive() const");
  return 
    (static_cast<const VK::LiteralList::Element*>(static_cast<const void*>(this)))->positive();
};


ulong VampireKernel::Literal::predicate() const
{
  CALL("predicate() const");
  ulong predId = 
    (static_cast<const VK::LiteralList::Element*>
     (static_cast<const void*>(this)))->literal()->functor();
  if (predId == VampireKernelConst::OrdEqNum) predId = VampireKernelConst::UnordEqNum;
  return predId;
};

const VampireKernel::Subterm* VampireKernel::Literal::firstArgument() const
{
  CALL("firstArgument() const");
  const TERM* args = 
    ((static_cast<const VK::LiteralList::Element*>
      (static_cast<const void*>(this)))->literal()->Args());
  if (args->IsNull()) return 0;
  return 
    static_cast<const VampireKernel::Subterm*>(static_cast<const void*>(args));
};


//======================================================================
#undef DEBUG_NAMESPACE
#ifdef DEBUG_VAMPIRE_KERNEL
#define DEBUG_NAMESPACE "VampireKernel::Subterm"
#endif    
#include "debugMacros.hpp"
//======================================================================

const VampireKernel::Subterm* VampireKernel::Subterm::next() const
{
  CALL("next() const");
  const TERM* nxt = (static_cast<const TERM*>(static_cast<const void*>(this)))->Next();
  if (nxt->IsNull()) return 0;
  return 
    static_cast<const VampireKernel::Subterm*>(static_cast<const void*>(nxt));
};

bool VampireKernel::Subterm::isVariable() const
{
  CALL("isVariable() const");
  return (static_cast<const TERM*>(static_cast<const void*>(this)))->isVariable();
};

ulong VampireKernel::Subterm::variableNumber() const
{
  CALL("variableNumber() const");
  ASSERT(isVariable());
  return (static_cast<const TERM*>(static_cast<const void*>(this)))->var();
};

ulong VampireKernel::Subterm::topFunction() const
{
  CALL("topFunction() const");
  ASSERT(!isVariable());
  return (static_cast<const TERM*>(static_cast<const void*>(this)))->First()->functor();
};

const VampireKernel::Subterm* VampireKernel::Subterm::firstArgument() const
{
  CALL("firstArgument() const");
  ASSERT(!isVariable());
  const TERM* args = (static_cast<const TERM*>(static_cast<const void*>(this)))->First()->Args();  
  if (args->IsNull()) return 0;
  return 
    static_cast<const VampireKernel::Subterm*>(static_cast<const void*>(args));
};


//======================================================================
#undef DEBUG_NAMESPACE
#ifdef DEBUG_VAMPIRE_KERNEL
#define DEBUG_NAMESPACE "VampireKernel::ClauseSetIterator"
#endif    
#include "debugMacros.hpp"
//======================================================================

VampireKernel::ClauseSetIterator::~ClauseSetIterator()
{
  CALL("destructor ~ClauseSetIterator()");
};

//======================================================================







