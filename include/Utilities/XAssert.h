#ifndef XASSERT_H
#define XASSERT_H

#include "XGlobal.h"
#include "Utilities/XProperty.h"
#include "Utilities/XCodeLocation.h"

#define X_ASSERT_VALUE_HANDLING 1

#if X_ASSERT_VALUE_HANDLING
# include <sstream>
#endif


namespace Eks
{

namespace detail
{

class EKSCORE_EXPORT Assert
  {
public:
  class Argument
    {
  public:
    const char *name;
#if X_ASSERT_VALUE_HANDLING
    std::string value;
#endif
    };

XProperties:
  XRORefProperty(DetailedCodeLocation, location);
  XROProperty(const char *, condition);
  XROProperty(const char *, message);

public:
  Assert(const DetailedCodeLocation &location, const char *condition, const char *message);

  template <typename T> Assert &arg(const char *name, const T &val)
    {
    Argument &arg = _arguments[_argCount++];

    arg.name = name;

#if X_ASSERT_VALUE_HANDLING
      {
      // using std:: classes here as Eks classes use assert.
      std::ostringstream str;
      str << val;
      arg.value = str.str();
      }
#else
    (void)val;
#endif

    return *this;
    }

  typedef bool FireFunction(const Assert &assert);
  static FireFunction *currentFireFunction();
  static void setCurrentFireFunction(FireFunction *f);

#ifdef Q_CC_MSVC
  typedef void __stdcall BreakFunction();
#else
  typedef void BreakFunction();
#endif
  static BreakFunction *currentBreakFunction();
  static void setCurrentBreakFunction(BreakFunction *f);

private:
  static bool defaultFire(const Assert &a);

  enum
    {
    MaxArgs = 16
    };
  Argument _arguments[MaxArgs];
  xsize _argCount;
  };

}

}

#define X_ASSERT_VARIABLE(variable, aa, n) .arg(#variable, variable)

#define X_CONSTRUCT_ASSERT(condition, message, ...) Eks::detail::Assert ass(X_CURRENT_CODE_LOCATION_DETAILED, #condition, message); (void)ass X_EXPAND_ARGS(X_ASSERT_VARIABLE, 0, __VA_ARGS__)

#if defined(X_WIN) && !defined(X_ARCH_ARM)
# define X_DEBUG_BREAK DebugBreak
#elif __clang__
#define X_DEBUG_BREAK __builtin_debugtrap()
#else
# define X_DEBUG_BREAK Eks::detail::Assert::currentBreakFunction()()
#endif

#if X_ASSERTS_ENABLED
# define xAssertMessage(condition, message, ...) if(!(condition) && Eks::detail::Assert::currentFireFunction()) { X_CONSTRUCT_ASSERT(condition, message, __VA_ARGS__); if(Eks::detail::Assert::currentFireFunction()(ass) ) { X_DEBUG_BREAK; } }
# define xVerify xAssert
#define X_USED_FOR_ASSERTS(x) x

#else
# define xAssertMessage(condition, message, ...)
# define xVerify(x, ...) x
#define X_USED_FOR_ASSERTS(x)

template <bool Condition>
struct xCompileTimeAssertDef
{
    enum { Value = 1 };
};
#endif

#define xCompileTimeAssert(x) static_assert(x, "Static Assert: " #x)

#define xAssert(condition, ...) xAssertMessage((condition), "", __VA_ARGS__)
#define xAssertFail(...) xAssert(0, __VA_ARGS__);
#define xAssertFailMessage(msg, ...) xAssertMessage(0, msg, __VA_ARGS__);




// xAssert Usage
// xAssert(i > 10, i)
// xAssertMessage(i > 10, "Oh noes i is too small", i)
// xAssertMessage(i > 10, "Oh noes (%1, %2, %3) is too small", i, j, k)


#endif // XASSERT_H
