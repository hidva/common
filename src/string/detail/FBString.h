/*
 * Copyright 2016 Facebook, Inc.
 *
 * Licensed under the Apache License, Version 2.0 (the "License");
 * you may not use this file except in compliance with the License.
 * You may obtain a copy of the License at
 *
 *   http://www.apache.org/licenses/LICENSE-2.0
 *
 * Unless required by applicable law or agreed to in writing, software
 * distributed under the License is distributed on an "AS IS" BASIS,
 * WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
 * See the License for the specific language governing permissions and
 * limitations under the License.
 */

// @author: Andrei Alexandrescu (aalexandre)
// String type.

#pragma once

#include <atomic>
#include <limits>
#include <type_traits>

// This file appears in two locations: inside fbcode and in the
// libstdc++ source code (when embedding fbstring as std::string).
// To aid in this schizophrenic use, _LIBSTDCXX_FBSTRING is defined in
// libstdc++'s c++config.h, to gate use inside fbcode v. libstdc++.
#ifdef _LIBSTDCXX_FBSTRING

#pragma GCC system_header

// When used as std::string replacement always disable assertions.
#ifndef NDEBUG
#define NDEBUG
#define FOLLY_DEFINED_NDEBUG_FOR_FBSTRING
#endif // NDEBUG

// Handle the cases where the fbcode version (folly/Malloc.h) is included
// either before or after this inclusion.
// #ifdef FOLLY_MALLOC_H_
// #undef FOLLY_MALLOC_H_
// #include "basic_fbstring_malloc.h" // nolint
// #else
// #include "basic_fbstring_malloc.h" // nolint
// #undef FOLLY_MALLOC_H_
// #endif

#else // !_LIBSTDCXX_FBSTRING

// #include <folly/Portability.h>

// libc++ doesn't provide this header, nor does msvc
// #ifdef FOLLY_HAVE_BITS_CXXCONFIG_H
// #include <bits/c++config.h>
// #endif

#include <string>
#include <cstring>
#include <cassert>
#include <algorithm>

// #include <folly/Traits.h>
// #include <folly/Malloc.h>
// #include <folly/Hash.h>
// #include <folly/ScopeGuard.h>

// #if FOLLY_HAVE_DEPRECATED_ASSOC
// #ifdef _GLIBCXX_SYMVER
// #include <ext/hash_set>
// #include <ext/hash_map>
// #endif
// #endif

#endif

// We defined these here rather than including Likely.h to avoid
// redefinition errors when fbstring is imported into libstdc++.
#if defined(__GNUC__) && __GNUC__ >= 4
#define FBSTRING_LIKELY(x)   (__builtin_expect((x), 1))
#define FBSTRING_UNLIKELY(x) (__builtin_expect((x), 0))
#else
#define FBSTRING_LIKELY(x)   (x)
#define FBSTRING_UNLIKELY(x) (x)
#endif

#pragma GCC diagnostic push
// Ignore shadowing warnings within this file, so includers can use -Wshadow.
#pragma GCC diagnostic ignored "-Wshadow"
// GCC 4.9 has a false positive in setSmallSize (probably
// https://gcc.gnu.org/bugzilla/show_bug.cgi?id=59124), disable
// compile-time array bound checking.
#pragma GCC diagnostic ignored "-Warray-bounds"

// FBString cannot use throw when replacing std::string, though it may still
// use std::__throw_*
// nolint
#define throw FOLLY_FBSTRING_MAY_NOT_USE_THROW

#ifdef _LIBSTDCXX_FBSTRING
namespace std _GLIBCXX_VISIBILITY(default) {
_GLIBCXX_BEGIN_NAMESPACE_VERSION
#else
namespace folly {
#endif

#if defined(__clang__)
# if __has_feature(address_sanitizer)
#  define FBSTRING_SANITIZE_ADDRESS
# endif
#elif defined (__GNUC__) && \
      (((__GNUC__ == 4) && (__GNUC_MINOR__ >= 8)) || (__GNUC__ >= 5)) && \
      __SANITIZE_ADDRESS__
# define FBSTRING_SANITIZE_ADDRESS
#endif

// When compiling with ASan, always heap-allocate the string even if
// it would fit in-situ, so that ASan can detect access to the string
// buffer after it has been invalidated (destroyed, resized, etc.).
// Note that this flag doesn't remove support for in-situ strings, as
// that would break ABI-compatibility and wouldn't allow linking code
// compiled with this flag with code compiled without.
#ifdef FBSTRING_SANITIZE_ADDRESS
# define FBSTRING_DISABLE_SSO true
#else
# define FBSTRING_DISABLE_SSO false
#endif

/**
 * Defines a special acquisition method for constructing fbstring
 * objects. AcquireMallocatedString means that the user passes a
 * pointer to a malloc-allocated string that the fbstring object will
 * take into custody.
 */
enum class AcquireMallocatedString {};

/*
 * fbstring_core_model is a mock-up type that defines all required
 * signatures of a fbstring core. The fbstring class itself uses such
 * a core object to implement all of the numerous member functions
 * required by the standard.
 *
 * If you want to define a new core, copy the definition below and
 * implement the primitives. Then plug the core into basic_fbstring as
 * a template argument.

template <class Char>
class fbstring_core_model {
public:
  fbstring_core_model();
  fbstring_core_model(const fbstring_core_model &);
  ~fbstring_core_model();
  // Returns a pointer to string's buffer (currently only contiguous
  // strings are supported). The pointer is guaranteed to be valid
  // until the next call to a non-const member function.
  const Char * data() const;
  // Much like data(), except the string is prepared to support
  // character-level changes. This call is a signal for
  // e.g. reference-counted implementation to fork the data. The
  // pointer is guaranteed to be valid until the next call to a
  // non-const member function.
  Char * mutable_data();
  // Returns a pointer to string's buffer and guarantees that a
  // readable '\0' lies right after the buffer. The pointer is
  // guaranteed to be valid until the next call to a non-const member
  // function.
  const Char * c_str() const;
  // Shrinks the string by delta characters. Asserts that delta <=
  // size().
  void shrink(size_t delta);
  // Expands the string by delta characters (i.e. after this call
  // size() will report the old size() plus delta) but without
  // initializing the expanded region. The expanded region is
  // zero-terminated. Returns a pointer to the memory to be
  // initialized (the beginning of the expanded portion). The caller
  // is expected to fill the expanded area appropriately.
  // If expGrowth is true, exponential growth is guaranteed.
  // It is not guaranteed not to reallocate even if size() + delta <
  // capacity(), so all references to the buffer are invalidated.
  Char* expand_noinit(size_t delta, bool expGrowth);
  // Expands the string by one character and sets the last character
  // to c.
  void push_back(Char c);
  // Returns the string's size.
  size_t size() const;
  // Returns the string's capacity, i.e. maximum size that the string
  // can grow to without reallocation. Note that for reference counted
  // strings that's technically a lie - even assigning characters
  // within the existing size would cause a reallocation.
  size_t capacity() const;
  // Returns true if the data underlying the string is actually shared
  // across multiple strings (in a refcounted fashion).
  bool isShared() const;
  // Makes sure that at least minCapacity characters are available for
  // the string without reallocation. For reference-counted strings,
  // it should fork the data even if minCapacity < size().
  void reserve(size_t minCapacity);
private:
  // Do not implement
  fbstring_core_model& operator=(const fbstring_core_model &);
};
*/

/**
 * This is the core of the string. The code should work on 32- and
 * 64-bit and both big- and little-endianan architectures with any
 * Char size.
 *
 * The storage is selected as follows (assuming we store one-byte
 * characters on a 64-bit machine): (a) "small" strings between 0 and
 * 23 chars are stored in-situ without allocation (the rightmost byte
 * stores the size); (b) "medium" strings from 24 through 254 chars
 * are stored in malloc-allocated memory that is copied eagerly; (c)
 * "large" strings of 255 chars and above are stored in a similar
 * structure as medium arrays, except that the string is
 * reference-counted and copied lazily. the reference count is
 * allocated right before the character array.
 *
 * The discriminator between these three strategies sits in two
 * bits of the rightmost char of the storage. If neither is set, then the
 * string is small (and its length sits in the lower-order bits on
 * little-endian or the high-order bits on big-endian of that
 * rightmost character). If the MSb is set, the string is medium width.
 * If the second MSb is set, then the string is large. On little-endian,
 * these 2 bits are the 2 MSbs of MediumLarge::capacity_, while on
 * big-endian, these 2 bits are the 2 LSbs. This keeps both little-endian
 * and big-endian fbstring_core equivalent with merely different ops used
 * to extract capacity/category.
 */
template <class Char> class fbstring_core {
protected:
// It's MSVC, so we just have to guess ... and allow an override
#ifdef _MSC_VER
# ifdef FOLLY_ENDIAN_BE
  static constexpr auto kIsLittleEndian = false;
# else
  static constexpr auto kIsLittleEndian = true;
# endif
#else
  static constexpr auto kIsLittleEndian =
      __BYTE_ORDER__ == __ORDER_LITTLE_ENDIAN__;
#endif
public:
  fbstring_core() noexcept { reset(); }

  fbstring_core(const fbstring_core & rhs) {
    assert(&rhs != this);
    // Simplest case first: small strings are bitblitted
    if (rhs.category() == Category::isSmall) {
      static_assert(offsetof(MediumLarge, data_) == 0,
          "fbstring layout failure");
      static_assert(offsetof(MediumLarge, size_) == sizeof(ml_.data_),
          "fbstring layout failure");
      static_assert(offsetof(MediumLarge, capacity_) == 2 * sizeof(ml_.data_),
          "fbstring layout failure");
      // Just write the whole thing, don't look at details. In
      // particular we need to copy capacity anyway because we want
      // to set the size (don't forget that the last character,
      // which stores a short string's length, is shared with the
      // ml_.capacity field).
      ml_ = rhs.ml_;
      assert(category() == Category::isSmall && this->size() == rhs.size());
    } else if (rhs.category() == Category::isLarge) {
      // Large strings are just refcounted
      ml_ = rhs.ml_;
      RefCounted::incrementRefs(ml_.data_);
      assert(category() == Category::isLarge && size() == rhs.size());
    } else {
      // Medium strings are copied eagerly. Don't forget to allocate
      // one extra Char for the null terminator.
      auto const allocSize =
           goodMallocSize((1 + rhs.ml_.size_) * sizeof(Char));
      ml_.data_ = static_cast<Char*>(checkedMalloc(allocSize));
      // Also copies terminator.
      fbstring_detail::pod_copy(rhs.ml_.data_,
                                rhs.ml_.data_ + rhs.ml_.size_ + 1,
                                ml_.data_);
      ml_.size_ = rhs.ml_.size_;
      ml_.setCapacity(allocSize / sizeof(Char) - 1, Category::isMedium);
      assert(category() == Category::isMedium);
    }
    assert(size() == rhs.size());
    assert(memcmp(data(), rhs.data(), size() * sizeof(Char)) == 0);
  }

  fbstring_core(fbstring_core&& goner) noexcept {
    // Take goner's guts
    ml_ = goner.ml_;
    // Clean goner's carcass
    goner.reset();
  }

  fbstring_core(const Char *const data,
                const size_t size,
                bool disableSSO = FBSTRING_DISABLE_SSO) {
#ifndef NDEBUG
#ifndef _LIBSTDCXX_FBSTRING
    SCOPE_EXIT {
      assert(this->size() == size);
      assert(memcmp(this->data(), data, size * sizeof(Char)) == 0);
    };
#endif
#endif

    // Simplest case first: small strings are bitblitted
    if (!disableSSO && size <= maxSmallSize) {
      // Layout is: Char* data_, size_t size_, size_t capacity_
      static_assert(sizeof(*this) == sizeof(Char*) + 2 * sizeof(size_t),
          "fbstring has unexpected size");
      static_assert(sizeof(Char*) == sizeof(size_t),
          "fbstring size assumption violation");
      // sizeof(size_t) must be a power of 2
      static_assert((sizeof(size_t) & (sizeof(size_t) - 1)) == 0,
          "fbstring size assumption violation");

      // If data is aligned, use fast word-wise copying. Otherwise,
      // use conservative memcpy.
      // The word-wise path reads bytes which are outside the range of
      // the string, and makes ASan unhappy, so we disable it when
      // compiling with ASan.
#ifndef FBSTRING_SANITIZE_ADDRESS
      if ((reinterpret_cast<size_t>(data) & (sizeof(size_t) - 1)) == 0) {
        const size_t byteSize = size * sizeof(Char);
        constexpr size_t wordWidth = sizeof(size_t);
        switch ((byteSize + wordWidth - 1) / wordWidth) { // Number of words.
        case 3:
          ml_.capacity_ = reinterpret_cast<const size_t*>(data)[2];
        case 2:
          ml_.size_ = reinterpret_cast<const size_t*>(data)[1];
        case 1:
          ml_.data_ = *reinterpret_cast<Char**>(const_cast<Char*>(data));
        case 0:
          break;
        }
      } else
#endif
      {
        fbstring_detail::pod_copy(data, data + size, small_);
      }
      setSmallSize(size);
    } else {
      if (size <= maxMediumSize) {
        // Medium strings are allocated normally. Don't forget to
        // allocate one extra Char for the terminating null.
        auto const allocSize = goodMallocSize((1 + size) * sizeof(Char));
        ml_.data_ = static_cast<Char*>(checkedMalloc(allocSize));
        fbstring_detail::pod_copy(data, data + size, ml_.data_);
        ml_.size_ = size;
        ml_.setCapacity(allocSize / sizeof(Char) - 1, Category::isMedium);
      } else {
        // Large strings are allocated differently
        size_t effectiveCapacity = size;
        auto const newRC = RefCounted::create(data, & effectiveCapacity);
        ml_.data_ = newRC->data_;
        ml_.size_ = size;
        ml_.setCapacity(effectiveCapacity, Category::isLarge);
      }
      ml_.data_[size] = '\0';
    }
  }

  ~fbstring_core() noexcept {
    auto const c = category();
    if (c == Category::isSmall) {
      return;
    }
    if (c == Category::isMedium) {
      free(ml_.data_);
      return;
    }
    RefCounted::decrementRefs(ml_.data_);
  }

  // Snatches a previously mallocated string. The parameter "size"
  // is the size of the string, and the parameter "allocatedSize"
  // is the size of the mallocated block.  The string must be
  // \0-terminated, so allocatedSize >= size + 1 and data[size] == '\0'.
  //
  // So if you want a 2-character string, pass malloc(3) as "data",
  // pass 2 as "size", and pass 3 as "allocatedSize".
  fbstring_core(Char * const data,
                const size_t size,
                const size_t allocatedSize,
                AcquireMallocatedString) {
    if (size > 0) {
      assert(allocatedSize >= size + 1);
      assert(data[size] == '\0');
      // Use the medium string storage
      ml_.data_ = data;
      ml_.size_ = size;
      // Don't forget about null terminator
      ml_.setCapacity(allocatedSize - 1, Category::isMedium);
    } else {
      // No need for the memory
      free(data);
      reset();
    }
  }

  // swap below doesn't test whether &rhs == this (and instead
  // potentially does extra work) on the premise that the rarity of
  // that situation actually makes the check more expensive than is
  // worth.
  void swap(fbstring_core & rhs) {
    auto const t = ml_;
    ml_ = rhs.ml_;
    rhs.ml_ = t;
  }

  // In C++11 data() and c_str() are 100% equivalent.
  const Char * data() const {
    return c_str();
  }

  Char * mutable_data() {
    auto const c = category();
    if (c == Category::isSmall) {
      return small_;
    }
    assert(c == Category::isMedium || c == Category::isLarge);
    if (c == Category::isLarge && RefCounted::refs(ml_.data_) > 1) {
      // Ensure unique.
      size_t effectiveCapacity = ml_.capacity();
      auto const newRC = RefCounted::create(& effectiveCapacity);
      // If this fails, someone placed the wrong capacity in an
      // fbstring.
      assert(effectiveCapacity >= ml_.capacity());
      // Also copies terminator.
      fbstring_detail::pod_copy(ml_.data_, ml_.data_ + ml_.size_ + 1,
                                newRC->data_);
      RefCounted::decrementRefs(ml_.data_);
      ml_.data_ = newRC->data_;
    }
    return ml_.data_;
  }

  const Char * c_str() const {
    auto const c = category();
    if (c == Category::isSmall) {
      assert(small_[smallSize()] == '\0');
      return small_;
    }
    assert(c == Category::isMedium || c == Category::isLarge);
    assert(ml_.data_[ml_.size_] == '\0');
    return ml_.data_;
  }

  void shrink(const size_t delta) {
    if (category() == Category::isSmall) {
      // Check for underflow
      assert(delta <= smallSize());
      setSmallSize(smallSize() - delta);
    } else if (category() == Category::isMedium ||
               RefCounted::refs(ml_.data_) == 1) {
      // Medium strings and unique large strings need no special
      // handling.
      assert(ml_.size_ >= delta);
      ml_.size_ -= delta;
      ml_.data_[ml_.size_] = '\0';
    } else {
      assert(ml_.size_ >= delta);
      // Shared large string, must make unique. This is because of the
      // durn terminator must be written, which may trample the shared
      // data.
      if (delta) {
        fbstring_core(ml_.data_, ml_.size_ - delta).swap(*this);
      }
      // No need to write the terminator.
    }
  }

  void reserve(size_t minCapacity, bool disableSSO = FBSTRING_DISABLE_SSO) {
    if (category() == Category::isLarge) {
      // Ensure unique
      if (RefCounted::refs(ml_.data_) > 1) {
        // We must make it unique regardless; in-place reallocation is
        // useless if the string is shared. In order to not surprise
        // people, reserve the new block at current capacity or
        // more. That way, a string's capacity never shrinks after a
        // call to reserve.
        minCapacity = std::max(minCapacity, ml_.capacity());
        auto const newRC = RefCounted::create(& minCapacity);
        // Also copies terminator.
        fbstring_detail::pod_copy(ml_.data_, ml_.data_ + ml_.size_ + 1,
                                   newRC->data_);
        RefCounted::decrementRefs(ml_.data_);
        ml_.data_ = newRC->data_;
        ml_.setCapacity(minCapacity, Category::isLarge);
        // size remains unchanged
      } else {
        // String is not shared, so let's try to realloc (if needed)
        if (minCapacity > ml_.capacity()) {
          // Asking for more memory
          auto const newRC =
               RefCounted::reallocate(ml_.data_, ml_.size_,
                                      ml_.capacity(), minCapacity);
          ml_.data_ = newRC->data_;
          ml_.setCapacity(minCapacity, Category::isLarge);
        }
        assert(capacity() >= minCapacity);
      }
    } else if (category() == Category::isMedium) {
      // String is not shared
      if (minCapacity <= ml_.capacity()) {
        return; // nothing to do, there's enough room
      }
      if (minCapacity <= maxMediumSize) {
        // Keep the string at medium size. Don't forget to allocate
        // one extra Char for the terminating null.
        size_t capacityBytes = goodMallocSize((1 + minCapacity) * sizeof(Char));
        // Also copies terminator.
        ml_.data_ = static_cast<Char *>(
          smartRealloc(
            ml_.data_,
            (ml_.size_ + 1) * sizeof(Char),
            (ml_.capacity() + 1) * sizeof(Char),
            capacityBytes));
        ml_.setCapacity(capacityBytes / sizeof(Char) - 1, Category::isMedium);
      } else {
        // Conversion from medium to large string
        fbstring_core nascent;
        // Will recurse to another branch of this function
        nascent.reserve(minCapacity);
        nascent.ml_.size_ = ml_.size_;
        // Also copies terminator.
        fbstring_detail::pod_copy(ml_.data_, ml_.data_ + ml_.size_ + 1,
                                  nascent.ml_.data_);
        nascent.swap(*this);
        assert(capacity() >= minCapacity);
      }
    } else {
      assert(category() == Category::isSmall);
      if (!disableSSO && minCapacity <= maxSmallSize) {
        // small
        // Nothing to do, everything stays put
      } else if (minCapacity <= maxMediumSize) {
        // medium
        // Don't forget to allocate one extra Char for the terminating null
        auto const allocSizeBytes =
          goodMallocSize((1 + minCapacity) * sizeof(Char));
        auto const pData = static_cast<Char*>(checkedMalloc(allocSizeBytes));
        auto const size = smallSize();
        // Also copies terminator.
        fbstring_detail::pod_copy(small_, small_ + size + 1, pData);
        ml_.data_ = pData;
        ml_.size_ = size;
        ml_.setCapacity(allocSizeBytes / sizeof(Char) - 1, Category::isMedium);
      } else {
        // large
        auto const newRC = RefCounted::create(& minCapacity);
        auto const size = smallSize();
        // Also copies terminator.
        fbstring_detail::pod_copy(small_, small_ + size + 1, newRC->data_);
        ml_.data_ = newRC->data_;
        ml_.size_ = size;
        ml_.setCapacity(minCapacity, Category::isLarge);
        assert(capacity() >= minCapacity);
      }
    }
    assert(capacity() >= minCapacity);
  }

  Char * expand_noinit(const size_t delta,
                       bool expGrowth = false,
                       bool disableSSO = FBSTRING_DISABLE_SSO) {
    // Strategy is simple: make room, then change size
    assert(capacity() >= size());
    size_t sz, newSz;
    if (category() == Category::isSmall) {
      sz = smallSize();
      newSz = sz + delta;
      if (!disableSSO && FBSTRING_LIKELY(newSz <= maxSmallSize)) {
        setSmallSize(newSz);
        return small_ + sz;
      }
      reserve(expGrowth ? std::max(newSz, 2 * maxSmallSize) : newSz);
    } else {
      sz = ml_.size_;
      newSz = sz + delta;
      if (FBSTRING_UNLIKELY(newSz > capacity())) {
        // ensures not shared
        reserve(expGrowth ? std::max(newSz, 1 + capacity() * 3 / 2) : newSz);
      }
    }
    assert(capacity() >= newSz);
    // Category can't be small - we took care of that above
    assert(category() == Category::isMedium || category() == Category::isLarge);
    ml_.size_ = newSz;
    ml_.data_[newSz] = '\0';
    assert(size() == newSz);
    return ml_.data_ + sz;
  }

  void push_back(Char c) {
    *expand_noinit(1, /* expGrowth = */ true) = c;
  }

  size_t size() const {
    return category() == Category::isSmall ? smallSize() : ml_.size_;
  }

  size_t capacity() const {
    switch (category()) {
      case Category::isSmall:
        return maxSmallSize;
      case Category::isLarge:
        // For large-sized strings, a multi-referenced chunk has no
        // available capacity. This is because any attempt to append
        // data would trigger a new allocation.
        if (RefCounted::refs(ml_.data_) > 1) return ml_.size_;
      default: {}
    }
    return ml_.capacity();
  }

  bool isShared() const {
    return category() == Category::isLarge && RefCounted::refs(ml_.data_) > 1;
  }

private:
  // Disabled
  fbstring_core & operator=(const fbstring_core & rhs);

  // Equivalent to setSmallSize(0) but a few ns faster in
  // microbenchmarks.
  void reset() {
    ml_.capacity_ = kIsLittleEndian
      ? maxSmallSize << (8 * (sizeof(size_t) - sizeof(Char)))
      : maxSmallSize << 2;
    small_[0] = '\0';
    assert(category() == Category::isSmall && size() == 0);
  }

  struct RefCounted {
    std::atomic<size_t> refCount_;
    Char data_[1];

    static RefCounted * fromData(Char * p) {
      return static_cast<RefCounted*>(
        static_cast<void*>(
          static_cast<unsigned char*>(static_cast<void*>(p))
          - sizeof(refCount_)));
    }

    static size_t refs(Char * p) {
      return fromData(p)->refCount_.load(std::memory_order_acquire);
    }

    static void incrementRefs(Char * p) {
      fromData(p)->refCount_.fetch_add(1, std::memory_order_acq_rel);
    }

    static void decrementRefs(Char * p) {
      auto const dis = fromData(p);
      size_t oldcnt = dis->refCount_.fetch_sub(1, std::memory_order_acq_rel);
      assert(oldcnt > 0);
      if (oldcnt == 1) {
        free(dis);
      }
    }

    static RefCounted * create(size_t * size) {
      // Don't forget to allocate one extra Char for the terminating
      // null. In this case, however, one Char is already part of the
      // struct.
      const size_t allocSize = goodMallocSize(
        sizeof(RefCounted) + *size * sizeof(Char));
      auto result = static_cast<RefCounted*>(checkedMalloc(allocSize));
      result->refCount_.store(1, std::memory_order_release);
      *size = (allocSize - sizeof(RefCounted)) / sizeof(Char);
      return result;
    }

    static RefCounted * create(const Char * data, size_t * size) {
      const size_t effectiveSize = *size;
      auto result = create(size);
      fbstring_detail::pod_copy(data, data + effectiveSize, result->data_);
      return result;
    }

    static RefCounted * reallocate(Char *const data,
                                   const size_t currentSize,
                                   const size_t currentCapacity,
                                   const size_t newCapacity) {
      assert(newCapacity > 0 && newCapacity > currentSize);
      auto const dis = fromData(data);
      assert(dis->refCount_.load(std::memory_order_acquire) == 1);
      // Don't forget to allocate one extra Char for the terminating
      // null. In this case, however, one Char is already part of the
      // struct.
      auto result = static_cast<RefCounted*>(
             smartRealloc(dis,
                          sizeof(RefCounted) + currentSize * sizeof(Char),
                          sizeof(RefCounted) + currentCapacity * sizeof(Char),
                          sizeof(RefCounted) + newCapacity * sizeof(Char)));
      assert(result->refCount_.load(std::memory_order_acquire) == 1);
      return result;
    }
  };

  typedef std::conditional<sizeof(size_t) == 4, uint32_t, uint64_t>::type
          category_type;

  enum class Category : category_type {
    isSmall = 0,
    isMedium = kIsLittleEndian
      ? sizeof(size_t) == 4 ? 0x80000000 : 0x8000000000000000
      : 0x2,
    isLarge =  kIsLittleEndian
      ? sizeof(size_t) == 4 ? 0x40000000 : 0x4000000000000000
      : 0x1,
  };

  Category category() const {
    // works for both big-endian and little-endian
    return static_cast<Category>(ml_.capacity_ & categoryExtractMask);
  }

  struct MediumLarge {
    Char * data_;
    size_t size_;
    size_t capacity_;

    size_t capacity() const {
      return kIsLittleEndian
        ? capacity_ & capacityExtractMask
        : capacity_ >> 2;
    }

    void setCapacity(size_t cap, Category cat) {
        capacity_ = kIsLittleEndian
          ? cap | static_cast<category_type>(cat)
          : (cap << 2) | static_cast<category_type>(cat);
    }
  };

  union {
    Char small_[sizeof(MediumLarge) / sizeof(Char)];
    MediumLarge ml_;
  };

  enum : size_t {
    lastChar = sizeof(MediumLarge) - 1,
    maxSmallSize = lastChar / sizeof(Char),
    maxMediumSize = 254 / sizeof(Char),            // coincides with the small
                                                   // bin size in dlmalloc
    categoryExtractMask = kIsLittleEndian
      ? sizeof(size_t) == 4 ? 0xC0000000 : 0xC000000000000000
      : 0x3,
    capacityExtractMask = kIsLittleEndian
      ? ~categoryExtractMask
      : 0x0 /*unused*/,
  };
  static_assert(!(sizeof(MediumLarge) % sizeof(Char)),
                "Corrupt memory layout for fbstring.");

  size_t smallSize() const {
    assert(category() == Category::isSmall);
    constexpr auto shift = kIsLittleEndian ? 0 : 2;
    auto smallShifted = static_cast<size_t>(small_[maxSmallSize]) >> shift;
    assert(static_cast<size_t>(maxSmallSize) >= smallShifted);
    return static_cast<size_t>(maxSmallSize) - smallShifted;
  }

  void setSmallSize(size_t s) {
    // Warning: this should work with uninitialized strings too,
    // so don't assume anything about the previous value of
    // small_[maxSmallSize].
    assert(s <= maxSmallSize);
    constexpr auto shift = kIsLittleEndian ? 0 : 2;
    small_[maxSmallSize] = (maxSmallSize - s) << shift;
    small_[s] = '\0';
    assert(category() == Category::isSmall && size() == s);
  }
};

#ifndef _LIBSTDCXX_FBSTRING
/**
 * Dummy fbstring core that uses an actual std::string. This doesn't
 * make any sense - it's just for testing purposes.
 */
template <class Char>
class dummy_fbstring_core {
public:
  dummy_fbstring_core() {
  }
  dummy_fbstring_core(const dummy_fbstring_core& another)
      : backend_(another.backend_) {
  }
  dummy_fbstring_core(const Char * s, size_t n)
      : backend_(s, n) {
  }
  void swap(dummy_fbstring_core & rhs) {
    backend_.swap(rhs.backend_);
  }
  const Char * data() const {
    return backend_.data();
  }
  Char * mutable_data() {
    //assert(!backend_.empty());
    return &*backend_.begin();
  }
  void shrink(size_t delta) {
    assert(delta <= size());
    backend_.resize(size() - delta);
  }
  Char * expand_noinit(size_t delta) {
    auto const sz = size();
    backend_.resize(size() + delta);
    return backend_.data() + sz;
  }
  void push_back(Char c) {
    backend_.push_back(c);
  }
  size_t size() const {
    return backend_.size();
  }
  size_t capacity() const {
    return backend_.capacity();
  }
  bool isShared() const {
    return false;
  }
  void reserve(size_t minCapacity) {
    backend_.reserve(minCapacity);
  }

private:
  std::basic_string<Char> backend_;
};
#endif // !_LIBSTDCXX_FBSTRING

/**
 * This is the basic_string replacement. For conformity,
 * basic_fbstring takes the same template parameters, plus the last
 * one which is the core.
 */
#ifdef _LIBSTDCXX_FBSTRING
template <typename E, class T, class A, class Storage>
#else
template <typename E,
          class T = std::char_traits<E>,
          class A = std::allocator<E>,
          class Storage = fbstring_core<E> >
#endif
class basic_fbstring {

  static void enforce(
      bool condition,
      void (*throw_exc)(const char*),
      const char* msg) {
    if (!condition) throw_exc(msg);
  }

  bool isSane() const {
    return
      begin() <= end() &&
      empty() == (size() == 0) &&
      empty() == (begin() == end()) &&
      size() <= max_size() &&
      capacity() <= max_size() &&
      size() <= capacity() &&
      begin()[size()] == '\0';
  }

  struct Invariant;
  friend struct Invariant;
  struct Invariant {
#ifndef NDEBUG
    explicit Invariant(const basic_fbstring& s) : s_(s) {
      assert(s_.isSane());
    }
    ~Invariant() {
      assert(s_.isSane());
    }
  private:
    const basic_fbstring& s_;
#else
    explicit Invariant(const basic_fbstring&) {}
#endif
    Invariant& operator=(const Invariant&);
  };

public:
  // types
  typedef T traits_type;
  typedef typename traits_type::char_type value_type;
  typedef A allocator_type;
  typedef typename A::size_type size_type;
  typedef typename A::difference_type difference_type;

  typedef typename A::reference reference;
  typedef typename A::const_reference const_reference;
  typedef typename A::pointer pointer;
  typedef typename A::const_pointer const_pointer;

  typedef E* iterator;
  typedef const E* const_iterator;
  typedef std::reverse_iterator<iterator
#ifdef NO_ITERATOR_TRAITS
                                , value_type
#endif
                                > reverse_iterator;
  typedef std::reverse_iterator<const_iterator
#ifdef NO_ITERATOR_TRAITS
                                , const value_type
#endif
                                > const_reverse_iterator;

  static const size_type npos;                     // = size_type(-1)
  typedef std::true_type IsRelocatable;

private:
  static void procrustes(size_type& n, size_type nmax) {
    if (n > nmax) n = nmax;
  }

public:

private:
  basic_fbstring& replaceImplDiscr(iterator i1, iterator i2,
                                   const value_type* s, size_type n,
                                   Selector<2>) {
    assert(i1 <= i2);
    assert(begin() <= i1 && i1 <= end());
    assert(begin() <= i2 && i2 <= end());
    return replace(i1, i2, s, s + n);
  }

  basic_fbstring& replaceImplDiscr(iterator i1, iterator i2,
                                   size_type n2, value_type c, Selector<1>) {
    const size_type n1 = i2 - i1;
    if (n1 > n2) {
      std::fill(i1, i1 + n2, c);
      erase(i1 + n2, i2);
    } else {
      std::fill(i1, i2, c);
      insert(i2, n2 - n1, c);
    }
    assert(isSane());
    return *this;
  }

  template <class InputIter>
  basic_fbstring& replaceImplDiscr(iterator i1, iterator i2,
                                   InputIter b, InputIter e,
                                   Selector<0>) {
    replaceImpl(i1, i2, b, e,
                typename std::iterator_traits<InputIter>::iterator_category());
    return *this;
  }

private:
 template <class FwdIterator>
 bool replaceAliased(iterator /* i1 */,
                     iterator /* i2 */,
                     FwdIterator /* s1 */,
                     FwdIterator /* s2 */,
                     std::false_type) {
    return false;
  }

  template <class FwdIterator>
  bool replaceAliased(iterator i1, iterator i2,
                      FwdIterator s1, FwdIterator s2, std::true_type) {
    static const std::less_equal<const value_type*> le =
      std::less_equal<const value_type*>();
    const bool aliased = le(&*begin(), &*s1) && le(&*s1, &*end());
    if (!aliased) {
      return false;
    }
    // Aliased replace, copy to new string
    basic_fbstring temp;
    temp.reserve(size() - (i2 - i1) + std::distance(s1, s2));
    temp.append(begin(), i1).append(s1, s2).append(i2, end());
    swap(temp);
    return true;
  }

  template <class FwdIterator>
  void replaceImpl(iterator i1, iterator i2,
                   FwdIterator s1, FwdIterator s2, std::forward_iterator_tag) {
    Invariant checker(*this);

    // Handle aliased replace
    if (replaceAliased(i1, i2, s1, s2,
          std::integral_constant<bool,
            std::is_same<FwdIterator, iterator>::value ||
            std::is_same<FwdIterator, const_iterator>::value>())) {
      return;
    }

    auto const n1 = i2 - i1;
    assert(n1 >= 0);
    auto const n2 = std::distance(s1, s2);
    assert(n2 >= 0);

    if (n1 > n2) {
      // shrinks
      std::copy(s1, s2, i1);
      erase(i1 + n2, i2);
    } else {
      // grows
      fbstring_detail::copy_n(s1, n1, i1);
      std::advance(s1, n1);
      insert(i2, s1, s2);
    }
    assert(isSane());
  }

  template <class InputIterator>
  void replaceImpl(iterator i1, iterator i2,
                   InputIterator b, InputIterator e, std::input_iterator_tag) {
    basic_fbstring temp(begin(), i1);
    temp.append(b, e).append(i2, end());
    swap(temp);
  }

public:
  template <class T1, class T2>
  basic_fbstring& replace(iterator i1, iterator i2,
                          T1 first_or_n_or_s, T2 last_or_c_or_n) {
    const bool
      num1 = std::numeric_limits<T1>::is_specialized,
      num2 = std::numeric_limits<T2>::is_specialized;
    return replaceImplDiscr(
      i1, i2, first_or_n_or_s, last_or_c_or_n,
      Selector<num1 ? (num2 ? 1 : -1) : (num2 ? 2 : 0)>());
  }

  size_type copy(value_type* s, size_type n, size_type pos = 0) const {
    enforce(pos <= size(), std::__throw_out_of_range, "");
    procrustes(n, size() - pos);

    fbstring_detail::pod_copy(
      data() + pos,
      data() + pos + n,
      s);
    return n;
  }

  void swap(basic_fbstring& rhs) {
    store_.swap(rhs.store_);
  }

  const value_type* c_str() const {
    return store_.c_str();
  }

  const value_type* data() const { return c_str(); }

  allocator_type get_allocator() const {
    return allocator_type();
  }

  size_type find(const basic_fbstring& str, size_type pos = 0) const {
    return find(str.data(), pos, str.length());
  }

  size_type find(const value_type* needle, const size_type pos,
                 const size_type nsize) const {
    if (!nsize) return pos;
    auto const size = this->size();
    // nsize + pos can overflow (eg pos == npos), guard against that by checking
    // that nsize + pos does not wrap around.
    if (nsize + pos > size || nsize + pos < pos) return npos;
    // Don't use std::search, use a Boyer-Moore-like trick by comparing
    // the last characters first
    auto const haystack = data();
    auto const nsize_1 = nsize - 1;
    auto const lastNeedle = needle[nsize_1];

    // Boyer-Moore skip value for the last char in the needle. Zero is
    // not a valid value; skip will be computed the first time it's
    // needed.
    size_type skip = 0;

    const E * i = haystack + pos;
    auto iEnd = haystack + size - nsize_1;

    while (i < iEnd) {
      // Boyer-Moore: match the last element in the needle
      while (i[nsize_1] != lastNeedle) {
        if (++i == iEnd) {
          // not found
          return npos;
        }
      }
      // Here we know that the last char matches
      // Continue in pedestrian mode
      for (size_t j = 0; ; ) {
        assert(j < nsize);
        if (i[j] != needle[j]) {
          // Not found, we can skip
          // Compute the skip value lazily
          if (skip == 0) {
            skip = 1;
            while (skip <= nsize_1 && needle[nsize_1 - skip] != lastNeedle) {
              ++skip;
            }
          }
          i += skip;
          break;
        }
        // Check if done searching
        if (++j == nsize) {
          // Yay
          return i - haystack;
        }
      }
    }
    return npos;
  }

  size_type find(const value_type* s, size_type pos = 0) const {
    return find(s, pos, traits_type::length(s));
  }

  size_type find (value_type c, size_type pos = 0) const {
    return find(&c, pos, 1);
  }

  size_type rfind(const basic_fbstring& str, size_type pos = npos) const {
    return rfind(str.data(), pos, str.length());
  }

  size_type rfind(const value_type* s, size_type pos, size_type n) const {
    if (n > length()) return npos;
    pos = std::min(pos, length() - n);
    if (n == 0) return pos;

    const_iterator i(begin() + pos);
    for (; ; --i) {
      if (traits_type::eq(*i, *s)
          && traits_type::compare(&*i, s, n) == 0) {
        return i - begin();
      }
      if (i == begin()) break;
    }
    return npos;
  }

  size_type rfind(const value_type* s, size_type pos = npos) const {
    return rfind(s, pos, traits_type::length(s));
  }

  size_type rfind(value_type c, size_type pos = npos) const {
    return rfind(&c, pos, 1);
  }

  size_type find_first_of(const basic_fbstring& str, size_type pos = 0) const {
    return find_first_of(str.data(), pos, str.length());
  }

  size_type find_first_of(const value_type* s,
                          size_type pos, size_type n) const {
    if (pos > length() || n == 0) return npos;
    const_iterator i(begin() + pos),
      finish(end());
    for (; i != finish; ++i) {
      if (traits_type::find(s, n, *i) != 0) {
        return i - begin();
      }
    }
    return npos;
  }

  size_type find_first_of(const value_type* s, size_type pos = 0) const {
    return find_first_of(s, pos, traits_type::length(s));
  }

  size_type find_first_of(value_type c, size_type pos = 0) const {
    return find_first_of(&c, pos, 1);
  }

  size_type find_last_of (const basic_fbstring& str,
                          size_type pos = npos) const {
    return find_last_of(str.data(), pos, str.length());
  }

  size_type find_last_of (const value_type* s, size_type pos,
                          size_type n) const {
    if (!empty() && n > 0) {
      pos = std::min(pos, length() - 1);
      const_iterator i(begin() + pos);
      for (;; --i) {
        if (traits_type::find(s, n, *i) != 0) {
          return i - begin();
        }
        if (i == begin()) break;
      }
    }
    return npos;
  }

  size_type find_last_of (const value_type* s,
                          size_type pos = npos) const {
    return find_last_of(s, pos, traits_type::length(s));
  }

  size_type find_last_of (value_type c, size_type pos = npos) const {
    return find_last_of(&c, pos, 1);
  }

  size_type find_first_not_of(const basic_fbstring& str,
                              size_type pos = 0) const {
    return find_first_not_of(str.data(), pos, str.size());
  }

  size_type find_first_not_of(const value_type* s, size_type pos,
                              size_type n) const {
    if (pos < length()) {
      const_iterator
        i(begin() + pos),
        finish(end());
      for (; i != finish; ++i) {
        if (traits_type::find(s, n, *i) == 0) {
          return i - begin();
        }
      }
    }
    return npos;
  }

  size_type find_first_not_of(const value_type* s,
                              size_type pos = 0) const {
    return find_first_not_of(s, pos, traits_type::length(s));
  }

  size_type find_first_not_of(value_type c, size_type pos = 0) const {
    return find_first_not_of(&c, pos, 1);
  }

  size_type find_last_not_of(const basic_fbstring& str,
                             size_type pos = npos) const {
    return find_last_not_of(str.data(), pos, str.length());
  }

  size_type find_last_not_of(const value_type* s, size_type pos,
                             size_type n) const {
    if (!this->empty()) {
      pos = std::min(pos, size() - 1);
      const_iterator i(begin() + pos);
      for (;; --i) {
        if (traits_type::find(s, n, *i) == 0) {
          return i - begin();
        }
        if (i == begin()) break;
      }
    }
    return npos;
  }

  size_type find_last_not_of(const value_type* s,
                             size_type pos = npos) const {
    return find_last_not_of(s, pos, traits_type::length(s));
  }

  size_type find_last_not_of (value_type c, size_type pos = npos) const {
    return find_last_not_of(&c, pos, 1);
  }

  basic_fbstring substr(size_type pos = 0, size_type n = npos) const& {
    enforce(pos <= size(), std::__throw_out_of_range, "");
    return basic_fbstring(data() + pos, std::min(n, size() - pos));
  }

  basic_fbstring substr(size_type pos = 0, size_type n = npos) && {
    enforce(pos <= size(), std::__throw_out_of_range, "");
    erase(0, pos);
    if (n < size()) resize(n);
    return std::move(*this);
  }

  int compare(const basic_fbstring& str) const {
    // FIX due to Goncalo N M de Carvalho July 18, 2005
    return compare(0, size(), str);
  }

  int compare(size_type pos1, size_type n1,
              const basic_fbstring& str) const {
    return compare(pos1, n1, str.data(), str.size());
  }

  int compare(size_type pos1, size_type n1,
              const value_type* s) const {
    return compare(pos1, n1, s, traits_type::length(s));
  }

  int compare(size_type pos1, size_type n1,
              const value_type* s, size_type n2) const {
    enforce(pos1 <= size(), std::__throw_out_of_range, "");
    procrustes(n1, size() - pos1);
    // The line below fixed by Jean-Francois Bastien, 04-23-2007. Thanks!
    const int r = traits_type::compare(pos1 + data(), s, std::min(n1, n2));
    return r != 0 ? r : n1 > n2 ? 1 : n1 < n2 ? -1 : 0;
  }

  int compare(size_type pos1, size_type n1,
              const basic_fbstring& str,
              size_type pos2, size_type n2) const {
    enforce(pos2 <= str.size(), std::__throw_out_of_range, "");
    return compare(pos1, n1, str.data() + pos2,
                   std::min(n2, str.size() - pos2));
  }

  // Code from Jean-Francois Bastien (03/26/2007)
  int compare(const value_type* s) const {
    // Could forward to compare(0, size(), s, traits_type::length(s))
    // but that does two extra checks
    const size_type n1(size()), n2(traits_type::length(s));
    const int r = traits_type::compare(data(), s, std::min(n1, n2));
    return r != 0 ? r : n1 > n2 ? 1 : n1 < n2 ? -1 : 0;
  }

private:
  // Data
  Storage store_;
};
