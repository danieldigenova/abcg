#include "energy.hpp"

#include <cppitertools/itertools.hpp>
#include <glm/gtx/fast_trigonometry.hpp>

void Energy::initializeGL(GLuint program, int quantity) {
  terminateGL();

  m_program = program;
  m_colorLoc = glGetUniformLocation(m_program, "color");
  m_scaleLoc = glGetUniformLocation(m_program, "scale");
  m_translationLoc = glGetUniformLocation(m_program, "translation");

  // Create enegy bar
  m_energy.clear();
  m_energy.resize(quantity);

  int i = 1;
  for (auto &energy : m_energy) {
    energy = createBar();
    energy.m_translation.x = -0.55;
	  energy.m_translation.y = -0.8f;
    energy.m_translation.x = energy.m_translation.x + 0.075f*i;
    i++;
  }
}

void Energy::paintGL() {
  glUseProgram(m_program);

  for (auto &energy : m_energy) {
    glBindVertexArray(energy.m_vao);

    glUniform4fv(m_colorLoc, 1, &energy.m_color.r);
    glUniform1f(m_scaleLoc, energy.m_scale);

    for (auto i : {-2, 0, 2}) {
      for (auto j : {-2, 0, 2}) {
        glUniform2f(m_translationLoc, energy.m_translation.x + j,
                    energy.m_translation.y + i);

        glDrawArrays(GL_TRIANGLE_FAN, 0, 4);
      }
    }

    glBindVertexArray(0);
  }

  glUseProgram(0);
}

void Energy::terminateGL() {
  for (auto energy : m_energy) {
    glDeleteBuffers(1, &energy.m_vbo);
    glDeleteVertexArrays(1, &energy.m_vao);
  }

  // Restore the energy value
  value = 15;
}

void Energy::update(const GameData &gameData) {
  // At least 1500 ms must have passed since the last decrease in the energy value
  if (energyTimer.elapsed() > 1500.0 / 1000.0 && 
      gameData.m_state == State::Playing) {
    energyTimer.restart();
    int i = value;
    int j = 0;
    for (auto &energy : m_energy){
      if(j >= i){
        // Colors a part of the bar in red
        energy.m_color = glm::vec4(1.0f, 0.0f, 0.0f, 0.0f);
      }
      j++;
    }
    // Decreases energy value
    value = value - 1;
  }
}

Energy::Bar Energy::createBar(glm::vec2 translation, float scale) {
  // Creates one of the n parts of the energy bar
  Bar energy;

  energy.m_color = glm::vec4(0.0f, 0.0f, 1.00f, 1.0f);
  energy.m_scale = scale;
  energy.m_translation = translation;

  // Create geometry
  std::vector<glm::vec2> positions(0);
  positions.emplace_back(-0.5f, 0.0f);
  positions.emplace_back(-0.5f, -0.5f);
  positions.emplace_back(0.0f, -0.5f);

  // Generate VBO
  glGenBuffers(1, &energy.m_vbo);
  glBindBuffer(GL_ARRAY_BUFFER, energy.m_vbo);
  glBufferData(GL_ARRAY_BUFFER, positions.size() * sizeof(glm::vec2),
               positions.data(), GL_STATIC_DRAW);
  glBindBuffer(GL_ARRAY_BUFFER, 0);

  // Get location of attributes in the program
  GLint positionAttribute{glGetAttribLocation(m_program, "inPosition")};

  // Create VAO
  glGenVertexArrays(1, &energy.m_vao);

  // Bind vertex attributes to current VAO
  glBindVertexArray(energy.m_vao);

  glBindBuffer(GL_ARRAY_BUFFER, energy.m_vbo);
  glEnableVertexAttribArray(positionAttribute);
  glVertexAttribPointer(positionAttribute, 2, GL_FLOAT, GL_FALSE, 0, nullptr);
  glBindBuffer(GL_ARRAY_BUFFER, 0);

  // End of binding to current VAO
  glBindVertexArray(0);

  return energy;
}