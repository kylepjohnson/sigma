//
// File:         BitWord.cpp
// Description:  
// Created:      Sep 8, 1999, 18:30
// Author:       Alexandre Riazanov
// mail:         riazanov@cs.man.ac.uk
//========================================================================
#include "BitWord.hpp"
//========================================================================
#ifdef DEBUG_BIT_WORD
 #define DEBUG_NAMESPACE "BitWord"
#else
 #undef DEBUG_NAMESPACE
#endif
#include "debugMacros.hpp"
//========================================================================
using namespace BK;

long BitWord::InitStatic::_count; // Implicitely initialised to 0L
ulong BitWord::baseValAllSet;
ulong BitWord::baseValAllClear;
ulong BitWord::singleBitMask[sizeof(ulong)*CHAR_BIT];
ulong BitWord::allButOneMask[sizeof(ulong)*CHAR_BIT];
ulong BitWord::pieceMask[sizeof(ulong)*CHAR_BIT][sizeof(ulong)*CHAR_BIT];
ulong BitWord::inversePieceMask[sizeof(ulong)*CHAR_BIT][sizeof(ulong)*CHAR_BIT];

void BitWord::initStatic()
{
 baseValAllSet = makeBaseValAllSet(); 
 baseValAllClear = 0ul;
 for (size_t n = 0; n < sizeof(ulong)*CHAR_BIT; n++)
  {
   singleBitMask[n] = makeSingleBitMask(n); 
   allButOneMask[n] = makeAllButOneMask(n);
  };
 for (size_t begin = 0; begin < sizeof(ulong)*CHAR_BIT; begin++)
  for (size_t end = begin; end < sizeof(ulong)*CHAR_BIT; end++)
   {
    pieceMask[begin][end] = makePieceMask(begin,end);
    inversePieceMask[begin][end] = makeInversePieceMask(begin,end);
   };
}; // void BitWord::initStatic()


ulong BitWord::makeBaseValAllSet()
{
 ulong x;
 ulong y = 1ul;
 while (y) { x = y; y = y << 1; };
 // now only the leftmost bit of x is set
 y = x;
 for (size_t n = 1; n < sizeof(ulong)*CHAR_BIT; n++) { x = x >> 1; y |= x; };
 return y;
};

ulong BitWord::makeSingleBitMask(size_t n)
{
 ulong x;
 ulong y = 1ul;
 while (y) { x = y; y = y << 1; };
 // now only the leftmost bit of x is set
 while (n) { x = x >> 1; n--; };
 return x;
};

ulong BitWord::makeAllButOneMask(size_t n)
{
 ulong result = makeSingleBitMask(n);
 result ^= makeBaseValAllSet();
 return result;
};

ulong BitWord::makePieceMask(size_t begin,size_t end)
{
 return ((makeBaseValAllSet() << begin) >> (sizeInBits() - end + begin - 1)) << (sizeInBits() - end - 1);
}; 

ulong BitWord::makeInversePieceMask(size_t begin,size_t end)
{
 return (makePieceMask(begin,end) ^ makeBaseValAllSet());
}; 


//==========================================================================


