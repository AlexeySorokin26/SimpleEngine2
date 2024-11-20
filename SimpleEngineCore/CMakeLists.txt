cmake_minimum_required(VERSION 3.12 FATAL_ERROR)

set(PROJECT_NAME SimpleEngineCore)

add_compile_definitions(SOURCE_DIR="${CMAKE_SOURCE_DIR}")

# Define shader files
set(SHADER_FILES    
	shaders/light_cube_vertex_shader.glsl
    shaders/light_cube_fragment_shader.glsl

	shaders/phong_cube_vertex_shader.glsl
	shaders/phong_cube_fragment_shader.glsl

	shaders/flate_sphere_vertex_shader.glsl
	shaders/flate_sphere_fragment_shader.glsl

	shaders/phong_sphere_vertex_shader.glsl
	shaders/phong_sphere_fragment_shader.glsl
)

set(ENGINE_PUBLIC_INCLUDES
	includes/SimpleEngineCore/Application.h	
	includes/SimpleEngineCore/Log.h	
	includes/SimpleEngineCore/Event.h	
	includes/SimpleEngineCore/Camera.h	
	includes/SimpleEngineCore/Keys.h	
	includes/SimpleEngineCore/Input.h
	includes/SimpleEngineCore/Input.h
	includes/SimpleEngineCore/Utils.h
)

set(ENGINE_PRIVATE_INCLUDES
	src/SimpleEngineCore/Window.h
	src/SimpleEngineCore/Modules/UIModule.h
	src/SimpleEngineCore/Rendering/OpenGL/ShaderProgram.h
	src/SimpleEngineCore/Rendering/OpenGL/VertexBuffer.h
	src/SimpleEngineCore/Rendering/OpenGL/VertexArray.h
	src/SimpleEngineCore/Rendering/OpenGL/IndexBuffer.h
	src/SimpleEngineCore/Rendering/OpenGL/Renderer_OpenGL.h
	src/SimpleEngineCore/Rendering/OpenGL/Texture2D.h
	src/SimpleEngineCore/Rendering/OpenGL/Mesh.h
	src/SimpleEngineCore/Rendering/OpenGL/Material.h
	src/SimpleEngineCore/Rendering/OpenGL/Light.h
)

set(ENGINE_PRIVATE_SOURCES
	src/SimpleEngineCore/Application.cpp
	src/SimpleEngineCore/Window.cpp
	src/SimpleEngineCore/Modules/UIModule.cpp
	src/SimpleEngineCore/Log.cpp
	src/SimpleEngineCore/Camera.cpp
	src/SimpleEngineCore/Input.cpp
	src/SimpleEngineCore/Utils.cpp
	src/SimpleEngineCore/Rendering/OpenGL/ShaderProgram.cpp
	src/SimpleEngineCore/Rendering/OpenGL/VertexBuffer.cpp
	src/SimpleEngineCore/Rendering/OpenGL/VertexArray.cpp
	src/SimpleEngineCore/Rendering/OpenGL/IndexBuffer.cpp
	src/SimpleEngineCore/Rendering/OpenGL/Renderer_OpenGL.cpp
	src/SimpleEngineCore/Rendering/OpenGL/Texture2D.cpp
	src/SimpleEngineCore/Rendering/OpenGL/Mesh.cpp
)

set(ENGINE_ALL_SOURCES
	${ENGINE_PUBLIC_INCLUDES}	
	${ENGINE_PRIVATE_INCLUDES}	
	${ENGINE_PRIVATE_SOURCES}	
	${SHADER_FILES}
)

add_library(
	${PROJECT_NAME} 
	STATIC
	${ENGINE_ALL_SOURCES} 
)

source_group(TREE ${CMAKE_CURRENT_SOURCE_DIR} FILES
	${ENGINE_PUBLIC_INCLUDES}
    ${ENGINE_PRIVATE_INCLUDES}
    ${ENGINE_PRIVATE_SOURCES}
)

source_group("Shaders" FILES ${SHADER_FILES})

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

add_subdirectory(../external/glm ${CMAKE_CURRENT_BINARY_DIR}/glm)
target_link_libraries(${PROJECT_NAME} PRIVATE glm)

target_include_directories(${PROJECT_NAME} PRIVATE ../external/stb)

#imgui library create
set(IMGUI_INCLUDES
	../external/imgui/imgui.h
	../external/imgui/backends/imgui_impl_glfw.h
	../external/imgui/backends/imgui_impl_opengl3.h
)

set(IMGUI_SOURCES
	../external/imgui/imgui.cpp
	../external/imgui/backends/imgui_impl_glfw.cpp
	../external/imgui/backends/imgui_impl_opengl3.cpp
	../external/imgui/imgui_demo.cpp
	../external/imgui/imgui_tables.cpp
	../external/imgui/imgui_widgets.cpp
	../external/imgui/imgui_draw.cpp
)

add_library(
	ImGui STATIC
	${IMGUI_INCLUDES} 
	${IMGUI_SOURCES} 
)

target_include_directories(ImGui PUBLIC ../external)
target_include_directories(ImGui PUBLIC ../external/imgui)
target_link_libraries(ImGui PRIVATE glad glfw)

target_link_libraries(${PROJECT_NAME} PRIVATE ImGui) 

