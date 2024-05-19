add_executable(dummy-client
    sanhok/game/dummy/app.cpp
    sanhok/game/dummy/dummy_client.hpp
    sanhok/game/dummy/dummy_client.cpp
    sanhok/game/dummy/dummy_player.hpp
    sanhok/game/dummy/dummy_player.cpp
)
target_compile_features(dummy-client PRIVATE cxx_std_20)
target_include_directories(dummy-client PRIVATE $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/>)
target_link_libraries(dummy-client PRIVATE sanhok::libgame)