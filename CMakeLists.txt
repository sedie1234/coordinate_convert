# cmake_minimum_required(VERSION 3.11 FATAL_ERROR)
# project(cuda_test LANGUAGES CXX CUDA)

# set(CMAKE_CXX_STANDARD 20)
# set(CMAKE_CUDA_STANDARD 20)

# add_executable(cuda_run src/main.cu
#                         src/convert_matrix.cpp
#                         src/cuda_test.cu)


cmake_minimum_required(VERSION 3.16)
project(CUDA_CPP_Example LANGUAGES CXX CUDA)

find_package(Eigen3 REQUIRED)

enable_language(CUDA)
include_directories(${CMAKE_CUDA_TOOLKIT_INCLUDE_DIRECTORIES})


set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CUDA_STANDARD 14)


add_executable(gpu_test src/main.cpp 
                        src/convert_matrix.cpp
                        src/matmul.cu)

target_link_libraries(gpu_test Eigen3::Eigen)

target_include_directories(gpu_test PUBLIC
    include
)