//
// File:         BuiltInFloatingPointArithmetic.hpp
// Description:  Built-in floating point arithmetic.  
// Created:      Jul 06, 2002, Alexandre Riazanov, riazanov@cs.man.ac.uk
//==================================================================
#ifndef BUILT_IN_FLOATING_POINT_ARITHMETIC_H
#define BUILT_IN_FLOATING_POINT_ARITHMETIC_H
//==================================================================
#include "jargon.hpp"
#include "VampireKernelDebugFlags.hpp"
#include "BuiltInTheory.hpp"
#include "Array.hpp"
#include "GlobAlloc.hpp"
#include "Clause.hpp"
#include "Flatterm.hpp"
//===================================================================
#undef DEBUG_NAMESPACE
#ifdef DEBUG_BUILT_IN_FLOATING_POINT_ARITHMETIC
 #define DEBUG_NAMESPACE "BuiltInFloatingPointArithmetic"
#endif
#include "debugMacros.hpp"
//===================================================================

namespace VK
{
  class TmpLiteral;
  class BuiltInFloatingPointArithmetic : public BuiltInTheory
    {
    public:
      BuiltInFloatingPointArithmetic(const char* name,
				     Clause::NewNumber* clauseNumberGenerator);
      ~BuiltInFloatingPointArithmetic();
      void* operator new(size_t);
      void operator delete(void* obj);
      bool interpretAs(ulong inputSymbolId,
		       const char* nativeSymbol);
      Clause* simplify(TmpLiteral* lit,TmpLiteral* newLit);
    private:
      
      typedef double (*BinaryFunctionInterpretation)(const double&,const double&);
      typedef double (*UnaryFunctionInterpretation)(const double&);
      typedef bool (*BinaryPredicateInterpretation)(const double&,const double&);

      class NativeSymbolDescriptor
	{
	public:
	  NativeSymbolDescriptor() : _idIsSet(false)
	    {
	    };
	  void setId(ulong id) { _id = id; _idIsSet = true; };
	  void setNativeName(const char* name) { _nativeName = name; };
	  void setArity(ulong a) { _arity = a; };
	  void setInterpretation(BinaryFunctionInterpretation fun) 
	  { 
	    _binFunInterpretation = fun; 
	  };
	  void setInterpretation(UnaryFunctionInterpretation fun) 
	  { 
	    _unFunInterpretation = fun; 
	  };
	  void setInterpretation(BinaryPredicateInterpretation pred) 
	  { 
	    _binPredInterpretation = pred; 
	  };
	  double evaluateOn(const double& arg1,const double& arg2)
	    {
	      CALL("evaluateOn(const double& arg1,const double& arg2)");
	      ASSERT(_arity == 2UL);
	      return _binFunInterpretation(arg1,arg2);
	    };
	  double evaluateOn(const double& arg)
	    {
	      CALL("evaluateOn(const double& arg)");
	      ASSERT(_arity == 1UL);
	      return _unFunInterpretation(arg);
	    };
	  bool evaluatePredicateOn(const double& arg1,const double& arg2)
	    {
	      CALL("evaluatePredicateOn(const double& arg1,const double& arg2)");
	      ASSERT(_arity == 2UL);
	      return _binPredInterpretation(arg1,arg2);
	    };

#ifndef NO_DEBUG_VIS
	  ostream& output(ostream& str) const
	    {
	      str << _nativeName << "/" << _arity << " <-- " << _id << "\n";
	      return str;
	    };
#endif	  


	private:
	  bool _idIsSet;
	  ulong _id;
	  const char* _nativeName;
	  ulong _arity;
	  BinaryFunctionInterpretation _binFunInterpretation;
	  UnaryFunctionInterpretation _unFunInterpretation;
	  BinaryPredicateInterpretation _binPredInterpretation;
	};

      typedef BK::Array<BK::GlobAlloc,NativeSymbolDescriptor*,32UL,BuiltInFloatingPointArithmetic> InterpretationMap;

    private:
      BuiltInFloatingPointArithmetic() 
	{
	};      
      NativeSymbolDescriptor* interpretation(ulong symbolId);
      bool canBeEvaluated(const Flatterm* term);
      bool evaluateTerm(const Flatterm* term,TmpLiteral* newLit);
      bool evaluateAtom(const Flatterm* term,bool& value);
      static double binaryPlusInterpretation(const double& arg1,
					     const double& arg2);
      
      static double unaryPlusInterpretation(const double& arg);

      static double binaryMinusInterpretation(const double& arg1,
					     const double& arg2);

      static double unaryMinusInterpretation(const double& arg);

      static double multiplyInterpretation(const double& arg1,
					   const double& arg2);
      static double divideInterpretation(const double& arg1,
					 const double& arg2);

      static double min2Interpretation(const double& arg1,
					 const double& arg2);

      static double max2Interpretation(const double& arg1,
					 const double& arg2);

      static bool greaterInterpretation(const double& arg1,
					const double& arg2);
      static bool greaterOrEqualInterpretation(const double& arg1,
					       const double& arg2);

      static bool lessInterpretation(const double& arg1,
				     const double& arg2);
      
      static bool lessOrEqualInterpretation(const double& arg1,
					    const double& arg2);
      
    private:      
      Clause::NewNumber* _clauseNumberGenerator;
      Clause* _fact;
      NativeSymbolDescriptor _binaryPlusDesc;
      NativeSymbolDescriptor _unaryPlusDesc;
      NativeSymbolDescriptor _binaryMinusDesc;
      NativeSymbolDescriptor _unaryMinusDesc;
      NativeSymbolDescriptor _multiplyDesc;
      NativeSymbolDescriptor _divideDesc;
      NativeSymbolDescriptor _min2Desc;
      NativeSymbolDescriptor _max2Desc;
      NativeSymbolDescriptor _greaterDesc;
      NativeSymbolDescriptor _greaterOrEqualDesc;
      NativeSymbolDescriptor _lessDesc;
      NativeSymbolDescriptor _lessOrEqualDesc;
      InterpretationMap _symbolInterpretation;
    }; // class BuiltInFloatingPointArithmetic
}; // namespace VK


//=================================================================
#endif
