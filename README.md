See task.json on how to build with gcc. Omit DL library if not needed.

Example of building the 1st exercise (from the root directory of this workspace):

```
$ g++ -O0 -std=c++17 -g -Iinclude -ldl -lglfw -lGL src/**/*.cpp(.N) src/**/*.c(.N) 1/*.cpp -o main.out
```

The `(.N)` makes sure that empty matches are not reported as errors.

Replace the `1/*.cpp` with some other folder if you need to compile other exercises.

## Dependancies

* compiler that supports C++17 (ISO/IEC 14882:2017)
* optarg header - provided by compiler (gcc, clang, mingw)
* DL - Dynamic Linking Library, only needed in some cases
* OpenGL 3.3+ - this should be supported by your drivers
* GLM
* GLFW3