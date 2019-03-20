# Rendor
Prototyping framework for developing interactive graphics applications using OpenGL

# Framework Dependencies
Rendor is a framework built on top of other libraries. Rendor currently depends on the following libraries:
 - GLFW (window / OpenGL context)
 - GLAD (OpenGL extension loader)
 - stb (various utilities, including loading images)

All dependencies are added as Git submodules; seperate downloads or checkouts should not be neccesary.

# Example
To start using Rendor, add Rendor as a Git submodule (recommended) and add it as a subdirectory in your CMakeLists.txt. To create an application, start with defining a class that derives from rendor::Application. Application.h already loads OpenGL for you, so you don't have to set anything up.

```c++
#include <rendor/Application.h>

class MyApp : public rendor::Application {
public:
  MyApp() : rendor::Application(4, 5, "My App", 1280, 720) {}
}
```

Then, in your main function, instanciate this class and call the **run()** method on it.

```c++
int main(int argc, char **argv) {
  MyApp().run();
}
```

This should give you a black window, since we aren't drawing anything yet.

T.B.C...
