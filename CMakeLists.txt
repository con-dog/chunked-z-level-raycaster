cmake_minimum_required(VERSION 3.20)

project(z_level_chunked_raycaster LANGUAGES C)

include(FetchContent)

option(RAYCASTER_FETCH_SDL "Download and build SDL3 instead of using a system package" OFF)

if(RAYCASTER_FETCH_SDL)
  set(SDL_SHARED OFF CACHE BOOL "" FORCE)
  set(SDL_STATIC ON CACHE BOOL "" FORCE)
  set(SDL_TEST_LIBRARY OFF CACHE BOOL "" FORCE)
  set(SDL_TESTS OFF CACHE BOOL "" FORCE)
  set(SDL_EXAMPLES OFF CACHE BOOL "" FORCE)

  FetchContent_Declare(
    SDL3
    GIT_REPOSITORY https://github.com/libsdl-org/SDL.git
    GIT_TAG release-3.4.8
    GIT_SHALLOW TRUE
  )
  FetchContent_MakeAvailable(SDL3)
else()
  find_package(SDL3 CONFIG REQUIRED)
endif()

add_executable(raycaster
  main.c
  config/sdl/sdl.c
  utils/math-utils.c
)

target_compile_features(raycaster PRIVATE c_std_11)
target_compile_options(raycaster PRIVATE
  $<$<C_COMPILER_ID:Clang,GNU>:-Wall;-Wextra>
)
target_link_libraries(raycaster PRIVATE SDL3::SDL3)

if(UNIX AND NOT APPLE AND NOT EMSCRIPTEN)
  target_link_libraries(raycaster PRIVATE m)
endif()
