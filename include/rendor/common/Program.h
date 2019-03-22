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

#ifndef RENDOR_PROGRAM_H
#define RENDOR_PROGRAM_H

#include <vector>
#include <glm/glm.hpp>

#include "rendor/common/Shader.h"

namespace rendor {

class ShaderProgram {
private:
  unsigned int programHandle;
  bool linked = false;

public:
  ShaderProgram();
  ShaderProgram(const ShaderProgram &otherCopy) = delete;
  ShaderProgram(const ShaderProgram &&otherMove) = delete;
  ~ShaderProgram();

  void attachShader(const Shader *shader) const;
  void detachShader(const Shader *shader) const;
  const bool linkProgram();

  void bindAttributeLocation(unsigned int attributeIndex, const std::string &name);
  void bindFragDataLocation(unsigned int colorNumber, const std::string &name);

  int getUniformLocation(const std::string &name);

  void setUniform1f(const std::string &name, float x);
  void setUniform2f(const std::string &name, glm::vec2 vec2);
  void setUniform3f(const std::string &name, glm::vec3 vec3);
  void setUniform4f(const std::string &name, glm::vec4 vec4);

  void setUniform1i(const std::string &name, int x);
  void setUniform2i(const std::string &name, glm::ivec2 vec2);
  void setUniform3i(const std::string &name, glm::ivec3 vec3);
  void setUniform4i(const std::string &name, glm::ivec4 vec4);

  void setUniform1u(const std::string &name, unsigned int x);
  void setUniform2u(const std::string &name, glm::uvec2 vec2);
  void setUniform3u(const std::string &name, glm::uvec3 vec3);
  void setUniform4u(const std::string &name, glm::uvec4 vec4);

  void setUniformMat2f(const std::string &name, glm::mat2 mat2);
  void setUniformMat3f(const std::string &name, glm::mat3 mat3);
  void setUniformMat4f(const std::string &name, glm::mat4 mat4);
  void setUniformMat2x3f(const std::string &name, glm::mat2x3 mat2x3);
  void setUniformMat3x2f(const std::string &name, glm::mat3x2 mat3x2);
  void setUniformMat2x4f(const std::string &name, glm::mat2x4 mat2x4);
  void setUniformMat4x2f(const std::string &name, glm::mat4x2 mat4x2);
  void setUniformMat3x4f(const std::string &name, glm::mat3x4 mat3x4);
  void setUniformMat4x3f(const std::string &name, glm::mat4x3 mat4x3);

  const unsigned int getHandle() const;
};

}

#endif //RENDOR_PROGRAM_H
