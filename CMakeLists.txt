cmake_minimum_required(VERSION 3.14)

SET(PROJECT "Oscilloscope")

SET(VERSION_MAJOR "1")
SET(VERSION_MINOR "0")
SET(VERSION "${VERSION_MAJOR}.${VERSION_MINOR}")
project( ${PROJECT} VERSION ${VERSION})

set(CMAKE_CXX_STANDARD 17)

if (MSVC)
    # warning level 4 and all warnings as errors
    add_compile_options(/W4 /WX)
else()
    # lots of warnings and all warnings as errors
    add_compile_options(-Wall -Wextra -pedantic -Werror)
endif()

include_directories(${CMAKE_SOURCE_DIR}/src)
file(GLOB SOURCES "src/*.cpp" "src/*.h")

set(SDL2_DIR "${CMAKE_SOURCE_DIR}/Externals/SDL2")

# Support both 32 and 64 bit builds
if (${CMAKE_SIZEOF_VOID_P} MATCHES 8)
	list(APPEND SDL2_LIB "${SDL2_DIR}/lib/x64/SDL2.lib")
	list(APPEND SDL2_LIB "${SDL2_DIR}/lib/x64/SDL2main.lib")
	set(MY_PATH "PATH=%PATH%" "${SDL2_DIR}/lib/x64")
else ()
	list(APPEND SDL2_LIB "${SDL2_DIR}/lib/x86/SDL2.lib")
	list(APPEND SDL2_LIB "${SDL2_DIR}/lib/x86/SDL2main.lib")
	set(MY_PATH "PATH=%PATH%" "${SDL2_DIR}/lib/x86")
endif ()

include_directories(${SDL2_DIR}/include)

add_executable( ${PROJECT} ${SOURCES} )

target_link_libraries(${PROJECT} ${SDL2_LIB})

set_target_properties(${PROJECT} PROPERTIES VS_DEBUGGER_ENVIRONMENT "${MY_PATH}")

#Extra step to copy resources
file(COPY ${CMAKE_SOURCE_DIR}/Resources DESTINATION ${CMAKE_CURRENT_BINARY_DIR}/${CONFIGURATION})