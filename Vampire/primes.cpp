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
// File primes.cpp
// computer prime numbers
//

#include <iostream> 
#include <cstdlib> 


int main ( int argc, char* argv [ ] )
{
  if (argc != 2) {
    cout << "This program must be called with one argument\n";
    return EXIT_FAILURE;
  }

  int total = atoi (argv[1]);
  if (total <= 0) {
    cout << "The argument must be a positive integer\n";
    return EXIT_FAILURE;
  }

  int* primes = new int [total+2];
  if (! primes) {
    cout << "The number is too large, insufficient memory\n";
    return EXIT_FAILURE;
  }

  // the first prime number
  primes[0] = 2;
  primes[1] = 3;
  int last = 1;
  cout << "2 3";
  // searching for the next prime p
  for (int p = primes[last] + 2; ;p += 2) {
    for (int ind = 1; ; ind++) {
      int pind = primes[ind];
      div_t d = div (p,pind);
      if (d.rem == 0) { // divisible, not prime
	// search p+2
	break;
      }
      long double lim = pind*pind;
      if (lim > p) { // search limit exceeded, p is prime
	primes [++last] = p;
	cout << ' ' << p;
	if (last == total) { // all primes generated
	  cout << "\n";
	  return EXIT_SUCCESS;
	}
	break;
      }
    }
  }
} // main

// --- end of file ---
