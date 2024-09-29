#include "VertexBuffer.h"

#include "SimpleEngineCore/Log.h"

#include <glad/glad.h>

namespace SimpleEngine {

	constexpr unsigned int shader_data_type_to_components_count(const ShaderDataType type) {
		switch (type) {
		case ShaderDataType::Float:
		case ShaderDataType::Int:
			return 1;
		case ShaderDataType::Float2:
		case ShaderDataType::Int2:
			return 2;
		case ShaderDataType::Float3:
		case ShaderDataType::Int3:
			return 3;
		case ShaderDataType::Float4:
		case ShaderDataType::Int4:
			return 4;
		}
		LOG_ERROR("shader data type to components type: unknown shader type!");
		return 0;
	}

	constexpr size_t shader_data_type_size(const ShaderDataType type) {
		switch (type) {
		case ShaderDataType::Float:
		case ShaderDataType::Float2:
		case ShaderDataType::Float3:
		case ShaderDataType::Float4:
			return sizeof(GLfloat) * shader_data_type_to_components_count(type);
		case ShaderDataType::Int:
		case ShaderDataType::Int2:
		case ShaderDataType::Int3:
		case ShaderDataType::Int4:
			return sizeof(GLint) * shader_data_type_to_components_count(type);
		}
		LOG_ERROR("shader data type size: unknown shader type!");
		return 0;
	}

	constexpr unsigned int shader_data_type_to_component_type(const ShaderDataType type) {
		switch (type) {
		case ShaderDataType::Float:
		case ShaderDataType::Float2:
		case ShaderDataType::Float3:
		case ShaderDataType::Float4:
			return GL_FLOAT;
		case ShaderDataType::Int:
		case ShaderDataType::Int2:
		case ShaderDataType::Int3:
		case ShaderDataType::Int4:
			return GL_INT;
		}
		LOG_ERROR("shader_data_type_to_component_type: unknown shader type!");
		return GL_FLOAT;
	}

	constexpr GLenum usage_to_GLenum(const VertexBuffer::EUsage usage) {
		switch (usage)
		{
		case VertexBuffer::EUsage::Static: return GL_STATIC_DRAW;
		case VertexBuffer::EUsage::Dynamic: return GL_DYNAMIC_DRAW;
		case VertexBuffer::EUsage::Stream: return GL_STREAM_DRAW;
		}

		LOG_ERROR("Unknown VertexBuffer usage");
		return GL_STREAM_DRAW;
	}

	VertexBuffer::VertexBuffer(const void* data, const size_t size, BufferLayout buffer_layout, const EUsage usage)
		: m_buffer_layout(std::move(buffer_layout)) {
		// NOW we have to PASS our CPU data in shaders 
		// using VERTEX BUFFER OBJECT to allocate and fill memory on gpu
		glGenBuffers(1, &m_id);
		// connect buffer and make it current (only one can be current)
		bind();
		// fill buffer by transfering data from cpu to gpu memory
		// static means data in buffer do not change
		glBufferData(GL_ARRAY_BUFFER, size, data, usage_to_GLenum(usage));

	}
	VertexBuffer::~VertexBuffer() {
		glDeleteBuffers(1, &m_id);
	}


	VertexBuffer::VertexBuffer(VertexBuffer&& vertexBuffer) noexcept :
		m_id(vertexBuffer.m_id), m_buffer_layout(std::move(vertexBuffer.m_buffer_layout))
	{
		vertexBuffer.m_id = 0;
	}

	VertexBuffer& VertexBuffer::operator= (VertexBuffer&& vertexBuffer) noexcept {
		m_id = vertexBuffer.m_id;
		vertexBuffer.m_id = 0;
		return *this;
	}

	void VertexBuffer::bind() const {
		glBindBuffer(GL_ARRAY_BUFFER, m_id);
	}

	void VertexBuffer::unbind() {
		glBindBuffer(GL_ARRAY_BUFFER, 0);
	}

	BufferElement::BufferElement(const ShaderDataType type)
		: type(type)

		, components_count(shader_data_type_to_components_count(type))
		, component_type(shader_data_type_to_component_type(type))
		, size(shader_data_type_size(type))
		, offset(0)
	{
	}
}