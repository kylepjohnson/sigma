//
// File:         BitWord.hpp
// Description:  
// Created:      Sep 8, 1999, 16:35
// Author:       Alexandre Riazanov
// mail:         riazanov@cs.man.ac.uk
//
//=================================================================== 
#ifndef BIT_WORD_H
//====================================================================
#define BIT_WORD_H
#ifndef NO_DEBUG_VIS
 #include <iostream>
#endif
#include <climits>                                   // for CHAR_BIT
#include <cstddef>                                   // for size_t
#include "jargon.hpp"
#include "BlodKorvDebugFlags.hpp"
//=====================================================================
#undef DEBUG_NAMESPACE
#ifdef DEBUG_BIT_WORD
 #define DEBUG_NAMESPACE "BitWord"
#endif
#include "debugMacros.hpp"
//=====================================================================
namespace BK 
{

class BitWord
{
 public:
  class InitStatic
  {
   public:
    InitStatic() 
    {
     if (!_count)
      {
       BitWord::initStatic();
       _count++;
      };
    };
    ~InitStatic() {};
   private:
    static long _count;
  };
 public:
  BitWord() : base((ulong)0) {};
  BitWord(const BitWord& bw) : base(bw.base) {};
  BitWord(const ulong& b) : base(b) {};
  ~BitWord() {};
  void init() { base = (ulong)0; };
  void init(const BitWord& bw) { base = bw.base; };
  void init(const ulong& b) { base = b; };
  void destroy() {};
  BitWord& operator=(const BitWord& bw) { base = bw.base; return *this; };
  
  static size_t sizeInBits() { return sizeof(ulong)*CHAR_BIT; };
  bool bit(const size_t& n) const 
  {
   return ((base & singleBitMask[n]) != 0UL);
  }; 
  
  ulong piece(const size_t& begin,const size_t& end) const
  {
   CALL("subword(const size_t& begin,const size_t& end) const");
   ASSERT(begin <= end);
   return (base << begin) >> (sizeInBits() - end + begin - 1);
  };
    
  void setPiece(const size_t& begin,const size_t& end,ulong p) 
  {
   CALL("setPiece(const size_t& begin,const size_t& end,ulong p)");
   (base &= (inversePieceMask[begin][end])) |= (p << (sizeInBits() - end - 1)); 
   
   ASSERT(piece(begin,end) == p);
  };

  operator bool() const { return base != 0UL; };

  void set(const size_t& n) { base |= singleBitMask[n]; }; 
  void set() { base = baseValAllSet; };
  void clear(const size_t& n) { base &= allButOneMask[n]; }; 
  void clear() { base = baseValAllClear; };
  void flip(size_t n) { base ^= singleBitMask[n]; };
  void flip() { base ^= baseValAllSet; };

  BitWord& operator&=(const BitWord& bw) { base &= bw.base; return *this; };
  BitWord& operator^=(const BitWord& bw) { base ^= bw.base; return *this; }; // exclusive OR
  BitWord& operator|=(const BitWord& bw) { base |= bw.base; return *this; }; // inclusive OR

  BitWord& operator-=(const BitWord& bw) 
  { 
   BitWord intersection(*this);
   return (*this) ^= (intersection &= bw);
  };

  void compare(const BitWord& arg,
               BitWord& thisMinusArg,
               BitWord& intersection) const
  {
   (intersection = *this) &= arg;
   (thisMinusArg = *this) ^= intersection;
  };

  void compare(const BitWord& arg,
               BitWord& thisMinusArg,
               BitWord& intersection,
               BitWord& argMinusThis) const
  {
   (intersection = *this) &= arg;
   (thisMinusArg = *this) ^= intersection;
   (argMinusThis = arg) ^= intersection;  
  };

  bool operator==(const BitWord& bw) const { return base == bw.base; };
  bool operator!=(const BitWord& bw) const { return base != bw.base; };
    
  bool superset(const BitWord& bw) const 
  {
   BitWord result(bw);
   result -= *this;
   return !result.base;
  };

  bool properSuperset(const BitWord& bw) const
  {
   return (base != bw.base) && superset(bw);
  }; 

  bool subset(const BitWord& bw) const
  {
   BitWord result(*this);
   result -= bw;
   return !result.base; 
  };

  bool properSubset(const BitWord& bw) const
  {
   return bw.properSuperset(*this);
  };
 
  size_t count() const 
  {
   size_t res = 0;
   for (size_t b = 0; b < sizeof(ulong)*CHAR_BIT; b++) if (bit(b)) res++; 
   return res;
  };

  bool containsAtLeast(size_t n) const
  {
   for (size_t b = 0; b < sizeof(ulong)*CHAR_BIT; b++)
    if (bit(b)) { n--; if (!n) return true; };
   return false;
  };

  static BitWord singleBit(size_t n) { return BitWord(singleBitMask[n]); };  
  static BitWord allButOne(size_t n) { return BitWord(allButOneMask[n]); };

 private:
  static void initStatic();
  static ulong makeBaseValAllSet();
  static ulong makeBaseValAllClear(); 
  static ulong makeSingleBitMask(size_t);
  static ulong makeAllButOneMask(size_t);
  static ulong makePieceMask(size_t,size_t);
  static ulong makeInversePieceMask(size_t,size_t);
 private:
  ulong base; 
  static bool statInitialized;
  static ulong baseValAllSet;
  static ulong baseValAllClear;
  static ulong singleBitMask[sizeof(ulong)*CHAR_BIT];
  static ulong allButOneMask[sizeof(ulong)*CHAR_BIT];
  static ulong pieceMask[sizeof(ulong)*CHAR_BIT][sizeof(ulong)*CHAR_BIT];
  static ulong inversePieceMask[sizeof(ulong)*CHAR_BIT][sizeof(ulong)*CHAR_BIT];
 friend class InitStatic;
};

}; // namespace BK

#ifndef NO_DEBUG_VIS
namespace std
{
 inline ostream& operator<<(ostream& str,const BK::BitWord& wd)
 {
  for (size_t n = 0; n < BK::BitWord::sizeInBits(); n++)
   str <<  (wd.bit(n) ? '1' : '0');
  return str; 
 };
};
#endif

//=========================================================================

static BK::BitWord::InitStatic bitWordInitStatic;

//=========================================================================
#undef DEBUG_NAMESPACE
//==========================================================================
#endif
