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
//  file Var.hpp
//  
//  05/09/2002, Bolzano
//


#ifndef __Var__
#  define __Var__

#include "Lst.hpp"


// variables
typedef int Var;
#define firstVar 0


class Substitution;


class VarList :
  public Lst<Var>
{
 public:
  // functions inherited from Lst
  const VarList& tail () const
    { return static_cast<const VarList&>(Lst<Var>::tail()); }
  VarList (Var head, const VarList& tail);
  VarList (const VarList& lst);
  explicit VarList (Var v); // one-element list
  VarList ();

  // miscellaneous
  void rectify (VarList, Substitution&, Var& last);
  Var max () const; // maximal variable in a non-empty list
};


class VarListList :
  public Lst<VarList>
{
 public:
  // functions inherited from Lst
  const VarListList& tail () const
    { return static_cast<const VarListList&>(Lst<VarList>::tail()); }
  VarListList (VarList head, const VarListList& tail);
  VarListList (const VarListList& lst);
  explicit VarListList (const VarList& t); // one-element list
  VarListList ();
};


// **************** class VarList, definitions ***********************

inline
VarList::VarList () 
{
}


inline
VarList::VarList (const VarList& lst) 
  :
  Lst<Var> (lst) 
{
}


inline
VarList::VarList (Var v) 
  :
  Lst<Var> (v) 
{
}


inline
VarList::VarList (Var head, const VarList& tail) 
  :
  Lst<Var> (head,tail) 
{
}


// **************** class VarListList, definitions ***********************


inline
VarListList::VarListList () 
{
}


inline
VarListList::VarListList (const VarListList& lst) 
  :
  Lst<VarList> (lst) 
{
}


inline
VarListList::VarListList (VarList head, const VarListList& tail) 
  :
  Lst<VarList> (head,tail) 
{
}


#endif // __Var__
