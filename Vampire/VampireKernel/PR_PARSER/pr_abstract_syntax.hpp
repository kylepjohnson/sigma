#ifndef PR_ABSTRACT_SYNTAX_H
//=======================================================================
#define PR_ABSTRACT_SYNTAX_H

#include <iostream>

//======================================================================
class PR_SYMBOL_OCC;
class PR_TERM;
class PR_TERM_LIST;
class PR_LITERAL;
class PR_LITERAL_LIST;
class PR_FILE_PARS_RESULT;


struct SYMBOL_COORDINATES
 {
  int lineNum;
  int charNum;
  inline void Assign(int ln,int cn) { lineNum = ln; charNum = cn; };
 }; // struct SYMBOL_COORDINATES 

class PR_SYMBOL_OCC
 {
  private:
   const char* symbol;
   int line_num;
   int char_num;
  public:
   PR_SYMBOL_OCC(const char* const sym,int ln,int cn)
    : symbol(sym),
      line_num(ln),
      char_num(cn)
     {};
   PR_SYMBOL_OCC(const char* sym,SYMBOL_COORDINATES& coord)
    : symbol(sym),
      line_num(coord.lineNum),
      char_num(coord.charNum)
     {};   
   inline const char* Symbol() const { return symbol; };   
   inline int LineNum() const { return line_num; };
   inline int CharNum() const { return char_num; };
   inline void Output() { std::cout << symbol; };
          
 }; // class PR_SYMBOL_OCC

class PR_TERM_LIST
 {
  private:
   PR_TERM* term;
   PR_TERM_LIST* next;   
  public:
   PR_TERM_LIST(PR_TERM* t,PR_TERM_LIST* lst)
    : term(t),
      next(lst)
    {};
   PR_TERM_LIST(PR_TERM* t)
    : term(t),
      next((PR_TERM_LIST*)0)
    {};
   inline PR_TERM* Term() const { return term; };
   inline PR_TERM_LIST* Next() const { return next; };
   static PR_TERM* Nth(PR_TERM_LIST* lst,int n);
   static void Inverse(PR_TERM_LIST*& lst);
   static int Length(PR_TERM_LIST* lst);
   static void Output(PR_TERM_LIST* lst);
 }; // class PR_TERM_LIST
  
class PR_TERM
 {
  public:
   enum TAG
    {
     ATOM_FUNCTOR,
     ATOM_STRING,
     ATOM_NUMBER,
     VAR,
     COMPLEX,
     LIST
    };
  private:
   TAG tag;
   PR_SYMBOL_OCC* symbol;
   PR_TERM_LIST* arguments;
  public:
   PR_TERM(TAG t,PR_SYMBOL_OCC* sym)
    : tag(t),
      symbol(sym),
      arguments((PR_TERM_LIST*)0)
      {      
      };
   PR_TERM(PR_SYMBOL_OCC* sym,PR_TERM_LIST* args)
    : tag(COMPLEX),
      symbol(sym),
      arguments(args)
      {
       PR_TERM_LIST::Inverse(arguments);
      };
   PR_TERM(PR_TERM_LIST* args)
    : tag(LIST),
      symbol((PR_SYMBOL_OCC*)0),
      arguments(args)
      {
       PR_TERM_LIST::Inverse(arguments);
      };
   inline TAG Tag() const { return tag; };
   inline PR_SYMBOL_OCC* Symbol() const { return symbol; };
   inline PR_TERM_LIST* Arguments() const { return arguments; };
   void Output();
 }; // class PR_TERM


class PR_LITERAL
 {
  private:
   PR_SYMBOL_OCC* header;
   PR_TERM_LIST* arguments;
  public:
   PR_LITERAL(PR_SYMBOL_OCC* h,PR_TERM_LIST* const args)
    : header(h),
      arguments(args)
    {};
   inline const PR_SYMBOL_OCC* Header() const { return header; };
   inline PR_TERM_LIST* Arguments() const { return arguments; };
   void Output();
 }; // class PR_LITERAL


class PR_LITERAL_LIST
 {
  private:
   PR_LITERAL* literal;
   PR_LITERAL_LIST* next;   
  public:
   PR_LITERAL_LIST(PR_LITERAL* l,PR_LITERAL_LIST* lst)
    : literal(l),
      next(lst)
    {};
   inline PR_LITERAL* Literal() const { return literal; };
   inline PR_LITERAL_LIST* Next() const { return next; };
    
   static inline void Add(PR_LITERAL* l,PR_LITERAL_LIST*& lst)
    {
     lst = new PR_LITERAL_LIST(l,lst);
    };
   static void Output(PR_LITERAL_LIST* lst);
 }; // class PR_LITERAL_LIST

class PR_FILE_PARS_RESULT
 {
  private:
   PR_LITERAL_LIST* literals;
   bool errorFlag;
   const char* errorMessage;
   int errorLineNum;
   int errorCharNum;
  public:
   PR_FILE_PARS_RESULT() : literals((PR_LITERAL_LIST*)0), errorFlag(false) {};
   inline PR_LITERAL_LIST* Literals() const { return literals; };
   inline void AddLiteral(PR_LITERAL* lit) 
    {
     literals = new PR_LITERAL_LIST(lit,literals);
    };
   inline void ErrorAt(const char* msg,int ln,int cn)
    {
     errorFlag = true;
     errorMessage = msg;
     errorLineNum = ln;
     errorCharNum = cn;
    };
   inline bool Error() const { return errorFlag; };
   inline const char* ErrorMessage() const { return errorMessage; };
   inline int ErrorLineNum() const { return errorLineNum; };
   inline int ErrorCharNum() const { return errorCharNum; };
 }; // class PR_FILE_PARS_RESULT

//=====================================================================
#endif
