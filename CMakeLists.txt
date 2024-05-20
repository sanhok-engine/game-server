cmake_minimum_required(VERSION 3.28)
project(sanhok-server
    VERSION 0.1
    LANGUAGES CXX
)

### Dependencies ###
include(FetchContent)
find_package(Boost REQUIRED COMPONENTS system)

FetchContent_Declare(
    flatbuffers
    GIT_REPOSITORY https://github.com/google/flatbuffers.git
    GIT_TAG v24.3.25
)
FetchContent_MakeAvailable(flatbuffers)

FetchContent_Declare(
    spdlog
    GIT_REPOSITORY https://github.com/gabime/spdlog.git
    GIT_TAG v1.14.1
)
FetchContent_MakeAvailable(spdlog)

FetchContent_Declare(
    glm
    GIT_REPOSITORY https://github.com/g-truc/glm.git
    GIT_TAG 1.0.1
)
FetchContent_MakeAvailable(glm)

FetchContent_Declare(
    Catch2
    GIT_REPOSITORY https://github.com/catchorg/Catch2.git
    GIT_TAG v3.5.4
)
FetchContent_MakeAvailable(Catch2)


### Targets ###
add_library(libsanhok STATIC
    sanhok/game/player/player.hpp
    sanhok/game/player/player.cpp
    sanhok/game/player/player_movement.hpp
    sanhok/game/player/player_movement.cpp
    sanhok/game/game_manager.cpp
    sanhok/game/game_manager.hpp
    sanhok/game/client.hpp
    sanhok/game/client.cpp
    sanhok/game/server.hpp
    sanhok/game/server.cpp
    sanhok/game/session.hpp
    sanhok/game/session.cpp
    sanhok/game/zone.hpp
    sanhok/game/zone.cpp

    sanhok/net/listener_tcp.hpp
    sanhok/net/peer_tcp.hpp
    sanhok/net/peer_udp.hpp

    sanhok/bip_buffer.hpp
    sanhok/concurrent_map.hpp
    sanhok/concurrent_queue.hpp
)
add_library(sanhok::libsanhok ALIAS libsanhok)
target_compile_features(libsanhok PUBLIC cxx_std_20)
target_include_directories(libsanhok PUBLIC $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}>)
target_link_libraries(libsanhok PUBLIC
    Boost::system
    flatbuffers
    spdlog::spdlog
    glm::glm)

# compile schemas
file(GLOB schemas RELATIVE ${CMAKE_CURRENT_SOURCE_DIR} sanhok/game/protocol/schemas/*.fbs)
add_custom_target(
    libsanhok-compile-schemas
    WORKING_DIRECTORY ${CMAKE_CURRENT_SOURCE_DIR}
    COMMAND
    flatc
    --cpp
    --cpp-std c++17
    --filename-suffix "\"\""
    --filename-ext hpp
    -o sanhok/game/protocol
    ${schemas}
    DEPENDS flatc
)
add_dependencies(libsanhok libsanhok-compile-schemas)

add_executable(sanhok-server sanhok/game/app.cpp)
target_compile_features(sanhok-server PRIVATE cxx_std_20)
target_link_libraries(sanhok-server PRIVATE sanhok::libsanhok)

# examples
add_subdirectory(examples/dummy-client)


### Tests ###
add_executable(libsanhok-tests
    sanhok/concurrent_map.test.cpp
    sanhok/concurrent_queue.test.cpp
)
target_compile_features(libsanhok-tests PRIVATE cxx_std_20)
target_link_libraries(libsanhok-tests PRIVATE sanhok::libsanhok Catch2::Catch2WithMain)
