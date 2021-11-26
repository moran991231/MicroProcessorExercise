# For more information about using CMake with Android Studio, read the
# documentation: https://d.android.com/studio/projects/add-native-code.html

# Sets the minimum version of CMake required to build the native library.

cmake_minimum_required(VERSION 3.10.2)

# Declares and names the project.

project("gaussian_blur_jni")

# Creates and names a library, sets it as either STATIC
# or SHARED, and provides the relative paths to its source code.
# You can define multiple libraries, and CMake builds them for you.
# Gradle automatically packages shared libraries with your APK.
add_library(GLES_mali STATIC IMPORTED)
add_library( # Sets the name of the library.
        OpenCLDriver
        SHARED
        OpenCLDriver_.cpp)

find_library(jnigraphics-lib
        jnigraphics)
find_library( # Sets the name of the path variable.
        log-lib
        log)


target_link_libraries( # Specifies the target library.
        OpenCLDriver
        E:/MicroProcessorExercise/opencl/lib/libGLES_mali.so
        ${log-lib}
        ${jnigraphics-lib})