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

#ifndef RENDOR_APPLICATION_H
#define RENDOR_APPLICATION_H

#include <glad/glad.h>
#include <GLFW/glfw3.h>
#include <string>

namespace rendor {

class Application {
 private:
  GLFWwindow *handle;
  std::string title;
  int width;
  int height;

 public:
  Application(int glMajor, int glMinor, const std::string &title, int width, int height);
  ~Application();

  void run();

 protected:
  virtual void onInit() {}
  virtual void onUpdate(double delta) {}
  virtual void onViewportResize(int width, int height) {}
  virtual void onKeyInput() {}
  virtual void onMouseInput() {}
  virtual void onMouseMove() {}

 private:
  inline static void OnViewportResizeCallback(GLFWwindow *window, int width, int height) {
    auto *application = (Application *)glfwGetWindowUserPointer(window);
    application->onViewportResize(width, height);
  }
};

}

#endif //RENDOR_APPLICATION_H
