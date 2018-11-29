#pragma once

#include "ECS/Manager.h"

#include "Components.h"

#include "Config.h"
#include "Graphics.h"

class World;

class Unit {
  glm::vec2 _target;

public:
  const ECS::Entity _id;

  constexpr static float unit_size = 0.5f * tile_size;
  constexpr static float unit_speed = 2.f;
  constexpr static HealthValue health = 150;
  constexpr static StrengthValue strength = 25;

  Unit(glm::vec2 pos, World&);

  glm::vec2 pos() const;
  bool selected() const;
  std::vector<glm::ivec2>& path() const;
  glm::ivec2 currentTarget() const;

  static void holo(View& view, glm::vec2 curr) {
    CircleBatch().add()
      .position(curr)
      .size({tile_size, tile_size})
      .color({1, 0.5, 0.5, 0.5})
      .draw(view);
  }
};
