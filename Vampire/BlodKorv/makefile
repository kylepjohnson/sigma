# File:    makefile 
# Author:  Alexandre Riazanov (riazanov@cs.man.ac.uk)
# Created: Feb 8, 2000
# Purpose: makefile for BlodKorv library 
################################################################
CC= g++ #      	# UNIX GNU compiler 
#CC= /opt/gcc-2.8.1/bin/g++
OPT=                # no optimisation, default
CFLAGS = -Wall #-I/usr/include 
################################################################

Obj= BitWord.o ClassDesc.o CommandLine.o Debug.o DestructionMode.o Exit.o FormatText.o GlobalClock.o GlobAlloc.o GlobAlloc2.o GlobalStopFlag.o Malloc.o RuntimeError.o 

################################################################

default: ${Obj}

.SUFFIXES:
.SUFFIXES: .cpp .o

.cpp.o:
	${CC} ${OPT} ${CFLAGS} -c $*.cpp

remake: clean ${Obj}

clean: 
	rm -f *.o ; rm -f *~ ; rm -f *.bak ; rm -f *.tmp 

################################################################
# DO NOT DELETE

BitWord.o: BitWord.hpp  jargon.hpp
BitWord.o: BlodKorvDebugFlags.hpp debugMacros.hpp Debug.hpp
ClassDesc.o: ClassDesc.hpp 
ClassDesc.o: jargon.hpp Debug.hpp BlodKorvDebugFlags.hpp
CommandLine.o: CommandLine.hpp 
CommandLine.o:  jargon.hpp
CommandLine.o: BlodKorvDebugFlags.hpp GList.hpp 
CommandLine.o: debugMacros.hpp Debug.hpp Malloc.hpp 
CommandLine.o: ClassDesc.hpp ObjDesc.hpp DestructionMode.hpp FormatText.hpp
Debug.o: jargon.hpp BlodKorvDebugFlags.hpp Exit.hpp
DestructionMode.o: DestructionMode.hpp
Exit.o: Exit.hpp DestructionMode.hpp
FormatText.o: FormatText.hpp jargon.hpp
FormatText.o: BlodKorvDebugFlags.hpp ClassDesc.hpp 
FormatText.o:  Debug.hpp Malloc.hpp
FormatText.o: ObjDesc.hpp debugMacros.hpp GList.hpp 
FormatText.o: DestructionMode.hpp
GlobAlloc.o: GlobAlloc.hpp  jargon.hpp
GlobAlloc.o: BlodKorvDebugFlags.hpp ClassDesc.hpp 
GlobAlloc.o:  Debug.hpp ObjDesc.hpp
GlobAlloc.o: AllocationFailureHandling.hpp Exit.hpp RuntimeError.hpp
GlobAlloc.o: DestructionMode.hpp debugMacros.hpp Allocator.hpp
GlobAlloc.o: BufferedAllocationStatistics.hpp
GlobAlloc2.o: GlobAlloc2.hpp jargon.hpp
GlobAlloc2.o: BlodKorvDebugFlags.hpp ClassDesc.hpp 
GlobAlloc2.o: Debug.hpp ObjDesc.hpp
GlobAlloc2.o: AllocationFailureHandling.hpp Exit.hpp RuntimeError.hpp
GlobAlloc2.o: DestructionMode.hpp debugMacros.hpp Allocator.hpp
GlobAlloc2.o: BufferedAllocationStatistics.hpp
GlobalClock.o: jargon.hpp 
GlobalClock.o: BlodKorvDebugFlags.hpp DestructionMode.hpp debugMacros.hpp
GlobalClock.o: Debug.hpp RuntimeError.hpp
GlobalStopFlag.o: GlobalStopFlag.hpp jargon.hpp BlodKorvDebugFlags.hpp
GlobalStopFlag.o: DestructionMode.hpp debugMacros.hpp Debug.hpp
Malloc.o: Malloc.hpp  jargon.hpp BlodKorvDebugFlags.hpp
Malloc.o: ClassDesc.hpp  Debug.hpp
Malloc.o: ObjDesc.hpp debugMacros.hpp
RuntimeError.o:  RuntimeError.hpp jargon.hpp Exit.hpp
