// Copyright 2015 Google Inc. All rights reserved.
//
// Licensed under the Apache License, Version 2.0 (the "License");
// you may not use this file except in compliance with the License.
// You may obtain a copy of the License at
//
//     http://www.apache.org/licenses/LICENSE-2.0
//
// Unless required by applicable law or agreed to in writing, software
// distributed under the License is distributed on an "AS IS" BASIS,
// WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
// See the License for the specific language governing permissions and
// limitations under the License.

#ifndef FPL_EVENT_MEMORY_BUFFER_H_
#define FPL_EVENT_MEMORY_BUFFER_H_

#include <cassert>
#include <cstddef>
#include <cstdint>
#include <vector>

namespace fpl {
namespace event {

// MemoryBuffer is a simple wrapper around a vector of bytes that makes it easy
// to convert offsets into concrete types. The buffer size may only be set once.
// Once the buffer is initialized, functions are provided to get at the memory
// at given offsets and cast it to the desired type. This is not guarenteed to
// work if bad offsets are supplied, so it is up to the caller to make sure they
// are keeping track of offset values. This class only provides convenience, not
// safety.
class MemoryBuffer {
 public:
  MemoryBuffer() : buffer_() {}

  // Resizes the buffer to the desired size. For the sake of simplicity, this is
  // only allowed to happen once. There are no use cases currently where
  // resizing would be allowed, so to prevent misuse any attempt to resize it
  // again will assert.
  void Initialize(size_t size) {
    assert(buffer_.size() == 0);
    buffer_.resize(size);
  }

  // Returns a raw pointer to the desired offset in the buffer. This is mostly
  // useful for getting a pointer to use placement new on.
  uint8_t* GetObjectPtr(ptrdiff_t offset) {
    assert(buffer_.size() > 0 &&
           offset < static_cast<ptrdiff_t>(buffer_.size()));
    return buffer_.data() + offset;
  }
  const uint8_t* GetObjectPtr(ptrdiff_t offset) const {
    assert(buffer_.size() > 0 &&
           offset < static_cast<ptrdiff_t>(buffer_.size()));
    return buffer_.data() + offset;
  }

  // Returns a pointer to an object located at the given offset. This does no
  // static or run time type checking, so the caller must be sure the memory
  // they're accessing is the type they expect.
  template <typename T>
  T* GetObject(ptrdiff_t offset) {
    return reinterpret_cast<T*>(GetObjectPtr(offset));
  }
  template <typename T>
  const T* GetObject(ptrdiff_t offset) const {
    return reinterpret_cast<const T*>(GetObjectPtr(offset));
  }

 private:
  std::vector<uint8_t> buffer_;
};

}  // event
}  // fpl

#endif  // FPL_EVENT_MEMORY_BUFFER_H_
