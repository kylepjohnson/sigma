//
// File:         ObjDesc.hpp
// Description:  Structure for object descriptors for debugging memory management. 
// Created:      Mar 3, 2000
// Author:       Alexandre Riazanov
// mail:         riazanov@cs.man.ac.uk
//====================================================
#ifndef NO_DEBUG
#ifndef OBJ_DESC_H
//====================================================
#define OBJ_DESC_H
#include <iostream>                
#include <cstdlib> 
#include "jargon.hpp"
#include "BlodKorvDebugFlags.hpp"
#include "ClassDesc.hpp"
#include "Debug.hpp"
//====================================================
namespace BK 
{
struct ObjDesc 
{
 enum Status { Persistent = 12345678, Recycled = 87654321 };
 #ifdef DEBUG_ALLOC_OBJ_PTR
  void* objPtr;
 #endif
 #ifdef DEBUG_ALLOC_OBJ_SIZE
  size_t size;
 #endif   
 #ifdef DEBUG_ALLOC_OBJ_STATUS
  Status status;       
 #endif
 #ifdef DEBUG_ALLOC_OBJ_TYPE
  ClassDesc* classDesc;
 #endif
 #ifdef DEBUG_ALLOC_EVENT_ID
  ulong eventId;
 #endif
  ObjDesc() {};
  ~ObjDesc() {};
 void* operator new (size_t) 
 {
  void* res = ::malloc(sizeof(ObjDesc)); 
  if (!res) 
   Debug::debugFeatureInternalFault("!ObjDesc::operator new (size_t)::res");
  return res; 
 };
 void operator delete(void* obj,size_t) 
 {
  free(obj);
 }; 
  void operator delete(void* obj)
  {
    free(obj);
  };
}; // struct ObjDesc

}; // namespace BK


namespace std
{
inline 
ostream& operator<<(ostream& str,const BK::ObjDesc& od)
{
 #ifdef DEBUG_ALLOC_OBJ_PTR
  str << "[" << (unsigned long)od.objPtr << "] ";
 #endif
 #ifdef DEBUG_ALLOC_OBJ_SIZE
  str << "size(" << od.size << ") ";
 #endif   
 #ifdef DEBUG_ALLOC_OBJ_STATUS
  str << "status(";
  switch (od.status)
   {
    case BK::ObjDesc::Persistent: str << "Persistent"; break;
    case BK::ObjDesc::Recycled: str << "Recycled"; break;
    default: str << "Unknown(=" << (ulong)od.status << ")";
   };  
  #ifdef DEBUG_ALLOC_EVENT_ID
   str << ',' << od.eventId;  
  #endif
  str << ") ";
 #endif
 #ifdef DEBUG_ALLOC_OBJ_TYPE
  str << ":" << (*od.classDesc);
 #endif
 
 return str;
}; 
};

//====================================================
#endif
#endif

