/***

Copyright 2017 Kyle Davison

Permission is hereby granted, free of charge, to any person obtaining a copy of this software and associated documentation files (the "Software"), to deal in the Software without restriction, including without limitation the rights to use, copy, modify, merge, publish, distribute, sublicense, and/or sell copies of the Software, and to permit persons to whom the Software is furnished to do so, subject to the following conditions:

The above copyright notice and this permission notice shall be included in all copies or substantial portions of the Software.

THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY, FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM, OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE SOFTWARE.

***/

#pragma once

//  Simple Ring (circular) buffer implementaton
//    adds some wrapping functions to a std::array<X,Y>.
//    uses two queues (read/write) to track writable/readable space
//  Note:
//    probably not the most efficient because of the queues.
//    The good news it will only take up sizeof(X)*Y + sizeof(std::array<X,Y>::iterator)*Y space.
//    There may be more efficient ways to do this.
//    honestly, I chose this implemtnation because its really really simple.

#include <array>
#include <queue>
#include <algorithm>

namespace BUFFER
{
template<typename T, std::size_t N>
class circular
  : public std::array<T, N>
{
public:
  circular() {
    _init();
  }

  template<typename... ARGS>
  circular(ARGS&&... args)
    : std::array<T,N>{{std::forward<ARGS>(args)...}}
  {
    _init();
  }

  bool push(T&& input) {
    if(!_write.empty()) {
      (*_write.front()) = std::move(input);
      _read.push(_write.front());
      _write.pop();
      return true;
    }
    return false;
  }

  bool pop(T &output) {
    if(!_read.empty()) {
      output = std::move(*_read.front());
      _write.push(_read.front());
      _read.pop();
      return true;
    }
    return false;
  }


private:
  using array_t = typename std::array<T,N>;
  using iterator_t = typename array_t::iterator;

  inline void _init() {
    for(auto iterator = this->begin(); iterator != this->end(); ++iterator) {
      _write.push(iterator);
    }
  }

  std::queue<iterator_t> _write, _read;
};
}  //::BUFFER::
