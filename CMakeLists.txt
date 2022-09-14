project(magician)
cmake_minimum_required(VERSION 3.16)

add_library(delegate INTERFACE delegate.hpp)
target_compile_features(delegate INTERFACE cxx_std_17)

add_library(traits INTERFACE traits.hpp)
target_compile_features(traits INTERFACE cxx_std_17)