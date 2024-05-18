add_executable(dummy-client
    skymarlin/game/dummy/app.cpp
    skymarlin/game/dummy/dummy_client.cpp
    skymarlin/game/dummy/player.cpp
)
target_compile_features(dummy-client PRIVATE cxx_std_20)
target_include_directories(dummy-client
    PRIVATE
    $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/>
)
target_link_libraries(dummy-client PRIVATE sanhok::libgame)