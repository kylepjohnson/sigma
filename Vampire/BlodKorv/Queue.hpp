//
// File:         Queue.hpp
// Description:  Queues of restricted capacity.
// Created:      Dec 24, 1999, 13:00
// Author:       Alexandre Riazanov
// mail:         riazanov@cs.man.ac.uk
//====================================================================
#ifndef QUEUE_H
//=====================================================================
#define QUEUE_H
#include "jargon.hpp"
//====================================================================
namespace BK 
{
template <class ElemType,unsigned long maxSize>
class Queue
{
 public:
  Queue()
  {
   reset();
  };
  ~Queue() 
  {
    CALL("destructor ~Queue()");
    BK_CORRUPT(hd);
    BK_CORRUPT(tl);
  };
  void init()
  {
    reset();
  };

  void destroy() 
  {
    CALL("destroy()");
    BK_CORRUPT(hd);
    BK_CORRUPT(tl);
  };

  void reset()
  {
   hd = 0;
   tl = 0;
  };
  unsigned long size() const { return tl - hd; };
  bool empty() const { return !(size()); };
  bool nonempty() const { return size(); };
  bool full() const { return size() >= maxSize; };
  ElemType& tail() { return data[tl % maxSize]; }; 
  void enqueue() { tl++; }; 
  const ElemType& head() const { return data[hd % maxSize]; };
  void dequeue() 
   { 
    hd++; 
    if ((hd >= maxSize) && (tl >= maxSize)) 
     {
      hd -= maxSize;
      tl -= maxSize;
     };  
   };    
 private: 
  ElemType data[maxSize];
  ulong hd;
  ulong tl;  
}; // template <class ElemType,unsigned long maxSize> class Queue 

}; // namespace BK


//=====================================================================
#endif


