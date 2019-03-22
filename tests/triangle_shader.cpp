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

#include <rendor/Application.h>
#include <rendor/common/Shader.h>
#include <rendor/common/Program.h>
#include <iostream>

class Triangle : public rendor::Application {
private:
  unsigned int vao;
  unsigned int vbo;
  rendor::Shader *vert;
  rendor::Shader *frag;
  rendor::ShaderProgram *shader;

public:
  Triangle() : rendor::Application(4, 5, "Triangle", 1280, 960) {}

protected:
  void onInit() override {
    Application::onInit();

    // Create and fill buffers
    glGenVertexArrays(1, &this->vao);
    glBindVertexArray(this->vao);

    glGenBuffers(1, &this->vbo);
    glBindBuffer(GL_ARRAY_BUFFER, this->vbo);

    std::vector<float> vertices({
                                    -0.5, -0.5, 0.0,
                                    0.5, -0.5, 0.0,
                                    0.0, 0.5, 0.0
                                });

    glBufferData(GL_ARRAY_BUFFER, vertices.size() * sizeof(float), &vertices[0], GL_STATIC_DRAW);
    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 3 * sizeof(float), (const void *) 0);
    glEnableVertexAttribArray(0);

    // Create shaders

    const char *vertexShader = "#version 450\n"
                               "\n"
                               "layout (location = 0) in vec3 pos;\n"
                               "\n"
                               "void main() {\n"
                               "    gl_Position = vec4(pos, 1.0f);\n"
                               "}\n";

    const char *fragmentshader = "#version 450\n"
                                 "\n"
                                 "out vec4 col;\n"
                                 "uniform vec4 color;"
                                 "\n"
                                 "void main() {\n"
                                 "    col = color;\n"
                                 "}\n";

    this->vert = new rendor::Shader(rendor::ShaderType::VertexShader);
    this->frag = new rendor::Shader(rendor::ShaderType::FragmentShader);

    vert->setShaderSource(vertexShader);
    frag->setShaderSource(fragmentshader);

    std::cout << "====== COMPILATION STATUS ======\n";
    std::cout << "Vertex shader compilation status: " << vert->compileShader() << "\n";
    std::cout << "Fragment shader compilation status: " << frag->compileShader() << "\n";

    std::cout << "\n====== SOURCE LENGTHS ======\n";
    std::cout << "Vertex shader source length: " << vert->getShaderSourceLength() << "\n";
    std::cout << "Fragment shader source length: " << frag->getShaderSourceLength() << "\n";

    std::cout << "\n====== SOURCE CODE ======\n";
    std::cout << "Vertex shader source: " << vert->getShaderSource() << "\n";
    std::cout << "Fragment shader source: " << frag->getShaderSource() << "\n";

    this->shader = new rendor::ShaderProgram();
    this->shader->attachShader(vert);
    this->shader->attachShader(frag);
    this->shader->linkProgram();
  }

  void onViewportResize(int width, int height) override {
    Application::onViewportResize(width, height);
    glViewport(0, 0, width, height);
  }

  void onUpdate(double delta) override {
    Application::onUpdate(delta);
    glClear(GL_COLOR_BUFFER_BIT);
    glClearColor(1.0, 0.2, 0.3, 1.0);

    glUseProgram(this->shader->getHandle());
    this->shader->setUniform4f("color", glm::vec4(1.0, 0.7, 0.2, 1.0));
    glDrawArrays(GL_TRIANGLES, 0, 3);
  }
};

int main(int argc, char **argv) {
  Triangle().run();
}
