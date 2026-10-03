cmake_minimum_required(VERSION 3.22)
project(FlappyBird CXX C)

set(CMAKE_CXX_STANDARD 17)

include(FetchContent)
FetchContent_Declare(
    raylib
    URL https://github.com/raysan5/raylib/archive/refs/tags/5.0.tar.gz
)
FetchContent_MakeAvailable(raylib)

# Criar a biblioteca dinamica libmain.so com o glue nativo do Android
add_library(main SHARED 
    main.cpp 
    ${ANDROID_NDK}/sources/android/native_app_glue/android_native_app_glue.c
)

target_include_directories(main PRIVATE ${ANDROID_NDK}/sources/android/native_app_glue)
target_compile_definitions(main PRIVATE PLATFORM_ANDROID)
target_link_libraries(main PRIVATE raylib EGL GLESv2 android log)
