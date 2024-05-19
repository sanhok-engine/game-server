cmake_minimum_required(VERSION 3.14)
project(game-server
    VERSION 0.1
    LANGUAGES CXX
)

### Dependencies ###
include(FetchContent)

FetchContent_Declare(
    libnet
    GIT_REPOSITORY https://github.com/sanhokkk/libnet.git
    GIT_TAG main
)
FetchContent_MakeAvailable(libnet)

FetchContent_Declare(
    glm
    GIT_REPOSITORY https://github.com/g-truc/glm.git
    GIT_TAG 1.0.1
)
FetchContent_MakeAvailable(glm)


### Targets ###
add_library(libgame STATIC
    sanhok/game/player/player.hpp
    sanhok/game/player/player.cpp
    sanhok/game/player/player_movement.hpp
    sanhok/game/player/player_movement.cpp
    sanhok/game/client.hpp
    sanhok/game/client.cpp
    sanhok/game/server.hpp
    sanhok/game/server.cpp
    sanhok/game/session.hpp
    sanhok/game/session.cpp
    sanhok/game/zone.hpp
    sanhok/game/zone.cpp
)
add_library(sanhok::libgame ALIAS libgame)
target_compile_features(libgame PUBLIC cxx_std_20)
target_include_directories(libgame
    PUBLIC $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}>
)
target_link_libraries(libgame PUBLIC sanhok::libnet glm::glm)

file(GLOB schemas RELATIVE ${CMAKE_CURRENT_SOURCE_DIR} sanhok/game/protocol/schemas/*.fbs)
skymarlin_compile_schemas(libgame "" sanhok/game/protocol "${schemas}")

add_executable(game-server sanhok/game/app.cpp)
target_compile_features(game-server PRIVATE cxx_std_20)
target_link_libraries(game-server PRIVATE sanhok::libgame)

# examples
add_subdirectory(examples/dummy-client)



