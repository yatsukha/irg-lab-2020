#pragma once

#include <cstdint>
#include <utility>
#include <optional>
#include <memory>

namespace irg {
  
  struct configuration {
    ::std::pair<::std::size_t, ::std::size_t> screen_dim;
    
    ::std::size_t epsilon;
    ::std::size_t max_iter;
    
    using float_pair = ::std::pair<float, float>;
    
    float_pair u;
    float_pair v;
    
    ::std::optional<float_pair> c = {};
  };
  
  configuration parse_configuration(int const argv, char** argc);
  
  using fractal_colors = ::std::unique_ptr<unsigned char[]>;
  
  fractal_colors mandelbrot_from(configuration const& conf);
  fractal_colors julia_from(configuration const& conf);
}