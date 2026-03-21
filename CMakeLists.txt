add_library(memorygame_e2e_framework
    src/GameDriver.cpp
    src/Interaction.cpp
)

target_include_directories(memorygame_e2e_framework
    PUBLIC
        ${CMAKE_CURRENT_SOURCE_DIR}/include
)

target_link_libraries(memorygame_e2e_framework
    PUBLIC
        Engine
        MemoryGameCore
)
