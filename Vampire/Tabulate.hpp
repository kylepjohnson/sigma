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
// File:         Tabulate.hpp
// Description:  
// Created:      May 25, 2003
// Author:       Andrei Voronkov
// mail:         voronkov@cs.man.ac.uk
//=======================================================


#ifndef __TABULATE__
#define __TABULATE__


// class ostream;
class Options;
class Statistics;
class Refutation;


class Tabulate
{
public:
  explicit Tabulate ();
  ~Tabulate();

  void tabulate (const Options& options);
  void tabulate (const Statistics& statistics);
  void tabulate (const Refutation& refutation);
  void result (char result);
  void errorMessage (const char* errorQualifier, const char* msg);
  bool exists () const { return _str != 0; }

private:
  // structure
  const Options* _options;
  ostream* _str;
  int _number;

  // auxiliary output functions
  void openUnit (const char* predicate);
  void closeUnit();
  void quotedAtom (const char* s);
  void unary (const char* predicate, const char* arg);
  void binary (const char* predicate, const char* arg1, const char* arg2);
  void binary (const char* predicate, const char* arg1, float arg2);
  void time (int hour, int min, int sec);
  void date (int day, int month, int year);
  void version (const char* field, const char* value);
  void version (const char* field, float value);
  void option (const char* field, const char* value);
}; // class Tabulate


#endif // __TABULATE__

