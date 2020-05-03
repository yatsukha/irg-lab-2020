project('irg', 'c', 'cpp')

asset_path = meson.current_source_dir()
output_dir = meson.current_build_dir()

run_target(
  'asset-copy',
  command: [
    [
      'cp',
      '-r',
      join_paths(asset_path, 'textures'),
      join_paths(asset_path, 'shaders'),
      join_paths(asset_path, 'objects'),
      output_dir
    ]
  ]
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
    '../src/irg/camera.cpp'
  ],
  include_directories: [
    '../include'
  ],
  dependencies: [
    dependency('OpenGL'),
    dependency('glfw3'),
    dependency('glm'),
    meson.get_compiler('c').find_library('dl')
  ],
  override_options: ['cpp_std=c++17']
)
