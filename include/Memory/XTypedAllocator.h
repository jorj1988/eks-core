	#ifndef XTYPEDALLOCATOR_H
#define XTYPEDALLOCATOR_H

#include "Memory/XAllocatorBase.h"

namespace Eks
{

template <typename T> class TypedAllocator;

template <typename Derived, typename T> class TypedAllocatorBase
  {
public:
  typedef T value_type;
  typedef value_type* pointer;
  typedef const value_type* const_pointer;
  typedef xsize size_type;
  typedef xptrdiff difference_type;

  template <class O> struct rebind
    {
    typedef TypedAllocator<O> other;
    };

  pointer allocate(xsize n, void *hint=0)
    {
    xAssert(allocator());
    (void)hint;
    return (pointer)allocator()->alloc(sizeof(value_type) * n);
    }

  pointer allocate(xsize n, xsize align)
    {
    xAssert(allocator());
    return (pointer)allocator()->alloc(sizeof(value_type) * n, align);
    }

  void deallocate(pointer p, size_type n)
    {
    xAssert(allocator());
    (void)n;
    allocator()->free(p);
    }

  template <typename D, typename X>
  bool operator==(const TypedAllocatorBase<D, X> &oth) const
    {
    return allocator() == oth.allocator();
    }

  AllocatorBase *allocator()
    {
    Derived *d = static_cast<Derived*>(this);
    return d->allocator();
    }

  const AllocatorBase *allocator() const
    {
    const Derived *d = static_cast<const Derived*>(this);
    return d->allocator();
    }
  };

template <typename T> class TypedAllocator : public TypedAllocatorBase<TypedAllocator<T>, T>
  {
public:
  typedef TypedAllocatorBase<TypedAllocator<T>, T> Base;
  typedef typename Base::value_type& reference;
  typedef const typename Base::value_type& const_reference;
  typedef typename Base::value_type&& rvalue_reference;

  TypedAllocator(AllocatorBase *alloc=0)
    {
    _alloc = alloc;
    }

  template <class U>
      TypedAllocator(const TypedAllocator<U>& alloc)
    {
    _alloc = alloc.allocator();
    }

  AllocatorBase *allocator() const
    {
    return _alloc;
    }

protected:
  AllocatorBase *_alloc;
  };

template <> class TypedAllocator<void> : public TypedAllocatorBase<TypedAllocator<void>, void>
  {
public:
  typedef void* pointer;

  TypedAllocator(AllocatorBase *alloc=0)
    {
    _alloc = alloc;
    }

  template <class U>
      TypedAllocator(const TypedAllocator<U>& alloc)
    {
    _alloc = alloc._alloc;
    }

  AllocatorBase *allocator() const { return _alloc; }

protected:
  AllocatorBase *_alloc;
  };


template <typename Allocator, typename T> class StaticTypedAllocator
    : public TypedAllocatorBase<StaticTypedAllocator<Allocator, T>, T>
  {
public:
  typedef TypedAllocatorBase<StaticTypedAllocator<Allocator, T>, T> Base;
  typedef typename Base::value_type& reference;
  typedef const typename Base::value_type& const_reference;

  StaticTypedAllocator()
    {
    }

  template <class U>
      StaticTypedAllocator(const StaticTypedAllocator<Allocator, U>&)
    {
    }

  void construct(typename Base::pointer p, const_reference val)
    {
    new(p) T(val);
    }

  template <typename U> void destroy(U *p)
    {
    (void)p;
    p->~U();
    }

  AllocatorBase *allocator() const { return Allocator::instance(); }
  };

template <typename Allocator> class StaticTypedAllocator<Allocator, void>
    : public TypedAllocatorBase<StaticTypedAllocator<Allocator, void>, void>
  {
public:
  typedef void* pointer;

  StaticTypedAllocator()
    {
    }

  template <class U>
      StaticTypedAllocator(const StaticTypedAllocator<Allocator, U>&)
    {
    }

  AllocatorBase *allocator() const { return Allocator::instance(); }
  };

// should be minimally sized.
xCompileTimeAssert(sizeof(StaticTypedAllocator<AllocatorBase, void>) == 1);

}

#endif // XTYPEDALLOCATOR_H
