/***

Copyright 2017 Kyle Davison

Permission is hereby granted, free of charge, to any person obtaining a copy of this software and associated documentation files (the "Software"), to deal in the Software without restriction, including without limitation the rights to use, copy, modify, merge, publish, distribute, sublicense, and/or sell copies of the Software, and to permit persons to whom the Software is furnished to do so, subject to the following conditions:

The above copyright notice and this permission notice shall be included in all copies or substantial portions of the Software.

THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY, FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM, OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE SOFTWARE.

***/

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
