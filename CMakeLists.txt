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
		src/basic-tutorial-1.c src/tutorials.h src/basic-tutorial-2.c src/basic-tutorial-3.c src/basic-tutorial-4.c)

set(GStreamerIncludeDirs
		/Library/Frameworks/GStreamer.framework/Headers
		/Library/Frameworks/GStreamer.framework/Versions/Current/include/gstreamer-1.0
		/Library/Frameworks/GStreamer.framework/Versions/Current/include
		/Library/Frameworks/GStreamer.framework/Versions/Current/include/glib-2.0
		/Library/Frameworks/GStreamer.framework/Versions/Current/lib/glib-2.0/include
		)
set(GStreamerLibDirs
		/Library/Frameworks/GStreamer.framework/Libraries
		)
set(GStreamerLibs
		gstreamer-1.0
		gobject-2.0
		glib-2.0
		intl
		)
target_include_directories(${CMAKE_PROJECT_NAME} PRIVATE ${GStreamerIncludeDirs})
target_link_directories(${CMAKE_PROJECT_NAME} PRIVATE ${GStreamerLibDirs})
target_link_libraries(${CMAKE_PROJECT_NAME} PRIVATE ${CONAN_LIBS} ${GStreamerLibs})

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
