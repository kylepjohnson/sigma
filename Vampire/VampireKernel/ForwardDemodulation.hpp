//
// File:         ForwardDemodulation.hpp
// Description:  Implements forward demodulation. 
// Created:      ???, Alexandre Riazanov, riazanov@cs.man.ac.uk
// Revised:      Dec 24, 2001, Alexandre Riazanov, riazanov@cs.man.ac.uk
//               The definition 
//                typedef Multiset<..> SetOfSplittingBranches
// 
//               replaced by 
// 
//                typedef ExpandingMultiset<..> SetOfSplittingBranches
//                       
// Revised:      Apr 29, 2002, Alexandre Riazanov, riazanov@cs.man.ac.uk
//               KBConstraint and SharedKBConstraint are no longer used. 
//               Generic SimplificationOrdering is used instead. 
// Revised:      May 15, 2002, Alexandre Riazanov, riazanov@cs.man.ac.uk
//               RewriteByPreordered, IntegratePreordered and RemovePreordered gone.
//====================================================
#ifndef FORWARD_DEMODULATION_H
//==================================================================
#define FORWARD_DEMODULATION_H
#ifndef NO_DEBUG_VIS
 #include <iostream>
#endif 
#include "jargon.hpp"
#include "VampireKernelDebugFlags.hpp"
#include "VampireKernelInterceptMacros.hpp"
#include "VampireKernelConst.hpp"
#include "GlobAlloc.hpp"
#include "GList.hpp"
#include "DestructionMode.hpp"
#include "RuntimeError.hpp"
#include "ForwardMatchingIndexTree.hpp"
#include "ForwardMatchingIndex.hpp"
#include "ExpandingMultiset.hpp"
#include "Term.hpp"
#include "Flatterm.hpp"
#include "ForwardMatchingSubstitutionCache.hpp"
#include "Tuple.hpp"
#include "SimplificationOrdering.hpp"
namespace VK
{
class Clause;
}; // namespace VK

//================================================
#undef DEBUG_NAMESPACE
#ifdef DEBUG_FORWARD_DEMODULATION
 #define DEBUG_NAMESPACE "ForwardDemodulation"
#endif 
#include "debugMacros.hpp"
//=================================================

namespace VK
{
class ForwardDemodulation
 {
  public:
   typedef BK::ExpandingMultiset<BK::GlobAlloc,ulong,32UL,ForwardDemodulation> SetOfSplittingBranches;
   class Index 
    {
     public:
      typedef BK::Tuple2<ulong,const Flatterm* const *> VarInstanceLocationPair;
      class Id {};
	  typedef BK::GList<BK::GlobAlloc,VarInstanceLocationPair,Id> StoredSubstitution;
      
      class ConstrRule
       {
        public:
	 ConstrRule() : 
	   clause(0), 
	   constr(0),
	   subst(0),
	   lhs(0)
	   {
	   };
	   
	 
	 void init(Clause* cl,SimplificationOrdering::StoredConstraint* con,
		   StoredSubstitution* sub,TERM* t) 
	   {
	     CALL("init(Clause* cl,SimplificationOrdering::StoredConstraint* con,StoredSubstitution* sub,TERM* t)");
	     clause = cl;
	     subst = sub;
	     lhs = t;
	     if (con) 
	       {
		 constr = SimplificationOrdering::current()->copy(con);
	       }
	     else 
	       constr = 0;
	   };

	 ~ConstrRule() 
	   {
	     CALL("desructor ~ConstrRule()");
	     if (BK::DestructionMode::isThorough())
	       {
		 if (constr) 
		   {
		     SimplificationOrdering::current()->releaseConstraint(constr); 
		   };
		 StoredSubstitution::destroyList(subst);
	       };
	   };
         Clause* clause;
         SimplificationOrdering::StoredConstraint* constr;
         StoredSubstitution* subst;  
         TERM* lhs;  
       private:
	 ConstrRule(const ConstrRule& cr) 
	   {
	     CALL("ConstrRule::constructor ConstrRule(const ConstrRule& cr)");
	     ICP("ICP0");	     
	   };
	 ConstrRule& operator=(const ConstrRule&)
	 {
	   CALL("operator=(const ConstrRule&)");
	   ICP("ICP0");
	   return *this;
	 };
      };
   
      typedef BK::GList<BK::GlobAlloc,ConstrRule,Id> ConstrRuleList; 
      

      
     public:
      class LeafInfo 
      {
       public:
	LeafInfo() : _rules(0) {};
	~LeafInfo() 
	{
	  CALL("Index::LeafInfo::destructor ~LeafInfo()");
	  if (BK::DestructionMode::isThorough())
	    ConstrRuleList::destroyList(_rules);
	};
        ConstrRuleList*& rules() { return _rules; };
       private:
	ConstrRuleList* _rules; 
      };

     public:
      Index() : degree(0L)
       {
	CALL("constructor Index::Index()");
       };
      ~Index() 
	{ 
	  CALL("ForwardDemodulation::destructor ~Index"); 
	}; 
      
      void init()
      {
	_matchingIndex.init();
	degree = 0L;
      };

      void destroy()
      {
	CALL("destroy()");
	_matchingIndex.destroy();
	BK_CORRUPT(*this);
      };
      
      void SetDegree(long d) { degree = d; }; 

      void Integrate(Clause* cl);
      void Remove(Clause* cl);
      Gem::FMCodeTree<VampireKernelConst::MaxTermSize,VampireKernelConst::MaxNumOfVariables,BK::GlobAlloc,TERM,Flatterm,LeafInfo,Index>::Leaf* Match(const Flatterm* query);
      Gem::FMCodeTree<VampireKernelConst::MaxTermSize,VampireKernelConst::MaxNumOfVariables,BK::GlobAlloc,TERM,Flatterm,LeafInfo,Index>::Leaf* MatchAgain();


      #ifndef NO_DEBUG
       bool clean() const { return  _matchingIndex.clean(); };
      #endif
      #ifndef NO_DEBUG_VIS       
       ostream&  output(ostream& str) const { return _matchingIndex.output(str); }; 
      #endif     

     private:
      Gem::FMCodeTree<VampireKernelConst::MaxTermSize,VampireKernelConst::MaxNumOfVariables,BK::GlobAlloc,TERM,Flatterm,LeafInfo,Index>::Leaf* Integrate(TERM* t);
      bool Remove(TERM* t,Gem::FMCodeTree<VampireKernelConst::MaxTermSize,VampireKernelConst::MaxNumOfVariables,BK::GlobAlloc,TERM,Flatterm,LeafInfo,Index>::Leaf*& leaf,Gem::FMCodeTree<VampireKernelConst::MaxTermSize,VampireKernelConst::MaxNumOfVariables,BK::GlobAlloc,TERM,Flatterm,LeafInfo,Index>::Instruction**& branch_to_destroy);
      void IntegrateConstrained(Clause* cl);
      
      void IntegrateConstrained(Clause* cl,
				TERM* t,
				SimplificationOrdering::StoredConstraint* constr);
      void RemoveConstrained(Clause* cl);
      void RemoveConstrained(Clause* cl,TERM* lhs);

      bool appropriateDegreeForGreater(const SimplificationOrdering::SpecialisedComparison::Category& cat) const;
      bool appropriateDegreeForLess(const SimplificationOrdering::SpecialisedComparison::Category& cat) const;

      StoredSubstitution* createStoredSubstitution();

     private:  
      Gem::ForwardMatchingIndex<VampireKernelConst::MaxTermSize,VampireKernelConst::MaxNumOfVariables,BK::GlobAlloc,TERM,Flatterm,LeafInfo,Index> _matchingIndex;

      long degree;
      
    }; // class Index




  public:
   ForwardDemodulation();
   ~ForwardDemodulation();
   void init();
   void destroy();
   void reset();

   void SetDegree(long d) { degree = d; index.SetDegree(d); };
   void SetFDOnSplittingBranchesFlag(bool fl) 
   {
    if (fDOnSplittingBranchesFlag && (!fl)) BK::RuntimeError::report("Switching fDOnSplittingBranchesFlag off.");
    fDOnSplittingBranchesFlag = fl; 
   }; 
   void Integrate(Clause* cl);
   void Remove(Clause* cl);

   static 
   Index::ConstrRuleList* 
   removeConstrRule(Clause* cl,
		    TERM* t,
		    Index::ConstrRuleList* lst
#ifdef INTERCEPT_INDEXED_SEARCH_FOR_GENERALISATIONS
		    , bool& reallyRemoved
#endif
		    );
   static Index::ConstrRuleList* insertConstrRule(Clause* cl,SimplificationOrdering::StoredConstraint* constr,Index::StoredSubstitution* subst,TERM* t,Index::ConstrRuleList* lst);

   bool Rewrite(const Flatterm* query,const SetOfSplittingBranches& splittingBranches)
   {
    CALL("Rewrite(const Flatterm* query,const SetOfSplittingBranches& splittingBranches)");
    ASSERT(degree > 0); 
    branchesUsed = false;
    return RewriteByConstrained(query,splittingBranches);
   };

   Clause* Rule() const { return rule; };  
   bool RHSNext() { return _ruleRHSInstance.next(); };    
   TERM RHSSymbol() { return _ruleRHSInstance.symbol(); };   
   bool SplittingBranchesUsed() const { return branchesUsed; }; 


   #ifndef NO_DEBUG
    bool clean() const { return index.clean(); };
   #endif

   #ifndef NO_DEBUG_VIS 
    ostream& outputIndex(ostream& str) { return index.output(str); };
   #endif

  private:   

   bool CanBeRewritingRule(const Clause* cl);   
   bool CheckSplittingBranches(const Clause* cl,const SetOfSplittingBranches& splittingBranches);
   bool RewriteByConstrained(const Flatterm* query,const SetOfSplittingBranches& splittingBranches);

   bool checkConstraint(const SimplificationOrdering::StoredConstraint* constr);


   void collectSubstCache(const Index::StoredSubstitution* storedSubst);

  private: // data
   Index index;
   Clause* rule;
   
   ForwardMatchingSubstitutionCache::Instance _ruleRHSInstance;

   long degree;
   bool fDOnSplittingBranchesFlag;
   bool branchesUsed;
   ForwardMatchingSubstitutionCache _substCache;
   SimplificationOrdering::ConstraintCheckOnForwardMatchingSubst* _constrCheck;
 }; // class ForwardDemodulation

}; // namespace VK

//=================================================================
#endif
