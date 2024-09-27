#pragma once 

#include <vector>

namespace SimpleEngine {

	enum class ShaderDataType {
		Float,
		Float2,
		Float3,
		Float4,
		Int,
		Int2,
		Int3,
		Int4
	};

	struct BufferElement {
		ShaderDataType type;
		uint32_t component_type; // for opengl 
		size_t components_count; // Float2 -> 2 components 
		size_t size; // size in bytes 
		size_t offset;
		BufferElement(const ShaderDataType type);
	};

	class BufferLayout {
	public:
		BufferLayout(std::initializer_list<BufferElement> elements) :
			m_elements(std::move(elements)) {

			size_t offset = 0;
			m_stride = 0;
			for (auto& e : m_elements) {
				e.offset = offset;
				offset += e.size;
				m_stride += e.size;
			}
		}
		const std::vector<BufferElement>& get_elements() const { return m_elements; }
		size_t get_stride() const { return m_stride; }
	private:
		std::vector<BufferElement> m_elements;
		size_t m_stride = 0;
	};

	class VertexBuffer {
	public:
		enum class EUsage {
			Static,
			Dynamic,
			Stream
		};

		VertexBuffer(const void* data, const size_t size, BufferLayout buffer_layout, const EUsage usage = VertexBuffer::EUsage::Static);
		~VertexBuffer();

		VertexBuffer(const VertexBuffer&) = delete;
		VertexBuffer& operator=(const VertexBuffer&) = delete;

		VertexBuffer(VertexBuffer&& vertexBuffer) noexcept;
		VertexBuffer& operator= (VertexBuffer&& vertexBuffer) noexcept;

		void bind() const;
		static void unbind();

		const BufferLayout& get_layout() const { return m_buffer_layout; }

	private:
		unsigned int m_id = 0;
		BufferLayout m_buffer_layout;
	};
}