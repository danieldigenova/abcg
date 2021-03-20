#include "bullets_ship.hpp"

#include <cppitertools/itertools.hpp>

void Bullets_ship::initializeGL(GLuint program) {
  terminateGL();

  m_program = program;
  m_colorLoc = glGetUniformLocation(m_program, "color");
  m_scaleLoc = glGetUniformLocation(m_program, "scale");
  m_translationLoc = glGetUniformLocation(m_program, "translation");

  m_bullets_ship.clear();

  // Create regular polygon
  auto sides{10};

  std::vector<glm::vec2> positions(0);
  positions.emplace_back(0, 0);
  auto step{M_PI * 2 / sides};
  for (auto angle : iter::range(0.0, M_PI * 2, step)) {
    positions.emplace_back(std::cos(angle), std::sin(angle));
  }
  positions.push_back(positions.at(1));

  // Generate VBO of positions
  glGenBuffers(1, &m_vbo);
  glBindBuffer(GL_ARRAY_BUFFER, m_vbo);
  glBufferData(GL_ARRAY_BUFFER, positions.size() * sizeof(glm::vec2),
               positions.data(), GL_STATIC_DRAW);
  glBindBuffer(GL_ARRAY_BUFFER, 0);

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

  // End of binding to current VAO
  glBindVertexArray(0);
}

void Bullets_ship::paintGL() {
  glUseProgram(m_program);

  glBindVertexArray(m_vao);
  glUniform4f(m_colorLoc, 1, 1, 1, 1);
  glUniform1f(m_scaleLoc, m_scale);

  for (auto &bullet_ship : m_bullets_ship) {
    glUniform2f(m_translationLoc, bullet_ship.m_translation.x,
                bullet_ship.m_translation.y);

    glDrawArrays(GL_TRIANGLE_FAN, 0, 12);
  }

  glBindVertexArray(0);

  glUseProgram(0);
}

void Bullets_ship::terminateGL() {
  glDeleteBuffers(1, &m_vbo);
  glDeleteVertexArrays(1, &m_vao);
}

void Bullets_ship::update(Ship &ship, const GameData &gameData, float deltaTime) {
  // Create a bullet
  if (gameData.m_input[static_cast<size_t>(Input::Fire)] &&
      gameData.m_state == State::Playing) {
    // At least 500 ms must have passed since the last bullet
    if (ship.m_bulletCoolDownTimer.elapsed() > 500.0 / 1000.0) {
      ship.m_bulletCoolDownTimer.restart();

      // Bullets are shot in the forward direction of the ship
      auto bulletSpeed{2.0f};

      Bullet_ship bullet_ship{.m_dead = false,
                    .m_translation = ship.m_translation,
                    .m_velocity = glm::vec2(0.0f, 1.00f) * bulletSpeed,
                    .m_color = glm::vec4(1.0f, 1.0f, 1.0f, 1.0f)};
      m_bullets_ship.push_back(bullet_ship);
    }
  }

  for (auto &bullet_ship : m_bullets_ship) {
    // Updates the position of the bullet
    bullet_ship.m_translation += bullet_ship.m_velocity * deltaTime;

    // Kill bullet if it goes off screen
    if (bullet_ship.m_translation.x < -1.1f) bullet_ship.m_dead = true;
    if (bullet_ship.m_translation.x > +1.1f) bullet_ship.m_dead = true;
    if (bullet_ship.m_translation.y > +1.1f) bullet_ship.m_dead = true;
  }

  // Remove dead bullets
  m_bullets_ship.remove_if([](const Bullet_ship &p) { return p.m_dead; });
}