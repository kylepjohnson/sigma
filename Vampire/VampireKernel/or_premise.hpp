#ifndef OR_PREMISE_H
//=================================================================
#define OR_PREMISE_H
#ifndef NO_DEBUG_VIS 
#  include <iostream>
#endif
#include "jargon.hpp"
#include "VampireKernelDebugFlags.hpp"
#include "Clause.hpp"
#include "prefix.hpp" 
#include "or_index.hpp"
#include "NewClause.hpp"
#include "Array.hpp"
#include "GlobAlloc.hpp"
//================================================
#undef DEBUG_NAMESPACE
#ifdef DEBUG_OR_PREMISE
 #define DEBUG_NAMESPACE "OR_PREMISE"
#endif
#include "debugMacros.hpp"
//=================================================
namespace VK
{

class OR_PREMISE
 {
  public: // methods
   OR_PREMISE(); 
   OR_PREMISE(NewClause* res); 
   ~OR_PREMISE();
   void init();
   void init(NewClause* res); 
   void destroy();


   void setResolvent(NewClause* res) { _resolvent = res; };

   PrefixSym* nthMaxLit(ulong n) 
     {
       CALL("nthMaxLit(ulong n)");
       ASSERT(n <= last_max);
       return _maxLiteral[n]; 
     };
   Clause* TheClause() const { return clause; };

   ulong LastMax() { return last_max; };
   ulong Index() { return index; };
   ulong CutLit() { return cut_lit; };

   void Load(ulong ind,Clause* c);
   void LoadOptimized(ulong ind,Clause* c);

   void Unload() 
   {
   };

   void Cut(ulong lit) 
    {
     CALL("Cut(ulong lit)");
     cut_lit = lit;
     _litIsMarked[cut_lit] = true;
    };
   void Load(ulong ind,RES_PAIR_LIST* res_pair)
    {
     CALL("Load(ulong ind,RES_PAIR_LIST* res_pair)");
     Load(ind,res_pair->TheClause());
     Cut((ulong)(res_pair->LitNum()));
    };
   void LoadOptimized(ulong ind,RES_PAIR_LIST* res_pair)
    {
     CALL("LoadOptimized(ulong ind,RES_PAIR_LIST* res_pair)");
     LoadOptimized(ind,res_pair->TheClause());
     Cut((ulong)(res_pair->LitNum()));
    };
   void Cut(TERM* lit) { Cut(clause->NumberOfLiteral(lit)); };
   void UnmarkCut() 
   { 
    CALL("UnmarkCut()");
    _litIsMarked[cut_lit] = false; 
   };
   bool CollectNonmarkedLiterals();
   bool NonmarkedExist() { return nonmarkedExist; };
  public: // output for debugging
   #ifndef NO_DEBUG_VIS  
    ostream& output(ostream& str) const;
   #endif

  protected: // structure
   NewClause* _resolvent;
   Clause* clause;
   ulong index;
   
   BK::Array<BK::GlobAlloc,PrefixSym,256UL,OR_PREMISE> _termMemory;
   PrefixSym* _freeTermMemory;
   
   BK::Array<BK::GlobAlloc,PrefixSym*,32UL,OR_PREMISE> _maxLiteral;

   BK::Array<BK::GlobAlloc,bool,32UL,OR_PREMISE> _litIsInherentlySelected;

   ulong last_max;

   BK::Array<BK::GlobAlloc,bool,32UL,OR_PREMISE> _litIsMarked;
   
   LiteralList* _allLiterals;
   LiteralList::Element* _firstNonselectedLit;

   ulong cut_lit;
   bool nonmarkedExist;

 }; // class OR_PREMISE
 
//******************* Definitions: *************************************

inline bool OR_PREMISE::CollectNonmarkedLiterals()
 {
  CALL("CollectNonmarkedLiterals()");
  ulong currMax = 0;

  nonmarkedExist = (_firstNonselectedLit != 0);   

  while (currMax <= last_max)
   {   
    if (!(_litIsMarked[currMax])) 
     {
      nonmarkedExist = true;
      if (!(_resolvent->PushPrefixLit(_maxLiteral[currMax],_litIsInherentlySelected[currMax])))
       {
        return false; 
       };
     };
    currMax++;
   };
  
   
  for(LiteralList::Iterator iter(_allLiterals,_firstNonselectedLit);iter.notEnd();iter.next())     
   if (!(_resolvent->PushStandardLit(index,iter.currentLiteral(),iter.currentElement()->isInherentlySelected()))) return false;
  
  return true;
 }; // inline bool OR_PREMISE::CollectNonmarkedLiterals()

}; // namespace VK


//=================================================================
#endif
