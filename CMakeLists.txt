cmake_minimum_required(VERSION 3.20)

project(z_level_chunked_raycaster LANGUAGES C)

include(FetchContent)

if(EMSCRIPTEN)
  set(raycaster_fetch_sdl_default ON)
else()
  set(raycaster_fetch_sdl_default OFF)
endif()

option(
  RAYCASTER_FETCH_SDL
  "Download and build SDL3 instead of using a system package"
  ${raycaster_fetch_sdl_default}
)
option(
  RAYCASTER_WEB_PRELOAD_ASSETS
  "Package the assets and manifests directories into the web build"
  OFF
)

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

if(EMSCRIPTEN)
  set_target_properties(raycaster PROPERTIES SUFFIX ".html")
  set_property(
    TARGET raycaster
    APPEND PROPERTY LINK_DEPENDS "${CMAKE_CURRENT_SOURCE_DIR}/web/shell.html"
  )

  target_link_options(raycaster PRIVATE
    "--shell-file=${CMAKE_CURRENT_SOURCE_DIR}/web/shell.html"
    "-sALLOW_MEMORY_GROWTH=1"
    "-sENVIRONMENT=web"
  )

  if(RAYCASTER_WEB_PRELOAD_ASSETS)
    target_link_options(raycaster PRIVATE
      "--preload-file=${CMAKE_CURRENT_SOURCE_DIR}/assets@/assets"
      "--preload-file=${CMAKE_CURRENT_SOURCE_DIR}/manifests@/manifests"
    )
  endif()
endif()
