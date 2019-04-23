/**
 * MIT License
 *
 * Copyright (c) 2019 Yoram
 *
 * Permission is hereby granted, free of charge, to any person obtaining a copy
 * of this software and associated documentation files (the "Software"), to deal
 * in the Software without restriction, including without limitation the rights
 * to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
 * copies of the Software, and to permit persons to whom the Software is
 * furnished to do so, subject to the following conditions:
 *
 * The above copyright notice and this permission notice shall be included in all
 * copies or substantial portions of the Software.
 *
 * THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
 * IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
 * FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
 * AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
 * LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
 * OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE
 * SOFTWARE.
 */

#ifndef RENDOR_INCLUDE_CG_COMMON_VERTEXARRAY_H_
#define RENDOR_INCLUDE_CG_COMMON_VERTEXARRAY_H_

#include "cg/common/Handlable.h"
#include "cg/common/Bindable.h"
#include <glad/glad.h>

namespace cg {

class VertexArray : public Handlable<unsigned int>, public Bindable {
 public:
  VertexArray() {
    glGenVertexArrays(1, &this->handle);
    verify();
#ifdef CG_GL_DEBUG
    std::cout << "<VertexArray>: Created vertex array with id " << this->handle << "\n";
#endif
  }

  ~VertexArray() {
    glDeleteVertexArrays(1, &this->handle);
  }

  void bind() override {
    glBindVertexArray(this->handle);
  }
  void unbind() override {
    glBindVertexArray(0);
  }
};

}

#endif //RENDOR_INCLUDE_CG_COMMON_VERTEXARRAY_H_
