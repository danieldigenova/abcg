#ifndef ENEMIES_HPP_
#define ENEMIES_HPP_

#include <list>

#include "abcg.hpp"
#include "gamedata.hpp"

class OpenGLWindow;

class Enemies {
 public:
  void initializeGL(GLuint program, int quantity);
  void paintGL();
  void terminateGL();

  void update(float deltaTime);

  struct Enemy {
    GLuint m_vao{};
    GLuint m_vbo{};
    GLuint m_ebo{};

    glm::vec4 m_color{1};
    bool m_hit{false};
    float m_scale{};
    glm::vec2 m_translation{};

    abcg::ElapsedTimer m_bulletCoolDownTimer;
  };

  std::list<Enemy> m_enemies;

 private:
  friend OpenGLWindow;

  GLuint m_program{};
  GLint m_colorLoc{};
  GLint m_translationLoc{};
  GLint m_scaleLoc{};

  Enemies::Enemy createEnemy(glm::vec2 translation = glm::vec2(0),
                                     float scale = 0.15f);
};

#endif