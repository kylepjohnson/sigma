//
// File:         CommandLine.hpp
// Description:  Parsing command line and printing usage.
// Created:      Jan 25, 2000, 15:22
// Author:       Alexandre Riazanov
// mail:         riazanov@cs.man.ac.uk
//

#ifndef COMMAND_LINE_H
//======================================================================================
#define COMMAND_LINE_H
#include <cstring>
#include <climits>
#include <cfloat> 
#include <iostream>
#include "jargon.hpp"
#include "BlodKorvDebugFlags.hpp"
#include "GList.hpp"
#include "Malloc.hpp"
using namespace std;
//======================================================================================
#undef DEBUG_NAMESPACE
#ifdef DEBUG_COMMAND_LINE
#define DEBUG_NAMESPACE "CommandLine"
#endif
#include "debugMacros.hpp"
//======================================================================================
namespace BK 
{

class CommandLine
{
 public:
  typedef GList<Malloc,const char*,CommandLine> StrList;
  typedef StrList::Iter StrListIter;

 public: 
  enum ParamType { String, Long, Double, Flag };
  class Enum
    {
    public:
      Enum(const char* val);
      Enum(const char* val,const Enum* next);
      Enum() {};
      void* operator new(size_t);
      const char* _val;
      const Enum* _next;
      friend class CommandLine;
    };
  class Values : private StrListIter
    {
    public:
      Values();
      operator bool() const; 
      bool next(const char*& str);
      bool next(long& n);
      bool next(double& n); 
      bool next(bool& flag);
    private:
      Values(StrList* lst);
      static bool readInf(const char* str,bool& positive);
      friend class CommandLine; 
    }; // class Values
 public:  
  CommandLine();
  ~CommandLine();
 
  void setHidingChar(char ch);

  // describing arguments

  static const Enum* val(const char* val);
  static const Enum* val(const char* val,const Enum* next);

  void option(const char* optName,const char* cmt);
  void option(const char* optName,ParamType paramType,const char* paramName,const char* cmt);
  
  void option(const char* optName,const Enum* paramType,const char* paramName,const char* cmt);



  void nonoptPrefix(int num,
                    const char* const * names,
                    const char* const * comments);
  void nonoptSuffix(int num,
                    const char* const * names,
                    const char* const * comments);
  void nonoptMiddle(const char* name,const char* comment);

  // parsing arguments  

  void arguments(int argC,const char* const * argV);

  bool parse();
 
  // getting values

  Values values(const char* optName) const;

  const char* nonoptVal(const char* name) const;

  Values nonoptMiddleValues() const;

  // getting help

  ostream& printCommand(ostream& str) const;
  void printUsage(const char* command) const;
  void printHelp(const char* command) const;
  void printError() const;
   
 private:
  struct OptDesc 
  { 
    OptDesc(const char* optName,const char* cmt);
    OptDesc(const char* optName,ParamType paramType,const char* paramName,const char* cmt);   
    OptDesc(const char* optName,const Enum* paramType,const char* paramName,const char* cmt);   

    void addVal(const char* val);

    bool shortOpt;
    const char* name;
    const Enum* possibleValues;
    ParamType ptype;
    const char* parName;
    StrList* values; 
    const char* comment;
  };
  
 public: // made public to please VC++
  typedef GList<Malloc,OptDesc,CommandLine> OptDescList;  
 private:
  bool parseOptions();
  bool parseNonopt();
  OptDescList* findDesc(const char* optName,bool shortOpt) const;     
  static bool strpref(const char* s1,const char* s2);
  static bool checkType(const char* str,ParamType tp,const Enum* possibleValues);
  static int findStr(const char* str,int arrSize,const char* const *arr); 
 private:
  OptDescList* optDescs;
  int argc;
  const char** argv; 
  int nonoptArg;
  int nonoptPrefSize;
  const char* const * nonoptPrefNames;
  const char* const * nonoptPrefComments;
  int nonoptSuffSize;
  const char* const * nonoptSuffNames;
  const char* const * nonoptSuffComments;
  const char* nonoptMiddleName;
  const char* nonoptMiddleComment;
  StrList* nonoptMidValues; 
  bool optParsSuccess;
  char hidingChar;
}; //class CommandLine 


}; // namespace BK


//======================================================================================
#undef DEBUG_NAMESPACE
//=======================================================================================
#endif
