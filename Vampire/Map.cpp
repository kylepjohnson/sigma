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
//  file Map.cpp 
//  defines possible lengths for maps
//


#include "Map.hpp"


// a sequence of primes, obtained by the program primes.cpp
// each next prime in the table is about double the previous prime
// 29/09/2002 Manchester
int _mapLengths[] =
  { 0, 29, 47, 89, 167, 277, 547, 1229, 2749, 6143, 12569, 
    26407, 48619, 93187, 187973,
    389173, 814309, 1712369, 3451241, 7368791, 17144507, 37667713,
    76918277, 141650963, 217645199,  0};

// a test
// 29/09/2002 Manchester
/*
int q() {
  Map<long, long> map;
  for (int i = 0; i < 30000000; i++) {
    long r = random ();
    
    long found;
    if (map.find (r, found)) {
      //cout << "Found: " << r << "\n";
    }
    else {
      map.insert (r,r);
    }
  }
  cout << "INSERTED\n";
  for (int i = 0; i < 100000000; i++) {
    long found;
    if (map.find (i,found)) {
      // cout << i << ": " << found << '\n';
    }
  }

  return 0;
}
*/

