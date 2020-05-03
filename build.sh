#!/usr/bin/env zsh

g++ -O0 -g -std=c++17 \
  -Iinclude \
  -ldl -lglfw -lGL \
  src/**/*.cpp(.N) \
  src/**/*.c(.N) \
  "$1/main.cpp" \
  -o main.out
