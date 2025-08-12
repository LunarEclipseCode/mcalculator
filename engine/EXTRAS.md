This guide explains how to build the engine if you decide to uncomment `#include <intsafe.h>` in [CalcManager/pch.h](./CalcManager/pch.h)

## Step 1: Copy Required Windows Header Files

If you run `%/path/to/emscripten%/emmake ninja` while `#include <intsafe.h>` is commented out, you will get `fatal error: 'intsafe.h' file not found`

To resolve this issue, copy the following files from `C:\Program Files (x86)\Windows Kits\10\Include\10.0.xxxxx.x\shared` to the `engine/CalcManager` folder. We have to copy more than just `intsafe.h` due to nested dependencies.

- `intsafe.h`
- `winapifamily.h`
- `winpackagefamily.h`
- `specstrings.h`
- `sal.h`
- `concurrencysal.h`
- `specstrings_strict.h`
- `driverspecs.h`
- `sdv_driverspecs.h`
- `no_sal2.h`

## Step 2: Modify intsafe.h for Emscripten Compatibility

Make the following replacements in `intsafe.h` to ensure compatibility with Emscripten:

### 1. Integer Type Definitions

**Find:**
```c
typedef __int64             LONGLONG;
typedef __int64             LONG64;
typedef signed __int64      INT64;
typedef unsigned __int64    ULONGLONG;
typedef unsigned __int64    DWORDLONG;
typedef unsigned __int64    ULONG64;
typedef unsigned __int64    DWORD64;
typedef unsigned __int64    UINT64;
```

**Replace with:**
```c
#if defined(__EMSCRIPTEN__)
    typedef int64_t             LONGLONG;
    typedef int64_t             LONG64;
    typedef int64_t             INT64;
    typedef uint64_t            ULONGLONG;
    typedef uint64_t            DWORDLONG;
    typedef uint64_t            ULONG64;
    typedef uint64_t            DWORD64;
    typedef uint64_t            UINT64;
#else
    typedef __int64             LONGLONG;
    typedef __int64             LONG64;
    typedef signed __int64      INT64;
    typedef unsigned __int64    ULONGLONG;
    typedef unsigned __int64    DWORDLONG;
    typedef unsigned __int64    ULONG64;
    typedef unsigned __int64    DWORD64;
    typedef unsigned __int64    UINT64;
#endif
```

### 2. Size Type Definitions

**Find:**
```c
#ifdef _WIN64
typedef __int64             ptrdiff_t;
typedef unsigned __int64    size_t;
#else
typedef _W64 int            ptrdiff_t;
typedef _W64 unsigned int   size_t;
#endif
```

**Replace with:**
```c
#ifndef __EMSCRIPTEN__
#ifdef _WIN64
typedef __int64             ptrdiff_t;
typedef unsigned __int64    size_t;
#else
typedef _W64 int            ptrdiff_t;
typedef _W64 unsigned int   size_t;
#endif
#endif
```

### 3. UInt32x32To64 Macro

**Find:**
```c
#ifndef UInt32x32To64
#if defined(MIDL_PASS) || defined(RC_INVOKED) || defined(_M_CEE_PURE) \
    || defined(_68K_) || defined(_MPPC_) \
    || defined(_M_IA64) || defined(_M_X64) || defined(_M_ARM) || defined(_M_ARM64) \
    || defined(_M_HYBRID_X86_ARM64)
#define UInt32x32To64(a, b) (((unsigned __int64)((unsigned int)(a))) * ((unsigned __int64)((unsigned int)(b))))
#elif defined(_M_IX86)
#define UInt32x32To64(a, b) ((unsigned __int64)(((unsigned __int64)((unsigned int)(a))) * ((unsigned int)(b))))
#else
#error Must define a target architecture.
#endif
#endif
```

**Replace with:**
```c
#ifndef UInt32x32To64
#if defined(__EMSCRIPTEN__)
#define UInt32x32To64(a, b) ((uint64_t)((uint32_t)(a)) * (uint64_t)((uint32_t)(b)))
#elif defined(MIDL_PASS) || defined(RC_INVOKED) || defined(_M_CEE_PURE) \
    || defined(_68K_) || defined(_MPPC_) \
    || defined(_M_IA64) || defined(_M_X64) || defined(_M_ARM) || defined(_M_ARM64) \
    || defined(_M_HYBRID_X86_ARM64)
#define UInt32x32To64(a, b) (((unsigned __int64)((unsigned int)(a))) * ((unsigned __int64)((unsigned int)(b))))
#elif defined(_M_IX86)
#define UInt32x32To64(a, b) ((unsigned __int64)(((unsigned __int64)((unsigned int)(a))) * ((unsigned int)(b))))
#else
#error Must define a target architecture.
#endif
#endif
```

### 4. __forceinline

Replace all instances of `__forceinline` with `FORCE_INLINE`.

### 2.5 Min/Max Value Definitions

**Find:** 

```c
//
// Min/Max type values
//
#define INT8_MIN        (-127i8 - 1)
#define SHORT_MIN       (-32768)
#define INT16_MIN       (-32767i16 - 1)
#ifndef INT_MIN
#define INT_MIN         (-2147483647 - 1)
#endif
#define INT32_MIN       (-2147483647i32 - 1)
#ifndef LONG_MIN
#define LONG_MIN        (-2147483647L - 1)
#endif
#define LONGLONG_MIN    (-9223372036854775807i64 - 1)
#define LONG64_MIN      (-9223372036854775807i64 - 1)
#define INT64_MIN       (-9223372036854775807i64 - 1)
#define INT128_MIN      (-170141183460469231731687303715884105727i128 - 1)

#ifdef _WIN64
#define INT_PTR_MIN     (-9223372036854775807i64 - 1)
#define LONG_PTR_MIN    (-9223372036854775807i64 - 1)
#define PTRDIFF_T_MIN   (-9223372036854775807i64 - 1)
#define SSIZE_T_MIN     (-9223372036854775807i64 - 1)
#else
#define INT_PTR_MIN     (-2147483647 - 1)
#define LONG_PTR_MIN    (-2147483647L - 1)
#define PTRDIFF_T_MIN   (-2147483647 - 1)
#define SSIZE_T_MIN     (-2147483647L - 1)
#endif

#define INT8_MAX        127i8
#define UINT8_MAX       0xffui8
#define BYTE_MAX        0xff
#define SHORT_MAX       32767
#define INT16_MAX       32767i16
#define USHORT_MAX      0xffff
#define UINT16_MAX      0xffffui16
#define WORD_MAX        0xffff
#ifndef INT_MAX
#define INT_MAX         2147483647
#endif
#define INT32_MAX       2147483647i32
#ifndef UINT_MAX
#define UINT_MAX        0xffffffff
#endif
#define UINT32_MAX      0xffffffffui32
#ifndef LONG_MAX
#define LONG_MAX        2147483647L
#endif
#ifndef ULONG_MAX
#define ULONG_MAX       0xffffffffUL
#endif
#define DWORD_MAX       0xffffffffUL
#define LONGLONG_MAX    9223372036854775807i64
#define LONG64_MAX      9223372036854775807i64
#define INT64_MAX       9223372036854775807i64
#define ULONGLONG_MAX   0xffffffffffffffffui64
#define DWORDLONG_MAX   0xffffffffffffffffui64
#define ULONG64_MAX     0xffffffffffffffffui64
#define DWORD64_MAX     0xffffffffffffffffui64
#define UINT64_MAX      0xffffffffffffffffui64
#define INT128_MAX      170141183460469231731687303715884105727i128
#define UINT128_MAX     0xffffffffffffffffffffffffffffffffui128

#undef SIZE_T_MAX

#ifdef _WIN64
#define INT_PTR_MAX     9223372036854775807i64
#define UINT_PTR_MAX    0xffffffffffffffffui64
#define LONG_PTR_MAX    9223372036854775807i64
#define ULONG_PTR_MAX   0xffffffffffffffffui64
#define DWORD_PTR_MAX   0xffffffffffffffffui64
#define PTRDIFF_T_MAX   9223372036854775807i64
#define SIZE_T_MAX      0xffffffffffffffffui64
#define SSIZE_T_MAX     9223372036854775807i64
#define _SIZE_T_MAX     0xffffffffffffffffui64
#else
#define INT_PTR_MAX     2147483647 
#define UINT_PTR_MAX    0xffffffff
#define LONG_PTR_MAX    2147483647L
#define ULONG_PTR_MAX   0xffffffffUL
#define DWORD_PTR_MAX   0xffffffffUL
#define PTRDIFF_T_MAX   2147483647
#define SIZE_T_MAX      0xffffffff
#define SSIZE_T_MAX     2147483647L
#define _SIZE_T_MAX     0xffffffffUL
#endif


//
// It is common for -1 to be used as an error value
//
#define INT8_ERROR      (-1i8)
#define UINT8_ERROR     0xffui8
#define BYTE_ERROR      0xff
#define SHORT_ERROR     (-1)
#define INT16_ERROR     (-1i16)
#define USHORT_ERROR    0xffff
#define UINT16_ERROR    0xffffui16
#define WORD_ERROR      0xffff
#define INT_ERROR       (-1)
#define INT32_ERROR     (-1i32)
#define UINT_ERROR      0xffffffff
#define UINT32_ERROR    0xffffffffui32
#define LONG_ERROR      (-1L)
#define ULONG_ERROR     0xffffffffUL
#define DWORD_ERROR     0xffffffffUL
#define LONGLONG_ERROR  (-1i64)
#define LONG64_ERROR    (-1i64)
#define INT64_ERROR     (-1i64)
#define ULONGLONG_ERROR 0xffffffffffffffffui64
#define DWORDLONG_ERROR 0xffffffffffffffffui64
#define ULONG64_ERROR   0xffffffffffffffffui64
#define UINT64_ERROR    0xffffffffffffffffui64

#ifdef _WIN64
#define INT_PTR_ERROR   (-1i64)
#define UINT_PTR_ERROR  0xffffffffffffffffui64
#define LONG_PTR_ERROR  (-1i64)
#define ULONG_PTR_ERROR 0xffffffffffffffffui64
#define DWORD_PTR_ERROR 0xffffffffffffffffui64
#define PTRDIFF_T_ERROR (-1i64)
#define SIZE_T_ERROR    0xffffffffffffffffui64
#define SSIZE_T_ERROR   (-1i64)
#define _SIZE_T_ERROR   0xffffffffffffffffui64
#else
#define INT_PTR_ERROR   (-1) 
#define UINT_PTR_ERROR  0xffffffff
#define LONG_PTR_ERROR  (-1L)
#define ULONG_PTR_ERROR 0xffffffffUL
#define DWORD_PTR_ERROR 0xffffffffUL
#define PTRDIFF_T_ERROR (-1)
#define SIZE_T_ERROR    0xffffffff
#define SSIZE_T_ERROR   (-1L)
#define _SIZE_T_ERROR   0xffffffffUL
#endif
```

**Replace with:**
```c
#ifdef _MSC_VER
    #define FORCE_INLINE __forceinline
    #define INT8_LITERAL(x) x##i8
    #define UINT8_LITERAL(x) x##ui8
    #define INT16_LITERAL(x) x##i16
    #define UINT16_LITERAL(x) x##ui16
    #define INT32_LITERAL(x) x##i32
    #define UINT32_LITERAL(x) x##ui32
    #define INT64_LITERAL(x) x##i64
    #define UINT64_LITERAL(x) x##ui64
    #define INT128_LITERAL(x) x##i128
    #define UINT128_LITERAL(x) x##ui128
#else
    #define FORCE_INLINE inline __attribute__((always_inline))
    #define INT8_LITERAL(x) ((int8_t)(x))
    #define UINT8_LITERAL(x) ((uint8_t)(x))
    #define INT16_LITERAL(x) ((int16_t)(x))
    #define UINT16_LITERAL(x) ((uint16_t)(x))
    #define INT32_LITERAL(x) ((int32_t)(x))
    #define UINT32_LITERAL(x) ((uint32_t)(x))
    #define INT64_LITERAL(x) ((int64_t)(x##LL))
    #define UINT64_LITERAL(x) ((uint64_t)(x##ULL))
    #define INT128_LITERAL(x) ((__int128)(x##LL))
    #define UINT128_LITERAL(x) ((unsigned __int128)(x##ULL))
#endif

//
// Min/Max type values
//
#define INT8_MIN        INT8_LITERAL(-128)
#define SHORT_MIN       (-32768)
#define INT16_MIN       INT16_LITERAL(-32768)
#ifndef INT_MIN
    #define INT_MIN         (-2147483647 - 1)
#endif
#define INT32_MIN       INT32_LITERAL(-2147483648)
#ifndef LONG_MIN
    #define LONG_MIN        (-2147483647L - 1)
#endif
#define LONGLONG_MIN    INT64_LITERAL(-9223372036854775808)
#define LONG64_MIN      INT64_LITERAL(-9223372036854775808)
#define INT64_MIN       INT64_LITERAL(-9223372036854775808)
#define INT128_MIN      INT128_LITERAL(-170141183460469231731687303715884105728)

#ifdef _WIN64
    #define INT_PTR_MIN     INT64_MIN
    #define LONG_PTR_MIN    INT64_MIN
    #define PTRDIFF_T_MIN   INT64_MIN
    #define SSIZE_T_MIN     INT64_MIN
#else
    #define INT_PTR_MIN     (-2147483647 - 1)
    #define LONG_PTR_MIN    (-2147483647L - 1)
    #define PTRDIFF_T_MIN   (-2147483647 - 1)
    #define SSIZE_T_MIN     (-2147483647L - 1)
#endif

#define INT8_MAX        INT8_LITERAL(127)
#define UINT8_MAX       UINT8_LITERAL(0xff)
#define BYTE_MAX        0xff
#define SHORT_MAX       32767
#define INT16_MAX       INT16_LITERAL(32767)
#define USHORT_MAX      0xffff
#define UINT16_MAX      UINT16_LITERAL(0xffff)
#define WORD_MAX        0xffff
#ifndef INT_MAX
#define INT_MAX         2147483647
#endif
#define INT32_MAX       INT32_LITERAL(2147483647)
#ifndef UINT_MAX
#define UINT_MAX        0xffffffff
#endif
#define UINT32_MAX      UINT32_LITERAL(0xffffffff)
#ifndef LONG_MAX
#define LONG_MAX        2147483647L
#endif
#ifndef ULONG_MAX
#define ULONG_MAX       0xffffffffUL
#endif
#define DWORD_MAX       0xffffffffUL
#define LONGLONG_MAX    INT64_LITERAL(9223372036854775807)
#define LONG64_MAX      INT64_LITERAL(9223372036854775807)
#define INT64_MAX       INT64_LITERAL(9223372036854775807)
#define ULONGLONG_MAX   UINT64_LITERAL(0xffffffffffffffff)
#define DWORDLONG_MAX   UINT64_LITERAL(0xffffffffffffffff)
#define ULONG64_MAX     UINT64_LITERAL(0xffffffffffffffff)
#define DWORD64_MAX     UINT64_LITERAL(0xffffffffffffffff)
#define UINT64_MAX      UINT64_LITERAL(0xffffffffffffffff)
#define INT128_MAX      INT128_LITERAL(170141183460469231731687303715884105727)
#define UINT128_MAX     UINT128_LITERAL(0xffffffffffffffffffffffffffffffff)

#undef SIZE_T_MAX

#ifdef _WIN64
    #define INT_PTR_MAX     INT64_MAX
    #define UINT_PTR_MAX    UINT64_MAX
    #define LONG_PTR_MAX    INT64_MAX
    #define ULONG_PTR_MAX   UINT64_MAX
    #define DWORD_PTR_MAX   UINT64_MAX
    #define PTRDIFF_T_MAX   INT64_MAX
    #define SIZE_T_MAX      UINT64_MAX
    #define SSIZE_T_MAX     INT64_MAX
    #define _SIZE_T_MAX     UINT64_MAX
#else
    #define INT_PTR_MAX     2147483647 
    #define UINT_PTR_MAX    0xffffffff
    #define LONG_PTR_MAX    2147483647L
    #define ULONG_PTR_MAX   0xffffffffUL
    #define DWORD_PTR_MAX   0xffffffffUL
    #define PTRDIFF_T_MAX   2147483647
    #define SIZE_T_MAX      0xffffffff
    #define SSIZE_T_MAX     2147483647L
    #define _SIZE_T_MAX     0xffffffffUL
#endif

//
// It is common for -1 to be used as an error value
//
#define INT8_ERROR      INT8_LITERAL(-1)
#define UINT8_ERROR     UINT8_MAX
#define BYTE_ERROR      0xff
#define SHORT_ERROR     (-1)
#define INT16_ERROR     INT16_LITERAL(-1)
#define USHORT_ERROR    0xffff
#define UINT16_ERROR    UINT16_MAX
#define WORD_ERROR      0xffff
#define INT_ERROR       (-1)
#define INT32_ERROR     INT32_LITERAL(-1)
#define UINT_ERROR      0xffffffff
#define UINT32_ERROR    UINT32_MAX
#define LONG_ERROR      (-1L)
#define ULONG_ERROR     0xffffffffUL
#define DWORD_ERROR     0xffffffffUL
#define LONGLONG_ERROR  INT64_LITERAL(-1)
#define LONG64_ERROR    INT64_LITERAL(-1)
#define INT64_ERROR     INT64_LITERAL(-1)
#define ULONGLONG_ERROR UINT64_MAX
#define DWORDLONG_ERROR UINT64_MAX
#define ULONG64_ERROR   UINT64_MAX
#define UINT64_ERROR    UINT64_MAX

#ifdef _WIN64
    #define INT_PTR_ERROR   INT64_LITERAL(-1)
    #define UINT_PTR_ERROR  UINT64_MAX
    #define LONG_PTR_ERROR  INT64_LITERAL(-1)
    #define ULONG_PTR_ERROR UINT64_MAX
    #define DWORD_PTR_ERROR UINT64_MAX
    #define PTRDIFF_T_ERROR INT64_LITERAL(-1)
    #define SIZE_T_ERROR    UINT64_MAX
    #define SSIZE_T_ERROR   INT64_LITERAL(-1)
    #define _SIZE_T_ERROR   UINT64_MAX
#else
    #define INT_PTR_ERROR   (-1) 
    #define UINT_PTR_ERROR  0xffffffff
    #define LONG_PTR_ERROR  (-1L)
    #define ULONG_PTR_ERROR 0xffffffffUL
    #define DWORD_PTR_ERROR 0xffffffffUL
    #define PTRDIFF_T_ERROR (-1)
    #define SIZE_T_ERROR    0xffffffff
    #define SSIZE_T_ERROR   (-1L)
    #define _SIZE_T_ERROR   0xffffffffUL
#endif
```

### 6. ULongLongToSizeT Definition

**Find:**
```c
#define ULongLongToSizeT    ULongLongToUIntPtr
```

**Replace with:**
```c
#ifdef __EMSCRIPTEN__
#define ULongLongToSizeT    ULongLongToULong
#else
#define ULongLongToSizeT    ULongLongToUIntPtr
#endif
```

## Step 3: Compilation

After making all the above changes, run:
```bash
%/path/to/emscripten%/emmake ninja
%/path/to/emscripten%/emcc -02 libEngine.a CalcManager/libCalcManager.a CalcModel/libCalcModel.a  -o engine.js
```

The build should now complete successfully.

## Expected Warnings

You may see warnings like `'....' macro redefined`. These warnings appear because the goal was to get the code to compile with minimal changes to the original `intsafe.h`. 

If these warnings cause issues in the future, you can use conditional statements to define variables only when they're not already available.