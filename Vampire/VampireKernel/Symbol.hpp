//
// File:         Symbols.hpp
// Description:  Signature symbols. 
// Created:      Sep 30, 2002, Alexandre Riazanov, riazanov@cs.man.ac.uk
// Status:       Sketch.
//============================================================================
#ifndef SYMBOL_H
#define SYMBOL_H
//============================================================================
#include <cstring>
#include "jargon.hpp"
#include "VampireKernelDebugFlags.hpp"
#include "TermWeightType.hpp"
#include "Comparison.hpp"
//============================================================================
#ifdef DEBUG_SYMBOL
#undef DEBUG_NAMESPACE
 #define DEBUG_NAMESPACE "Symbol<Alloc>"
#endif
#include "debugMacros.hpp"
//============================================================================

namespace VK 
{
  using namespace BK;

  template <class Alloc>
  class Symbol
  { 
  public:
    Symbol() {};
    ~Symbol() {};
    
    void resetProperties()
    {
      _isInputSymbol = true;
      _weight = (TermWeightType)1;
      _symbolPrecedenceClass = 0L;
    };

    ulong arity() const { return _arity; };
    void setArity(ulong a) { _arity = a; };
    ulong symbolId() const { return _symbolId; };
    void setSymbolId(ulong n) { _symbolId = n; };
    bool isInputSymbol() const { return _isInputSymbol; };
    void setIsInputSymbol(bool p) { _isInputSymbol = p; };
    const TermWeightType& weight() const { return _weight; };
    void setWeight(const TermWeightType& w) { _weight = w; };
    
    void hasPrecedence(long& cl,long& numInCl)
    {
      cl = _symbolPrecedenceClass;
      numInCl = -_symbolId;
    };    
    bool hasGreaterSymbolPrecedenceThan(const Symbol& sym) const;
    Comparison compareBySymbolPrecedenceWith(const Symbol& sym) const;

    void setPrecedenceClass(long n) { _symbolPrecedenceClass = n; };

    static long smallestPossiblePrecedenceClass() { return LONG_MIN; };
    static long greatestPossiblePrecedenceClass() { return LONG_MAX; };
 
    
  private:
    ulong _arity;
    ulong _symbolId;
    bool _isInputSymbol;
    TermWeightType _weight;
    long _symbolPrecedenceClass;
  }; // class Symbol
}; // namespace VK 

//============================================================================
#ifdef DEBUG_SYMBOL
#undef DEBUG_NAMESPACE
 #define DEBUG_NAMESPACE "Function<Alloc>"
#endif
#include "debugMacros.hpp"
//============================================================================

namespace VK 
{
  using namespace BK;

  template <class Alloc>
  class Function : public Symbol<Alloc>
  {
  public:
    Function() : _name(0) {};
    ~Function() 
    {
      if (DestructionMode::isThorough())
	if (_name)
	  Alloc::deallocate(static_cast<void*>(_name),strlen(_name) + 1);
    };
    void resetProperties()
    {
      Symbol<Alloc>::resetProperties();
      _commutativityLaw = 0;
    };
    const char* name() const 
    {
      CALL("name() const");
      return _name; 
    };
    void setName(const char* n) 
    {
      CALL("setName(const char* n)");
      if (_name)
	Alloc::deallocate(static_cast<void*>(_name),strlen(_name) + 1);
      _name = static_cast<char*>(Alloc::allocate(strlen(n) + 1));
      strcpy(_name,n);
    };
    bool isNumeral() const { return _isNumeral; };
    void setIsNumeral(bool p) 
    { 
      _isNumeral = p;
      if (p) setArity(0UL);
    };
    const double& numericValue() const
    {
      CALL("numericValue() const");
      ASSERT(_isNumeral);
      return _numericValue;
    };
    void setNumericValue(const double& val) { _numericValue = val; };
    bool isCommutative() const { return (bool)_commutativityLaw; }; 
    void* commutativityLaw() const { return _commutativityLaw; };
    void setCommutativityLaw(void* c) { _commutativityLaw = c; }; 
    bool operator==(const Function& fn) const;
    bool operator!=(const Function& fn) const;
    bool operator>(const Function& fn) const;
    bool operator>=(const Function& fn) const;
    bool operator<(const Function& fn) const;
    bool operator<=(const Function& fn) const;
  private:
    char* _name;
    bool _isNumeral;
    double _numericValue;
    void* _commutativityLaw;    
  }; // class Function
}; // namespace VK

//============================================================================
#ifdef DEBUG_SYMBOL
#undef DEBUG_NAMESPACE
 #define DEBUG_NAMESPACE "LiteralHeader<Alloc>"
#endif
#include "debugMacros.hpp"
//============================================================================

 
namespace VK 
{
  using namespace BK;

  template <class Alloc> class Predicate;

  template <class Alloc>
  class LiteralHeader 
  {
  public:
    enum Polarity
    {
      PositivePolarity = 0, // see isPositive() and isNegative() 
      NegativePolarity = 1  // see isPositive() and isNegative() 
    };
  public:
    LiteralHeader() {};
    ~LiteralHeader() {};
    ulong headerId() const { return _headerId; };
    void setHeaderId(ulong id) { _headerId = id; };
    const Predicate<Alloc>* predicate() const { return _predicate; };
    Predicate<Alloc>* predicate() { return _predicate; };
    void setPredicate(Predicate<Alloc>* p) { _predicate = p; };
    Polarity polarity() const { return _polarity; };
    void setPolarity(Polarity p) { _polarity = p; };
    bool isPositive() const { return !_polarity; };
    bool isNegative() const { return _polarity; };
    void hasPrecedence(long& precClass,long& numInPrecClass)
    {
      precClass = _precedenceClass;
      numInPrecClass = -_headerId;
    };
    const LiteralHeader* complementaryHeader() const { return _complementaryHeader; };
    LiteralHeader* complementaryHeader() { return _complementaryHeader; };
    void setComplementaryHeader(LiteralHeader* hd) { _complementaryHeader = hd; };
    bool isComplementary(const LiteralHeader* hd) const { return _complementaryHeader = hd; };
  private:
    ulong _headerId;
    Predicate<Alloc>* _predicate;
    Polarity _polarity;
    long _precedenceClass;
    LiteralHeader* _complementaryHeader;
  }; // class LiteralHeader
}; // namespace VK

//============================================================================
#ifdef DEBUG_SYMBOL
#undef DEBUG_NAMESPACE
 #define DEBUG_NAMESPACE "Predicate<Alloc>"
#endif
#include "debugMacros.hpp"
//============================================================================


namespace VK 
{
  using namespace BK;

  template <class Alloc>
  class Predicate : public Symbol<Alloc>
  {
  public:
    Predicate() : _name(0) {};
    ~Predicate()
    {
      if (DestructionMode::isThorough())
	if (_name)
	  Alloc::deallocate(static_cast<void*>(_name),strlen(_name) + 1);
    };
    void resetProperties()
    {
      Symbol<Alloc>::resetProperties();
      _symmetryLaw = 0;
      _isSplitting = false;
      _isAnswer = false;	
    };
    const char* name() const { return _name; };
    void setName(const char* n) 
    { 
      if (_name)
	Alloc::deallocate(static_cast<void*>(_name),strlen(_name) + 1);
      _name = static_cast<char*>(Alloc::allocate(strlen(n) + 1));
      strcpy(_name,n);
    };
    bool isSymmetric() const { return (bool)_symmetryLaw; }; 
    void* symmetryLaw() const { return _symmetryLaw; };
    void setSymmetryLaw(void* c) { _symmetryLaw = c; }; 
    bool isSplitting() const { return _isSplitting; };
    void setIsSplitting(bool p) { _isSplitting = p; };
    bool isAnswer() const { return _isAnswer; };
    const LiteralHeader<Alloc>* positiveHeader() const { return &_positiveHeader; };
    const LiteralHeader<Alloc>* negativeHeader() const { return &_negativeHeader; };
    LiteralHeader<Alloc>* positiveHeader() { return &_positiveHeader; };
    LiteralHeader<Alloc>* negativeHeader() { return &_negativeHeader; };
    bool operator==(const Predicate& pr) const 
    {
      return (arity() == pr.arity()) &&
	(strcmp(name(),pr.name()) == 0);
    };
    bool operator!=(const Predicate& pr) const 
    {
      return !(*this == pr);
    };
    bool operator>(const Predicate& pr) const 
    {
      return (arity() > pr.arity()) ||
	((arity() == pr.arity()) &&
         (strcmp(name(),pr.name()) > 0));
    };
    bool operator>=(const Predicate& pr) const 
    {
      return (arity() > pr.arity()) ||
	((arity() == pr.arity()) &&
         (strcmp(name(),pr.name()) >= 0));
    };
    bool operator<(const Predicate& pr) const 
    {
      return pr > *this;
    };    
    bool operator<=(const Predicate& pr) const 
    {
      return pr >= *this;
    };
  private:
    char* _name;
    void* _symmetryLaw;
    bool _isSplitting;
    bool _isAnswer;
    LiteralHeader<Alloc> _positiveHeader;
    LiteralHeader<Alloc> _negativeHeader;
  }; // class Predicate

}; // namespace VK 

//============================================================================
#ifdef DEBUG_SYMBOL
#undef DEBUG_NAMESPACE
 #define DEBUG_NAMESPACE "Symbol<Alloc>"
#endif
#include "debugMacros.hpp"
//============================================================================

namespace VK
{

template <class Alloc>
bool Symbol<Alloc>::hasGreaterSymbolPrecedenceThan(const Symbol& sym) const
{
  CALL("hasGreaterSymbolPrecedenceThan(const Symbol& sym) const");
  if (_symbolPrecedenceClass == sym._symbolPrecedenceClass)
    {
      return _symbolId < sym._symbolId;
    }
  else
    return _symbolPrecedenceClass > sym._symbolPrecedenceClass;
}; // bool Symbol<Alloc>::hasGreaterSymbolPrecedenceThan(const Symbol& sym) const


template <class Alloc>
BK::Comparison 
Symbol<Alloc>::compareBySymbolPrecedenceWith(const Symbol& sym) const
{
  CALL("compareBySymbolPrecedenceWith(const Symbol& sym) const");
  if (_symbolPrecedenceClass > sym._symbolPrecedenceClass)
    {
      return Greater;
    }
  else
    if (_symbolPrecedenceClass < sym._symbolPrecedenceClass)
      {
	return Less;
      }
    else // _symbolPrecedenceClass == sym._symbolPrecedenceClass
      if (_symbolId < sym._symbolId)
	{
	  return Greater;
	}
      else
	if (_symbolId > sym._symbolId)
	  {
	    return Less;
	  }
	else
	  return Equal;
}; // BK::Comparison Symbol<Alloc>::compareBySymbolPrecedenceWith(const Symbol& sym) const

}; // namespace VK

//============================================================================
#ifdef DEBUG_SYMBOL
#undef DEBUG_NAMESPACE
 #define DEBUG_NAMESPACE "Function<Alloc>"
#endif
#include "debugMacros.hpp"
//============================================================================

namespace VK
{
template <class Alloc>
bool Function<Alloc>::operator==(const Function& fn) const 
{
  if (isNumeral())
    {
      return fn.isNumeral() &&
	(numericValue() == fn.numericValue());
    }
  else
    {
      return (!fn.isNumeral()) &&
	(arity() == fn.arity()) &&
	(strcmp(name(),fn.name()) == 0);
    };
}; // bool operator==(const Function& fn) const 

template <class Alloc>
bool Function<Alloc>::operator!=(const Function& fn) const 
{
  return !(*this == fn);
}; // bool operator==(const Function& fn) const 

template <class Alloc>    
bool Function<Alloc>::operator>(const Function& fn) const 
{
  if (isNumeral())
    {
      if (fn.isNumeral())
	{
	  return numericValue() > fn.numericValue();
	}
      else
	return true;
    }
  else
    {
      if (fn.isNumeral())
	{
	  return false;
	}
      else
	{
	  return (arity() > fn.arity()) ||
	    ((arity() == fn.arity()) &&
	     (strcmp(name(),fn.name()) > 0));
	};
    };
}; // bool operator>(const Function& fn) const 

template <class Alloc>
bool Function<Alloc>::operator>=(const Function& fn) const 
{
  if (isNumeral())
    {
      if (fn.isNumeral())
	{
	  return numericValue() >= fn.numericValue();
	}
      else
	return true;
    }
  else
    {
      if (fn.isNumeral())
	{
	  return false;
	}
      else
	{
	  return (arity() > fn.arity()) ||
	    ((arity() == fn.arity()) &&
	     (strcmp(name(),fn.name()) >= 0));
	};
    };
}; // bool operator>=(const Function& fn) const 

template <class Alloc>
bool Function<Alloc>::operator<(const Function& fn) const 
{
  return fn > *this;
}; // bool operator<(const Function& fn) const 

template <class Alloc>
bool Function<Alloc>::operator<=(const Function& fn) const 
{
  return fn >= *this;
}; // bool operator<=(const Function& fn) const 

}; // namespace VK

//============================================================================
#ifdef DEBUG_SYMBOL
#undef DEBUG_NAMESPACE
 #define DEBUG_NAMESPACE "std"
#endif
#include "debugMacros.hpp"
//============================================================================


namespace std
{
  template <class Alloc>
  ostream& operator<<(ostream& str,const VK::Function<Alloc>& sym)
  {
    if (sym.isNumeral())
      {
	return str << sym.numeralValue();
      }
    else
      return str << sym.name();
  };

  template <class Alloc>
  ostream& operator<<(ostream& str,const VK::Predicate<Alloc>& sym)
  {
    return str << sym.name();
  };

  template <class Alloc>
  ostream& operator<<(ostream& str,const VK::LiteralHeader<Alloc>& hd)
  {
    if (hd.polarity() == VK::LiteralHeader<Alloc>::NegativePolarity)
      str << "~";
    return str << hd.name();
  };

}; // namespace std





//============================================================================
#endif
