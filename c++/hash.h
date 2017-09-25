/***

Copyright 2017 Kyle Davison

Permission is hereby granted, free of charge, to any person obtaining a copy of this software and associated documentation files (the "Software"), to deal in the Software without restriction, including without limitation the rights to use, copy, modify, merge, publish, distribute, sublicense, and/or sell copies of the Software, and to permit persons to whom the Software is furnished to do so, subject to the following conditions:

The above copyright notice and this permission notice shall be included in all copies or substantial portions of the Software.

THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY, FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM, OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE SOFTWARE.

***/

#pragma once

#include <cstdint>
#include <numeric>
#include <functional>
#include <memory>

namespace UTILITY
{

template<class T>
std::size_t hash(const T value)
{
  static const std::hash<T> _hash{};
  return _hash(value);
}

}  //::UTILITY::

namespace std
{
template <>
struct hash<std::tuple<std::size_t, std::size_t>> {
  std::size_t operator()(std::tuple<std::size_t, std::size_t> const& e) const {
    static std::array<uint32_t, 4> random = {
      221786277U, //TODO: turn these into actual random numbers
      792949839U,
      443479235U,
      253364473U
    };

    std::array<std::size_t, 4> product = {
      static_cast<uint32_t>(std::get<0>(e) & 0x00000000FFFFFFFF) * random[0],
      static_cast<uint32_t>(std::get<0>(e) & 0xFFFFFFFF00000000) * random[1],
      static_cast<uint32_t>(std::get<1>(e) & 0x00000000FFFFFFFF) * random[2],
      static_cast<uint32_t>(std::get<1>(e) & 0xFFFFFFFF00000000) * random[3]
    };

    return std::accumulate(product.begin(), product.end(), std::size_t(0));
  }
};

template<class T>
struct hash<std::tuple<T*, T*>> {
  std::size_t operator()(std::tuple<T*, T*> const& e) const {
    return std::hash<std::tuple<std::size_t, std::size_t>>{}(std::make_tuple(reinterpret_cast<std::size_t>(std::get<0>(e)), reinterpret_cast<std::size_t>(std::get<1>(e))));
  }
};

template<class T>
struct hash<std::tuple<std::shared_ptr<T>, std::shared_ptr<T>>> {
  std::size_t operator()(std::tuple<std::shared_ptr<T>, std::shared_ptr<T>> const& e) const {
    return std::hash<std::tuple<T*,T*>>{}(std::make_tuple(std::get<0>(e).get(), std::get<1>(e).get()));
  }
};
}
