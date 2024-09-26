#include "VertexBuffer.h"

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

	VertexBuffer::VertexBuffer(const void* data, const size_t size, const EUsage usage) {
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


	VertexBuffer::VertexBuffer(VertexBuffer&& vertexBuffer) noexcept {
		m_id = vertexBuffer.m_id;
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
}