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
//  file Lst.hpp
//  defines class Lst
//

#ifndef __Lst__
#define __Lst__

#include "assert.hpp"

template <class C> class LstData;


template <class C>
class Lst 
{
 public:
  // constructors
  Lst ();
  Lst (const Lst&);
  Lst (const C& head, const Lst& tail);
  explicit Lst (const C& t); // one-element list
  explicit Lst (LstData<C>*);
  ~Lst ();
  void operator = (const Lst& rhs);

  // declared but not defined, to prevent on-heap allocation
  // void* operator new (size_t);
  void append (const Lst&);

  // structure
  bool isEmpty () const;
  bool isNonEmpty () const;
  const C& head () const;
  const C& second () const;
  const Lst& tail () const;
  void pop ();
  bool operator == (const Lst& rhs) const;
  void makeEmpty ();

  // miscellaneous
  int length () const;
  bool member (const C elem) const;
  void push (C elem);
  void reverse (Lst& result) const;
  void reverse (Lst& result, const Lst& appendTo) const;
  void copy (Lst& to) const;

 protected:
  // structure
  LstData<C>* _data;
}; // class Lst


// this class was inside Lst but VC++ had not enough intelligence to compile it
template <class C>
class LstData 
{
 public:
  LstData ();

  inline LstData (const C& t, const Lst<C>& ts)
    :
    _counter (1),
    _head (t),
    _tail (ts)
	{
      TRACER( "LstData::LstData" );
	} // LstData::LstData

  inline explicit LstData (const C& t)
    :
    _counter (1),
    _head (t),
    _tail ((LstData<C>*)0)
	{
	  TRACER( "LstData::LstData" );
	} // LstData::LstData

  inline ~LstData ()
	{
	  TRACER( "LstData::~LstData" );

	  ASS (_counter == 0);
  } // LstData::~LstData

  inline void ref ()
    { 
      ASS (this);

      _counter++;
    } // LstData::ref ()


  inline void deref ()
  { 
    ASS (this);
    ASS (_counter > 0);

    _counter--;
    if (_counter == 0) {
      delete this;
    }
  } // LstData::deref ()

  inline const C& head () const
  { 
    ASS (this);
    ASS (_counter > 0);

    return _head; 
  } // LstData::head

  inline const Lst<C>& tail () const
  { 
    ASS (this);
    ASS (_counter > 0);

    return _tail;
  } // LstData::tail

 private:
  // structure
  unsigned _counter;
  C _head;
  Lst<C> _tail;
}; // LstData


// this class was Lst::Iterator but there was no way for VC++ to
// compile it
template <class C>
class Iterator
{
 public: 
  inline explicit Iterator (const Lst<C>& lst)
    : 
    _list (lst) 
  {
  } // Iterator::Iterator

  inline bool more () const
  { 
    return _list.isNonEmpty(); 
  } // Iterator::more ()

  inline C next ()
  {
    ASS( more() );

    C result (_list.head());
    _list.pop ();
  
    return result;
  } // Iterator::next

  inline void reset (const Lst<C>& lst)
  {
    _list = lst;
  } // Iterator::reset

 private:
  Lst<C> _list;
}; // class Iterator


// ******************* Lst definitions ************************

template <class C>
inline
Lst<C>::Lst () 
  : 
  _data (0) 
{
} // Lst::Lst


template <class C>
inline
Lst<C>::~Lst () 
{
  if (_data) {
    _data->deref ();
  }
} // Lst::~Lst


// copy constructor
// 25/08/2002 Torrevieja
template <class C>
inline
Lst<C>::Lst (const Lst& ts)
  :
  _data (ts._data)
{
  if (_data) {
    _data->ref ();
  }
} // Lst::Lst


// almost a copy constructor
// 25/08/2002 Torrevieja
template <class C>
inline
Lst<C>::Lst (LstData<C>* d)
  :
  _data (d)
{
  if (d) {
    d->ref ();
  }
} // Lst::Lst


// 'cons' list constructor
// 25/08/2002 Torrevieja
template <class C>
inline
Lst<C>::Lst (const C &hd, const Lst& tl)
  :
  _data (new LstData<C> (hd,tl))
{
} // Lst::Lst


// 'cons' list constructor
// 25/08/2002 Torrevieja
template <class C>
inline
Lst<C>::Lst (const C &hd)
  :
  _data (new LstData<C> (hd))
{
} // Lst::Lst


template <class C>
inline
bool Lst<C>::operator == (const Lst& rhs) const
{ 
  return _data == rhs._data; 
} // Lst::operator == (const Lst& rhs) const


template <class C>
inline
bool Lst<C>::isEmpty () const 
{ 
  return _data == 0; 
} // Lst::isEmpty ()


template <class C>
inline
bool Lst<C>::isNonEmpty () const 
{ 
  return _data != 0; 
} // Lst::isNonEmpty ()


template <class C>
inline
const C& Lst<C>::head () const 
{ 
  return _data->head (); 
} // Lst::head


template <class C>
inline
const C& Lst<C>::second () const 
{ 
  return _data->tail().head(); 
}


template <class C>
inline
const Lst<C>& Lst<C>::tail () const 
{ 
  return _data->tail(); 
}


// pop a list
// 25/08/2002 Torrevieja
// 04/05/2003 Manchester changed simply *this = tail () (caused errors???)
template <class C>
void Lst<C>::pop ()
{
  ASS (_data);

  LstData<C>* dtl = tail()._data;
  if (! dtl) {
    _data->deref();
    _data = 0;
    return;
  }

  dtl->ref ();
  _data->deref ();
  _data = dtl;
  //  *this = tail ();
} // Lst::pop


// C is a member of the list
// 05/09/2002 Bolzano
template <class C>
bool Lst<C>::member (const C elem) const
{
  Iterator<C> it (*this);
  while (it.more()) {
    if (it.next() == elem) {
      return true;
    }
  }

  return false;
} // Lst::member


template <class C>
inline
void Lst<C>::push (C elem)
{
  *this = Lst (elem,*this);
} // Lst::push


template <class C>
inline
void Lst<C>::makeEmpty ()
{
  if (_data) {
    _data->deref ();
    _data = 0;
  }
} // Lst::makeEmpty 


template <class C>
void Lst<C>::operator = (const Lst& t)
{
  if (t._data) {
    t._data->ref ();
  }

  if (_data) {
    _data->deref ();
  }

  _data = t._data;
} // Lst::operator=


// list append
// 30/08/2002 Torrevieja, not the fastest implementation,
//  can be made tail-recursive
// 14/04/2003 Torrevieja, type of to changed to reference
template <class C>
void Lst<C>::append (const Lst& to)
{
  if (isEmpty()) {
    *this = to;
    return;
  }

  Lst tl (tail());
  tl.append (to);
  *this = Lst (head(),tl);
} // Lst::append


// list length
// 30/08/2002 Torrevieja, copied from TermList
template <class C>
int Lst<C>::length () const
{
  int len = 0;

  for (LstData<C>* d = _data; d; d = d->tail()._data) {
    len ++;
  }

  return len;
} // Lst::length


// list reverse
// 14/04/2003 Torrevieja
template <class C>
void Lst<C>::reverse (Lst& result, const Lst& appendTo) const
{
  TRACER("LST::reverse/3");

  Lst rev (appendTo);

  Iterator<C> it (*this);
  while (it.more()) {
    rev.push (it.next());
  }

  result = rev;
} // Lst::reverse


// list reverse
// 15/04/2003 Torrevieja
template <class C>
inline
void Lst<C>::reverse (Lst& result) const
{
  TRACER("LST::reverse/2");

  Lst empty;
  reverse (result,empty);
} // Lst::reverse


// list copy
// 25/04/2003 Manchester
template <class C>
inline
void Lst<C>::copy (Lst& to) const
{
  TRACER("Lst::copy");

  if (isEmpty()) {
    return;
  }

  to = Lst (head());
  tail().copy (const_cast<Lst&>(to.tail()));
} // Lst::copy


// **************** Lst::Data definitions *********************

/*
template <class C>
inline 
Lst<C>::Data::Data (const C& t)
  :
  _counter (1),
  _head (t),
  _tail ((Data*)0)
{
  TRACER( "Lst::Data::Data" );
} // Lst::Data::Data

template <class C>
inline 
Lst<C>::Data::Data (const C& t, const Lst& ts)
  :
  _counter (1),
  _head (t),
  _tail (ts)
{
  TRACER( "Lst::Data::Data" );
} // Lst::Data::Data


template <class C>
inline 
Lst<C>::Data::~Data ()
{
  TRACER( "Lst::Data::~Data" );

  ASS (_counter == 0);
} // Lst::Data::~Data ()


template <class C>
inline
const C& Lst<C>::Data::head () const 
{ 
  ASS (this);
  ASS (_counter > 0);

  return _head; 
} // Lst::Data::head


template <class C>
inline
const Lst<C>& Lst<C>::Data::tail () const 
{ 
  ASS (this);
  ASS (_counter > 0);

  return _tail;
} // Lst::Data::tail


template <class C>
inline
void Lst<C>::Data::ref () 
{ 
  ASS (this);

  _counter++;
} // Lst::Data::ref ()


template <class C>
inline
void Lst<C>::Data::deref () 
{ 
  ASS (this);
  ASS (_counter > 0);

  _counter--;
  if (_counter == 0) {
    delete this;
  }
} // Lst::Data::deref ()


*/

// **************** Lst::Iterator definitions *********************


/*
template <class C>
inline
Lst<C>::Iterator::Iterator (const Lst& lst)
  : 
  _list (lst) 
{
} // Lst::Iterator::Iterator


template <class C>
inline
void Lst<C>::Iterator::reset (const Lst& lst)
{
  _list = lst;
} // Lst<C>::Iterator::reset


template <class C>
inline
bool Lst<C>::Iterator::more () const 
{ 
  return _list.isNonEmpty(); 
} // Lst::Iterator::more ()


// Lst::Iterator::next
// 30/08/2002 Torrevieja, copied from Term
template <class C>
C Lst<C>::Iterator::next ()
{
  ASS( more() );

  C result (_list.head());
  _list.pop ();
  
  return result;
} // Lst::Iterator::next
*/


#endif // __Lst__

