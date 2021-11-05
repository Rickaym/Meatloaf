cmake_minimum_required(VERSION 3.2)

project(Meatloaf)


if ("${GIT_REV}" STREQUAL "")
    set(GIT_REV "N/A")
    set(GIT_DIFF "")
    set(GIT_TAG "N/A")
    set(GIT_BRANCH "N/A")
else()
    execute_process(
        COMMAND bash -c "git diff --quiet --exit-code || echo +"
        OUTPUT_VARIABLE GIT_DIFF)
    execute_process(
        COMMAND git describe --exact-match --tags
        OUTPUT_VARIABLE GIT_TAG ERROR_QUIET)
    execute_process(
        COMMAND git rev-parse --abbrev-ref HEAD
        OUTPUT_VARIABLE GIT_BRANCH)

    string(STRIP "${GIT_REV}" GIT_REV)
    string(SUBSTRING "${GIT_REV}" 1 7 GIT_REV)
    string(STRIP "${GIT_DIFF}" GIT_DIFF)
    string(STRIP "${GIT_TAG}" GIT_TAG)
    string(STRIP "${GIT_BRANCH}" GIT_BRANCH)
endif()

set(VERSION "#pragma once\n\n#define GIT_REV \"${GIT_REV}${GIT_DIFF}\"
#define GIT_TAG \"${GIT_TAG}\"
#define GIT_BRANCH \"${GIT_BRANCH}\"")

if(EXISTS include/git.h)
    file(READ include/git.h VERSION_)
else()
    set(VERSION_ "")
endif()

if (NOT "${VERSION}" STREQUAL "${VERSION_}")
    file(WRITE include/git.h "${VERSION}")
endif()



file(GLOB SOURCES src/*.cpp)

file(GLOB HEADER_FILES include/*.h)

add_executable(Meatloaf ${SOURCES} ${HEADER_FILES})

target_include_directories(Meatloaf PRIVATE include)