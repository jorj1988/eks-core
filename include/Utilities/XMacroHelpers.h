#ifndef XMACROHELPERS_H
#define XMACROHELPERS_H

#define X_DISABLE_COPY(x) x(const x &); x &operator=(const x &);

#ifdef Q_OS_DARWIN
# define xCurrentFunction __PRETTY_FUNCTION__
#else
# if defined Q_OS_LINUX && __GNUC__
#  if defined __cplusplus ? __GNUC_PREREQ (2, 6) : __GNUC_PREREQ (2, 4)
#   define xCurrentFunction	__PRETTY_FUNCTION__
#  else
#   if defined __STDC_VERSION__ && __STDC_VERSION__ >= 199901L
#    define xCurrentFunction	__func__
#   else
#    define xCurrentFunction	"_No Function Info Available_"
#   endif
#  endif
# else
#  ifdef Q_CC_MINGW
#   define xCurrentFunction __PRETTY_FUNCTION__
#  else
#   define xCurrentFunction __FUNCTION__
#  endif
# endif
#endif

#define X_UNUSED(x)

// concatenates tokens, even when the tokens are macros themselves
#define X_JOIN_HELPER_HELPER(_0, _1)		_0##_1
#define X_JOIN_HELPER(_0, _1)				X_JOIN_HELPER_HELPER(_0, _1)
#define X_JOIN_IMPL(_0, _1)					X_JOIN_HELPER(_0, _1)

#define X_JOIN_2(_0, _1)																	X_JOIN_IMPL(_0, _1)
#define X_JOIN_3(_0, _1, _2)																X_JOIN_2(X_JOIN_2(_0, _1), _2)
#define X_JOIN_4(_0, _1, _2, _3)															X_JOIN_2(X_JOIN_3(_0, _1, _2), _3)
#define X_JOIN_5(_0, _1, _2, _3, _4)														X_JOIN_2(X_JOIN_4(_0, _1, _2, _3), _4)
#define X_JOIN_6(_0, _1, _2, _3, _4, _5)													X_JOIN_2(X_JOIN_5(_0, _1, _2, _3, _4), _5)
#define X_JOIN_7(_0, _1, _2, _3, _4, _5, _6)												X_JOIN_2(X_JOIN_6(_0, _1, _2, _3, _4, _5), _6)
#define X_JOIN_8(_0, _1, _2, _3, _4, _5, _6, _7)											X_JOIN_2(X_JOIN_7(_0, _1, _2, _3, _4, _5, _6), _7)
#define X_JOIN_9(_0, _1, _2, _3, _4, _5, _6, _7, _8)										X_JOIN_2(X_JOIN_8(_0, _1, _2, _3, _4, _5, _6, _7), _8)
#define X_JOIN_10(_0, _1, _2, _3, _4, _5, _6, _7, _8, _9)									X_JOIN_2(X_JOIN_9(_0, _1, _2, _3, _4, _5, _6, _7, _8), _9)
#define X_JOIN_11(_0, _1, _2, _3, _4, _5, _6, _7, _8, _9, _10)								X_JOIN_2(X_JOIN_10(_0, _1, _2, _3, _4, _5, _6, _7, _8, _9), _10)
#define X_JOIN_12(_0, _1, _2, _3, _4, _5, _6, _7, _8, _9, _10, _11)							X_JOIN_2(X_JOIN_11(_0, _1, _2, _3, _4, _5, _6, _7, _8, _9, _10), _11)
#define X_JOIN_13(_0, _1, _2, _3, _4, _5, _6, _7, _8, _9, _10, _11, _12)					X_JOIN_2(X_JOIN_12(_0, _1, _2, _3, _4, _5, _6, _7, _8, _9, _10, _11), _12)
#define X_JOIN_14(_0, _1, _2, _3, _4, _5, _6, _7, _8, _9, _10, _11, _12, _13)				X_JOIN_2(X_JOIN_13(_0, _1, _2, _3, _4, _5, _6, _7, _8, _9, _10, _11, _12), _13)
#define X_JOIN_15(_0, _1, _2, _3, _4, _5, _6, _7, _8, _9, _10, _11, _12, _13, _14)			X_JOIN_2(X_JOIN_14(_0, _1, _2, _3, _4, _5, _6, _7, _8, _9, _10, _11, _12, _13), _14)
#define X_JOIN_16(_0, _1, _2, _3, _4, _5, _6, _7, _8, _9, _10, _11, _12, _13, _14, _15)		X_JOIN_2(X_JOIN_15(_0, _1, _2, _3, _4, _5, _6, _7, _8, _9, _10, _11, _12, _13, _14), _15)


// chooses a value based on a condition
#define X_IF_0(t, f)			f
#define X_IF_1(t, f)			t
#define X_IF(cond, t, f)		X_JOIN_2(X_IF_, X_TO_BOOL(cond))(t, f)


// converts a condition into a boolean 0 (=false) or 1 (=true)
#define X_TO_BOOL_0 0
#define X_TO_BOOL_1 1
#define X_TO_BOOL_2 1
#define X_TO_BOOL_3 1
#define X_TO_BOOL_4 1
#define X_TO_BOOL_5 1
#define X_TO_BOOL_6 1
#define X_TO_BOOL_7 1
#define X_TO_BOOL_8 1
#define X_TO_BOOL_9 1
#define X_TO_BOOL_10 1
#define X_TO_BOOL_11 1
#define X_TO_BOOL_12 1
#define X_TO_BOOL_13 1
#define X_TO_BOOL_14 1
#define X_TO_BOOL_15 1
#define X_TO_BOOL_16 1

#define X_TO_BOOL(x)		X_JOIN_2(X_TO_BOOL_, x)


// Returns 1 if the arguments to the variadic macro are separated by a comma, 0 otherwise.
#define X_HAS_COMMA(...) X_HAS_COMMA_EVAL(X_HAS_COMMA_ARGS(__VA_ARGS__, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 0))
#define X_HAS_COMMA_EVAL(...) __VA_ARGS__
#define X_HAS_COMMA_ARGS(_1, _2, _3, _4, _5, _6, _7, _8, _9, _10, _11, _12, _13, _14, _15, _16, ...) _16


// Returns 1 if the argument list to the variadic macro is empty, 0 otherwise.
#define X_IS_EMPTY(...)                                     \
  X_HAS_COMMA                                               \
  (                                                         \
    X_JOIN_5                                                \
    (                                                       \
      X_IS_EMPTY_CASE_,                                     \
      X_HAS_COMMA(__VA_ARGS__),                             \
      X_HAS_COMMA(X_IS_EMPTY_BRACKET_TEST __VA_ARGS__),			\
      X_HAS_COMMA(__VA_ARGS__ (~)),                         \
      X_HAS_COMMA(X_IS_EMPTY_BRACKET_TEST __VA_ARGS__ (~))  \
    )                                                       \
  )

#define X_IS_EMPTY_CASE_0001			,
#define X_IS_EMPTY_BRACKET_TEST(...)	,

// X_VA_NUM_ARGS() is a very nifty macro to retrieve the number of arguments handed to a variable-argument macro.
// unfortunately, VS 2010 still has this preprocessor bug which treats a __VA_ARGS__ argument as being one single parameter:
// https://connect.microsoft.com/VisualStudio/feedback/details/521844/variadic-macro-treating-va-args-as-a-single-parameter-for-other-macros#details
#if _MSC_VER >= 1400
#	define X_VA_NUM_ARGS_HELPER(_1, _2, _3, _4, _5, _6, _7, _8, _9, _10, _11, _12, _13, _14, _15, _16, N, ...)	N
#	define X_VA_NUM_ARGS_REVERSE_SEQUENCE			16, 15, 14, 13, 12, 11, 10, 9, 8, 7, 6, 5, 4, 3, 2, 1
#	define X_VA_NUM_ARGS_LEFT (
#	define X_VA_NUM_ARGS_RIGHT )
#	define X_VA_NUM_ARGS(...)						X_VA_NUM_ARGS_HELPER X_VA_NUM_ARGS_LEFT __VA_ARGS__, X_VA_NUM_ARGS_REVERSE_SEQUENCE X_VA_NUM_ARGS_RIGHT
#else
#	define X_VA_NUM_ARGS(...)						X_VA_NUM_ARGS_HELPER(__VA_ARGS__, 16, 15, 14, 13, 12, 11, 10, 9, 8, 7, 6, 5, 4, 3, 2, 1)
#	define X_VA_NUM_ARGS_HELPER(_1, _2, _3, _4, _5, _6, _7, _8, _9, _10, _11, _12, _13, _14, _15, _16, N, ...)	N
#endif

// X_NUM_ARGS correctly handles the case of 0 arguments
#define X_NUM_ARGS(...)								X_IF(X_IS_EMPTY(__VA_ARGS__), 0, X_VA_NUM_ARGS(__VA_ARGS__))


// Expand any number of arguments into a list of operations called with those arguments
#define X_EXPAND_ARGS_0(op, arg, empty)
#define X_EXPAND_ARGS_1(op, arg, a1)																			op(a1, arg, 0)
#define X_EXPAND_ARGS_2(op, arg, a1, a2)																		op(a1, arg, 0) op(a2, arg, 1)
#define X_EXPAND_ARGS_3(op, arg, a1, a2, a3)																	op(a1, arg, 0) op(a2, arg, 1) op(a3, arg, 2)
#define X_EXPAND_ARGS_4(op, arg, a1, a2, a3, a4)																op(a1, arg, 0) op(a2, arg, 1) op(a3, arg, 2) op(a4, arg, 3)
#define X_EXPAND_ARGS_5(op, arg, a1, a2, a3, a4, a5)															op(a1, arg, 0) op(a2, arg, 1) op(a3, arg, 2) op(a4, arg, 3) op(a5, arg, 4)
#define X_EXPAND_ARGS_6(op, arg, a1, a2, a3, a4, a5, a6)														op(a1, arg, 0) op(a2, arg, 1) op(a3, arg, 2) op(a4, arg, 3) op(a5, arg, 4) op(a6, arg, 5)
#define X_EXPAND_ARGS_7(op, arg, a1, a2, a3, a4, a5, a6, a7)													op(a1, arg, 0) op(a2, arg, 1) op(a3, arg, 2) op(a4, arg, 3) op(a5, arg, 4) op(a6, arg, 5) op(a7, arg, 6)
#define X_EXPAND_ARGS_8(op, arg, a1, a2, a3, a4, a5, a6, a7, a8)												op(a1, arg, 0) op(a2, arg, 1) op(a3, arg, 2) op(a4, arg, 3) op(a5, arg, 4) op(a6, arg, 5) op(a7, arg, 6) op(a8, arg, 7)
#define X_EXPAND_ARGS_9(op, arg, a1, a2, a3, a4, a5, a6, a7, a8, a9)											op(a1, arg, 0) op(a2, arg, 1) op(a3, arg, 2) op(a4, arg, 3) op(a5, arg, 4) op(a6, arg, 5) op(a7, arg, 6) op(a8, arg, 7) op(a9, arg, 8)
#define X_EXPAND_ARGS_10(op, arg, a1, a2, a3, a4, a5, a6, a7, a8, a9, a10)									op(a1, arg, 0) op(a2, arg, 1) op(a3, arg, 2) op(a4, arg, 3) op(a5, arg, 4) op(a6, arg, 5) op(a7, arg, 6) op(a8, arg, 7) op(a9, arg, 8) op(a10, arg, 9)
#define X_EXPAND_ARGS_11(op, arg, a1, a2, a3, a4, a5, a6, a7, a8, a9, a10, a11)								op(a1, arg, 0) op(a2, arg, 1) op(a3, arg, 2) op(a4, arg, 3) op(a5, arg, 4) op(a6, arg, 5) op(a7, arg, 6) op(a8, arg, 7) op(a9, arg, 8) op(a10, arg, 9) op(a11, arg, 10)
#define X_EXPAND_ARGS_12(op, arg, a1, a2, a3, a4, a5, a6, a7, a8, a9, a10, a11, a12)						op(a1, arg, 0) op(a2, arg, 1) op(a3, arg, 2) op(a4, arg, 3) op(a5, arg, 4) op(a6, arg, 5) op(a7, arg, 6) op(a8, arg, 7) op(a9, arg, 8) op(a10, arg, 9) op(a11, arg, 10) op(a12, arg, 11)
#define X_EXPAND_ARGS_13(op, arg, a1, a2, a3, a4, a5, a6, a7, a8, a9, a10, a11, a12, a13)					op(a1, arg, 0) op(a2, arg, 1) op(a3, arg, 2) op(a4, arg, 3) op(a5, arg, 4) op(a6, arg, 5) op(a7, arg, 6) op(a8, arg, 7) op(a9, arg, 8) op(a10, arg, 9) op(a11, arg, 10) op(a12, arg, 11) op(a13, arg, 12)
#define X_EXPAND_ARGS_14(op, arg, a1, a2, a3, a4, a5, a6, a7, a8, a9, a10, a11, a12, a13, a14)			 op(a1, arg, 0) op(a2, arg, 1) op(a3, arg, 2) op(a4, arg, 3) op(a5, arg, 4) op(a6, arg, 5) op(a7, arg, 6) op(a8, arg, 7) op(a9, arg, 8) op(a10, arg, 9) op(a11, arg, 10) op(a12, arg, 11) op(a13, arg, 12) op(a14, arg, 13)
#define X_EXPAND_ARGS_15(op, arg, a1, a2, a3, a4, a5, a6, a7, a8, a9, a10, a11, a12, a13, a14, a15)			op(a1, arg, 0) op(a2, arg, 1) op(a3, arg, 2) op(a4, arg, 3) op(a5, arg, 4) op(a6, arg, 5) op(a7, arg, 6) op(a8, arg, 7) op(a9, arg, 8) op(a10, arg, 9) op(a11, arg, 10) op(a12, arg, 11) op(a13, arg, 12) op(a14, arg, 13) op(a15, arg, 14)
#define X_EXPAND_ARGS_16(op, arg, a1, a2, a3, a4, a5, a6, a7, a8, a9, a10, a11, a12, a13, a14, a15, a16)	op(a1, arg, 0) op(a2, arg, 1) op(a3, arg, 2) op(a4, arg, 3) op(a5, arg, 4) op(a6, arg, 5) op(a7, arg, 6) op(a8, arg, 7) op(a9, arg, 8) op(a10, arg, 9) op(a11, arg, 10) op(a12, arg, 11) op(a13, arg, 12) op(a14, arg, 13) op(a15, arg, 14) op(a16, arg, 15)

// X_PASS_ARGS passes __VA_ARGS__ as multiple parameters to another macro, working around the following bug:
// https://connect.microsoft.com/VisualStudio/feedback/details/521844/variadic-macro-treating-va-args-as-a-single-parameter-for-other-macros#details
#if _MSC_VER >= 1400
#	define X_PASS_ARGS_LEFT (
#	define X_PASS_ARGS_RIGHT )
#	define X_PASS_ARGS(...)							X_PASS_ARGS_LEFT __VA_ARGS__ X_PASS_ARGS_RIGHT
# define X_EXPAND_ARGS(op, a1, ...)		X_JOIN_2(X_EXPAND_ARGS_, X_NUM_ARGS(__VA_ARGS__)) X_PASS_ARGS(op, a1, __VA_ARGS__)
#else
# define X_EXPAND_ARGS(op, a1, ...)		X_JOIN_2(X_EXPAND_ARGS_, X_NUM_ARGS(__VA_ARGS__)) (op, a1, __VA_ARGS__)
#endif

#if defined(Q_CC_MSVC)
# define xForeach(VAR, RANGE) for each(VAR in RANGE)
#else
# define xForeach(VAR, RANGE) for(VAR : RANGE)
#endif

#endif // XMACROHELPERS_H
