//
// File:         MultisetOfVariables.hpp
// Description:  Small multisets of variables.
// Created:      Jan 25, 2001, 20:00
// Author:       Alexandre Riazanov
// mail:         riazanov@cs.man.ac.uk
// Note:         Until Vampire 1.18 this was a part of variables.h
//               Some day this module must be united with Multiset.h
//==================================================================
#ifndef MULTISET_OF_VARIABLES_H
//===================================================================
#define MULTISET_OF_VARIABLES_H
#ifndef NO_DEBUG_VIS
 #include <iostream>
#endif
#include "jargon.hpp"
#include "BlodKorvDebugFlags.hpp"
#include "Stack.hpp"
#include "Comparison.hpp"
//================================================
#undef DEBUG_NAMESPACE
#ifdef DEBUG_MULTISET_OF_VARIABLES
 #define DEBUG_NAMESPACE "MultisetOfVariables<MaxNumOfVariables>"
#endif
#include "debugMacros.hpp"
//=================================================

namespace BK 
{
template <ulong MaxNumOfVariables>
class MultisetOfVariables
 {
  private:
   Stack<ulong,MaxNumOfVariables> registered;
   ulong occurences[MaxNumOfVariables];
   ulong size;
   const ulong* iter;
  public:
   MultisetOfVariables() 
    : registered("MultisetOfVariables::registered"),
      size(0)
    {
      CALL("constructor MultisetOfVariables()");
     for (ulong v = 0; v < MaxNumOfVariables; v++) 
      occurences[v] = 0; 
    };
   ~MultisetOfVariables() 
   {
     CALL("destructor ~MultisetOfVariables()");
     BK_CORRUPT(occurences);
   };

   void init()
   {
      CALL("init()");
     registered.init("registered");
     size = 0UL;
     for (ulong v = 0; v < MaxNumOfVariables; v++) 
      occurences[v] = 0; 
   };

   void destroy() 
   {
     CALL("destroy()");
     BK_CORRUPT(occurences);
     registered.destroy(); 
   };

   MultisetOfVariables& operator=(MultisetOfVariables& vms)
    {
     Reset();
     registered = vms.registered;  
     vms.ResetIterator();
     const ulong* var = vms.NextVar();
     while (var)
      {
       occurences[*var] = vms.Occurences(*var);
       var = vms.NextVar();
      };
     size = vms.Size();
     return *this;
    };  
   
   void Reset()
    {
     while (!(registered.empty())) { occurences[registered.pop()] = 0; };
     size = 0;
    };
   void Register(ulong v)
    {
     if (!(occurences[v])) { registered.push(v); };
     occurences[v]++;
     size++;
    };
   void Register(ulong v,ulong new_occs)
    {
     if (!(occurences[v])) { registered.push(v); };
     occurences[v] += new_occs;
     size += new_occs;
    };
   
   void Unregister(ulong v) // v must be in the multiset!
    {
      // THIS FUNC SEEMS TO BE TOO EXOTIC! REVISE IT!
     occurences[v] = 0;
     ulong top_var = registered.pop();
     if (v != top_var) 
      {
       ulong* next_var = registered.begin();
       while ((*next_var) != v) next_var++;
       *next_var = top_var; 
      };      
    };
    
   const ulong& Occurences(ulong v) const { return occurences[v]; };
   const ulong& Size() const { return size; };
   ulong NumOfDifferent() const { return registered.size(); };
   operator bool() const { return registered.size(); };
   bool empty() const { return registered.empty(); };
   bool Belongs(ulong v) const { return (bool)(Occurences(v)); };
   
   void ResetIterator() 
   { 
    iter = registered.begin();
   };
   const ulong* NextVar() 
   { 
    CALL("NextVar()");
    if (iter < registered.end())
     {
      iter++;
      return iter - 1;
     };
    return 0;
   };

   void Add(MultisetOfVariables& vms)
    {
     CALL("Add(MultisetOfVariables& vms)");
     vms.ResetIterator();
     const ulong* var = vms.NextVar();
     while (var)
      {
       Register(*var,vms.Occurences(*var));
       var = vms.NextVar();
      };
    };
   
   void CollectDifference(MultisetOfVariables& vms1,MultisetOfVariables& vms2)
    {
     CALL("CollectDifference(MultisetOfVariables& vms1,MultisetOfVariables& vms2)");
     vms1.ResetIterator();
     ulong diff;
     for (const ulong* var = vms1.NextVar(); var; var = vms1.NextVar())
      {
       diff = vms1.Occurences(*var) - vms2.Occurences(*var);
       if (diff > 0) Register(*var,diff);
      }; 
    }; 


   bool IntersectionNonempty(MultisetOfVariables& vms) 
    {
     CALL("IntersectionNonempty(MultisetOfVariables& vms)");
     if ((!(Size())) || (!(vms.Size()))) { return false; };
     if (NumOfDifferent() <= vms.NumOfDifferent())
      {
       for (ulong* var = registered.begin(); var != registered.end(); var++)
        if (vms.Belongs(*var)) return true; 
      }
     else
      {
       for (ulong* var = vms.registered.begin(); var != vms.registered.end(); var++)
        if (Belongs(*var)) return true;        
      };
     return false;
    };
   
   bool equal(const MultisetOfVariables& vms)
    {
     CALL("equal(const MultisetOfVariables& vms)");  
     if ((Size() == vms.Size()) && (NumOfDifferent() == vms.NumOfDifferent()))
      {
       for (ulong* var = registered.begin(); var != registered.end(); var++) 
        if (Occurences(*var) != vms.Occurences(*var)) return false; 
       return true;
      }
     else { return false; };
    }; // bool equal(const MultisetOfVariables& vms)

   bool GreaterOrEqual(MultisetOfVariables& vms)
    {
     CALL("GreaterOrEqual(MultisetOfVariables& vms)");
     if (Size() == vms.Size())
      {
       if (NumOfDifferent() != vms.NumOfDifferent()) return false; 
       for (ulong* var = registered.begin(); var != registered.end(); var++) 
        if (Occurences(*var) != vms.Occurences(*var)) return false; 
       return true; // equal multisets
      }
     else
      {
       if (Size() > vms.Size())
        {
         if (NumOfDifferent() < vms.NumOfDifferent()) return false; 

         for (ulong* var = vms.registered.begin();var != vms.registered.end();var++)
          if (Occurences(*var) < vms.Occurences(*var)) return false;  
         return true; // greater
        }
       else // Size() < vms.Size()
        {
         return false; 
        };
      };     
    }; // bool GreaterOrEqual(MultisetOfVariables& vms)


   bool GreaterOrEqual(MultisetOfVariables& vms,bool& equal)
    {
     CALL("GreaterOrEqual(MultisetOfVariables& vms,bool& equal)");
     if (Size() == vms.Size())
      {
       if (NumOfDifferent() != vms.NumOfDifferent()) return false; 
       for (ulong* var = registered.begin(); var != registered.end(); var++) 
        if (Occurences(*var) != vms.Occurences(*var)) return false; 
       equal = true;
       return true; // equal multisets
      }
     else
      {
       if (Size() > vms.Size())
        {
         if (NumOfDifferent() < vms.NumOfDifferent()) return false; 

         for (ulong* var = vms.registered.begin();var != vms.registered.end();var++)
          if (Occurences(*var) < vms.Occurences(*var)) return false;  
         equal = false;
         return true; // greater
        }
       else // Size() < vms.Size()
        {
         return false; 
        };
      };     
    }; // bool GreaterOrEqual(MultisetOfVariables& vms,bool& equal)


   bool SubsetOf(MultisetOfVariables& vms) // not submultiset!
   {
    CALL("SubsetOf(MultisetOfVariables& vms)");
    if (NumOfDifferent() > vms.NumOfDifferent()) return false;
    for (ulong* var = registered.begin();var != registered.end();var++)
     if (!vms.Occurences(*var)) return false;
    return true;       
   }; // bool SubsetOf(MultisetOfVariables& vms) // not submultiset!     

   Comparison Compare(MultisetOfVariables& vms) 
    {
     CALL("Compare(MultisetOfVariables& vms)");
     if (Size() == vms.Size())
      {
       if (NumOfDifferent() != vms.NumOfDifferent()) return Incomparable; 

       for (ulong* var = registered.begin(); var != registered.end(); var++) 
        if (Occurences(*var) != vms.Occurences(*var)) return Incomparable; 
       return Equal;
      }
     else
      {
       if (Size() < vms.Size())
        {
         if (NumOfDifferent() > vms.NumOfDifferent()) return Incomparable; 
         for (ulong* var = registered.begin(); var != registered.end(); var++)
          if (Occurences(*var) > vms.Occurences(*var)) return Incomparable;
         return Less;
        }
       else
        {
         if (NumOfDifferent() < vms.NumOfDifferent()) return Incomparable; 
         for (ulong* var = vms.registered.begin(); var != vms.registered.end(); var++)
          if (Occurences(*var) < vms.Occurences(*var)) return Incomparable; 
         return Greater;
        };
      };
    };


  public: // for debugging
   #ifndef NO_DEBUG_VIS
    ostream& output(ostream& str)
    {
     if (!Size()) return str;   
     ResetIterator();
     const ulong* var = NextVar();
     str << Occurences(*var) << '*' << 'X' << *var;
     var = NextVar();
     while (var)
      {
       str << " + " << Occurences(*var) << '*' << 'X' << *var;
       var = NextVar();
      };
     return str;
    };
   #endif
 }; // class MultisetOfVariables
}; // namespace BK

#ifndef NO_DEBUG_VIS
namespace std
{
 template <ulong MaxNumOfVariables>
 inline ostream& operator<<(ostream& str,BK::MultisetOfVariables<MaxNumOfVariables>& vms) 
 {
  return vms.output(str);
 };
};
#endif    

//=================================================
#endif 
