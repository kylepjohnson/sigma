//
// File:         Clause.cpp
// Description:  Representation of stored clauses.
// Created:      Feb 13, 2000
// Author:       Alexandre Riazanov
// mail:         riazanov@cs.man.ac.uk
// Revised:      Dec 11, 2001. 
//               Freed from VampireKernelConst::MaxNumOfLiterals.
//====================================================
#include "Clause.hpp"
#include "VampireKernelInterceptMacros.hpp"
//================================================
#undef DEBUG_NAMESPACE
#ifdef DEBUG_CLAUSE
#define DEBUG_NAMESPACE "Clause"
#endif 
#include "debugMacros.hpp"
//================================================= 
using namespace std;
using namespace BK;
using namespace VK;

long Clause::InitStatic::_count; // Implicitely initialised to 0L
#ifdef DEBUG_ALLOC_OBJ_TYPE
ClassDesc Clause::_classDesc("Clause",sizeof(Clause),sizeof(Clause));
ClassDesc Clause::List::_classDesc("Clause::List",Clause::List::minSize(),Clause::List::maxSize());
#endif
float Clause::_selectedPosEqPenaltyCoefficient;
bool Clause::_useSelectedPosEqPenaltyCoefficient;      
float Clause::_nongoalPenaltyCoefficient;
bool Clause::_useNongoalPenaltyCoefficient;


Clause* Clause::unsharedCopy() const
{
  CALL("unsharedCopy() const");
  ASSERT(!isBuiltInTheoryFact());

  LiteralList::Iterator iter(LitList());
  LiteralList* targetLitList = new (LitList()->length()) LiteralList(LitList()->length(),LitList()->numOfSelectedLits());
  LiteralList::Iterator targetIter(targetLitList);

  while (iter.notEnd()) 
    {
      ASSERT(targetIter.notEnd());
      targetIter.currentElement()->setLiteral(iter.currentLiteral()->unsharedLitCopy(),iter.currentElement()->mark());


      iter.next();
      targetIter.next();
    };
  ASSERT(iter.end());
  ASSERT(targetIter.end());
 
  Clause* res = new Clause(targetLitList);
  _background.copyTo(res->background());
  res->number = number;


  res->_size = _size;
  res->_minChildWeight = _minChildWeight;

  //res->_flags = _flags; 
  ASSERT(!res->_flags);
  res->setMainSet(MainSetFree);
  if (flag(FlagHasChildren)) res->setFlag(FlagHasChildren);

  res->num_of_pos_eq = num_of_pos_eq;

  return res;
}; // Clause* Clause::unsharedCopy() const


void Clause::Destroy()         
{
  CALL("Destroy()");
  ASSERT(checkObj());
  INTERCEPT_BACKWARD_SUBSUMPTION_INDEXING_UNBIND_CLAUSE_ID(this);
  delete litList;
  //delete ancestors;
  delete this;
};  // Clause::Destroy()

void Clause::DestroyUnshared()         
{
  CALL("DestroyUnshared()");

  ASSERT(checkObj());
  INTERCEPT_BACKWARD_SUBSUMPTION_INDEXING_UNBIND_CLAUSE_ID(this);
  for (LiteralList::Iterator iter(LitList());iter.notEnd();iter.next()) 
    iter.currentLiteral()->destroyUnsharedLit();
   
  delete litList;
  //delete ancestors;
  delete this;
};  // Clause::DestroyUnshared()


 
bool Clause::MakeDefinitionOf(const TERM& definedHeader)
{
  CALL("MakeDefinitionOf(const TERM& definedHeader)");
  // true iff there is at least one literal with this header
  ulong headSize = 0UL;
  LiteralList::Iterator nextHeadLit(LitList());
  LiteralList::Iterator nextTailLit(LitList());
 
 find_next_head_lit:
  if (nextHeadLit.end()) 
    {
      if (headSize) 
	{
	  LitList()->setNumOfSelected(headSize); 
	  return true;
	};
      return false;
    };
  if (*(nextHeadLit.currentLiteral()) != definedHeader)
    {
      nextHeadLit.next();
      goto find_next_head_lit;   
    };
 find_next_tail_lit: 
  if (nextTailLit.litNum() < nextHeadLit.litNum())
    { 
      if (*(nextTailLit.currentLiteral()) != definedHeader)    
	{
	  // swap this literal with the current head literal
	  TERM* headLit = nextHeadLit.currentLiteral();
	  nextHeadLit.currentElement()->setLiteral(nextTailLit.currentLiteral());      
	  nextTailLit.currentElement()->setLiteral(headLit);
	  nextHeadLit.next();
	  nextTailLit.next();
	  goto find_next_head_lit;
	}
      else
	{
	  nextTailLit.next();
	  goto find_next_tail_lit;
	};
    };
  nextHeadLit.next();
  goto find_next_head_lit;

}; // bool Clause::MakeDefinitionOf(const TERM& definedHeader)

bool Clause::Definition(const DefinedHeaderTable* definedHeaderTable,TERM& definedHeader) const
{
  CALL("Definition(const DefinedHeaderTable* definedHeaderTable) const");
  for(LiteralList::Iterator iter(LitList());iter.notEnd();iter.next())
    if (definedHeaderTable->isDefined(iter.currentLiteral()->HeaderNum())) 
      {
	definedHeader = *(iter.currentLiteral());
	return true;
      };
  return false; 
}; // bool Clause::Definition(const DefinedHeaderTable* definedHeaderTable,TERM& definedHeader) const

bool Clause::HornClause() const
{
  CALL("HornClause() const");
  if (Unit()) { return true; };
  ulong num_of_pos = 0;
  for(LiteralList::Iterator iter(LitList());iter.notEnd();iter.next())
    {
      if (!iter.currentLiteral()->isAnswerLiteral())
	{
	  if (iter.currentLiteral()->Positive()) 
	    {
	      num_of_pos++;
	      if (num_of_pos > 1) { return false; };
	    };
	};
    };
  return true;
}; // bool Clause::HornClause() const

bool Clause::SymmetryLaw() const 
{
  if (numOfAllLiterals() != 2) return false;
  const TERM* lit1 = LitList()->firstLit();
  const TERM* lit2 = LitList()->nth(1)->literal();
  if (!TERM::Symmetry(lit1,lit2)) return false;
  return lit1->polarity() != lit2->polarity();
}; // bool Clause::SymmetryLaw() const


bool Clause::TotalityLaw() const 
{
  if (numOfAllLiterals() != 2) return false;
  const TERM* lit1 = LitList()->firstLit();
  const TERM* lit2 = LitList()->nth(1)->literal();
  if (!TERM::Symmetry(lit1,lit2)) return false;
  return lit1->polarity() == lit2->polarity();
}; // bool Clause::TotalityLaw() const


bool Clause::CommutativityLaw(ulong& func) const
{
  CALL("CommutativityLaw(ulong& func) const");
  if (Unit())
    {
      const TERM* lit = LitList()->firstLit();
      if  (lit->Positive() 
	   && lit->IsEquality()
	   && lit->Arg1()->IsReference()
	   && lit->Arg2()->IsReference()
	   && (TERM::Symmetry(lit->Arg1()->First(),lit->Arg2()->First())))
	{
	  func = lit->Arg1()->First()->functor();
	  return true;
	};
    }; 
  return false;     
}; // bool Clause::CommutativityLaw(ulong& func) const

ostream& Clause::outputInTPTPFormat(ostream& str) const
{
  CALL("outputInTPTPFormat(ostream& str) const");

  str << "input_clause(";
  str << "cl" << Number() << "_anc";
  if (!isInputClause())
    {
      for (const ClauseBackground::ClauseList* cl = _background.ancestors(); 
	   cl; 
	   cl = cl->tl())
	str << '_' << cl->hd()->Number();  
    };
  str << "_org_";
  ClauseBackground::Rules rules = _background.properties();
  size_t numOfRules = rules.count();
  if (rules.bit(ClauseBackground::VIP)) numOfRules--; 
  if (rules.bit(ClauseBackground::NegSel)) numOfRules--; 
  if (rules.bit(ClauseBackground::Input))
    { 
      str << "in";
      numOfRules--;
      if (numOfRules) str << '_';   
    };

  if (rules.bit(ClauseBackground::Prepro)) 
    {    
      str << "pp";
      numOfRules--;
      if (numOfRules) str << '_';
    };

  if (rules.bit(ClauseBackground::Subgoal)) 
    {    
      str << "sg";
      numOfRules--;
      if (numOfRules) str << '_';
    };

  if (rules.bit(ClauseBackground::BuiltInTheoryFact)) 
    {    
      str << "bitf";
      numOfRules--;
      if (numOfRules) str << '_';
    };

  if (rules.bit(ClauseBackground::Support)) 
    {    
      str << "sprt";
      numOfRules--;
      if (numOfRules) str << '_';
    };

  if (rules.bit(ClauseBackground::BinRes))
    {
      str << "br";
      numOfRules--;
      if (numOfRules) str << '_';
    };


  if (rules.bit(ClauseBackground::ForwSup)) 
    {
      str << "fs";
      numOfRules--;
      if (numOfRules) str << '_';
    };

  if (rules.bit(ClauseBackground::BackSup)) 
    {
      str << "bs";
      numOfRules--;
      if (numOfRules) str << '_';
    };

  if (rules.bit(ClauseBackground::EqRes)) 
    {
      str << "er";
      numOfRules--;
      if (numOfRules) str << '_';
    };  

  if (rules.bit(ClauseBackground::EqFact)) 
    {
      str << "ef";
      numOfRules--;
      if (numOfRules) str << '_';
    };     

  if (rules.bit(ClauseBackground::ForwDemod)) 
    {
      str << "fd";
      numOfRules--;
      if (numOfRules) str << '_';
    };

  if (rules.bit(ClauseBackground::BackDemod)) 
    {
      str << "bd";
      numOfRules--;
      if (numOfRules) str << '_';
    };

  if (rules.bit(ClauseBackground::EqResSimp)) 
    {
      str << "ers";
      numOfRules--;
      if (numOfRules) str << '_';
    };  

  if (rules.bit(ClauseBackground::ForwSubsRes)) 
    {
      str << "fsr";
      numOfRules--;
      if (numOfRules) str << '_';
    };

  if (rules.bit(ClauseBackground::BackSubsRes)) 
    {
      str << "bsr";
      numOfRules--;
      if (numOfRules) str << '_';
    };

  if (rules.bit(ClauseBackground::Split)) 
    {
      str << "sp"; 
      numOfRules--;
      if (numOfRules) str << '_';
    };

  if (rules.bit(ClauseBackground::Reanimation)) 
    {
      str << "rea"; 
      numOfRules--;
      if (numOfRules) str << '_';
    }; 

  if (rules.bit(ClauseBackground::Name)) 
    {
      str << "nm";
      numOfRules--;
      if (numOfRules) str << '_';
    }; 
  
  if (rules.bit(ClauseBackground::SimplificationByBuiltInTheories)) 
    str << "sbit";

  str << ',';
  str << "conjecture";
  str << ',';
  str << "\n [";


  for(LiteralList::Iterator iter(LitList());iter.notEnd();iter.next())  
    { 
      const TERM* lit = iter.currentLiteral();  
      str << "\n  " << (lit->Positive() ? "++" : "--");
      lit->outputInTPTPFormat(str);
      if (!iter.last()) str << ',';
    };
  str << "\n ]).";
  return str;
}; // ostream& Clause::outputInTPTPFormat(ostream& str) const


ostream& Clause::outputAsPrologTerm(ostream& str) const
{
  CALL("outputAsPrologTerm(ostream& str) const");
  str << '[';
  // clause body
  str << '[';

  ulong maxLits = LitList()->numOfSelectedLits();
  for (LiteralList::Iterator iter(LitList());iter.notEnd();iter.next())
    {
      if (maxLits) 
	{
	  str << (iter.currentLiteral()->Positive() ? "+++" : "---");
	  maxLits--;
	}
      else str << (iter.currentLiteral()->Positive() ? "++" : "--");
      iter.currentLiteral()->outputAsPrologTerm(str);
      if (!iter.last()) str << ','; 
    };

  str << ']';

  // number
  str << ',' << Number();
 
  // ancestors
  str << ",[";

  if (!isInputClause())
    {
      for (const ClauseBackground::ClauseList* cl = _background.ancestors(); 
	   cl; 
	   cl = cl->tl())
	{
	  str << cl->hd()->Number();
	  if (cl->tl()) str << ',';
	};
    };
  str << ']';
 
  // rules
  str << ",[";
  ClauseBackground::Rules rules = _background.properties();
  size_t numOfRules = rules.count();
  if (rules.bit(ClauseBackground::VIP)) numOfRules--; 
  if (rules.bit(ClauseBackground::NegSel)) numOfRules--; 
  if (rules.bit(ClauseBackground::Input))
    { 
      str << "in";
      numOfRules--;
      if (numOfRules) str << ',';
    };

  if (rules.bit(ClauseBackground::Prepro)) 
    {
      str << "pp";
      numOfRules--;
      if (numOfRules) str << ',';
    };

  if (rules.bit(ClauseBackground::Subgoal)) 
    {
      str << "sg";
      numOfRules--;
      if (numOfRules) str << ',';
    };

  if (rules.bit(ClauseBackground::BuiltInTheoryFact)) 
    {
      str << "bitf";
      numOfRules--;
      if (numOfRules) str << ',';
    };

  if (rules.bit(ClauseBackground::Support)) 
    {
      str << "sprt";
      numOfRules--;
      if (numOfRules) str << ',';
    };

  if (rules.bit(ClauseBackground::BinRes))
    {
      str << "br";
      numOfRules--;
      if (numOfRules) str << ',';
    };


  if (rules.bit(ClauseBackground::ForwSup)) 
    {
      str << "fs";
      numOfRules--;
      if (numOfRules) str << ',';
    };

  if (rules.bit(ClauseBackground::BackSup)) 
    {
      str << "bs";
      numOfRules--;
      if (numOfRules) str << ',';
    };

  if (rules.bit(ClauseBackground::EqRes)) 
    {
      str << "er";
      numOfRules--;
      if (numOfRules) str << ',';
    };  

  if (rules.bit(ClauseBackground::EqFact)) 
    {
      str << "ef";
      numOfRules--;
      if (numOfRules) str << ',';
    };     

  if (rules.bit(ClauseBackground::ForwDemod)) 
    {
      str << "fd";
      numOfRules--;
      if (numOfRules) str << ',';
    };

  if (rules.bit(ClauseBackground::BackDemod)) 
    {
      str << "bd";
      numOfRules--;
      if (numOfRules) str << ',';
    };

  if (rules.bit(ClauseBackground::EqResSimp)) 
    {
      str << "ers";
      numOfRules--;
      if (numOfRules) str << ',';
    };  

  if (rules.bit(ClauseBackground::ForwSubsRes)) 
    {
      str << "fsr";
      numOfRules--;
      if (numOfRules) str << ',';
    };

  if (rules.bit(ClauseBackground::BackSubsRes)) 
    {
      str << "bsr";
      numOfRules--;
      if (numOfRules) str << ',';
    };

  if (rules.bit(ClauseBackground::Split)) 
    {
      str << "sp"; 
      numOfRules--;
      if (numOfRules) str << ',';
    };

  if (rules.bit(ClauseBackground::Reanimation)) 
    {
      str << "rea"; 
      numOfRules--;
      if (numOfRules) str << ',';
    };

  if (rules.bit(ClauseBackground::Name)) 
    {
      str << "nm"; 
      numOfRules--;
      if (numOfRules) str << ',';
    };

  if (rules.bit(ClauseBackground::SimplificationByBuiltInTheories)) 
    str << "sbit";

  str << ']'; 


  return str << ']';
}; // ostream& Clause::outputAsPrologTerm(ostream& str) const

ostream& Clause::outputBackground(ostream& str) const
{
  if (IsVIP()) str << "vip ";
  str << '['; 
  BitWord rules = _background.properties();

  ClauseBackground::outputRules(str,rules);
  if (!isInputClause())
    {
      for (const ClauseBackground::ClauseList* cl = _background.ancestors(); 
	   cl; 
	   cl = cl->tl())
	{
	  str << cl->hd()->Number();
	  if (cl->tl()) str << ',';
	};
    };

  return str << ']';
}; // ostream& Clause::outputBackground(ostream& str) const


ostream& Clause::output(ostream& str) const  
{
  if (mainSet() == MainSetActive) str << "* ";
  if (mainSet() == MainSetDefinitions) str << "DEF ";
  str << Number() << ". ";
  if (isBuiltInTheoryFact())
    {
      str << "built_in_theory_fact ";
    }
  else
    {
      if (Empty()) { str << '#'; }
      else LitList()->output(str);
	  
      str << " /" << weight() << '/' << size() << '/' << MinResWeight() << "/"
	  << inferenceDepth() << "/ " 
	  <<  NumOfPosEq() << "pe ";
    };
  return outputBackground(str);   
}; // ostream& Clause::output(ostream& str) const


//=================================================  
