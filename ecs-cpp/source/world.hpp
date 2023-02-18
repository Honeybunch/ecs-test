#pragma once

class World {
public:
  World();
  ~World();

  bool tick(float delta_seconds);
};
