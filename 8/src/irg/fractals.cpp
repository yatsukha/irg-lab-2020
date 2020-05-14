#include <irg/fractals.hpp>

#include <string>
#include <complex>
#include <iostream>
#include <cmath>
#include <functional>
#include <cassert>

#include <getopt.h>

namespace irg {
  
  namespace detail {
    
    configuration::float_pair parse_pair(char const* c) {
      ::std::string str = c;
      return {::std::stof(str), ::std::stof(str.substr(str.find(',') + 1))};
    }
    
  }
  
  configuration parse_configuration(int const argv, char** argc) {
    configuration conf;
    int option;
    
    while ((option = ::getopt(argv, argc, "e:m:u:v:c:")) != -1) {
      switch (option) {
        case 'e':
          conf.epsilon = ::std::stoul(::optarg);
          break;
        case 'm':
          conf.max_iter = ::std::stoul(::optarg);
          break;
        case 'u':
          conf.u = detail::parse_pair(::optarg);
          break;
        case 'v':
          conf.v = detail::parse_pair(::optarg);
          break;
        case 'c':
          conf.c = detail::parse_pair(::optarg);
          break;
      }
    }
    
    return conf;
  }
   
  namespace detail {
     
    template<typename Callable>
    fractal_colors fractal_base(configuration const& conf,
                                Callable cz) {
      auto [w, h] = conf.screen_dim;
      
      fractal_colors fc{
        new fractal_colors::element_type[
          conf.screen_dim.first * conf.screen_dim.second * 3
        ]
      };
      
      auto u_range = conf.u.second - conf.u.first;
      auto v_range = conf.v.second - conf.v.first;
      
      ::std::size_t idx = 0;
      
      for (::std::size_t y = 0; y < h; ++y) {
        for (::std::size_t x = 0; x < w; ++x) {
          auto [c, z] = cz(
            static_cast<float>(x) / w
            * u_range + conf.u.first,
            static_cast<float>(y) / h
              * v_range + conf.v.first
          );
          
          ::std::size_t iter = 0;
          
          while (++iter < conf.max_iter && ::std::abs(z) < conf.epsilon) {
            z = (z * z) + c;
          }
          
          auto scale = static_cast<float>(iter) / conf.max_iter;
          
          fc[idx++] = ::std::pow(scale, 1) * 255.0f;
          fc[idx++] = ::std::pow(scale, 2) * 255.0f;
          fc[idx++] = ::std::pow(scale, 0.5) * 255.0f;
        }
      }
      
      return fc;
    }

  }
   
  fractal_colors mandelbrot_from(configuration const& conf) {
    return detail::fractal_base(conf, [](auto&& x, auto&& y) {
      return ::std::pair{
        ::std::complex{x, y},
        ::std::complex{0.f, 0.f}
      };
    });
  }
  
  fractal_colors julia_from(configuration const& conf) {
    assert(conf.c);
    
    return detail::fractal_base(conf, [c = *conf.c](auto&& x, auto&& y) {
      return ::std::pair{
        ::std::complex{c.first, c.second},
        ::std::complex{x, y}
      };
    });
  }
  
}