#include "ship.hpp"

#include <glm/gtx/fast_trigonometry.hpp>

void Ship::initializeGL(GLuint program) {
  terminateGL();

  m_program = program;
  m_colorLoc = glGetUniformLocation(m_program, "color");
  m_scaleLoc = glGetUniformLocation(m_program, "scale");
  m_translationLoc = glGetUniformLocation(m_program, "translation");

  m_translation = glm::vec2(0, -0.6f);

  std::array<glm::vec2, 14> positions{
    // Ship body
    glm::vec2{+00.0f, +15.5f}, glm::vec2{-07.5f, +07.5f},
    glm::vec2{+07.5f, +07.5f}, glm::vec2{-07.5f, +00.0f},
    glm::vec2{+07.5f, +00.0f}, glm::vec2{+00.0f, -09.5f},
    glm::vec2{-12.5f, -12.5f}, glm::vec2{+12.5f, -12.5f},

    // Thruster trail
    glm::vec2{-07.5f, -09.5f}, 
    glm::vec2{+00.0f, -19.0f}, 
    glm::vec2{+07.5f, -09.5f},
  };

  // Normalize
  for (auto &position :positions) {
    position /= glm::vec2{15.5f, 15.5f};
  }

  std::array indices{0, 1, 2,
                     1, 2, 3,
                     2, 3, 4,
                     3, 4, 5,
                     3, 5, 6,
                     4, 5, 7,
                     8, 9, 10,
                     11, 12, 13};

  // Generate VBO
  glGenBuffers(1, &m_vbo);
  glBindBuffer(GL_ARRAY_BUFFER, m_vbo);
  glBufferData(GL_ARRAY_BUFFER, sizeof(positions), positions.data(),
               GL_STATIC_DRAW);
  glBindBuffer(GL_ARRAY_BUFFER, 0);

  // Generate EBO
  glGenBuffers(1, &m_ebo);
  glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, m_ebo);
  glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(indices), indices.data(),
               GL_STATIC_DRAW);
  glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, 0);

  // Get location of attributes in the program
  GLint positionAttribute{glGetAttribLocation(m_program, "inPosition")};

  // Create VAO
  glGenVertexArrays(1, &m_vao);

  // Bind vertex attributes to current VAO
  glBindVertexArray(m_vao);

  glEnableVertexAttribArray(positionAttribute);
  glBindBuffer(GL_ARRAY_BUFFER, m_vbo);
  glVertexAttribPointer(positionAttribute, 2, GL_FLOAT, GL_FALSE, 0, nullptr);
  glBindBuffer(GL_ARRAY_BUFFER, 0);

  glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, m_ebo);

  // End of binding to current VAO
  glBindVertexArray(0);
}

void Ship::paintGL(const GameData &gameData) {
  if (gameData.m_state != State::Playing) return;

  glUseProgram(m_program);

  glBindVertexArray(m_vao);

  glUniform1f(m_scaleLoc, m_scale);
  glUniform2fv(m_translationLoc, 1, &m_translation.x);

  // Restart thruster blink timer every 100 ms
  if (m_trailBlinkTimer.elapsed() > 100.0 / 1000.0) m_trailBlinkTimer.restart();
  
  // Show thruster trail during 50 ms
  if (m_trailBlinkTimer.elapsed() < 50.0 / 1000.0) {
    glEnable(GL_BLEND);
    glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);

    // 50% transparent
    glUniform4f(m_colorLoc, 1.0f, 0.0f, 0.0f, 0.5f);

    glDrawElements(GL_TRIANGLES, 8 * 3, GL_UNSIGNED_INT, nullptr);

    glDisable(GL_BLEND);
  }

  glUniform4fv(m_colorLoc, 1, &m_color.r);
  glDrawElements(GL_TRIANGLES, 6 * 3, GL_UNSIGNED_INT, nullptr);

  glBindVertexArray(0);

  glUseProgram(0);
}

void Ship::terminateGL() {
  glDeleteBuffers(1, &m_vbo);
  glDeleteBuffers(1, &m_ebo);
  glDeleteVertexArrays(1, &m_vao);
}

void Ship::update(const GameData &gameData, float deltaTime) {
  // Translate the ship
  if (gameData.m_input[static_cast<size_t>(Input::Left)])
    m_translation.x -= 0.3f * deltaTime;
  if (gameData.m_input[static_cast<size_t>(Input::Right)])
    m_translation.x += 0.3f * deltaTime;

  if (m_translation.x < -1.0f) m_translation.x += 2.0f;
  if (m_translation.x > +1.0f) m_translation.x -= 2.0f;
}