/***

Copyright 2017 Kyle Davison

Permission is hereby granted, free of charge, to any person obtaining a copy of this software and associated documentation files (the "Software"), to deal in the Software without restriction, including without limitation the rights to use, copy, modify, merge, publish, distribute, sublicense, and/or sell copies of the Software, and to permit persons to whom the Software is furnished to do so, subject to the following conditions:

The above copyright notice and this permission notice shall be included in all copies or substantial portions of the Software.

THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY, FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM, OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE SOFTWARE.

***/

#pragma once

#include "rtti_id.h"
#include "signal.h"
#include <unordered_map>
#include <memory>

class signal_manager
{
public:
  typedef std::size_t key_t;
  typedef std::unique_ptr<::UTILITY::iSignal> value_t;
private:
  template<class... ARGS>
  using signal_t = ::UTILITY::Signal<ARGS...>;

  template<class... ARGS>
  using handle_t = typename signal_t<ARGS...>::handle_t;

  template<class... ARGS>
  using rtti_t = ::RTTI<signal_t<ARGS...>>;
public:
  signal_manager() = default;
  ~signal_manager() = default;

  template<class... ARGS>
  std::pair<key_t, bool> connect(handle_t<ARGS...> const& handle)
  {
    auto result = _signals.emplace(rtti_t<ARGS...>::ID, std::move(std::make_unique<signal_t<ARGS...>>()));
    return static_cast<signal_t<ARGS...>*>(result.first->second.get())->connect(handle);
  }

  template<class T, class... ARGS>
  std::pair<key_t, bool> connect(T *pInstance, void(T::*fn)(ARGS...))
  {
    assert(pInstance);
    return connect([=](ARGS... args) {
      (pInstance->*fn)(std::forward<ARGS>(args)...);
    });
  }

  template<class T, class... ARGS>
  std::pair<key_t, bool> connect(T *pInstance, void(T::*fn)(ARGS...) const)
  {
    assert(pInstance);
    return connect([=](ARGS... args) {
      (pInstance->*fn)(std::forward<ARGS>(args)...);
    });
  }

  void disconnect(std::pair<std::size_t, std::size_t> const& id)
  {
    auto result = _signals.find(id.first);
    if(result != _signals.end())
    {
      result->second->disconnect(id.second);
    }
  }

  void clear()
  {
    _signals.clear();
  }

  template<class... ARGS>
  void emit(ARGS... args)
  {
    auto iterator = _signals.find(rtti_t<ARGS...>::ID);
    if(iterator != _signals.end() && iterator->second)
    {
      static_cast<signal_t<ARGS...>*>(iterator->second.get())->emit(std::forward<ARGS>(args)...);
    }
  }

private:
  std::unordered_map<key_t, value_t> _signals;
};
