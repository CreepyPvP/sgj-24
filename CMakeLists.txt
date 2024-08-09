cmake_minimum_required(VERSION 3.25)

project(
    puzzle
)

set(ROOT_DIR ${PROJECT_SOURCE_DIR})

set(RAYLIB_DIR raylib)
add_subdirectory(${RAYLIB_DIR})


file(GLOB_RECURSE SOURCE_FILES
    ${CMAKE_SOURCE_DIR}/code/*.cpp
)

add_executable(${PROJECT_NAME} 
    ${SOURCE_FILES}
)

target_link_libraries(${PROJECT_NAME} 
	raylib
)
