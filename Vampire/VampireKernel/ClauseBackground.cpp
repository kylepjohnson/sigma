//
// File:         ClauseBackground.cpp
// Description:  Ancestors of a clause.
// Created:      Mar 3, 2000, 13:25
// Author:       Alexandre Riazanov
// mail:         riazanov@cs.man.ac.uk 
//==================================================================
#include "ClauseBackground.hpp"
//==================================================================
using namespace VK;
using namespace BK;

#ifdef DEBUG_ALLOC_OBJ_TYPE
 ClassDesc ClauseBackground::_classDesc("ClauseBackground",sizeof(ClauseBackground),sizeof(ClauseBackground));
 ClassDesc ClauseBackground::ClauseList::_classDesc("ClauseBackground::ClauseList",
                                                    ClauseBackground::ClauseList::minSize(),
                                                    ClauseBackground::ClauseList::maxSize());
#endif

//==================================================================
#ifdef DEBUG_CLAUSE_BACKGROUND
#undef DEBUG_NAMESPACE
#define DEBUG_NAMESPACE "ClauseBackground"
#endif
#include "debugMacros.hpp"
//==================================================================


      void ClauseBackground::copyTo(ClauseBackground& copy) const 
	{
	  CALL("ClauseBackground& copy");
	  if (isInput())
	    {
	      copy = *this;
	    }
	  else
	    {
	      ClauseList* orgClauses = _ancestors;
	      ClauseList* targClauses = 0;
	      ClauseList** targClausesAddr = &targClauses;      
	      while (orgClauses)
		{
		  *targClausesAddr = 
		    new ClauseList(orgClauses->hd(),static_cast<ClauseList*>(0));
		  targClausesAddr = &((*targClausesAddr)->tl());
		  orgClauses = orgClauses->tl();
		};
	      copy.init(_rules,targClauses,_inferenceDepth);
	    };
	}; // void ClauseBackground::copyTo(ClauseBackground& copy) const 




      ostream& ClauseBackground::outputRules(ostream& str,const BitWord& _rules) 
	{
	  if (_rules.bit(ClauseBackground::Input)) str << "in ";
	  if (_rules.bit(ClauseBackground::Prepro)) str << "pp ";
	  if (_rules.bit(ClauseBackground::Subgoal)) str << "sg ";
	  if (_rules.bit(ClauseBackground::BuiltInTheoryFact)) str << "bitf ";	
	  if (_rules.bit(ClauseBackground::Support)) str << "sprt ";  
	  if (_rules.bit(ClauseBackground::BinRes)) str << "br ";
	  if (_rules.bit(ClauseBackground::ForwSup)) str << "fs ";
	  if (_rules.bit(ClauseBackground::BackSup)) str << "bs ";
	  if (_rules.bit(ClauseBackground::EqRes)) str << "er ";
	  if (_rules.bit(ClauseBackground::EqFact)) str << "ef ";
	  if (_rules.bit(ClauseBackground::ForwDemod)) str << "fd ";
	  if (_rules.bit(ClauseBackground::BackDemod)) str << "bd ";
	  if (_rules.bit(ClauseBackground::EqResSimp)) str << "ers ";
	  if (_rules.bit(ClauseBackground::ForwSubsRes)) str << "fsr ";
	  if (_rules.bit(ClauseBackground::BackSubsRes)) str << "bsr ";
	  if (_rules.bit(ClauseBackground::Split)) str << "sp ";
	  if (_rules.bit(ClauseBackground::Reanimation)) str << "rea ";
	  if (_rules.bit(ClauseBackground::Name)) str << "nm ";
	  if (_rules.bit(ClauseBackground::NegSel)) str << "ns ";
	  if (_rules.bit(ClauseBackground::SimplificationByBuiltInTheories)) str << "sbit ";
	  return str;
	}; //  ostream& ClauseBackground::outputRules(ostream& str,const BitWord& _rules) 


//========================================
#ifdef DEBUG_CLAUSE_BACKGROUND
#undef DEBUG_NAMESPACE
#define DEBUG_NAMESPACE "OpenClauseBackground"
#endif
#include "debugMacros.hpp"
//========================================

      void OpenClauseBackground::InverseOrder() 
	{
	  Clause** b = _ancestors.begin();
	  Clause** e = _ancestors.last();    
	  while (b < e) 
	    {
	      Clause* tmp = *b;
	      *b = *e;
	      *e = tmp;
	      b++;
	      e--;
	    };
	}; // void OpenClauseBackground::InverseOrder() 



      void OpenClauseBackground::writeNewBackgroundTo(ClauseBackground& b) 
	{
	  CALL("writeNewBackgroundTo(ClauseBackground& b)");
	  ClauseBackground::ClauseList* clauses = 0;

	  for (Clause** anc = _ancestors.begin(); anc < _ancestors.end(); anc++)
	    {
	      clauses = new ClauseBackground::ClauseList(*anc,clauses);
	      
	    };
     
	  b.init(_rules,clauses,_inferenceDepth);
	}; // void OpenClauseBackground::writeNewBackgroundTo(ClauseBackground& b) 




//==================================================================
