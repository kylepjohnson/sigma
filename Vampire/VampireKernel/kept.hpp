// Revised: May 19, 2002, Alexandre Riazanov, riazanov@cs.man.ac.uk
//          Two more ways of estimating weight reachability added:
//          definitelyReachable(..) and potentiallyReachable(..).
// Revised: May 21, 2002, Alexandre Riazanov, riazanov@cs.man.ac.uk
//          Bug fix in Category(long weight).
//======================================================================

#ifndef KEPT_H
//======================================================================
#define KEPT_H
#include <climits>
#include "jargon.hpp"
#include "VampireKernelDebugFlags.hpp"
#include "VampireKernelConst.hpp"
#include "Clause.hpp"

//====================================================
#undef DEBUG_NAMESPACE
#ifdef DEBUG_KEPT
#define DEBUG_NAMESPACE "KEPT"
#endif
#include "debugMacros.hpp"
//====================================================
namespace VK
{

const long NUMBER_OF_KEPT_CATEGORIES = VampireKernelConst::MaxClauseSize + 1;

class KEPT
{
 public:
  KEPT() : pick_given_ratio(0), magic_coefficient(0.0) { Reset(); };
  ~KEPT() { CALL("destructor ~KEPT()"); }; 
  
  void init()
  {
    CALL("init()");
    for (long cat = 0; cat < NUMBER_OF_KEPT_CATEGORIES; cat++)
      category[cat].init();
    pick_given_ratio = 0;
    magic_coefficient = 0.0;
    Reset();
  };
  
  void destroy()
  {
    CALL("destroy()");
    for (long cat = 0; cat < NUMBER_OF_KEPT_CATEGORIES; cat++)
      category[cat].destroy();
  };

  void Reset()
    {
      max_priority_flag = false;
      num_of_currently_kept = 0L;
      num_of_used = 0L;
      num_of_removed = 0L;
      num_of_cancelled = 0L;
      numOfReachable = LONG_MAX;
      for (long cat = 0; cat < NUMBER_OF_KEPT_CATEGORIES; cat++) StatReset(cat);
      minCat = 0;
      maxCat = 0;
      max_accepted_cat = NUMBER_OF_KEPT_CATEGORIES-1;
      picked_by_weight = 0;
    };
  void DestroyAllClauses();
  void SetPickGivenRatio(long pgr) 
    {
      pick_given_ratio = pgr; 
      if (pgr > 0) magic_coefficient = 1.0 + ((1.0)/((float)pgr));
    };
  void MaxPriorityOn() { max_priority_flag = true; };
  void MaxPriorityOff() { max_priority_flag = false; }; 
  bool isEmpty() const { return !num_of_currently_kept; };
  long Size() const { return num_of_currently_kept; };
  long Total() const { return num_of_currently_kept + num_of_used + num_of_removed + num_of_cancelled; };
  long CurrentlyKept() const { return num_of_currently_kept; };
  long NumOfReachable() const { return numOfReachable; };
  long UsedAndRemoved() const { return (num_of_used + num_of_removed); };
  bool MaxPriorityEmpty() { return EmptyCat(0); };
  long UnreachableKept() const
    {
      return (num_of_currently_kept > numOfReachable)? (num_of_currently_kept - numOfReachable) : 0; 
    };
  void SetNumOfReachable1(long n) 
    {
      CALL("SetNumOfReachable1(const long n)");
      ASSERT(CheckStatistics());    
      numOfReachable = n; 
      CorrectMaxAcceptedCat1();
    };  
  void SetNumOfReachable2(long n) 
    {
      CALL("SetNumOfReachable2(const long n)");
      ASSERT(CheckStatistics());    
      numOfReachable = n; 
      CorrectMaxAcceptedCat2();
    };
  void SetNumOfReachable3(long n) 
    {
      CALL("SetNumOfReachable3(const long n)");
      ASSERT(CheckStatistics());    
      numOfReachable = n; 
      CorrectMaxAcceptedCat3();
    };
  long MaxAcceptedWeight() const
    {
      if (num_of_currently_kept < numOfReachable) return VampireKernelConst::MaxClauseSize; 
      if (pick_given_ratio > 0) { return Weight(max_accepted_cat); }
      else return 0L; // ??
    };
  long MaxPresentWeight() const { return Weight(maxCat); };
  long MinPresentWeight() const { return Weight(minCat); };

  void Cancel(Clause* c) { Clause::Remove(c); StatCancelled(Category(c)); };
  void Remove(Clause* c) 
    { 
      Clause::Remove(c); 
      StatRemoved(Category(c)); 
    };
  void Insert(Clause* c) 
    { 
      if (max_priority_flag) 
	{ 
	  c->AssignMaxPriority();
	};
      InsertAs(c,Category(c)); 
    };
  Clause* Next();
  Clause* TheWorstClause()
    {
      CALL("TheWorstClause()");
      for (long cat = maxCat; cat >= minCat; cat--) 
	if (!(EmptyCat(cat))) return category[cat].Previous();
      return 0;
    };

  Clause* TheWorstUnblockedClause()
    {
      CALL("TheWorstUnblockedClause()");
      for (long cat = maxCat; cat >= minCat; cat--) 
	if (!(EmptyCat(cat))) 
	  for (Clause* cand = category[cat].Previous(); cand != &(category[cat]); cand = cand->Previous())
	    if (!cand->Blocked()) return cand;
      return 0;   
    };

  Clause* TheMostUnreachableClause()
    {
      CALL("TheMostUnreachableClause()");
      if (!UnreachableKept()) return 0;
      long cat;
      ulong youngestNumber = 0UL;
      for (cat = maxCat;
	   (cat >= minCat) && (!definitelyReachable(cat));
	   --cat)
	{
	  if (!(EmptyCat(cat))) 
	    {
	      if (category[cat].Previous()->Number() > youngestNumber)
		youngestNumber = category[cat].Previous()->Number();
	    };
	};
      ++cat;
      if (!youngestNumber) return 0;
      while (cat <= maxCat)
	{
	  if (!(EmptyCat(cat))) 
	    {
	      if (category[cat].Previous()->Number() == youngestNumber)
		return category[cat].Previous();
	    };
	  ++cat;
	};
      ICP("END");
#if (!defined NO_DEBUG) || (defined _SUPPRESS_WARNINGS_)
      return 0;
#endif
    }; // Clause* TheMostUnreachableClause()



  void ResetIterator()
    {
      if (EmptyCat(minCat)) { next_clause = 0; return; };
      iter_cat = minCat;
      next_clause = category[iter_cat].Next();
    };
  Clause* IterNext() 
    {
      Clause* res = next_clause;     
      if (res)
	{
	  next_clause = res->Next();
	  if (next_clause == &(category[iter_cat]))
	    {
	      // try another category
	      iter_cat++;       
	      while ((iter_cat <= maxCat) && (EmptyCat(iter_cat))) { iter_cat++; };
	      if (iter_cat > maxCat) 
		{ 
		  next_clause = 0; 
		}
	      else { next_clause = category[iter_cat].Next(); };
	    };
	};
      return res;
    }; // Clause* IterNext() 
 
 private:   
  void NormalizeMinMaxCat(long lb,long rb)
    {
      minCat = lb;
      maxCat = rb;
      while ((EmptyCat(minCat)) && (minCat < maxCat)) minCat++;
      while ((EmptyCat(maxCat)) && (minCat < maxCat)) maxCat--;
    };
  
  void StatReset(long cat) 
    {
      currently_kept[cat] = 0L;
      used[cat] = 0L;
      removed[cat] = 0L;
    };
  
  void StatInserted(long cat)
    {
      currently_kept[cat]++;
      num_of_currently_kept++;    
      if (cat > maxCat) { NormalizeMinMaxCat(minCat,cat); }
      else
	if (cat < minCat) NormalizeMinMaxCat(cat,maxCat);
    };
   
  void StatUsed(long cat)
    {
      used[cat]++;
      currently_kept[cat]--;
      num_of_used++;     
      num_of_currently_kept--;
      numOfReachable--; // experimental
      if (EmptyCat(cat) && ((cat == minCat) || (cat == maxCat)))
	NormalizeMinMaxCat(minCat,maxCat);
    };
   
  void StatRemoved(long cat)
    {
      removed[cat]++;
      currently_kept[cat]--;
      num_of_removed++;
      num_of_currently_kept--;
      numOfReachable--; // experimental
      if (EmptyCat(cat) && ((cat == minCat) || (cat == maxCat)))
	NormalizeMinMaxCat(minCat,maxCat);
    };
  
  void StatCancelled(long cat)
    {
      num_of_cancelled++;
      currently_kept[cat]--;
      num_of_currently_kept--;
      if (EmptyCat(cat) && ((cat == minCat) || (cat == maxCat)))
	NormalizeMinMaxCat(minCat,maxCat);     
    };

  long Total(long cat) const { return currently_kept[cat] + used[cat] + removed[cat]; };
 
  long NumOfLighter(long cat)
    {
      long res = 0L;
      for (long c = 0; c < cat; c++) res += currently_kept[c]; 
      return res;
    };
   
  long NumOfHeavier(long cat)
    {
      long res = 0L;
      for (long c = cat + 1; c <= maxCat; c++) res += currently_kept[c];
      return res;
    };


  /********** old version, used in CASC-16 and CASC-17 *********/
  bool Reachable(long cat)
    {
      CALL("Reachable(long cat)");
      if (numOfReachable > num_of_currently_kept) return true;   
      long num_of_lighter_or_same_weight = NumOfLighter(cat) + currently_kept[cat];
      if (!num_of_lighter_or_same_weight) return true; 
      if (numOfReachable <= num_of_lighter_or_same_weight) return false;
      long magic_number = (long)(num_of_lighter_or_same_weight * magic_coefficient);
      return (numOfReachable - (num_of_currently_kept - numOfReachable) > magic_number);
    };
  /***********************/


  bool definitelyReachable(long cat)
    {
      CALL("definitelyReachable(long cat)");   
      if (numOfReachable > num_of_currently_kept) return true; 
      long num_of_lighter_or_same_weight = NumOfLighter(cat) + currently_kept[cat];
      if (!num_of_lighter_or_same_weight) return true; 
      if (numOfReachable <= num_of_lighter_or_same_weight) return false;
      long magic_number = (long)(num_of_lighter_or_same_weight * magic_coefficient);
      DOP(bool debugRes = (magic_number < numOfReachable));
      ASSERT((!debugRes) || potentiallyReachable(cat));
      // Definitely rechable provided that magic_number < numOfReachable,
      // even if some reachable clauses are of greater weight.
      return (magic_number < numOfReachable); 
    }; // bool definitelyReachable(long cat)


  bool potentiallyReachable(long cat)
    {
      CALL("potentiallyReachable(long cat)");   
      if (numOfReachable > num_of_currently_kept) return true; 
      long num_of_lighter_or_same_weight = NumOfLighter(cat) + currently_kept[cat];
      if (!num_of_lighter_or_same_weight) return true; 
      if (numOfReachable <= num_of_lighter_or_same_weight) return false;

      // May be reachable if all reachable clauses are of the same
      // or smaller weight. 
      return true; 
    }; // bool potentiallyReachable(long cat)
  

 
  void CorrectMaxAcceptedCat1();
  void CorrectMaxAcceptedCat2();
  void CorrectMaxAcceptedCat3();

  long Category(long weight) 
    { 
      if (pick_given_ratio > 0)
	{
	  if (weight < 0L) return 0L;
	  if (weight >= NUMBER_OF_KEPT_CATEGORIES) return NUMBER_OF_KEPT_CATEGORIES - 1;
          return weight;
	};
      return 0L;
    };

  long Weight(long cat) const { return cat; };
   
  long Category(Clause* c) 
    {
      if (c->MaxPriority()) return 0; 
      return Category(c->weight());
    };
  void InsertAs(Clause* c,long cat) { Clause::InsertBefore(c,&(category[cat])); StatInserted(cat); };
  bool EmptyCat(long cat) { return (category[cat].Next() == &(category[cat])); };
         
 public: // for debugging
#ifndef NO_DEBUG
  bool clean() const;
  long CountCurrentlyKept(long cat);
  bool CheckStatistics();
#endif 

 private: // main data
  bool max_priority_flag;
  long pick_given_ratio; // <=0 if no weighting is used (default)
  float magic_coefficient; // must be = 1 + (1/pick_given_ratio);
  long picked_by_weight;
  Clause category[NUMBER_OF_KEPT_CATEGORIES];
  long minCat;
  long maxCat;
  long max_accepted_cat;
 private: // statistics
  long currently_kept[NUMBER_OF_KEPT_CATEGORIES];
  long used[NUMBER_OF_KEPT_CATEGORIES];
  long removed[NUMBER_OF_KEPT_CATEGORIES];
  long num_of_currently_kept;
  long num_of_used;
  long num_of_removed;
  long num_of_cancelled;
  long numOfReachable;

 private: // iterator
  Clause* next_clause;
  long iter_cat; 
}; // class KEPT

}; // namespace VK
//====================================================
#undef DEBUG_NAMESPACE
#ifdef DEBUG_KEPT
#define DEBUG_NAMESPACE "KEPT"
#endif
#include "debugMacros.hpp"
//====================================================

namespace VK
{
inline void KEPT::CorrectMaxAcceptedCat1()
{
  if (num_of_currently_kept < numOfReachable) 
    {
      max_accepted_cat = NUMBER_OF_KEPT_CATEGORIES-1;
      return;
    };
  if (definitelyReachable(max_accepted_cat))
    {
      while (definitelyReachable(max_accepted_cat + 1))
	{
	  max_accepted_cat++;
	  if (max_accepted_cat > maxCat) 
	    {
	      max_accepted_cat = NUMBER_OF_KEPT_CATEGORIES-1;
	      return;
	    };
	};
    }
  else
    {
      do
	{
	  max_accepted_cat--;
	}
      while ((max_accepted_cat > 0) && (!(definitelyReachable(max_accepted_cat))));
    };     
}; // void KEPT::CorrectMaxAcceptedCat1()


inline void KEPT::CorrectMaxAcceptedCat2()
{
  if (num_of_currently_kept < numOfReachable) 
    {
      max_accepted_cat = NUMBER_OF_KEPT_CATEGORIES-1;
      return;
    };
  if (potentiallyReachable(max_accepted_cat))
    {
      while (potentiallyReachable(max_accepted_cat + 1))
	{
	  max_accepted_cat++;
	  if (max_accepted_cat > maxCat) 
	    {
	      max_accepted_cat = NUMBER_OF_KEPT_CATEGORIES-1;
	      return;
	    };
	};
    }
  else
    {
      do
	{
	  max_accepted_cat--;
	}
      while ((max_accepted_cat > 0) && (!(potentiallyReachable(max_accepted_cat))));
    };     
}; // void KEPT::CorrectMaxAcceptedCat2()

inline void KEPT::CorrectMaxAcceptedCat3()
{
  if (num_of_currently_kept < numOfReachable) 
    {
      max_accepted_cat = NUMBER_OF_KEPT_CATEGORIES-1;
      return;
    };
  if (Reachable(max_accepted_cat))
    {
      while (Reachable(max_accepted_cat + 1))
	{
	  max_accepted_cat++;
	  if (max_accepted_cat > maxCat) 
	    {
	      max_accepted_cat = NUMBER_OF_KEPT_CATEGORIES-1;
	      return;
	    };
	};
    }
  else
    {
      do
	{
	  max_accepted_cat--;
	}
      while ((max_accepted_cat > 0) && (!(Reachable(max_accepted_cat))));
    };     
}; // void KEPT::CorrectMaxAcceptedCat3()

}; // namespace VK
//====================================================
#endif
