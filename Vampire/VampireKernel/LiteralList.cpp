//
// File:         LiteralList.cpp
// Description:  Representation of lists of stored marked literals.
// Created:      Dec 09, 2000, 16:30
// Author:       Alexandre Riazanov
// mail:         riazanov@cs.man.ac.uk
//====================================================  
#include "LiteralList.hpp"
//==================================================== 
using namespace BK;
using namespace VK;

#ifdef DEBUG_ALLOC_OBJ_TYPE
 ClassDesc LiteralList::_classDesc("LiteralList",sizeof(LiteralList),sizeof(LiteralList));
 ClassDesc LiteralList::Element::_classDesc("LiteralList::Element",sizeof(LiteralList::Element),sizeof(LiteralList::Element));
#endif

ostream& LiteralList::output(ostream& str) const
{
 CALL("output(ostream& str) const");
 ConstIterator iter(this);
 ulong sel = numOfSelectedLits();
 for (ulong i = 0; i < sel; i++)
  {
   ASSERT(iter.notEnd());
   str << iter.currentLiteral();
   iter.next();       
   if (i + 1 < sel) str <<  " \\/ ";
  };  
  
 if (iter.notEnd()) str << " | ";
 while (iter.notEnd()) 
  { 
   str << iter.currentLiteral();
   iter.next();          
   if (iter.notEnd()) str << " \\/ ";
  };
 return str;
}; // ostream& LiteralList::output(ostream& str) const



//====================================================  

