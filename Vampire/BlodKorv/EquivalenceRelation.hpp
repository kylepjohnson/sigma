//
// File:         EquivalenceRelation.hpp
// Description:  Equivalence relations on small carriers. 
// Created:      ??? 
// Revised:      Jan 22, 2001, 20:30 (cosmetic changes) 
// Author:       Alexandre Riazanov
// mail:         riazanov@cs.man.ac.uk
//===============================================================
#ifndef EQUIVALENCE_RELATION_H
//================================================================
#define EQUIVALENCE_RELATION_H
#ifndef NO_DEBUG_VIS
 #include <iostream>
#endif
#include "jargon.hpp"
#include "BlodKorvDebugFlags.hpp"
//============================================================================
#undef DEBUG_NAMESPACE
#ifdef DEBUG_EQUIVALENCE_RELATION
 #define DEBUG_NAMESPACE "EquivalenceRelation<ulong SizeOfCarrier>"
#endif
#include "debugMacros.hpp" 
//============================================================================

namespace BK 
{

template <ulong SizeOfCarrier>
 class EquivalenceRelation
  {
   public:
    class Iterator // to enumerate a minimal frame of an equivalence relation
     {
      public:
       Iterator() {};
       Iterator(const EquivalenceRelation& er) 
        : next(er.registered), 
          end(er.next_registered),
          can_rep(er.can_rep) 
        {};
       void Reset(const EquivalenceRelation& er) 
        {
         next = er.registered;
         end = er.next_registered;
         can_rep = er.can_rep;
        };    
       bool Next(ulong& x,ulong& y)
        {
	 CALL("EquivalenceRelation<ulong SizeOfCarrier>::Iterator");
         if (next == end) return false;
         x = *next;
         y = can_rep[x];
         ASSERT(x > y);
         next++;
         return true;
        }; 
      private:
       const ulong* next;
       const ulong* end;
       const ulong* can_rep;
     }; // class Iterator 

    class EqClass // representation of an equivalence class
     {
      public:
       EqClass() {};
       EqClass(EquivalenceRelation& rel,const ulong& x) 
       { 
	 CALL("EqClass::constructor EqClass(EquivalenceRelation& rel,const ulong& x)");
	 ASSERT(rel.Normalized());
        _rel = &rel;
	_frame = rel.can_rep;
        _canRep = _frame[x];
        _next = rel.registered;
        _end = rel.next_registered;
       }; 

       void init(EquivalenceRelation& rel,const ulong& x)  
       {
	CALL("EqClass::init(EquivalenceRelation& rel,const ulong& x)");
        ASSERT(rel.Normalized());
        _rel = &rel;
	_frame = rel.can_rep;
        _canRep = _frame[x];
        _next = rel.registered;
        _end = rel.next_registered;
       };   

       void destroy() {};

       void reset() 
       {
        _next = _rel->registered;
        _end = _rel->next_registered;
       };     

       bool nextElement(ulong& x) 
       {
	if (_next > _end) return false;
	while (_next < _end)
	 {
	  x = *_next;
          _next++;
	  if (_frame[x] == _canRep) return true;
	 };  
        // _next == _end  
        x = _canRep;
        _next++;
        return true; 
       };            

      private:
       EquivalenceRelation* _rel;
       const ulong* _frame;
       ulong _canRep;
       const ulong* _next;
       const ulong* _end;
     }; // class EqClass

    class MergedPairs // pairs of items becoming equivalent after adding a pair
    {
     public:
      MergedPairs() {};
      MergedPairs(EquivalenceRelation& rel,const ulong& x,const ulong& y) { reset(rel,x,y); };
      void reset(EquivalenceRelation& rel,const ulong& x,const ulong& y)
      {
       CALL("MergedPairs::reset(EquivalenceRelation& rel,const ulong& x,const ulong& y)");
       ASSERT(!rel.Equivalent(x,y));
       rel.Normalize();
       _xClass.init(rel,x);
       _yClass.init(rel,y);
       ALWAYS(_xClass.nextElement(_currX)); 
      };
      bool nextPair(ulong& x,ulong& y)
      {
       CALL("nextPair(ulong& x,ulong& y)");
       if (_yClass.nextElement(y)) 
        {
	 if (_currX < y) { x = _currX; }
         else // result must be swapped
	  { 
	   x = y;
           y = _currX;
          };
         return true; 
        };
       if (_xClass.nextElement(_currX))  
	{
         _yClass.reset();
         ALWAYS(_yClass.nextElement(y));
         if (_currX < y) { x = _currX; }
         else // result must be swapped
	  { 
	   x = y;
           y = _currX;
          };
	 return true;
        };
       return false;
      }; 
     private:
      EqClass _xClass; 
      EqClass _yClass;
      ulong _currX;
    }; // class MergedPairs
    

   public: 
    EquivalenceRelation() : next_registered(registered), name("?")
     {
      for (ulong i = 0; i < SizeOfCarrier; i++) can_rep[i] = i;
     };
    
    EquivalenceRelation(const char* nm) : next_registered(registered), name(nm)
     {
      for (ulong i = 0; i < SizeOfCarrier; i++) can_rep[i] = i;
     }; 
    

    ~EquivalenceRelation() 
    { 
      CALLM(name,"destructor ~EquivalenceRelation()"); 
      BK_CORRUPT(*this);
    };

    void init()
    {
      CALLM("?","init()");
      next_registered = registered;
      name = "?";
      for (ulong i = 0; i < SizeOfCarrier; i++) can_rep[i] = i;
    };

    void init(const char* nm)
    {
      CALLM(nm,"init(const char* nm)");
      next_registered = registered;
      name = nm;
      for (ulong i = 0; i < SizeOfCarrier; i++) can_rep[i] = i;
    };
    
    void destroy() 
    {
      CALLM(name,"destroy()");
    };

    EquivalenceRelation& operator=(const EquivalenceRelation& er)
    {
     CALL("operator=(const EquivalenceRelation& er)");
     // This operation is used frequently.   
     Reset();     
     for (const ulong* x = er.registered; x < er.next_registered; x++)
      {
       // MakeEquivalent(*x,er.can_rep[*x]);
       ASSERT(*x > er.can_rep[*x]);
       can_rep[*x] = er.can_rep[*x];
       *next_registered = *x; 
       next_registered++;
      };
     return *this;
    };  

    void Reset() 
     {
      CALLM(name,"Reset()");
      while (next_registered != registered) 
       {
        next_registered--;
        ASSERT(inCarrier(*next_registered));
        can_rep[*next_registered] = *next_registered;
       };
     };
    bool Identity() const { return next_registered == registered; };
    bool OnePair() const { return (next_registered - registered) == 1; }; 
    ulong FrameSize() const { return next_registered - registered; };
    ulong CanRep(ulong x) const 
     {
      CALLM(name,"CanRep(ulong x) const");

      while (can_rep[x] != x) 
       {
        x = can_rep[x]; 
       };
      return x;
     };
    bool Equivalent(const ulong& x,const ulong& y) const
     {
      CALLM(name,"Equivalent(ulong x,ulong y) const");
      ASSERT(inCarrier(x));
      ASSERT(inCarrier(y));
      return CanRep(x) == CanRep(y);
     };
    void MakeEquivalent(ulong x,ulong y)
     {
      // Very frequently used
      CALLM(name,"MakeEquivalent(ulong x,ulong y)");
      ASSERT(inCarrier(x));
      ASSERT(inCarrier(y));
      ulong can_x = CanRep(x);
      ulong can_y = CanRep(y);
      if (can_x < can_y) { Register(can_y,can_x); }
      else 
       { 
        if (can_x > can_y) Register(can_x,can_y);  
        // otherwise nothing to do because can_x == can_y and x and y are already equivalent
       }; 
     };
    bool Split(ulong& x,ulong& y)
     { 
      CALLM(name,"Split(ulong& x,ulong& y)");
      if (next_registered == registered) return false; // identity relation
      next_registered--;
      x = *next_registered;
      y = can_rep[x];
      ASSERT(y == can_rep[y]);
      ASSERT(x > y);
      ASSERT(inCarrier(x));      
      can_rep[x] = x;
      return true;
     };
    
    void Subtract(EquivalenceRelation& eqRel) // modifies eqRel!
     {
      CALLM(name,"Subtract(EquivalenceRelation& eqRel)");
      Normalize();
      DOP(static EquivalenceRelation debugER1);
      DOP(static EquivalenceRelation debugER2);
      DOP(static EquivalenceRelation debugER3);
      DOP(static EquivalenceRelation debugER4);
      DOP(debugER1 = eqRel);
      DOP(debugER2 = *this);
      

      ulong x;
      ulong can_x;
	  ulong* i;
      for (i = next_registered - 1 ; i>= registered; i--)
       {
        x = *i;
        can_x = can_rep[x];
        if (eqRel.Equivalent(x,can_x)) { Unregister(x); }
        else
         {
          eqRel.MakeEquivalent(x,can_x);
         };
       }; 


      DOP(debugER3.MakeComposition(*this,debugER1));
      DOP(debugER4.MakeComposition(debugER2,debugER1));       
   
      ASSERT(debugER3.Implies(debugER4));
      ASSERT(debugER4.Implies(debugER3));
      #ifdef DEBUG_NAMESPACE
       for (i = registered; i < next_registered; i++) 
        {
	 ASSERT(!debugER1.Equivalent(*i,can_rep[*i]));  
         debugER1.MakeEquivalent(*i,can_rep[*i]);
        };   
      #endif
     
     }; // void Subtract(EquivalenceRelation& EquivalenceRelation)

 
    void MakeComposition(const EquivalenceRelation& er1,const EquivalenceRelation& er2)
    {
     CALL("MakeComposition(const EquivalenceRelation& er1,const EquivalenceRelation& er2)");
     Reset();
     const ulong* x;
     for (x = er1.registered; x < er1.next_registered; x++)
      MakeEquivalent(*x,er1.can_rep[*x]);
     for (x = er2.registered; x < er2.next_registered; x++)
      MakeEquivalent(*x,er2.can_rep[*x]);
    };

    void MakeComplement(EquivalenceRelation& er1,const EquivalenceRelation& er2)
    {
     // everything from er1 that is not in er2
     CALL("MakeComplement(const EquivalenceRelation& er1,const EquivalenceRelation& er2)");
     Reset();    
     er1.Normalize(); // the key idea 
     ulong x,can_x;
     for (const ulong* xp = er1.registered; xp < er1.next_registered; xp++)
      {
       x = *xp;
       can_x = er1.can_rep[x];
       if (!er2.Equivalent(x,can_x))
	{
	 ASSERT(Normalized());
         const ulong* yp;
	 for (yp = registered; 
              ((yp < next_registered) 
                && ((can_x != can_rep[*yp])
                    || (!er2.Equivalent(x,*yp))));
              yp++)
	  {
	    // empty body
          };
         if (yp == next_registered) MakeEquivalent(x,can_x);          
        };    
      };
     DOP(static EquivalenceRelation debugComp1("debugComp1"));
     DOP(static EquivalenceRelation debugComp2("debugComp2"));
     DOP(debugComp1.MakeComposition(er1,er2));
     DOP(debugComp2.MakeComposition(*this,er2));
     ASSERT(debugComp1.Implies(debugComp2));
    }; // void MakeComplement(EquivalenceRelation& er1,const EquivalenceRelation& er2)
    
    bool Implies(const EquivalenceRelation& er) const 
    {
     for (const ulong* x = registered; x < next_registered; x++)
       if (!er.Equivalent(*x,can_rep[*x])) return false;
     return true;      
    };
 
    void Normalize()
     {
      // Quite frequently called, it makes sense to optimise it.  
      CALL("Normalize()");
      // Optimises the representation 
      CALLM(name,"Normalize()");  
      for (ulong* i = next_registered - 1; i >= registered; i--)
       {
        can_rep[*i] = CanRep(*i);
        ASSERT(inCarrier(can_rep[*i]));
       }; 
     };   


    #ifndef NO_DEBUG
     bool Normalized() const
     {
      for (ulong* i = next_registered - 1; i >= registered; i--)       
       if (can_rep[can_rep[*i]] != can_rep[*i]) return false;
      return true;
     };
    #endif

   public: // for debugging
    #ifndef NO_DEBUG_VIS
     ostream& output(ostream& str) const
     {
      str << name << " : ";
      for (const ulong* i = registered; i < next_registered; i++)
       {
        str << *i << " -> " << can_rep[*i];
        if (i + 1 < next_registered) str << " | ";
       };
      return str;
     }; 
    #endif   

    #ifdef DEBUG_NAMESPACE
     static bool inCarrier(ulong x) { return (x < SizeOfCarrier); }
    #endif

 
   private:
    void Register(ulong x,ulong can_x) 
     {
      CALLM(name,"Register(ulong x,ulong can_x)");
      ASSERT(x > can_x);
      ASSERT(x == can_rep[x]);
      ASSERT(inCarrier(x));
      ASSERT(inCarrier(can_x));
      *next_registered = x;
      next_registered++;
      can_rep[x] = can_x; 
     }; // void Register(ulong x,ulong can_x)  
    void Unregister(ulong x)
     {
      CALLM(name,"Unregister(ulong x)");
      ASSERT(inCarrier(x));
      ASSERT(inCarrier(can_rep[x])); 
      can_rep[x] = x;
      next_registered--;
      if ((*next_registered) != x)
       {
        for (ulong* i = registered; i < next_registered; i++)
         if ((*i) == x)
          {
           *i = *next_registered;
           return;
          };
       };
     }; // void Unregister(ulong x)

   private:
    ulong can_rep[SizeOfCarrier];
    ulong registered[SizeOfCarrier];
    ulong* next_registered;
    const char* name;
   friend class Iterator;
   friend class EqClass;
  }; // class EquivalenceRelation<ulong SizeOfCarrier>

}; // namespace BK

#ifndef NO_DEBUG_VIS
namespace std
{
 template <ulong SizeOfCarrier>
 ostream& operator<<(ostream& str,const BK::EquivalenceRelation<SizeOfCarrier>& er)
 {
  return er.output(str);
 };
}; // namespace std
#endif



//============================================================================
#undef DEBUG_NAMESPACE 
//=======================================================
#endif
