
SpicaCpp
========

This is the C++ part of the [Spica meta-library](https://github.com/pchapin/spica).

The code in this library is intended to be cross-platform and to support multiple build tools.
Specifically:

+ The `Makefile` is the main source of authority for building SpicaCpp. However, it assumes
  GNUmake and thus can't (easily) be used in some cases.
  
+ The `.vscode` folder contains setting information for Visual Studio Code that should allow
  VSCode to work properly on all of its supported platforms: Windows (with Cygwin), macOS, and 
  Linux.
  
+ There are Eclipse/CDT project files (`.project`, `.cproject`, and `.settings`), but they are
  configured for the GNU macOS tool chain and thus will only work as-is on macOS platforms.
  
+ There is a project file for Visual Studio (`SpicaCpp.vcxproj`) that can be included into a
  solution for a larger system where SpicaCpp is being used. The Visual Studio solution file
  here (`SpicaCpp.sln`) builds the SpicaCpp test and benchmark program(s). It is useful when
  working on SpicaCpp as a standalone project.
  
+ There is a Code::Blocks project file (`SpicaCpp.cbp`) that can be included into a solution for
  a larger system where Code::Blocks is being used. The Code::Blocks workspace file
  (`SpicaCpp.workspace`) here builds the SpicaCpp test and benchmark program(s). It is useful
  when working on SpicaCpp as a standalone project.
  
  Note that Code::Blocks 20.03, the latest version at the time of this writing, bundles an old
  version of g++ with marginal C++ 2020 support. Code::Blocks may have trouble compiling
  SpicaCpp until it is upgraded. Alternatively one could configure Code::Blocks to use a more
  modern g++ from another source.
  
SpicaCpp currently assumes C++ 2020. However, some of the components may be inside the subset of
C++ 1998 supported by Open Watcom. Accordingly, there is an Open Watcom target definition file
(`SpicaCpp.tgt`) for the Open Watcom IDE (very out-of-date), and there are makefiles for various
Open Watcom targets under `owbuild`. Open Watcom support was once a priority for SpicaCpp, but
that is no longer the case. Because Open Watcom only implements C++ 1998 (at best), it may not
be able to compile much, or any, of SpicaCpp as it exists today.

Not all of these project files and build configurations are equally maintained. Generally, the
Makefile and the Visual Studio solution file (and corresponding project file) are most likely to
be up-to-date.

The API documentation for the library can be built using Doxygen. It is left in a folder named
`api` (load `index.html` into a web browser to view the documentation). The `doc` folder
contains additional documentation.

Peter Chapin  
spicacality@kelseymountain.org  
