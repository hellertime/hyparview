cmake_minimum_required(VERSION 3.3)
project(HYPARVIEW VERSION "0.0.1" LANGUAGES CXX C)

# smf + seastar build options
set(SMF_ENABLE_TESTS OFF CACHE BOOL "disable smf tests")
set(SMF_BUILD_PROGRAMS OFF CACHE BOOL "disable smf demo programs")
set(SEASTAR_ENABLE_TESTS OFF CACHE BOOL "disable seastar tests")

# subdirectories
add_subdirectory(smf)
add_subdirectory(hyparviewd)
