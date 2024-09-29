#include "VertexArray.h"

#include <glad/glad.h>

namespace SimpleEngine {
	SimpleEngine::VertexArray::VertexArray()
	{
		glGenVertexArrays(1, &m_id);
	}

	SimpleEngine::VertexArray::~VertexArray()
	{
		glDeleteVertexArrays(1, &m_id);
	}

	VertexArray& SimpleEngine::VertexArray::operator=(VertexArray&& vertex_array) noexcept
	{
		m_id = vertex_array.m_id;
		m_elements_count = vertex_array.m_elements_count;
		vertex_array.m_id = 0;
		vertex_array.m_elements_count = 0;
		return *this;
	}

	SimpleEngine::VertexArray::VertexArray(VertexArray&& vertex_array) noexcept :
		m_id(vertex_array.m_id), m_elements_count(vertex_array.m_elements_count)
	{
		vertex_array.m_id = 0;
		vertex_array.m_elements_count = 0;
	}

	// We have to BIND data from buffers with our shaders
	// basicly tell gpu how to manage data 
	// for that we are using VERTEX ARRAY OBJECT
	void SimpleEngine::VertexArray::add_buffer(const VertexBuffer& vertex_buffer)
	{
		// again have to make our vbo for points first active since currently for colors is active
		bind();
		vertex_buffer.bind(); // vbo 

		for (const BufferElement& current_el : vertex_buffer.get_layout().get_elements()) {
			// first we have to TURN ON position (location)
			glEnableVertexAttribArray(m_elements_count);
			// finally link data enabled vbo with our enabled loation
			// args: location, amount of data, type, norm, stride, shift
			glVertexAttribPointer(
				m_elements_count, // location
				static_cast<GLint>(current_el.components_count), // number of components in Float or FLOAT2
				current_el.component_type, // Float or INT or ...
				GL_FALSE, // do we have to normalize? 
				static_cast<GLsizei>(vertex_buffer.get_layout().get_stride()), // stride 
				reinterpret_cast<const void*>(current_el.offset) // shift 
			);
			++m_elements_count;
		}

	}

	void SimpleEngine::VertexArray::bind() const
	{
		glBindVertexArray(m_id);
	}

	void SimpleEngine::VertexArray::unbind()
	{
		glBindVertexArray(0);
	}

}