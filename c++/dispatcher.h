/***

Copyright 2017 Kyle Davison

Permission is hereby granted, free of charge, to any person obtaining a copy of this software and associated documentation files (the "Software"), to deal in the Software without restriction, including without limitation the rights to use, copy, modify, merge, publish, distribute, sublicense, and/or sell copies of the Software, and to permit persons to whom the Software is furnished to do so, subject to the following conditions:

The above copyright notice and this permission notice shall be included in all copies or substantial portions of the Software.

THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY, FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM, OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE SOFTWARE.

***/

#pragma once

#include "signal.h"
#include <vector>

class dispatcher final
{
public:
  using key_t = std::size_t;
public:
  dispatcher(const std::size_t queue = 1024){_events.reserve(queue);}
  ~dispatcher() = default;

  template<class... ARGS>
  std::pair<key_t, bool> connect(typename ::UTILITY::Signal<ARGS...>::handle_t const& handle)
  {
    using signal_t = ::UTILITY::Signal<ARGS...>;
    auto result = _signals.emplace(::RTTI<signal_t>::ID, std::make_unique<signal_t>());
    return static_cast<signal_t*>(result.first->second.get())->connect(handle);
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

  void clear() { _signals.clear(); _events.clear(); }

  template<class... ARGS>
  void enqueue(ARGS... args)
  {
    using signal_t = ::UTILITY::Signal<ARGS...>;
    auto result = _signals.emplace(::RTTI<signal_t>::ID, std::make_unique<signal_t>());
    _events.push_back([=]() {
      static_cast<signal_t*>(result.first->second.get())->emit(args...);
    });
  }

  void dispatch()
  {
    decltype(_events) events = std::move(_events);
    _events.reserve(events.size());
    for(const auto& event : events)
    {
      event();
    }
  }

  std::pair<std::size_t, std::size_t> count() const {
    return std::make_pair(_events.size(), _signals.size());
  }

private:
  using event_t = std::function<void()>;
  using value_t = std::unique_ptr<::UTILITY::iSignal>;

  std::vector<event_t> _events;
  std::unordered_map<key_t, value_t> _signals;
};
