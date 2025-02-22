#include "openglwindow.hpp"

#include <imgui.h>

#include "abcg.hpp"

void OpenGLWindow::handleEvent(SDL_Event &event) {
  // Keyboard events
  if (event.type == SDL_KEYDOWN) {
    if (event.key.keysym.sym == SDLK_SPACE)
      m_gameData.m_input.set(static_cast<size_t>(Input::Fire));
    if (event.key.keysym.sym == SDLK_LEFT || event.key.keysym.sym == SDLK_a)
      m_gameData.m_input.set(static_cast<size_t>(Input::Left));
    if (event.key.keysym.sym == SDLK_RIGHT || event.key.keysym.sym == SDLK_d)
      m_gameData.m_input.set(static_cast<size_t>(Input::Right));
  }
  if (event.type == SDL_KEYUP) {
    if (event.key.keysym.sym == SDLK_SPACE)
      m_gameData.m_input.reset(static_cast<size_t>(Input::Fire));
    if (event.key.keysym.sym == SDLK_LEFT || event.key.keysym.sym == SDLK_a)
      m_gameData.m_input.reset(static_cast<size_t>(Input::Left));
    if (event.key.keysym.sym == SDLK_RIGHT || event.key.keysym.sym == SDLK_d)
      m_gameData.m_input.reset(static_cast<size_t>(Input::Right));
  }

  // Mouse events
  if (event.type == SDL_MOUSEBUTTONDOWN) {
    if (event.button.button == SDL_BUTTON_LEFT)
      m_gameData.m_input.set(static_cast<size_t>(Input::Fire));
  }
  if (event.type == SDL_MOUSEBUTTONUP) {
    if (event.button.button == SDL_BUTTON_LEFT)
      m_gameData.m_input.reset(static_cast<size_t>(Input::Fire));
  }
}

void OpenGLWindow::initializeGL() {
  // Load a new font
  ImGuiIO &io{ImGui::GetIO()};
  auto filename{getAssetsPath() + "Inconsolata-Medium.ttf"};
  m_font = io.Fonts->AddFontFromFileTTF(filename.c_str(), 60.0f);
  if (m_font == nullptr) {
    throw abcg::Exception{abcg::Exception::Runtime("Cannot load font file")};
  }

  // Create program to render the other objects
  m_objectsProgram = createProgramFromFile(getAssetsPath() + "objects.vert",
                                           getAssetsPath() + "objects.frag");

  glClearColor(0, 0, 0, 1);

#if !defined(__EMSCRIPTEN__)
  glEnable(GL_PROGRAM_POINT_SIZE);
#endif

  // Create program to render the stars
  m_starsProgram = createProgramFromFile(getAssetsPath() + "stars.vert",
                                       getAssetsPath() + "stars.frag");

  restart();
}

void OpenGLWindow::restart() {
  m_gameData.m_state = State::Playing;

  m_starLayers.initializeGL(m_starsProgram, 25);
  m_ship.initializeGL(m_objectsProgram);
  m_energy.initializeGL(m_objectsProgram, 15);
  m_enemies.initializeGL(m_objectsProgram, 4);
  m_bullets_ship.initializeGL(m_objectsProgram);
  m_bullets_enemies.initializeGL(m_objectsProgram);
}

void OpenGLWindow::update() {
  float deltaTime{static_cast<float>(getDeltaTime())};

  // Wait 5 seconds before restarting
  if (m_gameData.m_state != State::Playing &&
      m_restartWaitTimer.elapsed() > 5) {
    restart();
    return;
  }

  m_ship.update(m_gameData, deltaTime);
  m_starLayers.update(deltaTime);
  m_enemies.update(deltaTime);
  m_energy.update(m_gameData);
  m_bullets_ship.update(m_ship, m_gameData, deltaTime);
  m_bullets_enemies.update(m_enemies, m_gameData, deltaTime);

  if (m_gameData.m_state == State::Playing) {
    checkCollisions();
    checkEnergy();
    checkWinCondition();
  }
}

void OpenGLWindow::paintGL() {
  update();

  glClear(GL_COLOR_BUFFER_BIT);
  glViewport(0, 0, m_viewportWidth, m_viewportHeight);

  m_starLayers.paintGL();
  m_enemies.paintGL();
  m_bullets_ship.paintGL();
  m_bullets_enemies.paintGL();
  m_energy.paintGL();
  m_ship.paintGL(m_gameData);  
}

void OpenGLWindow::paintUI() {
  abcg::OpenGLWindow::paintUI();

  {
    auto size{ImVec2(300, 85)};
    auto position{ImVec2((m_viewportWidth - size.x) / 2.0f,
                         (m_viewportHeight - size.y) / 2.0f)};
    ImGui::SetNextWindowPos(position);
    ImGui::SetNextWindowSize(size);
    ImGuiWindowFlags flags{ImGuiWindowFlags_NoBackground |
                           ImGuiWindowFlags_NoTitleBar |
                           ImGuiWindowFlags_NoInputs};
    ImGui::Begin(" ", nullptr, flags);
    ImGui::PushFont(m_font);

    if (m_gameData.m_state == State::GameOver) {
      ImGui::Text("Game Over!");
    } else if (m_gameData.m_state == State::Win) {
      ImGui::Text("*You Win!*");
    }

    ImGui::PopFont();
    ImGui::End();
  }
}

void OpenGLWindow::resizeGL(int width, int height) {
  m_viewportWidth = width;
  m_viewportHeight = height;

  glClear(GL_COLOR_BUFFER_BIT);
}

void OpenGLWindow::terminateGL() {
  glDeleteProgram(m_starsProgram);
  glDeleteProgram(m_objectsProgram);

  m_enemies.terminateGL();
  m_energy.terminateGL();
  m_bullets_ship.terminateGL();
  m_bullets_enemies.terminateGL(); 
  m_ship.terminateGL();
  m_starLayers.terminateGL();
}


void OpenGLWindow::checkCollisions() {

  // Check collision between bullets_enemies and ship
  for (auto &bullet_enemy : m_bullets_enemies.m_bullets_enemies) {
    if (bullet_enemy.m_dead) continue;    
    
      for (auto i : {-2, 0, 2}) {
        for (auto j : {-2, 0, 2}) {
          auto enemyTranslation{m_ship.m_translation + glm::vec2(i, j)};
          auto distance{
              glm::distance(bullet_enemy.m_translation, enemyTranslation)};

          if (distance < m_bullets_enemies.m_scale + m_ship.m_scale * 0.85f) {
            m_gameData.m_state = State::GameOver;
            m_restartWaitTimer.restart();
          }
        }
      }
  }

  // Check collision between bullets_ship and enemies
  for (auto &bullet_ship : m_bullets_ship.m_bullets_ship) {
    if (bullet_ship.m_dead) continue;
    
    for (auto &enemy : m_enemies.m_enemies) {
      for (auto i : {-2, 0, 2}) {
        for (auto j : {-2, 0, 2}) {
          auto enemyTranslation{enemy.m_translation + glm::vec2(i, j)};
          auto distance{
              glm::distance(bullet_ship.m_translation, enemyTranslation)};

          if (distance < m_bullets_ship.m_scale + enemy.m_scale * 0.85f) {
            enemy.m_hit = true;
            bullet_ship.m_dead = true;
          }
        }
      }
    }

    // Break enemies marked as hit
    for (auto &enemy : m_enemies.m_enemies) {
      if (enemy.m_hit && enemy.m_scale == 0.15f) {

        glm::vec2 offset{+2.0f, +1.5f};

        // Generates three minor enemies
        m_enemies.m_enemies.push_back(m_enemies.createEnemy(enemy.m_translation + offset * enemy.m_scale * 0.5f,
                                            enemy.m_scale * 0.5f));
        
        m_enemies.m_enemies.push_back(m_enemies.createEnemy(enemy.m_translation - offset * enemy.m_scale * 0.5f,
                                            enemy.m_scale * 0.5f));

        m_enemies.m_enemies.push_back(m_enemies.createEnemy(enemy.m_translation,
                                            enemy.m_scale * 0.5f));
      }
    }

    m_enemies.m_enemies.remove_if(
        [](const Enemies::Enemy &a) { return a.m_hit; });   
      
  }
}

void OpenGLWindow::checkEnergy() {
  if (m_energy.value < 0) {
    m_gameData.m_state = State::GameOver;
    m_restartWaitTimer.restart();
  }
}

void OpenGLWindow::checkWinCondition() {
  if (m_enemies.m_enemies.empty()) {
    m_gameData.m_state = State::Win;
    m_restartWaitTimer.restart();
  }
}