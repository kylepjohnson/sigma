#ifndef PR_PRIVATE_DECLARATIONS_H
//=======================================================
#define PR_PRIVATE_DECLARATIONS_H

#include <cstdio>

#ifndef __cplusplus
 #define __cplusplus
#endif

#define YY_NEVER_INTERACTIVE 1

#include "pr_abstract_syntax.hpp"
//=======================================================

union yystypeUnion
 { 
  SYMBOL_COORDINATES coord;
  PR_SYMBOL_OCC* token;
  PR_TERM* term;
  PR_TERM_LIST* term_list;
  PR_LITERAL* literal;
  PR_LITERAL_LIST* literal_list;
 }; 
 
#define YYSTYPE union yystypeUnion 

extern FILE* yyin;
int yylex();
void yyerror(char *str) {};
int yyparse();
void resetScanner();
int countTabs(char *str);
char* registerSym(const char* str);

extern int charNum;
extern int lineNum;
extern int numOfTabs;

extern PR_FILE_PARS_RESULT* FileParsResult;
//=======================================================
#endif
