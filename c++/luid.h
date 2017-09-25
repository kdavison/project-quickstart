/***

Copyright 2017 Kyle Davison

Permission is hereby granted, free of charge, to any person obtaining a copy of this software and associated documentation files (the "Software"), to deal in the Software without restriction, including without limitation the rights to use, copy, modify, merge, publish, distribute, sublicense, and/or sell copies of the Software, and to permit persons to whom the Software is furnished to do so, subject to the following conditions:

The above copyright notice and this permission notice shall be included in all copies or substantial portions of the Software.

THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY, FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM, OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE SOFTWARE.

***/

#pragma once

#include "hash.h"
#include <chrono>
#include <atomic>
#include <cstdint>

namespace UTILITY
{
//  Locally Unique ID
//    1. unique per program, per type
//    2. NOT unique between types
//    3. DO NOT USE FOR NETWORK STUFF!
//  TODO: is start value sufficiently random?!
template<class T>
struct LUID
{
  typedef std::size_t type_t;

  static std::size_t next() {
    return ++_value;
  }
private:
  static std::atomic<std::size_t> _value;
};

template<class T>
std::atomic<std::size_t> LUID<T>::_value(
  ::UTILITY::hash(std::chrono::high_resolution_clock::now().time_since_epoch().count())
);

}
