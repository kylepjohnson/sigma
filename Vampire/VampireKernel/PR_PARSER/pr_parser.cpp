
#include <iostream>
#include <cstring>
#include <cstdlib>
#include "pr_parser.hpp"
#include "pr_private_declarations.hpp"
#include "pr_parser.incl"
#include "pr_scanner.incl"
#include "pr_abstract_syntax.incl"

//************************************************************************

int charNum;
int lineNum;
int numOfTabs;

PR_FILE_PARS_RESULT* FileParsResult;

void resetScanner()
 {
  charNum = 0;
  lineNum = 1;
  numOfTabs = 0;
 };

int countTabs(char *str)
 {
  int l = strlen(str);
  int count = 0;
  for (int i=0;i<l;i++) { if (str[i] == '\t') ++count; };
  return count;
 };

char* registerSym(const char* str)
 {
  int l = strlen(str);
  char* res = new char[l+1];
  //res[l+1] = (char)0;
  strcpy(res,str);
  return res;     
 };

//******************* Main function: *************************************

void ParseFile(const char *fname,PR_FILE_PARS_RESULT* file_pars_res)
 {
   
  int pars_res;
  resetScanner();  
  yyin = fopen(fname,"r"); 
  FileParsResult = file_pars_res;
  if (yyin == NULL) 
   {
    FileParsResult->ErrorAt("Can not open file.",0,0);
   }
  else 
   {
    pars_res = yyparse(); 
    fclose(yyin); 
   };    
 }; // void ParseFile(char *fname,PR_FILE_PARS_RESULT* file_pars_res)

//**************************************************************************
