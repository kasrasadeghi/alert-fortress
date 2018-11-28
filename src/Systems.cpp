#include "Systems.h"
#include "Game.h"

std::vector<glm::ivec2> MoveSystem::findPath(std::vector<std::vector<Tile>>& region, glm::ivec2 start, glm::ivec2 end) {
  using P = glm::ivec2;

  std::deque<P> alive;
  std::deque<P> dead;
  alive.push_back(start);

  auto valid = [&region](P p) -> bool {
    auto bounds = p.x >= 0 && p.y >= 0 && p.x < World::world_size && p.y < World::world_size;
    return bounds && region[p.x][p.y] == Tile::GRASS;
  };

  auto seen = [&alive, &dead](P p) -> bool {
    return std::find(alive.begin(), alive.end(), p) != alive.end() ||
          std::find(dead.begin(), dead.end(), p) != dead.end();
  };

  constexpr auto ws = World::world_size;

  std::vector<std::vector<P>> backtrace(ws, std::vector(ws, P(-1, -1)));

  while (not alive.empty()) {
    auto curr = alive.front();
    alive.pop_front();

    if (curr == end) {
      break;
    }
    
    std::vector<P> neighbors = {curr + P(0, 1), curr + P(0, -1), curr + P(1, 0), curr + P(-1, 0)};

    // get valid neighbors
    for (auto& n : neighbors) {
      if (valid(n) && not seen(n)) {
        backtrace[n.x][n.y] = curr;
        alive.push_back(n);
      }
    }
    dead.push_back(curr);
  }

  std::vector<P> trace;
  auto curr = end;
  while (curr != start) {
    trace.push_back(curr);
    if (not valid(curr)) {
      std::puts("");
      break;
    }
    curr = backtrace[curr.x][curr.y];
  }

  std::reverse(trace.begin(), trace.end());

  return trace;
}

void MoveSystem::updateEntity(float dt, ECS::Entity entity) {
  auto& pos = ECS::Manager::getComponent<TransformComponent>(entity).pos;
  MotionComponent& motion = ECS::Manager::getComponent<MotionComponent>(entity);

  if (not motion.hasTarget) return;

  auto& region = motion.world.region();

  if (motion.path.empty()) {
    glm::ivec2 curr_cell = Game::mapCoordsToTile(pos);
    glm::ivec2 target_cell = Game::mapCoordsToTile(motion.target);

    motion.path = findPath(region, curr_cell, target_cell);
  }

  auto updatePosition = [dt, &pos, &motion](glm::vec2 target) {
    if (glm::distance(target, pos) > dt * motion.movementSpeed) {
      auto dir = glm::normalize(target - pos);
      pos += dir * dt * motion.movementSpeed;
      motion.currentTarget = target;
    } else {
      pos = target;
      motion.hasTarget = false;
      motion.path.clear();
    }
  };

  auto sees = [&region, pos](glm::vec2 target) -> bool {
    auto path = target - pos;
    auto l = glm::length(path);
    auto dir = normalize(path);
    for (float k = 0; k < l; k += 0.2 /* <- precision */) {
      glm::ivec2 p = Game::mapCoordsToTile(pos + dir * k);
      if (region[p.x][p.y] != Tile::GRASS) {
        return false;
      }
    }
    return true;
  };

  auto next = motion.path[0];
  for (auto p : motion.path) {
    if (sees(p)) {
      next = p;
    } else {
      break;
    }
  }
  auto target = Game::centerOfTile(next);
  
  updatePosition(target);
}