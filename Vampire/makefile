# File:    makefile 
# Author:  Alexandre Riazanov, Andrei Voronkov (<surname>@cs.man.ac.uk)
# Created: who remembers
# Purpose: makefile for various clones of Vampire
#
# Modified by Peter Denno. Only the KIF version has been tested, and WRT that,
# the ANTLR generation of the KIF and XML parsers has not been tested,
# instead, the 12 Antlr-2.6.0 generated files are provided with the source.
# See the README in this directory. 
################################################################
CC=/local/bin/g++
PPOPT=-DDEBUG_PREPRO=0 -DDEBUG_TRACE_ALL=0 -O4 -Wall# default options for compiling the preprocessor
#PPOPT=-O4 -Wall# default options for compiling the preprocessor
# VKOPT=-DDEBUG_ALL -DLARGE_TERMS # default options for compiling the kernel
VKOPT=-O4 -DNO_DEBUG -DHUGE_TERMS # default options for compiling the kernel

VAMPIRE=$(PWD)

MDOPT= -I/local/include -I/usr/include -I/usr/include/sys -I.  # default makedepend options
################################################################

PreproObj = ANTLRException.o\
            ASTFactory.o\
            ASTRefCount.o\
            Atom.o\
            BaseAST.o\
            BitSet.o\
            CharBuffer.o\
            CharScanner.o\
            Clause.o\
            CommonAST.o\
            CommonToken.o\
            Dump.o\
            Formula.o\
            Inference.o\
            InputBuffer.o\
            Int.o\
            IntNameTable.o\
            KIFLexer.o\
            KIFParser.o\
            LLkParser.o\
            LexerSharedInputState.o\
            Literal.o\
            Map.o\
            Memory.o\
            Miniscope.o\
            MismatchedCharException.o\
            MismatchedTokenException.o\
            NoViableAltException.o\
            NoViableAltForCharException.o\
            Options.o\
            Output.o\
            Parser.o\
            ParserSharedInputState.o\
            Problem.o\
            Query.o\
            Random.o\
            RecognitionException.o\
            Refutation.o\
            Renaming.o\
            Signature.o\
            Statistics.o\
            String.o\
            Substitution.o\
            SymCounter.o\
            Tabulate.o\
            Term.o\
            Token.o\
            TokenBuffer.o\
            TokenStreamSelector.o\
            Tracer.o\
            Unit.o\
            Var.o\
            assert.o\
            kif.o

KIFObj = ./XMLLexer.o\
	./XMLParser.o

TPTPObj = ./TPTPLexer.o\
	./TPTPParser.o

KernelObj = ./VampireKernel/libvkernel.a

################################################################

default: vampire

#KIFLexer.hpp KIFLexer.cpp KIFParser.hpp KIFParser.cpp: KIF.g 
#	export CLASSPATH=${CLASSPATH} ; java antlr.Tool KIF.g

#TPTPLexer.hpp TPTPLexer.cpp TPTPParser.hpp TPTPParser.cpp: TP
#	export CLASSPATH=${CLASSPATH} ; java antlr.Tool TPTP.g

#XMLLexer.hpp XMLLexer.cpp XMLParser.hpp XMLParser.cpp: XML.g 	ex
#	export CLASSPATH=${CLASSPATH} ; java antlr.Tool XML.g


################################################################

make: clean vampire 

vampire: ${KIFObj} ${TPTPObj} ${PreproObj} kernel
	${CC} -static ${PreproObj} ${TPTPObj} ${KIFObj} ${PPOPT} ${VKOPT} -L${VAMPIRE}/VampireKernel -L/lib -lvkernel -lstdc++ -lpthread -o vampire
	strip vampire

kif: ${KIFObj} ${PreproObj} kernel
	${CC} -static ${PreproObj} ${KIFObj} ${PPOPT} ${VKOPT} -L${VAMPIRE} -lstdc++ -o kif
	strip kif

vampireTPTP: ${PreproObj} ${TPTPObj} kernel kif.o query.o
	${CC} -static ${PreproObj} ${PPOPT} ${VKOPT} kif.o query.o -L${VAMPIRE}/VampireKernel ${KIFObj} -lvkernel -lstdc++  -o vampireTPTP
	strip vampireTPTP

prepro: ${PreproObj} 
	${CC} -static ${PPOPT} ${PreproObj} clausifier.cpp -o prepro

kernel:
	cd ./VampireKernel ; ${MAKE} libvkernel.a CC=${CC} OPT='${VKOPT}'

.SUFFIXES:
.SUFFIXES: .cpp .o

.cpp.o:
	${CC} ${PPOPT} ${VKOPT} -I. -I./antlr -Wall -I./VampireKernel -I./BlodKorv -I./Gematogen -c $*.cpp

clean:
#	rm -f KIFLexer.cpp KIFLexer.hpp KIFParser.cpp KIFParser.hpp KIFLexerTokenTypes.hpp KIFLexerTokenTypes.txt
#	rm -f XMLLexer.cpp XMLLexer.hpp XMLParser.cpp XMLParser.hpp XMLLexerTokenTypes.hpp XMLLexerTokenTypes.txt
	rm -f *.o vampire
	cd ./VampireKernel ; ${MAKE} clean

depend:
	makedepend ${MDOPT} -I./VampireKernel -I./BlodKorv -I./Gematogen *.cpp
	cd ./VampireKernel ; ${MAKE} depend

################################################################

# DO NOT DELETE

assert.o: assert.hpp 
Atom.o: Atom.hpp Term.hpp Signature.hpp Int.hpp 
Atom.o: assert.hpp 
Atom.o: Memory.hpp Var.hpp Lst.hpp ./VampireKernel/VampireKernel.hpp
Atom.o: ./BlodKorv/jargon.hpp
Clause.o: Clause.hpp Literal.hpp Atom.hpp Term.hpp Signature.hpp Int.hpp
Clause.o: assert.hpp 
Clause.o: Memory.hpp Var.hpp Lst.hpp ./VampireKernel/VampireKernel.hpp
Clause.o: ./BlodKorv/jargon.hpp Sort.hpp Renaming.hpp Map.hpp
DLGLexer.o: DLexer.cpp
Dump.o: assert.hpp 
Dump.o: Dump.hpp
Formula.o: Formula.hpp Atom.hpp Term.hpp Signature.hpp Int.hpp
Formula.o: assert.hpp  Tracer.hpp Memory.hpp Var.hpp
Formula.o: Lst.hpp ./VampireKernel/VampireKernel.hpp ./BlodKorv/jargon.hpp
Formula.o: Sort.hpp Substitution.hpp
IntNameTable.o: IntNameTable.hpp List.hpp assert.hpp 
IntNameTable.o: Tracer.hpp Memory.hpp Pair.hpp
kif.o: TPTPLexer.hpp
kif.o: $(VAMPIRE)/antlr/config.hpp
kif.o: $(VAMPIRE)/antlr/CommonToken.hpp
kif.o: $(VAMPIRE)/antlr/Token.hpp
kif.o: $(VAMPIRE)/antlr/RefCount.hpp
kif.o: $(VAMPIRE)/antlr/InputBuffer.hpp
kif.o: $(VAMPIRE)/antlr/CircularQueue.hpp
kif.o: $(VAMPIRE)/antlr/BitSet.hpp
kif.o: TPTPLexerTokenTypes.hpp
kif.o: $(VAMPIRE)/antlr/CharScanner.hpp
kif.o: $(VAMPIRE)/antlr/TokenStream.hpp
kif.o: $(VAMPIRE)/antlr/RecognitionException.hpp
kif.o: $(VAMPIRE)/antlr/ANTLRException.hpp
kif.o: $(VAMPIRE)/antlr/LexerSharedInputState.hpp
kif.o: Unit.hpp Chain.hpp Memory.hpp assert.hpp Tracer.hpp Sort.hpp
kif.o: Clause.hpp Literal.hpp Atom.hpp Term.hpp Signature.hpp Int.hpp
kif.o: Var.hpp Lst.hpp ./VampireKernel/VampireKernel.hpp
kif.o: ./BlodKorv/jargon.hpp Formula.hpp TPTPParser.hpp
kif.o: $(VAMPIRE)/antlr/TokenBuffer.hpp
kif.o: $(VAMPIRE)/antlr/LLkParser.hpp
kif.o: $(VAMPIRE)/antlr/Parser.hpp
kif.o: $(VAMPIRE)/antlr/ASTFactory.hpp
kif.o: $(VAMPIRE)/antlr/AST.hpp
kif.o: $(VAMPIRE)/antlr/ASTRefCount.hpp
kif.o: $(VAMPIRE)/antlr/ASTArray.hpp
kif.o: $(VAMPIRE)/antlr/ASTPair.hpp
kif.o: $(VAMPIRE)/antlr/ParserSharedInputState.hpp
kif.o: $(VAMPIRE)/antlr/CharStreamException.hpp
kif.o: $(VAMPIRE)/antlr/TokenStreamRecognitionException.hpp
kif.o: $(VAMPIRE)/antlr/TokenStreamException.hpp
kif.o: $(VAMPIRE)/antlr/TokenStreamSelector.hpp
kif.o: ./VampireKernel/Tabulation.hpp ./BlodKorv/GlobAlloc.hpp
kif.o: ./BlodKorv/BlodKorvDebugFlags.hpp ./BlodKorv/ClassDesc.hpp
kif.o: ./BlodKorv/Debug.hpp ./BlodKorv/ObjDesc.hpp
kif.o: ./BlodKorv/AllocationFailureHandling.hpp ./BlodKorv/Exit.hpp
kif.o: ./BlodKorv/RuntimeError.hpp ./BlodKorv/DestructionMode.hpp
kif.o: ./BlodKorv/debugMacros.hpp ./BlodKorv/Allocator.hpp
kif.o: ./BlodKorv/BufferedAllocationStatistics.hpp
kif.o: ./VampireKernel/VampireKernelDebugFlags.hpp
kif.o: ./VampireKernel/TermWeightType.hpp ./VampireKernel/Input.hpp
kif.o: ./BlodKorv/GList.hpp ./BlodKorv/Malloc.hpp ./BlodKorv/GlobalClock.hpp
kif.o: Options.hpp Output.hpp Renaming.hpp Map.hpp Problem.hpp
kif.o: IntNameTable.hpp Pair.hpp Refutation.hpp Statistics.hpp
ListSubstitution.o: ListSubstitution.hpp Term.hpp Signature.hpp Int.hpp
ListSubstitution.o: List.hpp assert.hpp 
ListSubstitution.o: Tracer.hpp Memory.hpp Var.hpp Lst.hpp
ListSubstitution.o: ./VampireKernel/VampireKernel.hpp ./BlodKorv/jargon.hpp
Literal.o: Literal.hpp Atom.hpp Term.hpp Signature.hpp Int.hpp
Literal.o: assert.hpp 
Literal.o: Tracer.hpp Memory.hpp Var.hpp Lst.hpp
Literal.o: ./VampireKernel/VampireKernel.hpp ./BlodKorv/jargon.hpp
Literal.o: Substitution.hpp
Map.o: Map.hpp assert.hpp 
Memory.o: Memory.hpp 
Memory.o: Tracer.hpp
Options.o: assert.hpp Tracer.hpp Options.hpp Output.hpp Formula.hpp Atom.hpp
Options.o: Term.hpp Signature.hpp Int.hpp List.hpp Memory.hpp Var.hpp Lst.hpp
Options.o: ./VampireKernel/VampireKernel.hpp ./BlodKorv/jargon.hpp
Output.o: Output.hpp Formula.hpp Atom.hpp Term.hpp Signature.hpp Int.hpp
Output.o: assert.hpp 
Output.o: Memory.hpp Var.hpp Lst.hpp ./VampireKernel/VampireKernel.hpp
Output.o: ./BlodKorv/jargon.hpp Unit.hpp Chain.hpp Sort.hpp Clause.hpp
Output.o: Literal.hpp Refutation.hpp Map.hpp Problem.hpp IntNameTable.hpp
Output.o: Pair.hpp
Problem.o: Problem.hpp Unit.hpp Chain.hpp Memory.hpp
Problem.o: assert.hpp Tracer.hpp Sort.hpp Clause.hpp Literal.hpp Atom.hpp
Problem.o: Term.hpp Signature.hpp Int.hpp List.hpp Var.hpp Lst.hpp
Problem.o: ./VampireKernel/VampireKernel.hpp ./BlodKorv/jargon.hpp
Problem.o: Formula.hpp Options.hpp Output.hpp SymCounter.hpp
Query.o: Formula.hpp Atom.hpp Term.hpp Signature.hpp Int.hpp
Query.o: assert.hpp Tracer.hpp Memory.hpp Var.hpp Lst.hpp
Query.o: ./VampireKernel/VampireKernel.hpp ./BlodKorv/jargon.hpp
Refutation.o: Refutation.hpp List.hpp assert.hpp 
Refutation.o: Tracer.hpp Memory.hpp Unit.hpp Chain.hpp Sort.hpp Clause.hpp
Refutation.o: Literal.hpp Atom.hpp Term.hpp Signature.hpp Int.hpp
Refutation.o: Var.hpp Lst.hpp ./VampireKernel/VampireKernel.hpp
Refutation.o: ./BlodKorv/jargon.hpp Formula.hpp Problem.hpp IntNameTable.hpp
Refutation.o: Pair.hpp Map.hpp
Signature.o: Signature.hpp Int.hpp
Signature.o: List.hpp assert.hpp Tracer.hpp Memory.hpp
Signature.o: SymCounter.hpp Unit.hpp Chain.hpp Sort.hpp Clause.hpp
Signature.o: Literal.hpp Atom.hpp Term.hpp Var.hpp Lst.hpp
Signature.o: ./VampireKernel/VampireKernel.hpp ./BlodKorv/jargon.hpp
Signature.o: Formula.hpp
Statistics.o: Statistics.hpp ./VampireKernel/VampireKernel.hpp
Statistics.o: ./BlodKorv/jargon.hpp Options.hpp Output.hpp Formula.hpp
Statistics.o: Atom.hpp Term.hpp Signature.hpp Int.hpp 
Statistics.o: List.hpp assert.hpp 
Statistics.o: Tracer.hpp Memory.hpp Var.hpp Lst.hpp
Substitution.o: Substitution.hpp Term.hpp Signature.hpp Int.hpp
Substitution.o: List.hpp assert.hpp 
Substitution.o: Tracer.hpp Memory.hpp Var.hpp Lst.hpp
Substitution.o: ./VampireKernel/VampireKernel.hpp ./BlodKorv/jargon.hpp
SymCounter.o: SymCounter.hpp Signature.hpp Int.hpp 
SymCounter.o: List.hpp assert.hpp 
SymCounter.o: Tracer.hpp Memory.hpp Unit.hpp Chain.hpp Sort.hpp Clause.hpp
SymCounter.o: Literal.hpp Atom.hpp Term.hpp Var.hpp Lst.hpp
SymCounter.o: ./VampireKernel/VampireKernel.hpp ./BlodKorv/jargon.hpp
SymCounter.o: Formula.hpp Problem.hpp
Term.o: Term.hpp Signature.hpp Int.hpp 
Term.o: assert.hpp 
Term.o: Memory.hpp Var.hpp Lst.hpp ./VampireKernel/VampireKernel.hpp
Term.o: ./BlodKorv/jargon.hpp Sort.hpp Substitution.hpp
TPTPLexer.o: TPTPLexer.hpp
TPTPLexer.o: $(VAMPIRE)/antlr/config.hpp
TPTPLexer.o: $(VAMPIRE)/antlr/CommonToken.hpp
TPTPLexer.o: $(VAMPIRE)/antlr/Token.hpp
TPTPLexer.o: $(VAMPIRE)/antlr/RefCount.hpp
TPTPLexer.o: $(VAMPIRE)/antlr/InputBuffer.hpp
TPTPLexer.o: $(VAMPIRE)/antlr/CircularQueue.hpp
TPTPLexer.o: $(VAMPIRE)/antlr/BitSet.hpp
TPTPLexer.o: TPTPLexerTokenTypes.hpp
TPTPLexer.o: $(VAMPIRE)/antlr/CharScanner.hpp
TPTPLexer.o: $(VAMPIRE)/antlr/TokenStream.hpp
TPTPLexer.o: $(VAMPIRE)/antlr/RecognitionException.hpp
TPTPLexer.o: $(VAMPIRE)/antlr/ANTLRException.hpp
TPTPLexer.o: $(VAMPIRE)/antlr/LexerSharedInputState.hpp
TPTPLexer.o: Unit.hpp Chain.hpp Memory.hpp 
TPTPLexer.o: assert.hpp Tracer.hpp Sort.hpp Clause.hpp Literal.hpp Atom.hpp
TPTPLexer.o: Term.hpp Signature.hpp Int.hpp 
TPTPLexer.o: List.hpp Var.hpp Lst.hpp ./VampireKernel/VampireKernel.hpp
TPTPLexer.o: ./BlodKorv/jargon.hpp Formula.hpp
TPTPLexer.o: $(VAMPIRE)/antlr/CharBuffer.hpp
TPTPLexer.o: $(VAMPIRE)/antlr/TokenStreamException.hpp
TPTPLexer.o: $(VAMPIRE)/antlr/TokenStreamIOException.hpp
TPTPLexer.o: $(VAMPIRE)/antlr/TokenStreamRecognitionException.hpp
TPTPLexer.o: $(VAMPIRE)/antlr/CharStreamException.hpp
TPTPLexer.o: $(VAMPIRE)/antlr/CharStreamIOException.hpp
TPTPLexer.o: $(VAMPIRE)/antlr/NoViableAltForCharException.hpp
TPTPParser.o: TPTPParser.hpp
TPTPParser.o: $(VAMPIRE)/antlr/config.hpp
TPTPParser.o: $(VAMPIRE)/antlr/TokenStream.hpp
TPTPParser.o: $(VAMPIRE)/antlr/Token.hpp
TPTPParser.o: $(VAMPIRE)/antlr/RefCount.hpp
TPTPParser.o: $(VAMPIRE)/antlr/TokenBuffer.hpp
TPTPParser.o: $(VAMPIRE)/antlr/CircularQueue.hpp
TPTPParser.o: TPTPLexerTokenTypes.hpp
TPTPParser.o: $(VAMPIRE)/antlr/LLkParser.hpp
TPTPParser.o: $(VAMPIRE)/antlr/Parser.hpp
TPTPParser.o: $(VAMPIRE)/antlr/BitSet.hpp
TPTPParser.o: $(VAMPIRE)/antlr/RecognitionException.hpp
TPTPParser.o: $(VAMPIRE)/antlr/ANTLRException.hpp
TPTPParser.o: $(VAMPIRE)/antlr/ASTFactory.hpp
TPTPParser.o: $(VAMPIRE)/antlr/AST.hpp
TPTPParser.o: $(VAMPIRE)/antlr/ASTRefCount.hpp
TPTPParser.o: $(VAMPIRE)/antlr/ASTArray.hpp
TPTPParser.o: $(VAMPIRE)/antlr/ASTPair.hpp
TPTPParser.o: $(VAMPIRE)/antlr/ParserSharedInputState.hpp
TPTPParser.o: Unit.hpp Chain.hpp Memory.hpp 
TPTPParser.o: assert.hpp Tracer.hpp Sort.hpp Clause.hpp Literal.hpp Atom.hpp
TPTPParser.o: Term.hpp Signature.hpp Int.hpp 
TPTPParser.o: List.hpp Var.hpp Lst.hpp ./VampireKernel/VampireKernel.hpp
TPTPParser.o: ./BlodKorv/jargon.hpp Formula.hpp
TPTPParser.o: $(VAMPIRE)/antlr/NoViableAltException.hpp
TPTPParser.o: $(VAMPIRE)/antlr/SemanticException.hpp
TPTPParser.o: IntNameTable.hpp Pair.hpp
TPTPParser.o: $(VAMPIRE)/antlr/TokenStreamSelector.hpp
TPTPParser.o: TPTPLexer.hpp
TPTPParser.o: $(VAMPIRE)/antlr/CommonToken.hpp
TPTPParser.o: $(VAMPIRE)/antlr/InputBuffer.hpp
TPTPParser.o: $(VAMPIRE)/antlr/CharScanner.hpp
TPTPParser.o: $(VAMPIRE)/antlr/LexerSharedInputState.hpp
TPTPParser.o: Options.hpp Output.hpp
Tracer.o: Tracer.hpp
Unit.o: Chain.hpp Memory.hpp assert.hpp Tracer.hpp
Unit.o: Sort.hpp Clause.hpp Literal.hpp Atom.hpp Term.hpp Signature.hpp
Unit.o: Int.hpp 
Unit.o: Var.hpp Lst.hpp ./VampireKernel/VampireKernel.hpp
Unit.o: ./BlodKorv/jargon.hpp Formula.hpp
Var.o: Var.hpp Lst.hpp assert.hpp 
Var.o: Substitution.hpp Term.hpp Signature.hpp Int.hpp 
Var.o: Memory.hpp ./VampireKernel/VampireKernel.hpp ./BlodKorv/jargon.hpp
