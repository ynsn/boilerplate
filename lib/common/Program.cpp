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
#include <glm/gtc/type_ptr.hpp>

#include "rendor/common/Program.h"

namespace rendor {

ShaderProgram::ShaderProgram() {
  this->programHandle = glCreateProgram();
  if (this->programHandle == 0) {
    fprintf(stderr, "Error: could not create program, id = 0...\n");
  }
}

ShaderProgram::~ShaderProgram() {
  glDeleteProgram(this->programHandle);
}

void ShaderProgram::attachShader(const Shader *shader) const {
  glAttachShader(this->programHandle, shader->getHandle());
}

void ShaderProgram::detachShader(const Shader *shader) const {
  glDetachShader(this->programHandle, shader->getHandle());
}

const bool ShaderProgram::linkProgram() {
  glLinkProgram(this->programHandle);

  int status = 0;
  glGetProgramiv(this->programHandle, GL_LINK_STATUS, &status);
  if (status == GL_FALSE) {
    int length = 0;
    glGetProgramiv(this->programHandle, GL_INFO_LOG_LENGTH, &length);

    std::vector<char> log(length);
    glGetProgramInfoLog(this->programHandle, length, &length, &log[0]);
    std::string logString(log.begin(), log.end());
    fprintf(stderr,
            "Program linking log (id = %u): %s",
            this->programHandle,
            logString.c_str());
    this->linked = false;
  } else {
    this->linked = true;
  }

  return this->linked;
}

void ShaderProgram::bindAttributeLocation(unsigned int attributeIndex, const std::string &name) {
  glBindAttribLocation(this->programHandle, attributeIndex, name.c_str());
}

void ShaderProgram::bindFragDataLocation(unsigned int colorNumber, const std::string &name) {
  glBindFragDataLocation(this->programHandle, colorNumber, name.c_str());
}

int ShaderProgram::getUniformLocation(const std::string &name) {
  return glGetUniformLocation(this->programHandle, name.c_str());
}

void ShaderProgram::setUniform1f(const std::string &name, float x) {
  glUniform1f(this->getUniformLocation(name), x);
}

void ShaderProgram::setUniform2f(const std::string &name, glm::vec2 vec2) {
  glUniform2fv(this->getUniformLocation(name), 1, glm::value_ptr(vec2));
}

void ShaderProgram::setUniform3f(const std::string &name, glm::vec3 vec3) {
  glUniform3fv(this->getUniformLocation(name), 1, glm::value_ptr(vec3));
}

void ShaderProgram::setUniform4f(const std::string &name, glm::vec4 vec4) {
  glUniform4fv(this->getUniformLocation(name), 1, glm::value_ptr(vec4));
}

void ShaderProgram::setUniform1i(const std::string &name, int x) {
  glUniform1i(this->getUniformLocation(name), x);
}

void ShaderProgram::setUniform2i(const std::string &name, glm::ivec2 vec2) {
  glUniform2iv(this->getUniformLocation(name), 1, glm::value_ptr(vec2));
}

void ShaderProgram::setUniform3i(const std::string &name, glm::ivec3 vec3) {
  glUniform3iv(this->getUniformLocation(name), 1, glm::value_ptr(vec3));
}

void ShaderProgram::setUniform4i(const std::string &name, glm::ivec4 vec4) {
  glUniform4iv(this->getUniformLocation(name), 1, glm::value_ptr(vec4));
}

void ShaderProgram::setUniform1u(const std::string &name, unsigned int x) {
  glUniform1ui(this->getUniformLocation(name), x);
}

void ShaderProgram::setUniform2u(const std::string &name, glm::uvec2 vec2) {
  glUniform2uiv(this->getUniformLocation(name), 1, glm::value_ptr(vec2));
}

void ShaderProgram::setUniform3u(const std::string &name, glm::uvec3 vec3) {
  glUniform3uiv(this->getUniformLocation(name), 1, glm::value_ptr(vec3));
}

void ShaderProgram::setUniform4u(const std::string &name, glm::uvec4 vec4) {
  glUniform4uiv(this->getUniformLocation(name), 1, glm::value_ptr(vec4));
}

void ShaderProgram::setUniformMat2f(const std::string &name, glm::mat2 mat2) {
  glUniformMatrix2fv(this->getUniformLocation(name), 1, GL_FALSE, glm::value_ptr(mat2));
}

void ShaderProgram::setUniformMat3f(const std::string &name, glm::mat3 mat3) {
  glUniformMatrix3fv(this->getUniformLocation(name), 1, GL_FALSE, glm::value_ptr(mat3));
}

void ShaderProgram::setUniformMat4f(const std::string &name, glm::mat4 mat4) {
  glUniformMatrix4fv(this->getUniformLocation(name), 1, GL_FALSE, glm::value_ptr(mat4));
}

void ShaderProgram::setUniformMat2x3f(const std::string &name, glm::mat2x3 mat2x3) {
  glUniformMatrix2x3fv(this->getUniformLocation(name), 1, GL_FALSE, glm::value_ptr(mat2x3));
}

void ShaderProgram::setUniformMat3x2f(const std::string &name, glm::mat3x2 mat3x2) {
  glUniformMatrix3x2fv(this->getUniformLocation(name), 1, GL_FALSE, glm::value_ptr(mat3x2));
}

void ShaderProgram::setUniformMat2x4f(const std::string &name, glm::mat2x4 mat2x4) {
  glUniformMatrix2x4fv(this->getUniformLocation(name), 1, GL_FALSE, glm::value_ptr(mat2x4));
}

void ShaderProgram::setUniformMat4x2f(const std::string &name, glm::mat4x2 mat4x2) {
  glUniformMatrix4x2fv(this->getUniformLocation(name), 1, GL_FALSE, glm::value_ptr(mat4x2));
}

void ShaderProgram::setUniformMat3x4f(const std::string &name, glm::mat3x4 mat3x4) {
  glUniformMatrix3x4fv(this->getUniformLocation(name), 1, GL_FALSE, glm::value_ptr(mat3x4));
}

void ShaderProgram::setUniformMat4x3f(const std::string &name, glm::mat4x3 mat4x3) {
  glUniformMatrix4x3fv(this->getUniformLocation(name), 1, GL_FALSE, glm::value_ptr(mat4x3));
}

const unsigned int ShaderProgram::getHandle() const {
  return this->programHandle;
}

}
