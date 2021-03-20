#ifndef SHIP_HPP_
#define SHIP_HPP_

#include "abcg.hpp"
#include "gamedata.hpp"

class Enemies;
class Bullets_ship;
class OpenGLWindow;
class StarLayers;

class Ship {
 public:
  void initializeGL(GLuint program);
  void paintGL(const GameData &gameData);
  void terminateGL();

  void update(const GameData &gameData, float deltaTime);

 private:
  friend Enemies;
  friend Bullets_ship;
  friend OpenGLWindow;
  friend StarLayers;

  GLuint m_program{};
  GLint m_translationLoc{};
  GLint m_colorLoc{};
  GLint m_scaleLoc{};

  GLuint m_vao{};
  GLuint m_vbo{};
  GLuint m_ebo{};

  glm::vec4 m_color{1};
  float m_scale{0.125f};
  glm::vec2 m_translation{glm::vec2(0)};

  abcg::ElapsedTimer m_trailBlinkTimer;
  abcg::ElapsedTimer m_bulletCoolDownTimer;
};

#endif