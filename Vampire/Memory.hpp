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
//  Memory handling for small and other structures
//  For small structures the memory handling is optimized
//
//  In the debug mode 
//    (1) can keep track of the amount of local and global memory allocated
//    (2) provides functions to check validity of a small memory piece
//    (3) halts on the deallocated of memory that was not allocated or
//        is deallocated
//    (4) can print all allocated but not deallocated structures (memory leak)
//
//  20/11/1999 Manchester, made from memory.h in kk
//  14/02/2000 Manchester, reimplemented completely
//  27/02/2002 Manchester, name () changed to classID ()
//


//
//  Allocated objects of the class must be of the same length
//  and not allocated before the memory initialiser is called
// 

#ifndef __memory__
#  define __memory__


#include <cstdlib>

#include "assert.hpp"


// it is essential that the class IDs are enumerated in order, each one is 
// unique, and CID_TOTAL_NUMBER gives their number.
//
enum ClassID {
  CID_VTERM,
  CID_CTERM,
  CID_NTERM,
  CID_ATOM,
  CID_QFORMULA,
  CID_VARLIST,
  CID_INTLIST,
  CID_VARLIST_LIST,
  CID_ASSOC_PAIR_LIST,
  CID_SYMBOL,
  CID_FSYM_LIST,
  CID_PSYM_LIST,
  CID_TERM_LIST,
  CID_ATOM_LIST,
  CID_LITERAL,
  CID_LITERAL_LIST,
  CID_CLAUSE,
  CID_CLAUSE_LIST,
  CID_FORMULA_LIST,
  CID_ENTRY,
  CID_ENTRY_LIST,
  CID_UNIT,
  CID_UNIT_LIST,
  CID_UNIT_LINK,
  CID_NAME_TABLE,
  CID_BINDING,
  CID_BINDING_LIST,
  CID_REN_BINDING,
  CID_REN_BINDING_LIST,
  CID_OPTION_LIST,
  CID_ULONG_LIST,
  CID_KERNEL_CLAUSE_LIST,
  CID_FORALL_OR_MINISCOPE,
  CID_DUMMY_QUANTIFIER_REMOVAL,
  CID_FLATTEN,
  CID_INFERENCE_TYPE1
}; // enum ClassIDs


// could not make it in-class since it must be global for all memories
extern const char* id2name (ClassID);


template <ClassID CID>
class Memory 
{
 public:
  void* operator new (size_t size)
  {
    _allocs ++;

    if (_freeList) {
      Memory* _result = _freeList;
      _freeList = _freeList->_next;
      return _result;
    }

    if (void* res = ::operator new (size)) {
      return res;
    }
    NO_MEMORY;
  } // Memory::operator new


  void operator delete (void* obj)
  {
    _deallocs ++;

    Memory* m = reinterpret_cast <Memory*>(obj);
    m->_next = _freeList;
    _freeList = m;
  }

  bool isValid () const
  {
    return (_id == CID) && (_allocationTag == ALLOCATED);
  }

  Memory () 
    :
    // mark the object as allocated
    _allocationTag (ALLOCATED),
    _id (CID)
  {
    if ( _allocated ) { // there was an allocated oject
      _next = _allocated;
      _previous = _allocated->_previous;
      if ( _allocated->_previous != 0 ) {
        _allocated->_previous->_next = this;
      }
      _allocated->_previous  = this;
    }
    else { // there was no allocated object
      _next = 0;
      _previous = 0;
      _allocated = this;
    }
  }

  ~Memory ()
  {
    if ( _allocationTag == DEALLOCATED ) {
      cerr << "Attempt to delete an already deleted object " << (void*)this << " of class " <<
              id2name (CID) << "\n";
      exit (EXIT_FAILURE);
    }
    if ( ! isValid () ) {
      cerr << "Trying to delete an object of class " <<
              id2name (CID) << ", but accessing an invalid memory piece\n";
      exit (EXIT_FAILURE);
    }

    // mark the object as de-allocated
    _allocationTag = DEALLOCATED;

    if (_next) {
      _next->_previous = _previous;
    }
    if (_previous) {
      _previous->_next = _next;
    }
    if ( _allocated == this ) {
      _allocated = _next ? _next : _previous;
    }
    // free-list will be changed by operator delete
  }

  class LeakChecker {
   public:
    LeakChecker () {}
    ~LeakChecker ()
    {
      if (Memory::_allocated) {
        int leaks = 0;
        for (Memory* n = Memory::_allocated; n; n = n->_next) {
          leaks ++;
        }
        for (Memory* p = Memory::_allocated->_previous; p; p = p->_previous) {
          leaks ++;
        }
        cout << "Class " << id2name (CID) << "\n"
                "  allocations: " << Memory::_allocs << "\n"
                "  deallocations: " << Memory::_deallocs << "\n";
        cout << "Class " << id2name (CID) << " has memory leaks (" <<
                leaks << " pieces)\n";
      }
    }
  };

  friend class LeakChecker;

 private:
  enum AllocationTag {
    ALLOCATED = 1234732,    // any two garbage values
    DEALLOCATED = 2374321
  };

  // structure
  Memory* _next;      // next in the free list, or next among allocated
  Memory* _previous;  // previous among allocated
  AllocationTag _allocationTag;
  const enum ClassID _id;      // must always be classID

  static Memory* _freeList;
  static Memory* _allocated;    // any allocated object, initially 0
  static int _allocs;
  static int _deallocs;
}; // class Memory


#endif
