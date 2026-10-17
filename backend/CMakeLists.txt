cmake_minimum_required(VERSION 3.24)
project(bcc323_backend VERSION 0.1.0 LANGUAGES CXX)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_EXTENSIONS OFF)

include(FetchContent)
find_path(ASIO_INCLUDE_DIR asio.hpp)
if(NOT ASIO_INCLUDE_DIR)
    FetchContent_Declare(asio
        GIT_REPOSITORY https://github.com/chriskohlhoff/asio.git
        GIT_TAG asio-1-30-2
        GIT_SHALLOW TRUE
    )
    FetchContent_MakeAvailable(asio)
    set(ASIO_INCLUDE_DIR "${asio_SOURCE_DIR}/asio/include" CACHE PATH "Asio headers" FORCE)
endif()
set(CROW_BUILD_EXAMPLES OFF CACHE BOOL "" FORCE)
set(CROW_BUILD_TESTS OFF CACHE BOOL "" FORCE)
set(CROW_INSTALL OFF CACHE BOOL "" FORCE)
FetchContent_Declare(crow
    GIT_REPOSITORY https://github.com/CrowCpp/Crow.git
    GIT_TAG v1.3.0
    GIT_SHALLOW TRUE
)
FetchContent_MakeAvailable(crow)

add_executable(bcc323-api src/main.cpp)
target_link_libraries(bcc323-api PRIVATE Crow::Crow)
if(WIN32)
    target_link_libraries(bcc323-api PRIVATE ws2_32 mswsock)
    target_compile_definitions(bcc323-api PRIVATE _WIN32_WINNT=0x0A00)
endif()
if(MSVC)
    target_compile_options(bcc323-api PRIVATE /W4)
else()
    target_compile_options(bcc323-api PRIVATE -Wall -Wextra -Wpedantic)
endif()

include(CTest)
if(BUILD_TESTING)
    find_package(Python3 REQUIRED COMPONENTS Interpreter)
    add_test(NAME api-smoke
        COMMAND ${Python3_EXECUTABLE} ${CMAKE_CURRENT_SOURCE_DIR}/test/smoke.py
                $<TARGET_FILE:bcc323-api>)
    set_tests_properties(api-smoke PROPERTIES TIMEOUT 30)
endif()
