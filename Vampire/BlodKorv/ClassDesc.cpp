//
// File:         ClassDesc.cpp
// Description:  Structure for class descriptors used in dynamic type control.
// Created:      Mar 2, 2000, 17:10
// Author:       Alexandre Riazanov
// mail:         riazanov@cs.man.ac.uk
//====================================================

#ifndef NO_DEBUG
//====================================================
#include "ClassDesc.hpp"
//====================================================
using namespace std;
using namespace BK;

ClassDesc::List* ClassDesc::_registered; // implicitely initialised to 0
ClassDesc ClassDesc::_universal("UniversalClass");
//====================================================
ostream& ClassDesc::outputAll(ostream& str,bool objectStatistics)
{
 str << "************* Registered classes: ***************\n";
 for (const List* cd = registeredClasses(); cd; cd = cd->next())
  {   
   str << cd->hd()->name() << ": min.size = " << cd->hd()->minSize() << ", max.size = " << cd->hd()->maxSize() << "\n";
   if (objectStatistics)
    {
     for (const SizeAllocDeallocList* obj =  cd->hd()->registeredObjects(); obj; obj = obj->next())
      {
       str << "     objects of the size " << obj->size() 
           << ": persistent " << obj->numOfAllocated() - obj->numOfDeallocated()
           << " = allocated " << obj->numOfAllocated()
           << " - deallocated " << obj->numOfDeallocated()
           << "\n"; 
      };
    };
  };
 str << "************* End of registered classes. ********\n"; 
 return str;
}; // ostream& ClassDesc::outputAll(ostream& str,bool objectStatistics)

//====================================================
#endif
