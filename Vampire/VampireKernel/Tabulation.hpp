//
// File:         Tabulation.hpp
// Description:  
// Created:      Mar 21, 2000, 16:35
// Author:       Alexandre Riazanov
// mail:         riazanov@cs.man.ac.uk
//=======================================================
#ifndef TABULATION_H
//=======================================================
#define TABULATION_H
#include <iostream>
#include "jargon.hpp"
#include "GlobAlloc.hpp"
#include "VampireKernelDebugFlags.hpp"
#include "TermWeightType.hpp"
//========================================================
#undef DEBUG_NAMESPACE
#ifdef DEBUG_TABULATION
#define DEBUG_NAMESPACE "Tabulation"
#endif
#include "debugMacros.hpp"
//========================================================
namespace VK
{

  class Clause;
  class Tabulation   
  {
  public:
    Tabulation() : _str(0), _jobId("unknown_job_id") 
    {
      CALL("constructor Tabulation()");
    };
    ~Tabulation() 
    {
      CALL("destructor ~Tabulation()");
    };
    void assignStream(ostream* str) { _str = str; };
    void assignJob(const char* jobId) { _jobId = jobId; };
    void assignKernelSession(ulong id) { _kernelSessionId = id; };

    // basic operations  
    void flush() { _str->flush(); };
    void open_unit() 
    {
      CALL("open_unit()");
      *_str << "vout("; 
      quoted_atom(_jobId);
      *_str << ','; 
    };    

    void open_ks_unit() 
    {
      CALL("open_ks_unit()");
      *_str << "vkout("; 
      quoted_atom(_jobId);
      *_str << ',' << _kernelSessionId << ',';
    };    
  
    void close_unit() { *_str << ").\n"; flush(); };
  
    void open_name() { *_str << '['; }; 
    void close_name() { *_str << ']'; };

    void comma() { *_str << ','; };
    void lbr() { *_str << '['; };
    void rbr() { *_str << ']'; };
    void lpar() { *_str << '('; };
    void rpar() { *_str << ')'; };
    void quoted_atom(const char* s) 
    {
      CALL("quoted_atom(const char* s)");
      *_str << '\'' << s << '\''; 
    };  
    void string(const char* s) { *_str << '\"' << s << '\"'; };
    void number(int n) { *_str << n; }; 
    void number(unsigned n) { *_str << n; }; 
    void number(long n) { *_str << n; }; 
    void number(unsigned long n) { *_str << n; }; 
    void number(float n) { *_str << n; };
    void number(double n) { *_str << n; };
    void boolean(bool b) { *_str << b; };
    void day(int dd) { number(dd); };
    void month(int mm) { number(mm); };
    void year(int yy) { number(yy); };
    void date(int dd,int mm,int yy) 
    { 
      lbr(); day(dd); comma(); month(mm); comma(); year(yy); rbr(); 
    };
    void hour(int hh) { number(hh); }; 
    void minute(int mm) { number(mm); };
    void second(int ss) { number(ss); }; 
    void time(int hh,int mm,int ss) 
    {
      lbr(); hour(hh); comma(); minute(mm); comma(); second(ss); rbr(); 
    };
    void simple_name(const char* n) { open_name(); quoted_atom(n); close_name(); };
    void simple_name(const char* n1,const char* n2)
    { 
      open_name(); quoted_atom(n1); comma(); quoted_atom(n2); close_name(); 
    };
    void simple_name(const char* n1,const char* n2,const char* n3)
    { 
      open_name(); quoted_atom(n1); comma(); quoted_atom(n2); comma(); quoted_atom(n3); close_name(); 
    };

    void signature_symbol(bool predicate,const char* printName,ulong arity)
    {
      CALL("signature_symbol(bool predicate,const char* printName,ulong arity)");
      lbr();
      *_str << ((predicate)? "predicate" : "function");
      comma();
      *_str << printName;
      comma();
      number(arity);
      rbr();
    };
  
    void numeric_constant(const double& val)
    {
      CALL("numeric_constant(const double& val)");
      lbr();
      *_str << "function";
      comma();
      *_str << val;
      comma();
      number(0UL);
      rbr();
    };


    //============ derived shorthands: =======================================

    void header(const char* testId,const char* problemName)
    { 
      *_str << "%========  " << testId << " : " << problemName << "  ==========\n";
   
    };
    void problemName(const char* pn) 
    { 
      open_unit(); simple_name("problem_name"); comma(); quoted_atom(pn); close_unit();
    
    };
    void testId(const char* tid)
    {
      open_unit(); simple_name("test_id"); comma(); quoted_atom(tid); close_unit();
   
    };
    void hostName(const char* hn)
    {
      open_unit(); simple_name("host_name"); comma(); quoted_atom(hn); close_unit();   
    };
    void jobDate(int dd,int mm,int yy)
    {
      open_unit(); simple_name("job_date"); comma(); date(dd,mm,yy); close_unit();
   
    };
    void jobTime(int hh,int mm,int ss)
    {
      open_unit(); simple_name("job_time"); comma(); time(hh,mm,ss); close_unit();
   
    };
    void version(float num,const char* desc) 
    {
      open_unit(); simple_name("version"); comma();
      lbr(); number(num); comma(); quoted_atom(desc); rbr(); close_unit(); 
   
    };
    void versionDate()
    {
      open_unit(); simple_name("version_date"); comma(); quoted_atom(__DATE__); close_unit();
   
    };
    void versionTime()
    {
      open_unit(); simple_name("version_time"); comma(); quoted_atom(__TIME__); close_unit();
   
    };
    void option(const char* optName,long optVal)
    {
      CALL("option(const char* optName,long optVal)");
      open_unit(); simple_name("option",optName); comma(); number(optVal); close_unit();
   
    };
    void option(const char* optName,unsigned long optVal)
    {
      CALL("option(const char* optName,unsigned long optVal)");
      open_unit(); simple_name("option",optName); comma(); number(optVal); close_unit();
   
    };
    void option(const char* optName,bool optVal)
    {
      CALL("option(const char* optName,bool optVal)");
      open_unit(); simple_name("option",optName); comma(); boolean(optVal); close_unit();
      
    };
    void option(const char* optName,const char* optVal) 
    {
      CALL("option(const char* optName,const char* optVal)");
      open_unit(); simple_name("option",optName); comma(); quoted_atom(optVal); close_unit();
   
    }; 
    void option(const char* optName,float optVal)
    {
      CALL("option(const char* optName,float optVal)");
      open_unit(); simple_name("option",optName); comma(); number(optVal); close_unit();
   
    }; 
    void option(const char* optName,double optVal)
    {
      CALL("option(const char* optName,double optVal)");
      open_unit(); simple_name("option",optName); comma(); number(optVal); close_unit();
   
    };
  
    void stat(const char* paramName,bool paramVal)
    { 
      CALL("stat(const char* paramName,bool paramVal)");
      open_unit(); simple_name("stat",paramName); comma(); boolean(paramVal); close_unit(); 
    };
    void stat(const char* paramName,int paramVal)
    {   
      CALL("stat(const char* paramName,int paramVal)");
      open_unit(); simple_name("stat",paramName); comma(); number(paramVal); close_unit(); 
    };
    void stat(const char* paramName,unsigned paramVal)
    {
      CALL("stat(const char* paramName,unsigned paramVal)");
      open_unit(); simple_name("stat",paramName); comma(); number(paramVal); close_unit();
    };
    void stat(const char* paramName,long paramVal)
    {
      CALL("stat(const char* paramName,long paramVal)");
      open_unit(); simple_name("stat",paramName); comma(); number(paramVal); close_unit();
    };
    void stat(const char* paramName,unsigned long paramVal)
    {
      CALL("stat(const char* paramName,unsigned long paramVal)");
      open_unit(); simple_name("stat",paramName); comma(); number(paramVal); close_unit();
    };
    void stat(const char* paramName,float paramVal)
    {
      CALL("stat(const char* paramName,float paramVal)");
      open_unit(); 
      simple_name("stat",paramName); 
      comma(); 
      number(paramVal); 
      close_unit();
    };
    void stat(const char* paramName,double paramVal)
    {
      CALL("stat(const char* paramName,double paramVal)");
      open_unit(); simple_name("stat",paramName); comma(); number(paramVal); close_unit();
    };
    void stat(const char* paramName,const char* paramVal)
    {
      CALL("stat(const char* paramName,const char* paramVal)");
      open_unit(); simple_name("stat",paramName); comma(); quoted_atom(paramVal); close_unit();
    };
    void kernelSessionResult(char r)
    {
      open_unit();
      simple_name("result"); comma();
      if (r == '+') { quoted_atom("+"); }
      else 
	if (r == '-') { quoted_atom("-"); }
	else quoted_atom("0");       
      close_unit();
    };

    void result(char r)
    {
      open_unit();
      simple_name("result"); comma();
      if (r == '+') { quoted_atom("+"); }
      else 
	if (r == '-') { quoted_atom("-"); }
	else quoted_atom("0");       
      close_unit();
    };

    void terminationReason(const char* tr)
    {
      open_unit(); 
      simple_name("termination_reason"); 
      comma(); 
      quoted_atom(tr); 
      close_unit();
    };  


    void headerPrecedence(bool positive,const char* predicateName,ulong arity,long precedence)
    {
      open_unit(); 
      simple_name("header_precedence");  
      comma();
      lbr(); 
      if (positive) { *_str << "++"; } else *_str << "--";
      signature_symbol(true,predicateName,arity);
      comma(); 
      number(precedence);       
      rbr();
      close_unit();
    };

    void symbolPrecedence(bool predicate,const char* symbolName,ulong arity,long precedence)
    {
      open_unit(); 
      simple_name("symbol_precedence");   
      comma();
      lbr(); 
      signature_symbol(predicate,symbolName,arity);
      comma();
      number(precedence);   
      rbr();
      close_unit();
    };

    void numericConstantPrecedence(const double& val,long precedence)
    {     
      open_unit(); 
      simple_name("symbol_precedence");   
      comma();
      lbr(); 
      numeric_constant(val);
      comma();
      number(precedence);   
      rbr();
      close_unit();
    };

    void symbolWeight(bool predicate,
		      const char* symbolName,
		      ulong arity,
		      long weightConstPart)
    {        
      open_unit(); 
      simple_name("symbol_weight");   
      comma();
      lbr(); 
      signature_symbol(predicate,symbolName,arity);
      comma();
      number(weightConstPart);   
      rbr();
      close_unit(); 
    };

    void numericConstantWeight(const double& val,
			       long weightConstPart)
    {       
      open_unit(); 
      simple_name("symbol_weight");   
      comma();
      lbr(); 
      numeric_constant(val);
      comma();
      number(weightConstPart);   
      rbr();
      close_unit(); 
    };


    void signatureFunction(bool skolem,const char* symbolName,ulong arity)
    {
      open_unit(); 
      simple_name("signature", ((skolem)? "skolem" : "input")); 
      comma(); 
      signature_symbol(false,symbolName,arity);
      close_unit();
    };

    void signaturePredicate(bool skolem,const char* symbolName,ulong arity)
    {
      open_unit(); 
      simple_name("signature", ((skolem)? "skolem" : "input")); 
      comma(); 
      signature_symbol(true,symbolName,arity);
      close_unit();
    };



    void signatureNumericConstant(bool skolem,const double& val)
    {
      open_unit(); 
      simple_name("signature", ((skolem)? "skolem" : "input")); 
      comma(); 
      numeric_constant(val);
      close_unit();
    };

  
    void answerPredicate(const char* predicateName,ulong arity)
    {
      open_unit(); 
      simple_name("answer_predicate"); 
      comma(); 
      signature_symbol(true,predicateName,arity);
      close_unit();
    };
  

    void proofClause(const Clause* cl);

    void errorMessage(const char* errorQualifier,const char* msg)
    {
      *_str << "verror("; 
      quoted_atom(_jobId);
      comma(); 
      lbr(); 
      quoted_atom(errorQualifier);
      rbr();
      comma();
      string(msg); 
      *_str << ").\n"; 
      flush(); 
    };

  
  private:
    ostream* _str;
    const char* _jobId;  
    ulong _kernelSessionId;
  }; // class Tabulation

}; // namespace VK

//=======================================================
#endif
