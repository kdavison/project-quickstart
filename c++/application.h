/***

Copyright 2017 Kyle Davison

Permission is hereby granted, free of charge, to any person obtaining a copy of this software and associated documentation files (the "Software"), to deal in the Software without restriction, including without limitation the rights to use, copy, modify, merge, publish, distribute, sublicense, and/or sell copies of the Software, and to permit persons to whom the Software is furnished to do so, subject to the following conditions:

The above copyright notice and this permission notice shall be included in all copies or substantial portions of the Software.

THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY, FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM, OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE SOFTWARE.

***/

#pragma once

#include <chrono>
#include <array>
#include <atomic>
#include <memory>
#include <iostream>

#include "iContext.h"

using namespace std::chrono_literals;

namespace Application
{
class FixedTimestep final
{
private:
  enum eState : uint8_t {
    CURRENT = 0,
    PREVIOUS,
    COUNT
  };
public:
  using clock = std::chrono::steady_clock;
  using time_point_t = std::chrono::time_point<clock>;
  using nanoseconds_t = std::chrono::nanoseconds;

public:
  FixedTimestep(const nanoseconds_t timestep = 16ms, time_point_t const& start = clock::now())
    : _global(start)
    , _accumulator(0)
    , _timestep(timestep)
    , _time{_global, _global - timestep}
  {
  }

  template<class T, class... ARGS>
  int32_t run(ARGS&&... args) {
    static_assert(std::is_base_of<iContext, T>::value, "Application Context must derive from iContext!");
    T context(std::forward<ARGS>(args)...);
    __run(context);
  }

  template<class T>
  int32_t run() {
    static_assert(std::is_base_of<iContext, T>::value, "Application Context must derive from iContext!");
    T context;
    __run(context);
  }

private:
  int32_t __run(iContext& _context) {
    try {
      while(_context.is_running()) {
        _time = {clock::now(), _time[eState::CURRENT]};
        _context.begin(_global, _time[eState::CURRENT] - _time[eState::PREVIOUS]);
        for(
          _accumulator += _time[eState::CURRENT] - _time[eState::PREVIOUS];
          _accumulator >= _timestep;
          _accumulator -= _timestep, _global += _timestep
        ) {
          _context.simulate(_global, _timestep);
        }
        _context.pre_render();
        _context.render();
        _context.end();
      }
    } catch(std::exception& ex) {
      std::cerr << "exception: " << ex.what() << std::endl;
      return -1;
    }
    return 0;
  }

  time_point_t _global;
  nanoseconds_t _accumulator, _timestep;
  std::array<time_point_t, eState::COUNT> _time;
};
}  //::Application::


