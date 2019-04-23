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

#define CG_GL_DEBUG
#define CG_GL_VERIFY

#include <cg/Application.h>
#include <cg/GUIComponent.h>
#include <cg/Mesh.h>
#include <cg/common/Shader.h>
#include <cg/common/Program.h>
#include <cg/common/VertexArray.h>
#include <iostream>
#include <filesystem>
#include <cg/Vertex.h>
#include <glm/glm.hpp>

#include <glm/gtc/type_ptr.hpp>
#include <glm/gtx/transform.hpp>
#include <assimp/Importer.hpp>
#include <assimp/postprocess.h>
#include <assimp/scene.h>
#include <meshoptimizer.h>
#include <cg/InfoImporter.h>
#include <glm/ext.hpp>
#include <glm/gtx/string_cast.hpp>

namespace fs = std::experimental::filesystem;

void APIENTRY message(GLenum source, GLenum tyoe, GLuint id,
                      GLenum severity, GLsizei length, const GLchar *message, const void *param) {
  std::cout << "OpenGL: " << message << "\n";
}

class Camera {
 private:

 public:
  glm::vec3 position;
  float rot[3] = {-20.0f, -0.0f, 0.0f};
  glm::vec3 scale;

  glm::vec3 up;
  glm::vec3 forward;
  glm::vec3 right;

  Camera(float fov, float n, float f) : fov(fov), nearPlane(n), farPlane(f) {
    this->up = glm::vec3(0.0f, 1.0f, 0.0f);
    this->forward = glm::vec3(0.0f, 0.0f, -1.0f);
    this->right = glm::vec3(1.0f, 0.0f, 0.0f);
    this->position = glm::vec3(0.0f, 9.0f, 15.0f);
    this->projection = glm::perspective(glm::radians(fov), aspect, nearPlane, farPlane);
  }

  void showDebugWindow() {
    ImGui::Begin("Camera");
    ImGui::SliderFloat("Field of View", &fov, 1.0f, 180.0f);
    ImGui::SliderFloat("Near Clipping Plane", &nearPlane, 0.01f, 10.0f);
    ImGui::SliderFloat("Far Clipping Plane", &farPlane, 0.0001f, 10000.0f);
    ImGui::SliderFloat3("Position", glm::value_ptr(position), -5.0f, 5.0f);
    ImGui::SliderFloat3("Rotation", rot, -45.0f, 45.0f);
    if (ImGui::Button("Reset Transform")) {
      rot[0] = 0.0f;
      rot[1] = -20.0f;
      rot[2] = 0.0f;
      position.x = 0.0f;
      position.y = 1.0f;
      position.z = 3.0f;
    }
    ImGui::End();

    this->projection = glm::perspective(glm::radians(fov), aspect, nearPlane, farPlane);
  }

  glm::mat4 *getProjectionMatrix() {
    return &this->projection;
  }

  glm::mat4 getViewMatrix() {
    glm::quat rotX = glm::inverse(glm::angleAxis(glm::radians(rot[0]), glm::vec3(1.0f, 0.0f, 0.0f)));
    glm::quat rotY = glm::angleAxis(glm::radians(rot[1]), glm::vec3(0.0f, 1.0f, 0.0f));
    glm::quat rotZ = glm::angleAxis(glm::radians(rot[2]), glm::vec3(0.0f, 0.0f, 1.0f));

    glm::quat rotateDirection = rotX*rotY*rotZ;
    rotateDirection = glm::normalize(rotateDirection);

    glm::mat4 rotation = glm::mat4_cast(rotateDirection);
    glm::mat4 translation = glm::inverse(glm::translate(position));

    return rotation*translation;
  }

 private:
  glm::mat4 projection;

  float aspect = 1280.0f/720.0f;
  float fov;
  float nearPlane;
  float farPlane;
};

class Transform {
 protected:
  glm::vec3 position;
  glm::quat rotation;
  glm::vec3 scale;

  glm::vec3 forward;
  glm::vec3 up;
  glm::vec3 right;
  glm::vec3 angles;

  glm::mat4x4 transformation;

  virtual void UpdateMatrix() {
    transformation = glm::translate(position)*glm::mat4_cast(rotation)*glm::scale(scale);
  }

  static const glm::vec3 zero() {
    return glm::vec3(0.0f, 0.0f, 0.0f);
  }

  static const glm::vec3 one() {
    return glm::vec3(1.0f, 1.0f, 1.0f);
  }

 public:
  Transform() {
    position = Transform::zero();
    rotation = glm::quat(Transform::zero());
    scale = Transform::one();

    forward = glm::vec3(0.0f, 0.0f, 1.0f);
    up = glm::vec3(0.0f, 1.0f, 0.0f);
    right = glm::cross(glm::normalize(up), glm::normalize(forward));

    angles = Transform::zero();
    UpdateMatrix();
    UpdateCoordinateVectors();
  }

  void UpdateCoordinateVectors() {
    forward = glm::normalize(glm::vec3(0.0f, 0.0f, 1.0f)*rotation);
    up = glm::normalize(glm::vec3(0.0f, 1.0f, 0.0f)*rotation);
    right = glm::normalize(glm::vec3(1.0f, 0.0f, 0.0f)*rotation);
  }

  void setPosition(const glm::vec3 &pos) {
    position = pos;
    UpdateCoordinateVectors();
    UpdateMatrix();
  }

  void setRotation(const glm::quat &rot) {
    UpdateCoordinateVectors();
    rotation = rot;

    UpdateMatrix();
  }

  void setRotation(const glm::vec3 &angles) {
    glm::quat r = glm::quat(glm::vec3(glm::radians(angles.x), glm::radians(angles.y), glm::radians(angles.z)));
    forward = glm::normalize(glm::vec3(0.0f, 0.0f, 1.0f)*r);
    up = glm::normalize(glm::vec3(0.0f, 1.0f, 0.0f)*r);
    right = glm::normalize(glm::vec3(2.0f, 0.0f, 0.0f)*r);

    auto x = glm::angleAxis(glm::radians(angles.x), right);
    auto y = glm::angleAxis(glm::radians(angles.y), up);
    auto z = glm::angleAxis(glm::radians(angles.z), forward);

    rotation = glm::normalize(z*x*y);

    UpdateMatrix();
  }

  void setScale(const glm::vec3 &scl) {
    scale = scl;
    //UpdateCoordinateVectors();
    UpdateMatrix();
  }

  glm::vec3 &getPosition() {
    return position;
  }

  glm::quat &getRotation() {
    return rotation;
  }

  glm::vec3 &getScale() {
    return scale;
  }

  glm::vec3 &getUp() {
    return up;
  }

  glm::vec3 &getForward() {
    return forward;
  }

  glm::vec3 &getRight() {
    return right;
  }
};

class FreeCamera : public Transform {
 private:
  glm::mat4 projection;
  glm::mat4 view;
  glm::mat4 inverseView;
  glm::mat4 viewProjection;

  float fov = 0.0f;
  float aspect = 1280.0f/720.0f;
  float nearPlane;
  float farPlane;

  void UpdateMatrix() override {

    view = glm::lookAt(getPosition(), getPosition() + forward, up);
    inverseView = glm::inverse(view);
    viewProjection = projection*view;
  }

  void UpdateProjection() {
    projection = glm::perspective(glm::radians(this->fov), this->aspect, this->nearPlane, this->farPlane);
  }

 public:
  FreeCamera(float fov, float aspect, float nearPlane, float farPlane)
      : fov(fov), aspect(aspect), nearPlane(nearPlane), farPlane(farPlane) {
    projection = glm::perspective(glm::radians(this->fov), this->aspect, this->nearPlane, this->farPlane);
  }

  void setFov(float degrees) {
    fov = glm::radians(degrees);
  }

  void setAspectRatio(float ratio) {
    aspect = ratio;
  }

  void setAspectRatio(float width, float height) {
    aspect = width/height;
  }

  void setNearPlane(float nearPlane) {
    this->nearPlane = nearPlane;
  }

  void setFarPlane(float farPlane) {
    this->farPlane = farPlane;
  }

  float getFov() { return fov; }
  float getAspectRatio() { return aspect; }
  float getNearPlane() { return this->nearPlane; }
  float getFarPlane() { return this->farPlane; }

  glm::mat4 getViewMatrix() {
    return view;
  }

  glm::mat4 getInverseViewMatrix() {
    return inverseView;
  }

  glm::mat4 getViewProjectionMatrix() {
    return viewProjection;
  }

  glm::mat4 getProjectionMatrix() {
    return projection;
  }

  glm::vec3 angles;

  void MoveForward(float delta, float speed) {
    position += forward*(speed*delta);
  }

  void MoveBackward(float delta, float speed) {
    position -= forward*(speed*delta);
  }

  void MoveLeft(float delta, float speed) {
    position += right*(speed*delta);
  }

  void MoveRight(float delta, float speed) {
    position -= right*(speed*delta);
  }

  void showCameraControls() {
    ImGui::Begin("Free Camera");
    ImGui::Text("Camera Properties");
    ImGui::Spacing();
    ImGui::SliderFloat("Field of View", &fov, 0.1f, 100.0f);
    ImGui::SliderFloat("Near plane", &nearPlane, 0.001f, 100000.0f);
    ImGui::SliderFloat("Far plane", &farPlane, 0.001f, 100000.0f);
    ImGui::Spacing();
    ImGui::SliderFloat3("Position", glm::value_ptr(position), -100.0f, 100.0f);
    if (ImGui::SliderFloat4("Rotation", glm::value_ptr(angles), -100.0f, 100.0f)) {
      std::cout << "Rotating...\n";
      this->setRotation(angles);
    }

    ImGui::Spacing();
    ImGui::Separator();
    ImGui::Spacing();

    ImGui::Text("Aspect Ratio: %f", aspect);
    ImGui::Text("Forward: %f, %f, %f", forward.x, forward.y, forward.z);
    ImGui::End();

    UpdateProjection();
    UpdateMatrix();
  }
};

class Triangle : public cg::Application {
 private:
  cg::Shader *vert;
  cg::Shader *frag;
  cg::ShaderProgram *shader;
  cg::AsyncInfoImporter imp;

  cg::Mesh *m;
  Camera *c;
  FreeCamera *free_camera_;

  float fov = 45.0f;
  float cubeX = 0.0f;
  float cubeY = 0.0f;
  float cubeZ = 0.0f;

  float speed = 0.2f;

  glm::mat4 model;
  bool showWireFrame = false;
  bool cull = true;

  fs::path currentPath;
  size_t reduction = 0;
  float error = 0.0f;

 public:
  Triangle() : cg::Application(4, 5, "Triangle", 1280, 720) {}

 protected:
  void onInit() override {
    Application::onInit();
    currentPath = fs::current_path();
    glDebugMessageCallback(message, nullptr);

    c = new Camera(60.0f, 0.1f, 1000.0f);
    free_camera_ = new FreeCamera(60.0f, 1280.0f/720.0f, 0.1f, 10000.0f);
    free_camera_->setRotation(glm::quat(45.0f, 0.0f, 0.0f, 0.0f));
    free_camera_->setPosition(glm::vec3(0.0f, 5.0f, -20.0f));
    imp.LoadAsync("dragon.obj");

    recompileShader();

    model = glm::translate(glm::mat4(1.0f), glm::vec3(cubeX, cubeY, cubeZ));
    glEnable(GL_DEPTH_TEST);
    glDepthFunc(GL_LESS);
  }

  void recompileShader() {
    this->vert = cg::Shader::LoadFromSourceFile(cg::ShaderType::VertexShader, "shader.vert");
    this->frag = cg::Shader::LoadFromSourceFile(cg::ShaderType::FragmentShader, "shader.frag");

    m = cg::Mesh::LoadMesh("cube.obj", 0);

    vert->compileShader();
    frag->compileShader();

    this->shader = new cg::ShaderProgram();
    this->shader->attachShader(vert);
    this->shader->attachShader(frag);
    this->shader->linkProgram();

    Transform t;
  }

  void onViewportResize(int width, int height) override {
    Application::onViewportResize(width, height);
    glViewport(0, 0, width, height);
  }

  float deltaTime = 0;
  float moveSpeed = 0.50;

  void onKeyInput(int key, int scancode, KeyInputType action, int mods) override {
    Application::onKeyInput(key, scancode, action, mods);

    if (key==GLFW_KEY_W) {
      free_camera_->MoveForward(deltaTime, moveSpeed);
    } else if (key==GLFW_KEY_S) {
      free_camera_->MoveBackward(deltaTime, moveSpeed);
    } else if (key==GLFW_KEY_A) {
      free_camera_->MoveLeft(deltaTime, moveSpeed);
    } else if (key==GLFW_KEY_D) {
      free_camera_->MoveRight(deltaTime, moveSpeed);
    } else if (key==GLFW_KEY_Q) {
      c->position.y -= moveSpeed*deltaTime;
    } else if (key==GLFW_KEY_E) {
      c->position.y += moveSpeed*deltaTime;
    }



  }

  bool mm = true;
  float lx;
  float ly;

  void onMouseMove(double x, double y) override {
    Application::onMouseMove(x, y);

    if (mm) {
      lx = x;
      ly = y;
      mm = false;
    }

    float ox = x - lx;
    float oy = y = ly;
    lx = x;
    ly = y;

    float s = 5.0f;
    ox *= s;
    oy *= s;

    glm::quat rotX = glm::rotate(ox, free_camera_->getUp());
    glm::quat rotY = glm::rotate(oy, free_camera_->getRight());
    glm::quat rot = glm::normalize(rotX * rotY);
    free_camera_->setRotation(rot + free_camera_->getRotation());
  }

  void onUpdate(float delta) override {
    Application::onUpdate(delta);
    deltaTime = delta;
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
    glClearColor(1.0, 0.2, 0.3, 1.0);

    if (imp.IsReady()) {
      cg::MeshInfo info = imp.Get();
      delete m;
      m = new cg::Mesh(info.Vertices(), info.Indices());
    }

    model = glm::rotate(model, speed*delta, glm::vec3(0, 1, 0));
    //glm::mat4 view = c->getViewMatrix();
    glm::mat4 view = free_camera_->getViewMatrix();
    glm::mat4 proj = free_camera_->getProjectionMatrix();

    if (showWireFrame)
      glPolygonMode(GL_FRONT_AND_BACK, GL_LINE);
    else
      glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);

    if (cull) {
      glEnable(GL_CULL_FACE);
      glCullFace(GL_BACK);
    } else {
      glDisable(GL_CULL_FACE);
    }

    if (m) m->draw(this->shader, model, view, proj);
  }

  std::vector<char> pathBuffer;

  bool ends_with(const std::string &s, const std::string &end) {
    return s.rfind(end)==(s.size() - end.size());
  }

  float fileSize = 0.0f;
  std::string fileType = "";

  void draw_file_manager() {
    ImGui::Text("%s", currentPath.string().c_str());
    ImGui::Separator();
    if (currentPath.has_parent_path()) {
      if (ImGui::Selectable("..", false, ImGuiSelectableFlags_AllowDoubleClick)) {
        if (ImGui::IsMouseDoubleClicked(0)) {
          currentPath = currentPath.parent_path();
        }
      }
    }

    if (ImGui::BeginPopupContextItem("rc")) {
      ImGui::Text("nee");
      ImGui::EndPopup();
    }

    for (const fs::directory_entry &entry : fs::directory_iterator(currentPath.string())) {
      if (ImGui::Selectable(entry.path().filename().string().c_str(),
                            false,
                            ImGuiSelectableFlags_None | ImGuiSelectableFlags_AllowDoubleClick)) {
        if (ImGui::IsMouseDoubleClicked(0)) {
          if (fs::is_directory(entry.path())) {
            currentPath = entry.path();
          } else if (ends_with(entry.path().filename().string(), ".obj")
              || ends_with(entry.path().filename().string(), ".fbx")) {
            //m = importer.Import(entry.path().string());
            imp.LoadAsync(entry.path().string());
          }
        } else {
          if (ImGui::IsMouseClicked(1)) {
            std::cerr << "hi";
            ImGui::OpenPopup("rc");
          }

          if (fs::is_directory(entry.path())) {
            fileSize = 0;
            fileType = "Directory";
          } else {
            fileSize = (float) fs::file_size(entry.path());
            fileType = entry.path().extension().string();
          }
        }

        if (ImGui::IsMouseClicked(1)) {
          ImGui::OpenPopup("rc");
        }
      }
    }

    ImGui::Separator();

    if (fileSize >= 0 && fileSize < 1024) {
      ImGui::Text("File size: %f bytes", fileSize);
    } else if (fileSize >= 1024 && fileSize < 1048576) {
      ImGui::Text("File size: %f kb", fileSize/1024);
    } else if (fileSize > 1048576) {
      ImGui::Text("File size: %f mb", fileSize/1048576);
    }

    ImGui::Text("File type: %s", fileType.c_str());

    ImGui::ProgressBar(imp.GetProgress());
    ImGui::Separator();

  }

  bool showScene = false;
  bool showModels = false;

  void onGui() override {
    Application::onGui();
    //c->showDebugWindow();
    free_camera_->showCameraControls();

    if (ImGui::BeginMainMenuBar()) {
      if (ImGui::BeginMenu("View")) {
        if (ImGui::MenuItem("Scene")) {
          showScene = true;
        }

        if (ImGui::MenuItem("Camera")) {
          showModels = true;
        }
        ImGui::EndMenu();
      }
      ImGui::EndMainMenuBar();
    }

    if (showScene) {
      if (!ImGui::Begin("Scene", &showScene, ImGuiWindowFlags_NoCollapse)) {
        ImGui::End();
      } else {
        ImGui::SliderFloat("Speed", &speed, 0.0f, 200.0f);
        ImGui::SliderFloat("Movement Speed", &moveSpeed, 0.25f, 400.0f);
        ImGui::Checkbox("Wireframe Mode", &showWireFrame);
        ImGui::Checkbox("Backface Culling", &cull);
        if (ImGui::Button("Recompile Shaders")) {
          recompileShader();
        }
        ImGui::End();
      }
    }

    if (showModels) {
      ImGui::Begin("Models", &showModels, ImGuiWindowFlags_NoCollapse);
      ImGui::BeginTabBar("Models", ImGuiTabBarFlags_Reorderable);

      if (ImGui::BeginTabItem("Explorer")) {
        draw_file_manager();
        ImGui::EndTabItem();
      }

      if (ImGui::BeginTabItem("Optimization")) {
        float acmr[] = {imp.GetOptimizationStats().acmr_before, imp.GetOptimizationStats().acmr_after};
        float atvr[] = {imp.GetOptimizationStats().atvr_before, imp.GetOptimizationStats().atvr_after};
        float overdraw[] = {imp.GetOptimizationStats().overdraw_before, imp.GetOptimizationStats().overdraw_after};
        float overfetch[] = {imp.GetOptimizationStats().overfetch_before, imp.GetOptimizationStats().overfetch_after};
        float inds[] = {static_cast<float>(imp.GetOptimizationStats().indices_before),
                        static_cast<float>(imp.GetOptimizationStats().indices_after)};
        ImGui::PlotHistogram("ACMR before/after (0.5 is best, 3.0 worst)", acmr, 2, 0, 0, 0.5f, 3.0f, ImVec2(150, 75));
        ImGui::PlotHistogram("ATVR before/after (1.0 is best, 6.0 worst)", atvr, 2, 0, 0, 1.0f, 6.0f, ImVec2(150, 75));
        ImGui::PlotHistogram("Overdraw before/after (lower is better)",
                             overdraw,
                             2,
                             0,
                             0,
                             0.0f,
                             10.0f,
                             ImVec2(150, 75));
        ImGui::PlotHistogram("Overfetch before/after (lower is better)",
                             overfetch,
                             2,
                             0,
                             0,
                             0.0f,
                             10.0f,
                             ImVec2(150, 75));
        ImGui::PlotHistogram("Indices before/after (lower is better)",
                             inds,
                             2,
                             0,
                             0,
                             0.0f,
                             3.402823466e+38F,
                             ImVec2(150, 75));
        ImGui::InputInt("Reduction", reinterpret_cast<int *>(&reduction));
        ImGui::InputFloat("Error", &error);
        if (ImGui::Button("Optimize")) {
          cg::MeshInfo info(m->vertices, m->indices);
          std::cout << info.Simplify(reduction, error) << "\n";
          delete m;
          m = new cg::Mesh(info.Vertices(), info.Indices());
        }
        ImGui::Text("Vertices: %i", m->vertices.size());
        ImGui::Text("Indices: %i", m->indices.size());
        ImGui::EndTabItem();
      }

      ImGui::EndTabBar();
      ImGui::End();
    }

  }
};

int main(int argc, char **argv) {

  Triangle().run();
}
