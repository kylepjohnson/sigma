//
// File:         Standalone.cpp
// Description:  Wraps a Vampire kernel to make a standalone program.  
// Created:      Jun 01, 2002, Alexandre Riazanov, riazanov@cs.man.ac.uk
//======================================================================
#include <ctime>
#include <climits>
#include <iostream>
#include <fstream>
#include <strstream> 
#include <csignal>
#include "jargon.hpp"
#if (BK_TARGET_OPERATIONAL_ENVIRONMENT_TYPE == BK_TARGET_OPERATIONAL_ENVIRONMENT_TYPE_UNIX)
#  include <unistd.h>
#elif (BK_TARGET_OPERATIONAL_ENVIRONMENT_TYPE == BK_TARGET_OPERATIONAL_ENVIRONMENT_TYPE_MSWIN32)
#  include <winsock2.h>
#  include <process.h>
#endif
#include "RuntimeError.hpp"
#include "GlobalClock.hpp"

#include "GlobalStopFlag.hpp"

#include "Exit.hpp"
#include "GlobAlloc.hpp"
#include "VampireKernelDebugFlags.hpp"
#include "VampireKernel.hpp"
#include "CommandLine.hpp"
#include "Input.hpp"
#include "Parser.hpp"
#include "Tabulation.hpp"
#if (defined INTERCEPT_FORWARD_MATCHING_INDEXING) || \
    (defined INTERCEPT_BACKWARD_SUBSUMPTION_INDEXING) || \
    (defined INTERCEPT_BACKWARD_MATCHING_INDEXING) || \
    (defined INTERCEPT_INDEXED_SEARCH_FOR_GENERALISATIONS) || \
    (defined INTERCEPT_BACKWARD_SUPERPOSITION_INDEXING)
#  include "Logging.hpp"
#endif

#include "VampireKernelInterceptMacros.hpp"
using namespace BK;
using namespace VK;
//======================================================================
#undef DEBUG_NAMESPACE
#ifdef DEBUG_STANDALONE
#define DEBUG_NAMESPACE "Standalone"
#endif    
#include "debugMacros.hpp"
//======================================================================


Tabulation* tabulation;
void runtimeErrorHook();
void debugSystemInternalErrorHook();
void setSignalHandlers();
void cancelSignalHandling();
const char* extractBasename(const char* fileName);
const char* generateJobId();
void communicateTerm(const Input::Term* term,VampireKernel& kernel);
void describeOptions(CommandLine& CL);

ostream* stdOutput = &cout;

#ifndef NO_DEBUG
void outputInferenceStructure(const VampireKernel::Clause* cl);
#endif

/********

void dehook(ulong ev)
{
  if (ev >= 20000000)
    {
      GlobalStopFlag::raise();
    };
  
};  

*******/

int main(int numarg,char** args)
{
  CALL("main(int numarg,char** args)");
  
  
  //  DF; SETDEBUGEVENTHOOK(dehook);
  

  setSignalHandlers();
  RuntimeError::errorHook = runtimeErrorHook;
#ifndef NO_DEBUG
  Debug::setErrorHook(debugSystemInternalErrorHook);
#endif

  time_t* jobTime = new time_t;
  *jobTime = time(static_cast<time_t*>(0));
  char hostname[256];
  gethostname(hostname,256);
  GlobalClock::Timer timer;
  timer.reset();
  timer.start();
  
  CommandLine CL; 
  describeOptions(CL);
  CL.arguments(numarg,args);
  if (!CL.parse())
    {
      if (CL.values("v"))
	{
	  cout << "% Vampire kernel v" 
	       << VampireKernel::versionNumber() 
	       << " " << VampireKernel::versionDescription() << '\n';
	};
      if (CL.values("h") || CL.values("help"))
	{ 
	  CL.printHelp("vkernel");  
	};
      if (CL.values("v") || CL.values("h") || CL.values("help")) 
	{
	  Exit::exit(1);
	};
      CL.printError();
      CL.printUsage("vkernel");
      Exit::exit(1);
    };
    
  if (CL.values("h") || CL.values("help")) CL.printHelp("vampire"); 

  const char* testId = "unspecified_test";
  (CL.values("T").next(testId) || CL.values("test_id").next(testId));

  const char* TPTPDirEnvironmentVar = "TPTP_DIR";
  const char* TPTPDir = "."; // default value
  const char* tmpTPTPdir = getenv(TPTPDirEnvironmentVar);
  (CL.values("I").next(tmpTPTPdir) || CL.values("include").next(tmpTPTPdir));
  if (tmpTPTPdir) TPTPDir = tmpTPTPdir;

  const char* problemFileName = CL.nonoptVal("<problem>");
  const char* problemFileBaseName = extractBasename(problemFileName);
  
#ifndef NO_DEBUG
  Debug::assignJob(problemFileBaseName);
#endif 

  bool silent = false;
  CL.values("silent").next(silent);
  stdOutput = &cout; 
  if (silent) 
    {
      stdOutput = 0;
    };
  

  // print some information
  
  if (stdOutput)
    {
      *stdOutput << "%=================================================================\n";
      *stdOutput << "% Vampire kernel v" << VampireKernel::versionNumber()  
	   << " " << VampireKernel::versionDescription() << '\n';  
#ifndef NO_DEBUG 
      *stdOutput << "% WARNING: COMPILED WITHOUT -DNO_DEBUG!\n";
      *stdOutput << "%          THIS MAY AFFECT THE PERFOMANCE BADLY!\n";         
#endif
#ifdef INTERCEPT_FORWARD_MATCHING_INDEXING
      *stdOutput << "% WARNING: FORWARD MATCHING INDEX OPERATIONS ARE LOGGED!\n"; 
      *stdOutput << "%          THIS MAY AFFECT THE PERFOMANCE BADLY!\n";         
#endif
#ifdef INTERCEPT_BACKWARD_SUBSUMPTION_INDEXING
      *stdOutput << "% WARNING: BACKWARD SUBSUMPTION INDEX OPERATIONS ARE LOGGED!\n"; 
      *stdOutput << "%          THIS MAY AFFECT THE PERFOMANCE BADLY!\n";         
#endif
#ifdef INTERCEPT_BACKWARD_MATCHING_INDEXING
      *stdOutput << "% WARNING: BACKWARD MATCHING INDEX OPERATIONS ARE LOGGED!\n"; 
      *stdOutput << "%          THIS MAY AFFECT THE PERFOMANCE BADLY!\n"; 
#endif

#ifdef INTERCEPT_INDEXED_SEARCH_FOR_GENERALISATIONS 
      *stdOutput << "% WARNING: FORWARD MATCHING INDEX OPERATIONS ARE LOGGED!\n"; 
      *stdOutput << "%          THIS MAY AFFECT THE PERFOMANCE BADLY!\n"; 
#endif

#ifdef INTERCEPT_BACKWARD_SUPERPOSITION_INDEXING
      *stdOutput << "% WARNING: UNIFICATION INDEXING OPERATIONS\n";
      *stdOutput << "%          FOR BACKWARD SUPERPOSITION ARE LOGGED!\n"; 
      *stdOutput << "%          THIS MAY AFFECT THE PERFOMANCE BADLY!\n"; 
#endif


#ifdef MEASURE_BACKWARD_CONSTR_CHECK
      *stdOutput << "% WARNING: BACKWARD CONSTRAINT CHECKS MEASURED!\n"; 
      *stdOutput << "%          THIS MAY AFFECT THE PERFOMANCE BADLY!\n";  
#endif


#ifdef MEASURE_FORWARD_CONSTR_CHECK
      *stdOutput << "% WARNING: FORWARD CONSTRAINT CHECKS MEASURED!\n"; 
      *stdOutput << "%          THIS MAY AFFECT THE PERFOMANCE BADLY!\n";  
#endif

#ifdef VKERNEL_FOR_STEP_RESOLUTION
      *stdOutput << "% WARNING: COMPILED WITH -DVKERNEL_FOR_STEP_RESOLUTION!\n"; 
      *stdOutput << "%          THIS MAY SLIGHTLY AFFECT THE PERFOMANCE!\n";  
#endif

      *stdOutput << "% Started: " << asctime(localtime(jobTime));
      *stdOutput << "% Host:    " << hostname << '\n'; 
      *stdOutput << "% Problem: " << problemFileName << '\n';
      *stdOutput << "% Command: "; CL.printCommand(*stdOutput) << '\n';
      *stdOutput << "%=================================================================\n";
    };

  VampireKernel::Options options;
  options.std_output = stdOutput;
  const char* jobId = generateJobId();
  

  // Tabulate job info
  
  options.tabulation = 0;
  const char* tabulationFileName;
  if (CL.values("tab").next(tabulationFileName))
    {
      options.tabulation = new ofstream();
      options.tabulation->open(tabulationFileName,ios::app);
      if (!(*options.tabulation))
	RuntimeError::report("Can not open ",tabulationFileName," for tabulation.\n");
    };
  
  if (options.tabulation)
    {
      tabulation = new Tabulation();
      tabulation->assignStream(options.tabulation);
      tabulation->assignJob(jobId);
      tabulation->header(testId,problemFileBaseName);   
      tabulation->problemName(problemFileBaseName);
      tabulation->testId(testId);
      tabulation->hostName(hostname);
      
      
      time_t jobTime = time(static_cast<time_t*>(0));
      tm* gmt = gmtime(&jobTime); 
      int jobDateDD = gmt->tm_mday;
      int jobDateMM = gmt->tm_mon;    
      int jobDateYY = 1900 + gmt->tm_year;
      int jobTimeHH = gmt->tm_hour;
      int jobTimeMM = gmt->tm_min;
      int jobTimeSS = gmt->tm_sec;

      tabulation->jobDate(jobDateDD,jobDateMM,jobDateYY);
      tabulation->jobTime(jobTimeHH,jobTimeMM,jobTimeSS);
      tabulation->version(VampireKernel::versionNumber(),
			 VampireKernel::versionDescription());
      tabulation->versionDate();
      tabulation->versionTime();
    };  


  // Parsing TPTP files
  
  const char* currentFileName;
  Parser parser;
  CommandLine::Values includedFiles = CL.nonoptMiddleValues();
  while (includedFiles.next(currentFileName))
    {
      if (!parser.parseFile(TPTPDir,currentFileName))
	{
	  if (tabulation)
	    {
	      tabulation->result('0');
	      tabulation->errorMessage("parsing",currentFileName);
	    };
	  Exit::exit(1);
	};
    };
  
  if (!parser.parseFile(TPTPDir,problemFileName))
    {
      if (tabulation)
	{
	  tabulation->result('0');
	  tabulation->errorMessage("parsing",problemFileName);
	}; 
      Exit::exit(1);
    };

  // Setting kernel options
  
  
  options.job_id = jobId; 

  options.main_alg = 0; // default
  CL.values("main_alg").next(options.main_alg); 
  if ((options.main_alg != 0L) && (options.main_alg != 3L))
    {
      options.main_alg = 3L;
    };

   
  // inference rules

  options.no_inferences_between_rules = false; // default
  CL.values("no_inferences_between_rules").next(options.no_inferences_between_rules);

  options.static_splitting = true; // default
  CL.values("static_splitting").next(options.static_splitting);

  options.dynamic_splitting = false; // default
  CL.values("dynamic_splitting").next(options.dynamic_splitting);
 
  options.splitting_style = 1L; // default
  CL.values("splitting_style").next(options.splitting_style);

  options.splitting_with_naming = true; // default
  CL.values("splitting_with_naming").next(options.splitting_with_naming);

  options.static_neg_eq_splitting = 3L; // default
  CL.values("static_neg_eq_splitting").next(options.static_neg_eq_splitting); 

  options.paramodulation = true; // default
  CL.values("paramodulation").next(options.paramodulation);

  options.sim_back_sup = 0L; // default
  CL.values("sim_back_sup").next(options.sim_back_sup); 


  options.selection = 4L; // default
  CL.values("selection").next(options.selection);
 
  options.literal_comparison_mode = 0L;
  CL.values("literal_comparison_mode").next(options.literal_comparison_mode);
  
  options.inherited_negative_selection = false; // default
  CL.values("inherited_negative_selection").next(options.inherited_negative_selection);
   
  // strategy 
  options.forward_subsumption = true; // default
  CL.values("forward_subsumption").next(options.forward_subsumption);

  options.fs_set_mode = false; // default
  CL.values("fs_set_mode").next(options.fs_set_mode); 

  options.fs_optimised_multiset_mode = false; // default
  CL.values("fs_optimised_multiset_mode").next(options.fs_optimised_multiset_mode);  

  options.fs_use_sig_filters = false; // default
  CL.values("fs_use_sig_filters").next(options.fs_use_sig_filters);

  options.fs_old_style = false; // default
  CL.values("fs_old_style").next(options.fs_old_style);

  options.simplify_by_fs = 1L; // default
  CL.values("simplify_by_fs").next(options.simplify_by_fs);


  options.forward_demodulation = 4L; // default
  CL.values("forward_demodulation").next(options.forward_demodulation);

  options.fd_on_splitting_branches = true; // default
  CL.values("fd_on_splitting_branches").next(options.fd_on_splitting_branches);

  options.normalize_by_commutativity = true; // default
  CL.values("normalize_by_commutativity").next(options.normalize_by_commutativity);

  options.backward_demodulation = 4L; // default
  CL.values("backward_demodulation").next(options.backward_demodulation);

  options.bd_on_splitting_branches = true; // default
  CL.values("bd_on_splitting_branches").next(options.bd_on_splitting_branches);
  

  options.simplify_by_eq_res = true; // default
  CL.values("simplify_by_eq_res").next(options.simplify_by_eq_res);

  options.backward_subsumption = true; // default
  CL.values("backward_subsumption").next(options.backward_subsumption);

  options.bs_set_mode = false; // default
  CL.values("bs_set_mode").next(options.bs_set_mode);  

  options.bs_optimised_multiset_mode = false; // default
  CL.values("bs_optimised_multiset_mode").next(options.bs_optimised_multiset_mode);  


  options.orphan_murder = false; // default
  CL.values("orphan_murder").next(options.orphan_murder);

  options.lrs = 1L; // default
  CL.values("lrs").next(options.lrs);  



  options.elim_def = 5L; // default
  CL.values("elim_def").next(options.elim_def);
 
  options.simplification_ordering = 
    VampireKernel::Options::NonrecursiveKBOrdering; // default
  const char* simplificationOrdering = "nonrecursive_KB";
  CL.values("simplification_ordering").next(simplificationOrdering);
  if (!strcmp(simplificationOrdering,"nonrecursive_KB"))
    {
      options.simplification_ordering = VampireKernel::Options::NonrecursiveKBOrdering;
    }
  else
    if (!strcmp(simplificationOrdering,"standard_KB"))
      {
	options.simplification_ordering = VampireKernel::Options::StandardKBOrdering;
      }
    else
      RuntimeError::report("The simplification ordering ",
			   simplificationOrdering,
			   " has not been implemented yet.");
  

  options.symbol_precedence_by_arity = 1L; // default  
  CL.values("symbol_precedence_by_arity").next(options.symbol_precedence_by_arity);

  options.header_precedence_kinky = false; // default  
  CL.values("header_precedence_kinky").next(options.header_precedence_kinky);

  
  options.term_weighting_scheme = 
    VampireKernel::Options::UniformTermWeighting; // default  

  const char* termWeightingScheme = "uniform"; // default  
  CL.values("term_weighting_scheme").next(termWeightingScheme); 
  if (!strcmp(termWeightingScheme,"nonuniform_constant"))
    {
      options.term_weighting_scheme = 
	VampireKernel::Options::NonuniformConstantTermWeighting;
    }
  else
    if (!strcmp(termWeightingScheme,"uniform"))
      {
	options.term_weighting_scheme = 
	  VampireKernel::Options::UniformTermWeighting;
      };

  options.symbol_weight_by_arity = 0L; // default
  CL.values("symbol_weight_by_arity").next(options.symbol_weight_by_arity);
  
  bool axiomsForSupportOnly = false;  // default
  CL.values("axioms_for_support_only").next(axiomsForSupportOnly);


  // numeric options 
  options.memory_limit = (LONG_MAX/2);
  (CL.values("m").next(options.memory_limit) || 
   CL.values("memory_limit").next(options.memory_limit));

  options.allocation_buffer_size = 10L; // default
  CL.values("allocation_buffer_size").next(options.allocation_buffer_size);
  options.allocation_buffer_price = 2.0; // default
  CL.values("allocation_buffer_price").next(options.allocation_buffer_price);

  options.max_skolem = 1024;
  CL.values("max_skolem").next(options.max_skolem);


  CL.values("max_active").next(options.max_active);
  CL.values("max_passive").next(options.max_passive);

  (CL.values("w").next(options.max_weight) || CL.values("max_weight").next(options.max_weight));
 
  options.max_inference_depth = LONG_MAX; // default
  CL.values("max_inference_depth").next(options.max_inference_depth);

  options.age_weight_ratio = 5; // default
  CL.values("age_weight_ratio").next(options.age_weight_ratio); 
 
  double selPosEqPenaltyCoeff = 1.0; // default 
  CL.values("selected_pos_eq_penalty_coefficient").next(selPosEqPenaltyCoeff);
  options.selected_pos_eq_penalty_coefficient = (float)selPosEqPenaltyCoeff;

  double nongoalPenaltyCoeff = 1.0; // default
  CL.values("nongoal_penalty_coefficient").next(nongoalPenaltyCoeff);
  options.nongoal_penalty_coefficient = (float)nongoalPenaltyCoeff;


  options.first_time_check = 5L; // default
  CL.values("first_time_check").next(options.first_time_check);  
  options.first_mem_check = 200; // default
  CL.values("first_mem_check").next(options.first_mem_check);   
    
  long maxNumberOfAnswers = 1L; // default
  CL.values("max_number_of_answers").next(maxNumberOfAnswers);

  // interface options    
  options.show_opt = false;
  CL.values("show_opt").next(options.show_opt);
  options.show_input = false;
  CL.values("show_input").next(options.show_input);
  options.show_prepro = false;
  CL.values("show_prepro").next(options.show_prepro);
  options.show_gen = false;
  CL.values("show_gen").next(options.show_gen);
  options.show_passive = false;
  CL.values("show_passive").next(options.show_passive);
  options.show_active = false;
  CL.values("show_active").next(options.show_active);
  options.proof = true; // default
  CL.values("proof").next(options.proof);
  options.show_profile = false;
  CL.values("show_profile").next(options.show_profile); 
  options.show_weight_limit_changes = false;
  CL.values("show_weight_limit_changes").next(options.show_weight_limit_changes); 

#ifdef INTERCEPT_BACKWARD_MATCHING_INDEXING
  const char* backwardMatchingIndexLogFile = "BackwardMatchingIndex.log";
  CL.values("bmilog").next(backwardMatchingIndexLogFile);
  BMILog.open(backwardMatchingIndexLogFile);
  maxSizeOfBMILogFile = LONG_MAX;
  if (CL.values("bmi_log_file_max_size").next(maxSizeOfBMILogFile))
    {
      maxSizeOfBMILogFile = maxSizeOfBMILogFile * 1024;
    };

  BMILog 
    << "/*\n" 
    << " This is a log file for backward matching indexing operations.\n" 
    << " Collected: " << asctime(localtime(jobTime))
    << " Host: " << hostname << "\n"        // << "." << domainname << "\n" 
    << " By Vampire kernel v" << VampireKernel::versionNumber() << "\n"
    << " Command: "; 
  for (int i = 0; i < numarg; i++) BMILog << args[i] << " "; 
  BMILog 
    << "\n" 
    << " Problem: " << problemFileName << "\n" 
    << "*/\n"; 
  BMILog.block();
#endif



#ifdef INTERCEPT_INDEXED_SEARCH_FOR_GENERALISATIONS
  const char* generalisationsIndexLogFile = "GeneralisationsIndex.log";
  CL.values("sfgilog").next(generalisationsIndexLogFile);
  SFGILog.open(generalisationsIndexLogFile);
  maxSizeOfSFGILogFile = LONG_MAX;
  if (CL.values("sfgi_log_file_max_size").next(maxSizeOfSFGILogFile))
    {
      maxSizeOfSFGILogFile = maxSizeOfSFGILogFile * 1024;
    };

  SFGILog 
    << "/*\n" 
    << " This is a log file for forward matching indexing operations.\n" 
    << " Collected: " << asctime(localtime(jobTime))
    << " Host: " << hostname << "\n"        // << "." << domainname << "\n" 
    << " By Vampire kernel v" << VampireKernel::versionNumber() << "\n"
    << " Command: "; 
  for (int i = 0; i < numarg; i++) SFGILog << args[i] << " "; 
  SFGILog 
    << "\n" 
    << " Problem: " << problemFileName << "\n" 
    << "*/\n"; 
  SFGILog.block();
#endif

  
#ifdef INTERCEPT_BACKWARD_SUPERPOSITION_INDEXING
  const char* backwardSuperpositionIndexLogFile = "BackwardSuperpositionIndex.log";
  CL.values("bsiulog").next(backwardSuperpositionIndexLogFile);
  BSIULog.open(backwardSuperpositionIndexLogFile);
  maxSizeOfBSIULogFile = LONG_MAX;
  if (CL.values("bsiu_log_file_max_size").next(maxSizeOfBSIULogFile))
    {
      maxSizeOfBSIULogFile = maxSizeOfBSIULogFile * 1024;
    };

  BSIULog 
    << "/*\n" 
    << " This is a log file for unification indexing operations\n" 
    << " for backward superposition.\n"
    << " Collected: " << asctime(localtime(jobTime))
    << " Host: " << hostname << "\n"        // << "." << domainname << "\n" 
    << " By Vampire kernel v" << VampireKernel::versionNumber() << "\n"
    << " Command: "; 
  for (int i = 0; i < numarg; i++) BSIULog << args[i] << " "; 
  BSIULog 
    << "\n" 
    << " Problem: " << problemFileName << "\n" 
    << "*/\n"; 
  BSIULog.block();
#endif
  
#ifdef VKERNEL_FOR_STEP_RESOLUTION
  options.step_resolution_literal_selection = false; // default
  CL.values("step_resolution_literal_selection").next(options.step_resolution_literal_selection); 
#endif



  /*****
#ifdef DEBUG_NAMESPACE
  // "Warming up" GlobAlloc
  for (int i = 0; i < 3; ++i)
    {
      GlobAlloc::reset();
      for (int j = 0; j < 10; ++j)
	{
	  for (ulong s = 1UL; s < 2000UL; ++s)
	    {
	      void* dummy = GlobAlloc::allocate(s);
	      dummy = 0;
	    };
	};
    };
  GlobAlloc::reset();
#endif
  **********/

  // Kernel session

  ASSERT(GlobAlloc::occupiedByObjects() == 0L);

  long numberOfFoundAnswers = 0L;

  VampireKernel kernel;

  try 
    {
      kernel.openSession(options);
  
      kernel.beginInputSignature();
  
      for (Input::Symbol* symbol = parser.signature().symbolList();
	   symbol;
	   symbol = symbol->next())
	{
	  ulong symbolId;
      const Input::InterpretationList* interpr;
	  switch (symbol->type())
	    {
	    case Input::Symbol::Function:
	      kernel.registerInputFunction(symbol->printName(),
					   symbol->arity(),
					   symbolId);
	      symbol->assignNumber(symbolId);
	      if (symbol->weightAssigned())
		kernel.setSymbolWeightConstPart(symbolId,symbol->weightConstPart());
	      if (symbol->precedenceAssigned())
		{
		  kernel.setSymbolPrecedence(symbolId,symbol->precedence());
		};
	      for (interpr =  symbol->interpretations();
		   interpr;
		   interpr = interpr->tl())
		{
		  kernel.interpretFunction(symbol->printName(),
					   symbol->arity(),
					   interpr->hd().theoryName(),
					   interpr->hd().nativeSymbolName());
		};
		   
	      break;
	  
	    case Input::Symbol::Predicate:
	      kernel.registerInputPredicate(symbol->printName(),
					    symbol->arity(),
					    symbol->isAnswer(),
					    symbolId);
	      symbol->assignNumber(symbolId);
	      if (symbol->weightAssigned())
		kernel.setSymbolWeightConstPart(symbolId,symbol->weightConstPart());
	      
	      if (symbol->precedenceAssigned())
		{
		  kernel.setSymbolPrecedence(symbolId,symbol->precedence());
		};
	    
	      for (interpr =  symbol->interpretations();
		   interpr;
		   interpr = interpr->tl())
		{
		  kernel.interpretPredicate(symbol->printName(),
					    symbol->arity(),
					    interpr->hd().theoryName(),
					    interpr->hd().nativeSymbolName());
		};
	      break;

	    default:
	      break;
	    };
      
	};

      kernel.endInputSignature();
  

      kernel.beginInputClauses();
      ulong clauseNumber = 1UL;
      for (const Input::ClauseList* cl = parser.clauses();
	   cl;
	   cl = cl->tl())
	{
	  bool isGoal = 
	    (cl->hd()->getCategory() == Input::Clause::Conjecture) ||
	    (cl->hd()->getCategory() == Input::Clause::Hypothesis);

	  bool isSupport = axiomsForSupportOnly && 
	    (cl->hd()->getCategory() == Input::Clause::Axiom);

	  kernel.beginInputClause(clauseNumber,static_cast<void*>(cl->hd()),isGoal,isSupport);
	  ++clauseNumber;
	  for (const Input::LiteralList* literals = cl->hd()->literals();
	       literals;
	       literals = literals->tl())
	    {
	      const Input::Literal* lit = literals->hd(); 
	      kernel.beginLiteral(lit->isPositive(),
				  lit->atom()->top()->number());
	      for (const Input::TermList* args = lit->atom()->arguments();
		   args;
		   args = args->tl())
		{
		  communicateTerm(args->hd(),kernel);
		};
      
	      kernel.endLiteral();		  
	    };
	  kernel.endInputClause();
	};  
      kernel.endInputClauses();

  
      long totalTimeLimit = LONG_MAX/10;
      (CL.values("t").next(totalTimeLimit) || 
       CL.values("time_limit").next(totalTimeLimit));      
      long sessionTimeLimit = totalTimeLimit*10 - timer.elapsedDeciseconds();
      kernel.saturate(clauseNumber,sessionTimeLimit);
    }
  catch (const VampireKernel::Emergency& e)
    {
      switch (e.type())
	{
	case VampireKernel::Emergency::AllocationFailure:
	  // nonfatal
	  if (tabulation)
	    {
	      kernel.tabulate();
	      tabulation->result('0');
	      tabulation->terminationReason("allocation failure");
	    };
	  TABULATE_BACKWARD_CONSTR_CHECK_STATISTICS(tabulation);
	  TABULATE_FORWARD_CONSTR_CHECK_STATISTICS(tabulation);
	  DestructionMode::makeFast();
	  return 0;
        case VampireKernel::Emergency::TimeLimit:
	  // nonfatal
	  if (tabulation)
	    {
	      COP("TimeLimit");

	      /**************
 DF; cout << "\n\n           STOP TRACING      \n\n";
 DF; TRACINGOFF;
	      ***************/

	      kernel.tabulate();
	      tabulation->result('0');
	      tabulation->terminationReason("time limit");
	    };
	  TABULATE_BACKWARD_CONSTR_CHECK_STATISTICS(tabulation);
	  TABULATE_FORWARD_CONSTR_CHECK_STATISTICS(tabulation);
	  DestructionMode::makeFast();
	  return 0;
	};
     
    }; // catch (const VampireKernel::Emergency& e)



 normal_kernel_termination:
  // the kernel session terminated in a normal way

  kernel.tabulate();

  switch (kernel.terminationReason())
    {
    case VampireKernel::UnknownTerminationReason:
      ICP("ICP0");
      RuntimeError::report("Kernel session terminated for unknown reason");
      if (tabulation)
	{
	  tabulation->result('0');
	  tabulation->terminationReason("unknown termination reason");
	};
      break;
    case VampireKernel::RefutationFound:

      if (tabulation)
	{
	  tabulation->result('+');
	  tabulation->terminationReason("refutation found");

	};
      ++numberOfFoundAnswers;
      if (stdOutput)
	{
	  *stdOutput << "% " << numberOfFoundAnswers << " answer";
	  if (numberOfFoundAnswers > 1L) *stdOutput << "s";
	  *stdOutput << " found so far.\n";
	};

      //DF; outputInferenceStructure(kernel.refutation());

      break;
    case VampireKernel::Satisfiable:

      if (tabulation)
	{
	  tabulation->result('-');
	  tabulation->terminationReason("satisfiable");
	};
      break;
    case VampireKernel::EmptyPassive:
      if (tabulation)
	{
	  tabulation->result('0');
	  tabulation->terminationReason("empty passive");
	};
      break;
    case VampireKernel::MaxPassive:
      if (tabulation)
	{
	  tabulation->result('0');
	  tabulation->terminationReason("max. passive");
	};
      break;
    case VampireKernel::MaxActive:
      if (tabulation)
	{
	  tabulation->result('0');
	  tabulation->terminationReason("max. active");
	};
      break;
    case VampireKernel::TimeLimitExpired:
      if (tabulation)
	{
	  tabulation->result('0');
	  tabulation->terminationReason("time limit");
	};
      break;
      
    case VampireKernel::AbortedOnAllocationFailure:
      if (tabulation)
	{
	  tabulation->result('0');
	  tabulation->terminationReason("allocation failure");
	};
      break;
      
    case VampireKernel::AbortedOnExternalInterrupt:
      if (tabulation)
	{
	  tabulation->result('0');
	  tabulation->terminationReason("external interrupt");
	};
      break;    
    case VampireKernel::AbortedOnErrorSignal:
      if (tabulation)
	{
	  tabulation->result('0');
	  tabulation->terminationReason("error signal");
	};
      break;
      
    case VampireKernel::AbortedOnError:
      if (tabulation)
	{
	  tabulation->result('0');
	  tabulation->terminationReason("error");
	};
      break;
    };

  if ((kernel.terminationReason() == VampireKernel::RefutationFound) && 
      (numberOfFoundAnswers < maxNumberOfAnswers))
    {
      try
	{
	  kernel.lookForAnotherAnswer();
	}
      catch (const VampireKernel::Emergency& e)
	{
	  switch (e.type())
	    {
	    case VampireKernel::Emergency::AllocationFailure:
	      // nonfatal
	      if (tabulation)
		{
		  kernel.tabulate();
		  tabulation->result('0');
		  tabulation->terminationReason("allocation failure");
		};
	      TABULATE_BACKWARD_CONSTR_CHECK_STATISTICS(tabulation);
	      TABULATE_FORWARD_CONSTR_CHECK_STATISTICS(tabulation);
	      DestructionMode::makeFast();
	      return 0;
	    case VampireKernel::Emergency::TimeLimit:
	      // nonfatal
	      if (tabulation)
		{
		  kernel.tabulate();
		  tabulation->result('0');
		  tabulation->terminationReason("time limit");
		};
	      TABULATE_BACKWARD_CONSTR_CHECK_STATISTICS(tabulation);
	      TABULATE_FORWARD_CONSTR_CHECK_STATISTICS(tabulation);
	      DestructionMode::makeFast();
	      return 0;
	    };
	  
	}; // catch (const VampireKernel::Emergency& e)
      goto normal_kernel_termination;
    };

  TABULATE_BACKWARD_CONSTR_CHECK_STATISTICS(tabulation);
  TABULATE_FORWARD_CONSTR_CHECK_STATISTICS(tabulation);

#ifdef DEBUG_MEMORY_LEAKS

  ASSERT(DestructionMode::isThorough());

  kernel.destroy(); // will automatically close the session

  if (GlobAlloc::occupiedByObjects())
    { 
      DWARN("") << "Memory leak: " 
		<< GlobAlloc::occupiedByObjects() << " bytes lost.\n"; 
      //ClassDesc::outputAll(cout,true) << "\n";
      //GlobAlloc::outputInternalStatistics(cout) << "\n";
#  ifdef DEBUG_ALLOC_OBJ_STATUS
      GlobAlloc::outputPersistent(cout,10000UL);
#  endif
      
    };
  GlobAlloc::reset(); 
  kernel.init(); // to enable destruction
#else
#  ifdef NO_DEBUG
  DestructionMode::makeFast();
#  endif
  kernel.closeSession(); // not really necessary here
#endif
  return 0;
}; // int main(int numarg,char** args)

//======================================================================
#undef DEBUG_NAMESPACE
#ifdef DEBUG_STANDALONE
#define DEBUG_NAMESPACE "Standalone"
#endif    
#include "debugMacros.hpp"
//======================================================================

void runtimeErrorHook()
{
  if (VampireKernel::currentSession())
    {
      cout << "% Trying to shut down current kernel session...\n";
      VampireKernel::currentSession()->reportError();
    };
  if (tabulation)
    {
      if (VampireKernel::currentSession()) 
	VampireKernel::currentSession()->tabulate();
      tabulation->result('0');
      tabulation->terminationReason("error");
      tabulation->errorMessage("runtime","some module detected inconsistency in run time");
    };
}; // void runtimeErrorHook()

void debugSystemInternalErrorHook()
{  
  if (VampireKernel::currentSession())
    {
      cout << "% Trying to shut down current kernel session...\n";
      VampireKernel::currentSession()->reportError();
    };
  if (tabulation)
    {
      if (VampireKernel::currentSession()) 
	VampireKernel::currentSession()->tabulate();
      tabulation->result('0');
      tabulation->terminationReason("error");
      tabulation->errorMessage("debug","debugging system detected internal inconsistency");
    };
}; // void debugSystemInternalErrorHook()

const char* spellSignal(int sigNum)
{
  switch (sigNum)
    {
#if (BK_TARGET_OPERATIONAL_ENVIRONMENT_TYPE == BK_TARGET_OPERATIONAL_ENVIRONMENT_TYPE_UNIX)
    case SIGTERM: return "SIGTERM";
    case SIGQUIT: return "SIGQUIT";
    case SIGHUP: return "SIGHUP";
    case SIGINT: return "SIGINT";
    case SIGXCPU: return "SIGXCPU";
    case SIGILL: return "SIGILL";
    case SIGFPE: return "SIGFPE";
    case SIGSEGV: return "SIGSEGV";
    case SIGBUS: return "SIGBUS";
    case SIGTRAP: return "SIGTRAP";
    case SIGABRT: return "SIGABRT";
#elif (BK_TARGET_OPERATIONAL_ENVIRONMENT_TYPE == BK_TARGET_OPERATIONAL_ENVIRONMENT_TYPE_MSWIN32)
    case SIGABRT: return "SIGABRT";
    case SIGFPE: return "SIGFPE";
    case SIGILL: return "SIGILL";
    case SIGINT: return "SIGINT";
    case SIGSEGV: return "SIGSEGV";
    case SIGTERM: return "SIGTERM";
#endif
    default: return "UNKNOWN SIGNAL";
    };
};

#if (BK_TARGET_OPERATIONAL_ENVIRONMENT_TYPE == BK_TARGET_OPERATIONAL_ENVIRONMENT_TYPE_UNIX)
void handleSignal(int sigNum)
{
  switch (sigNum)
    {
    case SIGTERM:
    case SIGQUIT:
    case SIGHUP:
    case SIGXCPU:
      if (stdOutput)
	*stdOutput << "% Aborted by signal " << spellSignal(sigNum) << "\n";
      if (VampireKernel::currentSession())
	{	  
	  if (stdOutput)
	    *stdOutput << "% Trying to shut down current kernel session...\n";
	  VampireKernel::currentSession()->reportInterrupt(sigNum);
	};
      if (tabulation)
	{
	  if (VampireKernel::currentSession()) 
	    VampireKernel::currentSession()->tabulate();
	  tabulation->result('0');
	  tabulation->terminationReason("external interrupt");
	};
      TABULATE_BACKWARD_CONSTR_CHECK_STATISTICS(tabulation);
      TABULATE_FORWARD_CONSTR_CHECK_STATISTICS(tabulation);
      if (stdOutput)
	*stdOutput << "% Bailing out now: aborted by interrupt signal " 
		   << spellSignal(sigNum) << "\n";
      cancelSignalHandling();
      Exit::exit(1);
      
    case SIGINT:
      {

	//DF; REPCPH;
	
	INTERCEPT_BACKWARD_CONSTR_CHECK_SUSPEND;
	INTERCEPT_FORWARD_CONSTR_CHECK_SUSPEND; 

	if (VampireKernel::currentSession())
	  {
	    VampireKernel::currentSession()->suspend();
	  }
	else
	  {
	    if (stdOutput)
	      *stdOutput << "% No kernel session running at the moment.\n";
	  };
	cout << "Continue? (y/n)\n";
	
	char a;
	cin >> a;
	
	if ((a == 'n') || (a == 'N'))
	  {
	    if (VampireKernel::currentSession())
	      VampireKernel::currentSession()->reportInterrupt(sigNum);
	    cout << "% Terminated by the user.\n";
	    if (tabulation)
	      {
		if (VampireKernel::currentSession()) 
		  VampireKernel::currentSession()->tabulate();
		tabulation->result('0');
		tabulation->terminationReason("external interrupt");
	      };
	    TABULATE_BACKWARD_CONSTR_CHECK_STATISTICS(tabulation);
	    TABULATE_FORWARD_CONSTR_CHECK_STATISTICS(tabulation);
	    Exit::exit(0); 
	  }
	else
	  {
            if (VampireKernel::currentSession())
	      VampireKernel::currentSession()->resume();
	    cout << "\nContinuing...\n";
	    INTERCEPT_BACKWARD_CONSTR_CHECK_RESUME;  
	    INTERCEPT_FORWARD_CONSTR_CHECK_RESUME; 
	  };
      };
      return;
      

    case SIGILL:
    case SIGFPE:
    case SIGSEGV:
    case SIGBUS:
    case SIGTRAP:
    case SIGABRT:
      cout << "% Error signal " << spellSignal(sigNum) << " catched\n";
      if (VampireKernel::currentSession())
	{
	  cout << "% Trying to shut down current kernel session...\n";
	  VampireKernel::currentSession()->reportErrorSignal(sigNum);
	};
      if (tabulation)
	{	  
	  if (VampireKernel::currentSession()) 
	    VampireKernel::currentSession()->tabulate();
	  const char* errorQualifier = "Error signal";
	  char errorMessage[256];
	  sprintf(errorMessage,"error signal %d received",sigNum);
	  tabulation->errorMessage(errorQualifier,errorMessage);
	  tabulation->result('0');
	  tabulation->terminationReason("error signal");
	};
      cout << "% Bailing out now: aborted by error signal " << spellSignal(sigNum) << "\n";
      cancelSignalHandling();
      Exit::exit(1); 
    default:
      break;
    };
}; // void handleSignal(int sigNum)

#elif (BK_TARGET_OPERATIONAL_ENVIRONMENT_TYPE == BK_TARGET_OPERATIONAL_ENVIRONMENT_TYPE_MSWIN32)
void handleSignal(int sigNum)
{
  switch (sigNum)
    {
    case SIGTERM:
      cancelSignalHandling();
      if (stdOutput)
	*stdOutput << "% Aborted by signal " << spellSignal(sigNum) << "\n";
      if (VampireKernel::currentSession())
	{	  
	  if (stdOutput)
	    *stdOutput << "% Trying to shut down current kernel session...\n";
	  VampireKernel::currentSession()->reportInterrupt(sigNum);
	};
      if (tabulation)
	{
	  if (VampireKernel::currentSession()) 
	    VampireKernel::currentSession()->tabulate();
	  tabulation->result('0');
	  tabulation->terminationReason("external interrupt");
	};
      TABULATE_BACKWARD_CONSTR_CHECK_STATISTICS(tabulation);
      TABULATE_FORWARD_CONSTR_CHECK_STATISTICS(tabulation);
      if (stdOutput)
	*stdOutput << "% Bailing out now: aborted by interrupt signal " 
		   << spellSignal(sigNum) << "\n";
      Exit::exit(1);
      
    case SIGINT:
      {

	//DF; REPCPH;
	
	INTERCEPT_BACKWARD_CONSTR_CHECK_SUSPEND;  
	INTERCEPT_FORWARD_CONSTR_CHECK_SUSPEND; 

	if (VampireKernel::currentSession())
	  {
	    VampireKernel::currentSession()->suspend();
	  }
	else
	  {
	    if (stdOutput)
	      *stdOutput << "% No kernel session running at the moment.\n";
	  };
	cout << "Continue? (y/n)\n";
	
	char a;
	cin >> a;
	
	if ((a == 'n') || (a == 'N'))
	  {
	    if (VampireKernel::currentSession())
	      VampireKernel::currentSession()->reportInterrupt(sigNum);
	    cout << "% Terminated by the user.\n";
	    if (tabulation)
	      {
		if (VampireKernel::currentSession()) 
		  VampireKernel::currentSession()->tabulate();
		tabulation->result('0');
		tabulation->terminationReason("external interrupt");
	      };
	    TABULATE_BACKWARD_CONSTR_CHECK_STATISTICS(tabulation);
	    TABULATE_FORWARD_CONSTR_CHECK_STATISTICS(tabulation);
	    Exit::exit(0); 
	  }
	else
	  {
            if (VampireKernel::currentSession())
	      VampireKernel::currentSession()->resume();
	    cout << "\nContinuing...\n";
	    INTERCEPT_BACKWARD_CONSTR_CHECK_RESUME; 
	    INTERCEPT_FORWARD_CONSTR_CHECK_RESUME; 
	  };
      };
      return;
      

    case SIGILL:
    case SIGFPE:
    case SIGSEGV:
    case SIGABRT:
      cancelSignalHandling();
      cout << "% Error signal " << spellSignal(sigNum) << " catched\n";
      if (VampireKernel::currentSession())
	{
	  cout << "% Trying to shut down current kernel session...\n";
	  VampireKernel::currentSession()->reportErrorSignal(sigNum);
	};
      if (tabulation)
	{	  
	  if (VampireKernel::currentSession()) 
	    VampireKernel::currentSession()->tabulate();
	  const char* errorQualifier = "Error signal";
	  char errorMessage[256];
	  sprintf(errorMessage,"error signal %d received",sigNum);
	  tabulation->errorMessage(errorQualifier,errorMessage);
	  tabulation->result('0');
	  tabulation->terminationReason("error signal");
	};
      cout << "% Bailing out now: aborted by error signal " << spellSignal(sigNum) << "\n";
      Exit::exit(1); 
    default:
      break;
    };
}; // void handleSignal(int sigNum)

#endif




#if (BK_TARGET_OPERATIONAL_ENVIRONMENT_TYPE == BK_TARGET_OPERATIONAL_ENVIRONMENT_TYPE_UNIX)

void cancelSignalHandling()
{
  signal(SIGTERM,SIG_DFL);
  signal(SIGQUIT,SIG_DFL);
  signal(SIGHUP,SIG_DFL);
  signal(SIGINT,SIG_DFL);
  signal(SIGXCPU,SIG_DFL);
#ifndef DEBUG_ERROR_SIGNALS
  signal(SIGILL,SIG_DFL);
  signal(SIGFPE,SIG_DFL);
  signal(SIGSEGV,SIG_DFL);
  signal(SIGBUS,SIG_DFL);
  signal(SIGTRAP,SIG_DFL);
  signal(SIGABRT,SIG_DFL);
#endif
}; // void cancelSignalHandling()

void setSignalHandlers()
{
  signal(SIGTERM,handleSignal);
  signal(SIGQUIT,handleSignal);
  signal(SIGHUP,handleSignal);
  signal(SIGINT,handleSignal);
  signal(SIGXCPU,handleSignal);
#ifndef DEBUG_ERROR_SIGNALS
  signal(SIGILL,handleSignal);
  signal(SIGFPE,handleSignal);
  signal(SIGSEGV,handleSignal);
  signal(SIGBUS,handleSignal);
  signal(SIGTRAP,handleSignal);
  signal(SIGABRT,handleSignal);
#endif
}; // void setSignalHandlers()


#elif (BK_TARGET_OPERATIONAL_ENVIRONMENT_TYPE == BK_TARGET_OPERATIONAL_ENVIRONMENT_TYPE_MSWIN32)

void cancelSignalHandling()
{
  signal(SIGTERM,SIG_DFL);
  signal(SIGINT,SIG_DFL);
#ifndef DEBUG_ERROR_SIGNALS
  signal(SIGILL,SIG_DFL);
  signal(SIGFPE,SIG_DFL);
  signal(SIGSEGV,SIG_DFL);
  signal(SIGABRT,SIG_DFL);
#endif
}; // void cancelSignalHandling()

void setSignalHandlers()
{
  signal(SIGTERM,handleSignal);
  signal(SIGINT,handleSignal);
#ifndef DEBUG_ERROR_SIGNALS
  signal(SIGILL,handleSignal);
  signal(SIGFPE,handleSignal);
  signal(SIGSEGV,handleSignal);
  signal(SIGABRT,handleSignal);
#endif
}; // void setSignalHandlers()

#endif


const char* extractBasename(const char* fileName)
{
  int c = strlen(fileName) - 1; 
  while ((c >= 0) && (fileName[c] != '/')) c--;
  c++;
  char* res = new char[strlen(fileName) - c + 1];
  strcpy(res,fileName + c); 
  return res;
};

const char* generateJobId()
{
  time_t sessionTime = time(static_cast<time_t*>(0)); 
  tm* gmt = gmtime(&sessionTime); 
  int sessionDateDD = gmt->tm_mday;
  int sessionDateMM = gmt->tm_mon;    
  int sessionDateYY = 1900 + gmt->tm_year;
  int sessionTimeHH = gmt->tm_hour;
  int sessionTimeMM = gmt->tm_min;
  int sessionTimeSS = gmt->tm_sec;
 
#if (BK_TARGET_OPERATIONAL_ENVIRONMENT_TYPE == BK_TARGET_OPERATIONAL_ENVIRONMENT_TYPE_UNIX)
  pid_t sessionProcessId = getpid();
#elif (BK_TARGET_OPERATIONAL_ENVIRONMENT_TYPE == BK_TARGET_OPERATIONAL_ENVIRONMENT_TYPE_MSWIN32)
  int sessionProcessId = _getpid();
#endif

  ostrstream str;
  const char* hhh = (sessionTimeHH < 10) ? "0" : "";
  const char* mmm = (sessionTimeMM < 10) ? "0" : "";
  const char* sss = (sessionTimeSS < 10) ? "0" : "";
  str << sessionDateDD << sessionDateMM  << sessionDateYY;
  str << hhh << sessionTimeHH << mmm << sessionTimeMM << sss << sessionTimeSS;
  str << sessionProcessId << '\0';
  char* res = new char[strlen(str.str()) + 1];
  strcpy(res,str.str());
  return res; 
  
}; // const char* generateJobId()

void communicateTerm(const Input::Term* term,VampireKernel& kernel)
{
  CALL("communicateTerm(const Input::Term* term,VampireKernel& kernel)");
  const Input::TermList* args;
  switch (term->top()->type())
    {
    case Input::Symbol::Variable:
      kernel.receiveVariable(term->top()->variableNumber());
      return;
    case Input::Symbol::Function:
      kernel.receiveFunction(term->top()->number());
      for (args = term->arguments();
	   args;
	   args = args->tl())
	{
	  communicateTerm(args->hd(),kernel);
	};
      return;
    default:
      RuntimeError::report("Bad input clause.");
    };

}; // void communicateTerm(const Input::Term* term,VampireKernel& kernel)


void describeOptions(CommandLine& CL)
{
  CL.option("h","help on command line syntax");
  CL.option("help","same as -h");
  CL.option("v","version information");
  CL.option("I",CommandLine::String,"<dir>","specifies the directory for making includes, "
	    "overrides the value obtained from the environment "
	    "variable TPTP_DIR");
  CL.option("include",CommandLine::String,"<dir>","same as -I <dir>");
  CL.option("test_id",CommandLine::String,"<test id>","test identifier, "
	    "   normaly used to identify "
	    "   series of Vampire runs. "
	    "   Default: <test id> = unspecified_test");
  CL.option("T",CommandLine::String,"<test id>","same as --test_id <test id>");

  // preprocessing-only mode
  CL.option("prepro_only",
	    CommandLine::Flag,
	    "<flag>", 
	    "<flag> = on switches preprocessing-only mode. "
	    "Default: <flag> = off");

 
  // main algorithm 

  CL.option("main_alg",CommandLine::Long,"<num>","sets the saturation algorithm, "
	    "   <num> = 0 => OTTER algorithm (default), "
	    "   <num> = 1,2 are obsolete, (3 is used instead)  "
	    "   <num> = 3 => DISCOUNT algorithm,  special memory management for storing passive clauses ");

  // inference rules

  CL.option("no_inferences_between_rules",
	    CommandLine::Flag,
	    "<flag>", 
	    "when <flag> = on, blocks resolution "
	    "(except simplifications) between clauses containing "
	    "both positive and negative literals. Default: <flag> = off");

  CL.option("static_splitting",CommandLine::Flag,"<flag>","switches static splitting, default: <flag> = on");
  CL.option("dynamic_splitting",CommandLine::Flag,"<flag>","switches dynamic splitting, default: <flag> = off");
  CL.option("splitting_style",CommandLine::Long,"<num>","<num> = 1 (default) => approximation of the standard "
	    "splitting by nonblocking splitting, "
	    "<num> = 2 => approximation of the standard "
	    "splitting by blocking splitting ");

  CL.option("splitting_with_naming",CommandLine::Flag,"<flag>",
	    "switches naming in splitting, default : <flag> = on");

  CL.option("static_neg_eq_splitting",CommandLine::Long,"<weight>","ground arguments of negative equalities "
	    "          whose weight exceeds <weight> will be "
	    "          renamed, <weight> = 0 switches "
	    "          neg. eq. splitting off. Default: <weight> = 3"); 
  CL.option("paramodulation",CommandLine::Flag,"<flag>","switches paramodulation, default: <flag> = on");

  CL.option("sim_back_sup",CommandLine::Long,"<degree>",
	    "describes how many redexes can be rewritten simultaneously when backward superposition is applied, "
	    "<degree> = 0 (default) => redexes are rewritten only in one literal, "
	    "<degree> > 0 => redexes in the whole clause are rewritten simultaneously ");

  CL.option("selection",CommandLine::Long,"<num>",": choses literal selection function "
	    "    (default <num> = 4), "
	    "    <num> = 1 => only maximal literals are selected; "
	    "    <num> = 2 => positive hyperresolution selection (negative "
	    "literals are always selected before any positive), "
	    "from several negative literals the one with the "
	    "maximal weight is selected, maximal literals are "
	    "selected in positive clauses; "
	    "    <num> = 3 => positive hyperresolution selection, "
	    "a heuristic intended to minimise the expected number of "
	    "possible inferences is used to chose from several negative literals, "
	    "maximal literals are selected in positive clauses; "
	    "    <num> = 4 (default) => maximal literals are selected, then we try to "
	    "improve selection by selecting one negative literal with maximal weight; "
	    "    <num> = 5 => maximal literals are selected, then we try to "
	    "improve selection by selecting one negative "
	    "literal minimising the expected number of possible inferences; "
	    "    <num> = 6 => positive hyperresolution selection (negative "
	    "literals are always selected before any positive). "
	    "From several negative literals the maximally nonground "
	    "is selected. From such literals we select those with greater size. "
	    "Maximal literals are selected in positive clauses; "
	    "    <num> = 7 => maximal literals are selected, then we try to "
	    "improve selection by selecting one negative literal with maximal "
	    "number of different variables, smaller depths of variable occurences "
	    "and bigger size; "
	    "Literal selections with <num> >= 1000 are arbitrary, "
	    "in most cases they make the procedure incomplete "
	    "(ond exception - Horn problems). "
	    "<num> = 1002 => If the clause contains some inherently selected "
	    "literals, only they are selected. "
	    "Otherwise, if the clause contains a literal "
	    "which is not a positive equality, "
	    "we never select a positive equality. "
	    "Among the literals available for selection "
	    "we always select a ""maximally nonground""literal. "
	    """Maximally nonground"" means having maximal total number "
	    "of variable occurences among the literals "
	    "having maximal number of different variables. "
	    "Among such literals we select one with the maximal size. "
	    "    <num> = 1003 => If the clause contains some inherently selected "
	    "literals, only they are selected. "
	    "Otherwise, if the clause contains a literal "
	    "which is not a positive equality, "
	    "we never select a positive equality. "
	    "Among the literals available for selection "
	    "we always select a literal of the maximal size. "
	    "Among several such literals we select by the function 1002. "
	    "    <num> = 1004 => If the clause contains some inherently selected "
	    "literals, only they are selected. "
	    "Otherwise, if the clause contains a literal "
	    "which is not a positive equality, "
	    "we never select a positive equality. "
	    "Among the literals available for selection "
	    "we always select a ""maximally ground"" literal. "
	    """Maximally ground"" means having minimal total number "
	    "of variable occurences "
	    "among literals having minimal number of different "
	    "variables. "
	    "Among such literals we select one with the maximal size. "
	    "    <num> = 1005 => If the clause contains some inherently selected "
	    "literals, only they are selected. "
	    "Otherwise, if the clause contains a literal "
	    "which is not a positive equality, "
	    "we never select a positive equality. "
	    "Among the literals available for selection "
	    "we always select "
	    "either a ""maximally ground"" negative literal, "
	    "or a ""maximally nonground"" positive literal. "
	    "Among such literals we select one with the maximal size. "
	    "   <num> = 1006 =>  Bidirectional selection.  In a rule, i.e. a clause "
	    "containing both positive and negative literals we  "
	    "always select one positive and one negative literal. "
	    "If we have several literals of the same polarity, we "
	    "select a bigger of the maximally nonground ones. "
	    "In a fact, i.e. a clause with all literals "
	    "of the same polarity, we select a bigger maximally "
	    "nonground literal. "
	    "Switches off --inherited_negative_selection. "
	    "   <num> = 1007 =>  Bidirectional selection. In a rule, i.e. a clause "
	    "containing both positive and negative literals we  "
	    "always select one positive and one negative literal. "
	    "If we have several literals of the same polarity, we select "
	    "those with maximal size. In a fact, i.e. a clause with "
	    "all literals of the same polarity, we select one "
	    "of the biggest literals. "
	    "Switches off --inherited_negative_selection. "
	    "   <num> = 1008 =>  Bidirectional selection.  In a rule, i.e. a clause "
	    "containing both positive and negative literals we  "
	    "always select one positive and one negative literal. "
	    "If we have several literals of the same polarity, we "
	    "select those that are maximally ground. In a fact, i.e. "
	    "a clause with all literals of the same polarity, we "
	    "select one maximally ground literal. "
	    "Switches off --inherited_negative_selection."
	    "    <num> = 1009 => Bidirectional selection.  In a rule, i.e. a clause "
	    "containing both positive and negative literals we  "
	    "always select one positive and one negative literal. "
	    "If we have several positive literals, we select "
	    "those that are maximally nonground. If we have several "
	    "negative literals, we select those that are maximally ground. "
	    "In a positive fact, i.e. a clause with only positive literals, "
	    "we select one maximally nonground literal. "
	    "In a negative fact, i.e. a clause with only negative literals, "
	    "we select one maximally ground literal. "
	    "Switches off --inherited_negative_selection."
	    "    <num> = -2 => same as <num> = 2 but positive selection is used instead of negative "
	    "selection for nonequality literals: positive nonequality literals "
	    "are treated as negative and vice versa; "
	    "    <num> = -3 => same as <num> = 3 but positive selection is used instead of negative "
	    "selection for nonequality literals: positive nonequality literals "
	    "are treated as negative and vice versa; "
	    "    <num> = -4 => same as <num> = 4 but positive selection is used instead of negative "
	    "selection for nonequality literals: positive nonequality literals "
	    "are treated as negative and vice versa; "
	    "    <num> = -6 => same as <num> = 6 but positive selection is used instead of negative "
	    "selection for nonequality literals: positive nonequality literals "
	    "are treated as negative and vice versa; "
	    "    <num> = -7 => same as <num> = 7 but positive selection is used instead of negative "
	    "selection for nonequality literals: positive nonequality literals "
	    "are treated as negative and vice versa; "
	    "    <num> = -1005 => same as <num> = 1005 but positive nonequality literals "
	    "are treated as negative and vice versa; "
	    "    <num> = -1009 => same as <num> = 1009 but positive nonequality literals "
	    "are treated as negative and vice versa");       

  CL.option("literal_comparison_mode",CommandLine::Long,"<num>"," adjust comparison"
	    "of literals in those literal selection function that use the notion "
	    "of maximal literal. "
	    "<num> = 0 (default) => we simply compare the atoms with the reduction ordering. "
	    "There is an exception: when at least one of the literals is equality, "
	    "we first compare precedences of their headers (predicate + polarity), "
	    "and only if this check is inconclusive (both are equations "
	    "of the same polarity) we compare the arguments. "
	    "<num> = 1 => we always select literals whose headers (predicate + polarity) "
	    "have greater precedence. Only literals with maximal headers are compared by "
	    "the reduction ordering. This makes comparison faster but at the cost "
	    "of selection quality; "
	    "<num> = 2 => same as <num> = 1 but complementary headers are considered "
	    "to have the same precedence. "); 

  CL.option("inherited_negative_selection",CommandLine::Flag,"<flag>",
	    "<flag> = on => selection of negative literals is inherited. "
	    "Default: <flag> = off");

  // strategy 
  CL.option("forward_subsumption",CommandLine::Flag,"<flag>","switches forward subsumption, default: <flag> = on");
  CL.option("fs_set_mode",CommandLine::Flag,"<flag>",
	    "switches set mode in forward subsumption, more efficient than the default multiset mode "
	    "but in rare circumstances may cause incompleteness. Default: <flag> = off");

  CL.option("fs_optimised_multiset_mode",CommandLine::Flag,"<flag>",
	    "relaxes multiset mode for forward subsumption while preserving "
	    "completeness, can be overriden by --fs_set_mode. Default: <flag> = off");

  CL.option("fs_use_sig_filters",CommandLine::Flag,"<flag>",
	    "Deprecated feature, may disappear any time. Switches use of signature filters in forward subsumption. "
	    "Default: <flag> = off");

  CL.option("fs_old_style",CommandLine::Flag,"<flag>",
	    "Deprecated feature, may disappear any time. Switches use of the nonadaptive version "
	    "of code trees in forward subsumption. Default: <flag> = off");

  CL.option("simplify_by_fs",CommandLine::Long,"<min.length>",
	    "<min.length> = 0 switches forward "
	    "subsumption resolution off, when <min.length> >= 1 "
	    "forward subsumption resolution is applied to clauses "
	    "whose length >= <min.length>. Default: <min.length> = 1");

  CL.option("forward_demodulation",CommandLine::Long,"<degree>",
	    "forward simplification by unit equalities, "
	    "<degree> <= 0 switches it off, "
	    "<degree> = 1 only preordered rules are used, "
	    "<degree> = 2 nearly preordered rules can be used, "
	    "<degree> = 3 volatile rules can be used, "
	    "<degree> >= 4 (default) any rules can be used"); 

  CL.option("fd_on_splitting_branches",CommandLine::Flag,"<flag>",
	    "switches forward demodulation with conditions that are marks "
	    "of splitting branches. Default: <flag> = on");

  CL.option("normalize_by_commutativity",CommandLine::Flag,"<flag>",
	    "switches faster normalization by commutativity laws. Default: <flag> = on");
   
  CL.option("backward_demodulation",CommandLine::Long,"<degree>",
	    "backward simplification by unit equalities, "
	    "<degree> <= 0 switches it off, "
	    "<degree> = 1 only preordered rules are used, "
	    "<degree> = 2 nearly preordered rules can be used, "
	    "<degree> = 3 volatile rules can be used, "
	    "<degree> >= 4 (default) any rules can be used");
 
  CL.option("bd_on_splitting_branches",CommandLine::Flag,"<flag>",
	    "switches backward demodulation with conditions that are "
	    "marks of splitting branches. Default: <flag> = on");

  CL.option("simplify_by_eq_res",CommandLine::Flag,"<flag>",
	    "switches simplification by equality resolution. Default: <flag> = on");


  CL.option("backward_subsumption",CommandLine::Flag,"<flag>",
	    "switches backward subsumption. Default: <flag> = on");

  CL.option("bs_set_mode",CommandLine::Flag,"<flag>",
	    "switches set mode in backward subsumption, "
	    "more efficient than the default multiset mode"
	    "but in rare circumstances may cause incompleteness. Default: <flag> = off");

  CL.option("bs_optimised_multiset_mode",CommandLine::Flag,"<flag>",
	    "relaxes multiset mode for backward subsumption while preserving "
	    "completeness, can be overriden by --bs_set_mode. Default: <flag> = off");


  CL.option("orphan_murder",CommandLine::Flag,"<flag>",
	    "switches orphan murder. Default: <flag> = off"); 


  CL.option("lrs",CommandLine::Long,"<num>"," : switches the Limited Resource Strategy, "
	    "<num> = 0 => the LRS is off; "
	    "<num> = 1 (default) => the standard LRS is on. It avoids recycling "
	    "unreachable passive clauses by imposing tougher weight limits; "
	    "<num> = 2 => similar to 1, but a more conservative weight reachability "
	    "estimation is used. Unreachable passive clauses are recycled; "
	    "<num> = 3 => similar to 2, but an even more conservative weight reachability "
	    "estimation is used; "
	    "<num> = 4 or <num> = 5 => experimental implementations of LRS. "
	    "Weigh limit is adjusted by trial and error, no estimation is necessary");
  
  CL.option("plot_usage_pace",CommandLine::String,"<file>","undocumented feature");
  CL.option("plot_weight_limit_changes",CommandLine::String,"<file>","undocumented feature");

  CL.option("elim_def",CommandLine::Long,"<num>", 
	    "sets limit on the number of definition elimination iterations. Default: <num> = 5");


  CL.option("simplification_ordering",
	    CommandLine::val("nonrecursive_KB",
			     CommandLine::val("standard_KB")),
	    "<val>",
	    "specifies the simplification ordering used by the calculus. "
	    "The possibilities are: "
	    "<val> = nonrecursive_KB (default), "
	    "<val> = standard_KB (to be implemented soon)");

  CL.option("symbol_precedence_by_arity",CommandLine::Long,"<num>","<num> = 0 - ranking depends only "
	    "    on the order of appearance, "
	    "    <num> > 0 (default) - headers with greater arity receive "
	    "       greater ranks, "
	    "    <num> < 0 - headers with smaller arity receive "
	    "       greater ranks; "); 

  CL.option("header_precedence_kinky",CommandLine::Flag,"<flag>",
	    "switches the kinky ranking of literal headers, "
	    "for details contact the authors. Default: <flag> = off");

  CL.option("term_weighting_scheme",
	    CommandLine::val("nonuniform_constant",
			     CommandLine::val("uniform")),
	    "<val>",
	    "specifies how the weight of terms is computed. "
	    "<val> = nonuniform_constant => weight(f(t1,..,tn)) = a0 + weight(t1) + .. + weight(tn); "
	    "<val> = uniform (default) => weight(f(t1,..,tn)) = 1 + weight(t1) + .. + weight(tn)");

  CL.option("symbol_weight_by_arity",CommandLine::Long,"<num>",
	    "Experimental feature, may disappear in the future versions. "
	    "When a nonuniform term weighting scheme is used, "
	    "this option specifies how the weights are assigned "
	    "to symbols depending on their arities. "
	    "It only affects those symbols that have not been "
	    "explicitely given weights by the user. "
	    "<num> >= 0 => weight(f) = (1 + <num>*arity(f)); "
	    "<num> < 0 => weight(f) = (1 - <num>*(MAX_ARITY - arity(f))), "
	    "MAX_ARITY = 1023; "
	    "Default: <num> = 0");
	    

  CL.option("axioms_for_support_only",CommandLine::Flag,"<flag>",
	    "When on, all input axioms will be treated as support clauses. "
	    "Default: <flag> = off");


  // numeric options 
 
  CL.option("t",CommandLine::Long,"<seconds>", "sets time limit to <seconds>, where <seconds> is a long integer");
  CL.option("time_limit",CommandLine::Long,"<seconds>","same as -t <seconds>");

  CL.option("m",CommandLine::Long,"<Kb>", "sets memory limit to <Kb>, where <Kb> is a long integer");
  CL.option("memory_limit",CommandLine::Long,"<Kb>","same as -m <Kb>");
  CL.option("allocation_buffer_size",CommandLine::Long,"<num>",
	    "as a percentage of the memory limit, default value is 10");
  CL.option("allocation_buffer_price",CommandLine::Double,"<coeff>",
	    "if <def> is the allocation buffer deficit, "
	    "the systems abandons any attempt to eliminate "
	    "the deficit as soon as <coeff>*<def> Kb have been "
	    "returned to the heap manager,<coeff> = 2.0 "
	    "is the default value");

  CL.option("max_skolem",CommandLine::Long,"<num>",
	    "limit on the number of Skolem symbols generated for "
	    "splitting and splitting of negative equalities. Default: <num> = 1024");

  CL.option("max_active",CommandLine::Long,"<number>","limit on the number of active clause. "
	    "Default: <number> = inf."); 
  CL.option("max_passive",CommandLine::Long,"<number>","limit on the number of passive clause. "
	    "Default: <number> = inf."); 
  CL.option("max_weight",CommandLine::Long,"<weight>","initial value of maximal "
	    "admissible clause weight, "
	    "<weight> is a long integer. "
	    "Default: <weight> = inf. ");
  CL.option("w",CommandLine::Long,"<weight>","same as --max_weight <weight>");

  CL.option("max_inference_depth",CommandLine::Long,"<depth>",
	    "specifies how deep an inference tree can be. "
	    "Default: <depth> = inf. ");

  CL.option("age_weight_ratio",CommandLine::Long,"<num>",
	    "same as pick_given_ratio in OTTER. Default: <num> = 5");

  CL.option("selected_pos_eq_penalty_coefficient",CommandLine::Double,"<num>",
	    "if a positive equality is selected in a clause, the weight "
	    "of the clause is multiplied by this coefficient. Default: <num> = 1.0. Values of <num> below 1.0 are ignored.");

  CL.option("nongoal_penalty_coefficient",CommandLine::Double,"<num>",
	    "if none of the clause ancestors is a goal, the weight "
	    "of the clause is multiplied by this coefficient. Default: <num> = 1.0. Values of <num> below 1.0 are ignored.");

  CL.option("first_time_check",CommandLine::Long,"<number>",
	    "the LRS is off until <number>% of the time limit has been used, "
	    "unless it is switched on by memory. Default <number> = 5");
  CL.option("first_mem_check",CommandLine::Long,"<number>",
	    "the LRS is off until <number>% of the memory limit has been used, "
	    "unless it is switched on by time. Default: <number> = 200");

  CL.option("max_number_of_answers",CommandLine::Long,"<number>",
	    "specifies the maximal number of answers to look for. "
	    "Default: <number> = 1");

  // interface options

  CL.option("show_opt",CommandLine::Flag,"<flag>",
	    "switches on output of options."
	    "Default: <flag> = off");  
  CL.option("show_input",CommandLine::Flag,"<flag>",
	    "switches output of input clauses."
	    "Default: <flag> = off");
  CL.option("show_prepro",CommandLine::Flag,"<flag>",
	    "switches output of preprocessed clauses."
	    "Default: <flag> = off");
  CL.option("show_gen",CommandLine::Flag,"<flag>",
	    "switches output of generated inferences."
	    "Default: <flag> = off");
  CL.option("show_passive",CommandLine::Flag,"<flag>",
	    "switches output of passive clauses."
	    "Default: <flag> = off");
  CL.option("show_active",CommandLine::Flag,"<flag>",
	    "switches output of active clauses."
	    "Default: <flag> = off");
  CL.option("show_definitions",CommandLine::Flag,"<flag>",
	    "switches output of clauses treated as definitions in stratified resolution."
	    "Default: <flag> = off");
  CL.option("proof",CommandLine::Flag,"<flag>",
	    "switches output of proofs. Default: <flag> = on");
  CL.option("show_profile",CommandLine::Flag,"<flag>",
	    "switches output of input and preprocessed input profiles."
	    "Default: <flag> = off");
  CL.option("show_weight_limit_changes",CommandLine::Flag,"<flag>",
	    "switches monitoring of current weight limit."
	    "Default: <flag> = off");
  CL.option("tab",CommandLine::String,"<file>","specifies a file to be used for tabulation."
	    " There is no default value for <file>."
	    " If this option is not given,"
	    " there will be no tabulation at all.");

  CL.option("silent",CommandLine::Flag,"<flag>",
	    "switches standard output off. Default: <flag> = off"); 
  

#ifdef INTERCEPT_FORWARD_MATCHING_INDEXING
  CL.option("fmilog",CommandLine::String,"<file>","specifies a file to be used for logging forward "
	    "     matching index operations"); 
  CL.option("fmi_log_file_max_size",CommandLine::Long,"<Kb>","specifies maximal size (in Kbytes) of the file to be used for "
	    "logging forward matching index operations"); 
#endif

#ifdef INTERCEPT_BACKWARD_SUBSUMPTION_INDEXING
  CL.option("bsilog",CommandLine::String,"<file>","specifies a file to be used for logging backward "
	    "     subsumption index operations"); 
  CL.option("bsi_log_file_max_size",CommandLine::Long,"<Kb>","specifies maximal size (in Kbytes) of the file to be used for "
	    "logging backward subsumption index operations"); 
 
#endif

#ifdef INTERCEPT_BACKWARD_MATCHING_INDEXING
  CL.option("bmilog",CommandLine::String,"<file>","specifies a file to be used for logging backward "
	    "     matching index operations"); 
  CL.option("bmi_log_file_max_size",CommandLine::Long,"<Kb>","specifies maximal size (in Kbytes) of "
	    "     the file to be used for "
	    "     logging backward matching index operations"); 
#endif



#ifdef INTERCEPT_INDEXED_SEARCH_FOR_GENERALISATIONS 
  CL.option("sfgilog",CommandLine::String,"<file>","specifies a file to be used for logging forward "
	    "     matching index (with exhaustive retrieval) operations"); 
  CL.option("sfgi_log_file_max_size",CommandLine::Long,"<Kb>","specifies maximal size (in Kbytes) of "
	    "     the file to be used for "
	    "     logging forward matching index (with exhaustive retrieval) operations"); 
#endif

#ifdef INTERCEPT_BACKWARD_SUPERPOSITION_INDEXING
  CL.option("bsiulog",CommandLine::String,"<file>","specifies a file to be used for logging "
	    "unification indexing operations for backward superposition"); 
  CL.option("bsiu_log_file_max_size",CommandLine::Long,"<Kb>","specifies maximal size (in Kbytes) of "
	    "the file to be used for logging "
	    "unification indexing operations for backward superposition"); 
#endif


#ifdef VKERNEL_FOR_STEP_RESOLUTION

  CL.option("step_resolution_literal_selection",CommandLine::Flag,"<flag>",
	    "switches adjustment of literal selection for step resolution. " 
            "If <flag> = on, then in a clause containing literals "
            "with nonnullary nonequality predicates and with first argument "
            "of depth greater than 0 only such literals are selected. "
            "This feature is experimental. " 
	    "Default: <flag> = off");
#endif

  // non-option arguments
  CL.nonoptMiddle("<incl.file>","files that are treated as they "
		  "were included in the problem file");
  static const char* nonoptSuffNames[] = { "<problem>" };
  static const char* nonoptSuffComments[] = { "main problem file" };
  CL.nonoptSuffix(1,nonoptSuffNames,nonoptSuffComments);

}; // void describeOptions(CommandLine& CL)



#ifndef NO_DEBUG
void outputInferenceStructure(const VampireKernel::Clause* cl)
{
  CALL("outputInferenceStructure(const VampireKernel::Clause* cl)");

  if (cl->isBuiltInTheoryFact()) 
    {
      cout << cl->number() << " built-in theory fact []\n";
      return;
    };


  if (cl->isInputClause()) 
    {
      cout << cl->number() << " input []\n";
      return;
    };
  const VampireKernel::Clause::Ancestor* anc;
  for (anc = cl->firstAncestor();
       anc;
       anc = anc->next())
    {
      outputInferenceStructure(anc->clause());
    };
  
  cout << cl->number() << " [";
  
  for (anc = cl->firstAncestor();
       anc;
       anc = anc->next())
    {
      cout << anc->clause()->number();
      if (anc->next()) cout << ",";
    };

  cout << "]\n";

}; // void outputInferenceStructure(const VampireKernel::Clause* cl)
#endif


//======================================================================
