See task.json on how to build with gcc. Omit DL library if not needed.

Example of building the 1st exercise (from the root directory of this workspace):

```
$ g++ -O0 -std=c++17 -g -Iinclude -ldl -lglfw -lGL src/**/*.cpp(.N) src/**/*.c(.N) 1/*.cpp -o main.out
```

The `(.N)` makes sure that empty matches are not reported as errors.

## Dependancies

* compiler that supports C++17 (ISO/IEC 14882:2017)
* DL - Dynamic Linking Library, only needed in some cases
* OpenGL 3.3+ - this should be supported by your drivers
* GLM
* GLFW3

## Things you might need to replace

* glad - src/glad, include/glad and include/KHR; generate using https://glad.dav1d.de/ with settings: gl version 3.3, core profile, generate loader