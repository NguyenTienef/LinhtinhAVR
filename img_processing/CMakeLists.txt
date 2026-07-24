# cmake_minimum_required(VERSION 3.10)
# project(img_processing)
# set(CMAKE_CXX_STANDARD 17)

# set(OpenCV_DIR "C:/Users/Admin/Desktop/Cfile/img_processing/opencv/build/install/x64/mingw/lib")
# find_package(OpenCV REQUIRED)
# include_directories(${OpenCV_INCLUDE_DIRS})
# include_directories(${CMAKE_SOURCE_DIR}/include)
# ============================================================
# KIỂU 1: Mỗi file .cpp trong src/ → 1 exe riêng
# (mỗi file phải có hàm main riêng)
# ============================================================
# file(GLOB SOURCES "test/Normal_cam.cpp")
# foreach(SOURCE ${SOURCES})
#     get_filename_component(EXE_NAME ${SOURCE} NAME_WE)
#     add_executable(${EXE_NAME} ${SOURCE}) 
#     target_link_libraries(${EXE_NAME} ${OpenCV_LIBS})
#     target_include_directories(${EXE_NAME} PRIVATE ${CMAKE_SOURCE_DIR}/include)
# endforeach()

# ============================================================
# KIỂU 2: Nhiều file .cpp → 1 exe duy nhất
# (chỉ 1 file có main, các file còn lại là helper/module)
# Bỏ comment phần dưới khi cần dùng, comment lại KIỂU 1
# ============================================================
# set(PROJECT_SOURCES
#     src/main.cpp
#     src/utils.cpp
#     src/image_processing.cpp
# )
# add_executable(my_app ${PROJECT_SOURCES})
# target_link_libraries(my_app ${OpenCV_LIBS})

cmake_minimum_required(VERSION 3.10)
project(img_processing)
set(CMAKE_CXX_STANDARD 17)

set(OpenCV_DIR "C:/Users/Admin/Desktop/Cfile/img_processing/opencv/build/install/x64/mingw/lib")
find_package(OpenCV REQUIRED)
include_directories(${OpenCV_INCLUDE_DIRS})
include_directories(${CMAKE_SOURCE_DIR}/include)

# ============================================================
# KIỂU 1: Mỗi file .cpp trong test/ → 1 exe riêng
# (mỗi file phải có hàm main riêng)
# ============================================================
file(GLOB SOURCES "test/Normal_cam.cpp")
foreach(SOURCE ${SOURCES})
    get_filename_component(EXE_NAME ${SOURCE} NAME_WE)
    add_executable(${EXE_NAME}
        ${SOURCE}
        ${CMAKE_SOURCE_DIR}/include/config.cpp
    )
    target_link_libraries(${EXE_NAME} ${OpenCV_LIBS})
    target_include_directories(${EXE_NAME} PRIVATE ${CMAKE_SOURCE_DIR}/include)
endforeach()

# ============================================================
# KIỂU 2: Nhiều file .cpp → 1 exe duy nhất
# (chỉ 1 file có main, các file còn lại là helper/module)
# Bỏ comment phần dưới khi cần dùng, comment lại KIỂU 1
# ============================================================
# set(PROJECT_SOURCES
#     src/main.cpp
#     src/utils.cpp
#     src/image_processing.cpp
#     include/config.cpp
# )
# add_executable(my_app ${PROJECT_SOURCES})
# target_link_libraries(my_app ${OpenCV_LIBS})