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

#ifndef RENDOR_SHADER_H
#define RENDOR_SHADER_H

#include <string>

namespace rendor {

enum class ShaderType {
  VertexShader = GL_VERTEX_SHADER,
  TessellationControlShader = GL_TESS_CONTROL_SHADER,
  TessellationEvaluationShader = GL_TESS_EVALUATION_SHADER,
  GeometryShader = GL_GEOMETRY_SHADER,
  FragmentShader = GL_FRAGMENT_SHADER,
  ComputeShader = GL_COMPUTE_SHADER
};

/// Shader - The Shader class is a thin abstraction of OpenGL's shader functionality. Because it is only a low level
/// abstraction, it does not do any safety checks and wrong usage may result in a crash.
class Shader {
private:
  ShaderType shaderType;
  unsigned int shaderHandle = 0;
  bool compiled = false;

  std::string shaderSource;

public:
  explicit Shader(const ShaderType &type);
  Shader(const Shader &otherCopy) = delete;
  Shader(const Shader &&otherMove) = delete;
  ~Shader();

  /// Sets the shader source code. Equivalent to 'glShaderSource'.
  /// \param shaderSource GLSL source string
  void setShaderSource(const std::string &shaderSource) const;

  /// Compiles the shader. Equivalent to 'glCompileShader', but includes error checking.
  /// \return true if compiled successfully, false if not
  bool compileShader();

  /// Gets the shader type. The returned value can be casted to a GLenum to retrieve OpenGL's type.
  /// This is equivalent to glGetShaderiv(GL_SHADER_TYPE), but returns a ShaderType stored in this class instead.
  /// \return type of the shader
  const ShaderType &getShaderType() const;

  /// Gets the shader source code. Equivalent to glGetShaderSource(), but returns an std::string instead.
  /// \return string containing the shader's source code
  const std::string getShaderSource() const;

  /// Gets the shader source length. Equivalent to glGetShaderiv(GL_SHADER_SOURCE_LENGTH).
  /// \return length of the shader's source code
  const int getShaderSourceLength() const;

  /// Checks if the shader is flagged for deletion. Equivalent to glGetShaderiv(GL_DELETE_STATUS).
  /// \return true if the shader is deleted / flagged for deletion, false if not
  const bool isDeleted() const;

  /// Checks if the shader was successfully compiled.
  /// \return true if the shader is compiled, false if not
  bool isCompiled() const;

  /// Gets the OpenGL handle (or ID) to the shader. This is the handle that - for example - can be used to attach this
  /// shader to a program using glAttachShader(someProgramId, aShader.getHandle());
  /// \return
  const unsigned int getHandle() const;

  /// Loads a shader from a file
  /// \param type1 the type of shader to load
  /// \param path the file path to the GLSL file
  /// \return new Shader instance that can be compiled. Note that this instance doesn't need a 'setShaderSource'
  /// operation
  static Shader *LoadFromSourceFile(const ShaderType &type1, const std::string &path);
};

}

#endif //RENDOR_SHADER_H
