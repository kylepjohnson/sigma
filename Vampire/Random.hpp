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
//  File Random.hpp: random number generation
//
//  18/02/2000 Manchester
//


#ifndef __RANDOM__
#  define __RANDOM__


#include <cstdlib>
#include <ctime>


//******************* class Random ********************


class Random 
{
  // structure
  
  static int _seed;             // currently used random seed
  static int _remainingBits;    // remaining number if random bits 
  static const int _bitsPerInt; // number of random bits that can be extracted from one random integer
  static int _bits;             // integer used for extracting random bits

  static int bitsPerInt ();     // finds _bitsPerInt;

 public:

  // maximal random integer
  static int max ();
  // generate random integer between 0 and max
  static int integer ();
  // generate random integer between 0 and modulus-1
  static int integer ( int modulus );
  // generate random bit (0 or 1)
  static int bit ();
  // sets the random seed to s
  static void seed (int s);
  // show the current seed
  static int seed ();
  // make seed truly random (calls time ())
  static void trulyRandom ();
}; // class Random


//******************* class Random, implementation ********************


inline
void Random::seed ( int s )
{
  _seed = s;
  srand ( s );
}


inline
int Random::seed ()
{
  return _seed;
}


inline
int Random::max ()
{
  return RAND_MAX;
}


inline
int Random::integer ()
{
  return rand ();
}


inline
int Random::integer ( int modulus )
{
  return integer () % modulus;
}


inline
void Random::trulyRandom ()
{
  seed ( time ( 0 ) );
}


inline
int Random::bit ()
{
  if ( _remainingBits == 0 ) {
    _remainingBits = _bitsPerInt;
    _bits = integer ();
  }

  int result = _bits % 2;
  _bits /= 2;
  _remainingBits --;

  return result;
} // Random::bit


#endif


