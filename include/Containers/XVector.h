#ifndef XVECTOR_H
#define XVECTOR_H

#include "XGlobal.h"
#include "Utilities/XAssert.h"
#include "Memory/XAllocatorBase.h"
#include "Memory/XTypedAllocator.h"
#include "Memory/XResourceDescription.h"
#include <limits>

namespace Eks
{

namespace detail
{

template <typename T, xsize Size, typename Alloc> class VectorStorage : public Alloc
  {
public:
  VectorStorage(const Alloc& a)
      : Alloc(a),
        _first(reinterpret_cast<T*>(data)),
        _last(_first+Size),
        _end(_first)
    {
    }

  bool isUsingEmbeddedStorage() const { return _first == reinterpret_cast<const T*>(data); }

  // the first member of the vector
  T *_first;
  // the element one past the capasity.
  T *_last;
  // the element one past the end of the active elements
  T *_end;

private:
  typename ResourceDescriptionTypeHelper<T>::Storage data[Size];
  };

template <typename T, typename Alloc> class VectorStorage<T, 0, Alloc> : public Alloc
  {
public:
  typedef std::bidirectional_iterator_tag iterator_category;
  typedef T value_type;
  typedef ptrdiff_t difference_type;
  typedef T *pointer;
  typedef T &reference;
  VectorStorage(const Alloc& a)
      : Alloc(a),
        _first(0),
        _last(0),
        _end(0)
    {
    }

  bool isUsingEmbeddedStorage() const { return false; }

  // the first member of the vector
  T *_first;
  // the element one past the capasity.
  T *_last;
  // the element one past the end of the active elements
  T *_end;
  };
}

template <typename T,
          xsize PreallocatedSize = 0,
          typename Alloc = TypedAllocator<T> >
  class Vector
    : private detail::VectorStorage<T, PreallocatedSize, Alloc>
  {
public:
  typedef detail::VectorStorage<T, PreallocatedSize, Alloc> ThisBase;
  typedef Vector<T, PreallocatedSize, Alloc> ThisType;
  typedef xsize size_type;
  typedef T* iterator;
  typedef const T* const_iterator;
  typedef std::reverse_iterator<iterator> reverse_iterator;
  typedef std::reverse_iterator<const_iterator> const_reverse_iterator;

  Vector(const Alloc &alloc=Alloc())
      : ThisBase(alloc)
    {
    }

  template <typename X, xsize Pre, typename AllocX>
    Vector(const Vector<X, Pre, AllocX> &v, const Alloc &alloc=Alloc())
      : ThisBase(alloc)
    {
    resizeAndCopy(v.size(), v.begin());
    }

  Vector(const ThisType& cpy)
      : ThisBase(cpy.allocator())
    {
    resizeAndCopy(cpy.size(), cpy.begin());
    }

  template <typename X, xsize Pre, typename AllocX>
    Vector(Vector<X, Pre, AllocX> &&v)
      : ThisBase(v.allocator())
    {
    swap(v);
    }

  explicit Vector(size_type size, const T& t, const Alloc &alloc=Alloc())
      : ThisBase(alloc)
    {
    resize(size, t);
    }

  ThisType& operator=(const ThisType& oth)
    {
    clear();
    resizeAndCopy(oth.size(), oth.begin());
    return *this;
    }

  ThisType& operator=(ThisType&& oth)
    {
    swap(oth);
    return *this;
    }

  ~Vector()
    {
    clear();
    squeeze();
    xAssert(ThisBase::_first == 0);
    }

  size_type size() const
    {
    return ThisBase::_end - ThisBase::_first;
    }

  bool isEmpty() const
    {
    return ThisBase::_first == ThisBase::_end;
    }

  size_type capacity() const
    {
    return ThisBase::_last - ThisBase::_first;
    }

  T &front()
    {
    xAssert(ThisBase::_first);
    return *ThisBase::_first;
    }

  const T &front() const
    {
    xAssert(ThisBase::_first);
    return *ThisBase::_first;
    }

  T &back()
    {
    xAssert(ThisBase::_end-1);
    return *(ThisBase::_end-1);
    }

  const T &back() const
    {
    xAssert(ThisBase::_end-1);
    return *(ThisBase::_end-1);
    }

  T *data()
    {
    return ThisBase::_first;
    }

  const T *data() const
    {
    return ThisBase::_first;
    }

  ThisType& operator<<(T &&t)
    {
    pushBack(std::move(t));
    return *this;
    }

  ThisType& operator<<(const T &t)
    {
    pushBack(t);
    return *this;
    }

  template <typename VecX>
  ThisType& operator+=(const VecX &t)
    {
    append(t);
    return *this;
    }

  iterator begin()
    {
    return ThisBase::_first;
    }

  iterator end()
    {
    return ThisBase::_end;
    }

  const_iterator begin() const
    {
    return ThisBase::_first;
    }

  const_iterator end() const
    {
    return ThisBase::_end;
    }

  const_iterator cbegin() const
    {
    return ThisBase::_first;
    }

  const_iterator cend() const
    {
    return ThisBase::_end;
    }

  reverse_iterator rbegin()
    {
    return reverse_iterator(ThisBase::_end);
    }

  reverse_iterator rend()
    {
    return reverse_iterator(ThisBase::_first);
    }

  const_reverse_iterator rbegin() const
    {
    return const_reverse_iterator(ThisBase::_end);
    }

  const_reverse_iterator rend() const
    {
    return const_reverse_iterator(ThisBase::_first);
    }

  void reserve(size_type newCapacity)
    {
    if(newCapacity <= capacity())
      {
      return;
      }

    const size_type s = size();
    const size_type oldCapacity = capacity();

    const size_type newBestCapacity = bestCapacity(newCapacity);

    T *newData = ThisBase::allocate(newBestCapacity, AlignmentOf<T>::Alignment);

    moveConstruct(newData, newData+s, ThisBase::_first);

    // destroy the old data
    for(size_type i = 0; i < s; ++i)
      {
      AllocatorBase::destruct(ThisBase::_first+i);
      }

    // always destroy the old data if it existed
    if(ThisBase::_first && !isUsingEmbeddedStorage())
      {
      ThisBase::deallocate(ThisBase::_first, oldCapacity);
      }

    ThisBase::_first = newData;
    ThisBase::_end = ThisBase::_first + s;
    ThisBase::_last = ThisBase::_first + newBestCapacity;
    }

  void squeeze()
    {
    if(ThisBase::_last == ThisBase::_end)
      {
      return;
      }

    const size_type s = size();
    const size_type newCapacity = s;

    T *oldData = ThisBase::_first;
    bool shouldDestroy = oldData && !isUsingEmbeddedStorage();
    xsize oldCapacity = capacity();

    if(s)
      {
      T *newData = ThisBase::allocate(newCapacity);

      // move the new elements
      move(newData, newData+s, ThisBase::_first);

      // destroy the old data
      for(size_type i = 0; i < s; ++i)
        {
        AllocatorBase::destruct(oldData+i);
        }

      ThisBase::_first = newData;
      ThisBase::_last = ThisBase::_end = ThisBase::_first + s;
      }
    else
      {
      // no old size, just reset pointers
      ThisBase::_last = ThisBase::_end = ThisBase::_first = 0;
      }

    // always destroy the old data if it existed
    if(shouldDestroy)
      {
      ThisBase::deallocate(oldData, oldCapacity);
      }
    }

  void resize(size_type newSize, const T &val)
    {
    size_type oldSize = resizeBase(newSize);

    // construct the extra members
    for(size_type i = oldSize; i < newSize; ++i)
      {
      AllocatorBase::construct(&at(i), val);
      }
    }

  void resize(size_type newSize)
    {
    size_type oldSize = resizeBase(newSize);

    // construct the extra members
    for(size_type i = oldSize; i < newSize; ++i)
      {
      AllocatorBase::construct(&at(i), T());
      }
    }

  template <typename It>
      void resizeAndCopy(size_type newSize, It val)
    {
    size_type s = size();

    // destroy extra members
    for(size_type i = newSize; i < s; ++i)
      {
      AllocatorBase::destruct(&at(i));
      }

    // adjust the end
    ThisBase::_end = ThisBase::_first + std::min(s, newSize);

    // now expand, copying the needed members
    reserve(newSize);

    // adjust the new end
    ThisBase::_end = ThisBase::_first + newSize;

    // construct the extra members
    for(size_type i = s; i < newSize; ++i, ++val)
      {
      AllocatorBase::construct(&at(i), *val);
      }
    }

  template <typename It>
      void resizeAndMove(size_type newSize, It val)
    {
    size_type s = size();

    // destroy extra members
    for(size_type i = newSize; i < s; ++i)
      {
      AllocatorBase::destruct(&at(i));
      }

    // adjust the end
    ThisBase::_end = ThisBase::_first + std::min(s, newSize);

    // now expand, copying the needed members
    reserve(newSize);

    // adjust the new end
    ThisBase::_end = ThisBase::_first + newSize;

    // construct the extra members
    for(size_type i = s; i < newSize; ++i, ++val)
      {
      AllocatorBase::construct(&at(i), std::move(*val));
      }
    }

  void clear()
    {
    resize(0);
    }

  bool compare(const T *other, size_type count, size_type start=0) const
    {
    if(start + count > length())
      {
      return false;
      }

    const T *ths = data() + start;

    for(xsize i = 0; i < count; ++i)
      {
      if(*(ths+i) != *(other+i))
        {
        return false;
        }
      }
    return true;
    }

  bool operator==(const ThisType &oth) const
    {
    const size_type s = size();

    return s == oth.size() &&
        compare(oth.data(), s);
    }

  bool operator!=(const ThisType &oth) const
    {
    return !(*this == oth);
    }

  size_type length() const { return size(); }

  size_type indexOf(const T &t, size_type from = 0) const
    {
    const_iterator it = begin() + from;
    const_iterator found = std::find(it, cend(), t);

    if(found == end())
      {
      return std::numeric_limits<size_type>::max();
      }
    return found - begin();
    }

  size_type indexOf(const T *t, size_type count, size_type from) const
    {
    xAssert(count >= 0);
    size_type index = indexOf(t[0], from);
    for(size_type i = 1; i < count; ++i)
      {
      if(at(index + i) != t[i])
        {
        return indexOf(t, count, index+1);
        }
      }

    return index;
    }

  bool contains(const T &t) const
    {
    return indexOf(t) != std::numeric_limits<size_type>::max();;
    }

  T popBack()
    {
    xAssert(ThisBase::_end);

    --ThisBase::_end;

    T bck = std::move(*ThisBase::_end);

    // destroy extra member
    AllocatorBase::destruct(ThisBase::_end);

    return std::move(bck);
    }

  template <typename... Args> void emplaceBack(Args &&... args)
    {
    const size_type s = size();
    if(s == capacity())
      {
      reserve(s + 1);
      }

    xAssert(ThisBase::_end < ThisBase::_last);

    new(ThisBase::_end) T{std::forward<Args>(args)...};

    ThisBase::_end++;
    }

  void pushBack(const T &t)
    {
    const size_type s = size();
    if(s == capacity())
      {
      reserve(s + 1);
      }

    xAssert(ThisBase::_end < ThisBase::_last);
    AllocatorBase::construct(ThisBase::_end, t);
    ThisBase::_end++;
    }

  void pushBack(T &&t)
    {
    const size_type s = size();
    if(s == capacity())
      {
      reserve(s + 1);
      }

    xAssert(ThisBase::_end < ThisBase::_last);
    AllocatorBase::construct(ThisBase::_end, std::move(t));
    ThisBase::_end++;
    }

  T &createBack()
    {
    pushBack(T());
    return back();
    }

  template <typename X> void append(X vals, size_type count)
    {
    resizeAndCopy(size() + count, vals);
    }

  template <typename Vec> void append(const Vec &vals)
    {
    resizeAndCopy(size() + vals.size(), vals.begin());
    }

  template <typename X>
      void copy(const iterator &begin, const iterator &end, X from)
    {
    for(iterator it = begin; it != end; ++it, ++from)
      {
      *it = *from;
      }
    }

  template <typename X>
      void move(const iterator &begin, const iterator &end, X from)
    {
    for(iterator it = begin; it != end; ++it, ++from)
      {
      *it = std::move(*from);
      }
    }

  bool isUsingEmbeddedStorage() const { return ThisBase::isUsingEmbeddedStorage(); }

  T &at(size_type i)
    {
    xAssert(i < size());
    return *(ThisBase::_first + i);
    }

  const T &at(size_type i) const
    {
    xAssert(i < size());
    return *(ThisBase::_first + i);
    }

  T &operator[](size_type i)
    {
    return at(i);
    }

  const T &operator[](size_type i) const
    {
    return at(i);
    }

  void setAllocator(const Alloc &a)
    {
    xAssert(ThisBase::_first == 0);
    allocator() = a;
    }

  Alloc &allocator()
    {
    return *this;
    }

  const Alloc &allocator() const
    {
    return *this;
    }

  size_type bestCapacity(size_type s) const
    {
    const size_type expandExistingPolicy = 2;
    const size_type expandReservePolicy = 2;
    return std::max(s * expandExistingPolicy, size() * expandReservePolicy);
    }

  void removeAt(size_type idx)
    {
    remove(begin() + idx);
    }

  iterator remove(iterator it)
    {
    return remove(it, it + 1);
    }

  iterator remove(iterator b, iterator e)
    {
    xAssert(b >= ThisBase::_first && b <= ThisBase::_end);
    xAssert(e >= ThisBase::_first && e <= ThisBase::_end);
    xAssert(b <= ThisBase::_end);

    const size_type elementsToRemove = (e - b);
    const size_type newSize = size() - elementsToRemove;

    // remove all the intermediate elements
    for(iterator it = b, from = e;
        from != ThisBase::_end;
        ++it, ++from)
      {
      *it = std::move(*from);
      }

    resize(newSize);

    // return the new pointer to the element after the last element
    return b;
    }

  iterator erase(iterator b, iterator e)
    {
    return remove(b, e);
    }

  void removeAll(const T &t)
    {
    for(iterator i = begin(), e = end();
        i != e;
        ++i
        )
      {
      if(*i == t)
        {
        i = remove(i);
        e = end();

        if(i == e)
          {
          break;
          }
        }
      }
    }

  template <typename StringType> void replace(size_type i, size_type l, const StringType &newContent)
    {
    if (l >= newContent.size())
      {
      auto repBeg = begin() + i;
      const auto repNewEnd = repBeg + newContent.size();
      const auto repOldEnd = repBeg + l;

      copy(repBeg, repNewEnd, newContent.begin());

      if (repNewEnd != repOldEnd)
        {
        erase(repNewEnd, repOldEnd);
        }
      }
    else if (l < newContent.size())
      {
      auto repBeg = begin() + i;
      const auto repOldEnd = repBeg + l;
      auto newStart = newContent.begin() + l;
      auto newEnd = newContent.end();

      copy(repBeg, repOldEnd, newContent.begin());
      insert(repOldEnd, newStart, newEnd);
      }
    }

  template <class Input> void insert(iterator position, Input first, Input last)
    {
    xsize s = last - first;
    reserve(size() + s);

    // move old elements
    auto from = ThisBase::_end - 1;
    for (auto i = ThisBase::_end+s-1; i >= position; --i, --from)
      {
      new(i) T(*from);
      AllocatorBase::destruct(from);
      }

    // copy new elements.
    auto it = first;
    for (auto i = position; i < (position+s); ++i, ++it)
      {
      new(i) T(*it);
      }

    ThisBase::_end += s;
    }

private:
  template <xsize Pre, typename AllocX>
  void swap(Vector<T, Pre, AllocX> &oth)
    {
    if(oth.allocator() == oth.allocator() &&
       !isUsingEmbeddedStorage() &&
       !oth.isUsingEmbeddedStorage())
      {
      T *&f = ThisBase::_first;
      T *&l = ThisBase::_last;
      T *&e = ThisBase::_end;
      oth.swapInternal(&f, &l, &e);
      }
    else
      {
      clear();
      resizeAndMove(oth.size(), oth.data());
      }
    }

  void swapInternal(T **f, T **l, T **e)
    {
    std::swap(ThisBase::_first, *f);
    std::swap(ThisBase::_last, *l);
    std::swap(ThisBase::_end, *e);
    }

  size_type resizeBase(size_type newSize)
    {
    size_type s = size();

    // destroy extra members
    for(size_type i = newSize; i < s; ++i)
      {
      AllocatorBase::destruct(&at(i));
      }

    // adjust the end
    ThisBase::_end = ThisBase::_first + std::min(s, newSize);

    // now expand, copying the needed members
    reserve(newSize);

    // adjust the new end
    ThisBase::_end = ThisBase::_first + newSize;

    return s;
    }

  template <typename X>
      void moveConstruct(const iterator &begin, const iterator &end, X from)
    {
    for(iterator it = begin; it != end; ++it, ++from)
      {
      new(&(*it)) T(std::move(*from));
      }
    }

  };

namespace detail
{
typedef StaticTypedAllocator<AllocatorBase, int> testIntGSA;
typedef VectorStorage <int, 0, testIntGSA> testVectorStorage;

// this is important - a simple vector should only be three pointers
xCompileTimeAssert(sizeof(testVectorStorage) == sizeof(void *) * 3);
xCompileTimeAssert(sizeof(Vector <int, 0, testIntGSA>) == sizeof(testVectorStorage));

// a vector with preallocation should be more or less three pointers with the extra data appended
xCompileTimeAssert(sizeof(Vector <int, 6, testIntGSA>) == (sizeof(void *) * 3 + sizeof(int) * 6));

// and a vector with preallocation and an allocator should be only one extra pointer
xCompileTimeAssert(sizeof(Vector <int, 6, TypedAllocator<int> >)
                     == (sizeof(void *) * 3 + sizeof(void *) + sizeof(int) * 6));
}

}

#endif
