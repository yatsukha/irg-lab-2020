def Settings(**kwargs):
  return {
    'flags': ['-x', 'c++', '-Wall', '-Wextra', '-I../include', '-ldl', '-lglfw', '-lGL']
  }
