//
// File:         BitSet.hpp
// Description:  Finite sets represented by bit strings.
// Created:      Nov 7, 1999, 19:50
// Author:       Alexandre Riazanov
// mail:         riazanov@cs.man.ac.uk
// Revised:      Dec 22, 2001.
//               1) Bug fix in  void* operator new(size_t,size_t s)
//=======================================================================
#ifndef BIT_SET_H
//=======================================================================
#define BIT_SET_H
#ifndef NO_DEBUG_VIS
 #include <iostream>
#endif
#include <cstddef>                                   // for size_t
#include "jargon.hpp"
#include "BlodKorvDebugFlags.hpp"
#include "BitWord.hpp" 
#include "BitString.hpp"
//=======================================================================
#undef DEBUG_NAMESPACE
#ifdef DEBUG_BIT_SET 
 #define DEBUG_NAMESPACE "BitSet<class Allocator>"
#endif
#include "debugMacros.hpp"
//=======================================================================
namespace BK 
{

template <class Allocator>
class BitSet : private BitString<Allocator>
{
 public: 
  typedef BitString<Allocator> BitStr;
 public:
  BitSet() : BitString<Allocator>(0) // empty set
  {
   CALL("constructor BitSet()");
  }; 
  ~BitSet() {};
  void* operator new(size_t) 
  {
   CALL("operator new(size_t)"); 
   return new(0) BitString<Allocator>(0); 
  }; // for empty sets only
  void operator delete(void* obj) 
  {
   CALL("operator delete(void* obj)"); 
   delete(static_cast< BitString<Allocator>* >(obj)); 
  };

  void operator delete(void* obj,size_t) 
  {
   CALL("operator delete(void* obj,size_t)"); 
   delete(static_cast< BitString<Allocator>* >(obj)); 
  };
  bool contains(size_t el) const
  {
   CALL("contains(size_t el) const");
   size_t wn = el/(BitWord::sizeInBits());  
   if (wn >= size()) return false;
   return word(wn).bit(el%(BitWord::sizeInBits()));
  };    

  size_t upperBound() const { return (size())*(BitWord::sizeInBits()); };
  ulong count() const
  {
   CALL("count() const");
   ulong res = 0;
   for (size_t wn = 0; wn < size(); wn++) res += word(wn).count();
   return res;
  };  
 
  bool containsAtLeast(size_t n) const
  {
   CALL("containsAtLeast(size_t n) const");
   const BitWord* wd = begin(); 
   for (size_t wn = 0; wn < size(); wn++)
    {
     n -= wd[wn].count();
     if (n <= 0) return true; 
    };
   return false;
  };
  
  BitSet* copy() const
  {
   CALL("copy() const");
   size_t wn = size();
   BitSet* res = new (wn) BitSet(wn);
   const BitWord* wd = begin();
   BitWord* res_wd = res->begin();
   while (wn) { wn--; res_wd[wn] = wd[wn]; }; 
   return res;  
  };

  bool write(size_t el)
  {
   CALL("write(size_t el)");
   size_t wn = el/(BitWord::sizeInBits());  
   if (wn < size()) 
    { 
     word(wn).set(el%(BitWord::sizeInBits())); 
     return true;
    };
   return false;
  };
  
  BitSet* add(size_t el) const
  {
   CALL("add(size_t el) const");
   size_t wn = el/(BitWord::sizeInBits());
   size_t sz = size();
   size_t new_sz = (wn >= sz) ? (wn + 1) : sz; 
   BitSet* res = new (new_sz) BitSet(new_sz);
   const BitWord* wd = begin();
   BitWord* res_wd = res->begin();
   size_t w; 
   for (w = 0; w < sz; w++) res_wd[w] = wd[w];
   for (w = sz; w < new_sz; w++) res_wd[w].clear();
   res_wd[wn].set(el%(BitWord::sizeInBits()));
   return res;
  };

  bool erase(size_t el)
  {
   CALL("erase(size_t el)");
   size_t wn = el/(BitWord::sizeInBits());  
   if (wn >= size()) return false;
   if (wn + 1 < size())
    {
     word(wn).clear(el%(BitWord::sizeInBits()));
     return true;
    }
   else // wn + 1 == size()
    {
     BitWord wd = word(wn);
     wd.clear(el%(BitWord::sizeInBits()));
     if (wd) { word(wn) = wd; return true; };
     return false; 
    };
  };

  BitSet* remove(size_t el) const
  {
   CALL("remove(size_t el) const");
   size_t wn = el/(BitWord::sizeInBits());
   size_t sz = size();
   if (wn + 1 == sz)
    {
     BitWord last_wd(word(wn));
     last_wd.clear(el%(BitWord::sizeInBits()));
     if (last_wd) goto same_length;
     while (wn)
      {
       wn--;
       last_wd = word(wn);
       if (last_wd) 
	{ 
         BitSet* res = new (wn + 1) BitSet(wn + 1);
         const BitWord* wd = begin();
         BitWord* res_wd = res->begin();         
         res_wd[wn] = last_wd;
         while (wn) 
	  {
           wn--;
           res_wd[wn] = wd[wn];
          };
         return res;
        };
      };
     return new BitSet();
    }
   else
    if (wn >= sz) { return copy(); }
    else // wn + 1 < sz
     {
      same_length: 
       BitSet* res = copy();
       res->word(wn).clear(el%(BitWord::sizeInBits()));
       return res;
     };    
  }; // BitSet* remove(size_t el) const

  bool nonempty() const { return size() != 0UL; }; 
  bool empty() const { return !size(); };
 
  bool subset(const BitSet& bs) const
  {
   CALL("subset(const BitSet& bs) const");
   const size_t str_sz = size();
   if (str_sz > bs.size()) return false;
   const BitWord* bw = begin();
   const BitWord* bs_bw = bs.begin();
   for (size_t wn = 0; wn < str_sz; wn++) if (!(bw[wn].subset(bs_bw[wn]))) return false;      
   return true;
  }; // bool subset(const BitSet& bs) const
    
  BitSet* intersection(const BitSet& bs) const
  {
   CALL("intersection(const BitSet& bs) const");
   size_t wn = (size() > bs.size()) ? bs.size() : size();
   BitWord bw_int;
   const BitWord* bw = begin();
   const BitWord* bs_bw = bs.begin();
   while (wn) 
    {
     wn--;
     if ((bw_int = bw[wn]) &= bs_bw[wn])
      {
       BitSet* res = new (wn + 1) BitSet(wn + 1);
       BitWord* res_bw = res->begin();
       res_bw[wn] = bw_int;
       while (wn) 
	{
         wn--;
         (res_bw[wn] = bw[wn]) &= bs_bw[wn];
        };
       return res;
      };
    };
   return new BitSet(); // empty set
  }; // BitSet* intersection(const BitSet& bs) const

  BitSet* difference(const BitSet& bs) const  
  {
   CALL("difference(const BitSet& bs) const");
   size_t wn = size();
   BitWord bw_diff;
   const BitWord* bw = begin();
   const BitWord* bs_bw = bs.begin();
   while (wn)
    {
     wn--;
     if ((bw_diff = bw[wn]) -= bs_bw[wn]) 
      {
       BitSet* res = new (wn + 1) BitSet(wn + 1);
       BitWord* res_bw = res->begin();
       res_bw[wn] = bw_diff;
       while (wn) 
	{
         wn--;
         (res_bw[wn] = bw[wn]) -= bs_bw[wn];
        };
       return res;
      };    
    };
   return new BitSet(); // empty set 
  }; // BitSet* difference(const BitSet& bs) const 

  BitSet* sum(const BitSet& bs) const  
  {
   CALL("sum(const BitSet& bs) const");
   size_t sz = size();
   size_t bs_sz = bs.size();
   BitSet* res;
   const BitWord* wd = begin(); 
   const BitWord* bs_wd = bs.begin(); 
   BitWord* res_wd;  

   if (sz > bs_sz)
    {
     res = new (sz) BitSet(sz);
     res_wd = res->begin();
	 size_t wn;
     for (wn = 0; wn < bs_sz; wn++) (res_wd[wn] = wd[wn]) |= bs_wd[wn];
     for (wn = bs_sz; wn < sz; wn++) res_wd[wn] = wd[wn];
    }  
   else // sz <= bs_sz
    {
     res = new (bs_sz) BitSet(bs_sz);
     res_wd = res->begin();   
     size_t wn;
     for (wn = 0; wn < sz; wn++) (res_wd[wn] = wd[wn]) |= bs_wd[wn];
     for (wn = sz; wn < bs_sz; wn++) res_wd[wn] = bs_wd[wn];
    };
   return res;
  };
    

 private:
  BitSet(size_t s) : BitString<Allocator>(s) {};
  void* operator new(size_t,size_t s) 
  {
   CALL("operator new(size_t,size_t s)");  
   return new(s) BitString<Allocator>(s);
  };  
};// template <class Allocator> class BitSet : private BitString 

}; // namespace BK

#ifndef NO_DEBUG_VIS
namespace std
{
 template <class Allocator>
 ostream& operator<<(ostream& str,const BK::BitSet<Allocator>* bs)
 {
  CALL("operator<<(ostream& str,const BK::BitSet<Allocator>* bs)");
  str << '{';
  ulong ct = bs->count();
  for (size_t el = 0; el < bs->upperBound(); el++)
   if (bs->contains(el))
    {
     str << el;
     ct--;
     if (ct) str << ',';
    };
  return str << '}'; 
 };
};
#endif

//======================================================================
#undef DEBUG_NAMESPACE
//=======================================================================
#endif
