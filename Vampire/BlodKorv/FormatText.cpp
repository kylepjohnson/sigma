//
// File:         FormatText.cpp
// Description:  Splits a text into lines and removes extra white space.  
// Created:      Apr 12, 2001, 20:30
// Author:       Alexandre Riazanov
// mail:         riazanov@cs.man.ac.uk
//=================================================================
#include "FormatText.hpp"
//=================================================================
using namespace BK;
#ifdef DEBUG_ALLOC_OBJ_TYPE
ClassDesc FormatText::StrList::_classDesc("FormatText::StrList::_classDesc",
    					   FormatText::StrList::minSize(),
					   FormatText::StrList::maxSize());
#endif

const char* FormatText::nextLine(long lineLength)
{
 if (!words) return 0; 
 StrList* tmp;
 if (long(strlen(words->hd())) + 2 >= lineLength)
  {
   delete [] currLine;
   currLine = new char[strlen(words->hd()) + 1];
   strcpy(currLine,words->hd());
   delete [] words->hd();
   tmp = words->tl();
   delete words; 
   words = tmp;
   return currLine;
  };

 // line of normal size 
 delete [] currLine;
 currLine = new char[lineLength + 1];
 currLine[lineLength] = (char)0;
 long currSym = 0;
 next_word:
  if ((!words) || (long(strlen(words->hd())) + 1 + currSym > lineLength)) 
   {
    while (currSym < lineLength) { currLine[currSym] = ' '; currSym++; };         
    return currLine;
   };
  currLine[currSym] = ' ';
  strcpy(currLine + currSym + 1,words->hd());
  currSym += 1 + strlen(words->hd());
  delete [] words->hd();
  tmp = words->tl();
  delete words; 
  words = tmp;
  goto next_word;
}; // const char* FormatText::nextLine(long lineLength)

void FormatText::readWords(const char* str)
{
 long n = strlen(str);
 bool spaces = true;
 char ch;
 long wlen = 0;
 const char* word = 0;
 char* tmpWord;

 for (long i = 0; i <= n; i++)
  {
   ch = str[i];
   if (spaces) // skipping spaces
    {
     if (!spaceChar(ch)) // begin new word
      {
       spaces = false;
       word = str + i; 
       wlen = 1;
      };
    }   
   else // reading a word
    {
     if (spaceChar(ch) || (!ch)) // end of the word   
      {
       spaces = true;        
       tmpWord = new char[wlen+1];
       tmpWord[wlen] = (char)0;
       strncpy(tmpWord,word,wlen);
       words = new StrList(tmpWord,words);
      }
     else // continue reading the word
      {
       wlen++;
      };
    };
  };
 
 StrList::inverse(words); 
}; // void FormatText::readWords(const char* str) 

void FormatText::destroyWords() 
{
 StrList* tmp;
 while (words) 
  { 
   delete [] words->hd();
   tmp = words->tl();
   delete words;
   words = tmp;
  };
}; // void FormatText::destroyWords()

//================================================================
