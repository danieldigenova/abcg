#ifndef OPENGLWINDOW_HPP_
#define OPENGLWINDOW_HPP_

#include <random>

#include "abcg.hpp"
#include "model.hpp"

class OpenGLWindow : public abcg::OpenGLWindow {
 protected:
  void initializeGL() override;
  void paintGL() override;
  void paintUI() override;
  void resizeGL(int width, int height) override;
  void terminateGL() override;

 private:
  static const int m_numBlocks{200};

  GLuint m_program{};

  int m_viewportWidth{};
  int m_viewportHeight{};

  std::default_random_engine m_randomEngine;

  Model m_model;

  std::array<glm::vec3, m_numBlocks> m_blockPositions;
  std::array<glm::vec3, m_numBlocks> m_blockRotations;
  std::array<float, m_numBlocks> m_theta;
  std::array<float, m_numBlocks> m_direction;
  
  glm::vec2 center{0};
  float radius{4.0f};
  float velocityZ{5.0f};

  float color_rate = 0.7f;
  int color_up_down = 1;

  glm::mat4 m_viewMatrix{1.0f};
  glm::mat4 m_projMatrix{1.0f};
  float m_FOV{40.0f};

  void randomizeBlock(glm::vec3 &position, glm::vec3 &rotation, float &theta, float &direction);
  void update();
};

#endif