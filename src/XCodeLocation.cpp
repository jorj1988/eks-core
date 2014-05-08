#include "Utilities/XCodeLocation.h"
#include "Containers/XStringSimple.h"
#include "Containers/XStringBuffer.h"

#if defined(X_DEBUG) && !defined(X_ARCH_ARM)
# define X_ENABLE_STACK_WALKING 1
#else
# define X_ENABLE_STACK_WALKING 0
#endif

#if defined(Q_CC_MSVC) && X_ENABLE_STACK_WALKING
# include <Windows.h>
# include <DbgHelp.h>
#endif

namespace Eks
{

CodeLocation::CodeLocation(const char *file, xuint32 line, const char *function)
    : _file(file), _line(line), _function(function)
  {
  }

CodeLocation::CodeLocation(const CodeLocation &l)
  : _file(l._file), _line(l._line), _function(l._function)
  {
  }

String CodeLocation::toString() const
  {
  String out;

  out += file();
  out += ":";
  out.appendType(line());
  out += " - ";
  out += function();

  return out;
  }

bool CodeLocation::operator==(const CodeLocation &a) const
  {
  return line() == a.line() &&
    strcmp(file(), a.file()) == 0 &&
    strcmp(function(), a.function()) == 0;
  }

void StackWalker::intialiseSymbolNames()
  {
#if X_ENABLE_STACK_WALKING
#ifdef Q_CC_MSVC
  SymInitialize(::GetCurrentProcess(), 0, true);
#endif
#endif
  }

void StackWalker::terminateSymbolNames()
  {
#if X_ENABLE_STACK_WALKING
#ifdef Q_CC_MSVC
  SymCleanup(::GetCurrentProcess());
#endif
#endif
  }

void StackWalker::getSymbolName(
    void *X_USED_FOR_ASSERTS(symbolLocation),
    Eks::String &X_USED_FOR_ASSERTS(symbolName),
    xsize X_USED_FOR_ASSERTS(maxSize))
  {
#if X_ENABLE_STACK_WALKING
#ifdef Q_CC_MSVC
  BYTE *buffer = (BYTE *)alloca(sizeof(SYMBOL_INFO) + maxSize);
  memset(buffer, 0, sizeof(buffer));

  PSYMBOL_INFO symbol = (PSYMBOL_INFO)buffer;
  symbol->SizeOfStruct = sizeof(SYMBOL_INFO);
  symbol->MaxNameLen = (ULONG)maxSize;

  DWORD64 symDisplacement = 0;
  if (!SymFromAddr(::GetCurrentProcess(),
                   reinterpret_cast<xsize>(symbolLocation),
                   &symDisplacement,
                   symbol))
    {
    symbol->Name[0] = 0;
    symDisplacement = 0;
    }

  symbolName.clear();
  symbolName.resizeAndCopy(strlen(symbol->Name), symbol->Name);

#else
  (void)symbolLocation;
  (void)symbolName;
  (void)maxSize;
#endif
#else
#endif
  }

void StackWalker::walk(
    xsize X_USED_FOR_ASSERTS(skip),
    Visitor *X_USED_FOR_ASSERTS(visit))
  {
#if X_ENABLE_STACK_WALKING
#ifdef Q_CC_MSVC

  struct Utils
    {
    static void doVisit(Visitor *visit, xsize i, void *pcOffset)
      {
      visit->visit(i, pcOffset);
      }
    };

  enum
    {
    BlockCaptureSize = 16
    };

  xsize position = skip + 1;
  const xsize start = position;
  void *frames[BlockCaptureSize];

  xuint16 captured = 0;
  do
    {
    captured = CaptureStackBackTrace((DWORD)position, BlockCaptureSize, frames, 0);

    for(xsize i = 0; i < captured; ++i)
      {
      Utils::doVisit(visit, position + i - start, frames[i]);
      }

    position += captured;
    } while(captured == BlockCaptureSize);

#else
  (void)skip;
  (void)visit;
#endif
#endif
  }
}




