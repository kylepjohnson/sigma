//
// File:         PagedPassiveClauseStorage.hpp
// Description:  Various signature related operations.  
// Created:      Apr 27, 2001, Alexandre Riazanov, riazanov@cs.man.ac.uk
// Revised:      May 21, 2002, Alexandre Riazanov, riazanov@cs.man.ac.uk
//               Bug fix in categoryNum(long clauseWeight). 
//====================================================
#ifndef PAGED_PASSIVE_CLAUSE_STORAGE_H
//=================================================
#define PAGED_PASSIVE_CLAUSE_STORAGE_H
#include <iostream>
#include "jargon.hpp"
#include "VampireKernelDebugFlags.hpp"
#include "VampireKernelConst.hpp"
#include "GlobAlloc.hpp"
#include "BitWord.hpp"
#include "PageQueueDisciplineStorage.hpp"
#include "Term.hpp"
#include "ClauseBackground.hpp"
#include "Clause.hpp"
//=================================================
#undef DEBUG_NAMESPACE
#ifdef DEBUG_PAGED_PASSIVE_CLAUSE_STORAGE
 #define DEBUG_NAMESPACE "PagedPassiveClauseStorage"
#endif 
#include "debugMacros.hpp"
//=================================================  

namespace VK
{
class PagedPassiveClauseStorage
{
 public: 
  class ClauseProfile
    {
    public:
      ClauseProfile() {};
      ~ClauseProfile() {};
      long clauseNumber() const { return _clauseNumber; };
      long clauseSize() const { return _clauseSize; };
      long clauseWeight() const { return _clauseWeight; };
      const BK::BitWord& rules() const { return _rules; };
      ulong numOfLiterals() const { return _numOfLiterals; };
      ulong numOfAncestors() const { return _numOfAncestors; };
      ostream& output(ostream& str) const;
    private:
      long _clauseNumber;
      long _clauseSize;
      long _clauseWeight;
      BK::BitWord _rules;
      ulong _numOfLiterals;
      ulong _numOfAncestors;
      friend class PagedPassiveClauseStorage;
    }; // class ClauseProfile
  
  class ClauseLiterals
    {
    public:
      ClauseLiterals() {};
      ~ClauseLiterals() {};
      bool nextLiteral() 
	{
	  CALL("ClauseLiterals::nextLiteral()");
	  ASSERT(!_numOfRemainingSymbols);
	  if (_numOfRemainingLiterals)
	    {
	      _numOfRemainingLiterals--;
	      _numOfRemainingSymbols = _cursor->Content();
	      _cursor++;
	      _currentLiteralIsInherentlySelected = 
			  (_cursor->Content() != 0UL);
	      _cursor++;
	      return true;
	    }
	  else return false;   
	};
      bool nextSymbol(TERM& sym) 
	{
	  CALL("ClauseLiterals::nextSymbol(TERM& sym)");
	  if (_numOfRemainingSymbols) 
	    {
	      _numOfRemainingSymbols--;
	      sym = *_cursor;
	      _cursor++;
	      return true;
	    }
	  else return false;
	};
      bool currentLiteralIsInherentlySelected() const
	{
	  CALL("currentLiteralIsInherentlySelected() const");
	  return _currentLiteralIsInherentlySelected;
	};
      void reset() 
	{
	  CALL("ClauseLiterals::reset()");    
	  _cursor = _beginning;
	  _numOfRemainingLiterals = _numberOfLiterals;
	  _numOfRemainingSymbols = 0;
	};
    
    private:
      const TERM* _beginning;
      ulong _numberOfLiterals;
      const TERM* _cursor;
      ulong _numOfRemainingLiterals;
      ulong _numOfRemainingSymbols;
      bool _currentLiteralIsInherentlySelected;
      friend class PagedPassiveClauseStorage;
    }; // class ClauseLiterals

  class ClauseAncestors
    {
    public:
      ClauseAncestors() {};
      ~ClauseAncestors() {};
      bool nextAncestor(Clause*& anc) 
	{
	  CALL("ClauseAncestors::nextAncestor(Clause*& anc)");
	  if (_numOfRemainingAncestors)
	    {
	      _numOfRemainingAncestors--;
	      anc = *_cursor;
	      _cursor++;
	      return true;
	    }
	  else return false;
	};
      void reset()
	{
	  CALL("ClauseAncestors::reset()");    
	  _cursor = _beginning;
	  _numOfRemainingAncestors = _numberOfAncestors;
	};
    private:
      Clause** _beginning;
      ulong _numberOfAncestors;
      Clause** _cursor;
      ulong _numOfRemainingAncestors;
      friend class PagedPassiveClauseStorage;
    }; // class ClauseAncestors

  class StoredClause
    {
    public:   
      StoredClause() {};
      ~StoredClause() {};
      ClauseProfile& profile() { return _profile; };
      ClauseLiterals& literals() { return _literals; };
      ClauseAncestors& ancestors() { return _ancestors; };

      long clauseNumber() const { return _profile.clauseNumber();  };
      long clauseSize() const { return _profile.clauseSize();  };
      const BK::BitWord& rules() const { return _profile.rules();  };
      ulong numOfLiterals() const { return  _profile.numOfLiterals(); };
      ulong numOfAncestors() const { return  _profile.numOfAncestors(); };   

      void resetLiterals() { _literals.reset(); };
      bool nextLiteral() { return _literals.nextLiteral(); };
      bool nextSymbol(TERM& sym) { return _literals.nextSymbol(sym); };

      void resetAncestors() { _ancestors.reset(); };
      bool nextAncestor(Clause*& anc) { return _ancestors.nextAncestor(anc); };

      bool isVIP() const { return rules().bit(ClauseBackground::VIP); };    
      bool isOrphan() 
	{
	  CALL("StoredClause::isOrphan()");
	  resetAncestors();
	  Clause* ancestor;
	  while (nextAncestor(ancestor)) 
	    if (!ancestor->Active()) return true;
	  return false;  
	};
      ostream& output(ostream& str);    
    
    private: 
      ClauseProfile _profile;
      ClauseLiterals _literals;
      ClauseAncestors _ancestors;
      friend class PagedPassiveClauseStorage;
    }; // class StoredClause

  class StoredClauseHandler
    {
    public:
      StoredClauseHandler() {};
      ~StoredClauseHandler() {};
      operator bool() const { return _memory != 0; };
    private:
      void* _memory;
      friend class PagedPassiveClauseStorage;
    }; // class StoredClauseHandler 

  class Iterator
    {
    public:
      Iterator() {};
      ~Iterator() {};
      void reset(PagedPassiveClauseStorage& storage)
	{
	  CALL("Iterator::reset(PagedPassiveClauseStorage& storage)");
	  _storage = &storage;
	  _nextCategoryNumber = _storage->_minNonemptyCatNum + 1;
	  _nextObject = _storage->_category[_storage->_minNonemptyCatNum].head();
	};  
 
      bool nextClause() 
	{
	  CALL("Iterator::nextClause()");
	  while (!_nextObject)
	    {
	      // try next category
	      if (_nextCategoryNumber > _storage->_maxNonemptyCatNum) return false;      
	      _nextObject = _storage->_category[_nextCategoryNumber].head();
	      _nextCategoryNumber++; 
	    };    

	  PagedPassiveClauseStorage::readFromMemory(PagedPassiveClauseStorage::Storage::objectContent(_nextObject),
						    _currentClause.profile(),
						    _currentClause.literals(),
						    _currentClause.ancestors());
	  _nextObject = Storage::nextObject(_nextObject);  
	  return true;
	};
      StoredClause& currentClause() { return _currentClause; };   
    private:
      PagedPassiveClauseStorage* _storage;
      StoredClause _currentClause;
      long _nextCategoryNumber; 
      void* _nextObject; 
    }; // class Iterator  


  class ConstIterator
    {
    public:
      ConstIterator() {};
      ~ConstIterator() {};
      void reset(const PagedPassiveClauseStorage& storage)
	{
	  CALL("ConstIterator::reset(const PagedPassiveClauseStorage& storage)");
	  _storage = &storage;
	  _nextCategoryNumber = _storage->_minNonemptyCatNum + 1;
	  _nextObject = _storage->_category[_storage->_minNonemptyCatNum].head();
	};  
 
      bool nextClause() 
	{
	  CALL("ConstIterator::nextClause()");
	  while (!_nextObject)
	    {
	      // try next category
	      if (_nextCategoryNumber > _storage->_maxNonemptyCatNum) return false;      
	      _nextObject = _storage->_category[_nextCategoryNumber].head();
	      _nextCategoryNumber++; 
	    };    

	  PagedPassiveClauseStorage::readFromMemory(PagedPassiveClauseStorage::Storage::objectContent(_nextObject),
						    _currentClause.profile(),
						    _currentClause.literals(),
						    _currentClause.ancestors());
	  _nextObject = Storage::nextObject(_nextObject);  
	  return true;
	};
      const StoredClause& currentClause() { return _currentClause; };   
    private:
      const PagedPassiveClauseStorage* _storage;
      StoredClause _currentClause;
      long _nextCategoryNumber; 
      const void* _nextObject; 
    }; // class ConstIterator  
 

 public:
  PagedPassiveClauseStorage();
  ~PagedPassiveClauseStorage();
  void init();
  void destroy();
  void reset() { CALL("reset()"); destroy(); init(); }; 
  
  static ulong pageSize() { return PageSize*sizeof(void*); }; 

  void setAgeWeightRatio(long awr) 
    { 
      CALL("setAgeWeightRatio(long awr)");
      ASSERT(awr >= 0);
      _ageWeightRatio = awr;
      _toBeSelectedByWeight = awr; 
    };
  void maximalPriorityOn() { _maximalPriorityFlag = true; };
  void maximalPriorityOff() { _maximalPriorityFlag = false; };

  // Properties 
 
  bool empty() const { return _maxNonemptyCatNum < _minNonemptyCatNum; };
  bool nonempty() const { return _maxNonemptyCatNum >= _minNonemptyCatNum; };
  long totalNumberOfClauses() const { return _totalNumberOfClauses; };
  long numberOfSelectedClauses() const { return _numberOfSelectedClauses; };
  long numberOfCancelledClauses() const { return _numberOfCancelledClauses; };
  long numberOfCurrentlyKeptClauses() const 
    { 
      return _totalNumberOfClauses - _numberOfSelectedClauses - _numberOfCancelledClauses;
    };

  // Writing new clauses

  void  openClause(StoredClauseHandler& handler,long clNum,long clSize,
                   long clWeight,const BK::BitWord rules,ulong numOfLits,
                   ulong numOfAncestors); 

  bool openClause(long clNum,long clSize,long clWeight,
                  const BK::BitWord rules,ulong numOfLits,ulong numOfAncestors)
    {
      CALL("openClause(long clNum,long clSize,long clWeight,const BK::BitWord rules,ulong numOfLits,ulong numOfAncestors)");
      StoredClauseHandler handler;
      openClause(handler,clNum,clSize,rules,numOfLits,numOfAncestors);
      return (bool)handler;
    }; // bool openClause(long clNum,long clSize,long clWeight,const BK::BitWord rules,ulong numOfLits,ulong numOfAncestors)
 
  void openLiteral(bool inherentlySelected)
    {
      CALL("openLiteral(bool inherentlySelected)");
      ASSERT(_debugClauseOpenForWriting); 
      ASSERT(_cursor + sizeInWords(sizeof(TERM)) <= _debugEndOfCurrentClause);
      ASSERT(_debugNumberOfLiteralsToWrite);
      _currentLiteral = (TERM*)_cursor;
      _currentLiteralSize = 0UL;
      _cursor += sizeInWords(sizeof(TERM));
      ((TERM*)_cursor)->Make((ulong)inherentlySelected);
      _cursor += sizeInWords(sizeof(TERM));
    }; // void openLiteral(bool inherentlySelected)

  void writeSymbol(const TERM& sym) 
    {
      CALL("writeSymbol(const TERM& sym)");
      ASSERT(_debugNumberOfLiteralsToWrite);
      ASSERT(_cursor + sizeInWords(sizeof(TERM)) <= _debugEndOfCurrentClause);
      *((TERM*)_cursor) = sym;
      _cursor += sizeInWords(sizeof(TERM));
      _currentLiteralSize++;
    }; // void writeSymbol(const TERM& sym) 
  
  void closeLiteral()
    { 
      CALL("closeLiteral()");
      ASSERT(_debugNumberOfLiteralsToWrite);
      DOP(_debugNumberOfLiteralsToWrite--);
      _currentLiteral->Make(_currentLiteralSize);       
    }; // void closeLiteral()

  void writeAncestor(const Clause* ancestor)
    {
      CALL("writeAncestor(const Clause* ancestor)");
      ASSERT(_debugClauseOpenForWriting); 
      ASSERT(!_debugNumberOfLiteralsToWrite);
      ASSERT(_debugNumberOfAncestorsToWrite);
      ASSERT(_cursor + 1 <= _debugEndOfCurrentClause);
      *_cursor = (void*)ancestor;     
      _cursor++;
      DOP(_debugNumberOfAncestorsToWrite--);
    }; // void writeAncestor(const Clause* ancestor)

  void endOfClause()
    {
      CALL("endOfClause()");
      ASSERT(_debugClauseOpenForWriting);  
      DOP(_debugClauseOpenForWriting = false);
      ASSERT(!_debugNumberOfLiteralsToWrite);
      ASSERT(!_debugNumberOfAncestorsToWrite);
      ASSERT(!empty()); 
      ASSERT(_category[_minNonemptyCatNum].nonempty());
      ASSERT(_category[_maxNonemptyCatNum].nonempty());
    }; // void endOfClause()


  // Selection and popping the selected clause. 
  bool select(StoredClauseHandler& selectedClauseHandler);  

  bool select(StoredClause& selectedClause);

  void popSelected();
       
  // Cancelling clauses at the end of the queue and recycling pages. 

  const ClauseProfile* worstClauseProfile() const;
  ///bool worstClause(ClauseProfile& prof);
  bool cancelWorstClause();

  bool recyclePage()
    {
      CALL("recyclePage()");
      return _freePages.recyclePage();
    }; // bool recyclePage()

  static void loadClause(const StoredClauseHandler& handler,StoredClause& clause)
    {
      CALL("loadClause(const StoredClauseHandler& handler,StoredClause& clause)");
      ASSERT((bool)handler);
      readFromMemory(handler._memory,clause.profile(),clause.literals(),clause.ancestors());
    }; // void loadClause(const StoredClauseHandler& handler,StoredClause& clause)



 private: 
	 enum
	 {
       PageSize = 2048UL // in machine words
	 };
  // make shure that PageSize qualifies for large object in the allocator
  typedef BK::PageQueueDisciplineStorage<BK::GlobAlloc,PageSize,PagedPassiveClauseStorage> Storage;
 private:
  long categoryNum(long clauseWeight) 
    {
      if (clauseWeight > (long)MaximalCategoryNumber) return MaximalCategoryNumber;
      if (clauseWeight < 0L) return 0L;
      return clauseWeight;
    };
  static ulong sizeInWords(ulong sizeInBytes) 
    {
      ulong res = sizeInBytes/sizeof(void*);
      if (sizeInBytes % sizeof(void*)) res++;
      return res;
    }; // ulong sizeInWords(ulong sizeInBytes)  

  static void selectFrom(Storage* cat,StoredClauseHandler& selectedClauseHandler)
    {
      CALL("selectFrom(Storage* cat,StoredClauseHandler& selectedClauseHandler)");
      ASSERT(cat);
      ASSERT(cat->nonempty());      
      selectedClauseHandler._memory = Storage::objectContent(cat->head());
    }; // void selectFrom(Storage* cat,StoredClauseHandler& selectedClauseHandler)
 
  static void readFromMemory(const void* memory,ClauseProfile& prof,ClauseLiterals& lits,ClauseAncestors& ancestors);

 private:
  enum 
  {
	  MaximalCategoryNumber = VampireKernelConst::MaxClauseSize
  };
 private:
  // parameters 
  long _ageWeightRatio;
  bool _maximalPriorityFlag; 

  // statistics
  long _minNonemptyCatNum; 
  long _maxNonemptyCatNum;
  long _toBeSelectedByWeight;

  long _totalNumberOfClauses;
  long _numberOfSelectedClauses;  
  long _numberOfCancelledClauses; 

  // storage
  Storage::FreePages _freePages;
  Storage _category[MaximalCategoryNumber + 1];

  Storage* _currentCategory;
  long _selectedCategoryNumber;
  bool _lastSelectedByWeight;
  void** _cursor;
  TERM* _currentLiteral;
  ulong _currentLiteralSize;
#ifdef DEBUG_NAMESPACE
  bool _debugClauseOpenForWriting; 
  void** _debugEndOfCurrentClause;
  ulong _debugNumberOfLiteralsToWrite;
  ulong _debugNumberOfAncestorsToWrite;
#endif  
  friend class Iterator;
  friend class ConstIterator;
}; // class PagedPassiveClauseStorage

}; // namespace VK
//====================================================
#endif
