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
//  file Literal.hpp
//  defines class Literal
//


#ifndef __Literal__
#define __Literal__


#include "Atom.hpp"


// ******************* class Literal, definition *********************


class Literal {
 private:
  class Data;

 public:
  // constructor
  Literal ();
  Literal (bool sign, const Atom& atom);
  Literal (const Literal& t);
  Literal (const VampireKernel::Literal* lit, const VampireKernel& kernel);
  ~Literal ();
  void operator= (const Literal& rhs);

  // query/change the structure
  bool positive () const;
  bool negative () const;
  bool sign () const;
  const Atom& atom () const;
  bool exists () const;

  // various
  bool isless (Literal l) const;       // comparison, used for normalisation
  Compare compare (Literal l) const; // comparison, used for normalisation
  void normalize ();
  bool isRenamingOf (Literal l, Substitution& sbst) const;
 private:
  // structure
  Data* _data;
}; // class Literal


class Literal::Data 
#   if DEBUG_PREPRO
    : public Memory <CID_LITERAL>
#   endif
{
 public:
  Data ();
  Data (bool sign, const Atom& atom);

  // structure access
  bool positive () const;
  bool negative () const;
  bool sign () const;
  const Atom& atom () const;

  void ref ();
  void deref ();

 protected:
  // structure
  int _counter;
  bool _sign;
  Atom _atom;
}; // class Literal::Data


class LiteralList 
: public Lst<Literal>
{
 public:
  // constructors
  LiteralList ();
  LiteralList (const LiteralList&);
  explicit LiteralList (const Literal& t); // one-element list
  LiteralList (const Literal& head, const LiteralList& tail);
  explicit LiteralList (LstData<Literal>*);
  LiteralList (const VampireKernel::Literal* lit, const VampireKernel& kernel);

  // inherited functions
  const LiteralList& tail () const
    { return static_cast<const LiteralList&>(Lst<Literal>::tail()); }

  // various
  bool isRenamingOf (LiteralList lst) const;
 private:
  bool isRenamingOf (LiteralList tried, LiteralList remaining, Substitution sbst) const;
}; // class LiteralList


// ******************* class Literal, implementation *********************


inline
Literal::Literal () 
  : 
  _data (0) 
{
} // Literal::Literal


inline
Literal::~Literal () 
{
  if (_data) {
    _data->deref ();
  }
} // Literal::~Literal


inline
Literal::Literal (const Literal& t)
  :
  _data (t._data)
{
  if (_data) {
    _data->ref ();
  }
} // Literal::Literal


inline
Literal::Literal (bool sign, const Atom& atom)
  :
  _data (new Data(sign,atom))
{
} // Literal::Literal (bool sign, const Atom& atom)


inline
bool Literal::exists () const 
{ 
  return _data != 0; 
} // Literal::exists ()


inline
bool Literal::positive () const
{
  return _data->sign();
} // Literal::positive ()


inline
bool Literal::negative () const
{
  return ! _data->sign();
} // Literal::negative ()


inline
bool Literal::sign () const
{
  return _data->sign();
} // Literal::sign ()


inline
const Atom& Literal::atom () const
{
  return _data->atom();
} // Literal::Atom ()


// **************** class Literal::Data implementation ******************


inline
Literal::Data::Data (bool sign, const Atom& atom) 
  : 
  _counter (1),
  _sign (sign), 
  _atom (atom)
{
} // Literal::Data::Data


inline
bool Literal::Data::sign () const 
{ 
  return _sign; 
} // Literal::Data::sign ()


inline
const Atom& Literal::Data::atom () const 
{ 
  return _atom; 
} // Literal::Data::atom ()


inline
void Literal::Data::ref () 
{ 
  ASS (this);

  _counter++;
} // Literal::Data::ref ()


inline
void Literal::Data::deref () 
{ 
  ASS (this);
  ASS (_counter > 0);
  _counter--;

  if (_counter == 0) {
    delete this;
  }
} // Literal::Data::deref ()


// ******************* LiteralList definitions ************************

inline
LiteralList::LiteralList () 
  : 
  Lst<Literal> ()
{
} // LiteralList::LiteralList


inline
LiteralList::LiteralList (const LiteralList& ts)
  :
  Lst<Literal> (ts)
{
} // LiteralList::LiteralList


inline
LiteralList::LiteralList (LstData<Literal>* d)
  :
  Lst<Literal> (d)
{
} // LiteralList::LiteralList


inline
LiteralList::LiteralList (const Literal &hd, const LiteralList& tl)
  :
  Lst<Literal> (hd,tl)
{
} // LiteralList::LiteralList


inline
LiteralList::LiteralList (const Literal &hd)
  :
  Lst<Literal> (hd)
{
} // LiteralList::LiteralList


#endif // Literal
