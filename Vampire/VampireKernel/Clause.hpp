//
// File:         clause.hpp
// Description:  Representation of stored clauses.
// Created:      Feb 13, 2000, Alexandre Riazanov, riazanov@cs.man.ac.uk
//====================================================
#ifndef CLAUSE_H
//===========================================================================
#define CLAUSE_H
#include <iostream>
#include <cstddef>
#include "jargon.hpp"
#include "VampireKernelDebugFlags.hpp"
#ifdef DEBUG_ALLOC_OBJ_TYPE
#  include "ClassDesc.hpp"       
#endif
#include "GlobAlloc.hpp"
#include "GList.hpp"
#include "BitWord.hpp"
#include "Term.hpp"
#include "ClauseBackground.hpp"
#include "LiteralList.hpp"
#include "ExpandingStack.hpp"
#include "DefinedHeaderTable.hpp"
#include "SymbolInfo.hpp"
using namespace std;
//===========================================================================
#undef DEBUG_NAMESPACE
#ifdef DEBUG_CLAUSE
#define DEBUG_NAMESPACE "Clause"
#endif 
#include "debugMacros.hpp"
//================================================= 
namespace VK
{                           
class Clause     
{
 public:
  class PrintRep;
  class Traversal;
  class OpenUnshared; 
  class NewNumber;
  class InitStatic
    {
    public: 
      InitStatic() 
	{
	  if (!_count) { Clause::initStatic(); _count++; };    
	};
      ~InitStatic() {};
    private:
      static long _count;
    };
 private:
  class ClauseList {};  
 public: 
  typedef BK::GList<BK::GlobAlloc,Clause*,ClauseList> List;
  enum MainSet
  {
    MainSetFree = 0UL, // fixed 
    MainSetNew = 1UL,
    MainSetPassive = 2UL,
    MainSetReservedPassive = 3UL, 
    MainSetActive = 4UL,
    MainSetDefinitions = 5UL,

    // end of active sets

    MainSetTrash = 6UL,
    MainSetHistory = 7UL,
    MainSetAnswers = 8UL
  };
  enum FlagNumber
  {
    MainSetPieceBegin = 0UL,
    MainSetPieceEnd = 3UL,

    FlagInClauseIndex = 4UL,
    FlagLiteralsShared = 5UL,
    FlagInForwardSubsumptionIndex = 6UL,
    FlagInBackwardSubsumptionIndex = 7UL,    
    FlagInForwardDemodulationIndex = 8UL,
    FlagInBackDemodulationIndex = 9UL, 
    FlagInInferenceDispatcherAsActive = 10UL,
    FlagInInferenceDispatcherAsDefinition = 11UL,
    FlagInClauseNumIndex = 12UL,
    FlagInQueueForBackSimplification = 13UL,
    FlagInQueueForIntegrationIntoBackwardDemodIndex = 14UL,

    FlagContainsAnswerLiterals = 29UL, 
    FlagHasChildren = 30UL,
    FlagIsBlocked = 31UL          
  };
 public:
  Clause(LiteralList* lits) : 
    litList (lits), 
    num_of_pos_eq(0)
    {
      setMainSet(MainSetFree);      
    };

  // for making arrays of dummy clauses in KEPT and CLAUSE_SET 
  Clause()
    {
      next = this;
      previous = this;  
      litList = 0;
      setMainSet(MainSetFree);
    };
  
  ~Clause() 
    {
      
    };
  
  // for making arrays of dummy clauses in KEPT and CLAUSE_SET 
  void init()
  {   
    next = this;
    previous = this;  
    litList = 0;
    _background.init();
    _flags.init();
    setMainSet(MainSetFree);
  };
  
  void destroy()
  {
    _flags.destroy();
    _background.destroy();
    BK_CORRUPT(*this);
  };

  static void initStatic()
    {
      _selectedPosEqPenaltyCoefficient = 1.0;      
      _useSelectedPosEqPenaltyCoefficient = false;
      _nongoalPenaltyCoefficient = 1.0;
      _useNongoalPenaltyCoefficient = false;
    };
  static void setSelectedPosEqPenaltyCoefficient(float coeff) 
    {
      _selectedPosEqPenaltyCoefficient = coeff;
      if (_selectedPosEqPenaltyCoefficient < 1.0) _selectedPosEqPenaltyCoefficient = 1.0;
      _useSelectedPosEqPenaltyCoefficient = (_selectedPosEqPenaltyCoefficient != 1.0); 
    };

  static void setNongoalPenaltyCoefficient(float coeff)
    {
      _nongoalPenaltyCoefficient = coeff;
      if (_nongoalPenaltyCoefficient < 1.0) _nongoalPenaltyCoefficient = 1.0;
      _useNongoalPenaltyCoefficient = (_nongoalPenaltyCoefficient != 1.0);
    };



  void* Clause::operator new(size_t)
    {
      CALL("operator new(size_t)");
#ifdef DEBUG_ALLOC_OBJ_TYPE 
      ALWAYS(_classDesc.registerAllocated(sizeof(Clause)));
      return BK::GlobAlloc::allocate(sizeof(Clause),&_classDesc);
#else
      return BK::GlobAlloc::allocate(sizeof(Clause));
#endif
    };
  void Clause::operator delete(void* obj)
    { 
      CALL("operator delete(void* obj)");
#ifdef DEBUG_ALLOC_OBJ_TYPE
      ALWAYS(_classDesc.registerDeallocated(sizeof(Clause)));
      BK::GlobAlloc::deallocate(obj,sizeof(Clause),&_classDesc);
#else
      BK::GlobAlloc::deallocate(obj,sizeof(Clause));
#endif
    };

  void clearFlags()
    {
      _flags.clear();
    }; 
  void setMainSet(const MainSet& ms) 
    {
      CALL("setMainSet(const MainSet& ms)");
      ASSERT(!(ms >> 4));   
      _flags.setPiece(MainSetPieceBegin,MainSetPieceEnd,ms);      
      ASSERT(mainSet() == ms);
    };
  MainSet mainSet() const { return (MainSet)_flags.piece(MainSetPieceBegin,MainSetPieceEnd); };
 
  bool flag(const FlagNumber& fn) const { return _flags.bit(fn); };
  
  void setFlag(const FlagNumber& fn) { _flags.set(fn); };
  void clearFlag(const FlagNumber& fn) { _flags.clear(fn); };

#ifndef NO_DEBUG 
  bool checkObj() const;
#endif  

  Clause* Next() const { return next; };
  void SetNext(Clause* c) { next = c; };
  Clause* Previous() const { return previous; };
  void SetPrevious(Clause* p) { previous = p; };
  LiteralList* LitList() const 
    {
      CALL("LitList() const");
      ASSERT(checkObj()); 
      return litList; 
    };
  
  const ClauseBackground& background() const 
    {
      CALL("background() const"); 
      ASSERT(checkObj()); 
      return _background; 
    }; 

  ClauseBackground& background()
    {
      CALL("background()"); 
      ASSERT(checkObj()); 
      return _background; 
    };  
  
  void setBackground(const ClauseBackground& a) 
    { 
      _background = a; 
    };


  void* inputClauseOrigin() const 
    {
      CALL("inputClauseOrigin() const");
      ASSERT(isInputClause());
      return _background.inputClauseOrigin();
    };

  void setInputClauseOrigin(void* orig)
    {
      CALL("setInputClauseOrigin(void* orig)");
      ASSERT(isInputClause());
      _background.setInputClauseOrigin(orig);
    };

  ulong Number() const 
    {
      CALL("Number() const");
      ASSERT(checkObj()); 
      return number;
    };
  void SetNumber(long n) 
    {
      number = n; 
    };

  long inferenceDepth() const { return _background.inferenceDepth(); };

  long weight() const 
    {
      CALL("weight() const"); 
      ASSERT(checkObj());
      long wt = _weightBase; 
      if ((_useSelectedPosEqPenaltyCoefficient) 
	  && (posEqSelected()))
	{
	  wt = (long)(wt*_selectedPosEqPenaltyCoefficient);
	};

      if ((_useNongoalPenaltyCoefficient) &&
	  (!isSubgoal()))
	{
	  wt = (long)(wt*_nongoalPenaltyCoefficient);	  
	};

      if (wt > (long)VampireKernelConst::MaxClauseSize) 
	{
	  wt = VampireKernelConst::MaxClauseSize;
	}
      else 
	if (wt < 1)
	  {
	    wt = 1;
	  };
      
      return wt;
    }; // long weight() const 



  

  long size() const  // logically different from weight()!
    {
      CALL("size() const"); 
      ASSERT(checkObj());
      if (_size < 0) return ((-1)*_size);
      return _size;
    };

    
  void AssignMaxPriority() 
    { 
      if (_size > 0) _size = (-1)*_size; 
    };
  bool MaxPriority() const 
    {
      CALL("MaxPriority() const"); 
      ASSERT(checkObj()); 
      return (_size < 0); 
    };    
  long NumOfPosEq() const 
    {
      CALL("NumOfPosEq() const"); 
      ASSERT(checkObj()); 
      return num_of_pos_eq; 
    };

  ulong NumOfEqLits() const 
    {
      CALL("NumOfEqLits() const");   
      ASSERT(checkObj()); 
      ulong res = 0;

      for(LiteralList::Iterator iter(LitList());iter.notEnd();iter.next()) 
	if (iter.currentElement()->isEquality()) res++;

      return res;       
    };
  
  bool containsEquality() const 
  {
    CALL("containsEquality() const"); 
    ASSERT(checkObj()); 
    for(LiteralList::Iterator iter(LitList());iter.notEnd();iter.next()) 
      if (iter.currentElement()->isEquality()) return true;
    return false;
  }; // bool containsEquality() const 

  void SetNumOfPosEq(long n) 
    { 
      num_of_pos_eq = n; 
    };
  long MinResWeight() const 
    {
      CALL("MinResWeight() const"); 
      ASSERT(checkObj());
      return _minChildWeight; 
    };

  void setMinChildWeight(long mcs) {  _minChildWeight = mcs; };

  ulong numOfAllLiterals() const 
    { 
      CALL("numOfAllLiterals() const");
      ASSERT(checkObj()); 
      return LitList()->length();
    };

  ulong numOfNonanswerLiterals() const
    {
      CALL("numOfNonanswerLiterals() const");
      ASSERT(checkObj());
      if (flag(FlagContainsAnswerLiterals))
	{
	  ulong res = 0;
	  for(LiteralList::Iterator iter(LitList());iter.notEnd();iter.next()) 
	    if (!iter.currentLiteral()->isAnswerLiteral()) res++; 
          ASSERT(res < numOfAllLiterals());
	  return res;  
	}
      else
	return numOfAllLiterals();
    };

  void setSize(long s) { _size = s; };

  void setWeightBase(long w) { _weightBase = w; };

  bool isInputClause() const
    {
      CALL("isInputClause()");
      return _background.isInput();
    };

  bool isSubgoal() const { return _background.isSubgoal(); };

  bool isBuiltInTheoryFact() const { return _background.isBuiltInTheoryFact(); };
  
  bool isSupportClause() const { return _background.isSupport(); };
  
  void markAsBuiltInTheoryFact()
    {
      _background.markAsBuiltInTheoryFact();
    };

  bool Active() const
    {
      CALL("Active() const");
      return  mainSet() < MainSetTrash; 
    };
 
  bool Blocked() const 
    { 
      CALL("Blocked() const"); 
      ASSERT(checkObj());
      return flag(FlagIsBlocked); 
    };
  
  void Block() 
    { 
      CALL("Block()"); 
      ASSERT(checkObj()); 
      setFlag(FlagIsBlocked); 
      // if (number > 0) number = (-1)*number; 
    };
  void Unblock() 
    {
      CALL("Unblock()"); 
      ASSERT(checkObj()); 
      clearFlag(FlagIsBlocked); 
    };

  void MakeVIP() { _background.MakeVIP(); };
  bool IsVIP() const { return _background.IsVIP(); };  
  

  void MarkAsName()
    {
      CALL("MarkAsName()");
      ASSERT(checkObj());
      _background.MarkAsName();
    };

  bool IsSplitName() const 
    {
      CALL("IsSplitName() const");
      ASSERT(checkObj());
      return (_background.properties().bit(ClauseBackground::Split) 
	      && _background.properties().bit(ClauseBackground::Name));
    };

  ulong NumberOfLiteral(const TERM* lit) const
    {
      CALL("NumberOfLiteral(TERM* lit) const");
      ASSERT(checkObj());
      ulong i = 0;
    
      for (LiteralList::Iterator iter(LitList());;iter.next())
	{
	  ASSERT(i < LitList()->length());
	  if (iter.currentLiteral() == lit) return i;
	  i++; 
	};
      ICP("0");
    }; // ulong NumberOfLiteral(const TERM* lit)

  bool Empty() const 
    { 
      CALL("Empty() const"); 
      return (!(numOfAllLiterals())); 
    };
  
  bool isAnswer() const 
    {
      CALL("isAnswer() const");
      ASSERT(LitList()->numOfSelectedLits() || LitList()->containsOnlyAnswerLiterals());
      return !LitList()->numOfSelectedLits();
    };
   

  bool Unit() const 
    { 
      CALL("Unit() const"); 
      // Returns true if the clause contains exactly one non-answer literal.  
      if (flag(FlagContainsAnswerLiterals)) 
	{
          ulong n = 0UL;
	  for(LiteralList::Iterator iter(LitList());iter.notEnd();iter.next()) 
	    if (!iter.currentLiteral()->isAnswerLiteral())
	      {
                if (n) return false;
		n++;                
	      };
          ASSERT((n == 0UL) || (n == 1UL)); 
          return n != 0UL; 
	} 
      else 
	return (numOfAllLiterals() == 1);
    };

  bool PositiveUnitEquality() const
    { 
      // Only applicable if literal selection has been
      // applied to the clause.
      if (!Unit()) return false;    
      const TERM* lit = LitList()->firstElem()->literal();
      return (lit->Positive()) && (lit->IsEquality());
    };
  bool PreorderedPositiveUnitEquality() const
    { 
      // Only applicable if literal selection has been
      // applied to the clause.
      if (!Unit()) return false;    
      const TERM* lit = LitList()->firstElem()->literal();
      return (lit->Positive()) && (lit->IsEquality()) && (lit->IsOrderedEq());
    };

  bool posEqSelected() const 
    {
      CALL("posEqSelected() const");
      ulong numOfSel = LitList()->numOfSelectedLits();
      for (LiteralList::Iterator iter(LitList()); numOfSel; iter.next())
	{
	  if (iter.currentElement()->isEquality() && iter.currentElement()->positive())
	    return true; 
	  numOfSel--;
	};
      return false;
    };

  bool MakeDefinitionOf(const TERM& definedHeader);

  bool DefinitionUnfoldingSelected(const DefinedHeaderTable* definedHeaderTable) const;
 
  bool Definition(const DefinedHeaderTable* definedHeaderTable,TERM& definedHeader) const;

  bool HornClause() const;

  bool IsRule() const; // <=> contains both positive and negative nonsplitting literals 
   
  bool SymmetryLaw() const;

  bool TotalityLaw() const;

  bool CommutativityLaw(ulong& func) const;

  bool ContainsEliminated(const SymbolInfo* predElimTable) const
    {
      CALL("ContainsEliminated(const SymbolInfo* predElimTable) const");
      for (LiteralList::Iterator iter(LitList()); iter.notEnd(); iter.next())
	if (predElimTable->isToBeEliminated(iter.currentElement()->functor())) 
	  return true;
      return false;
    }; 

  void Destroy();
  void DestroyUnshared();
  static void InsertAfter(Clause* c,Clause* db);
  static void InsertBefore(Clause* c,Clause* db);
  static void Remove(Clause* c);
  static Clause* MakeEmptyClause();
   
  Clause* unsharedCopy() const;
  
  bool IsOrphan() const 
    {
      CALL("IsOrphan() const");
      ASSERT(checkObj()); 
      for (const ClauseBackground::ClauseList* cl = _background.ancestors(); 
	   cl; 
	   cl = cl->tl())
	if (!cl->hd()->Active()) return true;
      return false;
    };

  bool allMaximalSelected() const
    {
      CALL("allMaximalSelected() const");
      return !_background.NegativeSelection(); 
    };

  //=========== output: ======================
  ostream& outputBackground(ostream& str) const;
  ostream& output(ostream& str) const;
  ostream& outputAsPrologTerm(ostream& str) const;
      
  ostream& outputInTPTPFormat(ostream& str) const;
  

 private:  // structure
  Clause* next;
  Clause* previous;
  LiteralList* litList; 
  ClauseBackground _background;
  long number;
  long _size;
  long _weightBase;
  long _minChildWeight;
  BK::BitWord _flags;   
  ulong num_of_pos_eq;
#ifdef DEBUG_ALLOC_OBJ_TYPE
  static BK::ClassDesc _classDesc;
#endif
  static float _selectedPosEqPenaltyCoefficient; 
  static bool _useSelectedPosEqPenaltyCoefficient;
  static float _nongoalPenaltyCoefficient;
  static bool _useNongoalPenaltyCoefficient;
  friend class InitStatic;
}; // class Clause
}; // namespace VK

namespace std
{
inline ostream& operator<<(ostream& str,const VK::Clause& cl)
{
  return cl.output(str);
}; 

inline ostream& operator<<(ostream& str,const VK::Clause* cl)
{
  return cl->output(str);
}; 
}; // namespace std


//================================================
#undef DEBUG_NAMESPACE
#ifdef DEBUG_CLAUSE
#define DEBUG_NAMESPACE "Clause::Traversal"
#endif
#include "debugMacros.hpp"
//=================================================
namespace VK
{
class Clause::Traversal
{
 public:
  enum State 
  { 
    EndOfClause,
    LitHeader,
    Func,
    Var               
  };
 public: 
  Traversal() : termTrav(), iter() { CALL("constructor Traversal()"); };
  Traversal(const Clause* c) : termTrav(), iter()
    {
      CALL("constructor Traversal(const Clause* c)");
      ASSERT(!c->isBuiltInTheoryFact());
      reset(c);  
    };   
  ~Traversal() { CALL("destructor ~Traversal()"); };
  void reset(const Clause* c)
    {
      CALL("reset(const Clause* c)");
      if (c->Empty()) { currState = EndOfClause; }
      else 
	{
	  currState = LitHeader;
                 
	  nextLit = c->LitList()->firstLit(); 
 
	  termTrav.reset(nextLit);

	  iter.reset(c->LitList());   
	  iter.next();
	};
    };  
  const State& state() const { return currState; };
  void next()
    {
      CALL("next()");
      termTrav.next();   
      if (termTrav.state() == TERM::Traversal::End) // end of literal
	if (iter.end()) { currState = EndOfClause; }
	else 
	  {
	    nextLit = iter.currentLiteral();
	    currState = LitHeader;
	    termTrav.reset(nextLit);
	    iter.next();
	  }
      else currState = (termTrav.state() == TERM::Traversal::Func) ? Func : Var;
    };

  void after()
    {
      CALL("after()");
      termTrav.after();
      if (termTrav.state() == TERM::Traversal::End) // end of literal
	if (iter.end()) { currState = EndOfClause; }
	else 
	  {
	    nextLit = iter.currentElement()->literal();
	    currState = LitHeader;
	    termTrav.reset(nextLit);
	    iter.next();
	  }
      else currState = (termTrav.state() == TERM::Traversal::Func) ? Func : Var;   
    };

  const TERM& symbol() const { return termTrav.symbol(); };
  const TERM& term() const { return termTrav.term(); };
 private:
  State currState;
  TERM::Traversal termTrav;
  const TERM* nextLit;
  LiteralList::Iterator iter;
}; // class Clause::Traversal 

}; // namespace VK


//================================================
#undef DEBUG_NAMESPACE
#ifdef DEBUG_CLAUSE
#define DEBUG_NAMESPACE "Clause::OpenUnshared"
#endif
#include "debugMacros.hpp"
//=================================================


namespace VK
{
class Clause::OpenUnshared
{
 public:
  OpenUnshared() : _literals(DOP("Clause::OpenUnshared::_literals"))
    {
      CALL("constructor OpenUnshared()");
      reset(); 
    };
  ~OpenUnshared() { CALL("destructor ~OpenUnshared()"); };

  void init()
  {
    CALL("init()");
    _currLit.init();
    _literals.init(DOP("Clause::OpenUnshared::_literals"));
    _background.init();
    reset();
  };

  void destroy()
  {
    CALL("destroy()");
    _background.destroy();
    _literals.destroy();
    _currLit.destroy();
    BK_CORRUPT(*this);
  };

  void reset()
    {
      _length = 0UL;
      _size = 0UL;
      _numOfPosEq = 0UL;
      _currLit.reset();
      _literals.reset();
      _background.Reset();
    };
  bool litHeader(const TERM& hd)
    {
      CALL("litHeader(const TERM& hd)");
      if (_length >= VampireKernelConst::MaxlNumOfLits) { recycle(); return false; };   
      if (_length) _literals.pushSafe(_currLit.result().First());  
      _length++;
      _size += (1 + hd.arity());
      if (hd.Positive() && hd.IsEquality()) _numOfPosEq++;
      _currLit.reset();
      if (!_currLit.litHeader(hd)) { recycle(); return false; }; 
      return true;
    };
  bool functor(const TERM& f)
    {
      CALL("functor(const TERM& f)");
      _size += f.arity();
      if (!_currLit.functor(f)) { recycle(); return false; }; 
      return true;
    };
  void variable(const TERM& v) { _currLit.variable(v); };
  
  void ancestor(Clause* cl) 
    { 
      _background.pushAncestor(cl,cl->inferenceDepth()); 
      if (cl->isSubgoal()) _background.markAsSubgoal();
    };
  void usedRule(ClauseBackground::Rule r) { _background.UsedRule(r); }; 
  void markAsSubgoal() { _background.markAsSubgoal(); };
  void markAsSupport() { _background.markAsSupport(); };

  Clause* result()  
    {
      CALL("result()");
      LiteralList* lits = new(_length) LiteralList(_length,_length);
      if (_length) // nonempty clause
	{
	  LiteralList::Iterator iter(lits);
	  while (_literals)
	    {
	      ASSERT(iter.notEnd()); 
	      iter.currentElement()->setLiteral(_literals.pop());
	      iter.next();
	    };
	  ASSERT(iter.last());
	  iter.currentElement()->setLiteral(_currLit.result().First());
	};
      Clause* res = new Clause(lits);
      _background.writeNewBackgroundTo(res->background());
      res->setSize(_size);
      res->setWeightBase(0);
      res->SetNumOfPosEq(_numOfPosEq);
      res->setMinChildWeight(0);   
      return res;
    };

  void recycle() 
    {
      CALL("recycle()");
      while (_literals) _literals.pop()->DestroyLit();
      _currLit.recycle();    
    };
 private: 
  ulong _length;
  ulong _size;
  ulong _numOfPosEq;
  TERM::OpenUnshared _currLit;
  BK::ExpandingStack<BK::GlobAlloc,TERM*,32,Clause::OpenUnshared> _literals;
  OpenClauseBackground _background;
}; // class Clause::OpenUnshared

}; // namespace VK


//================================================
#undef DEBUG_NAMESPACE
#ifdef DEBUG_CLAUSE
#define DEBUG_NAMESPACE "Clause::NewNumber"
#endif 
#include "debugMacros.hpp"
//=================================================

namespace VK
{
class Clause::NewNumber
{
 public:
  NewNumber() : _nextNumber(0UL) {};
  NewNumber(ulong n) : _nextNumber(n) {};
  void init() { _nextNumber = 0UL; };
  void destroy() {};
  void reset(ulong n) { _nextNumber = n; };
  ulong nextNumber() const { return _nextNumber; };
  ulong generate() { ++_nextNumber; return _nextNumber - 1; };
 private:
  ulong _nextNumber;
}; // class Clause::NewNumber

}; // namespace VK





//================================================
#undef DEBUG_NAMESPACE
#ifdef DEBUG_CLAUSE
#define DEBUG_NAMESPACE "Clause"
#endif 
#include "debugMacros.hpp"
//=================================================

namespace VK
{

inline bool Clause::DefinitionUnfoldingSelected(const DefinedHeaderTable* definedHeaderTable) const
{
  CALL("DefinitionUnfoldingSelected(const DefinedHeaderTable* definedHeaderTable) const");
  LiteralList::Iterator iter(LitList());
  ulong sel = LitList()->numOfSelectedLits();
  while (sel) 
    {
      ASSERT(iter.notEnd());
      TERM header = *(iter.currentLiteral());
      header.Inversepolarity();
      if (definedHeaderTable->isDefined(header.HeaderNum())) return true; 
      iter.next();
      sel--;
    };
  return false;
}; // bool Clause::DefinitionUnfoldingSelected(const DefinedHeaderTable* definedHeaderTable) const 

inline void Clause::InsertAfter(Clause* c,Clause* db)
{
  Clause* next_cl = db->next;
  c->next = next_cl;
  next_cl->previous = c;
  db->next = c;
  c->previous = db;
}  // Clause::InsertAfter()


inline void Clause::InsertBefore(Clause* c,Clause* db)
{
  Clause* previous_cl = db->previous;
  previous_cl->next = c;
  c->previous = previous_cl;
  c->next = db;
  db->previous = c;
}  // Clause::InsertBefore()



inline void Clause::Remove(Clause* c)
{
  Clause* previous_cl = c->previous;
  Clause* next_cl = c->next;

  previous_cl->next = next_cl;
  next_cl->previous = previous_cl;
};  // Clause::Remove()

inline Clause* Clause::MakeEmptyClause()   
{
  return new Clause(new (0UL) LiteralList(0UL,0UL));
};

inline bool Clause::IsRule() const
{
  CALL("IsRule() const");
  // Returns true if and only if 
  // the clause contains both positive
  // and negative non-splitting non-answer literals.

  ASSERT(!Empty());
  if (Unit()) return false;
  LiteralList::Iterator iter(LitList());
  while (iter.notEnd() && 
	 (iter.currentLiteral()->IsSplitting()) ||
	 iter.currentLiteral()->isAnswerLiteral())
    {
      iter.next();
    };
  if (iter.end()) return false; // only splitting or answer literals 
 
  TERM::Polarity firstLitPol = iter.currentLiteral()->polarity();
  iter.next();
  while (iter.notEnd())
    {
      if ((!iter.currentLiteral()->IsSplitting()) 
	  && (!iter.currentLiteral()->isAnswerLiteral()) 
	  && (iter.currentLiteral()->polarity() != firstLitPol)) 
	return true; 
      iter.next();   
    };
  return false; // same polarity everywhere 
}; // bool Clause::IsRule() const




#ifndef NO_DEBUG 

inline
bool Clause::checkObj() const
{
#ifndef DEBUG_ALLOC
  return true;
#else 
  BK::ObjDesc* objDesc = BK::GlobAlloc::hostObj(static_cast<const void*>(this));
  if (!objDesc) 
    { 
      DMSG("Clause::checkObj()> Null pointer to object descriptor.\n");  
      return false; 
    }; 
  bool res = true;   
#ifdef DEBUG_ALLOC_OBJ_SIZE
  if (objDesc->size !=  sizeof(Clause)) 
    {
      DMSG("Clause::checkObj()> objDesc->size !=  sizeof(Clause)\n");
      res = false;
    };
#endif
#ifdef DEBUG_ALLOC_OBJ_STATUS
  if (objDesc->status != BK::ObjDesc::Persistent) 
    {
      DMSG("Clause::checkObj()> objDesc->status != ObjDesc::Persistent\n");
      DMSG("Clause::checkObj()> clause # "); DMSG(number); DMSG('\n');  
      res = false;
    };
#endif
#ifdef DEBUG_ALLOC_OBJ_TYPE
  if (objDesc->classDesc != &_classDesc) 
    {
      DMSG("Clause::checkObj()> objDesc->classDesc != &_classDesc\n");
      res = false;
    };
#endif
  if (!res) DMSG("Clause::checkObj()> host object ") << *objDesc << "\n"; 
  return res;
#endif
}; // bool Clause::checkObj() const
#endif  

static Clause::InitStatic clauseInitStatic;


}; // namespace VK


//===========================================================================
#endif
