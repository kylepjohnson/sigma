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
//  File random.cpp: random numbers
//
//  990810 Uppsala
//  990920 Manchester: my own random number generator added for efficiency
//  20/02/2000 Manchester: changed slightly


#include "Random.hpp"


int Random::_seed = 1;          // default seed in ISO C
int Random::_remainingBits = 0; // remaining number if random bits
// number of random bits that can be extracted from one random integer
const int Random::_bitsPerInt = Random::bitsPerInt ();
int Random::_bits;             // integer used for extracting random bits


// finds _bitsPerInt
int Random::bitsPerInt ()
{
  int b = max () + 1;
  int bits = -1;

  while ( b != 0 ) {
    b /= 2;
    bits ++;
  }

  return bits;
} // Random::bitsPerInt


