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
 *
 * Also add information on how to contact you by electronic and paper mail.
 */

//
//  File assert.h
//  991120 Manchester, made from debug.h in kk
//


#ifndef __assert__
#  define __assert__


#include <iostream>
#include <cstdlib>
#include <string>
#include "Tracer.hpp"

using namespace std;


// ***************** MyException ********************


class MyException {
 public:                                
  MyException (const char* file, int line);
  MyException (const char* msg);
  MyException (const string& msg);
  virtual ~MyException () {}

  virtual void cry (ostream& str);

 private:                               
  string _file;
  int _line;
}; // MyException


// ***************** MemoryException ********************


class MemoryException {
 public:                                
  MemoryException (const char* file, int line);
  void cry (ostream& str);

 private:                               
  const char* _file;
  int _line;
}; // MemoryException


#define NO_MEMORY                                 \
      throw MemoryException (__FILE__,__LINE__);  

// ***************** ASSERT and IF ********************


#if DEBUG_PREPRO

#define EXCEPTION(Class)                  \
  class Class :                           \
    public MyException                    \
  {                                       \
   public:                                \
    Class (const char* file, int line)    \
      : MyException (file, line) {}       \
  }

// renamed to ASS not too conflict with Shura's ASSERT
#define ASS(Cond)                             \
    if (! (Cond)) {                           \
      MyTracer::printStack (cerr);            \
      MyTracer::printStack (cout);            \
      throw MyException (__FILE__,__LINE__);  \
    }                                         


#define IF( Switch, Statement )               \
    if ( (Switch) ) {                         \
      Statement;                              \
    }                                         \
    else {}

#else // ! DEBUG_PREPRO 
#define ASS(Cond)
#endif


#endif // __assert__


