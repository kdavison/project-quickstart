/***

Copyright 2017 Kyle Davison

Permission is hereby granted, free of charge, to any person obtaining a copy of this software and associated documentation files (the "Software"), to deal in the Software without restriction, including without limitation the rights to use, copy, modify, merge, publish, distribute, sublicense, and/or sell copies of the Software, and to permit persons to whom the Software is furnished to do so, subject to the following conditions:

The above copyright notice and this permission notice shall be included in all copies or substantial portions of the Software.

THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY, FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM, OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE SOFTWARE.

***/

#pragma once

//  this structure is very efficient, should be move-able.
//   put it on stack, heap whatever.
//   probably a decent method to create an object-pool

#include <array>

namespace BUFFER
{
template<typename T, std::size_t N>
class circular
  : public std::array<T,N>
{
  using iterator_t = typename std::array<T,N>::iterator;
public:
  void push(T&& input) {
    if(_current != this->end()) {
      (*_current++) = std::move(input);
    }
  }

  bool pop(T &result) {
    if(_current != this->begin()) {
      result = std::move(*this->begin());
      --_current;
      for(iterator_t i = this->begin()+1, t = this->begin(); i != _current; ++i, ++t) {
        (t) = std::move(i);
      }
      return true;
    }
    return false;
  }

private:
  iterator_t _current=this->begin();
};
}  //::BUFFER::
