cmake_minimum_required(VERSION 3.10)
project(img_processing)

set(CMAKE_CXX_STANDARD 17)

set(OpenCV_DIR "C:/Users/Admin/Desktop/Cfile/img_processing/opencv/build/install/x64/mingw/lib")
find_package(OpenCV REQUIRED)

include_directories(${OpenCV_INCLUDE_DIRS})

file(GLOB SOURCES "src/*.cpp")
foreach(SOURCE ${SOURCES})
    get_filename_component(EXE_NAME ${SOURCE} NAME_WE)
    add_executable(${EXE_NAME} ${SOURCE})
    target_link_libraries(${EXE_NAME} ${OpenCV_LIBS})
endforeach()