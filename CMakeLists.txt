# ==> Ensure proper CMake support

cmake_minimum_required(VERSION 3.10)

project(gl VERSION 0.1)

# ==> Force new C++ standard (we like new features here)

set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

# ==> Enable as much optimization as possible
set(CMAKE_CXX_FLAGS_RELEASE "${CMAKE_CXX_FLAGS_RELEASE} -D NDEBUG -O2")

# ==> Flags for debugging
set(CMAKE_CXX_FLAGS_DEBUG "${CMAKE_CXX_FLAGS_DEBUG} -D _DEBUG -ggdb3 -Wall -Wextra -Weffc++ -Wcast-align -Wcast-qual -Wchar-subscripts -Wctor-dtor-privacy -Wempty-body -Wfloat-equal -Wformat-nonliteral -Wformat-security -Wformat=2 -Winline -Wnon-virtual-dtor -Woverloaded-virtual -Wpacked -Wpointer-arith -Wredundant-decls -Wsign-promo -Wstrict-overflow=2 -Wsuggest-override -Wswitch-default -Wswitch-enum -Wundef -Wunreachable-code -Wunused -Wvariadic-macros -Wno-missing-field-initializers -Wno-narrowing -Wno-old-style-cast -Wno-varargs -fcheck-new -fsized-deallocation -fstack-check -fstack-protector -fstrict-overflow -fno-omit-frame-pointer -Og")

if ("${CMAKE_CXX_COMPILER_ID}" STREQUAL "GNU")
  set(CMAKE_CXX_FLAGS_DEBUG "${CMAKE_CXX_FLAGS_DEBUG} -Waggressive-loop-optimizations -Wconditionally-supported -Wformat-signedness -Wlogical-op -Wopenmp-simd -Wstrict-null-sentinel -Wsuggest-attribute=noreturn -Wsuggest-final-methods -Wsuggest-final-types -Wsync-nand -Wuseless-cast -fconcepts-diagnostics-depth=3 -Wno-literal-suffix")
endif ()

add_library(gl STATIC
    # Wrappers
    src/wrappers/proxy/opengl-error-handler.cpp

    src/wrappers/objects/vertex-array.cpp
    src/wrappers/objects/uniforms.cpp
    src/wrappers/objects/vertex-buffer.cpp

    src/wrappers/setup/opengl-setup.cpp

    # Extensions
    src/extensions/storage/vertex-layout.cpp

    src/extensions/renderer/renderer.cpp
    src/extensions/renderer/renderer-handler-window.cpp

    src/extensions/simple-drawer/drawing-manager.cpp

    src/math/shapes/circle.cpp
    src/math/shapes/line.cpp)

target_include_directories(gl PUBLIC # TODO: fix includes (just include include/)
    ${CMAKE_CURRENT_SOURCE_DIR}/include/gl/

    # Wrappers
    ${CMAKE_CURRENT_SOURCE_DIR}/include/gl/wrappers/objects/
    ${CMAKE_CURRENT_SOURCE_DIR}/include/gl/wrappers/proxy/
    ${CMAKE_CURRENT_SOURCE_DIR}/include/gl/wrappers/setup/

    # Extensions
    ${CMAKE_CURRENT_SOURCE_DIR}/include/gl/extensions/renderer/
    ${CMAKE_CURRENT_SOURCE_DIR}/include/gl/extensions/simple-drawer/
    ${CMAKE_CURRENT_SOURCE_DIR}/include/gl/extensions/storage/

    # Math
    ${CMAKE_CURRENT_SOURCE_DIR}/include/gl/math/
    ${CMAKE_CURRENT_SOURCE_DIR}/include/gl/math/shapes/)

# Generate OpenGL wrappers

add_subdirectory(include/gl/wrappers/proxy)

option(GL_BUILD_EXAMPLES "Build examples from 'examples' folder" FALSE)
if (${GL_BUILD_EXAMPLES})
  add_subdirectory(examples)
endif ()

# Copy runtime dependencies (TODO: improve)

file(COPY res DESTINATION "${CMAKE_BINARY_DIR}")

# Link OpenGL, GLEW and GLFW with gl

set(OpenGL_GL_PREFERENCE GLVND)

find_package(GLEW   REQUIRED)
find_package(glfw3  REQUIRED)
find_package(OpenGL REQUIRED)

target_include_directories(
    gl PUBLIC
    ${OPENGL_INCLUDE_DIRS} ${GLEW_INCLUDE_DIRS})

target_link_libraries(gl PUBLIC ${OPENGL_LIBRARIES} ${GLEW_LIBRARIES} glfw)
