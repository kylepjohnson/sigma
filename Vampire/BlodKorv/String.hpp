//
// File:         String.hpp
// Description:  Strings with natural semantics of ==,< and >. 
// Created:      Sep 30, 2002, Alexandre Riazanov, riazanov@cs.man.ac.uk
//============================================================================
#ifndef STRING_H
#define STRING_H 
//============================================================================
#include <cstring>
#include <iostream>
#include "jargon.hpp"
#include "BlodKorvDebugFlags.hpp"
#include "DestructionMode.hpp"
//============================================================================
#ifdef DEBUG_STRING
#undef DEBUG_NAMESPACE
#  define DEBUG_NAMESPACE "String<class Alloc>"
#endif
#include "debugMacros.hpp"
//============================================================================

namespace BK 
{
  template <class Alloc>
  class String
  {
  public:
    String() { init(); };
    String(const char* c) { init(c); };
    String(const String& str) { init(str); };
    ~String() { destroy(); };
    void init();
    void init(const char* c);
    void init(const String& str); 
    void destroy();
    String& operator=(const String& str);
    void* operator new(size_t);
    void operator delete(void* obj);
    const char* content() const;
    ulong length() const;
    bool operator==(const String& str);
    bool operator>(const String& str);
    bool operator>=(const String& str);
    bool operator<(const String& str);
    bool operator<=(const String& str);
  private:
    char* _content;
  }; // class String

template <class Alloc>
void String<Alloc>::init()
{
  _content = 0;
};

template <class Alloc>
void String<Alloc>::init(const char* c)
{
  _content = static_cast<char*>(Alloc::allocate(strlen(c) + 1));
  strcpy(_content,c);
};

template <class Alloc>
void String<Alloc>::init(const String& str)
{
  if (str._content) 
    {
      init(str._content);
    }
  else
    init();
};

template <class Alloc>
void String<Alloc>::destroy()
{
  CALL("destroy()");
  if (DestructionMode::isThorough())
    if (_content)
      {
	Alloc::deallocate(static_cast<void*>(_content),strlen(_content) + 1);
      };
  BK_CORRUPT(_content);
};


template <class Alloc>
String<Alloc>& String<Alloc>::operator=(const String& str)
{
  destroy();
  init(str);
};
  
template <class Alloc>
void* String<Alloc>::operator new(size_t)
{
  return Alloc::allocate(sizeof(String));
};

template <class Alloc>
void String<Alloc>::operator delete(void* obj)
{
  Alloc::deallocate(obj,sizeof(String));
};


template <class Alloc>
const char* String<Alloc>::content() const
{
  if (_content) return _content;
  return "";
};

      
template <class Alloc>
ulong String<Alloc>::length() const
{
  return strlen(content());
}; 

template <class Alloc>
bool String<Alloc>::operator==(const String& str)
{
  return strcmp(content(),str.content()) == 0;
};

template <class Alloc>
bool String<Alloc>::operator>(const String& str)
{
  return strcmp(content(),str.content()) > 0;
};

template <class Alloc>
bool String<Alloc>::operator>=(const String& str)
{  
  return strcmp(content(),str.content()) >= 0;
};

template <class Alloc>
bool String<Alloc>::operator<(const String& str)
{
  return strcmp(content(),str.content()) < 0;
};

template <class Alloc>
bool String<Alloc>::operator<=(const String& str)
{
  return strcmp(content(),str.content()) <= 0;
};

}; // namespace BK 

namespace std
{
  template <class Alloc>
  ostream& operator<<(ostream& stream,const BK::String<Alloc>& string)
  {
    return stream << string.content(); 
  };
}; // namespace std



//============================================================================
#endif
