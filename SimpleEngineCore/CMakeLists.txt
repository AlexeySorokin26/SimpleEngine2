cmake_minimum_required(VERSION 3.12 FATAL_ERROR)

set(PROJECT_NAME SimpleEngineCore)

set(ENGINE_PUBLIC_INCLUDES
	includes/SimpleEngineCore/Application.h	
	includes/SimpleEngineCore/Log.h	
)

set(ENGINE_PRIVATE_INCLUDES
	src/SimpleEngineCore/Window.h
)

set(ENGINE_PRIVATE_SOURCES
	src/SimpleEngineCore/Application.cpp
	src/SimpleEngineCore/Window.cpp
	src/SimpleEngineCore/Log.cpp
)

add_library(
	${PROJECT_NAME} 
	STATIC
	${ENGINE_PUBLIC_INCLUDES} 
	${ENGINE_PRIVATE_INCLUDES} 
	${ENGINE_PRIVATE_SOURCES} 
)

target_include_directories(
	${PROJECT_NAME} 
	PUBLIC
	includes
)

target_include_directories(
	${PROJECT_NAME} 
	PRIVATE
	src
)

target_compile_features(
	${PROJECT_NAME} 
	PUBLIC
	cxx_std_17
)

add_subdirectory(../external/glfw ${CMAKE_CURRENT_BINARY_DIR}/glfw)
# we add glfw with own cmake file 
# and also specify where to put binaries of generated lib as 2 arg
target_link_libraries(${PROJECT_NAME} PRIVATE glfw) # add now link it to our core 

add_subdirectory(../external/spdlog ${CMAKE_CURRENT_BINARY_DIR}/spdlog)
target_link_libraries(${PROJECT_NAME} PRIVATE spdlog)

add_subdirectory(../external/glad ${CMAKE_CURRENT_BINARY_DIR}/glad)
target_link_libraries(${PROJECT_NAME} PRIVATE glad)