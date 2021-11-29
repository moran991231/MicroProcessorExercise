# For more information about using CMake with Android Studio, read the
# documentation: https://d.android.com/studio/projects/add-native-code.html

# Sets the minimum version of CMake required to build the native library.

cmake_minimum_required(VERSION 3.10.2)


project("jaesun_final")


add_library(GLES_mali STATIC IMPORTED)
add_library( image SHARED image.cpp)
add_library( gpio_button SHARED gpio_button.c)
add_library( led SHARED led.c)
add_library( segment SHARED seven_segment.c)

find_library(jnigraphics-lib jnigraphics)
find_library(log-lib log)

target_link_libraries(
        image
        gpio_button
        led
        segment
        E:/MicroProcessorExercise/opencl/lib/libGLES_mali.so
        ${log-lib}
        ${jnigraphics-lib})