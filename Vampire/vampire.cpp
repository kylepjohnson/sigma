/** This software is an adaptation of the theorem prover Vampire for
 * working with large knowledge bases in the KIF format, see 
 * http://www.prover.info for publications on Vampire.
 *
 * Copyright (C) Andrei Voronkov and Alexandre Riazanov
 *
 * @author Alexandre Riazanov <riazanov@cs.man.ac.uk>
 * @author Andrei Voronkov <voronkov@cs.man.ac.uk>, <andrei@voronkov.com>
 *
 * @date 06/06/2003
 * 
 * This software is free software; you can redistribute it and/or
 * modify it under the terms of the GNU Lesser General Public
 * License as published by the Free Software Foundation; either
 * version 2.1 of the License, or (at your option) any later version.
 * 
 * This library is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
 * Lesser General Public License for more details.
 *
 * You should have received a copy of the GNU Lesser General Public
 * License along with this library; if not, write to the Free Software
 * Foundation, Inc., 59 Temple Place, Suite 330, Boston, MA  02111-1307  USA
 */

//              
// File kif.cpp
//


#include <iostream> 
#include <fstream>
#include <string>
#include <fstream>
#include <cstdlib>
#include <cstdio>
#include <signal.h>
#include <ctime>
// #include <unistd.h>
#ifdef _MSC_VER
#  include <strstream> 
#else
#  include <strstream> 
#endif

// parser-related files
// include of KIF lexer and parser MUST be BEFORE XML lexer and parser
// to force correct order of parsers generation and compilation
#include "KIFLexer.hpp"
#include "KIFParser.hpp"
#include "XMLLexer.hpp"
#include "XMLParser.hpp"
#include "TPTPLexer.hpp"
#include "TPTPParser.hpp"

#include "antlr/RecognitionException.hpp"
#include "antlr/CharStreamException.hpp"
#include "antlr/TokenStreamRecognitionException.hpp"

#include "antlr/TokenStreamSelector.hpp"
// create selector to deal with both lexers simultaneously
// it must be global to allow parsers to switch between lexers
antlr::TokenStreamSelector* selector;


// kernel files
#include "Tabulation.hpp"
#include "VampireKernel.hpp"
#include "Exit.hpp"
#include "GlobAlloc.hpp"
#include "Input.hpp"
#include "Random.hpp"
#include "GlobalClock.hpp"

// vampire files
#include "Options.hpp"
#include "Renaming.hpp"
#include "Problem.hpp"
#include "IntNameTable.hpp"
#include "Refutation.hpp"
#include "Statistics.hpp"
#include "Tabulate.hpp"


Tabulate tabulation;

void runtimeErrorHook();
void debugSystemInternalErrorHook();
void setSignalHandlers();
void cancelSignalHandling();
Query* readQuery ();
bool recover ();

// 25/12/2003 Manchester, changed passing main algorithm and splitting
void passOptions (const Options& vampire, 
		  VampireKernel::Options& kernel, 
		  const char* jobId)
{
  TRACER( "passOptions");

  kernel.job_id = jobId;
  switch (vampire.mainAlg ()) 
    {
    case Options::LRS:
      kernel.main_alg = 0;
      kernel.lrs = 3;
      break;

    case Options::OTTER:
      kernel.main_alg = 0;
      kernel.lrs = 0;
      break;

    case Options::DISCOUNT:
      kernel.main_alg = 3;
      kernel.lrs = 0;
      break;
    }

  switch (vampire.splitting ()) 
    {
    case Options::SPLIT_OFF:
      kernel.static_splitting = false;
      kernel.dynamic_splitting = false;
      break;

    case Options::SPLIT_PREPRO:
      kernel.static_splitting = true;
      kernel.dynamic_splitting = false;
      break;

    case Options::SPLIT_ON:
      kernel.static_splitting = true;
      kernel.dynamic_splitting = true;
      break;
    }

  kernel.no_inferences_between_rules= vampire.noInferencesBetweenRules ();
  kernel.splitting_style = vampire.splittingStyle ();
  kernel.splitting_with_naming = vampire.splittingWithNaming ();
  kernel.static_neg_eq_splitting = vampire.inequalitySplitting ();
  kernel.paramodulation = vampire.paramodulation ();
  // long sim_back_sup;
  kernel.selection = vampire.selection ();
  kernel.literal_comparison_mode = vampire.literalComparisonMode ();
  kernel.inherited_negative_selection = vampire.inheritedNegativeSelection ();

  switch (vampire.forwardSubsumption ()) 
    {
    case Options::SUBSUMPTION_OFF:
      kernel.forward_subsumption = false;
      break;

    case Options::SUBSUMPTION_MULTISET:
      kernel.forward_subsumption = true;
      kernel.fs_set_mode = false;
      kernel.fs_optimised_multiset_mode = true;
      break;

    case Options::SUBSUMPTION_OPTIMIZED:
      kernel.forward_subsumption = true;
      kernel.fs_set_mode = false;
      kernel.fs_optimised_multiset_mode = true;
      break;

    case Options::SUBSUMPTION_SET:
      kernel.forward_subsumption = true;
      kernel.fs_set_mode = true;
      kernel.fs_optimised_multiset_mode = false;
      break;
    }

  switch (vampire.backwardSubsumption ()) 
    {
    case Options::SUBSUMPTION_OFF:
      kernel.backward_subsumption = false;
      break;

    case Options::SUBSUMPTION_MULTISET:
      kernel.backward_subsumption = true;
      kernel.bs_set_mode = false;
      kernel.bs_optimised_multiset_mode = true;
      break;
    case Options::SUBSUMPTION_OPTIMIZED:
      kernel.backward_subsumption = true;
      kernel.bs_set_mode = false;
      kernel.bs_optimised_multiset_mode = true;
      break;

    case Options::SUBSUMPTION_SET:
      kernel.backward_subsumption = true;
      kernel.bs_set_mode = true;
      kernel.bs_optimised_multiset_mode = false;
      break;
    }
  // bool fs_use_sig_filters;
  // bool fs_old_style;
  kernel.simplify_by_fs = vampire.forwardSubsumptionResolution ();
  kernel.forward_demodulation = vampire.forwardDemodulation ();
  kernel.backward_demodulation = vampire.backwardDemodulation ();
  kernel.fd_on_splitting_branches = vampire.fdOnSplittingBranches ();
  // bool normalize_by_commutativity;
  // bool bd_on_splitting_branches;
  // bool simplify_by_eq_res;
  kernel.orphan_murder = vampire.orphanMurder ();
  kernel.elim_def = vampire.elimDef ();
  switch (vampire.simplificationOrdering ()) 
    {
    case Options::KBO:
      kernel.simplification_ordering = VampireKernel::Options::StandardKBOrdering;
      break;
    case Options::KBO_NONREC:
      kernel.simplification_ordering = VampireKernel::Options::NonrecursiveKBOrdering;
      break;
    }
  kernel.symbol_precedence_by_arity = vampire.symbolPrecedence ();
  kernel.header_precedence_kinky = vampire.headerPrecedenceKinky ();
  switch (vampire.termWeightingScheme ()) 
    {
    case Options::TWS_UNIFORM:
      kernel.term_weighting_scheme = VampireKernel::Options::UniformTermWeighting;
      break;
    case Options::TWS_NONUNIFORM_CONSTANT:
      kernel.term_weighting_scheme = VampireKernel::Options::NonuniformConstantTermWeighting;
      break;
    }
  kernel.symbol_weight_by_arity = vampire.symbolWeightByArity ();
  kernel.memory_limit = vampire.memoryLimit () * 1000;
  kernel.allocation_buffer_size = vampire.allocationBufferSize ();
  kernel.allocation_buffer_price = vampire.allocationBufferPrice ();
  kernel.max_skolem = vampire.maxSkolem ();
  kernel.max_active = vampire.maxActive ();
  kernel.max_passive = vampire.maxPassive ();
  kernel.max_weight = vampire.maxWeight ();
  kernel.max_inference_depth = vampire.maxInferenceDepth ();
  kernel.age_weight_ratio = vampire.ageWeightRatio ();
  kernel.selected_pos_eq_penalty_coefficient = vampire.selectedEqualityWeightCoefficient ();    
  kernel.nongoal_penalty_coefficient = vampire.nongoalWeightCoefficient ();
  kernel.first_time_check = vampire.lrsFirstTimeCheck ();
  kernel.first_mem_check = vampire.lrsFirstMemoryCheck ();
  kernel.show_opt = vampire.showOptions ();
  kernel.show_input = vampire.showInput ();
  kernel.show_prepro = vampire.showPrepro ();
  kernel.show_gen = vampire.showGen ();
  kernel.show_passive = vampire.showPassive ();
  kernel.show_active = vampire.showActive ();
  // the proof is produced by the preprocessor
  kernel.proof = false;
  kernel.show_profile = vampire.showProfile ();
  kernel.show_weight_limit_changes = vampire.showWeightLimitChanges ();  
  // tabulation is handled by preprocessor
  kernel.tabulation = 0;
  // all output is now also handled by the preprocessor
  kernel.std_output = 0;
} // passOptions (Options&, VampireKernel::Options&)


// 28/09/2002 Manchester, changed to include numerical terms
void passTerm (Term t, 
	       VampireKernel& kernel,
	       Renaming& renaming,
	       int& lastVar)
{
  TRACER("passTerm");

  switch (t.tag()) 
    {
    case Term::VAR: 
      {
	Var v = t.var();
	Var newV = renaming.get(v);
	if (newV == -1) { // v is not not bound
	  renaming.push (v, ++lastVar);
	  newV = lastVar;
	}
	kernel.receiveVariable(newV);
	return;
      }
    case Term::COMPOUND: 
      {
	// t is not a variable
	// pass the functor
	kernel.receiveFunction (t.functor()->kernelNumber());
	Iterator<Term> ts (t.args());
	while (ts.more()) {
	  passTerm (ts.next(), kernel, renaming, lastVar);
	}
	return;
      }
    case Term::NUMERIC:
      kernel.receiveNumericConstant(t.number());
      return;
    }
} // passTerm(Term* term, VampireKernel& kernel)


void passSignature (VampireKernel& kernel)
{
  TRACER ("passSignature");

  if (options->mode() == Options::KIF) {
    sig->addArithmetic (); // add arithmetic to the signature
  }

  kernel.beginInputSignature();
  
  // pass predicate symbols
  Signature::PredIterator preds (sig);
  while (preds.more()) {
    ulong symbolId; // to return symbolId
    Signature::Pred* pred = static_cast<Signature::Pred*>(preds.next());
    kernel.registerInputPredicate(pred->name(),
				  pred->arity(),
				  sig->isAnswer(pred), // answer predicate sign
				  symbolId);
    pred->kernelNumber (symbolId);
  }
  // pass function symbols
  Signature::FunIterator funs (sig);
  while (funs.more()) {
    ulong symbolId; // to return symbolId
    Signature::Symbol* fun = funs.next ();
    kernel.registerInputFunction(fun->name(),
				 fun->arity(),
				 symbolId);
    fun->kernelNumber (symbolId);
  }

  kernel.endInputSignature();

  if (options->mode() == Options::KIF) {
    // tell the kernel to interpret function and predicate symbols
    kernel.interpretFunction ("AdditionFn", 2, "floating_point_arithmetic", "binary_plus");
    kernel.interpretFunction ("SubtractionFn", 2, "floating_point_arithmetic", "binary_minus");
    // kernel.interpretFunction ("MultiplicationFn", 1, "floating_point_arithmetic", "unary_minus");
    kernel.interpretFunction ("MultiplicationFn", 2, "floating_point_arithmetic", "multiply");
    kernel.interpretFunction ("DivisionFn", 2, "floating_point_arithmetic", "divide");
    // kernel.interpretFunction ("min", 2, "floating_point_arithmetic", "min2");
    // kernel.interpretFunction ("max", 2, "floating_point_arithmetic", "max2");
    kernel.interpretPredicate (">", 2, "floating_point_arithmetic", "greater");
    kernel.interpretPredicate (">=", 2, "floating_point_arithmetic", "greater_or_equal");
    kernel.interpretPredicate ("<=", 2, "floating_point_arithmetic", "less_or_equal");
    kernel.interpretPredicate ("<", 2, "floating_point_arithmetic", "less");
    kernel.interpretPredicate ("equal", 2, "floating_point_arithmetic", "equal");
  }
} // passSignature


int passClauses (Problem& problem, VampireKernel& kernel)
{
  TRACER( "passClauses");

  kernel.beginInputClauses();
  int lastClauseNumber = 0;

  UnitChain::Iterator units (problem.units());

  while (units.more()) {
    // pick the next clause from the problem
    Unit u (units.next());
    ASS(u.unitType() == CLAUSE);
    int lastVar = -1;
    Renaming renaming;

    // read its number
    int number = u.number();
    if (number > lastClauseNumber) {
      lastClauseNumber = number;
    }
    
    // communicate the clause
    kernel.beginInputClause (number, 
			     u.giveAway(),
			     u.inputType() != AXIOM,
			     options->sos() ?
			       u.inputType() == AXIOM :
  			       false );

    // communicate its literals
    Iterator<Literal> lits (u.clause().literals());
    while (lits.more()) {
      Literal lit (lits.next());
      kernel.beginLiteral(lit.positive(),
			  lit.atom().functor()->kernelNumber());

      Iterator<Term> args (lit.atom().args());
      while ( args.more() ) {
	passTerm (args.next(), kernel, renaming, lastVar);
      }

      kernel.endLiteral();		  
    }

    kernel.endInputClause();
  }

  kernel.endInputClauses();

  return lastClauseNumber;
} // passClauses


char run (VampireKernel& kernel, 
	  long timeLimit,
	  VampireKernel::Options& options, 
	  Problem& problem
	  )
{
  TRACER ("run");

  try 
    {
      // reset global memory allocator, must be done before every
      // restart of the kernel
      BK::GlobAlloc::reset ();
      // tell kernel to start the new session
      kernel.openSession(options);
      // pass the signature from preprocessor to kernel
      passSignature (kernel);
      // pass the clauses from preprocessor to kernel
      // cerr << problem;
      int lastClauseNumber = passClauses (problem, kernel);
  
      TRACER ("kernel.saturate");
      kernel.saturate(lastClauseNumber+1,timeLimit);
    }
  catch (const VampireKernel::Emergency& e)
    {
      BK::DestructionMode::makeThorough();

      switch (e.type())
	{
	case VampireKernel::Emergency::AllocationFailure:
	  // nonfatal
	  return '0';
	case VampireKernel::Emergency::TimeLimit:
	  // nonfatal
	  return '0';
	};
    };
  // the kernel session terminated in a normal way

  char result = '0';
  switch (kernel.terminationReason())
    {
    case VampireKernel::UnknownTerminationReason:
      ICP("ICP0"); // debugging operator, the control must come here
      BK::RuntimeError::report("Kernel session terminated for unknown reason");
      break;
    case VampireKernel::RefutationFound:
      result = '+';
      break;
    case VampireKernel::Satisfiable:
      result = '-';
      break;
    case VampireKernel::EmptyPassive:
      break;
    case VampireKernel::MaxPassive:
      break;
    case VampireKernel::MaxActive:
      break;
    #ifdef DEBUG_PREPRO
    default:
      ASS(false);
    #endif
    };
  
  return result;
} // run (VampireKernel& kernel, ...)


char rerun (VampireKernel& kernel)
{
  try 
    {
      kernel.lookForAnotherAnswer();
    }
  catch (const VampireKernel::Emergency& e)
    {
      BK::DestructionMode::makeThorough();

      switch (e.type())
	{
	case VampireKernel::Emergency::AllocationFailure:
	  // nonfatal
	  return '0';
	case VampireKernel::Emergency::TimeLimit:
	  // nonfatal
	  return '0';
	};
    };
  // the kernel session terminated in a normal way

  char result = '0';
  switch (kernel.terminationReason())
    {
    case VampireKernel::UnknownTerminationReason:
      ICP("ICP0"); // debugging operator, teh control must come here
      BK::RuntimeError::report("Kernel session terminated for unknown reason");
    case VampireKernel::RefutationFound:
      result = '+';
      break;
    case VampireKernel::Satisfiable:
      result = '-';
      break;
    case VampireKernel::EmptyPassive:
      break;
    case VampireKernel::MaxPassive:
      break;
    case VampireKernel::MaxActive:
      break;
    #ifdef DEBUG_PREPRO
    default:
      ASS (false);
    #endif
    };
  
  return result;
} // rerun (VampireKernel& kernel)


void runtimeErrorHook()
{
  if (VampireKernel::currentSession()) {
    cout << "% Trying to shut down current kernel session...\n";
    VampireKernel::currentSession()->reportError();
  }

  tabulation.errorMessage ("runtime",
			   "some module detected inconsistency in run time");
} // void runtimeErrorHook()


void debugSystemInternalErrorHook()
{  
  if (VampireKernel::currentSession()) {
    cout << "% Trying to shut down current kernel session...\n";
    VampireKernel::currentSession()->reportError();
  }

  tabulation.errorMessage("debug",
			  "debugging system detected internal inconsistency");
} // void debugSystemInternalErrorHook()


const char* spellSignal(int sigNum)
{
  switch (sigNum)
    {
    case SIGTERM: return "SIGTERM";
# ifndef _MSC_VER
    case SIGQUIT: return "SIGQUIT";
    case SIGHUP: return "SIGHUP";
    case SIGXCPU: return "SIGXCPU";
    case SIGBUS: return "SIGBUS";
    case SIGTRAP: return "SIGTRAP";
# endif
    case SIGINT: return "SIGINT";
    case SIGILL: return "SIGILL";
    case SIGFPE: return "SIGFPE";
    case SIGSEGV: return "SIGSEGV";
    case SIGABRT: return "SIGABRT";
    default: return "UNKNOWN SIGNAL";
    };
};


void handleSignal(int sigNum)
{
  switch (sigNum)
    {
    case SIGTERM:
# ifndef _MSC_VER
    case SIGQUIT:
    case SIGHUP:
    case SIGXCPU:
      cout << "% Aborted by signal " << spellSignal(sigNum) << "\n";
      if (VampireKernel::currentSession()) {
	cout << "% Trying to shut down current kernel session...\n";
	VampireKernel::currentSession()->reportInterrupt(sigNum);
	// tabulation is now handled by preprocessor
	// VampireKernel::currentSession()->tabulate();
	if (tabulation.exists()) {
	  VampireKernel* kernel = VampireKernel::currentSession ();
	  const Statistics statistics (*kernel);
	  tabulation.tabulate (statistics);
	}
      }
      
      tabulation.result('0');
      
      cout << "% Bailing out now: aborted by interrupt signal " 
	   << spellSignal(sigNum) << "\n";
      cancelSignalHandling();
      BK::Exit::exit(1);
# endif

    case SIGINT:
      {
	if (VampireKernel::currentSession())
	  {
	    VampireKernel::currentSession()->suspend();
	  }
	else
	  {
	    cout << "% No kernel session running at the moment.\n";
	  };
	cout << "Continue? (y/n)\n";
	
	char a;
	cin >> a;
	
	if ((a == 'n') || (a == 'N'))
	  {
	    cout << "% Terminated by the user.\n";
	    VampireKernel* kernel = VampireKernel::currentSession();
	    if (kernel) {
	      VampireKernel::currentSession()->reportInterrupt(sigNum);
	      // now handled by preprocessor
	      // VampireKernel::currentSession()->tabulate();
	      if (tabulation.exists()) {
		const Statistics statistics (*kernel);
		tabulation.tabulate (statistics);
	      }
	    }
 	    tabulation.result('0');
	    BK::Exit::exit(0); 
	  }
	else
	  {
            if (VampireKernel::currentSession())
	      VampireKernel::currentSession()->resume();
	    cout << "\nContinuing...\n";
	  }
      }
      return;      

    case SIGILL:
    case SIGFPE:
    case SIGSEGV:

# ifndef _MSC_VER
    case SIGBUS:
    case SIGTRAP:
# endif
    case SIGABRT: 
      {
	cout << "% Error signal " << spellSignal(sigNum) << " caught\n";
	VampireKernel* kernel = VampireKernel::currentSession ();
	if (kernel) {
	  cout << "% Trying to shut down current kernel session...\n";
	  kernel->reportErrorSignal(sigNum);
	  // now handled by preprocessor
	  // VampireKernel::currentSession()->tabulate();
	  if (tabulation.exists()) {
	    const Statistics statistics (*kernel);
	    tabulation.tabulate (statistics);
	  }
	}
	else {
#if DEBUG_PREPRO
	  MyTracer::printStack (cout);
#endif
	}
	if (tabulation.exists()) {
	  char errorMessage[256];
	  sprintf(errorMessage,"error signal %d received",sigNum);
	  tabulation.errorMessage("Error signal", errorMessage);
	}
	cout << "% Bailing out now: aborted by error signal " << spellSignal(sigNum) << "\n";
	cancelSignalHandling();
	BK::Exit::exit(1); 
      }

    default:
      break;
    };
};

void cancelSignalHandling()
{
  signal(SIGTERM,SIG_DFL);
#ifndef _MSC_VER
  signal(SIGQUIT,SIG_DFL);
  signal(SIGHUP,SIG_DFL);
  signal(SIGXCPU,SIG_DFL);
#endif
  signal(SIGINT,SIG_DFL);
#ifndef DEBUG_ERROR_SIGNALS
  signal(SIGILL,SIG_DFL);
  signal(SIGFPE,SIG_DFL);
  signal(SIGSEGV,SIG_DFL);
#ifndef _MSC_VER
  signal(SIGBUS,SIG_DFL);
  signal(SIGTRAP,SIG_DFL);
#endif
  signal(SIGABRT,SIG_DFL);
#endif
}; // void cancelSignalHandling()

void setSignalHandlers()
{
  signal(SIGTERM,handleSignal);
#ifndef _MSC_VER
  signal(SIGQUIT,handleSignal);
  signal(SIGHUP,handleSignal);
  signal(SIGXCPU,handleSignal);
#endif
  signal(SIGINT,handleSignal);
#ifndef DEBUG_ERROR_SIGNALS
  signal(SIGILL,handleSignal);
  signal(SIGFPE,handleSignal);
  signal(SIGSEGV,handleSignal);
#ifndef _MSC_VER
  signal(SIGBUS,handleSignal);
  signal(SIGTRAP,handleSignal);
#endif
  signal(SIGABRT,handleSignal);
#endif
}; // void setSignalHandlers()


// initially assumed that result is empty
void readInput (const char* inputFileName, 
		FormulaList& formulas, // KIF syntax
		UnitList& units)       // TPTP syntax
{
  std::ifstream inFile (inputFileName); 
  if(!inFile) {
    return;
  };

  try {
    switch (options->inputSyntax()) 
      {
      case Options::INPUT_KIF: 
	{
	KIFLexer lexer(inFile); 
	KIFParser parser (lexer);
	parser.start(formulas);
	return;
	}

      case Options::INPUT_TPTP: 
	{
	TPTPLexer lexer(inFile); 
	TPTPParser parser (lexer);
	parser.units(units);
	return;
	}
      }
  }
  catch (antlr::CharStreamException& io) {
    std::cerr << "Error: ANTLR I/O Exception : " <<io.toString() << "\n";
    return;
  }
  catch(antlr::RecognitionException& e) {
    std::cerr << "Error: ANTLR Recognition Exception : "
	      << e.toString() << "\n";
    return;
  }  
  catch(antlr::TokenStreamRecognitionException& e) {
    std::cerr << "Error: ANTLR Token Recognition Exception: "
	      << "line " << e.recog.getLine() 
	      << " : " << e.toString() << '\n';
    return;
  }  
  catch(antlr::TokenStreamException& e) {
    std::cerr << "Error: ANTLR Token Stream Exception : "
	      << e.toString() << "\n";
    return;
  }  
  catch(antlr::ANTLRException& e) { 
    std::cerr << "Error: some ANTLR Exception: " << e.toString() << '\n';
    return;
  }
} // readInput


void kifMode ( Options& opts, 
	       UnitList units, 
	       BK::GlobalClock::Timer& timer )
{
  TRACER ("kifMode");

  Problem problem (units);
  // preprocess the problem
  problem.preprocess (opts);
    
  // create options for the kernel
  VampireKernel::Options kernelOptions;
  // pass the values of options from vampire to the kernel
  passOptions (opts, kernelOptions, ""); // "" is the job id (not required in KIF mode)

  bool kifSyntax = options->outputSyntax() == KIF;

  // main KIF loop
  for (;;) {
    // read next query
    Query* query = readQuery ();
    if (! query) { // something is wrong, maybe syntax error
      //      cerr << "syntax error detected\n";
      // do nothing
      if (kifSyntax) {
	cout << "<assertionResponse>\n"
	  "  Syntax error detected\n"
	  "</assertionResponse>\n";
      }
      else {
	cout << "Syntax error in assertion or query!\n";
      }
      // recover
      while (!cin.eof() && !recover()) {
      }
      continue;
    }

    // process query
    switch (query->type()) {
    case Query::ASSERTION:
      if (kifSyntax) {
	cout << "<assertionResponse>\n"
	  "  Formula has been added to the session database\n"
	  "</assertionResponse>\n";
      }
      // add assertion formula to the problem
      problem.addUnit (Unit (0, HYPOTHESIS, query->formula()));
      break;

    case Query::GOAL: {
      int answers = 0;
      if (kifSyntax) {
	cout << "<queryResponse>\n";
      }

      // add goal formula to the problem
      problem.addUnit (Unit (0, 
			     CONJECTURE, 
			     Formula (Formula::NOT, query->formula())));
      problem.preprocess (opts);

      // create new kernel
      VampireKernel kernel;

      // run the kernel until the time expires
      char result = run (kernel, 10 * query->timeLimit(), kernelOptions, problem);
      switch (result) 
	{
	case '+': {
	  answers ++;
	  Refutation ref (kernel, problem);
	  Output* output = Output::create (options->outputSyntax(), cout);
	  output->pretty (ref, answers);
	  //ref->output (cout, Options::SYNTAX_NATIVE, 2, answers);
	  //ref->output (cout, Options::SYNTAX_KIF, 2, answers);
	  delete output;
	  if (problem.originalAnswerVariables().isNonEmpty()) {
	    for (int attempts = query->bindingsLimit() - 1; attempts > 0; attempts--) {
	      result = rerun (kernel);
	      switch (result) 
		{
		case '+': 
		  {
		    answers++;
		    Refutation ref (kernel, problem);
		    output = Output::create (options->outputSyntax(), cout);
		    output->pretty (ref, answers);
		    delete output;
		    break;
		  }
	      
		case '-':
		case '0':
		  if ( ! kifSyntax ) {
		    cout << "No more proofs\n";
		  }
	      
		  // no more attempts in any of these cases:
		  attempts = 0;
		  break;
	      
		default:
		  ASS (false);
		}
	    }
	  }
	  break;
	}

	case '-':
	  if ( ! kifSyntax ) {
	    cout << "Satisfiability detected\n";
	  }
	  break;

	case '0':
	  if ( ! kifSyntax ) {
	    cout << "Refutation not found\n";
	  }
	  break;
	}

      //      kernel.closeSession(); 
      problem.removeAllConjectures ();
      if (kifSyntax) {
	if (answers == 0) { // no proof found
	  cout <<
	    "  <answer result='no'>\n"
	    "  </answer>\n";
	}
	else {
	}
	cout << "  <summary proofs='" << answers << "'/>\n";
	cout << "</queryResponse>\n";
      }
      break;
    }

    case Query::TERMINATE:
      delete query;
      return;

    default:
      ASS (false);
    }

    delete query;
  }
}  // kifMode


// running in the standard Vampire mode
// 26/09/2002 Manchester
void vampireMode ( Options& opts, 
		   UnitList& units, 
		   BK::GlobalClock::Timer& timer )
{
}  // vampireMode


int main ( int argc, char* argv [ ] )
{
  TRACER ("main");

  BK::GlobalClock::Timer timer;
  timer.reset();
  timer.start();

  // creating random seed for random number generation
  Random::trulyRandom ();

  setSignalHandlers();
  BK::RuntimeError::errorHook = runtimeErrorHook;
#ifndef NO_DEBUG
  BK::Debug::setErrorHook(debugSystemInternalErrorHook);
#endif

  // create new global signature
  Signature signature;
  sig = &signature;

  try {
    // read options from the command line
    Options opts;
    options = &opts; // must be before any operations on the options
    opts.kif();
    opts.correct (argc, argv);

    // open input file
    const char* inputFileName = opts.inputFile ();
    FormulaList formulas;
    UnitList units;
    readInput (inputFileName, formulas, units);
    if (formulas.isNonEmpty()) {
      units = UnitList (AXIOM, formulas);
    }
    else if (units.isEmpty()) { // no formulas, no units in the input
      throw MyException ("cannot open input file or syntax error in the file");
    }

    switch (opts.mode()) 
      {
      case Options::KIF:
	kifMode (opts, units, timer);
	break;

      case Options::VAMPIRE:
	vampireMode (opts, units, timer);
	break;

      case Options::CASC_18:
	throw MyException ("casc-18 mode has not been implemented yet");
      }

    delete IntNameTable::vars;
    BK::DestructionMode::makeFast();

    return EXIT_SUCCESS;
  }
  catch (MyException& me) {
    me.cry (cerr);
    me.cry (cout);
    delete IntNameTable::vars;

    BK::DestructionMode::makeFast();

    return EXIT_FAILURE;
  }
  catch (MemoryException& me) {
    me.cry (cerr);
    delete IntNameTable::vars;

    BK::DestructionMode::makeFast();

    exit (EXIT_FAILURE);
  }
} // main


Query* readQuery ()
{
  TRACER ("readQuery");

  // selector for dealing with both lexers simultaneously
  antlr::TokenStreamSelector select;
  selector = &select;

  try {
    // create outter lexer and associate it with input stream
    XMLLexer xmlLexer(std::cin); 
    // create inner lexer and associate it with the same stream as outer one's
    KIFLexer kifLexer(xmlLexer.getInputState()); 
    
    // set selector to deal with this two parsers
    selector->addInputStream( &xmlLexer, "XMLLexer");
    selector->addInputStream( &kifLexer, "KIFLexer");
    selector->select("XMLLexer");

    // create parser and run it
    XMLParser parser(select);

    return parser.start ();
  } 
  catch (antlr::CharStreamException&) {
    return 0;
  }
  catch(antlr::RecognitionException&) {
    return 0;
  }  
  catch(antlr::TokenStreamRecognitionException&) {
    return 0;
  }  
  catch(antlr::TokenStreamException&) {
    return 0;
  }  
  catch(antlr::ANTLRException&) { 
    return 0;
    }
} // readQuery


bool recover ()
{
  TRACER ("recover");

  try {
    // create outter lexer and associate it with input stream
    XMLLexer xmlLexer(std::cin); 
    
    // create parser and run it
    XMLParser parser(xmlLexer);

    parser.recover ();

    return true;
  } 
  catch (antlr::CharStreamException&) {
    return false;
  }
  catch(antlr::RecognitionException&) {
    return false;
  }  
  catch(antlr::TokenStreamRecognitionException&) {
    return false;
  }  
  catch(antlr::TokenStreamException&) {
    return false;
  }  
  catch(antlr::ANTLRException&) { 
    return false;
  }
} // recover


// --- end of file ---
