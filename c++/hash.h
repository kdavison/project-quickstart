#pragma once

namespace UTILITY
{

template<class T>
std::size_t hash(const T value)
{
  static const std::hash<T> _hash{};
  return _hash(value);
}

}  //::UTILITY::