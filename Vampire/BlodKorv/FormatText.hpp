//
// File:         FormatText.hpp
// Description:  Splits a text into lines and removes extra white space.  
// Created:      Feb 2, 2000, 16:45
// Author:       Alexandre Riazanov
// mail:         riazanov@cs.man.ac.uk
//=================================================================
#ifndef FORMAT_TEXT_H
//=================================================================
#define FORMAT_TEXT_H 
#include <cstring>
#include "jargon.hpp"
#include "BlodKorvDebugFlags.hpp"
#include "ClassDesc.hpp"
#include "Malloc.hpp"
#include "GList.hpp"
#include "DestructionMode.hpp"
//=================================================================
#undef DEBUG_NAMESPACE
#ifdef DEBUG_FORMAT_TEXT
 #define DEBUG_NAMESPACE "FormatText"
#endif
#include "debugMacros.hpp" 

//=================================================================
namespace BK 
{
class FormatText
{
 public:
  typedef GList<Malloc,char*,FormatText> StrList;
 public:
  FormatText(const char* str) 
   : words(0),
     currLine(new char[1])
  {
   readWords(str);
  };
  ~FormatText() 
    { 
      if (DestructionMode::isThorough())
	{
	  destroyWords(); 
	  delete [] currLine; 
	};
    };
  const char* nextLine(long lineLength);
 private: 
  FormatText() {};
  void readWords(const char* str);
  void destroyWords();
  bool spaceChar(char ch) { return (ch == ' ') || (ch == '\n') || (ch == '\t'); };
 private:
  StrList* words;
  char* currLine;
}; // class FormatText 

}; // namespace BK

//=================================================================
#endif
