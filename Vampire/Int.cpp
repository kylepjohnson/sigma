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
// File Int.cpp
//
//  09/06/2000 Manchester, toString()
//


#include "Int.hpp"
#include "assert.hpp"


void Int::toString ( int i, char* str )
{
  if ( i == 0 ) {
    str[0] = '0';
    str[1] = 0;
    return;
  }
  // i != 0
  if ( i < 0 ) {
    *str = '-';
    str ++;
    i = -i;
  }
  // i > 0

  char tmp [20];
  // put digits to tmp, but in the reverse order
  int j = 0;
  while ( i != 0 ) {
    tmp[j] = (i % 10) + '0';
    i = i / 10;
    j++;
  }
  // copy digits to tmp, converting then in the right order
  j--;
  while ( j >= 0 ) {
    *str = tmp[j];
    str++;
    j--;
  }
  *str = 0;
} // Int::toString


// return a string representing the number
// 27/05/2003 Manchester
char* Int::toString (int i)
{
  char tmp [20];
  toString (i,tmp);

  char* result = new char [strlen(tmp)+1];
  if (! result) {
    NO_MEMORY;
  }
  strcpy (result, tmp);

  return result;
} // Int::toString (int i)
