/** This software is an adaptation of the theorem prover Vampire for
 * working with large knowledge bases in the KIF format, see 
 * http://www.prover.info for publications on Vampire.
 *
 * Copyright (C) Andrei Voronkov and Alexandre Riazanov
 *
 * @author Alexandre Riazanov <riazanov@cs.man.ac.uk>
 * @author Andrei Voronkov <voronkov@cs.man.ac.uk>, <andrei@voronkov.com>
 *
 * @date 06/06/2003
 * 
 * This software is free software; you can redistribute it and/or
 * modify it under the terms of the GNU Lesser General Public
 * License as published by the Free Software Foundation; either
 * version 2.1 of the License, or (at your option) any later version.
 * 
 * This library is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
 * Lesser General Public License for more details.
 *
 * You should have received a copy of the GNU Lesser General Public
 * License along with this library; if not, write to the Free Software
 * Foundation, Inc., 59 Temple Place, Suite 330, Boston, MA  02111-1307  USA
 */

//
// File:         Tabulate.cpp
// Description:  
// Created:      May 25, 2003
// Author:       Andrei Voronkov
// mail:         voronkov@cs.man.ac.uk
//=======================================================


#include <iostream>

#ifdef _MSC_VER
#  include <winsock2.h>
#  include <process.h>
#else 
#  include <unistd.h>
#endif

#include <ctime>

#include "assert.hpp"
#include "Tabulate.hpp"
#include "Options.hpp"
#include "Statistics.hpp"
#include "Refutation.hpp"


// 26/05/2003 Manchester
// _str = 0 means no tabulation
Tabulate::Tabulate ()
  :
  _str(0)
{
} // Tabulate::Tabulate


// 26/05/2003 Manchester
Tabulate::~Tabulate() 
{
  delete _str;
} // Tabulate::~Tabulate


// 26/05/2003 Manchester
void Tabulate::result (char r)
{
  if (! _str) {
    return;
  }

  const char* result = 0;

  switch (r) 
    { 
    case '+':
      result = "+"; 
      break;

    case '-':
      result = "-"; 
      break;

    case '0':
      result = "0"; 
      break;

    default:
      ASS(false);
    }

  unary("result", result);
} // Tabulate::result (char r)


// 26/05/2003 Manchester
void Tabulate::tabulate (const Options& options)
{
  const char* tabFileName = options.tab ();
  if (! tabFileName ) {
    return;
  }

  // tabFileName defined
  _str = new ofstream (tabFileName, ios::app);
  if ( ! *_str ) {
    throw MyException ( "Can not open file for tabulation");
  }

  // compute test number and test id
  _number = options.testNumber();

  // tabulate problem name
  const char* problemFileName = options.inputFile();
  int c = strlen(problemFileName) - 1; 
  while (c >= 0 && problemFileName[c] != '/') {
    c--;
  }
  c++;
  char* problemName = new char[strlen(problemFileName) - c + 1];
  strcpy(problemName,problemFileName + c); 
  // create header
  *_str << "%======== " 
	<< options.testId() 
	<< " : " 
	<< problemFileName 
	<< " ==========\n";
  // output problem name
  unary("problem", problemName);
  delete [] problemName;

  // tabulate runtime information
  char hostname[256];
  gethostname(hostname,256);
  binary("runtime", "hostname", hostname);

  // runtime information, date and time
  time_t jobTime = ::time(static_cast<time_t*>(0));
  tm* gmt = gmtime(&jobTime); 
  date (gmt->tm_mday, gmt->tm_mon + 1, 1900 + gmt->tm_year);
  time (gmt->tm_hour, gmt->tm_min, gmt->tm_sec);

  // version information
  version ("number", VampireKernel::versionNumber());
  version ("description", VampireKernel::versionDescription());
  version ("date", __DATE__);
  version ("time", __TIME__);

  // options
  options.tabulate (*_str);
} // Tabulation::tabulate (const Options& options)


// 26/05/2003 Manchester
void Tabulate::tabulate (const Statistics& statistics)
{
  if (! _str) {
    return;
  }
  statistics.tabulate (*_str);
} // void Tabulate::tabulate (const Statistics& statistics)


// 26/05/2003 Manchester
void Tabulate::tabulate (const Refutation& ref)
{
  if (! _str) {
    return;
  }
} // void Tabulate::tabulate (const Refutation&)


// 26/05/2003 Manchester
void Tabulate::unary (const char* predicate, const char* arg) 
{
  openUnit (predicate);
  quotedAtom(arg);
  closeUnit ();
}


// 26/05/2003 Manchester
void Tabulate::closeUnit() 
{ 
  *_str << ").\n";
}


// 26/05/2003 Manchester
void Tabulate::quotedAtom (const char* s) 
{
  *_str << '\'' << s << '\''; 
}  


// 26/05/2003 Manchester
void Tabulate::errorMessage (const char* errorQualifier,const char* msg)
{
  binary ("error", errorQualifier, msg);
  _str->flush (); 
}


// 26/05/2003 Manchester
void Tabulate::binary (const char* predicate, 
		       const char* arg1,
		       const char* arg2)
{
  openUnit (predicate);
  *_str << arg1 << ',';
  quotedAtom(arg2);
  closeUnit ();
}


// 27/05/2003 Manchester
void Tabulate::binary (const char* predicate, 
		       const char* arg1,
		       float arg2)
{
  openUnit (predicate);
  *_str << arg1 << ',' << arg2;
  closeUnit ();
}


// 27/05/2003 Manchester
void Tabulate::version (const char* field, const char* value)
{
  binary ("version", field, value);
}


// 27/05/2003 Manchester
void Tabulate::version (const char* field, float value)
{
  binary ("version", field, value);
}


// 27/05/2003 Manchester
void Tabulate::time (int hour, int min, int sec)
{
  openUnit ("runtime");
  *_str << "time" 
	<< ",\'"
	<< (hour < 10 ? "0" : "")
	<< hour 
	<< ':' 
	<< (min < 10 ? "0" : "")
	<< min 
	<< '.' 
	<< (sec < 10 ? "0" : "")
	<< sec
	<< '\'';
  closeUnit ();
} // Tabulate::time


// 27/05/2003 Manchester
void Tabulate::date (int day, int month, int year)
{
  openUnit ("runtime");
  *_str << "date" 
	<< ",\'" 
	<< day
	<< '/' 
	<< month 
	<< '/' 
	<< year
	<< '\'';
  closeUnit ();
} // Tabulate::date


// 26/05/2003 Manchester
void Tabulate::openUnit (const char* predicate)
{
  *_str << predicate 
	<< '('
	<< _number
	<< ',';
} // Tabulate::openUnit (const char* predicate)


/*
void Tabulation::initialise (Options& opts, const char* jobId)
{
  const char* testId = opts.testId ();

  tabulation = new VK::Tabulation();
  tabulation->assignStream (tabStream);
  tabulation->assignJob (jobId);

  tabulation->header (testId, problemFileBaseName);   
  tabulation->problemName (problemFileBaseName);
#ifndef NO_DEBUG
  BK::Debug::assignJob(problemFileBaseName);
#endif 

  tabulation->testId (testId);


  return tabStream;
} // createTab




void Tabulate::open_ks_unit() 
{
  *_str << "vkout("; 
  quoted_atom(_jobId);
  *_str << ',' << _kernelSessionId << ',';
}    
  
  
void Tabulate::open_name() 
{ 
  *_str << '['; 
} 


void Tabulate::close_name() { *_str << ']'; }

void Tabulate::comma() { *_str << ','; }
void Tabulate::lbr() { *_str << '['; }
void Tabulate::rbr() { *_str << ']'; }
void Tabulate::lpar() { *_str << '('; }
void Tabulate::rpar() { *_str << ')'; }
void Tabulate::string(const char* s) { *_str << '\"' << s << '\"'; }
void Tabulate::number(int n) { *_str << n; } 
void Tabulate::number(unsigned n) { *_str << n; } 
void Tabulate::number(long n) { *_str << n; } 
void Tabulate::number(unsigned long n) { *_str << n; } 
void Tabulate::number(float n) { *_str << n; }
void Tabulate::number(double n) { *_str << n; }
void Tabulate::boolean(bool b) { *_str << b; }
void Tabulate::day(int dd) { number(dd); }
void Tabulate::month(int mm) { number(mm); }
void Tabulate::year(int yy) { number(yy); }
void Tabulate::date(int dd,int mm,int yy) 
{ 
  lbr(); day(dd); comma(); month(mm); comma(); year(yy); rbr(); 
}
void Tabulate::hour(int hh) { number(hh); } 
void Tabulate::minute(int mm) { number(mm); }
void Tabulate::second(int ss) { number(ss); } 
void Tabulate::time(int hh,int mm,int ss) 
{
  lbr(); hour(hh); comma(); minute(mm); comma(); second(ss); rbr(); 
}
void Tabulate::simple_name(const char* n) { open_name(); quoted_atom(n); close_name(); }
void Tabulate::simple_name(const char* n1,const char* n2)
{ 
  open_name(); quoted_atom(n1); comma(); quoted_atom(n2); close_name(); 
}
void Tabulate::simple_name(const char* n1,const char* n2,const char* n3)
{ 
  open_name(); quoted_atom(n1); comma(); quoted_atom(n2); comma(); quoted_atom(n3); close_name(); 
}

void Tabulate::signature_symbol(bool predicate,const char* printName,ulong arity)
{
  lbr();
  *_str << ((predicate)? "predicate" : "function");
  comma();
  *_str << printName;
  comma();
  number(arity);
  rbr();
}
  
void Tabulate::numeric_constant(const double& val)
{
  lbr();
  *_str << "function";
  comma();
  *_str << val;
  comma();
  number(0UL);
  rbr();
}


//============ derived shorthands: =======================================

void Tabulate::option(const char* optName,long optVal)
{
  open_unit(); simple_name("option",optName); comma(); number(optVal); close_unit();
   
}
void Tabulate::option(const char* optName,unsigned long optVal)
{
  open_unit(); simple_name("option",optName); comma(); number(optVal); close_unit();
   
}
void Tabulate::option(const char* optName,bool optVal)
{
  open_unit(); simple_name("option",optName); comma(); boolean(optVal); close_unit();
      
}
void Tabulate::option(const char* optName,const char* optVal) 
{
  open_unit(); simple_name("option",optName); comma(); quoted_atom(optVal); close_unit();
   
} 
void Tabulate::option(const char* optName,float optVal)
{
  open_unit(); simple_name("option",optName); comma(); number(optVal); close_unit();
   
} 
void Tabulate::option(const char* optName,double optVal)
{
  open_unit(); simple_name("option",optName); comma(); number(optVal); close_unit();
   
}
  
void Tabulate::stat(const char* paramName,bool paramVal)
{ 
  open_unit(); simple_name("stat",paramName); comma(); boolean(paramVal); close_unit(); 
}
void Tabulate::stat(const char* paramName,int paramVal)
{   
  open_unit(); simple_name("stat",paramName); comma(); number(paramVal); close_unit(); 
}
void Tabulate::stat(const char* paramName,unsigned paramVal)
{
  open_unit(); simple_name("stat",paramName); comma(); number(paramVal); close_unit();
}
void Tabulate::stat(const char* paramName,long paramVal)
{
  open_unit(); simple_name("stat",paramName); comma(); number(paramVal); close_unit();
}
void Tabulate::stat(const char* paramName,unsigned long paramVal)
{
  open_unit(); simple_name("stat",paramName); comma(); number(paramVal); close_unit();
}
void Tabulate::stat(const char* paramName,float paramVal)
{
  open_unit(); 
  simple_name("stat",paramName); 
  comma(); 
  number(paramVal); 
  close_unit();
}
void Tabulate::stat(const char* paramName,double paramVal)
{
  open_unit(); simple_name("stat",paramName); comma(); number(paramVal); close_unit();
}
void Tabulate::stat(const char* paramName,const char* paramVal)
{
  open_unit(); simple_name("stat",paramName); comma(); quoted_atom(paramVal); close_unit();
}

void Tabulate::terminationReason(const char* tr)
{
  open_unit(); 
  simple_name("termination_reason"); 
  comma(); 
  quoted_atom(tr); 
  close_unit();
}  


void Tabulate::headerPrecedence(bool positive,const char* predicateName,ulong arity,long precedence)
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
}

void Tabulate::symbolPrecedence(bool predicate,const char* symbolName,ulong arity,long precedence)
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
}

void Tabulate::numericConstantPrecedence(const double& val,long precedence)
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
}

void Tabulate::symbolWeight(bool predicate,
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
}

void Tabulate::numericConstantWeight(const double& val,
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
}


void Tabulate::signatureFunction(bool skolem,const char* symbolName,ulong arity)
{
  open_unit(); 
  simple_name("signature", ((skolem)? "skolem" : "input")); 
  comma(); 
  signature_symbol(false,symbolName,arity);
  close_unit();
}

void Tabulate::signaturePredicate(bool skolem,const char* symbolName,ulong arity)
{
  open_unit(); 
  simple_name("signature", ((skolem)? "skolem" : "input")); 
  comma(); 
  signature_symbol(true,symbolName,arity);
  close_unit();
}



void Tabulate::signatureNumericConstant(bool skolem,const double& val)
{
  open_unit(); 
  simple_name("signature", ((skolem)? "skolem" : "input")); 
  comma(); 
  numeric_constant(val);
  close_unit();
}

  
void Tabulate::answerPredicate(const char* predicateName,ulong arity)
{
  open_unit(); 
  simple_name("answer_predicate"); 
  comma(); 
  signature_symbol(true,predicateName,arity);
  close_unit();
}
  

void Tabulate::proofClause(const Clause* cl);

*/
