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
//  File assert.cpp
//


#include "assert.hpp"


MyException::MyException (const char* file, int line) 
  :  
  _file (file),                       
  _line (line)                        
{ 
}  // MyException::MyException

MyException::MyException (const char* msg) 
  :  
  _file (msg),
  _line (-1)                        
{
} // MyException::MyException                      


// added when _file changed to string
// 27/09/2002
MyException::MyException (const string& msg) 
  :  
  _file (msg),
  _line (-1)                        
{
} // MyException::MyException                      


void MyException::cry (ostream& str)
{
  if ( _line != -1 ) {
    str << "Error in file: "
	<< _file
	<< ", line: " << _line
	<< '\n' << flush;
  }
  else {
    str << "Error: " <<_file << '\n';
  }
} // MyException::cry


MemoryException::MemoryException (const char* file, int line) :  
  _file (file),                       
  _line (line)                        
{
  cry (cout);
} // MemoryException::MemoryException

void MemoryException::cry (ostream& str)
{
  str << "Memory could not be allocated: "
      << _file
      << ", line: " << _line
      << '\n';
}  // MemoryException::cry

