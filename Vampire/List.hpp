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

// *************************************************************
//
//  Class List<C> for implementing lists
//  990505 Manchester
//  990529 Manchester: setHead, setTail and conc added
//  990806 moved from KK to Sat
//  990925 Uppsala: lots of modifications
//  991017     Manchester: moved back to kk
//  26/02/2000 Manchester, inserted into library
//  14/05/2000 Dallas, function length added
//  15/05/2000 Dallas, function member added
//  29/05/2000 Manchester, isNonEmpty added
//  01/06/2000 Manchester, second added
//  04/06/2000 Manchester, remove added
//  04/06/2000 Manchester, nth() added
//  10/12/2000 Manchester, class Iterator added
//  17/12/2000 Manchester, Iterator::reset added
//  02/01/2001 Manchester, addLast added
//  07/01/2001 Manchester, pop changed to return a value
//  29/03/2001 Manchester, two small bugs fixed
//  09/05/2001 Manchester, DelIterator introduced
//  05/05/2001 Manchester, deleteNth ()
//  30/05/2001 Manchester, split
//  30/05/2001 Manchester, append
//  04/06/2001 Manchester, DelIterator::restore
//
// *************************************************************


#ifndef __list__
#  define __list__


#include "assert.hpp"
#include "Memory.hpp"


// *******************Class List*******************************

template <class C,ClassID ID>
class List
#   if DEBUG_PREPRO
    : public Memory <ID>
#   endif
{
 public:

  // constructors
  inline
  List ( C head )
    :
    _head ( head ),
    _tail ( 0 )
  {
  } // List::List

  inline
  List ( C head, List* tail )
    :
    _head ( head ),
    _tail ( tail )
  {
  } // List::List

  inline
  List () {}

  // create empty list
  inline
  static List* empty ()  
  {
    return 0;
  }

  inline
  bool isEmpty () const
  {
    return this == 0;
  } // List::isEmpty

  inline
  bool isNonEmpty () const
  {
    return this != 0;
  } // List::isNonEmpty

  inline
  List* tail () const
  {
    ASS ( this != 0 );

    return _tail;
  } // List::tail

  inline
  C head () const
  {
    ASS ( this != 0 );

    return _head;
  } // List::head

  // second element of the list
  inline
  C second () const
  {
    ASS ( this != 0 && _tail != 0 );

    return _tail->_head;
  } // List::second

  void head ( C head )
  {
    ASS ( this != 0 );

    _head = head;
  } // List::head


  inline
  void tail ( List* tail )
  {
    ASS ( this != 0 );

    _tail = tail;
  } // list::tail

  // destruction, copying
  inline
  void destroy ()
  {
    if ( this->isEmpty ()  )
      return;

    List* current = this;

    for (;;) {
      List* next = current->tail ();
      delete current;
      if ( next->isEmpty () )
        return;
      current = next;
    }
  } // List::destroy

  List* copy () const
  {
    if ( isEmpty () )
      return empty ();

    List* result = new List;
    result->head ( head () );
    List* previous = result;
    List* rest = tail ();

    while ( ! rest->isEmpty () ) {
      List* tmp = new List;
      tmp->head ( rest->head () );
      previous->tail ( tmp );
      previous = tmp;
      rest = rest->tail ();
    }

    previous->tail ( empty () );

    return result;
  }  // List::copy

  List* append (List* snd) 
  {
    if ( isEmpty () )
      return snd;

    List* result = new List;
    result->head ( head () );
    List* previous = result;
    List* rest = tail ();

    while ( ! rest->isEmpty () ) {
      List* tmp = new List;
      tmp->head ( rest->head () );
      previous->tail ( tmp );
      previous = tmp;
      rest = rest->tail ();
    }

    previous->tail ( snd );

    return result;
  }  // List::append

  inline
  List* cons ( C elem )
  {
    return new List ( elem, this );
  } // List::cons

  inline
  static void push ( C elem, List* &lst )
  {
    lst = lst->cons ( elem );
  } // List::push

  inline
  static C pop ( List* &lst )
  {
    ASS ( lst != 0 );

    List* tail = lst->tail ();
    C result = lst->head ();
    delete lst;
    lst = tail;

    return result;
  } // List::pop

  // moves the top from from to to
  inline 
  static void move ( List*& from, List*& to )
  {
    List* tail = from->tail ();
    from->tail ( to );
    to = from;
    from = tail;
  } // List::move

  // list concatenation
  static List* concat( List* first, List* second )
  {
    if ( first == 0 )
      return second;

    // an optimization, not really necessary
    if ( second == 0 )
      return first;

    List* current = first;
    for (;;) {
      List* next = current->tail ();
      if ( ! next ) {
        current->tail ( second );
        return first;
      }
      current = next;
      next = next->tail ();
    }
  } // List::concat

  List* reverse ()
  {
    if ( isEmpty () )
      return empty ();

    List* result = empty ();
    List* lst = this;

    while ( ! lst->isEmpty () ) {
      List* tl = lst->tail ();
      lst->tail ( result );
      result = lst;
      lst = tl;
    }

    return result;
  } // List::reverse

  // length of the list
  int length () const
  {
    int len = 0;

    for ( const List* lst = this; ! lst->isEmpty () ; lst = lst->tail() )
      len ++;

    return len;
  } // List::length

  // C is a member of the list
  bool member ( C elem )
  {
    for ( List* lst = this; ! lst->isEmpty () ; lst = lst->tail() ) {
      if ( lst->head() == elem )
        return true;
    }

    return false;
  } // List::member

  // removes first occurrence of elem from the list and deletes it
  List* remove ( C elem )         
  {
    if ( isEmpty() )
      return this;

    if ( head() == elem ) {
      List* result = tail ();
      delete this;
      return result;
    }

    if ( tail()->isEmpty() )
      return this;

    List* current = this;
    List* next = tail ();

    for (;;) {
      if ( next->head() == elem ) { // element found
        current->tail ( next->tail() );
        delete next;
        return this;
      }
      current = next;
      next = next->tail();
      if ( next->isEmpty() )
        return this;
    }
  } // List::remove
  
  // Nth element, counting from 0
  C nth ( int N ) const 
  {     
    // Nth element, counting from 0
    ASS ( N >= 0 );

    const List* l = this;

    while ( N != 0 ) {
      ASS ( l->isNonEmpty() );

      l = l->tail ();
      N-- ;
    }

    return l->head();
  } // nth

  // delete nth element and return it as the result
  static C deleteNth ( List*& lst, int N ) 
  {     
    // Nth element, counting from 0
    ASS ( N >= 0 );

    C result;
    List* l = lst;
    ASS ( lst->isNonEmpty() );

    if ( N == 0 ) {
      result = l->head();
      lst = l->tail();
      delete l;
      return result;
    }

    // N != 0
    List* next = l->tail();

    while ( --N != 0 ) {
      l = next;
      next = next->tail ();
      ASS ( next->isNonEmpty() );
    }
    //  now next must be deleted
    result = next->head();
    l->tail (next->tail());
    delete next;

    return result;
  } // deleteNth

  // add last element
  List* addLast ( C elem )
  {
    if ( ! this ) 
      return new List (elem);

    // nonempty, trying to find the end
    List* current;
    for ( current = this; current->_tail; current = current->_tail )
      ;

    current->tail ( new List (elem) );

    return this;
  } // List::addLast

  // split into two sublists, first of the length n
  List* split ( int n, List*& rest )
  {
    if ( ! this ) {
      ASS( n == 0 );
      rest = empty ();
      return empty ();
    }

    if ( n == 0 ) {
      rest = empty ();
      return this;
    }

    List* nth = this;
    while ( --n > 0 ) {
      ASS( nth );
      nth = nth->_tail;
    }
    
    ASS( nth );
    rest = nth->_tail;
    nth->_tail = empty ();
    return this;
  } // List::split

  class Iterator {
   public:
    
    inline explicit 
    Iterator ( List* l ) : _lst (l) {}  // due to WC++ compiler I could not move the definition outside !
    inline explicit 
    Iterator ( const List* l ) : _lst (const_cast<List*>(l)) {}  
    
    inline 
    C next () 
      { 
        C result = _lst->head(); 
        _lst = _lst->tail();
        return result;
      }
    
    inline 
    bool more () const { return ! _lst->isEmpty(); }

    inline
    void reset ( List* l ) { _lst = l; }

   private:
    List* _lst;
  };

  class DelIterator {
   public:
    
    inline 
    DelIterator ( List*& l ) : 
      _lst ( l ),
      #if DEBUG_PREPRO
        _prev ( 0 ), // required to check the double deletion ASS
      #endif
      _cur ( 0 )
      {}  
    
    inline 
    C next () 
      { 
        if ( _cur ) { // there was an element previously returned by next()
          _prev = _cur;
          _cur = _cur->tail();
          ASS( _cur );
        }
        else { // result must be the first element of the list
          _cur = _lst;
        }
        return _cur->head();
      }
    
    inline 
    bool more () 
      { 
        if ( _cur ) { // there was an element previously returned by next()
          return _cur->tail() != 0;
        }
        return ! _lst->isEmpty(); 
      }

    inline
    void del () 
      {
        // we can only delete the element returned by next
        ASS( _cur ); 
        // check that two delete requests in row did not occur
        ASS( _cur != _prev ); 

        if ( _cur == _lst ) { // the first element must be deleted
          _lst = _lst->tail ();
          delete _cur;
          _cur = 0;
          return;
        }

        // not the first element 
        _prev->tail (_cur->tail());
        delete _cur;
        _cur = _prev;
      }

    // 04/06/2001
    void restore ( C elem ) 
      {
        if ( _cur ) { // not the first element was deleted
          // we use that _cur and _prev point to the same element
          _cur = new List (elem,_cur->tail());
          _prev->tail (_cur);
          return;
        }
        // the first element was deleted
        push (elem, _lst);
        // _cur == 0, making the state as if just one more and next were called
        _cur = _lst;
      }

   private:
    List*& _lst;
    List* _prev;
    List* _cur; // _cur is the element returned by the last next, if 0, then no next was called
  };

 protected:  // structure

  C _head;
  List* _tail;
};  // class List


// ******************* Class List, definitions ****************************


#endif


