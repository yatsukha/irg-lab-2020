project('irg', 'c', 'cpp')

asset_path = meson.current_source_dir()
output_dir = meson.current_build_dir()

run_command(
  'cp',
  '-r',
  join_paths(asset_path, 'shaders'),
  output_dir
)

executable(
  'main.out', 
  sources: [
    'main.cpp',
    '../src/glad/glad.c',
    '../src/stb_image.cpp',
    '../src/irg/common.cpp',
    '../src/irg/keyboard.cpp',
    '../src/irg/mesh.cpp',
    '../src/irg/mouse.cpp',
    '../src/irg/primitive.cpp',
    '../src/irg/shape.cpp',
    '../src/irg/texture.cpp',
    '../src/irg/window.cpp',
    '../src/irg/camera.cpp',
    'src/irg/fractals.cpp'
  ],
  include_directories: [
    '../include',
    'include'
  ],
  dependencies: [
    dependency('OpenGL'),
    dependency('glfw3'),
    dependency('glm'),
    meson.get_compiler('c').find_library('dl')
  ],
  override_options: ['cpp_std=c++17']
)
