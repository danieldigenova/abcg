#ifndef OPENGLWINDOW_HPP_
#define OPENGLWINDOW_HPP_

#include <imgui.h>

#include "abcg.hpp"
#include "enemies.hpp"
#include "bullets_ship.hpp"
#include "bullets_enemies.hpp"
#include "ship.hpp"
#include "energy.hpp"
#include "starlayers.hpp"

class OpenGLWindow : public abcg::OpenGLWindow {
 protected:
  void handleEvent(SDL_Event& event) override;
  void initializeGL() override;
  void paintGL() override;
  void paintUI() override;
  void resizeGL(int width, int height) override;
  void terminateGL() override;
  void checkCollisions();
  void checkEnergy();
  void checkWinCondition();

 private:
  GLuint m_starsProgram{};
  GLuint m_objectsProgram{};

  int m_viewportWidth{};
  int m_viewportHeight{};

  GameData m_gameData;
  
  Enemies m_enemies;
  Bullets_ship m_bullets_ship;
  Bullets_enemies m_bullets_enemies;
  Ship m_ship;
  Energy m_energy;
  StarLayers m_starLayers;

  abcg::ElapsedTimer m_restartWaitTimer;

  ImFont* m_font{};

  std::default_random_engine m_randomEngine;

  void restart();
  void update();
};

#endif