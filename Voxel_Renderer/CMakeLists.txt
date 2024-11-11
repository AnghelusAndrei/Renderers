cmake_minimum_required(VERSION 3.7)
project(Voxel_Renderer VERSION 1.0) 

set(CXX_STANDARD 11)
set(CMAKE_MODULE_PATH ${CMAKE_MODULE_PATH} "${CMAKE_SOURCE_DIR}/cmake/")


#set(CMAKE_BUILD_TYPE Debug)
if ( ${CMAKE_SOURCE_DIR} STREQUAL ${CMAKE_BINARY_DIR} )
    message( FATAL_ERROR "In-source builds not allowed. Please make a new directory (called a build directory) and run CMake from there. You may need to remove CMakeCache.txt." )
endif()


add_executable(${PROJECT_NAME} src/main.cpp)


IF (WIN32)
    #SDL
    set(SDL2_LIBRARY CACHE PATH "C:\\Users\\megag\\Desktop\\SDL2-2.0.22\\lib\\x64\\SDL2.lib")
    set(SDL2_INCLUDE_DIR CACHE PATH "C:\\Users\\megag\\Desktop\\SDL2-2.0.22\\include")

    target_link_libraries(${PROJECT_NAME} "C:\\Users\\megag\\Desktop\\SDL2-2.0.22\\lib\\x64\\SDL2.lib")
    target_link_libraries(${PROJECT_NAME} "C:\\Users\\megag\\Desktop\\Voxel_Renderer\\lib\\SDL2_ttf.dll")
    target_include_directories(${PROJECT_NAME} PUBLIC SDL2_INCLUDE_DIR)

    #SDL_ttf
    set(SDL2_TTF_LIBRARY CACHE PATH "C:\\Users\\megag\\Desktop\\SDL2_ttf-2.20.0\\lib\\x64\\SDL2_ttf.lib")
    set(SDL2_TTF_INCLUDE_DIR CACHE PATH "C:\\Users\\megag\\Desktop\\SDL2_ttf-2.20.0\\include")

    target_link_libraries(${PROJECT_NAME} "C:\\Users\\megag\\Desktop\\SDL2_ttf-2.20.0\\lib\\x64\\SDL2_ttf.lib")
    target_link_libraries(${PROJECT_NAME} "C:\\Users\\megag\\Desktop\\SDL2_ttf-2.20.0\\lib\\x64\\SDL2_ttf.dll")
    target_include_directories(${PROJECT_NAME} PUBLIC SDL2_TTF_INCLUDE_DIR)
ELSE()
    find_package(SDL_ttf REQUIRED)
    find_package(SDL2 REQUIRED)
    find_package(OpenCL REQUIRED)

    include_directories(${SDL2_INCLUDE_DIRS} ${SDL_TTF_INCLUDE_DIRS} ${OpenCL_INCLUDE_DIRS})
    target_link_libraries(${PROJECT_NAME} ${SDL2_LIBRARIES} ${SDL_TTF_LIBRARIES} ${OpenCL_LIBRARIES})
ENDIF()



target_include_directories(${PROJECT_NAME} PUBLIC lib/PerlinNoise)

file(GLOB_RECURSE shaders "*.cl" "*.hcl")
file(COPY ${shaders} DESTINATION ${CMAKE_CURRENT_BINARY_DIR}/shd)
