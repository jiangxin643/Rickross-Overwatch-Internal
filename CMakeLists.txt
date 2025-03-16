cmake_minimum_required(VERSION 3.10)
project("ow" VERSION 1.0 LANGUAGES CXX)

set(CMAKE_CXX_COMPILER "x86_64-w64-mingw32-g++")
set(CMAKE_CXX_FLAGS "-std=c++20 -s -O3 -w -mnop-fun-dllimport -Wreturn-type -Wno-attributes -Wno-enum-compare -Wno-write-strings -Wno-builtin-declaration-mismatch -Wno-int-to-pointer-cast -fpermissive -fno-exceptions -fno-rtti -fno-stack-protector -fno-unwind-tables -fno-asynchronous-unwind-tables -fno-math-errno -fno-unroll-loops -fmerge-all-constants -fno-ident -ld3dcompiler -Wl,--exclude-all-symbols,--kill-at,--entry=entry")

set(CMAKE_CXX_IMPLICIT_LINK_LIBRARIES "")
set(CMAKE_CXX_STANDARD_LIBRARIES "")

function(add_sources IN_FILE)
	file(GLOB autopilot_sources CONFIGURE_DEPENDS "${IN_FILE}")
	set(BUILD_SOURCES_LIST "${BUILD_SOURCES_LIST}${autopilot_sources};" PARENT_SCOPE)
endfunction()

add_sources("base/**/*.cpp")
add_sources("**/*.cpp")

add_library(${PROJECT_NAME} SHARED ${BUILD_SOURCES_LIST})