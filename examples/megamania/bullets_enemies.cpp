#include "bullets_enemies.hpp"

#include <cppitertools/itertools.hpp>

void Bullets_enemies::initializeGL(GLuint program) {
  terminateGL();

  m_program = program;
  m_colorLoc = glGetUniformLocation(m_program, "color");
  m_scaleLoc = glGetUniformLocation(m_program, "scale");
  m_translationLoc = glGetUniformLocation(m_program, "translation");

  m_bullets_enemies.clear();

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

void Bullets_enemies::paintGL() {
  glUseProgram(m_program);

  glBindVertexArray(m_vao);
  glUniform4f(m_colorLoc, 1, 0, 0, 1);
  glUniform1f(m_scaleLoc, m_scale);

  for (auto &bullet_enemy : m_bullets_enemies) {
    glUniform2f(m_translationLoc, bullet_enemy.m_translation.x,
                bullet_enemy.m_translation.y);

    glDrawArrays(GL_TRIANGLE_FAN, 0, 12);
  }

  glBindVertexArray(0);

  glUseProgram(0);
}

void Bullets_enemies::terminateGL() {
  glDeleteBuffers(1, &m_vbo);
  glDeleteVertexArrays(1, &m_vao);
}

void Bullets_enemies::update(Enemies &enemy, const GameData &gameData, float deltaTime) {
  // Creates a bullet for each enemy
  if (gameData.m_state == State::Playing) {
      for (auto &enemy : enemy.m_enemies) {
        // At least 1000 ms must have passed since the last bullet
        if (enemy.m_bulletCoolDownTimer.elapsed() > 1000.0 / 1000.0) {
            enemy.m_bulletCoolDownTimer.restart();

            // Bullets are shot in the direction below the enemy
            auto bulletSpeed{1.0f};

            Bullet_enemy bullet_enemy{.m_dead = false,
                            .m_translation = enemy.m_translation,
                            .m_velocity = glm::vec2(0.0f, -0.75f) * bulletSpeed,
                            .m_color = glm::vec4(1.0f, 0.0f, 0.0f, 1.0f)};
            m_bullets_enemies.push_back(bullet_enemy);
        }
    }
  }
  

  for (auto &bullet_enemy : m_bullets_enemies) {
    // Updates the position of the bullet
    bullet_enemy.m_translation += bullet_enemy.m_velocity * deltaTime;

    // Kill bullet if it goes off screen
    if (bullet_enemy.m_translation.x < -1.1f) bullet_enemy.m_dead = true;
    if (bullet_enemy.m_translation.x > +1.1f) bullet_enemy.m_dead = true;
    if (bullet_enemy.m_translation.y < -1.1f) bullet_enemy.m_dead = true;
  }

  // Remove dead bullets
  m_bullets_enemies.remove_if([](const Bullet_enemy &p) { return p.m_dead; });
}