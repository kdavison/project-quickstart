#pragma once

#include <functional>
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
template<typename T, typename X = std::size_t>
struct LUID
{
  typedef std::enable_if_t<std::is_integral<X>::value, X> type_t;

  static type_t next() {
    return ++_value;
  }
private:
  static std::atomic<X> _value;
};

template<class T, typename X>
std::atomic<X> LUID<T,X>::_value(
  std::hash<X>{}(
    static_cast<X>(
      std::chrono::duration_cast<std::chrono::nanoseconds>(
        std::chrono::steady_clock::now().time_since_epoch()
      )
    )
  )
);
}
