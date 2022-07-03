# 
# File: CMakeLists.txt
# Author: Thyme Chrystal
#
# MIT License
#
# Copyright (c) 2022 Thyme Chrystal
#
# Permission is hereby granted, free of charge, to any person obtaining a copy
# of this software and associated documentation files (the "Software"), to deal
# in the Software without restriction, including without limitation the rights
# to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
# copies of the Software, and to permit persons to whom the Software is
# furnished to do so, subject to the following conditions:
#
# The above copyright notice and this permission notice shall be included in all
# copies or substantial portions of the Software.
#
# THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
# IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
# FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
# AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
# LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
# OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE
# SOFTWARE.
#

# Version required for Qt6 (added later)
cmake_minimum_required(VERSION 3.16.0)

# Define the project
project(test-console VERSION 1.0.0)

# Use C++17
set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED)

# Set up the output directries
set(CMAKE_ARCHIVE_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/lib)
set(CMAKE_LIBRARY_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/lib)
set(CMAKE_RUNTIME_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/bin)

# Set up include directories
include_directories(${CMAKE_SOURCE_DIR} ${CMAKE_BINARY_DIR})

# Include the platform specific stuff
add_subdirectory(platform)

# Generate the appropriate configuration for include
configure_file(${CMAKE_SOURCE_DIR}/platform/console-platform.h.in console-platform.h)

# Create the executable
set(TEST_CONSOLE_SOURCES
  main.cpp
  console.cpp
  trie.cpp
  ${PLATFORM_SOURCES}
)

set(TEST_CONSOLE_HEADERS
  console.h
  trie.h
  ${CMAKE_BINARY_DIR}/console-platform.h
  ${PLATFORM_HEADERS}
)

source_group("Header Files" ${TEST_CONSOLE_HEADERS})
source_group("Source Files" ${TEST_CONSOLE_SOURCES})

add_executable(test-console ${TEST_CONSOLE_SOURCES} ${TEST_CONSOLE_HEADERS})
