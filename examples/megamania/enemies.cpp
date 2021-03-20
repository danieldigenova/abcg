#include "enemies.hpp"

#include <cppitertools/itertools.hpp>
#include <glm/gtx/fast_trigonometry.hpp>

void Enemies::initializeGL(GLuint program, int quantity) {
  terminateGL();

  m_program = program;
  m_colorLoc = glGetUniformLocation(m_program, "color");
  m_scaleLoc = glGetUniformLocation(m_program, "scale");
  m_translationLoc = glGetUniformLocation(m_program, "translation");

  // Create enemies
  m_enemies.clear();
  m_enemies.resize(quantity);
  
  // Translates the starting position of the enemies
  int i = 2;
  for (auto &enemy : m_enemies) {
    enemy = createEnemy();
	  enemy.m_translation.y = 0.7f;
    enemy.m_translation.x = 0.5*i;
    i++;
  }
}

void Enemies::paintGL() {
  glUseProgram(m_program);

  for (auto &enemy : m_enemies) {
    glBindVertexArray(enemy.m_vao);

    glUniform4fv(m_colorLoc, 1, &enemy.m_color.r);
    glUniform1f(m_scaleLoc, enemy.m_scale);

    for (auto i : {-2, 0, 2}) {
      for (auto j : {-2, 0, 2}) {
        glUniform2f(m_translationLoc, enemy.m_translation.x + j,
                    enemy.m_translation.y + i);

         glDrawElements(GL_TRIANGLES, 19 * 3, GL_UNSIGNED_INT, nullptr);
      }
    }

    glBindVertexArray(0);
  }

  glUseProgram(0);
}

void Enemies::terminateGL() {
  for (auto enemy : m_enemies) {
    glDeleteBuffers(1, &enemy.m_vbo);
    glDeleteBuffers(1, &enemy.m_ebo);
    glDeleteVertexArrays(1, &enemy.m_vao);
  }
}

void Enemies::update(float deltaTime) {
  for (auto &enemy : m_enemies) {
    enemy.m_translation.x += 0.5f * deltaTime;

    // Wrap-around
    if (enemy.m_translation.x > +1.0f) enemy.m_translation.x -= 2.0f;
  }
}

Enemies::Enemy Enemies::createEnemy(glm::vec2 translation,
                                              float scale) {
  Enemy enemy;

  // Green color
  enemy.m_color = glm::vec4(0.5f, 1.0f, 0.31f, 1.0f);

  enemy.m_scale = scale;
  enemy.m_translation = translation;

  // Create geometry
  std::array<glm::vec2, 22> positions{
      glm::vec2{+00.0f, +00.0f}, glm::vec2{-02.5f, +00.0f},
      glm::vec2{-02.5f, +02.5f}, glm::vec2{+00.0f, +02.5f},
      glm::vec2{+02.5f, +02.5f}, glm::vec2{+02.5f, +00.0f},
      glm::vec2{+09.5f, +10.5f}, glm::vec2{+12.5f, +02.5f},
      glm::vec2{+12.5f, -02.5f}, glm::vec2{+09.5f, -02.5f},
      glm::vec2{+09.5f, +02.5f}, glm::vec2{+07.5f, +10.5f},
      glm::vec2{+07.5f, +12.5f}, glm::vec2{+09.5f, +12.5f},
      glm::vec2{-09.5f, +10.5f}, glm::vec2{-09.5f, +12.5f},
      glm::vec2{-07.5f, +12.5f}, glm::vec2{-07.5f, +10.5f},
      glm::vec2{-09.5f, +02.5f}, glm::vec2{-12.5f, +02.5f},
      glm::vec2{-12.5f, -02.5f}, glm::vec2{-09.5f, -02.5f},
  };
  
  // Normalize
  for (auto &position :positions) {
    position /= glm::vec2{15.5f, 15.5f};
  }

  std::array indices{0, 1, 2,
                     1, 2, 3,
                     2, 3, 0,
                     3, 5, 4,
                     5, 4, 3, 
                     5, 0, 3,
                     4, 6, 7,
                     7, 8, 9,
                     8, 9, 10,
                     9, 10, 7,
                     6, 11, 12,
                     6, 12, 13,
                     6, 4, 14, 
                     14, 15, 16,
                     14, 16, 17,
                     4, 14, 18,
                     14, 18, 19,
                     19, 20, 21,
                     21, 19, 18};

  // Generate VBO
  glGenBuffers(1, &enemy.m_vbo);
  glBindBuffer(GL_ARRAY_BUFFER, enemy.m_vbo);
  glBufferData(GL_ARRAY_BUFFER, sizeof(positions), positions.data(),
               GL_STATIC_DRAW);
  glBindBuffer(GL_ARRAY_BUFFER, 0);

  // Generate EBO
  glGenBuffers(1, &enemy.m_ebo);
  glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, enemy.m_ebo);
  glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(indices), indices.data(),
               GL_STATIC_DRAW);
  glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, 0);

  // Get location of attributes in the program
  GLint positionAttribute{glGetAttribLocation(m_program, "inPosition")};

  // Create VAO
  glGenVertexArrays(1, &enemy.m_vao);

  // Bind vertex attributes to current VAO
  glBindVertexArray(enemy.m_vao);

  glEnableVertexAttribArray(positionAttribute);
  glBindBuffer(GL_ARRAY_BUFFER, enemy.m_vbo);
  glVertexAttribPointer(positionAttribute, 2, GL_FLOAT, GL_FALSE, 0, nullptr);
  glBindBuffer(GL_ARRAY_BUFFER, 0);

  glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, enemy.m_ebo);

  // End of binding to current VAO
  glBindVertexArray(0);

  return enemy;
}