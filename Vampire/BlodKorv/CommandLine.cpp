//
// File:         CommandLine.cpp
// Description:  Parsing command line and printing usage.
// Created:      Apr 12, 2001, 18:10
// Author:       Alexandre Riazanov
// mail:         riazanov@cs.man.ac.uk
//
//======================================================================================
#include "CommandLine.hpp"
#include "DestructionMode.hpp"
#include "FormatText.hpp"
//======================================================================================
using namespace BK; 

#ifdef DEBUG_ALLOC_OBJ_TYPE


 ClassDesc CommandLine::StrList::_classDesc("CommandLine::StrList",
                                            CommandLine::StrList::minSize(),
                                            CommandLine::StrList::maxSize()); 
 
 ClassDesc CommandLine::OptDescList::_classDesc("CommandLine::OptDescList",
                                                CommandLine::OptDescList::minSize(),
                                                CommandLine::OptDescList::maxSize());
                  
#endif
//======================================================================================
//======================================================================================
#undef DEBUG_NAMESPACE
#ifdef DEBUG_COMMAND_LINE
 #define DEBUG_NAMESPACE "CommandLine"
#endif
#include "debugMacros.hpp"
//======================================================================================


CommandLine::CommandLine() 
  : optDescs(0),
    nonoptPrefSize(0),
    nonoptPrefNames(0),
    nonoptPrefComments(0),
    nonoptSuffSize(0),
    nonoptSuffNames(0),
    nonoptSuffComments(0),
    nonoptMiddleName(0),
    nonoptMiddleComment(0),
    nonoptMidValues(0),
    hidingChar('+')
{
};

CommandLine::~CommandLine() 
{
  CALL("destructor ~CommandLine()"); 
  if (DestructionMode::isThorough())
    {
      OptDescList::destroyList(optDescs);
      StrList::destroyList(nonoptMidValues);
    };
};



void CommandLine::option(const char* optName,const char* cmt) // option without argument  
{
  optDescs = new OptDescList(OptDesc(optName,cmt),optDescs); 
};

void CommandLine::option(const char* optName,ParamType paramType,const char* paramName,const char* cmt)
{
  optDescs = new OptDescList(OptDesc(optName,paramType,paramName,cmt),optDescs); 
};

void CommandLine::option(const char* optName,const Enum* paramType,const char* paramName,const char* cmt)
{
  optDescs = new OptDescList(OptDesc(optName,paramType,paramName,cmt),optDescs); 
};


void CommandLine::nonoptPrefix(int num,
			       const char* const * names,
			       const char* const * comments)
{
  nonoptPrefSize = num;
  nonoptPrefNames = names;
  nonoptPrefComments = comments;
};
void CommandLine::nonoptSuffix(int num,
			       const char* const * names,
			       const char* const * comments)
{
  nonoptSuffSize = num;
  nonoptSuffNames = names;
  nonoptSuffComments = comments;
};
void CommandLine::nonoptMiddle(const char* name,const char* comment)
{
  nonoptMiddleName = name;  
  nonoptMiddleComment = comment;
};

// parsing arguments  

void CommandLine::arguments(int argC,const char* const * argV)
{
  argc = 1;
  int i;
  for (i = 1; i < argC; i++) if (argV[i][0] != hidingChar) argc++;
  argv = new const char* [argc]; 
  argv[0] = argV[0];
  int j = 1;
  for (i = 1; i < argC; i++)
    if (argV[i][0] != hidingChar)
      {
	argv[j] = argV[i];
	j++;
      };
};

bool CommandLine::parse() { return (optParsSuccess = parseOptions()) && parseNonopt(); };
 
  // getting values

CommandLine::Values CommandLine::values(const char* optName) const
{
  CALL("values(const char* optName) const");
  OptDescList* desc = findDesc(optName,(strlen(optName) == 1));
  if (desc) return Values(desc->hd().values);
  DOP(DMSG("Option ") << optName << " was not described.\n");
  ICP("ICP1"); 
  return Values(0);
};

const char* CommandLine::nonoptVal(const char* name) const
{
  int i = findStr(name,nonoptPrefSize,nonoptPrefNames);
  if (i >= 0) return argv[nonoptArg + i];  
  i = findStr(name,nonoptSuffSize,nonoptSuffNames);
  if (i >= 0)
    {     
      return argv[argc - (nonoptSuffSize - i)];      
    };
  return 0;
};

CommandLine::Values CommandLine::nonoptMiddleValues() const { return Values(nonoptMidValues); };



bool CommandLine::parseOptions()
{
 int argn = 1;
 const char* arg;
 OptDescList* desc;

 next_arg: 
  if (argn >= argc) { nonoptArg = argn; return true; }; // all arguments are options
  arg = argv[argn];
  if (arg[0] != '-') { nonoptArg = argn; return true; };  
  // arg is an option
  if (arg[1] == '-') // arg is a long option or "--"
   {
    if (!(arg[2])) { nonoptArg = argn + 1; return true; }; // explicit end of options   
    desc = findDesc(arg + 2,false);    
    if (!desc) { nonoptArg = argn; return false; };  
    if (desc->hd().parName) // long option with parameter
     {    
      if (strlen(desc->hd().name) < strlen(arg + 2)) 
       {// treat the rest of arg as a parameter
	if (!checkType(arg + (2 + strlen(desc->hd().name)),
		       desc->hd().ptype,
		       desc->hd().possibleValues))     
	 { nonoptArg = argn; return false; };
        desc->hd().addVal(arg + (2 + strlen(desc->hd().name)));       
        argn++;
        goto next_arg;        
       }
      else // read parameter from the next argument 
       {
	argn++;
	if ((argn >= argc) || 
	    (!checkType(argv[argn],
			desc->hd().ptype,
			desc->hd().possibleValues)))
         { nonoptArg = argn; return false; };                   
        desc->hd().addVal(argv[argn]);
        argn++;
        goto next_arg;
       };
     }
    else // long option without parameter
     {
      if (strlen(desc->hd().name) != strlen(arg + 2)) 
       { 
        nonoptArg = argn; return false;     
       };
      desc->hd().addVal(arg);       
      argn++;
      goto next_arg;
     };
   } 
  else // arg is a collection of short options or the non-option "-"
   {
    if (!(arg[1])) { nonoptArg = argn; return true; };   
    for (unsigned c = 1; c < strlen(arg); c++)
     {     
      desc = findDesc(arg + c,true);

      if (!desc) { nonoptArg = argn; return false; };     
      if (desc->hd().parName)
       {// short option with parameter      
	if (c + 1 < strlen(arg))
	 {// treat the rest of arg as a parameter
	  if (!checkType(arg + (c + 1),
			 desc->hd().ptype,
			 desc->hd().possibleValues)) 
           { nonoptArg = argn; return false; };        
	  desc->hd().addVal(arg + (c + 1));           
          argn++;
          goto next_arg;
         }
        else// read parameter from the next argument
	 {
          argn++;
	  if ((argn >= argc) || 
	      (!checkType(argv[argn],
			  desc->hd().ptype,
			  desc->hd().possibleValues))) 
           { nonoptArg = argn; return false; };                                
          desc->hd().addVal(argv[argn]);
          argn++;
          goto next_arg;
         };
       }
      else // short option without parameter
       {
	desc->hd().addVal(arg);
       };
     };

    argn++;
    goto next_arg;
   };
}; // bool CommandLine::parseOptions() const 

CommandLine::OptDescList* CommandLine::findDesc(const char* optName,bool shortOpt) const
{
 OptDescList* lst = optDescs;
 while (lst)
  {
   if ((strpref(lst->hd().name,optName)) && (lst->hd().shortOpt == shortOpt)) return lst;
   lst = lst->tl(); 
  };
 return 0;
}; // OptDescList* CommandLine::findDesc(const char* optName)

bool CommandLine::checkType(const char* str,ParamType tp,const Enum* possibleValues)
{
 bool dummyBool;
 char* endptr;

  switch(tp)
  { 
    case Long: 
      if (Values::readInf(str,dummyBool)) return true;
      strtol(str,&endptr,10);
      return (endptr != str);
    case Double: 
      if (Values::readInf(str,dummyBool)) return true;
      strtod(str,&endptr);
      return (endptr != str);
      
    case String:
      if (possibleValues)
	{
	  for (const Enum* pv = possibleValues;
	       pv;
	       pv = pv->_next)
	    {
	      if (!strcmp(pv->_val,str)) return true;
	    };
	  return false;
	};
      return true;
    case Flag: 
      return (!strcmp(str,"on")) || (!strcmp(str,"off"));
    default : return false;
  };

}; // bool CommandLine::checkType(const char* str,ParamType tp,const Enum* possibleValues)

int CommandLine::findStr(const char* str,int arrSize,const char* const *arr)
{
 for (int i = 0; i < arrSize; i++) if (!strcmp(str,arr[i])) return i; 
 return -1;
}; // int CommandLine::findStr(const char* str,int arrSize,const char* const *arr)    

bool CommandLine::Values::next(const char*& str)
{
 if (bool(*this))
  {
   str = StrListIter::currEl();   
   StrListIter::next();
   return true;
  };
 return false;
}; // bool CommandLine::Values::next(const char*& str)

bool CommandLine::Values::next(long& n)
{
 if (bool(*this))
  {  
   bool posInf;
   if (readInf(StrListIter::currEl(),posInf))
    {
     n = (posInf) ? LONG_MAX : LONG_MIN;
     StrListIter::next();
     return true;
    };
    
    const char* str = StrListIter::currEl();
    StrListIter::next();
  
    n = strtol(str,0,10);
    return true;
  };
 return false; 
}; // bool CommandLine::Values::next(long& n)

bool CommandLine::Values::next(double& n)
{
 if (bool(*this))
  {  
   bool posInf;
   if (readInf(StrListIter::currEl(),posInf))
    {
     n = (posInf) ? DBL_MAX : DBL_MIN;
     StrListIter::next();
     return true;
    };
   const char* str = StrListIter::currEl();
   StrListIter::next();
   n = strtod(str,0);
   return true;
  };
 return false; 
}; // bool CommandLine::Values::next(double& n)


bool CommandLine::Values::next(bool& flag)
{ 
  CALL("Values::next(bool& flag)");
  if (bool(*this))
    {   
      flag = !strcmp(StrListIter::currEl(),"on");
      ASSERT(flag || !strcmp(StrListIter::currEl(),"off"));
      StrListIter::next();
      return true;
    };
  return false; 
}; // bool CommandLine::Values::next(bool& flag)

void CommandLine::printUsage(const char* command) const
{
 OptDescList* tmpOptDescs = OptDescList::inv(optDescs);
 cout << "usage: " << command << " ";
 if (tmpOptDescs) cout << "[options] [--] ";
 int i;
 for (i = 0; i < nonoptPrefSize; i++) cout << nonoptPrefNames[i] << " ";
 if (nonoptMiddleName) cout << nonoptMiddleName << " .. " << nonoptMiddleName << " ";
 for (i = 0; i < nonoptSuffSize; i++) cout << nonoptSuffNames[i] << " ";  
 cout << "\n";
 if (tmpOptDescs)
  {
   cout << "options: "; 
   int currEnd = 9;
   int scount;
   for(OptDescList* desc = tmpOptDescs; desc; desc = desc->tl())
    {
     scount = strlen(desc->hd().name);
     if (desc->hd().shortOpt) { scount++; } else scount += 2; 
     if (desc->hd().parName) scount += (strlen(desc->hd().parName) + 1);  
     if (desc->tl()) scount += 2; 

     if (scount + currEnd > 90) { currEnd = 9 + scount; cout << "\n         "; };  
     currEnd += scount;
     cout << "-" << ((desc->hd().shortOpt) ? "" : "-");
     cout << desc->hd().name;
     if (desc->hd().parName) cout << " " << desc->hd().parName;  
     if (desc->tl()) cout << ", "; 
    };
   cout << "\n";
  };
 OptDescList::destroyList(tmpOptDescs);
}; // void CommandLine::printUsage() const

void CommandLine::printHelp(const char* command) const
{
 printUsage(command);
 unsigned indent;
 const long stdLineLength = 80;

 // comments on nonoption arguments
 if (nonoptPrefNames)
  {
   for (int i = 0; i < nonoptPrefSize; i++)
    {
     cout << "  " << nonoptPrefNames[i] << " ";
     indent = strlen(nonoptPrefNames[i]) + 3;
     FormatText fcomm(nonoptPrefComments[i]);
     const char* cmtLine = fcomm.nextLine(stdLineLength - indent);
     if (cmtLine) 
      { 
       cout << cmtLine << "\n"; 
       while ((cmtLine = fcomm.nextLine(stdLineLength - 5))) 
        cout << "     " << cmtLine << "\n";
      }
     else cout << "\n";     
    };
  };

 if (nonoptMiddleName) 
  {
   cout << "  " << nonoptMiddleName << ".." << nonoptMiddleName << " ";   
   indent = 2*strlen(nonoptMiddleName) + 5;
   FormatText fcomm(nonoptMiddleComment);
   const char* cmtLine = fcomm.nextLine(stdLineLength - indent);
   if (cmtLine) 
    { 
     cout << cmtLine << "\n"; 
     while ((cmtLine = fcomm.nextLine(stdLineLength - 5))) 
      cout << "     " << cmtLine << "\n";
    }
   else cout << "\n";   
  };

 if (nonoptSuffNames)
  {
   for (int i = 0; i < nonoptSuffSize; i++)
    {
     cout << "  " << nonoptSuffNames[i] << " ";
     indent = strlen(nonoptSuffNames[i]) + 3;
     FormatText fcomm(nonoptSuffComments[i]);
     const char* cmtLine = fcomm.nextLine(stdLineLength - indent);
     if (cmtLine) 
      { 
       cout << cmtLine << "\n"; 
       while ((cmtLine = fcomm.nextLine(stdLineLength - 5))) 
        cout << "     " << cmtLine << "\n";
      }
     else cout << "\n";     
    };
  };
 
  // comments on options

 OptDescList* tmpOptDescs = OptDescList::inv(optDescs);
 if (tmpOptDescs)
  {
   cout << "\n";   
   for (OptDescList* desc = tmpOptDescs; desc; desc = desc->tl())
    {   
     cout << "  " << "-";
     indent = 3;
     if (!desc->hd().shortOpt) { cout << "-"; indent++; };
     cout << desc->hd().name;
     indent += strlen(desc->hd().name);
     if (desc->hd().parName) 
      { 
       cout << " " << desc->hd().parName; 
       indent += (strlen(desc->hd().parName) + 1);
      };
     cout << " ";
     indent++;
     FormatText fcomm(desc->hd().comment);
     const char* cmtLine = fcomm.nextLine(stdLineLength - indent);
     if (cmtLine) 
      { 
       cout << cmtLine << "\n"; 
       while ((cmtLine = fcomm.nextLine(stdLineLength - 5))) cout << "     " << cmtLine << "\n";
      }
     else cout << "\n";
   };
  };
 OptDescList::destroyList(tmpOptDescs);
}; // void CommandLine::printHelp(const char* command) const

void CommandLine::printError() const
{
 if (!optParsSuccess) 
  {
   cout << "Error in " << argv[nonoptArg] << " : can not parse options.\n"; 
  }
 else
  {
   cout << "Error after " << argv[nonoptArg - 1] << " : wrong number of non-option arguments.\n"; 
  };
}; // void CommandLine::printError() const

ostream& CommandLine::printCommand(ostream& str) const
{
 for (int i = 0; i < argc; i++) str << argv[i] << ' ';
 return str;
}; // ostream& CommandLine::printCommand(ostream& str) const



bool CommandLine::parseNonopt()
{
  int numOfNonopt = argc - nonoptArg;   
  if (nonoptMiddleName) 
    if (numOfNonopt >= nonoptPrefSize + nonoptSuffSize)
      {
	for (int i = (argc - nonoptSuffSize) - 1; i >= nonoptArg + nonoptPrefSize; i--) 
	  nonoptMidValues = new StrList(argv[i],nonoptMidValues);  
	return true;
      }
    else return false;
  return (numOfNonopt == nonoptPrefSize + nonoptSuffSize);
}; 



bool CommandLine::strpref(const char* s1,const char* s2)
{ 
  return (strlen(s1) <= strlen(s2)) && (!strncmp(s1,s2,strlen(s1)));
}; 

const CommandLine::Enum* CommandLine::val(const char* val)
{
  return new Enum(val);
};

const CommandLine::Enum* CommandLine::val(const char* val,const Enum* next)
{
  return new Enum(val,next);
};


//======================================================================================
#undef DEBUG_NAMESPACE
#ifdef DEBUG_COMMAND_LINE
 #define DEBUG_NAMESPACE "CommandLine::Values"
#endif
#include "debugMacros.hpp"
//======================================================================================

CommandLine::Values::Values() : StrListIter() 
{
};

CommandLine::Values::Values(StrList* lst) : StrListIter(lst) 
{
};

CommandLine::Values::operator bool() const { return StrListIter::rest() != 0; };


bool CommandLine::Values::readInf(const char* str,bool& positive)
{   
  if (!strcmp("inf",str)) { positive = true; return true; };
  if (!strcmp("+inf",str)) { positive = true; return true; };
  positive = false;
  return !strcmp("-inf",str);
}; // bool CommandLine::Values::readInf(const char* str,bool& positive)

//======================================================================================
#undef DEBUG_NAMESPACE
#ifdef DEBUG_COMMAND_LINE
 #define DEBUG_NAMESPACE "CommandLine::OptDesc"
#endif
#include "debugMacros.hpp"
//======================================================================================



CommandLine::OptDesc::OptDesc(const char* optName,const char* cmt) 
  : name(optName), parName(0), values(0), comment(cmt)
{
  shortOpt = (strlen(optName) == 1);    
};

CommandLine::OptDesc::OptDesc(const char* optName,ParamType paramType,const char* paramName,const char* cmt) 
  : name(optName), ptype(paramType), parName(paramName), values(0), comment(cmt)
{
  shortOpt = (strlen(optName) == 1); 
  possibleValues = 0;
};   

CommandLine::OptDesc::OptDesc(const char* optName,const Enum* paramType,const char* paramName,const char* cmt) 
  : name(optName), ptype(String), parName(paramName), values(0), comment(cmt)
{
  shortOpt = (strlen(optName) == 1);    
  possibleValues = paramType;
};   

void CommandLine::OptDesc::addVal(const char* val)
{
  values = new StrList(val,values);
};

//======================================================================================
#undef DEBUG_NAMESPACE
#ifdef DEBUG_COMMAND_LINE
 #define DEBUG_NAMESPACE "CommandLine::EnumVal"
#endif
#include "debugMacros.hpp"
//======================================================================================



CommandLine::Enum::Enum(const char* val) :
  _val(val),
  _next(0)
{
}; 

CommandLine::Enum::Enum(const char* val,const Enum* next) :
  _val(val),
  _next(next)
{
};

void* CommandLine::Enum::operator new(size_t size)
{
  return Malloc::allocate(size);
};


//======================================================================================



