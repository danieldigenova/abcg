#ifndef BULLETS_ENEMIES_HPP_
#define BULLETS_ENEMIES_HPP_

#include <list>

#include "abcg.hpp"
#include "gamedata.hpp"
#include "enemies.hpp"

class OpenGLWindow;

class Bullets_enemies {
 public:
  void initializeGL(GLuint program);
  void paintGL();
  void terminateGL();

  void update(Enemies &enemy, const GameData &gameData, float deltaTime);

 private:
  friend OpenGLWindow;

  GLuint m_program{};
  GLint m_colorLoc{};
  GLint m_translationLoc{};
  GLint m_scaleLoc{};

  GLuint m_vao{};
  GLuint m_vbo{};

  struct Bullet_enemy {
    bool m_dead{false};
    glm::vec2 m_translation{glm::vec2(0)};
    glm::vec2 m_velocity{glm::vec2(0)};
    glm::vec4 m_color{1};
  };

  float m_scale{0.015f};

  std::list<Bullet_enemy> m_bullets_enemies;
};

#endif