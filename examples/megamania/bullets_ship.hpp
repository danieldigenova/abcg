#ifndef BULLETS_SHIP_HPP_
#define BULLETS_SHIP_HPP_

#include <list>

#include "abcg.hpp"
#include "gamedata.hpp"
#include "ship.hpp"

class OpenGLWindow;

class Bullets_ship {
 public:
  void initializeGL(GLuint program);
  void paintGL();
  void terminateGL();

  void update(Ship &ship, const GameData &gameData, float deltaTime);

 private:
  friend OpenGLWindow;

  GLuint m_program{};
  GLint m_colorLoc{};
  GLint m_translationLoc{};
  GLint m_scaleLoc{};

  GLuint m_vao{};
  GLuint m_vbo{};

  struct Bullet_ship {
    bool m_dead{false};
    glm::vec2 m_translation{glm::vec2(0)};
    glm::vec2 m_velocity{glm::vec2(0)};
    glm::vec4 m_color{1};
  };

  float m_scale{0.015f};

  std::list<Bullet_ship> m_bullets_ship;
};

#endif