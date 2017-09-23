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
  using clock = std::chrono::high_resolution_clock;
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
  int32_t run(ARGS... args) {
    static_assert(std::is_base_of<iContext, T>::value, "Application Context must derive from iContext!");
    try {
      T _context(std::forward<ARGS>(args)...);
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

private:
  time_point_t _global;
  nanoseconds_t _accumulator, _timestep;
  std::array<time_point_t, eState::COUNT> _time;
};
}  //::Application::

