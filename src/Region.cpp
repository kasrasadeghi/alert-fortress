#include "Region.h"

bool Region::structureAt(glm::ivec2 cell) {
  return _structure_pos_set[cell.x][cell.y];
}

void Region::addStructure(glm::ivec2 cell) {
  _structure_pos_set[cell.x][cell.y] = 1;
}