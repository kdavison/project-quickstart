#pragma once

#include "luid.h"
#include <functional>
#include <mutex>
#include <unordered_map>
#include <vector>
#include <algorithm>
#include <cassert>

// Simple signal class
//   Modified from: http://simmesimme.github.io/tutorials/2015/09/20/signal-slot
//   Author: Simon Schneegans

namespace UTILITY
{
class iSignal
{
public:
  iSignal() = default;
  virtual ~iSignal() = default;
  virtual void disconnect(const std::size_t key) = 0;
  virtual void disconnect_all() = 0;
};

template<class... ARGS>
class Signal final
  : public iSignal
{
public:
  typedef typename ::UTILITY::LUID<Signal<ARGS...>>::type_t key_t;
  typedef std::function<void(ARGS...)> handle_t;
public:
  Signal() : _slots() { }

  Signal(Signal const& other)
    : _slots(other._slots)
  {
  }

  ~Signal() = default;

  template<class T>
  std::pair<key_t, bool> connect(T *pInstance, void (T::*fn)(ARGS...))
  {
    assert(pInstance);
    return connect([=](ARGS... args) {
      (pInstance->*fn)(std::forward<ARGS>(args)...);
    });
  }

  template<class T>
  std::pair<key_t, bool> connect(T *pInstance, void(T::*fn)(ARGS...) const)
  {
    assert(pInstance);
    return connect([=](ARGS... args) {
      (pInstance->*fn)(std::forward<ARGS>(args)...);
    });
  }

  std::pair<key_t, bool> connect(handle_t const& handle) const
  {
    //std::lock_guard<decltype(_mutex)> lock(_mutex);
    auto result = _slots.emplace(::UTILITY::LUID<Signal<ARGS...>>::next(), handle);
    return std::make_pair(result.first->first, result.second);
  }

  void disconnect(const key_t id)
  {
    //std::lock_guard<decltype(_mutex)> lock(_mutex);
    _slots.erase(id);
  }

  void disconnect_all()
  {
    //std::lock_guard<decltype(_mutex)> lock(_mutex);
    _slots.clear();
  }

  void emit(ARGS... args)
  {
    std::vector<handle_t> handles;
    handles.reserve(_slots.size());
    {
      //std::lock_guard<decltype(_mutex)> lock(_mutex);
      std::transform(_slots.begin(), _slots.end(), std::back_inserter(handles), [](const auto& entry)
      {
        return entry.second;
      });
    }
    for(auto& entry : handles)
    {
      if(entry)
      {
        entry(std::forward<ARGS>(args)...);
      }
    }
  }
private:
  //mutable std::mutex _mutex;
//  TODO:: replace with libcuckoo
  mutable std::unordered_map<key_t, handle_t> _slots;
};
}
