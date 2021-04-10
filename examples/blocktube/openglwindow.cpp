#include "openglwindow.hpp"

#include <imgui.h>
#include <math.h>

#include <cppitertools/itertools.hpp>
#include <glm/gtx/fast_trigonometry.hpp>

void OpenGLWindow::initializeGL() {
  glClearColor(0, 0, 0, 1);

  // Enable depth buffering
  glEnable(GL_DEPTH_TEST);

  // Create program
  m_program = createProgramFromFile(getAssetsPath() + "depth.vert",
                                    getAssetsPath() + "depth.frag");

  // Load model
  m_model.loadFromFile(getAssetsPath() + "block.obj");

  m_model.setupVAO(m_program);

  // Camera at (0,0,0) and looking towards the negative z
  m_viewMatrix =
      glm::lookAt(glm::vec3(0.0f, 0.0f, 0.0f), glm::vec3(0.0f, 0.0f, -1.0f),
                  glm::vec3(0.0f, 1.0f, 0.0f));

  // Setup blocks
  for (const auto index : iter::range(m_numBlocks)) {
    auto &position{m_blockPositions.at(index)};
    auto &rotation{m_blockRotations.at(index)};
    auto &theta{m_theta.at(index)};
    auto &direction{m_direction.at(index)};

    randomizeBlock(position, rotation, theta, direction);
  }
}

void OpenGLWindow::randomizeBlock(glm::vec3 &position, glm::vec3 &rotation, float &theta, float &direction) {
  // Get random position
  // z coordinates in the range [-100, 0]
  // theta angle in the range [0, 360]
  std::uniform_real_distribution<float> distPosZ(-100.0f, 0.0f);
  std::uniform_real_distribution<float> distTheta(0, 360.0f);
  theta = distTheta(m_randomEngine);

  // Calculates the x and y positions according to the theta angle
  position.x = center.x + radius * cos(glm::radians(theta));
  position.y = center.y + radius * sin(glm::radians(theta));
  position.z = distPosZ(m_randomEngine);

  // Set the rotation of the object in the z component
  rotation = glm::vec3(0, 0, 1.0f); 

  // Get a random direction of rotation
  std::uniform_real_distribution<float> distDir(-1.0f, 1.0f);
  direction = distDir(m_randomEngine);
}

void OpenGLWindow::paintGL() {
  update();

  // Clear color buffer and depth buffer
  glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

  glViewport(0, 0, m_viewportWidth, m_viewportHeight);

  glUseProgram(m_program);

  // Get location of uniform variables (could be precomputed)
  GLint viewMatrixLoc{glGetUniformLocation(m_program, "viewMatrix")};
  GLint projMatrixLoc{glGetUniformLocation(m_program, "projMatrix")};
  GLint modelMatrixLoc{glGetUniformLocation(m_program, "modelMatrix")};
  GLint colorLoc{glGetUniformLocation(m_program, "color")};

  // Set uniform variables used by every scene object
  glUniformMatrix4fv(viewMatrixLoc, 1, GL_FALSE, &m_viewMatrix[0][0]);
  glUniformMatrix4fv(projMatrixLoc, 1, GL_FALSE, &m_projMatrix[0][0]);
  glUniform4f(colorLoc, color_rate, 0.3f, 0.0f, 1.0f);

  // Render each block
  for (const auto index : iter::range(m_numBlocks)) {
    auto &position{m_blockPositions.at(index)};
    auto &rotation{m_blockRotations.at(index)};
    auto &theta{m_theta.at(index)};

    // Compute model matrix of the current block
    glm::mat4 modelMatrix{2.0f};
    modelMatrix = glm::translate(modelMatrix, position);
    modelMatrix = glm::scale(modelMatrix, glm::vec3(2.5f));
    // Rotate the block according to the theta angle
    modelMatrix = glm::rotate(modelMatrix, glm::radians(theta), rotation);

    // Set uniform variable
    glUniformMatrix4fv(modelMatrixLoc, 1, GL_FALSE, &modelMatrix[0][0]);

    m_model.render();
  }

  glUseProgram(0);
}

void OpenGLWindow::paintUI() {
  abcg::OpenGLWindow::paintUI();

  {
    auto widgetSize{ImVec2(250, 116)};
    ImGui::SetNextWindowPos(ImVec2(m_viewportWidth - widgetSize.x - 5, 5));
    ImGui::SetNextWindowSize(widgetSize);
    ImGui::Begin("Widget window", nullptr, ImGuiWindowFlags_NoDecoration);

    {
      ImGui::PushItemWidth(120);
      static std::size_t currentIndex{};
      std::vector<std::string> comboItems{"Perspective", "Orthographic"};

      if (ImGui::BeginCombo("Projection",
                            comboItems.at(currentIndex).c_str())) {
        for (auto index : iter::range(comboItems.size())) {
          const bool isSelected{currentIndex == index};
          if (ImGui::Selectable(comboItems.at(index).c_str(), isSelected))
            currentIndex = index;
          if (isSelected) ImGui::SetItemDefaultFocus();
        }
        ImGui::EndCombo();
      }
      ImGui::PopItemWidth();

      ImGui::PushItemWidth(170);
      auto aspect{static_cast<float>(m_viewportWidth) /
                  static_cast<float>(m_viewportHeight)};
      if (currentIndex == 0) {
        m_projMatrix = glm::perspective(glm::radians(m_FOV), aspect, 0.01f, 100.0f);
        // Control of FOV value
        ImGui::SliderFloat("FOV", &m_FOV, 10.0f, 179.0f, "%.0f degrees");
        // Control of radius value
        ImGui::SliderFloat("radius", &radius, 2.0f, 7.0f, "%.0f units");
        // Control of the speed value of the z component
        ImGui::SliderFloat("velocity", &velocityZ, 1.0f, 15.0f, "%.0f units");
      } else {
        m_projMatrix = glm::ortho(-20.0f * aspect, 20.0f * aspect, -20.0f,
                                  20.0f, 0.01f, 100.0f);
        // Control of radius value                     
        ImGui::SliderFloat("radius", &radius, 2.0f, 7.0f, "%.0f units");
        // Control of the speed value of the z component
        ImGui::SliderFloat("velocity", &velocityZ, 1.0f, 15.0f, "%.0f units");
      }
      ImGui::PopItemWidth();
    }

    ImGui::End();
  }
}

void OpenGLWindow::resizeGL(int width, int height) {
  m_viewportWidth = width;
  m_viewportHeight = height;
}

void OpenGLWindow::terminateGL() { glDeleteProgram(m_program); }

void OpenGLWindow::update() {
  
  float deltaTime{static_cast<float>(getDeltaTime())};

  // Changes the color of the block according to the color rate
  if (color_rate <= 0.00f) {
    color_up_down = 0;
  } else if (color_rate >= 1.0f) {
    color_up_down = 1;
  }

  if (color_up_down) {
    color_rate -= 0.02f * deltaTime;
  } else {
    color_rate += 0.02f * deltaTime;
  }

  // Update blocks
  for (const auto index : iter::range(m_numBlocks)) {
    auto &position{m_blockPositions.at(index)};
    auto &rotation{m_blockRotations.at(index)};
    auto &theta{m_theta.at(index)};
    auto &direction{m_direction.at(index)};

    // The block position in z increases velocityZ units per second
    position.z += deltaTime * velocityZ;

    // The theta angle increases 20 degrees per second
    theta += 20.0f * deltaTime * direction;

    // Calculates the x and y positions according to the theta angle
    position.x = center.x + radius * cos(glm::radians(theta));
    position.y = center.y + radius * sin(glm::radians(theta));

    // If this block is now behind the camera, select a new random position and
    // orientation, and move it back to -100
    if (position.z > 0.1f) {
      randomizeBlock(position, rotation, theta, direction);
      position.z = -100.0f;  // Back to -100
    }
  }
}