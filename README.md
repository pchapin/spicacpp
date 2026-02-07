
SpicaCpp
========

SpicaCpp was originally intended to be a collection of specialized algorithms and data structures in
C++. However, it also contains other components including a simple unit test framework, debugging
support classes, and various helper classes. In effect, the library has become a hodge-podge of
components that over the years I've found helpful or interesting.

The code in this library is intended to be cross-platform. It uses CMake as the meta-build tool
from which other builds can be generated.

SpicaCpp currently assumes C++ 2020.

The API documentation for the library can be built using Doxygen. It is left in a folder named
`api` inside the `docs` folder (load `index.html` into a web browser to view the documentation).
The `docs` folder contains additional documentation.

## Building SpicaCpp

There are several CMake presets defined in `CMakePresets.json`. Choose the appropriate preset for
your situation and configure the build with CMake. For example, if using `clang` on Linux do:

```bash
cmake --preset linux-clang-debug
cmake --build --preset linux-clang-debug
```

The presets use the Ninja build tool, so you will need that installed.

You can open the folder containing the project in CLion or Visual Studio and those tools should
automatically recognize the CMake configuration and let you select which preset(s) you want to
build.

Peter Chapin  
spicacality@kelseymountain.org  
