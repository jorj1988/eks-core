#ifndef XRANDOMACCESSBLOCKALLOCATOR_H
#define XRANDOMACCESSBLOCKALLOCATOR_H

#include "XGlobal.h"
#include "Utilities/XMacroHelpers.h"
#include "Utilities/XAssert.h"
#include "Utilities/XProperty.h"
#include "Containers/XUnorderedMap.h"
#include "Memory/XAllocatorBase.h"
#include "Utilities/XEventLogger.h"

#define Bit(index) 1 << index
#define Bits1To8   0x000000FF
#define Bits9To16  0x0000FF00
#define Bits17To24 0x00FF0000
#define Bits25To32 0xFF000000
#define FullMask (Bits1To8|Bits9To16|Bits17To24|Bits25To32)

namespace Eks
{

class EKSCORE_EXPORT FixedSizeBucketAllocatorBase
  {
public:

  class EKSCORE_EXPORT Bucket
    {
  XProperties:
    XProperty(Bucket *, next, setNext);

  public:
    Bucket(xsize objectSize, xsize count)
        : _next(0), _count(count), _size(objectSize)
      {
      xAssert((count % 32) == 0);
      _memory = Eks::roundToAlignment(_alloc);
      xAssertIsAligned(_memory);
      _masks = (xuint32*)((xuint8*)_memory + (_count*_size));
      xAssert(_count%32 == 0);

      for(xsize i=0, s=(_count/32); i<s; ++i)
        {
        _masks[i] = 0;
        }
      }

    ~Bucket()
      {
      }

    void *alloc()
      {
      xuint32 mask = 0;
      for(xsize i=0, s=(_count/32); i<s; ++i)
        {
        mask = _masks[i];
        if(mask != FullMask)
          {
          xuint8 spareBit = 0;
          if((mask&Bits1To8) != Bits1To8)
            {
            // spare in 1-8
            spareBit = findFirstSpareBit((xuint8)mask);
            }
          else if((mask&Bits9To16) != Bits9To16)
            {
            // spare in 9-16
            spareBit = findFirstSpareBit((xuint8)(mask >> 8)) + 8;
            }
          else if((mask&Bits17To24) != Bits17To24)
            {
            // spare in 17-24
            spareBit = findFirstSpareBit((xuint8)(mask >> 16)) + 16;
            }
          else
            {
            // spare in 25-32
            spareBit = findFirstSpareBit((xuint8)(mask >> 24)) + 24;
            }

          // mark spareBit as used.
          _masks[i] = mask|(1<<spareBit);

          xsize block = i * 32 + spareBit;
          xsize offset = block * _size;

          xuint8* data = _memory;
          data += offset;

          void *memory = (void *)data;
          return memory;
          }
        }
      return 0;
      }

    bool free(void *ptr)
      {
      if(contains(ptr))
        {
        // always positive
        xsize index = ( (char*)ptr - (char*)_memory ) / _size;
        xAssert((( (char*)ptr - (char*)_memory ) % _size) == 0);
        xsize block = index / 32;
        xsize blockIndex = index % 32;

        xAssert((_masks[block]&(1<<blockIndex)) != false);

        // set to 0
        _masks[block] = ~((~_masks[block])|(1<<blockIndex));
        xAssert((_masks[block]&(1<<blockIndex)) == false);

        return true;
        }
      return false;
      }

    bool empty() const
      {
      for(xsize i=0, s=(_count/32); i<s; ++i)
        {
        if(_masks[i] != 0)
          {
          return false;
          }
        }
      return true;
      }

    bool contains(void *ptr) const
      {
      if(ptr < _memory)
        {
        return false;
        }
      if(ptr >= (_memory + (_count*_size)))
        {
        return false;
        }
      return true;
      }

    void debugDump() const;

  private:
    xuint8 findFirstSpareBit(xuint8 mask)
      {
      for(xuint8 i=0; i<8; ++i)
        {
        if((Bit(i)&mask) == false)
          {
          return i;
          }
        }
      xAssert(0);
      return 8;
      }

    xsize _count;
    xsize _size;
    xuint32 *_masks;
    xuint8 *_memory;
    xuint8 _alloc[1];
    };

  FixedSizeBucketAllocatorBase(AllocatorBase *allocator, xsize s, xsize d=128, xsize e=1024)
      : _freePtr(0), _size(s), _defaultSize(d), _expandSize(e), _first(0), _allocator(allocator)
    {
    xAssert(_allocator);

    if(d != 0)
      {
      _first = createBucket(_defaultSize);
      }
    }

  ~FixedSizeBucketAllocatorBase()
    {
    Bucket *b = _first;
    while(b)
      {
      Bucket *b2 = b->next();
      destroyBucket(b);
      b = b2;
      }
    }

  Bucket *createBucket(xsize count)
    {
    xsize extraBucketSize = count*_size + (count / 8) + X_ALIGN_BYTE_COUNT - 1;
    void *alloc = _allocator->alloc(sizeof(Bucket) + extraBucketSize);

    // create the new bucket, with the extra space at the end.
    new(alloc) Bucket(_size, count);

    return (Bucket *)alloc;
    }

  void destroyBucket(Bucket *b)
    {
    _allocator->destroy(b);
    }

  void *alloc()
    {
    void *newMem = 0;

    Bucket *b = _freePtr ? _freePtr : _first;
    while(b)
      {
      newMem = b->alloc();
      if(newMem)
        {
        return newMem;
        }
      else
        {
        _freePtr = b;
        }
      b = b->next();
      }

    Bucket* newBlock = createBucket(_defaultSize);

    // add the new bucket to the end.
    if(_first)
      {
      Bucket *b = _first;
      while(b && b->next())
        {
        b = b->next();
        }
      b->setNext(newBlock);
      }
    else
      {
      _first = newBlock;
      }

    return newBlock->alloc();
    }

  bool free(void *ptr)
    {
    bool hitFreePtr = false;
    Bucket *b = _first;
    while(b)
      {
      if(b == _freePtr)
        {
        hitFreePtr = true;
        }

      if(b->free(ptr))
        {
        if(!hitFreePtr)
          {
          _freePtr = b;
          }
        return true;
        }
      b = b->next();
      }
    return false;
    }

  bool empty() const
    {
    Bucket *b = _first;
    while(b)
      {
      if(!b->empty())
        {
        return false;
        }
      b = b->next();
      }
    return true;
    }

  xsize bucketCount() const
    {
    xsize i = 0;
    Bucket *b = _first;
    while(b)
      {
      ++i;
      b = b->next();
      }
    return i;
    }

  bool contains(void *ptr) const
    {
    Bucket *b = _first;
    while(b)
      {
      if(b->contains(ptr))
        {
        return true;
        }
      b = b->next();
      }
    return false;
    }

  xsize size() const
    {
    return _size;
    }

  void debugDump() const;

private:
  X_DISABLE_COPY(FixedSizeBucketAllocatorBase);
  Bucket *_freePtr;
  xsize _size;
  xsize _defaultSize;
  xsize _expandSize;
  Bucket * _first;
  AllocatorBase *_allocator;
  };

class EKSCORE_EXPORT FixedSizeBucketAllocator
    : public AllocatorBase,
      FixedSizeBucketAllocatorBase
  {
public:
  FixedSizeBucketAllocator(AllocatorBase *allocator, xsize s, xsize d=128, xsize e=1024)
      : FixedSizeBucketAllocatorBase(allocator, s, d, e)
    {
    }

  FixedSizeBucketAllocator(
    AllocatorBase *allocator,
    const Eks::ResourceDescription &desc,
    xsize d=128,
    xsize e=1024)
      : FixedSizeBucketAllocatorBase(allocator, Eks::roundToAlignment(desc.size()), d, e)
    {
    }

  virtual void *alloc(xsize X_USED_FOR_ASSERTS(s), xsize X_USED_FOR_ASSERTS(alignment=1))
    {
    // alignment should be embedded in the size passed to the constructor.
    xAssert((s+alignment) == size());

    return FixedSizeBucketAllocatorBase::alloc();
    }

  virtual void free(void *mem)
    {
    FixedSizeBucketAllocatorBase::free(mem);
    }
  };

class EKSCORE_EXPORT BucketAllocator : public AllocatorBase
  {
public:
  BucketAllocator(AllocatorBase *alloc, xsize d=128, xsize e=1024)
      : _defaultSize(d), _expandSize(e), _internal(alloc), _allocator(alloc)
    {
    xAssert(_allocator);
    }

  virtual ~BucketAllocator()
    {
    HashType::iterator i = _internal.begin();
    HashType::iterator e = _internal.end();
    while (i != e)
      {
      _allocator->destroy(i->second);
      ++i;
      }
    }

  void *alloc(xsize size, xsize alignment=X_ALIGN_BYTE_COUNT)
    {
    size = Eks::roundToAlignment(size, alignment);

    FixedSizeBucketAllocatorBase *b = _internal.value(size, 0);
    if(!b)
      {
      b = _allocator->create<FixedSizeBucketAllocatorBase>(_allocator, size, _defaultSize, _expandSize);
      _internal[size] = b;
      }

    void *mem = b->alloc();
    xAssertIsSpecificAligned(mem, alignment);
    return mem;
    }

  void free(void *ptr)
    {
    HashType::iterator i = _internal.begin();
    HashType::iterator e = _internal.end();
    while (i != e)
      {
      if(i->second->contains(ptr))
        {
        xVerify(i->second->free(ptr));
        return;
        }
      ++i;
      }
    xAssertFail();
    }

  bool empty() const
    {
    HashType::const_iterator i = _internal.cbegin();
    HashType::const_iterator e = _internal.cend();
    while (i != e)
      {
      if(!i->second->empty())
        {
        return false;
        }
      ++i;
      }
    return true;
    }

  void debugDump() const;

private:
  X_DISABLE_COPY(BucketAllocator);
  xsize _defaultSize;
  xsize _expandSize;

  typedef UnorderedMap <xsize, FixedSizeBucketAllocatorBase *> HashType;
  HashType _internal;

  AllocatorBase *_allocator;
  };

}

#endif // XRANDOMACCESSBLOCKALLOCATOR_H
