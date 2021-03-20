#ifndef ENERGY_HPP_
#define ENERGY_HPP_

#include <list>

#include "abcg.hpp"
#include "gamedata.hpp"

class OpenGLWindow;

class Energy {
 public:
  void initializeGL(GLuint program, int quantity);
  void paintGL();
  void terminateGL();

  void update(const GameData &gameData);

 private:
  friend OpenGLWindow;

  GLuint m_program{};
  GLint m_colorLoc{};
  GLint m_translationLoc{};
  GLint m_scaleLoc{};

  struct Bar {
    GLuint m_vao{};
    GLuint m_vbo{};

    glm::vec4 m_color{1};
    float m_scale{};
    glm::vec2 m_translation{};
  };

  abcg::ElapsedTimer energyTimer;

  std::list<Bar> m_energy;
  int value = 15;

  Energy::Bar createBar(glm::vec2 translation = glm::vec2(0),
                                     float scale = 0.15f);
};

#endif