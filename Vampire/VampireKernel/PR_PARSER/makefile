# File:    makefile 
# Author:  Alexandre Riazanov (riazanov@cs.man.ac.uk)
# Created: Feb 8, 2000
# Purpose: makefile for simple Prolog parser
################################################################
CC= g++3 #      	# UNIX GNU compiler 
#CC= /opt/gcc-2.8.1/bin/g++
OPT=                # no optimisation, default
CFLAGS = -Wall #-I/usr/include 
################################################################
default: pr_parser.o
pr_parser.o: pr_parser.cpp pr_parser.hpp pr_private_declarations.hpp pr_abstract_syntax.hpp pr_abstract_syntax.incl pr_scanner.incl pr_parser.incl 
	  ${CC} ${OPT} ${CFLAGS} -c pr_parser.cpp

pr_scanner.incl: pr_scanner.lex
	     flex -opr_scanner.incl pr_scanner.lex

pr_parser.incl: pr_parser.yacc
	    bison pr_parser.yacc -o pr_parser.incl	
remake: clean pr_parser.o
clean: 
	rm -f *.o ; rm -f *~ ; rm -f *.bak ; rm -f *.tmp 

################################################################

