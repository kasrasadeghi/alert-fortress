#include "Game.h"
#include "Config.h"
#include "Graphics.h"

#include <glm/gtc/matrix_transform.hpp>

#define GLM_ENABLE_EXPERIMENTAL
#include <glm/gtx/string_cast.hpp>

#include <iostream>
#include <sstream>
#include <vector>

Game::Game() : _world(World::world_size), _window("Fortress Commander") {
  _window.setKeyCallback([this](auto&&... args) { keyCallback(args...); });
  _window.setMouseCallback([this](auto&&... args) { mouseCallback(args...); });
  // glfwSwapInterval(1);
  _view.center(view_size / 2.f * _window.widthScalingFactor(), view_size / 2.f)
      .radius(view_size / 2.f * _window.widthScalingFactor(), view_size / 2.f);
  // if (!_font.loadFromFile("arial.ttf")) { exit(1); }
}

template <typename T>
std::string str(T obj) {
  std::stringstream ss;
  ss << obj;
  return ss.str();
}

void Game::loop() {
  float last_time = glfwGetTime();
  TextRenderer t(_window.defaultView());

  while (_window.isOpen()) {
    glClearColor(0.2f, 0.3f, 0.3f, 1.0f);
    glClear(GL_COLOR_BUFFER_BIT);

    float dt = glfwGetTime() - last_time;
    last_time = glfwGetTime();

    for (Unit& u : _world._units) {
      u.update(dt);
    }

    handleTick(dt);

    _world.draw(_view, _selected_units);

    t.renderText(str(1.f / dt), 100, 50, 1, glm::vec3(0, 0, 0));

    if (_mode == ControlMode::BUILD || _mode == ControlMode::TERRAIN) {
      World::tileHolo(_view, getMouseTile());
    }
    if (_mode == ControlMode::UNIT) {
      Unit::holo(_view, getMouseCoords());
    }

    _window.swapBuffers();
    glfwPollEvents();
  }
}

void Game::keyCallback(int key, int scancode, int action, int mods) {
  if (key == GLFW_KEY_ESCAPE && action == GLFW_PRESS) {
    _window.close();
  }

  // TODO: temporary. normally left clicking on empty ground gets you out of a mode
  if (key == GLFW_KEY_Q && action == GLFW_PRESS) {
    _mode = ControlMode::NONE;
  }

  if (key == GLFW_KEY_B && action == GLFW_PRESS) {
    _mode = ControlMode::BUILD;
  }
  if (key == GLFW_KEY_U && action == GLFW_PRESS) {
    _mode = ControlMode::UNIT;
  }
  if (key == GLFW_KEY_T && action == GLFW_PRESS) {
    _mode = ControlMode::TERRAIN;
  }
}

void Game::mouseCallback(int button, int action, int mods) {
  // if (button == GLFW_MOUSE_BUTTON_1 && action == GLFW_PRESS) {
  //   auto p = getMouseTile();
  //   std::cout << p.x << ", " << p.y << std::endl;
  // }
  constexpr float select_threshhold = tile_size;
  static glm::vec2 select_start {-1, -1};
  
  
  if (action == GLFW_PRESS) {
    switch (_mode) {
    case ControlMode::NONE:
      if (button == GLFW_MOUSE_BUTTON_2) {
        // right click paths units
        for (uint id : _selected_units) {
          _world._units[id].pathTo(getMouseCoords());
        }

      } else if (button == GLFW_MOUSE_BUTTON_1) {
        // left click selects units
        select_start = getMouseCoords();
      }
      break;
    case ControlMode::BUILD:
      // TODO:_world.addStructure(getMouseTile());
      break;
    case ControlMode::TERRAIN:
      _paint = _world.flipCell(getMouseTile());
      break;
    case ControlMode::UNIT:
      // TODO: check if the add unit is in bounds
      _world._units.push_back(Unit(getMouseCoords()));
      break;
    }
  }

  if (action == GLFW_RELEASE && _mode == ControlMode::NONE && button == GLFW_MOUSE_BUTTON_1) {
    _selected_units.clear();
    glm::vec2 select_end = getMouseCoords();
    if (glm::distance(select_start, select_end) < select_threshhold) {
      // handle clicking on a unit
      auto optionalUnitID = _world.unitAt(select_end);
      if (optionalUnitID) {
        _selected_units.push_back(*optionalUnitID);
      }
    } else {
      // handle dragging a box
      for (uint id : _world.unitsIn(select_start, select_end)) {
        _selected_units.push_back(id);
      }
    }
  }

  //   if (_mode == ControlMode::TERRAIN && event.type == sf::Event::MouseMoved &&
  //       sf::Mouse::isButtonPressed(sf::Mouse::Button::Left)) {
  //     _world.setCell(getMouseTile(), _paint);
  //   }
}

void Game::handleTick(float dt) {
  constexpr float speed = 20 * tile_size;
  float d = speed * dt;

  _mouseViewMove(d);

  _keyboardViewMove(d);

  // TODO: move to cursor move
  if (_mode == ControlMode::TERRAIN) {
    if (glfwGetMouseButton(_window.window(), GLFW_MOUSE_BUTTON_1) == GLFW_PRESS) {
      _world.setCell(getMouseTile(), _paint);
    }
  }

  // lock view to world by rebounding
  _reboundViewToWorld();
}
