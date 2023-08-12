cmake_minimum_required(VERSION 3.15.0 FATAL_ERROR)

# Define the project name and language
project(crow_crash LANGUAGES CXX)

add_executable(crow_crash main.cpp)
target_include_directories(crow_crash PUBLIC asio/include)
