#pragma once

// COPYRIGHT KYLE DAVISON <kdavison at gmail dot com> 2017

// There is a 'one-definition rule', which means the linker
//   can only have 1 definition of a function due to static variables
//   and such.
// This nifty thing takes advantage of that and creates a function
//   which returns a cast'ed pointer to itself based on type.
// aka lightweight type-id!
// sadly the pointer isn't fixed between runs
//
// struct is an inheritable member for classes and such

namespace compile {
template<typename T>
static constexpr std::size_t id() {
  return reinterpret_cast<std::size_t>(&id<T>);
}

template<typename T>
struct identifier {
  static constexpr std::size_t ID() {
    return id<T>();
  }
};
} //::compile
