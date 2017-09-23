#pragma once

#include <chrono>

namespace Application
{
class iContext
{
public:
  using clock = std::chrono::high_resolution_clock;
  using time_point_t = std::chrono::time_point<clock>;
  using nanoseconds_t = std::chrono::nanoseconds;
public:
  iContext() = default;
  virtual ~iContext() = default;
  virtual bool is_running() const = 0;
  virtual void begin(time_point_t const& global_time, nanoseconds_t const& frame_time) = 0;
  virtual void simulate(time_point_t const& global_time, nanoseconds_t const& timestep) = 0;
  virtual void pre_render() = 0;
  virtual void render() = 0;
  virtual void end() = 0;
};
}  //::Application::
