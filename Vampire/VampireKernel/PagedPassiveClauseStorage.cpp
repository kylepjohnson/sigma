//
// File:         PagedPassiveClauseStorage.cpp
// Description:  Various signature related operations.  
// Created:      May 02, 2001, 13:20
// Author:       Alexandre Riazanov
// mail:         riazanov@cs.man.ac.uk
//====================================================
#include "PagedPassiveClauseStorage.hpp"
using namespace BK;
using namespace VK;
//=================================================
#undef DEBUG_NAMESPACE
#ifdef DEBUG_PAGED_PASSIVE_CLAUSE_STORAGE
 #define DEBUG_NAMESPACE "PagedPassiveClauseStorage"
#endif 
#include "debugMacros.hpp"
//=================================================  


PagedPassiveClauseStorage::PagedPassiveClauseStorage() 
{
  CALL("constructor PagedPassiveClauseStorage()");
  _ageWeightRatio = 0;
  _maximalPriorityFlag = false;
  _minNonemptyCatNum = MaximalCategoryNumber + 1; 
  _maxNonemptyCatNum = -1;
  _toBeSelectedByWeight = 0;
  _totalNumberOfClauses = 0;
  _numberOfSelectedClauses = 0;  
  _numberOfCancelledClauses = 0; 
  for (ulong cat = 0; cat <= MaximalCategoryNumber; cat++)
    {
      _category[cat].init(&_freePages);
    };
  
#ifdef DEBUG_NAMESPACE
  _debugClauseOpenForWriting = false; 
#endif  
};  


PagedPassiveClauseStorage::~PagedPassiveClauseStorage() 
{
  CALL("destructor ~PagedPassiveClauseStorage()");
}; // PagedPassiveClauseStorage::~PagedPassiveClauseStorage()
  
void PagedPassiveClauseStorage::init()
{
  CALL("init()");      
  _ageWeightRatio = 0;
  _maximalPriorityFlag = false;
  _minNonemptyCatNum = MaximalCategoryNumber + 1; 
  _maxNonemptyCatNum = -1;
  _toBeSelectedByWeight = 0;
  _totalNumberOfClauses = 0;
  _numberOfSelectedClauses = 0;  
  _numberOfCancelledClauses = 0; 

  _freePages.init();

  for (ulong cat = 0; cat <= MaximalCategoryNumber; cat++)
    {
      _category[cat].init(&_freePages);
    };
  
#ifdef DEBUG_NAMESPACE
  _debugClauseOpenForWriting = false; 
#endif  
}; // void PagedPassiveClauseStorage::init()

void PagedPassiveClauseStorage::destroy() 
{
  CALL("destroy()");
  for (ulong cat = 0; cat <= MaximalCategoryNumber; cat++)
    {
      _category[cat].destroy();
    };
  _freePages.destroy();
}; // void PagedPassiveClauseStorage::destroy() 



void  PagedPassiveClauseStorage::openClause(StoredClauseHandler& handler,long clNum,long clSize,long clWeight,const BitWord rules,ulong numOfLits,ulong numOfAncestors) 
{
  CALL("openClause(StoredClauseHandler& handler,long clNum,long clSize,long clWeight,const BitWord rules,ulong numOfLits,ulong numOfAncestors)");
  ASSERT(empty() || (_category[_minNonemptyCatNum].nonempty()));
  ASSERT(empty() || (_category[_maxNonemptyCatNum].nonempty()));

  long catNum = (_maximalPriorityFlag || (!_ageWeightRatio)) ? 0L : categoryNum(clWeight);
  ASSERT(catNum >= 0L);
  ASSERT(catNum <= (long)MaximalCategoryNumber);
  _currentCategory = _category + catNum;
  ulong totalSize = sizeInWords(sizeof(ClauseProfile)) 
    + sizeInWords((clSize + 2*numOfLits)*sizeof(TERM)) 
    + numOfAncestors;

  _cursor = static_cast<void**>(_currentCategory->tryToAllocate(totalSize));
  if (!_cursor) 
    {
      handler._memory = 0; 
      return;
    };
  _cursor = static_cast<void**>(Storage::objectContent(static_cast<void*>(_cursor)));;   
  handler._memory = static_cast<void*>(_cursor); 
  DOP(_debugEndOfCurrentClause = _cursor + totalSize);
  DOP(_debugNumberOfLiteralsToWrite = numOfLits);
  DOP(_debugNumberOfAncestorsToWrite = numOfAncestors);
  DOP(_debugClauseOpenForWriting = true);
   
  (static_cast<ClauseProfile*>(static_cast<void*>(_cursor)))->_clauseNumber = clNum;
  (static_cast<ClauseProfile*>(static_cast<void*>(_cursor)))->_clauseSize = clSize;
  (static_cast<ClauseProfile*>(static_cast<void*>(_cursor)))->_clauseWeight = clWeight;
  (static_cast<ClauseProfile*>(static_cast<void*>(_cursor)))->_rules = rules;
  (static_cast<ClauseProfile*>(static_cast<void*>(_cursor)))->_numOfLiterals = numOfLits;
  (static_cast<ClauseProfile*>(static_cast<void*>(_cursor)))->_numOfAncestors = numOfAncestors;
  _cursor += sizeInWords(sizeof(ClauseProfile));
   
  if (empty()) 
    {
      _minNonemptyCatNum = catNum;
      _maxNonemptyCatNum = catNum; 
    }
  else
    {
      if (_minNonemptyCatNum > catNum) _minNonemptyCatNum = catNum;
      if (_maxNonemptyCatNum < catNum) _maxNonemptyCatNum = catNum;
    };

  ASSERT(!empty());
  ASSERT(_category[_minNonemptyCatNum].nonempty());
  ASSERT(_category[_maxNonemptyCatNum].nonempty());

  _totalNumberOfClauses++; 
}; //void  PagedPassiveClauseStorage::openClause(StoredClauseHandler& handler,long clNum,long clSize,long clWeight,const BitWord rules,ulong numOfLits,ulong numOfAncestors)  


bool PagedPassiveClauseStorage::select(StoredClauseHandler& selectedClauseHandler) 
{
  CALL("select(ClauseHandler& selectedClauseHandler)");
  if (empty()) return false;

  if (_minNonemptyCatNum == 0L) 
    {
      _selectedCategoryNumber = 0L;
      selectFrom(_category + _selectedCategoryNumber,selectedClauseHandler);
      _lastSelectedByWeight = false;
      return true;
    };

  ASSERT(_ageWeightRatio || (!_toBeSelectedByWeight));
  if (!_toBeSelectedByWeight)
    {      
      // select by age
      long oldestCatNum = _minNonemptyCatNum;
      ASSERT(_category[_minNonemptyCatNum].nonempty());
      long oldestClauseNum = (static_cast<ClauseProfile*>(Storage::objectContent(_category[_minNonemptyCatNum].head())))->clauseNumber();
      for (long catNum = _minNonemptyCatNum + 1; catNum <= _maxNonemptyCatNum; catNum++)
	{
	  if (_category[catNum].nonempty())
	    {
	      if ((static_cast<ClauseProfile*>(Storage::objectContent(_category[catNum].head())))->clauseNumber() < oldestClauseNum)
		{
		  oldestCatNum = catNum;
		  oldestClauseNum = (static_cast<ClauseProfile*>(Storage::objectContent(_category[catNum].head())))->clauseNumber();     
		};
	    };
	};
      // now we know the oldest category 
      _selectedCategoryNumber =  oldestCatNum;   
      selectFrom(_category + _selectedCategoryNumber,selectedClauseHandler);
      _lastSelectedByWeight = false;
    }   
  else 
    {
      // select by weight 
      _selectedCategoryNumber = _minNonemptyCatNum;
      selectFrom(_category + _selectedCategoryNumber,selectedClauseHandler);
      _lastSelectedByWeight = true;
    };
  ASSERT((bool)selectedClauseHandler);
  return true;  
}; // bool PagedPassiveClauseStorage::select(ClauseHandler& selectedClauseHandler)   

bool PagedPassiveClauseStorage::select(StoredClause& selectedClause)
{
  CALL("select(StoredClause& selectedClause)");
  StoredClauseHandler selectedClauseHandler; 
  if (select(selectedClauseHandler))
    {
      loadClause(selectedClauseHandler,selectedClause); 
      return true;
    };
  return false;
}; // bool PagedPassiveClauseStorage::select(StoredClause& selectedClause)

void PagedPassiveClauseStorage::popSelected()
{
  CALL("popSelected()");
  ASSERT(_category[_selectedCategoryNumber].nonempty()); 
  _category[_selectedCategoryNumber].pop();

  // this category might be empty now 
  
  if (_category[_selectedCategoryNumber].empty())
    {
      if (_selectedCategoryNumber == _minNonemptyCatNum)
	{
	  do 
	    {
	      _minNonemptyCatNum++;     
	    }
	  while ((_minNonemptyCatNum <= _maxNonemptyCatNum) 
		 && (_category[_minNonemptyCatNum].empty()));
	}
      else
	if (_selectedCategoryNumber == _maxNonemptyCatNum)
	  {
	    do
	      {
		_maxNonemptyCatNum--;
	      }
	    while ((_maxNonemptyCatNum >= _minNonemptyCatNum) 
		   && (_category[_maxNonemptyCatNum].empty()));
	  };
    };

  ASSERT(empty() || (_category[_minNonemptyCatNum].nonempty()));
  ASSERT(empty() || (_category[_maxNonemptyCatNum].nonempty()));

  // statistics
  _numberOfSelectedClauses++;
  if (_lastSelectedByWeight) 
    {
      ASSERT(_toBeSelectedByWeight > 0);
      _toBeSelectedByWeight--; 
      ASSERT(_toBeSelectedByWeight >= 0);
    }
  else
    {
      ASSERT(!_toBeSelectedByWeight || (_selectedCategoryNumber == 0L));
      _toBeSelectedByWeight = _ageWeightRatio;
    }; 
}; // void PagedPassiveClauseStorage::popSelected()
       
// Cancelling clauses at the end of the queue and recycling pages. 

const PagedPassiveClauseStorage::ClauseProfile* PagedPassiveClauseStorage::worstClauseProfile() const
{
  CALL("worstClauseProfile() const");
  if (empty()) return 0;
  const Storage* cat = _category + _maxNonemptyCatNum; 
  ASSERT(cat->nonempty());   
  ASSERT(cat->endOfQueue());
  return static_cast<const ClauseProfile*>(Storage::objectContent(cat->endOfQueue()));
}; // const ClauseProfile* PagedPassiveClauseStorage::worstClauseProfile() const
   
/****
bool PagedPassiveClauseStorage::worstClause(ClauseProfile& prof)
{
  CALL("worstClause(ClauseProfile& prof)");   
  if (empty()) return false;  
  Storage* cat = _category + _maxNonemptyCatNum; 
  ASSERT(cat->nonempty());   
  ASSERT(cat->endOfQueue());
  ClauseProfile* clause = static_cast<ClauseProfile*>(Storage::objectContent(cat->endOfQueue()));
  ASSERT(clause);   
  prof._clauseNumber = clause->_clauseNumber;
  prof._clauseSize = clause->_clauseSize;
  prof._rules = clause->_rules;
  prof._numOfLiterals = clause->_numOfLiterals;
  prof._numOfAncestors = clause->_numOfAncestors;
  return true;     
}; // bool PagedPassiveClauseStorage::worstClause(ClauseProfile& prof)
******/ 

bool PagedPassiveClauseStorage::cancelWorstClause()
{
  CALL("cancelWorstClause()");
  if (empty()) return false;
  Storage* cat = _category + _maxNonemptyCatNum; 
  ASSERT(cat->nonempty()); 
  cat->cancelLastObject();
  if (cat->empty())
    {
      do
	{
	  _maxNonemptyCatNum--;
	}
      while ((_maxNonemptyCatNum >= _minNonemptyCatNum) 
	     && (_category[_maxNonemptyCatNum].empty()));
    };
  _numberOfCancelledClauses++;
  return true;
}; // bool PagedPassiveClauseStorage::cancelWorstClause()


void PagedPassiveClauseStorage::readFromMemory(const void* memory,ClauseProfile& prof,ClauseLiterals& lits,ClauseAncestors& ancestors)
{
  CALL("readFromMemory(const void* memory,ClauseProfile& prof,ClauseLiterals& lits,ClauseAncestors& ancestors)"); 
  ASSERT(memory);
  const ClauseProfile* profile = static_cast<const ClauseProfile*>(memory);
  prof._clauseNumber = profile->_clauseNumber;
  prof._clauseSize = profile->_clauseSize;
  prof._rules = profile->_rules;
  prof._numOfLiterals = profile->_numOfLiterals;
  prof._numOfAncestors = profile->_numOfAncestors;
  lits._numberOfLiterals = profile->_numOfLiterals;
  ancestors._numberOfAncestors = profile->_numOfAncestors;
  const TERM* storedLiterals = static_cast<const TERM*>(static_cast<const void*>(profile + 1));
  if (profile->_numOfLiterals)
    {
      lits._beginning = storedLiterals;
      // skip all the literals to get to the ancestors
      ulong numOfLitsToSkip = profile->_numOfLiterals; 
      while (numOfLitsToSkip)
	{
	  storedLiterals += (storedLiterals->Content() + 2);
	  numOfLitsToSkip--;
	};
    };
  ancestors._beginning = static_cast<Clause**>(const_cast<void*>(static_cast<const void*>(storedLiterals)));
  lits.reset();
  ancestors.reset();
}; // void PagedPassiveClauseStorage::readFromMemory(const void* memory,ClauseProfile& prof,ClauseLiterals& lits,ClauseAncestors& ancestors)

ostream& PagedPassiveClauseStorage::StoredClause::output(ostream& str)
{
  CALL("StoredClause::output(ostream& str)"); 
  str << "@ " << clauseNumber() << ". "; 

  if (!numOfLiterals()) { str << '#'; } 
  else // output literals
    {
      resetLiterals();
      TERM symbol;
      static TERM literal[2*VampireKernelConst::MaxTermSize];
      ALWAYS(nextLiteral());
      TERM::OpenCompact openLiteral(literal);
 
      while (nextSymbol(symbol)) 
	{ 
	  openLiteral.push(symbol);
	}; 
  
      str << literal; 
      while (nextLiteral())
	{ 
	  openLiteral.reset(literal);
     
	  while (nextSymbol(symbol)) 
	    {
	      openLiteral.push(symbol);
	    }; 

	  str << " \\/ " << literal;
	};  

    }; 

  str << ' ' << clauseSize() << "(0) ?pe";

  // now output clause background
  str << ((rules().bit(ClauseBackground::VIP)) ? "vip " : "    ") << '[';
  ClauseBackground::outputRules(str,rules());
  resetAncestors();
  Clause* ancestor;
  if (nextAncestor(ancestor))
    {
      str << ancestor->Number();
      while (nextAncestor(ancestor)) str << ',' << ancestor->Number(); 
    };
  str << "]";
  return str;
}; // ostream& PagedPassiveClauseStorage::StoredClause::output(ostream& str)  

ostream& PagedPassiveClauseStorage::ClauseProfile::output(ostream& str) const
{
  CALL("ClauseProfile::output(ostream& str) const");
  str << clauseNumber() << ". "; 
  str << ' ' << clauseSize() << "(0) ?pe";
  // now output clause background
  str << ((rules().bit(ClauseBackground::VIP)) ? "vip " : "    ") << '[';
  ClauseBackground::outputRules(str,rules());
  str << ']';
  return str;  
}; // ostream& PagedPassiveClauseStorage::ClauseProfile::output(ostream& str) const


//====================================================




