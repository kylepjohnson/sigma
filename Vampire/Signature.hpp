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
//  file signature.h 
//  defines class Signature consisting of predicate and function symbols
//
//  28/04/2002, Manchester, made by merging PSymbol and FSymbol
//  01/05/2002, Manchester, implementation of equality changed, Iterator added
//

#ifndef __signature__
#define __signature__

#define NO_OF_BUCKETS 2039


// ***************** class Signature, definition ****************************


// class ostream;
class TermList;


#include "Int.hpp"
#include "List.hpp"


class Atom;

class Signature {
 
 public:

  class Symbol;
  typedef List <Symbol*,CID_PSYM_LIST> List;

  class Symbol 
#   if DEBUG_PREPRO
    : public Memory <CID_SYMBOL>
#   endif
  {
   public:
    // query structure
    int arity () const { return _arity; }
    const char* name () const { return _name;}
    const int number () const { return _number; }
    void arity ( int ar ) { _arity = ar; };
    void kernelNumber (unsigned long n) { _kernelNumber = n; }
    unsigned long kernelNumber () const { return _kernelNumber; }

   protected:
    Symbol (const char* name, int arity, int& number) :
      _name (name),
      _arity (arity),
      _number (number++)
      {}
    // structure
    const char* _name;
    int _arity;
    int _number;
    unsigned long _kernelNumber;
  }; // class Signature::Symbol

  class Fun :
    public Symbol
  {
   public:
    Fun (const char* name, int arity, int& count) :
      Symbol (name,arity,count)
      {}
    Compare compare (const Fun*) const; // comparison, needed to normalize
  };

  class Pred :
    public Symbol
  {
   public:
    Pred (const char* name, int arity, int& count) :
      Symbol (name,arity,count)
      {}
    bool isEquality () const
      { return number() == 0; }

    Compare compare (const Pred*) const; // comparison, needed to normalize
  };

  // iterator, iterate over all function or predicate symbols
  class Iterator {
   public:
    explicit Iterator (List** buckets);
    bool more ();
    Symbol* next ();
   private:
    List** _firstBucket;
    List** _currentBucket;
    List* _currentList;
  }; // Iterator

  // iterator over all function symbols
  class FunIterator 
    : public Iterator {
   public:
    explicit FunIterator (Signature* s) :
      Iterator (s->_fbuckets)
      {}
  };

  // iterator over all predicate symbols
  class PredIterator 
    : public Iterator {
   public:
    explicit PredIterator (Signature* s) :
      Iterator (s->_pbuckets)
      {}
  };

  Signature ();
  ~Signature ();

  Fun* createFun ( const char* name, int arity )
    { return static_cast<Fun*>(create (name, arity, _fbuckets, false)); }
  Pred* createPred ( const char* name, int arity )
    { return static_cast<Pred*>(create (name, arity, _pbuckets, true)); }

  // structure
  Pred* equality () { return _equality; }
  int noOfPreds () const { return _noOfPreds; }
  int noOfFuns () const { return _noOfFuns; }
  bool arityCheck () const; // true if every symbol has only one arity

  // miscellaneous
  Fun* newSkolemFunction (int arity);
  void createAnswerAtom (TermList args, Atom& answer); // create answer atom predicate
  void addArithmetic ();         // add functions and relations of arithmetic
  bool isAnswer (const Pred* p) const
    { return p == _answer; }

 private:  
  List * _fbuckets [NO_OF_BUCKETS];
  List * _pbuckets [NO_OF_BUCKETS];
  int _noOfPreds;
  int _noOfFuns;
  int _lastSkolem;
  Pred* _answer;
 
  static int hash ( const char* );  // hashing function
  Symbol* create ( const char* str, int arity, List** buckets, bool isPred );
  Pred* _equality;

  // miscellaneous
  bool existsFun ( const char* name ) const;

 friend class SymCounter;
 friend class FunIterator;
 friend class PredIterator;
}; // class Signature


ostream& operator << ( ostream& str, const Signature::Symbol* p );


// the global signature
extern Signature* sig;

#endif // __signature__
