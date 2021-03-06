#ifndef XFLAGS_
#define XFLAGS_

#include "XGlobal.h"
#include <atomic>
#include <bitset>

namespace Eks
{
namespace detail
{

template <typename T> class Atomic
  {
public:
  Atomic(const T &t=0)
    {
    _data = t;
    }

  Atomic(const Atomic &t)
    {
    _data = (T)t._data;
    }

  void operator=(const Atomic &t)
    {
    _data = (T)t._data;
    }

  operator T() const
    {
    return _data;
    }

  void operator=(const T &t)
    {
    _data = t;
    }

  std::atomic<T> _data;
  };

template <xsize size> class BitHelpers
  {
  // M1: binary: 0101...
  // M2: binary: 00110011..
  // M3: binary:  4 zeros,  4 ones ...
  // H01: the sum of 256 to the power of 0,1,2,3...
  };

template <> class BitHelpers<1>
  {
public:
  enum class Fields : xuint8
    {
    M1 = 0x55,
    M2 = 0x33,
    M4 = 0x0f,
    H01 = 0x01
    };

  static xuint8 numberOfSetBits(xuint8 x)
    {
    x -= (x >> 1) & (xuint8)Fields::M1;                               // put count of each 2 bits into those 2 bits
    x = (x & (xuint8)Fields::M2) + ((x >> 2) & (xuint8)Fields::M2);   // put count of each 4 bits into those 4 bits
    x = (x + (x >> 4)) & (xuint8)Fields::M4;                          // put count of each 8 bits into those 8 bits
    return (x * (xuint8)Fields::H01);                                 // returns left 8 bits of x + (x<<8) + (x<<16) + (x<<24) + ...
    }
  };

template <> class BitHelpers<4>
  {
public:
  enum class Fields : xuint32
    {
    M1 = 0x55555555,
    M2 = 0x33333333,
    M4 = 0x0f0f0f0f,
    H01 = 0x01010101
    };

  static xuint8 numberOfSetBits(xuint32 x)
    {
    x -= (x >> 1) & (xuint32)Fields::M1;                                // put count of each 2 bits into those 2 bits
    x = (x & (xuint32)Fields::M2) + ((x >> 2) & (xuint32)Fields::M2);   // put count of each 4 bits into those 4 bits
    x = (x + (x >> 4)) & (xuint32)Fields::M4;                           // put count of each 8 bits into those 8 bits
    return (x * (xuint32)Fields::H01) >> ((sizeof(xuint32)-1)*8);       // returns left 8 bits of x + (x<<8) + (x<<16) + (x<<24) + ...
    }
  };
}

template <typename CONTAINER> class BitField
  {
public:
  BitField(CONTAINER c=0) : _internal(c)
    {
    }

  static BitField<CONTAINER> fromIndex(xuint8 i)
    {
    BitField<CONTAINER> b;
    b.setBitAtIndex(i);
    return b;
    }

  typedef detail::BitHelpers<sizeof(CONTAINER)> Helpers;

  bool hasAll(const BitField<CONTAINER> &c) const
    {
    return (_internal&c._internal) == (CONTAINER)c;
    }

  bool hasAny(const BitField<CONTAINER> &c) const
    {
    return (_internal&c._internal) != 0;
    }

  bool hasAny() const
    {
    return _internal != 0;
    }

  BitField<CONTAINER> operator~() const
    {
    return BitField<CONTAINER>(~_internal);
    }

  BitField<CONTAINER> operator&(const BitField<CONTAINER> &f) const
    {
    return BitField<CONTAINER>(_internal&f._internal);
    }

  bool operator==(CONTAINER c)
    {
    return _internal == c;
    }

  bool operator!=(CONTAINER c)
    {
    return _internal != c;
    }

  BitField<CONTAINER> &operator |=(CONTAINER c)
    {
    _internal |= c;
    return *this;
    }

  CONTAINER operator*() const
    {
    return _internal;
    }

  void setValue(CONTAINER c)
    {
    _internal = c;
    }

  CONTAINER value() const
    {
    return _internal;
    }

  xuint8 numberOfSetBits() const
    {
    return Helpers::numberOfSetBits(_internal);
    }

  bool firstSetIndex(xuint8* index) const
    {
    for(xuint8 i = 0; i < (sizeof(CONTAINER)*8); ++i)
      {
      CONTAINER mask = 1<<i;
      if((_internal&mask) != 0)
        {
        *index = i;
        return true;
        }
      }

    *index = (CONTAINER)-1;
    return false;
    }

  void setBitAtIndex(xuint8 index)
    {
    *this |= 1<<index;
    }

  bool hasIndex(xuint8 index) const
    {
    return hasAny(1<<index);
    }

protected:
  CONTAINER _internal;
  };

template <typename ENUM, typename CONTAINER=xuint32> class Flags : Eks::BitField<CONTAINER>
  {
public:
  Flags(CONTAINER c) : BitField<CONTAINER>(c) { }
  Flags(ENUM c) : BitField<CONTAINER>((CONTAINER)c) { }

  typedef decltype(std::declval<CONTAINER>() | std::declval<ENUM>()) OrResult;

  CONTAINER value() const { return BitField<CONTAINER>::value(); }

  bool hasFlag(ENUM c) const { return (value()&c) != 0; }
  bool hasAnyFlags(OrResult c) const { return (value()&c) != 0; }
  bool hasAnyFlags() const { return value() != 0; }
  bool hasAllFlags(OrResult c) const { return (value()&c) == (CONTAINER)c; }

  void clearFlag(ENUM c) { BitField<CONTAINER>::setValue(~(~value() | c)); }
  void setFlag(ENUM c) { BitField<CONTAINER>::setValue(value()|c); }
  void setFlag(ENUM c, bool val) { if(val) { setFlag(c); } else { clearFlag(c); } }

  Flags<ENUM, CONTAINER> operator~() const
    {
    return Flags(~value());
    }

  Flags<ENUM, CONTAINER> operator&(const Flags<ENUM, CONTAINER> &f) const
    {
    return Flags(value()&f.value());
    }

  bool operator==(CONTAINER c)
    {
    return value() == c;
    }

  bool operator!=(CONTAINER c)
    {
    return value() != c;
    }

  Flags<ENUM, CONTAINER> &operator |=(CONTAINER c)
    {
    value() |= c;
    return *this;
    }
  };

}

#endif // XFLAGS_
