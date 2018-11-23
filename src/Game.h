#pragma once

#include "Graphics.h"
// #include <SFML/Graphics.hpp>
#include <iostream>
#include <sstream>

#include "World.h"

enum class ControlMode { NONE, BUILD, UNIT, TERRAIN };

class Game {
  // sf::Font _font;
  World _world;
  RenderWindow _window;
  View _view;
  // sf::Clock _clock;
  // Tile _paint = Tile::GRASS;

  ControlMode _mode = ControlMode::NONE;

  //   template <typename T>
  //   sf::Text makeText(T obj) {
  //     std::stringstream ss;
  //     ss << obj;
  //     sf::Text t;
  //     t.setFont(_font);
  //     t.setString(ss.str());
  //     t.setCharacterSize(24);
  //     t.setFillColor(sf::Color::Black);

  //     return t;
  //   }

  //   void _mouseViewMove(float d) {
  //     constexpr int margin = 20;
  //     auto pos = sf::Mouse::getPosition();
  //     if (pos.x < margin) { _view.move(-d, 0); }
  //     if (pos.y < margin) { _view.move(0, -d); }

  //     if (pos.x > _window.getSize().x - margin) { _view.move(d, 0); }
  //     if (pos.y > _window.getSize().y - margin) { _view.move(0, d); }
  //   }

    void _keyboardViewMove(float d) {
      if (_window.getKey(GLFW_KEY_W) == GLFW_PRESS) { _view.move(0, -d); }
      if (_window.getKey(GLFW_KEY_S) == GLFW_PRESS) { _view.move(0, d); }
      if (_window.getKey(GLFW_KEY_A) == GLFW_PRESS) { _view.move(-d, 0); }
      if (_window.getKey(GLFW_KEY_D) == GLFW_PRESS) { _view.move(d, 0); }
      // if (_mode == ControlMode::TERRAIN) {
      //   if (sf::Mouse::isButtonPressed(sf::Mouse::Button::Left)) {
      //     _world.setCell(getMouseTile(), _paint);
      //   }
      // }
    }

  //   void _reboundViewToWorld() {
  //     auto topLeft = _view.getCenter() - (_view.getSize() / 2.f);
  //     auto bottomRight = _view.getCenter() + (_view.getSize() / 2.f);

  //     const auto viewRadius = (view_size * tile_size) / 2.f;
  //     const auto worldBorder = World::world_size * tile_size;

  //     if (topLeft.x < 0) {
  //       _view.setCenter(viewRadius * widthScalingFactor(),
  //       _view.getCenter().y);
  //     }
  //     if (topLeft.y < 0) { _view.setCenter(_view.getCenter().x, viewRadius);
  //     } if (bottomRight.x > worldBorder) {
  //       _view.setCenter(worldBorder - (viewRadius * widthScalingFactor()),
  //                       _view.getCenter().y);
  //     }
  //     if (bottomRight.y > worldBorder) {
  //       _view.setCenter(_view.getCenter().x, worldBorder - viewRadius);
  //     }
  //   }

public:
    constexpr static float view_size = 20 * tile_size;

  Game();

    glm::vec2 getMouseCoords() {
      return pixelToCoords(_window.mousePos());
    }
    
    glm::vec2 pixelToCoords(glm::vec2 p) {
      // coords = V' W v
      // W : window -> opengl
      // V : game   -> opengl so V' : opengl -> game
      return  _view.inv() * _window.defaultView().proj() * glm::vec4(p.x, p.y, 0, 1);
    }

    static glm::vec<2, int> mapCoordsToTile(glm::vec2 coords) {
      return glm::vec<2, int>(static_cast<int>(coords.x / tile_size),
                              static_cast<int>(coords.y / tile_size));
    }

    glm::vec<2, int> getMouseTile() {
      return mapCoordsToTile(getMouseCoords());
    }

  void loop();

  void keyCallback(int key, int scancode, int action, int mods);
  void mouseCallback(int button, int action, int mods);
  // void handleEvent(const sf::Event& event);

  // void handleViewInput(const sf::Time& dt);
  void handleTick(float dt);
};
