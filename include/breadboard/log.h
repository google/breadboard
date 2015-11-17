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

#ifndef BREADBOARD_LOG_H_
#define BREADBOARD_LOG_H_

#include <cstdarg>

namespace breadboard {

// In order to perform logging, the library needs to be provided with a logging
// function that fits this type signature.
typedef void (*LogFunc)(const char* fmt, va_list args);

// Register a logging function with the library.
void RegisterLogFunc(LogFunc log_func);

// Call the registered log function with the provided format string. This does
// nothing if no logging function has been registered.
void CallLogFunc(const char* format, ...);

}  // namespace breadboard

#endif  // BREADBOARD_LOG_H_
