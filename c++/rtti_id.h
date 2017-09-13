#pragma once

#include "hash.h"
#include <typeinfo>
#include <typeindex>

template<class T>
class RTTI
{
public:
  static const std::size_t ID;
  static const std::string NAME;
};

template<class T>
const std::size_t RTTI<T>::ID = ::UTILITY::hash(std::type_index(typeid(T)));

template<class T>
const std::string RTTI<T>::NAME = std::string(typeid(T).name());