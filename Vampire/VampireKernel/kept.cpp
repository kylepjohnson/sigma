#include "kept.hpp"
//====================================================
#undef DEBUG_NAMESPACE
#ifdef DEBUG_KEPT
 #define DEBUG_NAMESPACE "KEPT"
#endif
#include "debugMacros.hpp"
//====================================================



using namespace VK;
Clause* KEPT::Next() 
{
 if (!(EmptyCat(0)))
  {
   Clause* res = category[0].Next();
   Clause::Remove(res);
   StatUsed(0); 
   return res;
  };
  
 if (pick_given_ratio > 0)
  {
   if (EmptyCat(minCat))
    {
     return 0; 
    };
   if (picked_by_weight < pick_given_ratio)
    // pick by weight
    {
     picked_by_weight++;
     Clause* res = category[minCat].Next();
     Clause::Remove(res);
     StatUsed(minCat); 
     return res;
    }
   else // pick by history
    {
     unsigned long oldestNum = category[minCat].Next()->Number();
     long oldestCat = minCat;
     for (long cat = minCat + 1; cat<=maxCat; cat++)
      {
       if ((!(EmptyCat(cat))) && (category[cat].Next()->Number() < oldestNum))
        {
         oldestNum = category[cat].Next()->Number();
         oldestCat = cat;
        };
      };
     picked_by_weight = 0;
     Clause* res = category[oldestCat].Next();
     Clause::Remove(res);
     StatUsed(oldestCat);
     return res;
    };
  }
 else
  {
   if (EmptyCat(0)) { return 0; };
   Clause* res = category[0].Next();
   Clause::Remove(res);
   StatUsed(0);
   return res;
  };
}; //  Clause* KEPT::Next() 

void KEPT::DestroyAllClauses()
{
 CALL("DestroyAllClauses()");
 for (long cat = minCat; cat <= maxCat; cat++)
  {
   while (!(EmptyCat(cat)))
    {
     Clause* c = category[cat].Next();
     Clause::Remove(c);
     c->Destroy();
    };
  };
 Reset();  
}; // void KEPT::DestroyAllClauses()


#ifndef NO_DEBUG
 bool KEPT::clean() const
 {
  if (max_priority_flag 
      || num_of_currently_kept
      || num_of_used
      || num_of_removed) return false;
  if (numOfReachable != LONG_MAX) return false;
  for (long cat = 0; cat < NUMBER_OF_KEPT_CATEGORIES; cat++)
   {
    if (currently_kept[cat] || used[cat] || removed[cat]) return false;
    if (category[cat].Next() != (category + cat)) return false;
    if (category[cat].Previous() != (category + cat)) return false;  
   };         
  
 if (minCat || maxCat || picked_by_weight) return false;
  if (max_accepted_cat != NUMBER_OF_KEPT_CATEGORIES-1) return false;
   return true;
 }; // bool KEPT::clean() const

 inline long KEPT::CountCurrentlyKept(long cat)
  {
   Clause* cl = category[cat].Next();
   long res = 0L;
   while (cl != &(category[cat]))
    {
     res++;
     cl = cl->Next();
    };
   return res;
  }; 
   
 bool KEPT::CheckStatistics()
  {
   long sum_used = 0L;
   long sum_removed = 0L;
   long sum_currently_kept = 0L;
   for (long c = 0; c < NUMBER_OF_KEPT_CATEGORIES; c++)
    {
     sum_used += used[c];
     sum_removed += removed[c];
     sum_currently_kept += currently_kept[c];
        
     long real_curr_kept = CountCurrentlyKept(c);
     if (currently_kept[c] != real_curr_kept)
      {
       DOP(DMSG("KEPT::CheckStatistics() > ") << "currently_kept[" << c << "] CORRUPT!\n");
       DOP(DMSG("KEPT::CheckStatistics() > stat = ") << currently_kept[c] << ", real = " << real_curr_kept << "\n");
       return false;
      };
    };  
   if (sum_used != num_of_used) { return false; };
   if (sum_removed != num_of_removed) { return false; };
   if (sum_currently_kept != num_of_currently_kept) { return false; };
   if (minCat > maxCat) { return false; };
   if (minCat < maxCat) 
    {
     if (EmptyCat(minCat)) 
      {
       DOP(DMSG("KEPT::CheckStatistics() > ") 
            << "minCat(" << minCat << ") < maxCat("   
            << maxCat << ") but EMPTY!\n");
       return false;
      };
     if (EmptyCat(maxCat))
      {
       DOP(DMSG("KEPT::CheckStatistics() > ") 
           << "maxCat(" << maxCat << ") > minCat(" << minCat << ") but EMPTY!\n");
       return false;
      };
    };
   return true; 
  }; // bool KEPT::CheckStatistics() 
#endif 




//====================================================
