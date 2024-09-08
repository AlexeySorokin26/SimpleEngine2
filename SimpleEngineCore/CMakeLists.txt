cmake_minimum_required(VERSION 3.12 FATAL_ERROR)

set(PROJECT_NAME SimpleEngineCore)

add_library(
	${PROJECT_NAME} 
	STATIC
	src/SimpleEngineCore/Utils/test.cpp
	includes/SimpleEngineCore/Utils/test.h
)

target_include_directories(
	${PROJECT_NAME} 
	PUBLIC
	includes
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