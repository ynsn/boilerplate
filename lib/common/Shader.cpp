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

#include <glad/glad.h>
#include <vector>
#include <string>
#include <iostream>
#include <sstream>
#include <fstream>

#include "rendor/common/Shader.h"

namespace rendor {

rendor::Shader::Shader(const ShaderType &type) : shaderType(type) {
  this->shaderHandle = glCreateShader(static_cast<GLenum>(this->shaderType));
  if (this->shaderHandle == 0) {
    fprintf(stderr, "Error: could not create shader, id = 0...\n");
  }
}

Shader::~Shader() {
  glDeleteShader(this->shaderHandle);
}

void Shader::setShaderSource(const std::string &shaderSource) const {
  const char *source = shaderSource.c_str();
  glShaderSource(this->shaderHandle, 1, &source, nullptr);
}

bool Shader::compileShader() {
  glCompileShader(this->shaderHandle);

  int status = 0;
  glGetShaderiv(this->shaderHandle, GL_COMPILE_STATUS, &status);
  if (status == GL_FALSE) {
    int length = 0;
    glGetShaderiv(this->shaderHandle, GL_INFO_LOG_LENGTH, &length);

    std::vector<char> log(length);
    glGetShaderInfoLog(this->shaderHandle, length, &length, &log[0]);
    std::string logString(log.begin(), log.end());
    fprintf(stderr,
            "Shader compilation log (id = %u, type = %u): %s",
            this->shaderHandle,
            this->shaderType,
            logString.c_str());
    this->compiled = false;
  } else {
    this->compiled = true;
  }

  return this->compiled;
}

const ShaderType &Shader::getShaderType() const {
  return this->shaderType;
}

const std::string Shader::getShaderSource() const {
  std::vector<char> sourceBuf(getShaderSourceLength());

  glGetShaderSource(this->shaderHandle, sourceBuf.size(), NULL, &sourceBuf[0]);
  std::string source(sourceBuf.begin(), sourceBuf.end());
  return source;
}

const int Shader::getShaderSourceLength() const {
  int length;
  glGetShaderiv(this->shaderHandle, GL_SHADER_SOURCE_LENGTH, &length);
  return length;
}

const bool Shader::isDeleted() const {
  int deleted = false;
  glGetShaderiv(this->shaderHandle, GL_DELETE_STATUS, &deleted);
  return static_cast<const bool>(deleted);
}

bool Shader::isCompiled() const {
  return this->compiled;
}

const unsigned int Shader::getHandle() const {
  return this->shaderHandle;
}

Shader *Shader::LoadFromSourceFile(const ShaderType &type1, const std::string &path) {
  std::ifstream file;
  file.open(path);

  if (!file.is_open()) {
    fprintf(stderr, "Error: could not read file '%s'\n", path.c_str());
    return nullptr;
  }

  std::stringstream source;
  source << file.rdbuf();

  auto *shader = new Shader(type1);
  shader->setShaderSource(source.str());

  return shader;
}

}