// stdlibc++
#include <iostream>

// glm
#include <glm/glm.hpp>

// irg
#include <irg/irg.hpp>

namespace irg {

  void task11(::std::ostream& out) {
    auto v1 = ::glm::vec3{2, 3, -4} + ::glm::vec3{-1, 4, -1};

    out << "v1: " << v1 << "\n";
    out << "s:  " << ::glm::dot(v1, ::glm::vec3{-1, 4, -1}) << "\n";

    auto v2 = ::glm::cross(v1, ::glm::vec3{2, 2, 4});

    out << "v2: " << v2 << "\n";
    out << "v3: " << ::glm::normalize(v2) << "\n";
    out << "v4: " << -v2 << "\n";

    out << "M1:\n" 
        << ::glm::mat3{{1, 2, 4}, {2, 1, 5}, {3, 3, 1}} 
            + ::glm::mat3{{-1, 5, -4}, {2, -2, -1}, {-3, 7, 3}};

    out << "M2:\n" 
        << ::glm::mat3{{1, 2, 4}, {2, 1, 5}, {3, 3, 1}} 
            * ::glm::transpose(
                ::glm::mat3{{-1, 5, -4}, {2, -2, -1}, {-3, 7, 3}});

    out << "M3:\n" 
        << ::glm::mat3{{1, 2, 4}, {2, 1, 5}, {3, 3, 1}} 
            * ::glm::inverse(
                ::glm::mat3{{-1, 5, -4}, {2, -2, -1}, {-3, 7, 3}});
  }

  void task12(::std::ostream& out, ::std::istream& in) {
    out << "Supply parameters for 3 equations with 3 unknown values."
        << "\n";

    ::glm::mat3 a;
    ::glm::vec3 b;

    for (::glm::length_t c = 0; c < a.length(); ++c) {
      for (::glm::length_t r = 0; r < a[c].length(); ++r)
        in >> a[r][c];
      in >> b[c];
    }

    out << "Solution: " << ::glm::inverse(a) * b << "\n";
  }

  void task13(::std::ostream& out, ::std::istream& in) {
    out << "Supply three triangle points and one free point in the format "
        << "(x, y, z)." << "\n";

    ::glm::mat3 a;
    ::glm::vec3 b;

    for (::glm::length_t r = 0; r < a[0].length(); ++r)
      for (::glm::length_t c = 0; c < a.length(); ++c)
        in >> a[r][c];
    
    for (::glm::length_t c = 0; c < b.length(); ++c)
      in >> b[c];

    out << "Barycentric coordinates: " << ::glm::inverse(a) * b << "\n";
    out << a;
  }

}

int main(int argc, char const* const* argv) {
  ::std::ios_base::sync_with_stdio(false);
  ::std::cout << ::std::fixed;

  if (argc != 2)
    ::std::cout << "Pass the task number as a command line argument." << "\n",
    ::std::exit(EXIT_FAILURE);

  switch (argv[1][0]) {
    case '1':
      ::irg::task11(::std::cout);
      break;
    case '2':
      ::irg::task12(::std::cout, ::std::cin);
      break;
    case '3':
      ::irg::task13(::std::cout, ::std::cin);
      break;
    default:
      ::std::cerr << "Invalid argument." << "\n";
  }
}