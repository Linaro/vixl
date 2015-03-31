// Copyright 2014, ARM Limited
// All rights reserved.
//
// Redistribution and use in source and binary forms, with or without
// modification, are permitted provided that the following conditions are met:
//
//   * Redistributions of source code must retain the above copyright notice,
//     this list of conditions and the following disclaimer.
//   * Redistributions in binary form must reproduce the above copyright notice,
//     this list of conditions and the following disclaimer in the documentation
//     and/or other materials provided with the distribution.
//   * Neither the name of ARM Limited nor the names of its contributors may be
//     used to endorse or promote products derived from this software without
//     specific prior written permission.
//
// THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS CONTRIBUTORS "AS IS" AND
// ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE IMPLIED
// WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE ARE
// DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT OWNER OR CONTRIBUTORS BE LIABLE
// FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL
// DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR
// SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER
// CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY,
// OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE
// OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.

#include "test-runner.h"

#include "vixl/invalset.h"

namespace vixl {

// This file contains tests for the `InvalSet` and `InvalSetIterator` classes.

#define TEST(name)  TEST_(INVALSET_##name)

typedef ptrdiff_t KeyType;
typedef ptrdiff_t ValType;

// We test with an object for which the key and the value are distinct.
class Obj {
 public:
  Obj() {}
  Obj(KeyType key, ValType val) : key_(key), val_(val) {}
  KeyType key_;
  ValType val_;

  bool operator==(const Obj& other) const {
    return (key_ == other.key_) && (val_ == other.val_);
  }
  bool operator<(const Obj& other) const {
    return (key_ < other.key_) ||
        ((key_ == other.key_) && (val_ < other.val_));
  }
  bool operator<=(const Obj& other) const {
    return (key_ <= other.key_) ||
        ((key_ == other.key_) && (val_ <= other.val_));
  }
  bool operator>(const Obj& other) const {
    return (key_ > other.key_) ||
        ((key_ == other.key_) && (val_ > other.val_));
  }
};

static const unsigned kNPreallocatedElements = 8;
static const KeyType kInvalidKey = PTRDIFF_MAX;
static const size_t kReclaimFrom = 1000;
static const unsigned kReclaimFactor = 10;

typedef InvalSet<Obj,
                 kNPreallocatedElements,
                 KeyType,
                 kInvalidKey,
                 kReclaimFrom,
                 kReclaimFactor> TestSet;

template<>
inline KeyType InvalSet<Obj,
                        kNPreallocatedElements,
                        KeyType,
                        kInvalidKey,
                        kReclaimFrom,
                        kReclaimFactor>::Key(const Obj& obj) {
  return obj.key_;
}
template<>
inline void InvalSet<Obj,
                     kNPreallocatedElements,
                     KeyType,
                     kInvalidKey,
                     kReclaimFrom,
                     kReclaimFactor>::SetKey(Obj* obj, KeyType key) {
  obj->key_ = key;
}


TEST(basic_test) {
  TestSet set;
  VIXL_CHECK(set.empty() && (set.size() == 0));

  for (unsigned i = 0; i < kNPreallocatedElements; i++) {
    set.insert(Obj(i, i));
  }
  VIXL_CHECK(set.size() == kNPreallocatedElements);

  set.insert(Obj(-123, 456));
  set.insert(Obj(2718, 2871828));
  VIXL_CHECK(set.size() == kNPreallocatedElements + 2);
  VIXL_CHECK(set.min_element() == Obj(-123, 456));

  set.erase(Obj(-123, 456));
  VIXL_CHECK(set.min_element_key() == 0);

  set.clear();
  VIXL_CHECK(set.empty() && (set.size() == 0));
}


TEST(valid_element) {
  VIXL_CHECK(TestSet::IsValid(Obj(0, 0)));
  VIXL_CHECK(TestSet::IsValid(Obj(-1, 0)));
  VIXL_CHECK(TestSet::IsValid(Obj(kInvalidKey - 1, 0)));
  VIXL_CHECK(!TestSet::IsValid(Obj(kInvalidKey, 0)));
}


TEST(insert) {
  TestSet set;
  VIXL_CHECK(set.empty() && (set.size() == 0));

  for (unsigned i = 0; i < kNPreallocatedElements; i++) {
    set.insert(Obj(i, i));
  }
  VIXL_CHECK(set.size() == kNPreallocatedElements);
  set.insert(Obj(-123, 1));
  VIXL_CHECK(set.size() == kNPreallocatedElements + 1);
  set.insert(Obj(-123, 2));
  set.insert(Obj(-123, 3));
  VIXL_CHECK(set.size() == kNPreallocatedElements + 3);

  set.clear();
  VIXL_CHECK(set.empty() && (set.size() == 0));
}


TEST(erase) {
  TestSet set;
  VIXL_CHECK(set.empty() && (set.size() == 0));

  // Test with only preallocated elements in the set.
  VIXL_STATIC_ASSERT(kNPreallocatedElements >= 2);
  set.insert(Obj(2718, 0));
  set.erase(Obj(2718, 0));
  VIXL_CHECK(set.empty() && (set.size() == 0));
  set.insert(Obj(2718, 0));
  VIXL_CHECK(set.size() == 1);
  set.insert(Obj(2718, 1));
  VIXL_CHECK(set.size() == 2);
  set.erase(Obj(2718, 0));
  VIXL_CHECK(set.size() == 1);

  // Test with more elements.
  for (unsigned i = 0; i < 100 * kNPreallocatedElements; i++) {
    set.insert(Obj(i * i, i % 30));
    set.insert(Obj(i, -1));
  }
  size_t max_size = set.size();
  set.erase(Obj(100, -1));
  VIXL_CHECK(set.size() == max_size - 1);
  for (size_t i = 2; i <= max_size; i++) {
    set.erase(set.min_element());
    VIXL_CHECK(set.size() == max_size - i);
  }

  VIXL_CHECK(set.empty() && (set.size() == 0));
}


TEST(min) {
  TestSet set;
  VIXL_CHECK(set.empty() && (set.size() == 0));

  // Test with only preallocated elements in the set.
  VIXL_STATIC_ASSERT(kNPreallocatedElements >= 4);
  set.insert(Obj(-1, -1));
  set.insert(Obj(-1, 0));
  set.insert(Obj(0, 0));
  set.insert(Obj(1, 0));
  VIXL_CHECK(set.min_element() == Obj(-1, -1));
  VIXL_CHECK(set.min_element_key() == -1);
  VIXL_CHECK(set.min_element().key_ == set.min_element_key());

  // Test with more elements.
  set.clear();
  int max_index = 100 * kNPreallocatedElements;
  for (int i = 0; i <= max_index; i++) {
    // Insert elements out of order.
    int sign = ((i % 2) == 0) ? -1 : 1;
    set.insert(Obj(sign * i, i));
  }
  VIXL_CHECK(set.min_element() == Obj(-max_index, max_index));
  VIXL_CHECK(set.min_element().key_ == set.min_element_key());

  set.erase(Obj(0, 0));
  VIXL_CHECK(set.min_element() == Obj(-max_index, max_index));
  set.erase(set.min_element());
  VIXL_CHECK(set.min_element() == Obj(-(max_index - 2), max_index - 2));

  set.clear();
  VIXL_CHECK(set.empty() && (set.size() == 0));
}


TEST(iterator) {
  TestSet set;
  VIXL_CHECK(set.empty() && (set.size() == 0));

  // Test with only preallocated elements in the set.
  for (unsigned i = 0; i < kNPreallocatedElements; i++) {
    set.insert(Obj(i, i));
  }

  size_t size = 0;
  for (InvalSetIterator<TestSet> it(&set); !it.Done(); it.Advance()) {
    size++;
  }
  VIXL_CHECK(size == set.size());

  // Test with more elements.
  for (unsigned i = kNPreallocatedElements;
       i < 4 * kNPreallocatedElements;
       i++) {
    set.insert(Obj(i, i));
  }

  size = 0;
  for (InvalSetIterator<TestSet> it(&set); !it.Done(); it.Advance()) {
    size++;
  }
  VIXL_CHECK(size == set.size());

  // Test after an element has been deleted.
  size = 0;
  set.erase(Obj(0, 0));
  for (InvalSetIterator<TestSet> it(&set); !it.Done(); it.Advance()) {
    size++;
  }
  VIXL_CHECK(size == set.size());
}

}  // namespace vixl
