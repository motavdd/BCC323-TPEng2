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
find_package(OpenSSL REQUIRED)
find_package(CURL REQUIRED)

set(JWT_BUILD_EXAMPLES OFF CACHE BOOL "" FORCE)
set(JWT_BUILD_TESTS OFF CACHE BOOL "" FORCE)

FetchContent_Declare(jwt_cpp
    GIT_REPOSITORY https://github.com/Thalhammer/jwt-cpp.git
    GIT_TAG v0.7.1
    GIT_SHALLOW TRUE
)

FetchContent_MakeAvailable(jwt_cpp)

file(GLOB DOMAIN_SOURCES CONFIGURE_DEPENDS "${CMAKE_CURRENT_SOURCE_DIR}/src/domain/*.cpp")
add_library(academic-domain STATIC ${DOMAIN_SOURCES})
target_include_directories(academic-domain PUBLIC "${CMAKE_CURRENT_SOURCE_DIR}/include")

add_library(academic-application STATIC src/application/AcademicService.cpp)
target_link_libraries(academic-application PUBLIC academic-domain)

file(GLOB_RECURSE INFRASTRUCTURE_SOURCES CONFIGURE_DEPENDS "${CMAKE_CURRENT_SOURCE_DIR}/src/infrastructure/*.cpp")
add_library(academic-infrastructure STATIC ${INFRASTRUCTURE_SOURCES})
target_link_libraries(academic-infrastructure PUBLIC academic-application Crow::Crow jwt-cpp::jwt-cpp OpenSSL::Crypto CURL::libcurl)

file(GLOB HTTP_SOURCES CONFIGURE_DEPENDS "${CMAKE_CURRENT_SOURCE_DIR}/src/http/*.cpp")
add_executable(bcc323-api src/main.cpp src/config/config.cpp ${HTTP_SOURCES})
target_link_libraries(bcc323-api PRIVATE academic-infrastructure)
set_target_properties(bcc323-api PROPERTIES
    RUNTIME_OUTPUT_DIRECTORY "${CMAKE_CURRENT_BINARY_DIR}/bin/$<0:>"
)
if(WIN32)
    target_link_libraries(academic-infrastructure PUBLIC ws2_32 mswsock)
    target_compile_definitions(academic-infrastructure PUBLIC _WIN32_WINNT=0x0A00)
endif()
foreach(target academic-domain academic-application academic-infrastructure bcc323-api)
    if(MSVC)
        target_compile_options(${target} PRIVATE /W4)
    else()
        target_compile_options(${target} PRIVATE -Wall -Wextra -Wpedantic)
    endif()
endforeach()

include(CTest)
if(BUILD_TESTING)
    add_executable(application-tests test/application/academic_service_test.cpp)
    target_link_libraries(application-tests PRIVATE academic-application)
    add_test(NAME academic-service COMMAND application-tests)
    add_executable(auth-tests
        test/auth/token_verifier_test.cpp
        src/infrastructure/auth/FirebaseTokenVerifier.cpp
    )

    target_include_directories(auth-tests PRIVATE
        "${CMAKE_CURRENT_SOURCE_DIR}/include"
    )

    target_link_libraries(auth-tests PRIVATE
        jwt-cpp::jwt-cpp
    )

    target_compile_definitions(auth-tests PRIVATE
        AUTH_FIXTURES_DIR="${CMAKE_CURRENT_SOURCE_DIR}/test/auth/fixtures"
    )

    add_test(
        NAME auth-token-verifier
        COMMAND auth-tests
    )
    find_package(Python3 COMPONENTS Interpreter)
    if(Python3_Interpreter_FOUND)
        add_test(NAME api-smoke
            COMMAND ${Python3_EXECUTABLE} ${CMAKE_CURRENT_SOURCE_DIR}/test/smoke.py
                    $<TARGET_FILE:bcc323-api>)
        set_tests_properties(api-smoke PROPERTIES TIMEOUT 30)
    else()
        message(STATUS "Python 3 ausente: smoke HTTP disponível no CI; testes C++ continuam habilitados.")
    endif()
endif()
