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

#include <iostream>


#include "Memory.hpp"


#define mem_static(CID) Memory<CID>* Memory<CID>::_freeList = 0; Memory<CID>* Memory<CID>::_allocated = 0; int Memory<CID>::_allocs = 0; int Memory<CID>::_deallocs = 0; Memory<CID>::LeakChecker dummy_##CID;

mem_static (CID_VTERM);
mem_static (CID_CTERM);
mem_static (CID_NTERM);
mem_static (CID_ATOM);
mem_static (CID_QFORMULA);
mem_static (CID_VARLIST);
mem_static (CID_INTLIST);
mem_static (CID_VARLIST_LIST);
mem_static (CID_ASSOC_PAIR_LIST);
mem_static (CID_SYMBOL);
mem_static (CID_FSYM_LIST);
mem_static (CID_PSYM_LIST);
mem_static (CID_TERM_LIST);
mem_static (CID_ATOM_LIST);
mem_static (CID_LITERAL);
mem_static (CID_LITERAL_LIST);
mem_static (CID_KERNEL_CLAUSE_LIST);
mem_static (CID_CLAUSE_LIST);
mem_static (CID_FORMULA_LIST);
mem_static (CID_ENTRY);
mem_static (CID_ENTRY_LIST);
mem_static (CID_UNIT);
mem_static (CID_CLAUSE);
mem_static (CID_UNIT_LIST);
mem_static (CID_UNIT_LINK);
mem_static (CID_BINDING);
mem_static (CID_BINDING_LIST);
mem_static (CID_REN_BINDING);
mem_static (CID_REN_BINDING_LIST);
mem_static (CID_OPTION_LIST);
mem_static (CID_ULONG_LIST);
mem_static (CID_FORALL_OR_MINISCOPE);
mem_static (CID_DUMMY_QUANTIFIER_REMOVAL);
mem_static (CID_FLATTEN);
mem_static (CID_INFERENCE_TYPE1);


const char* id2name (ClassID cid)
{
  switch (cid) 
    {
    case CID_VTERM:
      return "Variable Term";
    case CID_CTERM:
      return "Compound Term";
    case CID_NTERM:
      return "Compound Term";
    case CID_ATOM:
      return "Atom";
    case CID_QFORMULA:
      return "Quantified Formula";
    case CID_VARLIST:
      return "Variable list";
    case CID_INTLIST:
      return "Integer list";
    case CID_VARLIST_LIST:
      return "Variable list list";
    case CID_ASSOC_PAIR_LIST:
      return "Assoc pair list";
    case CID_SYMBOL:
      return "Symbol";
    case CID_FSYM_LIST:
      return "Signature::Fun list";
    case CID_PSYM_LIST:
      return "Signature::Pred list";
    case CID_TERM_LIST:
      return "Term list";
    case CID_ATOM_LIST:
      return "Atom list";
    case CID_LITERAL:
      return "Literal";
    case CID_LITERAL_LIST:
      return "Literal list";
    case CID_KERNEL_CLAUSE_LIST:
      return "Kernel clause list";
    case CID_CLAUSE_LIST:
      return "Clause list";
    case CID_FORMULA_LIST:
      return "Formula CList";
    case CID_ENTRY:
      return "Entry";
    case CID_ENTRY_LIST:
      return "Entry list";
    case CID_UNIT:
      return "Unit";
    case CID_CLAUSE:
      return "Clause";
    case CID_UNIT_LIST:
      return "Unit list";
    case CID_UNIT_LINK:
      return "Unit link";
    case CID_BINDING:
      return "Substitution::Binding";
    case CID_BINDING_LIST:
      return "Substitution::Binding list";
    case CID_REN_BINDING:
      return "Renaming::Binding";
    case CID_REN_BINDING_LIST:
      return "Renaming::Binding list";
    case CID_OPTION_LIST:
      return "Option list";
    case CID_ULONG_LIST:
      return "ulong list";
    case CID_FORALL_OR_MINISCOPE:
      return "ForallOrMiniscope";
    case CID_DUMMY_QUANTIFIER_REMOVAL:
      return "DummyQuantifierRemoval";
    case CID_FLATTEN:
      return "Flatten";
    case CID_INFERENCE_TYPE1:
      return "InferenceType1";
    default:
      return "Unidentified class";
  }
} // id2name

