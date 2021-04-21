#ifndef OPENGLWINDOW_HPP_
#define OPENGLWINDOW_HPP_

#include <string_view>
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

  //float color_rate = 0.7f;
  //int color_up_down = 1;

  glm::mat4 m_modelMatrix{1.0f};
  glm::mat4 m_viewMatrix{1.0f};
  glm::mat4 m_projMatrix{1.0f};
  float m_FOV{40.0f};

  // Mapping mode
  // 0: triplanar; 1: cylindrical; 2: spherical; 3: from mesh
  int m_mappingMode = 3;

  // Light and material properties
  //glm::vec4 m_lightDir{0.0f, 0.0f, 0.0f, 0.0f};
  glm::vec4 m_Ia{1.0f};
  glm::vec4 m_Id{1.0f};
  glm::vec4 m_Is{1.0f};
  glm::vec4 m_Ka{};
  glm::vec4 m_Kd{};
  glm::vec4 m_Ks{};
  glm::vec3 m_K = {0.5f, 0.0005f, 0.0075f};
  float m_shininess{};

  void loadModel(std::string_view path);
  void randomizeBlock(glm::vec3 &position, glm::vec3 &rotation, float &theta, float &direction);
  void update();
};

#endif