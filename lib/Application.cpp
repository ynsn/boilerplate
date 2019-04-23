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

#include "cg/Application.h"

namespace cg {

Application::Application(int glMajor, int glMinor, const std::string &title, int width, int height)
    : title(title), width(width), height(height) {
  glfwInit();
  glfwWindowHint(GLFW_VERSION_MAJOR, glMajor);
  glfwWindowHint(GLFW_VERSION_MINOR, glMinor);
  glfwWindowHint(GLFW_SAMPLES, 8);
  this->handle = glfwCreateWindow(this->width, this->height, this->title.c_str(), nullptr, nullptr);

  glfwSetWindowUserPointer(this->handle, this);
  glfwSetFramebufferSizeCallback(this->handle, OnViewportResizeCallback);
  glfwSetKeyCallback(this->handle, OnKeyInputCallback);
  glfwSetCursorPosCallback(this->handle, OnMouseMoveCallback);

  glfwMakeContextCurrent(this->handle);
  gladLoadGLLoader((GLADloadproc) glfwGetProcAddress);
  glfwSwapInterval(1);
}

Application::~Application() {
  glfwDestroyWindow(this->handle);
}

void Application::run() {
  this->onInit();

  double time = glfwGetTime();
  double deltaTime;
  while (!glfwWindowShouldClose(this->handle)) {

    double currentTime = glfwGetTime();
    deltaTime = currentTime - time;
    this->onUpdate(deltaTime);
    time = currentTime;

    ImGui_ImplOpenGL3_NewFrame();
    ImGui_ImplGlfw_NewFrame();
    ImGui::NewFrame();
    this->onGui();
    ImGui::Render();
    ImGui_ImplOpenGL3_RenderDrawData(ImGui::GetDrawData());

    ImGuiIO &io = ImGui::GetIO();
    if (io.ConfigFlags & ImGuiConfigFlags_ViewportsEnable)
    {
      GLFWwindow* backup_current_context = glfwGetCurrentContext();
      ImGui::UpdatePlatformWindows();
      ImGui::RenderPlatformWindowsDefault();
      glfwMakeContextCurrent(backup_current_context);
    }

    glfwSwapBuffers(this->handle);
    glfwPollEvents();
  }
}

void Application::setTitle(const std::string &title) {
  this->title = title;
  glfwSetWindowTitle(this->handle, this->title.c_str());
}

const std::string &Application::getTitle() {
  return this->title;
}

void Application::setSize(int width, int height) {
  this->width = width;
  this->height = height;
  glfwSetWindowSize(this->handle, this->width, this->height);
}

void Application::setAspectRatio(int nomer, int denom) {
  glfwSetWindowAspectRatio(this->handle, nomer, denom);
}

int Application::getWidth() {
  return this->width;
}

int Application::getHeight() {
  return this->height;
}

}