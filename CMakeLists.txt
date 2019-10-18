cmake_minimum_required(VERSION 3.2)
project(gst)

if(NOT CMAKE_BUILD_TYPE)
    message(STATUS "Build type is default defined as Release. change it by define -DCMAKE_BUILD_TYPE=Debug as param to cmake")
    set(CMAKE_BUILD_TYPE Release)
endif()

# Download conan.cmake, download taged release - v0.14
if(NOT EXISTS "${CMAKE_SOURCE_DIR}/env/conan.cmake")
   message(STATUS "Downloading conan.cmake from https://github.com/conan-io/cmake-conan")
   file(DOWNLOAD "https://github.com/conan-io/cmake-conan/raw/v0.14/conan.cmake"
                 "${CMAKE_SOURCE_DIR}/env/conan.cmake")
endif()
include(env/conan.cmake)

conan_cmake_run(CONANFILE conanfile.txt
                BASIC_SETUP
                BUILD missing)

set(CMAKE_CXX_STANDARD 17)

add_executable(${CMAKE_PROJECT_NAME}
  src/Main.cpp
  src/gst.cpp
  src/gst.h
  )

target_link_libraries(${CMAKE_PROJECT_NAME} ${CONAN_LIBS})

# Tests
set(TEST_FILES
  	src/gst.cpp
  	src/gst.h
	tests/gstTest.cpp
	tests/Main.cpp
	)
set(TEST_NAME ${CMAKE_PROJECT_NAME}Test)

add_executable(${TEST_NAME} ${TEST_FILES})
target_link_libraries(${TEST_NAME} ${CONAN_LIBS})


install(DIRECTORY ${CMAKE_SOURCE_DIR}/env/
DESTINATION ${CMAKE_RUNTIME_OUTPUT_DIRECTORY}
FILES_MATCHING
PATTERN "*.*"
PERMISSIONS OWNER_READ OWNER_WRITE GROUP_READ WORLD_READ)
#FILES ${CMAKE_SOURCE_DIR}/env DESTINATION ${CMAKE_RUNTIME_OUTPUT_DIRECTORY})
