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
//  File tracer.h
//  01/05/2002 Manchester
//  24/10/2002 Manchester, changed after talking with Shura
//

#ifndef __MyTracer__
#  define __MyTracer__

#if DEBUG_TRACE_ALL
#  define TRACER(Fun) MyTracer tmp (Fun)


#include <iostream>


using namespace std;


class MyTracer {
 public:
  MyTracer (const char* fun);
  ~MyTracer ();
  static void printStack (ostream&);

 private:
  const char* _fun;
  MyTracer* _previous;
  static MyTracer* _current;

  void printStack (ostream&, int& depth);
};


#else
#  define TRACER(Fun) 
#endif

#endif // MyTracer
