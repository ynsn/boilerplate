

#ifndef RENDOR_INCLUDE_CG_GUICOMPONENT_H_
#define RENDOR_INCLUDE_CG_GUICOMPONENT_H_

#include "Application.h"

namespace cg {

class GuiWindow {
 protected:
  std::string title;
  ImGuiWindowFlags flags;

 public:
  bool open;

  GuiWindow(const std::string &title, bool open, ImGuiWindowFlags flags) : title(title), open(open), flags(flags) {}
  virtual ~GuiWindow() {}

  const virtual void onGUI() final {
    if (open) {
    ImGui::Begin(title.c_str(), &open, flags);
    draw();
    ImGui::End();}
  }

 private:

  virtual void draw() = 0;

 protected:
  void Text(const std::string &format) {
    ImGui::Text("%s", format.c_str());
  }

  void Text(const std::string &format, const std::string args...) {
    ImGui::Text(format.c_str(), args.c_str());
  }
};

}

#endif //RENDOR_INCLUDE_CG_GUICOMPONENT_H_
