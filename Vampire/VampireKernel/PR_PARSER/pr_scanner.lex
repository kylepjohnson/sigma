
%{

void ASSIGN_YYLVAL(const char* str)
 {
  yylval.token = new PR_SYMBOL_OCC(registerSym(str),lineNum,charNum);
 };

inline void MemoCoord()
 {
  yylval.coord.Assign(lineNum,charNum);
 };

%} 
 
           /***** Options *******/
           
%option noyywrap       
  
           /***** Start conditions ******/

%x string 
%x end_of_string
%s initial       
         
           /***** Definitions ******/
  
  
  /* punctuation */
           
DOT "."
COMMA ","
LEFT_PAR "(" 
RIGHT_PAR ")"
LEFT_BR "["
RIGHT_BR "]"
SLASH "/"
MINUS "-"
PLUS "+"
COLON ":"
NEGATION "~" 
EXCLAMATION "!"
QUESTION "?"
OR "|"
AND "&"
Equal "="
Greater ">"
Less "<"

  /* characters and identifiers */
  
lower_case [a-z_]
upper_case [A-Z]
alphabetic {lower_case}|{upper_case}
digit [0-9]
alphanumeric {alphabetic}|{digit}  
id_char {alphanumeric}|"_"
wrong_beginning [^.,:()[\]/'\ %a-zA-Z0-9\t\n+\-~&|=<!?]

NONNUMERIC_FUNCTOR {lower_case}{id_char}*
NUMERIC_CONSTANT ({MINUS}?)({digit}+)(({DOT}{digit}+)?) 
VAR_ID {upper_case}{id_char}*
WRONG_IDENTIFIER {NUMERIC_CONSTANT}{alphabetic}{id_char}*

             
   
%%
          /***** Rules ******/

  /* skipping comments/header information to the end of line */

"%"(.)* {}; 

  /* skipping blanks  */       
 
(" ")+  { charNum += yyleng; };
(\t)+  { charNum += yyleng; numOfTabs += yyleng; };
(\x0d)+ { charNum += yyleng; }; /* CtrlM */
          
  /* counting lines */
  
\n { lineNum++; charNum = 1; numOfTabs = 0; };

  /* strings enclosed in single quotes */

"'"          { 
              charNum++; 
              BEGIN(string); 
             };
 
<string>([^'\n]*)/"'"  {
                        charNum += yyleng;
                        numOfTabs += countTabs(yytext);
                        ASSIGN_YYLVAL(yytext);
                        BEGIN(end_of_string);
                        return STRING;
                       };
                       
<end_of_string>("'")  { charNum++; BEGIN(initial); };
              
                

<string>([^'\n]*)/\n {
                      charNum += yyleng;
                      numOfTabs += countTabs(yytext);
                      ASSIGN_YYLVAL(yytext);
                      lineNum++;                   
                      charNum = 1;
                      numOfTabs = 0;
                      BEGIN(initial);
                      return SCAN_ERROR;
                     }; 
         
  /* punctuation */
           
{DOT}        { charNum++; MemoCoord(); return DOT; };
{COMMA}      { charNum++; MemoCoord(); return COMMA; }; 
{LEFT_PAR}   { charNum++; MemoCoord(); return LEFT_PAR; }; 
{RIGHT_PAR}  { charNum++; MemoCoord(); return RIGHT_PAR; }; 
{LEFT_BR}    { charNum++; MemoCoord(); return LEFT_BR; }; 
{RIGHT_BR}   { charNum++; MemoCoord(); return RIGHT_BR; }; 
{MINUS}      { charNum++; MemoCoord(); return MINUS; };
{PLUS}       { charNum++; MemoCoord(); return PLUS; };
{COLON}      { charNum++; MemoCoord(); return COLON; };  

{NEGATION}     { charNum++; MemoCoord(); return NEGATION; }; 
{EXCLAMATION}  { charNum++; MemoCoord(); return EXCLAMATION; };
{QUESTION}     { charNum++; MemoCoord(); return QUESTION; };
{OR}           { charNum++; MemoCoord(); return OR; };
{AND}          { charNum++; MemoCoord(); return AND; };
{Equal}        { charNum++; MemoCoord(); return Equal; };
{Greater}      { charNum++; MemoCoord(); return Greater; };
{Less}         { charNum++; MemoCoord(); return Less; };

   /* identifiers */
   
{NONNUMERIC_FUNCTOR} {
                      charNum += yyleng;
                      ASSIGN_YYLVAL(yytext);
                      return NONNUMERIC_FUNCTOR;
                     };
                     
{NUMERIC_CONSTANT} {
                    charNum += yyleng;
                    ASSIGN_YYLVAL(yytext);
                    return NUMERIC_CONSTANT;
                   };

{VAR_ID} {
          charNum += yyleng;
          ASSIGN_YYLVAL(yytext);
          return VAR_ID;
         };

{WRONG_IDENTIFIER} {
                    charNum += yyleng;
                    ASSIGN_YYLVAL(yytext);
                    return SCAN_ERROR;                       
                   }; 

{wrong_beginning}+ {
                    charNum += yyleng;
                    ASSIGN_YYLVAL(yytext);
                    return SCAN_ERROR;                       
                   }; 


%%

