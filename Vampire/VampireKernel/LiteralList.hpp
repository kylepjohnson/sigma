//
// File:         LiteralList.hpp
// Description:  Representation of lists of stored marked literals.
// Created:      Dec 09, 2000, 16:20
// Author:       Alexandre Riazanov
// mail:         riazanov@cs.man.ac.uk
// Revised:      Dec 11, 2001.
//               LiteralList::Iterator::{litNum(),_litNum} added.
//====================================================
#ifndef LITERAL_LIST_H
//====================================================
#define LITERAL_LIST_H
#include <iostream>
#include "jargon.hpp"
#include "VampireKernelDebugFlags.hpp"
#ifdef DEBUG_ALLOC_OBJ_TYPE
#  include "ClassDesc.hpp"       
#endif
#include "VampireKernelConst.hpp"
#include "GlobAlloc.hpp"
#include "DestructionMode.hpp"
#include "Term.hpp"
using namespace std;
//===================================================
#undef DEBUG_NAMESPACE
#ifdef DEBUG_LITERAL_LIST
#  define DEBUG_NAMESPACE "LiteralList"
#endif 
#include "debugMacros.hpp"
//=================================================== 
namespace VK
{

  class LiteralList
  {
  public:
    enum Mark
    {
      Clean              = 0x00000000, 
      InherentlySelected = 0x00000001, 
      Reserved10         = 0x00000002, 
      Reserved11         = 0x00000003
    };
    class Element
    {
    public: 
      Mark mark() const 
      {
	CALL("mark() const");
	return 
	  static_cast<Mark>((reinterpret_cast<ulong>(_content) << (sizeof(TERM*)*CHAR_BIT - 2)) >> (sizeof(TERM*)*CHAR_BIT - 2));
      };

      bool isInherentlySelected() const 
      {
	CALL("isInherentlySelected() const"); 
	return mark() == InherentlySelected; 
      };

      void setMark(Mark m) 
      {
	CALL("Element::setMark(Mark m)");
	DOP(TERM* debugLit = literal());
	ASSERT((m >> 2) == 0UL);
	_content = 
	  reinterpret_cast<TERM*>(((reinterpret_cast<ulong>(_content) >> 2) << 2) | m);
	
	ASSERT(mark() == m); 
	ASSERT(literal() == debugLit);
      };

      TERM* literal() const
      {  
	CALL("Element::literal()");
	return 
	  reinterpret_cast<TERM*>((reinterpret_cast<ulong>(_content) >> 2) << 2);
      };
         
      TERM* cleanLiteral() const 
      {  
	// for those cases when we know that the literal is not marked
	CALL("Element::cleanLiteral() const");
	ASSERT(mark() == Clean);
	return _content;
      };
      TERM literalRef() const 
      {
	CALL("literalRef() const");
	return TERM(literal());
      }; 
      TERM cleanLiteralRef() const 
      {
	// for those cases when we know that the literal is not marked
	CALL("cleanLiteralRef() const");
	return TERM(_content);
      }; 
      void setLiteral(TERM* lit) 
      {
	CALL("setLiteral(TERM* lit)");
	_content = lit;
      };
      void setLiteral(TERM* lit,Mark m) 
      { 
	CALL("setLiteral(TERM* lit,Mark m)");
	_content = lit; 
	setMark(m);
	ASSERT(literal() == lit);
	ASSERT(mark() == m);
      };
      Element& operator=(const Element& el) 
      {
	_content = el._content; 
	return *this; 
      };
    
      Element* next() { return _next; };  
      const Element* next() const { return _next; };   


      // some useful shorthands 
      TERM::Arity arity() const { return literal()->arity(); };
      TERM::Polarity polarity() const { return literal()->polarity(); }; 
      bool positive() const { return literal()->Positive(); };
      bool negative() const { return literal()->Negative(); };
      TERM::Functor functor() const { return literal()->functor(); };
      ulong headerNum() const { return literal()->HeaderNum(); };
      bool isSymLit() const { return literal()->IsSymLitHeader(); };    
      bool isEquality() const { return literal()->IsEquality(); };     
      bool isOrderedEq() const { return literal()->IsOrderedEq(); };
      bool isUnorderedEq() const { return literal()->IsUnorderedEq(); };
      TERM* args() { return literal()->Args(); };
      const TERM* args() const { return literal()->Args(); }; 
      TERM* arg1() { return literal()->Arg1(); };
      const TERM* arg1() const { return literal()->Arg1(); };
      TERM* arg2() { return literal()->Arg2(); };
      const TERM* arg2() const { return literal()->Arg2(); };

    private:
      Element() {};
      Element(const Element&) {};
      Element(Element* nxt) { _next = nxt; };
      ~Element() {};
      void* operator new(size_t) 
      { 
#ifdef DEBUG_ALLOC_OBJ_TYPE
	ALWAYS(_classDesc.registerAllocated(sizeof(Element)));
	return BK::GlobAlloc::allocate(sizeof(Element),&_classDesc); 
#else
	return BK::GlobAlloc::allocate(sizeof(Element)); 
#endif 
      };
      void  operator delete(void* obj) 
      { 
	CALL("Element::operator delete(void* obj)");
#ifdef DEBUG_ALLOC_OBJ_TYPE
	ALWAYS(_classDesc.registerDeallocated(sizeof(Element)));
	BK::GlobAlloc::deallocate(obj,sizeof(Element),&_classDesc); 
#else
	BK::GlobAlloc::deallocate(obj,sizeof(Element)); 
#endif
      };
    private:
      TERM* _content;
      Element* _next;
#ifdef DEBUG_ALLOC_OBJ_TYPE 
      static BK::ClassDesc _classDesc;
#endif
      friend class LiteralList; 
    }; // class Element
  
    class Iterator;
    class ConstIterator;

  public:
    LiteralList(ulong numOfLits,ulong numOfSelectedLits)
    {
      CALL("constructor LiteralList(ulong numOfLits,ulong numOfSelectedLits)");
      ASSERT(numOfLits <= VampireKernelConst::MaxlNumOfLits);
      ASSERT(numOfSelectedLits <= numOfLits);
      ((_sizes = numOfLits) <<= 16) |= numOfSelectedLits;
      _elements = 0;
      for (ulong n = 0; n < numOfLits; n++)
	_elements = new Element(_elements);
   
    };
    ~LiteralList() 
    { 
      CALL("destructor ~LiteralList()");
      if (BK::DestructionMode::isThorough())
	{
	  while (_elements) 
	    { 
	      Element* tmp = _elements->next();
	      delete _elements;
	      _elements = tmp;  
	    };
	};
    };

    void* operator new(size_t,ulong numOfLits)
    {
#ifdef DEBUG_ALLOC_OBJ_TYPE
      ALWAYS(_classDesc.registerAllocated(sizeof(LiteralList)));
      return BK::GlobAlloc::allocate(sizeof(LiteralList),&_classDesc);
#else 
      return BK::GlobAlloc::allocate(sizeof(LiteralList)); 
#endif
    };

    void operator delete(void* obj)
    {
#ifdef DEBUG_ALLOC_OBJ_TYPE
      ALWAYS(_classDesc.registerDeallocated(sizeof(LiteralList)));
      BK::GlobAlloc::deallocate(obj,sizeof(LiteralList),&_classDesc);
#else
      BK::GlobAlloc::deallocate(obj,sizeof(LiteralList));
#endif
    };

    void operator delete(void* obj,ulong numOfLits)
    {
#ifdef DEBUG_ALLOC_OBJ_TYPE
      ALWAYS(_classDesc.registerDeallocated(sizeof(LiteralList)));
      BK::GlobAlloc::deallocate(obj,sizeof(LiteralList),&_classDesc);
#else
      BK::GlobAlloc::deallocate(obj,sizeof(LiteralList));
#endif
    };

  

    ulong length() const { return (_sizes >> 16); };
    ulong numOfSelectedLits() const { return ((_sizes << 16) >> 16); };
    void setNumOfSelected(ulong n) 
    { 
      CALL("setNumOfSelected(ulong n)");
      ASSERT(n < VampireKernelConst::MaxlNumOfLits);
      ((_sizes >>= 16) <<= 16) |= n;
    };
    Element* firstElem() 
    {
      return _elements; 
    };
    const Element* firstElem() const { return _elements; };
    TERM* firstLit() const 
    {
      CALL("firstLit() const");
      ASSERT((length()) && (length() <= VampireKernelConst::MaxlNumOfLits));
      return _elements->literal(); 
    };
  
    Element* nth(ulong n)
    {
      CALL("nth(ulong n)");
      ASSERT(n <= length());
      Element* res = _elements;
      while (n) 
	{
	  res = res->next();
	  n--;
	};
      return res;
    };

    bool containsOnlyAnswerLiterals() const
    {
      for (const Element* el = firstElem(); el; el = el->next())
	if (!el->literal()->isAnswerLiteral()) return false;
      return true;
    };

    ostream& output(ostream& str) const;
   
  private:
#ifdef DEBUG_ALLOC_OBJ_TYPE 
    static BK::ClassDesc _classDesc;
#endif
  private:
    LiteralList() { CALL("constructor LiteralList()"); ICP("0"); };
    

  private:
    ulong _sizes;  
    Element* _elements;
    friend class Iterator;
    friend class Element;
  }; // class LiteralList


  //===================================================
#undef DEBUG_NAMESPACE
#ifdef DEBUG_LITERAL_LIST
#  define DEBUG_NAMESPACE "LiteralList::Iterator"
#endif 
#include "debugMacros.hpp"
  //=================================================== 


  class LiteralList::Iterator
  {
  public:
    Iterator() : _current(0), _litNum(0UL)
    {
    };


    Iterator(LiteralList& lst) { reset(lst); };
    Iterator(LiteralList* lst) { reset(lst); };

    Iterator(LiteralList& lst,LiteralList::Element* start) { reset(lst,start); };
    Iterator(LiteralList* lst,LiteralList::Element* start) { reset(lst,start); };
    ~Iterator() {};

    void init()
    {
      _current = 0; 
      _litNum = 0UL;
    };
    void destroy() {};


    void reset(LiteralList& lst) 
    {
      _current = lst.firstElem();
      //_end = lst.endOfElements();
      _litNum = 0UL;
    };
    void reset(LiteralList* lst) 
    {
      _current = lst->firstElem();
      //_end = lst->endOfElements();
      _litNum = 0UL;
    };
    void reset(LiteralList& lst,LiteralList::Element* start) 
    {
      CALL("reset(LiteralList& lst,LiteralList::Element* start)");
      _current = start;
      //_end = lst.endOfElements();
      _litNum = 0UL;
    };
    void reset(LiteralList* lst,LiteralList::Element* start) 
    {
      CALL("reset(LiteralList* lst,LiteralList::Element* start)");
      _current = start;
      //_end = lst->endOfElements();
      _litNum = 0UL;
    };
    operator bool() const { return notEnd(); };
    bool end() const { return !_current; };
    bool last() const 
    { 
      return (_current) && (!_current->next()); 
    }; 
    bool notEnd() const { return _current != 0; };
    void next() 
    {
      CALL("Iterator::next()");
      ASSERT(_current);
      _current = _current->next();
      _litNum++;
    };

    Element* currentElement() const
    {
      CALL("currentElement() const");
      return _current;
    };
    TERM* currentLiteral() const
    {
      CALL("currentLiteral() const");
      ASSERT(_current);
      return _current->literal();
    };
    ulong litNum() const { return _litNum; };
  private:
    Element* _current;
    //Element* _end;
    ulong _litNum; 
  }; // class LiteralList::Iterator


  //===================================================
#undef DEBUG_NAMESPACE
#ifdef DEBUG_LITERAL_LIST
#  define DEBUG_NAMESPACE "LiteralList::ConstIterator"
#endif 
#include "debugMacros.hpp"
  //=================================================== 


  class LiteralList::ConstIterator
  {
  public:
    ConstIterator() : _current(0), _litNum(0UL)
    {
    };


    ConstIterator(const LiteralList& lst) { reset(lst); };
    ConstIterator(const LiteralList* lst) { reset(lst); };

    ConstIterator(const LiteralList& lst,const LiteralList::Element* start) { reset(lst,start); };
    ConstIterator(const LiteralList* lst,const LiteralList::Element* start) { reset(lst,start); };
    ~ConstIterator() {};

    void init()
    {
      _current = 0; 
      _litNum = 0UL;
    };
    void destroy() {};


    void reset(const LiteralList& lst) 
    {
      _current = lst.firstElem();
      //_end = lst.endOfElements();
      _litNum = 0UL;
    };
    void reset(const LiteralList* lst) 
    {
      _current = lst->firstElem();
      //_end = lst->endOfElements();
      _litNum = 0UL;
    };
    void reset(const LiteralList& lst,const LiteralList::Element* start) 
    {
      CALL("reset(const LiteralList& lst,const LiteralList::Element* start)");
      _current = start;
      //_end = lst.endOfElements();
      _litNum = 0UL;
    };
    void reset(const LiteralList* lst,const LiteralList::Element* start) 
    {
      CALL("reset(const LiteralList* lst,const LiteralList::Element* start)");
      _current = start;
      //_end = lst->endOfElements();
      _litNum = 0UL;
    };
    operator bool() const { return notEnd(); };
    bool end() const { return !_current; };
    bool last() const 
    { 
      return (_current) && (!_current->next()); 
    }; 
    bool notEnd() const { return _current != 0; };
    void next() 
    {
      CALL("ConstIterator::next()");
      ASSERT(_current);
      _current = _current->next();
      _litNum++;
    };

    const Element* currentElement() const
    {
      CALL("currentElement() const");
      return _current;
    };
    const TERM* currentLiteral() const
    {
      CALL("currentLiteral() const");
      ASSERT(_current);
      return _current->literal();
    };
    ulong litNum() const { return _litNum; };
  private:
    const Element* _current;
    //Element* _end;
    ulong _litNum; 
  }; // class LiteralList::ConstIterator







}; // namespace VK

//===================================================
#endif
