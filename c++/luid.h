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
