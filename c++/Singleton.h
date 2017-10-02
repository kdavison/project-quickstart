//  Basically no one should ever use this

#pragma once

#include <memory>
#include <cassert>

namespace _mpl
{
template<typename T>
class singleton
{
public:
  singleton() = delete;
  singleton(singleton<T> const&) = delete;
  singleton& operator=(singleton<T> const&) = delete;

  template<typename... ARGS>
  static T& instance(ARGS&&... args) {
    _instance = std::make_unique<T>(std::forward<ARGS>(args)...);
    assert(nullptr != _instance);
    return *_instance;
  }

  static T& instance(T&& t) {
    _instance = std::make_unique<T>(std::forward<T>(t));
    assert(nullptr != _instance);
    return *_instance;
  }

  static T& instance() {
    return *_instance;
  }

private:
  static std::unique_ptr<T> _instance;
};

template<typename T>
std::unique_ptr<T> singleton<T>::_instance(nullptr);
}

template<typename T, typename... ARGS>
T& Singleton(ARGS&&... args) {
  return _mpl::singleton<T>::instance(std::forward<ARGS>(args)...);
}

template<typename T>
T& Singleton() {
  return _mpl::singleton<T>::instance();
}