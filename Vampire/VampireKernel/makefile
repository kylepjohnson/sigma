# File:    makefile 
# Author:  Alexandre Riazanov (riazanov@cs.man.ac.uk)
# Created: Feb 8, 2000
# Purpose: makefile for VampireKernel
################################################################

CC= g++3 #      	# UNIX GNU compiler 
OPT=-O4 -DNO_DEBUG -DLARGE_TERMS# default compilation options
ADD_OPT=
MDOPT=  # default makedepend options
CFLAGS = -Wall -IPR_PARSER -I../BlodKorv -I../Gematogen -static
######################################################################

Obj = AutoFunctorPrecedence.o\
      ClauseBackground.o\
      binary_resolution.o backward_superposition.o BackDemodulation.o BackParaCandidates.o BackwardSubsumption.o BSIndex.o BSMachine.o\
      BuiltInFloatingPointArithmetic.o BuiltInTheory.o BuiltInTheoryDispatcher.o\
      Clause.o ClauseNumIndex.o ClauseQueue.o ComponentName.o ClauseAssembler.o\
      forward_subsumption.o forward_superposition.o ForwardDemodulation.o fs_code.o fs_command.o Flatterm.o \
      VampireKernelInterceptMacros.o\
      InferenceTree.o InferenceDispatcher.o InstanceRetrievalForDemodulation.o iu_command.o iu_machine.o\
      kept.o\
      LiteralList.o LiteralOrdering.o LiteralSelection.o LiteralSharing.o LocalUnification.o Logging.o\
      Master.o Master3.o\
      NewClause.o NonrecursiveKBOrdering.o NonuniformConstantTermWeighting.o\
      old_fs_code.o or_index.o or_premise.o\
      PagedPassiveClauseStorage.o prefix.o premise_factoring.o ProblemProfile.o\
      sharing.o Signature.o SimplificationOrdering.o Splitting.o Splitting3.o StandardKBOrdering.o\
      Tabulation.o TermWeighting.o TmpLitList.o tmp_literal.o Term.o\
      UniformTermWeighting.o\
      VampireKernel.o variables.o\
      WeightPolynomial.o weight_formula.o\
      WeightPolynomialSpecialisedComparisonCommand.o

BlodKorvObj = ../BlodKorv/BitWord.o\
        ../BlodKorv/ClassDesc.o\
        ../BlodKorv/Debug.o\
        ../BlodKorv/DestructionMode.o\
        ../BlodKorv/Exit.o\
	../BlodKorv/FormatText.o\
        ../BlodKorv/GlobalClock.o\
        ../BlodKorv/GlobAlloc.o\
	../BlodKorv/GlobalStopFlag.o\
        ../BlodKorv/Malloc.o\
        ../BlodKorv/RuntimeError.o
PPObj = PR_PARSER/pr_parser.o

################################################################

default: standalone

libvkernel.a: ${Obj} blodKorv
	ar -rs libvkernel.a ${Obj} ${BlodKorvObj}

standalone: blodKorv libvkernel.a prParser Standalone.o Parser.o Input.o
	${CC} ${OPT} ${CFLAGS} ../BlodKorv/CommandLine.o Standalone.o  ${PPObj}  Parser.o Input.o libvkernel.a -o libvkernel
#	strip -s libvkernel

.SUFFIXES:
.SUFFIXES: .cpp .o

.cpp.o:
	${CC} ${OPT} ${CFLAGS} ${ADD_OPT} -c $*.cpp


blodKorv:
	cd ../BlodKorv ; ${MAKE} CC='${CC}' OPT='${OPT}' ; cd ../VampireKernel

prParser:
	cd PR_PARSER ; ${MAKE} CC='${CC}' ; cd ..

remake: clean standalone

clean: 
	rm -f *.o *~ *.bak *.tmp libvkernel libvkernel.a core
	rm -f ./VKStandalone_VSProject/{Debug,Release}/*.{exe,obj,pch,idb,sbr,pdb}
	rm -f ./VKLibrary_VSProject/{Debug,Release}/*.{exe,obj,pch,idb,lib,sbr,pdb}
	cd ../BlodKorv ; ${MAKE} clean ; cd ../VampireKernel
	cd PR_PARSER ; ${MAKE} clean ; cd ..

depend:
	makedepend ${MDOPT} -I../BlodKorv -IPR_PARSER  -I../Gematogen *.cpp 
	cd ../BlodKorv;  makedepend ${MDOPT}  *.cpp ; cd ../VampireKernel

#############################################################################
# DO NOT DELETE

AutoFunctorPrecedence.o: AutoFunctorPrecedence.hpp ../BlodKorv/jargon.hpp
AutoFunctorPrecedence.o: VampireKernelDebugFlags.hpp
AutoFunctorPrecedence.o: ../BlodKorv/BlodKorvDebugFlags.hpp
AutoFunctorPrecedence.o: ../BlodKorv/debugMacros.hpp ../BlodKorv/Debug.hpp
AutoFunctorPrecedence.o: ../BlodKorv/ClassDesc.hpp ../BlodKorv/ObjDesc.hpp
AutoFunctorPrecedence.o: ../BlodKorv/AllocationFailureHandling.hpp
AutoFunctorPrecedence.o: ../BlodKorv/Exit.hpp ../BlodKorv/RuntimeError.hpp
AutoFunctorPrecedence.o: ../BlodKorv/Allocator.hpp
AutoFunctorPrecedence.o: ../BlodKorv/BufferedAllocationStatistics.hpp
AutoFunctorPrecedence.o: ../BlodKorv/Tuple.hpp ../BlodKorv/GSortTree.hpp
AutoFunctorPrecedence.o: ../BlodKorv/GList.hpp
BSIndex.o: BSIndex.hpp ../BlodKorv/jargon.hpp VampireKernelDebugFlags.hpp
BSIndex.o: ../BlodKorv/BlodKorvDebugFlags.hpp VampireKernelConst.hpp
BSIndex.o: ../BlodKorv/ClassDesc.hpp ../BlodKorv/Debug.hpp
BSIndex.o: ../BlodKorv/ObjDesc.hpp ../BlodKorv/AllocationFailureHandling.hpp
BSIndex.o: ../BlodKorv/Exit.hpp ../BlodKorv/RuntimeError.hpp
BSIndex.o: ../BlodKorv/DestructionMode.hpp ../BlodKorv/debugMacros.hpp
BSIndex.o: ../BlodKorv/Allocator.hpp
BSIndex.o: ../BlodKorv/BufferedAllocationStatistics.hpp
BSIndex.o: ../BlodKorv/Comparison.hpp ../BlodKorv/Stack.hpp
BSIndex.o: ../BlodKorv/ExpandingStack.hpp ../BlodKorv/Array.hpp
BSIndex.o: ../BlodKorv/BitSet.hpp ../BlodKorv/BitWord.hpp
BSIndex.o: ../BlodKorv/BitString.hpp ../BlodKorv/SmallHeaderSkipList.hpp
BSIndex.o: AutoFunctorPrecedence.hpp ../BlodKorv/Tuple.hpp
BSIndex.o: ../BlodKorv/GSortTree.hpp ../BlodKorv/GList.hpp
BSIndex.o: ../BlodKorv/SkipList.hpp ../Gematogen/PathIndexWithOpenPaths.hpp
BSIndex.o: ../Gematogen/GematogenDebugFlags.hpp
BSMachine.o: BSMachine.hpp ../BlodKorv/jargon.hpp VampireKernelDebugFlags.hpp
BSMachine.o: ../BlodKorv/BlodKorvDebugFlags.hpp VampireKernelConst.hpp
BSMachine.o: ../BlodKorv/Debug.hpp ../BlodKorv/ObjDesc.hpp
BSMachine.o: ../BlodKorv/AllocationFailureHandling.hpp ../BlodKorv/Exit.hpp
BSMachine.o: ../BlodKorv/RuntimeError.hpp ../BlodKorv/DestructionMode.hpp
BSMachine.o: ../BlodKorv/debugMacros.hpp ../BlodKorv/Allocator.hpp
BSMachine.o: ../BlodKorv/BufferedAllocationStatistics.hpp
BSMachine.o: ../BlodKorv/GlobalStopFlag.hpp ../BlodKorv/Array.hpp
BSMachine.o: ../BlodKorv/GIncCountingSort.hpp ../BlodKorv/Stack.hpp
BSMachine.o: ../BlodKorv/ExpandingStack.hpp BSIndex.hpp Term.hpp
BSMachine.o: ../BlodKorv/Comparison.hpp ../BlodKorv/BitSet.hpp
BSMachine.o: ../BlodKorv/BitWord.hpp ../BlodKorv/BitString.hpp
BSMachine.o: ../BlodKorv/SmallHeaderSkipList.hpp TermWeightType.hpp
BSMachine.o: ../BlodKorv/Tuple.hpp ../BlodKorv/GSortTree.hpp
BSMachine.o: ../BlodKorv/GList.hpp ../BlodKorv/SkipList.hpp
BSMachine.o: ../Gematogen/PathIndexWithOpenPaths.hpp
BSMachine.o: ../Gematogen/GematogenDebugFlags.hpp Clause.hpp
BSMachine.o: ClauseBackground.hpp LiteralList.hpp DefinedHeaderTable.hpp
BSMachine.o: ../BlodKorv/StaticCellArray.hpp
BackDemodulation.o: BackDemodulation.hpp ../BlodKorv/jargon.hpp
BackDemodulation.o: ../BlodKorv/BlodKorvDebugFlags.hpp
BackDemodulation.o: ../BlodKorv/ObjDesc.hpp
BackDemodulation.o: ../BlodKorv/AllocationFailureHandling.hpp
BackDemodulation.o: ../BlodKorv/Exit.hpp ../BlodKorv/RuntimeError.hpp
BackDemodulation.o: ../BlodKorv/DestructionMode.hpp
BackDemodulation.o: ../BlodKorv/debugMacros.hpp ../BlodKorv/Allocator.hpp
BackDemodulation.o: ../BlodKorv/BufferedAllocationStatistics.hpp
BackDemodulation.o: VampireKernelDebugFlags.hpp
BackDemodulation.o: ../BlodKorv/ExpandingMultiset.hpp
BackDemodulation.o: ../BlodKorv/ExpandingStack.hpp ../BlodKorv/Array.hpp
BackDemodulation.o: ../BlodKorv/Comparison.hpp ../BlodKorv/Stack.hpp
BackDemodulation.o: ../BlodKorv/BitSet.hpp ../BlodKorv/BitWord.hpp
BackDemodulation.o: ../BlodKorv/BitString.hpp
BackDemodulation.o: ../BlodKorv/SmallHeaderSkipList.hpp TermWeightType.hpp
BackDemodulation.o: AutoFunctorPrecedence.hpp ../BlodKorv/Tuple.hpp
BackDemodulation.o: ../BlodKorv/GSortTree.hpp ../BlodKorv/GList.hpp
BackDemodulation.o: NewClause.hpp prefix.hpp variables.hpp
BackDemodulation.o: ../BlodKorv/MultisetOfVariables.hpp WeightPolynomial.hpp
BackDemodulation.o: ../BlodKorv/LinearPolynomial.hpp
BackDemodulation.o: ../BlodKorv/LinearCombination.hpp ../BlodKorv/Math.hpp
BackDemodulation.o: ../BlodKorv/SharedLinearPolynomial.hpp
BackDemodulation.o: ../BlodKorv/GDiscTree.hpp OpenNewClause.hpp Flatterm.hpp
BackDemodulation.o: tmp_literal.hpp ../BlodKorv/Multiset.hpp
BackDemodulation.o: TermWeighting.hpp InstanceRetrievalForDemodulation.hpp
BackDemodulation.o: ../Gematogen/PathIndexingWithJoinsAndEarlyCleanUpForInstanceRetrieval.hpp
BackDemodulation.o: ../BlodKorv/GlobalStopFlag.hpp ../Gematogen/Gematogen.hpp
BackDemodulation.o: ../Gematogen/GematogenDebugFlags.hpp
BackDemodulation.o: ../Gematogen/PathIndex.hpp
BackDemodulation.o: ../BlodKorv/CompactNodeSmallHeaderSkipList.hpp
BackDemodulation.o: ../BlodKorv/GIncCountingSort.hpp
BackDemodulation.o: SimplificationOrdering.hpp TmpLitList.hpp
BackDemodulation.o: ClauseBackground.hpp ForwardDemodulation.hpp
BackDemodulation.o: VampireKernelInterceptMacros.hpp
BackDemodulation.o: ../Gematogen/ForwardMatchingIndexTree.hpp
BackDemodulation.o: ../BlodKorv/EquivalenceRelation.hpp
BackDemodulation.o: ../Gematogen/ForwardMatchingIndex.hpp
BackDemodulation.o: ForwardMatchingSubstitutionCache.hpp ClauseQueue.hpp
BackDemodulation.o: ../BlodKorv/FIFO.hpp Clause.hpp LiteralList.hpp
BackDemodulation.o: DefinedHeaderTable.hpp PagedPassiveClauseStorage.hpp
BackDemodulation.o: ../BlodKorv/PageQueueDisciplineStorage.hpp Splitting.hpp
BackDemodulation.o: DivIntoComp.hpp ClauseNumIndex.hpp
BackDemodulation.o: ../BlodKorv/SkipList.hpp
BackParaCandidates.o: BackParaCandidates.hpp ../BlodKorv/jargon.hpp
BackParaCandidates.o: VampireKernelDebugFlags.hpp
BackParaCandidates.o: ../BlodKorv/BlodKorvDebugFlags.hpp
BackParaCandidates.o: ../BlodKorv/ObjDesc.hpp
BackParaCandidates.o: ../BlodKorv/AllocationFailureHandling.hpp
BackParaCandidates.o: ../BlodKorv/Exit.hpp ../BlodKorv/RuntimeError.hpp
BackParaCandidates.o: ../BlodKorv/DestructionMode.hpp
BackParaCandidates.o: ../BlodKorv/debugMacros.hpp ../BlodKorv/Allocator.hpp
BackParaCandidates.o: ../BlodKorv/BufferedAllocationStatistics.hpp
BackParaCandidates.o: ../BlodKorv/SkipList.hpp ../BlodKorv/GList.hpp
BackParaCandidates.o: ../BlodKorv/Tuple.hpp SimplificationOrdering.hpp
BackParaCandidates.o: ../BlodKorv/Comparison.hpp WeightPolynomial.hpp
BackParaCandidates.o: ../BlodKorv/LinearPolynomial.hpp
BackParaCandidates.o: ../BlodKorv/LinearCombination.hpp ../BlodKorv/Stack.hpp
BackParaCandidates.o: ../BlodKorv/Math.hpp
BackParaCandidates.o: ../BlodKorv/SharedLinearPolynomial.hpp
BackParaCandidates.o: ../BlodKorv/GDiscTree.hpp TermWeightType.hpp
BackParaCandidates.o: ../BlodKorv/ExpandingStack.hpp ../BlodKorv/Array.hpp
BackParaCandidates.o: ../BlodKorv/BitSet.hpp ../BlodKorv/BitWord.hpp
BackParaCandidates.o: ../BlodKorv/BitString.hpp
BackParaCandidates.o: ../BlodKorv/SmallHeaderSkipList.hpp Signature.hpp
BackParaCandidates.o: AutoFunctorPrecedence.hpp ../BlodKorv/GSortTree.hpp
BackParaCandidates.o: InstanceRetrievalForDemodulation.hpp
BackParaCandidates.o: ../Gematogen/PathIndexingWithJoinsAndEarlyCleanUpForInstanceRetrieval.hpp
BackParaCandidates.o: ../BlodKorv/GlobalStopFlag.hpp
BackParaCandidates.o: ../Gematogen/Gematogen.hpp
BackParaCandidates.o: ../Gematogen/GematogenDebugFlags.hpp
BackParaCandidates.o: ../Gematogen/PathIndex.hpp
BackParaCandidates.o: ../BlodKorv/CompactNodeSmallHeaderSkipList.hpp
BackParaCandidates.o: ../BlodKorv/GIncCountingSort.hpp
BackParaCandidates.o: ../BlodKorv/Multiset.hpp
BackwardSubsumption.o: BackwardSubsumption.hpp ../BlodKorv/jargon.hpp
BackwardSubsumption.o: VampireKernelDebugFlags.hpp
BackwardSubsumption.o: ../BlodKorv/BlodKorvDebugFlags.hpp
BackwardSubsumption.o: ../BlodKorv/DestructionMode.hpp
BackwardSubsumption.o: ../BlodKorv/debugMacros.hpp ../BlodKorv/Debug.hpp
BackwardSubsumption.o: ../BlodKorv/GlobAlloc.hpp ../BlodKorv/ClassDesc.hpp
BackwardSubsumption.o: ../BlodKorv/ObjDesc.hpp
BackwardSubsumption.o: ../BlodKorv/AllocationFailureHandling.hpp
BackwardSubsumption.o: ../BlodKorv/RuntimeError.hpp ../BlodKorv/Allocator.hpp
BackwardSubsumption.o: ../BlodKorv/BufferedAllocationStatistics.hpp
BackwardSubsumption.o: ../BlodKorv/Comparison.hpp SymbolInfo.hpp
BackwardSubsumption.o: AutoFunctorPrecedence.hpp ../BlodKorv/Tuple.hpp
BackwardSubsumption.o: ../BlodKorv/GSortTree.hpp ../BlodKorv/GList.hpp
BackwardSubsumption.o: TermWeightType.hpp VampireKernel.hpp
BackwardSubsumption.o: ../BlodKorv/Stack.hpp ../BlodKorv/ExpandingStack.hpp
BackwardSubsumption.o: ../BlodKorv/BitSet.hpp ../BlodKorv/BitWord.hpp
BackwardSubsumption.o: ../BlodKorv/BitString.hpp
BackwardSubsumption.o: ../BlodKorv/SmallHeaderSkipList.hpp
BackwardSubsumption.o: ../BlodKorv/SkipList.hpp
BackwardSubsumption.o: ../Gematogen/PathIndexWithOpenPaths.hpp
BackwardSubsumption.o: ../Gematogen/GematogenDebugFlags.hpp BSMachine.hpp
BackwardSubsumption.o: ../BlodKorv/GlobalStopFlag.hpp
BackwardSubsumption.o: ../BlodKorv/GIncCountingSort.hpp Clause.hpp
BackwardSubsumption.o: ClauseBackground.hpp LiteralList.hpp
BackwardSubsumption.o: DefinedHeaderTable.hpp ../BlodKorv/StaticCellArray.hpp
BackwardSubsumption.o: ClauseNumIndex.hpp SubsumptionCheck.hpp
BackwardSubsumption.o: SimpleSubstitution.hpp
BuiltInFloatingPointArithmetic.o: ../BlodKorv/GlobAlloc.hpp
BuiltInFloatingPointArithmetic.o: ../BlodKorv/BlodKorvDebugFlags.hpp
BuiltInFloatingPointArithmetic.o: ../BlodKorv/ClassDesc.hpp
BuiltInFloatingPointArithmetic.o: ../BlodKorv/Debug.hpp
BuiltInFloatingPointArithmetic.o: ../BlodKorv/ObjDesc.hpp
BuiltInFloatingPointArithmetic.o: ../BlodKorv/AllocationFailureHandling.hpp
BuiltInFloatingPointArithmetic.o: ../BlodKorv/Exit.hpp
BuiltInFloatingPointArithmetic.o: ../BlodKorv/RuntimeError.hpp
BuiltInFloatingPointArithmetic.o: ../BlodKorv/DestructionMode.hpp
BuiltInFloatingPointArithmetic.o: ../BlodKorv/debugMacros.hpp
BuiltInFloatingPointArithmetic.o: ../BlodKorv/Allocator.hpp
BuiltInFloatingPointArithmetic.o: ../BlodKorv/BufferedAllocationStatistics.hpp
BuiltInFloatingPointArithmetic.o: BuiltInFloatingPointArithmetic.hpp
BuiltInFloatingPointArithmetic.o: VampireKernelDebugFlags.hpp
BuiltInFloatingPointArithmetic.o: ../BlodKorv/Array.hpp Clause.hpp
BuiltInFloatingPointArithmetic.o: ../BlodKorv/GList.hpp
BuiltInFloatingPointArithmetic.o: ../BlodKorv/BitWord.hpp Term.hpp
BuiltInFloatingPointArithmetic.o: VampireKernelConst.hpp
BuiltInFloatingPointArithmetic.o: ../BlodKorv/Comparison.hpp
BuiltInFloatingPointArithmetic.o: ../BlodKorv/Stack.hpp
BuiltInFloatingPointArithmetic.o: ../BlodKorv/ExpandingStack.hpp
BuiltInFloatingPointArithmetic.o: ../BlodKorv/BitSet.hpp
BuiltInFloatingPointArithmetic.o: ../BlodKorv/BitString.hpp
BuiltInFloatingPointArithmetic.o: ../BlodKorv/SmallHeaderSkipList.hpp
BuiltInFloatingPointArithmetic.o: TermWeightType.hpp VampireKernel.hpp
BuiltInFloatingPointArithmetic.o: SymbolInfo.hpp AutoFunctorPrecedence.hpp
BuiltInFloatingPointArithmetic.o: ../BlodKorv/Tuple.hpp
BuiltInFloatingPointArithmetic.o: ../BlodKorv/GSortTree.hpp
BuiltInFloatingPointArithmetic.o: ClauseBackground.hpp LiteralList.hpp
BuiltInFloatingPointArithmetic.o: DefinedHeaderTable.hpp Flatterm.hpp
BuiltInFloatingPointArithmetic.o: ../BlodKorv/MultisetOfVariables.hpp
BuiltInFloatingPointArithmetic.o: variables.hpp WeightPolynomial.hpp
BuiltInFloatingPointArithmetic.o: ../BlodKorv/LinearPolynomial.hpp
BuiltInFloatingPointArithmetic.o: ../BlodKorv/LinearCombination.hpp
BuiltInFloatingPointArithmetic.o: ../BlodKorv/Math.hpp
BuiltInFloatingPointArithmetic.o: ../BlodKorv/SharedLinearPolynomial.hpp
BuiltInFloatingPointArithmetic.o: ../BlodKorv/GDiscTree.hpp tmp_literal.hpp
BuiltInFloatingPointArithmetic.o: ../BlodKorv/Multiset.hpp TermWeighting.hpp
BuiltInFloatingPointArithmetic.o: InstanceRetrievalForDemodulation.hpp
BuiltInFloatingPointArithmetic.o: ../Gematogen/PathIndexingWithJoinsAndEarlyCleanUpForInstanceRetrieval.hpp
BuiltInFloatingPointArithmetic.o: ../BlodKorv/GlobalStopFlag.hpp
BuiltInFloatingPointArithmetic.o: ../Gematogen/Gematogen.hpp
BuiltInFloatingPointArithmetic.o: ../Gematogen/GematogenDebugFlags.hpp
BuiltInFloatingPointArithmetic.o: ../Gematogen/PathIndex.hpp
BuiltInFloatingPointArithmetic.o: ../BlodKorv/CompactNodeSmallHeaderSkipList.hpp
BuiltInFloatingPointArithmetic.o: ../BlodKorv/GIncCountingSort.hpp
BuiltInFloatingPointArithmetic.o: SimplificationOrdering.hpp
BuiltInTheory.o: ../BlodKorv/RuntimeError.hpp ../BlodKorv/jargon.hpp
BuiltInTheory.o: VampireKernelDebugFlags.hpp
BuiltInTheory.o: ../BlodKorv/BlodKorvDebugFlags.hpp
BuiltInTheory.o: ../BlodKorv/debugMacros.hpp ../BlodKorv/Debug.hpp
BuiltInTheoryDispatcher.o: ../BlodKorv/BlodKorvDebugFlags.hpp
BuiltInTheoryDispatcher.o: ../BlodKorv/ObjDesc.hpp
BuiltInTheoryDispatcher.o: ../BlodKorv/AllocationFailureHandling.hpp
BuiltInTheoryDispatcher.o: ../BlodKorv/Exit.hpp ../BlodKorv/RuntimeError.hpp
BuiltInTheoryDispatcher.o: ../BlodKorv/DestructionMode.hpp
BuiltInTheoryDispatcher.o: ../BlodKorv/debugMacros.hpp
BuiltInTheoryDispatcher.o: ../BlodKorv/Allocator.hpp
BuiltInTheoryDispatcher.o: ../BlodKorv/BufferedAllocationStatistics.hpp
BuiltInTheoryDispatcher.o: BuiltInTheoryDispatcher.hpp ../BlodKorv/GList.hpp
BuiltInTheoryDispatcher.o: Clause.hpp ../BlodKorv/BitWord.hpp Term.hpp
BuiltInTheoryDispatcher.o: VampireKernelConst.hpp ../BlodKorv/Comparison.hpp
BuiltInTheoryDispatcher.o: ../BlodKorv/Stack.hpp
BuiltInTheoryDispatcher.o: ../BlodKorv/ExpandingStack.hpp
BuiltInTheoryDispatcher.o: ../BlodKorv/Array.hpp ../BlodKorv/BitSet.hpp
BuiltInTheoryDispatcher.o: ../BlodKorv/BitString.hpp
BuiltInTheoryDispatcher.o: ../BlodKorv/SmallHeaderSkipList.hpp
BuiltInTheoryDispatcher.o: TermWeightType.hpp VampireKernel.hpp
BuiltInTheoryDispatcher.o: AutoFunctorPrecedence.hpp ../BlodKorv/Tuple.hpp
BuiltInTheoryDispatcher.o: ../BlodKorv/GSortTree.hpp ClauseBackground.hpp
BuiltInTheoryDispatcher.o: LiteralList.hpp DefinedHeaderTable.hpp
BuiltInTheoryDispatcher.o: BuiltInFloatingPointArithmetic.hpp
BuiltInTheoryDispatcher.o: BuiltInTheory.hpp Flatterm.hpp
BuiltInTheoryDispatcher.o: ../BlodKorv/MultisetOfVariables.hpp variables.hpp
BuiltInTheoryDispatcher.o: WeightPolynomial.hpp
BuiltInTheoryDispatcher.o: ../BlodKorv/LinearPolynomial.hpp
BuiltInTheoryDispatcher.o: ../BlodKorv/LinearCombination.hpp
BuiltInTheoryDispatcher.o: ../BlodKorv/Math.hpp
BuiltInTheoryDispatcher.o: ../BlodKorv/SharedLinearPolynomial.hpp
BuiltInTheoryDispatcher.o: ../BlodKorv/GDiscTree.hpp
Clause.o: ../BlodKorv/jargon.hpp VampireKernelDebugFlags.hpp
Clause.o: ../BlodKorv/BlodKorvDebugFlags.hpp ../BlodKorv/GlobAlloc.hpp
Clause.o: ../BlodKorv/Debug.hpp ../BlodKorv/ObjDesc.hpp
Clause.o: ../BlodKorv/AllocationFailureHandling.hpp ../BlodKorv/Exit.hpp
Clause.o: ../BlodKorv/RuntimeError.hpp ../BlodKorv/DestructionMode.hpp
Clause.o: ../BlodKorv/debugMacros.hpp ../BlodKorv/Allocator.hpp
Clause.o: ../BlodKorv/BufferedAllocationStatistics.hpp ../BlodKorv/GList.hpp
Clause.o: ../BlodKorv/BitWord.hpp Term.hpp VampireKernelConst.hpp
Clause.o: ../BlodKorv/Comparison.hpp ../BlodKorv/Stack.hpp
Clause.o: ../BlodKorv/ExpandingStack.hpp ../BlodKorv/Array.hpp
Clause.o: ../BlodKorv/BitSet.hpp ../BlodKorv/BitString.hpp
Clause.o: ../BlodKorv/SmallHeaderSkipList.hpp TermWeightType.hpp
Clause.o: ../BlodKorv/Tuple.hpp ../BlodKorv/GSortTree.hpp
Clause.o: ClauseBackground.hpp LiteralList.hpp DefinedHeaderTable.hpp
Clause.o: VampireKernelInterceptMacros.hpp
ClauseAssembler.o: ClauseAssembler.hpp ../BlodKorv/jargon.hpp
ClauseAssembler.o: VampireKernelDebugFlags.hpp
ClauseAssembler.o: ../BlodKorv/BlodKorvDebugFlags.hpp LiteralSharing.hpp
ClauseAssembler.o: ../BlodKorv/ObjDesc.hpp
ClauseAssembler.o: ../BlodKorv/AllocationFailureHandling.hpp
ClauseAssembler.o: ../BlodKorv/Exit.hpp ../BlodKorv/RuntimeError.hpp
ClauseAssembler.o: ../BlodKorv/DestructionMode.hpp
ClauseAssembler.o: ../BlodKorv/debugMacros.hpp ../BlodKorv/Allocator.hpp
ClauseAssembler.o: ../BlodKorv/BufferedAllocationStatistics.hpp
ClauseAssembler.o: VampireKernelConst.hpp ../BlodKorv/Comparison.hpp
ClauseAssembler.o: ../BlodKorv/Stack.hpp ../BlodKorv/ExpandingStack.hpp
ClauseAssembler.o: ../BlodKorv/BitSet.hpp ../BlodKorv/BitWord.hpp
ClauseAssembler.o: ../BlodKorv/BitString.hpp
ClauseAssembler.o: ../BlodKorv/SmallHeaderSkipList.hpp TermWeightType.hpp
ClauseAssembler.o: AutoFunctorPrecedence.hpp ../BlodKorv/Tuple.hpp
ClauseAssembler.o: ../BlodKorv/GSortTree.hpp ../BlodKorv/GList.hpp
ClauseAssembler.o: variables.hpp ../BlodKorv/MultisetOfVariables.hpp
ClauseAssembler.o: WeightPolynomial.hpp ../BlodKorv/LinearPolynomial.hpp
ClauseAssembler.o: ../BlodKorv/LinearCombination.hpp ../BlodKorv/Math.hpp
ClauseAssembler.o: ../BlodKorv/SharedLinearPolynomial.hpp
ClauseAssembler.o: ../BlodKorv/GDiscTree.hpp LiteralList.hpp sharing.hpp
ClauseAssembler.o: VampireKernelInterceptMacros.hpp dtree.hpp
ClauseAssembler.o: LiteralSelection.hpp TmpLitList.hpp tmp_literal.hpp
ClauseAssembler.o: ../BlodKorv/Multiset.hpp TermWeighting.hpp Flatterm.hpp
ClauseAssembler.o: InstanceRetrievalForDemodulation.hpp
ClauseAssembler.o: ../Gematogen/PathIndexingWithJoinsAndEarlyCleanUpForInstanceRetrieval.hpp
ClauseAssembler.o: ../BlodKorv/GlobalStopFlag.hpp ../Gematogen/Gematogen.hpp
ClauseAssembler.o: ../Gematogen/GematogenDebugFlags.hpp
ClauseAssembler.o: ../Gematogen/PathIndex.hpp
ClauseAssembler.o: ../BlodKorv/CompactNodeSmallHeaderSkipList.hpp
ClauseAssembler.o: ../BlodKorv/GIncCountingSort.hpp
ClauseAssembler.o: SimplificationOrdering.hpp LiteralOrdering.hpp Clause.hpp
ClauseAssembler.o: ClauseBackground.hpp DefinedHeaderTable.hpp
ClauseAssembler.o: PagedPassiveClauseStorage.hpp
ClauseAssembler.o: ../BlodKorv/PageQueueDisciplineStorage.hpp
ClauseBackground.o: ../BlodKorv/jargon.hpp VampireKernelDebugFlags.hpp
ClauseBackground.o: ../BlodKorv/BlodKorvDebugFlags.hpp
ClauseBackground.o: ../BlodKorv/Debug.hpp ../BlodKorv/GlobAlloc.hpp
ClauseBackground.o: ../BlodKorv/AllocationFailureHandling.hpp
ClauseBackground.o: ../BlodKorv/Exit.hpp ../BlodKorv/RuntimeError.hpp
ClauseBackground.o: ../BlodKorv/DestructionMode.hpp
ClauseBackground.o: ../BlodKorv/debugMacros.hpp ../BlodKorv/Allocator.hpp
ClauseBackground.o: ../BlodKorv/BufferedAllocationStatistics.hpp
ClauseBackground.o: ../BlodKorv/ExpandingStack.hpp ../BlodKorv/Array.hpp
ClauseBackground.o: ../BlodKorv/GList.hpp
ClauseNumIndex.o: ClauseNumIndex.hpp ../BlodKorv/jargon.hpp
ClauseNumIndex.o: VampireKernelDebugFlags.hpp
ClauseNumIndex.o: ../BlodKorv/BlodKorvDebugFlags.hpp ../BlodKorv/SkipList.hpp
ClauseNumIndex.o: ../BlodKorv/debugMacros.hpp ../BlodKorv/Debug.hpp
ClauseNumIndex.o: ../BlodKorv/AllocationFailureHandling.hpp
ClauseNumIndex.o: ../BlodKorv/Exit.hpp ../BlodKorv/RuntimeError.hpp
ClauseNumIndex.o: ../BlodKorv/Allocator.hpp
ClauseNumIndex.o: ../BlodKorv/BufferedAllocationStatistics.hpp Clause.hpp
ClauseNumIndex.o: ../BlodKorv/GList.hpp ../BlodKorv/BitWord.hpp Term.hpp
ClauseNumIndex.o: VampireKernelConst.hpp ../BlodKorv/Comparison.hpp
ClauseNumIndex.o: ../BlodKorv/Stack.hpp ../BlodKorv/ExpandingStack.hpp
ClauseNumIndex.o: ../BlodKorv/Array.hpp ../BlodKorv/BitSet.hpp
ClauseNumIndex.o: ../BlodKorv/BitString.hpp
ClauseNumIndex.o: ../BlodKorv/SmallHeaderSkipList.hpp TermWeightType.hpp
ClauseNumIndex.o: ../BlodKorv/Tuple.hpp ../BlodKorv/GSortTree.hpp
ClauseNumIndex.o: ClauseBackground.hpp LiteralList.hpp DefinedHeaderTable.hpp
ComponentName.o: VampireKernelDebugFlags.hpp
ComponentName.o: ../BlodKorv/BlodKorvDebugFlags.hpp VampireKernelConst.hpp
ComponentName.o: ../BlodKorv/MultisetOfVariables.hpp ../BlodKorv/Stack.hpp
ComponentName.o: ../BlodKorv/debugMacros.hpp ../BlodKorv/Debug.hpp
ComponentName.o: ../BlodKorv/Comparison.hpp
ComponentName.o: ../BlodKorv/EquivalenceRelation.hpp fs_command.hpp
ComponentName.o: ../BlodKorv/ClassDesc.hpp ../BlodKorv/ObjDesc.hpp
ComponentName.o: ../BlodKorv/AllocationFailureHandling.hpp
ComponentName.o: ../BlodKorv/Exit.hpp ../BlodKorv/RuntimeError.hpp
ComponentName.o: ../BlodKorv/DestructionMode.hpp ../BlodKorv/Allocator.hpp
ComponentName.o: ../BlodKorv/BufferedAllocationStatistics.hpp Term.hpp
ComponentName.o: ../BlodKorv/ExpandingStack.hpp ../BlodKorv/Array.hpp
ComponentName.o: ../BlodKorv/BitSet.hpp ../BlodKorv/BitWord.hpp
ComponentName.o: ../BlodKorv/BitString.hpp
ComponentName.o: ../BlodKorv/SmallHeaderSkipList.hpp TermWeightType.hpp
ComponentName.o: ../BlodKorv/Tuple.hpp ../BlodKorv/GSortTree.hpp
ComponentName.o: ../BlodKorv/GList.hpp old_fs_code.hpp ../BlodKorv/Queue.hpp
ComponentName.o: TmpLitList.hpp tmp_literal.hpp ../BlodKorv/Multiset.hpp
ComponentName.o: WeightPolynomial.hpp ../BlodKorv/LinearPolynomial.hpp
ComponentName.o: ../BlodKorv/LinearCombination.hpp ../BlodKorv/Math.hpp
ComponentName.o: ../BlodKorv/SharedLinearPolynomial.hpp
ComponentName.o: ../BlodKorv/GDiscTree.hpp TermWeighting.hpp Flatterm.hpp
ComponentName.o: variables.hpp InstanceRetrievalForDemodulation.hpp
ComponentName.o: ../Gematogen/PathIndexingWithJoinsAndEarlyCleanUpForInstanceRetrieval.hpp
ComponentName.o: ../BlodKorv/GlobalStopFlag.hpp ../Gematogen/Gematogen.hpp
ComponentName.o: ../Gematogen/GematogenDebugFlags.hpp
ComponentName.o: ../Gematogen/PathIndex.hpp
ComponentName.o: ../BlodKorv/CompactNodeSmallHeaderSkipList.hpp
ComponentName.o: ../BlodKorv/GIncCountingSort.hpp SimplificationOrdering.hpp
ComponentName.o: ../BlodKorv/ObjectPool.hpp
Flatterm.o: VampireKernelDebugFlags.hpp ../BlodKorv/BlodKorvDebugFlags.hpp
Flatterm.o: ../BlodKorv/ClassDesc.hpp ../BlodKorv/Debug.hpp
Flatterm.o: ../BlodKorv/ObjDesc.hpp ../BlodKorv/AllocationFailureHandling.hpp
Flatterm.o: ../BlodKorv/Exit.hpp ../BlodKorv/RuntimeError.hpp
Flatterm.o: ../BlodKorv/DestructionMode.hpp ../BlodKorv/debugMacros.hpp
Flatterm.o: ../BlodKorv/Allocator.hpp
Flatterm.o: ../BlodKorv/BufferedAllocationStatistics.hpp
Flatterm.o: ../BlodKorv/Comparison.hpp ../BlodKorv/Stack.hpp
Flatterm.o: ../BlodKorv/ExpandingStack.hpp ../BlodKorv/Array.hpp
Flatterm.o: ../BlodKorv/BitSet.hpp ../BlodKorv/BitWord.hpp
Flatterm.o: ../BlodKorv/BitString.hpp ../BlodKorv/SmallHeaderSkipList.hpp
Flatterm.o: AutoFunctorPrecedence.hpp ../BlodKorv/Tuple.hpp
Flatterm.o: ../BlodKorv/GSortTree.hpp ../BlodKorv/GList.hpp
Flatterm.o: ../BlodKorv/MultisetOfVariables.hpp variables.hpp
Flatterm.o: WeightPolynomial.hpp ../BlodKorv/LinearPolynomial.hpp
Flatterm.o: ../BlodKorv/LinearCombination.hpp ../BlodKorv/Math.hpp
Flatterm.o: ../BlodKorv/SharedLinearPolynomial.hpp ../BlodKorv/GDiscTree.hpp
ForwardDemodulation.o: ForwardDemodulation.hpp ../BlodKorv/jargon.hpp
ForwardDemodulation.o: VampireKernelDebugFlags.hpp
ForwardDemodulation.o: ../BlodKorv/BlodKorvDebugFlags.hpp
ForwardDemodulation.o: ../BlodKorv/DestructionMode.hpp
ForwardDemodulation.o: ../BlodKorv/debugMacros.hpp ../BlodKorv/Debug.hpp
ForwardDemodulation.o: ../BlodKorv/GlobAlloc.hpp ../BlodKorv/ClassDesc.hpp
ForwardDemodulation.o: ../BlodKorv/ObjDesc.hpp
ForwardDemodulation.o: ../BlodKorv/AllocationFailureHandling.hpp
ForwardDemodulation.o: ../BlodKorv/RuntimeError.hpp ../BlodKorv/Allocator.hpp
ForwardDemodulation.o: ../BlodKorv/BufferedAllocationStatistics.hpp
ForwardDemodulation.o: ../BlodKorv/Comparison.hpp SymbolInfo.hpp
ForwardDemodulation.o: AutoFunctorPrecedence.hpp ../BlodKorv/Tuple.hpp
ForwardDemodulation.o: ../BlodKorv/GSortTree.hpp ../BlodKorv/GList.hpp
ForwardDemodulation.o: TermWeightType.hpp VampireKernel.hpp
ForwardDemodulation.o: ../Gematogen/ForwardMatchingIndexTree.hpp
ForwardDemodulation.o: ../BlodKorv/GlobalStopFlag.hpp
ForwardDemodulation.o: ../Gematogen/GematogenDebugFlags.hpp
ForwardDemodulation.o: ../BlodKorv/Stack.hpp
ForwardDemodulation.o: ../BlodKorv/EquivalenceRelation.hpp
ForwardDemodulation.o: ../BlodKorv/MultisetOfVariables.hpp
ForwardDemodulation.o: ../Gematogen/ForwardMatchingIndex.hpp
ForwardDemodulation.o: ../BlodKorv/ExpandingMultiset.hpp
ForwardDemodulation.o: ../BlodKorv/ExpandingStack.hpp Term.hpp
ForwardDemodulation.o: ../BlodKorv/BitSet.hpp ../BlodKorv/BitWord.hpp
ForwardDemodulation.o: ../BlodKorv/BitString.hpp
ForwardDemodulation.o: ../BlodKorv/SmallHeaderSkipList.hpp Flatterm.hpp
ForwardDemodulation.o: variables.hpp WeightPolynomial.hpp
ForwardDemodulation.o: ../BlodKorv/LinearPolynomial.hpp
ForwardDemodulation.o: ../BlodKorv/LinearCombination.hpp ../BlodKorv/Math.hpp
ForwardDemodulation.o: ../BlodKorv/SharedLinearPolynomial.hpp
ForwardDemodulation.o: ../BlodKorv/GDiscTree.hpp
ForwardDemodulation.o: ForwardMatchingSubstitutionCache.hpp
ForwardDemodulation.o: SimplificationOrdering.hpp
ForwardDemodulation.o: InstanceRetrievalForDemodulation.hpp
ForwardDemodulation.o: ../Gematogen/PathIndexingWithJoinsAndEarlyCleanUpForInstanceRetrieval.hpp
ForwardDemodulation.o: ../Gematogen/Gematogen.hpp ../Gematogen/PathIndex.hpp
ForwardDemodulation.o: ../BlodKorv/CompactNodeSmallHeaderSkipList.hpp
ForwardDemodulation.o: ../BlodKorv/GIncCountingSort.hpp
ForwardDemodulation.o: ../BlodKorv/Multiset.hpp Clause.hpp
ForwardDemodulation.o: ClauseBackground.hpp LiteralList.hpp
ForwardDemodulation.o: DefinedHeaderTable.hpp
IUQuery.o: ../BlodKorv/DestructionMode.hpp ../BlodKorv/GlobAlloc.hpp
IUQuery.o: ../BlodKorv/ObjDesc.hpp ../BlodKorv/AllocationFailureHandling.hpp
IUQuery.o: ../BlodKorv/Exit.hpp ../BlodKorv/RuntimeError.hpp
IUQuery.o: ../BlodKorv/debugMacros.hpp ../BlodKorv/Allocator.hpp
IUQuery.o: ../BlodKorv/BufferedAllocationStatistics.hpp
IUQuery.o: ../BlodKorv/CacheAllocator.hpp VampireKernelDebugFlags.hpp
IUQuery.o: UTerm.hpp ../BlodKorv/InfiniteStack.hpp ../BlodKorv/ObjectPool.hpp
IUQuery.o: ../BlodKorv/Comparison.hpp ../BlodKorv/Stack.hpp
IUQuery.o: ../BlodKorv/ExpandingStack.hpp ../BlodKorv/Array.hpp
IUQuery.o: ../BlodKorv/BitSet.hpp ../BlodKorv/BitWord.hpp
IUQuery.o: ../BlodKorv/BitString.hpp ../BlodKorv/SmallHeaderSkipList.hpp
IUQuery.o: AutoFunctorPrecedence.hpp ../BlodKorv/Tuple.hpp
IUQuery.o: ../BlodKorv/GSortTree.hpp ../BlodKorv/GList.hpp UVariable.hpp
InferenceDispatcher.o: InferenceDispatcher.hpp ../BlodKorv/jargon.hpp
InferenceDispatcher.o: VampireKernelDebugFlags.hpp
InferenceDispatcher.o: ../BlodKorv/BlodKorvDebugFlags.hpp or_index.hpp
InferenceDispatcher.o: ../BlodKorv/ObjDesc.hpp
InferenceDispatcher.o: ../BlodKorv/AllocationFailureHandling.hpp
InferenceDispatcher.o: ../BlodKorv/Exit.hpp ../BlodKorv/RuntimeError.hpp
InferenceDispatcher.o: ../BlodKorv/DestructionMode.hpp
InferenceDispatcher.o: ../BlodKorv/debugMacros.hpp ../BlodKorv/Allocator.hpp
InferenceDispatcher.o: ../BlodKorv/BufferedAllocationStatistics.hpp
InferenceDispatcher.o: ../BlodKorv/Array.hpp VampireKernelConst.hpp
InferenceDispatcher.o: ../BlodKorv/Stack.hpp ../BlodKorv/ExpandingStack.hpp
InferenceDispatcher.o: ../BlodKorv/BitSet.hpp ../BlodKorv/BitWord.hpp
InferenceDispatcher.o: ../BlodKorv/BitString.hpp
InferenceDispatcher.o: ../BlodKorv/SmallHeaderSkipList.hpp TermWeightType.hpp
InferenceDispatcher.o: AutoFunctorPrecedence.hpp ../BlodKorv/Tuple.hpp
InferenceDispatcher.o: ../BlodKorv/GSortTree.hpp ../BlodKorv/GList.hpp
InferenceDispatcher.o: prefix.hpp variables.hpp
InferenceDispatcher.o: ../BlodKorv/MultisetOfVariables.hpp
InferenceDispatcher.o: WeightPolynomial.hpp ../BlodKorv/LinearPolynomial.hpp
InferenceDispatcher.o: ../BlodKorv/LinearCombination.hpp ../BlodKorv/Math.hpp
InferenceDispatcher.o: ../BlodKorv/SharedLinearPolynomial.hpp
InferenceDispatcher.o: ../BlodKorv/GDiscTree.hpp unification.hpp
InferenceDispatcher.o: iu_command.hpp BackParaCandidates.hpp
InferenceDispatcher.o: ../BlodKorv/SkipList.hpp SimplificationOrdering.hpp
InferenceDispatcher.o: InstanceRetrievalForDemodulation.hpp
InferenceDispatcher.o: ../Gematogen/PathIndexingWithJoinsAndEarlyCleanUpForInstanceRetrieval.hpp
InferenceDispatcher.o: ../BlodKorv/GlobalStopFlag.hpp
InferenceDispatcher.o: ../Gematogen/Gematogen.hpp
InferenceDispatcher.o: ../Gematogen/GematogenDebugFlags.hpp
InferenceDispatcher.o: ../Gematogen/PathIndex.hpp
InferenceDispatcher.o: ../BlodKorv/CompactNodeSmallHeaderSkipList.hpp
InferenceDispatcher.o: ../BlodKorv/GIncCountingSort.hpp
InferenceDispatcher.o: ../BlodKorv/Multiset.hpp
InferenceDispatcher.o: VampireKernelInterceptMacros.hpp binary_resolution.hpp
InferenceDispatcher.o: premise_factoring.hpp or_premise.hpp Clause.hpp
InferenceDispatcher.o: ClauseBackground.hpp LiteralList.hpp
InferenceDispatcher.o: DefinedHeaderTable.hpp NewClause.hpp OpenNewClause.hpp
InferenceDispatcher.o: Flatterm.hpp tmp_literal.hpp TermWeighting.hpp
InferenceDispatcher.o: TmpLitList.hpp ForwardDemodulation.hpp
InferenceDispatcher.o: ../Gematogen/ForwardMatchingIndexTree.hpp
InferenceDispatcher.o: ../BlodKorv/EquivalenceRelation.hpp
InferenceDispatcher.o: ../Gematogen/ForwardMatchingIndex.hpp
InferenceDispatcher.o: ../BlodKorv/ExpandingMultiset.hpp
InferenceDispatcher.o: ForwardMatchingSubstitutionCache.hpp ClauseQueue.hpp
InferenceDispatcher.o: ../BlodKorv/FIFO.hpp PagedPassiveClauseStorage.hpp
InferenceDispatcher.o: ../BlodKorv/PageQueueDisciplineStorage.hpp
InferenceDispatcher.o: Splitting.hpp DivIntoComp.hpp or_machine.hpp
InferenceDispatcher.o: iu_machine.hpp iu_translator.hpp weight_formula.hpp
InferenceDispatcher.o: forward_superposition.hpp para_machine.hpp
InferenceDispatcher.o: eq_factoring.hpp backward_superposition.hpp
InferenceTree.o: InferenceTree.hpp ../BlodKorv/jargon.hpp
InferenceTree.o: VampireKernelDebugFlags.hpp
InferenceTree.o: ../BlodKorv/BlodKorvDebugFlags.hpp ../BlodKorv/GSortTree.hpp
InferenceTree.o: ../BlodKorv/debugMacros.hpp ../BlodKorv/Debug.hpp
InferenceTree.o: ../BlodKorv/ObjDesc.hpp
InferenceTree.o: ../BlodKorv/AllocationFailureHandling.hpp
InferenceTree.o: ../BlodKorv/Exit.hpp ../BlodKorv/RuntimeError.hpp
InferenceTree.o: ../BlodKorv/Allocator.hpp
InferenceTree.o: ../BlodKorv/BufferedAllocationStatistics.hpp
InferenceTree.o: ../BlodKorv/BitWord.hpp Term.hpp VampireKernelConst.hpp
InferenceTree.o: ../BlodKorv/Comparison.hpp ../BlodKorv/Stack.hpp
InferenceTree.o: ../BlodKorv/ExpandingStack.hpp ../BlodKorv/Array.hpp
InferenceTree.o: ../BlodKorv/BitSet.hpp ../BlodKorv/BitString.hpp
InferenceTree.o: ../BlodKorv/SmallHeaderSkipList.hpp TermWeightType.hpp
InferenceTree.o: ../BlodKorv/Tuple.hpp ClauseBackground.hpp LiteralList.hpp
InferenceTree.o: DefinedHeaderTable.hpp
Input.o: ../BlodKorv/jargon.hpp VampireKernelDebugFlags.hpp
Input.o: ../BlodKorv/BlodKorvDebugFlags.hpp ../BlodKorv/DestructionMode.hpp
InstanceRetrievalForDemodulation.o: InstanceRetrievalForDemodulation.hpp
InstanceRetrievalForDemodulation.o: ../BlodKorv/jargon.hpp
InstanceRetrievalForDemodulation.o: VampireKernelDebugFlags.hpp
InstanceRetrievalForDemodulation.o: ../BlodKorv/BlodKorvDebugFlags.hpp
InstanceRetrievalForDemodulation.o: Term.hpp ../BlodKorv/GlobAlloc.hpp
InstanceRetrievalForDemodulation.o: ../BlodKorv/ClassDesc.hpp
InstanceRetrievalForDemodulation.o: ../BlodKorv/Debug.hpp
InstanceRetrievalForDemodulation.o: ../BlodKorv/ObjDesc.hpp
InstanceRetrievalForDemodulation.o: ../BlodKorv/AllocationFailureHandling.hpp
InstanceRetrievalForDemodulation.o: ../BlodKorv/Exit.hpp
InstanceRetrievalForDemodulation.o: ../BlodKorv/RuntimeError.hpp
InstanceRetrievalForDemodulation.o: ../BlodKorv/DestructionMode.hpp
InstanceRetrievalForDemodulation.o: ../BlodKorv/debugMacros.hpp
InstanceRetrievalForDemodulation.o: ../BlodKorv/Allocator.hpp
InstanceRetrievalForDemodulation.o: ../BlodKorv/BufferedAllocationStatistics.hpp
InstanceRetrievalForDemodulation.o: ../BlodKorv/Comparison.hpp
InstanceRetrievalForDemodulation.o: ../BlodKorv/Stack.hpp
InstanceRetrievalForDemodulation.o: ../BlodKorv/ExpandingStack.hpp
InstanceRetrievalForDemodulation.o: ../BlodKorv/Array.hpp
InstanceRetrievalForDemodulation.o: ../BlodKorv/BitSet.hpp
InstanceRetrievalForDemodulation.o: ../BlodKorv/BitWord.hpp
InstanceRetrievalForDemodulation.o: ../BlodKorv/BitString.hpp
InstanceRetrievalForDemodulation.o: ../BlodKorv/SmallHeaderSkipList.hpp
InstanceRetrievalForDemodulation.o: TermWeightType.hpp VampireKernel.hpp
InstanceRetrievalForDemodulation.o: AutoFunctorPrecedence.hpp
InstanceRetrievalForDemodulation.o: ../BlodKorv/Tuple.hpp
InstanceRetrievalForDemodulation.o: ../BlodKorv/GSortTree.hpp
InstanceRetrievalForDemodulation.o: ../BlodKorv/GList.hpp
InstanceRetrievalForDemodulation.o: ../Gematogen/PathIndexingWithJoinsAndEarlyCleanUpForInstanceRetrieval.hpp
InstanceRetrievalForDemodulation.o: ../BlodKorv/GlobalStopFlag.hpp
InstanceRetrievalForDemodulation.o: ../Gematogen/Gematogen.hpp
InstanceRetrievalForDemodulation.o: ../Gematogen/GematogenDebugFlags.hpp
InstanceRetrievalForDemodulation.o: ../Gematogen/PathIndex.hpp
InstanceRetrievalForDemodulation.o: ../BlodKorv/CompactNodeSmallHeaderSkipList.hpp
InstanceRetrievalForDemodulation.o: ../BlodKorv/GIncCountingSort.hpp
InstanceRetrievalForDemodulation.o: ../BlodKorv/Multiset.hpp
InstanceRetrievalForDemodulation.o: ../BlodKorv/LinearCombination.hpp
LiteralList.o: VampireKernelDebugFlags.hpp ../BlodKorv/BlodKorvDebugFlags.hpp
LiteralList.o: ../BlodKorv/ClassDesc.hpp ../BlodKorv/Debug.hpp
LiteralList.o: ../BlodKorv/ObjDesc.hpp
LiteralList.o: ../BlodKorv/AllocationFailureHandling.hpp ../BlodKorv/Exit.hpp
LiteralList.o: ../BlodKorv/RuntimeError.hpp ../BlodKorv/DestructionMode.hpp
LiteralList.o: ../BlodKorv/debugMacros.hpp ../BlodKorv/Allocator.hpp
LiteralList.o: ../BlodKorv/BufferedAllocationStatistics.hpp Term.hpp
LiteralList.o: ../BlodKorv/Comparison.hpp ../BlodKorv/Stack.hpp
LiteralList.o: ../BlodKorv/ExpandingStack.hpp ../BlodKorv/Array.hpp
LiteralList.o: ../BlodKorv/BitSet.hpp ../BlodKorv/BitWord.hpp
LiteralList.o: ../BlodKorv/BitString.hpp ../BlodKorv/SmallHeaderSkipList.hpp
LiteralList.o: AutoFunctorPrecedence.hpp ../BlodKorv/Tuple.hpp
LiteralList.o: ../BlodKorv/GSortTree.hpp ../BlodKorv/GList.hpp
LiteralOrdering.o: LiteralOrdering.hpp ../BlodKorv/jargon.hpp
LiteralOrdering.o: VampireKernelDebugFlags.hpp
LiteralOrdering.o: ../BlodKorv/BlodKorvDebugFlags.hpp
LiteralOrdering.o: ../BlodKorv/debugMacros.hpp ../BlodKorv/Debug.hpp
LiteralOrdering.o: SimplificationOrdering.hpp WeightPolynomial.hpp
LiteralOrdering.o: ../BlodKorv/LinearCombination.hpp ../BlodKorv/Stack.hpp
LiteralOrdering.o: ../BlodKorv/Math.hpp
LiteralOrdering.o: ../BlodKorv/SharedLinearPolynomial.hpp
LiteralOrdering.o: ../BlodKorv/GDiscTree.hpp ../BlodKorv/DestructionMode.hpp
LiteralOrdering.o: ../BlodKorv/ClassDesc.hpp ../BlodKorv/ObjDesc.hpp
LiteralOrdering.o: ../BlodKorv/AllocationFailureHandling.hpp
LiteralOrdering.o: ../BlodKorv/Exit.hpp ../BlodKorv/RuntimeError.hpp
LiteralOrdering.o: ../BlodKorv/Allocator.hpp
LiteralOrdering.o: ../BlodKorv/BufferedAllocationStatistics.hpp
LiteralOrdering.o: Term.hpp ../BlodKorv/ExpandingStack.hpp
LiteralOrdering.o: ../BlodKorv/Array.hpp ../BlodKorv/BitSet.hpp
LiteralOrdering.o: ../BlodKorv/BitWord.hpp ../BlodKorv/BitString.hpp
LiteralOrdering.o: ../BlodKorv/SmallHeaderSkipList.hpp Signature.hpp
LiteralOrdering.o: AutoFunctorPrecedence.hpp ../BlodKorv/Tuple.hpp
LiteralOrdering.o: ../BlodKorv/GSortTree.hpp ../BlodKorv/GList.hpp
LiteralOrdering.o: InstanceRetrievalForDemodulation.hpp
LiteralOrdering.o: ../Gematogen/PathIndexingWithJoinsAndEarlyCleanUpForInstanceRetrieval.hpp
LiteralOrdering.o: ../BlodKorv/GlobalStopFlag.hpp ../Gematogen/Gematogen.hpp
LiteralOrdering.o: ../Gematogen/GematogenDebugFlags.hpp
LiteralOrdering.o: ../Gematogen/PathIndex.hpp
LiteralOrdering.o: ../BlodKorv/CompactNodeSmallHeaderSkipList.hpp
LiteralOrdering.o: ../BlodKorv/GIncCountingSort.hpp ../BlodKorv/Multiset.hpp
LiteralOrdering.o: tmp_literal.hpp TermWeighting.hpp Flatterm.hpp
LiteralOrdering.o: ../BlodKorv/MultisetOfVariables.hpp variables.hpp
LiteralSelection.o: VampireKernelDebugFlags.hpp
LiteralSelection.o: ../BlodKorv/BlodKorvDebugFlags.hpp LiteralSelection.hpp
LiteralSelection.o: ../BlodKorv/jargon.hpp ../BlodKorv/RuntimeError.hpp
LiteralSelection.o: ../BlodKorv/ClassDesc.hpp ../BlodKorv/Debug.hpp
LiteralSelection.o: ../BlodKorv/ObjDesc.hpp
LiteralSelection.o: ../BlodKorv/AllocationFailureHandling.hpp
LiteralSelection.o: ../BlodKorv/Exit.hpp ../BlodKorv/DestructionMode.hpp
LiteralSelection.o: ../BlodKorv/debugMacros.hpp ../BlodKorv/Allocator.hpp
LiteralSelection.o: ../BlodKorv/BufferedAllocationStatistics.hpp
LiteralSelection.o: ../BlodKorv/Comparison.hpp ../BlodKorv/Stack.hpp
LiteralSelection.o: ../BlodKorv/ExpandingStack.hpp ../BlodKorv/Array.hpp
LiteralSelection.o: ../BlodKorv/BitSet.hpp ../BlodKorv/BitWord.hpp
LiteralSelection.o: ../BlodKorv/BitString.hpp
LiteralSelection.o: ../BlodKorv/SmallHeaderSkipList.hpp TermWeightType.hpp
LiteralSelection.o: AutoFunctorPrecedence.hpp ../BlodKorv/Tuple.hpp
LiteralSelection.o: ../BlodKorv/GSortTree.hpp ../BlodKorv/GList.hpp
LiteralSelection.o: tmp_literal.hpp ../BlodKorv/Multiset.hpp
LiteralSelection.o: WeightPolynomial.hpp ../BlodKorv/LinearPolynomial.hpp
LiteralSelection.o: ../BlodKorv/LinearCombination.hpp ../BlodKorv/Math.hpp
LiteralSelection.o: ../BlodKorv/SharedLinearPolynomial.hpp
LiteralSelection.o: ../BlodKorv/GDiscTree.hpp TermWeighting.hpp Flatterm.hpp
LiteralSelection.o: ../BlodKorv/MultisetOfVariables.hpp variables.hpp
LiteralSelection.o: InstanceRetrievalForDemodulation.hpp
LiteralSelection.o: ../Gematogen/PathIndexingWithJoinsAndEarlyCleanUpForInstanceRetrieval.hpp
LiteralSelection.o: ../BlodKorv/GlobalStopFlag.hpp ../Gematogen/Gematogen.hpp
LiteralSelection.o: ../Gematogen/GematogenDebugFlags.hpp
LiteralSelection.o: ../Gematogen/PathIndex.hpp
LiteralSelection.o: ../BlodKorv/CompactNodeSmallHeaderSkipList.hpp
LiteralSelection.o: ../BlodKorv/GIncCountingSort.hpp
LiteralSelection.o: SimplificationOrdering.hpp LiteralOrdering.hpp
LiteralSharing.o: LiteralSharing.hpp ../BlodKorv/jargon.hpp
LiteralSharing.o: VampireKernelDebugFlags.hpp
LiteralSharing.o: ../BlodKorv/BlodKorvDebugFlags.hpp
LiteralSharing.o: ../BlodKorv/ObjDesc.hpp
LiteralSharing.o: ../BlodKorv/AllocationFailureHandling.hpp
LiteralSharing.o: ../BlodKorv/Exit.hpp ../BlodKorv/RuntimeError.hpp
LiteralSharing.o: ../BlodKorv/DestructionMode.hpp ../BlodKorv/debugMacros.hpp
LiteralSharing.o: ../BlodKorv/Allocator.hpp
LiteralSharing.o: ../BlodKorv/BufferedAllocationStatistics.hpp
LiteralSharing.o: VampireKernelConst.hpp ../BlodKorv/Comparison.hpp
LiteralSharing.o: ../BlodKorv/Stack.hpp ../BlodKorv/ExpandingStack.hpp
LiteralSharing.o: ../BlodKorv/BitSet.hpp ../BlodKorv/BitWord.hpp
LiteralSharing.o: ../BlodKorv/BitString.hpp
LiteralSharing.o: ../BlodKorv/SmallHeaderSkipList.hpp TermWeightType.hpp
LiteralSharing.o: ../BlodKorv/Tuple.hpp ../BlodKorv/GSortTree.hpp
LiteralSharing.o: ../BlodKorv/GList.hpp Flatterm.hpp
LiteralSharing.o: ../BlodKorv/MultisetOfVariables.hpp variables.hpp
LiteralSharing.o: WeightPolynomial.hpp ../BlodKorv/LinearPolynomial.hpp
LiteralSharing.o: ../BlodKorv/LinearCombination.hpp ../BlodKorv/Math.hpp
LiteralSharing.o: ../BlodKorv/SharedLinearPolynomial.hpp
LiteralSharing.o: ../BlodKorv/GDiscTree.hpp dtree.hpp sharing.hpp
LiteralSharing.o: VampireKernelInterceptMacros.hpp
LocalUnification.o: ../BlodKorv/jargon.hpp VampireKernelDebugFlags.hpp
LocalUnification.o: ../BlodKorv/BlodKorvDebugFlags.hpp VampireKernelConst.hpp
LocalUnification.o: ../BlodKorv/ClassDesc.hpp ../BlodKorv/Debug.hpp
LocalUnification.o: ../BlodKorv/ObjDesc.hpp
LocalUnification.o: ../BlodKorv/AllocationFailureHandling.hpp
LocalUnification.o: ../BlodKorv/Exit.hpp ../BlodKorv/RuntimeError.hpp
LocalUnification.o: ../BlodKorv/DestructionMode.hpp
LocalUnification.o: ../BlodKorv/debugMacros.hpp ../BlodKorv/Allocator.hpp
LocalUnification.o: ../BlodKorv/BufferedAllocationStatistics.hpp
LocalUnification.o: ../BlodKorv/Comparison.hpp ../BlodKorv/Stack.hpp
LocalUnification.o: ../BlodKorv/ExpandingStack.hpp ../BlodKorv/Array.hpp
LocalUnification.o: ../BlodKorv/BitSet.hpp ../BlodKorv/BitWord.hpp
LocalUnification.o: ../BlodKorv/BitString.hpp
LocalUnification.o: ../BlodKorv/SmallHeaderSkipList.hpp TermWeightType.hpp
LocalUnification.o: AutoFunctorPrecedence.hpp ../BlodKorv/Tuple.hpp
LocalUnification.o: ../BlodKorv/GSortTree.hpp ../BlodKorv/GList.hpp
Logging.o: ../BlodKorv/jargon.hpp ../BlodKorv/RuntimeError.hpp
Logging.o: VampireKernelDebugFlags.hpp ../BlodKorv/BlodKorvDebugFlags.hpp
Logging.o: ../BlodKorv/ClassDesc.hpp ../BlodKorv/Debug.hpp
Logging.o: ../BlodKorv/ObjDesc.hpp ../BlodKorv/AllocationFailureHandling.hpp
Logging.o: ../BlodKorv/Exit.hpp ../BlodKorv/DestructionMode.hpp
Logging.o: ../BlodKorv/debugMacros.hpp ../BlodKorv/Allocator.hpp
Logging.o: ../BlodKorv/BufferedAllocationStatistics.hpp
Logging.o: ../BlodKorv/Comparison.hpp ../BlodKorv/Stack.hpp
Logging.o: ../BlodKorv/ExpandingStack.hpp ../BlodKorv/Array.hpp
Logging.o: ../BlodKorv/BitSet.hpp ../BlodKorv/BitWord.hpp
Logging.o: ../BlodKorv/BitString.hpp ../BlodKorv/SmallHeaderSkipList.hpp
Logging.o: TermWeightType.hpp VampireKernel.hpp Signature.hpp
Logging.o: ../BlodKorv/Tuple.hpp ../BlodKorv/GSortTree.hpp
Logging.o: ../BlodKorv/GList.hpp Clause.hpp ClauseBackground.hpp
Logging.o: LiteralList.hpp DefinedHeaderTable.hpp Flatterm.hpp
Logging.o: ../BlodKorv/MultisetOfVariables.hpp variables.hpp
Logging.o: WeightPolynomial.hpp ../BlodKorv/LinearPolynomial.hpp
Logging.o: ../BlodKorv/LinearCombination.hpp ../BlodKorv/Math.hpp
Logging.o: ../BlodKorv/SharedLinearPolynomial.hpp ../BlodKorv/GDiscTree.hpp
Logging.o: prefix.hpp
Master.o: Master.hpp ../BlodKorv/jargon.hpp VampireKernelDebugFlags.hpp
Master.o: ../BlodKorv/BlodKorvDebugFlags.hpp ../BlodKorv/GlobAlloc.hpp
Master.o: ../BlodKorv/AllocationFailureHandling.hpp ../BlodKorv/Exit.hpp
Master.o: ../BlodKorv/RuntimeError.hpp ../BlodKorv/DestructionMode.hpp
Master.o: ../BlodKorv/debugMacros.hpp ../BlodKorv/Allocator.hpp
Master.o: ../BlodKorv/BufferedAllocationStatistics.hpp
Master.o: ../BlodKorv/GlobalStopFlag.hpp ../BlodKorv/GlobalClock.hpp
Master.o: ../BlodKorv/BitWord.hpp Term.hpp VampireKernelConst.hpp
Master.o: ../BlodKorv/Comparison.hpp ../BlodKorv/Stack.hpp
Master.o: ../BlodKorv/ExpandingStack.hpp ../BlodKorv/Array.hpp
Master.o: ../BlodKorv/BitSet.hpp ../BlodKorv/BitString.hpp
Master.o: ../BlodKorv/SmallHeaderSkipList.hpp TermWeightType.hpp
Master.o: ../BlodKorv/Tuple.hpp ../BlodKorv/GSortTree.hpp
Master.o: ClauseBackground.hpp LiteralList.hpp DefinedHeaderTable.hpp
Master.o: kept.hpp sharing.hpp VampireKernelInterceptMacros.hpp
Master.o: ClauseAssembler.hpp LiteralSharing.hpp variables.hpp
Master.o: ../BlodKorv/MultisetOfVariables.hpp WeightPolynomial.hpp
Master.o: ../BlodKorv/LinearPolynomial.hpp ../BlodKorv/LinearCombination.hpp
Master.o: ../BlodKorv/Math.hpp ../BlodKorv/SharedLinearPolynomial.hpp
Master.o: ../BlodKorv/GDiscTree.hpp NewClause.hpp prefix.hpp
Master.o: OpenNewClause.hpp Flatterm.hpp tmp_literal.hpp
Master.o: ../BlodKorv/Multiset.hpp TermWeighting.hpp
Master.o: InstanceRetrievalForDemodulation.hpp
Master.o: ../Gematogen/PathIndexingWithJoinsAndEarlyCleanUpForInstanceRetrieval.hpp
Master.o: ../Gematogen/Gematogen.hpp ../Gematogen/GematogenDebugFlags.hpp
Master.o: ../Gematogen/PathIndex.hpp
Master.o: ../BlodKorv/CompactNodeSmallHeaderSkipList.hpp
Master.o: ../BlodKorv/GIncCountingSort.hpp SimplificationOrdering.hpp
Master.o: TmpLitList.hpp ForwardDemodulation.hpp
Master.o: ../Gematogen/ForwardMatchingIndexTree.hpp
Master.o: ../BlodKorv/EquivalenceRelation.hpp
Master.o: ../Gematogen/ForwardMatchingIndex.hpp
Master.o: ../BlodKorv/ExpandingMultiset.hpp
Master.o: ForwardMatchingSubstitutionCache.hpp ClauseQueue.hpp
Master.o: ../BlodKorv/FIFO.hpp PagedPassiveClauseStorage.hpp
Master.o: ../BlodKorv/PageQueueDisciplineStorage.hpp Splitting.hpp
Master.o: DivIntoComp.hpp ForwardSubsumptionFrontEnd.hpp
Master.o: forward_subsumption.hpp fs_code.hpp fs_command.hpp old_fs_code.hpp
Master.o: ../BlodKorv/Queue.hpp ../BlodKorv/ObjectPool.hpp ComponentName.hpp
Master.o: InferenceDispatcher.hpp or_index.hpp unification.hpp iu_command.hpp
Master.o: BackParaCandidates.hpp ../BlodKorv/SkipList.hpp
Master.o: binary_resolution.hpp premise_factoring.hpp or_premise.hpp
Master.o: or_machine.hpp iu_machine.hpp iu_translator.hpp weight_formula.hpp
Master.o: forward_superposition.hpp para_machine.hpp eq_factoring.hpp
Master.o: backward_superposition.hpp BackwardSubsumption.hpp BSIndex.hpp
Master.o: ../Gematogen/PathIndexWithOpenPaths.hpp BSMachine.hpp
Master.o: ../BlodKorv/StaticCellArray.hpp ClauseNumIndex.hpp
Master.o: SubsumptionCheck.hpp SimpleSubstitution.hpp BackDemodulation.hpp
Master.o: LiteralSelection.hpp LiteralOrdering.hpp
Master.o: BuiltInTheoryDispatcher.hpp
Master3.o: Master.hpp ../BlodKorv/jargon.hpp VampireKernelDebugFlags.hpp
Master3.o: ../BlodKorv/BlodKorvDebugFlags.hpp ../BlodKorv/GlobAlloc.hpp
Master3.o: ../BlodKorv/AllocationFailureHandling.hpp ../BlodKorv/Exit.hpp
Master3.o: ../BlodKorv/RuntimeError.hpp ../BlodKorv/DestructionMode.hpp
Master3.o: ../BlodKorv/debugMacros.hpp ../BlodKorv/Allocator.hpp
Master3.o: ../BlodKorv/BufferedAllocationStatistics.hpp
Master3.o: ../BlodKorv/GlobalStopFlag.hpp ../BlodKorv/GlobalClock.hpp
Master3.o: ../BlodKorv/BitWord.hpp Term.hpp VampireKernelConst.hpp
Master3.o: ../BlodKorv/Comparison.hpp ../BlodKorv/Stack.hpp
Master3.o: ../BlodKorv/ExpandingStack.hpp ../BlodKorv/Array.hpp
Master3.o: ../BlodKorv/BitSet.hpp ../BlodKorv/BitString.hpp
Master3.o: ../BlodKorv/SmallHeaderSkipList.hpp TermWeightType.hpp
Master3.o: ../BlodKorv/Tuple.hpp ../BlodKorv/GSortTree.hpp
Master3.o: ClauseBackground.hpp LiteralList.hpp DefinedHeaderTable.hpp
Master3.o: kept.hpp sharing.hpp VampireKernelInterceptMacros.hpp
Master3.o: ClauseAssembler.hpp LiteralSharing.hpp variables.hpp
Master3.o: ../BlodKorv/MultisetOfVariables.hpp WeightPolynomial.hpp
Master3.o: ../BlodKorv/LinearPolynomial.hpp ../BlodKorv/LinearCombination.hpp
Master3.o: ../BlodKorv/Math.hpp ../BlodKorv/SharedLinearPolynomial.hpp
Master3.o: ../BlodKorv/GDiscTree.hpp NewClause.hpp prefix.hpp
Master3.o: OpenNewClause.hpp Flatterm.hpp tmp_literal.hpp
Master3.o: ../BlodKorv/Multiset.hpp TermWeighting.hpp
Master3.o: InstanceRetrievalForDemodulation.hpp
Master3.o: ../Gematogen/PathIndexingWithJoinsAndEarlyCleanUpForInstanceRetrieval.hpp
Master3.o: ../Gematogen/Gematogen.hpp ../Gematogen/GematogenDebugFlags.hpp
Master3.o: ../Gematogen/PathIndex.hpp
Master3.o: ../BlodKorv/CompactNodeSmallHeaderSkipList.hpp
Master3.o: ../BlodKorv/GIncCountingSort.hpp SimplificationOrdering.hpp
Master3.o: TmpLitList.hpp ForwardDemodulation.hpp
Master3.o: ../Gematogen/ForwardMatchingIndexTree.hpp
Master3.o: ../BlodKorv/EquivalenceRelation.hpp
Master3.o: ../Gematogen/ForwardMatchingIndex.hpp
Master3.o: ../BlodKorv/ExpandingMultiset.hpp
Master3.o: ForwardMatchingSubstitutionCache.hpp ClauseQueue.hpp
Master3.o: ../BlodKorv/FIFO.hpp PagedPassiveClauseStorage.hpp
Master3.o: ../BlodKorv/PageQueueDisciplineStorage.hpp Splitting.hpp
Master3.o: DivIntoComp.hpp ForwardSubsumptionFrontEnd.hpp
Master3.o: forward_subsumption.hpp fs_code.hpp fs_command.hpp old_fs_code.hpp
Master3.o: ../BlodKorv/Queue.hpp ../BlodKorv/ObjectPool.hpp ComponentName.hpp
Master3.o: InferenceDispatcher.hpp or_index.hpp unification.hpp
Master3.o: iu_command.hpp BackParaCandidates.hpp ../BlodKorv/SkipList.hpp
Master3.o: binary_resolution.hpp premise_factoring.hpp or_premise.hpp
Master3.o: or_machine.hpp iu_machine.hpp iu_translator.hpp weight_formula.hpp
Master3.o: forward_superposition.hpp para_machine.hpp eq_factoring.hpp
Master3.o: backward_superposition.hpp BackwardSubsumption.hpp BSIndex.hpp
Master3.o: ../Gematogen/PathIndexWithOpenPaths.hpp BSMachine.hpp
Master3.o: ../BlodKorv/StaticCellArray.hpp ClauseNumIndex.hpp
Master3.o: SubsumptionCheck.hpp SimpleSubstitution.hpp BackDemodulation.hpp
Master3.o: LiteralSelection.hpp LiteralOrdering.hpp
NewClause.o: VampireKernelDebugFlags.hpp ../BlodKorv/BlodKorvDebugFlags.hpp
NewClause.o: ../BlodKorv/ClassDesc.hpp ../BlodKorv/Debug.hpp
NewClause.o: ../BlodKorv/ObjDesc.hpp
NewClause.o: ../BlodKorv/AllocationFailureHandling.hpp ../BlodKorv/Exit.hpp
NewClause.o: ../BlodKorv/RuntimeError.hpp ../BlodKorv/DestructionMode.hpp
NewClause.o: ../BlodKorv/debugMacros.hpp ../BlodKorv/Allocator.hpp
NewClause.o: ../BlodKorv/BufferedAllocationStatistics.hpp
NewClause.o: ../BlodKorv/Comparison.hpp ../BlodKorv/Stack.hpp
NewClause.o: ../BlodKorv/ExpandingStack.hpp ../BlodKorv/Array.hpp
NewClause.o: ../BlodKorv/BitSet.hpp ../BlodKorv/BitWord.hpp
NewClause.o: ../BlodKorv/BitString.hpp ../BlodKorv/SmallHeaderSkipList.hpp
NewClause.o: AutoFunctorPrecedence.hpp ../BlodKorv/Tuple.hpp
NewClause.o: ../BlodKorv/GSortTree.hpp ../BlodKorv/GList.hpp prefix.hpp
NewClause.o: variables.hpp ../BlodKorv/MultisetOfVariables.hpp
NewClause.o: WeightPolynomial.hpp ../BlodKorv/LinearPolynomial.hpp
NewClause.o: ../BlodKorv/LinearCombination.hpp ../BlodKorv/Math.hpp
NewClause.o: ../BlodKorv/SharedLinearPolynomial.hpp ../BlodKorv/GDiscTree.hpp
NewClause.o: OpenNewClause.hpp Flatterm.hpp tmp_literal.hpp
NewClause.o: ../BlodKorv/Multiset.hpp TermWeighting.hpp
NewClause.o: InstanceRetrievalForDemodulation.hpp
NewClause.o: ../Gematogen/PathIndexingWithJoinsAndEarlyCleanUpForInstanceRetrieval.hpp
NewClause.o: ../BlodKorv/GlobalStopFlag.hpp ../Gematogen/Gematogen.hpp
NewClause.o: ../Gematogen/GematogenDebugFlags.hpp ../Gematogen/PathIndex.hpp
NewClause.o: ../BlodKorv/CompactNodeSmallHeaderSkipList.hpp
NewClause.o: ../BlodKorv/GIncCountingSort.hpp SimplificationOrdering.hpp
NewClause.o: TmpLitList.hpp ClauseBackground.hpp ForwardDemodulation.hpp
NewClause.o: VampireKernelInterceptMacros.hpp
NewClause.o: ../Gematogen/ForwardMatchingIndexTree.hpp
NewClause.o: ../BlodKorv/EquivalenceRelation.hpp
NewClause.o: ../Gematogen/ForwardMatchingIndex.hpp
NewClause.o: ../BlodKorv/ExpandingMultiset.hpp
NewClause.o: ForwardMatchingSubstitutionCache.hpp ClauseQueue.hpp
NewClause.o: ../BlodKorv/FIFO.hpp Clause.hpp LiteralList.hpp
NewClause.o: DefinedHeaderTable.hpp PagedPassiveClauseStorage.hpp
NewClause.o: ../BlodKorv/PageQueueDisciplineStorage.hpp Splitting.hpp
NewClause.o: DivIntoComp.hpp ForwardSubsumptionFrontEnd.hpp
NewClause.o: forward_subsumption.hpp fs_code.hpp fs_command.hpp
NewClause.o: old_fs_code.hpp ../BlodKorv/Queue.hpp ../BlodKorv/ObjectPool.hpp
NewClause.o: ClauseAssembler.hpp LiteralSharing.hpp
NewClause.o: BuiltInTheoryDispatcher.hpp
NonrecursiveKBOrdering.o: ../BlodKorv/jargon.hpp ../BlodKorv/Array.hpp
NonrecursiveKBOrdering.o: ../BlodKorv/BlodKorvDebugFlags.hpp
NonrecursiveKBOrdering.o: ../BlodKorv/DestructionMode.hpp
NonrecursiveKBOrdering.o: ../BlodKorv/debugMacros.hpp ../BlodKorv/Debug.hpp
NonrecursiveKBOrdering.o: VampireKernelDebugFlags.hpp
NonrecursiveKBOrdering.o: ../BlodKorv/Exit.hpp Signature.hpp
NonrecursiveKBOrdering.o: ../BlodKorv/ClassDesc.hpp ../BlodKorv/ObjDesc.hpp
NonrecursiveKBOrdering.o: ../BlodKorv/AllocationFailureHandling.hpp
NonrecursiveKBOrdering.o: ../BlodKorv/RuntimeError.hpp
NonrecursiveKBOrdering.o: ../BlodKorv/Allocator.hpp
NonrecursiveKBOrdering.o: ../BlodKorv/BufferedAllocationStatistics.hpp
NonrecursiveKBOrdering.o: ../BlodKorv/Comparison.hpp SymbolInfo.hpp
NonrecursiveKBOrdering.o: AutoFunctorPrecedence.hpp ../BlodKorv/Tuple.hpp
NonrecursiveKBOrdering.o: ../BlodKorv/GSortTree.hpp ../BlodKorv/GList.hpp
NonrecursiveKBOrdering.o: TermWeightType.hpp VampireKernel.hpp
NonrecursiveKBOrdering.o: WeightPolynomial.hpp
NonrecursiveKBOrdering.o: ../BlodKorv/LinearPolynomial.hpp
NonrecursiveKBOrdering.o: ../BlodKorv/LinearCombination.hpp
NonrecursiveKBOrdering.o: ../BlodKorv/Stack.hpp ../BlodKorv/Math.hpp
NonrecursiveKBOrdering.o: ../BlodKorv/SharedLinearPolynomial.hpp
NonrecursiveKBOrdering.o: ../BlodKorv/GDiscTree.hpp Term.hpp
NonrecursiveKBOrdering.o: ../BlodKorv/ExpandingStack.hpp
NonrecursiveKBOrdering.o: ../BlodKorv/BitSet.hpp ../BlodKorv/BitWord.hpp
NonrecursiveKBOrdering.o: ../BlodKorv/BitString.hpp
NonrecursiveKBOrdering.o: ../BlodKorv/SmallHeaderSkipList.hpp
NonrecursiveKBOrdering.o: InstanceRetrievalForDemodulation.hpp
NonrecursiveKBOrdering.o: ../Gematogen/PathIndexingWithJoinsAndEarlyCleanUpForInstanceRetrieval.hpp
NonrecursiveKBOrdering.o: ../BlodKorv/GlobalStopFlag.hpp
NonrecursiveKBOrdering.o: ../Gematogen/Gematogen.hpp
NonrecursiveKBOrdering.o: ../Gematogen/GematogenDebugFlags.hpp
NonrecursiveKBOrdering.o: ../Gematogen/PathIndex.hpp
NonrecursiveKBOrdering.o: ../BlodKorv/CompactNodeSmallHeaderSkipList.hpp
NonrecursiveKBOrdering.o: ../BlodKorv/GIncCountingSort.hpp
NonrecursiveKBOrdering.o: ../BlodKorv/Multiset.hpp Flatterm.hpp
NonrecursiveKBOrdering.o: ../BlodKorv/MultisetOfVariables.hpp variables.hpp
NonrecursiveKBOrdering.o: prefix.hpp
NonrecursiveKBOrdering.o: WeightPolynomialSpecialisedComparisonCommand.hpp
NonrecursiveKBOrdering.o: ForwardMatchingSubstitutionCache.hpp
NonrecursiveKBOrdering.o: TermWeighting.hpp
NonuniformConstantTermWeighting.o: NonuniformConstantTermWeighting.hpp
NonuniformConstantTermWeighting.o: ../BlodKorv/jargon.hpp
NonuniformConstantTermWeighting.o: VampireKernelDebugFlags.hpp
NonuniformConstantTermWeighting.o: ../BlodKorv/BlodKorvDebugFlags.hpp
NonuniformConstantTermWeighting.o: WeightPolynomial.hpp
NonuniformConstantTermWeighting.o: ../BlodKorv/LinearPolynomial.hpp
NonuniformConstantTermWeighting.o: ../BlodKorv/LinearCombination.hpp
NonuniformConstantTermWeighting.o: ../BlodKorv/Stack.hpp
NonuniformConstantTermWeighting.o: ../BlodKorv/debugMacros.hpp
NonuniformConstantTermWeighting.o: ../BlodKorv/Debug.hpp
NonuniformConstantTermWeighting.o: ../BlodKorv/Comparison.hpp
NonuniformConstantTermWeighting.o: ../BlodKorv/Math.hpp
NonuniformConstantTermWeighting.o: ../BlodKorv/SharedLinearPolynomial.hpp
NonuniformConstantTermWeighting.o: ../BlodKorv/GDiscTree.hpp
NonuniformConstantTermWeighting.o: ../BlodKorv/DestructionMode.hpp
NonuniformConstantTermWeighting.o: ../BlodKorv/GlobAlloc.hpp
NonuniformConstantTermWeighting.o: ../BlodKorv/ClassDesc.hpp
NonuniformConstantTermWeighting.o: ../BlodKorv/ObjDesc.hpp
NonuniformConstantTermWeighting.o: ../BlodKorv/AllocationFailureHandling.hpp
NonuniformConstantTermWeighting.o: ../BlodKorv/Exit.hpp
NonuniformConstantTermWeighting.o: ../BlodKorv/RuntimeError.hpp
NonuniformConstantTermWeighting.o: ../BlodKorv/Allocator.hpp
NonuniformConstantTermWeighting.o: ../BlodKorv/BufferedAllocationStatistics.hpp
NonuniformConstantTermWeighting.o: TermWeightType.hpp VampireKernel.hpp
NonuniformConstantTermWeighting.o: ../BlodKorv/ExpandingStack.hpp
NonuniformConstantTermWeighting.o: ../BlodKorv/Array.hpp
NonuniformConstantTermWeighting.o: ../BlodKorv/BitSet.hpp
NonuniformConstantTermWeighting.o: ../BlodKorv/BitWord.hpp
NonuniformConstantTermWeighting.o: ../BlodKorv/BitString.hpp
NonuniformConstantTermWeighting.o: ../BlodKorv/SmallHeaderSkipList.hpp
NonuniformConstantTermWeighting.o: SymbolInfo.hpp AutoFunctorPrecedence.hpp
NonuniformConstantTermWeighting.o: ../BlodKorv/Tuple.hpp
NonuniformConstantTermWeighting.o: ../BlodKorv/GSortTree.hpp
NonuniformConstantTermWeighting.o: ../BlodKorv/GList.hpp
NonuniformConstantTermWeighting.o: ../BlodKorv/MultisetOfVariables.hpp
NonuniformConstantTermWeighting.o: variables.hpp
NonuniformConstantTermWeighting.o: InstanceRetrievalForDemodulation.hpp
NonuniformConstantTermWeighting.o: ../Gematogen/PathIndexingWithJoinsAndEarlyCleanUpForInstanceRetrieval.hpp
NonuniformConstantTermWeighting.o: ../BlodKorv/GlobalStopFlag.hpp
NonuniformConstantTermWeighting.o: ../Gematogen/Gematogen.hpp
NonuniformConstantTermWeighting.o: ../Gematogen/GematogenDebugFlags.hpp
NonuniformConstantTermWeighting.o: ../Gematogen/PathIndex.hpp
NonuniformConstantTermWeighting.o: ../BlodKorv/CompactNodeSmallHeaderSkipList.hpp
NonuniformConstantTermWeighting.o: ../BlodKorv/GIncCountingSort.hpp
NonuniformConstantTermWeighting.o: ../BlodKorv/Multiset.hpp prefix.hpp
NonuniformConstantTermWeighting.o: ForwardMatchingSubstitutionCache.hpp
NonuniformConstantTermWeighting.o: LocalUnification.hpp
OldBSIndex.o: OldBSIndex.hpp ../BlodKorv/jargon.hpp
OldBSIndex.o: VampireKernelDebugFlags.hpp ../BlodKorv/BlodKorvDebugFlags.hpp
OldBSIndex.o: ../BlodKorv/ClassDesc.hpp ../BlodKorv/Debug.hpp
OldBSIndex.o: ../BlodKorv/ObjDesc.hpp
OldBSIndex.o: ../BlodKorv/AllocationFailureHandling.hpp ../BlodKorv/Exit.hpp
OldBSIndex.o: ../BlodKorv/RuntimeError.hpp ../BlodKorv/debugMacros.hpp
OldBSIndex.o: ../BlodKorv/Allocator.hpp
OldBSIndex.o: ../BlodKorv/BufferedAllocationStatistics.hpp
OldBSIndex.o: ../BlodKorv/Comparison.hpp ../BlodKorv/Stack.hpp
OldBSIndex.o: ../BlodKorv/ExpandingStack.hpp ../BlodKorv/Array.hpp
OldBSIndex.o: ../BlodKorv/BitSet.hpp ../BlodKorv/BitWord.hpp
OldBSIndex.o: ../BlodKorv/BitString.hpp ../BlodKorv/SmallHeaderSkipList.hpp
OldBSIndex.o: AutoFunctorPrecedence.hpp ../BlodKorv/Tuple.hpp
OldBSIndex.o: ../BlodKorv/GSortTree.hpp ../BlodKorv/GList.hpp DiscTree.hpp
OldBackwardSubsumption.o: OldBackwardSubsumption.hpp ../BlodKorv/jargon.hpp
OldBackwardSubsumption.o: VampireKernelDebugFlags.hpp
OldBackwardSubsumption.o: ../BlodKorv/BlodKorvDebugFlags.hpp
OldBackwardSubsumption.o: ../BlodKorv/DestructionMode.hpp Term.hpp
OldBackwardSubsumption.o: ../BlodKorv/Debug.hpp ../BlodKorv/ObjDesc.hpp
OldBackwardSubsumption.o: ../BlodKorv/AllocationFailureHandling.hpp
OldBackwardSubsumption.o: ../BlodKorv/Exit.hpp ../BlodKorv/RuntimeError.hpp
OldBackwardSubsumption.o: ../BlodKorv/debugMacros.hpp
OldBackwardSubsumption.o: ../BlodKorv/Allocator.hpp
OldBackwardSubsumption.o: ../BlodKorv/BufferedAllocationStatistics.hpp
OldBackwardSubsumption.o: ../BlodKorv/Comparison.hpp ../BlodKorv/Stack.hpp
OldBackwardSubsumption.o: ../BlodKorv/ExpandingStack.hpp
OldBackwardSubsumption.o: ../BlodKorv/Array.hpp ../BlodKorv/BitSet.hpp
OldBackwardSubsumption.o: ../BlodKorv/BitWord.hpp ../BlodKorv/BitString.hpp
OldBackwardSubsumption.o: ../BlodKorv/SmallHeaderSkipList.hpp
OldBackwardSubsumption.o: TermWeightType.hpp VampireKernel.hpp
OldBackwardSubsumption.o: SymbolInfo.hpp AutoFunctorPrecedence.hpp
OldBackwardSubsumption.o: ../BlodKorv/Tuple.hpp ../BlodKorv/GSortTree.hpp
OldBackwardSubsumption.o: ../BlodKorv/GList.hpp DiscTree.hpp OldBSMachine.hpp
OldBackwardSubsumption.o: Clause.hpp ClauseBackground.hpp LiteralList.hpp
OldBackwardSubsumption.o: DefinedHeaderTable.hpp ClauseNumIndex.hpp
OldBackwardSubsumption.o: ../BlodKorv/SkipList.hpp SubsumptionCheck.hpp
OldBackwardSubsumption.o: SimpleSubstitution.hpp
OrdinaryUnification.o: ../BlodKorv/RuntimeError.hpp ../BlodKorv/jargon.hpp
OrdinaryUnification.o: ../BlodKorv/DestructionMode.hpp
OrdinaryUnification.o: ../BlodKorv/BlodKorvDebugFlags.hpp
OrdinaryUnification.o: ../BlodKorv/Debug.hpp ../BlodKorv/ObjDesc.hpp
OrdinaryUnification.o: ../BlodKorv/AllocationFailureHandling.hpp
OrdinaryUnification.o: ../BlodKorv/Exit.hpp ../BlodKorv/debugMacros.hpp
OrdinaryUnification.o: ../BlodKorv/Allocator.hpp
OrdinaryUnification.o: ../BlodKorv/BufferedAllocationStatistics.hpp
OrdinaryUnification.o: VampireKernelDebugFlags.hpp UTerm.hpp
OrdinaryUnification.o: ../BlodKorv/InfiniteStack.hpp
OrdinaryUnification.o: VampireKernelConst.hpp ../BlodKorv/Comparison.hpp
OrdinaryUnification.o: ../BlodKorv/Stack.hpp ../BlodKorv/ExpandingStack.hpp
OrdinaryUnification.o: ../BlodKorv/Array.hpp ../BlodKorv/BitSet.hpp
OrdinaryUnification.o: ../BlodKorv/BitWord.hpp ../BlodKorv/BitString.hpp
OrdinaryUnification.o: ../BlodKorv/SmallHeaderSkipList.hpp TermWeightType.hpp
OrdinaryUnification.o: AutoFunctorPrecedence.hpp ../BlodKorv/Tuple.hpp
OrdinaryUnification.o: ../BlodKorv/GSortTree.hpp ../BlodKorv/GList.hpp
OrdinaryUnification.o: UVariable.hpp
PagedPassiveClauseStorage.o: PagedPassiveClauseStorage.hpp
PagedPassiveClauseStorage.o: VampireKernelDebugFlags.hpp
PagedPassiveClauseStorage.o: ../BlodKorv/BlodKorvDebugFlags.hpp
PagedPassiveClauseStorage.o: ../BlodKorv/Debug.hpp ../BlodKorv/ObjDesc.hpp
PagedPassiveClauseStorage.o: ../BlodKorv/AllocationFailureHandling.hpp
PagedPassiveClauseStorage.o: ../BlodKorv/Exit.hpp
PagedPassiveClauseStorage.o: ../BlodKorv/RuntimeError.hpp
PagedPassiveClauseStorage.o: ../BlodKorv/DestructionMode.hpp
PagedPassiveClauseStorage.o: ../BlodKorv/debugMacros.hpp
PagedPassiveClauseStorage.o: ../BlodKorv/Allocator.hpp
PagedPassiveClauseStorage.o: ../BlodKorv/BufferedAllocationStatistics.hpp
PagedPassiveClauseStorage.o: ../BlodKorv/BitWord.hpp
PagedPassiveClauseStorage.o: ../BlodKorv/PageQueueDisciplineStorage.hpp
PagedPassiveClauseStorage.o: Term.hpp ../BlodKorv/Comparison.hpp
PagedPassiveClauseStorage.o: ../BlodKorv/Stack.hpp
PagedPassiveClauseStorage.o: ../BlodKorv/ExpandingStack.hpp
PagedPassiveClauseStorage.o: ../BlodKorv/Array.hpp ../BlodKorv/BitSet.hpp
PagedPassiveClauseStorage.o: ../BlodKorv/BitString.hpp
PagedPassiveClauseStorage.o: ../BlodKorv/SmallHeaderSkipList.hpp
PagedPassiveClauseStorage.o: TermWeightType.hpp VampireKernel.hpp
PagedPassiveClauseStorage.o: AutoFunctorPrecedence.hpp ../BlodKorv/Tuple.hpp
PagedPassiveClauseStorage.o: ../BlodKorv/GSortTree.hpp ../BlodKorv/GList.hpp
PagedPassiveClauseStorage.o: ClauseBackground.hpp Clause.hpp LiteralList.hpp
PagedPassiveClauseStorage.o: DefinedHeaderTable.hpp
Parser.o: ../BlodKorv/jargon.hpp ../BlodKorv/BlodKorvDebugFlags.hpp
Parser.o: ../BlodKorv/ObjDesc.hpp ../BlodKorv/debugMacros.hpp Parser.hpp
Parser.o: PR_PARSER/pr_parser.hpp
ProblemProfile.o: ../BlodKorv/jargon.hpp VampireKernelDebugFlags.hpp
ProblemProfile.o: ../BlodKorv/BlodKorvDebugFlags.hpp
ProblemProfile.o: ../BlodKorv/Debug.hpp ../BlodKorv/ObjDesc.hpp
ProblemProfile.o: ../BlodKorv/AllocationFailureHandling.hpp
ProblemProfile.o: ../BlodKorv/Exit.hpp ../BlodKorv/RuntimeError.hpp
ProblemProfile.o: ../BlodKorv/DestructionMode.hpp ../BlodKorv/debugMacros.hpp
ProblemProfile.o: ../BlodKorv/Allocator.hpp
ProblemProfile.o: ../BlodKorv/BufferedAllocationStatistics.hpp
ProblemProfile.o: ../BlodKorv/Array.hpp ClauseQueue.hpp ../BlodKorv/FIFO.hpp
ProblemProfile.o: ../BlodKorv/BitWord.hpp Term.hpp VampireKernelConst.hpp
ProblemProfile.o: ../BlodKorv/Comparison.hpp ../BlodKorv/Stack.hpp
ProblemProfile.o: ../BlodKorv/ExpandingStack.hpp ../BlodKorv/BitSet.hpp
ProblemProfile.o: ../BlodKorv/BitString.hpp
ProblemProfile.o: ../BlodKorv/SmallHeaderSkipList.hpp TermWeightType.hpp
ProblemProfile.o: ../BlodKorv/Tuple.hpp ../BlodKorv/GSortTree.hpp
ProblemProfile.o: ClauseBackground.hpp LiteralList.hpp DefinedHeaderTable.hpp
Sigma.o: ../BlodKorv/BlodKorvDebugFlags.hpp ../BlodKorv/ClassDesc.hpp
Sigma.o: ../BlodKorv/Debug.hpp ../BlodKorv/ObjDesc.hpp
Sigma.o: ../BlodKorv/AllocationFailureHandling.hpp ../BlodKorv/Exit.hpp
Sigma.o: ../BlodKorv/RuntimeError.hpp ../BlodKorv/DestructionMode.hpp
Sigma.o: ../BlodKorv/debugMacros.hpp ../BlodKorv/Allocator.hpp
Sigma.o: ../BlodKorv/BufferedAllocationStatistics.hpp
Sigma.o: VampireKernelDebugFlags.hpp ../BlodKorv/String.hpp
Sigma.o: ../BlodKorv/SkipList.hpp Symbol.hpp ../BlodKorv/ObjectPool.hpp
Signature.o: VampireKernelDebugFlags.hpp ../BlodKorv/BlodKorvDebugFlags.hpp
Signature.o: ../BlodKorv/Array.hpp ../BlodKorv/DestructionMode.hpp
Signature.o: ../BlodKorv/debugMacros.hpp ../BlodKorv/Debug.hpp
Signature.o: ../BlodKorv/GlobAlloc.hpp ../BlodKorv/ClassDesc.hpp
Signature.o: ../BlodKorv/ObjDesc.hpp
Signature.o: ../BlodKorv/AllocationFailureHandling.hpp ../BlodKorv/Exit.hpp
Signature.o: ../BlodKorv/RuntimeError.hpp ../BlodKorv/Allocator.hpp
Signature.o: ../BlodKorv/BufferedAllocationStatistics.hpp
Signature.o: ../BlodKorv/Comparison.hpp SymbolInfo.hpp
Signature.o: AutoFunctorPrecedence.hpp ../BlodKorv/Tuple.hpp
Signature.o: ../BlodKorv/GSortTree.hpp ../BlodKorv/GList.hpp
Signature.o: Tabulation.hpp
SimplificationOrdering.o: SimplificationOrdering.hpp ../BlodKorv/jargon.hpp
SimplificationOrdering.o: VampireKernelDebugFlags.hpp
SimplificationOrdering.o: ../BlodKorv/BlodKorvDebugFlags.hpp
SimplificationOrdering.o: ../BlodKorv/debugMacros.hpp ../BlodKorv/Debug.hpp
SimplificationOrdering.o: WeightPolynomial.hpp VampireKernelConst.hpp
SimplificationOrdering.o: ../BlodKorv/LinearPolynomial.hpp
SimplificationOrdering.o: ../BlodKorv/LinearCombination.hpp
SimplificationOrdering.o: ../BlodKorv/Stack.hpp ../BlodKorv/Math.hpp
SimplificationOrdering.o: ../BlodKorv/SharedLinearPolynomial.hpp
SimplificationOrdering.o: ../BlodKorv/GDiscTree.hpp
SimplificationOrdering.o: ../BlodKorv/DestructionMode.hpp
SimplificationOrdering.o: ../BlodKorv/ClassDesc.hpp ../BlodKorv/ObjDesc.hpp
SimplificationOrdering.o: ../BlodKorv/AllocationFailureHandling.hpp
SimplificationOrdering.o: ../BlodKorv/Exit.hpp ../BlodKorv/RuntimeError.hpp
SimplificationOrdering.o: ../BlodKorv/Allocator.hpp
SimplificationOrdering.o: ../BlodKorv/BufferedAllocationStatistics.hpp
SimplificationOrdering.o: TermWeightType.hpp VampireKernel.hpp
SimplificationOrdering.o: ../BlodKorv/ExpandingStack.hpp
SimplificationOrdering.o: ../BlodKorv/Array.hpp ../BlodKorv/BitSet.hpp
SimplificationOrdering.o: ../BlodKorv/BitWord.hpp ../BlodKorv/BitString.hpp
SimplificationOrdering.o: ../BlodKorv/SmallHeaderSkipList.hpp Signature.hpp
SimplificationOrdering.o: AutoFunctorPrecedence.hpp ../BlodKorv/Tuple.hpp
SimplificationOrdering.o: ../BlodKorv/GSortTree.hpp ../BlodKorv/GList.hpp
SimplificationOrdering.o: InstanceRetrievalForDemodulation.hpp
SimplificationOrdering.o: ../Gematogen/PathIndexingWithJoinsAndEarlyCleanUpForInstanceRetrieval.hpp
SimplificationOrdering.o: ../BlodKorv/GlobalStopFlag.hpp
SimplificationOrdering.o: ../Gematogen/Gematogen.hpp
SimplificationOrdering.o: ../Gematogen/GematogenDebugFlags.hpp
SimplificationOrdering.o: ../Gematogen/PathIndex.hpp
SimplificationOrdering.o: ../BlodKorv/CompactNodeSmallHeaderSkipList.hpp
SimplificationOrdering.o: ../BlodKorv/GIncCountingSort.hpp
SimplificationOrdering.o: ../BlodKorv/Multiset.hpp NonrecursiveKBOrdering.hpp
SimplificationOrdering.o: VampireKernelInterceptMacros.hpp Flatterm.hpp
SimplificationOrdering.o: ../BlodKorv/MultisetOfVariables.hpp variables.hpp
SimplificationOrdering.o: prefix.hpp
SimplificationOrdering.o: WeightPolynomialSpecialisedComparisonCommand.hpp
SimplificationOrdering.o: ForwardMatchingSubstitutionCache.hpp
Splitting.o: Splitting.hpp ../BlodKorv/jargon.hpp
Splitting.o: ../BlodKorv/RuntimeError.hpp VampireKernelDebugFlags.hpp
Splitting.o: ../BlodKorv/BlodKorvDebugFlags.hpp VampireKernelConst.hpp
Splitting.o: ../BlodKorv/debugMacros.hpp ../BlodKorv/Debug.hpp
Splitting.o: ../BlodKorv/ExpandingStack.hpp ../BlodKorv/GlobAlloc.hpp
Splitting.o: ../BlodKorv/ClassDesc.hpp ../BlodKorv/ObjDesc.hpp
Splitting.o: ../BlodKorv/AllocationFailureHandling.hpp ../BlodKorv/Exit.hpp
Splitting.o: ../BlodKorv/Allocator.hpp
Splitting.o: ../BlodKorv/BufferedAllocationStatistics.hpp DivIntoComp.hpp
Splitting.o: ../BlodKorv/Multiset.hpp ../BlodKorv/Stack.hpp
Splitting.o: ../BlodKorv/Comparison.hpp ClauseQueue.hpp ../BlodKorv/FIFO.hpp
Splitting.o: Clause.hpp ../BlodKorv/GList.hpp ../BlodKorv/BitWord.hpp
Splitting.o: Term.hpp ../BlodKorv/BitSet.hpp ../BlodKorv/BitString.hpp
Splitting.o: ../BlodKorv/SmallHeaderSkipList.hpp TermWeightType.hpp
Splitting.o: ../BlodKorv/Tuple.hpp ../BlodKorv/GSortTree.hpp
Splitting.o: ClauseBackground.hpp LiteralList.hpp DefinedHeaderTable.hpp
Splitting.o: TmpLitList.hpp tmp_literal.hpp WeightPolynomial.hpp
Splitting.o: ../BlodKorv/LinearPolynomial.hpp
Splitting.o: ../BlodKorv/LinearCombination.hpp ../BlodKorv/Math.hpp
Splitting.o: ../BlodKorv/SharedLinearPolynomial.hpp ../BlodKorv/GDiscTree.hpp
Splitting.o: TermWeighting.hpp Flatterm.hpp
Splitting.o: ../BlodKorv/MultisetOfVariables.hpp variables.hpp
Splitting.o: InstanceRetrievalForDemodulation.hpp
Splitting.o: ../Gematogen/PathIndexingWithJoinsAndEarlyCleanUpForInstanceRetrieval.hpp
Splitting.o: ../BlodKorv/GlobalStopFlag.hpp ../Gematogen/Gematogen.hpp
Splitting.o: ../Gematogen/GematogenDebugFlags.hpp ../Gematogen/PathIndex.hpp
Splitting.o: ../BlodKorv/CompactNodeSmallHeaderSkipList.hpp
Splitting.o: ../BlodKorv/GIncCountingSort.hpp SimplificationOrdering.hpp
Splitting.o: OpenNewClause.hpp ForwardSubsumptionFrontEnd.hpp
Splitting.o: forward_subsumption.hpp fs_code.hpp
Splitting.o: ../BlodKorv/EquivalenceRelation.hpp fs_command.hpp
Splitting.o: old_fs_code.hpp ../BlodKorv/Queue.hpp ../BlodKorv/ObjectPool.hpp
Splitting.o: ComponentName.hpp PagedPassiveClauseStorage.hpp
Splitting.o: ../BlodKorv/PageQueueDisciplineStorage.hpp ClauseAssembler.hpp
Splitting.o: LiteralSharing.hpp
Splitting3.o: Splitting.hpp ../BlodKorv/jargon.hpp
Splitting3.o: ../BlodKorv/RuntimeError.hpp VampireKernelDebugFlags.hpp
Splitting3.o: ../BlodKorv/BlodKorvDebugFlags.hpp VampireKernelConst.hpp
Splitting3.o: ../BlodKorv/debugMacros.hpp ../BlodKorv/Debug.hpp
Splitting3.o: ../BlodKorv/ExpandingStack.hpp ../BlodKorv/GlobAlloc.hpp
Splitting3.o: ../BlodKorv/ClassDesc.hpp ../BlodKorv/ObjDesc.hpp
Splitting3.o: ../BlodKorv/AllocationFailureHandling.hpp ../BlodKorv/Exit.hpp
Splitting3.o: ../BlodKorv/Allocator.hpp
Splitting3.o: ../BlodKorv/BufferedAllocationStatistics.hpp DivIntoComp.hpp
Splitting3.o: ../BlodKorv/Multiset.hpp ../BlodKorv/Stack.hpp
Splitting3.o: ../BlodKorv/Comparison.hpp ClauseQueue.hpp ../BlodKorv/FIFO.hpp
Splitting3.o: Clause.hpp ../BlodKorv/GList.hpp ../BlodKorv/BitWord.hpp
Splitting3.o: Term.hpp ../BlodKorv/BitSet.hpp ../BlodKorv/BitString.hpp
Splitting3.o: ../BlodKorv/SmallHeaderSkipList.hpp TermWeightType.hpp
Splitting3.o: ../BlodKorv/Tuple.hpp ../BlodKorv/GSortTree.hpp
Splitting3.o: ClauseBackground.hpp LiteralList.hpp DefinedHeaderTable.hpp
Splitting3.o: TmpLitList.hpp tmp_literal.hpp WeightPolynomial.hpp
Splitting3.o: ../BlodKorv/LinearPolynomial.hpp
Splitting3.o: ../BlodKorv/LinearCombination.hpp ../BlodKorv/Math.hpp
Splitting3.o: ../BlodKorv/SharedLinearPolynomial.hpp
Splitting3.o: ../BlodKorv/GDiscTree.hpp TermWeighting.hpp Flatterm.hpp
Splitting3.o: ../BlodKorv/MultisetOfVariables.hpp variables.hpp
Splitting3.o: InstanceRetrievalForDemodulation.hpp
Splitting3.o: ../Gematogen/PathIndexingWithJoinsAndEarlyCleanUpForInstanceRetrieval.hpp
Splitting3.o: ../BlodKorv/GlobalStopFlag.hpp ../Gematogen/Gematogen.hpp
Splitting3.o: ../Gematogen/GematogenDebugFlags.hpp ../Gematogen/PathIndex.hpp
Splitting3.o: ../BlodKorv/CompactNodeSmallHeaderSkipList.hpp
Splitting3.o: ../BlodKorv/GIncCountingSort.hpp SimplificationOrdering.hpp
Splitting3.o: OpenNewClause.hpp ForwardSubsumptionFrontEnd.hpp
Splitting3.o: forward_subsumption.hpp fs_code.hpp
Splitting3.o: ../BlodKorv/EquivalenceRelation.hpp fs_command.hpp
Splitting3.o: old_fs_code.hpp ../BlodKorv/Queue.hpp
Splitting3.o: ../BlodKorv/ObjectPool.hpp ComponentName.hpp
Splitting3.o: PagedPassiveClauseStorage.hpp
Splitting3.o: ../BlodKorv/PageQueueDisciplineStorage.hpp ClauseAssembler.hpp
Splitting3.o: LiteralSharing.hpp
Standalone.o: ../BlodKorv/jargon.hpp /usr/include/unistd.h
Standalone.o: /usr/include/features.h /usr/include/sys/cdefs.h
Standalone.o: /usr/include/gnu/stubs.h /usr/include/bits/posix_opt.h
Standalone.o: /usr/include/bits/pthreadtypes.h /usr/include/bits/sched.h
Standalone.o: /usr/include/bits/confname.h /usr/include/getopt.h
Standalone.o: ../BlodKorv/RuntimeError.hpp ../BlodKorv/GlobalClock.hpp
Standalone.o: ../BlodKorv/BlodKorvDebugFlags.hpp
Standalone.o: ../BlodKorv/DestructionMode.hpp ../BlodKorv/debugMacros.hpp
Standalone.o: ../BlodKorv/Debug.hpp ../BlodKorv/GlobalStopFlag.hpp
Standalone.o: ../BlodKorv/GlobAlloc.hpp ../BlodKorv/ClassDesc.hpp
Standalone.o: ../BlodKorv/ObjDesc.hpp
Standalone.o: ../BlodKorv/AllocationFailureHandling.hpp
Standalone.o: ../BlodKorv/Allocator.hpp
Standalone.o: ../BlodKorv/BufferedAllocationStatistics.hpp
Standalone.o: VampireKernelDebugFlags.hpp VampireKernel.hpp
Standalone.o: PR_PARSER/pr_abstract_syntax.hpp PR_PARSER/pr_parser.hpp
Standalone.o: Tabulation.hpp TermWeightType.hpp
Standalone.o: VampireKernelInterceptMacros.hpp Signature.hpp
Standalone.o: VampireKernelConst.hpp ../BlodKorv/Array.hpp
Standalone.o: ../BlodKorv/Comparison.hpp SymbolInfo.hpp
Standalone.o: AutoFunctorPrecedence.hpp ../BlodKorv/Tuple.hpp
Standalone.o: ../BlodKorv/GSortTree.hpp
StandardKBOrdering.o: ../BlodKorv/jargon.hpp ../BlodKorv/Array.hpp
StandardKBOrdering.o: ../BlodKorv/DestructionMode.hpp
StandardKBOrdering.o: ../BlodKorv/debugMacros.hpp ../BlodKorv/Debug.hpp
StandardKBOrdering.o: VampireKernelDebugFlags.hpp
StandardKBOrdering.o: ../BlodKorv/Exit.hpp Signature.hpp
StandardKBOrdering.o: ../BlodKorv/ClassDesc.hpp ../BlodKorv/ObjDesc.hpp
StandardKBOrdering.o: ../BlodKorv/AllocationFailureHandling.hpp
StandardKBOrdering.o: ../BlodKorv/RuntimeError.hpp ../BlodKorv/Allocator.hpp
StandardKBOrdering.o: ../BlodKorv/BufferedAllocationStatistics.hpp
StandardKBOrdering.o: ../BlodKorv/Comparison.hpp SymbolInfo.hpp
StandardKBOrdering.o: AutoFunctorPrecedence.hpp ../BlodKorv/Tuple.hpp
StandardKBOrdering.o: ../BlodKorv/GSortTree.hpp ../BlodKorv/GList.hpp
StandardKBOrdering.o: SimplificationOrdering.hpp WeightPolynomial.hpp
StandardKBOrdering.o: ../BlodKorv/LinearPolynomial.hpp
StandardKBOrdering.o: ../BlodKorv/LinearCombination.hpp ../BlodKorv/Stack.hpp
StandardKBOrdering.o: ../BlodKorv/Math.hpp
StandardKBOrdering.o: ../BlodKorv/SharedLinearPolynomial.hpp
StandardKBOrdering.o: ../BlodKorv/GDiscTree.hpp Term.hpp
StandardKBOrdering.o: ../BlodKorv/ExpandingStack.hpp ../BlodKorv/BitSet.hpp
StandardKBOrdering.o: ../BlodKorv/BitWord.hpp ../BlodKorv/BitString.hpp
StandardKBOrdering.o: ../BlodKorv/SmallHeaderSkipList.hpp
StandardKBOrdering.o: InstanceRetrievalForDemodulation.hpp
StandardKBOrdering.o: ../Gematogen/PathIndexingWithJoinsAndEarlyCleanUpForInstanceRetrieval.hpp
StandardKBOrdering.o: ../BlodKorv/GlobalStopFlag.hpp
StandardKBOrdering.o: ../Gematogen/Gematogen.hpp
StandardKBOrdering.o: ../Gematogen/GematogenDebugFlags.hpp
StandardKBOrdering.o: ../Gematogen/PathIndex.hpp
StandardKBOrdering.o: ../BlodKorv/CompactNodeSmallHeaderSkipList.hpp
StandardKBOrdering.o: ../BlodKorv/GIncCountingSort.hpp
StandardKBOrdering.o: ../BlodKorv/Multiset.hpp Flatterm.hpp
StandardKBOrdering.o: ../BlodKorv/MultisetOfVariables.hpp variables.hpp
StandardKBOrdering.o: prefix.hpp ../BlodKorv/ObjectPool.hpp
StandardKBOrdering.o: WeightPolynomialSpecialisedComparisonCommand.hpp
StandardKBOrdering.o: ForwardMatchingSubstitutionCache.hpp
StandardKBOrdering.o: LocalUnification.hpp TermWeighting.hpp unification.hpp
StandardKBOrdering.o: iu_command.hpp
Tabulation.o: ../BlodKorv/jargon.hpp VampireKernelDebugFlags.hpp
Tabulation.o: ../BlodKorv/BlodKorvDebugFlags.hpp ../BlodKorv/GlobAlloc.hpp
Tabulation.o: ../BlodKorv/Debug.hpp ../BlodKorv/ObjDesc.hpp
Tabulation.o: ../BlodKorv/AllocationFailureHandling.hpp ../BlodKorv/Exit.hpp
Tabulation.o: ../BlodKorv/RuntimeError.hpp ../BlodKorv/DestructionMode.hpp
Tabulation.o: ../BlodKorv/debugMacros.hpp ../BlodKorv/Allocator.hpp
Tabulation.o: ../BlodKorv/BufferedAllocationStatistics.hpp
Tabulation.o: ../BlodKorv/GList.hpp ../BlodKorv/BitWord.hpp Term.hpp
Tabulation.o: VampireKernelConst.hpp ../BlodKorv/Comparison.hpp
Tabulation.o: ../BlodKorv/Stack.hpp ../BlodKorv/ExpandingStack.hpp
Tabulation.o: ../BlodKorv/Array.hpp ../BlodKorv/BitSet.hpp
Tabulation.o: ../BlodKorv/BitString.hpp ../BlodKorv/SmallHeaderSkipList.hpp
Tabulation.o: AutoFunctorPrecedence.hpp ../BlodKorv/Tuple.hpp
Tabulation.o: ../BlodKorv/GSortTree.hpp ClauseBackground.hpp LiteralList.hpp
Tabulation.o: DefinedHeaderTable.hpp Tabulation.hpp
Term.o: ../BlodKorv/jargon.hpp VampireKernelDebugFlags.hpp
Term.o: ../BlodKorv/BlodKorvDebugFlags.hpp VampireKernelConst.hpp
Term.o: ../BlodKorv/ClassDesc.hpp ../BlodKorv/Debug.hpp
Term.o: ../BlodKorv/ObjDesc.hpp ../BlodKorv/AllocationFailureHandling.hpp
Term.o: ../BlodKorv/Exit.hpp ../BlodKorv/RuntimeError.hpp
Term.o: ../BlodKorv/DestructionMode.hpp ../BlodKorv/debugMacros.hpp
Term.o: ../BlodKorv/Allocator.hpp
Term.o: ../BlodKorv/BufferedAllocationStatistics.hpp
Term.o: ../BlodKorv/Comparison.hpp ../BlodKorv/Stack.hpp
Term.o: ../BlodKorv/ExpandingStack.hpp ../BlodKorv/Array.hpp
Term.o: ../BlodKorv/BitSet.hpp ../BlodKorv/BitWord.hpp
Term.o: ../BlodKorv/BitString.hpp ../BlodKorv/SmallHeaderSkipList.hpp
Term.o: ../BlodKorv/Tuple.hpp ../BlodKorv/GSortTree.hpp ../BlodKorv/GList.hpp
TermBlock.o: VampireKernelDebugFlags.hpp ../BlodKorv/BlodKorvDebugFlags.hpp
TermBlock.o: ../BlodKorv/Stack.hpp ../BlodKorv/debugMacros.hpp
TermBlock.o: ../BlodKorv/Debug.hpp ../BlodKorv/RuntimeError.hpp
TermWeighting.o: ../BlodKorv/jargon.hpp VampireKernelDebugFlags.hpp
TermWeighting.o: ../BlodKorv/BlodKorvDebugFlags.hpp VampireKernelConst.hpp
TermWeighting.o: ../BlodKorv/LinearPolynomial.hpp
TermWeighting.o: ../BlodKorv/LinearCombination.hpp ../BlodKorv/Stack.hpp
TermWeighting.o: ../BlodKorv/debugMacros.hpp ../BlodKorv/Debug.hpp
TermWeighting.o: ../BlodKorv/Comparison.hpp ../BlodKorv/Math.hpp
TermWeighting.o: ../BlodKorv/SharedLinearPolynomial.hpp
TermWeighting.o: ../BlodKorv/GDiscTree.hpp ../BlodKorv/DestructionMode.hpp
TermWeighting.o: ../BlodKorv/ClassDesc.hpp ../BlodKorv/ObjDesc.hpp
TermWeighting.o: ../BlodKorv/AllocationFailureHandling.hpp
TermWeighting.o: ../BlodKorv/Exit.hpp ../BlodKorv/RuntimeError.hpp
TermWeighting.o: ../BlodKorv/Allocator.hpp
TermWeighting.o: ../BlodKorv/BufferedAllocationStatistics.hpp
TermWeighting.o: Flatterm.hpp Term.hpp ../BlodKorv/ExpandingStack.hpp
TermWeighting.o: ../BlodKorv/Array.hpp ../BlodKorv/BitSet.hpp
TermWeighting.o: ../BlodKorv/BitWord.hpp ../BlodKorv/BitString.hpp
TermWeighting.o: ../BlodKorv/SmallHeaderSkipList.hpp Signature.hpp
TermWeighting.o: ../BlodKorv/Tuple.hpp ../BlodKorv/GSortTree.hpp
TermWeighting.o: ../BlodKorv/GList.hpp ../BlodKorv/MultisetOfVariables.hpp
TermWeighting.o: variables.hpp InstanceRetrievalForDemodulation.hpp
TermWeighting.o: ../Gematogen/PathIndexingWithJoinsAndEarlyCleanUpForInstanceRetrieval.hpp
TermWeighting.o: ../BlodKorv/GlobalStopFlag.hpp ../Gematogen/Gematogen.hpp
TermWeighting.o: ../Gematogen/GematogenDebugFlags.hpp
TermWeighting.o: ../Gematogen/PathIndex.hpp
TermWeighting.o: ../BlodKorv/CompactNodeSmallHeaderSkipList.hpp
TermWeighting.o: ../BlodKorv/GIncCountingSort.hpp ../BlodKorv/Multiset.hpp
TmpLitList.o: TmpLitList.hpp ../BlodKorv/jargon.hpp
TmpLitList.o: VampireKernelDebugFlags.hpp ../BlodKorv/BlodKorvDebugFlags.hpp
TmpLitList.o: ../BlodKorv/Debug.hpp ../BlodKorv/ObjDesc.hpp
TmpLitList.o: ../BlodKorv/AllocationFailureHandling.hpp ../BlodKorv/Exit.hpp
TmpLitList.o: ../BlodKorv/RuntimeError.hpp ../BlodKorv/DestructionMode.hpp
TmpLitList.o: ../BlodKorv/debugMacros.hpp ../BlodKorv/Allocator.hpp
TmpLitList.o: ../BlodKorv/BufferedAllocationStatistics.hpp
TmpLitList.o: ../BlodKorv/Comparison.hpp ../BlodKorv/Stack.hpp
TmpLitList.o: ../BlodKorv/ExpandingStack.hpp ../BlodKorv/Array.hpp
TmpLitList.o: ../BlodKorv/BitSet.hpp ../BlodKorv/BitWord.hpp
TmpLitList.o: ../BlodKorv/BitString.hpp ../BlodKorv/SmallHeaderSkipList.hpp
TmpLitList.o: AutoFunctorPrecedence.hpp ../BlodKorv/Tuple.hpp
TmpLitList.o: ../BlodKorv/GSortTree.hpp ../BlodKorv/GList.hpp tmp_literal.hpp
TmpLitList.o: ../BlodKorv/Multiset.hpp WeightPolynomial.hpp
TmpLitList.o: ../BlodKorv/LinearPolynomial.hpp
TmpLitList.o: ../BlodKorv/LinearCombination.hpp ../BlodKorv/Math.hpp
TmpLitList.o: ../BlodKorv/SharedLinearPolynomial.hpp
TmpLitList.o: ../BlodKorv/GDiscTree.hpp TermWeighting.hpp Flatterm.hpp
TmpLitList.o: ../BlodKorv/MultisetOfVariables.hpp variables.hpp
TmpLitList.o: InstanceRetrievalForDemodulation.hpp
TmpLitList.o: ../Gematogen/PathIndexingWithJoinsAndEarlyCleanUpForInstanceRetrieval.hpp
TmpLitList.o: ../BlodKorv/GlobalStopFlag.hpp ../Gematogen/Gematogen.hpp
TmpLitList.o: ../Gematogen/GematogenDebugFlags.hpp ../Gematogen/PathIndex.hpp
TmpLitList.o: ../BlodKorv/CompactNodeSmallHeaderSkipList.hpp
TmpLitList.o: ../BlodKorv/GIncCountingSort.hpp SimplificationOrdering.hpp
UTerm.o: ../BlodKorv/RuntimeError.hpp ../BlodKorv/DestructionMode.hpp
UTerm.o: ../BlodKorv/BlodKorvDebugFlags.hpp ../BlodKorv/ClassDesc.hpp
UTerm.o: ../BlodKorv/AllocationFailureHandling.hpp ../BlodKorv/Exit.hpp
UTerm.o: ../BlodKorv/debugMacros.hpp ../BlodKorv/Allocator.hpp
UTerm.o: ../BlodKorv/BufferedAllocationStatistics.hpp
UTerm.o: ../BlodKorv/InfiniteStack.hpp ../BlodKorv/ObjectPool.hpp
UTerm.o: VampireKernelConst.hpp ../BlodKorv/Comparison.hpp
UTerm.o: ../BlodKorv/Stack.hpp ../BlodKorv/ExpandingStack.hpp
UTerm.o: ../BlodKorv/Array.hpp ../BlodKorv/BitSet.hpp ../BlodKorv/BitWord.hpp
UTerm.o: ../BlodKorv/BitString.hpp ../BlodKorv/SmallHeaderSkipList.hpp
UTerm.o: AutoFunctorPrecedence.hpp ../BlodKorv/Tuple.hpp
UTerm.o: ../BlodKorv/GSortTree.hpp ../BlodKorv/GList.hpp UVariable.hpp
UVariable.o: ../BlodKorv/DestructionMode.hpp ../BlodKorv/GlobAlloc.hpp
UVariable.o: ../BlodKorv/Debug.hpp ../BlodKorv/ObjDesc.hpp
UVariable.o: ../BlodKorv/AllocationFailureHandling.hpp ../BlodKorv/Exit.hpp
UVariable.o: ../BlodKorv/RuntimeError.hpp ../BlodKorv/debugMacros.hpp
UVariable.o: ../BlodKorv/Allocator.hpp
UVariable.o: ../BlodKorv/BufferedAllocationStatistics.hpp
UVariable.o: ../BlodKorv/InfiniteStack.hpp ../BlodKorv/ObjectPool.hpp
UVariable.o: VampireKernelDebugFlags.hpp
UniformTermWeighting.o: UniformTermWeighting.hpp ../BlodKorv/jargon.hpp
UniformTermWeighting.o: VampireKernelDebugFlags.hpp
UniformTermWeighting.o: ../BlodKorv/BlodKorvDebugFlags.hpp TermWeighting.hpp
UniformTermWeighting.o: ../BlodKorv/LinearCombination.hpp
UniformTermWeighting.o: ../BlodKorv/Stack.hpp ../BlodKorv/debugMacros.hpp
UniformTermWeighting.o: ../BlodKorv/Debug.hpp ../BlodKorv/Comparison.hpp
UniformTermWeighting.o: ../BlodKorv/Math.hpp
UniformTermWeighting.o: ../BlodKorv/SharedLinearPolynomial.hpp
UniformTermWeighting.o: ../BlodKorv/GDiscTree.hpp
UniformTermWeighting.o: ../BlodKorv/DestructionMode.hpp
UniformTermWeighting.o: ../BlodKorv/ClassDesc.hpp ../BlodKorv/ObjDesc.hpp
UniformTermWeighting.o: ../BlodKorv/AllocationFailureHandling.hpp
UniformTermWeighting.o: ../BlodKorv/Exit.hpp ../BlodKorv/RuntimeError.hpp
UniformTermWeighting.o: ../BlodKorv/Allocator.hpp
UniformTermWeighting.o: ../BlodKorv/BufferedAllocationStatistics.hpp
UniformTermWeighting.o: TermWeightType.hpp VampireKernel.hpp
UniformTermWeighting.o: ../BlodKorv/ExpandingStack.hpp ../BlodKorv/Array.hpp
UniformTermWeighting.o: ../BlodKorv/BitSet.hpp ../BlodKorv/BitWord.hpp
UniformTermWeighting.o: ../BlodKorv/BitString.hpp
UniformTermWeighting.o: ../BlodKorv/SmallHeaderSkipList.hpp Signature.hpp
UniformTermWeighting.o: AutoFunctorPrecedence.hpp ../BlodKorv/Tuple.hpp
UniformTermWeighting.o: ../BlodKorv/GSortTree.hpp ../BlodKorv/GList.hpp
UniformTermWeighting.o: ../BlodKorv/MultisetOfVariables.hpp variables.hpp
UniformTermWeighting.o: InstanceRetrievalForDemodulation.hpp
UniformTermWeighting.o: ../Gematogen/PathIndexingWithJoinsAndEarlyCleanUpForInstanceRetrieval.hpp
UniformTermWeighting.o: ../BlodKorv/GlobalStopFlag.hpp
UniformTermWeighting.o: ../Gematogen/Gematogen.hpp
UniformTermWeighting.o: ../Gematogen/GematogenDebugFlags.hpp
UniformTermWeighting.o: ../Gematogen/PathIndex.hpp
UniformTermWeighting.o: ../BlodKorv/CompactNodeSmallHeaderSkipList.hpp
UniformTermWeighting.o: ../BlodKorv/GIncCountingSort.hpp
UniformTermWeighting.o: ../BlodKorv/Multiset.hpp prefix.hpp
UniformTermWeighting.o: ForwardMatchingSubstitutionCache.hpp
UniformTermWeighting.o: LocalUnification.hpp
VampireKernel.o: ../BlodKorv/jargon.hpp ../BlodKorv/BlodKorvDebugFlags.hpp
VampireKernel.o: ../BlodKorv/Debug.hpp ../BlodKorv/ObjDesc.hpp
VampireKernel.o: ../BlodKorv/AllocationFailureHandling.hpp
VampireKernel.o: ../BlodKorv/Exit.hpp ../BlodKorv/RuntimeError.hpp
VampireKernel.o: ../BlodKorv/DestructionMode.hpp ../BlodKorv/debugMacros.hpp
VampireKernel.o: ../BlodKorv/Allocator.hpp
VampireKernel.o: ../BlodKorv/BufferedAllocationStatistics.hpp
VampireKernel.o: ../BlodKorv/GlobalClock.hpp ../BlodKorv/GlobalStopFlag.hpp
VampireKernel.o: ../BlodKorv/Array.hpp ../BlodKorv/Comparison.hpp
VampireKernel.o: SymbolInfo.hpp AutoFunctorPrecedence.hpp
VampireKernel.o: ../BlodKorv/Tuple.hpp ../BlodKorv/GSortTree.hpp
VampireKernel.o: ../BlodKorv/GList.hpp TermWeightType.hpp Clause.hpp
VampireKernel.o: ../BlodKorv/BitWord.hpp Term.hpp ../BlodKorv/Stack.hpp
VampireKernel.o: ../BlodKorv/ExpandingStack.hpp ../BlodKorv/BitSet.hpp
VampireKernel.o: ../BlodKorv/BitString.hpp
VampireKernel.o: ../BlodKorv/SmallHeaderSkipList.hpp ClauseBackground.hpp
VampireKernel.o: LiteralList.hpp DefinedHeaderTable.hpp ClauseQueue.hpp
VampireKernel.o: ../BlodKorv/FIFO.hpp ProblemProfile.hpp Tabulation.hpp
VampireKernel.o: Master.hpp clause_set.hpp kept.hpp sharing.hpp
VampireKernel.o: VampireKernelInterceptMacros.hpp ClauseAssembler.hpp
VampireKernel.o: LiteralSharing.hpp variables.hpp
VampireKernel.o: ../BlodKorv/MultisetOfVariables.hpp WeightPolynomial.hpp
VampireKernel.o: ../BlodKorv/LinearPolynomial.hpp
VampireKernel.o: ../BlodKorv/LinearCombination.hpp ../BlodKorv/Math.hpp
VampireKernel.o: ../BlodKorv/SharedLinearPolynomial.hpp
VampireKernel.o: ../BlodKorv/GDiscTree.hpp NewClause.hpp prefix.hpp
VampireKernel.o: OpenNewClause.hpp Flatterm.hpp tmp_literal.hpp
VampireKernel.o: ../BlodKorv/Multiset.hpp TermWeighting.hpp
VampireKernel.o: InstanceRetrievalForDemodulation.hpp
VampireKernel.o: ../Gematogen/PathIndexingWithJoinsAndEarlyCleanUpForInstanceRetrieval.hpp
VampireKernel.o: ../Gematogen/Gematogen.hpp
VampireKernel.o: ../Gematogen/GematogenDebugFlags.hpp
VampireKernel.o: ../Gematogen/PathIndex.hpp
VampireKernel.o: ../BlodKorv/CompactNodeSmallHeaderSkipList.hpp
VampireKernel.o: ../BlodKorv/GIncCountingSort.hpp SimplificationOrdering.hpp
VampireKernel.o: TmpLitList.hpp ForwardDemodulation.hpp
VampireKernel.o: ../Gematogen/ForwardMatchingIndexTree.hpp
VampireKernel.o: ../BlodKorv/EquivalenceRelation.hpp
VampireKernel.o: ../Gematogen/ForwardMatchingIndex.hpp
VampireKernel.o: ../BlodKorv/ExpandingMultiset.hpp
VampireKernel.o: ForwardMatchingSubstitutionCache.hpp
VampireKernel.o: PagedPassiveClauseStorage.hpp
VampireKernel.o: ../BlodKorv/PageQueueDisciplineStorage.hpp Splitting.hpp
VampireKernel.o: DivIntoComp.hpp ForwardSubsumptionFrontEnd.hpp
VampireKernel.o: forward_subsumption.hpp fs_code.hpp fs_command.hpp
VampireKernel.o: old_fs_code.hpp ../BlodKorv/Queue.hpp
VampireKernel.o: ../BlodKorv/ObjectPool.hpp ComponentName.hpp
VampireKernel.o: InferenceDispatcher.hpp or_index.hpp unification.hpp
VampireKernel.o: iu_command.hpp BackParaCandidates.hpp
VampireKernel.o: ../BlodKorv/SkipList.hpp binary_resolution.hpp
VampireKernel.o: premise_factoring.hpp or_premise.hpp or_machine.hpp
VampireKernel.o: iu_machine.hpp iu_translator.hpp weight_formula.hpp
VampireKernel.o: forward_superposition.hpp para_machine.hpp eq_factoring.hpp
VampireKernel.o: backward_superposition.hpp BackwardSubsumption.hpp
VampireKernel.o: BSIndex.hpp ../Gematogen/PathIndexWithOpenPaths.hpp
VampireKernel.o: BSMachine.hpp ../BlodKorv/StaticCellArray.hpp
VampireKernel.o: ClauseNumIndex.hpp SubsumptionCheck.hpp
VampireKernel.o: SimpleSubstitution.hpp BackDemodulation.hpp
VampireKernel.o: LiteralSelection.hpp LiteralOrdering.hpp
VampireKernel.o: UniformTermWeighting.hpp NonuniformConstantTermWeighting.hpp
VampireKernel.o: NonrecursiveKBOrdering.hpp
VampireKernel.o: WeightPolynomialSpecialisedComparisonCommand.hpp
VampireKernel.o: StandardKBOrdering.hpp LocalUnification.hpp
VampireKernel.o: InferenceTree.hpp BuiltInTheoryDispatcher.hpp
WeightPolynomial.o: ../BlodKorv/jargon.hpp VampireKernelDebugFlags.hpp
WeightPolynomial.o: ../BlodKorv/BlodKorvDebugFlags.hpp VampireKernelConst.hpp
WeightPolynomial.o: ../BlodKorv/LinearPolynomial.hpp
WeightPolynomial.o: ../BlodKorv/LinearCombination.hpp ../BlodKorv/Stack.hpp
WeightPolynomial.o: ../BlodKorv/debugMacros.hpp ../BlodKorv/Debug.hpp
WeightPolynomial.o: ../BlodKorv/Comparison.hpp ../BlodKorv/Math.hpp
WeightPolynomial.o: ../BlodKorv/SharedLinearPolynomial.hpp
WeightPolynomial.o: ../BlodKorv/GDiscTree.hpp ../BlodKorv/DestructionMode.hpp
WeightPolynomial.o: ../BlodKorv/ClassDesc.hpp ../BlodKorv/ObjDesc.hpp
WeightPolynomial.o: ../BlodKorv/AllocationFailureHandling.hpp
WeightPolynomial.o: ../BlodKorv/Exit.hpp ../BlodKorv/RuntimeError.hpp
WeightPolynomial.o: ../BlodKorv/Allocator.hpp
WeightPolynomial.o: ../BlodKorv/BufferedAllocationStatistics.hpp
WeightPolynomial.o: WeightPolynomialSpecialisedComparisonCommand.hpp
WeightPolynomialSpecialisedComparisonCommand.o: WeightPolynomialSpecialisedComparisonCommand.hpp
WeightPolynomialSpecialisedComparisonCommand.o: ../BlodKorv/jargon.hpp
WeightPolynomialSpecialisedComparisonCommand.o: VampireKernelDebugFlags.hpp
WeightPolynomialSpecialisedComparisonCommand.o: ../BlodKorv/BlodKorvDebugFlags.hpp
WeightPolynomialSpecialisedComparisonCommand.o: VampireKernelConst.hpp
WeightPolynomialSpecialisedComparisonCommand.o: WeightPolynomial.hpp
WeightPolynomialSpecialisedComparisonCommand.o: ../BlodKorv/LinearPolynomial.hpp
WeightPolynomialSpecialisedComparisonCommand.o: ../BlodKorv/LinearCombination.hpp
WeightPolynomialSpecialisedComparisonCommand.o: ../BlodKorv/Stack.hpp
WeightPolynomialSpecialisedComparisonCommand.o: ../BlodKorv/debugMacros.hpp
WeightPolynomialSpecialisedComparisonCommand.o: ../BlodKorv/Debug.hpp
WeightPolynomialSpecialisedComparisonCommand.o: ../BlodKorv/Comparison.hpp
WeightPolynomialSpecialisedComparisonCommand.o: ../BlodKorv/Math.hpp
WeightPolynomialSpecialisedComparisonCommand.o: ../BlodKorv/SharedLinearPolynomial.hpp
WeightPolynomialSpecialisedComparisonCommand.o: ../BlodKorv/GDiscTree.hpp
WeightPolynomialSpecialisedComparisonCommand.o: ../BlodKorv/DestructionMode.hpp
WeightPolynomialSpecialisedComparisonCommand.o: ../BlodKorv/GlobAlloc.hpp
WeightPolynomialSpecialisedComparisonCommand.o: ../BlodKorv/ClassDesc.hpp
WeightPolynomialSpecialisedComparisonCommand.o: ../BlodKorv/ObjDesc.hpp
WeightPolynomialSpecialisedComparisonCommand.o: ../BlodKorv/AllocationFailureHandling.hpp
WeightPolynomialSpecialisedComparisonCommand.o: ../BlodKorv/Exit.hpp
WeightPolynomialSpecialisedComparisonCommand.o: ../BlodKorv/RuntimeError.hpp
WeightPolynomialSpecialisedComparisonCommand.o: ../BlodKorv/Allocator.hpp
WeightPolynomialSpecialisedComparisonCommand.o: ../BlodKorv/BufferedAllocationStatistics.hpp
WeightPolynomialSpecialisedComparisonCommand.o: TermWeightType.hpp
WeightPolynomialSpecialisedComparisonCommand.o: VampireKernel.hpp
backward_superposition.o: ../BlodKorv/jargon.hpp VampireKernelDebugFlags.hpp
backward_superposition.o: ../BlodKorv/BlodKorvDebugFlags.hpp
backward_superposition.o: para_machine.hpp iu_command.hpp prefix.hpp Term.hpp
backward_superposition.o: ../BlodKorv/ClassDesc.hpp ../BlodKorv/Debug.hpp
backward_superposition.o: ../BlodKorv/ObjDesc.hpp
backward_superposition.o: ../BlodKorv/AllocationFailureHandling.hpp
backward_superposition.o: ../BlodKorv/Exit.hpp ../BlodKorv/RuntimeError.hpp
backward_superposition.o: ../BlodKorv/debugMacros.hpp
backward_superposition.o: ../BlodKorv/Allocator.hpp
backward_superposition.o: ../BlodKorv/BufferedAllocationStatistics.hpp
backward_superposition.o: ../BlodKorv/Comparison.hpp ../BlodKorv/Stack.hpp
backward_superposition.o: ../BlodKorv/ExpandingStack.hpp
backward_superposition.o: ../BlodKorv/Array.hpp ../BlodKorv/BitSet.hpp
backward_superposition.o: ../BlodKorv/BitWord.hpp ../BlodKorv/BitString.hpp
backward_superposition.o: ../BlodKorv/SmallHeaderSkipList.hpp
backward_superposition.o: TermWeightType.hpp VampireKernel.hpp
backward_superposition.o: SymbolInfo.hpp AutoFunctorPrecedence.hpp
backward_superposition.o: ../BlodKorv/Tuple.hpp ../BlodKorv/GSortTree.hpp
backward_superposition.o: ../BlodKorv/GList.hpp variables.hpp
backward_superposition.o: ../BlodKorv/MultisetOfVariables.hpp
backward_superposition.o: WeightPolynomial.hpp
backward_superposition.o: ../BlodKorv/LinearPolynomial.hpp
backward_superposition.o: ../BlodKorv/LinearCombination.hpp
backward_superposition.o: ../BlodKorv/Math.hpp
backward_superposition.o: ../BlodKorv/SharedLinearPolynomial.hpp
backward_superposition.o: ../BlodKorv/GDiscTree.hpp iu_machine.hpp
backward_superposition.o: iu_translator.hpp or_index.hpp unification.hpp
backward_superposition.o: BackParaCandidates.hpp ../BlodKorv/SkipList.hpp
backward_superposition.o: SimplificationOrdering.hpp
backward_superposition.o: InstanceRetrievalForDemodulation.hpp
backward_superposition.o: ../Gematogen/PathIndexingWithJoinsAndEarlyCleanUpForInstanceRetrieval.hpp
backward_superposition.o: ../BlodKorv/GlobalStopFlag.hpp
backward_superposition.o: ../Gematogen/Gematogen.hpp
backward_superposition.o: ../Gematogen/GematogenDebugFlags.hpp
backward_superposition.o: ../Gematogen/PathIndex.hpp
backward_superposition.o: ../BlodKorv/CompactNodeSmallHeaderSkipList.hpp
backward_superposition.o: ../BlodKorv/GIncCountingSort.hpp
backward_superposition.o: ../BlodKorv/Multiset.hpp
backward_superposition.o: VampireKernelInterceptMacros.hpp NewClause.hpp
backward_superposition.o: OpenNewClause.hpp Flatterm.hpp tmp_literal.hpp
backward_superposition.o: TermWeighting.hpp TmpLitList.hpp
backward_superposition.o: ClauseBackground.hpp ForwardDemodulation.hpp
backward_superposition.o: ../Gematogen/ForwardMatchingIndexTree.hpp
backward_superposition.o: ../BlodKorv/EquivalenceRelation.hpp
backward_superposition.o: ../Gematogen/ForwardMatchingIndex.hpp
backward_superposition.o: ../BlodKorv/ExpandingMultiset.hpp
backward_superposition.o: ForwardMatchingSubstitutionCache.hpp
backward_superposition.o: ClauseQueue.hpp ../BlodKorv/FIFO.hpp Clause.hpp
backward_superposition.o: LiteralList.hpp DefinedHeaderTable.hpp
backward_superposition.o: PagedPassiveClauseStorage.hpp
backward_superposition.o: ../BlodKorv/PageQueueDisciplineStorage.hpp
backward_superposition.o: Splitting.hpp DivIntoComp.hpp weight_formula.hpp
binary_resolution.o: binary_resolution.hpp ../BlodKorv/jargon.hpp
binary_resolution.o: VampireKernelDebugFlags.hpp
binary_resolution.o: ../BlodKorv/BlodKorvDebugFlags.hpp premise_factoring.hpp
binary_resolution.o: ../BlodKorv/ObjDesc.hpp
binary_resolution.o: ../BlodKorv/AllocationFailureHandling.hpp
binary_resolution.o: ../BlodKorv/Exit.hpp ../BlodKorv/RuntimeError.hpp
binary_resolution.o: ../BlodKorv/DestructionMode.hpp
binary_resolution.o: ../BlodKorv/debugMacros.hpp ../BlodKorv/Allocator.hpp
binary_resolution.o: ../BlodKorv/BufferedAllocationStatistics.hpp
binary_resolution.o: ../BlodKorv/GList.hpp ../BlodKorv/BitWord.hpp Term.hpp
binary_resolution.o: VampireKernelConst.hpp ../BlodKorv/Comparison.hpp
binary_resolution.o: ../BlodKorv/Stack.hpp ../BlodKorv/ExpandingStack.hpp
binary_resolution.o: ../BlodKorv/Array.hpp ../BlodKorv/BitSet.hpp
binary_resolution.o: ../BlodKorv/BitString.hpp
binary_resolution.o: ../BlodKorv/SmallHeaderSkipList.hpp TermWeightType.hpp
binary_resolution.o: AutoFunctorPrecedence.hpp ../BlodKorv/Tuple.hpp
binary_resolution.o: ../BlodKorv/GSortTree.hpp ClauseBackground.hpp
binary_resolution.o: LiteralList.hpp DefinedHeaderTable.hpp prefix.hpp
binary_resolution.o: variables.hpp ../BlodKorv/MultisetOfVariables.hpp
binary_resolution.o: WeightPolynomial.hpp ../BlodKorv/LinearPolynomial.hpp
binary_resolution.o: ../BlodKorv/LinearCombination.hpp ../BlodKorv/Math.hpp
binary_resolution.o: ../BlodKorv/SharedLinearPolynomial.hpp
binary_resolution.o: ../BlodKorv/GDiscTree.hpp or_index.hpp unification.hpp
binary_resolution.o: iu_command.hpp BackParaCandidates.hpp
binary_resolution.o: ../BlodKorv/SkipList.hpp SimplificationOrdering.hpp
binary_resolution.o: InstanceRetrievalForDemodulation.hpp
binary_resolution.o: ../Gematogen/PathIndexingWithJoinsAndEarlyCleanUpForInstanceRetrieval.hpp
binary_resolution.o: ../BlodKorv/GlobalStopFlag.hpp
binary_resolution.o: ../Gematogen/Gematogen.hpp
binary_resolution.o: ../Gematogen/GematogenDebugFlags.hpp
binary_resolution.o: ../Gematogen/PathIndex.hpp
binary_resolution.o: ../BlodKorv/CompactNodeSmallHeaderSkipList.hpp
binary_resolution.o: ../BlodKorv/GIncCountingSort.hpp
binary_resolution.o: ../BlodKorv/Multiset.hpp
binary_resolution.o: VampireKernelInterceptMacros.hpp NewClause.hpp
binary_resolution.o: OpenNewClause.hpp Flatterm.hpp tmp_literal.hpp
binary_resolution.o: TermWeighting.hpp TmpLitList.hpp ForwardDemodulation.hpp
binary_resolution.o: ../Gematogen/ForwardMatchingIndexTree.hpp
binary_resolution.o: ../BlodKorv/EquivalenceRelation.hpp
binary_resolution.o: ../Gematogen/ForwardMatchingIndex.hpp
binary_resolution.o: ../BlodKorv/ExpandingMultiset.hpp
binary_resolution.o: ForwardMatchingSubstitutionCache.hpp ClauseQueue.hpp
binary_resolution.o: ../BlodKorv/FIFO.hpp PagedPassiveClauseStorage.hpp
binary_resolution.o: ../BlodKorv/PageQueueDisciplineStorage.hpp Splitting.hpp
binary_resolution.o: DivIntoComp.hpp or_machine.hpp iu_machine.hpp
binary_resolution.o: iu_translator.hpp weight_formula.hpp
forward_subsumption.o: ../BlodKorv/jargon.hpp VampireKernelDebugFlags.hpp
forward_subsumption.o: ../BlodKorv/BlodKorvDebugFlags.hpp
forward_subsumption.o: ../BlodKorv/MultisetOfVariables.hpp
forward_subsumption.o: ../BlodKorv/Stack.hpp ../BlodKorv/debugMacros.hpp
forward_subsumption.o: ../BlodKorv/Debug.hpp ../BlodKorv/Comparison.hpp
forward_subsumption.o: ../BlodKorv/EquivalenceRelation.hpp fs_command.hpp
forward_subsumption.o: ../BlodKorv/ClassDesc.hpp ../BlodKorv/ObjDesc.hpp
forward_subsumption.o: ../BlodKorv/AllocationFailureHandling.hpp
forward_subsumption.o: ../BlodKorv/Exit.hpp ../BlodKorv/RuntimeError.hpp
forward_subsumption.o: ../BlodKorv/DestructionMode.hpp
forward_subsumption.o: ../BlodKorv/Allocator.hpp
forward_subsumption.o: ../BlodKorv/BufferedAllocationStatistics.hpp Term.hpp
forward_subsumption.o: ../BlodKorv/ExpandingStack.hpp ../BlodKorv/Array.hpp
forward_subsumption.o: ../BlodKorv/BitSet.hpp ../BlodKorv/BitWord.hpp
forward_subsumption.o: ../BlodKorv/BitString.hpp
forward_subsumption.o: ../BlodKorv/SmallHeaderSkipList.hpp TermWeightType.hpp
forward_subsumption.o: AutoFunctorPrecedence.hpp ../BlodKorv/Tuple.hpp
forward_subsumption.o: ../BlodKorv/GSortTree.hpp ../BlodKorv/GList.hpp
forward_subsumption.o: old_fs_code.hpp ../BlodKorv/Queue.hpp TmpLitList.hpp
forward_subsumption.o: tmp_literal.hpp ../BlodKorv/Multiset.hpp
forward_subsumption.o: WeightPolynomial.hpp ../BlodKorv/LinearPolynomial.hpp
forward_subsumption.o: ../BlodKorv/LinearCombination.hpp ../BlodKorv/Math.hpp
forward_subsumption.o: ../BlodKorv/SharedLinearPolynomial.hpp
forward_subsumption.o: ../BlodKorv/GDiscTree.hpp TermWeighting.hpp
forward_subsumption.o: Flatterm.hpp variables.hpp
forward_subsumption.o: InstanceRetrievalForDemodulation.hpp
forward_subsumption.o: ../Gematogen/PathIndexingWithJoinsAndEarlyCleanUpForInstanceRetrieval.hpp
forward_subsumption.o: ../BlodKorv/GlobalStopFlag.hpp
forward_subsumption.o: ../Gematogen/Gematogen.hpp
forward_subsumption.o: ../Gematogen/GematogenDebugFlags.hpp
forward_subsumption.o: ../Gematogen/PathIndex.hpp
forward_subsumption.o: ../BlodKorv/CompactNodeSmallHeaderSkipList.hpp
forward_subsumption.o: ../BlodKorv/GIncCountingSort.hpp
forward_subsumption.o: SimplificationOrdering.hpp ../BlodKorv/ObjectPool.hpp
forward_subsumption.o: Clause.hpp ClauseBackground.hpp LiteralList.hpp
forward_subsumption.o: DefinedHeaderTable.hpp
forward_superposition.o: forward_superposition.hpp ../BlodKorv/jargon.hpp
forward_superposition.o: VampireKernelDebugFlags.hpp
forward_superposition.o: ../BlodKorv/BlodKorvDebugFlags.hpp
forward_superposition.o: ../BlodKorv/debugMacros.hpp ../BlodKorv/Debug.hpp
forward_superposition.o: prefix.hpp Term.hpp ../BlodKorv/GlobAlloc.hpp
forward_superposition.o: ../BlodKorv/ObjDesc.hpp
forward_superposition.o: ../BlodKorv/AllocationFailureHandling.hpp
forward_superposition.o: ../BlodKorv/Exit.hpp ../BlodKorv/RuntimeError.hpp
forward_superposition.o: ../BlodKorv/DestructionMode.hpp
forward_superposition.o: ../BlodKorv/Allocator.hpp
forward_superposition.o: ../BlodKorv/BufferedAllocationStatistics.hpp
forward_superposition.o: ../BlodKorv/Comparison.hpp
forward_superposition.o: ../BlodKorv/ExpandingStack.hpp ../BlodKorv/Array.hpp
forward_superposition.o: ../BlodKorv/BitSet.hpp ../BlodKorv/BitWord.hpp
forward_superposition.o: ../BlodKorv/BitString.hpp
forward_superposition.o: ../BlodKorv/SmallHeaderSkipList.hpp
forward_superposition.o: TermWeightType.hpp VampireKernel.hpp
forward_superposition.o: SymbolInfo.hpp AutoFunctorPrecedence.hpp
forward_superposition.o: ../BlodKorv/Tuple.hpp ../BlodKorv/GSortTree.hpp
forward_superposition.o: ../BlodKorv/GList.hpp variables.hpp
forward_superposition.o: ../BlodKorv/MultisetOfVariables.hpp
forward_superposition.o: WeightPolynomial.hpp
forward_superposition.o: ../BlodKorv/LinearPolynomial.hpp
forward_superposition.o: ../BlodKorv/LinearCombination.hpp
forward_superposition.o: ../BlodKorv/Math.hpp
forward_superposition.o: ../BlodKorv/SharedLinearPolynomial.hpp
forward_superposition.o: ../BlodKorv/GDiscTree.hpp iu_machine.hpp
forward_superposition.o: iu_translator.hpp or_index.hpp unification.hpp
forward_superposition.o: BackParaCandidates.hpp ../BlodKorv/SkipList.hpp
forward_superposition.o: SimplificationOrdering.hpp
forward_superposition.o: InstanceRetrievalForDemodulation.hpp
forward_superposition.o: ../Gematogen/PathIndexingWithJoinsAndEarlyCleanUpForInstanceRetrieval.hpp
forward_superposition.o: ../BlodKorv/GlobalStopFlag.hpp
forward_superposition.o: ../Gematogen/Gematogen.hpp
forward_superposition.o: ../Gematogen/GematogenDebugFlags.hpp
forward_superposition.o: ../Gematogen/PathIndex.hpp
forward_superposition.o: ../BlodKorv/CompactNodeSmallHeaderSkipList.hpp
forward_superposition.o: ../BlodKorv/GIncCountingSort.hpp
forward_superposition.o: ../BlodKorv/Multiset.hpp
forward_superposition.o: VampireKernelInterceptMacros.hpp NewClause.hpp
forward_superposition.o: OpenNewClause.hpp Flatterm.hpp tmp_literal.hpp
forward_superposition.o: TermWeighting.hpp TmpLitList.hpp
forward_superposition.o: ClauseBackground.hpp ForwardDemodulation.hpp
forward_superposition.o: ../Gematogen/ForwardMatchingIndexTree.hpp
forward_superposition.o: ../BlodKorv/EquivalenceRelation.hpp
forward_superposition.o: ../Gematogen/ForwardMatchingIndex.hpp
forward_superposition.o: ../BlodKorv/ExpandingMultiset.hpp
forward_superposition.o: ForwardMatchingSubstitutionCache.hpp ClauseQueue.hpp
forward_superposition.o: ../BlodKorv/FIFO.hpp Clause.hpp LiteralList.hpp
forward_superposition.o: DefinedHeaderTable.hpp PagedPassiveClauseStorage.hpp
forward_superposition.o: ../BlodKorv/PageQueueDisciplineStorage.hpp
forward_superposition.o: Splitting.hpp DivIntoComp.hpp weight_formula.hpp
fs_code.o: fs_code.hpp ../BlodKorv/jargon.hpp VampireKernelDebugFlags.hpp
fs_code.o: ../BlodKorv/BlodKorvDebugFlags.hpp VampireKernelConst.hpp
fs_code.o: ../BlodKorv/MultisetOfVariables.hpp ../BlodKorv/Stack.hpp
fs_code.o: ../BlodKorv/debugMacros.hpp ../BlodKorv/Debug.hpp
fs_code.o: ../BlodKorv/EquivalenceRelation.hpp fs_command.hpp
fs_code.o: ../BlodKorv/ClassDesc.hpp ../BlodKorv/ObjDesc.hpp
fs_code.o: ../BlodKorv/AllocationFailureHandling.hpp ../BlodKorv/Exit.hpp
fs_code.o: ../BlodKorv/RuntimeError.hpp ../BlodKorv/DestructionMode.hpp
fs_code.o: ../BlodKorv/Allocator.hpp
fs_code.o: ../BlodKorv/BufferedAllocationStatistics.hpp Term.hpp
fs_code.o: ../BlodKorv/ExpandingStack.hpp ../BlodKorv/Array.hpp
fs_code.o: ../BlodKorv/BitSet.hpp ../BlodKorv/BitWord.hpp
fs_code.o: ../BlodKorv/BitString.hpp ../BlodKorv/SmallHeaderSkipList.hpp
fs_code.o: AutoFunctorPrecedence.hpp ../BlodKorv/Tuple.hpp
fs_code.o: ../BlodKorv/GSortTree.hpp ../BlodKorv/GList.hpp old_fs_code.hpp
fs_code.o: ../BlodKorv/Queue.hpp TmpLitList.hpp tmp_literal.hpp
fs_code.o: ../BlodKorv/Multiset.hpp WeightPolynomial.hpp
fs_code.o: ../BlodKorv/LinearPolynomial.hpp ../BlodKorv/LinearCombination.hpp
fs_code.o: ../BlodKorv/Math.hpp ../BlodKorv/SharedLinearPolynomial.hpp
fs_code.o: ../BlodKorv/GDiscTree.hpp TermWeighting.hpp Flatterm.hpp
fs_code.o: variables.hpp InstanceRetrievalForDemodulation.hpp
fs_code.o: ../Gematogen/PathIndexingWithJoinsAndEarlyCleanUpForInstanceRetrieval.hpp
fs_code.o: ../BlodKorv/GlobalStopFlag.hpp ../Gematogen/Gematogen.hpp
fs_code.o: ../Gematogen/GematogenDebugFlags.hpp ../Gematogen/PathIndex.hpp
fs_code.o: ../BlodKorv/CompactNodeSmallHeaderSkipList.hpp
fs_code.o: ../BlodKorv/GIncCountingSort.hpp SimplificationOrdering.hpp
fs_code.o: Clause.hpp ClauseBackground.hpp LiteralList.hpp
fs_code.o: DefinedHeaderTable.hpp
fs_command.o: fs_command.hpp ../BlodKorv/jargon.hpp
fs_command.o: VampireKernelDebugFlags.hpp ../BlodKorv/BlodKorvDebugFlags.hpp
fs_command.o: ../BlodKorv/Debug.hpp ../BlodKorv/ObjDesc.hpp
fs_command.o: ../BlodKorv/AllocationFailureHandling.hpp ../BlodKorv/Exit.hpp
fs_command.o: ../BlodKorv/RuntimeError.hpp ../BlodKorv/DestructionMode.hpp
fs_command.o: ../BlodKorv/debugMacros.hpp ../BlodKorv/Allocator.hpp
fs_command.o: ../BlodKorv/BufferedAllocationStatistics.hpp Term.hpp
fs_command.o: ../BlodKorv/Comparison.hpp ../BlodKorv/Stack.hpp
fs_command.o: ../BlodKorv/ExpandingStack.hpp ../BlodKorv/Array.hpp
fs_command.o: ../BlodKorv/BitSet.hpp ../BlodKorv/BitWord.hpp
fs_command.o: ../BlodKorv/BitString.hpp ../BlodKorv/SmallHeaderSkipList.hpp
fs_command.o: AutoFunctorPrecedence.hpp ../BlodKorv/Tuple.hpp
fs_command.o: ../BlodKorv/GSortTree.hpp ../BlodKorv/GList.hpp
iu_command.o: iu_command.hpp ../BlodKorv/jargon.hpp
iu_command.o: VampireKernelDebugFlags.hpp ../BlodKorv/BlodKorvDebugFlags.hpp
iu_command.o: ../BlodKorv/ClassDesc.hpp ../BlodKorv/Debug.hpp
iu_command.o: ../BlodKorv/ObjDesc.hpp
iu_command.o: ../BlodKorv/AllocationFailureHandling.hpp ../BlodKorv/Exit.hpp
iu_command.o: ../BlodKorv/RuntimeError.hpp ../BlodKorv/DestructionMode.hpp
iu_command.o: ../BlodKorv/debugMacros.hpp ../BlodKorv/Allocator.hpp
iu_command.o: ../BlodKorv/BufferedAllocationStatistics.hpp
iu_command.o: ../BlodKorv/Comparison.hpp ../BlodKorv/Stack.hpp
iu_command.o: ../BlodKorv/ExpandingStack.hpp ../BlodKorv/Array.hpp
iu_command.o: ../BlodKorv/BitSet.hpp ../BlodKorv/BitWord.hpp
iu_command.o: ../BlodKorv/BitString.hpp ../BlodKorv/SmallHeaderSkipList.hpp
iu_command.o: AutoFunctorPrecedence.hpp ../BlodKorv/Tuple.hpp
iu_command.o: ../BlodKorv/GSortTree.hpp ../BlodKorv/GList.hpp variables.hpp
iu_command.o: ../BlodKorv/MultisetOfVariables.hpp WeightPolynomial.hpp
iu_command.o: ../BlodKorv/LinearPolynomial.hpp
iu_command.o: ../BlodKorv/LinearCombination.hpp ../BlodKorv/Math.hpp
iu_command.o: ../BlodKorv/SharedLinearPolynomial.hpp
iu_command.o: ../BlodKorv/GDiscTree.hpp
iu_machine.o: ../BlodKorv/GlobalStopFlag.hpp ../BlodKorv/jargon.hpp
iu_machine.o: ../BlodKorv/BlodKorvDebugFlags.hpp
iu_machine.o: ../BlodKorv/DestructionMode.hpp ../BlodKorv/debugMacros.hpp
iu_machine.o: VampireKernelDebugFlags.hpp VampireKernelConst.hpp
iu_machine.o: ../BlodKorv/ClassDesc.hpp ../BlodKorv/ObjDesc.hpp
iu_machine.o: ../BlodKorv/AllocationFailureHandling.hpp ../BlodKorv/Exit.hpp
iu_machine.o: ../BlodKorv/RuntimeError.hpp ../BlodKorv/Allocator.hpp
iu_machine.o: ../BlodKorv/BufferedAllocationStatistics.hpp
iu_machine.o: ../BlodKorv/Comparison.hpp ../BlodKorv/Stack.hpp
iu_machine.o: ../BlodKorv/ExpandingStack.hpp ../BlodKorv/Array.hpp
iu_machine.o: ../BlodKorv/BitSet.hpp ../BlodKorv/BitWord.hpp
iu_machine.o: ../BlodKorv/BitString.hpp ../BlodKorv/SmallHeaderSkipList.hpp
iu_machine.o: AutoFunctorPrecedence.hpp ../BlodKorv/Tuple.hpp
iu_machine.o: ../BlodKorv/GSortTree.hpp ../BlodKorv/GList.hpp variables.hpp
iu_machine.o: ../BlodKorv/MultisetOfVariables.hpp WeightPolynomial.hpp
iu_machine.o: ../BlodKorv/LinearPolynomial.hpp
iu_machine.o: ../BlodKorv/LinearCombination.hpp ../BlodKorv/Math.hpp
iu_machine.o: ../BlodKorv/SharedLinearPolynomial.hpp
iu_machine.o: ../BlodKorv/GDiscTree.hpp iu_translator.hpp or_index.hpp
iu_machine.o: unification.hpp BackParaCandidates.hpp ../BlodKorv/SkipList.hpp
iu_machine.o: SimplificationOrdering.hpp InstanceRetrievalForDemodulation.hpp
iu_machine.o: ../Gematogen/PathIndexingWithJoinsAndEarlyCleanUpForInstanceRetrieval.hpp
iu_machine.o: ../Gematogen/Gematogen.hpp ../Gematogen/GematogenDebugFlags.hpp
iu_machine.o: ../Gematogen/PathIndex.hpp
iu_machine.o: ../BlodKorv/CompactNodeSmallHeaderSkipList.hpp
iu_machine.o: ../BlodKorv/GIncCountingSort.hpp ../BlodKorv/Multiset.hpp
iu_machine.o: VampireKernelInterceptMacros.hpp
kept.o: VampireKernelDebugFlags.hpp ../BlodKorv/BlodKorvDebugFlags.hpp
kept.o: ../BlodKorv/ClassDesc.hpp ../BlodKorv/Debug.hpp
kept.o: ../BlodKorv/ObjDesc.hpp ../BlodKorv/AllocationFailureHandling.hpp
kept.o: ../BlodKorv/Exit.hpp ../BlodKorv/RuntimeError.hpp
kept.o: ../BlodKorv/DestructionMode.hpp ../BlodKorv/debugMacros.hpp
kept.o: ../BlodKorv/Allocator.hpp
kept.o: ../BlodKorv/BufferedAllocationStatistics.hpp ../BlodKorv/GList.hpp
kept.o: ../BlodKorv/BitWord.hpp Term.hpp ../BlodKorv/Comparison.hpp
kept.o: ../BlodKorv/Stack.hpp ../BlodKorv/ExpandingStack.hpp
kept.o: ../BlodKorv/Array.hpp ../BlodKorv/BitSet.hpp
kept.o: ../BlodKorv/BitString.hpp ../BlodKorv/SmallHeaderSkipList.hpp
kept.o: ../BlodKorv/Tuple.hpp ../BlodKorv/GSortTree.hpp ClauseBackground.hpp
kept.o: LiteralList.hpp DefinedHeaderTable.hpp
old_fs_code.o: VampireKernelDebugFlags.hpp ../BlodKorv/BlodKorvDebugFlags.hpp
old_fs_code.o: ../BlodKorv/MultisetOfVariables.hpp ../BlodKorv/Stack.hpp
old_fs_code.o: ../BlodKorv/debugMacros.hpp ../BlodKorv/Debug.hpp
old_fs_code.o: ../BlodKorv/Comparison.hpp fs_command.hpp
old_fs_code.o: ../BlodKorv/ClassDesc.hpp ../BlodKorv/ObjDesc.hpp
old_fs_code.o: ../BlodKorv/AllocationFailureHandling.hpp ../BlodKorv/Exit.hpp
old_fs_code.o: ../BlodKorv/RuntimeError.hpp ../BlodKorv/DestructionMode.hpp
old_fs_code.o: ../BlodKorv/Allocator.hpp
old_fs_code.o: ../BlodKorv/BufferedAllocationStatistics.hpp Term.hpp
old_fs_code.o: ../BlodKorv/ExpandingStack.hpp ../BlodKorv/Array.hpp
old_fs_code.o: ../BlodKorv/BitSet.hpp ../BlodKorv/BitWord.hpp
old_fs_code.o: ../BlodKorv/BitString.hpp ../BlodKorv/SmallHeaderSkipList.hpp
old_fs_code.o: AutoFunctorPrecedence.hpp ../BlodKorv/Tuple.hpp
old_fs_code.o: ../BlodKorv/GSortTree.hpp ../BlodKorv/GList.hpp
old_fs_code.o: ../BlodKorv/Queue.hpp Clause.hpp ClauseBackground.hpp
old_fs_code.o: LiteralList.hpp DefinedHeaderTable.hpp Flatterm.hpp
old_fs_code.o: variables.hpp WeightPolynomial.hpp
old_fs_code.o: ../BlodKorv/LinearPolynomial.hpp
old_fs_code.o: ../BlodKorv/LinearCombination.hpp ../BlodKorv/Math.hpp
old_fs_code.o: ../BlodKorv/SharedLinearPolynomial.hpp
old_fs_code.o: ../BlodKorv/GDiscTree.hpp TmpLitList.hpp tmp_literal.hpp
old_fs_code.o: ../BlodKorv/Multiset.hpp TermWeighting.hpp
old_fs_code.o: InstanceRetrievalForDemodulation.hpp
old_fs_code.o: ../Gematogen/PathIndexingWithJoinsAndEarlyCleanUpForInstanceRetrieval.hpp
old_fs_code.o: ../BlodKorv/GlobalStopFlag.hpp ../Gematogen/Gematogen.hpp
old_fs_code.o: ../Gematogen/GematogenDebugFlags.hpp
old_fs_code.o: ../Gematogen/PathIndex.hpp
old_fs_code.o: ../BlodKorv/CompactNodeSmallHeaderSkipList.hpp
old_fs_code.o: ../BlodKorv/GIncCountingSort.hpp SimplificationOrdering.hpp
or_index.o: VampireKernelDebugFlags.hpp ../BlodKorv/BlodKorvDebugFlags.hpp
or_index.o: ../BlodKorv/Debug.hpp ../BlodKorv/ObjDesc.hpp
or_index.o: ../BlodKorv/AllocationFailureHandling.hpp ../BlodKorv/Exit.hpp
or_index.o: ../BlodKorv/RuntimeError.hpp ../BlodKorv/DestructionMode.hpp
or_index.o: ../BlodKorv/debugMacros.hpp ../BlodKorv/Allocator.hpp
or_index.o: ../BlodKorv/BufferedAllocationStatistics.hpp
or_index.o: Term.hpp ../BlodKorv/Comparison.hpp ../BlodKorv/Stack.hpp
or_index.o: ../BlodKorv/ExpandingStack.hpp ../BlodKorv/BitSet.hpp
or_index.o: ../BlodKorv/BitWord.hpp ../BlodKorv/BitString.hpp
or_index.o: ../BlodKorv/SmallHeaderSkipList.hpp TermWeightType.hpp
or_index.o: ../BlodKorv/Tuple.hpp ../BlodKorv/GSortTree.hpp
or_index.o: ../BlodKorv/GList.hpp prefix.hpp variables.hpp
or_index.o: ../BlodKorv/MultisetOfVariables.hpp WeightPolynomial.hpp
or_index.o: ../BlodKorv/LinearPolynomial.hpp
or_index.o: ../BlodKorv/LinearCombination.hpp ../BlodKorv/Math.hpp
or_index.o: ../BlodKorv/SharedLinearPolynomial.hpp ../BlodKorv/GDiscTree.hpp
or_index.o: unification.hpp iu_command.hpp BackParaCandidates.hpp
or_index.o: ../BlodKorv/SkipList.hpp SimplificationOrdering.hpp
or_index.o: InstanceRetrievalForDemodulation.hpp
or_index.o: ../Gematogen/PathIndexingWithJoinsAndEarlyCleanUpForInstanceRetrieval.hpp
or_index.o: ../BlodKorv/GlobalStopFlag.hpp ../Gematogen/Gematogen.hpp
or_index.o: ../Gematogen/GematogenDebugFlags.hpp ../Gematogen/PathIndex.hpp
or_index.o: ../BlodKorv/CompactNodeSmallHeaderSkipList.hpp
or_index.o: ../BlodKorv/GIncCountingSort.hpp ../BlodKorv/Multiset.hpp
or_index.o: VampireKernelInterceptMacros.hpp Clause.hpp ClauseBackground.hpp
or_index.o: LiteralList.hpp DefinedHeaderTable.hpp
or_premise.o: VampireKernelDebugFlags.hpp ../BlodKorv/BlodKorvDebugFlags.hpp
or_premise.o: ../BlodKorv/Debug.hpp ../BlodKorv/ObjDesc.hpp
or_premise.o: ../BlodKorv/AllocationFailureHandling.hpp ../BlodKorv/Exit.hpp
or_premise.o: ../BlodKorv/RuntimeError.hpp ../BlodKorv/DestructionMode.hpp
or_premise.o: ../BlodKorv/debugMacros.hpp ../BlodKorv/Allocator.hpp
or_premise.o: ../BlodKorv/BufferedAllocationStatistics.hpp
or_premise.o: ../BlodKorv/GList.hpp ../BlodKorv/BitWord.hpp Term.hpp
or_premise.o: VampireKernelConst.hpp ../BlodKorv/Comparison.hpp
or_premise.o: ../BlodKorv/Stack.hpp ../BlodKorv/ExpandingStack.hpp
or_premise.o: ../BlodKorv/Array.hpp ../BlodKorv/BitSet.hpp
or_premise.o: ../BlodKorv/BitString.hpp ../BlodKorv/SmallHeaderSkipList.hpp
or_premise.o: AutoFunctorPrecedence.hpp ../BlodKorv/Tuple.hpp
or_premise.o: ../BlodKorv/GSortTree.hpp ClauseBackground.hpp LiteralList.hpp
or_premise.o: DefinedHeaderTable.hpp prefix.hpp variables.hpp
or_premise.o: ../BlodKorv/MultisetOfVariables.hpp WeightPolynomial.hpp
or_premise.o: ../BlodKorv/LinearPolynomial.hpp
or_premise.o: ../BlodKorv/LinearCombination.hpp ../BlodKorv/Math.hpp
or_premise.o: ../BlodKorv/SharedLinearPolynomial.hpp
or_premise.o: ../BlodKorv/GDiscTree.hpp or_index.hpp unification.hpp
or_premise.o: iu_command.hpp BackParaCandidates.hpp ../BlodKorv/SkipList.hpp
or_premise.o: SimplificationOrdering.hpp InstanceRetrievalForDemodulation.hpp
or_premise.o: ../Gematogen/PathIndexingWithJoinsAndEarlyCleanUpForInstanceRetrieval.hpp
or_premise.o: ../BlodKorv/GlobalStopFlag.hpp ../Gematogen/Gematogen.hpp
or_premise.o: ../Gematogen/GematogenDebugFlags.hpp ../Gematogen/PathIndex.hpp
or_premise.o: ../BlodKorv/CompactNodeSmallHeaderSkipList.hpp
or_premise.o: ../BlodKorv/GIncCountingSort.hpp ../BlodKorv/Multiset.hpp
or_premise.o: VampireKernelInterceptMacros.hpp NewClause.hpp
or_premise.o: OpenNewClause.hpp Flatterm.hpp tmp_literal.hpp
or_premise.o: TermWeighting.hpp TmpLitList.hpp ForwardDemodulation.hpp
or_premise.o: ../Gematogen/ForwardMatchingIndexTree.hpp
or_premise.o: ../BlodKorv/EquivalenceRelation.hpp
or_premise.o: ../Gematogen/ForwardMatchingIndex.hpp
or_premise.o: ../BlodKorv/ExpandingMultiset.hpp
or_premise.o: ForwardMatchingSubstitutionCache.hpp ClauseQueue.hpp
or_premise.o: ../BlodKorv/FIFO.hpp PagedPassiveClauseStorage.hpp
or_premise.o: ../BlodKorv/PageQueueDisciplineStorage.hpp Splitting.hpp
or_premise.o: DivIntoComp.hpp
prefix.o: prefix.hpp ../BlodKorv/jargon.hpp VampireKernelDebugFlags.hpp
prefix.o: ../BlodKorv/BlodKorvDebugFlags.hpp VampireKernelConst.hpp
prefix.o: ../BlodKorv/ClassDesc.hpp ../BlodKorv/Debug.hpp
prefix.o: ../BlodKorv/ObjDesc.hpp ../BlodKorv/AllocationFailureHandling.hpp
prefix.o: ../BlodKorv/Exit.hpp ../BlodKorv/RuntimeError.hpp
prefix.o: ../BlodKorv/DestructionMode.hpp ../BlodKorv/debugMacros.hpp
prefix.o: ../BlodKorv/Allocator.hpp
prefix.o: ../BlodKorv/BufferedAllocationStatistics.hpp
prefix.o: ../BlodKorv/Comparison.hpp ../BlodKorv/Stack.hpp
prefix.o: ../BlodKorv/ExpandingStack.hpp ../BlodKorv/Array.hpp
prefix.o: ../BlodKorv/BitSet.hpp ../BlodKorv/BitWord.hpp
prefix.o: ../BlodKorv/BitString.hpp ../BlodKorv/SmallHeaderSkipList.hpp
prefix.o: AutoFunctorPrecedence.hpp ../BlodKorv/Tuple.hpp
prefix.o: ../BlodKorv/GSortTree.hpp ../BlodKorv/GList.hpp variables.hpp
prefix.o: ../BlodKorv/MultisetOfVariables.hpp WeightPolynomial.hpp
prefix.o: ../BlodKorv/LinearPolynomial.hpp ../BlodKorv/LinearCombination.hpp
prefix.o: ../BlodKorv/Math.hpp ../BlodKorv/SharedLinearPolynomial.hpp
prefix.o: ../BlodKorv/GDiscTree.hpp
premise_factoring.o: premise_factoring.hpp ../BlodKorv/jargon.hpp
premise_factoring.o: VampireKernelDebugFlags.hpp
premise_factoring.o: ../BlodKorv/BlodKorvDebugFlags.hpp or_premise.hpp
premise_factoring.o: ../BlodKorv/Debug.hpp ../BlodKorv/ObjDesc.hpp
premise_factoring.o: ../BlodKorv/AllocationFailureHandling.hpp
premise_factoring.o: ../BlodKorv/Exit.hpp ../BlodKorv/RuntimeError.hpp
premise_factoring.o: ../BlodKorv/DestructionMode.hpp
premise_factoring.o: ../BlodKorv/debugMacros.hpp ../BlodKorv/Allocator.hpp
premise_factoring.o: ../BlodKorv/BufferedAllocationStatistics.hpp
premise_factoring.o: ../BlodKorv/GList.hpp ../BlodKorv/BitWord.hpp Term.hpp
premise_factoring.o: VampireKernelConst.hpp ../BlodKorv/Comparison.hpp
premise_factoring.o: ../BlodKorv/Stack.hpp ../BlodKorv/ExpandingStack.hpp
premise_factoring.o: ../BlodKorv/Array.hpp ../BlodKorv/BitSet.hpp
premise_factoring.o: ../BlodKorv/BitString.hpp
premise_factoring.o: ../BlodKorv/SmallHeaderSkipList.hpp TermWeightType.hpp
premise_factoring.o: AutoFunctorPrecedence.hpp ../BlodKorv/Tuple.hpp
premise_factoring.o: ../BlodKorv/GSortTree.hpp ClauseBackground.hpp
premise_factoring.o: LiteralList.hpp DefinedHeaderTable.hpp prefix.hpp
premise_factoring.o: variables.hpp ../BlodKorv/MultisetOfVariables.hpp
premise_factoring.o: WeightPolynomial.hpp ../BlodKorv/LinearPolynomial.hpp
premise_factoring.o: ../BlodKorv/LinearCombination.hpp ../BlodKorv/Math.hpp
premise_factoring.o: ../BlodKorv/SharedLinearPolynomial.hpp
premise_factoring.o: ../BlodKorv/GDiscTree.hpp or_index.hpp unification.hpp
premise_factoring.o: iu_command.hpp BackParaCandidates.hpp
premise_factoring.o: ../BlodKorv/SkipList.hpp SimplificationOrdering.hpp
premise_factoring.o: InstanceRetrievalForDemodulation.hpp
premise_factoring.o: ../Gematogen/PathIndexingWithJoinsAndEarlyCleanUpForInstanceRetrieval.hpp
premise_factoring.o: ../BlodKorv/GlobalStopFlag.hpp
premise_factoring.o: ../Gematogen/Gematogen.hpp
premise_factoring.o: ../Gematogen/GematogenDebugFlags.hpp
premise_factoring.o: ../Gematogen/PathIndex.hpp
premise_factoring.o: ../BlodKorv/CompactNodeSmallHeaderSkipList.hpp
premise_factoring.o: ../BlodKorv/GIncCountingSort.hpp
premise_factoring.o: ../BlodKorv/Multiset.hpp
premise_factoring.o: VampireKernelInterceptMacros.hpp NewClause.hpp
premise_factoring.o: OpenNewClause.hpp Flatterm.hpp tmp_literal.hpp
premise_factoring.o: TermWeighting.hpp TmpLitList.hpp ForwardDemodulation.hpp
premise_factoring.o: ../Gematogen/ForwardMatchingIndexTree.hpp
premise_factoring.o: ../BlodKorv/EquivalenceRelation.hpp
premise_factoring.o: ../Gematogen/ForwardMatchingIndex.hpp
premise_factoring.o: ../BlodKorv/ExpandingMultiset.hpp
premise_factoring.o: ForwardMatchingSubstitutionCache.hpp ClauseQueue.hpp
premise_factoring.o: ../BlodKorv/FIFO.hpp PagedPassiveClauseStorage.hpp
premise_factoring.o: ../BlodKorv/PageQueueDisciplineStorage.hpp Splitting.hpp
premise_factoring.o: DivIntoComp.hpp
sharing.o: ../BlodKorv/jargon.hpp VampireKernelDebugFlags.hpp
sharing.o: ../BlodKorv/BlodKorvDebugFlags.hpp VampireKernelConst.hpp
sharing.o: Signature.hpp ../BlodKorv/Array.hpp
sharing.o: ../BlodKorv/DestructionMode.hpp ../BlodKorv/debugMacros.hpp
sharing.o: ../BlodKorv/Debug.hpp ../BlodKorv/GlobAlloc.hpp
sharing.o: ../BlodKorv/ClassDesc.hpp ../BlodKorv/ObjDesc.hpp
sharing.o: ../BlodKorv/AllocationFailureHandling.hpp
sharing.o: ../BlodKorv/RuntimeError.hpp ../BlodKorv/Allocator.hpp
sharing.o: ../BlodKorv/BufferedAllocationStatistics.hpp
sharing.o: ../BlodKorv/Comparison.hpp SymbolInfo.hpp
sharing.o: AutoFunctorPrecedence.hpp ../BlodKorv/Tuple.hpp
sharing.o: ../BlodKorv/GSortTree.hpp ../BlodKorv/GList.hpp TermWeightType.hpp
sharing.o: ../BlodKorv/Stack.hpp ../BlodKorv/ExpandingStack.hpp
sharing.o: ../BlodKorv/BitSet.hpp ../BlodKorv/BitWord.hpp
sharing.o: ../BlodKorv/BitString.hpp ../BlodKorv/SmallHeaderSkipList.hpp
sharing.o: Flatterm.hpp ../BlodKorv/MultisetOfVariables.hpp variables.hpp
sharing.o: WeightPolynomial.hpp ../BlodKorv/LinearPolynomial.hpp
sharing.o: ../BlodKorv/LinearCombination.hpp ../BlodKorv/Math.hpp
sharing.o: ../BlodKorv/SharedLinearPolynomial.hpp ../BlodKorv/GDiscTree.hpp
tmp_literal.o: ../BlodKorv/Comparison.hpp ../BlodKorv/BlodKorvDebugFlags.hpp
tmp_literal.o: ../BlodKorv/debugMacros.hpp ../BlodKorv/Debug.hpp
tmp_literal.o: ../BlodKorv/Multiset.hpp ../BlodKorv/Stack.hpp
tmp_literal.o: VampireKernelDebugFlags.hpp VampireKernelConst.hpp
tmp_literal.o: ../BlodKorv/ClassDesc.hpp ../BlodKorv/ObjDesc.hpp
tmp_literal.o: ../BlodKorv/AllocationFailureHandling.hpp ../BlodKorv/Exit.hpp
tmp_literal.o: ../BlodKorv/RuntimeError.hpp ../BlodKorv/DestructionMode.hpp
tmp_literal.o: ../BlodKorv/Allocator.hpp
tmp_literal.o: ../BlodKorv/BufferedAllocationStatistics.hpp
tmp_literal.o: ../BlodKorv/ExpandingStack.hpp ../BlodKorv/Array.hpp
tmp_literal.o: ../BlodKorv/BitSet.hpp ../BlodKorv/BitWord.hpp
tmp_literal.o: ../BlodKorv/BitString.hpp ../BlodKorv/SmallHeaderSkipList.hpp
tmp_literal.o: AutoFunctorPrecedence.hpp ../BlodKorv/Tuple.hpp
tmp_literal.o: ../BlodKorv/GSortTree.hpp ../BlodKorv/GList.hpp
tmp_literal.o: WeightPolynomial.hpp ../BlodKorv/LinearPolynomial.hpp
tmp_literal.o: ../BlodKorv/LinearCombination.hpp ../BlodKorv/Math.hpp
tmp_literal.o: ../BlodKorv/SharedLinearPolynomial.hpp
tmp_literal.o: ../BlodKorv/GDiscTree.hpp TermWeighting.hpp Flatterm.hpp
tmp_literal.o: ../BlodKorv/MultisetOfVariables.hpp variables.hpp
tmp_literal.o: InstanceRetrievalForDemodulation.hpp
tmp_literal.o: ../Gematogen/PathIndexingWithJoinsAndEarlyCleanUpForInstanceRetrieval.hpp
tmp_literal.o: ../BlodKorv/GlobalStopFlag.hpp ../Gematogen/Gematogen.hpp
tmp_literal.o: ../Gematogen/GematogenDebugFlags.hpp
tmp_literal.o: ../Gematogen/PathIndex.hpp
tmp_literal.o: ../BlodKorv/CompactNodeSmallHeaderSkipList.hpp
tmp_literal.o: ../BlodKorv/GIncCountingSort.hpp SimplificationOrdering.hpp
variables.o: VampireKernelDebugFlags.hpp ../BlodKorv/BlodKorvDebugFlags.hpp
variables.o: ../BlodKorv/MultisetOfVariables.hpp ../BlodKorv/Stack.hpp
variables.o: ../BlodKorv/debugMacros.hpp ../BlodKorv/Debug.hpp
variables.o: ../BlodKorv/Comparison.hpp WeightPolynomial.hpp
variables.o: ../BlodKorv/LinearPolynomial.hpp
variables.o: ../BlodKorv/LinearCombination.hpp ../BlodKorv/Math.hpp
variables.o: ../BlodKorv/SharedLinearPolynomial.hpp ../BlodKorv/GDiscTree.hpp
variables.o: ../BlodKorv/DestructionMode.hpp ../BlodKorv/GlobAlloc.hpp
variables.o: ../BlodKorv/ObjDesc.hpp
variables.o: ../BlodKorv/AllocationFailureHandling.hpp ../BlodKorv/Exit.hpp
variables.o: ../BlodKorv/RuntimeError.hpp ../BlodKorv/Allocator.hpp
variables.o: ../BlodKorv/BufferedAllocationStatistics.hpp TermWeightType.hpp
weight_formula.o: ../BlodKorv/jargon.hpp ../BlodKorv/Comparison.hpp
weight_formula.o: ../BlodKorv/BlodKorvDebugFlags.hpp
weight_formula.o: ../BlodKorv/debugMacros.hpp ../BlodKorv/Debug.hpp
weight_formula.o: VampireKernelDebugFlags.hpp prefix.hpp
weight_formula.o: ../BlodKorv/ObjDesc.hpp
weight_formula.o: ../BlodKorv/AllocationFailureHandling.hpp
weight_formula.o: ../BlodKorv/Exit.hpp ../BlodKorv/RuntimeError.hpp
weight_formula.o: ../BlodKorv/DestructionMode.hpp ../BlodKorv/Allocator.hpp
weight_formula.o: ../BlodKorv/BufferedAllocationStatistics.hpp
weight_formula.o: ../BlodKorv/Stack.hpp ../BlodKorv/ExpandingStack.hpp
weight_formula.o: ../BlodKorv/Array.hpp ../BlodKorv/BitSet.hpp
weight_formula.o: ../BlodKorv/BitWord.hpp ../BlodKorv/BitString.hpp
weight_formula.o: ../BlodKorv/SmallHeaderSkipList.hpp TermWeightType.hpp
weight_formula.o: ../BlodKorv/Tuple.hpp ../BlodKorv/GSortTree.hpp
weight_formula.o: ../BlodKorv/GList.hpp variables.hpp
weight_formula.o: ../BlodKorv/MultisetOfVariables.hpp WeightPolynomial.hpp
weight_formula.o: ../BlodKorv/LinearPolynomial.hpp
weight_formula.o: ../BlodKorv/LinearCombination.hpp ../BlodKorv/Math.hpp
weight_formula.o: ../BlodKorv/SharedLinearPolynomial.hpp
weight_formula.o: ../BlodKorv/GDiscTree.hpp