//
// File:         ExpandingMultiset.hpp
// Description:  Multisets over expanding intervals of nonnegative numbers.
// Created:      Dec 20, 2001
// Author:       Alexandre Riazanov
// mail:         riazanov@cs.man.ac.uk
// Revised:      Dec 23, 2001.
//               1) Two versions of addSafe(..) and safeScoreOf(..) added. 
//               2) Initialisation of scores by 0L 
//                  is now done automatically using
//                  the functionality of Array<..>. 
//===============================================================================
#ifndef EXPANDING_MULTISET_H
//===============================================================================
#define EXPANDING_MULTISET_H
#ifndef NO_DEBUG_VIS
 #include <iostream>
#endif
#include "jargon.hpp"
#include "BlodKorvDebugFlags.hpp"
#include "ExpandingStack.hpp"
#include "Array.hpp"
//============================================================================
#undef DEBUG_NAMESPACE
#ifdef DEBUG_EXPANDING_MULTISET
 #define DEBUG_NAMESPACE "ExpandingMultiset<class Alloc,class CarrierType,ulong MinIncrementSize,class InstanceId>"
#endif
#include "debugMacros.hpp"
//=================================================================================
namespace BK 
{

template <class Alloc,class CarrierType,ulong MinIncrementSize,class InstanceId>
class ExpandingMultiset
{
 public:
  ExpandingMultiset() :
    #ifndef DEBUG_NAMESPACE
    _score(0L),
   #else
    _score(0L,"ExpandingMultiset<..>::_score"), 
   #endif
    _registered(DOP("ExpandingMultiset<..>::_registered"))
   #ifdef DEBUG_NAMESPACE
    , _name("?")
   #endif
  {
   CALL("constructor ExpandingMultiset()");
  };

  ExpandingMultiset(const char* name)  :
    #ifndef DEBUG_NAMESPACE
    _score(0L),
   #else
    _score(0L,"ExpandingMultiset<..>::_score"), 
   #endif
    _registered(DOP("ExpandingMultiset<..>::_registered"))
   #ifdef DEBUG_NAMESPACE
    , _name(name)
   #endif
  {
   CALL("constructor ExpandingMultiset(const char* name)");
  };
  
  ~ExpandingMultiset() { CALL("destructor ~ExpandingMultiset()"); };

  void init() 
  {
   CALLM("?","init()");
   
   #ifndef DEBUG_NAMESPACE
    _score.init(0L);
   #else
    _score.init(0L,"ExpandingMultiset<..>::_score"); 
   #endif
   _registered.init(DOP("ExpandingMultiset<..>::_registered")); 
   #ifdef DEBUG_NAMESPACE
    _name = "?";
   #endif
  };  

  void init(const char* name) 
  {
   CALLM(name,"init(const char* name)");
   #ifndef DEBUG_NAMESPACE
    _score.init(0L);
   #else
    _score.init(0L,"ExpandingMultiset<..>::_score"); 
   #endif 
   _registered.init(DOP("ExpandingMultiset<..>::_registered")); 
   #ifdef DEBUG_NAMESPACE
    _name = name;
   #endif
  };

  void destroy() 
  {
    CALLM(_name,"destroy()");
    _registered.destroy();
    _score.destroy();
  };

  void expand(const CarrierType& i)
  {
   CALLM(_name,"expand(const CarrierType& i)");
   _score.expand(i);
   _registered.expand(i);
  };
  
  void reset() 
  {
   CALLM(_name,"reset()");
   CarrierType el;
   while (_registered.nonempty())
    {
     el = _registered.pop();
     _score[el] = 0L;
    };
  };
  long scoreOf(const CarrierType& el) const  // unsafe but efficient
  {
   CALLM(_name,"scoreOf(const CarrierType& el) const");  
   return _score[el]; 
  };
  long safeScoreOf(const CarrierType& el) const  // safe but inefficient
  {
   CALLM(_name,"safeScoreOf(const CarrierType& el) const");
   if (el >= _score.size()) return false;  
   return _score[el]; 
  };

  void add(const CarrierType& el) // unsafe but efficient
  {
   CALLM(_name,"add(const CarrierType& el)");
   if (!(_score[el])) _registered.push(el); 
   _score[el]++;
  };
  void add(const CarrierType& el,ulong num) // unsafe but efficient
  {
   CALLM(_name,"add(const CarrierType& el,ulong num)");
   if (!(_score[el])) _registered.push(el); 
   _score[el] += num;
  };

  void addSafe(const CarrierType& el) // safe but inefficient
  {
   CALLM(_name,"addSafe(const CarrierType& el)");
   if (!(_score.sub(el))) _registered.pushSafe(el); 
   _score[el]++;
  };
  void addSafe(const CarrierType& el,ulong num) // safe but inefficient
  {
   CALLM(_name,"addSafe(const CarrierType& el,ulong num)");
   if (!(_score.sub(el))) _registered.pushSafe(el); 
   _score[el] += num;
  };

  ulong numOfDiffElements() const 
  { 
   return _registered.size(); 
  };
  const CarrierType& nth(ulong n) const  // unsafe but efficient
  {
   CALLM(_name,"nth(ulong n) const");
   return _registered.nth(n); 
  };
  #ifndef NO_DEBUG_VIS
   ostream& output(ostream& str) const
   {
    CALLM(_name,"output(ostream& str) const");
    str << '{';
    for (const CarrierType* el = _registered.begin(); el < _registered.end(); el++)
     {
      str << scoreOf(*el) << '*' << *el;
      if (el != _registered.last()) str << ' ';
     };  
    return str << '}';
   };
  #endif

  #ifndef NO_DEBUG
   void freeze() { _score.freeze(); _registered.freeze(); };
   void unfreeze() { _score.unfreeze(); _registered.unfreeze(); };
  #endif  

 private:
  Array<Alloc,ulong,MinIncrementSize,ExpandingMultiset<Alloc,CarrierType,MinIncrementSize,InstanceId> > _score;
  ExpandingStack<Alloc,CarrierType,MinIncrementSize,ExpandingMultiset<Alloc,CarrierType,MinIncrementSize,InstanceId> > _registered; 
  #ifdef DEBUG_NAMESPACE
   const char* _name;
  #endif
}; // template  <Alloc,CarrierType,MinIncrementSize,InstanceId> class ExpandingMultiset  


}; // namespace BK

//====================================================================
#endif
