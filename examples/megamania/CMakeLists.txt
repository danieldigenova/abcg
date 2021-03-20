project(megamania)
add_executable(${PROJECT_NAME} main.cpp openglwindow.cpp enemies.cpp
                               bullets_ship.cpp bullets_enemies.cpp ship.cpp energy.cpp starlayers.cpp)
enable_abcg(${PROJECT_NAME})