//
// File:         Signature.cpp
// Description:  Various signature related operations.  
// Created:      Apr 20, 2001, 14:30
// Author:       Alexandre Riazanov
// mail:         riazanov@cs.man.ac.uk
//====================================================
#include <cstdlib>
#include "Signature.hpp"
#include "Tabulation.hpp"
//=================================================
#undef DEBUG_NAMESPACE
#ifdef DEBUG_SIGNATURE
 #define DEBUG_NAMESPACE "Signature"
#endif 
#include "debugMacros.hpp"
//=================================================  

using namespace VK;

Signature* Signature::_current;


Signature::Signature() 
  : _inputVariableName(DOP("Signature::_inputVariableName")),
    _nextInputVariableNumber(0UL),
#ifdef NO_DEBUG
    _symbolName(static_cast<char*>(0),0),
#else
    _symbolName(static_cast<char*>(0),"Signature::_symbolName"),
#endif
    _numericConstantValue(DOP("Signature::_numericConstantValue")),
#ifdef NO_DEBUG
    _isNumericConstant(false,0),
    _isInputSymbol(false,0),
    _isPredicate(false,0),
    _isCommutative(false,0),
    _arity(ULONG_MAX,0),
    _isAnswerPredicate(false,0),
#else
    _isNumericConstant(false,"Signature::_isNumericConstant"),
    _isInputSymbol(false,"Signature::_isInputSymbol"),
    _isPredicate(false,"Signature::_isPredicate"),
    _isCommutative(false,"Signature::_isCommutative"),
    _arity(ULONG_MAX,"Signature::_arity"),
    _isAnswerPredicate(false,"Signature::_isAnswerPredicate"),
#endif
    _endOfInputFlag(false),
    _nextSymbolNumber(0UL),
  
    _numberOfSkolemSymbols(0L),
    _maxlNumOfSkolemSymbols(LONG_MAX),

    _nextSkolemConstantSuffix(0UL),
    _nextSkolemFunctionSuffix(0UL),
    _nextSkolemPredicateSuffix(0UL),
#ifdef DEBUG_NAMESPACE
    _weightIsSet(false,"Signature::_weightIsSet"),
    _weightConstantPart((TermWeightType)1,"Signature::_weightConstantPart"),
    _positiveHeaderPrecedence(DefaultHeaderPrecedence,"Signature::_positiveHeaderPrecedence"),  
    _negativeHeaderPrecedence(DefaultHeaderPrecedence,"Signature::_negativeHeaderPrecedence"),
#else
    _weightIsSet(false,0),
    _weightConstantPart((TermWeightType)1,0),
    _positiveHeaderPrecedence(DefaultHeaderPrecedence,0),  
    _negativeHeaderPrecedence(DefaultHeaderPrecedence,0),
#endif
    _minimalHeaderPrecedence(DefaultHeaderPrecedence),
    _maximalHeaderPrecedence(DefaultHeaderPrecedence),
    _autoSymbolPrecedence(),
    _symbolWeightByArity(1L)
{
  CALL("constructor Signature()");

  ASSERT(PositivePolarity == 0UL);
  ASSERT(NegativePolarity == 1UL);
  ASSERT(DefaultHeaderPrecedence == 0L);
  ASSERT(MinimalPossibleHeaderPrecedence == (LONG_MIN/4)); 
  ASSERT(MaximalPossibleHeaderPrecedence == (LONG_MAX/4));

  _symbolName.sub(VampireKernelConst::UnordEqNum) = "equal"; // equality predicate in the TPTP format 
  _isInputSymbol.sub(VampireKernelConst::UnordEqNum) = true;
  _isPredicate.sub(VampireKernelConst::UnordEqNum) = true;
  _isCommutative.sub(VampireKernelConst::UnordEqNum) = true;           
  _arity.sub(VampireKernelConst::UnordEqNum) = 2UL;
  setSymbolWeightConstantPart(VampireKernelConst::UnordEqNum,(TermWeightType)1);
  if (VampireKernelConst::UnordEqNum >= _nextSymbolNumber) _nextSymbolNumber = VampireKernelConst::UnordEqNum + 1;
  
  _symbolName.sub(VampireKernelConst::OrdEqNum) = "Equal"; // can not be an input symbol in the TPTP format 
  _isInputSymbol.sub(VampireKernelConst::OrdEqNum) = true;
  _isPredicate.sub(VampireKernelConst::OrdEqNum) = true; 
  _isCommutative.sub(VampireKernelConst::OrdEqNum) = true; 
  _arity.sub(VampireKernelConst::OrdEqNum) = 2UL; 
  setSymbolWeightConstantPart(VampireKernelConst::OrdEqNum,(TermWeightType)1);
  if (VampireKernelConst::OrdEqNum >= _nextSymbolNumber) 
    _nextSymbolNumber = VampireKernelConst::OrdEqNum + 1; 

  _symbolName.sub(VampireKernelConst::BuiltInTrueNum) = "True"; // can not be an input symbol in the TPTP format
  _isInputSymbol.sub(VampireKernelConst::BuiltInTrueNum) = true;
  _isPredicate.sub(VampireKernelConst::BuiltInTrueNum) = true; 
  _isCommutative.sub(VampireKernelConst::BuiltInTrueNum) = false; 
  _arity.sub(VampireKernelConst::BuiltInTrueNum) = 0UL;
  setSymbolWeightConstantPart(VampireKernelConst::BuiltInTrueNum,(TermWeightType)1);
  if (VampireKernelConst::BuiltInTrueNum >= _nextSymbolNumber) 
    _nextSymbolNumber = VampireKernelConst::BuiltInTrueNum + 1; 



  _symbolName.sub(VampireKernelConst::ReservedPred0) = "ReservedPred0"; // can not be an input symbol in the TPTP format 
  _isPredicate.sub(VampireKernelConst::ReservedPred0) = true; 
  _arity.sub(VampireKernelConst::ReservedPred0) = 2UL; 
  setSymbolWeightConstantPart(VampireKernelConst::ReservedPred0,(TermWeightType)1);
  if (VampireKernelConst::ReservedPred0 >= _nextSymbolNumber) _nextSymbolNumber = VampireKernelConst::ReservedPred0 + 1; 

  _symbolName.sub(VampireKernelConst::ReservedPred1) = "ReservedPred1"; // can not be an input symbol in the TPTP format 
  _isPredicate.sub(VampireKernelConst::ReservedPred1) = true; 
  _arity.sub(VampireKernelConst::ReservedPred1) = 2UL; 
  setSymbolWeightConstantPart(VampireKernelConst::ReservedPred1,(TermWeightType)1);
  if (VampireKernelConst::ReservedPred1 >= _nextSymbolNumber) _nextSymbolNumber = VampireKernelConst::ReservedPred1 + 1; 

  _symbolName.sub(VampireKernelConst::ReservedPred2) = "ReservedPred2"; // can not be an input symbol in the TPTP format
  _isPredicate.sub(VampireKernelConst::ReservedPred2) = true;
  _arity.sub(VampireKernelConst::ReservedPred2) = 2UL; 
  setSymbolWeightConstantPart(VampireKernelConst::ReservedPred2,(TermWeightType)1);
  if (VampireKernelConst::ReservedPred2 >= _nextSymbolNumber) _nextSymbolNumber = VampireKernelConst::ReservedPred2 + 1; 


  // Fill the "holes" between the reserved symbols
   
  for (ulong fn = 0; fn < _nextSymbolNumber; fn++) 
    if (!reservedSymbolNumber(fn)) 
      {
	_symbolName.sub(fn) = "NoSymbol"; // can not be an input symbol in the TPTP format  
	_arity.sub(fn) = 0UL;
      };

  // Create canonical names for all variables
  for (ulong vn = 0; vn < VampireKernelConst::MaxNumOfVariables; vn++)
    {
      _canonicalVariableName[vn] = createCanonicalVariableName(vn);
    };

  _autoSymbolPrecedence.assignFixed(VampireKernelConst::UnordEqNum,0L);
  _autoSymbolPrecedence.assignFixed(VampireKernelConst::OrdEqNum,0L);
  ASSERT_IN(checkObj(),"END");
}; // Signature::Signature() 



Signature::~Signature() 
{
  CALL("destructor ~Signature()");
}; // Signature::~Signature() 


void Signature::init() 
{
  CALL("void init()");

  ASSERT(PositivePolarity == 0UL);
  ASSERT(NegativePolarity == 1UL);
  ASSERT(DefaultHeaderPrecedence == 0L);
  ASSERT(MinimalPossibleHeaderPrecedence == (LONG_MIN/4)); 
  ASSERT(MaximalPossibleHeaderPrecedence == (LONG_MAX/4));



  _inputVariableName.init(DOP("Signature::_inputVariableName"));
  _nextInputVariableNumber = 0UL;
#ifdef NO_DEBUG
  _symbolName.init(static_cast<char*>(0),0);
#else
  _symbolName.init(static_cast<char*>(0),"Signature::_symbolName");
#endif
  _numericConstantValue.init(DOP("Signature::_numericConstantValue"));
#ifdef NO_DEBUG
  _isNumericConstant.init(false,0);
  _isInputSymbol.init(false,0);
  _isPredicate.init(false,0);
  _isCommutative.init(false,0);
  _arity.init(ULONG_MAX,0);
  _isAnswerPredicate.init(false,0);
#else
  _isNumericConstant.init(false,"Signature::_isNumericConstant");
  _isInputSymbol.init(false,"Signature::_isInputSymbol");
  _isPredicate.init(false,"Signature::_isPredicate");
  _isCommutative.init(false,"Signature::_isCommutative");
  _arity.init(ULONG_MAX,"Signature::_arity");
  _isAnswerPredicate.init(false,"Signature::_isAnswerPredicate");
#endif
  _endOfInputFlag = false;
  _nextSymbolNumber = 0UL;
  
  _numberOfSkolemSymbols = 0L;
  _maxlNumOfSkolemSymbols = LONG_MAX;

  _nextSkolemConstantSuffix = 0UL;
  _nextSkolemFunctionSuffix = 0UL;
  _nextSkolemPredicateSuffix = 0UL;

#ifdef DEBUG_NAMESPACE
  _weightIsSet.init(false,"Signature::_weightIsSet");
  _weightConstantPart.init((TermWeightType)1,"Signature::_weightConstantPart");
  _positiveHeaderPrecedence.init(DefaultHeaderPrecedence,"Signature::_positiveHeaderPrecedence");  
  _negativeHeaderPrecedence.init(DefaultHeaderPrecedence,"Signature::_negativeHeaderPrecedence");
#else
  _weightIsSet.init(false,0);
  _weightConstantPart.init((TermWeightType)1,0);
  _positiveHeaderPrecedence.init(DefaultHeaderPrecedence,0);  
  _negativeHeaderPrecedence.init(DefaultHeaderPrecedence,0);
#endif
  _minimalHeaderPrecedence = DefaultHeaderPrecedence;
  _maximalHeaderPrecedence = DefaultHeaderPrecedence;
  _autoSymbolPrecedence.init();
  _symbolWeightByArity = 1L;



  _symbolName.sub(VampireKernelConst::UnordEqNum) = "equal"; // equality predicate in the TPTP format 
  _isInputSymbol.sub(VampireKernelConst::UnordEqNum) = true;
  _isPredicate.sub(VampireKernelConst::UnordEqNum) = true;
  _isCommutative.sub(VampireKernelConst::UnordEqNum) = true;           
  _arity.sub(VampireKernelConst::UnordEqNum) = 2UL;
  setSymbolWeightConstantPart(VampireKernelConst::UnordEqNum,(TermWeightType)1);
  if (VampireKernelConst::UnordEqNum >= _nextSymbolNumber) _nextSymbolNumber = VampireKernelConst::UnordEqNum + 1;
  
  _symbolName.sub(VampireKernelConst::OrdEqNum) = "Equal"; // can not be an input symbol in the TPTP format 
  _isInputSymbol.sub(VampireKernelConst::OrdEqNum) = true;
  _isPredicate.sub(VampireKernelConst::OrdEqNum) = true; 
  _isCommutative.sub(VampireKernelConst::OrdEqNum) = true; 
  _arity.sub(VampireKernelConst::OrdEqNum) = 2UL; 
  setSymbolWeightConstantPart(VampireKernelConst::OrdEqNum,(TermWeightType)1);
  if (VampireKernelConst::OrdEqNum >= _nextSymbolNumber) 
    _nextSymbolNumber = VampireKernelConst::OrdEqNum + 1; 

  _symbolName.sub(VampireKernelConst::BuiltInTrueNum) = "True"; // can not be an input symbol in the TPTP format
  _isInputSymbol.sub(VampireKernelConst::BuiltInTrueNum) = true;
  _isPredicate.sub(VampireKernelConst::BuiltInTrueNum) = true; 
  _isCommutative.sub(VampireKernelConst::BuiltInTrueNum) = false; 
  _arity.sub(VampireKernelConst::BuiltInTrueNum) = 0UL;
  setSymbolWeightConstantPart(VampireKernelConst::BuiltInTrueNum,(TermWeightType)1);
  if (VampireKernelConst::BuiltInTrueNum >= _nextSymbolNumber) 
    _nextSymbolNumber = VampireKernelConst::BuiltInTrueNum + 1; 



  _symbolName.sub(VampireKernelConst::ReservedPred0) = "ReservedPred0"; // can not be an input symbol in the TPTP format 
  _isPredicate.sub(VampireKernelConst::ReservedPred0) = true; 
  _arity.sub(VampireKernelConst::ReservedPred0) = 2UL; 
  setSymbolWeightConstantPart(VampireKernelConst::ReservedPred0,(TermWeightType)1);
  if (VampireKernelConst::ReservedPred0 >= _nextSymbolNumber) _nextSymbolNumber = VampireKernelConst::ReservedPred0 + 1; 

  _symbolName.sub(VampireKernelConst::ReservedPred1) = "ReservedPred1"; // can not be an input symbol in the TPTP format 
  _isPredicate.sub(VampireKernelConst::ReservedPred1) = true; 
  _arity.sub(VampireKernelConst::ReservedPred1) = 2UL; 
  setSymbolWeightConstantPart(VampireKernelConst::ReservedPred1,(TermWeightType)1);
  if (VampireKernelConst::ReservedPred1 >= _nextSymbolNumber) _nextSymbolNumber = VampireKernelConst::ReservedPred1 + 1; 

  _symbolName.sub(VampireKernelConst::ReservedPred2) = "ReservedPred2"; // can not be an input symbol in the TPTP format
  _isPredicate.sub(VampireKernelConst::ReservedPred2) = true;
  _arity.sub(VampireKernelConst::ReservedPred2) = 2UL; 
  setSymbolWeightConstantPart(VampireKernelConst::ReservedPred2,(TermWeightType)1);
  if (VampireKernelConst::ReservedPred2 >= _nextSymbolNumber) _nextSymbolNumber = VampireKernelConst::ReservedPred2 + 1; 


  // Fill the "holes" between the reserved symbols
   
  for (ulong fn = 0; fn < _nextSymbolNumber; fn++) 
    if (!reservedSymbolNumber(fn)) 
      {
	_symbolName.sub(fn) = "NoSymbol"; // can not be an input symbol in the TPTP format  
	_arity.sub(fn) = 0UL;
      };

  // Create canonical names for all variables
  for (ulong vn = 0; vn < VampireKernelConst::MaxNumOfVariables; vn++)
    {
      _canonicalVariableName[vn] = createCanonicalVariableName(vn);
    };

  _autoSymbolPrecedence.assignFixed(VampireKernelConst::UnordEqNum,0L);
  _autoSymbolPrecedence.assignFixed(VampireKernelConst::OrdEqNum,0L);
  ASSERT_IN(checkObj(),"END");
}; // void Signature::init() 


void Signature::destroy()
{
  CALL("destroy()");
  _autoSymbolPrecedence.destroy();
  _negativeHeaderPrecedence.destroy();
  _positiveHeaderPrecedence.destroy();
  _weightConstantPart.destroy();
  _weightIsSet.destroy();
  _isAnswerPredicate.destroy();
  _arity.destroy();
  _isCommutative.destroy();
  _isPredicate.destroy();
  _isInputSymbol.destroy();
  _isNumericConstant.destroy();
  _numericConstantValue.destroy();
  _symbolName.destroy();
  _inputVariableName.destroy();
}; // void Signature::destroy()





void Signature::resetInputVariables() 
{ 
  CALL("resetInputVariables()");
  while (_nextInputVariableNumber)  
    {
      _nextInputVariableNumber--; 
      delete [] (_inputVariableName.sub(_nextInputVariableNumber));
    };
}; // void Signature::resetInputVariables() 

ulong Signature::inputVariableNumber(const char* varName)
{
  CALL("inputVariableNumber(const char* varName)");
  for (ulong vn = 0; vn < _nextInputVariableNumber; vn++)
    if (!(strcmp(varName,_inputVariableName.sub(vn)))) return vn;
  _inputVariableName.sub(_nextInputVariableNumber) = new char[strlen(varName) + 1];
  strcpy(_inputVariableName.sub(_nextInputVariableNumber),varName);
  _nextInputVariableNumber++;
  return _nextInputVariableNumber - 1; 
}; // ulong Signature::inputVariableNumber(const char* varName)

ulong Signature::inputSymbolNumber(bool predicate,const char* funName,ulong ar)
{
  CALL("inputSymbolNumber(bool predicate,const char* funName,ulong ar)");

  // first check if funName represents a numeric constant
  char* endPtr;
  double numConstValue = strtod(funName,&endPtr);
  if ((endPtr != funName) &&
      (*endPtr == '\0'))
    return numericConstantNumber(numConstValue);

  for (ulong fn = 0; fn < _nextSymbolNumber; fn++)
    {
      if (_symbolName.sub(fn))
	{
	  if ((!(strcmp(funName,_symbolName[fn]))) &&
	      (arity(fn) == ar) &&
	      (_isPredicate.sub(fn) == predicate))
	    {
	      return fn;
	    };
	};      
    };
  _symbolName.sub(_nextSymbolNumber) = new char[strlen(funName) + 1];
  _arity.sub(_nextSymbolNumber) = ar;
  strcpy(_symbolName.sub(_nextSymbolNumber),funName);
  _isInputSymbol.sub(_nextSymbolNumber) = true;
  _isPredicate.sub(_nextSymbolNumber) = predicate;   
  _isCommutative.sub(_nextSymbolNumber) = false; 
  _nextSymbolNumber++;
  ASSERT(_nextSymbolNumber <= _symbolName.size());
  ASSERT(_nextSymbolNumber <= _isInputSymbol.size());
  ASSERT(_nextSymbolNumber <= _isPredicate.size());
  ASSERT(_nextSymbolNumber <= _isCommutative.size());
  return _nextSymbolNumber - 1;  
}; // ulong Signature::inputSymbolNumber(bool predicate,const char* funName,ulong ar)


ulong Signature::numericConstantNumber(const double& value)
{
  CALL("numericConstantNumber(const double& value)");
  // temporary very inefficient implementation
  for (ulong fn = 0; fn < _nextSymbolNumber; fn++)
    {
      if ((isNumericConstant(fn)) &&
	  (value == _numericConstantValue[fn]))
	return fn;
    };
  // new numeric constant
  _numericConstantValue.sub(_nextSymbolNumber) = value;
  _arity.sub(_nextSymbolNumber) = 0UL;
  _isNumericConstant.sub(_nextSymbolNumber) = true;
  _isInputSymbol.sub(_nextSymbolNumber) = !_endOfInputFlag;
  _isPredicate.sub(_nextSymbolNumber) = false;   
  _isCommutative.sub(_nextSymbolNumber) = false; 

  // Temporary patch:
  
  _positiveHeaderPrecedence.sub(_nextSymbolNumber) = _minimalHeaderPrecedence;
  --_minimalHeaderPrecedence;

  _nextSymbolNumber++;
  ASSERT(_nextSymbolNumber <= _numericConstantValue.size());
  ASSERT(_nextSymbolNumber <= _isInputSymbol.size());
  ASSERT(_nextSymbolNumber <= _isPredicate.size());
  ASSERT(_nextSymbolNumber <= _isCommutative.size());
  return _nextSymbolNumber - 1;  
}; // ulong Signature::numericConstantNumber(const double& value)


void Signature::makeCommutative(ulong funNum)
{
  CALL("makeCommutative(ulong funNum)");
  _isCommutative.sub(funNum) = true;
}; 

void Signature::makeNoncommutative(ulong funNum)
{
  CALL("makeNoncommutative(ulong funNum)");
  _isCommutative.sub(funNum) = false;
};



void Signature::setCommutativity(const SymbolInfo* commutativityTable,
				 const SymbolInfo* symmetryTable)
{
  CALL("setCommutativity(const Clause* const *commutativityTable,const Clause* const *symmetryTable)");
  ASSERT(_nextSymbolNumber <= _isCommutative.size());
  for (ulong fn = 0; fn < _nextSymbolNumber; fn++)     
    _isCommutative[fn] = 
      (commutativityTable && commutativityTable->commutativityLaw(fn)) || 
      (symmetryTable && symmetryTable->symmetryLaw(fn));
};



bool Signature::createNewSkolemConstantNumber(ulong& res)
{
  CALL("createNewSkolemConstantNumber()");
  static char tmpString[256]; 
  if (_numberOfSkolemSymbols >= _maxlNumOfSkolemSymbols) 
    return false;
  strcpy(tmpString,VampireKernelConst::SkolemConstantNamePrefix());
  char* endOfPrefix = tmpString + strlen(VampireKernelConst::SkolemConstantNamePrefix());
  char* suffix;
 try_suffix:
  suffix = endOfPrefix; 
  print(_nextSkolemConstantSuffix,suffix);
  suffix[0] = '\0';
  if (isRegisteredSymbolName(tmpString)) 
    {
      // a function with this name already exists, try bigger suffix  
      _nextSkolemConstantSuffix++;
      goto try_suffix;
    };
  // this candidate is OK, register it
   
  _symbolName.sub(_nextSymbolNumber) = new char[strlen(tmpString) + 1];
  strcpy(_symbolName.sub(_nextSymbolNumber),tmpString);
  _isInputSymbol.sub(_nextSymbolNumber) = false;
  _isPredicate.sub(_nextSymbolNumber) = false;   
  _isCommutative.sub(_nextSymbolNumber) = false; 
  _arity.sub(_nextSymbolNumber) = 0UL;
  setSymbolWeightConstantPart(_nextSymbolNumber,autoWeight(0UL));
  res = _nextSymbolNumber;
  _nextSymbolNumber++;

  ASSERT(_nextSymbolNumber <= _symbolName.size());
  ASSERT(_nextSymbolNumber <= _isInputSymbol.size());
  ASSERT(_nextSymbolNumber <= _isPredicate.size());
  ASSERT(_nextSymbolNumber <= _isCommutative.size());

  _numberOfSkolemSymbols++;
  return true; 
}; // bool Signature::createNewSkolemConstantNumber(ulong& res)
  
bool Signature::createNewSkolemFunctionNumber(ulong arity,ulong& res)
{
  CALL("createNewSkolemFunctionNumber(arity,ulong& res)");
  static char tmpString[256]; 
  if (_numberOfSkolemSymbols >= _maxlNumOfSkolemSymbols) 
    return false;
  strcpy(tmpString,VampireKernelConst::SkolemFunctionNamePrefix());
  char* endOfPrefix = tmpString + strlen(VampireKernelConst::SkolemFunctionNamePrefix());
  char* suffix;
 try_suffix:
  suffix = endOfPrefix; 
  print(_nextSkolemFunctionSuffix,suffix);
  suffix[0] = '\0';
  if (isRegisteredSymbolName(tmpString)) 
    {
      // a function with this name already exists, try bigger suffix  
      _nextSkolemFunctionSuffix++;
      goto try_suffix;
    };
  // this candidate is OK, register it
   
  _symbolName.sub(_nextSymbolNumber) = new char[strlen(tmpString) + 1];
  strcpy(_symbolName.sub(_nextSymbolNumber),tmpString);
  _isInputSymbol.sub(_nextSymbolNumber) = false;
  _isPredicate.sub(_nextSymbolNumber) = false;   
  _isCommutative.sub(_nextSymbolNumber) = false; 
  _arity.sub(_nextSymbolNumber) = arity;
  setSymbolWeightConstantPart(_nextSymbolNumber,autoWeight(arity));
  res = _nextSymbolNumber;
  _nextSymbolNumber++;

  ASSERT(_nextSymbolNumber <= _symbolName.size());
  ASSERT(_nextSymbolNumber <= _isInputSymbol.size());
  ASSERT(_nextSymbolNumber <= _isPredicate.size());
  ASSERT(_nextSymbolNumber <= _isCommutative.size());

  _numberOfSkolemSymbols++; 
  return true;
}; // bool Signature::createNewSkolemFunctionNumber(ulong arity,ulong& res)

bool Signature::createNewSkolemPredicateNumber(ulong arity,ulong& res)
{
  CALL("createNewSkolemPredicateNumber(arity,ulong& res)");
  static char tmpString[256]; 
  if (_numberOfSkolemSymbols >= _maxlNumOfSkolemSymbols) 
    return false;
  strcpy(tmpString,VampireKernelConst::SkolemPredicateNamePrefix());
  char* endOfPrefix = tmpString + strlen(VampireKernelConst::SkolemPredicateNamePrefix());
  char* suffix;
 try_suffix:
  suffix = endOfPrefix; 
  print(_nextSkolemPredicateSuffix,suffix);
  suffix[0] = '\0';
  if (isRegisteredSymbolName(tmpString)) 
    {
      // a function with this name already exists, try bigger suffix  
      _nextSkolemPredicateSuffix++;
      goto try_suffix;
    }; 
  // this candidate is OK, register it
   
  _symbolName.sub(_nextSymbolNumber) = new char[strlen(tmpString) + 1];
  strcpy(_symbolName.sub(_nextSymbolNumber),tmpString);
  _isInputSymbol.sub(_nextSymbolNumber) = false;
  _isPredicate.sub(_nextSymbolNumber) = true;   
  _isCommutative.sub(_nextSymbolNumber) = false; 
  _arity.sub(_nextSymbolNumber) = arity;
  setSymbolWeightConstantPart(_nextSymbolNumber,autoWeight(arity));
  res = _nextSymbolNumber;
  _nextSymbolNumber++;

  ASSERT(_nextSymbolNumber <= _symbolName.size());
  ASSERT(_nextSymbolNumber <= _isInputSymbol.size());
  ASSERT(_nextSymbolNumber <= _isPredicate.size());
  ASSERT(_nextSymbolNumber <= _isCommutative.size());

  _numberOfSkolemSymbols++;  
  return true;
}; // bool Signature::createNewSkolemPredicateNumber(ulong arity,ulong& res)


ostream& Signature::output(ostream& str)
{ 
  str << "%=================== Signature: =====================\n";

  ASSERT((_nextSymbolNumber <= _symbolName.size()) || 
	 (_nextSymbolNumber <= _numericConstantValue.size()));
  ASSERT(_nextSymbolNumber <= _isInputSymbol.size());
  ASSERT(_nextSymbolNumber <= _isPredicate.size());
  ASSERT(_nextSymbolNumber <= _isCommutative.size());

  ulong fn;
  for (fn = 0; fn < _nextSymbolNumber; fn++)
    {
      if ((!reservedSymbolNumber(fn)) || (fn == VampireKernelConst::UnordEqNum))
	  
	{
	  str << fn << ": ";
	  outputSymbol(str,fn) << "/";
	  ASSERT(fn < _arity.size());
	  str << _arity[fn]; 
	  if (_isInputSymbol[fn]) { str << " input  "; } else str << " skolem ";
	  if (_isPredicate[fn]) 
	    { 
	      str << "predicate ";
	      if (_isCommutative[fn]) { str << "symmetric      "; } else str << "nonsymmetric  "; 
	    } 
	  else
	    {
	      str << "function  ";
	      if (_isCommutative[fn]) str << "commutative    ";  
	    };
	  str << "\n";
	};

    };
  for (fn = 0; fn < _nextSymbolNumber; fn++)
    {
      if ((!reservedSymbolNumber(fn)) || 
	  (fn == VampireKernelConst::UnordEqNum))
	{
	  if (_isPredicate[fn]) 
	    { 
	      str << "header_precedence(++";
	      outputSymbol(str,fn) 
		<< "/" << arity(fn) 
		<< ") = " << headerPrecedence(PositivePolarity,fn) << ",     "
		<< "header_precedence(--";
	      outputSymbol(str,fn)
		<< "/" << arity(fn) 
		<< ") = " << headerPrecedence(NegativePolarity,fn) << "\n";
	    };
	  str << "symbol_precedence("
	      << ((_isPredicate[fn])? "predicate" : "function") << " ";
	  outputSymbol(str,fn)	    
	    << "/" << arity(fn) 
	    << ") = " << symbolPrecedence(fn) << "\n";
	  
	  str << "weight("
	      << ((_isPredicate[fn])? "predicate" : "function") << " ";
	  outputSymbol(str,fn)     
	    << "/" << arity(fn) 
	    << ") = " << weightConstantPart(fn) << "\n";

	};
    };
  return str << "%============ End of signature. =================\n";
}; // ostream& Signature::output(ostream& str)




bool Signature::isRegisteredSymbolName(const char* str)
{
  CALL("isRegisteredSymbolName(const char* str)");
  for (ulong fn = 0; fn < _nextSymbolNumber; fn++)
    if ((!isNumericConstant(fn)) && 
	(!(strcmp(str,_symbolName.sub(fn))))) 
      return true;
  return false;  
}; // bool Signature::isRegisteredSymbolName(const char* str)

const char* Signature::createCanonicalVariableName(ulong varNum)
{
  CALL("createCanonicalVariableName(ulong varNum)");
  static char tmpString[256];
  strcpy(tmpString,VampireKernelConst::CanonicalVariableNamePrefix());
  char* cursor = tmpString + strlen(VampireKernelConst::CanonicalVariableNamePrefix());
  print(varNum,cursor);
  cursor[0] = '\0';
  char* res = new char[strlen(tmpString) + 1]; 
  strcpy(res,tmpString);
  return res;
}; // const char* Signature::createCanonicalVariableName(ulong varNum)



void Signature::endOfInput()
{
  CALL("endOfInput()");
  
  _endOfInputFlag = true;

  // Calculate precedences
  ulong fn;
  for (fn = 0; fn < nextSymbolNumber(); fn++)
    {
      _autoSymbolPrecedence.assignArity(fn,arity(fn));
    }; 
  _autoSymbolPrecedence.assignAllPrecedences();
  long headerPrecedence;
  for (fn = 0; fn < nextSymbolNumber(); fn++)
    { 
      if (_autoSymbolPrecedence.headerPrecedence(true,fn,headerPrecedence))
	{  
	  setHeaderPrecedence(1UL,fn,headerPrecedence); // 1UL <=> negative
	  if (headerPrecedence > _maximalHeaderPrecedence)
	    {
	      _maximalHeaderPrecedence = headerPrecedence;
	    }
	  else
	    if (headerPrecedence < _minimalHeaderPrecedence)
	      {
		_minimalHeaderPrecedence = headerPrecedence;
	      };

	  ALWAYS(_autoSymbolPrecedence.headerPrecedence(false,fn,headerPrecedence));
	  setHeaderPrecedence(0UL,fn,headerPrecedence); // 0UL <=> positive
	  if (headerPrecedence > _maximalHeaderPrecedence)
	    {
	      _maximalHeaderPrecedence = headerPrecedence;
	    }
	  else
	    if (headerPrecedence < _minimalHeaderPrecedence)
	      {
		_minimalHeaderPrecedence = headerPrecedence;
	      };
	};
    };

  // Assign weights
  for (fn = 0; fn < nextSymbolNumber(); fn++)
    {
      if (!weightIsSet(fn))
	setSymbolWeightConstantPart(fn,autoWeight(arity(fn)));
    };
 
  ASSERT((_nextSymbolNumber <= _symbolName.size()) || 
	 (_nextSymbolNumber <= _numericConstantValue.size()));
  ASSERT(_nextSymbolNumber <= _isInputSymbol.size());
  ASSERT(_nextSymbolNumber <= _isPredicate.size());
  ASSERT(_nextSymbolNumber <= _isCommutative.size());

}; // void Signature::endOfInput()
  

void Signature::tabulate(Tabulation* tabulation)
{
  CALL("tabulate(Tabulation* tabulation)");

  ASSERT((_nextSymbolNumber <= _symbolName.size()) || 
	 (_nextSymbolNumber <= _numericConstantValue.size()));
  ASSERT(_nextSymbolNumber <= _isInputSymbol.size());
  ASSERT(_nextSymbolNumber <= _isPredicate.size());
  ASSERT(_nextSymbolNumber <= _isCommutative.size());

  if (!tabulation) return;

  COP("COP10");

  for (ulong fn = 0; fn < _nextSymbolNumber; fn++)
    {
  COP("COP20");

      if (((!reservedSymbolNumber(fn)) || (fn == VampireKernelConst::UnordEqNum)) &&
	  (!isSkolem(fn)))
	{
COP("COP30");
	  if (_isPredicate[fn]) 
	    { 
COP("COP40");
	      tabulation->signaturePredicate(isSkolem(fn),symbolName(fn),arity(fn));
	      tabulation->headerPrecedence(true,symbolName(fn),arity(fn),headerPrecedence(PositivePolarity,fn));
	      tabulation->headerPrecedence(false,symbolName(fn),arity(fn),headerPrecedence(NegativePolarity,fn));	      
	      
	      tabulation->symbolPrecedence(true,symbolName(fn),arity(fn),symbolPrecedence(fn));
	      tabulation->symbolWeight(true,
				       symbolName(fn),
				       arity(fn),
				       weightConstantPart(fn));
	    }
	  else
	    {
COP("COP50");
	      if (isNumericConstant(fn))
		{
COP("COP60");
		  tabulation->signatureNumericConstant(isSkolem(fn),numericConstantValue(fn));
		  tabulation->numericConstantPrecedence(numericConstantValue(fn),symbolPrecedence(fn));
		  tabulation->numericConstantWeight(numericConstantValue(fn),
						    weightConstantPart(fn));
		}
	      else
		{

COP("COP70");
		  tabulation->signatureFunction(isSkolem(fn),symbolName(fn),arity(fn));
		  
		  tabulation->symbolPrecedence(false,symbolName(fn),arity(fn),symbolPrecedence(fn));
		  tabulation->symbolWeight(false,
					   symbolName(fn),
					   arity(fn),
					   weightConstantPart(fn));
		};
	    };
	};

    };
COP("END");
}; // void Signature::tabulate(Tabulation* tabulation)


//====================================================
