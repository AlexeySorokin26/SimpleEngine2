#include "IndexBuffer.h"

#include "SimpleEngineCore/Log.h"

#include <glad/glad.h>

namespace SimpleEngine {

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

	IndexBuffer::IndexBuffer(const void* data, const size_t count, const VertexBuffer::EUsage usage)
		: m_count(count)
	{
		glGenBuffers(1, &m_id);
		if (m_id == 0) {
			LOG_ERROR("Failed to generate an index buffer");
		}
		glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, m_id);
		glBufferData(GL_ELEMENT_ARRAY_BUFFER, count * sizeof(GLuint), data, usage_to_GLenum(usage));
		GLenum error = glGetError();
		if (error != GL_NO_ERROR) {
			LOG_ERROR("OpenGL error occurred: {0}", error);
		}
	}

	IndexBuffer::~IndexBuffer()
	{
		if (m_id != 0) {
			glDeleteBuffers(1, &m_id);
		}
	}

	IndexBuffer::IndexBuffer(IndexBuffer&& indexBuffer) noexcept
		: m_id(indexBuffer.m_id)
		, m_count(indexBuffer.m_count)
	{
		indexBuffer.m_id = 0;
		indexBuffer.m_count = 0;
	}

	IndexBuffer& IndexBuffer::operator=(IndexBuffer&& indexBuffer) noexcept
	{
		if (this != &indexBuffer) {
			glDeleteBuffers(1, &m_id);  // Clean up the current buffer if needed

			m_id = indexBuffer.m_id;
			m_count = indexBuffer.m_count;

			indexBuffer.m_id = 0;
			indexBuffer.m_count = 0;
		}
		return *this;
	}

	void IndexBuffer::bind() const
	{
		glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, m_id);
	}

	void IndexBuffer::unbind()
	{
		glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, 0);
	}
}
