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

#ifndef BREADBOARD_TYPE_H_
#define BREADBOARD_TYPE_H_

#include <cstddef>
#include <cstdint>

/// @file breadboard/type.h
///
/// @brief Metadata about types that are used as input and output edge
/// parameters on nodes.

namespace breadboard {

/// @typedef PlacementNewFunc
///
/// @brief A typedef for a function pointer that allocates a type at the given
/// memory address.
typedef void (*PlacementNewFunc)(uint8_t*);

/// @typedef OperatorDeleteFunc
///
/// @brief A typedef for a function pointer that deletes an object at the given
/// memory address.
typedef void (*OperatorDeleteFunc)(uint8_t*);

/// @struct Type
///
/// @brief Metadata about types that are used as input and output edge
/// parameters on nodes.
///
/// Type specifies the size and alignment of a given type, as well as how
/// to construct them using placement new in a buffer. By default they use
/// DefaultPlacementNew, which simply constucts an object at the given address.
/// If the default constructor can not be used, custom a PlacementNewFunc may be
/// supplied.
///
/// @note This class should not be instantiated directly. Instead, Types should
/// be registered through the TypeRegistry.
struct Type {
  Type() {}

  /// @brief Construct a Type object with the given metadata.
  ///
  /// @param[in] name_ The name of the type.
  ///
  /// @param[in] size_ The size of the type in bytes.
  ///
  /// @param[in] alignment_ The byte alignment of the type.
  ///
  /// @param[in] placement_new_func_ The function used to construct an instance
  /// of the type.
  ///
  /// @param[in] operator_delete_func_ The function used to delete and instance
  /// of the Type.
  Type(const char* name_, size_t size_, size_t alignment_,
       PlacementNewFunc placement_new_func_,
       OperatorDeleteFunc operator_delete_func_)
      : name(name_),
        size(size_),
        alignment(alignment_),
        placement_new_func(placement_new_func_),
        operator_delete_func(operator_delete_func_) {}

  /// @brief The name of the type.
  const char* name;

  /// @brief The size of the type in bytes.
  size_t size;

  /// @brief The byte alignment of the type.
  size_t alignment;

  /// @brief The function used to construct an instance of the type.
  PlacementNewFunc placement_new_func;

  /// @brief The function used to delete and instance of the Type.
  OperatorDeleteFunc operator_delete_func;
};

}  // namespace breadboard

#endif  // BREADBOARD_TYPE_H_
