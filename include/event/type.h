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

#ifndef FPL_EVENT_TYPE_H_
#define FPL_EVENT_TYPE_H_

#include <cstddef>
#include <cstdint>

namespace fpl {
namespace event {

typedef void (*PlacementNewFunc)(uint8_t*);
typedef void (*OperatorDeleteFunc)(uint8_t*);

// Type specifies the size and alignment of a given type, as well as how to
// construct them using placement new in a buffer. By default they use
// DefaultPlacementNew, which simply constucts an object at the given address.
// If the default constructor can not be used, custom a PlacementNewFunc may be
// supplied.
struct Type {
  Type() {}

  Type(const char* name_, size_t size_, size_t alignment_,
       PlacementNewFunc placement_new_func_,
       OperatorDeleteFunc operator_delete_func_)
      : name(name_),
        size(size_),
        alignment(alignment_),
        placement_new_func(placement_new_func_),
        operator_delete_func(operator_delete_func_) {}

  const char* name;
  size_t size;
  size_t alignment;
  PlacementNewFunc placement_new_func;
  OperatorDeleteFunc operator_delete_func;
};

}  // event
}  // fpl

#endif  // FPL_EVENT_TYPE_H_
