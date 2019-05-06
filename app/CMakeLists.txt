cmake_minimum_required(VERSION 3.4.1)

add_library(gcodec SHARED src/main/cpp/gcodec.cpp)

target_include_directories(gcodec PRIVATE ${CMAKE_SOURCE_DIR}/src/main/cpp/include) #头文件

add_library(ffmpeg SHARED IMPORTED)
set_target_properties(ffmpeg PROPERTIES IMPORTED_LOCATION ${CMAKE_SOURCE_DIR}/src/main/jniLibs/${CMAKE_ANDROID_ARCH_ABI}/libffmpeg.so)

add_library(yuv SHARED IMPORTED)
set_target_properties(yuv PROPERTIES IMPORTED_LOCATION ${CMAKE_SOURCE_DIR}/src/main/jniLibs/${CMAKE_ANDROID_ARCH_ABI}/libyuv.so)

find_library(log-lib log)

target_link_libraries(gcodec ffmpeg yuv ${log-lib} android)