#include <algorithm>
#include <vector>
#include <memory>
#include <string>
#include <sstream>
#include <unordered_set>
#include <cmath>

#include <unistd.h>
#include <getopt.h>

#include <glad/glad.h>
#include <GLFW/glfw3.h>
#include <glm/common.hpp>
#include <glm/glm.hpp>
#include <glm/gtc/quaternion.hpp>
#include <glm/ext/quaternion_common.hpp>
#include <glm/ext/quaternion_trigonometric.hpp>
#include <glm/gtx/quaternion.hpp>

#include <irg/common.hpp>
#include <irg/shader.hpp>
#include <irg/mouse.hpp>
#include <irg/shape.hpp>
#include <irg/mesh.hpp>
#include <irg/texture.hpp>
#include <irg/window.hpp>

namespace irg {

  struct global_params {
    bool invisible_light = false;
    ::glm::vec3 light_position = {0.6f, 0.6f, 0.6f};
    ::glm::vec3 light_scaling  = ::glm::vec3(0.05f);
    ::std::string light_object = "./4/objects/kocka.obj";

    ::glm::vec3 object_scaling   = ::glm::vec3(0.5f);
    ::std::string object_object  = "./4/objects/dragon.obj";
    ::std::string object_texture = "";

    ::glm::vec3 test_vertex = ::glm::vec3(0.0f);

    bool preserve_aspect_ratio = false;
  };

  namespace detail {
    
    void print_help() {
      ::std::cout << "Possible command line options:"
                  << "\n\n"
                  << "-v<arg>,<arg>,<arg> or --test_vertex=<arg>,<arg>,<arg>: "
                  << "3D point to test agains the object; "
                  << "the only REQUIRED argument"
                  << "\n\n"
                  << "-h or --help: displays this prompt"
                  << "\n\n"
                  << "-i or --invisible_light: moves the light out of the scene; "
                  << "by default this is off"
                  << "\n\n"
                  << "-S<arg> or --light_scaling=<arg>: "
                  << "light scaling, requires single float argument; "
                  << "default is 0.05"
                  << "\n\n"
                  << "-P<arg>,<arg>,<arg>, or --light_position=<arg>,<arg>,<arg>: "
                  << "light position, requires 3 float arguments separated by a comma; "
                  << "if invisible light is set, this will be ignored; "
                  << "default is upper right position"
                  << "\n\n"
                  << "-O<arg> or --light_object=<arg>: specifies the file from which "
                  << "the light object will be loaded, default is cube; "
                  << "the object will preserve aspect ratio"
                  << "\n\n"
                  << "-s<arg> or --object_scaling=<arg>: "
                  << "specifies the scaling of the object; "
                  << "default is 0.5"
                  << "\n\n"
                  << "-o<arg> or --object_object=<arg>: "
                  << "specifies the file from which to load the object; "
                  << "default is dragon"
                  << "\n\n"
                  << "-t<arg> or --object_texture=<arg>: "
                  << "specifies the path to texture file for the object; "
                  << "default is no texture"
                  << "\n\n"
                  << "-a or --preserve_aspect_ratio: preserves aspect ratio "
                  << "of the object; default is false; "
                  << "note that some objects might not be drawn properly with this option"
                  << "\n";

      terminate("");
    }

    ::glm::vec3 parse_vec3(char const* s) {
      ::std::stringstream ss(s);
      ::glm::vec3 ret;
      char c;
      ss >> ret.x >> c >> ret.y >> c >> ret.z;
      return ret;
    }

  }

  global_params parse_args(int const argc, char* const* argv) {
    global_params ret;

    constexpr ::option options[]{
      {"help", no_argument, nullptr, 'h'},
      {"invisible_light", no_argument, nullptr, 'i'},
      {"light_scaling", required_argument, nullptr, 'S'},
      {"light_position", required_argument, nullptr, 'P'},
      {"light_object", required_argument, nullptr, 'O'},
      {"object_scaling", required_argument, nullptr, 's'},
      {"object_object", required_argument, nullptr, 'o'},
      {"object_texture", required_argument, nullptr, 't'},
      {"preserve_aspect_ratio", no_argument, nullptr, 'a'},
      {"test_vertex", required_argument, nullptr, 'v'}
    };

    int opt;
    bool test_vertex_set = false;

    while (
      (opt = ::getopt_long(argc, argv, "ihP:S:O:s:o:t:av:", options, nullptr))
        != -1
    )
      switch (opt) {
        case 'h':
          detail::print_help();
          break;
        case 'S':
          ret.light_scaling = ::glm::vec3(::std::stof(optarg));
          break;
        case 'P':
          ret.light_position = detail::parse_vec3(optarg);
          break;
        case 'O':
          ret.light_object = optarg;
          break;
        case 's':
          ret.object_scaling = ::glm::vec3(::std::stof(optarg));
          break;
        case 'o':
          ret.object_object = optarg;
          break;
        case 't':
          ret.object_texture = optarg;
          break;
        case 'a':
          ret.preserve_aspect_ratio = true;
          break;
        case 'v':
          if (optarg == nullptr)
            ::std::cout << "x" << ::std::endl;
          ret.test_vertex = detail::parse_vec3(optarg);
          test_vertex_set = true;
          break;
      }

    if (!test_vertex_set)
      terminate("You must set a test vertex. See --help.");

    if (ret.invisible_light)
      ret.light_position = {5.0f, 5.0f, 5.0f};

    return ret;
  }

}

#define GUARD __FILE__##guard##__LINE__

int main(int const argc, char* const* argv) {
  auto params = ::irg::parse_args(argc, argv);

  auto  GUARD  = ::irg::init();
  auto* window = ::irg::create_window(800, 800);

  ::glfwSetMouseButtonCallback(window, ::irg::simple_mouse_events::click_callback);
  ::glfwSetCursorPosCallback(window, ::irg::simple_mouse_events::move_callback);

  ::glfwSetKeyCallback(window, ::irg::keyboard_events::callback);

  // loading light source

  ::irg::mesh light_source{
    ::irg::mesh::from_file(params.light_object.c_str(),
    {
      ::irg::shader("./4/shaders/light_vertex.glsl", GL_VERTEX_SHADER),
      ::irg::shader("./4/shaders/light_fragment.glsl", GL_FRAGMENT_SHADER)
    })};

  light_source.shader.set_uniform_matrix(
    "model",
    ::glm::scale(
      ::glm::translate(::glm::mat4(1.0f), params.light_position), 
      params.light_scaling));

  // loading given mesh and point

  auto vtx = ::irg::shader("./4/shaders/vertex.glsl", GL_VERTEX_SHADER);
  auto frg = ::irg::shader("./4/shaders/fragment.glsl", GL_FRAGMENT_SHADER);

  ::std::vector<::irg::mesh> objects;

  objects.push_back(
    ::irg::mesh::from_file(
      params.object_object.c_str(), 
      {vtx, frg}, 
      params.preserve_aspect_ratio)
  );

  objects.push_back(::std::move<::irg::mesh>(
    ::irg::mesh::from_file(
      "./4/objects/sphere.obj",
      {vtx, frg}, 
      params.preserve_aspect_ratio)
  ));

  ::std::cout << "Selected point " 
              << params.test_vertex.x << ", "
              << params.test_vertex.y << ", "
              << params.test_vertex.z
              << " is"
              << (objects[0].is_inside(params.test_vertex) ? "" : " not")
              << " inside the object. "
              << "This is true only if the object is convex."
              << "\n";

  ::std::cout << "Use arrows to move around the scene, "
              << "use R or T for rotation. "
              << "Hold left CTRL to rotate only the object. " 
              << "Note that this might ruin the previous conclusion about "
              << "the point being inside the object."
              << "\n";

  for (auto& object : objects)
    object.shader.set_uniform_vec3("light_position", params.light_position);

  objects[0].shader.transform_matrix(
    "model",
    ::glm::scale(::glm::mat4(1.0f), params.object_scaling));

  ::glm::vec3 point_scaling(0.01f);

  objects[1].shader.transform_matrix( //
    "model",
    ::glm::translate(
      ::glm::scale(::glm::mat4(1.0f), point_scaling), 
      (params.test_vertex / point_scaling) * params.object_scaling));

  if (params.object_texture.size()) {
    ::irg::texture tex{params.object_texture.c_str()};
    tex.use();
    for (auto& object : objects)
      object.shader.set_uniform_int("texture_present", 1);  
  }

  // object rotation

  auto s = ::std::sin(-M_PI_2);
  auto c = ::std::cos(-M_PI_2);

  ::glm::mat3 const r90{{c, s, 0}, {-s, c, 0}, {0, 0, 1}};

  bool tracking = false;
  auto* cursor = ::glfwCreateStandardCursor(GLFW_CROSSHAIR_CURSOR);
  ::irg::on_scope_exit cursor_guard{
    [&cursor]{
      ::glfwDestroyCursor(cursor);
    }
  };

  bool tracking_object = false;
  ::glm::vec3 direction{0.0f, 0.0f, 0.0f};

  ::irg::k_events.add_listener([&](auto const key, bool released) {
    if (key == GLFW_KEY_LEFT_CONTROL)
      tracking_object = !tracking_object;

    ::std::unordered_set<unsigned> keys{
      GLFW_KEY_UP, GLFW_KEY_DOWN, GLFW_KEY_LEFT, 
      GLFW_KEY_RIGHT, GLFW_KEY_R, GLFW_KEY_T};

    if (!keys.count(key))
      return ::irg::ob::action::remain;

    if (key == GLFW_KEY_UP)
      direction.y += released ? -1.0f : 1.0f;
    if (key == GLFW_KEY_DOWN)
      direction.y += released ? 1.0f : -1.0f;
    if (key == GLFW_KEY_LEFT)
      direction.x += released ? 1.0f : -1.0f;
    if (key == GLFW_KEY_RIGHT)
      direction.x += released ? -1.0f : 1.0f;
    if (key == GLFW_KEY_R)
      direction.z += released ? -1.0f : 1.0f;
    if (key == GLFW_KEY_T)
      direction.z += released ? 1.0f : -1.0f;

    return ::irg::ob::action::remain;
  });

  ::irg::w_events.add_listener([&](auto const w, auto const h) {
    float ratio = w / static_cast<float>(h);
    ::glm::vec3 scale(1.0f);
    if (ratio >= 1.0f)
      scale.x = 1 / ratio;
    else
      scale.y = ratio;

    auto mat = ::glm::scale(::glm::mat4(1.0f), scale);

    for (auto& object : objects)
      object.shader.set_uniform_matrix("screen_ratio", mat);

    light_source.shader.set_uniform_matrix("screen_ratio", mat);

    return ::irg::ob::action::remain;
  });

  glEnable(GL_DEPTH_TEST);
  ::glm::vec3 counters(0.0f);

  ::irg::window_loop(window, [&] {
    glClearColor(0.0f, 0.0f, 0.0f, 1.f);
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

    counters[0] += 0.01f;
    counters[1] += 0.02f;
    counters[2] += 0.035f;
    
    ::irg::color c{
      0.5f + ::std::sin(counters[0]) / 2.0f,
      0.5f + ::std::sin(counters[1]) / 2.0f,
      0.5f + ::std::sin(counters[2]) / 2.0f
    };

    if (::glm::length2(direction) > 1e-6) {
      auto mat = ::glm::toMat4(::glm::angleAxis(
        ::glm::radians(0.5f * (tracking_object ? -1.f : 1.f)), 
        ::glm::normalize(r90 * direction * (tracking_object ? -1.f : 1.f))));

      if (tracking_object) {
        objects[0].shader.transform_matrix("model", mat);
      } else {
        for (auto& object : objects)
          object.shader.transform_matrix("view", mat);
        light_source.shader.transform_matrix("view", mat);
      }
    }

    // tex.use();

    light_source.draw();
    light_source.shader.set_uniform_color("light_color", c);

    for (auto& object : objects)
      object.shader.set_uniform_color("light_color", c),
      object.draw();

    ::irg::assert_no_error();
  });
}