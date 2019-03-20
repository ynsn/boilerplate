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
 public:
  enum class KeyInputType {
    Press,
    Repeat,
    Release
  };

  enum class KeyInputModifier {
    Shift = 0x01,
    Ctrl = 0x02,
    Alt = 0x04,
    Super = 0x08
  };

 private:
  GLFWwindow *handle;
  std::string title;
  int width;
  int height;

 public:
  Application(int glMajor, int glMinor, const std::string &title, int width, int height);
  ~Application();

  void run();

  void setTitle(const std::string &title);
  const std::string &getTitle();

  void setSize(int width, int height);
  void setAspectRatio(int nomer, int denom);
  int getWidth();
  int getHeight();

 protected:
  virtual void onInit() {}
  virtual void onUpdate(double delta) {}
  virtual void onViewportResize(int width, int height) {}
  virtual void onKeyInput(int key, int scancode, KeyInputType action, int mods) {}
  virtual void onMouseInput() {}
  virtual void onMouseMove() {}

 private:
  inline static void OnViewportResizeCallback(GLFWwindow *window, int width, int height) {
    auto *application = (Application *) glfwGetWindowUserPointer(window);
    application->onViewportResize(width, height);
  }

  inline static void OnKeyInputCallback(GLFWwindow *window, int key, int scancode, int action, int mods) {
    auto *application = (Application *) glfwGetWindowUserPointer(window);

    KeyInputType inputAction;
    KeyInputModifier inputModifier;

    // Convert input action types
    switch (action) {
      case GLFW_PRESS:
        inputAction = KeyInputType::Press;
        break;
      case GLFW_REPEAT:
        inputAction = KeyInputType::Repeat;
      case GLFW_RELEASE:
        inputAction = KeyInputType::Release;
      default:
        inputAction = KeyInputType::Repeat;
        break;
    }

    application->onKeyInput(key, scancode, inputAction, mods);
  }
};

}

#endif //RENDOR_APPLICATION_H
