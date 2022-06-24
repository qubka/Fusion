cmake_minimum_required(VERSION 3.21)
project(game)

add_executable(${PROJECT_NAME} "sandbox_app.cpp")

target_link_libraries(${PROJECT_NAME} PRIVATE engine)

