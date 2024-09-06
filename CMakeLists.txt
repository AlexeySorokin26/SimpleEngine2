cmake_minimum_required(VERSION 3.12 FATAL_ERROR)

set(PROJECT_NAME SimpleEngine2)

add_subdirectory(SimpleEngineCore)
add_subdirectory(SimpleEngineEditor)

set_property(
	DIRECTORY 
	${CMAKE_CURRENT_SOURCE_DIR} 
	PROPERTY 
	VS_STARTUP_PROJECT 
	SimpleEngineEditor
)