# IRG lab exercises

Note: this readme assumes you are using a *nix based operating system.

## Dependancies

* compiler that supports C++17 (ISO/IEC 14882:2017)
* optarg header - provided by compiler (gcc, clang, mingw)
* DL - Dynamic Linking Library, only needed in some cases
* OpenGL 3.3+ - this should be supported by your drivers
* GLM
* GLFW3
* meson and ninja if you choose to build with it

## Building

### Building using provided script

Use `build.sh` script to build lab exercises. The script takes a number as a command line argument indicating which exercise to compile, and then outputs `main.out` executable into root directory. For exercises 5 and 6 it is necessary to move the executable into the exercise folder: `mv main.out 5`, as the file paths changed.

If the script does not work remove the null glob operators: `(.N)`.

### Building using meson

Lab exercises that have `meson.build` file in them (from exercise 5 upwards) can be built using meson. To do this change the current director the the directory in which the build file resides. After that use `meson <builddir>` to specify a build directory. This directory can not be '.', it must be a separate directory from the source tree. After that move into the directory with `cd <builddir>` and run `ninja` to compile the code. After that you need to copy the assets (shaders, objects and textures) to the build directory, and you can do that using `ninja asset-copy`. After that you can just run `./main.out`.
