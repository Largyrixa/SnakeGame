cmake_minimum_required(VERSION 3.10)

project(JogoCobra VERSION 1.0)

set(CMAKE_CXX_STANDART 20)
set(CMAKE_CXX_SANDART_REQUIRED True)

set(CMAKE_RUNTIME_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/bin)

add_executable(treinar
src/treinar.cpp
src/jogo.cpp
src/bot_genetico.cpp
src/rede_neural.cpp
src/treinador.cpp)

add_executable(bot_play
src/bot_play.cpp
src/bot_genetico.cpp
src/jogo.cpp
src/rede_neural.cpp)

add_executable(play
src/play.cpp
src/jogo.cpp)

target_include_directories(treinar PUBLIC "include")
target_link_libraries(treinar PRIVATE ncurses)
target_include_directories(bot_play PUBLIC "include")
target_link_libraries(bot_play PRIVATE ncurses)
target_include_directories(play PUBLIC "include")
target_link_libraries(play PRIVATE ncurses)
