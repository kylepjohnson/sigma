//
// File:         Multiset.hpp
// Description:  Multisets over a finite carrier.
// Created:      Sep 29, 1999, 21:20
// Author:       Alexandre Riazanov
// mail:         riazanov@cs.man.ac.uk
//===============================================================================
#ifndef MULTISET_H
//===============================================================================
#define MULTISET_H
#ifndef NO_DEBUG_VIS
 #include <iostream>
#endif
#include "jargon.hpp"
#include "BlodKorvDebugFlags.hpp"
#include "Stack.hpp"
#include "Comparison.hpp"
//============================================================================
#undef DEBUG_NAMESPACE
#ifdef DEBUG_MULTISET
 #define DEBUG_NAMESPACE "Multiset<class CarrierType,ulong Size>"
#endif
#include "debugMacros.hpp"
//=================================================================================

namespace BK 
{
template <class CarrierType,ulong Size>
class Multiset
{
 public:
  Multiset() 
   : registered("Multiset::registered") 
  {
    CALL("constructor Multiset()");
   for (ulong i = 0; i< Size; i++) score[i] = 0;  
  };
  ~Multiset() 
  { 
    CALL("destructor ~Multiset()");
    BK_CORRUPT(score);
  };

  void init()
  {
    CALL("init()");
    registered.init("Multiset::registered");
    for (ulong i = 0; i< Size; i++) score[i] = 0;
  };
  void destroy()
  {
    CALL("destroy()");
    BK_CORRUPT(score);
    registered.destroy();
  };

  void reset() 
  {
   CarrierType el;
   while (registered.nonempty())
    {
     el = registered.pop();
     score[el] = 0;
    };
  };
  long scoreOf(const CarrierType& el) const { return score[el]; };
  void add(const CarrierType& el)
  {
   if (!(score[el])) registered.push(el); 
   score[el]++;
  };
  void add(const CarrierType& el,ulong num)
  {
   if (!(score[el])) registered.push(el); 
   score[el] += num;
  };
  ulong numOfDiffElements() const { return registered.size(); };
  const CarrierType& nth(ulong n) const { return registered.nth(n); };
  #ifndef NO_DEBUG_VIS
   ostream& output(ostream& str) const
   {
    str << '{';
    for (const CarrierType* el = registered.begin(); el < registered.end(); el++)
     {
      str << scoreOf(*el) << '*' << *el;
      if (el != registered.last()) str << ' ';
     };  
    return str << '}';
   };
  #endif
 private:
  ulong score[Size];
  Stack<CarrierType,Size> registered; 
}; // template <class Carrier,ulong MaxElement> class Multiset  


}; // namespace BK

//============================================================================
#undef DEBUG_NAMESPACE
#ifdef DEBUG_MULTISET
 #define DEBUG_NAMESPACE "MultisetWithRemoval<class CarrierType,ulong Size>"
#endif
#include "debugMacros.hpp"
//=================================================================================

namespace BK 
{


template <class CarrierType,ulong Size>
class MultisetWithRemoval
{
 public:
  MultisetWithRemoval() 
   : registered("MultisetWithRemoval::registered") 
  {
    CALL("constructor MultisetWithRemoval()");
   for (ulong i = 0; i < Size; i++) score[i] = 0UL;  
  };
  ~MultisetWithRemoval() { CALL("destructor ~MultisetWithRemoval()"); };

  void init()
  {
    CALL("init()");
    registered.init("MultisetWithRemoval::registered");
    for (ulong i = 0; i < Size; i++) score[i] = 0UL;  
  };
  
  void destroy() 
  {
    CALL("destroy()");
    registered.destroy();
  };

  void reset() 
  {
    CALL("reset()");
   CarrierType el;
   while (registered.nonempty())
    {
     el = registered.pop();
     score[el] = 0UL;
    };
   ASSERT(checkIntegrity());
  };
  long scoreOf(const CarrierType& el) const { return score[el]; };
  void add(const CarrierType& el)
  {
   if (!(score[el])) 
    { 
     positionInRegistered[el] = registered.end();
     registered.push(el);
    }; 
   score[el]++;
  };
  void removeAll(const CarrierType& el)
  {
   CALL("removeAll(const CarrierType& el)");
   if (score[el])
    {
     ASSERT(registered.nonempty());
     CarrierType topEl = registered.pop();
     positionInRegistered[topEl] = positionInRegistered[el];
     *(positionInRegistered[el]) = topEl;
     score[el] = 0UL;
    };
  };

  ulong numOfDiffElements() const { return registered.size(); };
  const CarrierType& nth(ulong n) const { return registered.nth(n); };
  #ifndef NO_DEBUG
   bool checkIntegrity()
   {
    if (registered.empty())
     {
      for (ulong i = 0; i < Size; i++) if (score[i]) return false;
     };
    return true;
   };

  #endif
  #ifndef NO_DEBUG_VIS
   ostream& output(ostream& str) const
   {
    str << '{';
    for (const CarrierType* el = registered.begin(); el < registered.end(); el++)
     {
      str << scoreOf(*el) << '*' << *el;
      if (el != registered.last()) str << ' ';
     };  
    return str << '}';
   };
  #endif
 private:
  ulong score[Size];
  Stack<CarrierType,Size> registered; 
  CarrierType* positionInRegistered[Size];
}; // template <class Carrier,ulong MaxElement> class MultisetWithRemoval  



}; // namespace BK





//============================================================================
#undef DEBUG_NAMESPACE
#ifdef DEBUG_MULTISET
 #define DEBUG_NAMESPACE "MultisetWithMinMax<class CarrierType,ulong Size>"
#endif
#include "debugMacros.hpp"
//=================================================================================
namespace BK 
{

template <class CarrierType,ulong Size>
class MultisetWithMinMax
{
 public:
  MultisetWithMinMax() 
   : registered("MultisetWithMinMax::registered") 
  {
    CALL("constructor MultisetWithMinMax()");
   for (ulong i = 0; i< Size; i++) score[i] = 0;  
  };
  ~MultisetWithMinMax() { CALL("destructor ~MultisetWithMinMax()"); }; 
  void reset() 
  {
    CALL("reset()");
   CarrierType el;
   while (registered.nonempty())
    {
     el = registered.pop();
     score[el] = 0;
    };
  };
  bool empty() const { return registered.empty(); };
  long scoreOf(const CarrierType& el) const { return score[el]; };
  void add(const CarrierType& el)
  {
   if (registered.empty()) 
    {
     _minRegistered = el;
     _maxRegistered = el;
    }
   else
    {
     if (el < _minRegistered) 
      {
       _minRegistered = el;
      }
     else
      if  (el > _maxRegistered)   
       {
        _maxRegistered = el;
       };
    };
   if (!(score[el])) registered.push(el); 
   score[el]++;      
  };
  ulong numOfDiffElements() const { return registered.size(); };
  const CarrierType& nth(ulong n) const { return registered.nth(n); };
  Comparison compareByInverselyInducedOrder(const MultisetWithMinMax& ms)
  {
   CALL("compareByInverselyInducedOrder(const MultisetWithMinMax& ms)");
   if (empty())
    { 
     return (ms.empty()) ? Equal : Less;
    }
   else 
    if (ms.empty()) return Greater;
   
   // both nonempty
   if (_minRegistered == ms._minRegistered) 
    {
     CarrierType max = (_maxRegistered > ms._maxRegistered) ? _maxRegistered : ms._maxRegistered;   
     for (CarrierType el = _minRegistered; el <= max; el++)
      if (scoreOf(el) > ms.scoreOf(el)) { return Greater; }
      else 
       if (scoreOf(el) < ms.scoreOf(el)) return Less;
     return Equal; 
    }
   else 
    return (_minRegistered < ms._minRegistered)? Greater : Less;
  };
  #ifndef NO_DEBUG_VIS
   ostream& output(ostream& str) const
   {
    str << '{';
    for (const CarrierType* el = registered.begin(); el < registered.end(); el++)
     {
      str << scoreOf(*el) << '*' << *el;
      if (el != registered.last()) str << ' ';
     };  
    str << '}';
    if (!empty()) str << "min = " << _minRegistered << " & max = " << _maxRegistered;
    return str;
   };
  #endif
 private:
  ulong score[Size];
  Stack<CarrierType,Size> registered; 
  CarrierType _minRegistered;
  CarrierType _maxRegistered;
}; // template <class Carrier,ulong MaxElement> class MultisetWithMinMax  


}; // namespace BK

//==================================================================== 
#undef DEBUG_NAMESPACE
//====================================================================
#endif
