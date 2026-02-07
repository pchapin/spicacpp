
TODO
====

This project is in a state of flux because the build system is being converted from an ad hoc
collection of Makefiles to being CMake-based. This conversion is in progress; the examples,
benchmarks, and test program are not yet integrated with the CMake build. Here is the current TODO
list:

+ Convert tests to use Catch2 (or some other well-established C++ testing framework).
+ Add CMake integration for the examples.
+ Add CMake integration for the benchmarks.
+ Clean up the many warnings produced by the Microsoft compiler.
+ Replace C-ism with equivalent C++ constructions (that is, C library functions).
+ Some of the components use Boost threads. Change these to use C++ standard threads. See the
  `pending` folder and certain header files.
