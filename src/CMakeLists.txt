add_subdirectory (RenderBackends)
add_subdirectory (RenderFrontends)
target_sources (${PROJECT_NAME} PRIVATE ${CMAKE_CURRENT_SOURCE_DIR}/main.cpp
                                        ${CMAKE_CURRENT_SOURCE_DIR}/Timer.cpp)
