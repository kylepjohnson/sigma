#ifndef PR_PARSER_H
//=====================================================================
#define PR_PARSER_H
#include "pr_abstract_syntax.hpp"
//=====================================================================

void ParseFile(const char *fname,PR_FILE_PARS_RESULT* file_pars_res);
 
extern PR_FILE_PARS_RESULT* FileParsResult;

//=====================================================================
#endif
