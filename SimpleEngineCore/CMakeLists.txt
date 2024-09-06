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